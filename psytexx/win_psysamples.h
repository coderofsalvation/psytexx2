#ifndef __PSYWINSAMPLES__
#define __PSYWINSAMPLES__

#include "core/core.h"
#include "window_manager/wmanager.h"

extern long current_instrument;
extern long current_sample;

//INTERNAL STRUCTURES:

struct samples_data
{
    long this_window;
    long instruments_list;
    long samples_list;
    
    long ins_edit_button;
    long smp_edit_button;
    long ins_page_button;

    long button;
    int ins_or_smp;

    char *old_ys2;

    long current_instrument;
    long current_sample;
};

//FUNCTIONS:

void samples_refresh( long win_num, window_manager* ) sec1;  //Refresh instrument list
void samples2_refresh( long win_num, window_manager* ) sec1; //Refresh sample list
void unscale_top_win( samples_data *data, window_manager *wm ) sec1;

//HANDLERS:

long button_insteditor_handler( void *user_data, long win, window_manager *wm ) sec1;
long button_smpeditor_handler( void *user_data, long win, window_manager *wm ) sec1;
long smp_list_handler( void* user_data, long list_win, window_manager* ) sec1;
long instr_list_handler( void* user_data, long list_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long samples_handler( event*, window_manager* ) sec1;

#endif

