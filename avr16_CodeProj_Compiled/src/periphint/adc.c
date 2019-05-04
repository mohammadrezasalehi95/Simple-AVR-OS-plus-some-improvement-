
/***************************************************************************
 Project:		AVRILOS
 Title:			ADC Module for AVR
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/ATMEGA163/323/16/164p
 File:			serapp.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* ADC Sampling triggered by SysTick
* Software just reads 8 RAM locations one for each channel (1-8)
* Configuration: Single-Ended
***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../includes/types.h"
#include "../includes/settings.h"

extern INT8U v_SysStat;

#ifndef MOD_ADC8_ON

_INLINE_ void f_ConfigADC(void) {}
_INLINE_ void f_StartADC(void) {}
_INLINE_ void f_SysADC(void) {}

#else


volatile INT16U v_Chan[c_MAXADCCH];

void f_ConfigADC(void)
{
	volatile INT16U v_temp, i;

	        
	/* First channel (dummy read) */
	ADMUX = ADC_VREF_MODE|ADC_DATA_ADJ|0x00;
	
	#if c_MCU == c_AT90S8535
  	ADCSR = (1 << ADEN) | (1 << ADIE) | ADCPRSC_32;
  	v_temp = (ADCL) | ((ADCH) << 8);
  #elif (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
  	ADCSR = (1 << ADEN) | (1 << ADIE) | ADCPRSC_32;
  	v_temp = (ADCL) | ((ADCH) << 8);
  	PORTA = 0x00;
		DDRA = 0x00;
  #elif (c_MCU == c_ATMEGA16)
  	ADCSRA = (1 << ADEN) | (1 << ADIE) | ADCPRSC_32;
		v_temp = (ADCL) | ((ADCH) << 8);
  #elif c_MCU == c_ATMEGA164P		
  	INT8U v_aport=0; 		
 		for(i=0;i<c_MAXADCCH; i++) v_aport |= 1 << i; 
		DIDR0  = v_aport; // Analog Input Port
		ADCSRA = (1 << ADEN) | (1 << ADIE) | ADCPRSC_32;
		ADCSRB &= 0xF8;
		v_temp = (ADCL) | ((ADCH) << 8);
  #else
  #error "---- ERROR: No known CPU defined!!! ------"
  #endif
  
	for(i=0; i<c_MAXADCCH; i++) v_Chan[i] = 0xff;

}

void f_StartADC(void)
{
	v_SysStat &= ~(1 << b_ADCTick);
	
	#if (c_MCU == c_AT90S8535) || (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
		ADCSR |= 1 << ADSC;
	#elif (c_MCU == c_ATMEGA164P) || (c_MCU == c_ATMEGA16)
		ADCSRA |= 0x40;
	#endif
	
}

void f_SysADC(void)
{
	if ( (v_SysStat & (1 << b_ADCTick) ) != 0)
	{ 
		f_StartADC();
	}
}

/* i_ADC(void) */
/****************************************
; Get previous channel measurement
; Store value to appropriate memory
; Update ADMUX channel
; Real V = steps * 0.020mV
****************************************/

ISR(ADC_vect)
{
	INT8U v_chan_idx;
	INT16U v_data;

	v_chan_idx = ADMUX;
	
	v_data = (ADCL) | ((ADCH) << 8);
	//v_data = ADCL;
	//v_data |= (ADCH << 8);
	
	v_chan_idx &= 0xF0; // mask bits
	v_Chan[v_chan_idx] = v_data;

	v_chan_idx = ADMUX;
	if ( (v_chan_idx & (c_MAXADCCH-1)) != (c_MAXADCCH-1)) v_chan_idx++;
	else v_chan_idx &= ~(c_MAXADCCH-1);
	ADMUX = v_chan_idx | (ADC_VREF_MODE & 0xff);

}



#endif
