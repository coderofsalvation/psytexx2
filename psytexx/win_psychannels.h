#ifndef __PSYWINCHANNELS__
#define __PSYWINCHANNELS__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct channels_data
{
    long this_window;

    long mouse_button;
};

//FUNCTIONS:

void channels_redraw( long win_num, window_manager* ) sec1;

//WINDOW HANDLERS:

long channels_handler( event*, window_manager* ) sec1; //Channels (scopes) handler

#endif

