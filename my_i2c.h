/*
  File: i2c.h
  
  Description: i2c i/o functions.
  
  -STILL NEED I2C FAILURE CODE: device needs to be able to recover from bus collisions without rebooting
  
  -sending a pointer output parameter to i2cPutc is non-sensical; convert this parameter to a simple unsigned char

  Sometimes reading after the first device fails (infinite loop at i2cStartTransfer().  I am now printing the i2c status over uart:
    -when failure occurs, reading the status register gives 0x408 (0x400 === I2C_ARBITRATION_LOSS, 0x008 == I2C_START)
  -when successful, status is 0x10  (0x10 == I2C_STOP)

  -helpful: i2cgetstatus() appears to return the status after the last possible transaction. It returned 0x8008 when i was sending NACK NACK STOP before correction.
      After correction (sending only one NACK), getStatus returned 0x10 (STOP), which was the desired state, and reflected the last call to Stop().

*/      

#ifndef MY_I2C_H      
#define MY_I2C_H

#include "my_mcu.h"
#include <peripheral/i2c.h>      // I2C functions for IMU

#define I2CMODULE I2C2  // instead of parameterizing the i2c functions with a module # (which would allow dual i2c usage), I toggle it here, assuming we'll only use one module

//compact struct for communicating with I2C devices
typedef struct i2cDev{
  unsigned char devAddr;
  unsigned char subAddr;
} i2cDevice;

void i2cConfigModule(void);
void putI2CResult(I2C_RESULT resultVal);
BOOL I2CTransmitByte( I2C_MODULE i2c_mod, unsigned char data );
BOOL i2cStartTransfer( BOOL restart );
void i2cStopTransfer( void );
BOOL I2CReceiveByte( unsigned char* data );
void i2cSendAck(I2C_MODULE i2c_mod, BOOL ack);
      
BOOL i2cGetc(struct i2cDev* dev, unsigned char *rxData); //my functions
BOOL i2cPutc(struct i2cDev* dev, unsigned char* txData);
BOOL i2cWriteBit(struct i2cDev* dev, unsigned char bitNum, BOOL enable);
BOOL i2cPuts(struct i2cDev* dev, unsigned char txData[], int n);
BOOL i2cGets(struct i2cDev* dev, unsigned char rxData[], int nBytes);
BOOL i2cGetsManualIncrement(struct i2cDev* dev, unsigned char rxData[], unsigned int nRegs);


#endif
