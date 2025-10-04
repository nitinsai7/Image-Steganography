#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
        decInfo->stego_image_fname=argv[2];
        printf("source image stored\n");
    }
    else
    {
        printf("The source file is not .bmp,please pass .bmp file");
        return e_failure;
    }
    if(argv[3] !=NULL)
    {
        if(strcmp(strstr(argv[3],"."),".txt") == 0)
        {
           decInfo->secret_fname=argv[3];
          // Secret file
        }
        else
	return e_failure;
    }
    else
    decInfo->secret_fname="message.txt";
    return e_success;
}
Status open_decode_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image= fopen( decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->stego_image_fname);

    	return e_failure;
    }
        decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
        if (decInfo->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->secret_fname);

            return e_failure;
	}
     // No failure return e_success
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
        printf("Files opening successfull\n");
        if(decode_magic_string(decInfo) == e_success)
        {
            printf("Decoding of magic string successfull\n");
            if(decode_extn_file_size(decInfo) == e_success)
            {
                if(decode_secret_file_extn(decInfo) == e_success)
                {
                    printf("Decoding of extension succesfull\n");
                    if(decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("Decoding of file size succesfull\n");
                        if(decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("Decoding of data succesfull\n");
                            return e_success;
                        }
                        else
                        {
                            printf("ERROR : Decoding failure\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Decoding secret file size failure\n");
                    }
                }
                else
                {
                    printf("Decoding file extension failure\n");
                }

            }
            else
            {
                printf("Decoding extension file size failure\n");
            }
        }
        else
        {
            printf("Magic string decoding failure\n");
        }
    }
    else
    {
        printf("open file failure\n");
    }
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    rewind(decInfo->fptr_stego_image);
    fseek(decInfo->fptr_stego_image,54, SEEK_SET);
    int size=strlen(MAGIC_STRING);
    char magicstring[size];
    char buff[8];
    for(int i=0;i<size;i++)
    {
        char ch=0;
        fread(buff,1,8,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch,buff);
        magicstring[i]=ch;
    }
    magicstring[size]='\0';
    if ( strcmp(MAGIC_STRING,magicstring) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=image_buffer[i] & 0x01;
        *data = *data | (image_buffer[i] << 7-i );
    }
}

Status decode_extn_file_size( DecodeInfo *decInfo)
{
    int data=0;
    char buff[32];
    fread(buff,1,32,decInfo->fptr_stego_image);
    for(int i=0;i<32;i++)
    {
        buff[i]=buff[i] & 1;
        data= data | ( buff[i] << 31-i );
    }
    if( data <= MAX_FILE_SUFFIX)
    {
        decInfo->extn_size=data;
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buff[8];
    for(int i=0;i<decInfo->extn_size;i++)
    {
        char ch=0;
        fread(buff,1,8,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch,buff);
        decInfo->extn_secret_file[i]=ch;
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    int data=0;
    char buff[64];
    fread(buff,1,32,decInfo->fptr_stego_image);
    for(int i=0;i<32;i++)
    {
        buff[i]=buff[i] & 1;
        data= data | ( buff[i] << 31-i );
    }
    decInfo->size_secret_file=data;
    return e_success;
} 
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buff[8];
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        char ch=0;
        fread(buff,1,8,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch,buff);
        fwrite(&ch,1,1,decInfo->fptr_secret);
    }
    return e_success;
}
