/***************************************************************************
 Project:		AVRILOS
 Title:			Demo Serial Application
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			serapp.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Serial Application DEMO of command parsing and 
* integration with debugger/monitor
***************************************************************************/

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../periphint/Uart.h"
#include "../Utils/typeconv.h"
#include "../periphint/Timer1.h"

#include "../periphext/sst.h"
#include "../periphext/lpc.h"
#include "../debug/debugger.h"

#include <stdio.h>

#define    c_APPENTRYBUFLEN    10

#define c_PROMPT        '?'
#define c_NEWLINE        0x0a
#define c_ENTER            0x0d

#define c_SSTLEN        0x040000

extern INT8U v_SysStat;

volatile INT8U v_appcnt;
volatile INT8U v_pwmcnt;
volatile INT8U buf_appentry[c_APPENTRYBUFLEN];

volatile INT8 buf_types[60];

void f_SerApp_DbgEn(void);

void f_ProcessAPPCMD(void);

INT8U f_ProcessAPPKey(INT8U v_cmd);

#ifndef MOD_SERAPP_ON

void f_InitSerApp(void) {}

void f_SerApp(void) {}

#else

#define c_SERVOLIMITUP 70
#define c_SERVOLIMITDN 20
#define c_SERVOLIMITZR 50

extern INT8U v_ServoLock;
extern INT8U v_ServoUnLock;

void f_InitSerApp(void)
{
    v_appcnt = 0;
    v_pwmcnt = 50;
    // These set by the applic
    //v_ServoLock = c_SERVOLIMITUP;
    //v_ServoUnLock = c_SERVOLIMITDN;
}

void f_SerApp(void)
{
    INT16 result;
    INT8U ch;
    INT8U v_prockey;
    INT8 *p_str;

    // If running the debugger exit from serial application
    if ( ( v_SysStat & (1 << b_DBG) ) != 0) return;

    // serial appliaction command/key parsing
    if ( ( result = f_Uart_GetChar() ) != (-1) )
    {
        ch = result & 0xff;
        buf_appentry[v_appcnt] = ch;

        // check to see if key is a direct key action
        // or it is a "shell" command
        v_prockey = f_ProcessAPPKey(ch);
        if (v_prockey == 0)
        {
            v_appcnt++;
            // if maximum buffer length reached, show error
            // Else accept key
            if (v_appcnt == c_APPENTRYBUFLEN)
            {
                v_appcnt = 0;
                f_Uart_PutStr("---- Err\n");
                f_Uart_PutStr("> ");
            }
            else
            {
                f_Uart_PutChar(ch);
                // accepted key is Enter, process command
                if ( ch == c_ENTER)
                {
                    f_ProcessAPPCMD();
                    f_Uart_PutStr("> ");
                }
            }
        }

    }


}

// Direct key commands
INT8U f_ProcessAPPKey(INT8U v_cmd)
{
    INT8U v_retval;

    v_retval = 0;

    switch(v_cmd)
    {
        case 'c':				// Increase PWM
            v_pwmcnt++;
            if (v_pwmcnt > v_ServoLock) v_pwmcnt = v_ServoLock;
            f_PWM1Set(0, v_pwmcnt);
            f_Uart_PutStr("salam 1");
            v_retval = 1;
            break;
        case 'v':				// decrease PWM
            v_pwmcnt--;
            if (v_pwmcnt < v_ServoUnLock) v_pwmcnt = v_ServoUnLock;
            f_PWM1Set(0, v_pwmcnt);
            v_retval = 1;
            f_Uart_PutStr("salam 2");
            break;
        case 'z':				// Zero PWM
            v_pwmcnt = c_SERVOLIMITZR;
            v_retval = 1;
            f_PWM1Set(0, v_pwmcnt);
            f_Uart_PutStr("salam 3");
            break;
        case 'n':				// min PWM
            v_pwmcnt = v_ServoUnLock;
            v_retval = 1;
            f_PWM1Set(0, v_pwmcnt);
            f_Uart_PutStr("salam 4");
            break;
        case 'm':				// max PWM
            v_pwmcnt = v_ServoLock;
            v_retval = 1;
            f_Uart_PutStr("salam 5");
            f_PWM1Set(0, v_pwmcnt);
            break;
        case 'e':				// Enable PWM
            f_PWM1Enable(0);
            v_retval = 1;
            f_Uart_PutStr("salam 6");
            break;
        case 'd':				// Disable PWM
            f_PWM1Disable(0);
            v_retval = 1;
            f_Uart_PutStr("salam 7");
            break;

        default:
            f_Uart_PutStr("salam 8");
            v_retval = 0;
    }

    return v_retval;
}

// Shell Commands
void f_ProcessAPPCMD(void)
{
    INT8U cmd = buf_appentry[0];
    INT16U addr;
    INT8U *p_str;

    f_Uart_PutStr("\n");
    v_appcnt = 0;

    switch(cmd)
    {
        case 'q':				// Increment PWM
            v_pwmcnt++;
            f_PWM1Set(0, v_pwmcnt);
            f_Uart_PutStr("salam 9");

            break;
        case 'w':				// Decrement PWM
            v_pwmcnt--;
            f_PWM1Set(0, v_pwmcnt);
            f_Uart_PutStr("salam 10");

            break;

        // asterisk command same with debugger, & for blind revert to debugger
        case '*':
            f_Uart_PutStr("salam 11");

        case '&':
            f_Uart_PutStr("salam 12");

            f_SerApp_DbgEn();
            break;

        default:
            f_Uart_PutStr("\nErr\n");
            f_Uart_PutStr("salam 13");
            break;
    }

}

void f_SerApp_DbgEn(void)
{
    v_SysStat |= (1 << b_DBG);
    f_Uart_PutStr("-Going Debug\n");
}

#endif
