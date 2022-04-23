#include <PalmOS.h>
#include <PenInputMgr.h>
#include <VFSMgr.h>
#include "PceNativeCall.h"

#define ByteSwap16(n) (	((((unsigned long) n) <<8 ) & 0xFF00) | \
			((((unsigned long) n) >>8 ) & 0x00FF) )

#define ByteSwap32(n) (	((((unsigned long) n) << 24 ) & 0xFF000000) | \
			((((unsigned long) n) << 8 ) & 0x00FF0000) | \
			((((unsigned long) n) >> 8 ) & 0x0000FF00) | \
			((((unsigned long) n) >> 24 ) & 0x000000FF) )

struct ARM_INFO
{
    void *GLOBALS;
    void *GOT;
    void *FORMHANDLER;
    long ID;
};

FormPtr gpForm;

Boolean FormHandler( EventPtr event )
{
    Err err;
    Boolean handled = false;
    switch( event->eType )
    {
	case frmOpenEvent:
	    gpForm = FrmGetActiveForm();
	    FrmDrawForm( gpForm );
	    handled = true;
	    break;
	
	case winDisplayChangedEvent:
	    handled = true;
	    break;
	
	case frmCloseEvent:
    	    FrmEraseForm( gpForm );
	    FrmDeleteForm( gpForm );
    	    gpForm = 0;
    	    handled = true;
    	    break;
    }
    return handled;
}

#define memNewChunkFlagNonMovable    0x0200
#define memNewChunkFlagAllowLarge    0x1000
SysAppInfoPtr ai1, ai2, appInfo;
SysAppInfoPtr SysGetAppInfo( SysAppInfoPtr *rootAppPP,
                             SysAppInfoPtr *actionCodeAppPP )
    	                     SYS_TRAP( sysTrapSysUIBusy );

unsigned long our_heap = 0;

void mem_get_info(void)
{
    unsigned long free_bytes, max_chunk;
    MemHeapFreeBytes( 0, &free_bytes, &max_chunk  );
    if( free_bytes < 600 ) our_heap = 1; //We needs the Storage heap
}

void* mem_new(unsigned long size)
{
    unsigned long heap = 0;//our_heap;
    unsigned short ownID = appInfo->memOwnerID;
    return MemChunkNew( MemHeapID( 0, heap ), size, ownID | memNewChunkFlagNonMovable | memNewChunkFlagAllowLarge );
}

void mem_free(void *ptr)
{
    MemPtrFree(ptr);
}

void mem_on(void)
{
    /*
    if( our_heap )
    	MemSemaphoreRelease(1);
    */
}

void mem_off(void)
{
    /*
    if( our_heap )
    	MemSemaphoreReserve(1);
    */
}

void start_arm_code( void )
{
    ARM_INFO arm_info;
    unsigned long *text_section; //Main ARM code
    unsigned long *got_section;  //Global offset table: offset = rel[ got[ var number ] ];
    unsigned char *data_section; //Binary data of the ELF
    unsigned long *rel_section;  //.rel section

    MemHandle mem_handle_r;
    MemHandle mem_handle_d;
    MemHandle mem_handle_g;
    MemHandle mem_handle_c[ 8 ];
    long arm_size[ 8 ];
    long xtra[ 8 ];
    MemHandle mem_handle;

    // GET .REL SECTION SIZE:
    mem_handle_r = DmGetResource( 'armr', 0 );
    long rel_size = MemHandleSize( mem_handle_r );
    long rxtra = 4 - (rel_size & 3); if( rxtra == 4 ) rxtra = 0;

    // GET DATA SECTION SIZE:
    mem_handle_d = DmGetResource( 'armd', 0 );
    long data_size = MemHandleSize( mem_handle_d );
    long dxtra = 4 - (data_size & 3); if( dxtra == 4 ) dxtra = 0;

    // GET GOT SECTION SIZE:
    mem_handle_g = DmGetResource( 'armg', 0 );
    long got_size = MemHandleSize( mem_handle_g );
    long gxtra = 4 - (got_size & 3); if( gxtra == 4 ) gxtra = 0;

    // GET CODE SECTIONS:
    long code_size = 0;
    int cr;
    for( cr = 0; cr < 8; cr++ )
    {
	mem_handle_c[ cr ] = DmGetResource( 'armc', cr );
	if( mem_handle_c[ cr ] )
	{
	    arm_size[ cr ] = MemHandleSize( mem_handle_c[ cr ] );
	    xtra[ cr ] = 4 - ( arm_size[ cr ] & 3 ); if( xtra[ cr ] == 4 ) xtra[ cr ] = 0;
	}
	else 
	{
	    arm_size[ cr ] = 0;
	    xtra[ cr ] = 0;
	}
	code_size += arm_size[ cr ] + xtra[ cr ];
    }
    
    // CREATE ELF BUFFER:
    text_section = (unsigned long*)mem_new( code_size +
					    data_size + dxtra + 
					    rel_size + rxtra + 
					    got_size + gxtra );
    
    // COPY ALL SECTIONS TO THE BUFFER:
    long offset = 0;
    mem_off();
    for( cr = 0; cr < 8; cr++ )
    {
	if( arm_size[ cr ] )
	{
	    MemMove( text_section + offset, MemHandleLock( mem_handle_c[ cr ] ), arm_size[ cr ] );
	    offset += arm_size[ cr ] / 4;
	}
    }
    MemMove( text_section+(code_size/4), MemHandleLock( mem_handle_d ), data_size + dxtra );
    MemMove( text_section+(code_size/4)+((data_size+dxtra)/4), MemHandleLock( mem_handle_r ), rel_size + rxtra );
    MemMove( text_section+(code_size/4)+((data_size+dxtra)/4)+((rel_size+rxtra)/4), MemHandleLock( mem_handle_g ), got_size + gxtra );
    mem_on();

    // CLOSE RESOURCES:
    MemHandleUnlock( mem_handle_r );
    MemHandleUnlock( mem_handle_d );
    MemHandleUnlock( mem_handle_g );
    for( cr = 0; cr < 8; cr++ )
    {
	if( mem_handle_c[ cr ] )
	{
	    MemHandleUnlock( mem_handle_c[ cr ] );
	    DmReleaseResource( mem_handle_c[ cr ] );
	}
    }
    DmReleaseResource( mem_handle_r );
    DmReleaseResource( mem_handle_d );
    DmReleaseResource( mem_handle_g );

    // FIXING GOT and RELOCATION REFERENCES :
    got_section = text_section+(code_size/4)+((data_size+dxtra)/4)+((rel_size+rxtra)/4);
    rel_section = text_section+(code_size/4)+((data_size+dxtra)/4);
    long a, res;
    for( a = 0; a < got_size / 4; a++ )
    {
	res = ByteSwap32( got_section[ a ] ) + (long)text_section;
	got_section[ a ] = ByteSwap32( res );
    }
    for( a = 0; a < rel_size / 4; a++ )
    {
	rel_section[ a ] = ByteSwap32( rel_section[ a ] ) + (unsigned long)text_section;
	rel_section[ a ] = ByteSwap32( rel_section[ a ] );
    }
    unsigned long *real_got = text_section+(code_size/4)+((data_size+dxtra)/4)+((rel_size+rxtra)/4);

    // START ARM CODE :
    WinDrawChars( "SUNDOG ARM STARTER" __DATE__, 24, 10, 0 );
    arm_info.GLOBALS = (void*)ByteSwap32( text_section );
    arm_info.GOT = (void*)ByteSwap32( real_got );
    arm_info.ID = (long)ByteSwap32( appInfo->memOwnerID );
    arm_info.FORMHANDLER = (void*)ByteSwap32( &FormHandler );
    res = PceNativeCall( (NativeFuncType*)text_section, &arm_info );

    // FREE DATA :
    mem_free( text_section );
}

UInt32 PilotMain( UInt16 launchCode, void *cmdPBP, UInt16 launchFlags )
{
    if( launchCode == sysAppLaunchCmdNormalLaunch )
    {
    	appInfo = SysGetAppInfo( &ai1, &ai2 );					

	VFSRegisterDefaultDirectory( ".xm", expMediaType_Any, "/MUSIC/" );
	VFSRegisterDefaultDirectory( ".XM", expMediaType_Any, "/MUSIC/" );
	VFSRegisterDefaultDirectory( ".mod", expMediaType_Any, "/MUSIC/" );
	VFSRegisterDefaultDirectory( ".MOD", expMediaType_Any, "/MUSIC/" );
	VFSRegisterDefaultDirectory( ".wav", expMediaType_Any, "/MUSIC/" );
	VFSRegisterDefaultDirectory( ".WAV", expMediaType_Any, "/MUSIC/" );

	start_arm_code();
    }
    return 0;
}


