The method:
1) Scan the code section for direct syscalls, i.e. SVC 0 instructions. Once
found, backtrack until we find a MOV imm in x8 which is the syscall number. We
backtrack up to x instructions. For Musl with a small backtrack window (x=10),
this allows to identify 99% of syscall invocation points. The only thing that
is left are wrapper functions, for example `__syscall(id, param1, param2,
etc.)`, where the identifier comes from another register rather than from an
immediate value.

For these cases we look for wrapper invocations and backtrack the load of an
immediate in the register corresponding to the id parameter (x0 for Musl).

To generalize the technique to multiple C libraries or programs in general,
there is only a very small effort needed, doable even in the absence of source
ode: identify the wrappers (they have a very specific pattern) and identify
which register holds the syscall identifier (not very hard, it is the one
loaded with the immediate value that falls within syscall id range.

