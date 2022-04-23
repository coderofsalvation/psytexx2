/*
    PsyTexx: win_scrollbar.cpp. Scrollbar handler
    Copyright (C) 2002 - 2007  Zolotov Alexandr

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//*** Contact info: Zolotov Alexandr (NightRadio project)
//***               Ekaterinburg. Russia.
//***               Email: nightradio@gmail.com
//***               WWW: warmplace.ru

#include "../win_main.h"
#include "../win_scrollbar.h"
#include "../win_button.h"

char NEW_SCROLL_TYPE = 0;

void scrollbar_check( long win_num, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;

    if( data->max == 0 ) data->cur = 0;
    if( data->cur < 0 ) data->cur = 0;
    if( data->cur >= data->max + 1 ) data->cur = data->max; 
}

void scrollbar_draw( long win_num, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;

    long slider_size;
    long slider_pos;
    
    long move_region;  // in pixels
    //Scrollbar: <----###>
    //           < > - buttons (scroll left / scroll right)
    //           --- - move region
    //           ### - slider

    //Caclulate scroll-bar size (in pixels)
    long win_size;    
    long scroll_size; //scrollbar size without buttons
    if( data->type == 0 ) win_size = win->y_size * wm->char_y;
	else win_size = win->x_size * wm->char_x;
    if( data->type == 0 ) scroll_size = ( win->y_size - (BUTTON_SIZE<<1) ) * wm->char_y;
	else scroll_size = ( win->x_size - (BUTTON_SIZE<<1) ) * wm->char_x;
    data->scroll_size = scroll_size;

    if( scroll_size == 0 ) return;

    if( data->max == 0 || data->slider_size_items == 0 )
    {
	slider_size = scroll_size;
	slider_pos = 0;
    }
    else
    {
        //Calculate move-region (in pixels)
	long ppage = (data->slider_size_items << 10) / (data->max+1); if( ppage == 0 ) ppage = 1;
	move_region = (scroll_size << 10) / (ppage + (1<<10)); if( move_region == 0 ) move_region = 1;

	//Caclulate slider size (in pixels)
	slider_size = scroll_size - move_region;
	data->slider_size = slider_size;

        //Calculate one pixel size
	data->one_pixel_size = ( (data->max+1) << 10 ) / move_region;

        //Calculate slider position (in pixels)
	slider_pos = (data->cur << 10) / data->max;
        slider_pos *= move_region;
	slider_pos >>= 10;
        data->slider_pos = slider_pos;
    }
    
    //Draw it:
    if( data->type == 0 )
    { //Vertical:
	pdraw_box( win_num, 0, BUTTON_SIZE * wm->char_y, 
	           win->x_size * wm->char_x, 
		   slider_pos, 
		   wm->colors[6], wm );
	pdraw_box( win_num, 0, 
	           slider_pos + ( BUTTON_SIZE * wm->char_y ), 
		   win->x_size * wm->char_x, 
		   slider_size, 
		   wm->colors[13], wm );
	pdraw_box( win_num, 0, 
	           slider_pos + slider_size + ( BUTTON_SIZE * wm->char_y), 
	           win->x_size * wm->char_x, 
		   ((win->y_size - BUTTON_SIZE) * wm->char_y) - (slider_pos+slider_size+(BUTTON_SIZE*wm->char_y)), 
		   wm->colors[6], wm );
    }
    else
    { //Horisontal:
	pdraw_box( win_num, BUTTON_SIZE * wm->char_x, 0,
	           slider_pos, win->y_size * wm->char_y, 
		   wm->colors[6], wm );
	pdraw_box( win_num, slider_pos + ( BUTTON_SIZE * wm->char_x ), 0,
		   slider_size, win->y_size * wm->char_y, 
		   wm->colors[13], wm );
	pdraw_box( win_num, slider_pos + slider_size + ( BUTTON_SIZE * wm->char_x), 0,
		   ((win->x_size - BUTTON_SIZE) * wm->char_x) - (slider_pos+slider_size+(BUTTON_SIZE*wm->char_x)), 
	           win->y_size * wm->char_y, 
		   wm->colors[6], wm );
    }
    
    if( data->show_value_flag )
    { //Show value:
	char temp[ 16 ];
	if( data->show_value_type == 1 )
	{ //Dec:
	    int_to_string( data->cur + data->show_value_offset, temp );
	}
	else
	{ //Hex:
	    int_to_string_h( data->cur + data->show_value_offset, temp );
	}
	long str_size = 0;
	for( ; str_size < 16 ; ) if( temp[ str_size++ ] == 0 ) break; 
	str_size *= wm->char_x; str_size >>= 1; 
	draw_string( win_num, ( ( win->x_size * wm->char_x ) >> 1 ) - str_size, ( ( win->y_size * wm->char_y ) - wm->char_y ) >> 1, 0, temp, wm );
    }
}

void scrollbar_set_parameters( long win_num, long type, long max, long cur, long slider_size_items, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;
    
    if( max < 0 ) max = 0;
    
    data->type = (char)type;
    data->max = max;
    data->cur = cur;
    data->slider_size_items = slider_size_items;
}

void scrollbar_set_cur_value( long win_num, long cur, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;
    
    data->cur = cur;
    scrollbar_check( win_num, wm );
}

void scrollbar_set_step( long win_num, long step, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;
    
    data->step = step;
}

void scrollbar_set_value_showing( long win_num, char show_value_flag, char show_value_type, long show_value_offset, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;
    
    data->show_value_flag = show_value_flag;
    data->show_value_type = show_value_type;
    data->show_value_offset = show_value_offset;
}

void scrollbar_set_handler( long (*handler)(void*,long,window_manager*), void *user_data, long win_num, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;

    data->handler = (long (*)(void*,long,void*))handler;
    data->user_data = user_data;
}

long up_handler( void *user_data, long button_win, window_manager *wm )
{
    scrollbar_data *data = (scrollbar_data*)user_data;
    
    data->cur += data->step;
    scrollbar_check( data->this_window, wm );
    scrollbar_draw( data->this_window, wm );
    
    //User defined handler:
    if( data->handler ) data->handler( data->user_data, data->this_window, (void*)wm );
    
    return 0;
}

long down_handler( void *user_data, long button_win, window_manager *wm )
{
    scrollbar_data *data = (scrollbar_data*)user_data;
    
    data->cur -= data->step;
    scrollbar_check( data->this_window, wm );
    scrollbar_draw( data->this_window, wm );
    
    //User defined handler:
    if( data->handler ) data->handler( data->user_data, data->this_window, (void*)wm );

    return 0;
}

long scrollbar_handler(event *evt, window_manager *wm)
{
    window *win = wm->windows[ evt->event_win ]; //Our window
    scrollbar_data *data = (scrollbar_data*)win->data;
    
    switch( evt->event_type ) 
    {
	case EVT_AFTERCREATE:
	    win->data = mem_new( HEAP_DYNAMIC, sizeof(scrollbar_data), "button data", evt->event_win );
	    //Init data:
	    data = (scrollbar_data*)win->data;
	    data->this_window = evt->event_win;
	    data->max = 0;
	    data->cur = 0;
	    data->slider_size_items = 0;
	    data->slider_size = 0;
	    data->pressed = 0;
	    data->we_are_in_slider = 0;
	    data->handler = 0;
	    data->user_data = 0;
	    data->step = 4;
	    data->show_value_flag = 0;
	    data->show_value_type = 0;
	    data->show_value_offset = 0;
	    //if( win->x_size < win->y_size ) data->type = 0; else data->type = 1;
	    data->type = NEW_SCROLL_TYPE;
	    //Create buttons:
	    if( data->type == 0 )
	    { //Vertical:
		data->button_down = create_window( "vscrolldown",
		                                   0, 0, win->x_size, BUTTON_SIZE, wm->colors[9], 1,
		                                   evt->event_win, &button_handler, wm );
		set_window_string_controls( data->button_down, "0", "0", "100%", BUTTON_SIZE_STR, wm );
		button_set_name( txt_up, data->button_down, wm );
		button_set_autorepeat( 1, data->button_down, wm );
		data->button_up = create_window( "vscrollup",
		                                 0, win->y_size - BUTTON_SIZE, win->x_size, BUTTON_SIZE, wm->colors[9], 1,
		                                 evt->event_win, &button_handler, wm );
		set_window_string_controls( data->button_up, "0", "100%-"BUTTON_SIZE_STR, "100%", "100%", wm );
		button_set_name( txt_down, data->button_up, wm );
		button_set_autorepeat( 1, data->button_up, wm );
	    }
	    else
	    { //Horisontal:
		data->button_up = create_window( "hscrollup",
		                                 win->x_size - BUTTON_SIZE, 0, BUTTON_SIZE, win->y_size, wm->colors[9], 1,
		                                 evt->event_win, &button_handler, wm );
		set_window_string_controls( data->button_up, "100%-"BUTTON_SIZE_STR, "0", "100%", "100%", wm );
		button_set_name( txt_right, data->button_up, wm );
		button_set_autorepeat( 1, data->button_up, wm );
		data->button_down = create_window( "hscrolldown",
		                                   0, 0, BUTTON_SIZE, win->y_size, wm->colors[9], 1,
		                                   evt->event_win, &button_handler, wm );
		set_window_string_controls( data->button_down, "0", "0", BUTTON_SIZE_STR, "100%", wm );
		button_set_name( txt_left, data->button_down, wm );
		button_set_autorepeat( 1, data->button_down, wm );
	    }
	    button_set_handler( &up_handler, (void*)data, data->button_up, wm );
	    button_set_handler( &down_handler, (void*)data, data->button_down, wm );
	    NEW_SCROLL_TYPE = 0;
	    break;
	case EVT_BEFORECLOSE:
	    if( win->data ) mem_free( win->data );
	    break;
	case EVT_SHOW: 
	    //Show window:
	    win->visible = 1; //Make it visible
	    send_event(evt->event_win, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm); //Send DRAW event to all childs
	    break;
	case EVT_HIDE:
	    win->visible = 0;
	    break;
	case EVT_DRAW: 
	    //Draw window (if it's visible)
	    if( win->visible )
	    {
		//draw_window_touch_area( evt->event_win, wm );
		draw_touch_box( win->parent_win, win->x, win->y, win->x_size, win->y_size, evt->event_win, wm );
		draw_window_box( evt->event_win, wm ); //draw window box
		scrollbar_draw( evt->event_win, wm );  //draw scrollbar
	    }
	    break;
	case EVT_REDRAW: 
	    //Redraw window (if it's visible)
	    if( win->visible )
	    {
		draw_window_box( evt->event_win, wm ); //draw window box
		scrollbar_draw( evt->event_win, wm );  //draw scrollbar
	    }
	    break;
	case EVT_BUTTONDOWN:
	    if( evt->button & BUTTON_LEFT )
	    {
		if( data->type == 0 )
		{ //Vertical:
		    if( evt->y < data->slider_pos + (BUTTON_SIZE*wm->char_y) )  
			data->cur -= data->slider_size_items; //Page down
		    else
		    	if( evt->y > data->slider_pos + (BUTTON_SIZE*wm->char_y) + data->slider_size - 1 )  
		    	    data->cur += data->slider_size_items; //Page up
			else
			{
			    data->we_are_in_slider = 1;
			    data->start_x = evt->x;
			    data->start_y = evt->y;
			    data->start_value = data->cur;
			}
		}
		else
		{ //Horisontal:
		    if( evt->x < data->slider_pos + (BUTTON_SIZE*wm->char_x) )
			data->cur -= data->slider_size_items; //Page left
		    else
			if( evt->x > data->slider_pos + (BUTTON_SIZE*wm->char_x) + data->slider_size - 1 )
			    data->cur += data->slider_size_items; //Page right
			else
			{
			    data->we_are_in_slider = 1;
			    data->start_x = evt->x;
			    data->start_y = evt->y;
			    data->start_value = data->cur;
			}
		}
		scrollbar_check( evt->event_win, wm );
		data->pressed = 1;
		scrollbar_draw( evt->event_win, wm );  //draw scrollbar

		//User defined handler:
		if( data->handler ) data->handler( data->user_data, evt->event_win, (void*)wm );
	    }
	    break;
	case EVT_MOUSEMOVE:
	    if( data->pressed && data->we_are_in_slider )
	    {
		if( data->type == 0 )
		{ //Vertical:
		    data->cur = data->start_value;
		    data->cur += ( (evt->y - data->start_y) * data->one_pixel_size ) >> 10;
		    scrollbar_check( evt->event_win, wm );
		    scrollbar_draw( evt->event_win, wm );  //draw scrollbar
		}
		else 
		{ //Horisontal:
		    data->cur = data->start_value;
		    data->cur += ( (evt->x - data->start_x) * data->one_pixel_size ) >> 10;
		    scrollbar_check( evt->event_win, wm );
		    scrollbar_draw( evt->event_win, wm );  //draw scrollbar
		}

		//User defined handler:
		if( data->handler ) data->handler( data->user_data, evt->event_win, (void*)wm );
	    }
	    break;
	case EVT_BUTTONUP:
	    if( evt->button & BUTTON_LEFT && data->pressed )
	    {
		data->pressed = 0;
		data->we_are_in_slider = 0;

		//User defined handler:
		if( data->handler ) data->handler( data->user_data, evt->event_win, (void*)wm );
	    }
	    break;
    }
    return 0;
}
