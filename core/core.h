#ifndef __CORE__
#define __CORE__

#define PSYTEXX_VERSION "PsyTexx2 alpha 0.5"
#define PSYTEXX_DATE    __DATE__

#include <cstring>

//#define COLOR8BITS  //8 bit color
//#define COLOR16BITS //16 bit color
//#define COLOR32BITS //32 bit color
//#define LINUX       //OS = Linux
//#define TEXTMODE    //Linux Text Mode
//#define X11         //X11 support
//#define DIRECTX     //DirectDraw support
//#define OPENGL      //OpenGL support
//#define GDI         //GDI support
//#define WIN         //OS = Windows
//#define PALMOS      //OS = PalmOS
//#define SLOWMODE    //Slow mode for slow devices
//#define PALMOS_COMP_MODE //NOT USED ANYMORE
//#define NOSTORAGE   //Do not use the Storage Heap and MemSemaphores
//#define PALMLOWRES  //PalmOS low-density screen
//#define NATIVEARM   //PalmOS5 native ARM code

//#define DEMOENGINE

//Variations:
//WIN32:  COLOR32BITS + WIN [ + OPENGL / GDI ]
//LINUX:  COLOR32BITS + LINUX [ + TEXTMODE / OPENGL / X11 ]
//PALMOS: COLOR8BITS + PALMOS + SLOWMODE [ + NOSTORAGE / PALMLOWRES ]

typedef unsigned char   uchar;
typedef unsigned short  uint16;
typedef short           int16;
typedef unsigned long   ulong;
typedef signed long     slong;

typedef char UTF8_CHAR;
typedef unsigned short UTF16_CHAR;
typedef unsigned long UTF32_CHAR;

#ifdef LINUX
  #define UNIX 1
#endif

#ifdef PALMOS
    //Code sections: (for PalmOS GCC only!!)
    #define sec1 __attribute__ ((section ("sec1")))  /*Window handlers (exclude the table)*/
    #define sec2 __attribute__ ((section ("sec2")))  /*Table window handler*/
    #define sec3 __attribute__ ((section ("sec3")))  /*XM engine */
    #define sec4 __attribute__ ((section ("sec4")))  /*Window manager*/
    #define NATIVEARM
#else
    #define sec1  /*nothing*/
    #define sec2  /*nothing*/
    #define sec3  /*nothing*/
    #define sec4  /*nothing*/
    #define NONPALM
#endif

#endif

