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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "timers.h"


// for checksum data
/* common /sumcomm/ */
static dcomplex sums[NITER_DEFAULT+1];

/* common /mainarrays/ */
static double twiddle[NZ][NY][NX+1];
static dcomplex xnt[NZ][NY][NX+1];
static dcomplex y[NZ][NY][NX+1];
//static dcomplex pad1[128], pad2[128];


void appft(int niter, double *total_time, logical *verified)
{
  int i, j, k, kt, n12, n22, n32, ii, jj, kk, ii2, ik2;
  double ap;

  dcomplex exp1[NX], exp2[NY], exp3[NZ];

  for (i = 1; i <= 15; i++) {
    timer_clear(i);
  }         

  timer_start(2);      
  compute_initial_conditions(NX, NY, NZ, xnt);

  CompExp(NX, exp1);
  CompExp(NY, exp2);
  CompExp(NZ, exp3);          
  fftXYZ(1, NX, NY, NZ, xnt, (dcomplex *)y, exp1, exp2, exp3);
  timer_stop(2);

  timer_start(1);
  if (timers_enabled) timer_start(13);

  n12 = NX / 2;
  n22 = NY / 2;
  n32 = NZ / 2;
  ap = -4.0 * ALPHA * (PI * PI);
  for (i = 0; i < NZ; i++) {
    ii = i - (i / n32) * NZ;
    ii2 = ii * ii;
    for (k = 0; k < NY; k++) {
      kk = k - (k / n22) * NY;
      ik2 = ii2 + kk*kk;
      for (j = 0; j < NX; j++) {
        jj = j - (j / n12) * NX;
        twiddle[i][k][j] = exp(ap*(double)(jj*jj + ik2));
      }
    }
  }
  if (timers_enabled) timer_stop(13);

  if (timers_enabled) timer_start(12);
  compute_initial_conditions(NX, NY, NZ, xnt);
  if (timers_enabled) timer_stop(12);
  if (timers_enabled) timer_start(15);
  fftXYZ(1, NX, NY, NZ, xnt, (dcomplex *)y, exp1, exp2, exp3);
  if (timers_enabled) timer_stop(15);

  for (kt = 1; kt <= niter; kt++) {
    if (timers_enabled) timer_start(11);
    evolve(NX, NY, NZ, xnt, y, twiddle);
    if (timers_enabled) timer_stop(11);
    if (timers_enabled) timer_start(15);
    fftXYZ(-1, NX, NY, NZ, xnt, (dcomplex *)xnt, exp1, exp2, exp3);
    if (timers_enabled) timer_stop(15);
    if (timers_enabled) timer_start(10);
    CalculateChecksum(&sums[kt], kt, NX, NY, NZ, xnt);
    if (timers_enabled) timer_stop(10);
  }

  // Verification test.
  if (timers_enabled) timer_start(14);
  verify(NX, NY, NZ, niter, sums, verified);
  if (timers_enabled) timer_stop(14);
  timer_stop(1);

  *total_time = timer_read(1);
  if (!timers_enabled) return;

  printf(" FT subroutine timers \n");
  printf(" %26s =%9.4f\n", "FT total                  ", timer_read(1));
  printf(" %26s =%9.4f\n", "WarmUp time               ", timer_read(2));
  printf(" %26s =%9.4f\n", "fftXYZ body               ", timer_read(3));
  printf(" %26s =%9.4f\n", "Swarztrauber              ", timer_read(4));
  printf(" %26s =%9.4f\n", "X time                    ", timer_read(7));
  printf(" %26s =%9.4f\n", "Y time                    ", timer_read(8));
  printf(" %26s =%9.4f\n", "Z time                    ", timer_read(9));
  printf(" %26s =%9.4f\n", "CalculateChecksum         ", timer_read(10));
  printf(" %26s =%9.4f\n", "evolve                    ", timer_read(11));
  printf(" %26s =%9.4f\n", "compute_initial_conditions", timer_read(12));
  printf(" %26s =%9.4f\n", "twiddle                   ", timer_read(13));
  printf(" %26s =%9.4f\n", "verify                    ", timer_read(14));
  printf(" %26s =%9.4f\n", "fftXYZ                    ", timer_read(15));
  printf(" %26s =%9.4f\n", "Benchmark time            ", *total_time);
}
