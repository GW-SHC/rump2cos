#include "include/paws.h"
#include <netconfig.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "include/if_cnic.h"
#include <errno.h>

extern int vmid;

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

	rv = rump_pub_netconfig_ipv4_gw(addr);
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
	printf("paws.c\n");
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
	        fd  = cnic_create(1, "111.111.111.0", "255.255.255.0");	
	        fd2 = cnic_create(2, "222.222.222.0", "255.255.255.0");	
	} else {
		printf("creating cnic in VM%d\n", vmid);
	        fd  = cnic_create(1, "111.111.111.0", "255.255.255.0");	
	}

	printf("Done\nBlocking lwp thread indefinitly\n");
	bmk_sched_blockprepare();
	bmk_sched_block();
	assert(0);

	return 0;
}
