#ifdef DEMOENGINE

#ifndef ___JPG_H
#define ___JPG_H

#include <stdio.h>

typedef struct {
    unsigned char bits[16];
    unsigned char hval[256];
    unsigned char size[256];
    unsigned short code[256];
} jpeg_huffman_table_t;

typedef struct {
    FILE    *file;
    int     width;
    int     height;
    unsigned char *data;
    int     data_precision;
    int     num_components;
    int     restart_interval;
    struct {
        int     id;
        int     h;
        int     v;
        int     t;
        int     td;
        int     ta;
    } component_info[3];
    jpeg_huffman_table_t hac[4];
    jpeg_huffman_table_t hdc[4];
    int     qtable[4][64];
    struct {
        int     ss,se;
        int     ah,al; 
    } scan;
    int     dc[3];
    int     curbit;
    unsigned char   curbyte;
} jpeg_file_desc;

int jpeg_readmarkers();
void jpeg_ycbcr2rgb();
void jpeg_decompress();

struct image
{
    long xsize;
    long ysize;
    unsigned char *data;
};

extern image *images[ 256 ];

image* load_jpeg( char *name, int grayscale );
image* new_image( long xsize, long ysize, int grayscale );
void start_clean_images();
void clear_images();

extern jpeg_file_desc jpeg_file_s;

#endif

#endif
