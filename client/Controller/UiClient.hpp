#include "UiController.hpp"

class UiClient{
  public:
    UiClient(const char* path);
    UiClient() = delete; // c++ 11 allows expressly forbidding default constructor usage
    ~UiClient();
    void Run();

  private:
    //utils
    bool init();
    void readData();
    void updateUi();
    void display();
    void die();

    int sockfd;
    socklen_t sockLen;
    char sockPath[256];
    char buf[1024];
    string uiString;
    struct sockaddr_un srvAddr, cliAddr;
};

