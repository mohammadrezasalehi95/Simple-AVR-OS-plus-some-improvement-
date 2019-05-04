/***************************************************************************
 Project:		AVRILOS
 Title:			Timer0, SysTick
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/15/ATMEGA163/103/16/323/164p
 File:			Timer0.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Timer 0, Systick and SW Timers
***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../includes/ifc_time.h"

#ifndef MOD_SYSTIMER_ON
void f_InitSysTimer(void) {}
void f_SysTick(void) {}
void f_StartTimerLCM(void) {}
void f_StopTimerLCM(void) {}
void f_StartTimerKey(void) {}
void f_StopTimerKey(void) {}

#else

#ifdef MOD_LCMCHAR_ON
#include "../periphext/lcm_char.h"
volatile INT8U v_SwTimerLCM;
void f_SwTimerLCM(void);
volatile INT8U v_SwTimerLCM;
#endif

#ifdef MOD_KEY44_ON
#include "../periphext/keymat44.h"
void f_SwTimerKey(void);
volatile INT8U v_SwTimerKey;
#endif

void f_SystickSetErrLevel(INT16U v_errorlevel);

volatile INT16U v_SysTimer;
volatile INT16U v_ErrLevel;
volatile INT16U v_SwTimer_mS[c_MAXSWTIMERS];

extern INT8U v_SysStat;

void f_InitSysTimer(void);
void f_flashled(void);
void f_SysTick(void);


void f_flashled(void);

void f_InitSysTimer(void)
{
	INT8U v_temp;
	/*Stop Timer 0 for initialization*/

#if (c_MCU == c_AT90S8515) || (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
	TCCR0 = c_T0STOP;
	TCNT0 = c_T0TimeSlice;
	TIFR 	|= (1 << b_TOV0);
	TIMSK |= (1 << b_TOIE0);	
#elif c_MCU == c_ATMEGA164P		
	TCCR0A 	= 0x00;
	TCCR0B 	= c_T0CLK;
	TCNT0 	= c_T0TimeSlice;		/* reset TCNT0 */
	TIFR0 	|= (1 << b_TOV0);
	TIMSK0 	|= (1 << b_TOIE0);
#else
#error "---- ERROR: No known CPU defined!!! ------"
#endif	

    for(v_temp=0; v_temp < c_MAXSWTIMERS ; v_temp++) v_SwTimer_mS[v_temp] = 0;

    v_SysTimer = 0;
    v_ErrLevel = c_ALIVEOK_ticks;
    
#ifdef MOD_LCMCHAR_ON
	v_SwTimerLCM = 0;
#endif	

#ifdef MOD_KEY44_ON	
	v_SwTimerKey = 0;
#endif	
	
    //v_SwTimAlive = 0; 

#if (c_MCU == c_AT90S8515) || (c_MCU == c_ATMEGA16)	|| (c_MCU == c_ATMEGA323)	|| (c_MCU == c_ATMEGA163)	
	TCCR0 = c_T0CK8Start;
#endif

	v_temp = (c_DDRPALIVE);
	v_temp |= 1 << b_LedAlive;	
	(c_DDRPALIVE) = v_temp;
	
	v_temp = (c_PORTALIVE);
	v_temp |= 1 << b_LedAlive;	
	(c_PORTALIVE) = v_temp;	

}

#ifdef MOD_LCMCHAR_ON
void f_StartTimerLCM(void)
{
	v_SwTimerLCM = 2;	
}

void f_StopTimerLCM(void)
{
	v_SwTimerLCM = 0;    
}
#endif

#ifdef MOD_KEY44_ON	
void f_StartTimerKey(void)
{
	v_SwTimerKey = c_KeyPeriod;	
}

void f_StopTimerKey(void)
{
    v_SwTimerKey = 0;
}
#endif

void f_SysTick(void)
{
    INT8U v_idx;

    if ( (v_SysStat & (1 << b_SysTick ) ) != 0 )
  	{
  		v_SysStat &= ~(1 << b_SysTick );
        v_SysTimer++;
        
        // Wrap around Timer
        // Interval is 1 second
        if (v_SysTimer == c_SYSTIMEMAX) v_SysTimer = 0;
        
        // Toggle AliveLED depending on error state        
        if ( (v_SysTimer % v_ErrLevel) == 0)
        {                        
            f_flashled();            
        }
        
        if ( (v_SysTimer % c_APP_ticks) == 0)
        {
            v_SysStat |= (1 << b_AppTick );        
        }
        
        if ( (v_SysTimer % c_ADC_ticks) == 0)
        {
            v_SysStat |= (1 << b_ADCTick );        
        }
        
        for(v_idx=0;v_idx<c_MAXSWTIMERS; v_idx++)
        {
            if (v_SwTimer_mS[v_idx] != 0)
            {
                v_SwTimer_mS[v_idx]--;
            }
        }
        
#ifdef MOD_KEY44_ON
        if ( (v_SysTimer % c_KEYPAD_ticks) == 0) f_SwTimerKey();	    	
#endif 
#ifdef MOD_LCMCHAR_ON
        f_SwTimerLCM();	    			
#endif

  	}

}



void f_flashled(void)
{
	INT8U v_temp;
	v_temp = (c_PORTALIVE);
	v_temp ^= 1 << b_LedAlive;		
	(c_PORTALIVE) = v_temp;
}


void f_SwTimerKey(void)
{
#ifdef MOD_KEY44_ON
	f_GetHWKey();
#endif	
}



void f_SwTimerLCM(void)
{
#ifdef MOD_LCMCHAR_ON	
	if (v_SwTimerLCM > 0)
	{
        v_SwTimerLCM--;
		if (v_SwTimerLCM == 0) f_LCM_Timer();
	}
#endif	
}

void f_SystickSetErrLevel(INT16U v_errorlevel)
{
	v_ErrLevel = v_errorlevel;	
}


ISR(SIG_OVERFLOW0)        /* signal handler for tcnt0 overflow interrupt */
{
	v_SysStat |= (1 << b_SysTick);	
    /* reset counter to get this interrupt again */
	TCNT0 = c_T0TimeSlice;

}

#endif
