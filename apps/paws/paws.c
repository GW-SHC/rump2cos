#include <rump/rump_syscalls.h>
#include <rump/rump.h>

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "paws.h"

//#include <rump_vfs_private.h>


void
printdirs(char dir[])
{
  DIR *dip = opendir(dir);
  struct dirent *dp;
  while((dp = readdir(dip)) != NULL)
  {
    sleep(2);
    printf("Current directory %s/%s\n", dir, dp->d_name);
  }
}

int
testmkdir()
{
  int rv = -1;

  if(mkdir("/mnt", 777) == 0){
      printf("directory /mnt created\n");
      rv = 0;
  }

  return rv;
}

int
testmount()
{
  // Testing mounting with ext2 mount type

  struct ufs_args args;
  struct stat sb;
  int fd, rv;
  size_t size;
  char reader[200];
  char readerfirst[100];
  char writer[] = "abcdabcdabcabcdabcabcdabcdabcdabcabcdabcdabcdabcabcdabcabcdabcabcdabcabcdabcabcdabcddddddddd\n";
  char writer2[] = "efgefgefgefgefgefefefefefgefgefgefgefefefefgefefggefefgefefefgefgefefgefgefgefgefggefgefggggggggggg\n";

  fd = open("/dev/paws", O_RDWR);
  assert(fd != -1);

  rv = fstat(fd, &sb);
  assert(rv == 0);

  close(fd);

  size = (size_t)sb.st_size;
  args.fspec = "/dev/paws";
  rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
  assert(rv != -1);

  // The backing image for paws already has some directories and a file that were placed in the host directory
  // We can see those here and read the file...
  printf("\nfirst print:\n");
  printdirs("/mnt");
//  fd = open("/mnt/hobbes/hobbesfile", O_RDONLY);
//  assert(fd > -1);
//  rv = read(fd, readerfirst, sizeof(readerfirst)); // I forget how long the message is, so just read in a large amount
//  printf("%s\n", readerfirst);
//  sleep(5);

  rv = unmount_paws("/mnt", MNT_FORCE);
  assert(rv != -1);

  // Mount 2nd time to write a few files....
  rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
  assert(rv != -1);

  printf("\nMaking files...\n");

  fd = open("/mnt/file", O_RDWR | O_APPEND | O_CREAT);
  assert(fd > -1);
  fd = open("/mnt/file1", O_RDWR | O_APPEND | O_CREAT);
  assert(fd > -1);
  fd = open("/mnt/file2", O_RDWR | O_APPEND | O_CREAT);
  assert(fd > -1);

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

  // Writing to file 1
  fd = open("/mnt/file1", O_RDWR | O_APPEND);
  assert(fd > -1);
  printf("Writting to file1: ");
  rv = write(fd, writer, strlen(writer));
  assert(rv > 0);
  rv = write(fd, writer2, strlen(writer2));
  assert(rv > 0);
  close(fd);
  printf("done\n");

  rv = unmount_paws("/mnt", MNT_FORCE);
  assert(rv != -1);

  // Remounting so we can read from file1 to check to see the changes are still there
  rv = mount_paws(MOUNT_EXT2FS, "/mnt", 0, &args, sizeof(args));
  assert(rv != -1);

  fd = open("/mnt/file1", O_RDWR | O_APPEND);
  assert(fd > -1);

  printf("\nReading from file 1:\n");
  rv = read(fd, reader, strlen(writer)+strlen(writer2));
  assert(rv > 0);

  close(fd);

  printf("%s\n", reader);

  return 0;
}

int
testreadnwrite()
{
  char reader[50];
  char writer[] = "hello hobbes";
  char writer2[] = " back for more\n";
  int fd, rv;

  // Testing read and write

  fd = open("/dev/paws", O_RDWR);
  assert(fd != -1);

  printf("fd for /dev/paws: %d\n", fd);

  rv = write(fd, writer, strlen(writer));
  assert(rv > 0);

  rv = write(fd, writer2, strlen(writer2));
  assert(rv > 0);

  lseek(fd, 0, SEEK_SET);

  rv = read(fd, reader, strlen(writer)+strlen(writer2));
  assert(rv > 0);

  close(fd);

  printf("%s\n", reader);

  return 0;
}

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

  //testreadnwrite();
  testmkdir();
  testmount();

  return 0;
}
