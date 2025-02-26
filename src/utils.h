#pragma once

#include <stdint.h>


void __attribute__((noreturn)) die(const char *s);
void *xmalloc(uint64_t size);