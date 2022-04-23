#ifndef __V3NUS_FS__
#define __V3NUS_FS__


//V3NUS file system. Device depended.

#ifdef WIN
    #include "windows.h"
#endif

#ifdef UNIX
    #include "dirent.h" //for file find
    #include "unistd.h" //for current dir getting 
#endif


#define MAX_DISKS 16
#define DISKNAME_SIZE 4
#define MAX_DIR_LENGTH 512
#define MAX_FILE_LENGTH 256
extern long disks;                    //number of disks
extern char disk_names[4*MAX_DISKS];  //disk names
extern char current_dir[MAX_DIR_LENGTH];
void get_disks(void);                 //get info about local disks
char* get_disk_name( long number );   //get name of local disk
long get_current_disk(void);          //get number of the current disk
char* get_current_dir(void);          //get current dir (without disk. example: "mydir/")


#ifndef NONPALM

#include "VFSMgr.h"
//PalmOS functions:
#define MAX_RECORDS 256
#define MAX_F_POINTERS 2
typedef unsigned short FILE;
extern ulong r_size[MAX_F_POINTERS][MAX_RECORDS]; //size of each record in file
extern ulong f_size[MAX_F_POINTERS];              //size of each file (for each file pointer)
extern ulong cur_rec[MAX_F_POINTERS];             //current record number (for each file pointer)
extern long cur_off[MAX_F_POINTERS];              //current record offset (for each file pointer)
extern ulong cur_pnt[MAX_F_POINTERS];             //current file offset (for each file pointer)
extern ulong recs[MAX_F_POINTERS];                //number of records (for each file pointer)
extern DmOpenRef cur_db[MAX_F_POINTERS];          //current DB pointer
extern LocalID ID;                                //local ID for DB
extern uchar fp[MAX_F_POINTERS];                  //for each file pointer: 0-free 1-working
extern uchar *cur_rec_pnt[MAX_F_POINTERS];
extern MemHandle cur_rec_h[MAX_F_POINTERS];
extern FileRef vfs_file[ MAX_F_POINTERS ];
extern uchar write_flag[ MAX_F_POINTERS ];
extern uint16 vfs_volume_numbers[8];
extern uint16 vfs_volumes;

long open_vfs(void); //Return 1 if successful. vfs_volume_number = number of first finded VFS volume (flash-card)
int isBusy( DmOpenRef db, int index );  //Is record busy?
void recordsBusyReset( int f );
void get_records_info(DmOpenRef db,uint16 f_num);
FILE* fopen( const char *filename, const char *filemode );
int fclose( FILE *fpp );
void vfs2buffer( ulong f );
void buffer2vfs( ulong f );
int next_record( ulong f );
void rewind( FILE *fpp );
int getc( FILE *fpp );
ulong get_record_rest( FILE *fpp );
int ftell ( FILE *fpp );
uint16 fseek( FILE *fpp, long offset, long access );
int feof( FILE *fpp );
ulong fread( void *ptr, ulong el_size, ulong elements, FILE *fpp );
ulong fwrite( void *ptr, ulong el_size, ulong elements, FILE *fpp );
void fputc( int val, FILE *fpp );
ulong remove( const char *filename );

#else //PALMOS

//For non-palm devices: =======
#include "stdio.h"
//=============================

#endif //ndef NONPALM

ulong file_size( const char *filename ) sec3;



//FIND FILE FUNCTIONS:

//type in find_struct:
enum {
    TYPE_FILE = 0,
    TYPE_DIR
};

struct find_struct
{ //structure for file searching functions
    char *start_dir; //Example: "c:/mydir/" "d:/"
    char *mask;      //Example: "xm/mod/it" (or NULL for all files)
    
    char name[MAX_FILE_LENGTH];  //Finded file's name
    char type;                   //Finded file's type: 0 - file; 1 - dir

#ifndef NONPALM
    uint16 card_id;
    LocalID db_id;
    DmSearchStateType search_info;
    FileRef dir_ref;        //VFS: reference to the start_dir
    ulong dir_iterator;     //VFS: dir iterator
    FileInfoType file_info; //VFS: file info
#endif
#ifdef WIN
    WIN32_FIND_DATA find_data;
    HANDLE find_handle;
    char win_mask[MAX_FILE_LENGTH];      //Example: "*.xm *.mod *.it"
    char win_start_dir[MAX_DIR_LENGTH];  //Example: "mydir\*.xm"
#endif
#ifdef UNIX
    DIR *dir;
    struct dirent *current_file;
    char new_start_dir[MAX_DIR_LENGTH]; 
#endif
};

int find_first( find_struct* );  //Return values: 0 - no files
int find_next( find_struct* );   //Return values: 0 - no files
void find_close( find_struct* );

void save_string( char *str, uchar num, char *filename ); //save null-terminated string to the file
int load_string( char *str, uchar num, char *filename ); //load null-terminated string from file (or -1 if no such file)
void save_long( long num, char *filename );
long load_long( char *filename ); // -1 if no such file

enum
{
    OPT_SCREENX = 0,
    OPT_SCREENY,
    OPT_SCREENFLIP,
    OPT_SOUNDBUFFER,
    OPT_LAST
};

void read_file_with_options( char *filename );
long get_option( long option ); //Return -1 if no such option

#endif //__V3NUS_FS__
