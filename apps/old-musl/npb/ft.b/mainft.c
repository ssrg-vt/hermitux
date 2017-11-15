//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB FT code. This C        //
//  version is developed by the Center for Manycore Programming at Seoul   //
//  National University and derived from the serial Fortran versions in    //
//  "NPB3.3-SER" developed by NAS.                                         //
//                                                                         //
//  Permission to use, copy, distribute and modify this software for any   //
//  purpose with or without fee is hereby granted. This software is        //
//  provided "as is" without express or implied warranty.                  //
//                                                                         //
//  Information on NPB 3.3, including the technical report, the original   //
//  specifications, source code, results and information on how to submit  //
//  new results, is available at:                                          //
//                                                                         //
//           http://www.nas.nasa.gov/Software/NPB/                         //
//                                                                         //
//  Send comments or suggestions for this C version to cmp@aces.snu.ac.kr  //
//                                                                         //
//          Center for Manycore Programming                                //
//          School of Computer Science and Engineering                     //
//          Seoul National University                                      //
//          Seoul 151-744, Korea                                           //
//                                                                         //
//          E-mail:  cmp@aces.snu.ac.kr                                    //
//                                                                         //
//-------------------------------------------------------------------------//

//-------------------------------------------------------------------------//
// Authors: Sangmin Seo, Jungwon Kim, Jun Lee, Jeongho Nah, Gangwon Jo,    //
//          and Jaejin Lee                                                 //
//-------------------------------------------------------------------------//

//---------------------------------------------------------------------
// FT benchmark
//---------------------------------------------------------------------

#include <stdio.h>
#include <math.h>

#include "global.h"
#include "print_results.h"


static char getclass();

logical timers_enabled;


int main(int argc, char *argv[])
{
  int niter;
  char Class;
  double total_time, mflops;
  logical verified;

  FILE *fp;
  if ((fp = fopen("timer.flag", "r")) != NULL) {
    timers_enabled = true;
    fclose(fp);
  } else {
    timers_enabled = false;
  }

  niter = NITER_DEFAULT;

  printf("\n\n NAS Parallel Benchmarks (NPB3.3-SER-C) - FT Benchmark\n\n");
  printf(" Size                : %4dx%4dx%4d\n", NX, NY, NZ);
  printf(" Iterations          :     %10d\n", niter);
  printf("\n");

  Class = getclass();

  appft(niter, &total_time, &verified);

  if (total_time != 0.0) {
    mflops = 1.0e-6 * (double)NTOTAL *
            (14.8157 + 7.19641 * log((double)NTOTAL)
             + (5.23518 + 7.21113 * log((double)NTOTAL)) * niter)
            / total_time;
  } else {
    mflops = 0.0;
  }

  print_results("FT", Class, NX, NY, NZ, niter,
                total_time, mflops, "          floating point", verified, 
                NPBVERSION, COMPILETIME, CS1, CS2, CS3, CS4, 
                CS5, CS6, CS7);

  return 0;
}


static char getclass()
{
  if ((NX == 64) && (NY == 64) &&                 
      (NZ == 64) && (NITER_DEFAULT == 6)) {
    return 'S';
  } else if ((NX == 128) && (NY == 128) &&
             (NZ == 32) && (NITER_DEFAULT == 6)) {
    return 'W';
  } else if ((NX == 256) && (NY == 256) &&
             (NZ == 128) && (NITER_DEFAULT == 6)) {
    return 'A';
  } else if ((NX == 512) && (NY == 256) &&
             (NZ == 256) && (NITER_DEFAULT == 20)) {
    return 'B';
  } else if ((NX == 512) && (NY == 512) &&
             (NZ == 512) && (NITER_DEFAULT == 20)) {
    return 'C';
  } else if ((NX == 2048) && (NY == 1024) &&
             (NZ == 1024) && (NITER_DEFAULT == 25)) {
    return 'D';
  } else {
    return 'U';
  }
}

