#ifndef CEREBOT_CLIENT_H
#define CEREBOT_CLIENT_H

/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <string>
#include <iostream>
#include "../my_types.h"
*/
//#include "../../encoding/encoding.h"
#include "SysCommon.hpp"
#include "./Comm.hpp"
#include "./ViewController.hpp"
#include "./VectorRing.hpp"
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

/*
  TODO's:
    *get rid of magic numbers, standardize bufsizes
    *add unit tests if design settles
    *VecRing should not be the main database; need a complete datamodel class with historical and highspeed interfaces
    *fix so running does not requite sudo priviliges, nor the prior call to rfcomm bind 0 [btaddr]
    *constructor is suffering from parameter bloat; eg, why does CerebotClient need to know about StreamMode data type? etc
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

class CerebotClient
{
  public:
    ~CerebotClient();
    CerebotClient() = delete;
    CerebotClient(const char* devAddress, const char* devPath, const char* srvPath, int viewResolution);
    void Run();
    void Test();

  private:
    //void updateView(data);
    void run();
//    command getUserCommand();
    //void readCerebotData();
    //void sendCerebotData();    
    bool init();
    Comm* comms;
    ViewController* vc;  //thread with a child ui proc
    //ai AiController;  // state logic, filters, etc; needs access to all data structs
    
    bool error;
    thread* vcThread;
    thread* commThread;
    //model DataModel;   //all the data structures
    VectorRing* vecRing;
};

#endif
