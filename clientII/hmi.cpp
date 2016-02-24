/*
  Dumb program just sits around until another proc signals it. It will then
  read and display the state of the cerebot sensors from a file.

  Calling proc signals this one using this proc's pid and kill().


*/
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "../my_types.h"

#define NEW_SENSOR_DATA


using namespace std;

char buf[4096];
bool doRead;
struct imuVector imuVec;

/*
  Give the state file some standard format:
    acc:<val>
    gyr:<val>
    mag:
    bar:
    accSpi:

   Naw forget that. Print the binary struct to a file. This client will read it directly and put it into an IMU vector.
*/




/*
  On receipt of magical signal number, this proc will read information from a file
  and print it to the screen.
*/
static void action(int sig, siginfo_t *siginfo, void *context)
{
  doRead = true;
}

//Read updated information from the device-state file
void readState(int fd)
{
  int n;

  if(fd < 0){
    return;
  }

  if(n = read(fd,buf,511)){  //no iterative-reading: just read once in signal, no mas
    buf[n] = '\0';
    //memcpy((void*)imuVec,(void*)buf,sizeof(IMUVector));
    cout << buf << " abcd" << endl;
  }
}

void putState(int fd)
{
  //parse the data in buf and print it
  readState(fd);
  cout << "MY STATE" << endl;
}


int main(int argc, char* argv[])
{
  int fd;
  string fname;
  struct sigaction act;

  if(argc >=2){
    fname = argv[1];
  }
  else{
    fname = "./state";
  }

  if(fd = open(fname.c_str(),O_RDONLY) < 0){
    cout << "could not open file: " << fname << endl;
  }

  doRead = false;

  memset(&act, '\0', sizeof(act));

  /* Use the sa_sigaction field because the handles has two additional parameters */
  act.sa_sigaction = &action;

  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  act.sa_flags = SA_SIGINFO;

  if(sigaction(SIGCHLD, &act, NULL) < 0){
    perror ("sigaction");
    return 1;
  }

  //fix this to sleep until sigwakeup
  while(1){
    sleep(10);
    if(doRead){
      putState(fd);
      doRead = false;
    }
  }

  return 0;
}

