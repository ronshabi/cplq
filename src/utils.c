#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ansi.h"

void __attribute__((noreturn)) die(const char *s)
{
    fprintf(stderr, "%sfatal: %s%s\n", ANSI_BOLD ANSI_RED,ANSI_RESET, s);
    exit(EXIT_FAILURE);    
}

void *xmalloc(uint64_t size)
{
    void *p = malloc(size);
    if (!p) {
        die("malloc failed");
    }

    return p;
}
