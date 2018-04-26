//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is an OpenMP C version of the NPB LU code. This OpenMP  //
//  C version is developed by the Center for Manycore Programming at Seoul //
//  National University and derived from the OpenMP Fortran versions in    //
//  "NPB3.3-OMP" developed by NAS.                                         //
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
//  Send comments or suggestions for this OpenMP C version to              //
//  cmp@aces.snu.ac.kr                                                     //
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

#include <math.h>
#include "applu.incl"

//---------------------------------------------------------------------
// to compute the l2-norm of vector v.
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// To improve cache performance, second two dimensions padded by 1 
// for even number sizes only.  Only needed in v.
//---------------------------------------------------------------------
void l2norm (int ldx, int ldy, int ldz, int nx0, int ny0, int nz0,
     int ist, int iend, int jst, int jend,
     double v[][ldy/2*2+1][ldx/2*2+1][5], double sum[5])
{
  //---------------------------------------------------------------------
  // local variables
  //---------------------------------------------------------------------
  double sum_local[5];
  int i, j, k, m;

  for (m = 0; m < 5; m++) {
    sum[m] = 0.0;
  }

  #pragma omp parallel default(shared) private(i,j,k,m,sum_local)
  {
  for (m = 0; m < 5; m++) {
    sum_local[m] = 0.0;
  }
  #pragma omp for nowait
  for (k = 1; k < nz0-1; k++) {
    for (j = jst; j < jend; j++) {
      for (i = ist; i < iend; i++) {
        for (m = 0; m < 5; m++) {
          sum_local[m] = sum_local[m] + v[k][j][i][m] * v[k][j][i][m];
        }
      }
    }
  }
  for (m = 0; m < 5; m++) {
    #pragma omp atomic
    sum[m] += sum_local[m];
  }
  } //end parallel

  for (m = 0; m < 5; m++) {
    sum[m] = sqrt ( sum[m] / ( (nx0-2)*(ny0-2)*(nz0-2) ) );
  }
}

