#ifndef __DEMOENGINE__
#define __DEMOENGINE__

#include "core/core.h"
#include "window_manager/wmanager.h"
#include "xm/xm.h"

#define EFFECTS 128

void demo_init( window_manager *wm, xm_struct *xm );
void demo_close( window_manager *wm, xm_struct *xm );
void demo_render_frame( window_manager *wm, xm_struct *xm );

#endif

