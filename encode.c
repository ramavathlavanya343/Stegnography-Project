#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

int src_file_size;
char *secret_extn_Str;
int secret_extn_size;
int secret_file_size;

/* Function Definitions */

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
	fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "\nERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*
 *Calling all the functions required to do the encoding
 */


Status do_encoding(EncodeInfo * encInfo)
{
    if(open_files(encInfo)==e_failure)
    {
	printf("\nINFO : Files are not opened succeffuly\n");
	return e_failure;
    }
    printf("\nINFO : Files opened succeffuly \n");

    if(check_capacity(encInfo)==e_failure)
    {
	printf("\nINFO : File capacity is not enough to hide the secret message\n");
	return e_failure;
    }
    printf("\nINFO : File has capacity to hide the secret message and can proceed with the encoding\n");

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
	printf("\nINFO : Unable to copy the file header from source file to destination file\n");
	return e_failure;
    }
    printf("\nINFO : Header copied from source file to destination succesfully\n");

    if(encode_magic_string(MAGIC_STRING, encInfo)==e_failure)
    {
	printf("\nINFO : Unable to encode Magic String\n");
	return e_failure;
    }
    printf("\nINFO : Magic String encoded Successfully\n");

    if(encode_secret_file_size(secret_extn_size, encInfo)==e_failure)
    {
	printf("\nINFO : Unable to encode Secret file size\n");
    }
    printf("\nINFO : Secret file size enocoded successfully\n");

    if(encode_secret_file_extn(secret_extn_Str, encInfo)==e_failure)
    {
	printf("\nINFO : Unable to encode Secret file extention\n");
    } 
    printf("\nINFO : Secret file extention encoded successfully\n");

    if(encode_secret_file_size(secret_file_size, encInfo)==e_failure)
    {
	printf("\nINFO : Unable to encode Secret file data size\n");
    }
    printf("\nINFO : Secret data file size enocoded successfully\n");

    if(encode_secret_file_data(encInfo)==e_failure)
    {
	printf("\nINFO : Unable to encode Secret file data\n");
    }
    printf("\nINFO : Secret file data encoded successfully\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
	printf("\nINFO : Unable to copy the remaining data\n");
    }
    printf("\nINFO : Remaining data of source file encoded successfully\n");

    return e_success;
}

/*
 *finding the size of src file, extension file and secret file
 *check the capacity
 */

Status check_capacity(EncodeInfo * encInfo)
{
    //find the size of src file
    src_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);

    //find the size of extn and store in global variable
    secret_extn_Str = strstr(encInfo->secret_fname,".");

    //find the size of secrete file and store in global variable
    secret_extn_size = strlen(secret_extn_Str);

    fseek(encInfo->fptr_secret,0,SEEK_END);

    secret_file_size = ftell(encInfo->fptr_secret);

    rewind(encInfo->fptr_src_image);
    rewind(encInfo->fptr_secret);

    //check capacity size of src file >= 54 +2*8
    if(src_file_size >= ( 54 + 2*8+32+secret_extn_size*8 + 32 + secret_file_size*8 ))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

/*
 *taking a temporary char array of 54 bytes
 *reading 54 bytes from the srx file and storing in temporary array
 *writing the temporary array to the stego file
 */

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //declaring a temp char array
    char arr[54];

    //read the 54 bytes from src_image
    fread(arr,54,1,fptr_src_image);
    if(fwrite(arr,54,1,fptr_dest_image)==1)
    {
	return e_success;
    }

    return e_failure;
}

/*

 *calling a function to encode data into image
 *return value : e_success or e_failure on failing to encode

 */

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    //call the encode_datato image()
    if(encode_data_to_image(magic_string, 8, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
	return e_failure;
    }
    return e_success;
}

/*

 *taking a temporary array to store the data from the src image
 *fetching 1 byte of data from the magic string
 *encodint the data into the src file data
 *writing the encoded data into the stego file
 *repeat the processs for the length of data times

 */

Status encode_data_to_image(const char * data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //read 8 bytes of data from src image by fread and store in a buffer
    char image_buffer[size];

    for(int i=0; data[i]!='\0'; i++)
    {
	//fetch 1 byte of data from data
	fread(image_buffer,size,1,fptr_src_image);
       
	//encode part

	
	//repeat for length of data times
	encode_byte_to_lsb(data[i], image_buffer) ;
        
	//write the encoded data into stego image usinf fwrite
	if(fwrite(image_buffer,size,1, fptr_stego_image)!=1)
	{
	    return e_failure;
	}
    }
    return e_success;

}



/*

 *clear the lsb bit from the image data
 *get a bit from the char
 *replace the lsb bit with the bit from the data
 *repeat for 8 times

 */

void encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned int temp;
        
    //clear the lsb bit of image data
    //get a bit from ch
    //replace the lsb bits with the bit from data
    //repeate 8 times
    for(int i=0; i<8; i++)
    {
	image_buffer[i]=(image_buffer[i]) & (~1);
	temp = (unsigned) ((data & (1<<(7-i))) >> (7-i));
	image_buffer[i]=image_buffer[i] | temp;
    }
}


/*

 *reading 32 byted of data from the src image and storing in temp array
 *encoding the size into image 
 *writing the encoded data into the stego image
 *return value: e_success or e_failure on failing the encoding

 */


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    //read 32 bytes of data from src file and the store in temp arra[32]
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);

    //write the encoded data into stego image using ferite
    if(fwrite(buffer,32,1, encInfo->fptr_stego_image)!=1)
    {
	return e_failure;
    }
    return e_success;

}


/*
 *clear the lsb bit of array
 *get a bit from the size
 *replace the lsb bit with the bit from the size
 *repeat for 32 time`
 */

void encode_size_to_lsb(int size,char * buffer)
{
    unsigned int temp;

    //clear the lsb bit of arr[]

    //get a bit from size

    //replace the lsb

    //repeate 32 times
    for(int i=0; i<32; i++)
    {
	buffer[i]=(buffer[i]) & (~1);
	temp = (unsigned) (size & (1<<(31-i))) >> (31-i);
	buffer[i]=buffer[i] | temp;

    }

}

/*
 *encoding the extension into the stego image
 *return value: e_success or e_failure on failing to encode
 */

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{    

    if(encode_data_to_image(file_extn, 8, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
	return e_failure;
    }
    return e_success;

}


/*
 *reading the secret message from secret file
 *sroring the secret message in a temp char array
 *encoding the data into the stego image file
 */

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data_buffer[secret_file_size];

    //read the entire data from secrete file and store in temp array
    fread(data_buffer,secret_file_size,1,encInfo->fptr_secret);
    if(encode_data_to_image(data_buffer, 8, encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_failure)
    {
	return e_failure;
    }
    return e_success;
}

/*
 *reading bytes of data from the src image file 
 *writing the data to stego image file 
 *repeat till EOF
 */

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch[8];
    while(fread(ch,1,1,fptr_src)>0)
    {
	fwrite(ch,1,1,fptr_dest);
    }
}




