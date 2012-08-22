#include <propagate_list.h>

struct list *list_add(struct list *lh, struct list *p) {
	if (lh) {
		p->lh.next = lh->next;
		p->lh.prev = lh;
		lh->next = &p->lh;
	} else {
		p->lh.next = &p->lh;
		p->lh.prev = &p->lh;
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
