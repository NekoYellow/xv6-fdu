// Simple test: print the uid of this process.
// Since this is executed through fork()&exec() in shell,
// the uid printed is also the shell's.

#include "kernel/types.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid, uid;

  pid = getpid();
  uid = getuid(pid);
  
  if (fork() == 0) {
    if (getuid(getpid()) != uid) {
      printf("Error: child process did not inherit uid\n");
      exit(1);
    }
    exit(0);
  } else {
    wait(0);
  }

  uid ^= 1;
  if (setuid(pid, uid, PWD) < 0) {
    printf("Error: setuid failed\n");
    exit(1);
  }
  if (uid != getuid(pid)) {
    printf("Error: setuid did not work\n");
    exit(1);
  }

  if (fork() == 0) {
    if (getuid(getpid()) != uid) {
      printf("Error: child process did not inherit uid\n");
      exit(1);
    }
    exit(0);
  }

  printf("Everything ok if this is the only output\n");

  exit(0);
}
