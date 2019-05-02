
/***************************************************************************
 Project:		AVRILOS
 Title:			Matrix Keyboard 4x4
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			keymat44.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Matrix Keyboard 4x4 
* Relies on SysTick for Trigger execution and debounce
***************************************************************************/

/********************************
;Matrix Keyboard 4x4
;Dependecies on PORTs (2x4 bit)
;Dep. on timers for debounce
;
; KEY MATRIX
;
;	FF = NOKEY
;
; val = row/col
; if (val == 7) val=0
; else val &= 0x03
;
; 7 -> 0000 = 0
; E -> 0010 = 2
; D -> 0001 = 1
; B -> 0011 = 3
;
;		7	E	D	B
;	7	00	02	01	03
;	D	10	12	11	13
;	B	30	32	31	33
;	E	20	22	21	23
;
; CvtKey:
;		7	E	D	B
;	7	00	02	01	03
;	D	04	06	05	07
;	B	0C	0E	0D	0F
;	E	08	0A	09	0B
;
********************************/
#include <avr/io.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../periphint/Timer0.h"

extern INT8U v_SysStat;



#ifndef MOD_KEY44_ON

void f_ConfigKeyb4x4(void) {}
INT8U f_GetHWKey(void) { return 0; }
INT8U f_GetKey(void) { return 0; }

#else

volatile INT8U v_currkey;
volatile INT8U v_debcnt;

void f_ColsIn(void)
{
	outp( inp(c_COLDDR) & c_COLIMASK, c_COLDDR);
}
	
void f_ColsOut(void)
{
	outp( inp(c_COLDDR) | c_COLMASK, c_COLDDR);
}

void f_RowsIn(void)
{
	outp( inp(c_ROWDDR) & c_ROWIMASK, c_ROWDDR);
}
	
void f_RowsOut(void)
{
	outp( inp(c_ROWDDR) | c_ROWMASK, c_ROWDDR);
}

void f_ConfigKeyb4x4(void)
{
	v_currkey = c_NOKEY;
	v_debcnt = 0;
	f_StartTimerKey();

}

INT8U f_CvtKey(INT8U row, INT8U col)
{
	INT8U key;
	
	key = row | col;
	
	if (key == c_NOKEY) return c_NOKEY;
	
	key = col + row * 4;
	
	return key;
}

INT8U f_GetKey(void)
{
	if (v_debcnt != c_KEYDEBLIMIT) return c_NOKEY;
	else return v_currkey;
}


// this is called from SysTick
INT8U f_GetHWKey(void)
{
	INT8U v_ddrrow, v_ddrcol;	
	INT8U v_col, v_row, v_key;
	/* save DDR conditions for other peripherals */
	v_ddrrow= inp(c_ROWDDR);
	v_ddrcol= inp(c_COLDDR);
	
	/* scan matrix keyboard */
	/* ;first look activated Rows */
	f_RowsIn();
	f_ColsOut();
	outp( (inp(c_COLPORT) & c_COLIMASK), c_COLPORT);
	v_row = inp(c_ROWPIN);
	
	/* look activated Cols */
	f_ColsIn();
	f_RowsOut();
	outp(v_row, c_ROWPORT);
	v_col= inp(c_COLPIN);
	
	/* compute scan code */
	v_row &= c_COLIMASK;
	v_col &= c_ROWIMASK;

	if (v_row == 7) v_row = 0;
	else v_row &= 0x03;
	if (v_col == 7) v_col = 0;
	else v_col &= 0x03;

	/* restore DDRs */
	outp(v_row, c_ROWDDR);	
	outp(v_col, c_COLDDR);
	/* assign code */
	/* keymap translations if any */
	v_key = f_CvtKey(v_row, v_col);
	
	if (v_currkey != v_key) 
	{
		v_currkey = v_key;
		v_debcnt = 0;
	}
	else
	{
		if (v_debcnt < c_KEYDEBLIMIT) v_debcnt++;
	}
	
	return v_key;	

}

#endif
