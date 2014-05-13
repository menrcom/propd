#ifndef _PROPAGATE_LIST_H
#define _PROPAGATE_LIST_H 1


struct list {
	struct list *prev;
	struct list *next;
};

#define LIST_GET(addr, type, name) (type *)(addr - &((type *)0)->name)

struct list *list_add(struct list *, struct list *);
struct list *list_del(struct list *);

#endif
