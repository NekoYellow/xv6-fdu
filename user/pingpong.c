#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int ppid, cpid;
  char buf[2], *info = "a";
  int p[2];
  pipe(p);

  ppid = getpid();

  if ((cpid = fork()) == 0) { // child
    cpid = getpid();
    if (read(p[0], buf, 1) != 1) {
      fprintf(2, "Failed to read in child\n");
      exit(1);
    }
    close(p[0]);
    printf("%d: received ping from pid %d\n", cpid, ppid);
    if (write(p[1], buf, 1) != 1) {
      fprintf(2, "Failed to write in child\n");
      exit(1);
    }
    close(p[1]);
    exit(0);
  }
  // parent
  if (write(p[1], info, 1) != 1) {
    fprintf(2, "Failed to write in parent\n");
    exit(1);
  }
  close(p[1]);
  wait(0);
  if (read(p[0], buf, 1) != 1) {
    fprintf(2, "Failed to read in parent\n");
    exit(1);
  }
  printf("%d: received pong from pid %d\n", ppid, cpid);
  close(p[0]);
  exit(0);
}
