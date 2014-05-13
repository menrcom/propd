#include <propagate_list.h>

struct list *list_add(struct list *lh, struct list *p) {
	if (lh) {
		p->next = lh->next;
		p->prev = lh;
		lh->next = &p;
	} else {
		p->next = &p;
		p->prev = &p;
		lh = p;
	}

	return lh;
}

struct list *list_del(struct list *lh) {

	struct list *prev = lh->prev;
	struct list *next = lh->next;
	prev->next = next;
	next->prev = prev;
	return lh;

}
