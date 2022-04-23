#ifndef __PSYWINPLAYLIST__
#define __PSYWINPLAYLIST__

#include "core/core.h"
#include "window_manager/wmanager.h"

//PROPERTIES:
#define PROP_PLAYLIST   "prop_playlist" 

//INTERNAL STRUCTURES:

struct playlist_data
{
    long this_window;
    
    long button_close;
    long button_addfile;
    long button_addfiles;
    long button_delfile;
    long button_up;
    long button_down;
    long button_clear;
    long button_play;
    long button_random;
    long button_next;
    long button_prev;
    
    long list_files;
    
    long current_selected;
    
    long random_mode;
};

//FUNCTIONS:

void playlist_play_selected( long win_num, window_manager *wm ) sec1;
void playlist_select_next_track( long win_num, window_manager *wm ) sec1;
void playlist_select_previous_track( long win_num, window_manager *wm ) sec1;
void playlist_save( char *filename, long win_num, window_manager *wm ) sec1;
void playlist_load( char *filename, long win_num, window_manager *wm ) sec1;

//HANDLERS:

long button_pclose_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_addfile_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_addfiles_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_delfile_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_pup_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_pdown_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_plplay_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_random_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_plnext_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_plprev_handler( void *user_data, long win, window_manager *wm ) sec1;

//WINDOW HANDLERS:

long playlist_handler( event*, window_manager* ) sec1;

#endif

