#include <netconfig.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "../../rumprun/src-netbsd/sys/net/if_cnic.h"
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
int rumpns_rtinit(struct ifaddr *, int, int);
int rk_boot_done = 0;

/* HACK FIXME */
extern struct ifnet *rumpns_global_ifp;

static int
set_gw(char *addr)
{
	int rv;

	struct ifaddr *rt;
	struct sockaddr_in *rt_addr;
	struct sockaddr_in *rt_netmask;
	struct in_addr baddr4; /* network byte ip address */

	int netmask = 24;        /* highlevel netmask */
	uint32_t bit_mask;           /* actual bit mask */

	rt = (struct ifaddr *) malloc(sizeof(struct ifaddr));
	assert(rt);

	rt_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
	assert(rt_addr);

	rt_netmask = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
	assert(rt_netmask);

	/* Netmask */
	bit_mask = ~0;
	bit_mask = ~(bit_mask >> netmask);
	bit_mask = htonl(bit_mask); /* Go from host byte order to network byte order */

	memset(rt_netmask, '\0', sizeof(struct sockaddr_in));
	rt_netmask->sin_family = AF_INET;
	rt_netmask->sin_addr.s_addr = bit_mask;
	rt_netmask->sin_len = sizeof(struct sockaddr_in);

	/* Address */
	memset(&baddr4, '\0', sizeof(baddr4));
	rv = inet_pton(AF_INET, addr, &baddr4);
	assert(rv == 1);

	memset(rt_addr, '\0', sizeof(struct sockaddr_in));
	rt_addr->sin_family = AF_INET;
	rt_addr->sin_addr.s_addr = baddr4.s_addr;
	rt_addr->sin_len = sizeof(struct sockaddr_in);

	memset(rt, '\0', sizeof(struct ifaddr));
	rt->ifa_addr = (struct sockaddr *)rt_addr;
	rt->ifa_netmask = (struct sockaddr *)rt_netmask;
	assert(rumpns_global_ifp);
	rt->ifa_ifp = rumpns_global_ifp;

	rv = rumpns_rtinit(rt, RTM_ADD, RTF_UP | RTF_GATEWAY);

	return rv;
}

int
cnic_create(int num, char *addr, char *netmask){

	int rv, fd;
	int i;
	int debug = 1;
	char cnic_dev[11];
	char cnic[6];

	if(!vmid) printf("creating cnic%d for DOM0\n", num);
	else printf("creating cnic%d for VM%d\n", num, vmid);

	sprintf(cnic, "cnic%d", num);
	printf("%s\n", cnic);
	sprintf(cnic_dev, "/dev/cnic%d", num);
	printf("%s\n", cnic_dev);
	printf("addr: %s\n", addr);
	printf("netmask: %s\n", netmask);

	fd = open(cnic_dev, O_RDWR);
	printf("open fd: %d\n", fd);

	rv = ioctl(fd, CNICSDEBUG, &debug);
	if(!rv) printf("cnic%d set debug: success\n", num);
	else printf("cnic%d set debug: fail %d\n", num, rv);

	rv = rump_pub_netconfig_ipv4_ifaddr(cnic, addr, netmask);
	if(!rv) printf("ipv4 set address: success\n");
	else printf("ipv4 set address: fail %d\n", rv);

	/* We assume that this gateway route is for range of [addr, 255.255.255.0] */
	if(vmid == 0) rv = set_gw(addr);
	else rv = rump_pub_netconfig_ipv4_gw(addr);

	if(!rv) printf("ipv4 set gw: success\n");
	else printf("ipv4 set gw: fail %d\n", rv);

	rv = rump_pub_netconfig_ifup(cnic);
	if(!rv) printf("ifup: success\n");
	else printf("ifup: fail %d\n", rv);

	return fd;
}

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

//	if (argc != 4) {
//		printf("Usage: %s <in_port> <dst_port> <msg size>\n", argv[0]);
//		return -1;
//	}
//
//	msg_size = atoi(argv[3]);
	msg = malloc(msg_size);

	soutput.sin_family      = AF_INET;
	//soutput.sin_port        = htons(atoi(argv[2]));
	soutput.sin_port        = htons(OUT_PORT);
//	soutput.sin_addr.s_addr = inet_addr("10.0.1.6");//htonl(INADDR_ANY);//inet_addr(argv[1]);
//	printf("%x\n", (unsigned int)soutput.sin_addr.s_addr);
	soutput.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(argv[1]);
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
//		printf("%x\n", (unsigned int)soutput.sin_addr.s_addr);
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

	//int rv, fd, fd2;
	//int i;
	//int debug = 1;
	//char readbuf[1500];
	//
	///*
	// * Uncomment for paws file system tests
	// * paws_tests();
	// */

	//if(vmid == 0) {
	//	printf("----- dom0 -----\n");
	//        fd = cnic_create(0, "111.111.111.0", "255.255.255.0");
	//	assert(fd);
	//        fd = cnic_create(1, "222.222.222.0", "255.255.255.0");
	//	assert(fd);
	//} else if(vmid == 1){
	//	printf("----- VM%d -----\n", vmid);
	//        fd  = cnic_create(2, "111.111.111.1", "255.255.255.0");
	//	assert(fd);
	//} else if(vmid == 2){
	//	printf("----- VM%d -----\n", vmid);
	//        fd = cnic_create(3, "222.222.222.1", "255.255.255.0");
	//	assert(fd);
	//}

	//printf("Done\n");
	printf("%d: Starting udp-server [in:%d out:%d]\n", vmid, IN_PORT, OUT_PORT);
	__test_udp_server();

	assert(0);

	return 0;
}
