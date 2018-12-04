#include <stdio.h>
#include <math.h>
#include "type.h"


void print_results(char *name, char class, int n1, int n2, int n3, int niter,
    double t, double mops, char *optype, logical verified, char *npbversion,
    char *compiletime, char *cs1, char *cs2, char *cs3, char *cs4, char *cs5,
    char *cs6, char *cs7) 
{
  char size[16];
  int j;

  printf( "\n\n %s Benchmark Completed.\n", name );
  printf( " Class           =             %12c\n", class );

  // If this is not a grid-based problem (EP, FT, CG), then
  // we only print n1, which contains some measure of the
  // problem size. In that case, n2 and n3 are both zero.
  // Otherwise, we print the grid size n1xn2xn3

  if ( ( n2 == 0 ) && ( n3 == 0 ) ) {
    if ( ( name[0] == 'E' ) && ( name[1] == 'P' ) ) {
      sprintf( size, "%15.0lf", pow(2.0, n1) );
      j = 14;
      if ( size[j] == '.' ) {
        size[j] = ' '; 
        j--;
      }
      size[j+1] = '\0';
      printf( " Size            =          %15s\n", size );
    } else {
      printf( " Size            =             %12d\n", n1 );
    }
  } else {
    printf( " Size            =           %4dx%4dx%4d\n", n1, n2, n3 );
  }

  printf( " Iterations      =             %12d\n", niter );
  printf( " Time in seconds =             %12.2lf\n", t );
  printf( " Mop/s total     =          %15.2lf\n", mops );
  printf( " Operation type  = %24s\n", optype );
  if ( verified ) 
    printf( " Verification    =             %12s\n", "SUCCESSFUL" );
  else
    printf( " Verification    =             %12s\n", "UNSUCCESSFUL" );
  printf( " Version         =             %12s\n", npbversion );
  printf( " Compile date    =             %12s\n", compiletime );
  
  printf( "\n Compile options:\n"
          "    CC           = %s\n", cs1 );
  printf( "    CLINK        = %s\n", cs2 );
  printf( "    C_LIB        = %s\n", cs3 );
  printf( "    C_INC        = %s\n", cs4 );
  printf( "    CFLAGS       = %s\n", cs5 );
  printf( "    CLINKFLAGS   = %s\n", cs6 );
  printf( "    RAND         = %s\n", cs7 );

  printf( "\n--------------------------------------\n"
          " Please send all errors/feedbacks to:\n"
          " Center for Manycore Programming\n"
          " cmp@aces.snu.ac.kr\n"
          " http://aces.snu.ac.kr\n"
          "--------------------------------------\n\n");
}
