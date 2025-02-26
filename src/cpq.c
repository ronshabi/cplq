#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

extern FILE *yyin;
extern int yyparse();

struct list symbol_table;

void usage() {
    fprintf(stderr, "usage: ./cpq [source_file.ou]\n");
    exit(EXIT_FAILURE);
}

void check_file_extension(const char *filename)
{
    const char *p = strrchr(filename, '.');
    if (!p || strcmp(p, ".ou") != 0) {
        fprintf(stderr, "error: wrong file extension\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        usage();
    }

    xmalloc(-1);

    check_file_extension(argv[1]);
    yyin = fopen(argv[1], "r");

    list_init(&symbol_table);

    yyparse();

    return 0;
}

void yyerror(const char *s)
{
    printf("[yyerror: '%s']\n", s);
}