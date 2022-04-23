/*
    PsyTexx: win_popup.cpp. Popup window for dynamic menu
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
//***                      observer_page@mail.ru
//***               WWW: warmplace.ru

#include "../win_main.h"
#include "../win_popup.h"

int popup_opened = 0;

int start_popup_blocked( 
    char *text,
    int x, int y,
    window_manager *wm )
{
    if( popup_opened ) return -1;
    window *win = wm->windows[ win_popup ]; //Our window
    popup_data *data = (popup_data*)win->data;

    send_event( win_popup, EVT_SHOW, 0, 0, 0, 0, MODE_CHILDS, wm );
    //Set focus to this window:
    data->old_focus_window = wm->mouse_win;
    wm->mouse_win = win_popup;
    
    popup_opened = 1;
    data->result = -1;
    data->selected = -1;
    data->text = text;

    //Set window position:
    x /= wm->char_x;
    y /= wm->char_y;

    //Set window size:
    int lines = 1;
    int max_x_size = 1;
    int cur_x_size = 0;
    for( int i = 0; i < mem_strlen( text ); i++ )
    {
	if( text[ i ] == 0xA ) { lines++; cur_x_size = 0; }
	if( text[ i ] != 0xA && text[ i ] != 0xD ) 
	{
	    cur_x_size++;
	    if( cur_x_size > max_x_size ) max_x_size = cur_x_size;
	}
    }
    win->y_size = lines;
    win->x_size = max_x_size + 1;

    //Correct position:
    if( y + win->y_size >= wm->screen_y ) y = wm->screen_y - win->y_size;
    if( x + win->x_size > wm->screen_x ) x = wm->screen_x - win->x_size;
    move_window( win_popup, x, y, wm );

    while( data->result == -1 )
    {
	if( wm->events_count )
	{
	    int ev_num;
	    for( ev_num = 0; ev_num < wm->events_count; ev_num++ )
	    {
		event *evt = wm->events[ ( wm->event_pnt + ev_num ) & MAX_EVENT ];
		if( evt )
		    if( evt->event_win )
			if( evt->event_type == EVT_BUTTONDOWN && ( evt->button & 3 ) )
			    if( evt->event_win != win_popup )
			    {
				//Was a click on another window:
				send_event( win_popup, EVT_HIDE, 0, 0, 0, 0, MODE_CHILDS, wm );
				send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
				//wm->mouse_win = data->old_focus_window;
				data->result = -2;
				//break;
			    }
	    }
	}
	event_loop( wm, 0 );
    }

    popup_opened = 0;
    return data->result;
}

long popup_handler( event *evt, window_manager *wm )
{
    window *win = wm->windows[ evt->event_win ]; //Our window
    popup_data *data = (popup_data*)win->data;
    int handled = 0;
    
    switch( evt->event_type ) 
    {
	case EVT_AFTERCREATE:
	    win->data = mem_new( HEAP_DYNAMIC, sizeof(popup_data), "popup data", evt->event_win );
	    //Init data:
	    data = (popup_data*)win->data;
	    data->this_window = evt->event_win;
	    data->result = -1;
	    data->selected = -1;
	    data->text = "";
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
	    //Draw/redraw window (if it's visible)
	    if( win->visible ) draw_window_touch_area( evt->event_win, wm );
	case EVT_REDRAW: 
	    if( win->visible )
	    {
		pdraw_box( evt->event_win, 0, 0, win->x_size * wm->char_x, win->y_size * wm->char_y, blend( win->color, 0, 20 ), wm );
		//Show selection:
		if( data->selected >= 0 )
		{
		    pdraw_box( evt->event_win, 0, data->selected * wm->char_y, win->x_size * wm->char_x, wm->char_y, get_color( 255, 160, 90 ), wm );
		}
		//Draw text:
		draw_string( evt->event_win, wm->char_x / 2, 0, 1, data->text, wm );
	    }
	    break;

	case EVT_BUTTONDOWN:
	    handled = 1;
	    if( evt->button >> 3 )
	    {
		switch( evt->button >> 3 )
		{
		    case KEY_ESCAPE:
		    send_event( data->this_window, EVT_HIDE, 0, 0, 0, 0, MODE_CHILDS, wm );
		    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
		    wm->mouse_win = data->old_focus_window;
		    data->result = -2;
		    handled = 1;
		    break;
		}
	    }
	    if( evt->button & BUTTON_LEFT )
	    {
		data->selected = evt->y / wm->char_y;
		send_event( evt->event_win, EVT_REDRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
	    }
	    break;

	case EVT_BUTTONUP:
	    handled = 1;
	    if( evt->button & BUTTON_LEFT )
	    {
		if( data->selected >= 0 && data->selected < win->y_size )
		{
		    send_event( data->this_window, EVT_HIDE, 0, 0, 0, 0, MODE_CHILDS, wm );
		    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
		    data->result = data->selected;
		}
	    }
	    break;

	case EVT_MOUSEMOVE:
	    handled = 1;
	    if( evt->button & BUTTON_LEFT )
	    {
		data->selected = evt->y / wm->char_y;
		send_event( evt->event_win, EVT_REDRAW, 0, 0, 0, 0, MODE_CHILDS, wm );
	    }
	    break;
    }
    return handled;
}
