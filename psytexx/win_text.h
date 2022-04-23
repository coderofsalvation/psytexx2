#ifndef __PSYWINTEXT__
#define __PSYWINTEXT__

#include "core/core.h"
#include "window_manager/wmanager.h"

#define MAX_TEXT_LEN 512

extern long NEW_TEXT_SIZE; 

//INTERNAL STRUCTURES:

struct text_data
{
    long this_window;
    
    long readonly;
    uchar *text;
    long text_len;
    long cursor_position;
    long shift_status;
    
    long numerical_flag; //Simple text field or numerical ( 0 or 1 )
    long button_left;    //Buttons for numerical text field
    long button_right;   //...
    long min_value;      //Value bounds for numerical text field
    long max_value;      //...
    
    uchar *caption;      //Text field caption ( default value = 0 )
    
    long active;         // = 0 after ENTER

    long (*handler)(void*,long,void*); //User defined handler: 
                                       //handler(void *user_data, long text_window, void *window_manager)
    void *user_data;                   //Data for handler
};

//FUNCTIONS:

void text_set_text( long win_num, char *text, window_manager* ) sec1;
char* text_get_text( long win_num, window_manager* ) sec1;
void text_set_readonly( long win_num, long readonly, window_manager* ) sec1;
void text_set_numerical( long win_num, long numerical, window_manager* ) sec1;
void text_set_caption( long win_num, char *caption, window_manager* ) sec1;
void text_set_bounds( long win_num, long min, long max, window_manager* ) sec1;
void text_set_value( long win_num, long value, window_manager* ) sec1;
long text_get_value( long win_num, window_manager* ) sec1;
void text_set_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;

//HANDLERS:
long button_left_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_right_handler( void* user_data, long button_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long text_handler( event*, window_manager* ) sec1;

#endif

