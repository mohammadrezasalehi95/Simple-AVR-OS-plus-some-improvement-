/***************************************************************************
 Project:		AVRILOS
 Title:			Simple Smart Card Reader
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			scard.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Smart Card Reader (Simple PROM types) 
* 
***************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../includes/types.h"
#include "../includes/settings.h"
#include "../Utils/delay.h"
#include "../Utils/typeconv.h"
#include "../periphint/Uart.h"
#include "scard.h"

extern INT8U v_SysStat;

#define SetCCLK      do{ sbi(c_CFGPORT, b_CClk) ;}while(0)
#define ClrCCLK      do{ cbi(c_CFGPORT, b_CClk) ;}while(0)

#define c_CARDDEBOUNCE_VAL      5

#ifndef MOD_SCARD_ON

void f_Init_SCard(void) {} 
void f_SCard_CheckCardIn(void) {}
void f_SCard_MatchID(void) {}
void f_SCard_CCLK(void) {} 
void f_SCard_RstCard(void) {}
void f_SCard_Read(INT8U * p_data) {}

#else
// Put here your own card IDs
INT8U __attribute__ ((progmem)) lut_SCARDID[]={ 
    0x28,0x76,0xFE,0xF5,0x0E,0x3D,0xFE,0xC1,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0xFE, // card 1
    0xD7,0x85,0xFE,0xF6,0x12,0xB2,0x01,0x4A,0x00,0x00,0x00,0x1E,0xFE,0xFE,0xFE,0xFE  // card 2
    };

volatile INT8U v_CardStatus;

//void f_SCard_Read(INT8U * p_data);

void f_Init_SCard(void)
{
    // Output: CRST, CCLK
    // I/O (Read = Input) CDAT
    // Input NCARDIN
    c_SCARDDDR |= _BV(b_CRST) | _BV(b_CCLK);
    c_SCARDDDR &= ~(_BV(b_CDAT) | _BV(b_NCARDIN));
    c_SCARDPORT |= _BV(b_NCARDIN); // pull=up
    c_SCARDPORT |= _BV(b_CDAT);
    v_CardStatus = c_CARDDEBOUNCE_VAL;
}


INT8U f_SCard_CheckCardIn(void)
{
    INT8U v_result;
    
    v_result = c_CARD_NONE;
    
    if ( (c_SCARDPIN & _BV(b_NCARDIN)) == 0)
    {        
        if (v_CardStatus == 1)
        {   // card debounce done, match
            //f_Uart_PutStr("Card Inserted!\n");            
            v_result = f_SCard_MatchID();
            v_CardStatus = 0;
        }
        else if (v_CardStatus == 0)
        {   // card already read
            v_CardStatus = 0;
            v_result = c_CARD_READ;
        }
        else
        {   // debounce switch
            v_CardStatus--;
            v_result = c_CARD_DBNCE;
        }
        
    } 
    else 
    {
        v_CardStatus = c_CARDDEBOUNCE_VAL;
    }

    return v_result;
}


INT8U f_SCard_MatchID(void)
{
    INT8U buf_data[16];
    INT8U v_result;
    INT8U v_idx, v_scards;
    INT8U *p_data;
    INT8U v_card_data;
    
    
    v_result = c_CARD_IDBAD;
    
    f_SCard_Read(&buf_data[0]);
    
    p_data = (INT8U *) (&lut_SCARDID[0]);
    
    // for(i=0; i<16; i++) f_Uart_PutChar(buf_data[i]);
    for(v_scards = 0;  v_scards < (sizeof(lut_SCARDID)/(16*sizeof(INT8U))); v_scards++)
    {
        for(v_idx=0; v_idx<16; v_idx++)
        {
            v_card_data = PRG_RDB((p_data+v_scards*16+v_idx));            
            if (buf_data[v_idx] != v_card_data) break;
        }
        f_Uart_PutStr("\n");
        if (v_idx == 16) v_result = c_CARD_IDOK;
    }
    
	return v_result;
	
}

void f_SCard_CCLK(void)
{
    c_SCARDPORT |= _BV(b_CCLK);
    f_Delay_us(40);
    c_SCARDPORT &= ~(_BV(b_CCLK));
    f_Delay_us(40);
}

void f_SCard_RstCard(void)
{
    c_SCARDPORT |= _BV(b_CRST);
    f_Delay_us(200);
    f_SCard_CCLK();
    c_SCARDPORT &= ~(_BV(b_CRST));
    
}

void f_SCard_Read(INT8U * p_data)
{
    INT8U v_bytes, v_bits;
    INT8U v_currbyte;
    INT8 buf_byte[3];
    
    buf_byte[2] = 0;
    
    f_SCard_RstCard();
    for(v_bytes=0;v_bytes<16; v_bytes++)
    {
        v_currbyte = 0;
        for(v_bits=0;v_bits<8;v_bits++)
        {
            if ( (c_SCARDPIN & _BV(b_CDAT)) != 0) v_currbyte |= 1;
            v_currbyte = v_currbyte << 1;
            f_SCard_CCLK();
        }
        *(p_data+v_bytes) = v_currbyte;
        f_BinHex(v_currbyte, &buf_byte[0]);        
        f_Uart_PutStr(buf_byte);
    }
        
}
#endif


