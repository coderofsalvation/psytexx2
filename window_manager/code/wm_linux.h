/*
    PsyTexx: wm_linux.h. Platform-dependent module : Linux console
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

#ifndef __WINMANAGER_LINUX__
#define __WINMANAGER_LINUX__

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

#include <termios.h>
//#include <linux/keyboard.h>
#include <linux/kd.h>
//#include <linux/vt.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h> //timeval struct

pthread_t pt;                   //our mouse thread
struct termio savetty, settty;  //for set_term() and reset_term()
extern int xmouse_flag;
int mouse_auto_keyup;
Gpm_Connect conn;

window_manager *current_wm;

#define KEYBOARD "/dev/tty"
int fd; //File descriptor for keyboard
//Original keyboard modes:
int old_kbd_mode = 0;
termios old;
pthread_t key_th;      //our keyboard thread

void get_terminal_size(void);
void set_term();
void reset_term();
int mouse_handler(Gpm_Event *event, void *data);
void *mouse_thread(void *arg);
void mouse_init(void);
void mouse_close(void);
int keyboard_init(void);
int keyboard_close(void);

void small_pause( long milliseconds )
{
    timeval t;
    t.tv_sec = 0;
    t.tv_usec = (long) (milliseconds % 1000) * 1000;
    select( 0 + 1, 0, 0, 0, &t );
    draw_screen( current_wm );
}

void device_start( window_manager *wm )
{
    current_wm = wm;
    char_x_size = 1;
    char_y_size = 1;
    pscreen_x_size = 80;
    pscreen_y_size = 40;
    mouse_init();
    //keyboard_init();
    printf( "\033[%d;%dH", pscreen_y_size+1, 1 );
    printf( " -= Music Rules the World =-                          " );
}

void device_end( window_manager *wm )
{
    //keyboard_close();
    mouse_close();
    printf( "\033[0m" );
    printf( "\033[l" );
}

long device_event_handler( window_manager *wm )
{
    if( wm->events_count == 0 ) small_pause( 20 );
    if( wm->exit_flag ) return 1;
    return 0;
}

//Get console window size:
void get_terminal_size(void)
{
    int read_fd;
    char str[256];
    int x,y;
    fprintf(stderr,"\033[r");	  /* clear scrolling region */
    fprintf(stderr,"\033[255;255H"); /* go to lower right of screen */
    fprintf(stderr,"\033[6n");	  /* query current cursor location */
    read_fd = fileno(stdin);
    read(read_fd,str,256);
    sscanf(str, "\033[%d;%dR", &pscreen_y_size, &pscreen_x_size);
    pscreen_y_size--;
}

//Make possible getchar() without "enter" at the end of string
void set_term()
{
    ioctl(0, TCGETA, &savetty);
    ioctl(0, TCGETA, &settty);
    settty.c_lflag &= ICANON;
    settty.c_lflag &= ECHO;
    ioctl(0, TCSETAF, &settty);    
}

//Reset terminal at the end of our program
void reset_term()
{
    ioctl(0, TCSETAF, &savetty);
}

int mouse_handler(Gpm_Event *event, void *data)
{
    int buttons = 0, type = 0;
    
    if(event->type & GPM_DOWN) type = 0;
    if(event->type & GPM_UP) type = 1;
    if(event->type & GPM_MOVE) type = 2;
    if(event->buttons & GPM_B_LEFT) buttons |= 1;
    if(event->buttons & GPM_B_MIDDLE) buttons |= 2;
    if(event->buttons & GPM_B_RIGHT) buttons |= 4;
    
    push_button( event->x * wm.char_x, 
                 event->y * wm.char_y,
		 buttons, 1023, type, &wm );
    if( mouse_auto_keyup )
    {
	if( type == 0 )
	{ //Auto key up:
	    type = 1;
	    push_button( event->x * wm.char_x, 
        	         event->y * wm.char_y,
			 buttons, 1023, type, &wm );
	}
    }
    
    return 0;
}

//Mouse and keyboard events processing:  (Console version)
void *mouse_thread(void *arg)
{
    int c;
    char temp[128];
    
    while((c=Gpm_Getc(stdin)) != EOF) 
    { 
	if( c == 27 )
	{ //ESCAPE codes:
	    c = Gpm_Getc( stdin ); //Get next key
	    switch( c )
	    {
		case 91:
		c = Gpm_Getc( stdin );
		switch( c )
		{
		    case 68: // LEFT
		    push_button( 0, 0, KEY_LEFT << 3, 1023, 0, current_wm );
		    break;

		    case 67: // RIGHT
		    push_button( 0, 0, KEY_RIGHT << 3, 1023, 0, current_wm );
		    break;

		    case 65: // UP
		    push_button( 0, 0, KEY_UP << 3, 1023, 0, current_wm );
		    break;

		    case 66: // DOWN
		    push_button( 0, 0, KEY_DOWN << 3, 1023, 0, current_wm );
		    break;
		    
		    case 50: 
		    c = Gpm_Getc( stdin );
		    if( c == 126 ) // INSERT
			push_button( 0, 0, KEY_INSERT << 3, 1023, 0, current_wm );
		    break;
		    
		    case 51:
		    c = Gpm_Getc( stdin );
		    if( c == 126 ) // DELETE
			push_button( 0, 0, KEY_DELETE << 3, 1023, 0, current_wm );
		    break;
		    
		    case 53:
		    c = Gpm_Getc( stdin );
		    if( c == 126 ) // PAGEUP
			push_button( 0, 0, KEY_PAGEUP << 3, 1023, 0, current_wm );
		    break;
		    
		    case 54:
		    c = Gpm_Getc( stdin );
		    if( c == 126 ) // PAGEDOWN
			push_button( 0, 0, KEY_PAGEDOWN << 3, 1023, 0, current_wm );
		    break;
		    
		    case 52:
		    c = Gpm_Getc( stdin );
		    if( c == 126 ) // END
			push_button( 0, 0, KEY_END << 3, 1023, 0, current_wm );
		    break;
		    
		    case 49: // F5 F6 F7 F8 ...
		    c = Gpm_Getc( stdin );
		    switch( c )
		    {
			case 126: //HOME (console version)
			push_button( 0, 0, KEY_HOME << 3, 1023, 0, current_wm );
			break;
			
			case 53: // F5
			c = Gpm_Getc( stdin );
			if( c == 126 )
			    push_button( 0, 0, KEY_F5 << 3, 1023, 0, current_wm );
			if( c == 59 )
			{
			    c = Gpm_Getc( stdin );
			    if( c == 50 )
			    { // SHIFT + F5
				push_button( 0, 0, (KEY_F5+KEY_SHIFT) << 3, 1023, 0, current_wm );
			    }
			}
			break;

			case 55: // F6
			c = Gpm_Getc( stdin );
			if( c == 126 )
			    push_button( 0, 0, KEY_F6 << 3, 1023, 0, current_wm );
			if( c == 59 )
			{
			    c = Gpm_Getc( stdin );
			    if( c == 50 )
			    { // SHIFT + F6
				push_button( 0, 0, (KEY_F6+KEY_SHIFT) << 3, 1023, 0, current_wm );
			    }
			}
			break;

			case 56: // F7
			c = Gpm_Getc( stdin );
			if( c == 126 )
			    push_button( 0, 0, KEY_F7 << 3, 1023, 0, current_wm );
			if( c == 59 )
			{
			    c = Gpm_Getc( stdin );
			    if( c == 50 )
			    { // SHIFT + F7
				push_button( 0, 0, (KEY_F7+KEY_SHIFT) << 3, 1023, 0, current_wm );
			    }
			}
			break;

			case 57: // F8
			c = Gpm_Getc( stdin );
			if( c == 126 )
			    push_button( 0, 0, KEY_F8 << 3, 1023, 0, current_wm );
			if( c == 59 )
			{
			    c = Gpm_Getc( stdin );
			    if( c == 50 )
			    { // SHIFT + F8
				push_button( 0, 0, (KEY_F8+KEY_SHIFT) << 3, 1023, 0, current_wm );
			    }
			}
			break;
		    }
		    break;
		}
		break;
		
		case 79:
		c = Gpm_Getc( stdin );
		switch( c )
		{
		    case 72: // HOME
		    push_button( 0, 0, KEY_HOME << 3, 1023, 0, current_wm );
		    break;
		    
		    case 70: // END
		    push_button( 0, 0, KEY_END << 3, 1023, 0, current_wm );
		    break;
		    
		    case 50: // SHIFT +
		    c = Gpm_Getc( stdin );
		    switch( c )
		    {
			case 80: // F1
			push_button( 0, 0, (KEY_F1+KEY_SHIFT) << 3, 1023, 0, current_wm );
			break;

			case 81: // F2
			push_button( 0, 0, (KEY_F2+KEY_SHIFT) << 3, 1023, 0, current_wm );
			break;

			case 82: // F3
			push_button( 0, 0, (KEY_F3+KEY_SHIFT) << 3, 1023, 0, current_wm );
			break;

			case 83: // F4
			push_button( 0, 0, (KEY_F4+KEY_SHIFT) << 3, 1023, 0, current_wm );
			break;
		    }
		    break;
		    
		    case 80: // F1
		    push_button( 0, 0, KEY_F1 << 3, 1023, 0, current_wm );
		    break;

		    case 81: // F2
		    push_button( 0, 0, KEY_F2 << 3, 1023, 0, current_wm );
		    break;

		    case 82: // F3
		    push_button( 0, 0, KEY_F3 << 3, 1023, 0, current_wm );
		    break;

		    case 83: // F4
		    push_button( 0, 0, KEY_F4 << 3, 1023, 0, current_wm );
		    break;
		}
		break;
		
		case 27: //just an ESC key:
		push_button( 0, 0, KEY_ESCAPE << 3, 1023, 0, current_wm );
		break;
	    }
	}
	else
	{
	    //other key-codes:
	    push_button( 0, 0, c << 3, 1023, 0, current_wm );
	    //if( c == 'q' ) wm.exit_flag = 1; 
	}
    }
    
    printf("Mouse thread closed\n");
    pthread_exit(0);
    return 0;
}

//Mouse and console init:
void mouse_init(void)
{
    //GPM init:
    conn.eventMask  = ~GPM_MOVE;   /* Want to know about all the events */
    conn.defaultMask = GPM_MOVE;   /* don't handle anything by default  */
    conn.minMod     = 0;    /* want everything                   */  
    conn.maxMod     = 0;    /* all modifiers included            */
        
    if(Gpm_Open(&conn, 0) == -1)  printf("Cannot connect to mouse server");

    //GPM_XTERM_ON;
    gpm_handler = mouse_handler; //our mouse handler 
    gpm_zerobased = 1;
    gpm_visiblepointer = 1;
    set_term();
    get_terminal_size();         //get size of our screen
    
    //create thread for mouse processing:
    if( pthread_create(&pt,NULL,mouse_thread,0) != 0)
    {
	printf("Can't create mouse thread :(\n");
	return;
    }
}

void mouse_close(void)
{
    reset_term();
    pthread_cancel( (ulong)&pt );
    printf("GPM_FD = %d\n",gpm_fd);
    Gpm_Close();
}

//Keyboard events processing:
void *keyboard_thread(void *arg)
{
    int c;
    char buf[2];
    
    buf[1] = 0;
    
    while( 1 )
    {
	c = read( fd, buf, 1 );
	if( c > 0 )
	{
	    prints( buf );
	}
    }
    
    printf("Keyboard thread closed\n");
    pthread_exit(0);
    return 0;
}

int keyboard_init(void)
{
    char *kbd;
    if( !(kbd = getenv("CONSOLE")) )
	kbd = KEYBOARD;
    fd = open( kbd, O_RDONLY );//| O_NONBLOCK );
    if( fd < 0 )
    {
	prints( "Can't open keyboard" );
	return -1;
    }
    
    //Save previous settings:
    if( ioctl( fd, KDGKBMODE, &old_kbd_mode ) < 0 )
    {
	prints( "Can't save previous keyboard settings" );
	switch( errno )
	{
	    case EBADF: prints( "invalid descriptor" );
	    case EFAULT: prints( "memory error" );
	    case ENOTTY: prints( "not character device" );
	    case EINVAL: prints( "invalid request" );
	}
	return -1;
    }
    if( tcgetattr( fd, &old ) < 0 )
    {
	prints( "Can't save previous keyboard settings (2)" );
	return -1;
    }
    
    //Set medium-raw keyboard mode: (for scancodes getting)
    termios n;
    n = old;
    n.c_lflag &= ~( ICANON | ECHO /*| ISIG*/ );
    n.c_iflag &= ~( ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON /*| BRKINT*/ );  // :-(    )    
    n.c_cc[ VMIN ] = 0;
    n.c_cc[ VTIME ] = 0;
    if( tcsetattr( fd, TCSAFLUSH, &n ) < 0 )
    {
	prints( "tcsetattr() error in keyboard_init()" );
	keyboard_close();
	return -1;
    }
    if( ioctl( fd, KDSKBMODE, K_MEDIUMRAW ) < 0 )
    {
	prints( "Can't set medium-raw mode" );
	keyboard_close();
	return -1;
    }
    
    //create thread for keyboard processing:
    /*if( pthread_create(&key_th,NULL,keyboard_thread,0) != 0)
    {
	prints("Can't create keyboard thread :(\n");
	return -1;
    }*/

    return 0;
}

int keyboard_close(void)
{
    if( fd >= 0 )
    {
	//pthread_cancel( (ulong)&key_th );
	//reset terminal mode:
	if( ioctl( fd, KDSKBMODE, old_kbd_mode ) < 0 )
	{
	    prints( "ioctl() error in keyboard_close()" );
	    return -1;
	}
	tcsetattr( fd, TCSAFLUSH, &old );
	close( fd );
    }
    fd = -1;
    
    return 0;
}

//#################################
//#################################
//#################################

#endif
