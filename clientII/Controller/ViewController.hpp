#ifndef VIEW_CONTROLLER_HPP
#define VIEW_CONTROLLER_HPP

#include "SysCommon.hpp"
#include "ViewCommon.hpp"
#include "VectorRing.hpp"
#include "../my_types.h"


// was UiServer
class ViewController{
  public:
    ViewController(const char* path, VectorRing& imuRing, int usResolution);
    ViewController() = delete;
    ~ViewController();
    void Test();
    void Run();
    void Die();

  private:
    //utils
    void putError();
    void run();
    void send();
    bool initConnection();
    bool forkClient();
    bool init();

    int error;
    int usUpdateRate;  //millisecond refresh rate
    VectorRing& vecRing;
    char* procArgs[8];
    char arg1[64], arg2[64], arg3[64], arg4[64], arg5[64];
    int sockfd;
    socklen_t sockLen;
    bool initialized;
    char sockPath[128];
    char msg[1024];
    struct sockaddr_un srvAddr, cliAddr;  //only one of these is really needed, since comms will be unidirectional
};

#endif

