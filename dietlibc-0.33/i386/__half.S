#
#  Definiert Konstante (float)0.5 unter dem Namen __half
#  Wird von etlichen Funktionen benutzt (sinh, cosh, atanh)
#

.global __half
.data
.type   __half,@object
.size   __half,4
__half: .long 0x3f000000
