/***************************************************************************
 Project:		AVRILOS
 Title:			Demo Serial Application
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			delay.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Serial Application DEMO of command parsing and 
* integration with debugger/monitor
***************************************************************************/

#include <avr/io.h>

#include "../includes/types.h"
#include "../includes/settings.h"

void f_delay_loop_1(uint8_t __count)
{
	__asm__ volatile (
		"1: dec %0" "\n\t"
		"brne 1b"
		: "=r" (__count)
		: "0" (__count)
	);
}


void f_Delay_us(INT8U delay_us)
{
    INT8U microsec;
    for(microsec=0;microsec<delay_us; microsec++)
    {
        f_delay_loop_1(MPUCLK_Hz/1000000L);   // for 8MHz           
    }   
}

void f_Delay_ms(INT8U delay_ms)
{
    INT8U millisec;
    for(millisec=0;millisec<delay_ms; millisec++)
    {
        f_Delay_us(250); 
        f_Delay_us(250); 
        f_Delay_us(250); 
        f_Delay_us(250); 
    }      
    
}
