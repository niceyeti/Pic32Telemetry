#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include "../my_types.h"



/*

Connectivity issues:
  CANT CONNECT ???? CHECK BAUD RATES!!!

  CANT USE BLUETOOTH?  To create /dev/rfcomm0 use "sudo rfcomm bind 0 MAC_ADDRESS_OF_PMODBT2"  
    -sudo rfcomm bind 0 00:06:66:43:96:73
    -program must be run with sudo priviliges: sudo ./serial
    -to get the address of PMODBt2 use "hcitool scan" to scan for the device when it is powered on

  Starting bluetooth from the command line

  To stop :     sudo /etc/init.d/bluetooth stop
  To start :    sudo /etc/init.d/bluetooth start
  To restart :  sudo /etc/init.d/bluetooth restart
*/













/* baudrate settings are defined in <asm/termbits.h>, which is
   included by <termios.h> */
#define BAUDRATE B115200
//#define BAUDRATE B9600
/* change this definition for the correct port */
//#define MODEMDEVICE "/dev/rfcomm0"   // use for bluetooth connection under linux:
//  *if /dev/rfcomm0 does not exist, use "sudo rfcomm bind 0 ADDR_OF_PMODBT2" and prior use "hcitool scan" to find ADDR_OF_PMODBT2
//  *bt2 addr should be 00:06:66:43:96:73 so use >>>   sudo rfcomm bind 0 00:06:66:43:96:73
//#define MODEMDEVICE "/dev/ttyUSB1"    //one or the other... usb1 or usb0, wherever the cable is hooked up
//#define MODEMDEVICE "/dev/ttyUSB1"  // <-- direct serial over USB
#define MODEMDEVICE "/dev/rfcomm0"  // <--if using rfcomm bind etc, over bluetooth radio
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

// vector
/*
typedef struct deviceVector{
  short int X;
  short int Y;
  short int Z;  
  short int T;  //temp in celsius
  short int P;  //pressure in Pa
}VEC;
*/

/*
void putState(int i, int res, char buf[])
{
  int j, k, l;

  

  printf("%d %d %s",i,res,buf);

  
  
  







}
*/

void clearStr(char s[], int l)
{
  int i;

  for(i = 0; i < l; i++){
    s[i] = '\0';
  }
}



int getVec(IMU* v, char buf[256]) //require large string size
{
  char c;  //dummy

  if(buf[0] == '$'){
    printf("buffer: %s\n",buf);
    sscanf(buf,"%c %d %d %d %d %d %d %d %d %d %d %d %d ", &c, (int*)&v->accX, (int*)&v->accY, (int*)&v->accZ,(int*)&v->gyroRoll, (int*)&v->gyroPitch, (int*)&v->gyroYaw, (int*)&v->gyroTemp,(int*)&v->magX, (int*)&v->magY, (int*)&v->magZ,(int*)&v->bmpPa, (int*)&v->bmpTemp );
  }
  else{
    printf("str not found: >%s<\n",buf);
  }

  return 1;
}

//take a uart string and puts back into buf
//it has been very time consuming to get more compressed comms to work, so use this. its just easier for now.
//maps discrete device strings, instead of the mega string containing all 
/*
int getVec(IMU* v, char buf[])
{
  int c;  //dummy
  int success = 1;

  switch(buf[0]){
    case 'A':
        sscanf(buf,"%x %x %x %x", &c, (int*)&v->accX, (int*)&v->accY, (int*)&v->accZ);
      break;
    case 'M':
        sscanf(buf,"%x %x %x %x", &c, (int*)&v->magX, (int*)&v->magY, (int*)&v->magZ);
        //v->T = -1; v->B = -1;
      break;
    case 'G':
        sscanf(buf,"%x %x %x %x %x", &c, (int*)&v->gyroRoll, (int*)&v->gyroPitch, (int*)&v->gyroYaw, (int*)&v->gyroTemp);
      break;
    case 'B':
        sscanf(buf,"%x %x %x",&c, (int*)&v->bmpPa, (int*)&v->bmpTemp);
        //v->T = -1; v->B = -1;
      break;
    default:
        printf("slave str found: >%s<\n", buf);
        success = 0;
      break;
  }

  return success;
}
*/



void putImu(struct imuVector* v)
{
/*
  printf("A %x %x %x\n", v->accX, v->accY, v->accZ);
  printf("G %x %x %x %x\n", v->gyroRoll, v->gyroPitch, v->gyroYaw, v->gyroTemp);
  printf("M %x %x %x\n", v->magX, v->magY, v->magZ);
  //printf("B %xpa %dc\n", v->bmpPa, v->bmpTemp);
*/
  printf("A %d %d %d\n", v->accX, v->accY, v->accZ);
  printf("G %d %d %d %d\n", v->gyroRoll, v->gyroPitch, v->gyroYaw, v->gyroTemp);
  //printf("M %d %d %d\n", v->magX, v->magY, v->magZ);
}

//print the state using a simple ascii gui: make this write direct to stdout/console, w/out scrolling
void putState(struct imuVector* v)
{
  int i, j , k;
  short int x, y, z;
  int left, center, right;
  char str[128];

  center = 50;
  str[center] = 'x';
  str[center+50] = '|';
  
  printf("Acc X: %d  GyroX: %d sizeof(int): %d  sizeof(short): %d\n", v->accX, v->gyroRoll,(int)sizeof(int), (int)sizeof(short int));

  for(i = 0; i < 127; i++){
    str[i] = ' ';
  }
  str[127] = '\0';

  //accelerometer vals range from about -330 to about +240 (not sure why misaligned)
  x = v->accX / 10;
  if((x <= 50) && (x >= -50)){

    if(x > 0){
      for(i = center + 1; i < (x + center); i++){
        str[i] = '-';
      }
    }
    else{
      for(i = center - 1; i > (x + center); i--){
        str[i] = '-';
      }
    }
    printf("                                                Acc X\n   |%s\n",str);

    for(i = 0; i < 127; i++){
      str[i] = ' ';
    }
    str[127] = '\0';
  }
  else{
    printf("                                                Acc X\n   | [OUT OF RANGE]\n");
  }


    //accelerometer vals range from about -330 to about +240 (not sure why misaligned)
  x = v->gyroRoll / 250;
  if((x <= 50) && (x >= -50)){
    if(x > 0){
      for(i = center + 1; i < (x + center); i++){
        str[i] = '-';
      }
    }
    else{
      for(i = center - 1; i > (x + center); i--){
        str[i] = '-';
      }
    }

    printf("                                                Gyro X\n|%s\n",str);
  }
  else{
    printf("                                                Gyro X\n| [OUT OF RANGE]\n");
  }


/*

  x = v->accY / 8;
  if(x > 0){
    for(i = center; i < x + center; i++)
      str[i] = '-';
  }
  else{
    for(i = center; i > x + center; i--)
      str[i] = '-';
  }

  x = v->accZ / 8;
  if(x > 0){
    for(i = center; i < x + center; i++)
      str[i] = '-';
  }
  else{
    for(i = center; i > x + center; i--)
      str[i] = '-';
  }

  printf("%s\n",str);
  */
}


void my_memcpy(unsigned char* dst, unsigned char* src, int nbytes)
{
  int i;

  for(i = 0; i < nbytes; i++, dst++, src++){
    *dst = *src;
  }
}


volatile int STOP=FALSE; 

int main(void)
{
  int fd,i,c, res;
  struct termios oldtio,newtio;
  char buf[1024];
  //VEC dev;
  IMU imuvec;
  int imuSize = sizeof(IMU);

  memset((void*)&imuvec,0,imuSize);

  printf("SIZEOF INT %x SHORTINT %x\n", (int)sizeof(int), (int)sizeof(short int));
  getchar();


/*
  Open modem device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
 fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );  //research O_NDELAY and O_NONBLOCK as well for better asynchronicity
 if (fd <0) {perror(MODEMDEVICE); return -1; }

 tcgetattr(fd,&oldtio); /* save current serial port settings */
 bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

/* 
  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
  CRTSCTS : output hardware flow control (only used if the cable has
            all necessary lines. See sect. 7 of Serial-HOWTO)
  CS8     : 8n1 (8bit,no parity,1 stopbit)
  CLOCAL  : local connection, no modem contol
  CREAD   : enable receiving characters
*/
 newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
 
/*
  IGNPAR  : ignore bytes with parity errors
  ICRNL   : map CR to NL (otherwise a CR input on the other computer
            will not terminate input)
  otherwise make device raw (no other input processing)
*/
 newtio.c_iflag = IGNPAR | ICRNL;
 
/*
 Raw output.
*/
 newtio.c_oflag = 0;
 
/*
  ICANON  : enable canonical input
  disable all echo functionality, and don't send signals to calling program
*/
 newtio.c_lflag = ICANON;
 
/* 
  initialize all control characters 
  default values can be found in /usr/include/termios.h, and are given
  in the comments, but we don't need them here
*/
 newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
 newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
 newtio.c_cc[VERASE]   = 0;     /* del */
 newtio.c_cc[VKILL]    = 0;     /* @ */
 newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
 newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
 newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
 newtio.c_cc[VSWTC]    = 0;     /* '\0' */
 newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
 newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
 newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
 newtio.c_cc[VEOL]     = 0;     /* '\0' */
 newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
 newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
 newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
 newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
 newtio.c_cc[VEOL2]    = 0;     /* '\0' */

/* 
  now clean the modem line and activate the settings for the port
*/
 tcflush(fd, TCIFLUSH);
 tcsetattr(fd,TCSANOW,&newtio);

/*
  terminal settings done, now handle input
  In this example, inputting a 'z' at the beginning of a line will 
  exit the program.
*/
 for (i=0; STOP==FALSE; i++){     /* loop until we have a terminating condition */
 /* read blocks program execution until a line terminating character is 
    input, even if more than 255 chars are input. If the number
    of characters read is smaller than the number of chars available,
    subsequent reads will return the remaining chars. res will be set
    to the actual number of characters actually read */

    //use this for regular ascii string transmission, and parse the strings with sscanf()
    //res = read(fd,buf,255);

    //use this if attempting compressed packet transmission
     res = read(fd,buf,255);
     buf[255] = 0;
     tcflush(fd, TCIFLUSH);  //flush all data not read
    
    //memcpy((void*)&imuvec, buf, imuSize);
    //printf("reading imusize=%d\n",imuSize);
    //res = read(fd,(char*)&imuvec,imuSize);

    if(res == 0){
      printf("connection lost\n");
      break;
    }

    //printf("result: %s\n",buf);

    //string parsing version, ie, device sends ascii strings like "A 102 104 456" for accelerometer
    if(buf[0]=='$'){
      if(getVec(&imuvec,buf) )
        putImu(&imuvec);
        //putState(&imuvec);
    }
    else{
      printf("%5d %s",i,buf);
    }
/*

    if(buf[0] == 'A'){
      printf("%4d %3dbytes RXed: %s\n",i,res,buf);
    }

    if(buf[0] == '$'){
      //printf("here \n",buf);
      //strncpy((char*)&imuvec, &buf[1], imuSize);
      my_memcpy((unsigned char*)&imuvec, (unsigned char*)&buf[1], imuSize);
      printf("%4d %3dbytes Acc XYZ %x %x %x\n",i, res, imuvec.accX, imuvec.accY, imuvec.accZ);
    }
    else{
      printf("got: >%s<\n",buf);
    }
*/
    //buf[res]=0;             /* set end of string, so we can printf */
    
    //input must be preformatted as, for example "A [x val] [y val] [z val]
    //getVec(&dev,buf);
    //putState(i,res,);
    
    clearStr(buf,255);


    //if (buf[0]=='z') STOP=TRUE;
    if (i > 60000) { i = 0; }
 }
 /* restore the old port settings */
 tcsetattr(fd,TCSANOW,&oldtio);


  return 0;
}
