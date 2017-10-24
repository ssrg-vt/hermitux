#include <sys/atomic.h>
#include <stddef.h>
#include <assert.h>
#include <write12.h>

size_t a,b,c;

int main() {
  a=1;
  assert(__CAS(&a,0,3)==1 && a==1);
  assert(__CAS(&a,1,2)==1 && a==2);
  __write1("CAS test successful\n");

  assert(__atomic_add(&a,1)==3);
  assert(__atomic_add(&a,-2)==1);
  __write1("atomic_add test successful\n");
  return 0;
}
