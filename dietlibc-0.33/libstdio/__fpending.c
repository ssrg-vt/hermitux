#include <stdio_ext.h>
#include "dietstdio.h"

size_t __fpending(FILE* stream) {
  return stream->bm;
}
