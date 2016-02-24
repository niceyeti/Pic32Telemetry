#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define SA struct sockaddr
#define SOCK_PATH "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/state"

using namespace std;

void dg_cli(FILE* fp, int sockfd, const SA* psrvAddr, socklen_t servlen)
{
  int n;
  char msg[1024], rcvmsg[1024+1];

  if(fp == NULL){
    cout << "ERROR fp was null in dg_cli" << endl;
    return;
  }

  cout << "in dg_cli() with sockfd " << sockfd << " and servlen " << servlen << endl;

  while(fgets(msg,1024,fp) != NULL){
    sendto(sockfd,msg,strlen(msg),0,psrvAddr,servlen);
    cout << "sent to server: " << msg << endl;
    n = recvfrom(sockfd, rcvmsg, 1024,0,NULL,NULL);
    rcvmsg[n] = '\0';
    cout << "rcv'ed back: " << rcvmsg << endl;
  }
}

/*
  Unix domain datagram client and server implementation.
  Direct from Stevens' Network Programming, Vol1, pg 419.
*/

int main(int argc, char* argv[])
{
  int sockfd;
  struct sockaddr_un srvAddr, cliAddr;

  sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0);

  memset((void*)&cliAddr,0,sizeof(cliAddr));
  cliAddr.sun_family = AF_LOCAL;
  strncpy(cliAddr.sun_path,tmpnam(NULL),64);
  cout << cliAddr.sun_path << "  << the file name" << endl;
  cout << "sizeof cliAddr.sunpath: " << strnlen(cliAddr.sun_path,64) << endl;

  bind(sockfd, (SA*)&cliAddr, sizeof(cliAddr));

  memset((void*)&srvAddr,0,sizeof(srvAddr));
  srvAddr.sun_family = AF_LOCAL;
  strncpy(srvAddr.sun_path,SOCK_PATH,64);
  cout << "here dgcli" << endl;
  dg_cli(stdin,sockfd,(SA*)&srvAddr,sizeof(srvAddr));

  return 0;
}



