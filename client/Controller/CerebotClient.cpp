#include "CerebotClient.hpp"

CerebotClient::CerebotClient(const char* btAddress, const char* srvPath) : uiServer(srvPath)
{
  memset((void*)buf,0,1024);
  memset((void*)btAddr,0,64);
  memset(&oldtio,0,sizeof(struct termios));
  memset(&newtio,0,sizeof(struct termios));

  imuSize = sizeof(IMU);
  strncpy(btAddr,btAddress,64);
  vecRing.resize(10);
  for(int i = 0; i < 10; i++){
    memset((void*)&vecRing[i],0,sizeof(IMU));
  }
}

CerebotClient::~CerebotClient()
{
  vecRing.clear();
}

char* CerebotClient::GetBtAddr()
{
  return btAddr;
}

void CerebotClient::ReadCerebotData()
{
  //use this if attempting compressed packet transmission
  int n;

  n = read(btfd,buf,1023);
  if(n >= 0){
    buf[n] = '\0';
    cout << "read " << n << " bytes from cerebot: " << buf << endl;
  }
  else{
    cout << "ERROR read returned " << n << " in CerebotClient.ReadCerebotData() on error: " << strerror(errno) << endl;
  }

  DeserializeImuVector((U8*)buf,&vecRing[0]);
  cout << "vec accX: " << vecRing[0].accX << endl;
  memset((void*)buf,0,64);
  //tcflush(btfd, TCIFLUSH);  //flushes all data not read
}

//later
void CerebotClient::SendCerebotData()
{
  int n;

  n = write(btfd,buf,strnlen(buf,64));
  cout << "wrote " << n << " bytes: " << buf << endl;
}

 //tell thread data is ready
void CerebotClient::SendUiData()
{
  uiServer.SetBuffer(buf);
  uiServer.Send();
}

int CerebotClient::getVec(IMU* vec, char buf[256]) //require large string size
{
  char c;

  if(buf[0] == '$'){
    printf("buffer: %s\n",buf);
    sscanf(buf,"%c %d %d %d %d %d %d %d %d %d %d %d %d ", &c, (int*)&vec->accX, (int*)&vec->accY, (int*)&vec->accZ,(int*)&vec->gyroRoll, (int*)&vec->gyroPitch, (int*)&vec->gyroYaw, (int*)&vec->gyroTemp,(int*)&vec->magX, (int*)&vec->magY, (int*)&vec->magZ,(int*)&vec->bmpPa, (int*)&vec->bmpTemp);
  }
  else{
    printf("str not found: >%s<\n",buf);
  }

  return 1;
}

/*
  Alternative to the terminal modifications made in initCerebotComms, which
  for some reason didn't work in the multithreaded model. This is much
  simpler, but opening the file this way strips newlines and other non-text
  data, it appears, when reading from the rfcomm file descriptor.
*/
bool CerebotClient::initCerebotComms2()
{
  bool success = true;

  btfd = open(BT_DEVICE,O_RDONLY);
  if(btfd < 0){
    cout << "open() returned error " << errno << ": " << strerror(errno) << endl;
    cout << "(does " << BT_DEVICE << " exist? did you run sudo rfcomm bind 0 [btaddr]? did you run main prog as `sudo ./cerebot`?)" << endl;
    success = false;
  }

  return success;
}

//initializes the terminal for reading/writing the cerebot
bool CerebotClient::initCerebotComms()
{
  cout << "initializing comms" << endl;
  /*
    Open modem device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  btfd = open(BT_DEVICE, O_RDWR | O_NOCTTY );  //research O_NDELAY and O_NONBLOCK as well for better asynchronicity
  if (btfd < 0){
    cout << "ERROR CerebotClient.initCerebotComms() failed" << endl;
    perror(BT_DEVICE);
    return false;
  }

  cout << "initialized comms2" << endl;

  tcgetattr(btfd,&oldtio); /* save current serial port settings */
  memset((void*)&newtio,0,sizeof(newtio)); /* clear struct for new port settings */

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
  tcflush(btfd, TCIFLUSH);
  tcsetattr(btfd,TCSANOW,&newtio);
  cout << "comms initialized" << endl;

  return true;
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
        sscanf(buf,"%x %x %x %x", &c, (int*)&vec->accX, (int*)&vec->accY, (int*)&vec->accZ);
      break;
    case 'M':
        sscanf(buf,"%x %x %x %x", &c, (int*)&vec->magX, (int*)&vec->magY, (int*)&vec->magZ);
        //vec->T = -1; vec->B = -1;
      break;
    case 'G':
        sscanf(buf,"%x %x %x %x %x", &c, (int*)&vec->gyroRoll, (int*)&vec->gyroPitch, (int*)&vec->gyroYaw, (int*)&vec->gyroTemp);
      break;
    case 'B':
        sscanf(buf,"%x %x %x",&c, (int*)&vec->bmpPa, (int*)&vec->bmpTemp);
        //vec->T = -1; vec->B = -1;
      break;
    default:
        printf("slave str found: >%s<\n", buf);
        success = 0;
      break;
  }

  return success;
}
*/

void CerebotClient::Test()
{
  string testStr;

  if(!initCerebotComms2()){
    cout << "ERROR could not init comms in CerebotClient.Test()" << endl;
    return;
  }
  cout << "cerebot client running..." << endl;

  while(1){
    ReadCerebotData();
    putImu(&vecRing[0]);

    //cout << "enter some text to send to ui:" << endl;
    //std::cin >> testStr;
    //strncpy(buf,testStr.c_str(),64);
    //SendUiData();
  }
}

void CerebotClient::putImu(struct imuVector* vec)
{
  printf("A %x %x %x\n", vec->accX, vec->accY, vec->accZ);
  printf("G %x %x %x %x\n", vec->gyroRoll, vec->gyroPitch, vec->gyroYaw, vec->gyroTemp);
  printf("M %x %x %x\n", vec->magX, vec->magY, vec->magZ);
  printf("B %xpa %dc\n", vec->bmpPa, vec->bmpTemp);

  //printf("A %d %d %d\n", vec->accX, vec->accY, vec->accZ);
  //printf("G %d %d %d %d\n", vec->gyroRoll, vec->gyroPitch, vec->gyroYaw, vec->gyroTemp);
  //printf("M %d %d %d\n", vec->magX, vec->magY, vec->magZ);
}

//print the state using a simple ascii gui: make this write direct to stdout/console, w/out scrolling
void CerebotClient::putState(struct imuVector* vec)
{
  int i, j, k;
  short int x, y, z;
  int left, center, right;
  char str[128];

  center = 50;
  str[center] = 'x';
  str[center+50] = '|';
  
  printf("Acc X: %d  GyroX: %d sizeof(int): %d  sizeof(short): %d\n", vec->accX, vec->gyroRoll,(int)sizeof(int), (int)sizeof(short int));

  for(i = 0; i < 127; i++){
    str[i] = ' ';
  }
  str[127] = '\0';

  //accelerometer vals range from about -330 to about +240 (not sure why misaligned)
  x = vec->accX / 10;
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
  x = vec->gyroRoll / 250;
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

  x = vec->accY / 8;
  if(x > 0){
    for(i = center; i < x + center; i++)
      str[i] = '-';
  }
  else{
    for(i = center; i > x + center; i--)
      str[i] = '-';
  }

  x = vec->accZ / 8;
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

int main(void)
{
  CerebotClient client("00:06:66:43:96:73",PATH_TO_CLIENT);
  client.Test();

  return 0;
}



