#include "include/paws.h"
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

extern int vmid;
int
main(void)
{
	/*
	 * Uncomment for paws file system tests
	 */
	//paws_tests();

	printf("%d: Blocking lwp thread indefinitly\n", vmid);
	bmk_sched_blockprepare();
	bmk_sched_block();
	assert(0);

	return 0;
}
