#include <stdio_ext.h>
#include "dietstdio.h"

void __fpurge(FILE* stream) {
  stream->ungotten=0;
  stream->bs=stream->bm=0;
}
