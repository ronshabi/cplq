#pragma once

struct list {
    struct list *next;
    struct list *prev;
};

#define OFFSET_OF(type, member) __builtin_offsetof(type, member)
#define CONTAINER_OF(ptr, type, member) ((type*)(char *)ptr - OFFSET_OF(type, member))

void list_init(struct list *list);
void list_add(struct list *list, struct list *item);
