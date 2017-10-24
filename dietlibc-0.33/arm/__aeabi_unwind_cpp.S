.text
.global __aeabi_unwind_cpp_pr0
.hidden __aeabi_unwind_cpp_pr0
.type __aeabi_unwind_cpp_pr0, %function

.global __aeabi_unwind_cpp_pr1
.hidden __aeabi_unwind_cpp_pr1
.type __aeabi_unwind_cpp_pr1, %function

.global __aeabi_unwind_cpp_pr2
.hidden __aeabi_unwind_cpp_pr2
.type __aeabi_unwind_cpp_pr2, %function

__aeabi_unwind_cpp_pr0:
__aeabi_unwind_cpp_pr1:
__aeabi_unwind_cpp_pr2:
	mov	pc, lr	@ return from subroutine

.size __aeabi_unwind_cpp_pr0,.-__aeabi_unwind_cpp_pr0
.size __aeabi_unwind_cpp_pr1,.-__aeabi_unwind_cpp_pr1
.size __aeabi_unwind_cpp_pr2,.-__aeabi_unwind_cpp_pr2
