/***************************************************************************
 Project:		AVRILOS
 Title:			General Purpose SPI
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		ATMEGA163
 File:			spi.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* General SPI Interface 
***************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>

#include "../includes/types.h"
#include "../includes/settings.h"

#include "spi.h"

#define c_SPIBUFLEN	16

// Port Config
#define	c_MISO_Input			do{ cbi(c_SPIDDR, b_SPIMISO)  ;}while(0)
#define	c_MOSI_Output			do{ sbi(c_SPIDDR, b_SPIMOSI)  ;}while(0)
#define	c_CLK_Output			do{ sbi(c_SPIDDR, b_SPICLK)  ;}while(0)

#ifndef MOD_SPI_ON

void f_Init_SPI(INT8U v_config, INT8U v_CKx2) {}
void f_SPI_Reset(void) {}
INT8U f_SPI_TxRx(INT8U v_data){}
void f_SPI_Tx(INT8U v_data) {}
INT8U f_SPI_Rx(void) {}
void f_SPI_WaitSyncOp(void) {}
INT8U f_SPI_WaitAsyncOp(void) {}
INT8U f_SPI_QTx(INT8U v_data) {}
INT8U f_SPI_QRx(INT8U *p_data) {}
	
#else

/* uart globals */
volatile struct type_dev_spi {
	INT8U v_txbuf_cnt;
	INT8U v_rxbuf_cnt;
	INT16 v_txwr, v_txrd;
	INT16 v_rxwr, v_rxrd;
	INT8 buf_tx[c_SPIBUFLEN];
	INT8 buf_rx[c_SPIBUFLEN];	
} p_dev_spi;

void f_Init_SPI(INT8U v_config, INT8U v_CKx2)
{
	// configure direction
	c_MISO_Input;
	c_MOSI_Output;
	c_CLK_Output;

	SPSR = v_CKx2;
	(SPCR) =  v_config;	
}

void f_SPI_Reset(void)
{
	p_dev_spi.v_txbuf_cnt = 0;
	p_dev_spi.v_rxbuf_cnt = 0;
	p_dev_spi.v_txwr = 0;	 
	p_dev_spi.v_txrd = 0;
	p_dev_spi.v_rxwr = 0;
	p_dev_spi.v_rxrd = 0;
}

INT8U f_SPI_TxRx(INT8U v_data)
{
	(SPDR) = v_data;
	while( ! ( (SPSR) & (1<<SPIF) ) );	
         return  (SPDR);
}

void f_SPI_Tx(INT8U v_data)
{
	/*  SPI tx byte */	
	(SPDR) = v_data;
}

INT8U f_SPI_Rx(void)
{
	INT8U v_data;
		
	/* Will Clear SPIF */
	v_data = (SPDR);
	
	return v_data;
}

void f_SPI_WaitSyncOp(void)
{
	while( ! ( (SPSR) & (1<<SPIF) ) );	
}

// result 0: Busy, 1: ready
INT8U f_SPI_WaitAsyncOp(void)
{
	if ( !((SPSR) & (1<<SPIF)) )
		return 0;
	else
		return 1;		
}

INT8U f_SPI_QTx(INT8U v_data)
{
	INT8U v_status = c_SPI_QTX_FULL;

	if (p_dev_spi.v_txbuf_cnt < c_SPIBUFLEN)
    {
        cli();
        p_dev_spi.v_txbuf_cnt++;
		sei();
        p_dev_spi.buf_tx[p_dev_spi.v_txwr] = v_data;             /* put character into buffer */
        if (++p_dev_spi.v_txwr >= c_SPIBUFLEN) /* pointer wrapping */
            p_dev_spi.v_txwr = 0;
        
        /* Tx if ready to transmit, otherwise wait the ISR to send */
        if ( !((SPSR) & (1<<SPIF)) )
		{
			(SPDR) = p_dev_spi.buf_tx[p_dev_spi.v_txrd];
			
			if (++p_dev_spi.v_txrd >= c_SPIBUFLEN) /* pointer wrapping */
            p_dev_spi.v_txrd = 0;
			
			cli();
			p_dev_spi.v_txbuf_cnt--;
			sei();
		}
        
        
		v_status = c_SPI_QTX_OK;
    }
    
	return v_status;
}

INT8U f_SPI_QRx(INT8U *p_data)
{
	INT8U v_status = c_SPI_QRX_EMPTY;
	
	if (p_dev_spi.v_rxbuf_cnt > 0)
    {
		
		*p_data = p_dev_spi.buf_rx[p_dev_spi.v_rxrd];
			
		if (++p_dev_spi.v_rxrd >= c_SPIBUFLEN) /* pointer wrapping */
        p_dev_spi.v_rxrd = 0;
		
		cli();		
		p_dev_spi.v_rxbuf_cnt--;
		sei();
		
		v_status = c_SPI_QRX_OK;
    }
	
	return v_status;

}

#if (c_MCU == c_AT90S8515) || (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
ISR(SIG_SPI)
#elif c_MCU == c_ATMEGA164P
ISR(SIG_SPI)
#else
#error "----- NO SPI Vector!!! -----"
#endif

{
	INT8U v_data;
	
	v_data = (SPDR);
	
	if (p_dev_spi.v_txbuf_cnt > 0)
    {
		(SPDR) = p_dev_spi.buf_tx[p_dev_spi.v_txrd];
			
		if (++p_dev_spi.v_txrd >= c_SPIBUFLEN) /* pointer wrapping */
        p_dev_spi.v_txrd = 0;
			
		p_dev_spi.v_txbuf_cnt--;
    }
	
	if (p_dev_spi.v_rxbuf_cnt < c_SPIBUFLEN)
    {
		p_dev_spi.v_rxbuf_cnt++;
		p_dev_spi.buf_rx[p_dev_spi.v_rxwr] = v_data;             /* put character into buffer */
		if (++p_dev_spi.v_rxwr >= c_SPIBUFLEN) /* pointer wrapping */
		p_dev_spi.v_rxwr = 0;
		
	}
}


		
#endif


