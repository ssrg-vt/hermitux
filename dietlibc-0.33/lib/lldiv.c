#define _GNU_SOURCE
#include <stdlib.h>
#include <inttypes.h>

lldiv_t lldiv(long long numerator, long long denominator) {
  lldiv_t x;
  x.quot=numerator/denominator;
  x.rem=numerator-x.quot*denominator;
  return x;
}

imaxdiv_t imaxdiv(intmax_t numerator, intmax_t denominator)  __attribute__((alias("lldiv")));
