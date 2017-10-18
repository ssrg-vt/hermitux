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

#include "npbparams.h"
#include "type.h"

// Cache blocking params. These values are good for most
// RISC processors.  
// FFT parameters:
//  fftblock controls how many ffts are done at a time. 
//  The default is appropriate for most cache-based machines
//  On vector machines, the FFT can be vectorized with vector
//  length equal to the block size, so the block size should
//  be as large as possible. This is the size of the smallest
//  dimension of the problem: 128 for class A, 256 for class B and
//  512 for class C.

#define FFTBLOCK_DEFAULT      16
#define FFTBLOCKPAD_DEFAULT   18
#define CACHESIZE             8192
#define BLOCKMAX              32

#define SEED                  314159265.0
#define A                     1220703125.0
#define PI                    3.141592653589793238
#define ALPHA                 1.0e-6

/* common /timerscomm/ */
extern logical timers_enabled;


#define dcmplx(r,i)       (dcomplex){r, i}
#define dcmplx_add(a,b)   (dcomplex){(a).real+(b).real, (a).imag+(b).imag}
#define dcmplx_sub(a,b)   (dcomplex){(a).real-(b).real, (a).imag-(b).imag}
#define dcmplx_mul(a,b)   (dcomplex){((a).real*(b).real)-((a).imag*(b).imag),\
                                     ((a).real*(b).imag)+((a).imag*(b).real)}
#define dcmplx_mul2(a,b)  (dcomplex){(a).real*(b), (a).imag*(b)}
static inline dcomplex dcmplx_div(dcomplex z1, dcomplex z2) {
  double a = z1.real;
  double b = z1.imag;
  double c = z2.real;
  double d = z2.imag;

  double divisor = c*c + d*d;
  double real = (a*c + b*d) / divisor;
  double imag = (b*c - a*d) / divisor;
  dcomplex result = (dcomplex){real, imag};
  return result;
}
#define dcmplx_div2(a,b)  (dcomplex){(a).real/(b), (a).imag/(b)}
#define dcmplx_abs(x)     sqrt(((x).real*(x).real) + ((x).imag*(x).imag))

#define dconjg(x)         (dcomplex){(x).real, -1.0*(x).imag}


void appft(int niter, double *total_time, logical *verified);
void CompExp(int n, dcomplex exponent[n]);
int ilog2(int n);
void CalculateChecksum(dcomplex *csum, int iterN, int d1, int d2, int d3,
                       dcomplex u[d3][d2][d1+1]);
void compute_initial_conditions(int d1, int d2, int d3, 
                                dcomplex u0[d3][d2][d1+1]);
void evolve(int nx, int ny, int nz,
            dcomplex x[nz][ny][nx+1], dcomplex y[nz][ny][nx+1],
            double twiddle[nz][ny][nx+1]);
void fftXYZ(int sign, int n1, int n2, int n3,
            dcomplex x[n3][n2][n1+1], dcomplex xout[(n1+1)*n2*n3],
            dcomplex exp1[n1], dcomplex exp2[n2], dcomplex exp3[n3]);
void verify(int n1, int n2, int n3, int nt, dcomplex cksum[nt+1],
            logical *verified);

