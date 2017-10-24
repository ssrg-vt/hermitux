#include <stdio_ext.h>
#include "dietstdio.h"

size_t __fbufsize(FILE* stream) {
  return stream->buflen;
}
