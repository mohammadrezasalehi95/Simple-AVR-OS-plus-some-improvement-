/***************************************************************************
 Project:		AVRILOS
 Title:			SST 49F020A Flash memory
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		NA
 File:			sst.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Flash SST 49F020A access routines using LPC bus 
* 
***************************************************************************/

#include "../includes/types.h"
#include "../includes/settings.h"

#include "../periphext/lpc.h"
#include "sst.h"

void f_SSTWrite(INT32U addr, INT8U data)
{
	f_LPC_Write(0x00005555,0xAA);
	f_LPC_Write(0x00002AAA,0x55);
	f_LPC_Write(0x00005555,0xA0);
	f_LPC_Write(addr,data);
}

INT16U f_SSTID(void)
{
	INT16U id;
	
	/* Enter ID Mode */
	f_LPC_Write(0x00005555,0xAA);
	f_LPC_Write(0x00002AAA,0x55);
	f_LPC_Write(0x00005555,0x90);
	/* Get ID */
	id = f_LPC_Read(0x00000001);
	id = (id << 8) | f_LPC_Read(0x00000000);
	/* Exit ID Mode */
	f_LPC_Write(0x00005555,0xF0);
	return id;
}

/* 49F020 has 16 blocks (16KB each block -> 4 sectors 4KB each Sector) */
void f_SSTErase(INT16U base_addr, INT8U type)
{
	INT32U addr = base_addr;
	
	f_LPC_Write(0x00005555,0xAA);
	f_LPC_Write(0x00002AAA,0x55);
	f_LPC_Write(0x00005555,0x80);
	
	f_LPC_Write(0x00005555,0xAA);
	f_LPC_Write(0x00002AAA,0x55);
	
	if (type == c_SSTBLKERASE)			f_LPC_Write(addr, 0x50);
	else if (type == c_SSTSECERASE)		f_LPC_Write(addr, 0x30);
	else if (type == c_SSTCHIPERASE)	f_LPC_Write(addr, 0x10);
}



