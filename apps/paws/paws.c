#include "include/paws.h"
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
extern int vmid;

int rumpns_rtinit(struct ifaddr *, int, int);

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


int
main(void)
{

	int rv, fd, fd2;
	int i;
	int debug = 1;
	char readbuf[1500];
	
	/*
	 * Uncomment for paws file system tests
	 * paws_tests();
	 */

	if(vmid == 0) {
		printf("----- dom0 -----\n");
	        fd = cnic_create(0, "111.111.111.0", "255.255.255.0");	
		assert(fd);
	        fd = cnic_create(1, "222.222.222.0", "255.255.255.0");	
		assert(fd);
	} else if(vmid == 1){
		printf("----- VM%d -----\n", vmid);
	        fd  = cnic_create(2, "111.111.111.1", "255.255.255.0");	
		assert(fd);
	} else if(vmid == 2){
		printf("----- VM%d -----\n", vmid);
	        fd = cnic_create(3, "222.222.222.1", "255.255.255.0");	
		assert(fd);
	}

	printf("Done\nBlocking lwp thread indefinitly\n");
	bmk_sched_blockprepare();
	bmk_sched_block();
	assert(0);

	return 0;
}
