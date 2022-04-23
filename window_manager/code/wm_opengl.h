/*
    PsyTexx: wm_opengl.h. Platform-dependent module : OpenGL functions
    Copyright (C) 2002 - 2005  Zolotov Alexandr

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copyUntitled 1 of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//*** Contact info: Zolotov Alexandr (NightRadio project)
//***               Ekaterinburg. Russia.
//***               Email: nightradio@gmail.com
//***                      observer_page@mail.ru
//***               WWW: warmplace.ru

#ifndef __WINMANAGER_OPENGL__
#define __WINMANAGER_OPENGL__

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

void gl_init( void )
{
    /* set viewing projection */
    glMatrixMode( GL_PROJECTION );
    glFrustum( -0.5F, 0.5F, -0.5F, 0.5F, 0.0F, 3.0F );

    /* position viewer */
    glMatrixMode( GL_MODELVIEW );
    //glTranslatef(0.0F, 0.0F, -2.2F);

    glClearDepth( 1.0f );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 1 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glEnable( GL_COLOR_MATERIAL );
    glEnable( GL_BLEND );
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_LINE_SMOOTH_HINT );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	    
    glPixelTransferi( GL_INDEX_SHIFT, 0 );
    glPixelTransferi( GL_INDEX_OFFSET, 0 );
#ifdef COLOR8BITS
    glPixelTransferi( GL_MAP_COLOR, GL_TRUE );
#endif
}

void gl_resize( void )
{
    /* set viewport to cover the window */
    glViewport( 0, 0, winWidth, winHeight );
}

//#################################
//#################################
//#################################

#endif
