/*
    memory.cpp. Multiplatform memory manager
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2009 Alex Zolotov <nightradio@gmail.com>
*/

#include "../../core/core.h"
#include "../../core/debug.h"
#include "../../filesystem/v3nus_fs.h"
#include "../memory.h"

//Special things for PalmOS:
#ifdef PALMOS
    #define memNewChunkFlagNonMovable    0x0200
    #define memNewChunkFlagAllowLarge    0x1000  // this is not in the sdk *g*
    #define USE_FILES			 1
    SysAppInfoPtr ai1, ai2, appInfo;
    unsigned short ownID;
#endif

#ifndef PALMOS
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
#endif

#if defined(WIN) || defined(WINCE)
    #include <windows.h>
#endif

void *dstart = 0;
void *sstart = 0;
void* prev_dblock = 0;  //Previous mem block in dynamic heap
void* prev_sblock = 0;  //Previous mem block in storage heap
int dsize = 0;
int ssize = 0;
int max_dsize = 0;
int max_ssize = 0;
long mem_manager_started = 0;

long get_block_value( void *ptr, long offset )
{
    char *p = (char*)ptr; p += offset;
    long *m = (long*)p;
    return m[ 0 ];
}

void set_block_value( void *ptr, long offset, long value )
{
    char *p = (char*)ptr; p += offset;
    long *m = (long*)p;
    m[ 0 ] = value;
}

int mem_free_all()
{
    int rv = 0;
    
    char *ptr;
    char *next;
    long size;
    if( sstart )
    {
        dprint( "MEMORY CLEANUP (STORAGE)\n" );
	ptr = (char*)sstart;
	for(;;)
	{
	    next = (char*)get_block_value( ptr + MEM_BLOCK_INFO, MEM_NEXT );
	    size = get_block_value( ptr + MEM_BLOCK_INFO, MEM_SIZE );
#ifdef USE_NAMES
	    dprint( "FREE %d %s\n", size, ptr - MEM_NAME );
#else
	    dprint( "FREE %d\n", size );
#endif
	    simple_mem_free( ptr + MEM_BLOCK_INFO );
	    if( next == 0 ) break;
	    ptr = next;
	}
    }
    if( dstart )
    {
        dprint( "MEMORY CLEANUP (DYNAMIC)\n" );
	ptr = (char*)dstart;
	for(;;)
	{
	    next = (char*)get_block_value( ptr + MEM_BLOCK_INFO, MEM_NEXT );
	    size = get_block_value( ptr + MEM_BLOCK_INFO, MEM_SIZE );
#ifdef USE_NAMES
	    dprint( "FREE %d %s\n", size, ptr - MEM_NAME );
#else
	    dprint( "FREE %d\n", size );
#endif
	    simple_mem_free( ptr + MEM_BLOCK_INFO );
	    if( next == 0 ) break;
	    ptr = next;
	}
    }
    dprint( "Max dynamic memory used: %d\n", max_dsize );
    dprint( "Max storage memory used: %d\n", max_ssize );
    dprint( "%d %d\n", dsize, ssize );
    if( dsize || ssize ) rv = 1;
#ifdef USE_FILES
    remove( "mem_storage" );
    remove( "mem_dynamic" );
#endif
    return rv;
}

//Main functions:
void* mem_new( unsigned long heap, unsigned long size, const UTF8_CHAR *name, unsigned long id )
{
    return (void*)malloc( size );
}

void simple_mem_free( void *ptr )
{
    free( ptr );
}

void mem_free( void *ptr )
{
    simple_mem_free( ptr );
}

void mem_set( void *ptr, unsigned long size, unsigned char value )
{
    if( ptr == 0 ) return;
#ifdef PALMOS
    MemSet( ptr, size, value );
#else
    memset( ptr, value, size );
#endif
}

void* mem_resize( void *ptr, int new_size )
{
    if( ptr == 0 ) 
    {
	return MEM_NEW( HEAP_DYNAMIC, new_size );
    }
    mem_off();
    int old_size = mem_get_size( ptr );
#ifdef PALMOS
    //free() + new():
    void *new_mem = mem_new( mem_get_heap( ptr ) | mem_get_flags( ptr ), new_size, "resized block", 0 );
    uchar *c = (uchar*)new_mem;
    if( old_size > new_size )
	mem_copy( new_mem, ptr, new_size );
    else
	mem_copy( new_mem, ptr, old_size );
    mem_free( ptr );
#else
    //realloc():
    int change_prev_block = 0;
    if( prev_dblock == (uchar*)ptr - MEM_BLOCK_INFO ) change_prev_block |= 1;
    if( prev_sblock == (uchar*)ptr - MEM_BLOCK_INFO ) change_prev_block |= 2;
    uchar *c = (uchar*)realloc( (uchar*)ptr - MEM_BLOCK_INFO, new_size + MEM_BLOCK_INFO ) + MEM_BLOCK_INFO;
    if( change_prev_block & 1 ) prev_dblock = (uchar*)c - MEM_BLOCK_INFO;
    if( change_prev_block & 2 ) prev_sblock = (uchar*)c - MEM_BLOCK_INFO;
    void *new_mem = c;
    set_block_value( c, MEM_SIZE, new_size );
    uchar *prev = (uchar*)get_block_value( c, MEM_PREV );
    uchar *next = (uchar*)get_block_value( c, MEM_NEXT );
    int heap = mem_get_heap( c );
    if( heap == HEAP_DYNAMIC && prev == 0 )
    {
	dstart = c - MEM_BLOCK_INFO;
#ifdef USE_FILES
	FILE *f = fopen( "mem_dynamic", "wb" );
	if( f )
	{
	    fwrite( &dstart, 4, 1, f );
	    fclose( f );
	}
#endif
    }
    if( heap == HEAP_STORAGE && prev == 0 )
    {
	sstart = c - MEM_BLOCK_INFO;
#ifdef USE_FILES
	FILE *f = fopen( "mem_storage", "wb" );
	if( f )
	{
	    fwrite( &sstart, 4, 1, f );
	    fclose( f );
	}
#endif
    }
    if( prev != 0 )
    {
	set_block_value( prev + MEM_BLOCK_INFO, MEM_NEXT, (long)c - MEM_BLOCK_INFO );
    }
    if( next != 0 )
    {
	set_block_value( next + MEM_BLOCK_INFO, MEM_PREV, (long)c - MEM_BLOCK_INFO );
    }
    if( heap == HEAP_DYNAMIC ) { dsize += new_size - old_size; if( dsize > max_dsize ) max_dsize = dsize; }
    else
    if( heap == HEAP_STORAGE ) { ssize += new_size - old_size; if( ssize > max_ssize ) max_ssize = ssize; }
#endif
    if( old_size < new_size )
    {
	mem_set( c + old_size, new_size - old_size, 0 );
    }
    mem_on();
    return new_mem;
}

void mem_copy( void *dest, const void *src, unsigned long size )
{
    if( dest == 0 || src == 0 ) return;
#ifdef PALMOS
    MemMove( dest, src, size ); //It's for dinamic heap only!!
#else
    memcpy( dest, src, size );
#endif
}

int mem_cmp( const char *p1, const char *p2, unsigned long size )
{
    if( p1 == 0 || p2 == 0 ) return 0;
#ifdef PALMOS
    return MemCmp( p1, p2, size ); //It's for dinamic heap only!!
#else
    return memcmp( p1, p2, size );
#endif
}

void mem_strcat( char *dest, const char* src )
{
    if( dest == 0 || src == 0 ) return;
#ifdef PALMOS
    StrCat( dest, src );
#else
    strcat( dest, src );
#endif
}

long mem_strcmp( const char *s1, const char *s2 )
{
#ifdef PALMOS
    return StrCompare( s1, s2 );
#else
    return strcmp( s1, s2 );
#endif
}

int mem_strlen( const char *s )
{
    if( s == 0 ) return 0;
    int a;
    for( a = 0;; a++ ) if( s[ a ] == 0 ) break;
    return a;
}

int mem_strlen_utf32( const UTF32_CHAR *s )
{
    if( s == 0 ) return 0;
    int a;
    for( a = 0;; a++ ) if( s[ a ] == 0 ) break;
    return a;
}

char *mem_strdup( const char *s1 )
{
    int len = mem_strlen( s1 );
    char *newstr = (char*)MEM_NEW( HEAP_DYNAMIC, len + 1 );
    mem_copy( newstr, s1, len + 1 );
    return newstr;
}

long mem_get_heap( void *ptr )
{
    if( ptr == 0 ) return 0;
    char *p = (char*)ptr; p += MEM_HEAP;
    long *m = (long*)p;
    return ( m[ 0 ] - 123456 ) & HEAP_MASK;
}

long mem_get_flags( void *ptr )
{
    if( ptr == 0 ) return 0;
    char *p = (char*)ptr; p += MEM_HEAP;
    long *m = (long*)p;
    return ( m[ 0 ] - 123456 ) & (~HEAP_MASK);
}

long mem_get_size( void *ptr )
{
    if( ptr == 0 ) return 0;
    char *p = (char*)ptr; p += MEM_SIZE;
    long *m = (long*)p;
    return m[ 0 ];
}

char *mem_get_name( void *ptr )
{
#ifdef USE_NAMES
    if( ptr == 0 ) return 0;
    char *p = (char*)ptr; p += MEM_NAME;
#else
    char *p = 0;
#endif
    return p;
}

int off_count = 0;
void mem_on(void)
{
#ifdef PALMOS
#ifndef NOSTORAGE
    off_count--;
    if( off_count == 0 )
	MemSemaphoreRelease( 1 );
#endif
#endif
}

void mem_off(void)
{
#ifdef PALMOS
#ifndef NOSTORAGE
    if( off_count == 0 )
	MemSemaphoreReserve( 1 );
    off_count++;
#endif
#endif
}

void mem_palm_normal_mode( void )
{
#ifdef PALMOS
#ifndef NOSTORAGE
    if( off_count > 0 )
    { //At the moment mem protection is off:
	MemSemaphoreRelease( 1 ); //mem protection on
    }
#endif
#endif
}

void mem_palm_our_mode( void )
{
#ifdef PALMOS
#ifndef NOSTORAGE
    if( off_count > 0 )
    {
	MemSemaphoreReserve( 1 ); //mem protection off
    }
#endif
#endif
}

//Posix compatibility for PalmOS devices:
#ifdef PALMOS

void *malloc ( int size )
{
    return MEM_NEW( HEAP_DYNAMIC, size );
}

void *realloc ( void * ptr, int size )
{
    return mem_resize( ptr, size );
}

void free ( void * ptr )
{
    mem_free( ptr );
}

void *memcpy ( void * destination, const void * source, int num )
{
    mem_copy( destination, source, num );
}

int strcmp ( const char * str1, const char * str2 )
{
    return mem_strcmp( str1, str2 );
}

int memcmp ( const char * p1, const char * p2, int size )
{
    return mem_cmp( p1, p2, size );
}

int strlen ( const char * str1 )
{
    return mem_strlen( str1 );
}

#endif

