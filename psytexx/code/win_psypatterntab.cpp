/*
    PsyTexx: win_psypatterntab.cpp. Pattern-functions window handler (add, del, inc, dec ...)
    Copyright (C) 2002 - 2005  Zolotov Alexandr

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

#include "../win_psypattern.h"
#include "../win_psypatterntab.h"
#include "../win_button.h"
#include "../win_text.h"
#include "../win_main.h"
#include "xm/xm.h"

long button_inc_handler(void *user_data, long text_win, window_manager *wm)
{
    patterntab_data *data = (patterntab_data*)user_data;
    module *song = xm.song;
    uchar patnum = song->patterntable[ xm.tablepos ];
    patnum++;
    if( patnum >= song->patterns_num )
    { //Increment number of patterns in the song:
	song->patterns_num = (uint16)patnum + 1;
    }
    pattern *pat = song->patterns[ patnum ];
    int need_new_pat = 0;
    if( pat == 0 )
    {
	need_new_pat = 1;
    }
    else
    {
	if( pat->pattern_data == 0 )
	{
	    need_new_pat = 1;
	    mem_off();
	    clear_pattern( patnum, &xm );
	    mem_on();
	}
    }
    if( need_new_pat )
    { //Create new pattern:
	mem_off();
	new_pattern( patnum, 64, song->channels, &xm );
	clean_pattern( patnum, &xm );
	mem_on();
    }
    //Save pattern num:
    song->patterntable[ xm.tablepos ] = patnum;
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen
    return 0;
}

long button_dec_handler(void *user_data, long text_win, window_manager *wm)
{
    patterntab_data *data = (patterntab_data*)user_data;
    module *song = xm.song;
    uchar patnum = song->patterntable[ xm.tablepos ];
    patnum--;
    if( patnum >= song->patterns_num )
    { //Increment number of patterns in the song:
	song->patterns_num = (uint16)patnum + 1;
    }
    pattern *pat = song->patterns[ patnum ];
    int need_new_pat = 0;
    if( pat == 0 )
    {
	need_new_pat = 1;
    }
    else
    {
	if( pat->pattern_data == 0 )
	{
	    need_new_pat = 1;
	    mem_off();
	    clear_pattern( patnum, &xm );
	    mem_on();
	}
    }
    if( need_new_pat )
    { //Create new pattern:
	mem_off();
	new_pattern( patnum, 64, song->channels, &xm );
	clean_pattern( patnum, &xm );
	mem_on();
    }
    //Save pattern num:
    song->patterntable[ xm.tablepos ] = patnum;
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen
    return 0;
}

long button_ins_handler(void *user_data, long text_win, window_manager *wm)
{
    patterntab_data *data = (patterntab_data*)user_data;
    module *song = xm.song;
    uint16 len = song->length;
    len++;
    if( len > 256 ) len = 256;
    for( int a = len - 1; a > xm.tablepos; a-- )
    {
        song->patterntable[ a ] = song->patterntable[ a - 1 ];
    }
    song->length = len;
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen
    return 0;
}

long button_del_handler2(void *user_data, long text_win, window_manager *wm)
{
    patterntab_data *data = (patterntab_data*)user_data;
    module *song = xm.song;
    uint16 len = song->length;
    if( len > 1 )
    {
	for( int a = xm.tablepos; a < len - 1; a++ )
	{
	    song->patterntable[ a ] = song->patterntable[ a + 1 ];
	}
	len--;
	song->length = len;
    }
    if( xm.tablepos >= len )
	xm.tablepos = (long)len - 1;
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen
    return 0;
}

long patterntab_handler(event *evt, window_manager *wm)
{
    window *win = wm->windows[ evt->event_win ]; //Our window
    patterntab_data *data = (patterntab_data*)win->data;
    
    switch( evt->event_type ) 
    {
	case EVT_AFTERCREATE:
	    win->data = mem_new( HEAP_DYNAMIC, sizeof(patterntab_data), "pattern control", evt->event_win );
	    
	    //Init data:
	    data = (patterntab_data*)win->data;
	    
	    data->this_window = evt->event_win;

	    //Create buttons:
	    data->button_dec = create_window( "dec", win->x_size>>1, 3, win->x_size>>1, 2, wm->colors[13], 1, evt->event_win, &button_handler, wm );
	    set_window_string_controls( data->button_dec, "50%", "2", "100%", "4", wm );
	    button_set_name( "-", data->button_dec, wm );
	    button_set_handler( &button_dec_handler, data, data->button_dec, wm );
	    data->button_inc = create_window( "inc", win->x_size>>1, 1, win->x_size>>1, 2, wm->colors[13], 1, evt->event_win, &button_handler, wm );
	    set_window_string_controls( data->button_inc, "50%", "0", "100%", "2", wm );
	    button_set_name( "+", data->button_inc, wm );
	    button_set_handler( &button_inc_handler, data, data->button_inc, wm );
	    data->button_ins = create_window( "ins", 0, 1, win->x_size>>1, 2, wm->colors[9], 1, evt->event_win, &button_handler, wm );
	    set_window_string_controls( data->button_ins, "0", "0", "50%", "2", wm );
	    button_set_name( "INS", data->button_ins, wm );
	    button_set_handler( &button_ins_handler, data, data->button_ins, wm );
	    data->button_del = create_window( "bdel", 0, 3, win->x_size>>1, 2, wm->colors[9], 1, evt->event_win, &button_handler, wm );
	    set_window_string_controls( data->button_del, "0", "2", "50%", "4", wm );
	    button_set_name( "DEL", data->button_del, wm );
	    button_set_handler( &button_del_handler2, data, data->button_del, wm );
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
		draw_window_touch_area( evt->event_win, wm );
		draw_window_box( evt->event_win, wm ); //draw window box
		psypattern_draw_time( evt->event_win, wm ); //Draw time in this window
	    }
	    break;

	case EVT_REDRAW: 
	    //Redraw window (if it's visible)
	    if( win->visible )
	    {
		draw_window_box( evt->event_win, wm ); //draw window box
		psypattern_draw_time( evt->event_win, wm ); //Draw time
	    }
	    break;
    }
    return 0;
}
