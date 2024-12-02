#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    env();
    exit(0);
  }

  if(argc != 3){
    fprintf(2, "Usage: set name value\n");
    exit(-1);
  }

  if(setenv(2, argv[1], argv[2]) < 0){
    fprintf(2, "set failed\n");
    exit(-1);
  }

  exit(0);
}
