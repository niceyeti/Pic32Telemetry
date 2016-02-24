#ifndef MY_SPI_H
#define MY_SPI_H

#include "my_mcu.h"
#include "my_imu.h"
//#include <peripheral/spi.h>      // spi functions for IMU
#include <peripheral/spi_3xx_4xx.h>


//spi bits: BIT_8 is the R/W bit (1 == read). 8 bit start sequence is: [R/W | AUTOINCREMENT | ADDR_BIT_5 | ... | ADDR_BIT_0]
#define SPI_READ  0x80
#define SPI_WRITE 0x00
#define SPI_AUTO_INCREMENT 0x40  //not sure this is for all devices, or just the L3G4200D gyro manual and ADXL345 accelerometer

/*
typedef struct xyzvec{
  short int X;
  short int Y;
  short int Z;
}XYZVEC;
*/
// spi and pmod wifi methods
void spiInitialize(void);
void spiGets(unsigned char addr, unsigned char data[], int nchars);
void spiPuts(unsigned char addr, unsigned char data[], int nchars);
char spiTransfer (char c);
char spiPutc (char c);  // putc/getc for discrete comms: they toggle SS, so do not support multibyte comms.
char spiGetc(void);
void setup_SPI2(void);
void MRF24WB0MAinitialize();
void spiADXL345GetAccelManual(struct xyzVector* vec);
void spiADXL345GetAccelBurst(struct xyzVector* vec);
void spiADXL345AverageFive(struct xyzVector* vec);
BOOL spiADXL345TestConnection(void);
void spiADXL345Initialize(void);
void getADXL345CtrlRegs(void);
void smartInit(void);


#endif
