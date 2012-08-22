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

int select_process_events_and_timers(struct cfg_ctx *);

struct event_handler default_event_handler = {
	.run = select_process_events_and_timers,
	.name = "default"
};

struct event_handler select_event_handler = {
	.run = select_process_events_and_timers,
	.name = "select"
};

int select_process_events_and_timers(struct cfg_ctx *cfg) {

	fd_set read_fds, write_fds;
	int ready_fds = 0;

	/* do { */
	/* 	int max_fd = cfg->control_fd; */

	/* 	FD_ZERO(&read_fds); */
	/* 	FD_ZERO(&write_fds); */
	/* 	FD_SET(cfg->control_fd, &read_fds); */
	/* 	if(write_vector) FD_SET(cfg->control_fd, &write_fds); */


	/* 	if(cfg->debug) syslog(LOG_DEBUG, */
	/* 			      "process_events_and_timers() try to select," */
	/* 			      "with max fd: %d", */
	/* 			      max_fd); */


	/* 	ready_fds = select(max_fd + 1, &read_fds, &write_fds, */
	/* 			   NULL, NULL); */

	/* 	if(ready_fds < 0) { */
	/* 		syslog(LOG_ERR, */
	/* 		       "Error in select(): %s", strerror(errno)); */
	/* 		return 0; */
	/* 	} */

	/* 	if(FD_ISSET(cfg->control_fd, &read_fds)) { */
	/* 		ready_fds--; */
	/* 		/\* */
	/* 		 * New data on the control socket? */
	/* 		 *\/ */
	/* 		process_commands(cfg); */
	/* 	} */

	/* 	if(write_vector && ready_fds-- */
	/* 	   && FD_ISSET(cfg->control_fd, &write_fds)) { */
	/* 		/\* */
	/* 		 * Control socket ready to write? */
	/* 		 *\/ */
	/* 		process_writes(cfg); */
	/* 	} */

	/* } while(1); */
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
