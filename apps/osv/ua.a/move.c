//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB UA code. This C        //
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

//---------------------------------------------------------------
// move element to proper location in morton space filling curve
//---------------------------------------------------------------
void move()
{
  int i, iside, jface, iel, ntemp, ii1, ii2, n1, n2, cb;

  n2 = 2*6*nelt;
  n1 = n2*2;
  nr_init((int *)sje_new, n1, -1);
  nr_init((int *)ijel_new, n2, -1);

  for (iel = 0; iel < nelt; iel++) {
    i = mt_to_id[iel];
    treenew[iel] = tree[i];
    copy(xc_new[iel], xc[i], 8);
    copy(yc_new[iel], yc[i], 8);
    copy(zc_new[iel], zc[i], 8);

    for (iside = 0; iside < NSIDES; iside++) {
      jface = jjface[iside];
      cb = cbc[i][iside];
      xc_new[iel][iside] = xc[i][iside];
      yc_new[iel][iside] = yc[i][iside];
      zc_new[iel][iside] = zc[i][iside];
      cbc_new[iel][iside] = cb;

      if (cb == 2) {
        ntemp = sje[i][iside][0][0];
        ijel_new[iel][iside][0] = 0;
        ijel_new[iel][iside][1] = 0;
        sje_new[iel][iside][0][0] = id_to_mt[ntemp];

      } else if (cb == 1) {
        ntemp = sje[i][iside][0][0];
        ijel_new[iel][iside][0] = ijel[i][iside][0];
        ijel_new[iel][iside][1] = ijel[i][iside][1];
        sje_new[iel][iside][0][0] = id_to_mt[ntemp];

      } else if (cb == 3) {
        for (ii2 = 0; ii2 < 2; ii2++) {
          for (ii1 = 0; ii1 < 2; ii1++) {
            ntemp = sje[i][iside][ii2][ii1];
            ijel_new[iel][iside][0] = 0;
            ijel_new[iel][iside][1] = 0;
            sje_new[iel][iside][ii2][ii1] = id_to_mt[ntemp];
          }
        }

      } else if (cb == 0) {
        sje_new[iel][iside][0][0] = -1;
        sje_new[iel][iside][1][0] = -1;
        sje_new[iel][iside][0][1] = -1;
        sje_new[iel][iside][1][1] = -1;
      } 
    }

    copy(ta2[iel][0][0], ta1[i][0][0], NXYZ);
  }

  copy((double *)xc, (double *)xc_new, 8*nelt);
  copy((double *)yc, (double *)yc_new, 8*nelt);
  copy((double *)zc, (double *)zc_new, 8*nelt);
  ncopy((int *)sje, (int *)sje_new, 4*6*nelt);
  ncopy((int *)ijel, (int *)ijel_new, 2*6*nelt);
  ncopy((int *)cbc, (int *)cbc_new, 6*nelt);
  ncopy((int *)tree, (int *)treenew, nelt);
  copy((double *)ta1, (double *)ta2, NXYZ*nelt);

  for (iel = 0; iel < nelt; iel++) {
    mt_to_id[iel] = iel;
    id_to_mt[iel] = iel;
  }
}
