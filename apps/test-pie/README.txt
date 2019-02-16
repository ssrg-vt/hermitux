For PIE and ASLR we need a special toolchain that can generate binaries that
are both static and PIE. It seems it is only doable with musl currently:
https://www.openwall.com/lists/musl/2015/06/01/12 This makefile also download
that toolchain if needed, so it can take a bit of time

