#ifndef _INET_NTOP_CACHE_H_
#define _INET_NTOP_CACHE_H_
#include "first.h"

#include "base_decls.h"

const char * inet_ntop_cache_get_ip(server *srv, sock_addr *addr);

#endif
