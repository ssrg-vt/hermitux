#include <stdio_ext.h>
#include "dietstdio.h"

int __flbf(FILE* stream) {
  return !!(stream->flags&BUFLINEWISE);
}
