#include "ViewController.hpp"

ViewController::ViewController(const char* path, VectorRing& imuRing, int usResolution) : vecRing(imuRing)
{
  //redirect stdout
  //close(1);
  //open("./vcErrors.log",O_WRONLY);

  for(int i = 0; i < 8; i++){
    procArgs[i] = NULL;
  }

  strcpy(arg1,"/usr/bin/gnome-terminal");
  strcpy(arg2,"--title=CEREBOT TELEMETRY");
  strcpy(arg3,"--execute");  
  strcpy(arg4,"./view");

  procArgs[0] = arg1;
  procArgs[1] = arg2;
  procArgs[2] = arg3;
  procArgs[3] = arg4;

  usUpdateRate = usResolution;
  initialized = false;
  error = 0;
  sockLen = sizeof(sockaddr_un);
  memset(msg,0,1024);
  strncpy(sockPath,path,127);
  sockPath[127] = '\0';
}

ViewController::~ViewController()
{
  cout << "exiting vc..." << endl;
  close(sockfd);
}

void ViewController::Test()
{
  string buffer;

  if(!init()){
    cout << "ERROR could not init ViewController" << endl;
  }

  while(std::cin >> buffer){
    cout << "sending: " << buffer << endl;
    strncpy(msg,buffer.c_str(),64);
    send();
    //std::cin.flush();
  }
}

/*
  View controller spins for data, updating ui whenever it detects data arrival
*/
void ViewController::Run()
{
  if(init()){
    run();
  }

  putError();
}

//sleep, waiting for data to send to ui
void ViewController::run()
{
  while(!error){
    //note this loop is kind of an event subscription model. event occurs (data arrives, etc.), and action is triggered
    if(vecRing.HasNewData()){  //vecring has new data when comm object pushes a packet
      send();
      vecRing.ClearNewData();
    }
    usleep(usUpdateRate);
  }
}

void ViewController::putError()
{
  if(error){
    if(error == CEREBOT_KILL_SIGNAL){
      cout << "ViewController received kill signal from client. exiting." << endl;
    }
    else{
      cout << "ERROR ViewController object exited main loop with error " << error << ": " << strerror(error) << endl;
    }
  }
}

void ViewController::Die()
{
  //signal view proc to die, if still running
  if(sockfd >= 0){
    sendto(sockfd,DIE,strnlen(DIE,4),0,(SA*)&cliAddr,sockLen);
  }
  error = CEREBOT_KILL_SIGNAL;
}

void ViewController::send()
{
  cout << "in ViewController.Run() with sockfd " << sockfd << " and sockLen " << sockLen << endl;

  //sleep for new data
  int n = sendto(sockfd,msg,strlen(msg),0,(SA*)&cliAddr,sockLen);
  if(n < 0){
    error = errno;
    cout << "ERROR ViewController.Send().sendto() returned -1. Error str: " << strerror(errno) << endl;
  }
  else{
    cout << "sent " << n << " bytes: " << msg << endl;
  }
}

/*
  Opens unix domain socket for communicating with client.
*/
bool ViewController::initConnection()
{
  bool result = false;
  
  if((sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0)) >= 0){
    memset((void*)&cliAddr,0,sizeof(cliAddr));
    cliAddr.sun_family = AF_LOCAL;
    strncpy(cliAddr.sun_path,PATH_TO_CLIENT,100);
    //cout << cliAddr.sun_path << "  << the file name" << endl;
    //cout << "sizeof srvAddr.sunpath: " << strnlen(cliAddr.sun_path,100) << endl;
    unlink(cliAddr.sun_path);

    if(bind(sockfd, (SA*)&cliAddr, sizeof(cliAddr)) >= 0){
      memset((void*)&srvAddr,0,sizeof(srvAddr));
      srvAddr.sun_family = AF_LOCAL;
      strncpy(srvAddr.sun_path,PATH_TO_SERVER,64);
      result = true;
    }
    else{
      cout << "ERROR bind() failed in ViewController.Init()" << endl;
    }
  }
  else{
    cout << "ERROR socket() failed in ViewController.Init() errno =" << errno << "< sockfd=" << sockfd << endl;
    //putError();
  }

  if(!result){
    error = errno;
  }

  return result;
}

bool ViewController::forkClient()
{
  int childPid;
  cout << "fork GO!" << endl;

  if((childPid = vfork()) < 0){
    cout << "ERROR ViewController.forkClient fork() error" << endl;
    return false;
  }

  if(childPid == 0){
    //child execs a new terminal which will start the client proc
    execve(procArgs[0],procArgs,environ);
    cout << "ERROR ViewController.forkClient exec() error" << endl;
    exit(0);
  }

  return true;
}

//comms set up
bool ViewController::init()
{
  int result = initialized;

  //open comms
  if(!initialized && initConnection()){
    //create and detach the client terminal process
    if(forkClient()){
      //cout << "vc init successful" << endl;
      initialized = true;
      result = true;
    }
    else{
      error = errno;
      cout << "ERROR forkClient() failed in ViewController.Init()" << endl;
    }
  }
  else{
    error = errno;
    cout << "ERROR socket() failed in ViewController.Init() errno =" << errno << " intialized=" << initialized << endl;
    putError();
  }

  return result;
}

/*
int main(int argc, char* argv[])
{
  VectorRing ring(100);
  ViewController vc(PATH_TO_CLIENT,ring,10000);
  //vc.Run();
  vc.Test();

  return 0;
}
*/

