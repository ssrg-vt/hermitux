//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB BT code. This C        //
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
#include "work_lhs.h"
#include "timers.h"

//---------------------------------------------------------------------
// Performs line solves in Y direction by first factoring
// the block-tridiagonal matrix into an upper triangular matrix, 
// and then performing back substitution to solve for the unknow
// vectors of each line.  
// 
// Make sure we treat elements zero to cell_size in the direction
// of the sweep.
//---------------------------------------------------------------------
void y_solve()
{
  int i, j, k, m, n, jsize;

  //---------------------------------------------------------------------
  //---------------------------------------------------------------------

  if (timeron) timer_start(t_ysolve);

  //---------------------------------------------------------------------
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // This function computes the left hand side for the three y-factors   
  //---------------------------------------------------------------------

  jsize = grid_points[1]-1;

  //---------------------------------------------------------------------
  // Compute the indices for storing the tri-diagonal matrix;
  // determine a (labeled f) and n jacobians for cell c
  //---------------------------------------------------------------------
  for (k = 1; k <= grid_points[2]-2; k++) {
    for (i = 1; i <= grid_points[0]-2; i++) {
      for (j = 0; j <= jsize; j++) {
        tmp1 = rho_i[k][j][i];
        tmp2 = tmp1 * tmp1;
        tmp3 = tmp1 * tmp2;

        fjac[j][0][0] = 0.0;
        fjac[j][1][0] = 0.0;
        fjac[j][2][0] = 1.0;
        fjac[j][3][0] = 0.0;
        fjac[j][4][0] = 0.0;

        fjac[j][0][1] = - ( u[k][j][i][1]*u[k][j][i][2] ) * tmp2;
        fjac[j][1][1] = u[k][j][i][2] * tmp1;
        fjac[j][2][1] = u[k][j][i][1] * tmp1;
        fjac[j][3][1] = 0.0;
        fjac[j][4][1] = 0.0;

        fjac[j][0][2] = - ( u[k][j][i][2]*u[k][j][i][2]*tmp2)
          + c2 * qs[k][j][i];
        fjac[j][1][2] = - c2 *  u[k][j][i][1] * tmp1;
        fjac[j][2][2] = ( 2.0 - c2 ) *  u[k][j][i][2] * tmp1;
        fjac[j][3][2] = - c2 * u[k][j][i][3] * tmp1;
        fjac[j][4][2] = c2;

        fjac[j][0][3] = - ( u[k][j][i][2]*u[k][j][i][3] ) * tmp2;
        fjac[j][1][3] = 0.0;
        fjac[j][2][3] = u[k][j][i][3] * tmp1;
        fjac[j][3][3] = u[k][j][i][2] * tmp1;
        fjac[j][4][3] = 0.0;

        fjac[j][0][4] = ( c2 * 2.0 * square[k][j][i] - c1 * u[k][j][i][4] )
          * u[k][j][i][2] * tmp2;
        fjac[j][1][4] = - c2 * u[k][j][i][1]*u[k][j][i][2] * tmp2;
        fjac[j][2][4] = c1 * u[k][j][i][4] * tmp1 
          - c2 * ( qs[k][j][i] + u[k][j][i][2]*u[k][j][i][2] * tmp2 );
        fjac[j][3][4] = - c2 * ( u[k][j][i][2]*u[k][j][i][3] ) * tmp2;
        fjac[j][4][4] = c1 * u[k][j][i][2] * tmp1;

        njac[j][0][0] = 0.0;
        njac[j][1][0] = 0.0;
        njac[j][2][0] = 0.0;
        njac[j][3][0] = 0.0;
        njac[j][4][0] = 0.0;

        njac[j][0][1] = - c3c4 * tmp2 * u[k][j][i][1];
        njac[j][1][1] =   c3c4 * tmp1;
        njac[j][2][1] =   0.0;
        njac[j][3][1] =   0.0;
        njac[j][4][1] =   0.0;

        njac[j][0][2] = - con43 * c3c4 * tmp2 * u[k][j][i][2];
        njac[j][1][2] =   0.0;
        njac[j][2][2] =   con43 * c3c4 * tmp1;
        njac[j][3][2] =   0.0;
        njac[j][4][2] =   0.0;

        njac[j][0][3] = - c3c4 * tmp2 * u[k][j][i][3];
        njac[j][1][3] =   0.0;
        njac[j][2][3] =   0.0;
        njac[j][3][3] =   c3c4 * tmp1;
        njac[j][4][3] =   0.0;

        njac[j][0][4] = - (  c3c4
            - c1345 ) * tmp3 * (u[k][j][i][1]*u[k][j][i][1])
          - ( con43 * c3c4
              - c1345 ) * tmp3 * (u[k][j][i][2]*u[k][j][i][2])
          - ( c3c4 - c1345 ) * tmp3 * (u[k][j][i][3]*u[k][j][i][3])
          - c1345 * tmp2 * u[k][j][i][4];

        njac[j][1][4] = (  c3c4 - c1345 ) * tmp2 * u[k][j][i][1];
        njac[j][2][4] = ( con43 * c3c4 - c1345 ) * tmp2 * u[k][j][i][2];
        njac[j][3][4] = ( c3c4 - c1345 ) * tmp2 * u[k][j][i][3];
        njac[j][4][4] = ( c1345 ) * tmp1;
      }

      //---------------------------------------------------------------------
      // now joacobians set, so form left hand side in y direction
      //---------------------------------------------------------------------
      lhsinit(lhs, jsize);
      for (j = 1; j <= jsize-1; j++) {
        tmp1 = dt * ty1;
        tmp2 = dt * ty2;

        lhs[j][AA][0][0] = - tmp2 * fjac[j-1][0][0]
          - tmp1 * njac[j-1][0][0]
          - tmp1 * dy1; 
        lhs[j][AA][1][0] = - tmp2 * fjac[j-1][1][0]
          - tmp1 * njac[j-1][1][0];
        lhs[j][AA][2][0] = - tmp2 * fjac[j-1][2][0]
          - tmp1 * njac[j-1][2][0];
        lhs[j][AA][3][0] = - tmp2 * fjac[j-1][3][0]
          - tmp1 * njac[j-1][3][0];
        lhs[j][AA][4][0] = - tmp2 * fjac[j-1][4][0]
          - tmp1 * njac[j-1][4][0];

        lhs[j][AA][0][1] = - tmp2 * fjac[j-1][0][1]
          - tmp1 * njac[j-1][0][1];
        lhs[j][AA][1][1] = - tmp2 * fjac[j-1][1][1]
          - tmp1 * njac[j-1][1][1]
          - tmp1 * dy2;
        lhs[j][AA][2][1] = - tmp2 * fjac[j-1][2][1]
          - tmp1 * njac[j-1][2][1];
        lhs[j][AA][3][1] = - tmp2 * fjac[j-1][3][1]
          - tmp1 * njac[j-1][3][1];
        lhs[j][AA][4][1] = - tmp2 * fjac[j-1][4][1]
          - tmp1 * njac[j-1][4][1];

        lhs[j][AA][0][2] = - tmp2 * fjac[j-1][0][2]
          - tmp1 * njac[j-1][0][2];
        lhs[j][AA][1][2] = - tmp2 * fjac[j-1][1][2]
          - tmp1 * njac[j-1][1][2];
        lhs[j][AA][2][2] = - tmp2 * fjac[j-1][2][2]
          - tmp1 * njac[j-1][2][2]
          - tmp1 * dy3;
        lhs[j][AA][3][2] = - tmp2 * fjac[j-1][3][2]
          - tmp1 * njac[j-1][3][2];
        lhs[j][AA][4][2] = - tmp2 * fjac[j-1][4][2]
          - tmp1 * njac[j-1][4][2];

        lhs[j][AA][0][3] = - tmp2 * fjac[j-1][0][3]
          - tmp1 * njac[j-1][0][3];
        lhs[j][AA][1][3] = - tmp2 * fjac[j-1][1][3]
          - tmp1 * njac[j-1][1][3];
        lhs[j][AA][2][3] = - tmp2 * fjac[j-1][2][3]
          - tmp1 * njac[j-1][2][3];
        lhs[j][AA][3][3] = - tmp2 * fjac[j-1][3][3]
          - tmp1 * njac[j-1][3][3]
          - tmp1 * dy4;
        lhs[j][AA][4][3] = - tmp2 * fjac[j-1][4][3]
          - tmp1 * njac[j-1][4][3];

        lhs[j][AA][0][4] = - tmp2 * fjac[j-1][0][4]
          - tmp1 * njac[j-1][0][4];
        lhs[j][AA][1][4] = - tmp2 * fjac[j-1][1][4]
          - tmp1 * njac[j-1][1][4];
        lhs[j][AA][2][4] = - tmp2 * fjac[j-1][2][4]
          - tmp1 * njac[j-1][2][4];
        lhs[j][AA][3][4] = - tmp2 * fjac[j-1][3][4]
          - tmp1 * njac[j-1][3][4];
        lhs[j][AA][4][4] = - tmp2 * fjac[j-1][4][4]
          - tmp1 * njac[j-1][4][4]
          - tmp1 * dy5;

        lhs[j][BB][0][0] = 1.0
          + tmp1 * 2.0 * njac[j][0][0]
          + tmp1 * 2.0 * dy1;
        lhs[j][BB][1][0] = tmp1 * 2.0 * njac[j][1][0];
        lhs[j][BB][2][0] = tmp1 * 2.0 * njac[j][2][0];
        lhs[j][BB][3][0] = tmp1 * 2.0 * njac[j][3][0];
        lhs[j][BB][4][0] = tmp1 * 2.0 * njac[j][4][0];

        lhs[j][BB][0][1] = tmp1 * 2.0 * njac[j][0][1];
        lhs[j][BB][1][1] = 1.0
          + tmp1 * 2.0 * njac[j][1][1]
          + tmp1 * 2.0 * dy2;
        lhs[j][BB][2][1] = tmp1 * 2.0 * njac[j][2][1];
        lhs[j][BB][3][1] = tmp1 * 2.0 * njac[j][3][1];
        lhs[j][BB][4][1] = tmp1 * 2.0 * njac[j][4][1];

        lhs[j][BB][0][2] = tmp1 * 2.0 * njac[j][0][2];
        lhs[j][BB][1][2] = tmp1 * 2.0 * njac[j][1][2];
        lhs[j][BB][2][2] = 1.0
          + tmp1 * 2.0 * njac[j][2][2]
          + tmp1 * 2.0 * dy3;
        lhs[j][BB][3][2] = tmp1 * 2.0 * njac[j][3][2];
        lhs[j][BB][4][2] = tmp1 * 2.0 * njac[j][4][2];

        lhs[j][BB][0][3] = tmp1 * 2.0 * njac[j][0][3];
        lhs[j][BB][1][3] = tmp1 * 2.0 * njac[j][1][3];
        lhs[j][BB][2][3] = tmp1 * 2.0 * njac[j][2][3];
        lhs[j][BB][3][3] = 1.0
          + tmp1 * 2.0 * njac[j][3][3]
          + tmp1 * 2.0 * dy4;
        lhs[j][BB][4][3] = tmp1 * 2.0 * njac[j][4][3];

        lhs[j][BB][0][4] = tmp1 * 2.0 * njac[j][0][4];
        lhs[j][BB][1][4] = tmp1 * 2.0 * njac[j][1][4];
        lhs[j][BB][2][4] = tmp1 * 2.0 * njac[j][2][4];
        lhs[j][BB][3][4] = tmp1 * 2.0 * njac[j][3][4];
        lhs[j][BB][4][4] = 1.0
          + tmp1 * 2.0 * njac[j][4][4] 
          + tmp1 * 2.0 * dy5;

        lhs[j][CC][0][0] =  tmp2 * fjac[j+1][0][0]
          - tmp1 * njac[j+1][0][0]
          - tmp1 * dy1;
        lhs[j][CC][1][0] =  tmp2 * fjac[j+1][1][0]
          - tmp1 * njac[j+1][1][0];
        lhs[j][CC][2][0] =  tmp2 * fjac[j+1][2][0]
          - tmp1 * njac[j+1][2][0];
        lhs[j][CC][3][0] =  tmp2 * fjac[j+1][3][0]
          - tmp1 * njac[j+1][3][0];
        lhs[j][CC][4][0] =  tmp2 * fjac[j+1][4][0]
          - tmp1 * njac[j+1][4][0];

        lhs[j][CC][0][1] =  tmp2 * fjac[j+1][0][1]
          - tmp1 * njac[j+1][0][1];
        lhs[j][CC][1][1] =  tmp2 * fjac[j+1][1][1]
          - tmp1 * njac[j+1][1][1]
          - tmp1 * dy2;
        lhs[j][CC][2][1] =  tmp2 * fjac[j+1][2][1]
          - tmp1 * njac[j+1][2][1];
        lhs[j][CC][3][1] =  tmp2 * fjac[j+1][3][1]
          - tmp1 * njac[j+1][3][1];
        lhs[j][CC][4][1] =  tmp2 * fjac[j+1][4][1]
          - tmp1 * njac[j+1][4][1];

        lhs[j][CC][0][2] =  tmp2 * fjac[j+1][0][2]
          - tmp1 * njac[j+1][0][2];
        lhs[j][CC][1][2] =  tmp2 * fjac[j+1][1][2]
          - tmp1 * njac[j+1][1][2];
        lhs[j][CC][2][2] =  tmp2 * fjac[j+1][2][2]
          - tmp1 * njac[j+1][2][2]
          - tmp1 * dy3;
        lhs[j][CC][3][2] =  tmp2 * fjac[j+1][3][2]
          - tmp1 * njac[j+1][3][2];
        lhs[j][CC][4][2] =  tmp2 * fjac[j+1][4][2]
          - tmp1 * njac[j+1][4][2];

        lhs[j][CC][0][3] =  tmp2 * fjac[j+1][0][3]
          - tmp1 * njac[j+1][0][3];
        lhs[j][CC][1][3] =  tmp2 * fjac[j+1][1][3]
          - tmp1 * njac[j+1][1][3];
        lhs[j][CC][2][3] =  tmp2 * fjac[j+1][2][3]
          - tmp1 * njac[j+1][2][3];
        lhs[j][CC][3][3] =  tmp2 * fjac[j+1][3][3]
          - tmp1 * njac[j+1][3][3]
          - tmp1 * dy4;
        lhs[j][CC][4][3] =  tmp2 * fjac[j+1][4][3]
          - tmp1 * njac[j+1][4][3];

        lhs[j][CC][0][4] =  tmp2 * fjac[j+1][0][4]
          - tmp1 * njac[j+1][0][4];
        lhs[j][CC][1][4] =  tmp2 * fjac[j+1][1][4]
          - tmp1 * njac[j+1][1][4];
        lhs[j][CC][2][4] =  tmp2 * fjac[j+1][2][4]
          - tmp1 * njac[j+1][2][4];
        lhs[j][CC][3][4] =  tmp2 * fjac[j+1][3][4]
          - tmp1 * njac[j+1][3][4];
        lhs[j][CC][4][4] =  tmp2 * fjac[j+1][4][4]
          - tmp1 * njac[j+1][4][4]
          - tmp1 * dy5;
      }

      //---------------------------------------------------------------------
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // performs guaussian elimination on this cell.
      // 
      // assumes that unpacking routines for non-first cells 
      // preload C' and rhs' from previous cell.
      // 
      // assumed send happens outside this routine, but that
      // c'(JMAX) and rhs'(JMAX) will be sent to next cell
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // multiply c[k][0][i] by b_inverse and copy back to c
      // multiply rhs(0) by b_inverse(0) and copy to rhs
      //---------------------------------------------------------------------
      binvcrhs( lhs[0][BB], lhs[0][CC], rhs[k][0][i] );

      //---------------------------------------------------------------------
      // begin inner most do loop
      // do all the elements of the cell unless last 
      //---------------------------------------------------------------------
      for (j = 1; j <= jsize-1; j++) {
        //-------------------------------------------------------------------
        // subtract A*lhs_vector(j-1) from lhs_vector(j)
        // 
        // rhs(j) = rhs(j) - A*rhs(j-1)
        //-------------------------------------------------------------------
        matvec_sub(lhs[j][AA], rhs[k][j-1][i], rhs[k][j][i]);

        //-------------------------------------------------------------------
        // B(j) = B(j) - C(j-1)*A(j)
        //-------------------------------------------------------------------
        matmul_sub(lhs[j][AA], lhs[j-1][CC], lhs[j][BB]);

        //-------------------------------------------------------------------
        // multiply c[k][j][i] by b_inverse and copy back to c
        // multiply rhs[k][0][i] by b_inverse[k][0][i] and copy to rhs
        //-------------------------------------------------------------------
        binvcrhs( lhs[j][BB], lhs[j][CC], rhs[k][j][i] );
      }

      //---------------------------------------------------------------------
      // rhs(jsize) = rhs(jsize) - A*rhs(jsize-1)
      //---------------------------------------------------------------------
      matvec_sub(lhs[jsize][AA], rhs[k][jsize-1][i], rhs[k][jsize][i]);

      //---------------------------------------------------------------------
      // B(jsize) = B(jsize) - C(jsize-1)*A(jsize)
      // matmul_sub(AA,i,jsize,k,c,
      // $              CC,i,jsize-1,k,c,BB,i,jsize,k)
      //---------------------------------------------------------------------
      matmul_sub(lhs[jsize][AA], lhs[jsize-1][CC], lhs[jsize][BB]);

      //---------------------------------------------------------------------
      // multiply rhs(jsize) by b_inverse(jsize) and copy to rhs
      //---------------------------------------------------------------------
      binvrhs( lhs[jsize][BB], rhs[k][jsize][i] );

      //---------------------------------------------------------------------
      // back solve: if last cell, then generate U(jsize)=rhs(jsize)
      // else assume U(jsize) is loaded in un pack backsub_info
      // so just use it
      // after u(jstart) will be sent to next cell
      //---------------------------------------------------------------------
      for (j = jsize-1; j >= 0; j--) {
        for (m = 0; m < BLOCK_SIZE; m++) {
          for (n = 0; n < BLOCK_SIZE; n++) {
            rhs[k][j][i][m] = rhs[k][j][i][m] 
              - lhs[j][CC][n][m]*rhs[k][j+1][i][n];
          }
        }
      }
    }
  }
  if (timeron) timer_stop(t_ysolve);
}
