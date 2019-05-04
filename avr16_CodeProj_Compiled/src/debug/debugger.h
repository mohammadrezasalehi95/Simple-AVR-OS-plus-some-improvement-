void f_debug_readbyte    (void);
void f_debug_writebyte   (void);
void f_debug_asctab      (void);
void f_debug_bintab      (void);
void f_debug_PinA        (void);
void f_debug_PinB        (void);
void f_debug_PinC        (void);
void f_debug_PinD        (void);
void f_debug_readEEByte  (void);
void f_debug_writeEEbyte (void);
void f_debug_fpga_read   (void);
void f_debug_fpga_write  (void);
void f_debug_user_cmd    (void);
void f_debug_lcm_wrdata	 (void);
void f_debug_lcm_wrcmd	 (void);
void f_debug_lcm_read	 	 (void);
void f_debug_Port_write	 (void);
void f_debug_Port_read	 (void);
void f_debug_AnalogRead  (void);
void f_debug_lpc_read 	 (void);
void f_debug_lpc_write	 (void);
void f_debug_sst_write	 (void);
void f_debug_disable	   (void);
void f_debug_adis_read	(void);
void f_debug_adisbias_read(void);
void f_debug_adis_write(void);
void f_debug_adis_rd(void);
void f_debug_spi_rd(void);



void f_InitDebug(void);
void f_TaskDebugger(void);
void f_CheckSyntax(void);
void f_ProcessCMD(void);
void f_debug_port(INT8U data);
void f_debug_port_v(INT8U data);