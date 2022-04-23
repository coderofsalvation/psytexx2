#ifndef __PSYWINSAMPLEEDITOR__
#define __PSYWINSAMPLEEDITOR__

#include "core/core.h"
#include "window_manager/wmanager.h"

//PROPERTIES:

//INTERNAL STRUCTURES:

struct smpeditor_data
{
    long this_window;
    
    long scroll_volume;
    long scroll_pan;
    long scroll_finetune;
    long scroll_relative;
    long smpview;
    
    long button_close;
    long button_loop;
    long button_menu;
    long button_zoomin;
    long button_zoomout;
};

//FUNCTIONS:

void smpeditor_redraw( long win_num, window_manager *wm ) sec1;
void smpeditor_draw_info( long win_num, window_manager *wm ) sec1;

//HANDLERS:

long scroll_smpvol_handler( void *user_data, long win, window_manager* ) sec1;
long scroll_smppan_handler( void *user_data, long win, window_manager* ) sec1;
long scroll_smpfine_handler( void *user_data, long win, window_manager* ) sec1;
long button_loop_handler( void *user_data, long win, window_manager* ) sec1;
long button_menu_handler( void *user_data, long win_num, window_manager *wm ) sec1;
long button_zoomin_handler( void *user_data, long win_num, window_manager *wm ) sec1;
long button_zoomout_handler( void *user_data, long win_num, window_manager *wm ) sec1;
long button_sclose_handler( void *user_data, long win, window_manager* ) sec1;

//WINDOW HANDLERS:

long smpeditor_handler( event*, window_manager* ) sec1;

#endif

