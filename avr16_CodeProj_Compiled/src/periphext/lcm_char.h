#define c_RAM		0
#define c_EEPROM	1
#define c_FLASH		2

/***********************
;LCM 4 line addresses
;Line 1: 0x00
;Line 2: 0x40
;Line 3: 0x14
;Line 4: 0x54
;------------------------
;LCM 2 line addresses
;Line 1: 0x00
;Line 2: 0x40
***********************/

#define c_LCML1		0x00
#define c_LCML2		0x40
#define c_LCML3		0x14
#define c_LCML4		0x54

void f_ConfigLCM(void);
void f_LCM_Timer(void);
INT8U f_Wait_ms(INT8U del); 
void f_LCM_ClrDisp(void);
void f_LCM_DispOff(void);
void f_LCM_DispOn(void);
void f_LCM_WrCharData(INT8U data);
void f_LCM_set_DDRAM_Addr(INT8U addr);
void f_LCM_OutNibble(INT8U n, INT8U rs);
INT8U f_LCM_InNibble(INT8U rs);
void f_LCM_OutByte(INT8U b, INT8U rs);
INT8U f_LCM_InByte(INT8U rs);
void f_LCM_WaitRdy(void);

void f_LCM_WrMsg(INT8U pos, INT8U *p_msg, INT8U memtype);
void f_LCM_WrMsgUnBuf(INT8U pos, INT8U *p_msg, INT8U memtype);
void f_LCM_PutChar(INT8U ch);


#define f_LCM_WrMsgUnBufF(pos, string)	(f_LCM_WrMsgUnBuf(pos, PSTR(string), c_FLASH))
#define f_LCM_WrMsgF(pos, string)		(f_LCM_WrMsg(pos, PSTR(string), c_FLASH))


#define lcd_outcmd(n) f_LCM_OutByte(n, 0)
#define lcd_outdata(n) f_LCM_OutByte(n, 1)
#define lcd_incmd() f_LCM_InByte(0)
#define lcd_indata() f_LCM_InByte(1)
