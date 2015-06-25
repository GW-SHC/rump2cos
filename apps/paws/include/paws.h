#ifndef PAWS_H
#define PAWS_H

#include <sys/mount.h>
#include <ufs/ufs/ufsmount.h>
#include <sys/param.h>

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

int
mount_paws(const char *type, const char *dir, int flags, void *data, size_t data_len);

int
unmount_paws(const char *dir, int flags);

int
paws_tests(void);

#endif /* PAWS_H */
