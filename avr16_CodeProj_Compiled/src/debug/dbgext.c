/* Debug Extension commands */
/***************************************************************************
 Project:		AVRILOS
 Title:			Compact Debugger/Monitor extensions
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			dbgext.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Debugger Monitor for AVR (compact)
* Extension functions for external peripherals
***************************************************************************/

#include "../includes/types.h"
#include "../includes/settings.h"

#include "../periphext/lcm_char.h"

#include "../periphext/lpc.h"
#include "../periphext/sst.h"

/*  user debugger command */
void f_Dbg_User_Cmd(void)
{
}


/*
; When FPGA is not present or this type of communication
; with the FPGA is not valid place RET
; instead of real functions of Read/Write Regs
; through the interface
*/

void f_DebugFPGAWr(INT8U addr, INT8U data)
{

}

INT8U f_DebugFPGARd(INT8U addr)
{
	return 0;
}

void f_DebugLCMWrCmd(INT8U data)
{
#ifdef MOD_LCMCHAR_ON
	lcd_outcmd(data);
#endif
}

void f_DebugLCMWrData(INT8U data)
{
#ifdef MOD_LCMCHAR_ON	
	lcd_outdata(data);
#endif
}

INT8U f_DebugLCMRd(INT8U addr)
{
#ifdef MOD_LCMCHAR_ON	
	return lcd_incmd();
#else
	return 0;
#endif
}

void f_DebugLPCWr(INT32U addr, INT8U data)
{
#ifdef MOD_LPC_ON
	f_LPC_Write(addr, data);
#else
#endif
}

void f_DebugSSTWr(INT32U addr, INT8U data)
{
#ifdef MOD_LPC_ON
	f_SSTWrite(addr, data);
#else
#endif
}

INT8U f_DebugLPCRd(INT32U addr)
{
#ifdef MOD_LPC_ON
	return f_LPC_Read(addr);
#else
	return 0;
#endif
}

