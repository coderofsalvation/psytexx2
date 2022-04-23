#ifndef __PSYWINLIST__
#define __PSYWINLIST__

#include "core/core.h"
#include "window_manager/wmanager.h"

#define SCROLL_SIZE 2

//INTERNAL STRUCTURES:

#ifdef NOSTORAGE
    #define MAX_SIZE 16000
    #define MAX_ITEMS (long)4000
#else
    #define MAX_SIZE 65000
    #define MAX_ITEMS (long)16000
#endif

struct list_data
{
    long this_window;
    long button_up;  //UP button
    long button_down;//DOWN button
    long scrollbar;  //scrollbar window
    char *items;     //Items data
    long *items_ptr; //Item pointers
    long items_num;  //Number of items; 
    long first_item; //First showed item;
    long selected_item;

    long numbered_flag;
    long number_offset;
    
    long pressed_button; //For the list handler

    char editable;   //Editable flag (1/0)
    long edit_field; //Text window for editable lists

    long (*handler)(void*,long,void*); //User defined handler: 
                                       //handler(void *user_data, long list_window, void *window_manager)
    void *user_data;                   //Data for handler
};

//FUNCTIONS:

void list_set_numbered( long numbered_flag, long offset, long win_num, window_manager* ) sec1;
void list_set_editable( long editable_flag, long win_num, window_manager* ) sec1;
void list_set_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;
void list_set_edit_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;
void list_draw( long win_num, window_manager* ) sec1;                           //Redraw list-window content
void list_clear( long win_num, window_manager* ) sec1;                          //Clear list-window
void list_reset_selection( long win_num, window_manager* ) sec1;                
void list_add_item( char *item, char attr, long win_num, window_manager* ) sec1;//Add new item (with attribut) to list-window
void list_delete_item( long item_num, long win_num, window_manager* ) sec1;
void list_move_item_up( long item_num, long win_num, window_manager* ) sec1;
void list_move_item_down( long item_num, long win_num, window_manager* ) sec1;
char list_get_attr( long item_num, long win_num, window_manager* ) sec1;        //Get item's attribute
char* list_get_item( long item_num, long win_num, window_manager* ) sec1;       //Get item from the list-window
long list_get_selected_num( long win_num, window_manager* ) sec1;               //Get selected item number (or -1)
long list_compare_items( long item1, long item2, long win_num, window_manager *wm ) sec1;
void list_sort( long win_num, window_manager *wm ) sec1;
void list_make_selection_visible( long win_num, window_manager *wm ) sec1;

//HANDLERS:

long scroll_handler( void* user_data, long scroll_window, window_manager* ) sec1;

//WINDOW HANDLERS:

long list_handler( event*, window_manager* ) sec1; //List manager

#endif

