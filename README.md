The main branch is a project for the [CSCC](https://os.educg.net/#/index?TYPE=OS_HDN), and the other branches are xv6 labs.

Upd: Our project won the [third prize](https://os.educg.net/#/index?TYPE=OS_HDN)


This project is based on [xv6](https://github.com/mit-pdos/xv6-riscv)

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".


# OS原理赛道

本项目对xv6进行了扩展，实现了一些现代操作系统中常见的用户功能。

我们的工作可以设计成一到两个教学实验，服务于仅以了解为目的学习操作系统的学生。


## 1. 符号链接支持

原始的xv6仅支持硬链接（link），在许多时候不够灵活，比如不支持链接到文件夹（否则可能会在目录树上成环），因此我们实现了软链接（symbol link）。

系统调用`int symlink(char *target, char *path)`建立一个名为`path`的符号链接文件，指向`target`。在shell中可以通过`ln -s target path`来实现符号链接的建立。

测试文件user/symlinktest对符号链接实现进行测试，该文件来自[mit的xv6实验](https://pdos.csail.mit.edu/6.S081/2021/)

## 2. 目录加载

原有的`mkfs.c`仅支持把宿主机中的单个文件加载到虚拟机里，这里对其进行扩展，使它也能支持加载某个路径下的整个目录结构。

目前项目中新增了一个用于加载的测试目录home，并且加到了makefile里`fs.img`这一项的参数中，使得booting时home会被加载到xv6里。这一功能使得每次启动时就拥有一个相对复杂完整的文件目录结构成为可能，为后续其他功能的命令行测试提供了便利，避免了每次重启xv6需要手动`mkdir`的麻烦。

## 3. 用户权限机制

原始的xv6没有多用户机制，用户登录即具有所有权限，带来了潜在的安全性问题。如今的PC很少有需要多人使用的场景，因此我们认为传统的多用户机制没有必要，只需要实现普通用户和超级用户的切换。我们实现的接口如下：

系统调用：

1. 设置指定进程的uid（普通用户`CU`/超级用户`SU`），如果提供了正确的密码
```c
int setuid(int pid, int uid, char *password);
```

2. 获取指定进程的uid
```c
int getuid(int pid);
```

用户程序：

1. 成为超级用户，如果提供了正确的密码
```sh
su password
```

2. 切换回普通用户
```sh
exit
```

3. 获取当前用户（SU/CU）
```sh
whoami
```

实现细节：

系统内部每个进程有一个属性`int uid`记录当前进程的用户，并且会在fork时被继承。用户程序的实现相对trivial：注意到所有用户程序都会由sh进程fork+exec执行，我们只需要修改sh进程的`uid`就能实现整个shell环境的用户切换。

user/uidtest对这一功能进行测试。

有了这一权限机制，我们扩展了`ln`命令使之支持在超级用户权限下创建指向文件夹的链接。

我们还可以进一步为所有文件设置权限，用户只能访问和修改它拥有权限的那些文件。比如可以把根目录权限默认设为`SU`，这样就能防止在根目录下不小心`rm`掉整个文件系统。

## 4. 环境变量支持

我们给每个进程加上了环境变量表，在fork时会被继承。原始xv6的sh在执行程序上的实现非常trivial，仅会在根目录`/`下寻找可执行文件，环境变量的存在使得我们可以使其转而在`PATH`变量中的所有路径里寻找可执行文件。接口如下：

系统调用：

1. 获得指定进程的环境变量`key`的值（存入`val`）
```c
int getenv(int pid, char *key, char *val);
```

2. 把指定进程的环境变量`key`的值设为`val`
```c
int setenv(int pid, char *key, char *val);
```

3. 列举指定进程的所有环境变量
```c
int env(int pid);
```

用户程序：

1. 设置环境变量`name`的值为`value`
```sh
set name value
```

2. 扩展`echo`，会evaluate形如`$name`的参数，如果`name`是存在的环境变量
```sh
echo $name
```

实现细节：

环境变量表内部实现为哈希表，支持键值对的插入、修改。用户程序同样是直接对sh进程进行操作。

uid/envtest对这一功能进行测试。

在此基础上我们实现了`PATH`相关机制。启动时`PATH`作为初始环境变量，值为`/`（根目录）。在shell中输入一条指令后，如果输入的程序路径没有找到，shell会在`PATH`下的所有目录寻找该程序。这是现代shell具备的基本功能。
