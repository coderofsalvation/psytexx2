/*
    SunDog: sound_player.cpp. Main sound playing function
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

#include "../sound.h"

int main_callback( void *userData,
                   long stream,
                   void *_buffer,
                   long frameCount )
{
    //main variables: ============
    signed short *buffer = (signed short*) _buffer;
    long buffer_size = frameCount;
    sound_struct *U = (sound_struct*) userData;
    
    //clear buffer: ==============
    for( int i = 0; i < buffer_size * 2; i += 2 ) { buffer[ i ] = 0; buffer[ i + 1 ] = 0; }
    //============================

    //for stream stop: ===========
    if( U->need_to_stop ) { U->stream_stoped = 1; return 0; }
    //============================
    
    //render piece of sound: =====
    render_piece_of_sound( buffer, buffer_size, U->user_data );
    //============================

    return 0;
}

