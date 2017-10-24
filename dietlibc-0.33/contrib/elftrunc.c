#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <elf.h>

void die (int v,const char *s);
void die (int v,const char *s)
{
  write(2,s,strlen(s)); write(2,"\n",1);
  exit(v);
}

char fn[1024];
unsigned char buf[4096];

void trunc32(int fd);
void trunc32(int fd)
{
  Elf32_Ehdr eh32;
  Elf32_Phdr ph32;

  int in=fd, out, i;

  Elf32_Addr len,n;

  len=0;

  if ((n=read(in,&eh32,sizeof(eh32)))<1) die(3,"read elf hdr");

  lseek(in,(off_t)eh32.e_phoff,SEEK_SET);

  for (i=0; (i<eh32.e_phnum) && (read(in,&ph32,sizeof(ph32))>0) ; i++)
  {
    switch (ph32.p_type)
    {
    case PT_DYNAMIC:
    case PT_INTERP:
      die(3,"File is not static !");
      break;
    case PT_LOAD:
      if (len<(ph32.p_offset+ph32.p_filesz)) len=(ph32.p_offset+ph32.p_filesz);
      break;
    default:
      break;
    }
  }

  lseek(in,(off_t)n,SEEK_SET);
  len -= n;

  eh32.e_shoff=0;
  eh32.e_shnum=0;
  eh32.e_shstrndx=0;

  if ((out=open(fn,O_CREAT| O_TRUNC|O_WRONLY,0755))<0) die(2,"open outfile");

  write(out,&eh32,n);

  while(len && n)
  {
    if ((n=read(in,buf,sizeof(buf))))
    {
      write(out,buf,(len<n)?len:n);
      len-=(len<n)?len:n;
    }
  }

  close(out);
}

void trunc64(int fd);
void trunc64(int fd)
{
  Elf64_Ehdr eh64;
  Elf64_Phdr ph64;

  int in=fd, out, n, i;
  Elf64_Addr len;

  len=0;

  if ((n=read(in,&eh64,sizeof(eh64)))<1) die(3,"read elf hdr");

  lseek(in,(off_t)eh64.e_phoff,SEEK_SET);

  for (i=0; (i<eh64.e_phnum)&&(read(in,&ph64,sizeof(ph64))>0); i++)
  {
    switch (ph64.p_type)
    {
    case PT_DYNAMIC:
    case PT_INTERP:
      die(3,"File is not static !");
      break;
    case PT_LOAD:
      if (len<(ph64.p_offset+ph64.p_filesz)) len=(ph64.p_offset+ph64.p_filesz);
      break;
    default:
      break;
    }
  }

  lseek(in,n,SEEK_SET);
  len -= n;

  eh64.e_shoff=0;
  eh64.e_shnum=0;
  eh64.e_shstrndx=0;

  if ((out=open(fn,O_CREAT| O_TRUNC|O_WRONLY,0755))<0) die(2,"open outfile");

  write(out,&eh64,(size_t)n);

  while(len && n>0)
  {
    if ((n=read(in,buf,sizeof(buf)))>0)
    {
      write(out,buf,(size_t)((len<(size_t)n)?len:(size_t)n));
      len-=(len<(size_t)n)?len:(size_t)n;
    } else die(2,"read error");
  }

  close(out);
}

int main(int argc, char *argv[])
{
  long n;
  int in;

  if (argc!=2 && argc!=3) die(0,"usage: elftrunc srcprogname [dstprogname]");

  if ((in=open (argv[1],O_RDONLY))<0) die(1,"open input file");
  if ((n=read(in,buf,EI_NIDENT))<1) die(2,"read header of input");
  buf[n]=0;

  if (memcmp(buf,ELFMAG,SELFMAG)) die(2,"input file isn't an ELF !");

  lseek(in,0,SEEK_SET);

  strncpy(fn,argv[1],sizeof(fn)-10);
  strcat(fn,".trunc");

  if (buf[EI_CLASS]==ELFCLASS64)
    trunc64(in);
  else
    trunc32(in);

  close(in);
  if (argc==3)
      rename(fn,argv[2]);
  return 0;
}


