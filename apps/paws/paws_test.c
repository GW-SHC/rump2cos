#include "include/paws.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void
printdirs(char dir[])
{
        DIR *dip = opendir(dir);
        struct dirent *dp;
	int close_ret;

        while((dp = readdir(dip)) != NULL) {
		 //sleep(1);

                printf("Current directory %s/%s\n", dir, dp->d_name);
        }

	close_ret = closedir(dip);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);
}

static int
testmkdir(void)
{
        int rv = -1;

        if(mkdir("/mnt", 777) == 0){
                printf("directory /mnt created\n");
                rv = 0;
        }

        return rv;
}

/* mounting with ext2 mount type */
static int
testmount(void)
{

        struct ufs_args args;
        struct stat sb;
        int fd, rv, close_ret;
        size_t size;
        char reader[200];
        char readerfirst[100];
        char writer[] = "abcdabcdabcabcdabcabcdabcdabcdabcabcdabcdabcdabcabcdabcabcdabcabcdabcabcdabcabcdabcddddddddd\n";
        char writer2[] = "efgefgefgefgefgefefefefefgefgefgefgefefefefgefefggefefgefefefgefgefefgefgefgefgefggefgefggggggggggg\n";

	printf("Printing /\n");
	printdirs("/");
	printf("\n");

	printf("Printing /dev\n");
	printdirs("/dev");
	printf("\n");

        fd = open("/dev/paws", O_RDWR);
        assert(fd != -1);

	printf("fd for /dev/paws is: %d\n", fd);

        rv = fstat(fd, &sb);
        assert(rv == 0);

	printf("fstat info: st_dev %d\n", (int)(sb.st_dev));
	printf("fstat info: st_size %d\n", (int)(sb.st_size));

        close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);

        size = (size_t)sb.st_size;
        args.fspec = "/dev/paws";
        rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
        assert(rv != -1);

        /* The backing image for paws already has some directories and a file
	 * that were placed in the host directory
	 */
        printf("\nfirst print:\n");
        printdirs("/mnt");

        rv = unmount_paws("/mnt", MNT_FORCE);
        assert(rv != -1);

        /* Mount 2nd time to write a few files... */
        rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
        assert(rv != -1);

        printf("\nMaking files...\n");

        fd = open("/mnt/file", O_RDWR | O_APPEND | O_CREAT);
        assert(fd > -1);
	close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);
        fd = open("/mnt/file1", O_RDWR | O_APPEND | O_CREAT);
        assert(fd > -1);
	close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);
        fd = open("/mnt/file2", O_RDWR | O_APPEND | O_CREAT);
        assert(fd > -1);
	close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);

        printf("\nSecond print: \n");
        printdirs("/mnt");

        rv = unmount_paws("/mnt", MNT_FORCE);
        assert(rv != -1);

        printf("\nUnmounted, reprinting /mnt\n");
        printdirs("/mnt");

        printf("\nRemounting and printing /mnt\n");
        rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
        assert(rv != -1);
        printf("\nThird print:\n");
        printdirs("/mnt");

        /* Writing to file 1 */
        fd = open("/mnt/file1", O_RDWR | O_APPEND);
        assert(fd > -1);
        printf("Writting to file1: \n");
        rv = write(fd, writer, strlen(writer));
        assert(rv > 0);
        rv = write(fd, writer2, strlen(writer2));
        assert(rv > 0);
        close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);
        printf("done\n");

        rv = unmount_paws("/mnt", MNT_FORCE);
        assert(rv != -1);

        /* Remounting so we can read from file1 to check to see the changes are still there */
        rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
        assert(rv != -1);

        fd = open("/mnt/file1", O_RDWR | O_APPEND);
        assert(fd > -1);

        printf("\nReading from file 1:\n");
        rv = read(fd, reader, strlen(writer)+strlen(writer2));
        assert(rv > 0);

        close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);

        printf("%s\n", reader);

        return 0;
}

static int
testreadwrite(void)
{
        char reader[50];
        char writer[] = "hello hobbes";
        char writer2[] = " back for more\n";
        int fd, rv, close_ret;

        fd = open("/dev/paws", O_RDWR);
	if (fd == -1) printf("! errno: %d !\n ", errno);
        assert(fd != -1);

        printf("fd for /dev/paws: %d\n", fd);

        rv = write(fd, writer, strlen(writer));
        assert(rv > 0);

        rv = write(fd, writer2, strlen(writer2));
        assert(rv > 0);

        lseek(fd, 0, SEEK_SET);

        rv = read(fd, reader, strlen(writer)+strlen(writer2));
        assert(rv > 0);

        close_ret = close(fd);
	printf("close_ret: %d, %d\n", close_ret, __LINE__);

        printf("%s\n", reader);

        return 0;
}

void
paws_ifconfig(void)
{
	struct ifaddrs * addrs;
	struct ifaddrs * tmp;
	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
		{
			struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
			printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
		}

		tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
}

extern int booting;
extern rump_vmid;

int
paws_tests(void)
{
        int rv;

	printf("\nTesting read and writes\n");
        rv = testreadwrite();
	assert(rv == 0);

	printf("\nTesting mkdir\n");
        rv = testmkdir();
        assert(rv == 0);

	printf("\nTesting mount\n");
        rv = testmount();
        assert(rv == 0);

	printf("\nTesting ifconfig\n");
	paws_ifconfig();

        return 0;
}
