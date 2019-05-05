/***************************************************************************
 Project:		AVRILOS
 Title:			Compact Debugger/Monitor for AVR
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			debugger.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Debugger Monitor for AVR (compact)
***************************************************************************

/******************************************************************************************
; All data in Capital (case sensitive)
; Commands:
;	R XXXX 			: Read Byte at Addr XXXX
;	W XXXX YY		: Write Byte YY at XXXX
;	V XXXX			: See hex 4 bytes starting at XXXX
;	A XXXX			: see ASCII 4 bytes starting at XXXX
;	1/2/3/4			: PINA/B/C/D
;	B XX YY			: Write Port PortX(01-04), DDRX(11-14) YY
;	b XX 			  : Read Port PortX(01-04), DDRX(11-14)
; 	Q 0X				: Read Analog Port 0X (X: 0-7)
;	I XXXX			: Inspect Data in EEPROM at addr XXXX
;	P XXXX YY		: Write byte YY in EEPROM at addr XXXX
;	F 00XX 			: Read FPGA SERPIO reg XX (Custom Commands, depend on FPGA Code) 
;	f 00XX YY		: Write FPGA SERPIO Byte YY at reg XX (Custom Commands, depend on FPGA Code)
;   U ????????? 	: User Command
;	L 00XX 			: Read LCM addr XX
;	C XX    		: Write LCM command XX
;	D XX	    	: Write LCM data XX
;	S XXXXXXXX  	: Read 4 bytes at LPC Bus Addr. XXXXXXXX-X+4 (32 bits)
;	s XXXXXXXX YY	: Write LPC Byte at address XXXXXXXX (32 Bits)
;	t XXXXXXXX YY	: Write a byte at LPC FLASH SST49F020/A (with write protection)
;   *               : Revert To Serial APP. Disable Debugger
;  p                : Read SPI DR, SR
****************************************************************************************/
#include <avr/pgmspace.h>


#include "../includes/types.h"
#include "../includes/settings.h"
#include "../periphint/Uart.h"

#ifdef MOD_ADC8_ON
#include "../periphint/adc.h"
#endif

#ifdef MOD_EEPROM_ON
#include "../periphint/eeprom.h"
#endif

#include "../Utils/typeconv.h"
#include "debugger.h"
#include "dbgext.h"

#define c_DEBUGBUFLEN    16
#define c_PROMPT        '?'
#define c_NEWLINE        0x0a
#define c_ENTER            0x0d

#define c_CMDAVAIL        24


#ifndef MOD_DEBUGGER_ON

//void f_InitDebug(void) {}
//
//void f_TaskDebugger(void) {}
//
//void f_CheckSyntax(void) {}
//
//void f_ProcessCMD(void) {}
//
//#else

extern INT8U v_SysStat;
INT8U app_subscription = 0b0;


volatile INT8U v_debugcnt;
volatile INT8U buf_debugentry[c_DEBUGBUFLEN];
INT8U __attribute__ ((progmem)) lut_dbgcmd[] = {\
        'R', 'W', 'A', 'V', \
        '1', '2', '3', '4', \
        'I', 'P', \
        'F', 'f', 'U', \
        'L', 'C', 'D', \
        'B', 'b', 'Q', \
        'S', 's', 't', \
        '*', \
        'p'\
};

void (*f_lut_dbgfunc[c_CMDAVAIL])(void) =
        {f_debug_readbyte, f_debug_writebyte, f_debug_asctab, f_debug_bintab,
         f_debug_PinA, f_debug_PinB, f_debug_PinC, f_debug_PinD,
         f_debug_readEEByte, f_debug_writeEEbyte,
         f_debug_fpga_read, f_debug_fpga_write, f_debug_user_cmd,
         f_debug_lcm_read, f_debug_lcm_wrcmd, f_debug_lcm_wrdata,
         f_debug_Port_read, f_debug_AnalogRead,
         f_debug_lpc_read, f_debug_lpc_write, f_debug_sst_write,
         f_debug_disable,
         f_debug_spi_rd
        };

void f_InitDebug(void) {
    v_debugcnt = 0;
    f_Uart_ClrScr();
    f_Uart_PutStr("code 114");
    f_Uart_PutStr("? ");
    v_SysStat |= (1 << b_DBG);
}

void f_TaskDebugger(void) {
    INT16 result;
    INT8U ch;

    if ((v_SysStat & (1 << b_DBG)) == 0) return;

    if ((result = f_Uart_GetChar()) != (-1)) {
        ch = result & 0xff;
        buf_debugentry[v_debugcnt] = ch;
        v_debugcnt++;
        if (v_debugcnt == c_DEBUGBUFLEN) {
            v_debugcnt = 0;
            f_Uart_PutStr("\nErr\n");
            f_Uart_PutStr("\ncode 135\n");
            f_Uart_PutStr("? ");
        } else {
            f_Uart_PutChar(ch);
            if (ch == c_ENTER) {
                f_ProcessCMD();
                f_Uart_PutStr("? ");
            }

        }

    }
}

void f_CheckSyntax(void) {
}

void f_ProcessCMD(void) {
    INT8U idx;
    f_Uart_PutStr("\n");
    v_debugcnt = 0;
    f_CheckSyntax();
    idx = 0;
    while ((idx < c_CMDAVAIL) && (PRG_RDB(&lut_dbgcmd[idx]) != buf_debugentry[0])) {
        idx++;
    }

    if (idx != c_CMDAVAIL) {
        (*f_lut_dbgfunc[idx])();
    } else {
        f_Uart_PutStr("\nErr\n");
    }

}

void f_debug_port(INT8U data) {
    INT8 str[] = {"  \n"};
    f_BinHex(data, str);
    f_Uart_PutStr(str);
}

void f_debug_port_v(INT8U data) {
    INT8 str[] = {"   "};
    f_BinHex(data, str);
    f_Uart_PutStr(str);
}

void f_debug_readbyte(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = (f_HexBin(p_str));
    v_data = *(INT8U *) addr;
    f_debug_port(v_data);
}

void f_debug_writebyte(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4 + 1];
    v_data = f_HexBinByte(p_str);

    *(INT8U *) addr = v_data;
    /* readback */
    v_data = *(INT8U *) addr;
    f_debug_port(v_data);
}

void f_debug_asctab(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;
    INT8 str[] = {"        \n"};
    INT8U i;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = (f_HexBin(p_str));

    for (i = 0; i < 8; i++) {
        v_data = *(INT8U *) addr;
        addr++;
        str[i] = v_data;
    }

    f_Uart_PutStr(str);

}

void f_debug_bintab(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;
    INT8 str[] = {"            \n"};
    INT8U i;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = (f_HexBin(p_str));
    p_str = &str[0];

    for (i = 0; i < 6; i++) {
        v_data = *(INT8U *) addr;
        addr++;
        f_BinHex(v_data, p_str);
        p_str++;
        p_str++;
    }

    f_Uart_PutStr(str);

}

void f_debug_PinA(void) {
    INT8U data;
    data = inp(PINA);
    f_debug_port(data);
}

void f_debug_PinB(void) {
    INT8U data;
    data = inp(PINB);
    f_debug_port(data);
}

void f_debug_PinC(void) {
    INT8U data;
    data = inp(PINC);
    f_debug_port(data);
}

void f_debug_PinD(void) {
    INT8U data;
    data = inp(PIND);
    f_debug_port(data);
}


void f_debug_readEEByte(void) {
#ifdef MOD_EEPROM_ON
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = (f_HexBin(p_str));
    v_data = f_EERead( addr);
    f_debug_port(v_data);
#else
    f_Uart_PutStr("EEPROM Not Avail.\n");
#endif
}

void f_debug_writeEEbyte(void) {
#ifdef MOD_EEPROM_ON
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2+4+1];
    v_data = f_HexBinByte(p_str);

    f_EEWrite( addr, v_data);

    /* readback */
    v_data = f_EERead( addr);
    f_debug_port(v_data);
#else
    f_Uart_PutStr("EEPROM Not Avail.\n");
#endif
}

void f_debug_fpga_read(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBin(p_str);

    v_data = f_DebugFPGARd((INT8U) (addr & 0xff));
    f_debug_port(v_data);

}

void f_debug_fpga_write(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4 + 1];
    v_data = f_HexBinByte(p_str);

    f_DebugFPGAWr((INT8U) (addr & 0xff), v_data);

}

void f_debug_user_cmd(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBinByte(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 2 + 1];
    v_data = f_HexBinByte(p_str);
    if (v_data == 1) {
        app_subscription = app_subscription | (1 << addr);
    } else {
        app_subscription = app_subscription & (~(1 << addr));
    }
}

void f_debug_lcm_wrcmd(void) {
    INT8U v_data;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    v_data = f_HexBinByte(p_str);
    f_DebugLCMWrCmd((INT8U) (v_data));

}

void f_debug_lcm_wrdata(void) {
    INT8U v_data;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    v_data = f_HexBinByte(p_str);
    f_DebugLCMWrData((INT8U) (v_data));

}

void f_debug_lcm_read(void) {
    INT8U v_data;

    v_data = f_DebugLCMRd(0);
    f_debug_port(v_data);

}


void f_debug_lpc_read(void) {
    INT8U v_data;
    INT16U addr1, addr2;
    INT32U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr1 = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4];
    addr2 = f_HexBin(p_str);
    addr = addr1;
    addr = (addr << 16) | addr2;
    v_data = f_DebugLPCRd(addr++);
    f_debug_port_v(v_data);

    v_data = f_DebugLPCRd(addr++);
    f_debug_port_v(v_data);

    v_data = f_DebugLPCRd(addr++);
    f_debug_port_v(v_data);

    v_data = f_DebugLPCRd(addr++);
    f_debug_port(v_data);

}

void f_debug_lpc_write(void) {
    INT8U v_data;
    INT16U addr1, addr2;
    INT32U addr;
    INT8 *p_str;
    p_str = (INT8 *) &buf_debugentry[2];
    addr1 = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4];
    addr2 = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4 + 4 + 1];
    v_data = f_HexBinByte(p_str);
    addr = addr1;
    addr = (addr << 16) | addr2;
    f_DebugLPCWr(addr, v_data);

}

void f_debug_sst_write(void) {
    INT8U v_data;
    INT16U addr1, addr2;
    INT32U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr1 = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4];
    addr2 = f_HexBin(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 4 + 4 + 1];
    v_data = f_HexBinByte(p_str);
    addr = addr1;
    addr = (addr << 16) | addr2;
    f_DebugSSTWr(addr, v_data);

}

void f_debug_disable(void) {

    v_SysStat &= ~(1 << b_DBG);
    f_Uart_PutStr("-Going SerApp\n");
}

void f_debug_Port_write(void) {
    INT8U v_data;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBinByte(p_str);
    p_str = (INT8 *) &buf_debugentry[2 + 2 + 1];
    v_data = f_HexBinByte(p_str);

    switch (addr) {
        case 0x01:
            outp(v_data, PORTA);
            break;
        case 0x02:
            outp(v_data, PORTB);
            break;
        case 0x03:
            outp(v_data, PORTC);
            break;
        case 0x04:
            outp(v_data, PORTD);
            break;
        case 0x11:
            outp(v_data, DDRA);
            break;
        case 0x12:
            outp(v_data, DDRB);
            break;
        case 0x13:
            outp(v_data, DDRC);
            break;
        case 0x14:
            outp(v_data, DDRD);
            break;

        default:
            break;

    }

}

void f_debug_Port_read(void) {
    INT8U v_data = 0;
    INT16U addr;
    INT8 *p_str;

    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBinByte(p_str);

    switch (addr) {
        case 0x01:
            v_data = PORTA;
            break;
        case 0x02:
            v_data = PORTB;
            break;
        case 0x03:
            v_data = PORTC;
            break;
        case 0x04:
            v_data = PORTD;
            break;
        case 0x11:
            v_data = DDRA;
            break;
        case 0x12:
            v_data = DDRB;
            break;
        case 0x13:
            v_data = DDRC;
            break;
        case 0x14:
            v_data = DDRD;
            break;

        default:
            break;

    }

    f_debug_port(v_data);

}


void f_debug_AnalogRead(void) {
    INT8U v_data = 0;
    INT16U addr;
    INT8 *p_str;

#ifdef MOD_ADC8_ON
    p_str = (INT8 *) &buf_debugentry[2];
    addr = f_HexBinByte(p_str);

    if ( (addr < 0) || (addr > 7) )
    {
        f_Uart_PutStr("Bad Chan\n");
        return;
    }

    v_data = (INT8U) ( (v_Chan[addr] >> 8) & 0xff);
    f_debug_port_v(v_data);
    v_data = (INT8U) (v_Chan[addr] & 0xff);
    f_debug_port(v_data);
#else
    f_Uart_PutStr("ADC Not Avail.\n");
#endif
}

void f_debug_spi_rd(void) {
#ifdef MOD_SPI_ON
    INT8U v_data;

    v_data = (SPDR);
    f_debug_port(v_data);
    v_data = (SPSR);
    f_debug_port(v_data);

#endif
}


#endif
