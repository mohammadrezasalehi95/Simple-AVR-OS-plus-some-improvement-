/***************************************************************************
 Project:		AVRILOS
 Title:
 Author:		Ilias Alexopoulos
 Version:		2.10
 Last updated:	06-Jan-2011
 Target:		AT90S8535/ATMEGA163
 File:			ifc_time.h

* Support E-mail:
* avrilos@ilialex.gr
* 
* History
* V2.00, 28-Oct-2010: Initial Public Release
* V2.10, 06-Jan-2011: Modified xxxxxx_ticks defines to be INT16U instead of INT8U.
*
* DESCRIPTION
* Timer 0, Systick and SW Timers Definitions
* 
***************************************************************************/

/* Timer 0 */


// Alive Led Indications Set
#define   c_ALIVEOK_ms       250   /* Alive LED When Ok */
#define   c_ALIVESER_ms       500   /* Alive LED When Serial Error */

// Timed Tasks interval
#define c_AppInterval_ms        8		
#define c_ADCInterval_ms        32
#define c_KEYInterval_ms        16

// Maximum SW Timers (mS)
#define c_MAXSWTIMERS	4
#define c_SwTimer0      0   /* Timer Activation (0: Stop, >0: Run) */
#define c_SwTimer1      1
#define c_SwTimer2      2
#define c_SwTimer3      3


#define	c_ONESECOND_ms     	1000    /* 1ms system clock period */

#define	b_TOV0		0	/* Bit position in TIFR */
#define	b_TOIE0		0	/* Bit Position in TIMSK Int. Mask Reg. */
#define	b_OCIE1A	4
#define	b_OCIE1B	3
#define	b_OCF1A		4
#define	b_OCF1B		3

/* Hardware consts for TCCR0 */
#define	c_CLKOFF	0x00
#define	c_CLK		0x01
#define	c_CLK8		0x02
#define	c_CLK64		0x03
#define	c_CLK256	0x04
#define	c_CLK1024	0x05
#define	c_CLKEXTF	0x06
#define	c_CLKEXTR	0x07

/* Select Clock Source for T0 */
#define c_T0CLK 	c_CLK64
#define	c_T0DIV		64
#define	c_T0STOP			c_CLKOFF	/* Stop T0 */
#define	c_T0CK8Start		c_T0CLK		/* Start, CK/64 T0 (65KHz) */

#define c_T0Unit_ticks_per_ms   (MPUCLK_Hz/c_T0DIV)/c_ONESECOND_ms
#define c_T0TimeSlice   	    256-c_T0Unit_ticks_per_ms           // for 1mS interval
#define c_TimeSlice_Hz        c_T0TimeSlice/(MPUCLK_Hz/c_T0DIV)

/* Period for various functions */
#define c_KeyPeriod			20

// Computed values for systick
#define c_SYSTIMEMAX            c_SysTickPeriod_ms
//#define c_SysTickPeriod_ms      (1000.0/c_TimeSlice_Hz)
#define c_ALIVEOK_ticks         (INT16U) (c_TimeSlice_Hz/c_ALIVEOK_ms)    /* Alive LED When Ok */
#define c_ALIVESER_ticks        (INT16U) (c_TimeSlice_Hz/c_ALIVESER_ms)   /* Alive LED When Serial Error */
#define c_APP_ticks             (INT16U) (c_TimeSlice_Hz/c_AppInterval_ms)
#define c_ADC_ticks             (INT16U) (c_TimeSlice_Hz/c_ADCInterval_ms)
#define c_KEYPAD_ticks          (INT16U) (c_TimeSlice_Hz/c_KEYInterval_ms)


