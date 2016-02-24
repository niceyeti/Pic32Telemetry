#ifndef MY_ISR_H
#define MY_ISR_H

//#include <p32xxxx.h>
//#include <sys/asm.h>
//#include "FreeRTOS/portable/MPLAB/PIC32MX/ISR_Support.h"
#include "my_mcu.h"

#define MOTOR_CORRECTION_GAIN 0.03

//void __ISR_AT_VECTOR(_TIMER_3_VECTOR, ipl1) vT3InterruptHandler(void);
//void __attribute__( (interrupt(ipl1), vector(_TIMER_3_VECTOR))) vT3InterruptHandler( void ); // this prototype just has to be here. The priority is overridden by rtos. See http://www.freertos.org/port_PIC32_MIPS_MK4.html#DemoApp 
//void __ISR(_TIMER_3_VECTOR, IPL6AUTO) vT3InterruptHandler(void) // SW1

#endif
