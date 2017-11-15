//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB LU code. This C        //
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
#include "applu.incl"
#include "timers.h"

//---------------------------------------------------------------------
// to perform pseudo-time stepping SSOR iterations
// for five nonlinear pde's.
//---------------------------------------------------------------------
void ssor(int niter)
{
  //---------------------------------------------------------------------
  // local variables
  //---------------------------------------------------------------------
  int i, j, k, m, n;
  int istep;
  double tmp, tv[ISIZ2][ISIZ1][5];
  double delunm[5];

  //---------------------------------------------------------------------
  // begin pseudo-time stepping iterations
  //---------------------------------------------------------------------
  tmp = 1.0 / ( omega * ( 2.0 - omega ) );

  //---------------------------------------------------------------------
  // initialize a,b,c,d to zero (guarantees that page tables have been
  // formed, if applicable on given architecture, before timestepping).
  //---------------------------------------------------------------------
  for (j = 0; j < ISIZ2; j++) {
    for (i = 0; i < ISIZ1; i++) {
      for (n = 0; n < 5; n++) {
        for (m = 0; m < 5; m++) {
          a[j][i][n][m] = 0.0;
          b[j][i][n][m] = 0.0;
          c[j][i][n][m] = 0.0;
          d[j][i][n][m] = 0.0;
        }
      }
    }
  }
  for (i = 1; i <= t_last; i++) {
    timer_clear(i);
  }

  //---------------------------------------------------------------------
  // compute the steady-state residuals
  //---------------------------------------------------------------------
  rhs();

  //---------------------------------------------------------------------
  // compute the L2 norms of newton iteration residuals
  //---------------------------------------------------------------------
  l2norm( ISIZ1, ISIZ2, ISIZ3, nx0, ny0, nz0,
          ist, iend, jst, jend, rsd, rsdnm );

  /*
  if ( ipr == 1 ) {
    printf("           Initial residual norms\n");
    printf("\n");
    printf(" \n RMS-norm of steady-state residual for "
           "first pde  = %12.5E\n"
           " RMS-norm of steady-state residual for "
           "second pde = %12.5E\n"
           " RMS-norm of steady-state residual for "
           "third pde  = %12.5E\n"
           " RMS-norm of steady-state residual for "
           "fourth pde = %12.5E\n"
           " RMS-norm of steady-state residual for "
           "fifth pde  = %12.5E\n", 
           rsdnm[0], rsdnm[1], rsdnm[2], rsdnm[3], rsdnm[4]);
    printf("\nIteration RMS-residual of 5th PDE\n");
  }
  */
 
  for (i = 1; i <= t_last; i++) {
    timer_clear(i);
  }
  timer_start(1);

  //---------------------------------------------------------------------
  // the timestep loop
  //---------------------------------------------------------------------
  for (istep = 1; istep <= niter; istep++) {
    //if ( ( (istep % inorm) == 0 ) && ipr == 1 ) {
    //  printf(" \n     pseudo-time SSOR iteration no.=%4d\n\n", istep);
    //}
    if ((istep % 20) == 0 || istep == itmax || istep == 1) {
      if (niter > 1) printf(" Time step %4d\n", istep);
    }

    //---------------------------------------------------------------------
    // perform SSOR iteration
    //---------------------------------------------------------------------
    if (timeron) timer_start(t_rhs);
    for (k = 1; k < nz - 1; k++) {
      for (j = jst; j < jend; j++) {
        for (i = ist; i < iend; i++) {
          for (m = 0; m < 5; m++) {
            rsd[k][j][i][m] = dt * rsd[k][j][i][m];
          }
        }
      }
    }
    if (timeron) timer_stop(t_rhs);

    for (k = 1; k < nz -1; k++) {
      //---------------------------------------------------------------------
      // form the lower triangular part of the jacobian matrix
      //---------------------------------------------------------------------
      if (timeron) timer_start(t_jacld);
      jacld(k);
      if (timeron) timer_stop(t_jacld);

      //---------------------------------------------------------------------
      // perform the lower triangular solution
      //---------------------------------------------------------------------
      if (timeron) timer_start(t_blts);
      blts( ISIZ1, ISIZ2, ISIZ3,
            nx, ny, nz, k,
            omega,
            rsd, 
            a, b, c, d,
            ist, iend, jst, jend, 
            nx0, ny0 );
      if (timeron) timer_stop(t_blts);
    }
 
    for (k = nz - 2; k > 0; k--) {
      //---------------------------------------------------------------------
      // form the strictly upper triangular part of the jacobian matrix
      //---------------------------------------------------------------------
      if (timeron) timer_start(t_jacu);
      jacu(k);
      if (timeron) timer_stop(t_jacu);

      //---------------------------------------------------------------------
      // perform the upper triangular solution
      //---------------------------------------------------------------------
      if (timeron) timer_start(t_buts);
      buts( ISIZ1, ISIZ2, ISIZ3,
            nx, ny, nz, k,
            omega,
            rsd, tv,
            d, a, b, c,
            ist, iend, jst, jend,
            nx0, ny0 );
      if (timeron) timer_stop(t_buts);
    }

    //---------------------------------------------------------------------
    // update the variables
    //---------------------------------------------------------------------
    if (timeron) timer_start(t_add);
    for (k = 1; k < nz-1; k++) {
      for (j = jst; j < jend; j++) {
        for (i = ist; i < iend; i++) {
          for (m = 0; m < 5; m++) {
            u[k][j][i][m] = u[k][j][i][m] + tmp * rsd[k][j][i][m];
          }
        }
      }
    }
    if (timeron) timer_stop(t_add);

    //---------------------------------------------------------------------
    // compute the max-norms of newton iteration corrections
    //---------------------------------------------------------------------
    if ( (istep % inorm) == 0 ) {
      if (timeron) timer_start(t_l2norm);
      l2norm( ISIZ1, ISIZ2, ISIZ3, nx0, ny0, nz0,
              ist, iend, jst, jend,
              rsd, delunm );
      if (timeron) timer_stop(t_l2norm);
      /*
      if ( ipr == 1 ) {
        printf(" \n RMS-norm of SSOR-iteration correction "
               "for first pde  = %12.5E\n"
               " RMS-norm of SSOR-iteration correction "
               "for second pde = %12.5E\n"
               " RMS-norm of SSOR-iteration correction "
               "for third pde  = %12.5E\n"
               " RMS-norm of SSOR-iteration correction "
               "for fourth pde = %12.5E\n",
               " RMS-norm of SSOR-iteration correction "
               "for fifth pde  = %12.5E\n", 
               delunm[0], delunm[1], delunm[2], delunm[3], delunm[4]); 
      } else if ( ipr == 2 ) {
        printf("(%5d,%15.6f)\n", istep, delunm[4]);
      }
      */
    }
 
    //---------------------------------------------------------------------
    // compute the steady-state residuals
    //---------------------------------------------------------------------
    rhs();
 
    //---------------------------------------------------------------------
    // compute the max-norms of newton iteration residuals
    //---------------------------------------------------------------------
    if ( ((istep % inorm ) == 0 ) || ( istep == itmax ) ) {
      if (timeron) timer_start(t_l2norm);
      l2norm( ISIZ1, ISIZ2, ISIZ3, nx0, ny0, nz0,
              ist, iend, jst, jend, rsd, rsdnm );
      if (timeron) timer_stop(t_l2norm);
      /*
      if ( ipr == 1 ) {
        printf(" \n RMS-norm of steady-state residual for "
               "first pde  = %12.5E\n"
               " RMS-norm of steady-state residual for "
               "second pde = %12.5E\n"
               " RMS-norm of steady-state residual for "
               "third pde  = %12.5E\n"
               " RMS-norm of steady-state residual for "
               "fourth pde = %12.5E\n"
               " RMS-norm of steady-state residual for "
               "fifth pde  = %12.5E\n", 
               rsdnm[0], rsdnm[1], rsdnm[2], rsdnm[3], rsdnm[4]);
      }
      */
    }

    //---------------------------------------------------------------------
    // check the newton-iteration residuals against the tolerance levels
    //---------------------------------------------------------------------
    if ( ( rsdnm[0] < tolrsd[0] ) && ( rsdnm[1] < tolrsd[1] ) &&
         ( rsdnm[2] < tolrsd[2] ) && ( rsdnm[3] < tolrsd[3] ) &&
         ( rsdnm[4] < tolrsd[4] ) ) {
      //if (ipr == 1 ) {
      printf(" \n convergence was achieved after %4d pseudo-time steps\n",
          istep);
      //}
      break;
    }
  }

  timer_stop(1);
  maxtime = timer_read(1);
}

