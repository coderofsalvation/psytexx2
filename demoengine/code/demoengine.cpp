#include "../demoengine.h"
#include "../../time/timemanager.h"
#include "../tiny_jpg/jpg.h"
#ifndef PALMOS
    #include "stdlib.h"
    #include "math.h"
#endif

#ifdef DEMOENGINE

long frame_number = 0;
ulong start_time = 0;
ulong demo_start_time = 0;
char effects_exist = 0;
ulong current_time;

#include "demo_effects.h"

void demo_init( window_manager *wm, xm_struct *xm )
{
    char temp[ 23 ];
    temp[ 22 ] = 0;
    int a;
    char *instrname;
    start_clean_images();
    effects_exist = 0;
    for( int l = 0; l < EFFECTS; l++ )
    {
	if( xm->song->instruments[ l ] == 0 ) break; //No such instrument :(
	instrname = (char*)xm->song->instruments[ l ]->name;
	for( a = 0; a < 22; a++ )
	{
	    //Get effect name (f.e.: "#FADE")
	    temp[ a ] = instrname[ a ];
	    if( temp[ a ] == ' ' || temp[ a ] == 0 ) 
	    {
		temp[ a ] = 0;
		break;
	    }
	}
	//Is there effect with this name in the effects_table:
	demo_links[ l ] = 0;
	for( a = 0; ; a += 2 )
	{
	    if( effects_table[ a ] == 0 ) break;
	    if( mem_strcmp( effects_table[ a ], temp ) == 0 ) 
	    { //Effect exist!
	        demo_links[ l ] = ( long (*)( long, long, long, ulong, ulong, window_manager*, xm_struct* ) ) effects_table[ a + 1 ];
		demo_links[ l ] ( DEMO_MODE_INIT, 0, l, 0, 0, wm, xm );
		effects_exist = 1;
	        break;
	    }
	}
    }
#ifdef OPENGL
    //screen_transparency = 0.0F;
    demo_start_time = time_ticks();
#endif
}

void demo_close( window_manager *wm, xm_struct *xm )
{
    effects_exist = 0;
    for( int l = 0; l < EFFECTS; l++ )
    {
	if( demo_links[ l ] )
	{ //Close an effect:
	    demo_links[ l ] ( DEMO_MODE_CLOSE, 0, l, 0, 0, wm, xm );
	    demo_links[ l ] = 0;
	}
    }
    clear_images();
#ifdef OPENGL
    screen_transparency = 1.0F;
    demo_start_time = 0;
#endif
}

void demo_render_frame( window_manager *wm, xm_struct *xm )
{
#ifdef OPENGL
    /* clear color and depth buffers */ 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluPerspective( 45.0f, 1.33, 0.001, 200.0f );
    glMatrixMode( GL_MODELVIEW );
#endif

    //First frame init:
    if( frame_number == 0 ) 
	start_time = time_ticks();

    //Get current time:
    current_time = time_ticks() - start_time;
    
    //Fade out PsyTexx screen:
    if( demo_start_time && effects_exist )
	screen_transparency = 1.0F - ( ( ( (float)time_ticks() - (float)demo_start_time ) / (float)time_ticks_per_second() ) / 2 );
    
    //Scan channels:
    for( int c = 0; c < xm->song->channels; c++ )
    {
	if( xm->channels[ c ]->demo_count != xm->channels[ c ]->prev_demo_count )
	{
	    xm->channels[ c ]->prev_demo_count = xm->channels[ c ]->demo_count;
	    xm->channels[ c ]->demo_start_time = current_time;
	    //First frame init:
	    if( xm->channels[ c ]->demo_inst > 0 && demo_links[ xm->channels[ c ]->demo_inst - 1 ] )
		demo_links[ xm->channels[ c ]->demo_inst - 1 ] 
		( DEMO_MODE_BEFORERENDER, c, xm->channels[ c ]->demo_inst - 1, current_time - xm->channels[ c ]->demo_start_time, current_time, wm, xm );
	}
	//Render effect on a current channel:
	if( xm->channels[ c ]->demo_inst > 0 && demo_links[ xm->channels[ c ]->demo_inst - 1 ] )
	    demo_links[ xm->channels[ c ]->demo_inst - 1 ] 
	    ( DEMO_MODE_RENDER, c, xm->channels[ c ]->demo_inst - 1, current_time - xm->channels[ c ]->demo_start_time, current_time, wm, xm );
    }

    frame_number++;

#ifdef OPENGL
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
#endif
}

#endif

