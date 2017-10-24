#include "PIC.h"

.text
.global sleep
.type   sleep,@function

sleep:
	movl    4(%esp),%eax            # Argument holen
	PIC_SAVE		# non-PIC: empty line
	PIC_INIT		# non-PIC: empty line
	pushl   $0                      # 0 ns
	pushl   %eax                    # 'x' µs warten
	movl    %esp,%eax
	pushl   %eax                    # zweimal ein Zeiger auf das Stackobjekt
	pushl   %eax                    # ptr  ptr  sec  nsec  return  arg
#ifdef __DYN_LIB
	call	nanosleep@PLT
#else
	call    nanosleep
#endif
	movl    20(%esp),%eax           # 'x' holen
	subl    8(%esp),%eax            # schon abgelaufende Zeit subtrahieren
	addl    $16,%esp
	PIC_RESTORE		# non-PIC: empty line
	ret
