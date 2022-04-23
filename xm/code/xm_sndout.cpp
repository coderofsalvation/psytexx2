/*
    PsyTexx: xm_sndout.cpp. Multiplatform functions for the sound output
    Copyright (C) 2002 - 2005  Zolotov Alexandr

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//*** Contact info: Zolotov Alexandr (NightRadio project)
//***               Ekaterinburg. Russia.
//***               Email: nightradio@gmail.com
//***                      observer_page@mail.ru
//***               WWW: warmplace.ru

#include "../xm.h"

#ifdef NONPALM
    #include "../xm_play.h"
#endif
#ifdef NATIVEARM
    #include "../xm_play.h"
#endif

sound_struct xm; //Main sound structure;

#ifdef LINUX
    #include <fcntl.h>
    #include <linux/soundcard.h>
    #include <pthread.h>
    #include <sys/ioctl.h>
    int BUFLEN = 1024;
    int dsp;
    pthread_t pth;
    void *sound_thread (void *arg)
    {
	if( get_option( OPT_SOUNDBUFFER ) != -1 ) BUFLEN = get_option( OPT_SOUNDBUFFER );
	char buf[ BUFLEN * 4 ];
        long len = BUFLEN;
        for(;;) {
	    main_callback( (sound_struct *)arg, 0, buf, len );
	    if( dsp >= 0 ) write( dsp, buf, len * 4 ); else break;
	}
	pthread_exit(0);
	return 0;
    }
#endif

#ifndef NONPALM
    #include "PalmOS.h"
    SndStreamRef main_stream = 0;
    MemHandle ARM_code_handler;
    char *ARM_code;
    char paused = 0;
#endif

#ifdef WIN32
    #include "dsound.h"
    #define NUMEVENTS 2
    LPDIRECTSOUND               lpds;
    DSBUFFERDESC                dsbdesc;
    LPDIRECTSOUNDBUFFER         lpdsb = 0;
    LPDIRECTSOUNDBUFFER         lpdsbPrimary;
    LPDIRECTSOUNDNOTIFY         lpdsNotify;
    WAVEFORMATEX                *pwfx;
    HMMIO                       hmmio;
    MMCKINFO                    mmckinfoData, mmckinfoParent;
    DSBPOSITIONNOTIFY           rgdsbpn[NUMEVENTS];
    HANDLE                      rghEvent[NUMEVENTS];

    //Sound thread:
    HANDLE sound_thread;
    SECURITY_ATTRIBUTES atr;
    bool StreamToBuffer( ulong dwPos )
    {
	LONG            lNumToWrite;
	DWORD           dwStartOfs;
	VOID            *lpvPtr1, *lpvPtr2;
	DWORD           dwBytes1, dwBytes2;
	static DWORD    dwStopNextTime = 0xFFFF;
 
	if (dwStopNextTime == dwPos)   // All data has been played
	{
		lpdsb->Stop();
		dwStopNextTime = 0xFFFF;
		return TRUE;
	}

	if (dwStopNextTime != 0xFFFF)  // No more to stream, but keep
		                           // playing to end of data
		return TRUE;

	if (dwPos == 0)
		dwStartOfs = rgdsbpn[NUMEVENTS - 1].dwOffset;
	else
		dwStartOfs = rgdsbpn[dwPos-1].dwOffset;

	lNumToWrite = (LONG) rgdsbpn[dwPos].dwOffset - dwStartOfs;
	if (lNumToWrite < 0) lNumToWrite += dsbdesc.dwBufferBytes;

	IDirectSoundBuffer_Lock(lpdsb,
                 dwStartOfs,       // Offset of lock start
                 lNumToWrite,      // Number of bytes to lock
                 &lpvPtr1,         // Address of lock start
                 &dwBytes1,        // Count of bytes locked
                 &lpvPtr2,         // Address of wrap around
                 &dwBytes2,        // Count of wrap around bytes
                 0);               // Flags

	//Write data to the locked buffer:
	main_callback( &xm, 0, lpvPtr1, dwBytes1 >> 2 );

	IDirectSoundBuffer_Unlock(lpdsb, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

	return TRUE;
    }

    unsigned long __stdcall sound_callback( void *par )
    {
	while( 1 )
	{
	    DWORD dwEvt = MsgWaitForMultipleObjects(
				NUMEVENTS,      // How many possible events
				rghEvent,       // Location of handles
				FALSE,          // Wait for all?
				INFINITE,       // How long to wait
				QS_ALLINPUT);   // Any message is an event
 
	    dwEvt -= WAIT_OBJECT_0;
 
	    // If the event was set by the buffer, there's input
	    // to process. 
 
	    if (dwEvt < NUMEVENTS) 
	    {
		if( lpdsb )	StreamToBuffer(dwEvt); // copy data to output stream
	    }
	}
	return 0;
    }
#endif

int temp_variable = 0;
void temp_function(void)
{
    temp_variable++;
    if( temp_variable == 65000 ) prints( "temp function is working" );
}

void sound_stream_init(void)
{
#ifdef LINUX
    int temp;
#endif
#ifndef NONPALM
    ulong processor; //Processor type
#endif

    mem_off(); //Storage memory protection OFF (for PalmOS devices)
    tables_init();            //XM tables init
    clear_struct( &xm );      //Clear main sound structure
    new_channels( MAX_REAL_CHANNELS, &xm );  //Create channels
    new_song( &xm );          //Create new song
    create_silent_song( &xm );//Create silent song (1 empty pattern)
    mem_on(); //Storage memory protection ON (for PalmOS devices)

    int samples = 0;
#ifndef NONPALM
    samples = 2048;
#else
    samples = 128000;
#endif
    if( samples )
	xm.buffer = (long*)mem_new( HEAP_DYNAMIC, samples * 2 * sizeof( long ), "32buffer", 0 );

#ifdef LINUX
    //Start PsyTexx first time:
    dsp = open ( "/dev/dsp", O_WRONLY, 0 );
    //dsp = -1;
    if (dsp == -1)
    {
        prints( "Can't open sound device" );
        return;
    }
    temp = 1;
    ioctl (dsp, SNDCTL_DSP_STEREO, &temp);
    temp = 16;
    ioctl (dsp, SNDCTL_DSP_SAMPLESIZE, &temp);
    temp = 44100;
    ioctl (dsp, SNDCTL_DSP_SPEED, &temp);
    temp = 16 << 16 | 8;
    ioctl (dsp, SNDCTL_DSP_SETFRAGMENT, &temp);
    ioctl (dsp, SNDCTL_DSP_GETBLKSIZE, &temp);
    
    //Create sound thread:
    if( pthread_create ( &pth, NULL, sound_thread, &xm ) != 0 )
    {
        printf ("Can't create sound thread!\n");
        return;
    }
#endif

#ifndef NONPALM
#ifndef NATIVEARM
    ARM_code_handler = DmGetResource( 'armc', 1000 );
    ARM_code = (char*)MemHandleLock( ARM_code_handler );
#else
    ARM_code = (char*)main_callback;
#endif //NATIVEARM
    FtrGet( sysFileCSystem, sysFtrNumProcessorID, &processor );
    if( sysFtrNumProcessorIsARM( processor ) )
    SndStreamCreate( &main_stream,
                     sndOutput,
		     44100,
		     sndInt16Little,
		     sndStereo,
		     (SndStreamBufferCallback) ARM_code,
		     &xm,
		     4096,
		     1 );
    else main_stream = 0;
#endif

#ifdef WIN32
	HWND hWnd = GetForegroundWindow();
	if( hWnd == NULL )
	{
		hWnd = GetDesktopWindow();
	}
	if FAILED( DirectSoundCreate( 0, &lpds, NULL ) )
	{
		MessageBox( hWnd,"DSound: DirectSoundCreate error","PsyTexx Error",MB_OK);
		return;
	}
	if FAILED( IDirectSound_SetCooperativeLevel(
               lpds, hWnd, DSSCL_PRIORITY ) )
	{
		MessageBox( hWnd,"DSound: SetCooperativeLevel error","PsyTexx Error",MB_OK);
        return;
	}

	// Obtain primary buffer

    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if FAILED( lpds->CreateSoundBuffer(&dsbdesc, &lpdsbPrimary, NULL) )
	{
		MessageBox( hWnd,"DSound: CreateSoundBuffer error","PsyTexx Error",MB_OK);
		return;
	}

    // Set primary buffer format

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
    wfx.wFormatTag = WAVE_FORMAT_PCM; 
    wfx.nChannels = 2; 
    wfx.nSamplesPerSec = 44100; 
    wfx.wBitsPerSample = 16; 
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    lpdsbPrimary->SetFormat(&wfx);

	pwfx = &wfx;

	// Secondary buffer:
	
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = 
            DSBCAPS_GETCURRENTPOSITION2   // Always a good idea
            | DSBCAPS_GLOBALFOCUS         // Allows background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY; // Needed for notification
 
    // The size of the buffer is arbitrary, but should be at least
    // two seconds, to keep data writes well ahead of the play
    // position.
 
    if( get_option( OPT_SOUNDBUFFER ) != -1 )
	dsbdesc.dwBufferBytes = get_option( OPT_SOUNDBUFFER ) * 4;
    else
	dsbdesc.dwBufferBytes = 6000 * 4;
    dsbdesc.lpwfxFormat = pwfx;

    if FAILED( IDirectSound_CreateSoundBuffer(
              lpds, &dsbdesc, &lpdsb, NULL) )
    {
		MessageBox( hWnd,"DSound: Create secondary buffer error","PsyTexx Error",MB_OK);
		return;
	}

	//Create buffer events:
	
	for (int i = 0; i < NUMEVENTS; i++)
    {
        rghEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == rghEvent[i]) 
		{
			MessageBox( hWnd,"DSound: Create event error","PsyTexx Error",MB_OK);
			return;
		}
    }
	rgdsbpn[0].dwOffset = 0;
    rgdsbpn[0].hEventNotify = rghEvent[0];
    rgdsbpn[1].dwOffset = (dsbdesc.dwBufferBytes/2);
    rgdsbpn[1].hEventNotify = rghEvent[1];
	
	if FAILED( lpdsb->QueryInterface( IID_IDirectSoundNotify, (VOID **)&lpdsNotify) )
	{
		MessageBox( hWnd,"DSound: QueryInterface error","PsyTexx Error",MB_OK);
		return;
	}
 
    if FAILED(IDirectSoundNotify_SetNotificationPositions(
             lpdsNotify, NUMEVENTS, rgdsbpn))
    {
        IDirectSoundNotify_Release(lpdsNotify);
		MessageBox( hWnd,"DSound: SetNotificationPositions error","PsyTexx Error",MB_OK);
		return;
    }

	IDirectSoundBuffer_Play( lpdsb, 0, 0, DSBPLAY_LOOPING );

	//Create main thread:
	atr.nLength = sizeof(atr);
	atr.lpSecurityDescriptor = 0;
	atr.bInheritHandle = 0;
	sound_thread = CreateThread( &atr, 4096, &sound_callback, &xm, 0, 0 );
#endif
}

void sound_stream_play(void)
{
#ifdef LINUX
    if( dsp >= 0 )
    {
	xm.need_to_stop = 0;
    }
#endif

#ifndef NONPALM
    if( main_stream )
    {
	xm.need_to_stop = 0;
	if( paused ) 
	{
	    SndStreamPause( main_stream, 0 );
	    paused = 0;
	}
	else SndStreamStart( main_stream );
    }
#endif

#ifdef WIN32
    if( lpdsb )
    {
	xm.need_to_stop = 0;
    }
#endif
}

void sound_stream_stop(void)
{
#ifdef LINUX
    if( dsp >= 0 )
    {
	xm.stream_stoped = 0;
	xm.need_to_stop = 1;
	while( xm.stream_stoped == 0 ) 
	{
	    temp_function(); 
	} //Waiting for stoping
    }
#endif

#ifndef NONPALM
    if( main_stream )
    {
	xm.stream_stoped = 0;
	xm.need_to_stop = 1;
	while( xm.stream_stoped == 0 ) { temp_function(); } //Waiting for stoping
	SndStreamPause( main_stream, 1 );
	paused = 1;
    }
#endif

#ifdef WIN32
    if( lpdsb )
    {
	xm.stream_stoped = 0;
	xm.need_to_stop = 1;
	while( xm.stream_stoped == 0 ) { temp_function(); } //Waiting for stoping
    }
#endif
}

void sound_stream_close(void)
{
    sound_stream_stop();

#ifdef LINUX
    int our_dsp = dsp;
    dsp = -1;
    if( our_dsp >= 0 ) close( our_dsp );
#endif

#ifndef NONPALM
    if( main_stream )
    {
	SndStreamDelete( main_stream );
    }
#ifndef NATIVEARM
    MemHandleUnlock( ARM_code_handler );
#endif //NATIVEARM
#endif

#ifdef WIN32
    CloseHandle( sound_thread );
    if( lpdsb )
    {
        if (lpdsNotify)
	    lpdsNotify->Release();
	if (lpds)
	    lpds->Release();
    }
#endif

    mem_off(); //Storage memory protection OFF (for PalmOS devices)
    clear_song( &xm );
    clear_channels( &xm );
    close_song( &xm );
    mem_on(); //Storage memory protection ON (for PalmOS devices)

    if( xm.buffer ) 
    {
	mem_free( xm.buffer );
	xm.buffer = 0;
    }
}

void sound_set_volume( long volume )
{
    xm.global_volume = volume;
    #ifndef NONPALM
    #ifdef SLOWMODE
	volume <<= 3;
	if( volume > 1023 ) volume = 1023;
	SndStreamSetVolume( main_stream, volume );
    #endif
    #endif
}
