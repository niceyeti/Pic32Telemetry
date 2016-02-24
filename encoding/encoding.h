#ifndef MY_ENCODING_H
#define MY_ENCODING_H

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef MY_TYPES_H
#include "../my_types.h"
#endif

#define DBG 1
#define INVALID_CHAR 64 //some value not in the base64 char-value set of numbers 0-63
#define IS_LITTLE_ENDIAN 1
#define PACKET_DELIM "$$$$"
#define PACKET_SIZE (sizeof(IMU) + 4)

enum StreamMode { RAW = 0, BASE64 };

void InitDecodeTable();
U16 swapBytes(U16 shrt);
short int my_htons(short int shrt);
short int my_ntohs(short int shrt);
void ntohsImuVector(IMU* vec);
void htonsImuVector(IMU* vec);
int Deserialize(U8* ibuf, U8 obuf[], int nBytes);
int Serialize(U8* ibuf, U8 obuf[], int nBytes);
void SerializeImuVector(IMU* vec,U8 obuf[]);
void DeserializeImuVector(U8* ibuf,IMU* vec);

#endif
