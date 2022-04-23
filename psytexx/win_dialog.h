#ifndef __PSYWINDIALOG__
#define __PSYWINDIALOG__

#include "core/core.h"
#include "window_manager/wmanager.h"

//INTERNAL STRUCTURES:

struct dialog_data
{
    long this_window;
    long button_ok;
    long button_cancel;
    long current_button;
    
    char *title;
    long title_len;
    
    long old_focus_window;
    
    void (*handler)(void*,long,window_manager*);
    void *user_data;
};

extern long dialog_visible;

//FUNCTIONS:

void start_dialog( char *text, char *ok, char *cancel, 
                   void (*)(void*, long, window_manager*),   //(user data, pressed button, window_manager)
		   void *user_data,
                   long win_num, window_manager* );

int start_dialog_blocked( 
    char *text, char *ok, char *cancel, 
    window_manager* );

//HANDLERS:

long ok_button_handler( void *user_data, long button_win, window_manager* ) sec1;
long cancel_button_handler( void *user_data, long button_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long dialog_handler( event*, window_manager* ) sec1;

#endif

