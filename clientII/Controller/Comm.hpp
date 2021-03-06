#ifndef COMM_HPP
#define COMM_HPP

#include "SysCommon.hpp"
#include "ViewCommon.hpp"
#include "../../encoding/encoding.h"
#include "VectorRing.hpp"

/*
  Needs encoder, if used.
  Comm may itself need to become multithreaded, maintaining a read and a write
  thread for multiplexing communications.

  The Comm object also needs access to some data structure to write vector
  data to; in a synchronous manner, its easy to pass a vector parameter,
  but this throttles comms. If the comm object is to read asynchronously
  in a polling loop, then it needs access to a vector data structure object
  that implements locks/mutexes, since other objects will be accessing it...
  or just use a queue or a non-mutex ring buffer like in SW testing.

  Anyway the expected behavior for now is just to have the Comm process
  read forever. The cerebot client just calls Run(), and the comm process
  continually writes data to some hideous global data structure.
  This object will probably change a lot, but try to keep it generic.

  Additionally, this should remain a highlevel interface for now, from which
  other Comm objects (for different comm types) could inherit.

  TODO: analyze packet-loss in the ring buffer
  TODO: split comm object and reader object (reader's responsibility is read state
  machine, and encoding logic)

*/

class Comm
{
  public:
    Comm(const char* devAddr, const char* devicePath, VectorRing& ring);
    ~Comm();
    Comm() = delete;
    //not currently used, but this is nice as a public function, which outside threads can call, while the main
    //thread of this class/object just sits and reads
    int SendData(const U8* data, int nbytes);
    void Run(); //spawns and kick off comm thread; comm thread will call internal run() method
    void Die();  //kill comm thread and die
    //void ReConnect();
    int getVec(IMU* vec, char buf[]); //require large string size

  private:
    void ReadData();
    void putError();
    bool initComms();
    int readData();
    bool hasFullPacket();
    void pushPacket();
    void deserializePacket();
    int findMostRecentPacket();
    void updateReadFlags();

    char errorPath[32];
    IMU imuVec;
    VectorRing& vecRing;
    const int packetSize;
    //data structures for reading
    const int bufSize;
    int head;
    int packetIndex;
    char readBuf[1024];
    char dataBuf[1024];
    //general parts
    int commfd;
    bool error;
    char devAddress[64];
    char devPath[64];
};

#endif
