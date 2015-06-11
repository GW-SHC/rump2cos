#include "paws.h"

#include <sys/param.h>
#include <sys/mount.h>

int
mount_paws(const char *type, const char *dir, int flags, void *data, size_t data_len)
{
  int rv;
  rv = mount(type, dir, flags, data, data_len);
  return rv;
}

int
unmount_paws(const char *dir, int flags)
{
  int rv;
  rv = unmount(dir, flags);
  return rv;
}
