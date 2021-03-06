#include "View.hpp"

View::View(const char* path, float updateRate)
{
  sockfd = -1;
  sockLen = sizeof(sockaddr_un);
  memset((void*)sockPath,0,256);
  memset((void*)buf,0,1024);
  memset((void*)uiString,0,1024);
  memset((void*)&cliAddr,0,sizeof(sockaddr_un));
  memset((void*)&srvAddr,0,sizeof(sockaddr_un));
  strncpy(sockPath,path,255);
  ms_resolution = updateRate;
}

View::~View()
{
  close(sockfd);
}

/*
  View runs on separate terminal, displaying the current cerebot state.
*/
bool View::init()
{
  bool result = false;

/*
  if(sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0) > 0){
    unlink(sockPath);
    memset((void*)&cliAddr,0,sizeof(cliAddr));
    cliAddr.sun_family = AF_LOCAL;
    strncpy(cliAddr.sun_path,sockPath,100);
    if(bind(sockfd, (SA*)&cliAddr, sizeof(cliAddr)) >= 0){
      result = true;
    }
    else{
      cout << "ERROR bind() failed in UiServer.Init()" << endl;
    }
    //dg_echo(sockfd,(SA*)&srvAddr,sizeof(srvAddr));
  }
  else{
    cout << "ERROR socket() failed in UiServer.Init()" << endl;
  }
*/

  if((sockfd = socket(AF_LOCAL,SOCK_DGRAM,0)) > 0){
    memset((void*)&cliAddr,0,sizeof(cliAddr));
    cliAddr.sun_family = AF_LOCAL;
    strncpy(cliAddr.sun_path,PATH_TO_CLIENT,100);
    cout << cliAddr.sun_path << "  << the file name" << endl;
    cout << "sizeof srvAddr.sunpath: " << strnlen(cliAddr.sun_path,100) << endl;
    unlink(cliAddr.sun_path);

    if(bind(sockfd,(SA*)&cliAddr,sizeof(cliAddr)) >= 0){
      memset((void*)&srvAddr,0,sizeof(srvAddr));
      srvAddr.sun_family = AF_LOCAL;
      strncpy(srvAddr.sun_path,PATH_TO_SERVER,64);
      result = true;
    }
    else{
      cout << "ERROR bind() failed in UiServer.Init()" << endl;
      cout << "errormsg: " << strerror(errno) << endl;
    }
  }
  else{
    cout << "ERROR socket() failed in UiServer.Init()" << endl;
  }

  return result;
}

//client will spend most its time blocked here, in recvfrom
void View::readData()
{
  cout << "client waiting for data" << endl;
  //client blocks and waits for data
  int n = recvfrom(sockfd, buf, 1024, 0, (SA*)&srvAddr, &sockLen);
  if(n < 0){
    cout << "ERROR recvfrom returned " << n << " in View.ReadData()" << endl;
    buf[0] = '\0';
  }else if(strncmp(buf,DIE,3) == 0){
    cout << "DIE received!!!" << endl;
    die();
  }
  else{
    buf[n] = '\0';
    cout << "read " << n << " bytes: " << buf << "\n!!! prolly still needs htons"<< endl;
    memcpy((void*)&vec,(void*)buf,sizeof(IMU));
    //cout << "from: " << cliAddr.sun_path << endl;
  }
}

//print the output string
void View::display()
{
  //system("cls");
  cout << uiString << endl;
}

void View::die()
{
  //TODO: kill the terminal, not just the proc inside it
  cout << "View dying in five seconds..." << endl;
  sleep(5);
  exit(0);
}

void View::Run()
{
  if(!init()){
    cout << "ERROR View could not initialize" << endl;
    return;
  }

  // the View will spend most its time blocked in readData()
  while(1){
    readData();
    updateUi();
    display();
    sleep(ms_resolution);
  }

  cout << "ERROR view returning from Run()" << endl;
}

//build the output string
void View::updateUi()
{
  int i, len;

  cout << "updating ui" << endl;
  sprintf(uiString,"A %x %x %x\nG %x %x %x %x\nM %x %x %x\nB %xpa %dc\n", \
    vec.accX, vec.accY, vec.accZ, \
    vec.gyroRoll, vec.gyroPitch, vec.gyroYaw, vec.gyroTemp, \
    vec.magX, vec.magY, vec.magZ, \
    vec.bmpPa, vec.bmpTemp);

  /*
  len = strnlen(uiString,1024);
  for(i = len; i < 1023; i++){
    uiString[i] = ' ';
  }
  */

  uiString[1023] = '\0';
  //printf("A %d %d %d\n", vec.accX, vec.accY, vec.accZ);
  //printf("G %d %d %d %d\n", vec.gyroRoll, vec.gyroPitch, vec.gyroYaw, vec.gyroTemp);
  //printf("M %d %d %d\n", vec.magX, vec.magY, vec.magZ);
}

bool isIntString(char* numString)
{
  int i;
  
  if(numString == NULL){
    return false;
  }
  
  for(i = 0; numString[i] != '\0'; i++){
    if(numString[i] < '0' || numString[i] > '9'){
      return false;
    }
  }

  return true;
}

/*
//print the state using a simple ascii gui: make this write direct to stdout/console, w/out scrolling
void View::putState(struct imuVector* vec)
{
  int i, j, k;
  short int x, y, z;
  int left, center, right;
  char str[128];

  center = 50;
  str[center] = 'x';
  str[center+50] = '|';
  
  printf("Acc X: %d  GyroX: %d sizeof(int): %d  sizeof(short): %d\n", vec.accX, vec.gyroRoll,(int)sizeof(int), (int)sizeof(short int));

  for(i = 0; i < 127; i++){
    str[i] = ' ';
  }
  str[127] = '\0';

  //accelerometer vals range from about -330 to about +240 (not sure why misaligned)
  x = vec.accX / 10;
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
  x = vec.gyroRoll / 250;
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

  x = vec.accY / 8;
  if(x > 0){
    for(i = center; i < x + center; i++)
      str[i] = '-';
  }
  else{
    for(i = center; i > x + center; i--)
      str[i] = '-';
  }

  x = vec.accZ / 8;
  if(x > 0){
    for(i = center; i < x + center; i++)
      str[i] = '-';
  }
  else{
    for(i = center; i > x + center; i--)
      str[i] = '-';
  }

  printf("%s\n",str);
  
}
*/


//proc driver; calling script must pass PATH_TO_CLIENT and ui-resolution update rate (in milliseconds)
int main(int argc, char* argv[])
{
  float ms_resolution = 0.001;
  
  if(argc = 3){
    if(isIntString(argv[2])){
      ms_resolution = (float)atoi(argv[2]) / 1000;
    }
  }
  
  View client(PATH_TO_CLIENT,ms_resolution);
  client.Run();

  cout << "ERROR View died prematurely, exiting in 5 seconds..." << endl;
  sleep(5);
  
  return 0;
}
