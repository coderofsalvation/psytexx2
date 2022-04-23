#ifndef __PSYWINSCROLL__
#define __PSYWINSCROLL__

#include "core/core.h"
#include "window_manager/wmanager.h"

#define BUTTON_SIZE 2
#define BUTTON_SIZE_STR "2"

extern char NEW_SCROLL_TYPE;

//INTERNAL STRUCTURES:

struct scrollbar_data
{
    char type;       //0 - vertical; 1 - horisontal
    long max;        //Max value
    long cur;        //Current value
    long scroll_size;//Scrollbar size (in pixels) without buttons
    long slider_size_items; //Size of real slider (in items)
    long slider_size;       //Size of real slider (in pixels)
    long slider_pos;        //Slider position (in pixels)
    long one_pixel_size;//Size of one pixel (in items). 10 bit fixed point value
    long step;       //One step size (default = 4)

    long this_window;
    long button_up;
    long button_down;

    char pressed;  //If pressed
    
    char we_are_in_slider;
    long start_x;
    long start_y;
    long start_value;
    
    char show_value_flag;
    char show_value_type; //0 - hex; 1 - dec
    long show_value_offset;
    
    long (*handler)(void*,long,void*); //User defined handler: 
                                       //handler(void *user_data, long scrollbar_window, void *window_manager)
    void *user_data;                   //Data for handler
};

//FUNCTIONS:

void scrollbar_check( long win_num, window_manager* ) sec1;
void scrollbar_draw( long win_num, window_manager* ) sec1;
void scrollbar_set_parameters( long win_num, long type, long max, long cur, long slider_size_items, window_manager* ) sec1;
void scrollbar_set_cur_value( long win_num, long cur, window_manager* ) sec1;
void scrollbar_set_step( long win_num, long step, window_manager* ) sec1;
void scrollbar_set_value_showing( long win_num, 
                                  char show_value_flag, 
				  char show_value_type, 
				  long show_value_offset, 
				  window_manager *wm ) sec1;
void scrollbar_set_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;

//HANDLERS:

long up_handler( void* user_data, long button_win, window_manager* ) sec1;
long down_handler( void* user_data, long button_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long scrollbar_handler( event*, window_manager* ) sec1; //Scrollbar handler

#endif

