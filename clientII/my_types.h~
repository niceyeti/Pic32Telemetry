

#ifndef MY_TYPES_H
#define MY_TYPES_H

//manually-defined base64 size of an imu vector
#define IMU_BASE64_SIZE 36  //these will need to be manually maintained, or your gonna have issues kid...
#define IMU_SIZE 24

typedef unsigned int U32;
typedef unsigned short int U16;
typedef unsigned char U8;


// IMU data packet: must mirror that used by server/client or misalignment will occur...
typedef struct imuVector{
  short int accX;  //accelerometer vals
  short int accY;
  short int accZ;
  short int gyroRoll; //gyroscope vals
  short int gyroPitch;
  short int gyroYaw;
  short int gyroTemp;
  short int magX;  //magnetometer vals
  short int magY;
  short int magZ;
  short int bmpPa; //barometer / temp vals
  short int bmpTemp;
}IMU;

// generalized minimal packet for a single device, such as the ADXL345 over SPI
typedef struct xyzVector{
  short int X;
  short int Y;
  short int Z;
}xyzVec;

//all the calibration constants needed for reading temp and pressure from the BMP085 (see manual)
typedef struct bmp085Vector{
    short int ac1;
    short int ac2;
    short int ac3;
    unsigned short int ac4;
    unsigned short int ac5;
    unsigned short int ac6;
    short int b1;
    short int b2;
    short int mb;
    short int mc;
    short int md;
    short int oss;
    unsigned long int rawTemp;
    unsigned long int rawPress;
    short int trueTemp;  // temp in 0.1 degrees Celsius increments
    long int truePress;  // Pressure in Pascals
}BMP085;


#endif


