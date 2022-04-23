#ifndef __WINDOWMANAGER__
#define __WINDOWMANAGER__

//Principles of working:
//  window size quantum = 1 text character (for example: 8x8);
//  it's static window manager - you need to redraw all windows, if some window was moved or resized;
//  root window or dialog must use draw_window_touch_area() on DRAW event handling;
//  child windows (buttons, scrollbars etc) must use draw_touch_box() on DRAW event handling.

#include "struct.h"

extern long pscreen_x_size;   //Screen x-size (in pixels)
extern long pscreen_y_size;   //Screen y-size (is pixels)
extern long char_x_size;      //One char (symbol) x-size
extern long char_y_size;      //One char (symbol) y-size

#ifdef LINUX
extern int mouse_auto_keyup;
#endif

extern unsigned char font[2048];

//################################
//## MAIN FUNCTIONS:            ##
//################################

void win_init( window_manager* );			    //init window manager
void win_close( window_manager* );			    //close window manager
void resize_all_windows( window_manager* );		    //resize (using win string controls) all windows
void resize_all( long, window_manager* );		    //resize (reinit) all screens and windows

//################################
//## EVENT FUNCTIONS:           ##
//################################

void event_loop(window_manager*, int loop);		    //main event loop
void set_eventloop_handler( void (*)( window_manager* ), window_manager * ); //set user-defined eventloop handler
int  process_event(event*,window_manager*);		    //process event
void send_event(long,long,long x,long y,long button, long pressure, long mode, window_manager*);
							    //send event to some window
long null_handler(event*,window_manager*);		    //SIMPLE WIN HANDLER: null window
long child_handler(event*,window_manager*);		    //SIMPLE WIN HANDLER: child window
long desktop_handler(event*,window_manager*);		    //SIMPLE WIN HANDLER: desktop window
long create_scrollarea(long win_num,window_manager*);	    //
long scrollarea_handler(event*,window_manager*);	    //SIMPLE WIN HANDLER: small scroll buttons for an window
long keyboard_handler(event*,window_manager*);		    //SIMPLE WIN HANDLER: virtual keyboard

//################################
//## TIME FUNCTIONS:            ##
//################################

void wm_timer_set( ulong t, void (*)( void*, window_manager* ), void *data, window_manager *wm ); //Set timer. 1024 = one second
void wm_timer_close( window_manager *wm );

//################################
//## CONNECTION WITH THE OS:    ##
//################################

void push_button(long x,long y,long button,long pressure,   //Send button press/move/unpress event to the window manager
                 long type,window_manager *wm);		    //x/y: coordinates on the screen (in pixels)
		                                            //button: 1 - left; 2 - middle; 4 - right; 8 - keyboard...
							    //pressure: button pressure (for MIDI or Digital Pen)
							    //type: 0 - down; 1 - up; 2 - move
							    //This function must be called by OS

//################################
//## WINDOW FUNCTIONS:          ##
//################################

long create_window(char *name,
                   long x,long y,long x_size, long y_size, long color, long draw_box,
                   long parent, 
		   long (*)(event*,window_manager*), 
		   window_manager*) sec4;
void close_window(long win_number, window_manager*) sec4;
void create_child(long parent, long child_win, window_manager *wm) sec4;
void move_window( long win_num, long x, long y, window_manager *wm ) sec4;
void calculate_real_window_position( long win_num, window_manager *wm ) sec4; //Calculate real window values
void calculate_real_window_position_with_childs( long win_num, window_manager *wm ) sec4; //Calculate real window values
void set_window_string_controls( long win_num, char *xs, char *ys, char *xs2, char *ys2, window_manager *wm ) sec4;

//################################
//## DRAWING FUNCTIONS:         ##
//################################

void int_to_string( int value, char *str ) sec4;   //DEC
void int_to_string_h( int value, char *str ) sec4; //HEX
void ext_long_to_string( long value, char *str, long chars, window_manager* ) sec4;
long get_color( uchar r, uchar g, uchar b ) sec4;                 //Get color value by RGB
long red( COLOR c ) sec4;
long green( COLOR c ) sec4;
long blue( COLOR c ) sec4;
COLOR blend( COLOR c1, COLOR c2, int value ) sec4;
void load_font( window_manager* ) sec4;                           //Load user font (font.bmp)
void load_background_bmp( window_manager* ) sec4;                 //Load user background (back.bmp) and init color gradient
void draw_string(long,long,long,long,char*,window_manager*) sec4; //draw text string (relative x/y in pixels) in some window
void draw_pixel( long win_num, long x, long y, long color, window_manager *wm ) sec4;
void draw_rectangle( long win_num, long x, long y, long x_size, long y_size, long color, window_manager *wm ) sec4;
void draw_rectangle2( long win_num, long x, long y, long x_size, long y_size, long color, window_manager *wm ) sec4;
void draw_line( long win_num, int x1, int y1, int x2, int y2, COLOR color, window_manager *wm );
void draw_vert_line( long win_num, long x, long y, long y_size, long color, long add, window_manager *wm ) sec4;
void draw_horis_line( long win_num, long x, long y, long x_size, long color, long add, window_manager *wm ) sec4;
void pdraw_box(long win_num, long x, long y,                      //Draw box (ralative x/y in pixels) in some window
               long x_size, long y_size, long color,
 	       window_manager*) sec4;                    
void draw_box(long win_num, long x, long y,                       //Draw box (ralative x/y in symbols) in some window
              long x_size, long y_size, long color,
	      long chr, window_manager*) sec4; 
//Draw touch box (for the new child-window) on exist window:
void draw_touch_box( long back_win, long x, long y, long x_size, long y_size,
                     long fore_win, window_manager *wm ) sec4;
void matrix_draw_string(long,long,long,uchar,char*,window_manager*) sec4; //draw text string in matrix
                                                                          //(relative x/y in symbols)
void matrix_draw_box(long win_num, long x, long y,                       //Draw box in matrix
                     long x_size, long y_size, long color,               //(ralative x/y in symbols)
	             window_manager*) sec4;                    
void matrix_clear_box(long win_num, long x, long y,                      //Clear box in matrix
                      long x_size, long y_size,                          //(ralative x/y in symbols)
	              window_manager*) sec4;                    
void matrix_draw( window_manager* ) sec4;
void draw_window_box(long,window_manager*) sec4;                  //draw window box
//Draw touch box (for the new parent window) over all windows:
void draw_window_touch_area(long,window_manager*) sec4;           //draw window touch area
void redraw_screen(window_manager*) sec4;                         //redraw full screen

//################################
//## DEVICE DEPENDENT:          ##
//################################

void create_active_screen_part(window_manager*) sec4;        //DEVICE DEPENDENT: create active screen part
void close_active_screen_part(window_manager*) sec4;         //DEVICE DEPENDENT: close active screen part
void fast_draw_char(long ptr,long,long,uchar,window_manager*) sec4;//DEVICE DEPENDENT: fast draw symbol
void draw_char(long,long,long,long,window_manager*) sec4;    //DEVICE DEPENDENT: draw one symbol (x/y in pixels)
void draw_screen_part(long,window_manager*) sec4;            //DEVICE DEPENDENT: draw active screen part
void draw_screen(window_manager*) sec4;                      //DEVICE DEPENDENT: draw changed parts of the screen
void set_palette(window_manager*) sec4;                      //DEVICE DEPENDENT: set RGB palette

//################################
//## DECLARED IN EVENTLOOP.C:   ##
//################################

long device_event_handler(window_manager*) sec4;             //DEVICE DEPENDENT: device event handler (return 1 for EXIT)
void device_start(window_manager*) sec4;                     //DEVICE DEPENDENT: device start (before main loop)
void device_end(window_manager*) sec4;                       //DEVICE DEPENDENT: device end   (after main loop)


#endif

