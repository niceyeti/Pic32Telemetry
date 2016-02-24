#include "my_i2c.h"

///////////////////////////////////////////////////////////////////////////////
/////////////////I2C Module Code///////////////////////////////////////////////
// Write or clear a bit.  To clear, pass FALSE for third parameter.
BOOL i2cWriteBit(struct i2cDev* dev, unsigned char bitNum, BOOL enable)
{
    unsigned char byte = '\0';
    i2cGetc(dev, &byte);

    byte = (enable != FALSE) ? (byte | (1 << bitNum)) : (byte & ~(1 << bitNum));
    return i2cPutc(dev, &byte);
}

//params: device address and subaddress, and a pointer to a data byte mem location
//cascade changes to i2cPuts() below
BOOL i2cPutc(struct i2cDev* dev, unsigned char* txData)
{
  //debug output
  int test = 0;

  /*
  //debug
  I2C_STATUS status;
  char str[64] = {'\0'};
  status = I2CGetStatus(I2CMODULE);
  test = (int)status | test;
  sprintf(str,"init i2C status: 0x%x\n\r",test);
  putsUART1(str);
  //end debug
  */

  if( !i2cStartTransfer(FALSE) ){
    putsUART1("ERROR i2cPutc failed to initiate START cond\n\r");
    return FALSE;
  }

  //Write byte to slave:
  //  Transmit sequence a byte at a time: Slave write address, slave sub address (the id register), then data to write
  if( !I2CTransmitByte(I2CMODULE, ( dev->devAddr << 1 | 0) ) ){ //send write address
    putsUART1("ERROR i2cPutc() failed to send dev addr\n\r");
    return FALSE;
  }

  if( !I2CTransmitByte(I2CMODULE, dev->subAddr ) ){ //send subaddress to write to
    putsUART1("ERROR i2cPutc() failed to send dev subaddr\n\r");
    return FALSE;
  }

  //putsUART1("here\n\r");
  if( !I2CTransmitByte(I2CMODULE, *txData ) ){ //send data byte to write to
    putsUART1("ERROR i2cPutc() failed to send txData byte\n\r");
    return FALSE;
  }

  i2cStopTransfer();
  delay(128);

  //putsUART1("post stop\n\r");
  //I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_START | I2C_STOP | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)

  return TRUE;
}

//just an iterative modification of i2cPutc
BOOL i2cPuts(struct i2cDev* dev, unsigned char txData[], int nBytes)
{
  int i = 0;

  /*
  //debug output
  int test = 0;
  I2C_STATUS status;
  char str[64] = {'\0'};
  status = I2CGetStatus(I2CMODULE);
  test = (int)status | test;
  sprintf(str,"init i2C status: 0x%x\n\r",test);
  putsUART1(str);
  //end debug
  */

  if( !i2cStartTransfer(FALSE) ){
    putsUART1("ERROR i2cPuts failed to initiate START cond\n\r");
    return FALSE;
  }

  //Write byte to slave:
  //  Transmit sequence a byte at a time: Slave write address, slave sub address (the id register), then data to write
  if( !I2CTransmitByte(I2CMODULE, ( dev->devAddr << 1 | 0) ) ){ //send write address
    putsUART1("ERROR i2cPuts() failed sending devAddr\n\r");
    return FALSE;
  }

  if( !I2CTransmitByte(I2CMODULE, dev->subAddr ) ){ //send subaddress to write to
    putsUART1("ERROR i2cPuts() failed sending dev subAddr\n\r");
    return FALSE;
  }

  while(i < nBytes){
    if( !I2CTransmitByte(I2CMODULE, txData[i++] ) ){ //send subaddress to write to
      putsUART1("ERROR i2cPuts() failed to send txData byte\n\r");
      return FALSE;
    }
  }

  i2cStopTransfer();

  //I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_START | I2C_STOP | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)

   return TRUE;
 }

//params: device address and subaddress, and a pointer to a data byte mem location
BOOL i2cGetc(struct i2cDev* dev, unsigned char *rxData)
{
  int i = 0;
  I2C_RESULT result = I2C_SUCCESS;
  char errorStr[64] = {'\0'};

  /*
  //debug output
  int test = 0;
  int Success = 0b1111;
  I2C_STATUS status;
  char str[64] = {'\0'};
  status = I2CGetStatus(I2CMODULE);
  test = (int)status | test;
  sprintf(str,"init i2C status: 0x%x\n\r",test);
  putsUART1(str);
  //end debug
  */

  if( !i2cStartTransfer(FALSE) ){
    putsUART1("i2cStart xfer failed in i2cGetc()\n\r");
    return FALSE;
  }

  //Read data from slave:
  //  Transmit sequence a byte at a time: Slave write address, slave sub address (the id register), but NOT the read register slave address (comes below, after repeated start command)
  if ( !I2CTransmitByte(I2CMODULE, ( dev->devAddr << 1 | 0) )){ //send write address
    putsUART1("ERROR devAddr could not be sent in i2cGetc()\n\r");
    return FALSE;
  }

  if( !I2CTransmitByte(I2CMODULE, dev->subAddr ) ){ //send subaddress to read from
    putsUART1("Error: devSubAddr could not be sent in i2cGetc()\n\r");
    return FALSE;
  }

  i2cStartTransfer(TRUE);  //now send repeated start signal followed by slave read address + read bit
  ////putsUART1("made it to repeat start\n\r");

  if( !I2CTransmitByte(I2CMODULE, (dev->devAddr << 1 | 1) ) ){  //send the read address
    putsUART1("ERROR Failed to send devSubAddr_R in i2cGetc()\n\r");
    i2cStopTransfer();
    return FALSE;
  }

  result = I2CReceiverEnable(I2CMODULE, TRUE);
  if(result != I2C_SUCCESS ){
    sprintf(errorStr,"ERROR I2CRxEnable returned 0x%x in i2cGetc()\n\r",(int)result);
    putsUART1(errorStr);
    I2CReceiverEnable(I2CMODULE, FALSE);
    i2cStopTransfer();
    return FALSE;
  }

  //while( !I2CReceivedDataIsAvailable( I2CMODULE ));
  //wait for data to arrive: optimistically if we made it this far, the wait should succeed
  while( I2CReceivedDataIsAvailable( I2CMODULE ) == FALSE);/*{ putsUART1("waiting for char\n"); } && (i < 32000) ){
    i++;
    //delay(16); //wait about 128+ clks
  }
  */
  
  if(i >= 32000){
    putsUART1("ERROR Data not available in i2cGetc()\n\r");
    i2cSendAck(I2CMODULE, FALSE);
    I2CReceiverEnable(I2CMODULE, FALSE);
    i2cStopTransfer();
    return FALSE;
  }

  *rxData = I2CGetByte(I2CMODULE);
//putsUART1("got rx data\n");
  i2cSendAck(I2CMODULE, FALSE); // IF LOOP-READING MULTIPLE BYTES, THIS SHOULD PASS *TRUE* (false is for ending transmission: master sends NACK, then STOP
  I2CReceiverEnable(I2CMODULE, FALSE);  //dont forget to disable receiver once Rx completed!  Not known if this is necessary or not
  i2cStopTransfer();

  // thru debug needed for freeRTOS ????
  delay(200);


  // End the transfer
  //putsUART1("pre\n\r");  succeeds / prints
  //i2cSendAck(I2CMODULE, FALSE);
//putsUART1("post\n"); //succeeds / print
  //I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_START | I2C_STOP | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)

  return TRUE;
}

/*
  read a sequence of bytes from a slave.  Devices typically offer reg address auto-incrementing.
  Reading sequential registers entails reading from the lowest reg addr, the device then increments
  the subaddress on successive reads.
  THIS FUNCTION NOT TESTED

  See i2cGetsManualIncrement().  The behavior of this function is purely device specific.  Some devices support
  autoincrementing, some do not.  Those that do may only support it under specific conditions.  For example, the
  adxl345 support autoincrementing only when each read is buffered by 8 clock pulses.

  adxl345: supports autoincrement, but only if reads are buffered by 8 i2c clock pulses.
  mag: supports autoincrement by default, "without further master intervention".  Manual gives no further info.
  gyro: supports autoincrement depending on usage of read ops and value of FIFO_CTRL_REG (see manual for "read_burst")
  barometer:


  PRECON: Device must support or be configured-for address auto-incrementing.

 */
BOOL i2cGets(struct i2cDev* dev, unsigned char rxData[], int nBytes)
{
  int i = 0, readCt = 0;
  BOOL ack = TRUE;
  I2C_RESULT result = I2C_SUCCESS;
  char errorStr[64] = {'\0'};

  /*
  //debug output
  int test = 0;
  int Success = 0b1111;
  I2C_STATUS status;
  char str[64] = {'\0'};
  status = I2CGetStatus(I2CMODULE);
  test = (int)status | test;
  sprintf(str,"init i2C status: 0x%2x\n\r",test);
  //putsUART1(str);
  //end debug
  */

  if( !i2cStartTransfer(FALSE) ){
    putsUART1("ERROR i2cGets(): i2cStart xfer failed\n\r");
    return FALSE;
  }

  //Read data from slave:
  //  Transmit sequence a byte at a time: Slave write address, slave sub address (the id register), but NOT the read register slave address (comes below, after repeated start command)
  if ( !I2CTransmitByte(I2CMODULE, ( dev->devAddr << 1 | 0)) ){ //send write address
    sprintf(errorStr,"ERROR i2cGets(): devAddr >0x%x< not sent\n\r",dev->devAddr);
    putsUART1(errorStr);
    return FALSE;
  }

  if( !I2CTransmitByte(I2CMODULE, dev->subAddr ) ){ //send subaddress to read from
    sprintf(errorStr,"ERROR i2cGets(): subAddr >0x%x< not sent\n\r",dev->subAddr);
    putsUART1(errorStr);
    return FALSE;
  }

  i2cStartTransfer(TRUE);  //now send repeated start signal followed by slave read address + read bit
  ////putsUART1("made it to repeat start\n\r");

  if( I2CTransmitByte(I2CMODULE, (dev->devAddr << 1 | 1) ) ){ //send the read address

    result = I2CReceiverEnable(I2CMODULE, TRUE);
    if(result != I2C_SUCCESS ){
      sprintf(errorStr,"ERROR i2cGets(): I2CRxEnable() returned 0x%x\n\r",(int)result);
      putsUART1(errorStr);
      I2CReceiverEnable(I2CMODULE, FALSE);
      i2cStopTransfer();
      return FALSE;
    }

    //iteratively read bytes from the bus
    for(readCt = 0; readCt < nBytes; readCt++){

      //wait ~128 clks for data to arrive: optimistically if we made it this far, the wait should succeed, some devices just need time to catch up...
      for(i = 0; !I2CReceivedDataIsAvailable(I2CMODULE) && (i < 10); i++){ delay(TINY_DLY); }

      if(i < 10){  //outer error check if > 10, then something went wrong...
        rxData[readCt] = I2CGetByte(I2CMODULE);
        ack = (readCt < (nBytes - 1)) ? TRUE : FALSE; //this check is necessary so we don't send TRUE on the last read
        i2cSendAck(I2CMODULE, ack); // IF LOOP-READING MULTIPLE BYTES: THIS MUST PASS *TRUE* (FALSE is for ending transmission: master sends NACK, then STOP)

        /*
           Wait for 6600 sys-clks (3 ops * 2200: < comparison, ++, and Nop()).
           This is specifically for the adxl345, which requires 8 i2c-clk pulses for autoincrement to occur.
           80 MHz sysclk / 100 KHz i2c-clk = 800 sysClk/i2cClk * 8 == 6400 sysClks
        */
        //delayMS(1);
        //for(i = 0; i < 3000; i++){ Nop(); }  //damn thing doesn't work


        //sprintf(compass_X,"Rx'ed: 0x%x\n\r", *rxData );
      }
      else{
        sprintf(errorStr,"ERROR i2cGets(): Data not available >> rdCt=0x%x i=0x%x\n\r",readCt, i);
        putsUART1(errorStr);
        i2cSendAck(I2CMODULE, FALSE);
        I2CReceiverEnable(I2CMODULE, FALSE);
        i2cStopTransfer();
        return FALSE;
      }
    }

    //putsUART1(compass_X);
    I2CReceiverEnable(I2CMODULE, FALSE);  //dont forget to disable receiver once Rx completed!  Not known if this is necessary or not
  }
  else{
    putsUART1("ERROR i2cGets(): Failed to send devSubAddr_R\n\r");
    i2cStopTransfer();
    return FALSE;
  }

  // End the transfer
  //putsUART1("pre\n\r");  succeeds / prints
  //i2cSendAck(I2CMODULE, FALSE);
  i2cStopTransfer();
  delay(100);  //added for rtos
  // putsUART1("post\n\r"); succeeds / print
  //I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_START | I2C_STOP | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)
}

/*
  i2cGets() only supports devices that support auto-incrementing register read.  This
  function instead calls i2cGetc() iteratively, manually getting each byte in a
  complete i2c transmission (start, W, Sub, restart, R, byte, stop).
  Since the target registers are almost always in sequential blocks of 8 bit registers,
  this function takes in a device struct containing the starting register address,
  then reads up to n sequential registers.

  WARNINGS: Expected returns of this function may be device specific.  Some devices (adxl345)
  require a discrete pause of n-clk sigs before a new register may be addressed, and likewise
  may update/not-update registers as expected for a given sequential register read.

  Using i2cGets() is always preferred, not just for speed, but also because this function
  risks updating MSB/LSB registers between reads operations, such that when MSB byte is read,
  its associated LSB is updated with new data before being read, as follows:

     MSB     LSB       Op
  1  0x12    0x34      Master reads MSB 0x12>>
  2  0x56    0x78      <<Slave updates regs
  3  0x56    0x78      Master reads LSB 0x78>>

    Result:          Master reads 0x1278
    Intended result: Master reads 0x1234

  This is extremely likely to occur, and update will almost always occur while the master is busy
  initiating a new i2c transmission between the MSB and LSB read.
  Some devices offer blocking bits or other measures to prevent this result, but not all.

  Returns: SUCCESS or FAIL.  We could instead return the number of bytes read, along with some
  failure/success output parameter.
*/
BOOL i2cGetsManualIncrement(struct i2cDev* dev, unsigned char rxData[], unsigned int nRegs)
{
  int i = 0;

  //loop through the sequential register addresses
  for(i = 0; i < nRegs; i++, dev->subAddr++){
    if( !i2cGetc(dev, rxData+i) ){
      return FALSE;
    }
  }

  return TRUE;
}



/*****************************************************************************
I2C configuration:

-necessary I2C freq is unclear

reference: I2CMODULE
void I2CConfigure ( i2c_modULE id, I2C_CONFIGURATION flags )
  precon/usage:
unsigned int I2CSetFrequency ( i2c_modULE id, unsigned int sourceClock, unsigned int i2cClock );
  precon/usage: peripheral clock must already be configured (running?)
void I2CSetSlaveAddress ( i2c_modULE id, UINT16 address, UINT16 mask,
                              I2C_ADDRESS_MODE flags )

void I2CEnable( i2c_modULE id, BOOL enable )
  precondition: The module should be appropriately configured (using I2CConfigure and
    I2CSetFrequency, and possibly I2CSetSlaveAddress) before being enabled.

BOOL I2CBusIsIdle( i2c_modULE id ) This is a routine to determine if the I2C bus is idle or busy.
I2C_RESULT I2CStart( i2c_modULE id ) This routine sends the "start" signal (a falling edge on SDA while SCL is high) to start a transfer on the I2C bus.
I2C_RESULT I2CRepeatStart ( i2c_modULE id ) This routine supports sending a repeated Start condition to change slave targets or transfer direction to support certain I2C transfer formats.
void I2CStop ( i2c_modULE id ) This is a routine to send an I2C Stop condition to terminate a transfer.
BOOL I2CTransmitterIsReady ( i2c_modULE id ) This is a routine to detect if the transmitter is ready to accept data to transmit.

BOOL I2CTransmissionHasCompleted ( i2c_modULE id ) The module must have been configured appropriately and enabled, a transfer
                          must have been previously started, and a data or address byte must have
                          been sent.
BOOL I2CByteWasAcknowledged ( i2c_modULE id )    This routine allows a transmitter to determine if the byte just sent was
                          positively acknowledged (ACK'd) or negatively acknowledged (NAK'd) by the
                          receiver. Byte must have been sent.
I2C_RESULT I2CReceiverEnable ( i2c_modULE id, BOOL enable ) This is a routine to enable the module to receive data from the I2C bus.
                        This routine enables the module to receive data from the I2C bus.
                        The module must have been configured appropriately and enabled, a transfer
                        must have been previously started (either by the I2C module or by an
                        external master), and module must be the intended receiver of the next byte
                        of data.
BOOL I2CReceivedDataIsAvailable ( i2c_modULE id );  The I2C module must have been configured appropriately and enabled, and a
                          transfer must have been previously started.
BOOL I2CReceivedByteIsAnAddress ( i2c_modULE id )  The module must have been configured appropriately and enabled, and a
                          transfer must have been previously started.
BYTE I2CGetByte ( i2c_modULE id )  The module must have been configured appropriately and enable, a transfer
                  must have been previously started, and a byte of data must have been
                  received from the I2C bus.
 void I2CAcknowledgeByte ( i2c_modULE id, BOOL ack )The module must have been configured appropriately and enabled, a transfer
                          must have been previously started, and a byte of data must have been
                          received from the I2C bus.
BOOL I2CAcknowledgeHasCompleted ( i2c_modULE id )
I2C_STATUS I2CGetStatus ( i2c_modULE id )   This routine provides a bitmask of the current status flags for the I2C module.
void I2CClearStatus ( i2c_modULE id, I2C_STATUS status )

Write:
1. Send a start sequence
2. Send the I2C address of the slave with the R/W bit low (even address)
3. Send the internal register number you want to write to
4. Send the data byte
5. [Optionally, send any further data bytes]
6. Send the stop sequence.
Read:
1. Send a start sequence
2. Send 0xC0 ( I2C address of the CMPS03 with the R/W bit low (even address)
3. Send 0x01 (Internal address of the bearing register)
4. Send a start sequence again (repeated start)
5. Send 0xC1 ( I2C address of the CMPS03 with the R/W bit high (odd address)
6. Read data byte from CMPS03
7. Send the stop sequence

Look up: OpenI2CMODULE() function (other one-liners?)

*****************************************************************************/
void i2cConfigModule(void)
{
  unsigned int actualClock = 0;
  char debug[64];

  I2CConfigure( I2CMODULE, 0 );  //I2C_CONFIG_FLAGS should be zero; no config bits seem to apply to normal use (test)
  actualClock = I2CSetFrequency(I2CMODULE, PERIPHERAL_CLOCK, I2C_CLOCK_FREQ);
    /*if ( abs(actualClock-I2C_CLOCK_FREQ) > I2C_CLOCK_FREQ/10 )
    {
      putsUART1("I2CMODULE clock frequency (%ld) error exceeds 10%%\n", actualClock);
    }*/

  //sprintf(debug,"i2c clk: %d\n",actualClock);
  //putsUART1(debug);

  I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)
  I2CEnable( I2CMODULE, TRUE );    /*can only enable I2C connection after clock is configured:
                  The module should be appropriately configured (using I2CConfigure and
                  I2CSetFrequency, and possibly I2CSetSlaveAddress) before being enabled.*/
  //i2cStopTransfer();
  //should clear the i/o buffers here
  I2CReceiverEnable(I2CMODULE, FALSE);
}

/*******************************************************************************
  Function:
    BOOL i2cStartTransfer( BOOL restart )
  Summary:
    Starts (or restarts) a transfer to/from the device.
  Description:
    This routine starts (or restarts) a transfer to/from the device, waiting (in
    a blocking loop) until the start (or re-start) condition has completed.
  Precondition:
    The I2C module must have been initialized.
  Parameters:
    restart - If FALSE, send a "Start" condition
            - If TRUE, send a "Restart" condition\
  Returns:
    TRUE    - If successful
    FALSE   - If a collision occured during Start signaling
  Example:
    <code>
    i2cStartTransfer(FALSE);
    </code>
  Remarks:
    This is a blocking routine that waits for the bus to be idle and the Start
    (or Restart) signal to complete.
  *****************************************************************************/
BOOL i2cStartTransfer( BOOL restart )
{
    I2C_STATUS  result;
  int i = 0;

  // Send the Start (or Restart) signal
  if(restart){
    //I2CBusIsIdle(I2CMODULE) <--calling this is inappropriate here; this call is for initiating transfers only

    result = I2CRepeatStart(I2CMODULE);

    //this error condition occurs frequently in single-master mode when
    //the MCU is not turned off and back on before resetting the program.
    //Only the caller can resolve this failure for repeatStart, since they
    //must restart the communication/device-addressing sequence.
        if(result == I2C_MASTER_BUS_COLLISION){
          putsUART1("i2cRepeatStart() failed due to I2C_MASTER_BUS_COLLISION\r\n");
          return FALSE;
        }
  }
  else{
    // Wait for the bus to be idle, then start the transfer
    while( !I2CBusIsIdle(I2CMODULE) && (i < 10)){
      //putsUART1("Waiting for idle i2c bus, before i2cstart()\r\n");
      //putsUART1("Waiting...\n\r");
      i++;
    }

    result = I2CStart(I2CMODULE);

    //this error condition occurs frequently in single-master mode when
    //the MCU is not turned off and back on before reseting the program.
    //I dont know how to handle such collisions, so this is brute force:
    if(result == I2C_MASTER_BUS_COLLISION){
      I2CStop(I2CMODULE);  //send STOP and restart the system
      I2CEnable(I2CMODULE, FALSE);
      for(i = 0; i < 256; i++){
        asm("nop");
      }
      I2CClearStatus ( I2CMODULE, I2C_ARBITRATION_LOSS | I2C_SUCCESS | I2C_ERROR | I2C_MASTER_BUS_COLLISION | I2C_RECEIVE_OVERFLOW | I2C_TRANSMITTER_OVERFLOW ); //second param selects flags to clear (all of them)
      I2CEnable(I2CMODULE, TRUE);

      result = I2CStart(I2CMODULE);
      if(result == I2C_MASTER_BUS_COLLISION){
        putsUART1("I2C_START FAILURE");
        return FALSE;
      }
    }
  }

  //hastily pulled out a lot of these wait-loops, some of which appeared infinite at times.
  //this one was necessary, else a master bus collision would occur in ALL cases, not just the common one when the board not power-cycled
  do {
    result = I2CGetStatus(I2CMODULE);  //Wait for the signal to complete
  } while ( !(result & I2C_START) );

  return TRUE;
}

void i2cSendAck(I2C_MODULE i2c_mod, BOOL ack)
{
  //I2C_STATUS  status;

  I2CAcknowledgeByte(i2c_mod,ack);

  while(!I2CTransmissionHasCompleted(i2c_mod));
}

void i2cStopTransfer( void )
{
    I2C_STATUS status = 0;

    // Send the Stop signal
    I2CStop(I2CMODULE);
    //delay(10);

    /*  This check was problematic. Sometimes this function would never return.
    // Wait for the signal to complete
  do
    { status = I2CGetStatus(I2CMODULE);
    } while ( !(status & I2C_STOP) ); <--I suspect I2C_STOP. This bit *detects* a stop condition, which doesn't seem like something a master would receive.
    why would master want to detect its own stop condition? I2C_STOP does not appear to detect a *stop* state on the bus, but rather the stop signal itself.
    ...Which is also relative to the receiver, which means recevier_enabled must be TRUE.
  */
}

BOOL I2CTransmitByte( I2C_MODULE i2c_mod, unsigned char data )
{
  int i = 0;

  while(!I2CTransmitterIsReady(i2c_mod));//{ putsUART1("i2ctxbyte waiting 1\n");}

  if(I2CSendByte(i2c_mod, data) == I2C_MASTER_BUS_COLLISION){
    putsUART1("I2CtxByte ERROR I2C Master Bus Collision\n\r");
    return FALSE;
  }

  while(!I2CTransmissionHasCompleted(i2c_mod));//{ putsUART1("i2ctxbyte waiting 2\n");}

  while( !I2CByteWasAcknowledged(I2CMODULE) && (i < 5) ){
    putsUART1("Waiting for ACK in TxByte()\n\r");
    i++;
    //delay(128);
  }

  if(i >= 5){
    putsUART1("ERROR I2CTransmitByte failed\n\r");
    return FALSE;
  }

  return TRUE;
}

void putI2CResult(I2C_RESULT resultVal)
{
  switch(resultVal)
  {
    case I2C_SUCCESS:
      putsUART1("I2C_SUCCESS\n\r\0");
      break;
    case I2C_ERROR:
      putsUART1("I2C_ERROR\n\r\0");
      break;
    case I2C_MASTER_BUS_COLLISION:
      putsUART1("I2C_MASTER_BUS_COLLISION\n\r\0");
      if(I2CMODULE == I2C1)
          I2C1STAT &= 0xFBFF;
      if(I2CMODULE == I2C2)
          I2C2STAT &= 0xFBFF;
      break;
    case I2C_RECEIVE_OVERFLOW:
      putsUART1("I2C_RECEIVE_OVERFLOW\n\r\0");
      break;
    default:
      putsUART1("WARN: I2C_RESULT error code unknown!\n\r\0");
      break;
  }
}

/*******************************************************************************
  Function:
    BOOL I2CReceiveByte( unsigned char* data )

  Summary:
     Get one byte of data.

  Description:
    This routine checks for recieve overflow if not if data is available get
 *  byte.

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    if(!RecieveOneByte(data))
 *    Success = 0;
    </code>

  Remarks:
    This is a blocking routine that waits for the Stop signal to complete. FROM ONLINE AT http://www.edaboard.com/thread248565.html
  *****************************************************************************/
BOOL I2CReceiveByte(unsigned char* data)
{
    if (!I2CReceiverEnable(I2CMODULE, TRUE) == I2C_RECEIVE_OVERFLOW)
    {    return FALSE;  }
    else
    {
        while(!I2CReceivedDataIsAvailable(I2CMODULE));
        *(data) = I2CGetByte(I2CMODULE);
    }

    return TRUE;
}
