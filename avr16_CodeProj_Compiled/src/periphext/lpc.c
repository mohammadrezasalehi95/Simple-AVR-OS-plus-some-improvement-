
/***************************************************************************
 Project:		AVRILOS
 Title:			General purpose LPC bus implementation
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			lpc.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* LPC Bus implementation used for access LPC devices 
* 
***************************************************************************/

#include <avr/io.h>

#include "../includes/types.h"
#include "../includes/settings.h"


extern INT8U v_SysStat;

#define LPC_RD	0x04
#define LPC_WR	0x06
#define LPCLD(v,x) do {v &= 0xF0; v |= x & 0x0F;  } while(0)
#define LPCOUT (outp( inp(c_LPCDDR)  | BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3), c_LPCDDR ) )
#define LPCIN (outp( inp(c_LPCDDR)  &  ~( BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3)) , c_LPCDDR) )

#define	LPCClk	do{ sbi(c_LPCBPORT, b_LCLK); \
					cbi(c_LPCBPORT, b_LCLK); \
					}while(0)

//outp( (inp(c_LPCPORT) & ~( BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3))) | (data & ( BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3))), c_LPCPORT);
#define LPCDOUT(data) do { 					\
						outp( ( inp(c_LPCPORT) & 0xF0 ) | ( (data) & 0x0F ), c_LPCPORT);	\
						LPCClk;				\
					} while(0)
					
#define LPCDIN(data) do { 					\
						data = inp(c_LPCPIN) & ( BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3) );	\
						LPCClk;				\
					} while(0)
					
#ifndef MOD_LPC_ON

void f_ConfigLPC(void) {}
void f_LPC_Write(INT32U addr, INT8U data) {}
INT8U f_LPC_Read(INT32U addr) { return 0; }

#else

void f_ConfigLPC(void) 
{
	LPCOUT;
	outp( inp(c_LPCBDDR) | BV(b_LCLK) | BV(b_LFRAME), c_LPCBDDR);
	outp( inp(c_LPCPORT) | BV(b_LAD0) | BV(b_LAD1) | BV(b_LAD2) | BV(b_LAD3) , c_LPCPORT);
	
}

void f_LPC_Write(INT32U addr, INT8U data) 
{
	INT8U lpc;
	INT8U i;
	
	lpc = inp(c_LPCPORT);
	
	/* LFRAME - START */
	LPCDOUT(0X00);
	LPCDOUT(LPC_WR);
	
	for(i=0; i<8; i++)
	{
		LPCDOUT(addr >> (28-4*i));
	}
	
	LPCDOUT(data);
	LPCDOUT(data >> 4);
	
	/* TAR */
	LPCDOUT(0x0F);
	/* TAR */
	LPCIN;
	LPCDIN(lpc);
	/* SYNC */
	LPCDIN(lpc);
	
	/* TAR */
	LPCOUT;	
	LPCDOUT(0x0F);
	/* TAR */
	LPCDOUT(0x0F);
	
	/* Dummy cycles */
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);
	
}

INT8U f_LPC_Read(INT32U addr) 
{ 
	INT8U lpc;
	INT8U i;
	INT8U data;
	
	lpc = inp(c_LPCPORT);
	
	/* LFRAME - START */
	LPCDOUT(0x00);
	LPCDOUT(LPC_RD);
	
	for(i=0; i<8; i++)
	{
		LPCDOUT(addr >> (28-4*i) );
	}
	
	/* TAR */
	LPCDOUT(0x0F);
	/* TAR */
	LPCIN;
	LPCDIN(lpc);
	
	/* SYNC */
	LPCDIN(lpc);
	
	/* Data */
	LPCDIN(data);
	lpc = data;
	LPCDIN(data);
	data = (data << 4) | lpc;
	
	/* TAR */
	LPCOUT;	
	LPCDOUT(0x0F);
	/* TAR */
	LPCDOUT(0x0F);
	
	/* Dummy cycles */
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);
	LPCDOUT(0x0F);


	return data; 
}

#endif
