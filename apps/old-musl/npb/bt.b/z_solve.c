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
// Performs line solves in Z direction by first factoring
// the block-tridiagonal matrix into an upper triangular matrix, 
// and then performing back substitution to solve for the unknow
// vectors of each line.  
// 
// Make sure we treat elements zero to cell_size in the direction
// of the sweep.
//---------------------------------------------------------------------
void z_solve()
{
  int i, j, k, m, n, ksize;

  //---------------------------------------------------------------------
  //---------------------------------------------------------------------

  if (timeron) timer_start(t_zsolve);

  //---------------------------------------------------------------------
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // This function computes the left hand side for the three z-factors   
  //---------------------------------------------------------------------

  ksize = grid_points[2]-1;

  //---------------------------------------------------------------------
  // Compute the indices for storing the block-diagonal matrix;
  // determine c (labeled f) and s jacobians
  //---------------------------------------------------------------------
  for (j = 1; j <= grid_points[1]-2; j++) {
    for (i = 1; i <= grid_points[0]-2; i++) {
      for (k = 0; k <= ksize; k++) {
        tmp1 = 1.0 / u[k][j][i][0];
        tmp2 = tmp1 * tmp1;
        tmp3 = tmp1 * tmp2;

        fjac[k][0][0] = 0.0;
        fjac[k][1][0] = 0.0;
        fjac[k][2][0] = 0.0;
        fjac[k][3][0] = 1.0;
        fjac[k][4][0] = 0.0;

        fjac[k][0][1] = - ( u[k][j][i][1]*u[k][j][i][3] ) * tmp2;
        fjac[k][1][1] = u[k][j][i][3] * tmp1;
        fjac[k][2][1] = 0.0;
        fjac[k][3][1] = u[k][j][i][1] * tmp1;
        fjac[k][4][1] = 0.0;

        fjac[k][0][2] = - ( u[k][j][i][2]*u[k][j][i][3] ) * tmp2;
        fjac[k][1][2] = 0.0;
        fjac[k][2][2] = u[k][j][i][3] * tmp1;
        fjac[k][3][2] = u[k][j][i][2] * tmp1;
        fjac[k][4][2] = 0.0;

        fjac[k][0][3] = - (u[k][j][i][3]*u[k][j][i][3] * tmp2 ) 
          + c2 * qs[k][j][i];
        fjac[k][1][3] = - c2 *  u[k][j][i][1] * tmp1;
        fjac[k][2][3] = - c2 *  u[k][j][i][2] * tmp1;
        fjac[k][3][3] = ( 2.0 - c2 ) *  u[k][j][i][3] * tmp1;
        fjac[k][4][3] = c2;

        fjac[k][0][4] = ( c2 * 2.0 * square[k][j][i] - c1 * u[k][j][i][4] )
          * u[k][j][i][3] * tmp2;
        fjac[k][1][4] = - c2 * ( u[k][j][i][1]*u[k][j][i][3] ) * tmp2;
        fjac[k][2][4] = - c2 * ( u[k][j][i][2]*u[k][j][i][3] ) * tmp2;
        fjac[k][3][4] = c1 * ( u[k][j][i][4] * tmp1 )
          - c2 * ( qs[k][j][i] + u[k][j][i][3]*u[k][j][i][3] * tmp2 );
        fjac[k][4][4] = c1 * u[k][j][i][3] * tmp1;

        njac[k][0][0] = 0.0;
        njac[k][1][0] = 0.0;
        njac[k][2][0] = 0.0;
        njac[k][3][0] = 0.0;
        njac[k][4][0] = 0.0;

        njac[k][0][1] = - c3c4 * tmp2 * u[k][j][i][1];
        njac[k][1][1] =   c3c4 * tmp1;
        njac[k][2][1] =   0.0;
        njac[k][3][1] =   0.0;
        njac[k][4][1] =   0.0;

        njac[k][0][2] = - c3c4 * tmp2 * u[k][j][i][2];
        njac[k][1][2] =   0.0;
        njac[k][2][2] =   c3c4 * tmp1;
        njac[k][3][2] =   0.0;
        njac[k][4][2] =   0.0;

        njac[k][0][3] = - con43 * c3c4 * tmp2 * u[k][j][i][3];
        njac[k][1][3] =   0.0;
        njac[k][2][3] =   0.0;
        njac[k][3][3] =   con43 * c3 * c4 * tmp1;
        njac[k][4][3] =   0.0;

        njac[k][0][4] = - (  c3c4
            - c1345 ) * tmp3 * (u[k][j][i][1]*u[k][j][i][1])
          - ( c3c4 - c1345 ) * tmp3 * (u[k][j][i][2]*u[k][j][i][2])
          - ( con43 * c3c4
              - c1345 ) * tmp3 * (u[k][j][i][3]*u[k][j][i][3])
          - c1345 * tmp2 * u[k][j][i][4];

        njac[k][1][4] = (  c3c4 - c1345 ) * tmp2 * u[k][j][i][1];
        njac[k][2][4] = (  c3c4 - c1345 ) * tmp2 * u[k][j][i][2];
        njac[k][3][4] = ( con43 * c3c4
            - c1345 ) * tmp2 * u[k][j][i][3];
        njac[k][4][4] = ( c1345 )* tmp1;
      }

      //---------------------------------------------------------------------
      // now jacobians set, so form left hand side in z direction
      //---------------------------------------------------------------------
      lhsinit(lhs, ksize);
      for (k = 1; k <= ksize-1; k++) {
        tmp1 = dt * tz1;
        tmp2 = dt * tz2;

        lhs[k][AA][0][0] = - tmp2 * fjac[k-1][0][0]
          - tmp1 * njac[k-1][0][0]
          - tmp1 * dz1; 
        lhs[k][AA][1][0] = - tmp2 * fjac[k-1][1][0]
          - tmp1 * njac[k-1][1][0];
        lhs[k][AA][2][0] = - tmp2 * fjac[k-1][2][0]
          - tmp1 * njac[k-1][2][0];
        lhs[k][AA][3][0] = - tmp2 * fjac[k-1][3][0]
          - tmp1 * njac[k-1][3][0];
        lhs[k][AA][4][0] = - tmp2 * fjac[k-1][4][0]
          - tmp1 * njac[k-1][4][0];

        lhs[k][AA][0][1] = - tmp2 * fjac[k-1][0][1]
          - tmp1 * njac[k-1][0][1];
        lhs[k][AA][1][1] = - tmp2 * fjac[k-1][1][1]
          - tmp1 * njac[k-1][1][1]
          - tmp1 * dz2;
        lhs[k][AA][2][1] = - tmp2 * fjac[k-1][2][1]
          - tmp1 * njac[k-1][2][1];
        lhs[k][AA][3][1] = - tmp2 * fjac[k-1][3][1]
          - tmp1 * njac[k-1][3][1];
        lhs[k][AA][4][1] = - tmp2 * fjac[k-1][4][1]
          - tmp1 * njac[k-1][4][1];

        lhs[k][AA][0][2] = - tmp2 * fjac[k-1][0][2]
          - tmp1 * njac[k-1][0][2];
        lhs[k][AA][1][2] = - tmp2 * fjac[k-1][1][2]
          - tmp1 * njac[k-1][1][2];
        lhs[k][AA][2][2] = - tmp2 * fjac[k-1][2][2]
          - tmp1 * njac[k-1][2][2]
          - tmp1 * dz3;
        lhs[k][AA][3][2] = - tmp2 * fjac[k-1][3][2]
          - tmp1 * njac[k-1][3][2];
        lhs[k][AA][4][2] = - tmp2 * fjac[k-1][4][2]
          - tmp1 * njac[k-1][4][2];

        lhs[k][AA][0][3] = - tmp2 * fjac[k-1][0][3]
          - tmp1 * njac[k-1][0][3];
        lhs[k][AA][1][3] = - tmp2 * fjac[k-1][1][3]
          - tmp1 * njac[k-1][1][3];
        lhs[k][AA][2][3] = - tmp2 * fjac[k-1][2][3]
          - tmp1 * njac[k-1][2][3];
        lhs[k][AA][3][3] = - tmp2 * fjac[k-1][3][3]
          - tmp1 * njac[k-1][3][3]
          - tmp1 * dz4;
        lhs[k][AA][4][3] = - tmp2 * fjac[k-1][4][3]
          - tmp1 * njac[k-1][4][3];

        lhs[k][AA][0][4] = - tmp2 * fjac[k-1][0][4]
          - tmp1 * njac[k-1][0][4];
        lhs[k][AA][1][4] = - tmp2 * fjac[k-1][1][4]
          - tmp1 * njac[k-1][1][4];
        lhs[k][AA][2][4] = - tmp2 * fjac[k-1][2][4]
          - tmp1 * njac[k-1][2][4];
        lhs[k][AA][3][4] = - tmp2 * fjac[k-1][3][4]
          - tmp1 * njac[k-1][3][4];
        lhs[k][AA][4][4] = - tmp2 * fjac[k-1][4][4]
          - tmp1 * njac[k-1][4][4]
          - tmp1 * dz5;

        lhs[k][BB][0][0] = 1.0
          + tmp1 * 2.0 * njac[k][0][0]
          + tmp1 * 2.0 * dz1;
        lhs[k][BB][1][0] = tmp1 * 2.0 * njac[k][1][0];
        lhs[k][BB][2][0] = tmp1 * 2.0 * njac[k][2][0];
        lhs[k][BB][3][0] = tmp1 * 2.0 * njac[k][3][0];
        lhs[k][BB][4][0] = tmp1 * 2.0 * njac[k][4][0];

        lhs[k][BB][0][1] = tmp1 * 2.0 * njac[k][0][1];
        lhs[k][BB][1][1] = 1.0
          + tmp1 * 2.0 * njac[k][1][1]
          + tmp1 * 2.0 * dz2;
        lhs[k][BB][2][1] = tmp1 * 2.0 * njac[k][2][1];
        lhs[k][BB][3][1] = tmp1 * 2.0 * njac[k][3][1];
        lhs[k][BB][4][1] = tmp1 * 2.0 * njac[k][4][1];

        lhs[k][BB][0][2] = tmp1 * 2.0 * njac[k][0][2];
        lhs[k][BB][1][2] = tmp1 * 2.0 * njac[k][1][2];
        lhs[k][BB][2][2] = 1.0
          + tmp1 * 2.0 * njac[k][2][2]
          + tmp1 * 2.0 * dz3;
        lhs[k][BB][3][2] = tmp1 * 2.0 * njac[k][3][2];
        lhs[k][BB][4][2] = tmp1 * 2.0 * njac[k][4][2];

        lhs[k][BB][0][3] = tmp1 * 2.0 * njac[k][0][3];
        lhs[k][BB][1][3] = tmp1 * 2.0 * njac[k][1][3];
        lhs[k][BB][2][3] = tmp1 * 2.0 * njac[k][2][3];
        lhs[k][BB][3][3] = 1.0
          + tmp1 * 2.0 * njac[k][3][3]
          + tmp1 * 2.0 * dz4;
        lhs[k][BB][4][3] = tmp1 * 2.0 * njac[k][4][3];

        lhs[k][BB][0][4] = tmp1 * 2.0 * njac[k][0][4];
        lhs[k][BB][1][4] = tmp1 * 2.0 * njac[k][1][4];
        lhs[k][BB][2][4] = tmp1 * 2.0 * njac[k][2][4];
        lhs[k][BB][3][4] = tmp1 * 2.0 * njac[k][3][4];
        lhs[k][BB][4][4] = 1.0
          + tmp1 * 2.0 * njac[k][4][4] 
          + tmp1 * 2.0 * dz5;

        lhs[k][CC][0][0] =  tmp2 * fjac[k+1][0][0]
          - tmp1 * njac[k+1][0][0]
          - tmp1 * dz1;
        lhs[k][CC][1][0] =  tmp2 * fjac[k+1][1][0]
          - tmp1 * njac[k+1][1][0];
        lhs[k][CC][2][0] =  tmp2 * fjac[k+1][2][0]
          - tmp1 * njac[k+1][2][0];
        lhs[k][CC][3][0] =  tmp2 * fjac[k+1][3][0]
          - tmp1 * njac[k+1][3][0];
        lhs[k][CC][4][0] =  tmp2 * fjac[k+1][4][0]
          - tmp1 * njac[k+1][4][0];

        lhs[k][CC][0][1] =  tmp2 * fjac[k+1][0][1]
          - tmp1 * njac[k+1][0][1];
        lhs[k][CC][1][1] =  tmp2 * fjac[k+1][1][1]
          - tmp1 * njac[k+1][1][1]
          - tmp1 * dz2;
        lhs[k][CC][2][1] =  tmp2 * fjac[k+1][2][1]
          - tmp1 * njac[k+1][2][1];
        lhs[k][CC][3][1] =  tmp2 * fjac[k+1][3][1]
          - tmp1 * njac[k+1][3][1];
        lhs[k][CC][4][1] =  tmp2 * fjac[k+1][4][1]
          - tmp1 * njac[k+1][4][1];

        lhs[k][CC][0][2] =  tmp2 * fjac[k+1][0][2]
          - tmp1 * njac[k+1][0][2];
        lhs[k][CC][1][2] =  tmp2 * fjac[k+1][1][2]
          - tmp1 * njac[k+1][1][2];
        lhs[k][CC][2][2] =  tmp2 * fjac[k+1][2][2]
          - tmp1 * njac[k+1][2][2]
          - tmp1 * dz3;
        lhs[k][CC][3][2] =  tmp2 * fjac[k+1][3][2]
          - tmp1 * njac[k+1][3][2];
        lhs[k][CC][4][2] =  tmp2 * fjac[k+1][4][2]
          - tmp1 * njac[k+1][4][2];

        lhs[k][CC][0][3] =  tmp2 * fjac[k+1][0][3]
          - tmp1 * njac[k+1][0][3];
        lhs[k][CC][1][3] =  tmp2 * fjac[k+1][1][3]
          - tmp1 * njac[k+1][1][3];
        lhs[k][CC][2][3] =  tmp2 * fjac[k+1][2][3]
          - tmp1 * njac[k+1][2][3];
        lhs[k][CC][3][3] =  tmp2 * fjac[k+1][3][3]
          - tmp1 * njac[k+1][3][3]
          - tmp1 * dz4;
        lhs[k][CC][4][3] =  tmp2 * fjac[k+1][4][3]
          - tmp1 * njac[k+1][4][3];

        lhs[k][CC][0][4] =  tmp2 * fjac[k+1][0][4]
          - tmp1 * njac[k+1][0][4];
        lhs[k][CC][1][4] =  tmp2 * fjac[k+1][1][4]
          - tmp1 * njac[k+1][1][4];
        lhs[k][CC][2][4] =  tmp2 * fjac[k+1][2][4]
          - tmp1 * njac[k+1][2][4];
        lhs[k][CC][3][4] =  tmp2 * fjac[k+1][3][4]
          - tmp1 * njac[k+1][3][4];
        lhs[k][CC][4][4] =  tmp2 * fjac[k+1][4][4]
          - tmp1 * njac[k+1][4][4]
          - tmp1 * dz5;
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
      // c'(KMAX) and rhs'(KMAX) will be sent to next cell.
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // outer most do loops - sweeping in i direction
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // multiply c[0][j][i] by b_inverse and copy back to c
      // multiply rhs(0) by b_inverse(0) and copy to rhs
      //---------------------------------------------------------------------
      binvcrhs( lhs[0][BB], lhs[0][CC], rhs[0][j][i] );

      //---------------------------------------------------------------------
      // begin inner most do loop
      // do all the elements of the cell unless last 
      //---------------------------------------------------------------------
      for (k = 1; k <= ksize-1; k++) {
        //-------------------------------------------------------------------
        // subtract A*lhs_vector(k-1) from lhs_vector(k)
        // 
        // rhs(k) = rhs(k) - A*rhs(k-1)
        //-------------------------------------------------------------------
        matvec_sub(lhs[k][AA], rhs[k-1][j][i], rhs[k][j][i]);

        //-------------------------------------------------------------------
        // B(k) = B(k) - C(k-1)*A(k)
        // matmul_sub(AA,i,j,k,c,CC,i,j,k-1,c,BB,i,j,k)
        //-------------------------------------------------------------------
        matmul_sub(lhs[k][AA], lhs[k-1][CC], lhs[k][BB]);

        //-------------------------------------------------------------------
        // multiply c[k][j][i] by b_inverse and copy back to c
        // multiply rhs[0][j][i] by b_inverse[0][j][i] and copy to rhs
        //-------------------------------------------------------------------
        binvcrhs( lhs[k][BB], lhs[k][CC], rhs[k][j][i] );
      }

      //---------------------------------------------------------------------
      // Now finish up special cases for last cell
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // rhs(ksize) = rhs(ksize) - A*rhs(ksize-1)
      //---------------------------------------------------------------------
      matvec_sub(lhs[ksize][AA], rhs[ksize-1][j][i], rhs[ksize][j][i]);

      //---------------------------------------------------------------------
      // B(ksize) = B(ksize) - C(ksize-1)*A(ksize)
      // matmul_sub(AA,i,j,ksize,c,
      // $              CC,i,j,ksize-1,c,BB,i,j,ksize)
      //---------------------------------------------------------------------
      matmul_sub(lhs[ksize][AA], lhs[ksize-1][CC], lhs[ksize][BB]);

      //---------------------------------------------------------------------
      // multiply rhs(ksize) by b_inverse(ksize) and copy to rhs
      //---------------------------------------------------------------------
      binvrhs( lhs[ksize][BB], rhs[ksize][j][i] );

      //---------------------------------------------------------------------
      //---------------------------------------------------------------------

      //---------------------------------------------------------------------
      // back solve: if last cell, then generate U(ksize)=rhs(ksize)
      // else assume U(ksize) is loaded in un pack backsub_info
      // so just use it
      // after u(kstart) will be sent to next cell
      //---------------------------------------------------------------------

      for (k = ksize-1; k >= 0; k--) {
        for (m = 0; m < BLOCK_SIZE; m++) {
          for (n = 0; n < BLOCK_SIZE; n++) {
            rhs[k][j][i][m] = rhs[k][j][i][m] 
              - lhs[k][CC][n][m]*rhs[k+1][j][i][n];
          }
        }
      }
    }
  }
  if (timeron) timer_stop(t_zsolve);
}
