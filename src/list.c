#include "list.h"

void list_init(struct list *list)
{
    list->next = list;
    list->prev = list;
}

void list_add(struct list *list, struct list *item)
{
    item->next = list->next;
    item->prev = list;
    list->next->prev = item;
    list->next = item;
}

