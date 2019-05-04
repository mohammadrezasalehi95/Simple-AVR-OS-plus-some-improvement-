void f_DebugFPGAWr(INT8U addr, INT8U data);
INT8U f_DebugFPGARd(INT8U addr);
void f_Dbg_User_Cmd(void);
INT8U f_DebugLCMRd(INT8U addr);
void f_DebugLCMWrCmd(INT8U data);
void f_DebugLCMWrData(INT8U data);
void f_DebugLPCWr(INT32U addr, INT8U data);
void f_DebugSSTWr(INT32U addr, INT8U data);
INT8U f_DebugLPCRd(INT32U addr);


