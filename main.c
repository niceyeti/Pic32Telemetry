/*******************************************************************************
 * Programmer:                                                                 *
 * Class: CptS 466                                                             *
 * Lab Project:                                                                *
 * Date:                                                                       *
 *                                                                             *
 * Description:  FreeRTOS Project Template                                     *


  Is it better to use user-defined timer interrupt, or use freeRTOS timer objects
  with callbacks?
  MPLAb emulator????

 To run the client from the cmd line, go to ~/Desktop/Microprocs/Serial and check
 * out Serial.c.
 * 
 * 

 ******************************************************************************/

// #include all necessary standard and user-defined libraries
#include <plib.h> // Includes all major functions and macros required to develop
                  // programs for the PIC32MX4
#include <p32xxxx.h> // Need specific PIC32 names for memory regions

/*
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "./Source/include/ConfigPerformance.h"
*/
#include "my_mcu.h"
#include "my_ping.h"
//#include "my_imu.h"
#include "my_spi.h"
#include "my_controller.h"
//#include "stdio.h"
#include "encoding/encoding.h"

// Place your #pragma statements here, or in another .h file;
// #pragma statements are used to set your operating clock frequency

/* SYSCLK = 80 MHz (8 MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care */

/* Oscillator Settings
*/
#pragma config FNOSC = PRIPLL // Oscillator selection
#pragma config POSCMOD = EC // Primary oscillator mode
#pragma config FPLLIDIV = DIV_2 // PLL input divider
#pragma config FPLLMUL = MUL_20 // PLL multiplier
#pragma config FPLLODIV = DIV_1 // PLL output divider
#pragma config FPBDIV = DIV_2 // Peripheral bus clock divider
#pragma config FSOSCEN = OFF // Secondary oscillator enable


static void SetupHardware( void );

/*
  Serializes and packetizes data, then sends it over bluetooth via UART1.

  The degenerate protocol for sending off imu data to the linux client is
  to serialize the data in base64 encoding, and to prefix the data with
  '$$$$'. This means the data is sent as plain ascii text, which helps it get
  through terminals that would otherwise detect null bytes or control characters
  in raw data. The prefix '$$$$' is just a degenerate way of delimiting packets.

*/
void SendDataPacket(char sendbuf[], IMU* imuVec)
{
  int n;

  //packet header-start signal is '$$$$'
  sendbuf[0] = '$';
  sendbuf[1] = '$';
  sendbuf[2] = '$';
  sendbuf[3] = '$';
  //serialize the data
  n = SerializeImuVector(&imuVec,&sendbuf[4]);
  //send it off
  sendUART1(sendbuf,n+4);
}

int main(void)
{
  int a, i, j, success;
  char enc[128] = {'\0'};
  char dec[128] = {'\0'};
  //char magStr[64] = {'\0'};
  //char accStr[64] = {'\0'};
  //char gyroStr[64] = {'\0'};
  //char barStr[64] = {'\0'};
  //char dbg[256] = {'\0'};
  struct xyzVector xyzvec    = {0,0,0};
  struct bmp085Vector bmpVec = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  STATE myState;
  //struct imuVector imuVec[3]    = {0,0,0,0,0,0,0,0,0,0,0,0};
  IMU imuVec = {0,0,0,0,0,0,0,0,0,0,0,0};

	// Setup/initialize devices
  //TODO: failout and recovery logic if dev init fails
  SetupHardware();
  initializeIMU(&bmpVec);
  INTEnableInterrupts();
  
  while(1){

    LATBINV = 0x2400;
    clearStr(enc,64);
    //read the data
    readIMU(&imuVec,&bmpVec);
    //spiADXL345GetAccelBurst(&xyzvec);
    SendImuPacket(enc,&imuVec);
    //Packetize((U8*)&imuVec,sizeof(IMU));
    //SerializeImuVector(&imuVec,enc);
    //my_memcpy((unsigned char*)&enc[4],(unsigned char*)&imuVec, sizeof(IMU));
    //enc[4+sizeof(IMU)] = '\0';
    //puts only works for strings, not data, unless data is first base64 encoded
    //enc[IMU_BASE64_SIZE] = '\0';
    //putsUART1(enc);
    //sendUART1(enc,sizeof(IMU)+4);
    
    

    /*
    readIMU(&imuVec, &bmpVec);
    LATBINV = 0x2400;
    sprintf(dbg,"ac %04x %04x %04x",imuVec.accX,imuVec.accY,imuVec.accZ);
    putsUART1(dbg);
    sprintf(dbg,"gy %04x %04x %04x",imuVec.gyroRoll,imuVec.gyroPitch,imuVec.gyroYaw);
    putsUART1(dbg);
    sprintf(dbg,"mg %04x %04x %04x",imuVec.magX,imuVec.magY,imuVec.magZ);
    putsUART1(dbg);
    sprintf(dbg," %4d %4dc\n",imuVec.bmpPa,imuVec.bmpTemp);
    putsUART1(dbg);

    spiADXL345GetAccelBurst(&xyzvec);
    sprintf(accStr,"spiAcl burst: 0x%4x 0x%4x 0x%4x\n",0x0000FFFF & (unsigned int)xyzvec.X,0x0000FFFF & (unsigned int)xyzvec.Y,0x0000FFFF & (unsigned int)xyzvec.Z);
    putsUART1(accStr);
    */


    //updateState(&myState);
    //sprintf(dbg,"DC %3d accX %4d d_accX %2d dir: %c\n\0", myState.nextDC, myState.sensors[myState.cur].accX, myState.sensors[myState.cur].accX - myState.sensors[myState.last].accX, (myState.motorDir == 0 ? 'R' : 'F'));
    //sprintf(dbg,"nxtDC%5d accX %4d d_accX %d gRoll %8d\n\0", st->nextDC, st->sensors[st->cur].accX, st->sensors[st->cur].accX - st->sensors[st->last].accX, st->sensors[0].gyroRoll);
    //putsUART1(dbg);
  }

	return 0;
}

/*************************************************************
 * Function:                                                 *
 * Date Created:                                             *
 * Date Last Modified:                                       *
 * Description:                                              *
 * Input parameters:                                         *
 * Returns:                                                  *
 * Usages:                                                   *
 * Preconditions:                                            *
 * Postconditions:                                           *
 *************************************************************/

// Put other function definitions below main (), or they
// may go in another .c source file; Functions most likely
// will include port and device setups/initalizations;
// Be sure to comment all functions with the above block

/*-----------------------------------------------------------*/

void SetupHardware(void)
{
  /* Configure the hardware for maximum performance. */
  //vHardwareConfigurePerformance();

  /* Setup to use the external interrupt controller. */
  //vHardwareUseMultiVectoredInterrupts();
  INTDisableInterrupts();
  //portDISABLE_INTERRUPTS();
  TRISBCLR = 0x3C00; // Set on-board LEDs as outputs
  LATBCLR = 0x3C00;
  configurePortIO();
  configOCModule2();
  configOCModule3();
  configureTimer2(PR_BASE);
  //configureInterrupts();		//do we want? yes, may as well write nice input capture
  //configureInputCaptureMods();
  setupUART1(PERIPHERAL_CLOCK); //Andy version of UART setup
  setup_SPI2();
  smartInit();
  i2cConfigModule();
  //configureTimer3(31250);
  configureInterrupts();
}
/*-----------------------------------------------------------*/



