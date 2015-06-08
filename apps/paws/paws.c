#include <rump/rump_syscalls.h>
#include <rump/rump.h>

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

int
main()
{
  int rv;

  rv = rump_init();

  if (rv)
    printf("rump_init failed\n");

  rv = rump_pub_etfs_register("/paws", "paws", RUMP_ETFS_BLK);

  if (rv)
    printf("rump_pub_etfs_register failed\n");


  DIR *dip = opendir("/dev");
  struct dirent *dp;
  while((dp = readdir(dip)) != NULL)
  {
    sleep(1);
    printf("Current directory /dev/%s\n", dp->d_name);
  }


  dip = opendir(".");
  while((dp = readdir(dip)) != NULL)
  {
    sleep(1);
    printf("Current directory %s\n", dp->d_name);
  }

	return 0;
}
