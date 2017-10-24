#include <stdio_ext.h>
#include "dietstdio.h"

int __freadable(FILE* stream) {
  return !!(stream->flags&CANREAD);
}
