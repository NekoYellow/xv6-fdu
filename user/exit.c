// Exit su mode.

#include "kernel/types.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(setuid(2, CU, PWD) < 0){
    fprintf(2, "exit su failed\n");
  } else{
    fprintf(2, "CU now\n");
  }

  exit(0);
}