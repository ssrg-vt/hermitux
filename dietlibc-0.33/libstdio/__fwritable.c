#include <stdio_ext.h>
#include "dietstdio.h"

int __fwritable(FILE* stream) {
  return !!(stream->flags&CANWRITE);
}
