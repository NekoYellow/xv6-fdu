#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd, fd2;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  memset(buf, ' ', DIRSIZ-4);
  buf[DIRSIZ-4] = '\0';
  printf("FILE%s TYPE\tINODE\tNLINK\tSIZE\n", buf);

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%s %d\t%d\t%d\t%d\n", fmtname(path), st.type, st.ino, st.nlink, (int) st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if((fd2 = open(buf, O_RDONLY | O_NOFOLLOW)) < 0){
        printf("ls: cannot open %s\n", buf);
        close(fd2);
        continue;
      }
      if(fstat(fd2, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        close(fd2);
        continue;
      }
      close(fd2);
      printf("%s %d\t%d\t%d\t%d\n", fmtname(buf), st.type, st.ino, st.nlink, (int) st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
