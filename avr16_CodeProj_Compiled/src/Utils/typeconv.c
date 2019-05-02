/***************************************************************************
 Project:		AVRILOS
 Title:			Type conversion functions
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			typeconv.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Type conversion used to avoid stdio functions for lower footprint
* 
***************************************************************************/

#include "../includes/types.h"
#include "../includes/settings.h"

/*********************************************************************
* Function: void f_BinHex(INT8U bin_in, INT8 *str_out)
* Convert binary number to ASCII Hex
* Input:
*   bin_in: input byte to convert to string ASCII
* Output:
*   str_out: pointer to output string. Outputs two chars.
**********************************************************************/

void f_BinHex(INT8U bin_in, INT8 *str_out)
{
	static INT8 lut_Hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	*(str_out+0) = lut_Hex[(bin_in >> 4) & 0x0f];
	*(str_out+1) = lut_Hex[bin_in & 0x0f];
}

/*********************************************************************
* Function: INT16U f_HexBin(INT8 *hex_in)
* Convert ASCII Hex to binary number
* Input:
*   hex_in: Input hex string to convert to word
* Output:
*   return value of 16-bits (short int)
**********************************************************************/

INT16U f_HexBin(INT8 *hex_in)
{
	INT16U data = 0;
	INT8U i;
	INT8 cbyte;

	for(i=0; i<4; i++)
	{
		cbyte = *(hex_in+i);
		if ( cbyte >= 'A') cbyte = (cbyte - 'A') + 10;
		else cbyte-= '0';
		data |= (cbyte << (4*(3-i)));
	}

	return data;
}

/*********************************************************************
* Function: INT8U f_HexBinByte(INT8 *hex_in)
* Convert ASCII Hex to binary number
* Input:
*   hex_in: Input hex string to convert to byte
* Output:
*   return value of 8-bits (byte)
**********************************************************************/

INT8U f_HexBinByte(INT8 *hex_in)
{
	INT8U data = 0;
	INT8U i;
	INT8 cbyte;

	for(i=0; i<2; i++)
	{
		cbyte = *(hex_in+i);
		if ( cbyte >= 'A') cbyte = (cbyte - 'A') + 10;
		else cbyte-= '0';
		data |= (cbyte << (4*(1-i)));
	}

	return data;
}
