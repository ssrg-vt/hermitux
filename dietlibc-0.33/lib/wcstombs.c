#include <wchar.h>
#include "dietlocale.h"
#include <wchar.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

size_t wcstombs(char *dest, const wchar_t *src, size_t len) {
  return wcsrtombs(dest,&src,len,NULL);
}
