#include "kernel/types.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int uid;
  char *su = "SU", *cu = "CU";

  uid = getuid(2);
  if(uid < 0){
    fprintf(2, "whoami failed\n");
    exit(1);
  }
  printf("%s\n", (uid == SU ? su : cu));

  exit(0);
}