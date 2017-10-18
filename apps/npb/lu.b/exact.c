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

#include "applu.incl"

//---------------------------------------------------------------------
//
//   compute the exact solution at (i,j,k)
//
//---------------------------------------------------------------------
void exact(int i, int j, int k, double u000ijk[])
{
  //---------------------------------------------------------------------
  // local variables
  //---------------------------------------------------------------------
  int m;
  double xi, eta, zeta;

  xi   = ( (double)i ) / ( nx0 - 1 );
  eta  = ( (double)j ) / ( ny0 - 1 );
  zeta = ( (double)k ) / ( nz - 1 );

  for (m = 0; m < 5; m++) {
    u000ijk[m] =  ce[m][0]
      + (ce[m][1]
      + (ce[m][4]
      + (ce[m][7]
      +  ce[m][10] * xi) * xi) * xi) * xi
      + (ce[m][2]
      + (ce[m][5]
      + (ce[m][8]
      +  ce[m][11] * eta) * eta) * eta) * eta
      + (ce[m][3]
      + (ce[m][6]
      + (ce[m][9]
      +  ce[m][12] * zeta) * zeta) * zeta) * zeta;
  }
}

