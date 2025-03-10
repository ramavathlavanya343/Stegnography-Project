/*

Name:Lavanya
Date:23-05-2024
Description:LSB Image Steganography
Sample Input:
Sample Out:

 */

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main()
{

    char ch;

    printf("Please Select the choice to Continue For Encoding or Decoding\nEnter 1 for Encoding 2 for Decoding:");

    scanf(" %c",&ch);	


    EncodeInfo encodeInfo;
    DecodeInfo decodeInfo;


    switch (ch)
    {

	case '1' :

	    encodeInfo.src_image_fname="beautiful.bmp";
	    encodeInfo.secret_fname="secret.txt";
	    encodeInfo.stego_image_fname="stego.bmp";

	    if (do_encoding(&encodeInfo) ==e_success) {
		printf("\nINFO :Encoding Completed Successfully.\n");
	    }
	    else 
	    {
		printf("\nINFO :Encoding Failed.\n");


	    }
	    break;
     
	case '2' :

	    decodeInfo.src_image_fname = "stego.bmp";
	    decodeInfo.output_file_name = "output";

	    if(do_decoding(&decodeInfo)==e_success)
	    {
		printf("\nINFO : Decoding Completed Successfully\n");
	    }
	    else
	    {
		printf("\nINFO : Decoding Failed\n");
	    }
	    break;

    }
}
