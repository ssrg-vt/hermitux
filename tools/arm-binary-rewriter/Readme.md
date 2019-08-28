# Method

Intuitively, one may think that because aarch64 is a fixed-size instruction
set, replacing any system call with a function call would be as easy as
rewriting an instruction. However, a particularity of that architecture makes
it more complicated. A fundamental difference with x86-64 is that with aarch64,
when a function is called (BL -- Branch and Link), the return address is not
pushed on the stack but is rather put in a register, x30. This is done
automatically by the BL instruction. This may be changed (setting in to another
register or even on the stack) but by default any standard compiler will use
x30.

If we replace a system call instruction, SVC 0, with a function call to the
corresponding system call, BL, the issue is that this BL will overwrite
whatever was in x30 before. So we are loosing the return address of the
function that originally called SVC. We _need_ to use a BL and cannot use any
other type of B (we would not be able to return to the calling context from the
system call handler otherwise).

We are doing binary rewriting and we do not have the space to save and restore
x30 before our BL to the system call handler without overwriting additional
instruction and thus having to resort to complex solutions like we did for x86.

However, with further careful analysis, we can determine many situations in
which we can replace SVC with B or BL:

1) If the next instruction right after the SVC is a RET, then we can simply
   replace the SVC with a B to the syscall handler. The RET instruction at the
   end of the syscall handler will then return to the function that originally
   called the function calling SVC, which is the expected behavior.

2) In most situations, x30 will actually be saved on the stack within the
   function calling SVC, and restored before that functions return. The main
   reasons for this is that the function is calling BL so it needs to save x30.
   In these cases, it is safe for us to replace the SVC with a BL given that
   x30 has been saved and will be restored.
   This is true in many situations but we cannot assume it is true in all, so
   the challenge is: how to identify the cases in which we can safely replace
   SVC with a BL?

We need to do the following: get the CFG and explore all ret instruction that
we can reach from the function we are in, starting from the SVC. For all these
function exit points, we check if x30 is loaded from memory: this would mean
that x30 is restored and it was saved before. We can then replace SVC with BL.
If we find a path with no x30 restoration, we cannot rewrite.
