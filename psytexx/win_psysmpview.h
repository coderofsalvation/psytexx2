#ifndef __PSYWINSAMPLEVIEW__
#define __PSYWINSAMPLEVIEW__

#include "core/core.h"
#include "window_manager/wmanager.h"

#define SAMPLE_MENU "SELECT ALL\nCUT\nCOPY\nPASTE\nCROP\nCLEAR\nSET LOOP\nSMOOTH\nNORMALIZE\nREVERSE"

//PROPERTIES:

//INTERNAL STRUCTURES:

struct smpview_data
{
    long this_window;

    long scrollbar;
    int precision_shift;

    short *prev_smp_data;
    int prev_smp_len;

    void *copy_buffer;

    int start_x;
    int start_y;
    int start_offset;
    int start_reppnt1;
    int start_reppnt2;
    int dragmode; //1 - cursor drag; 2 - reppnt1; 3 - reppnt2

    long offset;
    long delta;		    //Number of samples in one pixel

    int cursor;
    int selected_size;
};

//FUNCTIONS:

void smpview_edit_op( smpview_data *data, int op_num ) sec1;
void smpview_zoom( long win_num, int inout, window_manager *wm ) sec1;
void smpview_redraw( long win_num, window_manager *wm ) sec1;

//HANDLERS:

//WINDOW HANDLERS:

long smpview_handler( event*, window_manager* ) sec1;

#endif

