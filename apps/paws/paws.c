#include "include/paws.h"

#include <netconfig.h>

#include <stdio.h>
#include <malloc.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "/home/lucasbaier/research/rump2cos/rumprun/src-netbsd/sys/net/if_cnic.h"

#include <errno.h>

int
main(void)
{

	int rv, fd, fd2;
	int i;
	int debug = 1;
	char readbuf[1500];
	extern int rump_vmid;
	
	printf("vmid: %d\n", rump_vmid);

	printf("Clean verison of paws. Using netconfig interface in buildrump.sh. vmid: %d\n", rump_vmid);
	printf("Should have a real networking device at this point\n");

	printf("Lets make a cnic device...\n");
	
	if(rump_vmid == 0){
		printf("creating dom0\n");
	       	fd = create_dom0();	
		fd2= cnic_create(rump_vmid);
	}else{
		fd = cnic_create(rump_vmid);
	}

	while(1) {
		memset(readbuf, '\0', sizeof(readbuf));
		rv = read(fd, readbuf, sizeof(readbuf));

		//i = 0;
		//while(i != rv) {
		//	printf("%04x ", readbuf[i]);
		//	i++;
		//}
		//printf("\n");

		//printf("\nSending a reply...\n");

		/*
		 * bytes 17 - 20 are new source address
		 * bytes 13 - 16 are new destination address
		 * Modify packet directly and write out to cnic device
		 */
		char writebuf[rv];
		memcpy(writebuf, readbuf, rv);
		writebuf[16] = readbuf[12];
		writebuf[17] = readbuf[13];
		writebuf[18] = readbuf[14];
		writebuf[19] = readbuf[15];

		writebuf[12] = readbuf[16];
		writebuf[13] = readbuf[17];
		writebuf[14] = readbuf[18];
		writebuf[15] = readbuf[19];

		write(fd, writebuf, sizeof(writebuf));
	}

	printf("done\n");

	printf("Spinning...\n");

	while(1);



	return 0;
}

int 
cnic_create(int rump_vmid){
	printf("creating cnic for rk%d. \n", rump_vmid);
	int rv, fd;
	int i;
	int debug = 1;

	fd = open("/dev/cnic1", O_RDWR);
	printf("open fd: %d\n", fd);

	//rv = ioctl(fd, CNICSDEBUG, &debug);
	//printf("Tun Debug: %d\n", rv);

	rv = rump_pub_netconfig_ipv4_ifaddr("cnic1", "111.111.111.0", "255.255.225.0");
	printf("ipv4_ifaddr: %d\n", rv);
	rv = rump_pub_netconfig_ipv4_gw("111.111.111.0");
	
	printf("ipv4_gw: %d\n", rv);
	printf("TODO: Check above function to see what it does\n");

	rv = rump_pub_netconfig_ifup("cnic1");
	printf("ifup: %d\n", rv);

	return fd;
}

int
create_dom0(void){

	int rv, fd;
	int i;
	int debug = 1;
	
	fd = open("/dev/cnic0", O_RDWR);
	printf("open fd: %d\n", fd);

	//rv = ioctl(fd, CNICSDEBUG, &debug);
	//printf("Tun Debug: %d\n", rv);

	rv = rump_pub_netconfig_ipv4_ifaddr("cnic0", "111.111.110.0", "255.255.255.0");
	printf("ipv4_ifaddr: %d\n", rv);
	rv = rump_pub_netconfig_ipv4_gw("111.111.110.0");
	
	printf("ipv4_gw: %d\n", rv);
	printf("TODO: Check above function to see what it does\n");

	rv = rump_pub_netconfig_ifup("cnic0");
	printf("ifup: %d\n", rv);
	
	return fd;	
}








