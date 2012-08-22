#ifndef _MODULES_H
#define _MODULES_H 1

#include <propagated.h>
#include "../config.h"

struct event_handler default_event_handler;
struct event_handler select_event_handler;
struct event_handler epoll_event_handler;

static struct event_handler *event_handlers[] = {
	&default_event_handler,
	&select_event_handler,
	&epoll_event_handler,
	NULL
};

#endif

/*
 * Local variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 *  c-indent-level: 8
 *  c-basic-offset: 8
 *  tab-width: 8
 * End:
 */
