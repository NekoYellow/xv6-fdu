#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

// Get free memory amount and number of processes in use.
uint64 sys_sysinfo(void) {
  uint64 addr;
  struct proc *p = myproc();
  struct sysinfo info;
  if (argaddr(0, &addr) < 0)
    return -1;
  info.freemem = memamt();
  info.nproc = proccnt();
  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
      return -1;
  return 0;
}