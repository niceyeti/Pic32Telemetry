#ifndef VIEW_CONTROLLER_HPP
#define VIEW_CONTROLLER_HPP
/*
  Common definitions and includes shared by UiClient and UiServer class.
*/
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

#define DIE "DIE"  //theres a probability distribution as to whether or not the imu will ever match this value...
#define PATH_TO_CLIENT "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/cli"
#define PATH_TO_SERVER "/home/jesse/MPLABXProjects/Rebuild_NoRTOS.X/client/srv"
#define SA struct sockaddr

using std::cout;
using std::endl;
using std::string;

#endif
