#ifndef __PSYWINFILES__
#define __PSYWINFILES__

#include "core/core.h"
#include "window_manager/wmanager.h"
#include "filesystem/v3nus_fs.h"

//FILE TYPES:
#define MODFILES           "xm/XM/mod/MOD"
#define INSFILES           "xi/XI/wav/WAV"

//PROPERTIES:
#define PROP_XMFILES_DIR   "prop_files_xm_"
#define PROP_INSFILES_DIR  "prop_files_ins_"
#define PROP_XMFILE_NAME   "prop_files_xmname_"
#define PROP_INSFILE_NAME  "prop_files_insname_"

//INTERNAL STRUCTURES:

struct files_data
{
    long this_window;    //this window handler
    long list_window;    //list window handler
    char disk_number;    //current disk number
    char dir_name[MAX_DIR_LENGTH];  //current dir name (example: "mydir/")
    char full_path[MAX_DIR_LENGTH]; //full path (disk name + dir name [+ filename])
    char *current_mask;  
    long button_up1;     //".." button
    long button_up2;     //"*.*" button
    long button_disk[16];
    long button_load;
    long button_save;
    long button_del;
    long button_mods;
    long button_instr;
    long button_close;
    long current_files;  //XM, INSTRUMENTS ...
    long text_filename;  //Text windows:
    long text_file;      // ...
    long text_dirname;   // ...
    long text_dir;       // ...
    
    long (*handler)(void*,long,void*); //User defined "LOAD" handler: 
                                       //handler(void *user_data, long files_window, void *window_manager)
    void *user_data;                   //Data for handler

    long (*handler2)(void*,long,void*);//User defined "SAVE" handler: 
                                       //handler2(void *user_data, long files_window, void *window_manager)
    void *user_data2;                  //Data for handler2
};

//FUNCTIONS:

void files_set_save_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;
void files_set_handler( long (*)(void*,long,window_manager*), void *user_data, long win_num, window_manager* ) sec1;
void files_refresh( long win_num, window_manager* ) sec1;   //Create files list
long files_dir_up( long win_num, window_manager* ) sec1;    //Go to the parent dir ("../" button)
char* files_get_file( long win_num, window_manager* ) sec1; //Get full selected file name or NULL

//HANDLERS:

long button1_handler( void* user_data, long button_win, window_manager* ) sec1;
long button2_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_load_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_save_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_del_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_mods_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_instr_handler( void* user_data, long button_win, window_manager* ) sec1;
long button_close_handler( void* user_data, long button_win, window_manager* ) sec1;
long disk_button_handler( void* user_data, long button_win, window_manager* ) sec1;
long list_select_handler( void* user_data, long list_win, window_manager* ) sec1;

//WINDOW HANDLERS:

long files_handler( event*, window_manager* ) sec1; //File manager

#endif

