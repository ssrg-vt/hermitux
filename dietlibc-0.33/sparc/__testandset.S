.text
.align 4
.global __testandset
__testandset:
	retl
	ldstub	[%o0], %o0
