#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/select.h>
#include<sys/time.h>
#include<unistd.h>
#include<propagate.h>
#include "../config.h"

struct in_addr dstip, gwip, localip;
struct sockaddr_in mcast_address;
int open_control_socket(char *, char*);
int send_route(struct in_addr, struct in_addr, int, struct sockaddr_in *);

int main(int argc, char **argv) {

	if(argc < 2) {
			printf("Usage: %s <mcast IP> [<local IP>]\n", argv[0]);
			return -1;
	}

	int control_fd = open_control_socket(argv[1], argc==3?argv[2]:NULL);

	if(-1 == control_fd) return -1;

	int err = send_request(control_fd, &mcast_address);

	close(control_fd);

	return !err;
}

int send_request(int s, struct sockaddr_in *to) {
	struct propagate_packet pkt = { .type = 'g', .destination = 0, .gateway = 0 };

	if(-1 == sendto(s, &pkt,
			sizeof(struct propagate_packet), 0,
			(struct sockaddr *)to, sizeof(struct sockaddr_in))) printf("Error sending packet: %s\n", strerror(errno));

	return 1;
}

int open_control_socket(char *mcast_addr, char *local_addr){

	int sock;

	/* Set addresses */
	memset(&mcast_address, 0, sizeof(mcast_address));

	mcast_address.sin_family = AF_INET;
	mcast_address.sin_port = htons(65535);

	if(!inet_aton(mcast_addr, &mcast_address.sin_addr)) {
		printf("Invalid multicast address: %s\n", mcast_addr);
		return -1;
	}

	if(local_addr && !inet_aton(local_addr, &localip)) {
		printf("Invalid local address: %s\n", local_addr);
		return -1;
	}

	/* Create socket */

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		printf("Cannot create socket: %s\nExiting...\n",
		       strerror(errno));
		return -1;
	}

	/* Set multicasting interface */
	if(local_addr && -1 == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
			&localip, sizeof(localip))) {

		printf("Cannot set multicast local address: %s\n",
				strerror(errno));

		close(sock);
		return -1;

	}

	struct ip_mreq mreq = { .imr_multiaddr = mcast_address.sin_addr, .imr_interface = localip};

	/* Set multicasting membership */
#ifdef DEBUG
	printf("I ready to add membership\nAddress of mreq: %x\n mreq.imr_multiaddr: %x\nmreq.imr_interface: %x\n", &mreq, ntohl(mreq.imr_multiaddr.s_addr), ntohl(mreq.imr_interface.s_addr));
#endif
	if(-1 == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			&mreq, sizeof(mreq))) {

		printf("Cannot set multicast membership: %s\n",
				strerror(errno));

		close(sock);
		return -1;

	}

	return sock;
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
