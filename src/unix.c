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

int process_writes(int control_fd) {
/* #ifdef DEBUG */
/* 	syslog(LOG_INFO, "process_writes()"); */
/* #endif */
/* 	/\* Write pkt_vector into control_fd*\/ */
/* 	int bt = 1; */
/* 	struct pkt_vector *wp = write_vector; */

/* 	while(wp && bt) { */
/* 		bt = sendto(control_fd, &wp->pkt, */
/* 			    sizeof(struct propagate_packet), 0, */
/* 			    (struct sockaddr *)&mca, sizeof(mca)); */

/* 		switch(bt){ */
/* 		case -1: */
/* 			if(errno == EAGAIN) bt = 0; */
/* 			else syslog(LOG_INFO, */
/* 				    "Error sending packet: %s", */
/* 				    strerror(errno)); */
/* 			break; */
/* 		case 0: */
/* 			break; */
/* 		default: */
/* 			if(bt == sizeof(struct propagate_packet)) { */
/* 				wp = wp->next; */
/* 				free(write_vector); */
/* 				write_vector = wp; */
/* 			} /\* Else - try to write it again. *\/ */
/* 		} */
/* 	}; */

	return 1;
}

int open_control_socket(struct cfg_ctx *cfg){

	int yes = 1;
	
	/* Create socket */

	cfg->control_fd = socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, 0);

	if(cfg->control_fd < 0){
		syslog(LOG_ERR,
		       "Cannot create socket: %s\nExiting...\n",
		       strerror(errno));

		return 0;
	}


	/* Set multicast looping back */
	if(-1 == setsockopt(cfg->control_fd,
			    IPPROTO_IP,
			    IP_MULTICAST_LOOP,
			    &yes,
			    sizeof(yes)))
	{
		syslog(LOG_ERR,
		       "Cannot set multicast looping back: %s",
		       strerror(errno));

		close(cfg->control_fd);
		return 0;
	}


	/* Set multicasting interface */
	if(cfg->localip && -1 == setsockopt(cfg->control_fd,
					    IPPROTO_IP,
					    IP_MULTICAST_IF,
					    &cfg->laddr,
					    sizeof(struct in_addr)))
	{
		syslog(LOG_ERR, "Cannot set multicast local address to %s: %s",
		       cfg->localip,
		       strerror(errno));

		close(cfg->control_fd);
		return 0;
	}

	struct ip_mreq mreq = { .imr_multiaddr = cfg->maddr.sin_addr, .imr_interface = cfg->laddr};

	/* Set multicasting membership */
	if(-1 == setsockopt(cfg->control_fd,
			    IPPROTO_IP,
			    IP_ADD_MEMBERSHIP,
			    &mreq,
			    sizeof(mreq)))
	{
		syslog(LOG_ERR, "Cannot set multicast membership: %s",
		       strerror(errno));

		close(cfg->control_fd);
		return 0;
	}


	if(-1 == bind(cfg->control_fd,
		      (struct sockaddr *)&cfg->maddr,
		      sizeof(struct sockaddr_in)))
	{
		syslog(LOG_ERR, "Cannot bind socket to mcast addr %s: %s",
		       cfg->mcastip, strerror(errno));

		return 0;
	}

	return cfg->control_fd;
}



int setup(struct cfg_ctx *cfg) {

	openlog("propagated", LOG_PID, LOG_DAEMON);

	cfg->recv_buffer.data = calloc(cfg->recv_buffer.packets_total,
				       sizeof(struct propagate_packet));
	cfg->send_buffer.data = calloc(cfg->send_buffer.packets_total,
				       sizeof(struct propagate_packet));

	if(!cfg->recv_buffer.data || !cfg->send_buffer.data) {
		syslog(LOG_ERR, "setup: Cannot allocate memory.");
		return 0;
	}

	if(cfg->daemonize && daemon(0,0)) {
		syslog(LOG_ERR, "setup: Cannot daemonize: %s", strerror(errno));
		return 0;
	}

	FILE *fp_pid = fopen(cfg->pidfile, "w");

	if(!fp_pid) {
		syslog(LOG_ERR, "setup: Cannot write pid-file: %s", strerror(errno));
		return 0;
	}

	fprintf(fp_pid, "%d", getpid());

	fclose(fp_pid);

	if(cfg->debug)
		syslog(LOG_DEBUG,
		       "setup: propagated started. mcast addr: %s, gw addr: %s, Local addr: %s",
		       cfg->mcastip,
		       cfg->gwip,
		       cfg->localip?cfg->localip:"0.0.0.0");

	return open_control_socket(cfg);

}

void cleanup(struct cfg_ctx *cfg) {
	if(!cfg) return;
	if(cfg->recv_buffer.data) free(cfg->recv_buffer.data);
	if(cfg->send_buffer.data) free(cfg->send_buffer.data);
	unlink(cfg->pidfile);
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
