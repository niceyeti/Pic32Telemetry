#include <stdio.h>

int main(int argc, char** args)
{
  int i;
  char *s1, *s2 = "uhohs uhohs uhohs uhohs";
  char buf[4096];


  //printf("%8d hallo! %s\n",argc,args[1]);
  //main(++argc,args);

  printf("%s\n",args[1]);
  printf(">>%s\n",args[2]);

  s1 = args[1];
  for(i = 0; i < 4000; i++,s1++){
    buf[i] = *s1;
    printf("%c",*s1);
  }
  buf[i] = 0;

  printf(">>%s\n",buf);
  
  /*
  s1 = args[1];
  for(i = 0; i < 200; i++,s1--){
    printf("%c\n",*s1);
  }
  */


  return 0;
}


