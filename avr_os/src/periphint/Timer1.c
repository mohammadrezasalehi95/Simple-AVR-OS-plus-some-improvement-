
/***************************************************************************
 Project:		AVRILOS
 Title:			Timer1 Functions, PWM, Servo control
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/ATMEGA163
 File:			Timer1.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Control of R/C servos with PWM control
***************************************************************************/

#include <avr/io.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../includes/ifc_time.h"
#include "Timer1.h"

#ifndef MOD_PWMTIMER_ON

void f_InitPWM1(void) {}
void f_PWM1Enable(INT8U v_channel) {}
void f_PWM1Disable(INT8U v_channel) {}
void f_PWM1Set(INT8U v_channel, INT8U v_setpoint)  {}

#else




extern INT8U v_SysStat;


void f_InitPWM1(void)
{
	/*Stop Timer 0 for initialization*/
    
    (TCCR1A) = 0xA3; // 10-bit PWM
    (TCCR1B) = 0x0C; // CLK/256 = 31250Khz
    //(c_PWMDDR) |= (1 << b_PWM1) | (1 << b_PWM2); // inverted PWM
    
    f_PWM1Disable(0);
    f_PWM1Disable(1);
    
}

void f_PWM1Enable(INT8U v_channel)
{
    
    if (v_channel == 0)
    {
        (c_PWMDDR) |= (1 << b_PWM1);
    }
    else if (v_channel == 1)
    {
        (c_PWMDDR) |= (1 << b_PWM2);
    }
    else //unknown channel, do nothing
    {
    
    }
}

void f_PWM1Disable(INT8U v_channel)
{
    
    if (v_channel == 0)
    {
        (c_PWMDDR) &= ~(1 << b_PWM1);
    }
    else if (v_channel == 1)
    {
        (c_PWMDDR) &= ~(1 << b_PWM2);
    }
    else //unknown channel, do nothing
    {
    
    }
}


void f_PWM1Set(INT8U v_channel, INT8U v_setpoint) 
{
    if (v_channel == 0)
    {
        (OCR1AH) = 0;
        (OCR1AL) = v_setpoint;
    }
    else if (v_channel == 1)
    {
        (OCR1BH) = 0;
        (OCR1BL) = v_setpoint;
    }
    else //unknown channel, do nothing
    {
    
    }

}


#endif
