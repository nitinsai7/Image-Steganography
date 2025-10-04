#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
// Main function
int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    // Calling check_operation_type()
    if( check_operation_type(argv) == e_encode )      				
     {
      	   printf("You have selected encoding\n");                               
	   sleep(2);
	   if(argc <= 5 && argc >= 4 )
           {   
	    // Calling read_and_validate_encode_args()
           if( read_and_validate_encode_args(argv,&encInfo) == e_success)
	   printf("Read and validation is done\n");
	   else 
	   printf("Read and validation is failed\n");
	   // Calling Encoding function
           if( do_encoding(&encInfo) == e_success )	                                      
           {
             printf("Completed encoding\n"); 
             return e_success;
	   }
	   else
	   {
	   printf("Encoding failed\n");
	   return e_failure;
	   }
	   } 
     }
     // Calling check_operation_type()
     else if( check_operation_type(argv) == e_decode )
     {
            printf("You have selected decoding\n");
	    sleep(2);
	    if(argc <=4 && argc >=3)
	    {	
	    if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	    printf("Read and validation is done\n");
	    else
	    printf("Read and validation failed\n");
	    if( do_decoding(&decInfo) == e_success)
	    {
	     printf("Completed decoding\n");
	     return e_success;
	    }
	    else
	    {
		printf("Decoding failed\n");
		return e_failure;
	    }
	    }
     }
     // Calling check_operation_type()
     else if( check_operation_type(argv) == e_unsupported )
     {
            printf("Pass the corrected argument\nusage : encoding and decoding\n");
	    return e_failure;
     }
}
// check_operation_type() function
OperationType check_operation_type(char *argv[])
{
// Checking the 1st argument null or not
if( argv[1]!=NULL )
{
    // Checking the 1st argument is invalid or not
    if( strcmp(argv[1],"-e") == 0 )
    return e_encode;
    // Return to main function
    else if( strcmp(argv[1],"-d") == 0 )
    return e_decode;
    // Return to main function
    else
    return e_unsupported;
    // Return to main function
}
else
{
      return e_unsupported;
      // Return to main function
}
}

