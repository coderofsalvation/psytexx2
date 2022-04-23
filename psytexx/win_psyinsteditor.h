#ifndef __PSYWININSTRUMENTEDITOR__
#define __PSYWININSTRUMENTEDITOR__

#include "core/core.h"
#include "window_manager/wmanager.h"

//PROPERTIES:

//INTERNAL STRUCTURES:

struct insteditor_data
{
    long this_window;
    
    long pars_window;

    long scroll_volume;
    long scroll_pan;
    long scroll_finetune;
    long scroll_relative;
    long scroll_vibsweep;
    long scroll_vibdepth;
    long scroll_vibrate;
    long button_death;
    
    long button_close;
};

//FUNCTIONS:

void insteditor_redraw( long win_num, window_manager *wm ) sec1;

//HANDLERS:

long scroll_instvol_handler( void *user_data, long win, window_manager* ) sec1;
long scroll_instpan_handler( void *user_data, long win, window_manager* ) sec1;
long scroll_instfine_handler( void *user_data, long win, window_manager* ) sec1;
long scroll_instrelative_handler( void *user_data, long win, window_manager *wm ) sec1;
long scroll_instsweep_handler( void *user_data, long win, window_manager *wm ) sec1;
long scroll_instdepth_handler( void *user_data, long win, window_manager *wm ) sec1;
long scroll_instrate_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_iclose_handler( void *user_data, long win, window_manager* ) sec1;

//WINDOW HANDLERS:

long insteditor_handler( event*, window_manager* ) sec1;

#endif

