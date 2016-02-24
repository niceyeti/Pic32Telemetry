#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

//#define SA struct sockaddr
//#define SOCK_PATH "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/state"

void UiClient::PutState()
{
  cout << "server echoing: " << buf << endl;
}

void UiClient::Run()
{
  int n;
  socklen_t len;

  if(!Init()){
    cout << "ERROR UiServer could not initalize" << endl;
    return;
  }

  while(1){
    len = clilen;
    //server blocks and waits for data
    n = recvfrom(sockfd, buf, 1024, 0, pcliaddr, &len);
    //and simply outputs it when it arrives, then blocks again
    putState();
    sendto(sockfd,buf,n,0,(SA*)&cliaddr,len);
  }

}

//comms set up
bool UiClient::Init()
{
  int success = FALSE;

  if(sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0) > 0){ 
    unlink(sockPath);
    memset((void*)&servaddr,0,sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strncpy(servaddr.sun_path,sockPath,100);
    if(bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) >= 0){
      success = TRUE;
    }
    else{
      cout << "ERROR bind() failed in UiServer.Init()" << endl;
    }
    //dg_echo(sockfd,(SA*)&cliaddr,sizeof(cliaddr));
  }
  else{
    cout << "ERROR socket() failed in UiServer.Init()" << endl;
  }

  return success;
}



