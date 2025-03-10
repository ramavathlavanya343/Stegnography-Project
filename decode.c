#include<stdio.h>
#include<string.h>
#include"decode.h"
#include"types.h"
#include"common.h"

char extn[10];
/* Function Definitions */


/*
   Step 1: Open the source file 
   Step 2: Decode the magic String
   Step 3: Compare original Magic string and Decode Magice string
   Step 4: Decode the extn size and extn file
   Step 5: Open the output.txt file
   Step 6: Decode file size and file data
   Step 7: Close the output.txt file

*/

/*
 *Calling all the functions require for decoding secret data
 */

Status do_decoding(DecodeInfo * decodeInfo)
{

    /* open the  src file */
    if(open_File(decodeInfo,"src")==e_failure)
    {
	printf("\nINFO : Unable to open source file\n");
	return e_failure;
    }
     
    printf("\nINFO : Source file opened Successfully\n");
    
    /* check decode the magic string */ 
    if(decode_magic_string(MAGIC_STRING,decodeInfo)==e_failure)
    {
	printf("\nINFO : Failed to decode magic string\n");
	return e_failure;
    }

    printf("\nINFO : Decoded magic string successfully\n");

    if(decode_extn_size(decodeInfo)==e_failure)
    {
	printf("\nINFO : Decoding extention size was unsuccessful\n");
	return e_failure;
    }
    printf("\nINFO : Decoded extention size successfully\n");

    if(decode_extn(decodeInfo)==e_failure)
    {
	printf("\nINFO : Decodign extention was unsuccessful\n");
	return e_failure;
    }
    printf("\nINFO : Decoded extention successfully\n");

    if(open_File(decodeInfo, "out"))
    {
	printf("\nINFO : Unable to open Output file\n");
	return e_failure;

    }
    printf("\nINFO : Output file opened successfully\n");

    if(decode_file_size(decodeInfo)==e_failure)
    {
	printf("\nINFO : Decoding file size was unsuccessful\n");
	return e_failure;
    }
    printf("\nINFO : Decoded file size successfully\n");

    if(decode_secret_message(decodeInfo)==e_failure)
    {
	printf("\nINFO : Decoding secret data was unsuccessful\n");
	return e_failure;
    }
    printf("\nINFO : Decoded secret data successfully\n");

    return e_success;


}

/*
 *opening the src file if the flag variable is 1
 *opening the output file if the flag variable is 0
 */

Status open_File(DecodeInfo * decodeInfo, char * f_type)

{ 
    if(!strcmp("src",f_type))
    {
	//open the src_image file in read mode
	decodeInfo->fptr_src_image=fopen(decodeInfo->src_image_fname,"r");


	//Checking the srror msg
	if( decodeInfo->fptr_src_image == NULL)
	{
	    perror("fopen");
	    //erroe msg
	    fprintf(stderr,"\nERROR: Unable to open file %s\n",decodeInfo->src_image_fname);
	    return e_failure;
	}
	return e_success;
    }
    else if (!strcmp("out",f_type))
    {
     
        
	char str[20];
	strcpy(str, "output");
	strcat(str,extn);



	decodeInfo->fptr_ouput_file = fopen(str,"w");

	if(decodeInfo->fptr_ouput_file == NULL)
	{
	    perror("fopen");
	    fprintf(stderr,"\nERROR: Unable to open file %s\n",decodeInfo->output_file_name);
	    return e_failure;
	}
	return e_success;
    } 
    return e_failure;

}


/*
 *positioning the offset at 54 byte
 *reading 8 bytes of data from the image
 *decode the data from image
 *comparing the data with the magic string 
 *repeat for the length of data times
 */



Status decode_magic_string(const char * data, DecodeInfo * decodeInfo)
{
    //declare a char buffer and store it in 8 bytes
    char buffer[8];
    char ch;

    //check the magic string and original ms is matched or not
    fseek(decodeInfo->fptr_src_image,54,SEEK_CUR);
    for(int i=0; data[i]!=0; i++)
    {
	//read the 8 bytes of data from src_image
	fread(buffer,8,1,decodeInfo->fptr_src_image);

	//decode part
	if (decode_data_from_image(buffer)!=data[i]) //check the magic string are same then only we have proceed
	{
	    printf("\nMagic Stirng not Matched\n");
	    return e_failure;
	}
    }
    printf("\nMagic Stirng Matched\n");
    return e_success;
}


/*
 *initialize a char variable with 0 
 *left shit the char
 *get a bit from the data
 *combine the char variable and data
 *repeat for 8 times
 */

char decode_data_from_image(char * buffer)

{
    char ch=0;
    
    //iterate throgth the loop upto 8 
    for(int i=0; i<8; i++)
    {
	//fetch the 1 byte of data
	ch=ch<<1;

	ch=(ch|(buffer[i]&1)); //get the 1 byte data and clear 
    }
    return ch;
}

/*
 *reading 32 bytes of data from the src image file
 *storing in a temporary array
 *decoding the size from the image and strong in an int vatiable
 */

Status decode_extn_size(DecodeInfo * decodeInfo)
{
    char buffer[32]; //size of the file
    
    //read 32 bytes from src_file
    fread(buffer, 32, 1, decodeInfo->fptr_src_image);

    //decode part
    decodeInfo->output_file_extn_size = decode_size_from_image(buffer);

    return e_success;
}

/*
 *initialize a int variable with 0 
 *left shit the int variable
 *get a bit from the data
 *combine the data with int variable
 *repeat for 32 times
 */



int decode_size_from_image(char * buffer)
{
    int size=0;

    //repeate the 32 times
    for(int i=0; i<32; i++)
    {
	//fetch the 1 byte of data
	size=size<<1;

	//set the byts
	size=(size|(buffer[i]&1)); //get 1 bit and clear the bit 
    }
    return size;
}


/*
 *reading the data from the src image
 *storing the data in temp array
 *decoding the data from the image
 *repeat for extension size times
 */

Status decode_extn(DecodeInfo * decodeInfo)
{
    //find the size of the file
    int size=decodeInfo->output_file_extn_size;
    char buffer[8];
    for(int i=0; i<size; i++)
    {
	//read the from src_image at a time we can read only 1 character
	fread(buffer,8,1,decodeInfo->fptr_src_image);
	extn[i]= decode_data_from_image(buffer);
    }
    extn[size]='\0';
    
    return e_success;
}

/*
 *reading data from the src image
 *storing the data into a temp array
 *decode the size from the image 
 *collect the size in an int variable
 */


Status decode_file_size(DecodeInfo * decodeInfo)
{
    char buffer[32];

    //read the data from ser_image
    fread(buffer, 32, 1, decodeInfo->fptr_src_image);
   //decode part
    decodeInfo->output_file_size = decode_size_from_image(buffer);

    return e_success;
    
}

/*
 *reading data from the src image file 
 *decoding data from the image 
 *storing the data into a char array
 *repeat for size of file times
 *write the data into output file
 */


Status decode_secret_message(DecodeInfo * decodeInfo)
{
    //size of the file
    int size = decodeInfo->output_file_size;


    char buffer[8], msg[100];
    
    for(int i=0; i<size; i++)
    {
	//read the data from src_image
	fread(buffer,8,1,decodeInfo->fptr_src_image);

	msg[i]= decode_data_from_image(buffer);
        
    }
    msg[size]='\0';
    
    //after decoding strore the msg in msg 
    if(fwrite(msg,size,1,decodeInfo->fptr_ouput_file)!=1)
    {
	return e_failure;
    }
    //after completing the  decoding the msg close the file
    fclose(decodeInfo->fptr_ouput_file);
    return e_success;
}



