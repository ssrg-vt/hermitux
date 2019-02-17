//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB SP code. This C        //
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

#include "header.h"

//---------------------------------------------------------------------
// This subroutine initializes the field variable u using 
// tri-linear transfinite interpolation of the boundary values     
//---------------------------------------------------------------------
void initialize()
{
  int i, j, k, m, ix, iy, iz;
  double xi, eta, zeta, Pface[2][3][5], Pxi, Peta, Pzeta, temp[5];

  //---------------------------------------------------------------------
  //  Later (in compute_rhs) we compute 1/u for every element. A few of 
  //  the corner elements are not used, but it convenient (and faster) 
  //  to compute the whole thing with a simple loop. Make sure those 
  //  values are nonzero by initializing the whole thing here. 
  //---------------------------------------------------------------------
  for (k = 0; k <= grid_points[2]-1; k++) {
    for (j = 0; j <= grid_points[1]-1; j++) {
      for (i = 0; i <= grid_points[0]-1; i++) {
        u[k][j][i][0] = 1.0;
        u[k][j][i][1] = 0.0;
        u[k][j][i][2] = 0.0;
        u[k][j][i][3] = 0.0;
        u[k][j][i][4] = 1.0;
      }
    }
  }

  //---------------------------------------------------------------------
  // first store the "interpolated" values everywhere on the grid    
  //---------------------------------------------------------------------
  for (k = 0; k <= grid_points[2]-1; k++) {
    zeta = (double)k * dnzm1;
    for (j = 0; j <= grid_points[1]-1; j++) {
      eta = (double)j * dnym1;
      for (i = 0; i <= grid_points[0]-1; i++) {
        xi = (double)i * dnxm1;

        for (ix = 0; ix < 2; ix++) {
          Pxi = (double)ix;
          exact_solution(Pxi, eta, zeta, &Pface[ix][0][0]);
        }

        for (iy = 0; iy < 2; iy++) {
          Peta = (double)iy;
          exact_solution(xi, Peta, zeta, &Pface[iy][1][0]);
        }

        for (iz = 0; iz < 2; iz++) {
          Pzeta = (double)iz;
          exact_solution(xi, eta, Pzeta, &Pface[iz][2][0]);
        }

        for (m = 0; m < 5; m++) {
          Pxi   = xi   * Pface[1][0][m] + (1.0-xi)   * Pface[0][0][m];
          Peta  = eta  * Pface[1][1][m] + (1.0-eta)  * Pface[0][1][m];
          Pzeta = zeta * Pface[1][2][m] + (1.0-zeta) * Pface[0][2][m];

          u[k][j][i][m] = Pxi + Peta + Pzeta - 
                          Pxi*Peta - Pxi*Pzeta - Peta*Pzeta + 
                          Pxi*Peta*Pzeta;
        }
      }
    }
  }


  //---------------------------------------------------------------------
  // now store the exact values on the boundaries        
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // west face                                                  
  //---------------------------------------------------------------------
  xi = 0.0;
  i  = 0;
  for (k = 0; k <= grid_points[2]-1; k++) {
    zeta = (double)k * dnzm1;
    for (j = 0; j <= grid_points[1]-1; j++) {
      eta = (double)j * dnym1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }

  //---------------------------------------------------------------------
  // east face                                                      
  //---------------------------------------------------------------------
  xi = 1.0;
  i  = grid_points[0]-1;
  for (k = 0; k <= grid_points[2]-1; k++) {
    zeta = (double)k * dnzm1;
    for (j = 0; j <= grid_points[1]-1; j++) {
      eta = (double)j * dnym1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }

  //---------------------------------------------------------------------
  // south face                                                 
  //---------------------------------------------------------------------
  eta = 0.0;
  j   = 0;
  for (k = 0; k <= grid_points[2]-1; k++) {
    zeta = (double)k * dnzm1;
    for (i = 0; i <= grid_points[0]-1; i++) {
      xi = (double)i * dnxm1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }

  //---------------------------------------------------------------------
  // north face                                    
  //---------------------------------------------------------------------
  eta = 1.0;
  j   = grid_points[1]-1;
  for (k = 0; k <= grid_points[2]-1; k++) {
    zeta = (double)k * dnzm1;
    for (i = 0; i <= grid_points[0]-1; i++) {
      xi = (double)i * dnxm1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }

  //---------------------------------------------------------------------
  // bottom face                                       
  //---------------------------------------------------------------------
  zeta = 0.0;
  k    = 0;
  for (j = 0; j <= grid_points[1]-1; j++) {
    eta = (double)j * dnym1;
    for (i =0; i <= grid_points[0]-1; i++) {
      xi = (double)i * dnxm1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }

  //---------------------------------------------------------------------
  // top face     
  //---------------------------------------------------------------------
  zeta = 1.0;
  k    = grid_points[2]-1;
  for (j = 0; j <= grid_points[1]-1; j++) {
    eta = (double)j * dnym1;
    for (i =0; i <= grid_points[0]-1; i++) {
      xi = (double)i * dnxm1;
      exact_solution(xi, eta, zeta, temp);
      for (m = 0; m < 5; m++) {
        u[k][j][i][m] = temp[m];
      }
    }
  }
}


void lhsinit(int ni, int nj)
{
  int j, m;

  //---------------------------------------------------------------------
  // zap the whole left hand side for starters
  // set all diagonal values to 1. This is overkill, but convenient
  //---------------------------------------------------------------------
  for (j = 1; j <= nj; j++) {
    for (m = 0; m < 5; m++) {
      lhs [j][0][m] = 0.0;
      lhsp[j][0][m] = 0.0;
      lhsm[j][0][m] = 0.0;
      lhs [j][ni][m] = 0.0;
      lhsp[j][ni][m] = 0.0;
      lhsm[j][ni][m] = 0.0;
    }
    lhs [j][0][2] = 1.0;
    lhsp[j][0][2] = 1.0;
    lhsm[j][0][2] = 1.0;
    lhs [j][ni][2] = 1.0;
    lhsp[j][ni][2] = 1.0;
    lhsm[j][ni][2] = 1.0;
  }
}


void lhsinitj(int nj, int ni)
{
  int i, m;

  //---------------------------------------------------------------------
  // zap the whole left hand side for starters
  // set all diagonal values to 1. This is overkill, but convenient
  //---------------------------------------------------------------------
  for (i = 1; i <= ni; i++) {
    for (m = 0; m < 5; m++) {
      lhs [0][i][m] = 0.0;
      lhsp[0][i][m] = 0.0;
      lhsm[0][i][m] = 0.0;
      lhs [nj][i][m] = 0.0;
      lhsp[nj][i][m] = 0.0;
      lhsm[nj][i][m] = 0.0;
    }
    lhs [0][i][2] = 1.0;
    lhsp[0][i][2] = 1.0;
    lhsm[0][i][2] = 1.0;
    lhs [nj][i][2] = 1.0;
    lhsp[nj][i][2] = 1.0;
    lhsm[nj][i][2] = 1.0;
  }
}
