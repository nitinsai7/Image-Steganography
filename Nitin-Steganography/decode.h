#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    int extn_size;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    int size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

}DecodeInfo;

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *data, char *image_buffer);

Status decode_extn_file_size( DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
