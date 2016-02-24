/*
   Device variables for IMU GY-80 9DOF Arduino sensor.
   Compiler warning: addresses must be sent as bytes: using #define does not guarantee byte size.

   Description: Contains implementation for adxl345 accelerometer, y2g9000 gyroscope, bmp085 barometer,
   and hmc5883l magnetometer.
   
*/

#ifndef MY_IMU_H
#define MY_IMU_H

#define L3G4200D_ADDR    0b1101001  //alternate address: 0b1101000
#define L3G4200D_ADDR_W  0b11010010  //110100xb Two possible address permutations: LSB is one or zero depending on SDO pin to allow two Gyros on same I2C bus
#define L3G4200D_ADDR_R  0b11010011

#define L3G4200D_X_L      0x28
#define L3G4200D_X_H      0x29
#define L3G4200D_Y_L      0x2A
#define L3G4200D_Y_H      0x2B
#define L3G4200D_Z_H      0x2C
#define L3G4200D_Z_L      0x2D
#define L3G4200D_TEMP     0x26
#define L3G4200D_FIFO_CTL 0x2E
#define L3G4200D_CTL_REG1 0x20
#define L3G4200D_CTL_REG2 0x21
#define L3G4200D_CTL_REG3 0x22
#define L3G4200D_CTL_REG4 0x23
#define L3G4200D_CTL_REG5 0x24
#define L3G4200D_STATUS   0x27

#define L3G4200D_ID_ADDR 0x0F
#define L3G4200D_ID_VAL  0xD3

#define BMP085_ADDR   0x77
#define BMP085_ADDR_W 0xEE //oxEE write
#define BMP085_ADDR_R 0xEF //oxEF read
#define BMP085_ID_ADDR 0xD0
#define BMP085_ID_VAL 0x55
#define BMP085_GET_TEMP 0x2E
#define BMP085_GET_PRESS 0x34
#define BMP085_CTRL   0xF4
#define BMP085_DO1    0xF6  //Data output regs 1 and 2: pressure and temperature are output from these.
#define BMP085_DO2    0xF7  //The sequence is start/select measurement of temp or press, then read the vals here.
#define BMP085_AC1_H  0xAA  //ERROR OUT if reading any of these calib. reg's returns 0x0 or 0xFF
#define BMP085_AC1_L  0xAB
#define BMP085_AC2_H  0xAC
#define BMP085_AC2_L  0xAD
#define BMP085_AC3_H  0xAE
#define BMP085_AC3_L  0xAF
#define BMP085_AC4_H  0xB0
#define BMP085_AC4_L  0xB1
#define BMP085_AC5_H  0xB2
#define BMP085_AC5_L  0xB3  //ERROR OUT if reading any of these calib. reg's returns 0x0 or 0xFF
#define BMP085_AC6_H  0xB4
#define BMP085_AC6_L  0xB5
#define BMP085_B1_H   0xB6
#define BMP085_B1_L   0xB7
#define BMP085_B2_H   0xB8
#define BMP085_B2_L   0xB9
#define BMP085_MB_H   0xBA
#define BMP085_MB_L   0xBB  //ERROR OUT if reading any of these calib. reg's returns 0x0 or 0xFF
#define BMP085_MC_H   0xBC
#define BMP085_MC_L   0xBD
#define BMP085_MD_H   0xBE
#define BMP085_MD_L   0xBF


#define HMC5883L_ADDR   0x1E  //0b0011110
#define HMC5883L_ADDR_R 0x3D
#define HMC5883L_ADDR_W 0x3C
#define HMC5883L_CFIG_A  0x0
#define HMC5883L_CFIG_B  0x1
#define HMC5883L_MODE  0x2
#define HMC5883L_X_H  0x3  //data vals are signed, in range 0xF800 to 0x07FF
#define HMC5883L_X_L  0x4
#define HMC5883L_Y_H  0x5
#define HMC5883L_Y_L  0x6
#define HMC5883L_Z_H  0x7
#define HMC5883L_Z_L  0x8
#define HMC5883L_STATUS  0x9
#define HMC5883L_ID_ADDR  0xA
#define HMC5883L_ID_VAL  0x48

// Accelerometer: The output data is twos complement, with DATAx0 as the
// least significant byte and DATAx1 as the most significant byte, where x represent X, Y, or Z.
//search adxl345 pdf for this register's usage. Device is in standby on start measurement enabled by writing bit D3 (BIT_3, starting from 0)
//which should be done only after dev is configured.
#define ADXL345_POWER_CTRL  0x2D
#define ADXL345_PWR_CTRL  0x2D
#define ADXL345_PCTL_AUTOSLEEP_BIT  0x04
#define ADXL345_PCTL_MEASURE_BIT 0x08   //bit_4
#define ADXL345_ADDR_W     0xA6 // 0b10100110    0x3A, 0b00111010 alternate addr
#define ADXL345_ADDR_R     0xA7 // 0b10100111    0x3B, 0b00111011 alternate addr
#define ADXL345_ADDR       0x53
#define ADXL345_ID_ADDR    0x0  // yup, zero
#define ADXL345_ID_VAL     0xE5
#define ADXL345_FIFO_CTRL  0x38
#define ADXL345_DATA_FRMT  0x31
#define ADXL345_X_L        0x32
#define ADXL345_X_H        0x33
#define ADXL345_Y_L        0x34
#define ADXL345_Y_H        0x35
#define ADXL345_Z_L        0x36
#define ADXL345_Z_H        0x37

#define ADXL345_READ_FAIL  0x01
#define L3G4200D_READ_FAIL 0x02
#define HMC5883L_READ_FAIL 0x04
#define BMP085_READ_FAIL   0x08


#include "my_i2c.h"

//primary
void sampleIMU(IMU* last, BMP085* bmp);
short int lazyMid(short int x1, short int x2, short int x3);
void filterImuSample(IMU readings[2], IMU* last, unsigned char status);
unsigned char readIMU(struct imuVector* imu, struct bmp085Vector* bmpVec);
int initializeIMU(struct bmp085Vector *barVec);

//testers
BOOL ADXL345TestConnection(void);
BOOL L3G4200DTestConnection(void);
BOOL HMC5883LTestConnection(void);
BOOL BMP085TestConnection(void);

//self testers
void ADXL345SelfTest(void);

//util
void clrImuVector(struct imuVector* imu);
void clrBmpVector(struct bmp085Vector* bmp);

//comm
//void SerializeImuVector(IMU* vec,char* buf);

//getters
BOOL HMC5883LGetMag(unsigned char magBytes[]);
BOOL L3G4200DGetGyro(unsigned char gyroBytes[]);
BOOL ADXL345GetAccel(unsigned char accBytes[]);
BOOL BMP085GetTempAndPressure(struct bmp085Vector *bmpVec);

//inits
BOOL InitializeADXL345(void);
BOOL InitializeHMC5883L(void);
BOOL InitializeL3G4200D(void);
BOOL InitializeBMP085(struct bmp085Vector *barVec);

//bmp085 util
BOOL BMP085GetCalCoefficients(struct bmp085Vector *barVec);
BOOL BMP085SetCalCoefficients(void);
void BMP085CalcTempAndPressure(struct bmp085Vector *barVec);





#endif

