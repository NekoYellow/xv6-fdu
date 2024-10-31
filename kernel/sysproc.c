#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

// 全局变量，存储 trace 掩码
int trace_mask = 0;

uint64 sys_exit(void) {
  int n;
  if (argint(0, &n) < 0) return -1;
  exit(n);
  return 0;  // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  if (argaddr(0, &p) < 0) return -1;
  return wait(p);
}

uint64 sys_sbrk(void) {
  int addr;
  int n;

  if (argint(0, &n) < 0) return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0) return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  if (argint(0, &n) < 0) return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (myproc()->killed) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  if (argint(0, &pid) < 0) return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_rename(void) {
  char name[16];
  int len = argstr(0, name, MAXPATH);
  if (len < 0) {
    return -1;
  }
  struct proc *p = myproc();
  memmove(p->name, name, len);
  p->name[len] = '\0';
  return 0;
}

uint64 sys_trace(void) {
  int mask;
  if(argint(0, &mask) < 0)
    return -1;
  trace_mask = mask;  // 设置全局的 trace 掩码
  return 0;
}

// 系统调用函数
uint64 sys_sysinfo(void) {
  struct sysinfo info;
  struct proc *p = myproc();
  uint64 addr;  // 用户空间地址

  if (argaddr(0, &addr) < 0)  // 获取用户传入的参数
    return -1;

  // 获取空闲内存量
  info.freemem = kfreemem();        // 需要自己实现kfreemem获取内存
  info.nproc = get_unused_procs();  // 获取UNUSED状态的进程数，需要实现该函数

  // 将结构体信息复制到用户空间
  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0) return -1;

  return 0;
}

// 统计进程调度信息的系统调用
uint64 sys_wait_sched(void) {
  uint64 p0, p1, p2, ret;
  struct proc *p = myproc();
  int runnable_time, running_time, sleep_time;

  if (argaddr(0, &p0) < 0) return -1;
  if (argaddr(1, &p1) < 0) return -1;
  if (argaddr(2, &p2) < 0) return -1;

  ret = wait_sched(&runnable_time, &running_time, &sleep_time);
  if (copyout(p->pagetable, p0, (char *)&runnable_time, sizeof(runnable_time)) < 0) return -1;
  if (copyout(p->pagetable, p1, (char *)&running_time, sizeof(running_time)) < 0) return -1;
  if (copyout(p->pagetable, p2, (char *)&sleep_time, sizeof(sleep_time)) < 0) return -1;

  return ret;
}


// 设置进程优先级的系统调用
uint64 sys_set_priority(void) {
  int priority, pid;

  if (argint(0, &priority) < 0) return -1;
  if (argint(1, &pid) < 0) return -1;
  return set_priority(priority, pid);
}