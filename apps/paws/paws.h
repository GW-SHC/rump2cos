#ifndef PAWS_H
#define PAWS_H

#include <sys/mount.h>
#include <ufs/ufs/ufsmount.h>
#include <sys/param.h>


int mount_paws(const char *type, const char *dir, int flags, void *data, size_t data_len);
int unmount_paws(const char *dir, int flags);
#endif

