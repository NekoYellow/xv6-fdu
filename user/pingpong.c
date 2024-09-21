#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int ppid, cpid;
  char buf[2], *info = "a";
  int p2c[2], c2p[2];
  pipe(p2c); pipe(c2p);

  ppid = getpid();

  if ((cpid = fork()) == 0) { // child
    cpid = getpid();
    close(p2c[1]);
    if (read(p2c[0], buf, 1) != 1) {
      fprintf(2, "Failed to read in child\n");
      exit(1);
    }
    printf("%d: received ping from pid %d\n", cpid, ppid);
    close(c2p[0]);
    if (write(c2p[1], buf, 1) != 1) {
      fprintf(2, "Failed to write in child\n");
      exit(1);
    }
    close(c2p[1]);
    exit(0);
  }
  // parent
  close(p2c[0]);
  if (write(p2c[1], info, 1) != 1) {
    fprintf(2, "Failed to write in parent\n");
    exit(1);
  }
  close(p2c[1]);
  wait(0);
  close(c2p[1]);
  if (read(c2p[0], buf, 1) != 1) {
    fprintf(2, "Failed to read in parent\n");
    exit(1);
  }
  printf("%d: received pong from pid %d\n", ppid, cpid);
  exit(0);
}
