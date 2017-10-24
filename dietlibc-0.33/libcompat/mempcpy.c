#define _GNU_SOURCE
#include <string.h>

void *mempcpy(void* __restrict__ dst, const void* __restrict__ src, size_t n) {
  return memcpy(dst,src,n)+n;
}
