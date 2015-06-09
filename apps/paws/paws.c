#include <rump/rump_syscalls.h>
#include <rump/rump.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

//#include <rump_vfs_private.h>

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

  /* Turns out that to see paws in some way we need to have rump_vfs_makeonedevnode
   * It is as if, etfsregister was not designed to show the registered device...
   *  However, accessing the etfsregistered device still results in a valid fd being returned
   * Unfortunatly, if the etfsregistered device's key, eg: /paws, is set to be the same thing
   * as makeonedevnode eg: /dev/paws, then the device will not show up in the list, for what I believe
   * to be because it has already found that device to exist there. *even though it does not show*.
   *
   * It seems that the function I am using here then, is used then not to simply display the name of the device
   * but also to register it. This would lead me to believe that I should use a some inner function to simply
   * add my paws to the list of /dev devices, but as I now know I can still acess the propper etfsregistered
   * device... I no longer care and will simply put the etfsregistered device in /paws while keeping the
   * makeonedevnode device within /dev/paws. I will then simply be sure to open just /dev */

  //rv = rump_vfs_makeonedevnode(8192, "/dev/paws", 2, 2); // The 8192 is device type
  // The 2's are Major and Minor nums respectivly
  //if(rv)
  //  printf("rump_vfs_makeonedevnode failed\n");

  char reader[524288];
  char writer[] = "hello hobbes";
  char writer2[] = " back for more";

  int fd = open("/paws", O_RDWR);
  assert(fd != -1);
  rv = write(fd, writer, strlen(writer));
  assert(rv > 0);
  rv = write(fd, writer2, strlen(writer2));
  assert(rv > 0);
  lseek(fd, 0, SEEK_SET);
  rv = read(fd, reader, strlen(writer)+strlen(writer2));
  assert(rv > 0);
  close(fd);

  printf("%s\n", reader);

  //printdirs("/dev");
  //printdirs("/tmp");

  return 0;
}

void
printdirs(char dir[])
{
  DIR *dip = opendir(dir);
  struct dirent *dp;
  while((dp = readdir(dip)) != NULL)
  {
    sleep(1);
    printf("Current directory %s/%s\n", dir, dp->d_name);
  }
}
