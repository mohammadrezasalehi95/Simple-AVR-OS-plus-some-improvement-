
/***************************************************************************
 Project:		AVRILOS
 Title:			Xilinx FPGA Configuration (Spartan)
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			xcs_cfg.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Configure XCS05/10 LCA in serial-slave mode using SPI 
* Status in v_StatReg.b_FPGACfg
***************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "cfgfpga.h"

extern INT8U v_StatReg;
extern INT16U lut_cfgfpga[];


#ifndef MOD_FPGAXCS_ON
void f_CfgSPIFPGA(void) {}
#else

void f_CfgSPIFPGA(void)
{
/*****************************************************
	Init Ports to begin (re-)configuration of FPGA
	set CCLK, and Reset (ISP: SCLK-PB7, PB6)
	D/P,DIN (PD6, PD7)
*****************************************************/

	CCLKOutput;
	ResetOutput;
	DinOutput;
	DoneInput;
	SetRst;

/*****************************************************
	Go to init configuration state
	D/P<='0', CCLK, DIN <='1'
*****************************************************/
	SetCCLK;
	SetDin;
/*****************************************************
	 SetUp SPI
	 With 8Mhz Clk, SPI 500KHz
	 No Interrupt, No Double Speed
*****************************************************/
	outp(0, SPSR);
/*****************************************************
 No Interrupt, SPI Enable, LSB first, Master,
 CPOL=1, CPHA=1, Clk/16 0x7d
*****************************************************/
	outp( (1<<SPE) | (1<<DORD) | (1<<MSTR) | (1<<CPOL) | (1<<CPHA) | (1 << SPR1) , SPCR);

/* Program Pulse */
	ClrRst;
	ClrRst;
	ClrRst;
	ClrRst;
/* Prog > 300nS */
	SetRst;

/* Until Init goes High, wait */
	while( !(inp(c_CFGPIN) & (1<<b_Init)) );


/* Configuration memory cleared proceed to next state */

/* Configuration state	   */
/* Byte Transfer 0x83e     */

	INT16U cfglen = c_CFGSIZE+170*2;
	INT16U idx = 0;
	INT8U cfgdata;
	INT8U *p_data;
	
	p_data = (INT8U *) (&lut_cfgfpga[0]);
	
	for(idx=0; idx<cfglen; idx++)	
	{
		//cfgdata = PRG_RDB(&lut_cfgfpga[idx]);
		cfgdata = PRG_RDB((p_data+idx));
		
		/*  SPI tx byte */
		outp(cfgdata, SPDR);
		while(!(inp(SPSR) & (1<<SPIF)));
	}

	/* To Clear SPIF */
	inp(SPDR);

/**************************************************
	Check if  Init went Low, if yes reconfigure
	check for Init = high
	if OK then Configuration Complete!
	Else (Low) Scotty
**************************************************/

/*
	Read REAL Pin State!
	default Pass
*/
	if(!(inp(c_CFGPIN) & (1<<b_Init))) v_StatReg |= 1 << b_FPGACfg;

	CCLKInput;
	//ResetInput;
	DinInput;
	DoneInput;
	// DISABLE SPI
	outp( 0 , SPCR);
}
/****************************************************************************************
		END FPGA XCS "SPARTAN" CFG FUNCTIONS
****************************************************************************************/
#endif
