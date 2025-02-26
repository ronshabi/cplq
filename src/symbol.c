#include "symbol.h"
#include "list.h"

void symbol_table_init(struct symbol_table *stbl)
{
    list_init(&stbl->symbols);
}

struct symbol *symbol_table_add_symbol(struct symbol_table *stbl, const char *name, enum symbol_type type)
{
    struct symbol *symbol;
}
