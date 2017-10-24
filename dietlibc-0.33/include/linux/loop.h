#ifndef _LINUX_LOOP_H
#define _LINUX_LOOP_H

#include <sys/cdefs.h>
#include <inttypes.h>

__BEGIN_DECLS

/* stolen form kernel */

#define LO_NAME_SIZE	64
#define LO_KEY_SIZE	32


/*
 * Loop flags
 */
enum {
	LO_FLAGS_READ_ONLY	= 1,
	LO_FLAGS_AUTOCLEAR	= 4,
	LO_FLAGS_PARTSCAN	= 8,
};

struct loop_info {
  int		lo_number;	/* ioctl r/o */
  dev_t		lo_device;	/* ioctl r/o */
  unsigned long	lo_inode;	/* ioctl r/o */
  dev_t		lo_rdevice;	/* ioctl r/o */
  int		lo_offset;
  int		lo_encrypt_type;
  int		lo_encrypt_key_size;	/* ioctl w/o */
  int		lo_flags;	/* ioctl r/o */
  char		lo_name[LO_NAME_SIZE];
  unsigned char	lo_encrypt_key[LO_KEY_SIZE]; /* ioctl w/o */
  unsigned long	lo_init[2];
  char		reserved[4];
};

struct loop_info64 {
  uint64_t		   lo_device;			/* ioctl r/o */
  uint64_t		   lo_inode;			/* ioctl r/o */
  uint64_t		   lo_rdevice;			/* ioctl r/o */
  uint64_t		   lo_offset;
  uint64_t		   lo_sizelimit;/* bytes, 0 == max available */
  uint32_t		   lo_number;			/* ioctl r/o */
  uint32_t		   lo_encrypt_type;
  uint32_t		   lo_encrypt_key_size;		/* ioctl w/o */
  uint32_t		   lo_flags;			/* ioctl r/o */
  unsigned char		   lo_file_name[LO_NAME_SIZE];
  unsigned char		   lo_crypt_name[LO_NAME_SIZE];
  unsigned char		   lo_encrypt_key[LO_KEY_SIZE]; /* ioctl w/o */
  uint64_t		   lo_init[2];
};

/* Loop filter types */
#define LO_CRYPT_NONE		0
#define LO_CRYPT_XOR		1
#define LO_CRYPT_DES		2
#define LO_CRYPT_FISH2		3    /* Twofish encryption */
#define LO_CRYPT_BLOW		4
#define LO_CRYPT_CAST128	5
#define LO_CRYPT_IDEA		6
#define LO_CRYPT_DUMMY		9
#define LO_CRYPT_SKIPJACK	10
#define LO_CRYPT_CRYPTOAPI	18
#define MAX_LO_CRYPT		20

/* IOCTL commands --- we will commandeer 0x4C ('L') */
#define LOOP_SET_FD	0x4C00
#define LOOP_CLR_FD	0x4C01
#define LOOP_SET_STATUS	0x4C02
#define LOOP_GET_STATUS	0x4C03

__END_DECLS

#endif
