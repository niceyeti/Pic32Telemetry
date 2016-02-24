#include "my_spi.h"

//test this instead of raw transmissions
//SPiChnPutC(2, (unsigned int)(SPI_WRITE | ADXL345_POWER_CTRL));
//SPiChnPutC(2, (unsigned int)0x18);
void dumbInit(void)
{
  char debug[64] = {'\0'};
  unsigned char data[8] = {'\0'};
  int i = 0;

/*
  while(i++ < 50){
    //SPI2BUF = 0;
    //delayUS(5);
    data[0] = SPI2BUF;
    delayUS(5);
  }
*/



  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_WRITE | ADXL345_DATA_FRMT);
  while(SPI2STATbits.SPIBUSY);
  delayUS(2);
  SPI2BUF = (unsigned char)0x02;  //value determined from practice, reading i2c adxl345
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(30);

  delayMS(1);

  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_WRITE | ADXL345_FIFO_CTRL);
  while(SPI2STATbits.SPIBUSY);
  delayUS(2);
  SPI2BUF = (unsigned char)0x80;  //value determined from practice, reading i2c adxl345
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(30);



  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_WRITE | ADXL345_POWER_CTRL); //send the adxl345 id reg addr, expect that 0xE5 comes back
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  delayUS(2);
  SPI2BUF = 0; //turn it off
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(30);


  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_WRITE | ADXL345_POWER_CTRL);
  while(SPI2STATbits.SPIBUSY);
  delayUS(2);
  SPI2BUF = (unsigned char)0x18; //turn it on
  while(SPI2STATbits.SPIBUSY);   // wait until SPI transmission complete
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(20);


  //debug: for verification, read the registers after writing to them
  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_READ | ADXL345_DATA_FRMT); //send the adxl345 id reg addr, expect that 0xE5 comes back
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  //delayUS(2);
  data[1] = SPI2BUF;
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(20);

  delayMS(10);

  //debug: for verification, read the register after writing to it
  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);
  SPI2BUF = (unsigned char)(SPI_READ | ADXL345_POWER_CTRL);
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  //delayUS(2);
  data[0] = SPI2BUF;
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  delayUS(10);
  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(20);

  // debugging
  sprintf(debug,"PWRCTL 0x%x DATAFMT 0x%x\n",(unsigned char)data[0],(unsigned char)data[1]);
  putsUART1(debug);
}

// for preserving device state
void getADXL345CtrlRegs(void)
{
  char debug[64] = {'\0'};
  int uints[8] = {0};

  SpiChnGetRov(SPI_CHANNEL2, TRUE);

  // read DATA_FRMT
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_READ | ADXL345_DATA_FRMT );
  while(SPI2STATbits.SPIBUSY);
  uints[0] = SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);

  // read FIFO_MODE
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_READ | ADXL345_FIFO_CTRL );
  while(SPI2STATbits.SPIBUSY);
  uints[1] = SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);


  //writing to PWR_CTRL must occur last, after all other regs configured
  // read PWR_CTRL
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_READ | ADXL345_PWR_CTRL );
  while(SPI2STATbits.SPIBUSY);
  uints[2] = SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);

  // read PWR_CTRL
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_READ | ADXL345_PWR_CTRL );
  while(SPI2STATbits.SPIBUSY);
  uints[3] = SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);

  sprintf(debug,"D_FMT 0x%x FIFO_MODE 0x%x PWR_CTL 0x%x PWR_CTL 0x%x\n",uints[0],uints[1],uints[2],uints[3]);
  putsUART1(debug);
}



/*
  BW_RATE: Write ____. Default is 0xA for 100hz output data rate (research further to get right val)
  ***BW_RATE has no bearing on output data unless using trigger interrupts.***
  DATA_FRMT: 0b10, select +-8g mode, all others zero.  For full-res, set BIT_4 and BIT_1 and BIT_0
  PWR_CTRL:  Set BIT_3 to enable measurement; all other bits 0.
  FIFO_CTRL: Write BIT_7==1 and BIT_6==0 to put in stream mode; all others zero.
  

*/
void smartInit(void)
{
  char debug[64] = {'\0'};

  // write DATA_FRMT
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_WRITE | ADXL345_DATA_FRMT );
  while(SPI2STATbits.SPIBUSY);
  SpiChnPutC(2, 0x02 );
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);


  // write FIFO_MODE
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_WRITE | ADXL345_FIFO_CTRL );
  while(SPI2STATbits.SPIBUSY);
  SpiChnPutC(2, 0x80 );
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);


  //writing to PWR_CTRL must occur last, after all other regs configured
  // write PWR_CTRL
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_WRITE | ADXL345_PWR_CTRL );
  while(SPI2STATbits.SPIBUSY);
  SpiChnPutC(2, 0x00);  // first shut it off to force reset (will be enabled below)
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);

  // write PWR_CTRL
  LATGbits.LATG9 = 0;  // select slave device
  delayUS(20);
  SpiChnPutC(2, SPI_WRITE | ADXL345_PWR_CTRL );
  while(SPI2STATbits.SPIBUSY);
  SpiChnPutC(2, 0x08);  //set bit 3 to enable measurement
  while(SPI2STATbits.SPIBUSY);
  delayUS(10);
  LATGbits.LATG9 = 1;  // release slave device
  delayUS(100);

  //spirov()

  //sprintf(accStr,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(accStr);

}



//use this instead of spiinit() below
void setup_SPI2(void)
{
	// void	SpiChnOpen(int chn, SpiCtrlFlags config, unsigned int fpbDiv);
	// SpiChnOpen (1, SPI_OPEN_MSTEN | SPI_OPEN_SMP_END | SPI_OPEN_MODE8, 1024);
	//SpiChnOpen (2, SPI_CON_MSTEN  | SPI_CON_MODE8 | SPI_CON_ON | CLK_POL_ACTIVE_LOW, 256);

  // last parameter (40) srcClkDiv specifies srcClk divisor term (2*(SPIBRG+1)),
	SpiChnOpen (2, SPI_CON_MSTEN  | SPI_CON_MODE8 | SPI_CON_ON | CLK_POL_ACTIVE_LOW, 40);
  // slave off
  PORTGbits.RG9 = 1;
  delayUS(100);
  
  //PORTGbits.RG9 = 0;
  //delayUS(20);

  SpiChnGetRov(SPI_CHANNEL2, TRUE);

  //delayUS(10);
  //PORTGbits.RG9 = 1;

	// Create a falling edge pin SS to start communication
	//PORTSetBits (IOPORT_G, BIT_9);
	//delay (1000);
	//PORTClearBits (IOPORT_G, BIT_9);
}


//For SPI2 on the cerebot/ pic32 board
void spiInitialize(void)
{
  int i = 0;
  unsigned char dummy = 0;

  SPI2CONbits.ON      = 0;        // disable SPI to reset any previous state
  dummy               = SPI2BUF;  // clear receive buffer

  //how many chars deep is the FIFO?
  while(dummy && (i++ < 32)){ dummy = SPI2BUF; }

  /*
    See PIC32 peripheral ref man: spiClkFreq = PBCLK / (2 * (SPIxBRG + 1))
    So for 1 MHz operation, SPIxBRG == 19 --> 40MHz / (2 * (19 + 1)) = 40MHz / 40
  */
  SPI2BRG             = 19;  // 0x10011 == 19

//  SPI2CON = 0x8120;

  SPI2CONbits.FRMEN   = 1;        // disable frame mode
  SPI2CONbits.MSTEN   = 1;        // enable master mode
  SPI2CONbits.CKE     = 1;        // set clock-to-data timing
  SPI2CONbits.SMP     = 1;        // input data is sampled at end of data output time
  SPI2CONbits.MODE16  = 0;        // sets data width to 8 bits (SPIxCON bits 11/10 determine data width; see refman)
  SPI2CONbits.MODE32  = 0;        // sets data width to 8 bits
  SPI2CONbits.ON      = 1;        // turn SPI on

  //May choose to ignore this: selectable 8, 16, or 32 bit data mode. 16 bit may be preferrable for IMU packet (?).

  // Signal G9 is on the same pin as SPI2SS (Slave Select) signal.
  // Since slave select is not used in this configuration
  // the pin is used as a reset signal for the external SPI slave.

  PORTGbits.RG9       = 0;  // slave on...
  delayMS(300);
  PORTGbits.RG9       = 1;  // ...slave off  (resets device)
  delayMS(1);
}

// test connection by verifying reading the id addr returns 0xE5
BOOL spiADXL345TestConnection(void)
{
  BOOL result = FALSE;

  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);

  //SPI2BUF = addr;
/* //direct method, confirmed working
  SPI2BUF = (unsigned char)SPI_READ | ADXL345_ID_ADDR; //send the adxl345 id reg addr, expect that 0xE5 comes back
  //SPI2BUF = SPI_READ | SPI_AUTO_INCREMENT | addr;
  while (SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  result = (ADXL345_ID_VAL == SPI2BUF);
*/
  SpiChnPutC(2, SPI_READ | ADXL345_ID_ADDR );
  while (SPI2STATbits.SPIBUSY); // wait until SPI transmission complete
  result = (SpiChnGetC(2) == ADXL345_ID_VAL);

  LATGbits.LATG9 = 1;   // deselect slave device
  delayUS(20);

  return result;
}

void spiADXL345AverageFive(struct xyzVector* vec)
{
  int i, x, y, z;

  for(i = 0; i < 5; i++){
    //spiADXL345GetAccelManual(vec);
    spiADXL345GetAccelBurst(vec);
    x += vec->X;
    y += vec->Y;
    z += vec->Z;
  }

  vec->X = x / 5;
  vec->Y = y / 5;
  vec->Z = z / 5;
}



/*
  uses manual addressing, no autoincrement
  
  According to the manual, this actually shouldn't work, since transmissions are supposed to
  have discrete ~SS signals, eg, one ~ss/ss segment is suitable only to one read/write operation.
  So maybe this is only working because of the putc operations???

*/
void spiADXL345GetAccelManual(struct xyzVector* vec)
{
  //unsigned char data[6] = {0};
  //char debug[64] = {'\0'};

/*
  spiGets(ADXL345_X_L, data, 6);

  vec->X = ((unsigned short)data[1] << 8) | data[0];
  vec->Y = ((unsigned short)data[3] << 8) | data[2];
  vec->Z = ((unsigned short)data[5] << 8) | data[4];
*/

  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);

  SpiChnPutC(2, SPI_READ | ADXL345_X_L );
  while(SPI2STATbits.SPIBUSY);
  vec->X = SpiChnReadC(2);
  SpiChnPutC(2, SPI_READ | ADXL345_X_H );
  while(SPI2STATbits.SPIBUSY);
  vec->X |= (SpiChnReadC(2) << 8);

  SpiChnPutC(2, SPI_READ | ADXL345_Y_L );
  while(SPI2STATbits.SPIBUSY);
  vec->Y = SpiChnReadC(2);
  SpiChnPutC(2, SPI_READ | ADXL345_Y_H );
  while(SPI2STATbits.SPIBUSY);
  vec->Y |= (SpiChnReadC(2) << 8);

  SpiChnPutC(2, SPI_READ | ADXL345_Z_L );
  while(SPI2STATbits.SPIBUSY);
  vec->Z = SpiChnReadC(2);
  SpiChnPutC(2, SPI_READ | ADXL345_Z_H );
  while(SPI2STATbits.SPIBUSY);
  vec->Z |= (SpiChnReadC(2) << 8);

  //sprintf(accStr,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(accStr);

  LATGbits.LATG9 = 1;            // release slave device
  delayUS(20);

  //sprintf(debug,"spiAcl: 0x%x 0x%x 0x%x\n",0x0000FFFF & (unsigned int)vec->X,0x0000FFFF & (unsigned int)vec->Y,0x0000FFFF & (unsigned int)vec->Z);
  //putsUART1(debug);
  //sprintf(debug,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(debug);

  SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

}

/*

  check http://wiki.analog.com/resources/tools-software/uc-drivers/microchip/adxl345
  lots of ref code

  Correctly reads burst, but compare with getAccelManual above. The LSB of Y and Z are
  only either 00 or FF, depending on the sign (rotate the adxl345 to observe alternation
  between 00 and FF.

*/
void spiADXL345GetAccelBurst(struct xyzVector* vec)
{
  //unsigned char data[6] = {0};
  //char debug[64] = {'\0'};

  //findings: delays have no effect, nor does getspirov() call, nor addressing ech reg before each read

  SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);

  SpiChnPutC(2, SPI_READ | SPI_AUTO_INCREMENT | ADXL345_X_L );
  while(SPI2STATbits.SPIBUSY);
  vec->X = 0x000000FF & SpiChnReadC(2);
  vec->X = 0x000000FF & SpiChnReadC(2);
  //delayUS(80);
   SpiChnPutC(2,0);
  while(SPI2STATbits.SPIBUSY);
  vec->X |= (SpiChnReadC(2) << 8);
   SpiChnPutC(2,0);

  while(SPI2STATbits.SPIBUSY);
  //delayUS(80);
  vec->Y = 0x000000FF & SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
   SpiChnPutC(2,0);
  //delayUS(80);
  while(SPI2STATbits.SPIBUSY);
  vec->Y |= (SpiChnReadC(2) << 8);
   SpiChnPutC(2,0);

  while(SPI2STATbits.SPIBUSY);
  //delayUS(80);
  vec->Z = 0x000000FF & SpiChnReadC(2);
  while(SPI2STATbits.SPIBUSY);
   SpiChnPutC(2,0);
  while(SPI2STATbits.SPIBUSY);
  //delayUS(80);
  vec->Z |= (SpiChnReadC(2) << 8);

  //sprintf(accStr,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(accStr);

  LATGbits.LATG9 = 1;            // release slave device
  delayUS(20);

  //sprintf(debug,"spiAcl burst: 0x%x 0x%x 0x%x\n",0x0000FFFF & (unsigned int)vec->X,0x0000FFFF & (unsigned int)vec->Y,0x0000FFFF & (unsigned int)vec->Z);
  //putsUART1(debug);
  //sprintf(debug,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(debug);
  

  //SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

}

/* 
  NOT TESTED

   this cannot call spiGetc/spiPutc since these are only good for discrete one char transmissions;
   as the SS line must be low for the entire transmission.

  Standard use-case for this function is device configuration sequence: address a register, write a single byte to it.

  WARNING: The inner loop is designed to wait 8 spi clks for the ADXL345 device to autoincrement.
  I suspect the 8 clk requirement is for the 8 bit format of the ADXL345.  So when using this function
  in 16 or 32 bit mode, a different wait would likely be necessary, proportional to data format selection.
*/
void spiPuts(unsigned char addr, unsigned char data[], int nchars)
{
  unsigned char dummy;
  int i = 0, j = 0;

  LATBbits.LATB11 = 1;
  LATGbits.LATG9 = 0;            // select slave device: drive SS low
  delayUS(1);

  SPI2BUF = (unsigned char)(SPI_WRITE | SPI_AUTO_INCREMENT | addr);  //multibyte write bit???
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete

  for(i = 0; i < nchars; i++){

    SPI2BUF = data[i];
    while (SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete

    //comms are a ring, so read byte from buf for every byte sent ... necessary ???
    dummy = SPI2BUF;
    while (SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete

    //delay 8 spi clk pulses for device to autoincrement to next register
    // sysClk 80MHz and spiClk 1MHz, to 8 spi clks == 80 sys clks
    delayUS(1);
  }

  LATGbits.LATG9 = 1;            // release slave device
  LATBbits.LATB11 = 0;
}

/* 
  NOT TESTED

  this cannot call spiGetc/spiPutc since these are only good for discrete, one char transmissions;
   as the SS line must be low for the entire transmission.

  WARNING: The inner loop is designed to wait 8 spi clks for the ADXL345 device to autoincrement.
  I suspect the 8 clk requirement is for the 8 bit format of the ADXL345.  So when using this function
  in 16 or 32 bit mode, a different wait would likely be necessary, proportional to data format selection.
*/
void spiGets(unsigned char addr, unsigned char data[], int nchars)
{
  int i = 0, j = 0;

  LATBbits.LATB11 = 1;
  LATGbits.LATG9 = 0;            // select slave device
  delayUS(1);

  //SPI2BUF = addr;
  SPI2BUF = (unsigned char)0xF2;
  //SPI2BUF = SPI_READ | SPI_AUTO_INCREMENT | addr;
  while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete

  for(i = 0; i < nchars; i++){

    data[i] = SPI2BUF;

    //send null byte for each byte we wish to read from slave
    SPI2BUF = 0;
    while(SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete

    //delay 8 spi clk pulses for device to autoincrement to next register
    // sysClk 80MHz and spiClk 1MHz, to 8 spi clks == 80 sys clks
    delayUS(1);
  }

  LATGbits.LATG9 = 1;            // release slave device
  LATBbits.LATB11 = 0;
}


//The driver for both sending and receiving, since master/slave data line is a ring / circular buffer
char spiTransfer (char c)
{

/*
  Example of sending a command to a slave and reading its response:

  LATBbits.LATB4 = 0;                  // select slave device
  SPI1_transfer( tx_data);             // transmit byte tx_data
  char response = SPI1_transfer( 0);   // send dummy byte and read response
  LATBbits.LATB4 = 1;                  // release slave device
*/

  LATBbits.LATB11 = 1;
  LATGbits.LATG9 = 0;            // select slave device
  delayUS(1);
  SPI2BUF = c;                   // send data to slave
  while (SPI2STATbits.SPIBUSY);  // wait until SPI transmission complete
  delayUS(1);
  LATGbits.LATG9 = 1;            // release slave device
  LATBbits.LATB11 = 0;

  return SPI2BUF;

}

char spiGetc(void)
{
    return spiTransfer(0);
}

char spiPutc (char c)
{
    return spiTransfer(c);
}

////////////////////////// end spi code /////////////////////////////

/*  dirty old form (some edits from last attempted; logic was to use delays to wait for increment
void spiADXL345GetAccelBurst(struct xyzVector* vec)
{
  unsigned char data[6] = {0};
  char debug[64] = {'\0'};

/*
  spiGets(ADXL345_X_L, data, 6);

  vec->X = ((unsigned short)data[1] << 8) | data[0];
  vec->Y = ((unsigned short)data[3] << 8) | data[2];
  vec->Z = ((unsigned short)data[5] << 8) | data[4];
//
  SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

  LATGbits.LATG9 = 0;   // select slave device
  delayUS(20);

  SpiChnPutC(2, SPI_READ | SPI_AUTO_INCREMENT | ADXL345_X_L );
  while(SPI2STATbits.SPIBUSY);
  vec->X = SpiChnReadC(2);
  SpiChnPutC(2, 0 );
  while(SPI2STATbits.SPIBUSY);
  //delayUS(20); //delay 80 sysclocks, or >8 us for autoincrement
  vec->X |= (SpiChnReadC(2) << 8);
  SpiChnPutC(2, 0 );

  //delayUS(20); //delay 80 sysclocks, or >8 us

  SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

  //SpiChnPutC(2, SPI_READ | ADXL345_Y_L );
  while(SPI2STATbits.SPIBUSY);
  vec->Y = SpiChnReadC(2);
  SpiChnPutC(2, 0 );
  while(SPI2STATbits.SPIBUSY);
  delayUS(20); //delay 80 sysclocks, or >8 us
  vec->Y |= (SpiChnReadC(2) << 8);
  delayUS(20); //delay 80 sysclocks, or >8 us
  while(SPI2STATbits.SPIBUSY);
  vec->Z = SpiChnReadC(2);
  delayUS(20); //delay 80 sysclocks, or >8 us
  vec->Z |= (SpiChnReadC(2) << 8);

  //sprintf(accStr,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(accStr);

  LATGbits.LATG9 = 1;            // release slave device
  delayUS(20);

  sprintf(debug,"spiAcl burst: 0x%x 0x%x 0x%x\n",0x0000FFFF & (unsigned int)vec->X,0x0000FFFF & (unsigned int)vec->Y,0x0000FFFF & (unsigned int)vec->Z);
  putsUART1(debug);
  //sprintf(debug,"SPI2CON 0x%x SPI2STAT 0x%x\n",SPI2CON,SPI2STAT);
  //putsUART1(debug);

  SpiChnGetRov(SPI_CHANNEL2, TRUE); //clear overflow

}
*/
