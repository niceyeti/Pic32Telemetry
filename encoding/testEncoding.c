/*
  This is test code for both the client/server side of the base64 encodings.
  Everything here runs on linux, not on the MCU. The MCU will pretty
  much always utilize serialization functionality, not deserialization
  of state logic packets.

  The tests here are far from complete, compared with a mature email encoder.
  If this ends up being used often, some more rigorous tests will be needed.
*/
#include "stdio.h"
#include "unistd.h"     //for io in test functions only
#include "sys/fcntl.h"
#include "encoding.h"

void TestImuSerialization(void);
void TestSerialize(void);
void TestKoala(void);
void putVector(const IMU* vector);

int main(void)
{
  InitDecodeTable();
  TestSerialize();
  TestImuSerialization();

  //encode and decode the kaola in this directory
  TestKoala();
  return 0;
}

void putVector(const IMU* vector)
{
  printf("\nVector:\n");
  printf("Acl:  %5u %5u %5u\n",vector->accX, vector->accY, vector->accZ);
  printf("Gyr:  %5u %5u %5u %5u\n",vector->gyroRoll, vector->gyroPitch, vector->gyroYaw,vector->gyroTemp);
  printf("Mag:  %5u %5u %5u\n",vector->magX, vector->magY, vector->magZ);
  printf("Bar:  %5u %5u\n\n",vector->bmpPa, vector->bmpTemp);
}

void TestImuSerialization(void)
{
  IMU vec1 = {13,45,234,3435,423,875,12445,9,0,1,8656,76};
  const IMU expected = {13,45,234,3435,423,875,12445,9,0,1,8656,76};
  IMU vec2 = {999,999,999,999,999,999,999,999,999,999,999,999};
  U8 buf[8192] = {'\0'};
  int equal;

  buf[24] = '$';
  buf[25] = '\0';

  printf("vec1 before serialization:\n");
  putVector(&vec1);
  printf("begin enc...\n");
  SerializeImuVector(&vec1,buf);
  printf("vec1 after serialization:\n");
  putVector(&vec1);
  printf("%s\n",buf);
  DeserializeImuVector(buf,&vec2);
  printf("deseralized vector:\n");
  putVector(&vec2);

  equal = vec2.accX == expected.accX;
  equal = equal && (vec2.accY      == expected.accY);
  equal = equal && (vec2.accZ      == expected.accZ);
  equal = equal && (vec2.gyroRoll  == expected.gyroRoll);
  equal = equal && (vec2.gyroPitch == expected.gyroPitch);
  equal = equal && (vec2.gyroYaw   == expected.gyroYaw);
  equal = equal && (vec2.gyroTemp  == expected.gyroTemp);
  equal = equal && (vec2.magX      == expected.magX);
  equal = equal && (vec2.magY      == expected.magY);
  equal = equal && (vec2.magZ      == expected.magZ);
  equal = equal && (vec2.bmpPa     == expected.bmpPa);
  equal = equal && (vec2.bmpTemp   == expected.bmpTemp);
  if(equal){
    printf("PASS  TestImuSerialization\n");
  }
  else{
    printf("FAIL  TestImuSerialization\n");
    printf("Examine testVec and result:\n");
    printf("Expected values:");
    putVector(&expected);
    printf("Result values:");
    putVector(&vec2);
  }
}

void TestSerialize(void)
{
  char buf[256] = "172137790000-=][poiuytrewqasdfghjkl;'/.,mnbvcxz"; 
  char obuf[256];
  char result[256];
  char* expectedSerial = "MTcyMTM3NzkwMDAwLT1dW3BvaXV5dHJld3Fhc2RmZ2hqa2w7Jy8uLG1uYnZjeHo=";
  char* expectedDeserial = "172137790000-=][poiuytrewqasdfghjkl;'/.,mnbvcxz";

  printf("Serializing >%s<\n",buf);
  Serialize(buf,obuf,strlen(buf));
  printf("Serialized: >%s<\n",obuf);
  if(strncmp(expectedSerial,obuf,256) != 0){
   printf("ERROR expected serialization %s\n\t\tbut received %s\n",expectedSerial,obuf);
  }
  else{
    printf("SUCCESS serialization successful");
  }
  Deserialize(obuf,result,strlen(obuf));
  if(strncmp(expectedDeserial,result,256) != 0){
   printf("ERROR expected deserialization %s\n\t\tbut received %s\n",expectedDeserial,result);
  }
  else{
    printf("SUCCESS deserialization successful");
  }
  printf("Deserialized: >%s<\n",result);
}

void TestKoala(void)
{
  int ifile, ofile, n;
  char ibuf[4096];
  char obuf[8192];
  char deserialized[8192];

  ifile = open("./koala.jpg",O_RDONLY);
  ofile = open("./codec.jpg",O_WRONLY);
  while(n = read(ifile,ibuf,4095)){
    ibuf[4095] = '\0';
    printf("n=%d input: %s\n",n,ibuf);
    Serialize(ibuf,obuf,n);
    printf("len=%u serialized: %s\n",(int)strnlen(obuf,8192),obuf);
    Deserialize(obuf,deserialized,strnlen(obuf,8192));
    printf("len=%u writing: %s\n",n,deserialized);
    write(ofile,deserialized,n);
  }
  printf("TestKoala() completed\n");

  close(ifile);
  close(ofile);
}

