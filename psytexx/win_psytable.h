#ifndef __PSYWINTABLE__
#define __PSYWINTABLE__

#include "core/core.h"
#include "window_manager/wmanager.h"

//NEW TABLE: =======================
//(SET IT UP BEFORE WINDOW CREATION)
extern long table_type; 
extern int CREATE_TABLE_WITH_EDIT_BUTTON;
//==================================

enum field_type
{
    FIELD_NONE = 0,
    FIELD_CHANNEL,
    FIELD_PATTERN_NUM,
    FIELD_PATTERNTABLE_POS,
    FIELD_NOTE,
    FIELD_PATTERN_POS,
    FIELD_LAST
};

enum element_type
{
    ELEMENT_NONE = 0,
    ELEMENT_PATTERN_NUM,
    ELEMENT_NOTE,
    ELEMENT_INSTRUMENT,
    ELEMENT_VOLUME,
    ELEMENT_EFFECT,
    ELEMENT_PARAMETER,
    ELEMENT_DIVIDER,
    ELEMENT_LAST
};

extern uchar element_xsize[ ELEMENT_LAST ];

#define MAX_ELEMENTS           8
#define TYPE_VSCROLL           1
#define TYPE_HSCROLL           2
#define TYPE_DIVIDERS          4
#define HALIGN                 1
#define VALIGN                 2


//TABLE:
//
//#######################
//#   #
//#   #        H
//#   #
//#######################
//#   #    |    |    |
//#   #    |    |    |
//#   #------------------
//# V #    |    |    |
//#   #    |  CELLS  |
//#   #------------------
//#   #    |    |    |
//
//H - horisontal field
//V - vertical field
//
//One cell = array of elements

//INTERNAL STRUCTURES:

struct table_data
{
    long this_window;

    long h_scroll;    //HScroll window
    long v_scroll;    //VScroll window

    long type;        //Table type: TYPE_xxx
    long scroll_size; //Size of scrollbars
    
    long vertical_field;   //check the field_type enum
    long horisontal_field; //check the field_type enum
    long vertical_field_size;
    long horisontal_field_size;
    
    long elements_align;   //HALIGN or VALIGN (horisontal or vertical)
    long elements[ MAX_ELEMENTS ]; //One cell of the table (visible)
    long elements_num;     //Number of elements in one cell
    long full_elements[ MAX_ELEMENTS ]; //Full cell of the table
    long full_elements_num; //Number of elements in full cell
    long cell_xsize;
    long cell_ysize;
    
    long xstart;  //Start position in horisontal field
    long ystart;  //Start position in vertical field
    long v_offset;//Offset in vertical field
    long h_offset;//Offset in horisontal field
    long v_cells; //Number of vertical visible cells in current window
    long h_cells; //Number of horisontal visible cells in current window
    
    long current_cell;    //Number of current cell
    long current_element; //Number of current element
    long current_offset;  //Number of current element offset
    
    uchar scroll_flag;
    
    //Selection:
    long selection;       //0 - finished; 1 - user selecting new region
    long sel_x;
    long sel_y;
    long sel_xsize;
    long sel_ysize;

    long fullscreen_button;
    long edit_button;
    uchar fullscreen;        //1 - fullscreen button enabled
    uchar fullscreen_status; //1 - maximized
    long change_view_button;
    uchar change_view;       //1 - "change view" button enabled
    uchar change_view_status;
    char *old_y_string;
    
    //Tracker specific variables:
    long record_status;   //Record mode ON/OFF
    COLOR record_color;
    
    void *copy_buffer[3]; //Full table (pattern) / Block / Column (channel)
    long copy_buffer_xsize[3];
    long copy_buffer_ysize[3];
};

//FUNCTIONS:

long table_field_values( long field_type ) sec2; //Get number of field values
char* table_field_text( long field_type, long value, window_manager* ) sec2; //Convert field value to text
long table_field_value( long field_type ) sec2; //Get current (real) field value
void table_field_set_value( long field_type, long new_value ) sec2;
long table_get_element_value( table_data *data, long element_type ) sec2;
long table_set_element_value( table_data *data, long element_type, long element_value ) sec2;
void table_cell_draw( table_data *data,
                      long win_num,
		      long x,
		      long y,
		      long highlight,
		      long current_cell_flag,
		      window_manager* ) sec2;
void table_draw_record_status( long win_num, char, window_manager* ) sec2;
void table_draw( long win_num, window_manager* ) sec2;
void table_new( long win_num,
		long type,
                long vertical_field,
		long horisontal_field,
		long elements_align,
		long elements_num,
		window_manager*,
		... ) sec2;
void table_full_cell( long win_num, long elements_num, window_manager*, ... ) sec2;    
void table_set_type( long type_flags, long win_num, window_manager* ) sec2;
void table_set_fullscreen( long fullscreen, long win_num, window_manager* ) sec2;
void table_set_change_view( long change_view, long win_num, window_manager* ) sec2;
void handle_key( ulong key, long win_num, window_manager* ) sec2;

//HANDLERS:

long change_view_button_handler( void* user_data, long button_win, window_manager* wm ) sec2;
long fullscreen_button_handler( void* user_data, long button_win, window_manager* wm ) sec2;
long v_scroll_handler(void *user_data, long scroll_win, window_manager*);
long h_scroll_handler(void *user_data, long scroll_win, window_manager*);

//WINDOW HANDLERS:

long table_handler( event*, window_manager* );

#endif

