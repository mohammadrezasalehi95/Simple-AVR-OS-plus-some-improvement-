
/***************************************************************************
 Project:		AVRILOS
 Title:			LCD Module driver for HD44780
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			lcm_char.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*
* DESCRIPTION
* This is an LCD module driver for character based LCD. Code is adopted from Joerg Wunsch
* The initial code was written in assembly (were it was tested). 
* However this C version is not tested
***************************************************************************/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * HD44780 LCD display driver
 *
 * The LCD controller is used in 4-bit mode with a full bi-directional
 * interface (i.e. R/~W is connected) so the busy flag can be read.
 *
 * $Id: hd44780.c 2002 2009-06-25 20:21:16Z joerg_wunsch $
 */
 
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../periphint/Timer0.h"
#include "../utils/delay.h"
#include "lcm_char.h"

#define LCMClk      do{                             \
                        sbi(c_LCMPORT, b_LCM_E);    \
                        __builtin_avr_delay_cycles(8); \
                        cbi(c_LCMPORT, b_LCM_E);    \
                    }while(0)


#define HD44780_CLR      0x01
#define HD44780_HOME     0x02
#define HD44780_ENTMODE(inc, shift) (0x04|(inc? 0x02: 0)|(shift? 1: 0))
#define HD44780_DISPCTL(disp, cursor, blink) \
(0x08|(disp? 0x04: 0)|(cursor? 0x02: 0)|(blink? 1: 0))
#define HD44780_SHIFT(shift, right) \
(0x10|(shift? 0x08: 0)|(right? 0x04: 0))
#define HD44780_FNSET(if8bit, twoline, font8x10) \
(0x20|(if8bit? 0x10: 0)|(twoline? 0x08: 0)|(font8x10? 0x04: 0))
#define HD44780_CGADDR(addr) (0x40|(addr & 0x3f))
#define HD44780_DDADDR(addr) (0x80|(addr & 0x7f))
#define HD44780_BUSYFLAG 0x80

typedef struct 
{
    INT8U pos;
    INT8U v_currchar;
	INT8U msg[22];
} type_lcm_text;

volatile type_lcm_text lcm_text;
extern INT8U v_SysStat;


#ifndef MOD_LCMCHAR_ON
_INLINE_ void f_ConfigLCM(void) {}
_INLINE_ void f_LCM_Timer(void) {}
_INLINE_ void f_LCM_ClrDisp(void) {}
_INLINE_ void f_LCM_DispOff(void) {}
_INLINE_ void f_LCM_DispOn(void) {}
_INLINE_ void f_LCM_WrCharData(INT8U data) {}
_INLINE_ void f_LCM_set_DDRAM_Addr(INT8U addr) {}
_INLINE_ void f_LCM_OutNibble(INT8U n, INT8U rs) {}
_INLINE_ INT8U f_LCM_InNibble(INT8U rs) { return 0; }
_INLINE_ void f_LCM_OutByte(INT8U b, INT8U rs) {}
_INLINE_ INT8U f_LCM_InByte(INT8U rs) { return 0; }
_INLINE_ void f_LCM_WaitRdy(void) {}

_INLINE_ void f_LCM_WrMsg(INT8U pos, INT8U *p_msg, INT8U memtype) {}
_INLINE_ void f_LCM_WrMsgUnBuf(INT8U pos, INT8U *p_msg, INT8U memtype) {}
_INLINE_ void f_LCM_PutChar(INT8U ch) {}

#else

    
void f_LCM_OutNibble(INT8U n, INT8U rs)
{
	INT8U temp;

  	temp = n & ( BV(b_LCM_D4)|BV(b_LCM_D5)|BV(b_LCM_D6)|BV(b_LCM_D7) );
  	outp(temp, c_LCMPORT );

	cbi(c_LCMPORT, b_LCM_RW);
  
  	if (rs)
  		sbi(c_LCMPORT, b_LCM_RS);
  	else
    	cbi(c_LCMPORT, b_LCM_RS);

  	LCMClk;
}


INT8U f_LCM_InNibble(INT8U rs)
{
  	INT8U temp;

	outp(inp(c_LCMDDR) & ~(BV(b_LCM_D4)|BV(b_LCM_D5)|BV(b_LCM_D6)|BV(b_LCM_D7)), c_LCMDDR);

	sbi(c_LCMPORT, b_LCM_RW);
  
  	if (rs)
  		sbi(c_LCMPORT, b_LCM_RS);
  	else
    	cbi(c_LCMPORT, b_LCM_RS);

  	LCMClk;
  
	temp = inp(c_LCMPIN) & (BV(b_LCM_D4)|BV(b_LCM_D5)|BV(b_LCM_D6)|BV(b_LCM_D7));
  	cbi(c_LCMPORT, b_LCM_RW);
  	outp(inp(c_LCMDDR) | (BV(b_LCM_D4)|BV(b_LCM_D5)|BV(b_LCM_D6)|BV(b_LCM_D7)), c_LCMDDR);

  	return temp;
}

void f_LCM_OutByte(INT8U b, INT8U rs)
{
  f_LCM_OutNibble(b >> 4, rs);
  f_LCM_OutNibble(b & 0xf, rs);
}

INT8U f_LCM_InByte(INT8U rs)
{
  INT8U x;

  x = f_LCM_InNibble(rs) << 4;
  x |= f_LCM_InNibble(rs);
  return x;
}

void f_LCM_WaitRdy(void)
{
  while (lcd_incmd() & HD44780_BUSYFLAG) ;
}

void f_LCM_ClrDisp(void)
{
    /* Entry Clear Display */
    outp(0x00, c_LCMPORT);
    LCMClk;
    outp(0x01, c_LCMPORT);
    LCMClk;
    f_StartTimerLCM();
}

void f_LCM_DispOff(void)
{
    /* display off */
    outp(0x00, c_LCMPORT);
    LCMClk;
    outp(0x08, c_LCMPORT);
    LCMClk;
}
        
void f_LCM_DispOn(void)
{
    /* display on */
    outp(0x00, c_LCMPORT);
    LCMClk;
    outp(0x0c, c_LCMPORT);
    LCMClk;
}

void f_LCM_cpdata(INT8U *msg, INT8U memtype)
{
    INT8U i;
    INT8U *p_msg = msg;
    
    
    if (memtype == c_RAM)
    {
        for(i=0;i<22;i++)
        {
            lcm_text.msg[i] = *(p_msg+i);
        }
    }
    else if (memtype == c_FLASH)
    {
        for(i=0;i<22;i++)
        {
            lcm_text.msg[i] = PRG_RDB(p_msg+i);
        }
    }
    else
    { /* EEPROM */
    }
}
void f_LCM_WrMsg(INT8U pos, INT8U msg[], INT8U memtype)
{    
    f_LCM_cpdata(msg, memtype);
    lcd_outcmd( (0x80 | pos) );
    lcd_outdata(lcm_text.msg[0]);
    lcm_text.pos = pos;
    lcm_text.v_currchar = 1;
    
    if (lcm_text.msg[0] != 0)
    {
        f_StartTimerLCM();
        v_SysStat |= 1 << b_LCDBusy;
    }
    
}

void f_LCM_WrMsgUnBuf(INT8U pos, INT8U *msg, INT8U memtype)
{
	INT8U ch;
	INT8U i;
		    
	while ( (v_SysStat & (1 << b_LCDBusy) ) != 0 );
	
    f_LCM_cpdata(msg, memtype);
	f_LCM_WaitRdy();
    lcd_outcmd( (0x80 | pos) );
    lcm_text.pos = pos;
    i = 0;
	
	f_LCM_WaitRdy();
    while( (ch=lcm_text.msg[i++]) != 0 )
    {
    	lcd_outdata(ch);
		f_LCM_WaitRdy();
    }
    lcm_text.v_currchar = i;
}

void f_LCM_PutChar(INT8U ch)
{
		f_LCM_WaitRdy();
		lcd_outdata(ch);
}

const INT8U __attribute__ ((progmem)) chargen[] =
{
  0x18, 0x14, 0x19, 0x15, 0x12, 0x05, 0x05, 0, /* 0: RX */
  0x1c, 0x08, 0x0d, 0x0d, 0x02, 0x05, 0x05, 0, /* 1: TX */
  0x08, 0x1f, 0x08, 0x00, 0x02, 0x1f, 0x02, 0, /* 2: <-/-> */
  0x02, 0x1f, 0x02, 0x00, 0x02, 0x1f, 0x02, 0, /* 3: ->/-> */
  0x00, 0x00, 0x0a, 0x1f, 0x0a, 0x00, 0x00, 0, /* 4: <--> */
  0x18, 0x10, 0x14, 0x02, 0x0b, 0x0a, 0x03, 0, /* 5: rit */
};

void f_LCM_Init_CharGen(void)
{
	INT8U i;
	INT8 c;

	lcd_outcmd(HD44780_CGADDR(0));
	f_LCM_WaitRdy();

	for (i = 0; i < sizeof chargen / sizeof chargen[0]; i++)
	{
		c = PRG_RDB(chargen+i);
		//c = __lpm_inline((uint16_t)(chargen + i));
		lcd_outdata(c);
		f_LCM_WaitRdy();
	}

	lcd_outcmd(HD44780_DDADDR(0));
	f_LCM_WaitRdy();
}

void f_LCM_Timer(void)
{
    INT8U v_char;
    
    v_char = lcm_text.msg[lcm_text.v_currchar++];
    if ((v_char == 0) || (lcm_text.v_currchar == 22))
    {
        v_SysStat &= ~(1 << b_LCDBusy);
        return;
    }
    lcd_outdata(v_char);
    f_StartTimerLCM();
}

void f_ConfigLCM(void)
{
	
    /* LCM PON Reset */
    outp(0x7f, c_LCMDDR);

	f_LCM_OutNibble(0x03, 0);
	f_Delay_ms(15);
	f_LCM_OutNibble(0x03, 0);
	f_Delay_ms(15);
	f_LCM_OutNibble(0x03, 0);
	f_Delay_ms(15);

  	f_LCM_OutNibble(HD44780_FNSET(0, 1, 0) >> 4, 0);
  	f_LCM_WaitRdy();
  	lcd_outcmd(HD44780_FNSET(0, 1, 0));
 	f_LCM_WaitRdy();
  	lcd_outcmd(HD44780_CLR);
  	f_LCM_WaitRdy();
  	lcd_outcmd(HD44780_DISPCTL(1, 1, 1));
  	f_LCM_WaitRdy();
  	lcd_outcmd(HD44780_ENTMODE(1, 0));
  	f_LCM_WaitRdy();

	f_LCM_Init_CharGen();
	
/*	
  	for (i = 0; i < 80; i++)
    {
		if (i <= 5)
			c = i;			// RAM chargen
      	else
			c = (i - 6) + '!';

      	lcd_outdata(c);
	    f_LCM_WaitRdy();
    }
  	lcd_outcmd(HD44780_DISPCTL(1, 0, 0));
  	f_LCM_WaitRdy();
*/

}   
#endif

