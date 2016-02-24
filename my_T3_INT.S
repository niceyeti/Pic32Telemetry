// -----------------------------------------------------------------------------
//                    Interrupt Wrapper for Timer 2
// -----------------------------------------------------------------------------
//void __attribute__( (interrupt(ipl1), vector(_TIMER_2_VECTOR))) vT3InterruptHandler( void ); // this prototype just has to be here. The priority is overridden by rtos. See http://www.freertos.org/port_PIC32_MIPS_MK4.html#DemoApp 
//void __ISR(_TIMER_3_VECTOR, IPL6AUTO) vT3InterruptHandler(void) // SW1



#include <p32xxxx.h>
#include <sys/asm.h>
#include "FreeRTOS/portable/MPLAB/PIC32MX/ISR_Support.h"

//#include "my_isr.h"

//void __ISR_AT_VECTOR(_TIMER_3_VECTOR, ipl1) vT3InterruptHandler(void);

	.set	nomips16
	.set 	noreorder
 	
	.extern vT3InterruptHandler
	.extern xISRStackTop
	.global	vT3InterruptWrapper

	.set	noreorder
	.set 	noat
	.ent	vT3InterruptWrapper

vT3InterruptWrapper:

	portSAVE_CONTEXT
	jal vT3InterruptHandler
	nop
	portRESTORE_CONTEXT

	.end	vT3InterruptWrapper

