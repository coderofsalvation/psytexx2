#ifndef __PSYWINPATTERN__
#define __PSYWINPATTERN__

#include "core/core.h"
#include "window_manager/wmanager.h"

extern long text_bpm;
extern long text_speed;
extern long text_patsize;
extern long text_add;
extern long text_channels;

extern ulong psytexx_start_time;
extern ulong play_start_time;
extern ulong playing_time;

//INTERNAL STRUCTURES:

struct pattern_data
{
    long this_window;    //this window handler
    //Text windows:
    long text_bpm;       //Global BPM
    long text_speed;     //Global Speed
    long text_patsize;   //Current pattern size
    long text_add;       //Write note step
    long text_channels;  //Number of channels
    //Channels window:
    long channels;
};

//FUNCTIONS:

void psypattern_redraw_channels( long win_num, window_manager *wm ) sec1;
void psypattern_draw_time( long win_num, window_manager *wm ) sec1;

//HANDLERS:

long text_bpm_handler( void* user_data, long button_win, window_manager* ) sec1;
long text_speed_handler( void* user_data, long button_win, window_manager* ) sec1;
long text_patsize_handler( void* user_data, long button_win, window_manager* ) sec1;
long text_add_handler( void* user_data, long button_win, window_manager* ) sec1;
long text_channels_handler( void* user_data, long button_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long pattern_handler( event*, window_manager* ) sec1; //Pattern properties

#endif

