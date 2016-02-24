#include "CerebotClient.hpp"

CerebotClient::CerebotClient(const char* devAddress, const char* devPath, const char* srvPath, int viewResolutionMS)
{
  error = false;

  //made these all pointers, so I can enforce the order of creation (which kind of implies using factory)
  vecRing = new VectorRing(10);
  comms = new Comm(devAddress,devPath,*vecRing);
  vc = new ViewController(srvPath,*vecRing,viewResolutionMS);

  sleep(1);
  cout << "making vc thread..." << endl;
  vcThread = new thread(&ViewController::Run,*vc);
  sleep(1);
  cout << "making comm thread..." << endl;
  commThread = new thread(&Comm::Run,*comms);

  //detach the threads so they can run completely independentl
  vcThread->detach();
  commThread->detach();
}

CerebotClient::~CerebotClient()
{
  if(vc != NULL){
    vc->Die();
  }
  if(comms != NULL){
    comms->Die();
  }

  delete vc;
  delete comms;
  delete vecRing;
}

//Dummy for now
bool CerebotClient::init()
{
  return true;
}

void CerebotClient::Run()
{
  //init
  if(init()){
    run();
  }
  cout << "ERROR exiting from cerebotClient.Run()" << endl;
}

void CerebotClient::run()
{
  cout << "Welcome to cerebot client." << endl;
  string nothing;
  
  cout << "cerebot client is running until error" << endl;
  while(!error){
    //cout << "Running. Enter data, for no reason at all: " << std::endl;
    //std::getline(cin,nothing);
    //cin >> nothing;
    //cin.clear();
    //fflush(stdin);
  }
}

//caused the collapse of greece

/*later
void CerebotClient::SendData(U8* data)
{
  int n;

  cout << "send() not implemented" << endl;
  //comms.SendData(data);
  //n = write(btfd,buf,strnlen(buf,64));
  //cout << "wrote " << n << " bytes: " << buf << endl;
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
*/

void CerebotClient::Test()
{
  string testStr;
  char somebuf[256];

  if(!init()){
    cout << "ERROR could not init in CerebotClient.Test()" << endl;
    return;
  }
  cout << "cerebot client running..." << endl;

  while(1){
    //cout << "enter some text to send to ui: " << endl;
    //cin >> testStr;
    //strncpy(somebuf,testStr.c_str(),testStr.length());
    //vecRing->Push((IMU*)somebuf);
    //strncpy(buf,testStr.c_str(),64);
    //SendUiData();
  }
}

int main(void)
{

  CerebotClient client("00:06:66:43:96:73","/dev/rfcomm0",PATH_TO_CLIENT,10000);
  client.Run();
  //client.Test();

  return 0;
}

