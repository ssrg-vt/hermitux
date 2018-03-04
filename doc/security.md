# HermiTux and security

## Filesystem forwarding
As shown in the experiments, forwardign filesystem to a Linux host is fast 
compared to in-unikernel implementations (OSv, rump). This is because we make
use of the mature Linux FS stack, benefit from the efficient Linux page cache.
Moreover, read/write operations are fowrarded with zero-copy as the hypervisor
running in the host has full visibility of the guest memory.

However, forwarding FS calls to the host might be a security concern as it 
breaks the isolation of the guest. Thus, we do the followign things to address
this issue:
- All the files access are checked by uhyve:
 - By default, a unikernel is not allowed to access files (other than read/write)
   on the host
 - If a unikernel wants to access some files, a whitelist can be provided by
   the system administrator. It can be fine grained such as Read/write permission
   on which file, and insidfe which directory. 

## Addiitonal security features
  - We run uhyve under the secomp filter, balcklisting all system calls except
    the 13 needed to execute uhyve (over ~350 linux syscalls)
  - TODO more fine grained stuff such as ioctl only on /dev/kvm, and only with
    specific cmds
