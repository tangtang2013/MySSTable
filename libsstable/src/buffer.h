#ifndef __SSTABLE_BUF_H
#define __SSTABLE_BUF_H

//#include "internal.h"
#include <stddef.h>
#include "stdint.h"
#include "common.h"
#include "tinybloom.h"

typedef struct buffer 
{
	char *buf;
	int SEEK;
	int NUL;
	int buflen;
}buffer_t;

struct buffer *buffer_new(size_t reserve);
void buffer_free(struct buffer *b);

void buffer_clear(struct buffer *b);
char *buffer_detach(struct buffer *b);

void buffer_putc(struct buffer *b, const char c);
void buffer_putstr(struct buffer *b, const char *str);
void buffer_putnstr(struct buffer *b, const char *str, size_t n);
void buffer_putint(struct buffer *b, int val);
void buffer_scatf(struct buffer *b, const char *fmt, ...);
void buffer_putlong(struct buffer *b, uint64_t val);
void buffer_putshort(struct buffer *b, short val);
void buffer_putdata(struct buffer *b, struct data_item* d);
void buffer_putfilter(struct buffer *b,bloom_filter* bfilter);

uint32_t buffer_getint(struct buffer *b);
uint64_t buffer_getlong(struct buffer *b);
char *buffer_getnstr(struct buffer *b, size_t n);
char buffer_getchar(struct buffer *b);
struct data_item* buffer_getdata(struct buffer *b);
bloom_filter* buffer_getfilter(struct buffer *b);

void buffer_seekfirst(struct buffer *b);

void buffer_dump(struct buffer *b);

#endif
