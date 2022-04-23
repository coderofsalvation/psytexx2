#ifndef __SOUND__
#define __SOUND__

//Main sound header

#include "core/core.h"
#include <unistd.h>

//Structures:

enum 
{
    STATUS_STOP = 0,
    STATUS_PLAY,
};

struct sound_struct
{
    long	status;		    //Current playing status
    long	need_to_stop;	    //Set it to 1 if you want to stop sound stream
    long	stream_stoped;	    //If stream really stoped

    void	*user_data;	    //Data for user defined render_piece_of_sound()
};

//Variables:

extern void *user_sound_data;

#ifdef LINUX
extern int dsp;
#endif

//Functions:

int main_callback( void*, long, void*, long ) sec3;
extern void render_piece_of_sound( signed short *buffer, int buffer_size, void *user_data );

void sound_stream_init(void) sec3;
void sound_stream_play(void) sec3;
void sound_stream_stop(void) sec3;
void sound_stream_close(void) sec3;

#endif
