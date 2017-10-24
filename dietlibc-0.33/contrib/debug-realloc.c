#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

static struct malloced {
  unsigned long len,maplen;
  struct malloced* next;
  unsigned long magic;
}* root=0;

static const unsigned long MAGIC=(unsigned long)0xfefec0dedeadbeefull;

static void checkmagic() {
  struct malloced* x=root;
  while (x) {
    if (x->magic != MAGIC)
      abort();
    x=x->next;
  }
}

static void* domalloc(size_t size) {
  char* n,* m;
  unsigned long s=size+sizeof(struct malloced);
  checkmagic();
  s=(s+4096+4095)&~4095;	// add one page, and round up to fill one page
  n=mmap(0,s,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  if (n==MAP_FAILED) return 0;
  m=n-size;  while (m<n) m+=4096;
//  mprotect(m,size+sizeof(struct malloced),PROT_READ|PROT_WRITE);
  mprotect(n,((unsigned long)m&4095)+sizeof(struct malloced)+(size&~4095),PROT_READ|PROT_WRITE);
  ((struct malloced*)n)->len=size;
  ((struct malloced*)n)->maplen=s;
  ((struct malloced*)n)->next=root;
  ((struct malloced*)n)->magic=MAGIC;
  root=(struct malloced*)n;
  return m;
}

void* malloc(size_t size) {
  void* m=domalloc(size);;
#ifdef DEBUG
  printf("malloc(%zu) -> %p\n",size,m);
#endif
  return m;
}

static void dofree(void* ptr) {
  struct malloced** x=&root;
  checkmagic();
  while (*x) {
    if (((char*)(*x))+sizeof(struct malloced)<(char*)ptr && 
	((char*)(*x))+4096>(char*)ptr) {
      struct malloced* old=*x;
      *x=(*x)->next;
      mprotect(old,old->maplen,PROT_NONE);
      return;
    }
    x=&(*x)->next;
  }
  abort();
}

void free(void* ptr) {
#ifdef DEBUG
  printf("free(%p)\n",ptr);
#endif
  dofree(ptr);
}

void *realloc(void *ptr, size_t size) {
  unsigned long oldsize=0;
  struct malloced* x=root;
  char* fnord;
  if (ptr) {
    oldsize=-1;
    while (x) {
      if (((char*)x)+sizeof(struct malloced)<(char*)ptr && 
	  ((char*)x)+4096>(char*)ptr) {
	oldsize=x->len;
	break;
      }
      x=x->next;
    }
    if (oldsize==(unsigned long)-1)
      abort();
  }
  fnord=domalloc(size);
  memcpy(fnord,ptr,size>oldsize?oldsize:size);
  if (oldsize) dofree(ptr);
#ifdef DEBUG
  printf("realloc(%p,%zu) -> %p\n",ptr,size,fnord);
#endif
  return fnord;
}
