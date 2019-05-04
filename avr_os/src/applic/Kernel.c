/***************************************************************************
 Project:		AVRILOS
 Title:
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/15/ATMEGA163/103/16/323/164p
 File:			Kernel.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*
* DESCRIPTION
* Kernel AVRILOS Main file
***************************************************************************
* AVR
* SubFunctions Implemented:
*--------------------------------------------------------------------------	
	Uart
	Timer0
	ADC (internal)
	Debugger
	FPGA config (Spartan/X3K)
	LCM - CHAR
	LPC - SST 49LF020
	SERAPP - Serial download of SST
	Keyboard 4x4	
	EEPROM		
    SPI
    
	------- To go	
	I2C	

***************************************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "../includes/types.h"
#include "../includes/settings.h"

#include "../periphint/Uart.h"
#include "../periphint/Timer0.h"
#include "../periphint/Timer1.h"
#include "../periphint/adc.h"
#include "../debug/debugger.h"
#include "../periphext/xcs_cfg.h"
#include "../Applic/serapp.h"
#include "../periphext/lcm_char.h"
#include "../periphext/keymat44.h"
#include "../periphext/lpc.h"
#include "../periphext/scard.h"

#include "../Applic/cfg/xcs05_cfg.h"

#include "../applic/serapp.h"
#include "../applic/applic.h"

volatile INT8U v_SysStat;
volatile INT8U v_StatReg;


int main(void)
{
	v_SysStat = 0;
	INT8U v_led = 0;
	INT8U v_key;
	INT8U buf_g[9];

	f_InitSysTimer();

#ifdef MOD_UART_ON
	f_ConfigSerial();
#else
#warning "No UART"
#endif

#ifdef MOD_ADC8_ON	
	f_ConfigADC();
#endif

#ifdef MOD_DEBUGGER_ON	
	f_InitDebug();
#endif

#ifdef MOD_FPGAXCS_ON	
	f_CfgSPIFPGA();
#endif
	
#ifdef MOD_LCMCHAR_ON	
	f_ConfigLCM();
#endif

#ifdef MOD_KEY44_ON	
	f_ConfigKeyb4x4();
#endif

#ifdef MOD_LPC_ON
	f_ConfigLPC();
#endif

#ifdef MOD_PWMTIMER_ON
    f_InitPWM1();
#endif 

#ifdef MOD_SCARD_ON
    f_Init_SCard();
#endif 

#ifdef	MOD_SERAPP_ON
	// Application Level Init
	f_InitSerApp();
#endif

#ifdef DCFG_LOWPOW
	cbi(MCUCR, SM0);                          // Idle mode for sleeps
	cbi(MCUCR, SM1);
	sbi(MCUCR, SE);
#endif


	sei();                   /* enable interrupts */
	f_Uart_ClrScr();
	f_Uart_PutStr(c_VEROSSTR);
	f_Uart_PutStr(c_VERAPPSTR);
	f_Uart_PutStr("\nsalam slaam dastoor bedid");

#ifdef MOD_LCMCHAR_ON
	//f_LCM_WrMsgUnBuf(c_LCML1, c_VEROSSTR, c_FLASH);
	//f_LCM_WrMsgF(c_LCML2, c_VERAPPSTR);
	f_LCM_WrMsgUnBufF(c_LCML1, c_VEROSSTR);
	f_LCM_WrMsgUnBufF(c_LCML2, "\nsalam slaam dastoor bedid");
	f_LCM_WrMsgUnBufF(c_LCML2, c_VERAPPSTR);
	f_LCM_WrMsgUnBufF(c_LCML3, "AVRILOS Demo");
	f_LCM_WrMsgUnBufF(c_LCML4, "CodeProject!");
#endif



    for(;;)
    {
			f_SysTick();
			
			#ifdef MOD_ADC8_ON	
			f_SysADC();
			#endif
			
			#if defined (MOD_DEBUGGER_ON)
			f_TaskDebugger();
			#endif

			#ifdef	MOD_SERAPP_ON
			f_SerApp();
			#endif
			
            #ifdef MOD_SCARD_ON
            //f_SCard_CheckCardIn();
            #endif

			#ifdef MOD_KEY44_ON			
			if ( (v_key = f_GetKey()) != c_NOKEY) f_Uart_PutChar( v_key+'A');
			#endif	
			
			#ifdef MOD_LCMCHAR_ON			
			//if ( (v_key = f_GetKey()) != c_NOKEY) f_LCM_PutChar(v_key+'A');
			#endif	
			
			if ( ( v_SysStat & (1 << b_AppTick) ) != 0)
			{
				v_SysStat &= ~(1 << b_AppTick);
				f_Applic();
				
			}
			
			#ifdef DCFG_LOWPOW
			asm("sleep"::);
			#endif

    };              /* loop forever */
}

