#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       2000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name
#define USERSTACK    1     // user stack pages
#define SU 0 // Superuser role
#define CU 1 // Common user role
#define MAXPWD 16   // maximum password length
#define PWD "xv6su" // demo password
#define HASH_B 233  // param for hashing
#define HASH_P 97   // param for hashing
#define MAXENVK 32  // maximum environ var name
#define MAXENVV 128 // maximum environ var value
