

#ifndef CEREBOT_CLIENT_H
#define CEREBOT_CLIENT_H


class UiClient{
  public:
    UiClient(const char* path);
    UiClient() = delete; // c++ 11 allows expressly forbidding default constructor usage
    ~UiClient();
    void Init();
    void ReadData();
    void Run();
    void UpdateUi();

  private:
    int sockfd;
    const char sockPath[256];
    char buf[1024];
    struct sockaddr_un servaddr, cliaddr;
};


class UiServer{
  public:
    UiServer(const char* path);
    UiServer() = delete;
    ~UiServer();
    void SendData();
    void Run();
    void UpdateUi(); 

  private:
    int sockfd;
    char sockPath[255];
    char buf[1024];
    struct sockaddr_un servaddr, cliaddr;
};

/*
  Controller is the main actor behind the comms and control
  to and from the cerebot.
  Controller has a ui-thread (a server) and a primary thread
  for sending commands and updating internal ai logic.
*/
class CerebotController{
  public:
    CerebotController(char* sockpath, char* btAddr) : CerebotClient(), UiServer(), UiClient();
    CerebotController() = delete;
    CerebotController(char* sockpath, char* btaddr);
    ~CerebotController();
    void GetState();
    void SendCommand();
    void Init(); // spins off ui thread

  private:
    CerebotClient cerebotClient;
    uiClient UiClient;
    uiServer UiServer;
};

class CerebotClient{
  public:
    CerebotClient() = delete;
    CerebotClient(char* btAddress);
    char* GetBtAddr();
    void ReadCerebotData();
    void SendCerebotData();
    void SendUiData();

  private:
    char btaddr[64]; //an address of the form XX:XX:XX:XX:XX:XX. Use 'hcitool scan' to find yours.
    int sockfd;
    struct termios oldtio, newtio;
};

#endif

