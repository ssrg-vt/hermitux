#include <sys/cdefs.h>
#include <stdlib.h>

static void exch(char* base,size_t size,size_t a,size_t b) {
  char* x=base+a*size;
  char* y=base+b*size;
  while (size) {
    char z=*x;
    *x=*y;
    *y=z;
    --size; ++x; ++y;
  }
}

#define RAND

/* Quicksort with 3-way partitioning, ala Sedgewick */
/* Blame him for the scary variable names */
/* http://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf */
static void quicksort(char* base,size_t size,ssize_t l,ssize_t r,
		      int (*compar)(const void*,const void*)) {
  ssize_t i=l-1, j=r, p=l-1, q=r, k;
  char* v=base+r*size;
  if (r<=l) return;

#ifdef RAND
  /*
     We chose the rightmost element in the array to be sorted as pivot,
     which is OK if the data is random, but which is horrible if the
     data is already sorted.  Try to improve by exchanging it with a
     random other pivot.
   */
  exch(base,size,l+(rand()%(r-l)),r);
#elif defined MID
  /*
     We chose the rightmost element in the array to be sorted as pivot,
     which is OK if the data is random, but which is horrible if the
     data is already sorted.  Try to improve by chosing the middle
     element instead.
   */
  exch(base,size,l+(r-l)/2,r);
#endif

  for (;;) {
    while (++i != r && compar(base+i*size,v)<0) ;
    while (compar(v,base+(--j)*size)<0) if (j == l) break;
    if (i >= j) break;
    exch(base,size,i,j);
    if (compar(base+i*size,v)==0) exch(base,size,++p,i);
    if (compar(v,base+j*size)==0) exch(base,size,j,--q);
  }
  exch(base,size,i,r); j = i-1; ++i;
  for (k=l; k<p; k++, j--) exch(base,size,k,j);
  for (k=r-1; k>q; k--, i++) exch(base,size,i,k);
  quicksort(base,size,l,j,compar);
  quicksort(base,size,i,r,compar);
}

void qsort(void* base,size_t nmemb,size_t size,int (*compar)(const void*,const void*)) {
  /* check for integer overflows */
  if (nmemb >= (((size_t)-1)>>1) ||
      size >= (((size_t)-1)>>1)) return;
#if 0
  if (sizeof(size_t) < sizeof(unsigned long long)) {
    if ((unsigned long long)size * nmemb > (size_t)-1) return;
  } else {
    if (size*nmemb/nmemb != size) return;
  }
#endif
  if (nmemb>1)
    quicksort(base,size,0,nmemb-1,compar);
}
