#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<syslog.h>
#include<propagated.h>
#include<modules.h>
#include "../config.h"

struct cfg_ctx *configure(int argc, char ** argv) {
	
	if(argc < 3) {
		printf("Usage: %s <mcast IP> <gw IP> [<local IP>] [-d]\n", argv[0]);
		return NULL;
	}

	struct cfg_ctx *root_cfg = malloc(sizeof(struct cfg_ctx));

	if(!root_cfg) {
		printf("configure: Cannot allocate %d bytes of memory\n", sizeof(struct cfg_ctx));
		return root_cfg;
	}

	memset(root_cfg, 0, sizeof(struct cfg_ctx));

	root_cfg->recv_buffer.packets_total = 166; /* 166 packets of 9 octets each */
	root_cfg->cmd_get_routes = "/sbin/route -n |grep UH";
	root_cfg->cmd_set_route = "/sbin/ip route replace %s/32 via %s\n";
	root_cfg->cmd_del_route = "/sbin/ip route delete  %s/32\n";
	root_cfg->cmd_nul_route = "/sbin/ip route replace blackhole %s/32\n";
	root_cfg->pidfile = "/var/run/propagated.pid";
	root_cfg->mcastip = argv[1];
	root_cfg->gwip = argv[2];
	root_cfg->localip = argc>3?argv[3]:NULL;
	root_cfg->eh = &default_event_handler;

	if(!inet_aton(root_cfg->mcastip, &root_cfg->maddr.sin_addr)) {
		syslog(LOG_ERR, "Invalid multicast address: %s", root_cfg->mcastip);
		return NULL;
	} else {
		root_cfg->maddr.sin_family = AF_INET;
		root_cfg->maddr.sin_port = htons(65535);

	}

	if(!inet_aton(root_cfg->gwip, &root_cfg->gaddr)) {
		syslog(LOG_ERR, "Invalid gateway address: %s", root_cfg->gwip);
		return NULL;
	}

	if(root_cfg->localip && !inet_aton(root_cfg->localip, &root_cfg->laddr)) {
		syslog(LOG_ERR, "Invalid local address: %s", root_cfg->localip);
		return NULL;
	}


	int i;

	if(argc > 4)
		for(i = 1; i < argc+1; i++)
			if (argv[i][0] == '-')
				switch (argv[i][1]) {
				case 'd':
					root_cfg->daemonize = 1;
					break;
				case 'p':
					root_cfg->passive = 1;
					break;
				case 'D':
					root_cfg->debug = 1;
					break;
				}

	return root_cfg;
};


/*
 * Local variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 *  c-indent-level: 8
 *  c-basic-offset: 8
 *  tab-width: 8
 * End:
 */
