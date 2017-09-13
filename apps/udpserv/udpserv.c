#include <netconfig.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netconfig.h>
#include <errno.h>
#include <fcntl.h>
#include "include/udpserv.h"

struct cos_rumpcalls;
extern struct cos_rumpcalls crcalls;
extern int vmid;
extern int cycs_per_usec;
#define __rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

#define IN_PORT  9998
#define OUT_PORT 9999
#define MSG_SZ   16
#define TP_INFO_MS (unsigned long long)(5*1000)

unsigned long long prev = 0, now = 0;
unsigned long msg_count = 0;

int
__test_udp_server(void)
{
	int fd, fdr;
	struct sockaddr_in soutput, sinput;
	int msg_size=MSG_SZ;
	char *msg;
	int tp_counter = 0;

	msg = malloc(msg_size);

	soutput.sin_family      = AF_INET;
	soutput.sin_port        = htons(OUT_PORT);
//	printf("%x\n", (unsigned int)soutput.sin_addr.s_addr);
	soutput.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Sending to port %d\n", OUT_PORT);
	if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Establishing socket");
		return -1;
	}
	if ((fdr = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Establishing receive socket");
		return -1;
	}

	sinput.sin_family      = AF_INET;
	sinput.sin_port        = htons(IN_PORT);
	sinput.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("binding receive socket to port %d\n", IN_PORT);
	if (bind(fdr, (struct sockaddr *)&sinput, sizeof(sinput))) {
		perror("binding receive socket");
		return -1;
	}

	__rdtscll(now);
	prev = now;

	while (1) {
		struct sockaddr sa;
		socklen_t len;

		if (recvfrom(fdr, msg, msg_size, 0, &sa, &len) != msg_size) {
			perror("read");
			continue;
		}
		//printf("Received-msg: seqno:%u time:%llu\n", ((unsigned int *)msg)[0], ((unsigned long long *)msg)[1]);
		/* Reply to the sender */
		soutput.sin_addr.s_addr = ((struct sockaddr_in*)&sa)->sin_addr.s_addr;
		if (sendto(fd, msg, msg_size, 0, (struct sockaddr*)&soutput, sizeof(soutput)) < 0) {
			perror("sendto");
			continue;
		}
		//printf("Sent-msg: seqno:%u time:%llu\n", ((unsigned int *)msg)[0], ((unsigned long long *)msg)[1]);

		msg_count ++;
		__rdtscll(now);
	//	printf("now:%llu prev:%llu %llu, cycs_per_msec:%d\n", now, prev, now - prev, cycs_per_msec);
		if ((now - prev) >= ((unsigned long long)cycs_per_usec * TP_INFO_MS * 1000)) {
			printf("%d:Msgs processed:%lu, last seqno:%u\n", tp_counter++, msg_count, ((unsigned int *)msg)[0]);
			msg_count = 0;
			prev = now;
		}
	}

	return -1;
}

int
main(void)
{
	printf("%d: Starting udp-server [in:%d out:%d]\n", vmid, IN_PORT, OUT_PORT);
	__test_udp_server();

	assert(0);

	return 0;
}
