#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <cstring>
#include "../../my_types.h"


using namespace std;




/*
  Continues calling read() until n bytes have been received.
  Note how raw and dangerous this function is, no packet alignment,
  etc.
  Returns 0 on success (all bytes read), or -1, indicating error.
*/
int Readn(int fd, char buf[], int nBytes)
{
  int n, sum;

  sum = n = 0;
  while(n >= 0 && sum < nBytes){
    n = read(fd,&buf[sum],nBytes-sum);
    if(n > 0){
      sum += n;
    }
  }

  return n;
}


/*
  Just reads an prints input received over bluetooth, for testing.
*/
int main(void)
{
  int fd, n;
  string str;
  char buf[1024];

  fd = open("/dev/rfcomm0",O_RDONLY);
  if(fd < 0){
    cout << "open() returned error " << errno << ": " << strerror(errno) << endl;
  }

  while(1){
    n = Readn(fd,buf,IMU_BASE64_SIZE);
    if(n >= 0){
      buf[IMU_BASE64_SIZE] = '\0';
      cout << "buf:  " << buf << endl;
    }
    else{
      cout << "readn() returned error " << errno << ": " << strerror(errno) << endl;
      break;
    }
  }

  return 0;
}





