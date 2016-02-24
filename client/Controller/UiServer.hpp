#include "UiController.hpp"
#include <stdlib.h>

/*
  Runs on a separate thread from the main controller, but within its proc space.
*/
class UiServer{
  public:
    UiServer(const char* path);
    UiServer() = delete;
    ~UiServer();
    void Send();
    void Test();
    void SetBuffer(const char* buf);

  private:
    //utils
    void die();
    bool initConnection();
    bool forkClient();
    bool init();

    char* procArgs[8];
    char arg1[64], arg2[64], arg3[64], arg4[64], arg5[64];
    int sockfd;
    socklen_t sockLen;
    /* 
      This is mostly useless now. The server forks a terminal process, which executes 
      a gnome-terminal, which then executes the uiClient process. So the child pid
      will be that of the terminal, not the uiClient process. However you could
      designate the client to send the server its pid on startup, so they could be linked
      across the OS in other ways besides the socket. But its also cleaner for them
      to not know anything about eachother.
    */
    int childPid;
    char sockPath[256];
    char msg[1024];
    struct sockaddr_un srvAddr, cliAddr;  //only one of these is really needed, since comms will be unidirectional
};

