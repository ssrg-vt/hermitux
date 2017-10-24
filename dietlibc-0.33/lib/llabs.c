#include <endian.h>
#include <stdlib.h>
#include <inttypes.h>

#if __WORDSIZE != 64
long long int llabs(long long int i) { if (i<0) i=-i; return i; }
intmax_t imaxabs(intmax_t i) __attribute__((alias("llabs")));
#endif
