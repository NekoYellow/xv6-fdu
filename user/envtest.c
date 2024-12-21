// Simple test: print the uid of this process.
// Since this is executed through fork()&exec() in shell,
// the uid printed is also the shell's.

#include "kernel/types.h"
#include "Kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  char buf[MAXENVV], nbuf[MAXENVV], *key = "key", *val = "val";

  pid = getpid();
  if (setenv(pid, key, val) < 0) {
    printf("Error: setenv failed\n");
    exit(1);
  }
  if (getenv(pid, key, buf) < 0) {
    printf("Error: getenv failed\n");
    exit(1);
  }
  if (strcmp(buf, val)) {
    printf("Error: value expected is %s but found %s\n", val, buf);
  }

  if (fork() == 0) {
    if (getenv(pid, key, nbuf) < 0) {
      printf("Error: child process did not inherit env\n");
      exit(1);
    }
    if (strcmp(nbuf, val)) {
      printf("Error: child process have env inconsistent with parent\n");
      exit(1);
    }
    exit(0);
  } else {
    wait(0);
  }

  printf("Everything ok if this is the only output\n");

  exit(0);
}
