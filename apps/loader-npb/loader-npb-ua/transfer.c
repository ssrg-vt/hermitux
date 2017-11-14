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

//------------------------------------------------------------------
// Map values from mortar(tmor) to element(tx)
//------------------------------------------------------------------
void transf(double tmor[], double tx[])
{
  double tmp[2][LX1][LX1];
  int ig1, ig2, ig3, ig4, ie, iface, il1, il2, il3, il4;
  int nnje, ije1, ije2, col, i, j, ig, il;

  // zero out tx on element boundaries
  col2(tx, (double *)tmult, ntot);

  for (ie = 0; ie < nelt; ie++) {
    for (iface = 0; iface < NSIDES; iface++) {
      // get the collocation point index of the four local corners on the
      // face iface of element ie
      il1 = idel[ie][iface][0][0];
      il2 = idel[ie][iface][0][LX1-1];
      il3 = idel[ie][iface][LX1-1][0];
      il4 = idel[ie][iface][LX1-1][LX1-1];

      // get the mortar indices of the four local corners
      ig1 = idmo[ie][iface][0][0][0][0];
      ig2 = idmo[ie][iface][1][0][0][LX1-1];
      ig3 = idmo[ie][iface][0][1][LX1-1][0];
      ig4 = idmo[ie][iface][1][1][LX1-1][LX1-1];

      // copy the value from tmor to tx for these four local corners
      tx[il1] = tmor[ig1];
      tx[il2] = tmor[ig2];
      tx[il3] = tmor[ig3];
      tx[il4] = tmor[ig4];

      // nnje=1 for conforming faces, nnje=2 for nonconforming faces
      if (cbc[ie][iface] == 3) {
        nnje = 2;
      } else {
        nnje = 1;
      }

      // for nonconforming faces
      if (nnje == 2) {
        // nonconforming faces have four pieces of mortar, first map them to
        // two intermediate mortars, stored in tmp
        r_init((double *)tmp, LX1*LX1*2, 0.0);

        for (ije1 = 0; ije1 < nnje; ije1++) {
          for (ije2 = 0; ije2 < nnje; ije2++) {
            for (col = 0; col < LX1; col++) {
              // in each row col, when coloumn i=1 or LX1, the value
              // in tmor is copied to tmp
              i = v_end[ije2];
              ig = idmo[ie][iface][ije2][ije1][col][i];
              tmp[ije1][col][i] = tmor[ig];

              // in each row col, value in the interior three collocation
              // points is computed by apply mapping matrix qbnew to tmor
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][col][i];
                for (j = 0; j < LX1; j++) {
                  ig = idmo[ie][iface][ije2][ije1][col][j];
                  tmp[ije1][col][i] = tmp[ije1][col][i] +
                    qbnew[ije2][j][i-1]*tmor[ig];
                }
              }
            }
          }
        }

        // mapping from two pieces of intermediate mortar tmp to element
        // face tx
        for (ije1 = 0; ije1 < nnje; ije1++) {
          // the first column, col=0, is an edge of face iface.
          // the value on the three interior collocation points, tx, is
          // computed by applying mapping matrices qbnew to tmp.
          // the mapping result is divided by 2, because there will be
          // duplicated contribution from another face sharing this edge.
          col = 0;
          for (i = 1; i < LX1-1; i++) {
            il= idel[ie][iface][i][col];
            for (j = 0; j < LX1; j++) {
              tx[il] = tx[il] + qbnew[ije1][j][i-1]*
                tmp[ije1][j][col]*0.5;
            }
          }

          // for column 1 ~ lx-2
          for (col = 1; col < LX1-1; col++) {
            //when i=0 or LX1-1, the collocation points are also on an edge of
            // the face, so the mapping result also needs to be divided by 2
            i = v_end[ije1];
            il = idel[ie][iface][i][col];
            tx[il] = tx[il]+tmp[ije1][i][col]*0.5;

            // compute the value at interior collocation points in
            // columns 1 ~ LX1-1
            for (i = 1; i < LX1-1; i++) {
              il = idel[ie][iface][i][col];
              for (j = 0; j < LX1; j++) {
                tx[il] = tx[il] + qbnew[ije1][j][i-1]* tmp[ije1][j][col];
              }
            }
          }

          // same as col=0
          col = LX1-1;
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][i][col];
            for (j = 0; j < LX1; j++) {
              tx[il] = tx[il] + qbnew[ije1][j][i-1]*
                tmp[ije1][j][col]*0.5;
            }
          }
        }

        // for conforming faces
      } else {
        // face interior
        for (col = 1; col < LX1-1; col++) {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][col][i];
            ig = idmo[ie][iface][0][0][col][i];
            tx[il] = tmor[ig];
          }
        }

        // edges of conforming faces

        // if local edge 0 is a nonconforming edge
        if (idmo[ie][iface][0][0][0][LX1-1] != -1) {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][0][i];
            for (ije1 = 0; ije1 < 2; ije1++) {
              for (j = 0; j < LX1; j++) {
                ig = idmo[ie][iface][ije1][0][0][j];
                tx[il] = tx[il] + qbnew[ije1][j][i-1]*tmor[ig]*0.5;
              }
            }
          }

          // if local edge 0 is a conforming edge
        } else {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][0][i];
            ig = idmo[ie][iface][0][0][0][i];
            tx[il] = tmor[ig];
          }
        }

        // if local edge 1 is a nonconforming edge
        if (idmo[ie][iface][1][0][1][LX1-1] != -1) {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][i][LX1-1];
            for (ije1 = 0; ije1 < 2; ije1++) {
              for (j = 0; j < LX1; j++) {
                ig = idmo[ie][iface][1][ije1][j][LX1-1];
                tx[il] = tx[il] + qbnew[ije1][j][i-1]*tmor[ig]*0.5;
              }
            }
          }

          // if local edge 1 is a conforming edge
        } else {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][i][LX1-1];
            ig = idmo[ie][iface][0][0][i][LX1-1];
            tx[il] = tmor[ig];
          }
        }

        // if local edge 2 is a nonconforming edge
        if (idmo[ie][iface][0][1][LX1-1][1] != -1) {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][LX1-1][i];
            for (ije1 = 0; ije1 < 2; ije1++) {
              for (j = 0; j < LX1; j++) {
                ig = idmo[ie][iface][ije1][1][LX1-1][j];
                tx[il] = tx[il] + qbnew[ije1][j][i-1]*tmor[ig]*0.5;
              }
            }
          }

          // if local edge 2 is a conforming edge
        } else {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][LX1-1][i];
            ig = idmo[ie][iface][0][0][LX1-1][i];
            tx[il] = tmor[ig];
          }
        }

        // if local edge 3 is a nonconforming edge
        if (idmo[ie][iface][0][0][LX1-1][0] != -1) {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][i][0];
            for (ije1 = 0; ije1 < 2; ije1++) {
              for (j = 0; j < LX1; j++) {
                ig = idmo[ie][iface][0][ije1][j][0];
                tx[il] = tx[il] + qbnew[ije1][j][i-1]*tmor[ig]*0.5;
              }
            }
          }
          // if local edge 3 is a conforming edge
        } else {
          for (i = 1; i < LX1-1; i++) {
            il = idel[ie][iface][i][0];
            ig = idmo[ie][iface][0][0][i][0];
            tx[il] = tmor[ig];
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------
// Map from element(tx) to mortar(tmor).
// tmor sums contributions from all elements.
//------------------------------------------------------------------
void transfb(double tmor[], double tx[])
{
  const double third = 1.0/3.0;
  int shift;

  double tmp, tmp1, temp[2][LX1][LX1], top[2][LX1];
  int il1, il2, il3, il4, ig1, ig2, ig3, ig4, ie, iface, nnje;
  int ije1, ije2, col, i, j, ije, ig, il;

  r_init(tmor, nmor, 0.0);

  for (ie = 0; ie < nelt; ie++) {
    for (iface = 0; iface < NSIDES; iface++) {
      // nnje=1 for conforming faces, nnje=2 for nonconforming faces
      if (cbc[ie][iface] == 3) {
        nnje = 2;
      } else {
        nnje = 1;
      }

      // get collocation point index of four local corners on the face
      il1 = idel[ie][iface][0][0];
      il2 = idel[ie][iface][0][LX1-1];
      il3 = idel[ie][iface][LX1-1][0];
      il4 = idel[ie][iface][LX1-1][LX1-1];

      // get the mortar indices of the four local corners
      ig1 = idmo[ie][iface][0][0][0][0];
      ig2 = idmo[ie][iface][1][0][0][LX1-1];
      ig3 = idmo[ie][iface][0][1][LX1-1][0];
      ig4 = idmo[ie][iface][1][1][LX1-1][LX1-1];

      // sum the values from tx to tmor for these four local corners
      // only 1/3 of the value is summed, since there will be two duplicated
      // contributions from the other two faces sharing this vertex
      tmor[ig1] = tmor[ig1]+tx[il1]*third;
      tmor[ig2] = tmor[ig2]+tx[il2]*third;
      tmor[ig3] = tmor[ig3]+tx[il3]*third;
      tmor[ig4] = tmor[ig4]+tx[il4]*third;

      // for nonconforming faces
      if (nnje == 2) {
        r_init((double *)temp, LX1*LX1*2, 0.0);

        // nonconforming faces have four pieces of mortar, first map tx to
        // two intermediate mortars stored in temp
        for (ije2 = 0; ije2 < nnje; ije2++) {
          shift = ije2;
          for (col = 0; col < LX1; col++) {
            // For mortar points on face edge (top and bottom), copy the
            // value from tx to temp
            il = idel[ie][iface][v_end[ije2]][col];
            temp[ije2][v_end[ije2]][col] = tx[il];

            // For mortar points on face edge (top and bottom), calculate
            // the interior points' contribution to them, i.e. top()
            j = v_end[ije2];
            tmp = 0.0;
            for (i = 1; i < LX1-1; i++) {
              il = idel[ie][iface][i][col];
              tmp = tmp + qbnew[ije2][j][i-1]*tx[il];
            }

            top[ije2][col] = tmp;

            // Use mapping matrices qbnew to map the value from tx to temp
            // for mortar points not on the top bottom face edge.
            for (j = 2-shift-1; j < LX1-shift; j++) {
              tmp = 0.0;
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][i][col];
                tmp = tmp + qbnew[ije2][j][i-1]*tx[il];
              };
              temp[ije2][j][col] = tmp + temp[ije2][j][col];
            }
          }
        }

        // mapping from temp to tmor
        for (ije1 = 0; ije1 < nnje; ije1++) {
          shift = ije1;
          for (ije2 = 0; ije2 < nnje; ije2++) {

            // for each column of collocation points on a piece of mortar
            for (col = 2-shift-1; col < LX1-shift; col++) {

              // For the end point, which is on an edge (local edge 1,3),
              // the contribution is halved since there will be duplicated
              // contribution from another face sharing this edge.

              ig = idmo[ie][iface][ije2][ije1][col][v_end[ije2]];
              tmor[ig] = tmor[ig]+temp[ije1][col][v_end[ije2]]*0.5;

              // In each row of collocation points on a piece of mortar,
              // sum the contributions from interior collocation points
              // (i=1,LX1-2)
              for (j = 0; j < LX1; j++) {
                tmp = 0.0;
                for (i = 1; i < LX1-1; i++) {
                  tmp = tmp + qbnew[ije2][j][i-1] * temp[ije1][col][i];
                }
                ig = idmo[ie][iface][ije2][ije1][col][j];
                tmor[ig] = tmor[ig]+tmp;
              }
            }

            // For tmor on local edge 0 and 2, tmp is the contribution from
            // an edge, so it is halved because of duplicated contribution
            // from another face sharing this edge. tmp1 is contribution
            // from face interior.

            col = v_end[ije1];
            ig = idmo[ie][iface][ije2][ije1][col][v_end[ije2]];
            tmor[ig] = tmor[ig]+top[ije1][v_end[ije2]]*0.5;
            for (j = 0; j < LX1; j++) {
              tmp = 0.0;
              tmp1 = 0.0;
              for (i = 1; i < LX1-1; i++) {
                tmp  = tmp  + qbnew[ije2][j][i-1] * temp[ije1][col][i];
                tmp1 = tmp1 + qbnew[ije2][j][i-1] * top[ije1][i];
              }
              ig = idmo[ie][iface][ije2][ije1][col][j];
              tmor[ig] = tmor[ig]+tmp*0.5+tmp1;
            }
          }
        }

        // for conforming faces
      } else {

        // face interior
        for (col = 1; col < LX1-1; col++) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][col][j];
            ig = idmo[ie][iface][0][0][col][j];
            tmor[ig] = tmor[ig]+tx[il];
          }
        }

        // edges of conforming faces

        // if local edge 0 is a nonconforming edge
        if (idmo[ie][iface][0][0][0][LX1-1] != -1) {
          for (ije = 0; ije < 2; ije++) {
            for (j = 0; j < LX1; j++) {
              tmp = 0.0;
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][0][i];
                tmp= tmp + qbnew[ije][j][i-1]*tx[il];
              }
              ig = idmo[ie][iface][ije][0][0][j];
              tmor[ig] = tmor[ig]+tmp*0.5;
            }
          }

          // if local edge 0 is a conforming edge
        } else {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][0][j];
            ig = idmo[ie][iface][0][0][0][j];
            tmor[ig] = tmor[ig]+tx[il]*0.5;
          }
        }

        // if local edge 1 is a nonconforming edge
        if (idmo[ie][iface][1][0][1][LX1-1] != -1) {
          for (ije = 0; ije < 2; ije++) {
            for (j = 0; j < LX1; j++) {
              tmp = 0.0;
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][i][LX1-1];
                tmp = tmp + qbnew[ije][j][i-1]*tx[il];
              }
              ig = idmo[ie][iface][1][ije][j][LX1-1];
              tmor[ig] = tmor[ig]+tmp*0.5;
            }
          }

          // if local edge 1 is a conforming edge
        } else {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][LX1-1];
            ig = idmo[ie][iface][0][0][j][LX1-1];
            tmor[ig] = tmor[ig]+tx[il]*0.5;
          }
        }

        // if local edge 2 is a nonconforming edge
        if (idmo[ie][iface][0][1][LX1-1][1] != -1) {
          for (ije = 0; ije < 2; ije++) {
            for (j = 0; j < LX1; j++) {
              tmp = 0.0;
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][LX1-1][i];
                tmp = tmp + qbnew[ije][j][i-1]*tx[il];
              }
              ig = idmo[ie][iface][ije][1][LX1-1][j];
              tmor[ig] = tmor[ig]+tmp*0.5;
            }
          }

          // if local edge 2 is a conforming edge
        } else {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][LX1-1][j];
            ig = idmo[ie][iface][0][0][LX1-1][j];
            tmor[ig] = tmor[ig]+tx[il]*0.5;
          }
        }

        // if local edge 3 is a nonconforming edge
        if (idmo[ie][iface][0][0][LX1-1][0] != -1) {
          for (ije = 0; ije < 2; ije++) {
            for (j = 0; j < LX1; j++) {
              tmp = 0.0;
              for (i = 1; i < LX1-1; i++) {
                il = idel[ie][iface][i][0];
                tmp = tmp + qbnew[ije][j][i-1]*tx[il];
              }
              ig = idmo[ie][iface][0][ije][j][0];
              tmor[ig] = tmor[ig]+tmp*0.5;
            }
          }

          // if local edge 3 is a conforming edge
        } else {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][0];
            ig = idmo[ie][iface][0][0][j][0];
            tmor[ig] = tmor[ig]+tx[il]*0.5;
          }
        }
      } //nnje=1
    }
  }
}


//--------------------------------------------------------------
// This subroutine performs the edge to mortar mapping and
// calculates the mapping result on the mortar point at a vertex
// under situation 1,2, or 3.
// n refers to the configuration of three edges sharing a vertex,
// n = 1: only one edge is nonconforming
// n = 2: two edges are nonconforming
// n = 3: three edges are nonconforming
//-------------------------------------------------------------------
void transfb_cor_e(int n, double *tmor, double tx[LX1][LX1][LX1])
{
  double tmp;
  int i;

  tmp = tx[0][0][0];

  for (i = 1; i < LX1-1; i++) {
    tmp = tmp + qbnew[0][0][i-1]*tx[0][0][i];
  }

  if (n > 1) {
    for (i = 1; i < LX1-1; i++) {
      tmp = tmp + qbnew[0][0][i-1]*tx[0][i][0];
    }
  }

  if (n == 3) {
    for (i = 1; i < LX1-1; i++) {
      tmp = tmp + qbnew[0][0][i-1]*tx[i][0][0];
    }
  }

  *tmor = tmp;
}


//--------------------------------------------------------------
// This subroutine performs the mapping from face to mortar.
// Output tmor is the mapping result on a mortar vertex
// of situations of three edges and three faces sharing a vertex:
// n=4: only one face is nonconforming
// n=5: one face and one edge are nonconforming
// n=6: two faces are nonconforming
// n=7: three faces are nonconforming
//--------------------------------------------------------------
void transfb_cor_f(int n, double *tmor, double tx[LX1][LX1][LX1])
{
  double temp[LX1], tmp;
  int col, i;

  r_init(temp, LX1, 0.0);

  for (col = 0; col < LX1; col++) {
    temp[col] = tx[0][0][col];
    for (i = 1; i < LX1-1; i++) {
      temp[col] = temp[col] + qbnew[0][0][i-1]*tx[0][i][col];
    }
  }
  tmp = temp[0];

  for (i = 1; i < LX1-1; i++) {
    tmp = tmp + qbnew[0][0][i-1] *temp[i];
  }

  if (n == 5) {
    for (i = 1; i < LX1-1; i++) {
      tmp = tmp + qbnew[0][0][i-1] *tx[i][0][0];
    }
  }

  if (n >= 6) {
    r_init(temp, LX1, 0.0);
    for (col = 0; col < LX1; col++) {
      for (i = 1; i < LX1-1; i++) {
        temp[col] = temp[col] + qbnew[0][0][i-1]*tx[i][0][col];
      }
    }
    tmp = tmp+temp[0];
    for (i = 1; i < LX1-1; i++) {
      tmp = tmp +qbnew[0][0][i-1] *temp[i];
    }
  }

  if (n == 7) {
    r_init(temp, LX1, 0.0);
    for (col = 1; col < LX1-1; col++) {
      for (i = 1; i < LX1-1; i++) {
        temp[col] = temp[col] + qbnew[0][0][i-1]*tx[i][col][0];
      }
    }
    for (i = 1; i < LX1-1; i++) {
      tmp = tmp + qbnew[0][0][i-1] *temp[i];
    }
  }

  *tmor = tmp;
}


//------------------------------------------------------------------------
// Perform mortar to element mapping on a nonconforming face.
// This subroutin is used when all entries in tmor are zero except
// one tmor[j][i]=1. So this routine is simplified. Only one piece of
// mortar  (tmor only has two indices) and one piece of intermediate
// mortar (tmp) are involved.
//------------------------------------------------------------------------
void transf_nc(double tmor[LX1][LX1], double tx[LX1][LX1])
{
  double tmp[LX1][LX1];
  int col, i, j;

  r_init((double *)tmp, LX1*LX1, 0.0);
  for (col = 0; col < LX1; col++) {
    i = 0;
    tmp[col][i] = tmor[col][i];
    for (i = 1; i < LX1-1; i++) {
      for (j = 0; j < LX1; j++) {
        tmp[col][i] = tmp[col][i] + qbnew[0][j][i-1]*tmor[col][j];
      }
    }
  }

  for (col = 0; col < LX1; col++) {
    i = 0;
    tx[i][col] = tx[i][col] + tmp[i][col];
    for (i = 1; i < LX1-1; i++) {
      for (j = 0; j < LX1; j++) {
        tx[i][col] = tx[i][col] + qbnew[0][j][i-1]*tmp[j][col];
      }
    }
  }
}


//------------------------------------------------------------------------
// Performs mapping from element to mortar when the nonconforming
// edges are shared by two conforming faces of an element.
//------------------------------------------------------------------------
void transfb_nc0(double tmor[LX1][LX1], double tx[LX1][LX1][LX1])
{
  int i, j;

  r_init((double *)tmor, LX1*LX1, 0.0);
  for (j = 0; j < LX1; j++) {
    for (i = 1; i < LX1-1; i++) {
      tmor[0][j]= tmor[0][j] + qbnew[0][j][i-1]*tx[0][0][i];
    }
  }
}


//------------------------------------------------------------------------
// Maps values from element to mortar when the nonconforming edges are
// shared by two nonconforming faces of an element.
// Although each face shall have four pieces of mortar, only value in
// one piece (location (0,0)) is used in the calling routine so only
// the value in the first mortar is calculated in this subroutine.
//------------------------------------------------------------------------
void transfb_nc2(double tmor[LX1][LX1], double tx[LX1][LX1])
{
  double bottom[LX1], temp[LX1][LX1];
  int col, j, i;

  r_init((double *)tmor, LX1*LX1, 0.0);
  r_init((double *)temp, LX1*LX1, 0.0);
  tmor[0][0] = tx[0][0];

  // mapping from tx to intermediate mortar temp + bottom
  for (col = 0; col < LX1; col++) {
    temp[0][col] = tx[0][col];
    j = 0;
    bottom[col] = 0.0;;
    for (i = 1; i < LX1-1; i++) {
      bottom[col] = bottom[col] + qbnew[0][j][i-1]*tx[i][col];
    }

    for (j = 1; j < LX1; j++) {
      for (i = 1; i < LX1-1; i++) {
        temp[j][col] = temp[j][col] + qbnew[0][j][i-1]*tx[i][col];
      }
    }
  }

  // from intermediate mortar to mortar

  // On the nonconforming edge, temp is divided by 2 as there will be
  // a duplicate contribution from another face sharing this edge
  col = 0;
  for (j = 0; j < LX1; j++) {
    for (i = 1; i < LX1-1; i++) {
      tmor[col][j] = tmor[col][j]+ qbnew[0][j][i-1] * bottom[i] +
        qbnew[0][j][i-1] * temp[col][i] * 0.5;
    }
  }

  for (col = 1; col < LX1; col++) {
    tmor[col][0] = tmor[col][0]+temp[col][0];
    for (j = 0; j < LX1; j++) {
      for (i = 1; i < LX1-1; i++) {
        tmor[col][j] = tmor[col][j] + qbnew[0][j][i-1] *temp[col][i];
      }
    }
  }
}


//------------------------------------------------------------------------
// Maps values from element to mortar when the nonconforming edges are
// shared by a nonconforming face and a conforming face of an element
//------------------------------------------------------------------------
void transfb_nc1(double tmor[LX1][LX1], double tx[LX1][LX1])
{
  double bottom[LX1], temp[LX1][LX1];
  int col, j, i;

  r_init((double *)tmor, LX1*LX1, 0.0);
  r_init((double *)temp, LX1*LX1, 0.0);

  tmor[0][0] = tx[0][0];
  // Contribution from the nonconforming faces
  // Since the calling subroutine is only interested in the value on the
  // mortar (location (0,0)), only this piece of mortar is calculated.

  for (col = 0; col < LX1; col++) {
    temp[0][col] = tx[0][col];
    j = 0;
    bottom[col] = 0.0;
    for (i = 1; i < LX1-1; i++) {
      bottom[col] = bottom[col] + qbnew[0][j][i-1]*tx[i][col];
    }

    for (j = 1; j < LX1; j++) {
      for (i = 1; i < LX1-1; i++) {
        temp[j][col] = temp[j][col] + qbnew[0][j][i-1]*tx[i][col];
      }
    }
  }

  col = 0;
  tmor[col][0] = tmor[col][0]+bottom[0];
  for (j = 0; j < LX1; j++) {
    for (i = 1; i < LX1-1; i++) {
      // temp is not divided by 2 here. It includes the contribution
      // from the other conforming face.
      tmor[col][j] = tmor[col][j] + qbnew[0][j][i-1] *bottom[i] +
                                    qbnew[0][j][i-1] *temp[col][i];
    }
  }

  for (col = 1; col < LX1; col++) {
    tmor[col][0] = tmor[col][0]+temp[col][0];
    for (j = 0; j < LX1; j++) {
      for (i = 1; i < LX1-1; i++) {
        tmor[col][j] = tmor[col][j] + qbnew[0][j][i-1] *temp[col][i];
      }
    }
  }
}


//-------------------------------------------------------------------
// Prepare initial guess for cg. All values from conforming
// boundary are copied and summed on tmor.
//-------------------------------------------------------------------
void transfb_c(double tx[])
{
  const double third = 1.0/3.0;
  int il1, il2, il3, il4, ig1, ig2, ig3, ig4, ie, iface, col, j, ig, il;

  r_init(tmort, nmor, 0.0);

  for (ie = 0; ie < nelt; ie++) {
    for (iface = 0; iface < NSIDES; iface++) {
      if (cbc[ie][iface] != 3) {
        il1 = idel[ie][iface][0][0];
        il2 = idel[ie][iface][0][LX1-1];
        il3 = idel[ie][iface][LX1-1][0];
        il4 = idel[ie][iface][LX1-1][LX1-1];
        ig1 = idmo[ie][iface][0][0][0][0];
        ig2 = idmo[ie][iface][1][0][0][LX1-1];
        ig3 = idmo[ie][iface][0][1][LX1-1][0];
        ig4 = idmo[ie][iface][1][1][LX1-1][LX1-1];

        tmort[ig1] = tmort[ig1]+tx[il1]*third;
        tmort[ig2] = tmort[ig2]+tx[il2]*third;
        tmort[ig3] = tmort[ig3]+tx[il3]*third;
        tmort[ig4] = tmort[ig4]+tx[il4]*third;

        for (col = 1; col < LX1-1; col++) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][col][j];
            ig = idmo[ie][iface][0][0][col][j];
            tmort[ig] = tmort[ig]+tx[il];
          }
        }

        if (idmo[ie][iface][0][0][0][LX1-1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][0][j];
            ig = idmo[ie][iface][0][0][0][j];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
          }
        }

        if (idmo[ie][iface][1][0][1][LX1-1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][LX1-1];
            ig = idmo[ie][iface][0][0][j][LX1-1];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
          }
        }

        if (idmo[ie][iface][0][1][LX1-1][1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][LX1-1][j];
            ig = idmo[ie][iface][0][0][LX1-1][j];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
          }
        }

        if (idmo[ie][iface][0][0][LX1-1][0] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][0];
            ig = idmo[ie][iface][0][0][j][0];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
          }
        }
      }
    }
  }
}


//-------------------------------------------------------------------
// Prepare initial guess for CG. All values from conforming
// boundary are copied and summed in tmort.
// mormult is multiplicity, which is used to average tmort.
//-------------------------------------------------------------------
void transfb_c_2(double tx[])
{
  const double third = 1.0/3.0;
  int il1, il2, il3, il4, ig1, ig2, ig3, ig4, ie, iface, col, j, ig, il;

  r_init(tmort, nmor, 0.0);
  r_init(mormult, nmor, 0.0);

  for (ie = 0; ie < nelt; ie++) {
    for (iface = 0; iface < NSIDES; iface++) {

      if (cbc[ie][iface] != 3) {
        il1 = idel[ie][iface][0][0];
        il2 = idel[ie][iface][0][LX1-1];
        il3 = idel[ie][iface][LX1-1][0];
        il4 = idel[ie][iface][LX1-1][LX1-1];
        ig1 = idmo[ie][iface][0][0][0][0];
        ig2 = idmo[ie][iface][1][0][0][LX1-1];
        ig3 = idmo[ie][iface][0][1][LX1-1][0];
        ig4 = idmo[ie][iface][1][1][LX1-1][LX1-1];

        tmort[ig1] = tmort[ig1]+tx[il1]*third;
        tmort[ig2] = tmort[ig2]+tx[il2]*third;
        tmort[ig3] = tmort[ig3]+tx[il3]*third;
        tmort[ig4] = tmort[ig4]+tx[il4]*third;
        mormult[ig1] = mormult[ig1]+third;
        mormult[ig2] = mormult[ig2]+third;
        mormult[ig3] = mormult[ig3]+third;
        mormult[ig4] = mormult[ig4]+third;

        for (col = 1; col < LX1-1; col++) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][col][j];
            ig = idmo[ie][iface][0][0][col][j];
            tmort[ig] = tmort[ig]+tx[il];
            mormult[ig] = mormult[ig]+1.0;
          }
        }

        if (idmo[ie][iface][0][0][0][LX1-1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][0][j];
            ig = idmo[ie][iface][0][0][0][j];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
            mormult[ig] = mormult[ig]+0.5;
          }
        }

        if (idmo[ie][iface][1][0][1][LX1-1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][LX1-1];
            ig = idmo[ie][iface][0][0][j][LX1-1];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
            mormult[ig] = mormult[ig]+0.5;
          }
        }

        if (idmo[ie][iface][0][1][LX1-1][1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][LX1-1][j];
            ig = idmo[ie][iface][0][0][LX1-1][j];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
            mormult[ig] = mormult[ig]+0.5;
          }
        }

        if (idmo[ie][iface][0][0][LX1-1][0] == -1) {
          for (j = 1; j < LX1-1; j++) {
            il = idel[ie][iface][j][0];
            ig = idmo[ie][iface][0][0][j][0];
            tmort[ig] = tmort[ig]+tx[il]*0.5;
            mormult[ig] = mormult[ig]+0.5;
          }
        }
      }
    }
  }
}
