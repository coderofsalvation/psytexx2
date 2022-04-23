/*
    PsyTexx: wm_palmos.h. Platform-dependent module : PalmOS
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
//***                      observer_page@mail.ru
//***               WWW: warmplace.ru

#ifndef __WINMANAGER_PALMOS__
#define __WINMANAGER_PALMOS__

#include <PalmOS.h>
#include <PenInputMgr.h>
#ifdef NATIVEARM
    #include "palm_functions.h"
#endif

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

void wait( void )
{
    int cur_ticks = time_ticks();
    int counter = 0;
    while( time_ticks() < cur_ticks + ( time_ticks_per_second() / 2 ) )
    {
	device_event_handler( 0 );
	counter++;
	if( ( counter & 0xFFFF ) == 0 ) prints( "MAIN: waiting..." );
    }
}

FormPtr gpForm;
int formIsOpen = 0;
int formResized = 0;

void MainFormResize( FormPtr frmP )
{
    RectangleType dispBounds, bounds;
    
    // Get/Set the window bounds
    WinGetBounds( WinGetDisplayWindow(), &dispBounds );
    WinSetWindowBounds( FrmGetWindowHandle(frmP), &dispBounds );
}

Boolean FormHandler( EventPtr event )
{
    RectangleType bounds;
    UInt32 pinMgrVersion;
    Err err;
    Boolean handled = false;

//In NATIVEARM mode form handler is in the arm_starter.cpp
#ifndef NATIVEARM
    switch( event->eType )
    {
	case frmOpenEvent:
	    gpForm = FrmGetActiveForm();

	    err = FtrGet( pinCreator, pinFtrAPIVersion, &pinMgrVersion );
	    if ( !err && pinMgrVersion != pinAPIVersion1_0 )
	    {
		SysSetOrientation( sysOrientationLandscape );
		FrmSetDIAPolicyAttr( gpForm, frmDIAPolicyCustom );
		PINSetInputTriggerState( pinInputTriggerEnabled );
		PINSetInputAreaState( pinInputAreaHide ); //user state
		StatHide();
	    }
	    else
	    { //No pinAPI: (TungstenT for example)
		formResized = 1;
	    }
	
	    // Resize the form
	    MainFormResize( gpForm );

	    FrmDrawForm(gpForm);

	    formIsOpen = 1;
	    handled = true;
	    break;
	
	case winDisplayChangedEvent:
	    WinGetBounds( WinGetDisplayWindow(), &bounds );
	    WinSetBounds( WinGetActiveWindow(), &bounds );
	    formResized = 1;
	    handled = true;
	    break;
	
	case frmCloseEvent:
	    FrmEraseForm(gpForm);
    	    FrmDeleteForm(gpForm);
	    gpForm = 0;
	    formIsOpen = 0;
	    handled = true;
	    break;
    }
#endif
    return handled;
}

Boolean ApplicationHandleEvent( EventPtr event )
{
    FormPtr frm;
    UInt16 formId;
    Boolean handled = false;
    FormPtr pForm;

#ifndef NATIVEARM
    switch( event->eType )
#else
    switch( BSwap16(event->eType) )
#endif
    {
	case frmLoadEvent:
	    #ifndef NATIVEARM
	    pForm = FrmInitForm( event->data.frmLoad.formID );
	    #else
	    unsigned short *ptr = (unsigned short*)event;
	    pForm = FrmInitForm( BSwap16(ptr[4]) );//BSwap16(event->data.frmLoad.formID) );
	    #endif
	    
	    FrmSetActiveForm( pForm );

	    #ifndef NATIVEARM
	    FrmSetEventHandler( pForm, FormHandler );
	    #else
	    FrmSetEventHandler( pForm, (Boolean (*)(EventType*))form_handler );
	    #endif

	    handled = true;
	    break;
    }
    return handled;
}

void device_start( window_manager *wm )
{
    //Open window:
#ifndef PALMOS_COMP_MODE
    prints( "MAIN: open form" );
    FrmGotoForm( 8888 );
    wait();
    /*
    UInt32 pinMgrVersion;
    prints( "MAIN: FtrGet" );
    Err err = FtrGet( pinCreator, pinFtrAPIVersion, &pinMgrVersion );
    prints( "MAIN: waiting for form open/resize" );
    if ( !err && pinMgrVersion != pinAPIVersion1_0 )
    { //Palm with large screen:
	while( !formResized ) device_event_handler( 0 );
    }
    else
    { //Palm like TungstenT (320x320 or 160x160):
	while( !formIsOpen ) device_event_handler( 0 );
    }
    RectangleType bounds;
    //prints( "MAIN: get form ptr" );
    FormType *fp;// = FrmGetFormPtr( 8888 );
    for(;;) 
    { 
        device_event_handler( 0 ); fp = FrmGetFormPtr( 8888 );
        if( fp )
        {
	    FrmGetFormBounds( fp, &bounds );
	    if( bounds.extent.x != 32 || bounds.extent.y != 32 ) break;
	}
    }
    prints2( "MAIN: fp = ", (long)fp );
    prints( "MAIN: get form bounds" );
    FrmGetFormBounds( fp, &bounds );
    */

    prints( "MAIN: get form size" );
#ifdef PALMLOWRES
    pscreen_x_size = 160;//bounds.extent.x;
    pscreen_y_size = 160;//bounds.extent.y;
#else
    pscreen_x_size = 320;//bounds.extent.x << 1;
    pscreen_y_size = 320;//bounds.extent.y << 1;
#endif

#else //PALMOS_COMP_MODE (No forms):

#ifdef PALMLOWRES
    pscreen_x_size = 160;
    pscreen_y_size = 160;
#else
    pscreen_x_size = 320;
    pscreen_y_size = 320;
#endif

#endif
    prints( "MAIN: device start finished" );
}

void device_end( window_manager *wm )
{
    //Close window:
#ifndef PALMOS_COMP_MODE
    prints( "MAIN: close all forms" );
    FrmCloseAllForms();
    wait();
#else
    prints( "MAIN: device end" );
#endif
}

long old_chrr;

long device_event_handler( window_manager *wm )
{
    EventType event;
    UInt16 error;
    long chrr,x,y;
    char keydown = 0;

    EvtGetEvent( &event, 0 );

    if( wm )
    {    
#ifndef NATIVEARM
    chrr = event.data.keyDown.chr;
    x = event.screenX;
    y = event.screenY;
    switch( event.eType ) 
    {
#else
    UInt16 *ptr = (UInt16*)&event;
    chrr = BSwap16( ptr[4] );
    x = BSwap16( ptr[2] );
    y = BSwap16( ptr[3] );
    switch( BSwap16(event.eType) ) 
    {
#endif
#ifdef PALMLOWRES
	case penDownEvent: push_button( x, y, 1, 1023, 0, wm ); break;
	case penMoveEvent: push_button( x, y, 1, 1023, 2, wm ); break;
	case penUpEvent:   push_button( x, y, 1, 1023, 1, wm ); break;
#else
	case penDownEvent: push_button( x<<1, y<<1, 1, 1023, 0, wm ); break;
	case penMoveEvent: push_button( x<<1, y<<1, 1, 1023, 2, wm ); break;
	case penUpEvent:   push_button( x<<1, y<<1, 1, 1023, 1, wm ); break;
#endif
	case keyDownEvent:
	    if( chrr >= 11 && chrr <= 12 ) keydown = 1;
	    if( chrr >= 516 && chrr <= 519 ) keydown = 1;
	    ulong resulted_key;
	    switch( chrr )
	    {
		case 0x1E: resulted_key = KEY_UP; break;
		case 0x1F: resulted_key = KEY_DOWN; break;
		case 0x1C: resulted_key = KEY_LEFT; break;
		case 0x1D: resulted_key = KEY_RIGHT; break;
		case 0x08: resulted_key = KEY_BACKSPACE; break;
		case 0x0A: resulted_key = KEY_ENTER; break;
		case 0x0B: resulted_key = KEY_UP; break;
		case 0x0C: resulted_key = KEY_DOWN; break;
		default: resulted_key = chrr; if( resulted_key > 255 ) resulted_key = 0; break;
	    }
	    push_button( 0, 0, resulted_key << 3, 1023, 0, wm );
	    break;
	/*case keyUpEvent:
	    chrr = event.data.keyDown.chr;
	    push_button( 0, 0, wm->buttons_table[ chrr ] << 3, 1023, 1, wm );
	    break;*/
    }
    
    chrr = KeyCurrentState();
    if( !keydown )
    {
	if( chrr & 0x1000000 && !( old_chrr & 0x1000000 ) ) { push_button( 0, 0, KEY_LEFT << 3, 1023, 0, wm ); keydown = 1; }
	if( !( chrr & 0x1000000 ) && old_chrr & 0x1000000 ) { push_button( 0, 0, KEY_LEFT << 3, 1023, 1, wm ); }
	if( chrr & 0x2000000 && !( old_chrr & 0x2000000 ) ) { push_button( 0, 0, KEY_RIGHT << 3, 1023, 0, wm ); keydown = 1; }
	if( !( chrr & 0x2000000 ) && old_chrr & 0x2000000 ) { push_button( 0, 0, KEY_RIGHT << 3, 1023, 1, wm ); }
	if( chrr & 0x4000000 && !( old_chrr & 0x4000000 ) ) { push_button( 0, 0, KEY_SPACE << 3, 1023, 0, wm ); keydown = 1; }
	if( !( chrr & 0x4000000 ) && old_chrr & 0x4000000 ) { push_button( 0, 0, KEY_SPACE << 3, 1023, 1, wm ); }
    }
    old_chrr = chrr;

    } //if( wm )

    if( !keydown )
    {
#ifndef PALMOS_COMP_MODE
	if( !SysHandleEvent(&event) && !MenuHandleEvent(0,&event,&error) && !ApplicationHandleEvent(&event) )
	    FrmDispatchEvent(&event);
#else
	if( !SysHandleEvent( &event ) ) MenuHandleEvent( 0, &event,&error );
#endif
    }

#ifndef NATIVEARM
    #ifndef PALMOS_COMP_MODE
    if( event.eType == winDisplayChangedEvent ) formResized = 1;
    if( event.eType == frmOpenEvent ) formIsOpen = 1; 
    if( event.eType == frmCloseEvent ) formIsOpen = 0;
    if( event.eType == winExitEvent ) formIsOpen = 0;
    #endif
    if( event.eType == appStopEvent ) 
#else
    #ifndef PALMOS_COMP_MODE
    if( BSwap16( event.eType ) == winDisplayChangedEvent ) formResized = 1;
    if( BSwap16( event.eType ) == frmOpenEvent ) formIsOpen = 1; 
    if( BSwap16( event.eType ) == frmCloseEvent ) formIsOpen = 0;
    if( BSwap16( event.eType ) == winExitEvent ) formIsOpen = 0;
    #endif
    if( BSwap16( event.eType ) == appStopEvent ) 
#endif
    {
	if( wm ) push_button( pscreen_x_size / 2, pscreen_y_size - 40, KEY_ESCAPE<<3, 1023, 0, wm );
    }

    if( wm ) if( wm->exit_flag ) return 1;

    return 0;
}

//#################################
//#################################
//#################################

#endif
