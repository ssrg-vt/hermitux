.text
.globl __testandset
.proc  __testandset
__testandset:
	mov  ar.ccv = r0
	mov  r29 = 1
	ld8  r3 = [r32]
	;;
	cmpxchg8.acq  r8 = [r32], r29, ar.ccv /* cmpxchg is atomic */
	br.ret.sptk.clr b0
.endp __testandset
.size __testandset, . - __testandset
