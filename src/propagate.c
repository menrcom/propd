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

static inline struct pkt_buffer *new_buffer(struct cfg_ctx *cfg) {
	/* Вставлять новый буфер в голову списка */
	struct pkt_buffer *b = malloc(sizeof(struct pkt_buffer));

	if (!b) {
		syslog(LOG_ERR,
		       "new_buffer: Cannot allocate %d bytes of memory.",
		       sizeof(struct pkt_buffer));
		return NULL;
	}

	cfg->send_queue = list_add(cfg->send_queue, b);

	return b;
}

static inline int buffer_full(struct list *lh) {
	struct pkt_buffer *b = LIST_GET(lh, struct pkt_buffer, lh);
	return b->packets_total == b->packets_present;
}

static struct propagate_packet *enqueue_route(struct cfg_ctx *cfg, struct in_addr *addr) {

	/*
	  Если уже есть буфер - заполнять его
	  Иначе делать новый и добавлять в список буферов

	  Возможные варианты:
	  - буферов нет вообще:      создавать новый буфер
	  - буфера есть
	  - последний заполнен:    создавать новый буфер
	  - последний не заполнен: пихать маршрут в существующий буфер
	*/

	struct pkt_buffer *b;

	if (cfg->send_queue && !buffer_full(cfg->send_queue)) {

		if(cfg->debug)
			syslog(LOG_INFO,
			       "enqueue_route: there is space left in buffer");
		b = LIST_GET(cfg->send_queue, struct pkt_buffer, lh);

	} else
		b = new_buffer(cfg);

	if (!b) {
		syslog(LOG_ERR,
		       "enqueue_route: there is no space left in buffer,"
		       " or no buffer, and cannot allocate new buffer.");
		return NULL;

	}

	if(cfg->debug)
		syslog(LOG_INFO,
		       "enqueue_route: there is no space left in buffer,"
		       " or no buffer");

	struct propagate_packet *p = b->data + b->packets_present;

	p->type = 's';
	p->destination = addr.s_addr;
	p->gateway = cfg->gaddr.s_addr;

	return p;
}

static int enqueue_routes(struct cfg_ctx *cfg) {

	int vc = 0;
	FILE *ip_route = popen(cfg->cmd_get_routes, "r");
	char line[256], ip[17];
	line[255] = ip[16] = 0;

	if(!ip_route) {
		syslog(LOG_ERR,
		       "queue_send_routes: Cannot execute %s: %s",
		       cfg->cmd_get_routes,
		       strerror(errno));
		return 0;
	}

	/*
	  По идее, если все маршруты с предудущего вызова ещё не отправлены,
	  то очередь непустая.
	  Значит, можно не отправлять маршруты заново.
	 */

	if(cfg->send_queue) {
		syslog(LOG_INFO,
		       "enqueue_routes: There is routes left in queue."
		       "Ignoring send request.");
		return vc;
	}

	while(fgets(line, 255, ip_route)){

		if(cfg->debug)
			syslog(LOG_INFO,
			       "send_routes: got route: %s",
			       line);

		struct in_addr daddr;

		if(1 == sscanf(line, "%15[0-9.]", ip) && inet_aton(ip, &daddr)) {
			if(cfg->debug)
				syslog(LOG_INFO, "send_routes: got ip: %s", ip);
			if(!enqueue_route(cfg, &daddr)) break;
			vc++;
		} else {
			syslog(LOG_INFO, "Invalid route output line: %s", line);
		}

	};

	pclose(ip_route);
	/* Return vector length */
	return vc;
}


static int do_route_set(struct cfg_ctx cfg, char *cmd) {

	char ip[17], gw[17];
	struct propagate_packet * msg = cfg->recv_buffer.data + cfg->recv_buffer.packets_processed;

	inet_ntop(AF_INET, &msg->destination, ip, 16);
	inet_ntop(AF_INET, &msg->gateway, gw, 16);

	char line[256];

	ip[16] = ip[17] = line[255] = 0;

 	sprintf(line, cmd, ip, gw);

	if(cfg->debug)
		syslog(LOG_DEBUG, "do_route_set: Will do: %s", line);

	int err = system(line);
	if(err) syslog(LOG_ERR, "do_route_set: Cannot execute %s. Code: %d", line, err);

	return !err;

}

static int recvd_route_delete(struct cfg_ctx *cfg) {
	if(cfg->debug)
		syslog(LOG_INFO, "recvd_route_delete();");
	return do_route_set(cfg, cfg->cmd_del_route);
}

static int recvd_route_null(struct cfg_ctx *cfg) {
	if(cfg->debug)
		syslog(LOG_INFO, "recvd_route_null();");
	return do_route_set(cfg, cfg->cmd_nul_route);
}

static int recvd_route_set(struct cfg_ctx *cfg) {
	if(cfg->debug)
		syslog(LOG_INFO, "recvd_route_set();");
	return do_route_set(cfg, cfg->cmd_set_route);
}


static int recv_full_buffer(struct cfg_ctx *cfg) {

	if(cfg->debug) syslog(LOG_INFO, "recv_full_buffer: Processing control FD: %d", cfg->control_fd);

	struct propagate_packet *buf = cfg->recv_buffer.data + cfg->recv_buffer.packets_present;
	ssize_t bt = btotal = 0;
	ssize_t bn = sizeof(struct propagate_packet)
		* (cfg->recv_buffer.packets_total - cfg->recv_buffer.packets_present);

	do {

		bt = recvfrom(cfg->control_fd,
			      buf + btotal,
			      bn - btotal,
			      0, NULL, NULL);

		if (bt == -1)
			switch (errno) {
			case EAGAIN:
			case EWOULDBLOCK:
				bt = 0;
				break;
			default:
				syslog(LOG_INFO, "recv_full_buffer: Error in recv: %s",
				       strerror(errno));
			}

		else if (bt > 0 ) btotal += bt;

	} while (bt > 0 && bn - btotal > 0);

	return btotal;
}

static void process_command(struct cfg_ctx *cfg) {

	/* TODO: Add source address for recvfrom. Discard updates from my local address? */

	struct propagate_packet *p = cfg->recv_buffer.data + cfg->recv_buffer.packets_processed;

	if(cfg->debug) syslog(LOG_INFO, "process_commands: Received request: %c", p->type);

	switch(p->type) {

	case 'g': /* Get */
		if(!cfg->passive)
			queue_send_routes(cfg);
		break;

	case 's': /* Set */
		if(cfg->debug) syslog(LOG_INFO,
				      "process_commands: my addr: %x, p->gateway: %x",
				      (int)cfg->laddr.s_addr,
				      (int)p->gateway);

		if(p->gateway != cfg->laddr.s_addr)
			recvd_route_set(cfg);
		break;

	case 'd': /* Delete */
		recvd_route_delete(cfg);
		break;

	case 'n': /* Set to null */
		recvd_route_null(cfg);
		break;

	default:;
		/* unsupported or invalid message */
	}

	cfg->recv_buffer.packets_processed++;
}

void process_commands(struct cfg_ctx *cfg) {
	while(recv_full_buffer(cfg))
		while(cfg->recv_buffer.packets_processed < cfg->recv_buffer.packets_total)
			process_command(cfg);
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
