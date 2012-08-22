#ifndef _PROPAGATED_H
#define _PROPAGATED_H 1

#include <netinet/in.h>
#include <propagate.h>
#include "../config.h"

struct cfg_ctx;

struct event_handler {
	int (*run)(struct cfg_ctx *);
	char *name;
};

struct cfg_ctx {
	int daemonize:1;
	int passive:1;
	int debug:1;
	int control_fd;
	char *cmd_get_routes;
	char *cmd_set_route;
	char *cmd_del_route;
	char *cmd_nul_route;
	char *pidfile;
	char *mcastip;
	char *gwip;
	char *localip;
	struct pkt_buffer recv_buffer;
	struct list *send_queue;
	struct sockaddr_in maddr;
	struct in_addr gaddr;
	struct in_addr laddr;
	struct event_handler *eh;
};

struct cfg_ctx *configure(int, char **);
int setup(struct cfg_ctx *);
void cleanup(struct cfg_ctx *);

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
