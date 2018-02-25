# GDB support

## Implementation description

Hermitux support GDB debugging of both application and kernel through the remote
gdb capabilities. Most of the code was leveraged from solo5/ukvm, see the files
with `gdb` in their names here:
https://github.com/Solo5/solo5/tree/master/ukvm

Implementing support for a gdb remote target is relatively easy. In our case,
the target is the uhyve hypervisor. It communicates with a gdb client using
a well defined protocol:
https://sourceware.org/gdb/onlinedocs/gdb/Remote-Protocol.html

This protocol is implemented using a simple socket connection between uhyve and
the gdb client.

To be supported by gdb, the target must be able to:
- Communicate with the client on a debug exception (ex: breakpoint reached).
  With uhyve, a debug exception within the guest results in a trap to the
  hypervisor with `KVM_EXIT_DEBUG` status, thus we can send a notification
  to the gdb client over the socket connection.
- Read and modify the CPU registers: this is doable from the hypervisor using
  `KVM_SET_REGS`, `KVM_SET_SREGS`, `KVM_GET_REGS`, `KVM_GET_SREGS`.
- Read and modify the guest virtual address space. This is also doable from 
  uhyve as the guest memory is also mapped in the hypervisor address space.

Some modifications were necessary to adapt the ukvm gdb stub to uhyve. Ukvm
is expecting to run the solo5 unikernel, which differs from HermiTux in the
way the virtual address space is layed out. When gdb request some guest memory
access (for example inspecting a variable or setting a breakpoint), the target
needs to translate a guest virtual address into a virtual address within the
hypervisor virtual address space. This involve computing the corresponding
guest physical address as the hypervisor only has visibility over the guest
physical memory. With solo5, there is a direct mapping between guest virtual
and guest physical memory, so the translation is easy:

```
guest_physical = guest_virtual + offset
```

Where `offset` is a constant equal to the starting address of the big hypervisor 
buffer correspondign to the guest physical memory.

With HermiTux, it is a bit more complicated as while static memory is directly 
mapped (eveyrhing that comes from the kernel & binary ELF sections), dynamic 
memory such as kernel and application stacks, kernel and application heaps, are
mapped using regular 4-level page tables.

Thus, with HermiTux, in order to compute the guest physical address from the
virtual one, we use the KVM feature `KVM_TRANSLATE` which walks from the
hypervisor the guest page tables.

## Features

- As they live in the same address space, it is possible to conjointly debug the
  guest application as well as the kernel. On the contrary to regular 
  unikernels, with HermiTux the kernel and the application reside in different
  binaries. It is possible to load symbols from multiple binaries using
  the gdb command `add-symbol-file`: this is how we add the application symbols
  after calling gdb with the kernel binary as parameter. To load the 
  application symbols we also need to instruct gdb of the location in the 
  virtual address space for the application text section. The application is 
  loaded at 0x400000 but the `.text` section may be located a bit further in the
  address space. We can get that information using `readelf -S` and look at the
  `.text` location. See `tools/hermitux-gdb` for a gdb wrapper that is doing all
  of this automatically so that the user can fully transparently call it as he
  would call gdb.

- Adding and removing breakpoints is supported.

- Inspecting memory for checking the value of a variable or printing the stack 
  is supported. Memory writes, for example to modify a variable at runtime is
  also supported.

- Inspectign and modifying the registers value is supported (for example one
  needs to have access to the value of `%rsp` to print the stack.

- Single step execution is supported, both at the C code line as well as 
  assembly instruction level (`layout asm` works).

- The hypervisor automatically breaks on the first guest instruction (in a 
  similar fashion as qemu `-S` parameter), making boot process debugging 
  possible, including the part written in assembly.

- Hardware breakpoints are supported (uhyve can access and modify the VCPU 
  state)

- Watchpoint and conditional breakpoints work

- Interrupting the VM while it executes is not supported. It could probably be 
  done but would involve a complete redesign of the GDB stub as a thread 
  separated from uhyve execution flow (in order to received GDB asynchronous
  interrupt requests

- Multi-threading: not supported for now.

## Usage

### GDB wrapper

We provide a gdb wrapper (`tools/hermitux-gdb`) for quick and easy joint
debugging of application and kernel. First, edit this script and set the
`HERMITUX_BASE` variable to point on HermiTux codebase root folder on your
filesystem.

Next, launch an HermiTux application with the `HERMIT_DEBUG` envirnonment 
variable set to `1`:
```
$ HERMIT_DEBUG=1 HERMIT_ISLE=uhyve HERMIT_TUX=1 path/to/proxy path/to/linux/binary
proxy: Waiting for a debugger, see tools/hermitux-gdb for info.
```

In another terminal use the wrapper:
```
tools/hermitux-gdb path/to/linux/binary
```

### Manual connection

First let's look at the exact virtual address the linux binary text section:

```
readelf -SW path/to/linux/binary
# ...
  [ 6] .text             PROGBITS        0000000000400490 # ...
# ...
```
Here we can see that `.text` starts at `0x400490`. Launch the HermiTux 
application with debug mode enabled:
```
$ HERMIT_DEBUG=1 HERMIT_ISLE=uhyve HERMIT_TUX=1 path/to/proxy path/to/linux/binary
proxy: Waiting for a debugger, see tools/hermitux-gdb for info.
```

In another terminal start gdb with the HermiTux kernel as parameter and add the
linux binary symbols based on the relevant virtual address, then connect to
the remote target:
```
gdb path/to/hermitux/kernel
(gdb) add-symbol-file path/to/linux/binary 0x400490
(gdb) target remote :1234
```

### Printing the stack content

```
x/100xw $rsp
```
