#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<syslog.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<propagated.h>

int epoll_process_events_and_timers(struct cfg_ctx *);

struct event_handler epoll_event_handler = {
	.run = epoll_process_events_and_timers,
	.name = "epoll"
};

int epoll_process_events_and_timers(struct cfg_ctx *cfg) {
	return 0;
}


/*
 * Local variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 *  c-indent-level: 8
 *  c-basic-offset: 8
 *  tab-width: 8
 * End:
 */
