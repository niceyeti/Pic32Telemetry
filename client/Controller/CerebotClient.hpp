#ifndef CEREBOT_CLIENT_H
#define CEREBOT_CLIENT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <string>
#include <iostream>
#include <vector>
#include "../../my_types.h"
#include "./UiServer.hpp"
#include "../../encoding/encoding.h"

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
#define BT_DEVICE "/dev/rfcomm0"  // <--if using rfcomm bind etc, over bluetooth radio
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

using std::cout;
using std::endl;
using std::string;
using std::vector;



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
 Controller is the main actor behind the comms and control
  to and from the cerebot.
  Controller has a ui-thread (a server) and a primary thread
  for sending commands and updating internal ai logic.

class CerebotController{
  public:
    CerebotController(char* sockpath, char* btAddr) : CerebotClient(btAddr), UiServer(sockpath), UiClient(sockpath);
    CerebotController() = delete;
    CerebotController(char* sockpath, char* btaddr);
    ~CerebotController();
    void ReadState();
    void SendCommand();
    void Init(); // spins off ui process: a client and server pair
    void Run();
    
  private:
    Thread uiServerThread;
    CerebotClient cerebotClient;
   //UiClient uiClient;
    UiServer uiServer;
};
*/

class CerebotClient
{
  public:
    CerebotClient() = delete;
    CerebotClient(const char* btAddress, const char* srvPath);
    ~CerebotClient();
    char* GetBtAddr();
    void ReadCerebotData();
    void SendCerebotData();
    void SendUiData();  //tell thread data is ready
    void Test();

  private:
    void putState(struct imuVector* vec);
    void putImu(struct imuVector* vec);
    bool initCerebotComms();
    bool initCerebotComms2();
    int getVec(IMU* vec, char buf[256]);

    UiServer uiServer;
    int btfd;
    struct termios oldtio, newtio;
    vector<IMU> vecRing;
    int imuSize;
    char buf[1024];
    char btAddr[64]; //an address of the form XX:XX:XX:XX:XX:XX. Use 'hcitool scan' to find yours.
};

#endif
