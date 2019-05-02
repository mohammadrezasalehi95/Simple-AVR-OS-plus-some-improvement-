/***************************************************************************
 Project:		AVRILOS
 Title:			Compact Debugger/Monitor for AVR
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/15/ATMEGA163/103/16/323/164p
 File:			Uart.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*
* DESCRIPTION
* UART Interface for serial communications
***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../includes/types.h"
#include "../includes/settings.h"
#include "uart.h"


#ifndef MOD_UART_ON
void f_ConfigSerial(void) {}
void f_Uart_ClrScr(void) {}
void f_Uart_NL(void) {}
bool f_Uart_PutChar(INT8 c) { return 0; }
INT16  f_Uart_GetChar(void) { return 0; }
bool f_Uart_PutStr(INT8 s[]) { return 0; }

#else


/* uart globals */
volatile INT8U v_uart_txbuf_cnt;
volatile INT8U v_uart_rxbuf_cnt;
INT8 *p_uart_txin, *p_uart_txout;
INT8 *p_uart_rxin, *p_uart_rxout;
INT8 UART_CLR[] = {ESC, '[','H', ESC, '[', '2', 'J',0};
INT8 UART_NL[] = {0x0d,0x0a,0};
INT8 buf_uart_tx[c_TXBUFLEN];
INT8 buf_uart_rx[c_RXBUFLEN];

/* automatically calcuate baud register value */
#define UART_BAUD_SELECT ( (MPUCLK_Hz-16L*UART_BAUD_RATE)/(UART_BAUD_RATE*16L) )		

/*********************************************************************
* Function: f_ConfigSerial
* Configure UART Hardware and setup FIFO
* Input:
*   None
* Output:
*   None
**********************************************************************/
void f_ConfigSerial(void)
/* initialize uart */
{
	
	// enable RxD/TxD and interrupts
	#if c_MCU == c_AT90S8515
		// AT90S8515,8535,ATmega103,etc
		/* set baud rate */
        UBRR = (INT8U) UART_BAUD_SELECT;
		UCR = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);		      
	
	#elif c_MCU == c_ATMEGA163
		/* set baud rate */
		UBRR = (INT8U) UART_BAUD_SELECT;
		UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
		
	#elif (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)
		/* set baud rate */
		UBRRH = (((INT8U)(UART_BAUD_SELECT)) >> 8 ) & 0xff ;
        UBRRL = ((INT8U)(UART_BAUD_SELECT)) & 0xff ;		
		/* enable RxD/TxD, receive complete int */
		UCSRA = 0x00;
		UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
		// Async, No parity, 1 stop, 8 data bits
		/* Frame Format */    
		UCSRC=0x06;
	  
	#elif c_MCU == c_ATMEGA164P
		// For Mega164p (UCSR0B)
		/* set baud rate */
		UBRR0H = (((INT8U)(UART_BAUD_SELECT)) >> 8 ) & 0xff ;
		UBRR0L = ((INT8U)(UART_BAUD_SELECT)) & 0xff ;
		UCSR0A = 0x00;
		
		/* enable RxD/TxD, receive complete int */
		UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
		// Async, No parity, 1 stop, 8 data bits
		/* Frame Format */    
		UCSR0C=0x06;

	#else
	#error "---- ERROR: No known CPU defined!!! ------"
	#endif

    p_uart_txin  = p_uart_txout = buf_uart_tx;
    p_uart_rxin  = p_uart_rxout = buf_uart_rx;
    v_uart_txbuf_cnt = 0;
    v_uart_rxbuf_cnt = 0;
}


#if (c_MCU == c_AT90S8515) || (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
ISR(SIG_UART_DATA)
#elif c_MCU == c_ATMEGA164P
ISR(SIG_USART_DATA)
#else
#error "----- NO SERTX Vector!!! -----"
#endif

/* signal handler for uart data buffer empty interrupt */
{
    if (v_uart_txbuf_cnt > 0)
    {
    	/* write byte to data buffer */
    	#ifdef UDR0
    		UDR0 = *p_uart_txout;
    	#else
    		UDR = *p_uart_txout;
        #endif
        
        if (++p_uart_txout >= buf_uart_tx + c_TXBUFLEN) /* Pointer wrapping */
            p_uart_txout = buf_uart_tx;
        if (--v_uart_txbuf_cnt == 0)             /* if buffer is empty: */
        {
        	/* disable UDRIE int */	
        	#if c_MCU == c_AT90S8515
       		UCR &= ~_BV(UDRIE);
        	#elif (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
       		UCSRB &= ~_BV(UDRIE);
        	#elif c_MCU == c_ATMEGA164P		
			UCSR0B &= ~_BV(UDRIE0);
			#endif 
        }
    }
}


#if (c_MCU == c_AT90S8515) || (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
ISR(SIG_UART_RECV)
#elif c_MCU == c_ATMEGA164P
ISR(SIG_USART_RECV)
#else
#error "----- NO SERRX Vector!!! -----"
#endif

/* signal handler for receive complete interrupt */
{
	/* read byte from receive register */
	#ifdef UDR0
    	*p_uart_rxin = UDR0;
    #else
    	*p_uart_rxin = UDR;
    #endif
        
    v_uart_rxbuf_cnt++;
    if (++p_uart_rxin >= buf_uart_rx + c_RXBUFLEN) /* Pointer wrapping */
        p_uart_rxin = buf_uart_rx;
}

/*********************************************************************
* Function: f_Uart_GetChar
* Get Char from Rx FIFO
* Input:
*   None
* Output:
*   If Rx FIFO empty 0xFFFF (-1)
*   else result is 0x00CC where CC character received
**********************************************************************/
INT16 f_Uart_GetChar(void)
{
    INT8U c;

    if (v_uart_rxbuf_cnt > 0)
    {
        cli();
        v_uart_rxbuf_cnt--;
        c = *p_uart_rxout;              /* get character from buffer */
        if (++p_uart_rxout >= buf_uart_rx + c_RXBUFLEN) /* pointer wrapping */
            p_uart_rxout = buf_uart_rx;
        sei();
        return c;
    }
    else
        return -1;                          /* buffer is empty */
}

/*********************************************************************
* Function: f_Uart_PutChar
* Put Char to Tx FIFO
* Input:
*   Character to transmit
* Output:
*   1: if success
*   0: if Tx FIFO full
**********************************************************************/
bool f_Uart_PutChar(INT8 c)
{
    if (v_uart_txbuf_cnt < c_TXBUFLEN)
    {
        cli();
        v_uart_txbuf_cnt++;
        *p_uart_txin = c;               /* put character into buffer */
        if (++p_uart_txin >= buf_uart_tx + c_TXBUFLEN) /* pointer wrapping */
            p_uart_txin = buf_uart_tx;
        
        /* enable UDRIE int */
        
        #if c_MCU == c_AT90S8515
       	UCR |= _BV(UDRIE);
        #elif (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
       	UCSRB |= _BV(UDRIE);
        #elif c_MCU == c_ATMEGA164P		
        UCSR0B |= _BV(UDRIE0);
        #endif
          
        
        sei();
        return 1;
    }
    else
        return 0;                           /* buffer is full */
}

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
bool f_Uart_PutStr(INT8 s[])
{
    char *c = s;

    while (*c)
        if (f_Uart_PutChar(*c))
            c++;

    return 1;
}

/* Send a 'clear screen' to a VT100 terminal */
void f_Uart_ClrScr(void)
{
    f_Uart_PutStr(UART_CLR);
}

/* Send a 'new line' */
void f_Uart_NL(void)
{
    f_Uart_PutStr(UART_NL);
}

#endif
