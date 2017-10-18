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
// compute the upper triangular part of the jacobian matrix
//---------------------------------------------------------------------
void jacu(int k)
{
  //---------------------------------------------------------------------
  // local variables
  //---------------------------------------------------------------------
  int i, j;
  double r43;
  double c1345;
  double c34;
  double tmp1, tmp2, tmp3;

  r43 = ( 4.0 / 3.0 );
  c1345 = C1 * C3 * C4 * C5;
  c34 = C3 * C4;

  for (j = jst; j < jend; j++) {
    for (i = ist; i < iend; i++) {
      //---------------------------------------------------------------------
      // form the block daigonal
      //---------------------------------------------------------------------
      tmp1 = rho_i[k][j][i];
      tmp2 = tmp1 * tmp1;
      tmp3 = tmp1 * tmp2;

      d[j][i][0][0] = 1.0 + dt * 2.0 * ( tx1 * dx1 + ty1 * dy1 + tz1 * dz1 );
      d[j][i][1][0] = 0.0;
      d[j][i][2][0] = 0.0;
      d[j][i][3][0] = 0.0;
      d[j][i][4][0] = 0.0;

      d[j][i][0][1] =  dt * 2.0
        * ( - tx1 * r43 - ty1 - tz1 )
        * ( c34 * tmp2 * u[k][j][i][1] );
      d[j][i][1][1] =  1.0
        + dt * 2.0 * c34 * tmp1 
        * (  tx1 * r43 + ty1 + tz1 )
        + dt * 2.0 * ( tx1 * dx2 + ty1 * dy2 + tz1 * dz2 );
      d[j][i][2][1] = 0.0;
      d[j][i][3][1] = 0.0;
      d[j][i][4][1] = 0.0;

      d[j][i][0][2] = dt * 2.0
        * ( - tx1 - ty1 * r43 - tz1 )
        * ( c34 * tmp2 * u[k][j][i][2] );
      d[j][i][1][2] = 0.0;
      d[j][i][2][2] = 1.0
        + dt * 2.0 * c34 * tmp1
        * (  tx1 + ty1 * r43 + tz1 )
        + dt * 2.0 * ( tx1 * dx3 + ty1 * dy3 + tz1 * dz3 );
      d[j][i][3][2] = 0.0;
      d[j][i][4][2] = 0.0;

      d[j][i][0][3] = dt * 2.0
        * ( - tx1 - ty1 - tz1 * r43 )
        * ( c34 * tmp2 * u[k][j][i][3] );
      d[j][i][1][3] = 0.0;
      d[j][i][2][3] = 0.0;
      d[j][i][3][3] = 1.0
        + dt * 2.0 * c34 * tmp1
        * (  tx1 + ty1 + tz1 * r43 )
        + dt * 2.0 * ( tx1 * dx4 + ty1 * dy4 + tz1 * dz4 );
      d[j][i][4][3] = 0.0;

      d[j][i][0][4] = -dt * 2.0
        * ( ( ( tx1 * ( r43*c34 - c1345 )
                + ty1 * ( c34 - c1345 )
                + tz1 * ( c34 - c1345 ) ) * ( u[k][j][i][1]*u[k][j][i][1] )
              + ( tx1 * ( c34 - c1345 )
                + ty1 * ( r43*c34 - c1345 )
                + tz1 * ( c34 - c1345 ) ) * ( u[k][j][i][2]*u[k][j][i][2] )
              + ( tx1 * ( c34 - c1345 )
                + ty1 * ( c34 - c1345 )
                + tz1 * ( r43*c34 - c1345 ) ) * (u[k][j][i][3]*u[k][j][i][3])
            ) * tmp3
            + ( tx1 + ty1 + tz1 ) * c1345 * tmp2 * u[k][j][i][4] );

      d[j][i][1][4] = dt * 2.0
        * ( tx1 * ( r43*c34 - c1345 )
          + ty1 * (     c34 - c1345 )
          + tz1 * (     c34 - c1345 ) ) * tmp2 * u[k][j][i][1];
      d[j][i][2][4] = dt * 2.0
        * ( tx1 * ( c34 - c1345 )
          + ty1 * ( r43*c34 -c1345 )
          + tz1 * ( c34 - c1345 ) ) * tmp2 * u[k][j][i][2];
      d[j][i][3][4] = dt * 2.0
        * ( tx1 * ( c34 - c1345 )
          + ty1 * ( c34 - c1345 )
          + tz1 * ( r43*c34 - c1345 ) ) * tmp2 * u[k][j][i][3];
      d[j][i][4][4] = 1.0
        + dt * 2.0 * ( tx1 + ty1 + tz1 ) * c1345 * tmp1
        + dt * 2.0 * ( tx1 * dx5 + ty1 * dy5 + tz1 * dz5 );

      //---------------------------------------------------------------------
      // form the first block sub-diagonal
      //---------------------------------------------------------------------
      tmp1 = rho_i[k][j][i+1];
      tmp2 = tmp1 * tmp1;
      tmp3 = tmp1 * tmp2;

      a[j][i][0][0] = - dt * tx1 * dx1;
      a[j][i][1][0] =   dt * tx2;
      a[j][i][2][0] =   0.0;
      a[j][i][3][0] =   0.0;
      a[j][i][4][0] =   0.0;

      a[j][i][0][1] =  dt * tx2
        * ( - ( u[k][j][i+1][1] * tmp1 ) * ( u[k][j][i+1][1] * tmp1 )
            + C2 * qs[k][j][i+1] * tmp1 )
        - dt * tx1 * ( - r43 * c34 * tmp2 * u[k][j][i+1][1] );
      a[j][i][1][1] =  dt * tx2
        * ( ( 2.0 - C2 ) * ( u[k][j][i+1][1] * tmp1 ) )
        - dt * tx1 * ( r43 * c34 * tmp1 )
        - dt * tx1 * dx2;
      a[j][i][2][1] =  dt * tx2
        * ( - C2 * ( u[k][j][i+1][2] * tmp1 ) );
      a[j][i][3][1] =  dt * tx2
        * ( - C2 * ( u[k][j][i+1][3] * tmp1 ) );
      a[j][i][4][1] =  dt * tx2 * C2 ;

      a[j][i][0][2] =  dt * tx2
        * ( - ( u[k][j][i+1][1] * u[k][j][i+1][2] ) * tmp2 )
        - dt * tx1 * ( - c34 * tmp2 * u[k][j][i+1][2] );
      a[j][i][1][2] =  dt * tx2 * ( u[k][j][i+1][2] * tmp1 );
      a[j][i][2][2] =  dt * tx2 * ( u[k][j][i+1][1] * tmp1 )
        - dt * tx1 * ( c34 * tmp1 )
        - dt * tx1 * dx3;
      a[j][i][3][2] = 0.0;
      a[j][i][4][2] = 0.0;

      a[j][i][0][3] = dt * tx2
        * ( - ( u[k][j][i+1][1]*u[k][j][i+1][3] ) * tmp2 )
        - dt * tx1 * ( - c34 * tmp2 * u[k][j][i+1][3] );
      a[j][i][1][3] = dt * tx2 * ( u[k][j][i+1][3] * tmp1 );
      a[j][i][2][3] = 0.0;
      a[j][i][3][3] = dt * tx2 * ( u[k][j][i+1][1] * tmp1 )
        - dt * tx1 * ( c34 * tmp1 )
        - dt * tx1 * dx4;
      a[j][i][4][3] = 0.0;

      a[j][i][0][4] = dt * tx2
        * ( ( C2 * 2.0 * qs[k][j][i+1]
            - C1 * u[k][j][i+1][4] )
        * ( u[k][j][i+1][1] * tmp2 ) )
        - dt * tx1
        * ( - ( r43*c34 - c1345 ) * tmp3 * ( u[k][j][i+1][1]*u[k][j][i+1][1] )
            - (     c34 - c1345 ) * tmp3 * ( u[k][j][i+1][2]*u[k][j][i+1][2] )
            - (     c34 - c1345 ) * tmp3 * ( u[k][j][i+1][3]*u[k][j][i+1][3] )
            - c1345 * tmp2 * u[k][j][i+1][4] );
      a[j][i][1][4] = dt * tx2
        * ( C1 * ( u[k][j][i+1][4] * tmp1 )
            - C2
            * ( u[k][j][i+1][1]*u[k][j][i+1][1] * tmp2
              + qs[k][j][i+1] * tmp1 ) )
        - dt * tx1
        * ( r43*c34 - c1345 ) * tmp2 * u[k][j][i+1][1];
      a[j][i][2][4] = dt * tx2
        * ( - C2 * ( u[k][j][i+1][2]*u[k][j][i+1][1] ) * tmp2 )
        - dt * tx1
        * (  c34 - c1345 ) * tmp2 * u[k][j][i+1][2];
      a[j][i][3][4] = dt * tx2
        * ( - C2 * ( u[k][j][i+1][3]*u[k][j][i+1][1] ) * tmp2 )
        - dt * tx1
        * (  c34 - c1345 ) * tmp2 * u[k][j][i+1][3];
      a[j][i][4][4] = dt * tx2
        * ( C1 * ( u[k][j][i+1][1] * tmp1 ) )
        - dt * tx1 * c1345 * tmp1
        - dt * tx1 * dx5;

      //---------------------------------------------------------------------
      // form the second block sub-diagonal
      //---------------------------------------------------------------------
      tmp1 = rho_i[k][j+1][i];
      tmp2 = tmp1 * tmp1;
      tmp3 = tmp1 * tmp2;

      b[j][i][0][0] = - dt * ty1 * dy1;
      b[j][i][1][0] =   0.0;
      b[j][i][2][0] =  dt * ty2;
      b[j][i][3][0] =   0.0;
      b[j][i][4][0] =   0.0;

      b[j][i][0][1] =  dt * ty2
        * ( - ( u[k][j+1][i][1]*u[k][j+1][i][2] ) * tmp2 )
        - dt * ty1 * ( - c34 * tmp2 * u[k][j+1][i][1] );
      b[j][i][1][1] =  dt * ty2 * ( u[k][j+1][i][2] * tmp1 )
        - dt * ty1 * ( c34 * tmp1 )
        - dt * ty1 * dy2;
      b[j][i][2][1] =  dt * ty2 * ( u[k][j+1][i][1] * tmp1 );
      b[j][i][3][1] = 0.0;
      b[j][i][4][1] = 0.0;

      b[j][i][0][2] =  dt * ty2
        * ( - ( u[k][j+1][i][2] * tmp1 ) * ( u[k][j+1][i][2] * tmp1 )
            + C2 * ( qs[k][j+1][i] * tmp1 ) )
        - dt * ty1 * ( - r43 * c34 * tmp2 * u[k][j+1][i][2] );
      b[j][i][1][2] =  dt * ty2
        * ( - C2 * ( u[k][j+1][i][1] * tmp1 ) );
      b[j][i][2][2] =  dt * ty2 * ( ( 2.0 - C2 )
          * ( u[k][j+1][i][2] * tmp1 ) )
        - dt * ty1 * ( r43 * c34 * tmp1 )
        - dt * ty1 * dy3;
      b[j][i][3][2] =  dt * ty2
        * ( - C2 * ( u[k][j+1][i][3] * tmp1 ) );
      b[j][i][4][2] =  dt * ty2 * C2;

      b[j][i][0][3] =  dt * ty2
        * ( - ( u[k][j+1][i][2]*u[k][j+1][i][3] ) * tmp2 )
        - dt * ty1 * ( - c34 * tmp2 * u[k][j+1][i][3] );
      b[j][i][1][3] = 0.0;
      b[j][i][2][3] =  dt * ty2 * ( u[k][j+1][i][3] * tmp1 );
      b[j][i][3][3] =  dt * ty2 * ( u[k][j+1][i][2] * tmp1 )
        - dt * ty1 * ( c34 * tmp1 )
        - dt * ty1 * dy4;
      b[j][i][4][3] = 0.0;

      b[j][i][0][4] =  dt * ty2
        * ( ( C2 * 2.0 * qs[k][j+1][i]
            - C1 * u[k][j+1][i][4] )
        * ( u[k][j+1][i][2] * tmp2 ) )
        - dt * ty1
        * ( - (     c34 - c1345 )*tmp3*(u[k][j+1][i][1]*u[k][j+1][i][1])
            - ( r43*c34 - c1345 )*tmp3*(u[k][j+1][i][2]*u[k][j+1][i][2])
            - (     c34 - c1345 )*tmp3*(u[k][j+1][i][3]*u[k][j+1][i][3])
            - c1345*tmp2*u[k][j+1][i][4] );
      b[j][i][1][4] =  dt * ty2
        * ( - C2 * ( u[k][j+1][i][1]*u[k][j+1][i][2] ) * tmp2 )
        - dt * ty1
        * ( c34 - c1345 ) * tmp2 * u[k][j+1][i][1];
      b[j][i][2][4] =  dt * ty2
        * ( C1 * ( u[k][j+1][i][4] * tmp1 )
            - C2 
            * ( qs[k][j+1][i] * tmp1
              + u[k][j+1][i][2]*u[k][j+1][i][2] * tmp2 ) )
        - dt * ty1
        * ( r43*c34 - c1345 ) * tmp2 * u[k][j+1][i][2];
      b[j][i][3][4] =  dt * ty2
        * ( - C2 * ( u[k][j+1][i][2]*u[k][j+1][i][3] ) * tmp2 )
        - dt * ty1 * ( c34 - c1345 ) * tmp2 * u[k][j+1][i][3];
      b[j][i][4][4] =  dt * ty2
        * ( C1 * ( u[k][j+1][i][2] * tmp1 ) )
        - dt * ty1 * c1345 * tmp1
        - dt * ty1 * dy5;

      //---------------------------------------------------------------------
      // form the third block sub-diagonal
      //---------------------------------------------------------------------
      tmp1 = rho_i[k+1][j][i];
      tmp2 = tmp1 * tmp1;
      tmp3 = tmp1 * tmp2;

      c[j][i][0][0] = - dt * tz1 * dz1;
      c[j][i][1][0] =   0.0;
      c[j][i][2][0] =   0.0;
      c[j][i][3][0] = dt * tz2;
      c[j][i][4][0] =   0.0;

      c[j][i][0][1] = dt * tz2
        * ( - ( u[k+1][j][i][1]*u[k+1][j][i][3] ) * tmp2 )
        - dt * tz1 * ( - c34 * tmp2 * u[k+1][j][i][1] );
      c[j][i][1][1] = dt * tz2 * ( u[k+1][j][i][3] * tmp1 )
        - dt * tz1 * c34 * tmp1
        - dt * tz1 * dz2;
      c[j][i][2][1] = 0.0;
      c[j][i][3][1] = dt * tz2 * ( u[k+1][j][i][1] * tmp1 );
      c[j][i][4][1] = 0.0;

      c[j][i][0][2] = dt * tz2
        * ( - ( u[k+1][j][i][2]*u[k+1][j][i][3] ) * tmp2 )
        - dt * tz1 * ( - c34 * tmp2 * u[k+1][j][i][2] );
      c[j][i][1][2] = 0.0;
      c[j][i][2][2] = dt * tz2 * ( u[k+1][j][i][3] * tmp1 )
        - dt * tz1 * ( c34 * tmp1 )
        - dt * tz1 * dz3;
      c[j][i][3][2] = dt * tz2 * ( u[k+1][j][i][2] * tmp1 );
      c[j][i][4][2] = 0.0;

      c[j][i][0][3] = dt * tz2
        * ( - ( u[k+1][j][i][3] * tmp1 ) * ( u[k+1][j][i][3] * tmp1 )
            + C2 * ( qs[k+1][j][i] * tmp1 ) )
        - dt * tz1 * ( - r43 * c34 * tmp2 * u[k+1][j][i][3] );
      c[j][i][1][3] = dt * tz2
        * ( - C2 * ( u[k+1][j][i][1] * tmp1 ) );
      c[j][i][2][3] = dt * tz2
        * ( - C2 * ( u[k+1][j][i][2] * tmp1 ) );
      c[j][i][3][3] = dt * tz2 * ( 2.0 - C2 )
        * ( u[k+1][j][i][3] * tmp1 )
        - dt * tz1 * ( r43 * c34 * tmp1 )
        - dt * tz1 * dz4;
      c[j][i][4][3] = dt * tz2 * C2;

      c[j][i][0][4] = dt * tz2
        * ( ( C2 * 2.0 * qs[k+1][j][i]
            - C1 * u[k+1][j][i][4] )
                 * ( u[k+1][j][i][3] * tmp2 ) )
        - dt * tz1
        * ( - ( c34 - c1345 ) * tmp3 * (u[k+1][j][i][1]*u[k+1][j][i][1])
            - ( c34 - c1345 ) * tmp3 * (u[k+1][j][i][2]*u[k+1][j][i][2])
            - ( r43*c34 - c1345 )* tmp3 * (u[k+1][j][i][3]*u[k+1][j][i][3])
            - c1345 * tmp2 * u[k+1][j][i][4] );
      c[j][i][1][4] = dt * tz2
        * ( - C2 * ( u[k+1][j][i][1]*u[k+1][j][i][3] ) * tmp2 )
        - dt * tz1 * ( c34 - c1345 ) * tmp2 * u[k+1][j][i][1];
      c[j][i][2][4] = dt * tz2
        * ( - C2 * ( u[k+1][j][i][2]*u[k+1][j][i][3] ) * tmp2 )
        - dt * tz1 * ( c34 - c1345 ) * tmp2 * u[k+1][j][i][2];
      c[j][i][3][4] = dt * tz2
        * ( C1 * ( u[k+1][j][i][4] * tmp1 )
            - C2
            * ( qs[k+1][j][i] * tmp1
              + u[k+1][j][i][3]*u[k+1][j][i][3] * tmp2 ) )
        - dt * tz1 * ( r43*c34 - c1345 ) * tmp2 * u[k+1][j][i][3];
      c[j][i][4][4] = dt * tz2
        * ( C1 * ( u[k+1][j][i][3] * tmp1 ) )
        - dt * tz1 * c1345 * tmp1
        - dt * tz1 * dz5;
    }
  }
}

