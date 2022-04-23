#ifndef __DEBUG__
#define __DEBUG__

#include "core.h"

void hide_debug();
void show_debug();

void debug_set_output_file( char *filename );
void debug_reset();
void sprint( char *dest_str, char *str, ... );
void dprint( char *str, ... );
void debug_close();

//Compatibility with old versions of debug.cpp:
#define print( a ) dprint( "%d\n", a )
#define prints( a ) dprint( "%s\n", a )
#define prints2( a, num ) dprint( "%s%d\n", a, num )
#define print2s( a1, a2 ) dprint( "%s%s\n", a1, a2 );

#endif

