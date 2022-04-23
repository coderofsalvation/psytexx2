/*
    PsyTexx: psytexx.cpp. PsyTexx main()
    Copyright (C) 2002 - 2007  Zolotov Alexandr

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
//***               WWW: warmplace.ru

#ifdef NONPALM
    #include <stdio.h>
#endif

#include "../win_main.h"

//################################
//## DEVICE VARIABLES:          ##
//################################

//PALMOS
#ifndef NONPALM
    #include <PalmOS.h>
#ifndef NATIVEARM
    SysAppInfoPtr SysGetAppInfo(SysAppInfoPtr *rootAppPP,
	                        SysAppInfoPtr *actionCodeAppPP)
    	                        SYS_TRAP(sysTrapSysUIBusy);
#else
    #define arm_startup __attribute__ ((section ("arm_startup")))
    #include "palm_functions.h"
#endif //NATIVEARM
#endif

//################################
//################################
//################################

//################################
//## APPLICATION MAIN:          ##
//################################

//********************************
//WIN32 MAIN *********************
#ifdef WIN
int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
    {
	wm.hCurrentInst = hCurrentInst;
	wm.hPreviousInst = hPreviousInst; 
	wm.lpszCmdLine = lpszCmdLine;
	wm.nCmdShow = nCmdShow;
#endif
//********************************
//********************************

//********************************
//PALMOS MAIN ********************
#ifndef NONPALM
#ifndef NATIVEARM
UInt32 PilotMain(UInt16 launchCode, void *cmdPBP, UInt16 launchFlags)
{
    if( launchCode == sysAppLaunchCmdSyncNotify )
    {
	VFSRegisterDefaultDirectory( ".XM", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".xm", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".MOD", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".mod", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".S3M", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".s3m", expMediaType_Any, "/MOD/" );
	VFSRegisterDefaultDirectory( ".xi", expMediaType_Any, "/INSTRUMENTS/" );
	VFSRegisterDefaultDirectory( ".XI", expMediaType_Any, "/INSTRUMENTS/" );
    }
    if( launchCode == sysAppLaunchCmdNormalLaunch )
    {
	appInfo = SysGetAppInfo( &ai1, &ai2 );
	ownID = appInfo->memOwnerID;
#else
long ARM_PalmOS_main( const void *emulStateP, void *userData, Call68KFuncType *call68KFuncP ) arm_startup;
long ARM_PalmOS_main( const void *emulStateP, void *userData, Call68KFuncType *call68KFuncP )
{
    {
	void *oldGOT;
	register void *gGOT asm ("r10");
	ARM_INFO *arm_info = (ARM_INFO *)userData;
	oldGOT = (void*)gGOT;
	gGOT = (void *)arm_info->GOT;
	ownID = (unsigned short)arm_info->ID;
	form_handler = arm_info->FORMHANDLER; //form_handler defined in palm_functions.cpp
	CALL_INIT
	prints( "MAIN: ARM started" );
#endif //NATIVEARM
	int autooff_time = SysSetAutoOffTime( 0 );
#endif //NONPALM
//********************************
//********************************

//********************************
//LINUX MAIN *********************
#ifdef LINUX
int main(int argc, char *argv[])
{
    {
    wm.argc = argc;
    wm.argv = argv;
#ifdef TEXTMODE
	char* str;
	if( argc > 1 )
	{
	    str = argv[ 1 ];
	    if( str[ 0 ] == '-' || str[ 0 ] == '?' )
	    {
		printf( "\033[%dm\033[%dm", 37, 40 );
		printf( "PsyTexx 2 for Linux.\n" );
		printf( "\033[%dm\033[%dm", 36, 40 );
		printf( "Command line options:\n" );
		printf( "\033[%dm\033[%dm", 35, 40 );
		printf( "m - auto mouse key up (for some GPM versions)\n" );
		printf( "\033[%dm\033[%dm", 37, 40 );
		return 0;
	    }
	    if( str[ 0 ] == 'm' ) mouse_auto_keyup = 1;
	}
#endif
#endif
//********************************
//********************************

	debug_reset();

	prints( "" );
	prints( "" );
	prints( PSYTEXX_VERSION );
	prints( PSYTEXX_DATE );
	prints( "" );
	prints( "STARTING..." );
	prints( "" );
	prints( "" );

	psy_windows_init();
	psy_event_loop();
        psy_windows_close();

	debug_close();
	mem_free_all();       //Close all memory blocks

	prints( "" );
	prints( "" );
	prints( "BYE !" );
	prints( "" );
	prints( "" );

#ifndef NONPALM
	SysSetAutoOffTime( autooff_time );
#endif
#ifdef NATIVEARM
	gGOT = (void*)oldGOT;
#endif
    }

    return 0;
}

//################################
//################################
//################################
