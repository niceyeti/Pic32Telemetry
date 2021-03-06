#ifndef VIEW_HPP
#define VIEW_HPP


/*
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
//#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <errno.h>
*/

#include "../my_types.h"
#include "SysCommon.hpp"
#include "ViewCommon.hpp"

// was UiClient
class View
{
  public:
    View(const char* path, float updateRate);
    View() = delete; // c++ 11 allows expressly forbidding default constructor usage
    ~View();
    void Run();

  private:
    //utils
    bool init();
    void readData();
    void updateUi();
    void display();
    void die();
    //void putImu(const IMU& vec);

    IMU vec;
    int ms_resolution;
    int sockfd;
    socklen_t sockLen;
    char sockPath[256];
    char buf[1024];
    char uiString[1024];
    //string uiString;
    struct sockaddr_un srvAddr, cliAddr;
};

#endif
