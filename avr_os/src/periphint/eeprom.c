/***************************************************************************
 Project:		AVRILOS
 Title:			EEPROM Functions
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/15/ATMEGA163/103/16/323/164p
 File:			eeprom.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* EEPROM access functions
***************************************************************************/

#include "../includes/types.h"
#include "../includes/settings.h"

#include <avr/eeprom.h> 

#ifndef MOD_EEPROM_ON
INT8U f_EERead(INT8U v_addr) { return 0; }
void f_EEWrite(INT8U v_addr, INT8U v_data) {}

#else

INT8U f_EERead(INT8U v_addr)
{
    INT8U v_data;
    
    v_data = eeprom_read_byte ((INT8U *) v_addr);
    return v_data;
}

void f_EEWrite(INT8U v_addr, INT8U v_data)
{
 	eeprom_write_byte ((INT8U *) v_addr, v_data);
}

#endif


