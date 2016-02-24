#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#define SA struct sockaddr
#define SOCK_PATH "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/state"

char msg[1024];


using namespace std;

void UiClient::PutState()
{
  cout << "server echoing: " << msg << endl;
}

void UiClient::Run()
{
  int n;
  socklen_t len;

  while(1){
    len = clilen;
    n = recvfrom(sockfd, msg, 1024, 0, pcliAddr, &len);
    putState();
    sendto(sockfd,msg,n,0,pcliAddr,len);
  }
}

/*

*/
UiClient::Init()
{
  sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0);

  unlink(SOCK_PATH);
  memset((void*)&srvAddr,0,sizeof(srvAddr));
  srvAddr.sun_family = AF_LOCAL;
  strncpy(srvAddr.sun_path,SOCK_PATH,64);

  bind(sockfd, (SA*)&srvAddr, sizeof(srvAddr));

  dg_echo(sockfd,(SA*)&cliAddr,sizeof(cliAddr));

  return 0;
}



