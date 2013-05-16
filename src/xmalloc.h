#ifndef __SSTABLE_XMALLOC_H
#define __SSTABLE_XMALLOC_H

//#include "internal.h"
#include <stddef.h>

void *xmalloc(size_t n);
void *xcalloc(size_t n, size_t s);
void *xrealloc(void *p, size_t n);
void xfree(void *p);

#endif
