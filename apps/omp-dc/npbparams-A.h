#define CLASS 'A'
/*
   This file is generated automatically by the setparams utility.
   It sets the number of processors and the class of the NPB
   in this directory. Do not modify it by hand.
   This file provided for backward compatibility.
   It is not used in DC benchmark.   */
   
long long int input_tuples=1000000, attrnum=15;
#define COMPILETIME "19 Mar 2018"
#define NPBVERSION "3.3.1"
#define CC "$(CROSS_COMPILE)gcc"
#define CFLAGS "-g -Wall -O3 -fopenmp -mcmodel=medium"
#define CLINK "$(CC)"
#define CLINKFLAGS "-O3 -fopenmp -mcmodel=medium"
#define C_LIB "-lm"
#define C_INC "-I../common"
