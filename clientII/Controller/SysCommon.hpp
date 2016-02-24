#ifndef SYS_COMMON_HPP  //this is just the union of a bunch of system stuff and libs frequently used
#define SYS_COMMON_HPP

  //sys specific
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/un.h>
  #include <sys/types.h>
  #include <fcntl.h>

  //cpp libs
  #include <vector>
  #include <iostream>
  #include <string>
  #include <thread>  //this requires linking -lpthread and also --std=c++11 in compile
  //c stdlibs
  #include <time.h>  //usleep
  #include <string.h>
  #include <errno.h>
  //#include <stdio.h>

  //multiple objects are designed to die on receipt of this
  #define CEREBOT_KILL_SIGNAL 12345678

  //usings
  using std::vector;
  using std::cout;
  using std::cin;
  using std::endl;
  using std::flush;
  using std::string;
  using std::thread;

#endif
