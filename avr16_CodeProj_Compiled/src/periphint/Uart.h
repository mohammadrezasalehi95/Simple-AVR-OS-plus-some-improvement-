

#ifndef UART_H
#define UART_H

#define ESC            0x1b


/*********************************************************************
* Function: f_ConfigSerial
* Configure UART Hardware and setup FIFO
* Input:
*   None
* Output:
*   None
**********************************************************************/
extern void f_ConfigSerial(void);

/* Send a 'clear screen' to a VT100 terminal */
extern void f_Uart_ClrScr(void);
/* Send a 'new line' */
extern void f_Uart_NL(void);

/*********************************************************************
* Function: f_Uart_PutChar
* Put Char to Tx FIFO
* Input:
*   Character to transmit
* Output:
*   1: if success
*   0: if Tx FIFO full
**********************************************************************/
extern bool f_Uart_PutChar(INT8 c);

/*********************************************************************
* Function: f_Uart_GetChar
* Get Char from Rx FIFO
* Input:
*   None
* Output:
*   If Rx FIFO empty 0xFFFF (-1)
*   else result is 0x00CC where CC character received
**********************************************************************/
extern INT16  f_Uart_GetChar(void);

/*********************************************************************
* Function: f_Uart_PutStr
* Put string to Tx FIFO
* Input:
*   Null terminated String to send
* Output:
*   1: success (always)
* If string length is larger than FIFO this routine blocks and waits
* until Tx FIFO is empty again. In order to avoid such delays TxFIFO 
* should be larger than your larger string
**********************************************************************/
extern bool f_Uart_PutStr(INT8 s[]);

#endif
