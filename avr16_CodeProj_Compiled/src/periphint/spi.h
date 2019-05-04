/****************************************************************************************
;
;		BEGIN AVR SPI FUNCTIONS
;****************************************************************************************
;----------------------------------------------------------------------------------------
; General Purpose SPI
;
; Project: Butterfly
; Device: AVR Internal SPI unit
; Type: Interface
; File: spi.h
; Target: ATmega163
; Version
; V0.01 	25/04/2009		Initial Release
;----------------------------------------------------------------------------------------
****************************************************************************************/

/*******************************************************
	AVR SPI Control (ATMega163)
********************************************************/
#ifndef __SPIFUNC__
#define __SPIFUNC__

#include <avr/io.h>

#define c_CPOL_ON 			(1<<CPOL)
#define c_CPHA_ON 			(1<<CPHA)
#define c_ORDER_LSB2MSB		(1<<DORD)
#define c_MASTER_ON			(1<<MSTR)
#define c_SPI_IRQEN			(1<<SPIE)
#define c_SPI_EN			(1<<SPE)

#define c_SPI_CLK_FclkBy4		0
#define c_SPI_CLK_FclkBy16		1
#define c_SPI_CLK_FclkBy64		2
#define c_SPI_CLK_FclkBy128		3
#define c_SPI_CKx2_ON			1
#define c_SPI_CKx2_OFF		0

#define c_SPI_QTX_FULL			0
#define c_SPI_QTX_OK			1
#define c_SPI_QRX_EMPTY			0
#define c_SPI_QRX_OK			1

void f_Init_SPI(INT8U v_config, INT8U v_CKx2);
void f_SPI_Reset(void);
INT8U f_SPI_TxRx(INT8U v_data);
void f_SPI_Tx(INT8U v_data);
INT8U f_SPI_Rx(void);
void f_SPI_WaitSyncOp(void);
INT8U f_SPI_WaitAsyncOp(void);
INT8U f_SPI_QTx(INT8U v_data);
INT8U f_SPI_QRx(INT8U *p_data);



#endif
