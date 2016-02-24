#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <cstring>

using namespace std;

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
    n = read(fd,buf,1023);
    if(n > 0){
      buf[n] = '\0';
      cout << "buf[" << n << "]: " << buf << endl;
    }
    else{
      cout << "read() returned error " << errno << ": " << strerror(errno) << endl;
      break;
    }
  }

  return 0;
}





