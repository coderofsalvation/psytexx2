
//Demo effects modes:
enum {
    DEMO_MODE_INIT = 0,
    DEMO_MODE_BEFORERENDER,
    DEMO_MODE_RENDER,
    DEMO_MODE_CLOSE
};

//Links: INSTRUMENT --> DEMO EFFECT
long (*demo_links[ EFFECTS * 2 ])( long mode, long channel, long link, ulong t1, ulong t2, window_manager *wm, sound_struct *xm ) = { 0 };
float links_par[ EFFECTS ][ 256 ] = { 0 }; //Parameters for each demo effect

//##########################################################################
//SOME POWERFULL FUNCTIONS #################################################
//##########################################################################

long myrand()
{
    static long int seed = 0;
    
    seed *= 13;
    seed ^= 0x55555555;
	    
    return seed;
}

void radial_blur_part( uchar *pic, long xsize, long ysize, char part )
{
    long cx, cy;
    long tx, ty;
    long ptr;
    float accumulator_x[ 1024 ] = { 0 };
    float accumulator_y;
    float coef_x;
    float coef_y;
    float temp;
    float force = 0.5;
    float result;

    //Parts:
    // 0 1
    // 2 3

    long ptr_inc;
    if( part == 0 || part == 2 )
    {
	ptr_inc = -1;
    }
    if( part == 1 || part == 3 )
    {
	ptr_inc = 1;
    }
    
    for( cy = ( ysize / 2 ) - 1, ty = 0; cy >= 0; cy--, ty++ )
    {
	if( part == 2 || part == 3 )
	    ptr = ( ( ysize / 2 ) - ( cy - ( ysize / 2 ) + 1 ) ) * xsize + ( xsize / 2 );
	else
	    ptr = cy * xsize + ( xsize / 2 );
	if( part == 0 || part == 2 ) ptr -= 1;
	for( cx = ( xsize / 2 ) - 1, tx = 0; cx >= 0; cx--, tx++ )
	{
	    //get accumulator values:
	    accumulator_y = accumulator_x[ cx ];
	    accumulator_x[ cx ] = accumulator_x[ cx + 1 ] + (float)pic[ ptr ];
	    //fade accumumlators:
	    force = 0.5;
	    if( !ty || !tx ) force = 0.53;
	    if( cx == cy ) 
	    {
		coef_x = force;
		coef_y = force;
	    }
	    else
	    {
		if( cx < cy )
		{
		    temp = 1.0F - ( (float)( cy - cx ) / ( ( ysize / 2 ) - cx ) );
		    coef_y = force * temp;
		    coef_x = force * ( 1 + ( 1 - temp ) );
		}
		else
		{
		    temp = 1.0F - ( (float)( cx - cy ) / ( ( xsize / 2 ) - cy ) );
		    coef_x = force * temp;
		    coef_y = force * ( 1 + ( 1 - temp ) );
		}
	    }
	    accumulator_y *= coef_y;
	    accumulator_x[ cx ] *= coef_x;
	    //save new accumulators:
	    accumulator_x[ cx ] += accumulator_y;
	    //save result:
	    result = accumulator_x[ cx ] / 5 + (float)pic[ ptr ];
	    if( result > 255 ) result = 255;
	    pic[ ptr ] = (uchar)result;
	    ptr += ptr_inc;
	}
    }
}

void radial_blur( image *img )
{
    radial_blur_part( img->data, img->xsize, img->ysize, 0 );
    radial_blur_part( img->data, img->xsize, img->ysize, 1 );
    radial_blur_part( img->data, img->xsize, img->ysize, 2 );
    radial_blur_part( img->data, img->xsize, img->ysize, 3 );    
}

void draw_textured_rect( float xsize, float ysize, float z )
{
    glTexCoord2f( 0, 0 ); glVertex3f( -xsize, ysize, z );
    glTexCoord2f( 1, 0 ); glVertex3f( xsize, ysize, z );
    glTexCoord2f( 1, 1 ); glVertex3f( xsize, -ysize, z );
    glTexCoord2f( 0, 1 ); glVertex3f( -xsize, -ysize, z );
}

//##########################################################################
//##########################################################################
//##########################################################################

//Radial blur picture scroller:
image *lenta;
image *lenta2;
uchar rand_line[ 2048 ];
long scroller_effect( long mode, long channel, long link, ulong t1, ulong t2, window_manager *wm, sound_struct *xm )
{
    if( mode == DEMO_MODE_INIT )
    {
	lenta = load_jpeg( "data/lenta.jpg", 1 );
	lenta2 = new_image( 512, 512, 1 );
	for( int rp = 0; rp < 2048; rp++ ) rand_line[ rp ] = myrand();
    }
    
    if( mode == DEMO_MODE_CLOSE )
    {
    }

    if( mode == DEMO_MODE_RENDER )
    {
#ifdef OPENGL
	float size = 0.9F;
	float z = -2.0;

	//Scroll:
	float f_y = (float)t1 / time_ticks_per_second();
	f_y *= 20;
	long y = (long)f_y;
	y &= 511;
	long dest_ptr = 0;
	long rand_add = myrand() & 2047;
	for( long cy = 0; cy < 512; cy++ )
	{
	    long ptr = ( ( cy + y ) & 511 ) * 512;
	    long res;
	    rand_add += myrand();
	    for( long cx = 0; cx < 512; cx++ )
	    {
		res = ( lenta->data[ ptr ] * rand_line[ rand_add & 2047 ] ) >> 8;
		//res = lenta->data[ ptr ];
		//if( res > 255 ) res = 255;
		lenta2->data[ dest_ptr ] = res;
		ptr++;
		dest_ptr++;
		rand_add++;
	    }
	}
	radial_blur( lenta2 );
	glBindTexture( GL_TEXTURE_2D, 2 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA8, lenta2->xsize, lenta2->ysize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, lenta2->data );

	glPushMatrix();
	glDisable( GL_DEPTH_TEST );
	glTranslatef( 0.3F, 0, 0 );
	glBegin( GL_POLYGON );
	//glColor4f( 1.0F, 1.0F, 1.0F, 1.0F - ((float)t1 / time_ticks_per_second()) ); 
	//glColor4f( 1.0F, 0.9F, 0.8F, 1.0F );
	glColor4f( 1.0F, 0.8F, 0.0F, 2.0F );
	draw_textured_rect( size + 0.5F, size, z );
        glEnd(); 
	glBegin( GL_POLYGON );
	glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );
	draw_textured_rect( size + 0.5F, size, z );
        glEnd(); 
	glEnable( GL_DEPTH_TEST );
	glPopMatrix();				
#endif
    }
    return 0;
}

//Left border:
image *lborder;
char frame = 0;
long left_border_effect( long mode, long channel, long link, ulong t1, ulong t2, window_manager *wm, sound_struct *xm )
{
    if( mode == DEMO_MODE_INIT )
    {
	lborder = load_jpeg( "data/left_border.jpg", 1 );
	glBindTexture( GL_TEXTURE_2D, 3 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA8, lborder->xsize, lborder->ysize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, lborder->data );
    }

    if( mode == DEMO_MODE_RENDER )
    {
	glBindTexture( GL_TEXTURE_2D, 3 );
	frame++; frame &= 1;
	glBegin( GL_POLYGON );
	//draw_textured_rect( 0.55, 0.5, -1.0 );
        glEnd(); 
    }
    return 0;
}

//Isosphere effect:
#include "demo_models.h"
image *polygon;
long isosphere_effect( long mode, long channel, long link, ulong t1, ulong t2, window_manager *wm, sound_struct *xm )
{
    if( mode == DEMO_MODE_INIT )
    {
	lborder = load_jpeg( "data/polygon.jpg", 1 );
	glBindTexture( GL_TEXTURE_2D, 4 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA8, lborder->xsize, lborder->ysize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, lborder->data );
    }

    if( mode == DEMO_MODE_RENDER )
    {
	float scale = 0.2;
	for( float count = 0; count < 6; count++ )
	{ 
	    long a = 0;
	    for( ;; )
	    {
		if( sphere[ a ] == 555 ) break;
		float x1 = sphere[ a++ ];
		float y1 = sphere[ a++ ];
	        float z1 = sphere[ a++ ];
		float x2 = sphere[ a++ ];
		float y2 = sphere[ a++ ];
		float z2 = sphere[ a++ ];
		float x3 = sphere[ a++ ];
		float y3 = sphere[ a++ ];
		float z3 = sphere[ a++ ];
		glPushMatrix();
		float count2 = count / 60;
		glTranslatef( 0.3F, 0, -2.0F );
		if( count == 5 )
		    glScalef( 5 + count2, 5 + count2, 5 + count2 );
		else
		    glScalef( 1 + count2, 1 + count2, 1 + count2 );
		float r = (float)t1 / time_ticks_per_second();
		r *= 5;
		glRotatef( r, r * 2.4F, r * 3, 80 );
		if( count == 5 )
		{
		    glBindTexture( GL_TEXTURE_2D, 3 );
		    glColor4f( 1.0F, 1.0F, 1.0F, 1 - ( count / 6 ) );
		    glBegin( GL_TRIANGLE_STRIP );
		}
		else
		{
		    glBindTexture( GL_TEXTURE_2D, 4 );
		    glColor4f( sin( r / 2 ) + 1, sin( r / 2 ) + 1, 1, 
		               ( 1 - ( count / 6 ) ) * ( ( sin( r / 2 ) + 1 ) / 2 ) );
		    glBegin( GL_LINE_STRIP );
		}
		glTexCoord2f( 0, 0 ); glVertex3f( x1 * scale, y1 * scale, z1 * scale );
		glTexCoord2f( 0, 1 ); glVertex3f( x2 * scale, y2 * scale, z2 * scale );
		glTexCoord2f( 1, 0 ); glVertex3f( x3 * scale, y3 * scale, z3 * scale );
		glEnd();
		glPopMatrix();				
	    }
	}
    }
    
    return 0;
}

//Open eyes effect:
float open_status = 0;
long open_effect( long mode, long channel, long link, ulong t1, ulong t2, window_manager *wm, sound_struct *xm )
{
    if( mode == DEMO_MODE_INIT )
    {
	open_status = 0;
    }
    
    if( mode == DEMO_MODE_RENDER )
    {
	open_status = (float)t1 / time_ticks_per_second();
	float shade_status = open_status / 2;
	open_status /= 14;
	open_status = 0.4;
	glPushMatrix();
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );
	
	//Up part:
	glColor4f( 0, 0, 0, 1.0F );
	glBegin( GL_POLYGON );
	glVertex3f( -5, open_status, -2 );
	glVertex3f( 5, open_status, -2 );
	glVertex3f( 5, 5, -2 );
	glVertex3f( -5, 5, -2 );
	glEnd();
	//Up line:
	glColor4f( 0.7F, 0.6F, 0.5F, 1.0F );
	glBegin( GL_LINE_STRIP );
	glVertex3f( -5, open_status, -2 );
	glVertex3f( 5, open_status, -2 );
	glEnd();

	//Down part:
	glColor4f( 0, 0, 0, 1.0F );
	glBegin( GL_POLYGON );
	glVertex3f( -5, -open_status, -2 );
	glVertex3f( 5, -open_status, -2 );
	glVertex3f( 5, -5, -2 );
	glVertex3f( -5, -5, -2 );
	glEnd();
	//Down line
	glColor4f( 0.7F, 0.6F, 0.5F, 1.0F );
	glBegin( GL_LINE_STRIP );
	glVertex3f( -5, -open_status, -2 );
	glVertex3f( 5, -open_status, -2 );
	glEnd();
	
	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, 3 );
	glColor4f( 1.0F, 0.9F, 0.9F, 1.0F );
	glBegin( GL_POLYGON );
	glTexCoord2f( 0, shade_status / 8 ); glVertex3f( -1.5, -open_status - 0.01, -2 );
	glTexCoord2f( 1, shade_status / 8 ); glVertex3f( 1.5, -open_status - 0.01, -2 );
	glTexCoord2f( 1, shade_status / 8 ); glVertex3f( 1.5, -open_status - 0.02, -2 );
	glTexCoord2f( 0, shade_status / 8 ); glVertex3f( -1.5, -open_status - 0.02, -2 );
	glEnd();
	glBegin( GL_POLYGON );
	glTexCoord2f( 0, shade_status / 5 ); glVertex3f( -1.5, -open_status - 0.03, -2 );
	glTexCoord2f( 1, shade_status / 5 ); glVertex3f( 1.5, -open_status - 0.03, -2 );
	glTexCoord2f( 1, shade_status / 5 ); glVertex3f( 1.5, -open_status - 0.04, -2 );
	glTexCoord2f( 0, shade_status / 5 ); glVertex3f( -1.5, -open_status - 0.04, -2 );
	glEnd();

	glDisable( GL_TEXTURE_2D );

	//Black fade:
	if( 1.0F - shade_status > 0 )
	{
	    glColor4f( 0, 0, 0, 1.0F - shade_status );
	    glBegin( GL_POLYGON );
	    draw_textured_rect( -2, -2, -1 );
	    glEnd();
	}

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glPopMatrix();				
    }
    
    return 0;
}

//Effects table:
void* effects_table[] = 
{
    (void*)"#SCROLLER", (void*)scroller_effect,
    (void*)"#LBORDER", (void*)left_border_effect,
    (void*)"#ISOSPHERE", (void*)isosphere_effect,
    (void*)"#OPEN", (void*)open_effect,
    0, 0
};
