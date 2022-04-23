#ifndef __PSYWINPOPUP__
#define __PSYWINPOPUP__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct popup_data
{
    long this_window;

    char *text;

    long result;
    int selected;

    long old_focus_window;
};

//FUNCTIONS:

int start_popup_blocked( 
    char *text,
    int x, int y,
    window_manager* );

//HANDLERS:

//WINDOW HANDLERS:

long popup_handler( event*, window_manager* ) sec1;

#endif

