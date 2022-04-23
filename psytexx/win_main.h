#ifndef __PSYWINMAIN__
#define __PSYWINMAIN__

#include "core/core.h"
#include "core/debug.h"
#include "window_manager/wmanager.h"
#include "xm/xm.h"

//######################################
//## PSYTEXX VARIABLES:               ##
//######################################

extern window_manager wm;     //Window manager
extern xm_struct xm;	      //Main XM sound structure;

extern long win_desktop;      //Desktop window
extern long win_top;
extern long win_files;        //Files window
extern long win_menu;         //PsyTexx main menu
extern long win_patterntable; //Pattern table
extern long win_patterncontrol;
extern long win_patterneditor;//Pattern editor
extern long win_samples;      //Instrument list
extern long win_pattern_prop; //Pattern properties
extern long win_config;       //PsyTexx config window
extern long win_playlist;     //PsyTexx playlist
extern long win_insteditor;   //Instrument editor
extern long win_smpeditor;    //Sample editor
extern long win_net;	      //Sound net with effects and synths
extern long win_keyboard;     //Virtual keyboard
extern long win_dialog;
extern long win_popup;

//######################################
//## PSYTEXX FUNCTIONS:               ##
//######################################

void psy_windows_init( void ) sec1;
void psy_event_loop( void ) sec1;
void psy_windows_close( void ) sec1;
void psy_wm_handler( window_manager *wm ) sec1;

long handler_load_xm( void *user_data, long files_window, window_manager* ) sec1;
long handler_save_xm( void *user_data, long files_window, window_manager* ) sec1;

#endif

