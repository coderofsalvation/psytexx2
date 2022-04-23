#ifndef __PSYWINPATTERNTAB__
#define __PSYWINPATTERNTAB__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct patterntab_data
{
    long this_window;    //this window handler
    //Buttons:
    long button_inc;
    long button_dec;
    long button_ins;
    long button_del;
};

//FUNCTIONS:

//HANDLERS:

long button_inc_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_dec_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_ins_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_del_handler2( void* user_data, long button_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long patterntab_handler( event*, window_manager* ) sec1; //Pattern properties

#endif

