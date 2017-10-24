/* fast memchr -- Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at> */
#include <string.h>
#include "dietfeatures.h"
#include "dietstring.h"

#if __WORDSIZE == 64
# define MB 0x7efefefefefefeff
#else
# define MB 0x7efefeff
#endif

void *
memchr (const void *s, int c, size_t n)
{
    const unsigned char *pc = (unsigned char *) s;
#ifdef WANT_SMALLER_STRING_ROUTINES
    for (;n--;pc++) if (*pc == c) return ((void *) pc);
	return (NULL);
#else
    unsigned long l, lb, lt;
    int tmp;

    if ((tmp = STRALIGN(s)) || n < sizeof(unsigned long)) {
	if (n < sizeof(unsigned long)) tmp = n;
	for (; tmp-- && n--; pc++)
	    if (*pc == c)
		return ((char *) pc);
	if (n == (size_t) - 1)
	    return (NULL);
    }

    lb = c | c << 8;
    lb |= lb << 16;
#if __WORDSIZE == 64
    lb |= lb << 32;
#endif

    while (n >= sizeof(unsigned long)) {
	l = *(unsigned long *) pc;
	lt = l ^ lb;
	if ((((lt + MB) ^ ~lt) & ~MB)) {
	    while (l && (l & 0xff) != (unsigned long) c) l >>= 8, pc++;
	    if (l) return ((char *) pc);
	} else
	pc += sizeof(unsigned long);
	n -= sizeof(unsigned long);
    }
    for (; n--; pc++) if (*pc == c) return ((char *) pc);
    return (NULL);
#endif
}
