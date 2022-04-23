#ifndef __PSYWINNET__
#define __PSYWINNET__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct net_data
{
    long this_window;

    long ctls_window;
    long win_menu;
    long new_button;

    long list_window;
    long list_window_opened;
    long items_list;
    long items_ok;
    long items_cancel;

    long selected_item;

    int new_item_x;	//In precents
    int new_item_y;	//In precents

    int link_drag;

    int drag_started;
    int drag_start_x;
    int drag_start_y;
    int drag_item_x;
    int drag_item_y;

    int offset_x;	//In percents (0..1024)
    int offset_y;	//In percents (0..1024)
};

//FUNCTIONS:

void net_redraw( net_data *data, window_manager *wm );

//HANDLERS:

long new_button_handler( void *user_data, long button_win, window_manager *wm );
long item_ok_button_handler( void *user_data, long button_win, window_manager *wm );
long item_cancel_button_handler( void *user_data, long button_win, window_manager *wm );

//WINDOW HANDLERS:

long net_handler( event*, window_manager* );

#endif

