#include "include/paws.h"

/* We use weak symbols to use the RK provided gcc script to get the bake to work.. It is a pain in the @&% */

#pragma weak rump_init
#pragma weak rump_pub_etfs_register

int
main(void)
{
        int rv;

        rv = rump_init();

        if (rv)
                printf("rump_init failed\n");


        //rv = rump_pub_etfs_register("/dev/paws", "paws", RUMP_ETFS_BLK);

        //if (rv)
        //        printf("rump_pub_etfs_register failed\n");


        paws_tests();

	while(1) {
		sleep(10);
	}

        return 0;
}
