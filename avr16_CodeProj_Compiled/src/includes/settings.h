/***************************************************************************
 Project:		AVRILOS
 Title:			Settings for Hardware &	Software
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/ATMEGA163
 File:			settings.h

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Global Header File
***************************************************************************/


#include "legacy.h"

#define c_VEROSSTR "\n \n {NOTICE!} \n [Hi this is my compiled version of AVRILOS]\n \n AVRILOS " XSTR(c_VEROS)" started...\r\n"
#define c_VERAPPSTR "APP Version " XSTR(c_VERAPP)"\r\n"

/***********************************
	Modules Included in Program
	Undef unneeded modules
***********************************/

/* Defines set from makefile */
/* These are here just for reference */
//#define MOD_SYSTIMER_ON
//#define MOD_UART_ON
//#define MOD_ADC8_ON
//#define MOD_DEBUGGER_ON
//#define MOD_FPGAXCS_ON
//#define MOD_LCMCHAR_ON
//#define MOD_KEY44_ON
//#define MOD_LPC_ON
//#define MOD_SERAPP_ON
//#define MOD_SPI_ON



// CPU Definitions must match makefile (swdef.in)
#define c_AT90S8515			0
#define c_ATMEGA16 		    1
#define c_ATMEGA163 		2
#define c_ATMEGA164P 		3
#define c_ATMEGA323 		4

/**************** Serial Port (UART) ****************/
/* set baud rate here if not in makefile 			*/
//#define UART_BAUD_RATE 		19200

/**************** Timer 0 ***************************/
/* System Tick 										*/
/* if not set in makefile				*/
//#define	c_SysTickPeriod_ms	500

/* Hardware Defined Constant */
/* Defined in makefile  */
//#define MPUCLK_Hz        	8000000               /* 4MHz processor */


/* Select Port and bit for LED Alive */
#define	b_LedAlive	4	
#define	c_PORTALIVE	PORTB
#define	c_DDRPALIVE	DDRB

/**************** FPGA Configuration I/O ***********/
#define	c_CFGPIN		PINB
#define	c_CFGPORT		PORTB
#define	c_CFGDDR		DDRB
#define c_CFGSIZE		6748        // Xilinx spartan XCS05 configuration memory

#define	b_Done			1
#define	b_Reset		    2

/* Used with SPI */
#define	b_Din			5		/* MOSI for SPI instead of MISO (6) */
#define	b_CClk			7

/* for Spartan/XL, XC4000 */
#define	b_Prog		    b_Reset
#define	b_Init		    b_Done

/**************** FPGA Application I/O *************/
#define	b_MCUClk		6
#define	b_MCUFrame	    1
#define	b_MCUData		5

#define	b_FPGACLK		0

#define	c_IFCPIN		PINB
#define	c_IFCPORT		PORTB
#define	c_IFCDDR		DDRB

/**************** LCM ******************************/
#define c_LCMPIN		PINC
#define c_LCMPORT		PORTC
#define c_LCMDDR		DDRC

/* Port C */
#define	b_LCM_RS		PC6
#define b_LCM_RW		PC5
#define b_LCM_E			PC4
#define b_LCM_D7 		PC3
#define b_LCM_D6 		PC2
#define b_LCM_D5 		PC1
#define b_LCM_D4 		PC0

/**************** LPC Bus ****************************/
#define c_LPCPIN		PINC
#define c_LPCPORT		PORTC
#define c_LPCDDR		DDRC
#define c_LPCBDDR		DDRB
#define c_LPCBPORT		PORTB

/* Port B */
#define b_LFRAME		PB1
#define b_LCLK			PB6
/* Port C */
#define b_LAD3 			PC3
#define b_LAD2 			PC2
#define b_LAD1 			PC1
#define b_LAD0 			PC0

/* Used with SPI */
/**************** SPI Configuration I/O ***********/
#define	c_SPIPIN	    PINB
#define	c_SPIPORT	    PORTB
#define	c_SPIDDR	    DDRB

#define	b_SPIMOSI		5		
#define	b_SPIMISO		6
#define	b_SPICLK		7


/*************** UART Buffer Sizes ******************/
#define c_RXBUFLEN	16 // 16
#define c_TXBUFLEN	64  // 16

/**************** PWM ****************************/
#define	c_PWMPIN	    PIND
#define	c_PWMPORT	PORTD
#define	c_PWMDDR	DDRD


#define	b_PWM1		4
#define	b_PWM2		5

/**************** SCARD ****************************/
#define c_SCARDPIN      	PINC 
#define c_SCARDPORT     	PORTC
#define c_SCARDDDR      	DDRC

#define b_CRST				0		// Card RST
#define b_CCLK				1		// Card Clock
#define b_CDAT				2		// Card Data	
#define b_NCARDIN     	    3       // Card inserted


/*************** ADC Channels ******************/
#define c_MAXADCCH 		8

/* Possible settings for Vref - do not change */
#define VREF_AREF		0x00
#define VREF_AVCC		0x40
#define VREF_RESV		0x80
#define VREF_256V		0xC0
/* Data adjustment right/Left */
#define DATA_RADJ		0x00
#define DATA_LADJ		0x20
/* reference channels */
#define CHANREF_122V	0x1E
#define CHANREF_GND		0x1F

/* ADC Prescaler */
#define ADCPRSC_2		0x01
#define ADCPRSC_4		0x02
#define ADCPRSC_8		0x03
#define ADCPRSC_16	    0x04
#define ADCPRSC_32	    0x05
#define ADCPRSC_64	    0x06
#define ADCPRSC_128	    0x07

/* selected Setting for Vref */
#define ADC_VREF_MODE	VREF_AREF
#define ADC_DATA_ADJ	DATA_RADJ

/*************** Matrix Keyboard 4x4 ******************/
#define c_ROWDDR		DDRC
#define c_COLDDR		DDRB
#define c_ROWPORT		PORTC
#define c_COLPORT		PORTB
#define c_ROWPIN		PINC
#define c_COLPIN		PINB
#define c_ROWMASK		0x0f
#define c_ROWIMASK		0xf0
#define c_COLMASK		0xf0
#define c_COLIMASK		0x0f
#define c_NOKEY			0xff

#define c_KEYBUFSIZE	4
#define c_KEYDEBLIMIT	3

/*************** SysStat Register ******************/
#define b_SysTick				0
#define b_DBG					1
#define b_AppTick				2
#define b_ADCTick				3
#define b_LCDBusy				4



/*************** StatReg Register ******************/
#define b_FPGACfg	0


/*****************************************/
/* Firmware Settings */
#define PRSC1              1
#define PRSC1_SELECT       1               /* divide by 1 */

