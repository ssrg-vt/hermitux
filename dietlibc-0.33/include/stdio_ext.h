#ifndef _STDIO_EXT_H
#define _STDIO_EXT_H

#include <stdio.h>

__BEGIN_DECLS

size_t __fbufsize(FILE *stream);
size_t __fpending(FILE *stream);
int __flbf(FILE *stream);
int __freadable(FILE *stream);
int __fwritable(FILE *stream);
int __freading(FILE *stream);
int __fwriting(FILE *stream);
int __fsetlocking(FILE *stream, int type);
void _flushlbf(void);
void __fpurge(FILE *stream);

__END_DECLS

#endif
