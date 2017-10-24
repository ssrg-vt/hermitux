/*
 * dynamic unified-syscalls
 * Olaf Dreesen
 */

#include "../syscalls.s/environ.S"
#include "../syscalls.s/errno.S"

#include <dietfeatures.h>
#include "syscalls.h"
#include "PIC.h"

#ifdef WANT_SYSENTER
.data
.type __vsyscall,@object
.global __vsyscall
__vsyscall:
.Lvsyscall:
.long	.Lcallint80

.text
.Lcallint80:
	int	$0x80
	ret
#endif

/* ok now include all syscalls.s (*.S) and sysdep *.S */

#include "mmap.S"

/* here on i386 I had split them some pre- some post-unified-syscall,
 * because the jumps pre-u.s.c. have only a 1 byte displacement. the first
 * post-u.s.c. have also 1 byte dsplacement the rest a 4 byte.
 * this pre/post split saves about 250 byte of code ....
 *
 * Please put additional syscalls AFTER the post-u.s.c. 
 * or try to not break the savings :) */

#include "select.S"

#include "../syscalls.s/fork.S"
#include "../syscalls.s/read.S"
#include "../syscalls.s/write.S"
#include "../syscalls.s/open.S"
#include "../syscalls.s/close.S"
#include "../syscalls.s/waitpid.S"

#include "../syscalls.s/getgid.S"
#include "../syscalls.s/getpgid.S"
#include "../syscalls.s/getpid.S"
#include "../syscalls.s/getppid.S"
#include "../syscalls.s/getuid.S"
#include "../syscalls.s/kill.S"
#include "../syscalls.s/ioctl.S"
#include "../syscalls.s/_llseek.S"
#include "../syscalls.s/lseek.S"
#include "../syscalls.s/lstat.S"
#include "../syscalls.s/lstat64.S"
#include "../syscalls.s/mkdir.S"
#include "../syscalls.s/mprotect.S"
#include "../syscalls.s/nanosleep.S"
#include "../syscalls.s/pipe.S"
#include "../syscalls.s/poll.S"
#include "../syscalls.s/readv.S"
#include "../syscalls.s/stat.S"
#include "../syscalls.s/stat64.S"
#include "../syscalls.s/truncate.S"
#include "../syscalls.s/umask.S"
#include "../syscalls.s/wait4.S"
#include "../syscalls.s/writev.S"
#include "../syscalls.s/__getpagesize.S"
#include "../syscalls.s/ustat.S"

/* OK this is basicaly unified.S */
.text
.type __you_tried_to_link_a_dietlibc_object_against_glibc,@function
.type __nop,@function
.type __unified_syscall_256,@function
__unified_syscall_256:
	movzwl	%ax, %eax
	jmp	1f
.size __unified_syscall_256,.-__unified_syscall_256
.type exit,@function
.weak exit
exit:
.type _exit,@function
.type __exit,@function
.global _exit
_exit:
__exit:
	mov $__NR_exit, %al
.type __unified_syscall,@function
__unified_syscall:
	movzbl	%al, %eax
1:
	push	%edi
	push	%esi
	push	%ebx
	movl	%esp,%edi
#ifdef WANT_SYSENTER
	/* PIC-code ... */
	pushl	%ebp
	PIC_INIT
	movl	.Lvsyscall@GOTOFF(%ebx),%ebp
	pushl	%ebx
#endif
	movl	0x10(%edi),%ebx
	movl	0x14(%edi),%ecx
	movl	0x18(%edi),%edx
	movl	0x1c(%edi),%esi
	movl	0x20(%edi),%edi
#ifdef WANT_SYSENTER
	call	*%ebp
	popl	%ebx
	popl	%ebp
#else
	int	$0x80
#endif
	cmp	$-124,%eax
	jbe	.Lnoerror
	neg	%eax

	/* PIC-code Ohhhhh My. */
	push	%eax
#ifndef WANT_SYSENTER
	PIC_INIT
#endif
	call	__errno_location@PLT
	popl	%ebx
	movl	%ebx,(%eax)
	orl	$-1,%eax
.Lnoerror:
	pop	%ebx
	pop	%esi
	pop	%edi
#include "dietuglyweaks.h"
	ret

#include "../syscalls.s/__pread.S"
#include "../syscalls.s/__pwrite.S"
#include "../syscalls.s/__reboot.S"
#include "../syscalls.s/access.S"
#include "../syscalls.s/adjtimex.S"
#include "../syscalls.s/brk.S"
#include "../syscalls.s/chdir.S"
#include "../syscalls.s/chmod.S"
#include "../syscalls.s/chown.S"
#include "../syscalls.s/chroot.S"
#include "../syscalls.s/dup.S"
#include "../syscalls.s/dup2.S"
#include "../syscalls.s/execve.S"
#include "../syscalls.s/fchdir.S"
#include "../syscalls.s/fchmod.S"
#include "../syscalls.s/fchown.S"
#include "../syscalls.s/fcntl.S"
#include "../syscalls.s/flock.S"
#include "../syscalls.s/fstat.S"
#include "../syscalls.s/fstat64.S"
#include "../syscalls.s/fstatfs.S"
#include "../syscalls.s/fsync.S"
#include "../syscalls.s/fdatasync.S"
#include "../syscalls.s/ftruncate.S"
#include "../syscalls.s/getcwd.S"
#include "../syscalls.s/getdents.S"
#include "../syscalls.s/getdents64.S"
#include "../syscalls.s/getegid.S"
#include "../syscalls.s/geteuid.S"
#include "../syscalls.s/getgroups.S"
#include "../syscalls.s/getitimer.S"
#include "../syscalls.s/getpriority.S"
#include "../syscalls.s/getresgid.S"
#include "../syscalls.s/getresuid.S"
#include "../syscalls.s/getrlimit.S"
#include "../syscalls.s/getrusage.S"
#include "../syscalls.s/getsid.S"
#include "../syscalls.s/gettimeofday.S"
#include "../syscalls.s/settimeofday.S"
#include "../syscalls.s/ioperm.S"
#include "../syscalls.s/iopl.S"
#include "../syscalls.s/ipc.S"
#include "../syscalls.s/lchown.S"
#include "../syscalls.s/link.S"
#include "../syscalls.s/mknod.S"
#include "../syscalls.s/mlock.S"
#include "../syscalls.s/mlockall.S"
#include "../syscalls.s/mount.S"
#include "../syscalls.s/mremap.S"
#include "../syscalls.s/munlockall.S"
#include "../syscalls.s/munmap.S"
#include "../syscalls.s/n_sigaction.S"
#include "../syscalls.s/n_sigpending.S"
#include "../syscalls.s/n_sigprocmask.S"
#include "../syscalls.s/n_sigsuspend.S"
#include "../syscalls.s/nice.S"
#include "../syscalls.s/pause.S"
#include "../syscalls.s/personality.S"
#include "../syscalls.s/query_module.S"
#include "../syscalls.s/init_module.S"
#include "../syscalls.s/delete_module.S"
#include "../syscalls.s/create_module.S"
#include "../syscalls.s/readlink.S"
#include "../syscalls.s/rename.S"
#include "../syscalls.s/rmdir.S"
#include "../syscalls.s/rt_sigaction.S"
#include "../syscalls.s/rt_sigpending.S"
#include "../syscalls.s/rt_sigprocmask.S"
#include "../syscalls.s/rt_sigqueueinfo.S"
#include "../syscalls.s/rt_sigsuspend.S"
#include "../syscalls.s/rt_sigtimedwait.S"
#include "../syscalls.s/sched_getaffinity.S"
#include "../syscalls.s/sched_setaffinity.S"
#include "../syscalls.s/sched_get_priority_max.S"
#include "../syscalls.s/sched_get_priority_min.S"
#include "../syscalls.s/sched_getparam.S"
#include "../syscalls.s/sched_getscheduler.S"
#include "../syscalls.s/sched_rr_get_interval.S"
#include "../syscalls.s/sched_setparam.S"
#include "../syscalls.s/sched_setscheduler.S"
#include "../syscalls.s/sched_yield.S"
#include "../syscalls.s/sendfile.S"
#include "../syscalls.s/setdomainname.S"
#include "../syscalls.s/setfsgid.S"
#include "../syscalls.s/setfsuid.S"
#include "../syscalls.s/setgid.S"
#include "../syscalls.s/setgroups.S"
#include "../syscalls.s/sethostname.S"
#include "../syscalls.s/setitimer.S"
#include "../syscalls.s/setpgid.S"
#include "../syscalls.s/setpriority.S"
#include "../syscalls.s/setregid.S"
#include "../syscalls.s/setresgid.S"
#include "../syscalls.s/setresuid.S"
#include "../syscalls.s/setreuid.S"
#include "../syscalls.s/setrlimit.S"
#include "../syscalls.s/setsid.S"
#include "../syscalls.s/setuid.S"
#include "../syscalls.s/sigaltstack.S"
#include "../syscalls.s/statfs.S"
#include "../syscalls.s/stime.S"
#include "../syscalls.s/swapoff.S"
#include "../syscalls.s/swapon.S"
#include "../syscalls.s/symlink.S"
#include "../syscalls.s/sync.S"
#include "../syscalls.s/sysctl.S"
#include "../syscalls.s/sysinfo.S"
#include "../syscalls.s/syslog.S"
#include "../syscalls.s/time.S"
#include "../syscalls.s/times.S"
#include "../syscalls.s/umount.S"
#include "../syscalls.s/umount2.S"
#include "../syscalls.s/uname.S"
#include "../syscalls.s/unlink.S"
#include "../syscalls.s/utime.S"
#include "../syscalls.s/vhangup.S"
#include "../syscalls.s/msync.S"

#include "../syscalls.s/chown32.S"
#include "../syscalls.s/fchown32.S"
#include "../syscalls.s/getegid32.S"
#include "../syscalls.s/geteuid32.S"
#include "../syscalls.s/getgid32.S"
#include "../syscalls.s/getgroups32.S"
#include "../syscalls.s/getresgid32.S"
#include "../syscalls.s/getuid32.S"
#include "../syscalls.s/lchown32.S"
#include "../syscalls.s/setfsgid32.S"
#include "../syscalls.s/setfsuid32.S"
#include "../syscalls.s/setgid32.S"
#include "../syscalls.s/setregid32.S"
#include "../syscalls.s/setresgid32.S"
#include "../syscalls.s/setreuid32.S"
#include "../syscalls.s/setuid32.S"
#include "../syscalls.s/alarm.S"
#include "../syscalls.s/ptrace.S"
#include "../syscalls.s/truncate64.S"
#include "../syscalls.s/ftruncate64.S"
#include "../syscalls.s/epoll_create.S"
#include "../syscalls.s/epoll_ctl.S"
#include "../syscalls.s/epoll_wait.S"
#include "../syscalls.s/sendfile64.S"
#include "../syscalls.s/sysfs.S"
#include "../syscalls.s/capget.S"
#include "../syscalls.s/capset.S"
#include "../syscalls.s/fcntl64.S"

#include "../syscalls.s/timer_create.S"
#include "../syscalls.s/timer_settime.S"
#include "../syscalls.s/timer_gettime.S"
#include "../syscalls.s/timer_getoverrun.S"
#include "../syscalls.s/timer_delete.S"

#include "../syscalls.s/clock_settime.S"
#include "../syscalls.s/clock_gettime.S"
#include "../syscalls.s/clock_getres.S"
#include "../syscalls.s/clock_nanosleep.S"

#include "../syscalls.s/mq_getsetattr.S"
#include "../syscalls.s/mq_notify.S"
#include "../syscalls.s/mq_open.S"
#include "../syscalls.s/mq_timedreceive.S"
#include "../syscalls.s/mq_timedsend.S"
#include "../syscalls.s/mq_unlink.S"

#include "../syscalls.s/statfs64.S"
#include "../syscalls.s/fstatfs64.S"

#include "../syscalls.s/tkill.S"
#include "../syscalls.s/tgkill.S"
#include "../syscalls.s/set_tid_address.S"
#include "../syscalls.s/set_thread_area.S"
#include "../syscalls.s/set_mempolicy.S"
#include "../syscalls.s/rt_sigreturn.S"
#include "../syscalls.s/gettid.S"
#include "../syscalls.s/get_thread_area.S"

#include "../syscalls.s/remap_file_pages.S"
#include "../syscalls.s/readahead.S"
#include "../syscalls.s/quotactl.S"
#include "../syscalls.s/mincore.S"
#include "../syscalls.s/mbind.S"
#include "../syscalls.s/get_mempolicy.S"
#include "../syscalls.s/futex.S"
#include "../syscalls.s/fadvise64.S"
#include "../syscalls.s/fadvise64_64.S"
#include "../syscalls.s/vserver.S"

#include "../syscalls.s/request_key.S"
#include "../syscalls.s/keyctl.S"
#include "../syscalls.s/add_key.S"

#include "../syscalls.s/io_submit.S"
#include "../syscalls.s/io_setup.S"
#include "../syscalls.s/io_getevents.S"
#include "../syscalls.s/io_destroy.S"
#include "../syscalls.s/io_cancel.S"

#include "../syscalls.s/inotify_rm_watch.S"
#include "../syscalls.s/inotify_init.S"
#include "../syscalls.s/inotify_add_watch.S"

#include "../syscalls.s/setxattr.S"
#include "../syscalls.s/lsetxattr.S"
#include "../syscalls.s/fsetxattr.S"
#include "../syscalls.s/removexattr.S"
#include "../syscalls.s/lremovexattr.S"
#include "../syscalls.s/fremovexattr.S"
#include "../syscalls.s/listxattr.S"
#include "../syscalls.s/llistxattr.S"
#include "../syscalls.s/flistxattr.S"
#include "../syscalls.s/getxattr.S"
#include "../syscalls.s/lgetxattr.S"
#include "../syscalls.s/fgetxattr.S"

#include "../syscalls.s/timerfd.S"
#include "../syscalls.s/__signalfd.S"

#include "../syscalls.s/fallocate.S"
#include "../syscalls.s/utimes.S"

#undef __PIC__
#undef __socketcall
/* oh what a kludge! */
#define socketcall .Lsocketcall
#include "send.S"
#include "recv.S"
#include "socket.S"
#include "accept.S"
#include "shutdown.S"
#include "socketpair.S"
#include "setsockopt.S"
#include "sendto.S"
#include "sendmsg.S"
#include "recvmsg.S"
#include "recvfrom.S"
#include "listen.S"
#include "getsockopt.S"
#include "getsockname.S"
#include "getpeername.S"
#include "connect.S"
#include "bind.S"
#undef socketcall
.Lsocketcall:
#include "socketcall.S"
#define __PIC__
#undef __socketcall


/* so now this was the last u.s.c. */

/* other asm-files ... */
#define PIC

#define _exit __exit
#include "clone.S"
#undef _exit

#include "__longjmp.S"
#include "setjmp.S"

#include "syscalls.s/__guard.S"
