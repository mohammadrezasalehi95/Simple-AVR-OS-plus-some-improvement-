/***************************************************************************
 Project:		AVRILOS
 Title:         Example Lock Application
 Author:		Ilias Alexopoulos
 Version:		2.00
 Last updated:	28-Oct-2010
 Target:		AT90S8535/ATMEGA163
 File:			applic.c

* Support E-mail:
* avrilos@ilialex.gr
* 
* license: See license.txt on root directory (CDDL)
*

* DESCRIPTION
* Demo Application
* Using Smart card and a Servo to lock/unlock
*
* You should program the lut_SCARDID in scard.c file constant with your card data.
* Initially you have just to enter the card and the serial number 
* will show up at your serial terminal. Then you have to program this
* value at the lut_SCARDID. Currently the lut_SCARDID contains two random
* cards numbers.
***************************************************************************/

#include <avr/eeprom.h>
#include <avr/io.h>
#include "../debug/debugger.h"
#include "../includes/types.h"
#include "../includes/settings.h"
#include "../Utils/delay.h"
#include "../periphint/Uart.h"
#include "../periphint/Timer0.h"
#include "../periphint/Timer1.h"
#include "../periphext/scard.h"
#include "applic.h"

// State machine of application
typedef enum {
    state_IDLE, state_WaitTO2Lock, state_ARM, state_WaitTO2UnLock
} type_Lock_state;

// Current state machine variable                
volatile type_Lock_state state_Lock;


// LED ARM
#define c_ARMLEDPORT   PORTB
#define c_ARMLEDPIN    PINB
#define c_ARMLEDDDR    DDRB
#define b_ARMLed       3

#define SetLED_ARM_Out        do{ c_ARMLEDDDR |=  _BV(b_ARMLed) ;} while(0)
#define SetLED_ARM_Set        do{ c_ARMLEDPORT |=  _BV(b_ARMLed) ;} while(0)
#define SetLED_ARM_Clr        do{ c_ARMLEDPORT &=  ~_BV(b_ARMLed) ;} while(0)


// Assign SW timer 0 for timing operations
// We just need only one timer
#define c_TO_TIMER            0

// Timeouts we are using in our application by timer 0
#define c_WAIT2LOCK_03sec    3000
#define c_WAIT2UNLOCK_03sec  3000

// Servo Position control (PWM value)
// Should be set to your actual physical motion
// that the servo have to do
// You might need to adjust these depending your mechanical setup
#define c_SERVOLock            50 // 1st position
#define c_SERVOUnLock          30 // 2nd position

// You can modify these values on the fly through the debugger
// And control Servo final position
volatile INT8U v_ServoLock;
volatile INT8U v_ServoUnLock;
extern INT8U app_subscription;
void f_LockFSM(void);

void f_LockLock(void);

void (*f_Applic_function[8])(void) =
        {f_Applic, my_Applic, my_Applic2, my_Applic3,
         my_Applic4, my_Applic5, my_Applic6, my_Applic7};

void applications_reducer(INT8U num) {
    if (app_subscription&(1<<num)){
        (*f_Applic_function[num])();
    }
}

void f_Applic(void) {
    // Execute the State Machine
    f_LockFSM();
}

INT8U c = 0;
void my_Applic(void) {
    c++;
    f_Delay_ms(100);
    outp(c, PORTA);
}
INT16U start2=0x0171;
INT16U end2=0x0179;
INT8U i2 = 0;
void my_Applic2(void) {
    f_Uart_PutStr("\ncode 108a\n");
        INT8U v_data;
        v_data = *(INT8U *)(start2+i);
        f_debug_port(v_data);
        if (start2+i2==end2){
            i2=0;
            f_Uart_PutStr("\ncode 113a\n");
        }
}

void my_Applic3(void) {

}

void my_Applic4(void) {

}

void my_Applic5(void) {

}

void my_Applic6(void) {

}

void my_Applic7(void) {
}


// Application Inititialization called before Kernel main loop
void f_Init_Applic(void) {
    // initialize state machine
    state_Lock = state_IDLE;
    // SW Timer inactive
    v_SwTimer_mS[c_TO_TIMER] = 0;
    // Initialize PWM channel for Servo Control
    // We are using Channel 0.
    f_InitPWM1();

    // Initialize variables
    v_ServoLock = c_SERVOLock;
    v_ServoUnLock = c_SERVOUnLock;

}

// Function called by the main kernel loop (task)
// Should not block



// Utility functions needed by the FSM (Finite State Machine)
void f_LockLock(void) {
    f_PWM1Enable(0);
    f_PWM1Set(0, v_ServoLock);
    // Debug info to serial port
    f_Uart_PutStr("\nLock\n");
}

void f_LockIdle(void) {
    // If you need lower power consumption
    // You may disable the PWM during idle
    // The servo will stay in its last position
    // Un-comment the following line to do this
    //f_PWM1Disable(0);
    // Debug info to serial port
    f_Uart_PutStr("\nLock Idle\n");
}

void f_LockUnLock(void) {
    f_PWM1Enable(0);
    f_PWM1Set(0, v_ServoUnLock);
    // Debug info to serial port
    f_Uart_PutStr("\nUnlock\n");
}

// Main control FSM (Finite State Machine)
void f_LockFSM(void) {
    INT8U v_status;

    switch (state_Lock) {
        case state_IDLE:
            // If smart card inserted and ID matches then go to lock state
            v_status = f_SCard_CheckCardIn();
            if (v_status == c_CARD_IDOK) {
                // next state to go on next iteration
                state_Lock = state_WaitTO2Lock;
                // Activate servo lock position
                f_LockLock();
                // In order to Wait to lock, activate SW timer with time to wait
                v_SwTimer_mS[c_TO_TIMER] = c_WAIT2LOCK_03sec;
            } // Else wait here for ever            
            break;

        case state_WaitTO2Lock:
            if (v_SwTimer_mS[c_TO_TIMER] == 0) { // finished TO, servo locked
                state_Lock = state_ARM;
                // disable servo during idle
                f_LockIdle();
            }
            break;

        case state_ARM:
            // If smart card inserted and ID matches then go to lock state
            v_status = f_SCard_CheckCardIn();
            if (v_status == c_CARD_IDOK) {
                // next state to go on next iteration            
                state_Lock = state_WaitTO2UnLock;
                // Activate servo to unlock position
                f_LockUnLock();
                // In order to Wait to wait for motion completion, activate SW timer with time to wait
                v_SwTimer_mS[c_TO_TIMER] = c_WAIT2UNLOCK_03sec;
            }
            break;

        case state_WaitTO2UnLock:
            if (v_SwTimer_mS[c_TO_TIMER] == 0) { // finished TO, servo should have finished its motion
                // return to idle position
                state_Lock = state_IDLE;
                f_LockIdle();
            }
            break;

        default:
            // just in case we fall over an unknown state
            state_Lock = state_IDLE;
            break;
    }


}
    





