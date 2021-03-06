#include "Comm.hpp"

Comm::Comm(const char* devAddr, const char* devicePath, VectorRing& ring) : vecRing(ring), bufSize(1024), packetSize(IMU_BASE64_SIZE)
{
  //redirect stdout
  //close(1);
  //open("./commErrors.log",O_WRONLY);

  if(devAddr){
    strncpy(devAddress,devAddr,63);
  }
  devAddress[63] = '\0';
  if(devPath){
    strncpy(devPath,devicePath,63);
  }
  devPath[63] = '\0';

  memset(&imuVec,0,sizeof(IMU));
  memset((void*)readBuf,0,1024);

  error = 0;
  head = 0;
  packetIndex = 0;
}

Comm::~Comm()
{
  close(commfd);
  cout << "Comm exiting..." << endl;
}

/*
  Innermost utility for reading a file descriptor. This is
  the appropriate layer at which to handle base64 encoding/decoding,
  before writing data to the actual data buffer. This makes encoding/decoding
  logic (if present) invisible to the logical reading of data.

          Sender (mcu)                                      Receiver (comm object)
  --> data --> no encoding ---> write bytes ---> receive bytes --> dont decode --> output data -->
           |                 ^                                 |                 ^
           v                 |                                 v                 |
           -- encode data --->                                 --> decode data -->

  TODO: this could be broken off into its own reader class

  Notes: caller requests reading strictly up to nbytes into buf. if the input is base64 encoded,
  this means we need to read more raw bytes into _buf (by a factor of about 4/3) in order to get nbytes in
  the final output 'buf'.

int Comm::_read(int fd, char obuf[], int nbytes)
{
  int n, bytesToRead;

  // map
  if(readMode == BASE64){
    //if base64-mode, increase number of bytes to read by a factor of 4/3  (so floor(nbytes * 4/3) to stay at or under nbytes)
    bytesToRead = (nbytes / 3) * 4;
  }
  else{
    bytesToRead = nbytes;
  }

  // do
  n = read(fd, _buf, bytesToRead);
  _buf[n] = '\0';

  // decode
  if(readMode == BASE64){
    n = Deserialize(_buf,obuf,n);
  }
  else{
    memcpy((void*)obuf,(void*)_buf,n);
  }

  return n;
}
*/

/*

*/
int Comm::readData()
{
  //beginning at head, read up to the number of remaining available bytes
  int n = read(commfd,&readBuf[head],bufSize-head);
  if(n >= 0){
    head += n;
    readBuf[head] = '\0';
    cout << "read " << n << " bytes from cerebot: " << readBuf << endl;
    //bytesReceived += n; //bytes since end of last packet read
  }
  else{
    error = errno;
    cout << "ERROR comm fd read() returned -1 on errno " << errno << ": " << strerror(errno) << endl;
  }

  return n;
}

void Comm::Die()
{
  error = CEREBOT_KILL_SIGNAL;
}

//indicates that more bytes have been read, after the packetIndex, than the size of a packet
//TODO: should this be sized based, or should we detect if the next '$$$$' has been received? Based it on a pktSize param, like ip?
bool Comm::hasFullPacket()
{
  //return packetReceived && ((head - packetIndex) >= sizeof(IMU));
  return (head - packetIndex) >= packetSize;
}

/*
  Directly passes an imu to vecRing; no decode.
*/
void Comm::pushPacket()
{
  //push data vector directly into vecRing from readBuf
  //vecRing.Push((IMU*)&readBuf[packetIndex]);
  vecRing.Push((IMU*)&imuVec);
}


void Comm::deserializePacket()
{
  // from encoding.c
  DeserializeImuVector((U8*)&readBuf[packetIndex],&imuVec);
}

//Rewinds from 'head' to the most recent '$' in the stream, indicating the most recent packet position.
//Using this function effectively discards any older, unread packets. The assumption is that the
//next read() call will fill in the rest of the packet, which will be extracted.
//TODO: This could fail on wraparound, for instance if the buf contains: '$ABCD'. This will deref array[-1], possibly causing stack smash, etc
int Comm::findMostRecentPacket()
{
  int i;

  //rewind to start of most recent packet
  for(i = head; i >= packetIndex; i--){
    if(readBuf[i] == '$' && readBuf[i-1] == '$'){
      break;
    }
  }

  return i;
}

void Comm::updateReadFlags()
{
  //near the end of the buffer, so copy remaining data to front
  if(packetIndex >= 1000 - sizeof(IMU)){
    //copy unprocessed data at end of readBuf to front
    //this is extremely unsafe, only works because readBuf is several times larger than packet size))
    memcpy((void*)readBuf,(void*)&readBuf[packetIndex],head-packetIndex);
    head = head - packetIndex;
  }

  //this does NOT necessarily find most recent FULL packet; expect the next call to read() to fill in the remainder
  packetIndex = findMostRecentPacket();
}

/*
  readData is a state machine that reads from a file descriptor until it detects a complete packet reception.
  The behavior is simply to read k bytes into buffer, check if those k-bytes include a complete packet
  delimiter. If yes, mark packetReceived (which does not necessarily mean the packet is complete). We then
  check if a full packet has arrvied (when n-bytes since start of packets is gte sizeof a packet). If it has,
  the packet is extracted and pushed to vector ring. In all cases, we then read() again, and continue through
  this state machine.
*/
void Comm::ReadData()
{
  if(readData() > 0){
    if(hasFullPacket()){
      deserializePacket();
      pushPacket();
      updateReadFlags();
    }
  }
}

/*
  Main method of Comm. This could be modeled in any number of ways,
  but currently Comm object sits in an infinite loop reading data,
  pushing it to vecRing, and checking for data to send back to the
  cerebot. The sending and receiving need to be on separate threads.
*/
void Comm::Run()
{
  cout << "in commRun()" << endl;
  if(initComms()){
    while(!error){
      ReadData();
    }
  }
  else{
    cout << "ERROR Comm object could not init comms" << endl;
  }
  cout << "leaving commRun()" << endl;
  putError();
}

void Comm::putError()
{
  if(error){
    if(error == CEREBOT_KILL_SIGNAL){
      cout << "Comm received kill signal from client. exiting." << endl;
    }
    else{
      cout << "ERROR comm object exited main loop with error " << error << ": " << strerror(error) << endl;
    }
  }
}

//TODO
int Comm::SendData(const U8* data, int nbytes)
{
  int n = write(commfd,(char*)data,nbytes);

  return n;
}

/*
  Alternative to the terminal modifications made in old initCerebotComms, which
  for some reason didn't work in the multithreaded model. This is much
  simpler, but opening the file this way strips newlines and other non-text
  data, it appears, when reading from the rfcomm file descriptor.
*/
bool Comm::initComms()
{
  bool success = true;

  commfd = open(devPath,O_RDONLY);
  if(commfd < 0){
    cout << "open() returned error " << errno << ": " << strerror(errno) << endl;
    cout << "(does " << devAddress << " exist? did you run sudo rfcomm bind 0 " << devAddress << "? did you run main prog as `sudo ./cerebot`?)" << endl;
    success = false;
    error = errno;
  }

  return success;
}

/*
int Comm::getVec(IMU* vec, char readBuf[]) //require large string size
{
  char c;

  if(readBuf[0] == '$'){
    printf("buffer: %s\n",readBuf);
    sscanf(readBuf,"%c %d %d %d %d %d %d %d %d %d %d %d %d ", &c, (int*)&vec->accX, (int*)&vec->accY, (int*)&vec->accZ,(int*)&vec->gyroRoll, (int*)&vec->gyroPitch, (int*)&vec->gyroYaw, (int*)&vec->gyroTemp,(int*)&vec->magX, (int*)&vec->magY, (int*)&vec->magZ,(int*)&vec->bmpPa, (int*)&vec->bmpTemp);
  }
  else{
    printf("str not found: >%s<\n",readBuf);
  }

  return 1;
}
*/

/*starting at beginning, look forward for the start of a packet
//This function also returns the start position of the new packet in pktStart.
void Comm::checkForPacket()
{
  for(int i = packetIndex; i < head && i < (bufSize-4); i++){
    if(readBuf[i] == '$' && readBuf[i+3] == '$'){
      packetIndex = i+4;
      packetReceived = true;
    }
  }
}
*/

//take a uart string and puts back into readBuf
//it has been very time consuming to get more compressed comms to work, so use this. its just easier for now.
//maps discrete device strings, instead of the mega string containing all 
/*
int getVec(IMU* v, char readBuf[])
{
  int c;  //dummy
  int success = 1;

  switch(readBuf[0]){
    case 'A':
        sscanf(readBuf,"%x %x %x %x", &c, (int*)&vec->accX, (int*)&vec->accY, (int*)&vec->accZ);
      break;
    case 'M':
        sscanf(readBuf,"%x %x %x %x", &c, (int*)&vec->magX, (int*)&vec->magY, (int*)&vec->magZ);
        //vec->T = -1; vec->B = -1;
      break;
    case 'G':
        sscanf(readBuf,"%x %x %x %x %x", &c, (int*)&vec->gyroRoll, (int*)&vec->gyroPitch, (int*)&vec->gyroYaw, (int*)&vec->gyroTemp);
      break;
    case 'B':
        sscanf(readBuf,"%x %x %x",&c, (int*)&vec->bmpPa, (int*)&vec->bmpTemp);
        //vec->T = -1; vec->B = -1;
      break;
    default:
        printf("slave str found: >%s<\n", readBuf);
        success = 0;
      break;
  }

  return success;
}
*/


