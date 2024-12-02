#include "kernel/types.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: su password\n");
    exit(1);
  }

  if(setuid(2, SU, argv[1]) < 0){
    fprintf(2, "su failed\n");
  } else{
    fprintf(2, "SU now\n");
  }

  exit(0);
}