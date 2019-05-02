
/*****************************************************************
 				FPGA CONFIGURATION PARAMETERS
				Ports for FPGA Config CFGPORT
******************************************************************/


#define ToggleCCLK do{ \
					cbi(c_CFGPORT, b_CClk); \
					sbi(c_CFGPORT, b_CClk);	\
					}while(0)


#define	SetRst 		do{ sbi(c_CFGPORT, b_Reset);}while(0)
#define	ClrRst		do{ cbi(c_CFGPORT, b_Reset);}while(0)
#define SetCCLK		do{ sbi(c_CFGPORT, b_CClk) ;}while(0)
#define	ClrCCLK		do{ cbi(c_CFGPORT, b_CClk) ;}while(0)
#define	SetDone		do{ sbi(c_CFGPORT, b_Done) ;}while(0)
#define	ClrDone		do{ cbi(c_CFGPORT, b_Done) ;}while(0)
#define	SetDin		do{ sbi(c_CFGPORT, b_Din)  ;}while(0)
#define	ClrDin		do{ cbi(c_CFGPORT, b_Din)  ;}while(0)
#define	DoneInput	do{ cbi(c_CFGDDR, b_Done)  ;}while(0)
#define	DoneOutput	do{ sbi(c_CFGDDR, b_Done)  ;}while(0)
#define	DinInput	do{ cbi(c_CFGDDR, b_Din)   ;}while(0)
#define	DinOutput	do{ sbi(c_CFGDDR, b_Din)   ;}while(0)
#define	CCLKInput	do{ cbi(c_CFGDDR, b_CClk)  ;}while(0)
#define	CCLKOutput	do{ sbi(c_CFGDDR, b_CClk)  ;}while(0)
#define ResetInput	do{ cbi(c_CFGDDR, b_Reset) ;}while(0)
#define	ResetOutput	do{ sbi(c_CFGDDR, b_Reset) ;}while(0)

