#include <stdlib.h>
#include <wchar.h>
#include "dietlocale.h"
#include <wchar.h>
#include <errno.h>
#include <string.h>

size_t wcsrtombs(char *dest, const wchar_t **src, size_t len, mbstate_t *ps) {
  wchar_t c;
  char buf[MB_CUR_MAX];
  size_t cur;
  char* max;
  if (!src || !*src) {
inval:
    errno=EINVAL;
    return -1;
  }
  if (!dest) {
    len=-1;
    max=dest+len;
  }
  for (cur=0; (c=**src); ++*src) {
    size_t n;
    char* s=__likely(len-cur>=MB_CUR_MAX)?(dest?dest+cur:NULL):buf;
    n=wcrtomb(s,c,ps);
    if (n==(size_t)-1) return -1;
    if (dest && s==buf) {
      /* check if we fit */
      if (len<n) return cur;
      memcpy(dest+cur,buf,n);
    }
    cur+=n;
  }
  if (dest && len>cur) dest[cur]=0;
keinplatz:
  return cur;
}

