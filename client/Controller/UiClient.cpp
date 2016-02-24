#include "UiClient.hpp"

UiClient::UiClient(const char* path)
{
  sockfd = -1;
  sockLen = sizeof(sockaddr_un);
  memset((void*)sockPath,0,256);
  memset((void*)buf,0,1024);
  memset((void*)&cliAddr,0,sizeof(sockaddr_un));
  memset((void*)&srvAddr,0,sizeof(sockaddr_un));
  strncpy(sockPath,path,255);
}

UiClient::~UiClient()
{
  close(sockfd);
}

/*
  Client runs on the terminal displaying the current cerebot state.
*/
bool UiClient::init()
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
void UiClient::readData()
{
  cout << "client waiting for data" << endl;
  //client blocks and waits for data
  int n = recvfrom(sockfd, buf, 1024, 0, (SA*)&srvAddr, &sockLen);
  if(n < 0){
    cout << "ERROR recvfrom returned " << n << " in UiClient.ReadData()" << endl;
    buf[0] = '\0';
  }else if(strncmp(buf,DIE,3) == 0){
    die();
  }
  else{
    buf[n] = '\0';
    cout << "read " << n << " bytes: " << buf << endl;
    //cout << "from: " << cliAddr.sun_path << endl;
  }
}

//build the output string
void UiClient::updateUi()
{
  cout << "updating ui" << endl;
}

//print the output string
void UiClient::display()
{
  cout << "buf" << buf << endl;
}


void UiClient::die()
{
  //TODO: kill the terminal, not just the proc inside it
  cout << "uiClient dying" << endl;
  exit(0);
}

void UiClient::Run()
{
  if(!init()){
    cout << "ERROR UiClient could not initialize" << endl;
    return;
  }

  // the uiClient will spend most its time blocked in readData()
  while(1){
    readData();
    updateUi();
    display();
  }
}

//proc driver
int main(int argc, char* argv[])
{
  UiClient client(PATH_TO_CLIENT);

  client.Run();
  cout << "ERROR uiClient died prematurely" << endl;

  return 0;
}
