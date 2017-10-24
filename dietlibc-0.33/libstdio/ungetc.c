#include "dietstdio.h"

int ungetc_unlocked(int c, FILE *stream) {
  if (stream->ungotten || c<0 || c>255)
    return EOF;
  /* GNU configure configure wants us to differentiate between an ungetc
   * of the byte that was actually there ("backup ungetc") and a
   * different byte. Sigh. */
  if (stream->bm && stream->buf[stream->bm-1]==c) {
    --stream->bm;
  } else {
    stream->ungotten=1;
    stream->ungetbuf=(unsigned char)c;
  }
  stream->flags&=~(ERRORINDICATOR|EOFINDICATOR);
  return c;
}

int ungetc(int c, FILE *stream) __attribute__((weak,alias("ungetc_unlocked")));
