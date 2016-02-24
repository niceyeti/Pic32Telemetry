#include "my_imu.h"

const char base64EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
  Function sampleIcMU()

  Reads the IMU twice and takes in the last imu values, then takes mid of the
  three as an estimate of the current imu state. Using last vals is nice
  because it cuts out a third imu reading, and also provides minute feedforward input
  from one state to the next.

  Post-condition: *last will hold the current state estimate after this executes.

  This function and filter() aren't really necessary, since the devices themselves
  can usually be set to average [n] readings internally before updating their internal data.
  Sometimes a 255 will be spit out, but maybe these are specific cases that can be detected.

*/
void sampleIMU(IMU* last, BMP085* bmp)
{
  unsigned char status = 0;

  IMU imuData[2];

  status =  readIMU(&imuData[0], bmp);
  status |= readIMU(&imuData[1], bmp);

  filterImuSample(imuData, last, status);
}

//find the pseudo-median of three numbers by arranging them in order. 
//this is far from perfect, but velocity of readings should overcome the error rate
short int lazyMid(short int x1, short int x2, short int x3) //x3 represents the oldest data
{
  // x1 >= x2:           `````x2`````x1`````
  if(x1 >= x2){
    if(x1 <= x3){        //x3 in right hole
      return x1;
    }
    else if (x2 >= x3){  //x3 in left hole
      return x2;
    }
    else{
      return x3;         //x3 in middle hole
    }
  }
  // x1 < x2:            `````x1`````x2`````
  else{  
    if(x1 >= x3){        //x3 in left hole
      return x1;
    }
    else if (x2 <= x3){  //x3 in right hole
      return x2;
    }
    else{                //x3 in middle hole
      return x3; 
    }
  }
}

/*
  Filter the 3 readings in a sample by simply taking the majority vote of the three readings.
  This is essentially a classification function. The readings with the least numerical distance
  between them are assumed valid.  So if a device reads { 10, 11, -200 } then -200 is omitted.
  Instead of averaging the two valid values, just return the most recent one.

  I like the last parameter.  This way the previous read (state) provides input into the estimation
  of the current value.

  When a val is changing rapidly, the outlier will usually be the most accurate value, at the leading edge
  of a new state.  Taking the mid just makes for a "settle time" but only at this atomic level of resolution.

  There are matters of accuracy which resolution is likelier to resolve than *perfection* here.

*/
void filterImuSample(IMU readings[2], IMU* last, unsigned char status)
{
  
  //if( !(status & ADXL345_READ_FAIL))  //check this later
  last->accX = lazyMid(readings[0].accX, readings[1].accX, last->accX);  
  last->accY = lazyMid(readings[0].accY, readings[1].accY, last->accY);  
  last->accZ = lazyMid(readings[0].accZ, readings[1].accZ, last->accZ);  

  last->gyroRoll = lazyMid(readings[0].gyroRoll, readings[1].gyroRoll, last->gyroRoll);  
  last->gyroPitch = lazyMid(readings[0].gyroPitch, readings[1].gyroPitch, last->gyroPitch);  
  last->gyroPitch = lazyMid(readings[0].gyroYaw, readings[1].gyroYaw, last->gyroYaw);  
  last->gyroTemp = readings[0].gyroTemp;  //temp doesn't need correction

  last->magX = lazyMid(readings[0].magX, readings[1].magX, last->magX);  
  last->magY = lazyMid(readings[0].magY, readings[1].magY, last->magY);  
  last->magX = lazyMid(readings[0].magZ, readings[1].magZ, last->magZ);  
/*
  last->bmpPa = lazyMid(readings[0].bmpPa, readings[1].bmpPa, last->bmpPa);  
  last->bmpTemp = lazyMid(readings[0].bmpTemp, readings[1].bmpTemp, last->bmpTemp);
*/
}

/*
  Fail proof, but devices must be initialized before calling this.
  There's actually not much reason to format the unsigned chars to ints, since master
  could do this.  This version is just for debugging.

  Returns: an integer bit string representing device failures.  0b0000 indicates all succeeded.
    0b0001 = 1 = ADXL345 FAIL
    0b0010 = 2 = L3G4200D FAIL
    0b0100 = 4 = HMC5883L FAIL
    0b1000 = 8 = BMP085 FAIL
*/
unsigned char readIMU(struct imuVector* imu, struct bmp085Vector* bmpVec)
{
  unsigned int i = 0;
  unsigned char result = 0, dataBytes[20] = {'\0'};

  //clrImuVector(imu);

  //putsUART1("debugA\n\r");

  //get all the imu Vals
  if( ADXL345TestConnection() ){
    ADXL345GetAccel(dataBytes);
  }
  else{
    result |= ADXL345_READ_FAIL;
  }

  //putsUART1("debugB\n\r");
  if( L3G4200DTestConnection() ){
    L3G4200DGetGyro(dataBytes + 6);
  }
  else{
    result |= L3G4200D_READ_FAIL;
  }

  if( HMC5883LTestConnection() ){
    HMC5883LGetMag(dataBytes + 13);
  }
  else{
    result |= HMC5883L_READ_FAIL;
  }
/*
  if( BMP085TestConnection() ){
    BMP085GetTempAndPressure(bmpVec);
  }
  else{
    result |= BMP085_READ_FAIL;
  }
*/

  imu->accX = ((short int)dataBytes[1] << 8) | (0x00FF & dataBytes[0]);  //accelerometer vals
  imu->accY = ((short int)dataBytes[3] << 8) | (0x00FF & dataBytes[2]);
  imu->accZ = ((short int)dataBytes[5] << 8) | (0x00FF & dataBytes[4]);
  imu->gyroRoll = ((short int)dataBytes[7] << 8) | (0x00FF & dataBytes[6]); //gyroscope vals
  imu->gyroPitch = ((short int)dataBytes[9] << 8) | (0x00FF & dataBytes[8]);
  imu->gyroYaw = ((short int)dataBytes[11] << 8) | (0x00FF & dataBytes[10]);
  imu->gyroTemp = 0x00FF & dataBytes[12];  //the solo additional temp val from the gyro
  imu->magX = ((short int)dataBytes[13] << 8) | (0x00FF & dataBytes[14]);  //magnetometer vals: notice that MSB/LSB is reversed for the HMC5883L
  imu->magY = ((short int)dataBytes[15] << 8) | (0x00FF & dataBytes[16]);
  imu->magZ = ((short int)dataBytes[17] << 8) | (0x00FF & dataBytes[18]);
  imu->bmpPa =  bmpVec->truePress; //barometer / temp vals
  imu->bmpTemp =  0xFFFF & bmpVec->trueTemp;

  return result;
}

/*
short int imu_htons(short int shrt)
{
  return ((shrt & 0x00FF) << 8) || ((shrt & 0xFF00) >> 8);
}

/*
  htons and then base64's the imuVector

void encodedImuVector(IMU* vec, char obuf[])
{
  int i, tabIndex, padBytes, nSegments, nBytes = sizeof(struct imuVector);
  unsigned char* data = (unsigned char*)&vec;
   
  nSegments = sizeof(struct imuVector) / 3;
  padBytes = 3 - nBytes % 3;

  //htons all the vals
  vec->accX = htons(vec->accX);
  vec->accY = imu_htons(vec->accY);
  vec->accZ = imu_htons(vec->accZ);
  vec->gyroRoll = imu_htons(vec->gyroRoll);
  vec->gyroPitch = imu_htons(vec->gyroPitch);
  vec->gyroYaw = imu_htons(vec->gyroYaw);
  vec->gyroTemp = imu_htons(vec->gyroTemp);
  vec->magX = imu_htons(vec->magX);
  vec->magY = imu_htons(vec->magY);
  vec->magZ = imu_htons(vec->magZ);
  vec->bmpPa = imu_htons(vec->bmpPa);
  vec->bmpTemp = imu_htons(vec->bmpTemp);

  for(i = 0, j= 0; i < nSegments; i += 3){
    //get the first output byte
    tabIndex = (data[i] & 0xFC) >> 2;
    obuf[j++] = base64EncodeTable[tabIndex];
    //get second
    tabIndex = ((data[i] & 0x03) << 6) || ((data[i+1] & 0xF0) >> 4);
    obuf[j++] = base64EncodeTable[tabIndex];
    //get third
    tabIndex = ((data[i+1] & 0x0F) << 2) || ((data[i+2] & 0xC0) >> 6);
    obuf[j++] = base64EncodeTable[tabIndex];
    //get fourth
    tabIndex = (data[i+2] & 0x3F) >> 2;
    obuf[j++] = base64EncodeTable[tabIndex];
  }

  if(padBytes > 0){
    //get the first output byte, as normal
    tabIndex  = (data[i] & 0xFC) >> 2;
    obuf[j++] = base64EncodeTable[tabIndex];

    if(padBytes == 1){
      //get second, as normal
      tabIndex  = ((data[i] & 0x03) << 6) || ((data[i+1] & 0xF0) >> 4);
      obuf[j++] = base64EncodeTable[tabIndex];
      //get third
      tabIndex  = (data[i+1] & 0x0F) << 2;
      obuf[j++] = base64EncodeTable[tabIndex];
      //fourth is pad
      obuf[j++] = '=';
    }
    else if(padBytes == 2){
      //get half of second
      tabIndex  = (data[i] & 0x03) << 6;
      obuf[j++] = base64EncodeTable[tabIndex];
      //third and fourth are pad char
      obuf[j++] = '=';
      obuf[j++] = '=';
    }
  }

}

/*
  Returns base-64 encoding of imu vector for putting on a wire.
  This function is for 32 bit MCUs only.

void SerializeImuVector(IMU* vec, unsigned char* obuf)
{
  int i, padding, numBytes = sizeof(struct imuVector);
  unsigned char* start;

  htonsImuVector(vec);
   

}
*/

/*
  bmpVec param is required to store the cal data of the BMP085 on start up.

  Returns 0 for Success. Non-zero returns:
    0b0001 = 1 = ADXL345 FAIL
    0b0010 = 2 = L3G4200D FAIL
    0b0100 = 4 = HMC5883L FAIL
    0b1000 = 8 = BMP085 FAIL

*/
int initializeIMU(struct bmp085Vector *bmpVec)
{
  int status = 0;

  putsUART1("ADXL345 initialize\n");
  if( !InitializeADXL345() ){
    putsUART1("ADXL345 Failed to initialize\n");
    status |= 0b0001;
  }
  delay(128);
  //putsUART1("success 1\n\r");
  if( !InitializeL3G4200D() ){
    //putsUART1("L3G4200D Failed to initialize\n\r");
    status |= 0b0010;
  }
  delay(128);
  //putsUART1("success 2\n\r");
  if( !InitializeHMC5883L() ){
    //putsUART1("HMC5883L Failed to initialize\n\r");
    status |= 0b0100;
  }
  delay(128);
  //putsUART1("success 3\n");
/*
  if( !InitializeBMP085(bmpVec) ){
    //putsUART1("BMP085 Failed to initialize\n\r");
    status |= 0b1000;
  }
*/
  if(status != 0){
    putsUART1("IMU failed to initialize completely\n\r");
  }
  //putsUART1("success 4\n");

  return status;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////BMP085 code/////////////////////////////////////////////////
/*
 The BMP085 includes a temp and atmospheric pressure sensor, but the
 raw temp and pressure outputs must be fed to calculations which also require
 factory-set calibration values.  Here we test the connection by verifying a read
 of the BMP085 id reg, then verify reading the calibration coefficients so that we have
 them in memory.
*/
BOOL InitializeBMP085(struct bmp085Vector *barVec)
{

  if( !BMP085TestConnection() ){
    //putsUART1("ERROR BMP085 init() failed\n\r");
    return FALSE;
  }

  return BMP085GetCalCoefficients(barVec);
}

// Must return 0x55 for successful connection
BOOL BMP085TestConnection(void)
{
  unsigned char testByte = 0;
  //char debugStr[64] = {'\0'};
  struct i2cDev dev;
    dev.devAddr = BMP085_ADDR;
    dev.subAddr = BMP085_ID_ADDR;

  i2cGetc(&dev, &testByte);
  //sprintf(debugStr,"Id of reg[%d]: 0x%x\n\r",dev.subAddr,(int)testByte);
  //putsUART1(debugStr);

  return testByte == BMP085_ID_VAL;
}

//Gets the calibration coefficients from BMP085 EEPROM; needed for calculating pres and temp data (see manual)
BOOL BMP085GetCalCoefficients(struct bmp085Vector *barVec)
{
  unsigned char byteStr[22] = {'\0'};
  BOOL success = FALSE;
  unsigned char debugStr[256] = {'\0'};
  unsigned short int MSB = 0, LSB = 0;
  short int shints[11] = {0};
  short int* nP = (short int*)barVec;  //point nP at first short entry in barVec struct; allows sequential iteration over struct
  int i = 0;
  struct i2cDev dev;
    dev.devAddr = BMP085_ADDR;
    dev.subAddr = BMP085_AC1_H; // 0xAA, address of first EEPROM register for reading Calibration regs sequentially

  //putsUART1("buff\n\r");
  //read the 22 hi/lo bytes of cal regs
  if( i2cGetsManualIncrement(&dev, byteStr, 22) ){
    //putsUART1("here2\n\r");
    //convert the bytes to short ints and store in barVec

    //get the first three *signed* ints
    MSB = ((short int)byteStr[0]) << 8;
    LSB = 0x00FF & (short int)byteStr[1];  //had to enumerate instead of iterate because of the scattered sign/unsign int bmp085 struct
    barVec->ac1 = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[2]) << 8;
    LSB = 0x00FF & (short int)byteStr[3];
    barVec->ac2 = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[4]) << 8;
    LSB = 0x00FF & (short int)byteStr[5];
    barVec->ac3 = (short int)(MSB | LSB);

    //get the three *unsigned* ints
    MSB = ((unsigned short int)byteStr[6]) << 8;
    LSB = 0x00FF & (unsigned short int)byteStr[7];
    barVec->ac4 = (unsigned short int)(MSB | LSB);

    MSB = ((unsigned short int)byteStr[8]) << 8;
    LSB = 0x00FF & (unsigned short int)byteStr[9];
    barVec->ac5 = (unsigned short int)(MSB | LSB);

    MSB = ((unsigned short int)byteStr[10]) << 8;
    LSB = 0x00FF & (unsigned short int)byteStr[11];
    barVec->ac6 = (unsigned short int)(MSB | LSB);

    //get the five remaining *signed* ints
    MSB = ((short int)byteStr[12]) << 8;
    LSB = 0x00FF & (short int)byteStr[13];
    barVec->b1 = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[14]) << 8;
    LSB = 0x00FF & (short int)byteStr[15];
    barVec->b2 = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[16]) << 8;
    LSB = 0x00FF & (short int)byteStr[17];
    barVec->mb = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[18]) << 8;
    LSB = 0x00FF & (short int)byteStr[19];
    barVec->mc = (short int)(MSB | LSB);

    MSB = ((short int)byteStr[20]) << 8;
    LSB = 0x00FF & (short int)byteStr[21];
    barVec->md = (short int)(MSB | LSB);

    /*
    //debug
    sprintf(debugStr,"cal vals: %d %d %d %d %d\n\r\t%d %d %d %d %d %d\n\r\0",
            barVec->ac1,barVec->ac2,barVec->ac3,barVec->ac4,barVec->ac5,
            barVec->ac6,barVec->b1,barVec->b2,barVec->mb,barVec->mc,barVec->md);
    putsUART1(debugStr);
    //delayMS(3000);
    */

    success = TRUE;
  }
  else{
    success = FALSE;
    putsUART1("ERROR i2cGets() failed in BMP085GetCalCoefficients()\n\r");
  }

  return success;
}

/*
   Read and calculate temp value. Getting pressure depends on getting temp, hence 2 in 1.
   GetTemp() could be its own function, but getPRess() would require getTemp().
   See page 12 of the BMP085 manual.
*/
BOOL BMP085GetTempAndPressure(struct bmp085Vector *barVec)
{
  int i = 0;
  unsigned char txByte = '\0', d0 = '\0', d1 = '\0';
  struct i2cDev dev;
    dev.devAddr = BMP085_ADDR;
    dev.subAddr = BMP085_GET_TEMP;

 /*
  //debug
  char debugStr[256] = {'\0'};
  sprintf(debugStr,"cal vals: %d %d %d %d %d\n\r\t%d %d %d %d %d %d\n\r\0",
            barVec->ac1,barVec->ac2,barVec->ac3,barVec->ac4,barVec->ac5,
            barVec->ac6,barVec->b1,barVec->b2,barVec->mb,barVec->mc,barVec->md);
  putsUART1(debugStr);
*/

  //putsUART1("debug getTemp()\n\r");
  barVec->oss = 0;  //set oversampling to zero; this may need to be 3 (??), if you look at manual's formula and vals

  if( !BMP085TestConnection() ){
    putsUART1("Connection failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  dev.subAddr = BMP085_CTRL;
  txByte = BMP085_GET_TEMP;
  if( !i2cPutc(&dev, &txByte) ){
    putsUART1("Selecting temp reg failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  //delay at least 4.5 ms
  delayMS(20);

  dev.subAddr = BMP085_DO1; //0xF6 data-out MSB reg
  if( !i2cGetc(&dev, &d0) ){
    putsUART1("Read DO1 failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  dev.subAddr = BMP085_DO2; //0xF7 data-out LSB reg
  if( !i2cGetc(&dev, &d1)){
    putsUART1("Read D02 failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  barVec->rawTemp = ((unsigned short int)d0 << 8) | (0x000000FF & (unsigned short int)d1);

  //now fetch the raw pressure val
  dev.subAddr = BMP085_CTRL;  // control reg 0xF4
  txByte = BMP085_GET_PRESS;  // 0x34 (OR'ed with OSS selection; 0 default OK; see manual)
  if( !i2cPutc(&dev, &txByte) ){
    putsUART1("Selecting pressure reg failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  //delay at least 4.5 ms
  delayMS(20);

  dev.subAddr = BMP085_DO1; //0xF6 data-out MSB reg
  if( !i2cGetc(&dev, &d0) ){
    putsUART1("Read DO1 press failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  dev.subAddr = BMP085_DO2; //0xF7 data-out LSB reg
  if( !i2cGetc(&dev, &d1)){
    putsUART1("Read DO2 press failed in BMP085getTempAndPress()\n\r");
    return FALSE;
  }

  barVec->rawPress = ((unsigned short int)d0 << 8) | (0x000000FF & (unsigned short int)d1);

  BMP085CalcTempAndPressure( barVec );

  return TRUE;
}

//All based on manual formulae. barVec must be loaded with cal coefficients before call.
// Temp valu is in 0.1 Celsius format (so 150 == 15.0 deg celsius), pressure in Pa's.
void BMP085CalcTempAndPressure(struct bmp085Vector *barVec)
{
	int  tval = 0, pval = 0;
	long int  x1 = 0, x2 = 0, x3 = 0, b3 = 0, b5 = 0, b6 = 0, p = 0, b7 = 0;
	unsigned long int  b4 = 0;
	//unsigned char oss = 3;

	x1 = ((long int)((barVec->rawTemp - barVec->ac6) * barVec->ac5)) >> 15;
  x2 = ((long int)barVec->mc << 11) / (x1 + (long int)barVec->md);
	b5 = x1 + x2;
	barVec->trueTemp = (unsigned short int)((b5 + 8) >> 4);

	b6 = b5 - 4000;
	x1 = ((long int)barVec->b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = ((long int)barVec->ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((long int)barVec->ac1 * 4 + x3) << (barVec->oss + 2)) >> 2;
	x1 = ((long int)barVec->ac3 * b6) >> 13;
	x2 = ((long int)barVec->b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = (x1 + x2 + 2) >> 2;
	b4 = ((long int)barVec->ac4 * (unsigned long int)(x3 + 32768)) >> 15;
	b7 = ((unsigned long int)barVec->rawPress - b3) * (50000 >> barVec->oss);

  if(b7 < 0x80000000){
    p = (b7 * 2) / b4;
  }
  else{
    p = (b7 / b4) * 2;
  }

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	barVec->truePress = (unsigned short int) (p + ((x1 + x2 + 3791) >> 4));
}


/* Cal coefficients are set at the factory; I dont believe we need to set these. Ever.
//Vals are fixed (see manual), and must be loaded at configuration time.
BOOL BMP085SetCalCoefficients(void)
{
  int i = 0;
  struct i2cDev dev;
  struct bmp085Vector barVec;
  unsigned char regVals[22] = {'\0'};
  char debugStr[128] = {'\0'};

  dev.devAddr = BMP085_ADDR;
  dev.subAddr = BMP085_AC1_H;

  regVals[0] = (unsigned char)(0x000000FF & (408 >> 8));  // MSB comes first,
	regVals[1] = (unsigned char)(0x000000FF & 408);         // then LSB, for each pair of lines.

  regVals[2] = (unsigned char)(0x000000FF & (-72 >> 8));
	regVals[3] = (unsigned char)(0x000000FF & -72);

  regVals[4] = (unsigned char)(0x000000FF & (-14383 >> 8));
	regVals[5] = (unsigned char)(0x000000FF & -14383);

  regVals[6] = (unsigned char)(0x000000FF & (32741 >> 8));
	regVals[7] = (unsigned char)(0x000000FF & 32741);

  regVals[8] = (unsigned char)(0x000000FF & (32757 >> 8));
	regVals[9] = (unsigned char)(0x000000FF & 32757);

  regVals[10] = (unsigned char)(0x000000FF & (23153 >> 8));
	regVals[11] = (unsigned char)(0x000000FF & 23153);

  regVals[12] = (unsigned char)(0x000000FF & (6190 >> 8));
	regVals[13] = (unsigned char)(0x000000FF & 6190);

  regVals[14] = (unsigned char)(0x000000FF & (4 >> 8));
	regVals[15] = (unsigned char)(0x000000FF & 4);

  regVals[16] = (unsigned char)(0x000000FF & (-32767 >> 8));
	regVals[17] = (unsigned char)(0x000000FF & -32767);

  regVals[18] = (unsigned char)(0x000000FF & (-8711 >> 8));
	regVals[19] = (unsigned char)(0x000000FF & -8711);

  regVals[20] = (unsigned char)(0x000000FF & (2868 >> 8));
  regVals[21] = (unsigned char)(0x000000FF & 2868);


  sprintf(debugStr,"Vals: %d %d %d %d %d %d\n\r\0",(int)((regVals[0] << 8) |regVals[1]), (int)((regVals[2] << 8) |regVals[3]), (int)((regVals[4] << 8) |regVals[5]), (int)((regVals[6] << 8) |regVals[7]), (int)((regVals[8] << 8) |regVals[9]),(int)((regVals[10] << 8) |regVals[11]));
  putsUART1(debugStr);
  clearStr(debugStr,128);
  sprintf(debugStr,"Vals: %d %d %d %d %d %d\n\r\0",(int)((regVals[12] << 8) |regVals[13]), (int)((regVals[14] << 8) |regVals[15]), (int)((regVals[16] << 8) |regVals[17]), (int)((regVals[18] << 8) |regVals[19]), (int)((regVals[20] << 8) |regVals[21]),(int)((regVals[22] << 8) |regVals[23]));
  putsUART1(debugStr);


  for(i = 0; i < 22; i++, dev.subAddr++){
    if( !i2cPutc(&dev, regVals+i) ){
      return FALSE;
    }
  }

  return TRUE;
}
*/


/////////////////////////////////////////////////////////////////////////////
/////////////////ADXL345 code////////////////////////////////////////////////
BOOL ADXL345TestConnection(void)
{
  unsigned char testByte = 0;
  //char debugStr[64] = {'\0'};
  struct i2cDev dev;
    dev.devAddr = ADXL345_ADDR;
    dev.subAddr = ADXL345_ID_ADDR;

  i2cGetc(&dev, &testByte);
  //sprintf(debugStr,"Id of reg[%d]: 0x%x\n\r",dev.subAddr,(int)testByte);
  //putsUART1(debugStr);

  return testByte == ADXL345_ID_VAL;
}

/*
  ADXL345 self test: from manual "It is recommended that the
part be set to full-resolution, 16 g mode to ensure that there is
sufficient dynamic range for the entire self-test shift."
  See the Quickstart guide for test algorithm.

*/
void ADXL345SelfTest(void)
{
  return;
}

BOOL InitializeADXL345(void)
{
  char debug[64] = {'\0'};
  unsigned char byte = 0;
  struct i2cDev dev;

  dev.devAddr = ADXL345_ADDR;  // gives 0xA6 for write, 0xA7 for read
  dev.subAddr = ADXL345_POWER_CTRL;
  byte = 0;

  //putsUART1("in init\n");

  if( i2cPutc(&dev, &byte) ){ // reset all power settings to 0
    delay(TINY_DLY);
    //putsUART1("debug1\n\r");
    //set autosleep enabled bit (bit 4) in pwr_ctl reg 0x2d
      //setAutoSleepEnabled(TRUE); I2Cdev::writeBit(devAddr, ADXL345_RA_POWER_CTL, ADXL345_PCTL_AUTOSLEEP_BIT, enabled);
    if(i2cWriteBit(&dev, (unsigned char)4, TRUE) ){ //ADXL345_PCTL_AUTOSLEEP_BIT == 4
      //putsUART1("debug1\n\r");
      delay(TINY_DLY);
      /*  //outputs 0x10
      i2cGetc(&dev, &byte);
      sprintf(debug,"ADXL345_DATA_FRMT: %x\n",(int)byte);
      putsUART1(debug);
      */

      /* code segment doesn't work: tried many configs of range and full-res bits, but in this state all reads are the same (all zeroes or just some previously stored x, y, z vals over and over)
      dev.subAddr = ADXL345_DATA_FRMT;
      i2cGetc(&dev, &byte);

      byte |= 0b00000001;
      i2cPutc(&dev, &byte);

      //no dice here either: now write to the data format register, select resolution mode and data output range
      //dev.subAddr = ADXL345_DATA_FRMT;
      //byte = 0b00000000;    //write 0b00001001 : select full-res mode and +-4g range (this is mostly of arbitrary)
      //if(i2cPutc(&dev, &byte) ){
      */
        //enable measurement
        if(i2cWriteBit(&dev, (unsigned char)3, TRUE)){  //ADXL345_PCTL_MEASURE_BIT = bit_3
          //putsUART1("debug3\n\r");
          delay(TINY_DLY);
          return TRUE;
        }
      //}
    }
  }

  putsUART1("ERROR InitializeADXL345() FAIL\n\r");
  return FALSE;
}

/*  Z value testing (modify to test any particular register / data val)
  dev.devAddr = ADXL345_ADDR;
  dev.subAddr = ADXL345_X_L;
  i2cGetc(&dev, &TxByte);
  zLo = (0x00FF & (unsigned short int)TxByte);
  dev.subAddr = ADXL345_X_H;
  i2cGetc(&dev, &TxByte);
  zHi = (0x00FF & (unsigned short int)TxByte);

  sprintf(accStr,"Acc Z val: %d\n", (int)((short int)(zHi << 8) | zLo)  );
  putsUART1(accStr);
*/



/*
  The following code was tested successfully in main:
    //Z value is a high value due to gravity (?)
    dev.subAddr = ADXL345_X_L;
    i2cGetc(&dev, &(testStr[0]) );
    dev.subAddr = ADXL345_X_H;
    i2cGetc(&dev, &(testStr[1]) );
    dev.subAddr = ADXL345_Y_L;
    i2cGetc(&dev, &(testStr[2]) );
    dev.subAddr = ADXL345_Y_H;
    i2cGetc(&dev, &(testStr[3]) );
    dev.subAddr = ADXL345_Z_L;
    i2cGetc(&dev, &(testStr[4]) );
    dev.subAddr = ADXL345_Z_H;
    i2cGetc(&dev, &(testStr[5]) );

    //format the vals
    XL = 0x000000FF & testStr[0];
    XH = 0x000000FF & testStr[1];
    YL = 0x000000FF & testStr[2];
    YH = 0x000000FF & testStr[3];
    ZL = 0x000000FF & testStr[4];
    ZH = 0x000000FF & testStr[5];
    X = (XH << 8) | XL;
    Y = (YH << 8) | YL;
    Z = (ZH << 8) | ZL;

    sprintf(accStr,"Acc(X,Y,X)  0x%04X  0x%04X  0x%04X\n\r",X,Y,Z);
    putsUART1(accStr);

    X = 0;   Y = 0;  Z = 0;
    XL = 0; YL = 0; ZL = 0;
    XH = 0; YH = 0; ZH = 0;
    clearStr(accStr,64);
    clearStr(testStr,64);

Read in XL,XH,YL,YH,ZL,ZH accel vals

  From the manual : "After the register
    addressing and the first byte of data, each subsequent set of
    clock pulses (eight clock pulses) causes the ADXL345 to point
    to the next register for a read or write."



*/
BOOL ADXL345GetAccel(unsigned char accBytes[])
{
  //short int zLo, zHi;
  //char c;
  //char accStr[64] = {'\0'};
  struct i2cDev adxl345;
    adxl345.devAddr = ADXL345_ADDR;
    adxl345.subAddr = ADXL345_X_L;

  //ADXL345 autoincrement not yet implemented, so loop and increment the subAddr manually
  //multibyte read is suggested so we get the consistent readings between reading hi/lo byte register
/*
  if( !i2cGets(&adxl345, accBytes,6) ){
    putsUART1("ERROR i2cGets() failed in ADXL345GetAccel()\n\r");
    return FALSE;
  }
*/

  if( !i2cGetsManualIncrement(&adxl345,accBytes,6) ){
    putsUART1("ERROR i2cGets() failed in ADXL345GetAccel()\n\r");
    return FALSE;
  }

/*
  //z value testing
  adxl345.devAddr = ADXL345_ADDR;
  adxl345.subAddr = ADXL345_Z_L;
  i2cGetc(&adxl345, &c);
  zLo = (0x00FF & (unsigned short int)c);
  adxl345.subAddr = ADXL345_Z_H;
  i2cGetc(&adxl345, &c);
  zHi = (0x00FF & (unsigned short int)c);
  sprintf(accStr,"Acc Z val: %d\n", (int)((short int)(zHi << 8) | zLo)  );
  putsUART1(accStr);
*/


  //sprintf(accStr,"getAcc(Z):  0x%04X\n\r", (0x0000FFFF & ((int)accBytes[1] << 8)) | accBytes[0],(0x0000FFFF & ((int)accBytes[3] << 8)) | accBytes[2],(0x0000FFFF & ((int)accBytes[5] << 8)) | accBytes[4] );
  //sprintf(accStr,"getAcc(Z):  0x%04X\n", ((short)accBytes[5] << 8) | accBytes[4] );
  //putsUART1(accStr);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////L3G4200D code///////////////////////////////////////////////
BOOL L3G4200DTestConnection(void)
{
  unsigned char testByte = 0;
  //char debugStr[64] = {'\0'};
  struct i2cDev dev;
    dev.devAddr = L3G4200D_ADDR;
    dev.subAddr = L3G4200D_ID_ADDR;

  i2cGetc(&dev, &testByte);
  //sprintf(debugStr,"Id of reg[%d]: 0x%x\n\r",dev.subAddr,(int)testByte);
  //putsUART1(debugStr);

  return testByte == L3G4200D_ID_VAL;
}

//turns on gyro, 500 deg/sec sensitivity, BDU mode (ctrl_reg5)
BOOL InitializeL3G4200D(void)
{
  unsigned char byte = 0;
  struct i2cDev dev;

  dev.devAddr = L3G4200D_ADDR;
  dev.subAddr = L3G4200D_CTL_REG1;
  byte = 0x1E;

  if( i2cPutc(&dev, &byte) ){ // set all power settings to 0x1F (power on, some bandwidth selection or other)
    //putsUART1("debug1\n\r");

    // Set scale (500 deg/sec), BDU on (doesnt update output registers until MSB and LSB are read)
    dev.devAddr = L3G4200D_ADDR;
    dev.subAddr = L3G4200D_CTL_REG5;
    byte = 0x90;
    if( i2cPutc(&dev, &byte) ){ //ADXL345_PCTL_AUTOSLEEP_BIT == 4
      //putsUART1("debug2\n\r");
      return TRUE;
    }
  }

  putsUART1("ERROR L3G4200Dinitialize() FAIL\n\r");
  return FALSE;
}

//Get gyro x, y, and Z angular velocities, as well as the gyro's temp sensor reading.
BOOL L3G4200DGetGyro(unsigned char gyroBytes[])
{
  int i = 0;
  //char accStr[64] = {'\0'};
  struct i2cDev gyro;
    gyro.devAddr = L3G4200D_ADDR;
    gyro.subAddr = L3G4200D_X_L;

  //ADXL345 autoincrement not yet implemented, so loop and increment the subAddr manually
  //multibyte read is suggested so we get the consistent readings between reading hi/lo byte register
  /*
  while((success = TRUE) && (i < 6)){
    adxl345.subAddr += i;
    sprintf(str,"subAddr 0x%x\n\r",(int)adxl345.subAddr);
    putsUART1(str);
    success = i2cGetc(&adxl345, inChars[i] );
    i++;
  }
  */

  if( !i2cGetsManualIncrement(&gyro,gyroBytes,6) ){
    putsUART1("ERROR i2cGetsManualIncrement() failed in L3G4200DGetGyro()\n\r");
    return FALSE;
  }

  //now get the temperature
  gyro.subAddr = L3G4200D_TEMP;
  i2cGetc(&gyro,gyroBytes+6);

  //debug
  //sprintf(gyroStr,"getGyro(X,Y,X, temp): 0x%04X  0x%04X  0x%04X  0x%04X\n\r", (0x0000FFFF & ((int)gyroBytes[1] << 8)) | gyroBytes[0],(0x0000FFFF & ((int)gyroBytes[3] << 8)) | gyroBytes[2],(0x0000FFFF & ((int)gyroBytes[5] << 8)) | gyroBytes[4], (0x000000FF & gyroBytes[6]) );
  //putsUART1(gyroStr);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////HMC5883L code///////////////////////////////////////////////
// Must return 0x48 for successful connection
BOOL HMC5883LTestConnection(void)
{
  unsigned char testByte = 0;
  //char debugStr[64] = {'\0'};
  struct i2cDev dev;
    dev.devAddr = HMC5883L_ADDR;
    dev.subAddr = HMC5883L_ID_ADDR;

  i2cGetc(&dev, &testByte);
  //sprintf(debugStr,"Id of reg[%d]: 0x%x\n\r",dev.subAddr,(int)testByte);
  //putsUART1(debugStr);

  return testByte == HMC5883L_ID_VAL;
}

/*
  See manual. Clear BIT_1 in the STATUS register after reading all registers. To preserve measurement state
  for a given read sequence, BIT_1 locks data measurement if not all regs have been read. BIT_7 in the MODE
  register is also set every time a read occurs in single-measurement mode.
*/
BOOL InitializeHMC5883L(void)
{
  unsigned char byte = 0;
  struct i2cDev dev;

  dev.devAddr = HMC5883L_ADDR;  // 0x1E
  dev.subAddr = HMC5883L_CFIG_A; //0x0
  byte = 0x70; // 0b01110000 Clear BIT_7 (manual specs this is necessity), 8 samples averaged per output (default), data output rate 15 hz, Normal measurement mode

  if( i2cPutc(&dev, &byte) ){ // reset all power settings to 0
    //putsUART1("debug1\n\r");
    dev.subAddr = HMC5883L_MODE;
    byte = 0;

    if( i2cPutc(&dev, &byte) ){ // reset all power settings to 0; from manual example
      //putsUART1("debug2\n\r");

      //clear BIT_1 in the STATUS register (measurement lock bit): this is just a precondition
      dev.subAddr = HMC5883L_STATUS;
      if(i2cWriteBit(&dev, (unsigned char)1, FALSE) ){ //ADXL345_PCTL_AUTOSLEEP_BIT == 4
        //putsUART1("HMC5883L init success\n\r");
        return TRUE;
      }
    }
  }

  putsUART1("ERROR InitializeHMC5883L() FAIL\n\r");
  return FALSE;
}

/*
  Get the X, Y, and Z magnetometer vectors.
*/
BOOL HMC5883LGetMag(unsigned char magBytes[])
{
  int i = 0;
  //char accStr[64] = {'\0'};
  struct i2cDev mag;
    mag.devAddr = HMC5883L_ADDR;
    mag.subAddr = HMC5883L_X_H;

  //ADXL345 autoincrement not yet implemented, so loop and increment the subAddr manually
  //multibyte read is suggested so we get the consistent readings between reading hi/lo byte register
  /*
  while((success = TRUE) && (i < 6)){
    adxl345.subAddr += i;
    sprintf(str,"subAddr 0x%x\n\r",(int)adxl345.subAddr);
    putsUART1(str);
    success = i2cGetc(&adxl345, inChars[i] );
    i++;
  }
  */

  if( !i2cGetsManualIncrement(&mag,magBytes,6) ){
    putsUART1("ERROR i2cGetsManualIncrement() failed in HMC5883LGetMag()\n\r");
    return FALSE;
  }

  //debug
  //sprintf(gyroStr,"getGyro(X,Y,X): 0x%04X  0x%04X  0x%04X\n\r", (0x0000FFFF & ((int)gyroBytes[1] << 8)) | gyroBytes[0],(0x0000FFFF & ((int)gyroBytes[3] << 8)) | gyroBytes[2],(0x0000FFFF & ((int)gyroBytes[5] << 8)) | gyroBytes[4] );
  //putsUART1(gyroStr);

  return TRUE;
}



void clrBmpVector(struct bmp085Vector* bmp)
{
  bmp->ac1 = 0;
  bmp->ac2 = 0;
  bmp->ac3 = 0;
  bmp->ac4 = 0;
  bmp->ac5 = 0;
  bmp->ac6 = 0;
  bmp->b1 = 0;
  bmp->b2 = 0;
  bmp->mb = 0;
  bmp->mc = 0;
  bmp->md = 0;
  bmp->oss = 0;
  bmp->rawTemp = 0;
  bmp->rawPress = 0;
  bmp->trueTemp = 0;
  bmp->truePress = 0;
}

void clrImuVector(struct imuVector *imu)
{
  imu->accX = 0;  //accelerometer vals
  imu->accY = 0;
  imu->accZ = 0;
  imu->gyroRoll = 0; //gyroscope vals
  imu->gyroPitch = 0;
  imu->gyroYaw = 0;
  imu->gyroTemp = 0;
  imu->magX = 0;  //magnetometer vals
  imu->magY = 0;
  imu->magZ = 0;
  imu->bmpPa = 0; //barometer / temp vals
  imu->bmpTemp = 0;
}

