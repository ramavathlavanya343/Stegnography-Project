#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

/*
 *
 *
 *
 *
 */


typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;

    char *output_file_name;
    FILE *fptr_ouput_file;
    char *output_file_extn;
    int output_file_extn_size;
    int output_file_size;

} DecodeInfo;

/* Decode properties */
Status do_decoding(DecodeInfo *decodeInfo);
 
/* open the files */
Status open_File(DecodeInfo * decodeInfo, char * f_type);

/* decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decodeInfo);

/* decode the extn size*/
Status decode_extn_size(DecodeInfo *decodeInfo);

/* decode data from image*/
char decode_data_from_image(char * buffer);

/* decode size from image */
int decode_size_from_image(char * buffer);

/* decode the extn file */
Status decode_extn(DecodeInfo *decodeInfo);

/* decode file_size */
Status decode_file_size(DecodeInfo *decodeInfo);

/* decode the secret message */
Status decode_secret_message(DecodeInfo * decodeInfo);

#endif

