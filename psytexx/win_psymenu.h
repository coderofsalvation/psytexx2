#ifndef __PSYWINMENU__
#define __PSYWINMENU__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct menu_data
{
    long play_button;
    long patplay_button;
    long stop_button;
    long net_button;
    long files_button;
    long playlist_button;
    long kbd_button;
    long config_button;
    long clear_button;

    long menu_button;
    
    long files_status;
    long config_status;
    long playlist_status;
};

//FUNCTIONS:

//HANDLERS:

long play_button_handler( void *user_data, long button_win, window_manager* );
long patplay_button_handler( void *user_data, long button_win, window_manager* );
long stop_button_handler( void *user_data, long button_win, window_manager* );
long net_button_handler( void *user_data, long button_win, window_manager* );
long files_button_handler( void *user_data, long button_win, window_manager* );
long kbd_button_handler( void *user_data, long button_win, window_manager* );
long config_button_handler( void *user_data, long button_win, window_manager* );
long playlist_button_handler( void *user_data, long button_win, window_manager* );
long clear_button_handler( void *user_data, long button_win, window_manager* );

//WINDOW HANDLERS:

long menu_handler( event*, window_manager* ) sec1;

#endif

