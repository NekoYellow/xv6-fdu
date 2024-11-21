// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

uint extern ticks;

// 修改bcache的结构 变成hash table
struct {
  struct spinlock lock[NBUCKETS];
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head[NBUCKETS];
} bcache;

void binit(void)
{
  struct buf *b;

  for (int i = 0; i < NBUCKETS; i++) {
    initlock(&bcache.lock[i], "bcache");
  }

  // append bufs to buckets
  for (b = bcache.buf; b < bcache.buf+NBUF-1; b++) {
    int i = (b - bcache.buf) % NBUCKETS;
    if (!bcache.head[i].next) bcache.head[i].next = b;
    else b->next = bcache.head[i].next, bcache.head[i].next = b;
    initsleeplock(&b->lock, "buffer");
  }
}

int can_lock(int i, int j) {
  for (int k = 0; k <= NBUCKETS/2; k++) {
    if ((i+k) % NBUCKETS == j) return 1;
  }
  return 0;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf* bget(uint dev, uint blockno) {
  struct buf *b, *selected;

  int id = blockno % NBUCKETS;
  acquire(&bcache.lock[id]);

  // Is the block already cached?
  for (b = bcache.head[id].next; b; b = b->next) {
    if (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      if (holding(&bcache.lock[id]))
        release(&bcache.lock[id]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  int mni = -1;
  uint mnt = __UINT32_MAX__;
  // find the LRU unused buffer
  for (int j = 0; j < NBUCKETS; j++) {
    if (!can_lock(id, j)) continue;
    if (j != id) acquire(&bcache.lock[j]);
    for (b = bcache.head[j].next; b; b = b->next) {
      if (b->refcnt) continue;
      if (b->time < mnt) {
        mnt = b->time;
        if (~mni && mni != j && holding(&bcache.lock[mni]))
          release(&bcache.lock[mni]);
        mni = j;
      }
    }
    if (j != id && j != mni && holding(&bcache.lock[j]))
      release(&bcache.lock[j]);
  }

  if (mni == -1)
    panic("bget: no buffers");

  selected = bcache.head[mni].next;
  for (b = &bcache.head[mni]; b->next; b = b->next) {
    if ((b->next)->refcnt == 0 && (b->next)->time == mnt) {
      selected = b->next;
      b->next = b->next->next;
      break;
    }
  }

  if (mni != id && holding(&bcache.lock[mni]))
    release(&bcache.lock[mni]);
  
  for (b = &bcache.head[id]; b->next; b = b->next);
  b->next = selected;
  selected->next = 0;
  selected->dev = dev;
  selected->blockno = blockno;
  selected->valid = 0;
  selected->refcnt = 1;

  if (holding(&bcache.lock[id]))
    release(&bcache.lock[id]);
  acquiresleep(&selected->lock);
  return selected;
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int id = b->blockno % NBUCKETS;
  acquire(&bcache.lock[id]);
  b->refcnt--;
  if (b->refcnt == 0) {
    b->time = ticks;
  }
  
  release(&bcache.lock[id]);
}

void
bpin(struct buf *b) {
  int id = b->blockno % NBUCKETS;
  acquire(&bcache.lock[id]);
  b->refcnt++;
  release(&bcache.lock[id]);
}

void
bunpin(struct buf *b) {
  int id = b->blockno % NBUCKETS;
  acquire(&bcache.lock[id]);
  b->refcnt--;
  release(&bcache.lock[id]);
}


