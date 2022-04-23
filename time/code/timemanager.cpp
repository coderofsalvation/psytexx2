#include "../timemanager.h"

#ifdef LINUX
    #include <time.h>
#endif
#ifndef NONPALM
    #include <PalmOS.h>
#endif
#ifdef WIN
    #include <windows.h>
    #include <time.h>
#endif

ulong time_hours( void )
{
#ifdef LINUX
    //LINUX:
    time_t t;
    time( &t );
    return localtime( &t )->tm_hour;
#endif
#ifndef NONPALM
    //PALM:
    DateTimeType t;
    TimSecondsToDateTime( TimGetSeconds(), &t );
    return t.hour;
#endif
#ifdef WIN
    //WINDOWS:
    time_t t;
    time( &t );
    return localtime( &t )->tm_hour;
#endif
}

ulong time_minutes( void )
{
#ifdef LINUX
    //LINUX:
    time_t t;
    time( &t );
    return localtime( &t )->tm_min;
#endif
#ifndef NONPALM
    //PALM:
    DateTimeType t;
    TimSecondsToDateTime( TimGetSeconds(), &t );
    return t.minute;
#endif
#ifdef WIN
    //WINDOWS:
    time_t t;
    time( &t );
    return localtime( &t )->tm_min;
#endif
}

ulong time_seconds( void )
{
#ifdef LINUX
    //LINUX:
    time_t t;
    time( &t );
    return localtime( &t )->tm_sec;
#endif
#ifndef NONPALM
    //PALM:
    DateTimeType t;
    TimSecondsToDateTime( TimGetSeconds(), &t );
    return t.second;
#endif
#ifdef WIN
    //WINDOWS:
    time_t t;
    time( &t );
    return localtime( &t )->tm_sec;
#endif
}

ulong time_ticks_per_second( void )
{
#ifdef LINUX
    //LINUX:
    return 1000;
#endif
#ifndef NONPALM
    //PALM:
    return SysTicksPerSecond();
#endif
#ifdef WIN
    //WINDOWS:
    return 1000;
#endif
}

ulong time_ticks( void )
{
#ifdef LINUX
    //LINUX:
    time_t tt;
    time( &tt );
    
    timespec t;
    clock_gettime( CLOCK_REALTIME, &t );
    return ( localtime( &tt )->tm_hour * 3600000 ) + 
           ( localtime( &tt )->tm_min * 60000 ) + 
	   ( localtime( &tt )->tm_sec * 1000 ) +
	   ( t.tv_nsec / 1000000 );
#endif
#ifndef NONPALM
    //PALM:
    return TimGetTicks();
#endif
#ifdef WIN
    //WINDOWS:
    return GetTickCount();
#endif
}

