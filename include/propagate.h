#ifndef _PROPAGATE_H
#define _PROPAGATE_H 1
#include <stdint.h>
#include <propagate_list.h>
#include "../config.h"

struct __attribute__ ((packed)) propagate_packet {
        uint8_t type; /* get - 'g', set - 's', delete - 'd', null - 'n' */
        uint32_t destination; /* IP in network order */
        uint32_t gateway;     /* GW in network order */
};

struct pkt_buffer {
	struct propagate_packet *data;
	int packets_total;     /* buffer size in packets */
	int packets_present;   /* packets in buffer */
	int packets_processed; /* packets processed */
	struct list lh;
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
