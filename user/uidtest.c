// Simple test: print the uid of this process.
// Since this is executed through fork()&exec() in shell,
// the uid printed is also the shell's.

#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid, uid;

  pid = getpid();
  uid = getuid(pid);
  printf("#%d: %d\n", pid, uid);

  exit(0);
}
