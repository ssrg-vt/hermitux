#include <endian.h>
#include <stdlib.h>
#include <inttypes.h>

#if __WORDSIZE == 64
long int labs(long int i) { return i>=0?i:-i; }
long long int llabs(long long int i) __attribute__((alias("labs")));
intmax_t imaxabs(intmax_t i) __attribute__((alias("labs")));
#endif
