/*
    PsyTexx: win_psypattern.cpp. Pattern-properties window handler (size, channels ...)
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
#include "../win_button.h"
#include "../win_text.h"
#include "../win_main.h"
#include "../win_psychannels.h"
#include "memory/memory.h"
#include "time/timemanager.h"
#include "xm/xm.h"

long text_bpm;
long text_speed;
long text_patsize;
long text_add;
long text_channels;

ulong psytexx_start_time = 0;
ulong play_start_time = 0;
ulong playing_time = 0;

long text_bpm_handler(void *user_data, long text_win, window_manager *wm)
{
    pattern_data *data = (pattern_data*)user_data;

    set_bpm( text_get_value( text_win, wm ), &xm );

    return 0;
}

long text_speed_handler(void *user_data, long text_win, window_manager *wm)
{
    pattern_data *data = (pattern_data*)user_data;

    set_speed( text_get_value( text_win, wm ), &xm );

    return 0;
}

long text_patsize_handler(void *user_data, long text_win, window_manager *wm)
{
    pattern_data *data = (pattern_data*)user_data;

    module *song = xm.song;
    long tablepos = xm.tablepos;
    uint16 patnum = song->patterntable[ tablepos ];
    // Resize pattern:
    mem_off();
    resize_pattern( patnum, (uint16)text_get_value( text_win, wm ), song->channels, &xm );
    mem_on();
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen

    return 0;
}

long text_add_handler(void *user_data, long text_win, window_manager *wm)
{
    pattern_data *data = (pattern_data*)user_data;

    return 0;
}

long text_channels_handler(void *user_data, long text_win, window_manager *wm)
{
    pattern_data *data = (pattern_data*)user_data;

    //New number of channels:
    uint16 channels = (uint16)text_get_value( text_win, wm );

    module *song = xm.song;

    //Resize all patterns:
    for( uint16 a = 0; a < 256; a++ )
    {
	pattern *pat = song->patterns[ a ];
	if( pat )
	{
	    // Resize pattern:
	    mem_off();
	    resize_pattern( a, pat->rows, channels, &xm );	
	    mem_on();
	}
    }
    
    song->channels = channels; //Set new number
    
    send_event( 0, EVT_DRAW, 0, 0, 0, 0, MODE_CHILDS, wm ); //Redraw screen

    return 0;
}

void psypattern_redraw_channels( long win_num, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    pattern_data *data = (pattern_data*)win->data;
    channels_redraw( data->channels, wm );
}

void get_text_time( char *dest, long h, long m, long s )
{
    dest[ 0 ] = 0;
    char temp2[3];
    long val, val10;
    val = val10 = h; val10 = val / 10;
    temp2[ 0 ] = (char)val10 + '0';
    temp2[ 1 ] = (char)val - ( (char)val10 * 10 ) + '0';
    temp2[ 2 ] = 0;
    mem_strcat( dest, temp2 );
    mem_strcat( dest, ":" );
    val = val10 = m; val10 = val / 10;
    temp2[ 0 ] = (char)val10 + '0';
    temp2[ 1 ] = (char)val - ( (char)val10 * 10 ) + '0';
    mem_strcat( dest, temp2 );
    mem_strcat( dest, ":" );
    val = val10 = s; val10 = val / 10;
    temp2[ 0 ] = (char)val10 + '0';
    temp2[ 1 ] = (char)val - ( (char)val10 * 10 ) + '0';
    mem_strcat( dest, temp2 );
}

//Get psytexx working time:
void psypattern_draw_time( long win_num, window_manager *wm )
{
    window *win = wm->windows[ win_num ]; //Our window
    pattern_data *data = (pattern_data*)win->data;

    //Current time:
    char temp2[ 16 ];
    char temp3[ 64 ];
    temp3[ 0 ] = 0;
    get_text_time( temp2, time_hours(), time_minutes(), time_seconds() );
    mem_strcat( temp3, temp2 );
 
    //playing time:
    if( xm.status > 0 )
    {
	playing_time = time_ticks() - play_start_time;
	playing_time /= time_ticks_per_second();
    }
    get_text_time( temp2, playing_time / 3600, ( playing_time / 60 ) % 60, playing_time % 60 );
    mem_strcat( temp3, " " );
    mem_strcat( temp3, temp2 );

    matrix_draw_box( win_num, 0, 5, 44, 1, win->color, wm );
    matrix_draw_string( win_num, 0, 5, ATR_SHADOW, temp3, wm );
}

long pattern_handler(event *evt, window_manager *wm)
{
    window *win = wm->windows[ evt->event_win ]; //Our window
    pattern_data *data = (pattern_data*)win->data;
    
    switch( evt->event_type ) 
    {
	case EVT_AFTERCREATE:
	    win->data = mem_new( HEAP_DYNAMIC, sizeof(pattern_data), "pattern properties", evt->event_win );
	    
	    //Init data:
	    data = (pattern_data*)win->data;
	    
	    data->this_window = evt->event_win;

	    //Create buttons:
	    NEW_TEXT_SIZE = 8;
	    text_bpm = 
	    data->text_bpm = create_window( "bpm", 0, 1, 7, 2, wm->colors[6], 1, evt->event_win, &text_handler, wm );
	    text_set_numerical( data->text_bpm, 1, wm );
	    text_set_caption( data->text_bpm, "BPM", wm );
	    text_set_value( data->text_bpm, 125, wm );
	    text_set_bounds( data->text_bpm, 0, 255, wm );
	    text_set_handler( &text_bpm_handler, (void*)data, data->text_bpm, wm );

	    NEW_TEXT_SIZE = 8;
	    text_speed = 
	    data->text_speed = create_window( "tpl", 0, 2+1, 7, 2, wm->colors[6], 1, evt->event_win, &text_handler, wm );
	    text_set_numerical( data->text_speed, 1, wm );
	    text_set_caption( data->text_speed, "TPL", wm );
	    text_set_value( data->text_speed, 6, wm );
	    text_set_bounds( data->text_speed, 0, 31, wm );
	    text_set_handler( &text_speed_handler, (void*)data, data->text_speed, wm );

	    NEW_TEXT_SIZE = 8;
	    text_patsize = 
	    data->text_patsize = create_window( "psize", 7 + 1, 1, 7, 2, wm->colors[6], 1, evt->event_win, &text_handler, wm );
	    text_set_numerical( data->text_patsize, 1, wm );
	    text_set_caption( data->text_patsize, "PSZ", wm );
	    text_set_value( data->text_patsize, 64, wm );
	    text_set_bounds( data->text_patsize, 1, 512, wm );
	    text_set_handler( &text_patsize_handler, (void*)data, data->text_patsize, wm );

	    NEW_TEXT_SIZE = 8;
	    text_add = 
	    data->text_add = create_window( "add", 7 + 1, 2+1, 7, 2, wm->colors[6], 1, evt->event_win, &text_handler, wm );
	    text_set_numerical( data->text_add, 1, wm );
	    text_set_caption( data->text_add, "ADD", wm );
	    text_set_value( data->text_add, 1, wm );
	    text_set_bounds( data->text_add, 1, 32, wm );
	    text_set_handler( &text_add_handler, (void*)data, data->text_add, wm );

	    NEW_TEXT_SIZE = 8;
	    text_channels = 
	    data->text_channels = create_window( "channels", 7+1+7+1, 1, 6, 2, wm->colors[6], 1, evt->event_win, &text_handler, wm );
	    text_set_numerical( data->text_channels, 1, wm );
	    text_set_caption( data->text_channels, "CHN.", wm );
	    text_set_value( data->text_channels, 8, wm );
	    text_set_bounds( data->text_channels, 1, MAX_CHANNELS, wm );
	    text_set_handler( &text_channels_handler, (void*)data, data->text_channels, wm );

#ifndef NONPALM
	    data->channels = create_window( "scopes", 7+1+7+1+7, 1, 30, 4, wm->colors[15], 1, evt->event_win, &channels_handler, wm );
#else
	    data->channels = create_window( "scopes", 7+1+7+1+7, 1, 40, 4, wm->colors[15], 1, evt->event_win, &channels_handler, wm );
#endif

	    create_scrollarea( evt->event_win, wm );
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
		psypattern_draw_time( evt->event_win, wm );
	    }
	    break;

	case EVT_REDRAW: 
	    //Redraw window (if it's visible)
	    if( win->visible )
	    {
		draw_window_box( evt->event_win, wm ); //draw window box
		psypattern_draw_time( evt->event_win, wm );
	    }
	    break;
    }
    return 0;
}
