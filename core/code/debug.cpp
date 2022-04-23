/*
    SunDog: debug.cpp. Debug functions
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

#include "../core.h"
#include "../../memory/memory.h"
#include "../../window_manager/wmanager.h"
#include "../debug.h"
#include <stdarg.h>

#ifdef NONPALM
    #include <stdio.h>
#else
    #ifdef NATIVEARM
	#include "palm_functions.h"
    #endif
#endif

#define Y_LIMIT 50

int hide_debug_counter = 0;

void hide_debug()
{
    hide_debug_counter++;
}

void show_debug()
{
    if( hide_debug_counter > 0 )
	hide_debug_counter--;
}

long debug_count = 0;
char temp_debug_buf[ 256 ];
char *debug_buf = 0;
int debug_buf_size = 0;
int y = 10;

char *debug_output_file = "log.txt";

void debug_set_output_file( char *filename )
{
    debug_output_file = filename;
}

void debug_reset()
{
#ifdef NONPALM
    remove( debug_output_file );
#endif
}

void sprint( char *dest_str, char *str, ... )
{
    va_list p;
    va_start( p, str );

    int ptr = 0;
    int ptr2 = 0;
    char num_str[ 64 ];
    int len;

    //Make a string:
    for(;;)
    {
	if( str[ ptr ] == 0 ) break;
	if( str[ ptr ] == '%' )
	{
	    if( str[ ptr + 1 ] == 'd' )
	    {
		//Integer value:
		int arg = va_arg( p, int );
		int_to_string( arg, num_str );
		len = mem_strlen( num_str );
		mem_copy( dest_str + ptr2, num_str, len );
		ptr2 += len;
		ptr++;
	    }
	    else
	    if( str[ ptr + 1 ] == 's' )
	    {
		//ASCII string:
		char *arg2 = va_arg( p, char* );
		if( arg2 )
		{
		    len = mem_strlen( arg2 );
		    if( len )
		    {
			mem_copy( dest_str + ptr2, arg2, len );
			ptr2 += len;
		    }
		}
		ptr++;
	    }
	}
	else
	{
	    dest_str[ ptr2 ] = str[ ptr ];
	    ptr2++;
	}
	ptr++;
    }
    dest_str[ ptr2 ] = 0;
    va_end( p );
}

void dprint( char *str, ... )
{
    if( hide_debug_counter ) return;

    va_list p;
    va_start( p, str );
    if( debug_buf_size == 0 )
    {
	debug_buf = temp_debug_buf;
	debug_buf_size = 256;
	debug_buf = (char*)MEM_NEW( HEAP_DYNAMIC, 256 );
    }
    int ptr = 0;
    int ptr2 = 0;
    char num_str[ 64 ];
    int len;

    //Make a number:
    int_to_string( debug_count, num_str );
    len = mem_strlen( num_str );
    mem_copy( debug_buf, num_str, len );
    debug_buf[ len ] = ':';
    debug_buf[ len + 1 ] = ' ';
    ptr2 += len + 2;
    debug_count++;

    //Make a string:
    for(;;)
    {
	if( str[ ptr ] == 0 ) break;
	if( str[ ptr ] == '%' )
	{
	    if( str[ ptr + 1 ] == 'd' )
	    {
		int arg = va_arg( p, int );
		int_to_string( arg, num_str );
		len = mem_strlen( num_str );
		if( ptr2 + len >= debug_buf_size )
		{
		    //Resize debug buffer:
		    debug_buf_size += 256;
		    debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
		}
		mem_copy( debug_buf + ptr2, num_str, len );
		ptr2 += len;
		ptr++;
	    }
	    else
	    if( str[ ptr + 1 ] == 's' )
	    {
		//ASCII string:
		char *arg2 = va_arg( p, char* );
		if( arg2 )
		{
		    len = mem_strlen( arg2 );
		    if( len )
		    {
			if( ptr2 + len >= debug_buf_size )
			{
			    //Resize debug buffer:
			    debug_buf_size += 256;
			    debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
			}
			mem_copy( debug_buf + ptr2, arg2, len );
			ptr2 += len;
		    }
		}
		ptr++;
	    }
	}
	else
	{
	    debug_buf[ ptr2 ] = str[ ptr ];
	    ptr2++;
	    if( ptr2 >= debug_buf_size )
	    {
		//Resize debug buffer:
		debug_buf_size += 256;
		debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
	    }
	}
	ptr++;
    }
    debug_buf[ ptr2 ] = 0;
    va_end( p );

    //Save result:
#ifdef NONPALM
    FILE *f = fopen( debug_output_file, "ab" );
    if( f )
    {
	fprintf( f, "%s", debug_buf );
	fclose( f );
    }
    printf( "%s", debug_buf );
#else
    //PalmOS:
    int a;
    for( a = 0; a < 128; a++ )
    {
	if( debug_buf[ a ] == 0 ) break;
	if( debug_buf[ a ] == 0xA ) break;
	if( debug_buf[ a ] == 0xD ) break;
    }
    WinDrawChars( "                                                                                ", 80, 0, y );
    WinDrawChars( debug_buf, a, 0, y );
    y += 10;
    if( y >= Y_LIMIT ) y = 0;
#endif
}

void debug_close()
{
#ifndef PALMOS
#endif
    if( debug_buf && debug_buf != temp_debug_buf ) 
    {
	mem_free( debug_buf );
	debug_buf = temp_debug_buf;
	debug_buf_size = 256;
    }
}
