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
// this function performs the solution of the approximate factorization
// step in the z-direction for all five matrix components
// simultaneously. The Thomas algorithm is employed to solve the
// systems for the z-lines. Boundary conditions are non-periodic
//---------------------------------------------------------------------
void z_solve()
{
  int i, j, k, k1, k2, m;
  double ru1, fac1, fac2;

  if (timeron) timer_start(t_zsolve);
  for (j = 1; j <= ny2; j++) {
    lhsinitj(nz2+1, nx2);

    //---------------------------------------------------------------------
    // Computes the left hand side for the three z-factors   
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // first fill the lhs for the u-eigenvalue                          
    //---------------------------------------------------------------------
    for (i = 1; i <= nx2; i++) {
      for (k = 0; k <= nz2+1; k++) {
        ru1 = c3c4*rho_i[k][j][i];
        cv[k] = ws[k][j][i];
        rhos[k] = max(max(dz4+con43*ru1, dz5+c1c5*ru1), max(dzmax+ru1, dz1));
      }

      for (k = 1; k <= nz2; k++) {
        lhs[k][i][0] =  0.0;
        lhs[k][i][1] = -dttz2 * cv[k-1] - dttz1 * rhos[k-1];
        lhs[k][i][2] =  1.0 + c2dttz1 * rhos[k];
        lhs[k][i][3] =  dttz2 * cv[k+1] - dttz1 * rhos[k+1];
        lhs[k][i][4] =  0.0;
      }
    }

    //---------------------------------------------------------------------
    // add fourth order dissipation                                  
    //---------------------------------------------------------------------
    for (i = 1; i <= nx2; i++) {
      k = 1;
      lhs[k][i][2] = lhs[k][i][2] + comz5;
      lhs[k][i][3] = lhs[k][i][3] - comz4;
      lhs[k][i][4] = lhs[k][i][4] + comz1;

      k = 2;
      lhs[k][i][1] = lhs[k][i][1] - comz4;
      lhs[k][i][2] = lhs[k][i][2] + comz6;
      lhs[k][i][3] = lhs[k][i][3] - comz4;
      lhs[k][i][4] = lhs[k][i][4] + comz1;
    }

    for (k = 3; k <= nz2-2; k++) {
      for (i = 1; i <= nx2; i++) {
        lhs[k][i][0] = lhs[k][i][0] + comz1;
        lhs[k][i][1] = lhs[k][i][1] - comz4;
        lhs[k][i][2] = lhs[k][i][2] + comz6;
        lhs[k][i][3] = lhs[k][i][3] - comz4;
        lhs[k][i][4] = lhs[k][i][4] + comz1;
      }
    }

    for (i = 1; i <= nx2; i++) {
      k = nz2-1;
      lhs[k][i][0] = lhs[k][i][0] + comz1;
      lhs[k][i][1] = lhs[k][i][1] - comz4;
      lhs[k][i][2] = lhs[k][i][2] + comz6;
      lhs[k][i][3] = lhs[k][i][3] - comz4;

      k = nz2;
      lhs[k][i][0] = lhs[k][i][0] + comz1;
      lhs[k][i][1] = lhs[k][i][1] - comz4;
      lhs[k][i][2] = lhs[k][i][2] + comz5;
    }

    //---------------------------------------------------------------------
    // subsequently, fill the other factors (u+c), (u-c) 
    //---------------------------------------------------------------------
    for (k = 1; k <= nz2; k++) {
      for (i = 1; i <= nx2; i++) {
        lhsp[k][i][0] = lhs[k][i][0];
        lhsp[k][i][1] = lhs[k][i][1] - dttz2 * speed[k-1][j][i];
        lhsp[k][i][2] = lhs[k][i][2];
        lhsp[k][i][3] = lhs[k][i][3] + dttz2 * speed[k+1][j][i];
        lhsp[k][i][4] = lhs[k][i][4];
        lhsm[k][i][0] = lhs[k][i][0];
        lhsm[k][i][1] = lhs[k][i][1] + dttz2 * speed[k-1][j][i];
        lhsm[k][i][2] = lhs[k][i][2];
        lhsm[k][i][3] = lhs[k][i][3] - dttz2 * speed[k+1][j][i];
        lhsm[k][i][4] = lhs[k][i][4];
      }
    }


    //---------------------------------------------------------------------
    // FORWARD ELIMINATION  
    //---------------------------------------------------------------------
    for (k = 0; k <= grid_points[2]-3; k++) {
      k1 = k + 1;
      k2 = k + 2;
      for (i = 1; i <= nx2; i++) {
        fac1 = 1.0/lhs[k][i][2];
        lhs[k][i][3] = fac1*lhs[k][i][3];
        lhs[k][i][4] = fac1*lhs[k][i][4];
        for (m = 0; m < 3; m++) {
          rhs[k][j][i][m] = fac1*rhs[k][j][i][m];
        }
        lhs[k1][i][2] = lhs[k1][i][2] - lhs[k1][i][1]*lhs[k][i][3];
        lhs[k1][i][3] = lhs[k1][i][3] - lhs[k1][i][1]*lhs[k][i][4];
        for (m = 0; m < 3; m++) {
          rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhs[k1][i][1]*rhs[k][j][i][m];
        }
        lhs[k2][i][1] = lhs[k2][i][1] - lhs[k2][i][0]*lhs[k][i][3];
        lhs[k2][i][2] = lhs[k2][i][2] - lhs[k2][i][0]*lhs[k][i][4];
        for (m = 0; m < 3; m++) {
          rhs[k2][j][i][m] = rhs[k2][j][i][m] - lhs[k2][i][0]*rhs[k][j][i][m];
        }
      }
    }

    //---------------------------------------------------------------------
    // The last two rows in this grid block are a bit different, 
    // since they for (not have two more rows available for the
    // elimination of off-diagonal entries
    //---------------------------------------------------------------------
    k  = grid_points[2]-2;
    k1 = grid_points[2]-1;
    for (i = 1; i <= nx2; i++) {
      fac1 = 1.0/lhs[k][i][2];
      lhs[k][i][3] = fac1*lhs[k][i][3];
      lhs[k][i][4] = fac1*lhs[k][i][4];
      for (m = 0; m < 3; m++) {
        rhs[k][j][i][m] = fac1*rhs[k][j][i][m];
      }
      lhs[k1][i][2] = lhs[k1][i][2] - lhs[k1][i][1]*lhs[k][i][3];
      lhs[k1][i][3] = lhs[k1][i][3] - lhs[k1][i][1]*lhs[k][i][4];
      for (m = 0; m < 3; m++) {
        rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhs[k1][i][1]*rhs[k][j][i][m];
      }

      //---------------------------------------------------------------------
      // scale the last row immediately
      //---------------------------------------------------------------------
      fac2 = 1.0/lhs[k1][i][2];
      for (m = 0; m < 3; m++) {
        rhs[k1][j][i][m] = fac2*rhs[k1][j][i][m];
      }
    }

    //---------------------------------------------------------------------
    // for (the u+c and the u-c factors               
    //---------------------------------------------------------------------
    for (k = 0; k <= grid_points[2]-3; k++) {
      k1 = k + 1;
      k2 = k + 2;
      for (i = 1; i <= nx2; i++) {
        m = 3;
        fac1 = 1.0/lhsp[k][i][2];
        lhsp[k][i][3]    = fac1*lhsp[k][i][3];
        lhsp[k][i][4]    = fac1*lhsp[k][i][4];
        rhs[k][j][i][m]  = fac1*rhs[k][j][i][m];
        lhsp[k1][i][2]   = lhsp[k1][i][2] - lhsp[k1][i][1]*lhsp[k][i][3];
        lhsp[k1][i][3]   = lhsp[k1][i][3] - lhsp[k1][i][1]*lhsp[k][i][4];
        rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhsp[k1][i][1]*rhs[k][j][i][m];
        lhsp[k2][i][1]   = lhsp[k2][i][1] - lhsp[k2][i][0]*lhsp[k][i][3];
        lhsp[k2][i][2]   = lhsp[k2][i][2] - lhsp[k2][i][0]*lhsp[k][i][4];
        rhs[k2][j][i][m] = rhs[k2][j][i][m] - lhsp[k2][i][0]*rhs[k][j][i][m];

        m = 4;
        fac1 = 1.0/lhsm[k][i][2];
        lhsm[k][i][3]    = fac1*lhsm[k][i][3];
        lhsm[k][i][4]    = fac1*lhsm[k][i][4];
        rhs[k][j][i][m]  = fac1*rhs[k][j][i][m];
        lhsm[k1][i][2]   = lhsm[k1][i][2] - lhsm[k1][i][1]*lhsm[k][i][3];
        lhsm[k1][i][3]   = lhsm[k1][i][3] - lhsm[k1][i][1]*lhsm[k][i][4];
        rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhsm[k1][i][1]*rhs[k][j][i][m];
        lhsm[k2][i][1]   = lhsm[k2][i][1] - lhsm[k2][i][0]*lhsm[k][i][3];
        lhsm[k2][i][2]   = lhsm[k2][i][2] - lhsm[k2][i][0]*lhsm[k][i][4];
        rhs[k2][j][i][m] = rhs[k2][j][i][m] - lhsm[k2][i][0]*rhs[k][j][i][m];
      }
    }

    //---------------------------------------------------------------------
    // And again the last two rows separately
    //---------------------------------------------------------------------
    k  = grid_points[2]-2;
    k1 = grid_points[2]-1;
    for (i = 1; i <= nx2; i++) {
      m = 3;
      fac1 = 1.0/lhsp[k][i][2];
      lhsp[k][i][3]    = fac1*lhsp[k][i][3];
      lhsp[k][i][4]    = fac1*lhsp[k][i][4];
      rhs[k][j][i][m]  = fac1*rhs[k][j][i][m];
      lhsp[k1][i][2]   = lhsp[k1][i][2] - lhsp[k1][i][1]*lhsp[k][i][3];
      lhsp[k1][i][3]   = lhsp[k1][i][3] - lhsp[k1][i][1]*lhsp[k][i][4];
      rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhsp[k1][i][1]*rhs[k][j][i][m];

      m = 4;
      fac1 = 1.0/lhsm[k][i][2];
      lhsm[k][i][3]    = fac1*lhsm[k][i][3];
      lhsm[k][i][4]    = fac1*lhsm[k][i][4];
      rhs[k][j][i][m]  = fac1*rhs[k][j][i][m];
      lhsm[k1][i][2]   = lhsm[k1][i][2] - lhsm[k1][i][1]*lhsm[k][i][3];
      lhsm[k1][i][3]   = lhsm[k1][i][3] - lhsm[k1][i][1]*lhsm[k][i][4];
      rhs[k1][j][i][m] = rhs[k1][j][i][m] - lhsm[k1][i][1]*rhs[k][j][i][m];

      //---------------------------------------------------------------------
      // Scale the last row immediately (some of this is overkill
      // if this is the last cell)
      //---------------------------------------------------------------------
      rhs[k1][j][i][3] = rhs[k1][j][i][3]/lhsp[k1][i][2];
      rhs[k1][j][i][4] = rhs[k1][j][i][4]/lhsm[k1][i][2];
    }


    //---------------------------------------------------------------------
    // BACKSUBSTITUTION 
    //---------------------------------------------------------------------
    k  = grid_points[2]-2;
    k1 = grid_points[2]-1;
    for (i = 1; i <= nx2; i++) {
      for (m = 0; m < 3; m++) {
        rhs[k][j][i][m] = rhs[k][j][i][m] - lhs[k][i][3]*rhs[k1][j][i][m];
      }

      rhs[k][j][i][3] = rhs[k][j][i][3] - lhsp[k][i][3]*rhs[k1][j][i][3];
      rhs[k][j][i][4] = rhs[k][j][i][4] - lhsm[k][i][3]*rhs[k1][j][i][4];
    }

    //---------------------------------------------------------------------
    // Whether or not this is the last processor, we always have
    // to complete the back-substitution 
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // The first three factors
    //---------------------------------------------------------------------
    for (k = grid_points[2]-3; k >= 0; k--) {
      k1 = k + 1;
      k2 = k + 2;
      for (i = 1; i <= nx2; i++) {
        for (m = 0; m < 3; m++) {
          rhs[k][j][i][m] = rhs[k][j][i][m] - 
                            lhs[k][i][3]*rhs[k1][j][i][m] -
                            lhs[k][i][4]*rhs[k2][j][i][m];
        }

        //-------------------------------------------------------------------
        // And the remaining two
        //-------------------------------------------------------------------
        rhs[k][j][i][3] = rhs[k][j][i][3] - 
                          lhsp[k][i][3]*rhs[k1][j][i][3] -
                          lhsp[k][i][4]*rhs[k2][j][i][3];
        rhs[k][j][i][4] = rhs[k][j][i][4] - 
                          lhsm[k][i][3]*rhs[k1][j][i][4] -
                          lhsm[k][i][4]*rhs[k2][j][i][4];
      }
    }
  }
  if (timeron) timer_stop(t_zsolve);

  tzetar();
}
