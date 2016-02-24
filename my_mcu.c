#include "my_mcu.h"

void my_memcpy(unsigned char* dst, unsigned char* src, int nbytes)
{
  int i;

  for(i = 0; i < nbytes; i++, dst++, src++){
    *dst = *src;
  }
}

void clearStr(unsigned char str[], int len)
{
  int i = 0;

  while( i < len ){
    str[i++] = '\0';
  }
}

/*

removed to motors.c
//Common bug: writing direction to direction register; remember the logic state of the pins determines direction, so write to PORTx reg, not TRISx.
void setMotorsReverse(void)
{
  SetPulseOC2(DC_ZeroPct, DC_ZeroPct);
  SetPulseOC3(DC_ZeroPct, DC_ZeroPct);
  delay(TINY_DLY);

  PORTWrite(IOPORT_D, BIT_7);
  PORTClearBits(IOPORT_D, BIT_6);

  /*
  //PORTSetPinsDigitalOut(IOPORT_D, BIT_6);    //Invert bits 6 and 7 on RD port
  //PORTWrite(IOPORT_D, BIT_7 | BIT_6);

  }

void setMotorsForward(void)
{
  SetPulseOC2(DC_ZeroPct, DC_ZeroPct);
  SetPulseOC3(DC_ZeroPct, DC_ZeroPct);
  delay(TINY_DLY);
  PORTWrite(IOPORT_D, BIT_6);
  PORTClearBits(IOPORT_D, BIT_7);

  /*
  PORTSetPinsDigitalOut (IOPORT_D, BIT_7 | BIT_6);
  //PORTSetPinsDigitalIn (IOPORT_D, BIT_6);
  
}
*/

/*******************************************************************
 Function: void winSequence(void)
 * Returns: void
 * Input Parameters: void
 * OutParams: void
 * Description: Outputs a friggin awesome win sequence.
 ********************************************************************/
void winSequence(void)
{
    short count = 0;

    while(count < 12)
    {
        PORTClearBits(IOPORT_B, BIT_1 | BIT_3);
        PORTSetBits(IOPORT_B, BIT_0 | BIT_2);
        delay(LONG_DLY);
        clearLEDs();
        PORTClearBits(IOPORT_B, BIT_0 | BIT_2);
        PORTSetBits(IOPORT_B, BIT_1 | BIT_3);
        delay(LONG_DLY);
        clearLEDs();
        count++;
    }
}

/*
  Delay for some matter of milliseconds based on system clock.
  Needs calculation.
*/
void delayMS(int ms)
{
  long int i, j, clkPerMs, scaledRate, scaledMS;

  clkPerMs =  SYSTEM_CLOCK / 1000;  //for 80 MHz, clockPerMS == 80,000
  scaledRate = clkPerMs / 4;  //this is because the inner loop must break every 1 ms, and on every iteration it executes three ops.
  scaledMS = ms / 16;

  //really this should execute for ms * 5 times, due to comparison and increment ops in the loops
  for(i = 0; i < scaledMS; i++){  //outer loop count ms
    for(j = 0; j < scaledRate; j++){  //inner loop iterates for (0.001 seconds) number of times
      Nop();
    }
  }
}

void delayUS(int us)
{
  int i = 0, j = 0;

  //80 MHz sysclock gives 80 clks per microsecond
  for(i = 0; i < 80; i++){
    for(j = 0; j < us; j++){
      Nop();
    }
  }
}

/*******************************************************************
 Function: void loseSequence(void)
 * Returns: void
 * Input Parameters: void
 * OutParams: void
 * Description: This function displays a lose sequence for big losers.
 ********************************************************************/
void loseSequence(void)
{
    unsigned int new_state = 0;
    short i = 0, j = 0;

    while(i < 4)
    {
        while(j < 4)
        {
            output_led(j % 4);
            delay(SHORT_DLY);
            j++;
        }

        clearLEDs();
        delay(TINY_DLY);
        j = 3;

        while(j >= 0)
        {
        //scroll left
            output_led(j % 4);
            delay(SHORT_DLY);
            j--;
        }
        clearLEDs();
        delay(TINY_DLY);
        i++;
    }

    clearLEDs();
}

/****************************************************************************************
 Function: void delay(unsigned int ms)
 Returns: void
 Input Parameters: u_int ms (amount of delay)
 Description: This function delays operations for a specified number of instruction cycles, by passing the assembly instruction
 NOP (no-operation) to the system. This causes the CPU to execute no-operation for the number of cycles specified by the
 input parameter "ms", an unsigned integer type.  Recall the max size of an integer is 65,535 and the PIC CPU runs at around
 80 MHz, or 80 million cycles per second.  So if we pass 65535 into this function, the maximum delay is about 65535 / 80 million,
 or 0.8 milliseconds.  So how do we get a longer delay?  Simply nest another loop inside the main loop of this function to get
 ms^2 (ms squared) performance, or likewise you can call the function itself from within a loop.
 Usages: delaying, debouncing (see wikipedia "debouncing")
 Preconditions: None
 Postconditions: A timed delay occurs
 ****************************************************************************************/
void delay(unsigned int ms)
{
    unsigned int count = 0;

    for(count = 0; count < ms; count++)
    {
        asm("NOP");
    }

    /* Alternative counter loop template for longer delays: nested loops can square the number of overall loop iterations,
       so this loop will execute a maximum of 65535 * 65535 times, or about 4 billion times.  Dividing this value by our
       clock frequency of 80 MHz gives a delay of around 53 seconds, clearly a very long (maximum) delay.

    unsigned int count = 0;
    unsigned int inner_count = 0;

    for(count = 0; count < ms; count++)
    {
      for(inner_count = 0; inner_count < ms; inner_count++)
      {
        asm("NOP");
      }
    }
    */
}


/*****************************************************************************************
 Function: configurePortIO(void)
 Description: This function sets up the pins to the on-board LEDs as output pins

 Notes: may need to call AD1CFG, as in original assembly, to set analog ports to digital???

    Mapping: Arrows indicate software/PIC mapping to physical ports on Cerebot board.
    OCx (output compare) port-locations drive the motors.

    Left motor HB5:   (PIC)  (Cerebot)
              OC3  ->  RD2  (output waveform toggles the H-bridge enable)
              Dir  ->  RD6  (direction)
              IC3 ->   RD10
              Vdd  ->  Vdd
              Gnd  ->  Gnd

    Right motor HB5:   (PIC)  (Cerebot)
              OC2  ->  RD1
              Dir  ->  RD7  (direction)
              IC2 ->  RD9
              Vdd  ->  Vdd
              Gnd  ->  Gnd

    UART(B-Tooth):  (PIC)  (Cerebot)
              U1TX ->  RF02
              U1RX ->  RF08

    I2C2 (if used):
              SCL2 -> RA02  (no other peripherals use RAxx, so it is the least interdependent port source for I2C, esp with freeRTOS)
              SDA2 -> RA03
              
    I2C1 (if used):
              SCL1 -> RA14
              SDA1 -> RA15
              From the PIC32 IC2 manual (sect 24):
              In I2C mode, pin SCL is clock and pin SDA is data. The module will override the data direction
              bits (TRIS bits) for these pins.

    SPI 2:    SDI2  -> RG7
              SDO2  -> RG8
              CLK2  -> RG6
              SS2   -> RG9 [configured for 1MHz operation in spiInitialize() ]

   ~~~~~~~!!! PORT E IS FOR 5 VOLT ONLY !!!~~~~~~~
   !  HC-SR04 Ping))) Sensor:
   !     RE03 == TRIG
   !     RE02 == ECHO
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


 *****************************************************************************************/
void configurePortIO (void)
{
  //unsigned int portState = 0;

  /******************************************************************************
  WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!

  WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!

  PORTE is set to 5 volts for the ping sensor peripheral: attach no other pmods to
  any PORTE port, unless 5 volt.
  ******************************************************************************/

  //Ping Sensor configuration. Uses Peripheral ports RE02 and RE03 just as basic ports (no special PIC internal peripherals)
  //SEE WARNING ABOVE: port E is 5 volt ONLY
    PORTSetPinsDigitalOut (IOPORT_E, BIT_3);
    PORTSetPinsDigitalIn (IOPORT_E, BIT_2);  //set all remaining RE pins as inputs
    PORTEbits.RE3       = 0;  //send all outputs low initially

  //Motor configuration:   SET: BIT_7 (RD7)    CLEAR: BIT_6 (RD6), BIT_2 (OC3/RD2), BIT_1 (OC2/RD1)
    PORTSetPinsDigitalOut (IOPORT_D, BIT_7 | BIT_6 | BIT_2 | BIT_1);
    PORTSetPinsDigitalIn (IOPORT_D, BIT_10 | BIT_9);

  //set BIT_6 to 1 (left motor direction), BIT_7 (and all others) to 0 (right motor direction)
    PORTWrite(IOPORT_D, BIT_6);
    PORTClearBits(IOPORT_D, BIT_7);

  //UART configuration: Set BIT_8 to 1 (RF02), clear BIT_2 to 0 (RF08)
    PORTSetPinsDigitalOut (IOPORT_F, BIT_8);
    PORTSetPinsDigitalIn (IOPORT_F, BIT_2);

  //LED signal configuration: set all Port_B as output. BIT_3 output is for HC-SR04 sensor TRIG signal.
    PORTSetPinsDigitalOut (IOPORT_B, BIT_10 | BIT_11 | BIT_12 | BIT_13);
  //  PORTSetPinsDigitalIn (IOPORT_B, BIT_12 );  //BIT_12 input is for ECHO signal from HC-SR04

  //I2C1 setup    SCL1 = RA14    SDA1 = RA15
    PORTSetPinsDigitalIn(IOPORT_A, BIT_14 | BIT_15);  //see header: the manual says this I/O will be appropriately overridden by the IC2 module itself
 //  why was SCL being set as input, above??
  
  //I2C2 setup    SCL2 = RA02    SDA2 = RA03
    PORTSetPinsDigitalIn(IOPORT_A, BIT_3 | BIT_2);  //see header: the refmanual says this I/O will be appropriately overridden by the IC2 module itself
    //PORTSetPinsDigitalOut(IOPORT_A, BIT_2);
  
  //SPI setup for digilent pmod wifi, pmod adxl345
    PORTSetPinsDigitalOut(IOPORT_G, BIT_6 | BIT_8 | BIT_9 );  //Set SDO1, SCLK, SS lines output
    PORTSetPinsDigitalOut(IOPORT_B, BIT_14 | BIT_15 );        //Set all the rest of em to outputs to ignore...
    PORTSetPinsDigitalOut(IOPORT_D, BIT_4 | BIT_5 );        // these too.
    PORTSetPinsDigitalIn(IOPORT_G, BIT_7 );  //Set SDI1 to input at RG07
    //now initialize the spi port values, make sure they're low
    PORTGbits.RG6       = 0;  //port G
    PORTGbits.RG8       = 0;
    PORTGbits.RG9       = 1;  // SS2 line is active-low, so drive it high initially
    PORTBbits.RB14      = 0; //port B
    PORTBbits.RB15      = 0;
    PORTDbits.RD4       = 0;  //port D
    PORTDbits.RD5       = 0;

  //Also set RB15, RD05, RD4, and RB14 to inputs.  These are four additional wifi mod signals.

}

/*
  Line mode config: Enabling UART transmissions will immediately cause a TX interrupt to
    indicate that the transmitter needs data, unless the transmitter FIFO/buffer
    was pre-loaded with data.

  #define PERIPHERAL_CLOCK    10000000
  #define DESIRED_DATA_RATE   19200

  unsigned int    actualDataRate;

    actualDataRate = UARTSetDataRate(UART1, PERIPHERAL_CLOCK, DESIRED_DATA_RATE);

  Also useful:
  unsigned int UARTSetDataRate ( UART_MODULE id, unsigned int sourceClock, unsigned int dataRate );
  unsigned int UARTGetDataRate ( UART_MODULE id, unsigned int sourceClock );
  BOOL UARTTransmitterIsReady ( UART_MODULE id );
  void UARTSendDataByte ( UART_MODULE id, BYTE data )
  void UARTSendData ( UART_MODULE id, UART_DATA data )
      UART_DATA data = 0x1ff; //can be 8-bit or 9-bit word, depending on how UART is configured

      if (UARTTransmitterIsReady(UART1))
      {
        UARTSendData(UART1, data);
      }
  BOOL UARTReceivedDataIsAvailable ( UART_MODULE id )
  BOOL UARTTransmissionHasCompleted ( UART_MODULE id )
  BYTE UARTGetDataByte ( UART_MODULE id )
  UART_DATA UARTGetData ( UART_MODULE id )
  void UARTSendBreak ( UART_MODULE id )
  void UARTStartAutoDataRateDetect ( UART_MODULE id )

*/
void configureUART1(void)
{
  unsigned int    actualDataRate;

  //void UARTConfigure ( UART_MODULE id, UART_CONFIGURATION flags );
    UARTConfigure ( UART1, UART_ENABLE_PINS_CTS_RTS | UART_RTS_WHEN_RX_NOT_FULL);

  //UART_INTERRUPT_ON_TX_NOT_FULL is default mode
    UARTSetFifoMode( UART1, UART_INTERRUPT_ON_RX_NOT_EMPTY | UART_INTERRUPT_ON_TX_NOT_FULL );

    UARTSetLineControl( UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 );

  //unsigned int UARTSetDataRate ( UART_MODULE id, unsigned int sourceClock, unsigned int dataRate )
    actualDataRate = UARTSetDataRate ( UART1, PERIPHERAL_CLOCK, DESIRED_DATA_RATE );

  //ConfigIntUART1(UART_RX_INT_EN | UART_TX_INT_DIS | UART_ERR_INT_EN | UART_INT_PR0 | UART_INT_SUB_PR0);
    ConfigIntUART1(UART_RX_INT_EN | UART_TX_INT_DIS | UART_ERR_INT_DIS | UART_INT_PR6 | UART_INT_SUB_PR3 | UART_TX_INT_PR0);
}

//Andy version
void setupUART1 (unsigned int pb_clock)
{
  // OpenUART1 (config1, config2, ubrg)
  OpenUART1 (UART_EN | UART_IDLE_CON | UART_RX_TX | UART_DIS_WAKE | UART_DIS_LOOPBACK | UART_DIS_ABAUD | UART_NO_PAR_8BIT | UART_1STOPBIT | UART_IRDA_DIS |
               UART_MODE_FLOWCTRL | UART_DIS_BCLK_CTS_RTS | UART_NORMAL_RX | UART_BRGH_SIXTEEN,
               UART_TX_PIN_LOW | UART_RX_ENABLE | UART_TX_ENABLE | UART_INT_TX | UART_INT_RX_CHAR | UART_ADR_DETECT_DIS  | UART_RX_OVERRUN_CLEAR,
         mUARTBRG(pb_clock, DESIRED_DATA_RATE));
}

/*
  Remarks:
    Enabling the UART trasnmitter may cause an immediate UART TX interrupt
    request (if the UART TX interrupt is enabled), unless the transmit buffer
    has been pre-loaded with data.
*/
void enableUART1(void)
{
  UARTEnable( UART1, UART_ENABLE_FLAGS(UART_ENABLE | UART_PERIPHERAL | UART_RX | UART_TX));
}

/*
  USE THIS, NOT putsUARTx(). This sends an entire
  buffer of nbytes, including null bytes. putsUART() is only
  for null-terminated buffers.

  TODO: The TX register is 32 bits wide, so need to evaluate if we're using the
  full capabilities of the TX register.
*/
void sendUART1(char* data, int nbytes)
{
  int i;

  //int quot, rem; <--parameters for writing bytes in blocks of four
  for(i = 0; i < nbytes; i++){
    //TODO: write 4-bytes at a time?
    WriteUART1(data[i]); //waits for uart-TRMT bit, then writes one byte (as a U32) to uart TX buffer
  }
}


/*
  Set up timer 2 as the primary time slicing mechanism, so give it decent resolution for high/low res stuff.
  
  prescaler: 256
  period: 3125 (see calc below)
  clock input: sysclk == 80MHz
  timer output: 80MHz / 256 = 312500 Hz

  For 10 ms interrupt: 0.010 * timer frequency = period ==> 0.010 * 312500 = 3125 period

  Interrupts: in int config function

  Starts timer.

*/
void configureTimer2(int Period)
{
  //ConfigIntTimer2( T2_INT_OFF | T2_INT_PRIOR_6 );
  OpenTimer2(T3_ON | T3_IDLE_CON | T2_PS_1_2, Period);
  WriteTimer2(0);
}

void configureTimer3(int Period)
{
  //ConfigIntTimer2( T2_INT_OFF | T2_INT_PRIOR_6 );
  OpenTimer3(T3_ON | T3_IDLE_CON | T3_PS_1_256, Period);
  WriteTimer3(0);
}

//configure T4 for 10 MHz (given sysclk == 80 MHz)
void configureTimer4(int Period)
{
  ConfigIntTimer4( T4_INT_OFF );
  WriteTimer4(0);
  OpenTimer4(T4_ON | T4_IDLE_CON | T4_32BIT_MODE_OFF | T4_PS_1_8, Period);
}


/*
//void OpenOC1( config, value1, value2)
//Ex.: OpenOC1( OC_IDLE_STOP | OC_TIMER_MODE32 | OC_PWM_FAULT_PIN_DISABLE, OC_SINGLE_PULSE , 0x0000, 0xFFFF )
void configOCModule2(void)
{
  OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);
}

void configOCModule3(void)
{
  OpenOC3(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);
}

//unsigned int myInt = ReadDCOC3PWM(); <--gets duty cycle
//unisgned int myInt = ReadRegOC3()  preferable
// SetPulseOC1(0xFF, 0xFFFF)  I believe first val is desired OCxRS, second is desire PR
void writeOC2(unsigned int newOC2RValue, unsigned int newOC2RSValue)
{
  SetPulseOC2(newOC2RValue, newOC2RSValue);  //returns OCxR or OCxRS as u_int
}

void writeOC3(unsigned int newOC3RValue, unsigned int newOC3RSValue)
{
  SetPulseOC3(newOC3RValue, newOC3RSValue);  //returns OCxR or OCxRS as u_int
}
*/
/*
INT_IC2  //from int_3xx_4xx.h file -> all INT_SOURCE values
INT_IC3
INT_U1RX
INT_U1TX

void INTGetFlag(INT_SOURCE source);  int.h
void INTSetFlag(INT_SOURCE source);
void INTClearFlag(INT_SOURCE source);
unsigned int INTGetInterruptVectorNumber(void)
void INTEnable(INT_SOURCE source, INT_EN_DIS enable);
void INTSetVectorPriority(INT_VECTOR vector, INT_PRIORITY priority);
INT_PRIORITY INTGetVectorPriority(INT_VECTOR vector);

C:\Program Files (x86)\Microchip\xc32\v1.20\pic32mx\include\peripheral\int.h
*most macros and types are in int_3xx_4xx.h; priority levels are in int.h;
*vector numbers/mapping is in C:\Program Files (x86)\Microchip\xc32\v1.20\pic32mx\include\proc\p32mx460f512l.h

*/
//configure IC2, IC3, and U1RX interrupts
//elected instead to do interrupt configuration within its associated module (IC, UARt, etc) config
void configureInterrupts(void)
{
  INTDisableInterrupts();
  //mClearAllIECRegister();

  INTEnableSystemMultiVectoredInt();

  INTClearFlag(INT_U1RX);
  INTClearFlag(INT_U1TX);
  INTClearFlag(INT_U1E);
  INTClearFlag(INT_T3);

  //INTSetVectorPriority(INT_UART_1_VECTOR, INT_PRIORITY_LEVEL_6);    //set priority to 6, all ints
  //INTSetVectorPriority(INT_T2, INT_PRIORITY_LEVEL_6);    //set priority to 6, all ints
  //INTEnable(INT_UART_1_VECTOR, INT_ENABLED);
  //INTEnable(INT_UART_1_VECTOR, INT_ENABLED);
  //ConfigIntTimer3( T3_INT_ON | T3_INT_PRIOR_6 );


  // INTEnable(INT_T3, INT_ENABLED);


  //mU1SetIntPriority(INT_PRIORITY_LEVEL_6);
  //mU1TXIntEnable(INT_DISABLE); //disable the TX interrupt   Old Crap
  
  // In freeRTOS, ints will be enabled as soon as first proc acquires context, so do not enable ints here if using RTOS!
  //INTEnableInterrupts();
}

/*********************************************************************************************
	A more advanced feature that allows feedback from the motors.
	Useful fact: the motor drive ratio is 1:19, the rest you can discover in the reference manual
	Interrupt on every 4th capture, where 4 sigs needed for each capture, so 16 signals = 16/19 turns of the wheel
*********************************************************************************************/
void configureInputCaptureMods(void)
{
	INTDisableInterrupts();

	OpenCapture2( IC_ON | IC_IDLE_STOP | IC_FEDGE_RISE | IC_TIMER2_SRC | IC_INT_4CAPTURE | IC_EVERY_4_RISE_EDGE);
	OpenCapture3( IC_ON | IC_IDLE_STOP | IC_FEDGE_RISE | IC_TIMER2_SRC | IC_INT_4CAPTURE | IC_EVERY_4_RISE_EDGE);

	ConfigIntCapture2(IC_INT_ON | IC_INT_PRIOR_5 | IC_INT_SUB_PRIOR_3);
	ConfigIntCapture3(IC_INT_ON | IC_INT_PRIOR_5 | IC_INT_SUB_PRIOR_2);

	INTClearFlag(INT_IC2);
	INTClearFlag(INT_IC3);

	INTEnableInterrupts();
}

void sigError(void)
{
  int i = 0, j = 0;

  for(i = 0; i < 5; i++)
  {
    PORTSetBits(IOPORT_B, BIT_10 | BIT_11 | BIT_12 | BIT_13);
    for(j = 0; j < 1; j++){
      delay(LONG_DLY);
    }
    PORTClearBits(IOPORT_B, BIT_10 | BIT_11 | BIT_12 | BIT_13);
    for(j = 0; j < 1; j++){
      delay(LONG_DLY);
    }
  }
}

// Clear on-board and peripheral LEDs
void clearLEDs(void)
{
  PORTClearBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3);
}

/*******************************************************************
 Function: void output_led (short index)
 * Returns: void
 * Input Parameters: short index
 * OutParams: void
 * Description: Thsi function outputs a single led based on an integer
 * index.  The index is simply to make the program more readable and
 * traceable than using bytes to store the state of four leds.
 ********************************************************************/
//NOTE: ouput was reversed (currently not), 0->bit4...3->bit0
void output_led (short index)
{
  unsigned int led_states = 0x00000000;

        switch(index){
        case 0:
            led_states = BIT_0;
            break;
        case 1:
            led_states = BIT_1;
            break;
        case 2:
            led_states = BIT_2;
            break;
        case 3:
            led_states = BIT_3;
            break;
        case 4:
            led_states = BIT_0 | BIT_1 | BIT_2 | BIT_3;
            break;
        default:
            led_states = BIT_0 | BIT_1 | BIT_2 | BIT_3;
            break;
        }

        PORTSetBits(IOPORT_B, led_states);
}

