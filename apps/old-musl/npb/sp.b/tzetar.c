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
// block-diagonal matrix-vector multiplication                       
//---------------------------------------------------------------------
void tzetar()
{
  int i, j, k;
  double t1, t2, t3, ac, xvel, yvel, zvel, r1, r2, r3, r4, r5;
  double btuz, ac2u, uzik1;

  if (timeron) timer_start(t_tzetar);
  for (k = 1; k <= nz2; k++) {
    for (j = 1; j <= ny2; j++) {
      for (i = 1; i <= nx2; i++) {
        xvel = us[k][j][i];
        yvel = vs[k][j][i];
        zvel = ws[k][j][i];
        ac   = speed[k][j][i];

        ac2u = ac*ac;

        r1 = rhs[k][j][i][0];
        r2 = rhs[k][j][i][1];
        r3 = rhs[k][j][i][2];
        r4 = rhs[k][j][i][3];
        r5 = rhs[k][j][i][4];     

        uzik1 = u[k][j][i][0];
        btuz  = bt * uzik1;

        t1 = btuz/ac * (r4 + r5);
        t2 = r3 + t1;
        t3 = btuz * (r4 - r5);

        rhs[k][j][i][0] = t2;
        rhs[k][j][i][1] = -uzik1*r2 + xvel*t2;
        rhs[k][j][i][2] =  uzik1*r1 + yvel*t2;
        rhs[k][j][i][3] =  zvel*t2  + t3;
        rhs[k][j][i][4] =  uzik1*(-xvel*r2 + yvel*r1) + 
                           qs[k][j][i]*t2 + c2iv*ac2u*t1 + zvel*t3;
      }
    }
  }
  if (timeron) timer_stop(t_tzetar);
}

