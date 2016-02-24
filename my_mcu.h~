/*
  File: mcu.h
  
  Description: Contains all generic board configuration functions. Modify functions as needed
  to support or exclude various peripherals. Expect this file to be volatile.
  Includes user code like led's, string clears, etc. and UART stuff as well.
  
  Contains ISR's, since they are tightly coupled with a lot of config code.
*/

#ifndef MY_MCU_H
#define MY_MCU_H


#include <peripheral/ports.h>    // PORTSetPinsDigitalIn (), PORTSetPinsDigitalOut (), PORTRead (), PORTWrite (), BIT_XX, IOPORT_X
#include <peripheral/uart.h>    // UART lib:
#include <peripheral/timer.h>    // Timer lib:
#include <peripheral/outcompare.h>  // Output Compare lib:
#include <peripheral/int.h>      // Interrupt lib:
#include <peripheral/incap.h>    // Input capture lib:
#include <peripheral/system.h>    // Set up the system and perihperal clocks for best performance
#include <proc/p32mx460f512l.h>   // Vector table constants
#include <sys/attribs.h>       // for ISR...???
//#include <stdlib.h>            //sprintf, etc.
//#include <stdio.h>
//#include <peripheral/i2c.h>      // I2C functions for IMU
#include "my_types.h"


#define SYSTEM_CLOCK        80000000ul
#define PERIPHERAL_CLOCK    40000000ul
#define I2C_CLOCK_FREQ      100000
#define DESIRED_DATA_RATE   115200  //default for pmod bt2 is 115200
#define U1_INT_PRI          6
#define PR_BASE             256
#define TINY_DLY            128
#define VERYSHORT_DLY       4096
#define SHORT_DLY           8192
#define MEDIUM_DLY          32768
#define LONG_DLY            65000
#define INT_DISABLE         0
#define DC_ZeroPct      0
#define DC_50Pct      128
#define DC_60Pct      154
#define DC_65Pct      166
#define DC_70Pct      179
#define DC_75Pct      192
#define DC_80Pct      205
#define DC_90Pct      230
#define I2C_CONFIG_FLAGS  0    //no I2C config bits seem to apply to normal usage

//void __ISR_AT_VECTOR(_TIMER_3_VECTOR, ipl1) vT3InterruptHandler(void);


void configurePortIO(void);
void configOCModule2(void);
void configOCModule3(void);
void configureUART1(void);
void enableUART1(void);
void configureTimer2(int Period);
void configureTimer3(int Period);
void configureTimer4(int Period);
void configureInterrupts(void);
void configureInputCaptureMods(void);

void delayMS(int ms);
void delayUS(int us);
void delay(unsigned int ms);
void sendUART1(char* data, int nbytes);
void writeOC2(unsigned int newOC2RValue, unsigned int newOC2RSValue);
void writeOC3(unsigned int newOC3RValue, unsigned int newOC3RSValue);
void setMotorsForward(void);
void setMotorsReverse(void);
void clearStr(unsigned char str[], int len);
void my_memcpy(unsigned char* dst, unsigned char* src, int nbytes);


void clearLEDs(void);
void output_led (short index);
void winSequence(void);
void loseSequence(void);
void setupUART1 (unsigned int pb_clock); //Andy version of UART setup
void sigError(void);
void dumbInit(void);

#endif
