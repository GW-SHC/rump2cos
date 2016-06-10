#include "include/paws.h"

#include <stdio.h>
#include <malloc.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>

#include <net/if_tun.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#ifndef SA_SIZE
# define SA_SIZE(sa)                        \
	(  (!(sa) || ((struct sockaddr *)(sa))->sa_len == 0) ?  \
	   sizeof(long)     :               \
	   1 + ( (((struct sockaddr *)(sa))->sa_len - 1) | (sizeof(long) - 1) ) )
#endif

void
route(struct in_addr *dst)
{
	int s, rv;
	char buf[1024];

	struct rt_msghdr *rtm = (struct rt_msghdr *)buf;

	printf("\nAdding to routing table...\n");

	s = socket(AF_ROUTE, SOCK_RAW, 0);
	printf("fd for route socket: %d\n", s);

	memset(rtm, 0, sizeof(struct rt_msghdr));
	rtm->rtm_msglen = sizeof(struct rt_msghdr);
	rtm->rtm_type   = RTM_ADD;
	rtm->rtm_errno  = 0;

	/* Set destination address */
	//rtm->rtm_dst.sin_family = AF_INET;
	//rtm->rtm_dst.sin_addr.s_addr = dst->s_addr;

	/* Use interface #1 as outgoing interface */
	rtm->rtm_index = 1;

	rv = write(s, (caddr_t)rtm, rtm->rtm_msglen);
	printf("rv from route write call: %d\n", rv);

	printf("Done\n");

	return;
}

int
main(void)
{
	/*
	 * Shit,
	 * This seems overly complicated to me :/
	 * But the ioctl god seems to be appeased by this setup
	 */

	int fd0, fd1, sock0, sock1, rv; /* file descriptor for tun0 and socket descriptor for the end point of tun0 */
	int  forwarding, allowsrcrt, forwsrcrt, redirect;
	int mib[5];
	size_t len;
	int debug = 1;

	char writebuf[] = "hello worldhello worldhello worldhello worldhello worldhello worldhello worldhello world\0";
	char readbuf[2000];

	char *string_addr_tun0 = "10.0.0.1";
	struct in_addr baddr4_tun0;  /* network byte ip address */
	struct sockaddr_in addr_tun0; /* address struct that will be placed in socket */
	char *string_addr_tun1 = "10.0.0.2";
	struct in_addr baddr4_tun1;  /* network byte ip address */
	struct sockaddr_in addr_tun1; /* address struct that will be placed in socket */

	int port0 = 1110;
	int port1 = port0;

	struct ifreq ifr_tun0; /* Interface used to configure tun0 */
	struct ifaliasreq ifa_tun0; /* Updated version of ifr_tun0 */
	struct ifreq ifr_tun1; /* Interface used to configure tun1 */
	struct ifaliasreq ifa_tun1; /* Updated version of ifr_tun1 */

	int netmask = 24;        /* highlevel netmask */
	uint32_t bit_mask;           /* actual bit mask */
	struct sockaddr_in mask; /* mask struct that will be placed in socket */

	/* Variables used in routing table */
	struct rt_msghdr *rtm;
	char *routingtable, *lim, *next;
	char line[500];
	struct sockaddr *sa;
	struct sockaddr_in *sockin;

	printf("Kernel options...\n");

	len = sizeof(forwarding);
	rv = sysctlbyname("net.inet.ip.forwarding", &forwarding, &len, NULL, 0);
	printf("rv from sysctlbyname: %d\n", rv);
	printf("current value of forwarding: %d\n", forwarding);

	len = sizeof(redirect);
	rv = sysctlbyname("net.inet.ip.redirect", &redirect, &len, NULL, 0);
	printf("rv from sysctlbyname: %d\n", rv);
	printf("current value of redirect: %d\n", redirect);

	len = sizeof(allowsrcrt);
	rv = sysctlbyname("net.inet.ip.allowsrcrt", &allowsrcrt, &len, NULL, 0);
	printf("rv from sysctlbyname: %d\n", rv);
	printf("current value of allowsrcrt: %d\n", allowsrcrt);

	len = sizeof(forwsrcrt);
	rv = sysctlbyname("net.inet.ip.forwsrcrt", &forwsrcrt, &len, NULL, 0);
	printf("rv from sysctlbyname: %d\n", rv);
	printf("current value of forwsrcrt: %d\n", forwsrcrt);

	printf("Done\n");


	/* Lets get some routing tables! */
	printf("\nFetching routing tables...\n");

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = 0;
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;

	rv = sysctl(mib, 6, NULL, &len, NULL, 0);
	printf("len: %d\n", len);
	printf("rv from sysctl: %d\n", rv);
	printf("malloc space needed for routing table...\n");
	routingtable = malloc(len);
	assert(routingtable != NULL);
	printf("Done\n");
	rv = sysctl(mib, 6, routingtable, &len, NULL, 0);
	printf("rv from sysctl: %d\n", rv);

	/* Print routing table now and again at then end of configuration */
	lim  = routingtable + len;
	for (next = routingtable; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		sa = (struct sockaddr *)(rtm + 1);
		sa = (struct sockaddr *)(SA_SIZE(sa) + (char *)sa);
		sockin = (struct sockaddr_in *)sa;
		inet_ntop(AF_INET, &sockin->sin_addr.s_addr, line, sizeof(line) - 1);
		printf("%s\n", line);
	}

	printf("Done\n");


	/* FOR TUN0 */


	printf("\nConfiguring tun0 and tun1...\n");

	fd0 = open("/dev/tun0", O_RDWR);
	assert(fd0 != -1);
	printf("fd0 of tun0: %d\n", fd0);

	fd1 = open("/dev/tun1", O_RDWR);
	assert(fd1 != -1);
	printf("fd1 of tun1: %d\n", fd1);

	/* Set debugging mode for more imformation */
	printf("Set debugging mode for more imformation\n");
	rv = ioctl(fd0, TUNSDEBUG, &debug);
	assert(!rv);
	rv = ioctl(fd1, TUNSDEBUG, &debug);
	assert(!rv);

	/* Initialize socket to configure tun device with */
	printf("Initialize sockets to configure tun devices\n");
	sock0 = socket(AF_INET, SOCK_DGRAM, 0);
	assert(sock0 != -1);
	sock1 = socket(AF_INET, SOCK_DGRAM, 0);
	assert(sock1 != -1);

	/* Set the interface name */
	printf("Set the interface name \"tun0\" and \"tun1\"\n");
	memset(&ifr_tun0, '\0', sizeof(ifr_tun0));
	snprintf(ifr_tun0.ifr_name, sizeof(ifr_tun0.ifr_name), "tun0");
	memset(&ifa_tun0, '\0', sizeof(ifa_tun0));
	strlcpy(ifa_tun0.ifra_name, "tun0", sizeof(ifa_tun0.ifra_name));
	memset(&ifr_tun1, '\0', sizeof(ifr_tun1));
	snprintf(ifr_tun1.ifr_name, sizeof(ifr_tun1.ifr_name), "tun1");
	memset(&ifa_tun1, '\0', sizeof(ifa_tun1));
	strlcpy(ifa_tun1.ifra_name, "tun1", sizeof(ifa_tun1.ifra_name));

	/* Set mask from Netmask */
	printf("Set bit mask from Netmask\n");
	bit_mask = ~0;
	bit_mask = ~(bit_mask >> netmask);
	bit_mask = htonl(bit_mask); /* Go from host byte order to network byte order */

	/* Change string ip address to network byte ip address */
	printf("Change string ip address to network byte ip address\n");
	memset(&baddr4_tun0, '\0', sizeof(baddr4_tun0));
	rv = inet_pton(AF_INET, string_addr_tun0, &baddr4_tun0);
	assert(rv == 1);
	memset(&baddr4_tun1, '\0', sizeof(baddr4_tun1));
	rv = inet_pton(AF_INET, string_addr_tun1, &baddr4_tun1);
	assert(rv == 1);

	/* Assign destination address and netmask to proper structs */
	printf("Setting ip address, port and  netmask\n");
	memset(&addr_tun0, '\0', sizeof(addr_tun0));
	addr_tun0.sin_family = AF_INET;
	addr_tun0.sin_addr.s_addr = baddr4_tun0.s_addr;
	addr_tun0.sin_len = sizeof(addr_tun0);
	memcpy(&ifa_tun0.ifra_addr, &addr_tun0, sizeof(addr_tun0));

	memset(&addr_tun1, '\0', sizeof(addr_tun1));
	addr_tun1.sin_family = AF_INET;
	addr_tun1.sin_addr.s_addr = baddr4_tun1.s_addr;
	addr_tun1.sin_len = sizeof(addr_tun1);
	memcpy(&ifa_tun1.ifra_addr, &addr_tun1, sizeof(addr_tun1));

	memcpy(&ifa_tun0.ifra_dstaddr, &addr_tun0, sizeof(addr_tun0)); /* Set the destination address. tun1 */
	memcpy(&ifa_tun1.ifra_dstaddr, &addr_tun1, sizeof(addr_tun1)); /* Set the destination address. tun0 */

	/* Now do the masks  */
	memset(&mask, '\0', sizeof(mask));
	mask.sin_family = AF_INET;
	mask.sin_addr.s_addr = bit_mask;
	mask.sin_len = sizeof(mask);
	memcpy(&ifa_tun0.ifra_mask, &mask, sizeof(ifa_tun0.ifra_mask));
	memcpy(&ifa_tun1.ifra_mask, &mask, sizeof(ifa_tun1.ifra_mask));

	/* Now do the ports */
	addr_tun0.sin_port = htons(port0);
	addr_tun1.sin_port = htons(port1);

	rv = ioctl(sock0, SIOCAIFADDR, &ifa_tun0);
	assert(rv != -1);
	rv = ioctl(sock1, SIOCAIFADDR, &ifa_tun1);
	assert(rv != -1);


	printf("Setting the socket interface to UP\n");
	ifr_tun0.ifr_flags |= IFF_UP;
	rv = ioctl(sock0, SIOCSIFFLAGS, &ifr_tun0);
	assert(rv != -1);
	ifr_tun1.ifr_flags |= IFF_UP;
	rv = ioctl(sock1, SIOCSIFFLAGS, &ifr_tun1);
	assert(rv != -1);

	/* Try to add routes */
	//route(baddr4_tun0);

	printf("Turn off blocking io\n");
	rv = ioctl(fd0, FIONBIO, 1);
	assert(rv != -1);
	rv = ioctl(fd1, FIONBIO, 1);
	assert(rv != -1);

	printf("Done with configuration\n");

	/* READ AND WRITE TESTS */

	printf("\nAttempting to write to networking stack and read from tun0...\n");

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	int sock_serv = socket(AF_INET, SOCK_DGRAM, 0);
	rv = connect(sock, (struct sockaddr *)&addr_tun0, sizeof(addr_tun0));
	printf("return value from connect: %d\n", rv);
	rv = send(sock, writebuf, sizeof(writebuf), 0);
	printf("send completed with %d bytes written\n", rv);

	printf("done\n");

	printf("\nAttempting to read from tun0 to get ip packet...\n");

	int n, i = 0;
	memset(readbuf, '\0', sizeof(readbuf));
	ioctl(fd0, FIONREAD, &n);
	printf("Size of next packet to read: %d\n", n);
	rv = read(fd0, readbuf, n);
	printf("Read completed with %d bytes read\n", rv);

	while(i != n) {
		putchar((char)readbuf[i]);
		i++;
	}

	printf("\ndone\n");

	printf("\nAttempting to write to tun0 and tun1 with ip packet destined to tun0...\n");

	rv = write(fd0, readbuf, n); /* We only want to write the number of bytes that we read in */
	printf("write completed with %d bytes written\n", rv);
	rv = write(fd1, readbuf, n); /* We only want to write the number of bytes that we read in */
	printf("write completed with %d bytes written\n", rv);

	printf("done\n");

	printf("\nAttempting to read from tun0 to get ip packet...\n");

	memset(readbuf, '\0', sizeof(readbuf));
	ioctl(fd0, FIONREAD, &n);
	printf("Size of next packet to read: %d\n", n);
	rv = read(fd0, readbuf, sizeof(readbuf));
	printf("Read completed with %d bytes read\n", rv);

	printf("done\n");

	close(fd0);
	close(fd1);

	//paws_tests();

	return 0;
}
