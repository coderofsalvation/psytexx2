#ifndef __PSYWINBUTTON__
#define __PSYWINBUTTON__

#include "core/core.h"
#include "window_manager/wmanager.h"

extern char txt_up[4];
extern char txt_down[4];
extern char txt_left[4];
extern char txt_right[4];

//INTERNAL STRUCTURES:

struct button_data
{
    long this_window;
    char *name;     //Button name
    long len;       //Name length (in chars)
    char pressed;   //If pressed
    long color;     //Button color
    long x;         //Name x-position (in pixels)
    long y;         //Name y-position (in pixels)
    char autorepeat;//Auto-repeat mode ON/OFF
    char autorepeat_pressed;
    long (*handler)(void*,long,void*); //User defined handler: 
                                       //handler(void *user_data, long button_window, void *window_manager)
    void *user_data;                   //Data for handler
};

//FUNCTIONS:

void button_set_color( COLOR color, long win_num, window_manager* ) sec1;
void button_set_name( char *name, long win_num, window_manager* ) sec1; 
void button_set_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;
void button_set_autorepeat( char autorepeat, long win_num, window_manager* wm ) sec1;

//WINDOW HANDLERS:

long button_handler( event*, window_manager* ) sec1; //Button handler

#endif

