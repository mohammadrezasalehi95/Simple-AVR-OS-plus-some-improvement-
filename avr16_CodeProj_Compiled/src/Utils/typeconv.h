/*********************************************************************
* Function: INT16U f_HexBin(INT8 *hex_in)
* Convert ASCII Hex to binary number
* Input:
*   hex_in: Input hex string to convert to word
* Output:
*   return value of 16-bits (short int)
**********************************************************************/
INT16U f_HexBin(INT8 *hex_in);

/*********************************************************************
* Function: void f_BinHex(INT8U bin_in, INT8 *str_out)
* Convert binary number to ASCII Hex
* Input:
*   bin_in: input byte to convert to string ASCII
* Output:
*   str_out: pointer to output string. Outputs two chars.
**********************************************************************/
void f_BinHex(INT8U bin_in, INT8 *str_out);

/*********************************************************************
* Function: INT8U f_HexBinByte(INT8 *hex_in)
* Convert ASCII Hex to binary number
* Input:
*   hex_in: Input hex string to convert to byte
* Output:
*   return value of 8-bits (byte)
**********************************************************************/
INT8U f_HexBinByte(INT8 *hex_in);
