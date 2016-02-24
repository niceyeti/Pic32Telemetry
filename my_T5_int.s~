// -----------------------------------------------------------------------------
//                    Interrupt Wrapper for Timer 5
// -----------------------------------------------------------------------------
#include <p32xxxx.h>
#include <sys/asm.h>
#include "FreeRTOS/portable/MPLAB/PIC32MX/ISR_Support.h"

	.set	nomips16
	.set 	noreorder
 	
	.extern vT5InterruptHandler
	.extern xISRStackTop
	.global	vT5InterruptWrapper

	.set	noreorder
	.set 	noat
	.ent	vT5InterruptWrapper

vT5InterruptWrapper:

	portSAVE_CONTEXT
	jal vT5InterruptHandler
	nop
	portRESTORE_CONTEXT

	.end	vT5InterruptWrapper

