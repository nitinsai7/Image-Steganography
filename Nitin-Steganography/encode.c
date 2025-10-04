#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    if(strcmp(strstr(argv[3],"."), ".txt")==0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    if(argv[4]!=NULL)
    {
       if(strstr(argv[4],".") != NULL)
       {
           if(strcmp(strstr(argv[4],".bmp"),".bmp")==0)
           {
               encInfo->stego_image_fname = argv[4];
           }
           else
           {
             return e_failure;
           }
       }
       else
           return e_failure;
    }
    if(argv[4]==NULL)
    {
       encInfo->stego_image_fname = "output.bmp";
    }
    return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    int size =strlen(MAGIC_STRING);
    if(encInfo->image_capacity > (54+(4+4+4+encInfo->size_secret_file+size)*8))
    {
       return e_success;
    }
    else
    {
       return e_failure;
    }
}        
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("files are opened successfully\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("check capacity is done\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                printf("header copied successfully\n");
                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                {
                    printf("magic string encoded succesfully\n");
                    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                    {
                        printf("secret file extn size encoded successfully\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                        {
                            printf("secret file extn encoded successfully\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
                            {
                                printf("sceret file size encoded successfully\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("secret file data encoded successfully\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("remaining image data copied successfully\n");
                                    }
                                    else
                                    {
                                        printf("remaining image data copying failed\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("secret file data encoding failed\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("secret file size encoding failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("secret file extn encoding failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("secret file extn size encoding failed\n");
                        return e_failure;
                    }              
                }
                else
                {
                    printf("magic string encoding failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("copying header failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("check capacity failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("file open failed\n");
        return e_failure;
    }
    return e_success;
}   
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fseek(fptr_src_image,0,SEEK_SET);
    if(fread(buffer,1,54,fptr_src_image) == 54)
    fwrite(buffer,1,54,fptr_dest_image);
    else
    return e_failure;
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    int size=strlen(MAGIC_STRING);
    encode_data_to_image(magic_string,size,encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    return e_success;
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image,EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
    {
        fread(encInfo->image_data,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],encInfo->image_data);
        fwrite(encInfo->image_data,8,1,fptr_stego_image);
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i]&(0xfe))|((data & 1<<7-i)>>(7-i));
    }
   return e_success;
}
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[32];
    fread (str, 32, 1, fptr_src_image);
    if(encode_size_to_lsb(size, str) == e_success)
    fwrite(str, 32, 1, fptr_stego_image);
    else
    return e_failure;
    return e_success;
}
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=(image_buffer[i]&(0xfe))|((size & 1<<31-i)>>(31-i));
    }
    return e_success;
}
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image,encInfo) == e_success)
    return e_success;
    else
    return e_failure;
}
Status encode_secret_file_size(long size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 32, 1, encInfo->fptr_src_image);
    if(encode_size_to_lsb(size, str) == e_success)
    fwrite(str, 32, 1, encInfo->fptr_stego_image);
    else
    return e_failure;
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch=ftell(fptr_src);
    while((fread(&ch,1,1,fptr_src))>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}
