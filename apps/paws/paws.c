#include "include/paws.h"

int
main()
{
        int rv;

        rv = rump_init();

        if (rv)
                printf("rump_init failed\n");

        rv = rump_pub_etfs_register("/dev/paws", "paws", RUMP_ETFS_BLK);

        if (rv)
                printf("rump_pub_etfs_register failed\n");


        paws_tests();

        return 0;
}
