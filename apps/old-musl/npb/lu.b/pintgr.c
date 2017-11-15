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

void pintgr()
{
  //---------------------------------------------------------------------
  // local variables
  //---------------------------------------------------------------------
  int i, j, k;
  int ibeg, ifin, ifin1;
  int jbeg, jfin, jfin1;
  double phi1[ISIZ3+2][ISIZ2+2];
  double phi2[ISIZ3+2][ISIZ2+2];
  double frc1, frc2, frc3;

  //---------------------------------------------------------------------
  // set up the sub-domains for integeration in each processor
  //---------------------------------------------------------------------
  ibeg = ii1;
  ifin = ii2;
  jbeg = ji1;
  jfin = ji2;
  ifin1 = ifin - 1;
  jfin1 = jfin - 1;

  //---------------------------------------------------------------------
  // initialize
  //---------------------------------------------------------------------
  for (k = 0; k <= ISIZ3+1; k++) {
    for (i = 0; i <= ISIZ2+1; i++) {
      phi1[k][i] = 0.0;
      phi2[k][i] = 0.0;
    }
  }

  for (j = jbeg; j < jfin; j++) {
    for (i = ibeg; i < ifin; i++) {
      k = ki1;

      phi1[j][i] = C2*(  u[k][j][i][4]
          - 0.50 * (  u[k][j][i][1] * u[k][j][i][1]
                    + u[k][j][i][2] * u[k][j][i][2]
                    + u[k][j][i][3] * u[k][j][i][3] )
                   / u[k][j][i][0] );

      k = ki2 - 1;

      phi2[j][i] = C2*(  u[k][j][i][4]
          - 0.50 * (  u[k][j][i][1] * u[k][j][i][1]
                    + u[k][j][i][2] * u[k][j][i][2]
                    + u[k][j][i][3] * u[k][j][i][3] )
                   / u[k][j][i][0] );
    }
  }

  frc1 = 0.0;
  for (j = jbeg; j < jfin1; j++) {
    for (i = ibeg; i < ifin1; i++) {
      frc1 = frc1 + (  phi1[j][i]
                     + phi1[j][i+1]
                     + phi1[j+1][i]
                     + phi1[j+1][i+1]
                     + phi2[j][i]
                     + phi2[j][i+1]
                     + phi2[j+1][i]
                     + phi2[j+1][i+1] );
    }
  }
  frc1 = dxi * deta * frc1;

  //---------------------------------------------------------------------
  // initialize
  //---------------------------------------------------------------------
  for (k = 0; k <= ISIZ3+1; k++) {
    for (i = 0; i <= ISIZ2+1; i++) {
      phi1[k][i] = 0.0;
      phi2[k][i] = 0.0;
    }
  }
  if (jbeg == ji1) {
    for (k = ki1; k < ki2; k++) {
      for (i = ibeg; i < ifin; i++) {
        phi1[k][i] = C2*(  u[k][jbeg][i][4]
            - 0.50 * (  u[k][jbeg][i][1] * u[k][jbeg][i][1]
                      + u[k][jbeg][i][2] * u[k][jbeg][i][2]
                      + u[k][jbeg][i][3] * u[k][jbeg][i][3] )
                     / u[k][jbeg][i][0] );
      }
    }
  }

  if (jfin == ji2) {
    for (k = ki1; k < ki2; k++) {
      for (i = ibeg; i < ifin; i++) {
        phi2[k][i] = C2*(  u[k][jfin-1][i][4]
            - 0.50 * (  u[k][jfin-1][i][1] * u[k][jfin-1][i][1]
                      + u[k][jfin-1][i][2] * u[k][jfin-1][i][2]
                      + u[k][jfin-1][i][3] * u[k][jfin-1][i][3] )
                     / u[k][jfin-1][i][0] );
      }
    }
  }

  frc2 = 0.0;
  for (k = ki1; k < ki2-1; k++) {
    for (i = ibeg; i < ifin1; i++) {
      frc2 = frc2 + (  phi1[k][i]
                     + phi1[k][i+1]
                     + phi1[k+1][i]
                     + phi1[k+1][i+1]
                     + phi2[k][i]
                     + phi2[k][i+1]
                     + phi2[k+1][i]
                     + phi2[k+1][i+1] );
    }
  }
  frc2 = dxi * dzeta * frc2;

  //---------------------------------------------------------------------
  // initialize
  //---------------------------------------------------------------------
  for (k = 0; k <= ISIZ3+1; k++) {
    for (i = 0; i <= ISIZ2+1; i++) {
      phi1[k][i] = 0.0;
      phi2[k][i] = 0.0;
    }
  }
  if (ibeg == ii1) {
    for (k = ki1; k < ki2; k++) {
      for (j = jbeg; j < jfin; j++) {
        phi1[k][j] = C2*(  u[k][j][ibeg][4]
            - 0.50 * (  u[k][j][ibeg][1] * u[k][j][ibeg][1]
                      + u[k][j][ibeg][2] * u[k][j][ibeg][2]
                      + u[k][j][ibeg][3] * u[k][j][ibeg][3] )
                     / u[k][j][ibeg][0] );
      }
    }
  }

  if (ifin == ii2) {
    for (k = ki1; k < ki2; k++) {
      for (j = jbeg; j < jfin; j++) {
        phi2[k][j] = C2*(  u[k][j][ifin-1][4]
            - 0.50 * (  u[k][j][ifin-1][1] * u[k][j][ifin-1][1]
                      + u[k][j][ifin-1][2] * u[k][j][ifin-1][2]
                      + u[k][j][ifin-1][3] * u[k][j][ifin-1][3] )
                     / u[k][j][ifin-1][0] );
      }
    }
  }

  frc3 = 0.0;
  for (k = ki1; k < ki2-1; k++) {
    for (j = jbeg; j < jfin1; j++) {
      frc3 = frc3 + (  phi1[k][j]
                     + phi1[k][j+1]
                     + phi1[k+1][j]
                     + phi1[k+1][j+1]
                     + phi2[k][j]
                     + phi2[k][j+1]
                     + phi2[k+1][j]
                     + phi2[k+1][j+1] );
    }
  }
  frc3 = deta * dzeta * frc3;

  frc = 0.25 * ( frc1 + frc2 + frc3 );
  //printf("\n\n     surface integral = %12.5E\n\n\n", frc);
}

