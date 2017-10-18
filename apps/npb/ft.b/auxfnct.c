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
#include <math.h>

#include "global.h"
#include "randdp.h"


//---------------------------------------------------------------------
// compute the roots-of-unity array that will be used for subsequent FFTs. 
//---------------------------------------------------------------------
void CompExp(int n, dcomplex exponent[n])
{
  int m, nu, ku, i, j, ln;
  double t, ti;
  const double pi = 3.141592653589793238;

  nu = n;
  m = ilog2(n);
  exponent[0] = dcmplx(m, 0.0);
  ku = 2;
  ln = 1;
  for (j = 1; j <= m; j++) {
    t = pi / ln;
    for (i = 0; i <= ln - 1; i++) {
      ti = i * t;
      exponent[i+ku-1] = dcmplx(cos(ti), sin(ti));
    }
    ku = ku + ln;
    ln = 2 * ln;
  }
}


int ilog2(int n)
{
  int nn, lg;
  if (n == 1) return 0;

  lg = 1;
  nn = 2;
  while (nn < n) {
    nn = nn * 2;
    lg = lg + 1;
  }
  return lg;
}


//---------------------------------------------------------------------
// compute a^exponent mod 2^46
//---------------------------------------------------------------------
static double ipow46(double a, int exponent)
{
  double result, dummy, q, r;
  int n, n2;

  //---------------------------------------------------------------------
  // Use
  //   a^n = a^(n/2)*a^(n/2) if n even else
  //   a^n = a*a^(n-1)       if n odd
  //---------------------------------------------------------------------
  result = 1;
  if (exponent == 0) return result;
  q = a;
  r = 1;
  n = exponent;

  while (n > 1) {
    n2 = n / 2;
    if (n2 * 2 == n) {
      dummy = randlc(&q, q);
      n = n2;
    } else {
      dummy = randlc(&r, q);
      n = n-1;
    }
  }
  dummy = randlc(&r, q);
  result = r;
  return result;
}


void CalculateChecksum(dcomplex *csum, int iterN, int d1, int d2, int d3,
                       dcomplex u[d3][d2][d1+1])
{
  int i, i1, ii, ji, ki;
  dcomplex csum_temp = dcmplx(0.0, 0.0);
  for (i = 1; i <= 1024; i++) {
    i1 = i;
    ii = i1 % d1;
    ji = 3 * i1 % d2;
    ki = 5 * i1 % d3;
    csum_temp = dcmplx_add(csum_temp, u[ki][ji][ii]);
  }
  csum_temp = dcmplx_div2(csum_temp, (double)(d1*d2*d3));
  printf(" T =%5d     Checksum =%22.12E%22.12E\n", 
      iterN, csum_temp.real, csum_temp.imag);
  *csum = csum_temp;
}


void compute_initial_conditions(int d1, int d2, int d3, 
                                dcomplex u0[d3][d2][d1+1])
{
  dcomplex tmp[MAXDIM];
  double x0, start, an, dummy;
  double RanStarts[MAXDIM];

  int i, j, k;
  const double seed = 314159265.0;
  const double a = 1220703125.0;

  start = seed;
  //---------------------------------------------------------------------
  // Jump to the starting element for our first plane.
  //---------------------------------------------------------------------
  an = ipow46(a, 0);
  dummy = randlc(&start, an);
  an = ipow46(a, 2*d1*d2);
  //---------------------------------------------------------------------
  // Go through by z planes filling in one square at a time.
  //---------------------------------------------------------------------
  RanStarts[0] = start;
  for (k = 1; k < d3; k++) {
    dummy = randlc(&start, an);
    RanStarts[k] = start;
  }

  for (k = 0; k < d3; k++) {
    x0 = RanStarts[k];
    for (j = 0; j < d2; j++) {
      vranlc(2*d1, &x0, a, (double *)tmp);
      for (i = 0; i < d1; i++) {
        u0[k][j][i] = tmp[i];
      }
    }
  }
}


void evolve(int nx, int ny, int nz,
            dcomplex x[nz][ny][nx+1], dcomplex y[nz][ny][nx+1],
            double twiddle[nz][ny][nx+1])
{
  int i, j, k;
  for (i = 0; i < nz; i++) {
    for (k = 0; k < ny; k++) {
      for (j = 0; j < nx; j++) {
        y[i][k][j] = dcmplx_mul2(y[i][k][j], twiddle[i][k][j]);
        x[i][k][j] = y[i][k][j];
      }
    }
  }
}

