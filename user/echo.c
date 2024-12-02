#include "kernel/types.h"
#include "kernel/stat.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char buf[MAXENVV];

  for(i = 1; i < argc; i++){
    if(argv[i][0] == '$'){
      memset(buf, 0, sizeof(buf));
      if(getenv(2, argv[i]+1, buf) < 0){
        fprintf(2, "getenv failed\n");
        exit(-1);
      }
      if(*buf != 0)
        write(1, buf, strlen(buf));
      else
        write(1, argv[i], strlen(argv[i]));
    } else{
      write(1, argv[i], strlen(argv[i]));
    }
    if(i + 1 < argc){
      write(1, " ", 1);
    } else {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
