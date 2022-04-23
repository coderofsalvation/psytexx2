/*
    PsyTexx: wm_linuxgl.h. Platform-dependent module : Linux OpenGL + XWindows
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

#ifndef __WINMANAGER_LINUX_GL__
#define __WINMANAGER_LINUX_GL__

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>   //for getenv();
#include <sys/time.h> //timeval struct

void *event_thread( void *arg );

window_manager *current_wm;

int winWidth = 800, winHeight = 600;
int startWidth, startHeight;
XSetWindowAttributes swa;
Colormap        cmap;
XEvent          event;
int             dummy;
int		xscreen;
int		depth = 0;
int 		auto_repeat = 0;

#ifdef OPENGL
    #include "wm_opengl.h"
    GLXContext      cx;
    static int      snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
    static int      dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
    XVisualInfo    *vi;
#else
    Visual *vi;
#endif

void small_pause( long milliseconds )
{
    //XFlush( current_wm->dpy );
#ifndef DEMOENGINE 
    timeval t;
    t.tv_sec = 0;
    t.tv_usec = (long) (milliseconds % 1000) * 1000;
    select( 0 + 1, 0, 0, 0, &t );
#endif //Demoengine get 100% of the CPU speed
}

void device_start( window_manager *wm )
{
    current_wm = wm;
#ifdef OPENGL
    pscreen_x_size = startWidth = winWidth = 512;
    pscreen_y_size = startHeight = winHeight = 512;
#endif
#ifdef X11
    pscreen_x_size = startWidth = winWidth = 640;
    pscreen_y_size = startHeight = winHeight = 480;
    if( get_option( OPT_SCREENX ) != -1 ) pscreen_x_size = startWidth = winWidth = get_option( OPT_SCREENX );
    if( get_option( OPT_SCREENY ) != -1 ) pscreen_y_size = startHeight = winHeight = get_option( OPT_SCREENY );
#endif
    
    /*** open a connection to the X server ***/
    char *name;
    if( (name = getenv( "DISPLAY" )) == NULL )
	name = ":0";
    wm->dpy = XOpenDisplay( name );
    if( wm->dpy == NULL ) 
    {
    	prints( "could not open display\n" );
    }
#ifndef OPENGL
    //Simple X11 init (not GLX) :
    xscreen = XDefaultScreen( wm->dpy );
    vi = XDefaultVisual( wm->dpy, xscreen ); wm->win_visual = vi; if( !vi ) prints( "XDefaultVisual error" );
    depth = XDefaultDepth( wm->dpy, xscreen ); wm->win_depth = depth; if( !depth ) prints( "XDefaultDepth error" );
    cmap = XDefaultColormap( wm->dpy, xscreen );
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask 
                     | FocusChangeMask | StructureNotifyMask;
    wm->win = XCreateWindow( wm->dpy, XDefaultRootWindow(wm->dpy), 0, 0, 
                             winWidth, winHeight, 0, CopyFromParent,
			     InputOutput, vi, CWBorderPixel | CWColormap | CWEventMask, 
			     &swa );
    XStoreName( wm->dpy, wm->win, "PsyTexx" );

    wm->win_gc = XDefaultGC( wm->dpy, xscreen ); if( !wm->win_gc ) prints( "XDefaultGC error" );
    wm->win_img = XCreateImage( wm->dpy, wm->win_visual, depth, ZPixmap, 
                                0, 0, pscreen_x_size, pscreen_y_size, 8, 0 );
    wm->win_buffer = wm->win_img->data = (char*)malloc( wm->win_img->bytes_per_line * pscreen_y_size + 16 );
    wm->win_img_depth = wm->win_img->bytes_per_line / pscreen_x_size;
    if( !wm->win_img ) prints( "XCreateImage error" );
#endif //!OPENGL

#ifdef OPENGL
    /********************************/
    /*** Initialize GLX routines  ***/
    /********************************/

    /*** make sure OpenGL's GLX extension supported ***/
    if( !glXQueryExtension( wm->dpy, &dummy, &dummy ) ) 
    {
    	prints( "X server has no OpenGL GLX extension" );
    }
    else prints( "MAIN: OpenGL GLX extension found" );

    /*** find an appropriate visual ***/
    /* find an OpenGL-capable RGB visual with depth buffer */
    vi = glXChooseVisual( wm->dpy, DefaultScreen(wm->dpy), dblBuf );
    wm->doubleBuffer = GL_TRUE;
    if( vi == NULL ) 
    {
    	vi = glXChooseVisual( wm->dpy, DefaultScreen(wm->dpy), snglBuf );
	if( vi == NULL ) 
	{
	    prints( "no RGB visual with depth buffer" );
	}
	wm->doubleBuffer = GL_FALSE;
    }

    /*** create an OpenGL rendering context  ***/
    /* create an OpenGL rendering context */
    cx = glXCreateContext( wm->dpy, vi, /* no sharing of display lists */ None,
			   /* direct rendering if possible */ GL_TRUE );
    if( cx == NULL ) 
    {
	prints( "could not create rendering context" );
    }

    /*** create an X window with the selected visual ***/
    /* create an X colormap since probably not using default visual */
    cmap = XCreateColormap( wm->dpy, RootWindow(wm->dpy, vi->screen), vi->visual, AllocNone );
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask 
                     | KeyPressMask | KeyReleaseMask | FocusChangeMask | StructureNotifyMask;
    wm->win = XCreateWindow( wm->dpy, RootWindow(wm->dpy, vi->screen), 0, 0, 
                         winWidth, winHeight, 0, vi->depth,
			 InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, 
			 &swa );
    XSetStandardProperties( wm->dpy, wm->win, "PsyTexx (OpenGL)", "PsyTexx (OpenGL)", None, wm->argv, wm->argc, NULL );
#endif //OPENGL

    /*** request the X window to be displayed on the screen ***/
    XMapWindow( wm->dpy, wm->win );
    
#ifdef OPENGL
    /*** bind the rendering context to the window ***/
    glXMakeCurrent( wm->dpy, wm->win, cx );
    gl_init();
    gl_resize();
#endif

    XkbSetDetectableAutoRepeat( wm->dpy, 1, &auto_repeat );
    
    //XGrabKeyboard( wm->dpy, wm->win, 1, GrabModeAsync, GrabModeAsync, CurrentTime );
    //XGrabKey( wm->dpy, 0, ControlMask, wm->win, 1, 1, GrabModeSync );
}

void device_end( window_manager *wm )
{
    int temp;
    XkbSetDetectableAutoRepeat( wm->dpy, auto_repeat, &temp );
#ifndef OPENGL
    XDestroyImage( wm->win_img );
#endif
    XDestroyWindow( wm->dpy, wm->win );
    XCloseDisplay( wm->dpy );
}

#ifdef OPENGL
#define GET_WINDOW_COORDINATES \
	/*Real coordinates -> window_manager coordinates*/\
	x = winX;\
	y = winY;\
	x = ( x << 11 ) / winWidth; x *= current_wm->pscreen_x; x >>= 11;\
	y = ( y << 11 ) / winHeight; y *= current_wm->pscreen_y; y >>= 11;
#else
#define GET_WINDOW_COORDINATES \
	/*Real coordinates -> window_manager coordinates*/\
	x = winX;\
	y = winY;
#endif

long device_event_handler( window_manager *wm )
{
    //if( wm->events_count == 0 ) XPeekEvent( wm->dpy, &event ); //Waiting for the next event
    int old_screen_x, old_screen_y;
    int old_char_x, old_char_y;
    int winX, winY;
    int x, y, window_num;
    window *win;
    int button = 0;
    ulong resulted_key = 0;
    ulong mod_state = 0;
    KeySym sym;
    XSizeHints size_hints;
    if( XPending( wm->dpy ) )
    {
	XNextEvent( wm->dpy, &event );
        switch( event.type ) 
	{
	    case FocusIn:
		//All keyboard event must sending to the PsyTexx window only:
		XGrabKeyboard( wm->dpy, wm->win, 1, GrabModeAsync, GrabModeAsync, CurrentTime ); 
		break;

	    case FocusOut:
		XUngrabKeyboard( wm->dpy, CurrentTime );
		break;
		
	    case MapNotify:
	    case Expose:
		#ifndef OPENGL
		redraw_screen( wm );
		#endif
		break;
	
	    case DestroyNotify:
		wm->exit_flag = 1;
		break;
		
	    case ConfigureNotify:
		#ifndef OPENGL
		size_hints.flags = PResizeInc | PMinSize;
		size_hints.width_inc = wm->char_x;
		size_hints.height_inc = wm->char_y;
		size_hints.min_width = startWidth;
		size_hints.min_height = startHeight;
		XSetNormalHints( wm->dpy, wm->win, &size_hints );
		#endif
		winWidth = event.xconfigure.width;
	        winHeight = event.xconfigure.height;
		#ifdef OPENGL
		gl_resize();
		#else
		old_screen_x = wm->screen_x;
		old_screen_y = wm->screen_y;
		wm->screen_x = winWidth / wm->char_x;
		wm->screen_y = winHeight / wm->char_y;
		wm->pscreen_x = wm->screen_x * wm->char_x;
		wm->pscreen_y = wm->screen_y * wm->char_y;
		if( old_screen_x != wm->screen_x || old_screen_y != wm->screen_y )
		{
		    resize_all( 1, wm ); //Resize window manager
		    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
	        }
		#endif
		break;
		
	    case MotionNotify:
		if( event.xmotion.window != wm->win ) break;
		winX = event.xmotion.x;
		winY = event.xmotion.y;
		GET_WINDOW_COORDINATES;
		if( event.xmotion.state & Button1Mask )
		    button |= BUTTON_LEFT;
		if( event.xmotion.state & Button2Mask )
		    button |= BUTTON_MIDDLE;
		if( event.xmotion.state & Button3Mask )
		    button |= BUTTON_RIGHT;
		push_button( x, y, button, 1023, 2, current_wm );
		break;
		
	    case ButtonPress:
	    case ButtonRelease:
		if( event.xmotion.window != wm->win ) break;
		winX = event.xbutton.x;
		winY = event.xbutton.y;
		GET_WINDOW_COORDINATES;
		/* Get pressed button */
	    	if( event.xbutton.button == 1 )
		    button = BUTTON_LEFT;
		else if( event.xbutton.button == 2 )
		    button = BUTTON_MIDDLE;
		else if( event.xbutton.button == 3 )
		    button = BUTTON_RIGHT;
		else if( ( event.xbutton.button == 4 || event.xbutton.button == 5 ) && event.type == ButtonPress )
		{
		    if( event.xbutton.button == 4 ) 
			resulted_key = KEY_SCROLLUP << 3;
		    else
			resulted_key = KEY_SCROLLDOWN << 3;
		    if( x < 0 ) x = 0;
		    if( y < 0 ) y = 0;
		    if( x < current_wm->pscreen_x && y < current_wm->pscreen_y )
		    {
			window_num = current_wm->t_screen[ ((y/current_wm->char_y) * current_wm->screen_x) + (x/current_wm->char_x) ];
			win = current_wm->windows[ window_num ];
			send_event( window_num, EVT_BUTTONDOWN, x - win->real_x, y - win->real_y,
			    resulted_key, 1023, MODE_WINDOW, current_wm );					
		    }				    
		}
		/* Get any other buttons that might be already held */
		if( event.xbutton.state & Button1Mask )
		    button |= BUTTON_LEFT;
		if( event.xbutton.state & Button2Mask )
		    button |= BUTTON_MIDDLE;
		if( event.xbutton.state & Button3Mask )
		    button |= BUTTON_RIGHT;
		if( event.type == ButtonPress )
		    push_button( x, y, button, 1023, 0, current_wm );
		else
		    push_button( x, y, button, 1023, 1, current_wm );
		break;
		
	    case KeyPress:
	    case KeyRelease:
		if( event.xkey.state & ControlMask ) mod_state |= KEY_CTRL;
		if( event.xkey.state & ShiftMask ) mod_state |= KEY_SHIFT;
		if( event.xkey.state & Mod1Mask ) mod_state |= KEY_ALT;
		sym = XKeycodeToKeysym( wm->dpy, event.xkey.keycode, 0 );
		if( sym == NoSymbol || sym == 0 ) break;
		if( sym <= 0xFF ) resulted_key = sym;
		switch( sym )
		{
		    case XK_F1: resulted_key = KEY_F1; break;
		    case XK_F2: resulted_key = KEY_F2; break;
		    case XK_F3: resulted_key = KEY_F3; break;
		    case XK_F4: resulted_key = KEY_F4; break;
		    case XK_F5: resulted_key = KEY_F5; break;
		    case XK_F6: resulted_key = KEY_F6; break;
		    case XK_F7: resulted_key = KEY_F7; break;
		    case XK_F8: resulted_key = KEY_F8; break;
		    case XK_BackSpace: resulted_key = KEY_BACKSPACE; break;
		    case XK_Tab: resulted_key = KEY_TAB; break;
		    case XK_Return: resulted_key = KEY_ENTER; break;
		    case XK_Escape: resulted_key = KEY_ESCAPE; break;
		    case XK_Left: resulted_key = KEY_LEFT; break;
		    case XK_Right: resulted_key = KEY_RIGHT; break;
		    case XK_Up: resulted_key = KEY_UP; break;
		    case XK_Down: resulted_key = KEY_DOWN; break;
		    case XK_Home: resulted_key = KEY_HOME; break;
		    case XK_End: resulted_key = KEY_END; break;
		    case XK_Page_Up: resulted_key = KEY_PAGEUP; break;
		    case XK_Page_Down: resulted_key = KEY_PAGEDOWN; break;
		    case XK_Delete: resulted_key = KEY_DELETE; break;
		    case XK_Insert: resulted_key = KEY_INSERT; break;
		    case XK_Caps_Lock: resulted_key = KEY_CAPS; break;		    
		}
		if( event.type == KeyPress )
		    push_button( 0, 0, (resulted_key | mod_state) << 3, 1023, 0, current_wm );
		else
		    push_button( 0, 0, (resulted_key | mod_state) << 3, 1023, 1, current_wm );
		break;
	}
    } 
    else
    {
	//There are no X11 events
	if( wm->events_count == 0 ) small_pause( 1 ); //And no WM events
    }
    if( wm->exit_flag ) return 1;
    return 0;
}

//#################################
//#################################
//#################################

#endif
