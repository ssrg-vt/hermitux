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


/* common /blockinfo/ */
static int fftblock;
//static int fftblockpad;

/* common /workarr/ */
static dcomplex plane[(BLOCKMAX+1)*MAXDIM];
//static dcomplex pad[128];
static dcomplex scr[MAXDIM][BLOCKMAX+1];


//---------------------------------------------------------------------
// Computes NY N-point complex-to-complex FFTs of X using an algorithm due
// to Swarztrauber.  X is both the input and the output array, while Y is a 
// scratch array.  It is assumed that N = 2^M.  Before calling 
// Swarztrauber to 
// perform FFTs
//---------------------------------------------------------------------
static void Swarztrauber(int is, int m, int vlen, int n, int xd1,
                         void *ox, dcomplex exponent[n])
{
  dcomplex (*x)[xd1] = (dcomplex (*)[xd1])ox;

  int i, j, l;
  dcomplex u1, x11, x21;
  int k, n1, li, lj, lk, ku, i11, i12, i21, i22;

  if (timers_enabled) timer_start(4);
  //---------------------------------------------------------------------
  // Perform one variant of the Stockham FFT.
  //---------------------------------------------------------------------
  n1 = n / 2;
  lj = 1;
  li = 1 << m;
  for (l = 1; l <= m; l += 2) {
    lk = lj;
    lj = 2 * lk;
    li = li / 2;
    ku = li;

    for (i = 0; i <= li - 1; i++) {
      i11 = i * lk;
      i12 = i11 + n1;
      i21 = i * lj;
      i22 = i21 + lk;

      if (is >= 1) {
        u1 = exponent[ku+i];
      } else {
        u1 = dconjg(exponent[ku+i]);
      }
      for (k = 0; k <= lk - 1; k++) {
        for (j = 0; j < vlen; j++) {
          x11 = x[i11+k][j];
          x21 = x[i12+k][j];
          scr[i21+k][j] = dcmplx_add(x11, x21);
          scr[i22+k][j] = dcmplx_mul(u1, dcmplx_sub(x11, x21));
        }
      }
    }

    if (l == m) {
      for (k = 0; k < n; k++) {
        for (j = 0; j < vlen; j++) {
          x[k][j] = scr[k][j];
        }
      }
    } else {
      lk = lj;
      lj = 2 * lk;
      li = li / 2;
      ku = li;

      for (i = 0; i <= li - 1; i++) {
        i11 = i * lk;
        i12 = i11 + n1;
        i21 = i * lj;
        i22 = i21 + lk;

        if (is >= 1) {
          u1 = exponent[ku+i];
        } else {
          u1 = dconjg(exponent[ku+i]);
        }
        for (k = 0; k <= lk - 1; k++) {
          for (j = 0; j < vlen; j++) {
            x11 = scr[i11+k][j];
            x21 = scr[i12+k][j];
            x[i21+k][j] = dcmplx_add(x11, x21);
            x[i22+k][j] = dcmplx_mul(u1, dcmplx_sub(x11, x21));
          }
        }
      }
    }
  }
  if (timers_enabled) timer_stop(4);
}


void fftXYZ(int sign, int n1, int n2, int n3,
            dcomplex x[n3][n2][n1+1], dcomplex xout[(n1+1)*n2*n3],
            dcomplex exp1[n1], dcomplex exp2[n2], dcomplex exp3[n3])
{
  int i, j, k, log;
  int bls, ble;
  int len;
  int blkp;

  if (timers_enabled) timer_start(3);

  fftblock = CACHESIZE / n1;
  if (fftblock >= BLOCKMAX) fftblock = BLOCKMAX;
  blkp = fftblock + 1;
  log = ilog2(n1);
  if (timers_enabled) timer_start(7);
  for (k = 0; k < n3; k++) {
    for (bls = 0; bls < n2; bls += fftblock) {
      ble = bls + fftblock - 1;
      if (ble > n2) ble = n2 - 1;
      len = ble - bls + 1;
      for (j = bls; j <= ble; j++) {
        for (i = 0; i < n1; i++) {
          plane[j-bls+blkp*i] = x[k][j][i];
        }
      }
      Swarztrauber(sign, log, len, n1, blkp, plane, exp1);
      for (j = bls; j <= ble; j++) {
        for (i = 0; i < n1; i++) {
          x[k][j][i] = plane[j-bls+blkp*i];
        }
      }
    }
  }
  if (timers_enabled) timer_stop(7);

  fftblock = CACHESIZE / n2;
  if (fftblock >= BLOCKMAX) fftblock = BLOCKMAX;
  blkp = fftblock + 1;
  log = ilog2(n2);
  if (timers_enabled) timer_start(8);
  for (k = 0; k < n3; k++) {
    for (bls = 0; bls < n1; bls += fftblock) {
      ble = bls + fftblock - 1;
      if (ble > n1) ble = n1 - 1;
      len = ble - bls + 1;
      Swarztrauber(sign, log, len, n2, n1+1, &x[k][0][bls], exp2);
    }
  }
  if (timers_enabled) timer_stop(8);

  fftblock = CACHESIZE / n3;
  if (fftblock >= BLOCKMAX) fftblock = BLOCKMAX;
  blkp = fftblock + 1;
  log = ilog2(n3);
  if (timers_enabled) timer_start(9);
  for (k = 0; k < n2; k++) {
    for (bls = 0; bls < n1; bls += fftblock) {
      ble = bls + fftblock - 1;
      if (ble > n1) ble = n1 - 1;
      len = ble - bls + 1;
      for (i = 0; i < n3; i++) {
        for (j = bls; j <= ble; j++) {
          plane[j-bls+blkp*i] = x[i][k][j];
        }
      }
      Swarztrauber(sign, log, len, n3, blkp, plane, exp3);
      for (i = 0; i <= n3-1; i++) {
        for (j = bls; j <= ble; j++) {
          xout[j+(n1+1)*(k+n2*i)] = plane[j-bls+blkp*i];
        }
      }
    }
  }
  if (timers_enabled) timer_stop(9);
  if (timers_enabled) timer_stop(3);
}

