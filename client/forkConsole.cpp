//#include "common.c"
//#include "socket.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;



/*
  Startup prog for forking processes related to comms i/o with cerebot.
  For now, just forks a terminal from the current proc, gets its pid,
  and pipes telemetry data to it over a unix socket (just for fun).

  KISS: Just implements a simple fork-exec pattern.
*/
int main(int argc, char* argv[], char** envp)
{
  int pid, n, fdflag, termfd, pfd[2];
  char *args[8] = {"/usr/bin/gnome-terminal","--title=CEREBOT TELEMETRY", "--execute", "./hmi", NULL};
  char buf[1024], buf2[128];
  string ibuf, pidstr, childout;

  cout << "GO!" << endl;

  if(pipe(pfd) < 0){
    cout << "pipe error" << endl;
    return 1;
  }

  if((pid = vfork()) < 0){
    cout << "fork error" << endl;
    return 1;
  }

  if(pid == 0){
    //child closes all its file descriptors, input end of pipe, and dupes pipe to stdout
    close(0);
    //close(1);
    close(pfd[0]);
    dup2(1,pfd[1]);      // plug output of pipe into child's stdout
    fcntl(1,F_SETFD,0);  //set stdout to not close-on-exec
    fdflag = fcntl(1,F_GETFD,0);
    if(fdflag == FD_CLOEXEC){
      cout << "error, FD_CLOEXEC still set for stdout" << endl;
    }
    execve(args[0],args,envp);
    cout << "exec error" << endl;
  }
  else{
    cout << "parent waiting for input... pid = " << pid << endl;
    //parent keeps all its descriptors open, closes only output end of pipe
    close(pfd[1]);
    cout << "parent continuing" << endl; 
    while(cin >> ibuf){  //loop and write to new, detached terminal
      write(pfd[0],ibuf.c_str(),ibuf.size());
      cout << "written: " << ibuf << endl;
      pid = std::stoi(ibuf.c_str());

      //might need to wait until child proc is up and running
      cout << "calling kill(" << pid << ")" << endl;
      kill(pid,SIGCHLD);
    }
  }


/*
  if(pid == 0){
    fcntl(1,F_SETFD,0);  //set stdout to not close-on-exec
    fdflag = fcntl(1,F_GETFD,0);
    if(fdflag == FD_CLOEXEC){
      cout << "error, FD_CLOEXEC still set for stdout" << endl;
    }
    execve(args[0],args,envp);
    cout << "exec error" << endl;
  }
  else{
    childout = "/proc/";
    childout += std::to_string(pid);;
    childout += "/fd/1";
    cout << "child's stdout path: " << childout << endl;
    termfd = open(childout.c_str(),O_WRONLY);
    if(termfd < 0){
      cout << "crap, didn't work..." << endl;
    }
    cout << "parent waiting for input..." << endl;
    //parent keeps all its descriptors open, closes only output end of pipe
    //close(pfd[1]);
    cout << "parent continuing" << endl; 
    while(cin >> ibuf){  //loop and write to new, detached terminal
      n = write(termfd,ibuf.c_str(),ibuf.size());
      cout << n << " bytes written to child: " << ibuf << endl;
    }
  }
*/


/*  
  if(pid == 0){
    //child closes all its file descriptors, input end of pipe, and dupes pipe to stdout
    close(0);
    //close(1);
    close(pfd[0]);
    dup2(1,pfd[1]);      // plug output of pipe into child's stdout
    fcntl(1,F_SETFD,0);  //set stdout to not close-on-exec
    fdflag = fcntl(1,F_GETFD,0);
    if(fdflag == FD_CLOEXEC){
      cout << "error, FD_CLOEXEC still set for stdout" << endl;
    }
    execve(args[0],args,envp);
    cout << "exec error" << endl;
  }
  else{
    cout << "parent waiting for input..." << endl;
    //parent keeps all its descriptors open, closes only output end of pipe
    close(pfd[1]);
    cout << "parent continuing" << endl; 
    while(cin >> ibuf){  //loop and write to new, detached terminal
      write(pfd[0],ibuf.c_str(),ibuf.size());
      cout << "written: " << ibuf << endl;
    }
  }

*/

  return 0;
}



