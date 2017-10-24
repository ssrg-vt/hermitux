#define _POSIX_SOURCE
#include "dietdirent.h"
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

struct linux_dirent {
  long		d_ino;
  off_t		d_off;
  uint16_t	d_reclen;
  char		d_name[1];
};

int readdir_r(DIR *d,struct dirent* entry, struct dirent** result) {
  struct linux_dirent* ld;
  *result=0;
  if (!d->num || (d->cur += ((struct dirent*)(d->buf+d->cur))->d_reclen)>=d->num) {
    int res=getdents(d->fd,(struct dirent*)d->buf,sizeof (d->buf)-1);
    if (res<=0)
      return res<0;
    d->num=res; d->cur=0;
  }
  ld=(struct linux_dirent*)(d->buf+d->cur);
  if (ld->d_reclen < sizeof(struct linux_dirent))
    return 0;	/* can't happen */
  *result=entry;
  entry->d_ino=ld->d_ino;
  entry->d_off=ld->d_off;
  entry->d_reclen=ld->d_reclen;
  entry->d_type=ld->d_name[ld->d_reclen-offsetof(struct linux_dirent,d_name)-1];
  memcpy(entry->d_name,ld->d_name,ld->d_reclen-offsetof(struct linux_dirent,d_name));
  return 0;
}
