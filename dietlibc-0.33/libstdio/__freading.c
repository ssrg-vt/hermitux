#include <stdio_ext.h>
#include "dietstdio.h"

int __freading(FILE* stream) {
  return !(stream->flags&CANWRITE);
}
