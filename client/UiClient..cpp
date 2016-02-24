#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//#define SA struct sockaddr
//#define SOCK_PATH "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/state"

//using namespace std;

//void dg_cli(FILE* fp, int sockfd, const SA* pservaddr, socklen_t servlen)
UiClient::Run()
{
  int n;

  if(fp == NULL){
    cout << "ERROR fp was null in dg_cli" << endl;
    return;
  }

  cout << "in dg_cli() with sockfd " << sockfd << " and servlen " << servlen << endl;

  while(fgets(msg,1024,fp) != NULL){
    sendto(sockfd,msg,strlen(msg),0,pservaddr,servlen);
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

bool UiClient::Init()
{
  bool success = FALSE;

  if(sockfd = socket(AF_LOCAL,SOCK_DGRAM, 0) > 0){
    memset((void*)&cliaddr,0,sizeof(cliaddr));
    cliaddr.sun_family = AF_LOCAL;
    strncpy(cliaddr.sun_path,tmpnam(NULL),100);
    cout << cliaddr.sun_path << "  << the file name" << endl;
    cout << "sizeof cliaddr.sunpath: " << strnlen(cliaddr.sun_path,64) << endl;

    if(bind(sockfd, (SA*)&cliaddr, sizeof(cliaddr)) >= 0){
      memset((void*)&servaddr,0,sizeof(servaddr));
      servaddr.sun_family = AF_LOCAL;
      strncpy(servaddr.sun_path,SOCK_PATH,64);
      success = TRUE;
    }
    else{
      cout << "ERROR bind() failed in UiServer.Init()" << endl;
    }
  }
  else{
    cout << "ERROR socket() failed in UiServer.Init()" << endl;
  }

    //dg_cli(stdin,sockfd,(SA*)&servaddr,sizeof(servaddr));

  return success;
}



