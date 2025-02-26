#pragma once

#include "list.h"
#include <stdbool.h>

enum symbol_type {
    SYMBOL_INT,
    SYMBOL_FLOAT,
    SYMBOL_ID
};

struct symbol {
    char *name;
    enum symbol_type type;
    union {
        int i;
        double d;
    } val;
};

struct symbol_table {
    struct list symbols;
};

void symbol_table_init(struct symbol_table *stbl);
struct symbol *symbol_table_add_symbol(struct symbol_table *stbl, const char *name, enum symbol_type type);