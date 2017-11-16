/* CLASS = B */
/*
   This file is generated automatically by the setparams utility.
   It sets the number of processors and the class of the NPB
   in this directory. Do not modify it by hand.   
*/

/* full problem size */
#define ISIZ1  102
#define ISIZ2  102
#define ISIZ3  102

/* number of iterations and how often to print the norm */
#define ITMAX_DEFAULT  250
#define INORM_DEFAULT  250
#define DT_DEFAULT     2.0

#define CONVERTDOUBLE  false
#define COMPILETIME "17 Oct 2017"
#define NPBVERSION "3.3.1"
#define CS1 "../../musl/prefix/bin/musl-gcc"
#define CS2 "$(CC)"
#define CS3 "-static -L../../musl/prefix/lib -lm"
#define CS4 "-I../common -I../../musl/prefix/include"
#define CS5 "-g -Wall -O3 -mcmodel=medium"
#define CS6 "-O3 -mcmodel=medium"
#define CS7 "randdp"