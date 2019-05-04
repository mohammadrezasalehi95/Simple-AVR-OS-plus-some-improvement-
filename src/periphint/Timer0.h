
#include "../includes/ifc_time.h"

void f_InitSysTimer(void);
void f_SysTick(void);
void f_StartTimerLCM(void);
void f_StopTimerLCM(void);
void f_StartTimerKey(void);
void f_StopTimerKey(void);
void f_SystickSetErrLevel(INT16U v_errorlevel);

extern INT16U v_SwTimer_mS[c_MAXSWTIMERS];