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

static void get_emo(int ie, int n, int ng);
static void mor_assign(int mor_v[3], int *count);
static void mor_edge(int ie, int face, int iel, int mor_v[3]);
static void edgecopy_s(int face, int iel);
static void mor_s_e(int n, int face, int iel, int mor_s_v[2][4]);
static void mor_s_e_nn(int n, int face, int iel, int mor_s_v[4], int nn);
static void mortar_vertex(int i, int iel, int count);
static void mor_ne(int mor_v[3], int nn, int edge, int face,
                   int edge2, int face2, int ntemp, int iel);


//-----------------------------------------------------------------
// generate mortar point index number 
//-----------------------------------------------------------------
void mortar()
{
  int count, iel, jface, ntemp, i, ii, jj, ntemp1;
  int iii, jjj, face2, ne, ie, edge_g, ie2;
  int mor_v[3], cb, cb1, cb2, cb3, cb4, cb5, cb6;
  int space, sumcb, ij1, ij2, n1, n2, n3, n4, n5;

  n1 = LX1*LX1*6*4*nelt;
  nr_init((int *)idmo, n1, -1);

  n2 = 8*nelt;
  nr_init(nemo, n2, -1);
  nr_init((int *)vassign, n2, -1);

  n3 = 2*64*nelt;
  nr_init((int *)emo, n3, -1);

  n4 = 12*nelt;
  l_init((logical *)if_1_edge, n4, false);

  n5 = 2*12*nelt;
  nr_init((int *)diagn, n5, -1) ;

  // Mortar points indices are generated in two steps: first generate 
  // them for all element vertices (corner points), then for conforming 
  // edge and conforming face interiors. Each time a new mortar index 
  // is generated for a mortar point, it is broadcast to all elements 
  // sharing this mortar point. 

  // VERTICES
  count = -1;

  // assign mortar point indices to element vertices
  for (iel = 0; iel < nelt; iel++) {

    // first calculate how many new mortar indices will be generated for 
    // each element.

    // For each element, at least one vertex (vertex 7) will be new mortar
    // point. All possible new mortar points will be on face 1,3 or 5. By
    // checking the type of these three faces, we are able to tell
    // how many new mortar vertex points will be generated in each element.

    cb = cbc[iel][5];
    cb1 = cbc[iel][3];
    cb2 = cbc[iel][1];

    // For different combinations of the type of these three faces,
    // we group them into 27 configurations.
    // For different face types we assign the following integers:
    //        1 for type 2 or 3
    //        2 for type 0
    //        5 for type 1
    // By summing these integers for faces 1,3 and 5, sumcb will have 
    // 10 different numbers indicating 10 different combinations. 

    sumcb = 0;
    if (cb == 2 || cb == 3) {
      sumcb = sumcb+1;
    } else if (cb == 0) {
      sumcb = sumcb+2;
    } else if (cb == 1) {
      sumcb = sumcb+5;
    }
    if (cb1 == 2 || cb1 == 3) {
      sumcb = sumcb+1;
    } else if (cb1 == 0) {
      sumcb = sumcb+2;
    } else if (cb1 == 1) {
      sumcb = sumcb+5;
    }
    if (cb2 == 2 || cb2 == 3) {
      sumcb = sumcb+1;
    } else if (cb2 == 0) {
      sumcb = sumcb+2;
    } else if (cb2 == 1) {
      sumcb = sumcb+5;
    }

    // compute newc[iel]
    // newc[iel] records how many new mortar indices will be generated
    //           for element iel
    // vassign[iel][i] records the element vertex of the i'th new mortar 
    //           vertex point for element iel. e.g. vassign[iel][1]=8 means
    //           the 2nd new mortar vertex point generated on element
    //           iel is iel's 8th vertex.

    if (sumcb == 3) {
      // the three face types for face 1,3, and 5 are 2 2 2
      newc[iel] = 1;
      vassign[iel][0] = 7;

    } else if (sumcb == 4) {
      // the three face types for face 1,3 and 5 are 2 2 0 (not 
      // necessarily in this order)
      newc[iel] = 2;
      if (cb == 0) {
        vassign[iel][0] = 3;
      } else if (cb1 == 0) {
        vassign[iel][0] = 5;
      } else if (cb2 == 0) {
        vassign[iel][0] = 6;
      }
      vassign[iel][1] = 7;

    } else if (sumcb == 7) {
      // the three face types for face 1,3 and 5 are 2 2 1 (not 
      // necessarily in this order)
      if (cb == 1) {
        ij1 = ijel[iel][5][0];
        ij2 = ijel[iel][5][1];
        if (ij1 == 0 && ij2 == 0) {
          newc[iel] = 2;
          vassign[iel][0] = 3;
          vassign[iel][1] = 7;
        } else if (ij1 == 0 && ij2 == 1) {
          ntemp = sje[iel][5][0][0];
          if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 3;
            vassign[iel][1] = 7;
          }
        } else if (ij1 == 1 && ij2 == 0) {
          ntemp = sje[iel][5][0][0];
          if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 3;
            vassign[iel][1] = 7;
          }
        } else {
          newc[iel] = 1;
          vassign[iel][0] = 7;
        }
      } else if (cb1 == 1) {
        ij1 = ijel[iel][3][0];
        ij2 = ijel[iel][3][1];
        if (ij1 == 0 && ij2 == 0) {
          newc[iel] = 2;
          vassign[iel][0] = 5;
          vassign[iel][1] = 7;
        } else if (ij1 == 0 && ij2 == 1) {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 5;
            vassign[iel][1] = 7;
          }
        } else if (ij1 == 1 && ij2 == 0) {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 5;
            vassign[iel][1] = 7;
          }
        } else {
          newc[iel] = 1;
          vassign[iel][0] = 7;
        }

      } else if (cb2 == 1) {
        ij1 = ijel[iel][1][0];
        ij2 = ijel[iel][1][1];
        if (ij1 == 0 && ij2 == 0) {
          newc[iel] = 2;
          vassign[iel][0] = 6;
          vassign[iel][1] = 7;
        } else if (ij1 == 0 && ij2 == 1) {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 6;
            vassign[iel][1] = 7;
          }

        } else if (ij1 == 1 && ij2 == 0) {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
            newc[iel] = 1;
            vassign[iel][0] = 7;
          } else {
            newc[iel] = 2;
            vassign[iel][0] = 6;
            vassign[iel][1] = 7;
          }
        } else {
          newc[iel] = 1;
          vassign[iel][0] = 7;
        }
      }

    } else if (sumcb == 5) {
      // the three face types for face 1,3 and 5 are 2/3 0 0 (not 
      // necessarily in this order)
      newc[iel] = 4;
      if (cb == 2 || cb == 3) {
        vassign[iel][0] = 4;
        vassign[iel][1] = 5;
        vassign[iel][2] = 6;
        vassign[iel][3] = 7;
      } else if (cb1 == 2 || cb1 == 3) {
        vassign[iel][0] = 2;
        vassign[iel][1] = 3;
        vassign[iel][2] = 6;
        vassign[iel][3] = 7;
      } else if (cb2 == 2 || cb2 == 3) {
        vassign[iel][0] = 1;
        vassign[iel][1] = 3;
        vassign[iel][2] = 5;
        vassign[iel][3] = 7;
      }

    } else if (sumcb == 8) {
      // the three face types for face 1,3 and 5 are 2 0 1 (not 
      // necessarily in this order)

      // if face 2 of type 1
      if (cb == 1) {
        if (cb1 == 2 || cb1 == 3) {
          ij1 = ijel[iel][5][0];
          if (ij1 == 0) {
            newc[iel] = 4;
            vassign[iel][0] = 2;
            vassign[iel][1] = 3;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          } else {
            ntemp = sje[iel][5][0][0];
            if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 6;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }

        } else if (cb2 == 2 || cb2 == 3) {
          if (ijel[iel][5][1] == 0) {
            newc[iel] = 4;
            vassign[iel][0] = 1;
            vassign[iel][1] = 3;
            vassign[iel][2] = 5;
            vassign[iel][3] = 7;
          } else {
            ntemp = sje[iel][5][0][0];
            if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 5;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 5;
              vassign[iel][2] = 7;
            }
          }
        }

        // if face 4 of type 1
      } else if (cb1 == 1) {
        if (cb == 2 || cb == 3) {
          ij1 = ijel[iel][3][0];
          ij2 = ijel[iel][3][1];

          if (ij1 == 0 && ij2 == 0) {
            ntemp = sje[iel][3][0][0];
            if (cbc[ntemp][1] == 3 && sje[ntemp][1][0][0] < iel) {
              newc[iel] = 3;
              vassign[iel][0] = 5;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            } else {
              newc[iel] = 4;
              vassign[iel][0] = 4;
              vassign[iel][1] = 5;
              vassign[iel][2] = 6;
              vassign[iel][3] = 7;
            }
          } else if (ij1 == 0 && ij2 == 1) {
            ntemp = sje[iel][3][0][0];
            if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
              newc[iel] = 3;
              vassign[iel][0] = 4;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            } else {
              newc[iel] = 4;
              vassign[iel][0] = 4;
              vassign[iel][1] = 5;
              vassign[iel][2] = 6;
              vassign[iel][3] = 7;
            }
          } else if (ij1 == 1 && ij2 == 0) {
            ntemp = sje[iel][3][0][0];
            if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 6;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 5;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          } else if (ij1 == 1 && ij2 == 1) {
            ntemp = sje[iel][3][0][0];
            if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 6;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 4;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }
        } else {
          if (ijel[iel][3][1] == 0) {
            newc[iel] = 4;
            vassign[iel][0] = 1;
            vassign[iel][1] = 3;
            vassign[iel][2] = 5;
            vassign[iel][3] = 7;
          } else {
            ntemp = sje[iel][3][0][0];
            if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 3;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 5;
              vassign[iel][2] = 7;
            }
          }
        }
        // if face 6 of type 1
      } else if (cb2 == 1) {
        if (cb == 2 || cb == 3) {
          if (ijel[iel][1][0] == 0) {
            newc[iel] = 4;
            vassign[iel][0] = 4;
            vassign[iel][1] = 5;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          } else {
            ntemp = sje[iel][1][0][0];
            if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 5;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 5;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }
        } else { 
          if (ijel[iel][1][1] == 0) {
            newc[iel] = 4;
            vassign[iel][0] = 2;
            vassign[iel][1] = 3;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          } else {
            ntemp = sje[iel][1][0][0];
            if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 3;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }
        }
      }

    } else if (sumcb == 11) {
      // the three face type for face 2,4 and 6 are 2 1 1(not 
      // necessarily in this order)
      if (cb == 2 || cb == 3) {
        if (ijel[iel][3][0] == 0) {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][1] == 3 && sje[ntemp][1][0][0] < iel) {
            newc[iel] = 3;
            vassign[iel][0] = 5;
            vassign[iel][1] = 6;
            vassign[iel][2] = 7;
          } else {
            newc[iel] = 4;
            vassign[iel][0] = 4;
            vassign[iel][1] = 5;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          }

          // if ijel[iel][3][0]=1
        } else {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] < iel) {
            ntemp1 = sje[iel][3][0][0];
            if (cbc[ntemp1][4] == 3 && sje[ntemp1][4][0][0] < iel) {
              newc[iel] = 1;
              vassign[iel][0] = 7;
            } else {
              newc[iel] = 2;
              vassign[iel][0] = 5;
              vassign[iel][1] = 7;
            }
          } else {
            ntemp1 = sje[iel][3][0][0];
            if (cbc[ntemp1][4] == 3 && sje[ntemp1][4][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 6;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 5;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }
        }
      } else if (cb1 == 2 || cb1 == 3) {
        if (ijel[iel][1][1] == 0) {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] < iel) {
            newc[iel] = 3;
            vassign[iel][0] = 3;
            vassign[iel][1] = 6;
            vassign[iel][2] = 7;
          } else {
            newc[iel] = 4;
            vassign[iel][0] = 2;
            vassign[iel][1] = 3;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          }
          // if ijel[iel][1][1]=1
        } else {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] < iel) {
            ntemp1 = sje[iel][5][0][0];
            if (cbc[ntemp1][2] == 3 && sje[ntemp1][2][0][0] < iel) {
              newc[iel] = 1;
              vassign[iel][0] = 7;
            } else {
              newc[iel] = 2;
              vassign[iel][0] = 3;
              vassign[iel][1] = 7;
            }
          } else {
            ntemp1 = sje[iel][5][0][0];
            if (cbc[ntemp1][2] == 3 && sje[ntemp1][2][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 6;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 6;
              vassign[iel][2] = 7;
            }
          }
        }
      } else if (cb2 == 2 || cb2 == 3) {
        if (ijel[iel][5][1] == 0) {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] < iel) {
            newc[iel] = 3;
            vassign[iel][0] = 3;
            vassign[iel][1] = 5;
            vassign[iel][2] = 7;
          } else {
            newc[iel] = 4;
            vassign[iel][0] = 1;
            vassign[iel][1] = 3;
            vassign[iel][2] = 5;
            vassign[iel][3] = 7;
          }
          // if ijel[iel][5][1]=1
        } else {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] < iel) {
            ntemp1 = sje[iel][5][0][0];
            if (cbc[ntemp1][0] == 3 && sje[ntemp1][0][0][0] < iel) {
              newc[iel] = 1;
              vassign[iel][0] = 7;
            } else {
              newc[iel] = 2;
              vassign[iel][0] = 3;
              vassign[iel][1] = 7;
            }
          } else {
            ntemp1 = sje[iel][5][0][0];
            if (cbc[ntemp1][0] == 3 && sje[ntemp1][0][0][0] < iel) {
              newc[iel] = 2;
              vassign[iel][0] = 5;
              vassign[iel][1] = 7;
            } else {
              newc[iel] = 3;
              vassign[iel][0] = 3;
              vassign[iel][1] = 5;
              vassign[iel][2] = 7;
            }
          }
        }
      }

    } else if (sumcb == 6) {
      // the three face type for face 1,3 and 5 are 0 0 0(not 
      // necessarily in this order)
      newc[iel] = 8;
      vassign[iel][0] = 0;
      vassign[iel][1] = 1;
      vassign[iel][2] = 2;
      vassign[iel][3] = 3;
      vassign[iel][4] = 4;
      vassign[iel][5] = 5;
      vassign[iel][6] = 6;
      vassign[iel][7] = 7;

    } else if (sumcb == 9) {
      // the three face type for face 1,3 and 5 are 0 0 1(not 
      // necessarily in this order)
      newc[iel] = 7;
      vassign[iel][0] = 1;
      vassign[iel][1] = 2;
      vassign[iel][2] = 3;
      vassign[iel][3] = 4;
      vassign[iel][4] = 5;
      vassign[iel][5] = 6;
      vassign[iel][6] = 7;

    } else if (sumcb == 12) {
      // the three face type for face 1,3 and 5 are 0 1 1(not 
      // necessarily in this order)
      if (cb == 0) {
        ntemp = sje[iel][1][0][0];
        if (cbc[ntemp][3] == 3 && sje[ntemp][3][0][0] < iel) {
          newc[iel] = 6;
          vassign[iel][0] = 1;
          vassign[iel][1] = 2;
          vassign[iel][2] = 3;
          vassign[iel][3] = 5;
          vassign[iel][4] = 6;
          vassign[iel][5] = 7;
        } else {
          newc[iel] = 7;
          vassign[iel][0] = 1;
          vassign[iel][1] = 2;
          vassign[iel][2] = 3;
          vassign[iel][3] = 4;
          vassign[iel][4] = 5;
          vassign[iel][5] = 6;
          vassign[iel][6] = 7;
        }
      } else if (cb1 == 0) {
        newc[iel] = 7;
        vassign[iel][0] = 1;
        vassign[iel][1] = 2;
        vassign[iel][2] = 3;
        vassign[iel][3] = 4;
        vassign[iel][4] = 5;
        vassign[iel][5] = 6;
        vassign[iel][6] = 7;
      } else if (cb2 == 0) {
        ntemp = sje[iel][3][0][0];
        if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] < iel) {
          newc[iel] = 6;
          vassign[iel][0] = 2;
          vassign[iel][1] = 3;
          vassign[iel][2] = 4;
          vassign[iel][3] = 5;
          vassign[iel][4] = 6;
          vassign[iel][5] = 7;
        } else {
          newc[iel] = 7;
          vassign[iel][0] = 1;
          vassign[iel][1] = 2;
          vassign[iel][2] = 3;
          vassign[iel][3] = 4;
          vassign[iel][4] = 5;
          vassign[iel][5] = 6;
          vassign[iel][6] = 7;
        }
      }

    } else if (sumcb == 15) {
      // the three face type for face 1,3 and 5 are 1 1 1(not 
      // necessarily in this order)
      ntemp = sje[iel][3][0][0];
      ntemp1 = sje[iel][1][0][0];
      if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] < iel) {
        if (cbc[ntemp][1] == 3 && sje[ntemp][1][0][0] < iel) {
          if (cbc[ntemp1][5] == 3 && sje[ntemp1][5][0][0] < iel) {
            newc[iel] = 4;
            vassign[iel][0] = 3;
            vassign[iel][1] = 5;
            vassign[iel][2] = 6;
            vassign[iel][3] = 7;
          } else {
            newc[iel] = 5;
            vassign[iel][0] = 2;
            vassign[iel][1] = 3;
            vassign[iel][2] = 5;
            vassign[iel][3] = 6;
            vassign[iel][4] = 7;
          }
        } else {
          if (cbc[ntemp1][5] == 3 && sje[ntemp1][5][0][0] < iel) {
            newc[iel] = 5;
            vassign[iel][0] = 3;
            vassign[iel][1] = 4;
            vassign[iel][2] = 5;
            vassign[iel][3] = 6;
            vassign[iel][4] = 7;
          } else {
            newc[iel] = 6;
            vassign[iel][0] = 2;
            vassign[iel][1] = 3;
            vassign[iel][2] = 4;
            vassign[iel][3] = 5;
            vassign[iel][4] = 6;
            vassign[iel][5] = 7;
          }
        }
      } else {
        if (cbc[ntemp][1] == 3 && sje[ntemp][1][0][0] < iel) {
          if (cbc[ntemp1][5] == 3 && sje[ntemp1][5][0][0] < iel) {
            newc[iel] = 5;
            vassign[iel][0] = 1;
            vassign[iel][1] = 3;
            vassign[iel][2] = 5;
            vassign[iel][3] = 6;
            vassign[iel][4] = 7;
          } else {
            newc[iel] = 6;
            vassign[iel][0] = 1;
            vassign[iel][1] = 2;
            vassign[iel][2] = 3;
            vassign[iel][3] = 5;
            vassign[iel][4] = 6;
            vassign[iel][5] = 7;
          }
        } else {
          if (cbc[ntemp1][5] == 3 && sje[ntemp1][5][0][0] < iel) {
            newc[iel] = 6;
            vassign[iel][0] = 1;
            vassign[iel][1] = 3;
            vassign[iel][2] = 4;
            vassign[iel][3] = 5;
            vassign[iel][4] = 6;
            vassign[iel][5] = 7;

          } else {
            newc[iel] = 7;
            vassign[iel][0] = 1; 
            vassign[iel][1] = 2; 
            vassign[iel][2] = 3; 
            vassign[iel][3] = 4;
            vassign[iel][4] = 5;
            vassign[iel][5] = 6;
            vassign[iel][6] = 7;
          }
        }
      }
    }
  }

  // end computing how many new mortar vertex points will be generated
  // on each element.

  // Compute (potentially in parallel) front[iel], which records how many 
  // new mortar point indices are to be generated from element 0 to iel.
  // front[iel]=newc[0]+newc[1]+...+newc[iel]

  ncopy(front, newc, nelt);

  parallel_add(front);

  // On each element, generate new mortar point indices and assign them
  // to all elements sharing this mortar point. Note, if a mortar point 
  // is shared by several elements, the mortar point index of it will only
  // be generated on the element with the lowest element index. 

  for (iel = 0; iel < nelt; iel++) {
    // compute the starting vertex mortar point index in element iel
    front[iel] = front[iel]-newc[iel];

    for (i = 0; i < newc[iel]; i++) {
      // count is the new mortar index number, which will be assigned
      // to a vertex of iel and broadcast to all other elements sharing
      // this vertex point.
      count = front[iel]+i;
      mortar_vertex(vassign[iel][i], iel, count);
    }
  }

  // nvertex records how many mortar indices are for element vertices.
  // It is used in the computation of the preconditioner.
  nvertex = count + 1;

  // CONFORMING EDGE AND FACE INTERIOR

  // find out how many new mortar point indices will be assigned to all
  // conforming edges and all conforming face interiors on each element


  // eassign[iel][i]=true   indicates that the i'th edge on iel will 
  //                        generate new mortar points. 
  // ncon_edge[iel][i]=true indicates that the i'th edge on iel is 
  //                        nonconforming
  n1 = 12*nelt;
  l_init((logical *)ncon_edge, n1, false);
  l_init((logical *)eassign, n1, false);

  // fassign[iel][i]=true indicates that the i'th face of iel will 
  //                      generate new mortar points
  n2 = 6*nelt;
  l_init((logical *)fassign, n2, false);

  // newe records how many new edges are to be assigned
  // diagn[iel][n][0] records the element index of neighbor element of iel,
  //            that shares edge n of iel
  // diagn[iel][n][1] records the neighbor element diagn[iel][n][0] shares 
  //            which part of edge n of iel. diagn[iel][n][1]=0 refers to left
  //            or bottom half of the edge n, diagn[iel][n][1]=1 refers
  //            to the right or top part of edge n.
  // if_1_edge[iel][n]=true indicates that the size of iel is smaller than 
  //            that of its neighbor connected, neighbored by edge n only
  for (iel = 0; iel < nelt; iel++) {
    newc[iel] = 0;
    newe[iel] = 0;
    newi[iel] = 0;
    cb1 = cbc[iel][0];
    cb2 = cbc[iel][1];
    cb3 = cbc[iel][2];
    cb4 = cbc[iel][3];
    cb5 = cbc[iel][4];
    cb6 = cbc[iel][5];

    // on face 6

    if (cb6 == 0) {
      if (cb4 == 0 || cb4 == 1) {
        // if face 6 is of type 0 and face 4 is of type 0 or type 1, the edge
        // shared by face 4 and 6 (edge 10) will generate new mortar point
        // indices.
        newe[iel] = newe[iel]+1;
        eassign[iel][10] = true;
      }
      if (cb1 != 3) {
        // if face 1 is of type 3, the edge shared by face 6 and 1 (edge 0)
        // will generate new mortar points indices.
        newe[iel] = newe[iel]+1;
        eassign[iel][0] = true;
      }
      if (cb3 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][8] = true;
      }
      if (cb2 == 0 || cb2 == 1) {
        newe[iel] = newe[iel]+1;
        eassign[iel][4] = true;
      }
    } else if (cb6 == 1) {
      if (cb4 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][10] = true;
      } else if (cb4 == 1) {

        // If face 6 and face 4 both are of type 1, ntemp is the neighbor
        // element on face 4.
        ntemp = sje[iel][3][0][0];

        // if ntemp's face 6 is not noncoforming or the neighbor element
        // of ntemp on face 6 has an element index larger than iel, the 
        // edge shared by face 6 and 4 (edge 10) will generate new mortar
        // point indices.
        if (cbc[ntemp][5] != 3 || sje[ntemp][5][0][0] > iel) {

          newe[iel] = newe[iel]+1;
          eassign[iel][10] = true;
          // if the face 6 of ntemp is of type 2
          if (cbc[ntemp][5] == 2) {
            // The neighbor element of iel, neighbored by edge 10, is 
            // sje[ntemp][5][0][0] (the neighbor element of ntemp on ntemp's
            // face 6).
            diagn[iel][10][0] = sje[ntemp][5][0][0];
            // The neighbor element of iel, neighbored by edge 10 shares
            // the ijel[iel][5][1] part of edge 10 of iel
            diagn[iel][10][1] = ijel[iel][5][1];
            // edge 9 of element sje[ntemp][5][0][0] (the neighbor element of 
            // ntemp on ntemp's face 6) is a nonconforming edge
            ncon_edge[sje[ntemp][5][0][0]][9] = true;
            // if_1_edge[iel][n]=true indicates that iel is of a smaller
            //size than its neighbor element, neighbored by edge n of iel only.
            if_1_edge[iel][10] = true;
          }
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] > iel) {
            diagn[iel][10][0] = sje[ntemp][5][ijel[iel][5][1]][1];
          }
        }
      }

      if (cb1 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][0] = true;
      } else if (cb1 == 1) {
        ntemp = sje[iel][0][0][0];
        if (cbc[ntemp][5] != 3 || sje[ntemp][5][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][0] = true;
          if (cbc[ntemp][5] == 2) {
            diagn[iel][0][0] = sje[ntemp][5][0][0];
            diagn[iel][0][1] = ijel[iel][5][0];
            ncon_edge[sje[ntemp][5][0][0]][6] = true;
            if_1_edge[iel][0] = true;
          }
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] > iel) {
            diagn[iel][0][0] = sje[ntemp][5][0][ijel[iel][5][0]];
          }
        }
      } else if (cb1 == 2) {
        if (ijel[iel][5][1] == 1) {
          ntemp = sje[iel][0][0][0];
          if (cbc[ntemp][5] == 1) {
            newe[iel] = newe[iel]+1;
            eassign[iel][0] = true;
            // if cbc[ntemp][5]=2
          } else {
            if (sje[ntemp][5][0][0] > iel) {
              newe[iel] = newe[iel]+1;
              eassign[iel][0] = true;
              diagn[iel][0][0] = sje[ntemp][5][0][0];
            }
          }
        } else {
          newe[iel] = newe[iel]+1;
          eassign[iel][0] = true;
        }
      }

      if (cb3 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][8] = true;
      } else if (cb3 == 1) {
        ntemp = sje[iel][2][0][0];
        if (cbc[ntemp][5] != 3 || sje[ntemp][5][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][8] = true;
          if (cbc[ntemp][5] == 2) {
            diagn[iel][8][0] = sje[ntemp][5][0][0];
            diagn[iel][8][1] = ijel[iel][5][1];
            ncon_edge[sje[ntemp][5][0][0]][11] = true;
            if_1_edge[iel][8] = true;
          }
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] > iel) {
            diagn[iel][8][0] = sje[ntemp][5][ijel[iel][5][1]][1];
          }
        }
      } else if (cb3 == 2) {
        if (ijel[iel][5][0] == 1) {
          ntemp = sje[iel][2][0][0];
          if (cbc[ntemp][5] == 1) {
            newe[iel] = newe[iel]+1;
            eassign[iel][8] = true;
            // if cbc[ntemp][5]=2
          } else {
            if (sje[ntemp][5][0][0] > iel) {
              newe[iel] = newe[iel]+1;
              eassign[iel][8] = true;
              diagn[iel][8][0] = sje[ntemp][5][0][0];
            }
          }
        } else {
          newe[iel] = newe[iel]+1;
          eassign[iel][8] = true;
        }
      }

      if (cb2 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][4] = true;
      } else if (cb2 == 1) {
        ntemp = sje[iel][1][0][0];
        if (cbc[ntemp][5] != 3 || sje[ntemp][5][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][4] = true;
          if (cbc[ntemp][5] == 2) {
            diagn[iel][4][0] = sje[ntemp][5][0][0];
            diagn[iel][4][1] = ijel[iel][5][0];
            ncon_edge[sje[ntemp][5][0][0]][2] = true;
            if_1_edge[iel][4] = true;
          }
          if (cbc[ntemp][5] == 3 && sje[ntemp][5][0][0] > iel) {
            diagn[iel][8][0] = sje[ntemp][5][ijel[iel][5][1]][1];
          }
        }
      }
    }

    // one face 4
    if (cb4 == 0) {
      if (cb1 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][3] = true;
      }
      if (cb5 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][11] = true;
      }
      if (cb2 == 0 || cb2 == 1) {
        newe[iel] = newe[iel]+1;
        eassign[iel][7] = true;
      } 

    } else if (cb4 == 1) {
      if (cb1 == 2) {
        if (ijel[iel][3][1] == 0) {
          newe[iel] = newe[iel]+1;
          eassign[iel][3] = true;
        } else {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][0] != 3 || sje[ntemp][0][0][0] > iel) {
            newe[iel] = newe[iel]+1;
            eassign[iel][3] = true;
            if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] > iel) {
              diagn[iel][3][0] = sje[ntemp][0][1][ijel[iel][3][0]];
            }
          }
        }
      } else if (cb1 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][3] = true;
      } else if (cb1 == 1) {
        ntemp = sje[iel][3][0][0];
        if (cbc[ntemp][0] != 3 || sje[ntemp][0][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][3] = true;
          if (cbc[ntemp][0] == 2) {
            diagn[iel][3][0] = sje[ntemp][0][0][0];
            diagn[iel][3][1] = ijel[iel][3][0];
            ncon_edge[sje[ntemp][0][0][0]][5] = true;
            if_1_edge[iel][3] = true;
          }
          if (cbc[ntemp][0] == 3 && sje[ntemp][0][0][0] > iel) {
            diagn[iel][3][0] = sje[ntemp][0][1][ijel[iel][3][0]];
          }
        }
      }
      if (cb5 == 2) {
        if (ijel[iel][3][0] == 0) {
          newe[iel] = newe[iel]+1;
          eassign[iel][11] = true;
        } else {
          ntemp = sje[iel][3][0][0];
          if (cbc[ntemp][4] != 3 || sje[ntemp][4][0][0] > iel) {
            newe[iel] = newe[iel]+1;
            eassign[iel][11] = true;
            if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] > iel) {
              diagn[iel][11][0] = sje[ntemp][4][ijel[iel][3][1]][1];
            }
          }
        }
      } else if (cb5 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][11] = true;
      } else if (cb5 == 1) {
        ntemp = sje[iel][3][0][0];
        if (cbc[ntemp][4] != 3 || sje[ntemp][4][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][11] = true;
          if (cbc[ntemp][4] == 2) {
            diagn[iel][11][0] = sje[ntemp][4][0][0];
            diagn[iel][11][1] = ijel[iel][3][1];
            ncon_edge[sje[ntemp][4][0][0]][8] = true;
            if_1_edge[iel][11] = true;
          }
          if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] > iel) {
            diagn[iel][11][0] = sje[ntemp][4][ijel[iel][3][1]][1];
          }
        }
      }
      if (cb2 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][7] = true;
      } else if (cb2 == 1) {
        ntemp = sje[iel][3][0][0];
        if (cbc[ntemp][1] != 3 || sje[ntemp][1][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][7] = true;
          if (cbc[ntemp][1] == 2) {
            diagn[iel][7][0] = sje[ntemp][1][0][0];
            diagn[iel][7][1] = ijel[iel][3][0];
            ncon_edge[sje[ntemp][1][0][0]][1] = true;
            if_1_edge[iel][7] = true;
          }
          if (cbc[ntemp][1] == 3 && sje[ntemp][1][0][0] > iel) {
            diagn[iel][7][0] = sje[ntemp][2][1][ijel[iel][3][0]];
          }
        }
      }
    }

    // on face 2
    if (cb2 == 0) {
      if (cb3 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][5] = true;
      }
      if (cb5 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][6] = true;
      }
    } else if (cb2 == 1) {
      if (cb3 == 2) {
        if (ijel[iel][1][1] == 0) {
          newe[iel] = newe[iel]+1;
          eassign[iel][5] = true;
        } else {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][2] != 3 || sje[ntemp][2][0][0] > iel) {
            newe[iel] = newe[iel]+1;
            eassign[iel][5] = true;
            if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] > iel) {
              diagn[iel][5][0] = sje[ntemp][2][1][ijel[iel][1][0]];
            }
          }
        }
      } else if (cb3 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][5] = true;
      } else if (cb3 == 1) {
        ntemp = sje[iel][1][0][0];
        if (cbc[ntemp][2] != 3 || sje[ntemp][2][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][5] = true;
          if (cbc[ntemp][2] == 2) {
            diagn[iel][5][0] = sje[ntemp][2][0][0];
            diagn[iel][5][1] = ijel[iel][1][0];
            ncon_edge[sje[ntemp][2][0][0]][3] = true;
            if_1_edge[iel][5] = true;
          }
          if (cbc[ntemp][2] == 3 && sje[ntemp][2][0][0] > iel) {
            diagn[iel][5][0] = sje[ntemp][2][1][ijel[iel][3][0]];
          }
        }
      }
      if (cb5 == 2) {
        if (ijel[iel][1][0] == 0) {
          newe[iel] = newe[iel]+1;
          eassign[iel][6] = true;
        } else {
          ntemp = sje[iel][1][0][0];
          if (cbc[ntemp][4] != 3 || sje[ntemp][4][0][0] > iel) {
            newe[iel] = newe[iel]+1;
            eassign[iel][6] = true;
            if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] > iel) {
              diagn[iel][6][0] = sje[ntemp][4][1][ijel[iel][1][1]];
            }
          }
        }
      } else if (cb5 == 0) {
        newe[iel] = newe[iel]+1;
        eassign[iel][6] = true;
      } else if (cb5 == 1) {
        ntemp = sje[iel][1][0][0];
        if (cbc[ntemp][4] != 3 || sje[ntemp][4][0][0] > iel) {
          newe[iel] = newe[iel]+1;
          eassign[iel][6] = true;
          if (cbc[ntemp][4] == 2) {
            diagn[iel][6][0] = sje[ntemp][4][0][0];
            diagn[iel][6][1] = ijel[iel][1][1];
            ncon_edge[sje[ntemp][4][0][0]][0] = true;
            if_1_edge[iel][6] = true;
          }
          if (cbc[ntemp][4] == 3 && sje[ntemp][4][0][0] > iel) {
            diagn[iel][6][0] = sje[ntemp][4][ijel[iel][3][1]][1];
          }
        }
      }
    }

    // on face 1
    if (cb1 == 1) {
      newe[iel] = newe[iel]+2;
      eassign[iel][1] = true;
      if (cb3 == 1) {
        ntemp = sje[iel][0][0][0];
        if (cbc[ntemp][2] == 2) {
          diagn[iel][1][0] = sje[ntemp][2][0][0];
          diagn[iel][1][1] = ijel[iel][0][0];
          ncon_edge[sje[ntemp][2][0][0]][7] = true;
          if_1_edge[iel][1] = true;
        } else if (cbc[ntemp][2] == 3) {
          diagn[iel][1][0] = sje[ntemp][2][0][ijel[iel][0][0]];
        }
      } else if (cb3 == 2) {
        ntemp = sje[iel][2][0][0];
        if (ijel[iel][0][1] == 1) {
          if (cbc[ntemp][0] == 2) {
            diagn[iel][1][0] = sje[ntemp][0][0][0];
          }
        }
      }

      eassign[iel][2] = true;
      if (cb5 == 1) {
        ntemp = sje[iel][0][0][0];
        if (cbc[ntemp][4] == 2) {
          diagn[iel][2][0] = sje[ntemp][4][0][0];
          diagn[iel][2][1] = ijel[iel][0][1];
          ncon_edge[sje[ntemp][4][0][0]][4] = true;
          if_1_edge[iel][2] = true;
        } else if (cbc[ntemp][4] == 3) {
          diagn[iel][2][0] = sje[ntemp][4][0][ijel[iel][0][1]];
        }
      } else if (cb5 == 2) {
        ntemp = sje[iel][4][0][0];
        if (ijel[iel][0][0] == 1) {
          if (cbc[ntemp][0] == 2) {
            diagn[iel][2][0] = sje[ntemp][0][0][0];
          }
        }

      }
    } else if (cb1 == 2) {
      if (cb3 == 2) {
        ntemp = sje[iel][0][0][0];
        if (cbc[ntemp][2] != 3) {
          newe[iel] = newe[iel]+1;
          eassign[iel][1] = true;
          if (cbc[ntemp][2] == 2) {
            diagn[iel][1][0] = sje[ntemp][2][0][0];
          } 
        }
      } else if (cb3 == 0 || cb3 == 1) {
        newe[iel] = newe[iel]+1;
        eassign[iel][1] = true;
        if (cb3 == 1) {
          ntemp = sje[iel][0][0][0];
          if (cbc[ntemp][2] == 2) {
            diagn[iel][1][0] = sje[ntemp][2][0][0];
          }
        }
      }
      if (cb5 == 2) {
        ntemp = sje[iel][0][0][0];
        if (cbc[ntemp][4] != 3) {
          newe[iel] = newe[iel]+1;
          eassign[iel][2] = true;
          if (cbc[ntemp][4] == 2) {
            diagn[iel][2][0] = sje[ntemp][4][0][0];
          }
        }
      } else if (cb5 == 0 || cb5 == 1) {
        newe[iel] = newe[iel]+1;
        eassign[iel][2] = true;
        if (cb5 == 1) {
          ntemp = sje[iel][0][0][0];
          if (cbc[ntemp][4] == 2) {
            diagn[iel][2][0] = sje[ntemp][4][0][0];
          }
        }
      }
    } else if (cb1 == 0) {
      if (cb3 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][1] = true;
      }
      if (cb5 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][2] = true;
      }
    }

    // on face 3
    if (cb3 == 1) {
      newe[iel] = newe[iel]+1;
      eassign[iel][9] = true;
      if (cb5 == 1) {
        ntemp = sje[iel][2][0][0];
        if (cbc[ntemp][4] == 2) {
          diagn[iel][9][0] = sje[ntemp][4][0][0];
          diagn[iel][9][1] = ijel[iel][2][1];
          ncon_edge[sje[ntemp][4][0][0]][10] = true;
          if_1_edge[iel][9] = true;
        }
      }
      if (ijel[iel][2][0] == 1) {
        ntemp = sje[iel][2][0][0];
        if (cbc[ntemp][4] == 3) {
          diagn[iel][9][0] = sje[ntemp][4][ijel[iel][2][1]][0];
        }
      }
    } else if (cb3 == 2) {
      if (cb5 == 2) {
        ntemp = sje[iel][2][0][0];
        if (cbc[ntemp][4] != 3) {
          newe[iel] = newe[iel]+1;
          eassign[iel][9] = true;
          if (cbc[ntemp][4] == 2) {
            diagn[iel][9][0] = sje[ntemp][4][0][0];
          }
        }
      } else if (cb5 == 0 || cb5 == 1) {
        newe[iel] = newe[iel]+1;
        eassign[iel][9] = true;
        if (cb5 == 1) {
          ntemp = sje[iel][2][0][0];
          if (cbc[ntemp][4] == 2) {
            diagn[iel][9][0] = sje[ntemp][4][0][0];
          } 
        }
      }
    } else if (cb3 == 0) {
      if (cb5 != 3) {
        newe[iel] = newe[iel]+1;
        eassign[iel][9] = true;
      }
    }

    // CONFORMING FACE INTERIOR

    // find how many new mortar point indices will be assigned
    // to face interiors on all faces on each element

    // newi record how many new face interior points will be assigned

    // on face 6
    if (cb6 == 1 || cb6 == 0) {
      newi[iel] = newi[iel]+9;
      fassign[iel][5] = true;
    }
    // on face 4
    if (cb4 == 1 || cb4 == 0) {
      newi[iel] = newi[iel]+9;
      fassign[iel][3] = true;
    }
    // on face 2
    if (cb2 == 1 || cb2 == 0) {
      newi[iel] = newi[iel]+9;
      fassign[iel][1] = true;
    }
    // on face 1
    if (cb1 != 3) {
      newi[iel] = newi[iel]+9;
      fassign[iel][0] = true;
    }
    // on face 3
    if (cb3 != 3) {
      newi[iel] = newi[iel]+9;
      fassign[iel][2] = true;
    }
    // on face 5
    if (cb5 != 3) {
      newi[iel] = newi[iel]+9;
      fassign[iel][4] = true;
    }

    // newc is the total number of new mortar point indices
    // to be assigned to each element.
    newc[iel] = newe[iel]*3+newi[iel];
  }

  // Compute (potentially in parallel) front[iel], which records how 
  // many new mortar point indices are to be assigned (to conforming 
  // edges and conforming face interiors) from element 0 to iel.
  // front[iel]=newc[0]+newc[1]+...+newc[iel]

  ncopy(front, newc, nelt);

  parallel_add(front);

  // nmor is the total number or mortar points
  nmor = nvertex+front[nelt-1];

  // Generate (potentially in parallel) new mortar point indices on 
  // each conforming element face. On each face, first visit all 
  // conforming edges, and then the face interior.
  for (iel = 0; iel < nelt; iel++) {
    front[iel] = front[iel]-newc[iel];
    count = nvertex+front[iel];
    for (i = 0; i < 6; i++) {
      cb1 = cbc[iel][i];
      if (i < 2) {
        ne = 4;
        space = 1;
      } else if (i < 4) {
        ne = 3;
        space = 2;

        // i loops over faces. Only 4 faces need to be examed for edge visit.
        // On face 1, edge 0,1,2 and 3 will be visited. On face 2, edge 4,5,6
        // and 7 will be visited. On face 3, edge 8 and 9 will be visited and
        // on face 4, edge 10 and 11 will be visited. The 12 edges can be 
        // covered by four faces, there is no need to visit edges on face
        // 5 and 6.  So ne is set to be 0. 
        // However, i still needs to loop over 4 and 5, since the interiors
        // of face 5 and 6 still need to be visited.

      } else {
        ne = 0;
        space = 1;
      }

      for (ie = 0; ie < ne; ie += space) {
        edge_g = edgenumber[i][ie];
        if (eassign[iel][edge_g]) {
          // generate the new mortar points index, mor_v
          mor_assign(mor_v, &count);
          // assign mor_v to local edge ie of face i on element iel
          mor_edge(ie, i, iel, mor_v);

          // Since this edge is shared by another face of element 
          // iel, assign mor_v to the corresponding edge on the other 
          // face also.

          // find the other face
          face2 = f_e_ef[i][ie];
          // find the local edge index of this edge on the other face
          ie2 = localedgenumber[edge_g][face2];
          // asssign mor_v  to local edge ie2 of face face2 on element iel
          mor_edge(ie2, face2, iel, mor_v);

          // There are some neighbor elements also sharing this edge. Assign
          // mor_v to neighbor element, neighbored by face i.
          if (cbc[iel][i] == 2) {
            ntemp = sje[iel][i][0][0];
            mor_edge(ie, jjface[i], ntemp, mor_v);
            mor_edge(op[ie2], face2, ntemp, mor_v);
          }

          // assign mor_v  to neighbor element neighbored by face face2
          if (cbc[iel][face2] == 2) {
            ntemp = sje[iel][face2][0][0];
            mor_edge(ie2, jjface[face2], ntemp, mor_v);
            mor_edge(op[ie], i, ntemp, mor_v);
          }

          // assign mor_v to neighbor element sharing this edge

          // if the neighbor is of the same size of iel
          if (!if_1_edge[iel][edgenumber[i][ie]]) {
            if (diagn[iel][edgenumber[i][ie]][0] != -1) {
              ntemp = diagn[iel][edgenumber[i][ie]][0];
              mor_edge(op[ie2], jjface[face2], ntemp, mor_v);
              mor_edge(op[ie], jjface[i], ntemp, mor_v);
            }

            // if the neighbor has a size larger than iel's
          } else {
            if (diagn[iel][edgenumber[i][ie]][0] != -1) {
              ntemp = diagn[iel][edgenumber[i][ie]][0];
              mor_ne(mor_v, diagn[iel][edgenumber[i][ie]][1], 
                     ie, i, ie2, face2, iel, ntemp);
            }
          }
        }
      } 

      if (fassign[iel][i]) {
        // generate new mortar points index in face interior. 
        // if face i is of type 2 or iel doesn't have a neighbor element,
        // assign new mortar point indices to interior mortar points
        // of face i of iel.
        cb = cbc[iel][i];
        if (cb == 1 || cb == 0) {
          for (jj = 1; jj < LX1-1; jj++) {
            for (ii = 1; ii < LX1-1; ii++) {
              idmo[iel][i][0][0][jj][ii] = count;
              count = count+1;
            }
          }

          // if face i is of type 2, assign new mortar point indices
          // to iel as well as to the neighboring element on face i
        } else if (cb == 2) {
          if (idmo[iel][i][0][0][1][1] == -1) {
            ntemp = sje[iel][i][0][0];
            jface = jjface[i];
            for (jj = 1; jj < LX1-1; jj++) {
              for (ii = 1; ii < LX1-1; ii++) {
                idmo[iel][i][0][0][jj][ii] = count;
                idmo[ntemp][jface][0][0][jj][ii] = count;
                count = count+1;
              }
            }
          } 
        }
      }
    }
  }

  // for edges on nonconforming faces, copy the mortar points indices
  // from neighbors.
  for (iel = 0; iel < nelt; iel++) {
    for (i = 0; i < 6; i++) {
      cb = cbc[iel][i];
      if (cb == 3) {
        // edges 
        edgecopy_s(i, iel);
      } 

      // face interior 

      jface = jjface[i];
      if (cb == 3) {
        for (iii = 0; iii < 2; iii++) {
          for (jjj = 0; jjj < 2; jjj++) {
            ntemp = sje[iel][i][jjj][iii];
            for (jj = 0; jj < LX1; jj++) {
              for (ii = 0; ii < LX1; ii++) {
                idmo[iel][i][jjj][iii][jj][ii] =
                  idmo[ntemp][jface][0][0][jj][ii];
              }
            }
            idmo[iel][i][jjj][iii][0][0] = idmo[ntemp][jface][0][0][0][0];
            idmo[iel][i][jjj][iii][0][LX1-1] = idmo[ntemp][jface][1][0][0][LX1-1];
            idmo[iel][i][jjj][iii][LX1-1][0] = idmo[ntemp][jface][0][1][LX1-1][0];
            idmo[iel][i][jjj][iii][LX1-1][LX1-1]=
              idmo[ntemp][jface][1][1][LX1-1][LX1-1];
          }
        }
      }
    }
  }
}

       
//-----------------------------------------------------------------
// This subroutine fills array emo.
// emo  records all elements sharing the same mortar point 
//              (only applies to element vertices) .
// emo[n][i][0] gives the element ID of the i'th element sharing
//              mortar point n. (emo[n][i][0]=ie), ie is element
//              index.
// emo[n][i][1] gives the vertex index of mortar point n on this
//              element (emo[n][i][1]=ng), ng is the vertex index.
// nemo[n] records the total number of elements sharing mortar 
//              point n.
//-----------------------------------------------------------------
static void get_emo(int ie, int n, int ng)
{
  int ntemp, i;
  logical L1;

  L1 = false;
  for (i = 0; i <= nemo[n]; i++) {
    if (emo[n][i][0] == ie) L1 = true;
  }
  if (!L1) {
    ntemp = nemo[n]+1;
    nemo[n] = ntemp;
    emo[n][ntemp][0] = ie;
    emo[n][ntemp][1] = ng;
  }
}


//-----------------------------------------------------------------
// Check whether the i's vertex of element iel is at the same
// location as j's vertex of element ntemp.
//-----------------------------------------------------------------
logical ifsame(int iel, int i, int ntemp, int j)
{
  if (ntemp == -1 || iel == -1) return false;
  if (xc[iel][i] == xc[ntemp][j] && yc[iel][i] == yc[ntemp][j] && 
      zc[iel][i] == zc[ntemp][j]) {
    return true;
  }
  return false;
}


//-----------------------------------------------------------------
// Assign three consecutive numbers for mor_v, which will
// be assigned to the three interior points of an edge as the 
// mortar point indices.
//-----------------------------------------------------------------
static void mor_assign(int mor_v[3], int *count)
{
  int i;

  for (i = 0; i < 3; i++) {
    mor_v[i] = *count;
    *count = *count + 1;
  }
}

     
//-----------------------------------------------------------------
// Copy the mortar points index from mor_v to local 
// edge ie of the face'th face on element iel.
// The edge is conforming.
//-----------------------------------------------------------------
static void mor_edge(int ie, int face, int iel, int mor_v[3])
{
  int i, j, nn;

  if (ie == 0) {
    j = 0;
    for (nn = 1; nn < LX1-1; nn++) {
      idmo[iel][face][0][0][j][nn] = mor_v[nn-1];
    }
  } else if (ie == 1) { 
    i = LX1-1;
    for (nn = 1; nn < LX1-1; nn++) {
      idmo[iel][face][0][0][nn][i] = mor_v[nn-1];
    }
  } else if (ie == 2) { 
    j = LX1-1;
    for (nn = 1; nn < LX1-1; nn++) {
      idmo[iel][face][0][0][j][nn] = mor_v[nn-1];
    }
  } else if (ie == 3) { 
    i = 0;
    for (nn = 1; nn < LX1-1; nn++) {
      idmo[iel][face][0][0][nn][i] = mor_v[nn-1];
    }
  }
}


//------------------------------------------------------------
// Copy mortar points index on edges from neighbor elements 
// to an element face of the 3rd type.
//------------------------------------------------------------
static void edgecopy_s(int face, int iel)
{
  int ntemp1, ntemp2, ntemp3, ntemp4;
  int edge_g, edge_l, face2, mor_s_v[2][4], i;

  // find four neighbors on this face (3rd type)
  ntemp1 = sje[iel][face][0][0];
  ntemp2 = sje[iel][face][1][0];
  ntemp3 = sje[iel][face][0][1];
  ntemp4 = sje[iel][face][1][1];

  // local edge 1

  // mor_s_v is the array of mortar indices to  be copied.
  nr_init((int *)mor_s_v, 4*2, -1);
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[0][i-1] = idmo[ntemp1][jjface[face]][0][0][0][i];
  }
  mor_s_v[0][LX1-2] = idmo[ntemp1][jjface[face]][1][0][0][LX1-1];
  for (i = 0; i < LX1-1; i++) {
    mor_s_v[1][i] = idmo[ntemp2][jjface[face]][0][0][0][i];
  }

  // copy mor_s_v to local edge 0 on this face
  mor_s_e(0, face, iel, mor_s_v);

  // copy mor_s_v to the corresponding edge on the other face sharing
  // local edge 0
  face2 = f_e_ef[face][0];
  edge_g = edgenumber[face][0];
  edge_l = localedgenumber[edge_g][face2];
  mor_s_e(edge_l, face2, iel, mor_s_v);

  // local edge 1
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[0][i-1] = idmo[ntemp2][jjface[face]][0][0][i][LX1-1];
  }
  mor_s_v[0][LX1-2] = idmo[ntemp2][jjface[face]][1][1][LX1-1][LX1-1];

  mor_s_v[1][0] = idmo[ntemp4][jjface[face]][1][0][0][LX1-1];
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[1][i] = idmo[ntemp4][jjface[face]][0][0][i][LX1-1];
  }

  mor_s_e(1, face, iel, mor_s_v);
  face2 = f_e_ef[face][1];
  edge_g = edgenumber[face][1];
  edge_l = localedgenumber[edge_g][face2];
  mor_s_e(edge_l, face2, iel, mor_s_v);

  // local edge 2
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[0][i-1] = idmo[ntemp3][jjface[face]][0][0][LX1-1][i];
  }
  mor_s_v[0][LX1-2] = idmo[ntemp3][jjface[face]][1][1][LX1-1][LX1-1];

  mor_s_v[1][0] = idmo[ntemp4][jjface[face]][0][1][LX1-1][0];
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[1][i] = idmo[ntemp4][jjface[face]][0][0][LX1-1][i];
  }

  mor_s_e(2, face, iel, mor_s_v);
  face2 = f_e_ef[face][2];
  edge_g = edgenumber[face][2];
  edge_l = localedgenumber[edge_g][face2];
  mor_s_e(edge_l, face2, iel, mor_s_v);

  // local edge 3
  for (i = 1; i < LX1-1; i++) {
    mor_s_v[0][i-1] = idmo[ntemp1][jjface[face]][0][0][i][0];
  }
  mor_s_v[0][LX1-2] = idmo[ntemp1][jjface[face]][0][1][LX1-1][0];

  for (i = 0; i < LX1-1; i++) {
    mor_s_v[1][i] = idmo[ntemp3][jjface[face]][0][0][i][0];
  }

  mor_s_e(3, face, iel, mor_s_v);
  face2 = f_e_ef[face][3];
  edge_g = edgenumber[face][3];
  edge_l = localedgenumber[edge_g][face2];
  mor_s_e(edge_l, face2, iel, mor_s_v);
}


//------------------------------------------------------------
// Copy mortar points index from mor_s_v to local edge n
// on face "face" of element iel. The edge is nonconforming. 
//------------------------------------------------------------
static void mor_s_e(int n, int face, int iel, int mor_s_v[2][4])
{
  int i;

  if (n == 0) {
    for (i = 1; i < LX1; i++) {
      idmo[iel][face][0][0][0][i] = mor_s_v[0][i-1];
    }
    for (i = 0; i < LX1-1; i++) {
      idmo[iel][face][1][0][0][i] = mor_s_v[1][i];
    }
  } else if (n == 1) {
    for (i = 1; i < LX1; i++) {
      idmo[iel][face][1][0][i][LX1-1] = mor_s_v[0][i-1];
    }
    for (i = 0; i < LX1-1; i++) {
      idmo[iel][face][1][1][i][LX1-1] = mor_s_v[1][i];
    }
  } else if (n == 2) {
    for (i = 1; i < LX1; i++) {
      idmo[iel][face][0][1][LX1-1][i] = mor_s_v[0][i-1];
    }
    for (i = 0; i < LX1-1; i++) {
      idmo[iel][face][1][1][LX1-1][i] = mor_s_v[1][i];
    }
  } else if (n == 3) {
    for (i = 1; i < LX1; i++) {
      idmo[iel][face][0][0][i][0] = mor_s_v[0][i-1];
    }
    for (i = 0; i < LX1-1; i++) {
      idmo[iel][face][0][1][i][0] = mor_s_v[1][i];
    }
  }
}


//------------------------------------------------------------
// Copy mortar point indices from mor_s_v to local edge n
// on face "face" of element iel. nn is the edge mortar index,
// which indicates that mor_s_v  corresponds to left/bottom or 
// right/top part of the edge.
//------------------------------------------------------------
static void mor_s_e_nn(int n, int face, int iel, int mor_s_v[4], int nn)
{
  int i;

  if (n == 0) {
    if (nn == 0) {
      for (i = 1; i < LX1; i++) {
        idmo[iel][face][0][0][0][i] = mor_s_v[i-1];
      }
    } else {
      for (i = 0; i < LX1-1; i++) {
        idmo[iel][face][1][0][0][i] = mor_s_v[i];
      }
    }
  } else if (n == 1) {
    if (nn == 0) {
      for (i = 1; i < LX1; i++) {
        idmo[iel][face][1][0][i][LX1-1] = mor_s_v[i-1];
      }
    } else {
      for (i = 0; i < LX1-1; i++) {
        idmo[iel][face][1][1][i][LX1-1] = mor_s_v[i];
      }
    }
  } else if (n == 2) {
    if (nn == 0) {
      for (i = 1; i < LX1; i++) {
        idmo[iel][face][0][1][LX1-1][i] = mor_s_v[i-1];
      }
    } else {
      for (i = 0; i < LX1-1; i++) {
        idmo[iel][face][1][1][LX1-1][i] = mor_s_v[i];
      }
    }
  } else if (n == 3) {
    if (nn == 0) {
      for (i = 1; i < LX1; i++) {
        idmo[iel][face][0][0][i][0] = mor_s_v[i-1];
      }
    } else {
      for (i = 0; i < LX1-1; i++) {
        idmo[iel][face][0][1][i][0] = mor_s_v[i];
      }
    }
  }
}


//---------------------------------------------------------------
// Assign mortar point index "count" to iel's i'th vertex
// and also to all elements sharing this vertex.
//---------------------------------------------------------------
static void mortar_vertex(int i, int iel, int count)
{
  int ntempx[8], ifntempx[8], lc_a[3], nnb[3];
  int face_a[3], itemp, ntemp, ii, jj, j[3];
  int iintempx[3], l, nbe, lc, temp;
  logical if_temp;

  for (l = 0; l < 8; l++) {
    ntempx[l] = -1;
    ifntempx[l] = -1;
  }

  // face_a records the three faces sharing this vertex on iel.
  // lc_a gives the local corner number of this vertex on each 
  // face in face_a.
  for (l = 0; l < 3; l++) {
    face_a[l] = f_c[i][l];
    lc_a[l] = local_corner[face_a[l]][i];
  }

  // each vertex is shared by at most 8 elements. 
  // ntempx[j] gives the element index of a POSSIBLE element with its 
  // j'th  vertex is iel's i'th vertex
  // ifntempx[i]=ntempx[i] means  ntempx[i] exists 
  // ifntempx[i]=-1 means ntempx[i] does not exist.

  ntempx[7-i] = iel;
  ifntempx[7-i] = iel;

  // first find all elements sharing this vertex, ifntempx

  // find the three possible neighbors of iel, neighbored by faces 
  // listed in array face_a

  for (itemp = 0; itemp < 3; itemp++) {
    // j[itemp] is the local corner number of this vertex on the 
    // neighbor element on the corresponding face.
    j[itemp] = c_f[jjface[face_a[itemp]]][lc_a[itemp]];

    // iitempx[itemp] records the vertex index of i on the
    // neighbor element, neighborned by face_a[itemp]
    iintempx[itemp] = cal_intempx[face_a[itemp]][lc_a[itemp]];

    // ntemp refers the neighbor element 
    ntemp = -1;

    // if the face is nonconforming, find out in which piece of the 
    // mortar the vertex is located
    ii = cal_iijj[lc_a[itemp]][0];
    jj = cal_iijj[lc_a[itemp]][1];
    ntemp = sje[iel][face_a[itemp]][jj][ii];

    // if the face is conforming
    if (ntemp == -1) {
      ntemp = sje[iel][face_a[itemp]][0][0];
      // find the possible neighbor        
      ntempx[iintempx[itemp]] = ntemp;
      // check whether this possible neighbor is a real neighbor or not
      if (ntemp != -1) {
        if (ifsame(ntemp, j[itemp], iel, i)) {
          ifntempx[iintempx[itemp]] = ntemp;
        }
      }

      // if the face is nonconforming
    } else {
      if (ntemp != -1) {
        if (ifsame(ntemp, j[itemp], iel, i)) {
          ifntempx[iintempx[itemp]] = ntemp;
          ntempx[iintempx[itemp]] = ntemp;
        }
      }
    } 
  }

  // find the possible three neighbors, neighbored by an edge only
  for (l = 0; l < 3; l++) {
    // find first existing neighbor of any of the faces in array face_a
    if_temp = false;
    if (l == 0) {
      if_temp = true;
    } else if (l == 1) {
      if (ifntempx[iintempx[l-1]] == -1) {
        if_temp = true;
      }
    } else if (l == 2) {
      if (ifntempx[iintempx[l-1]] == -1 && ifntempx[iintempx[l-2]] == -1) {
        if_temp = true;
      }
    }

    if (if_temp) {
      if (ifntempx[iintempx[l]] != -1) {
        nbe = ifntempx[iintempx[l]];
        // if 1st neighor exists, check the neighbor's two neighbors in
        // the other two directions. 
        // e.g. if l=0, check directions 1 and 2,i.e. itemp=1,2,1
        // if l=1, itemp=2,0,-2
        // if l=2, itemp=0,1,1
         
        itemp = face_l1[l];
        while ((l != 1 && itemp <= face_l2[l]) ||
               (l == 1 && itemp >= face_l2[l])) {
          //lc is the local corner number of this vertex on face face_a[itemp]
          // on the neighbor element of iel, neighbored by a face face_a[l]
          lc = local_corner[face_a[itemp]][j[l]];
          // temp is the vertex index of this vertex on the neighbor element
          // neighbored by an edge
          temp = cal_intempx[face_a[itemp]][lc];
          ii = cal_iijj[lc][0];
          jj = cal_iijj[lc][1];
          ntemp = sje[nbe][face_a[itemp]][jj][ii];

          // if the face face_a[itemp] is conforming
          if (ntemp == -1) {
            ntemp = sje[nbe][face_a[itemp]][0][0];
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe, j[l])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                // nnb[itemp] records the neighbor element neighbored by an
                // edge only
                nnb[itemp] = ntemp;
              }
            }

            // if the face face_a[itemp] is nonconforming
          } else {
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe, j[l])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                nnb[itemp] = ntemp;
              }
            }
          }

          itemp += face_ld[l];
        }

        // check the last neighbor element, neighbored by an edge

        // ifntempx[iintempx[l]] has been visited in the above, now 
        // check another neighbor element(nbe) neighbored by a face 

        // if the neighbor element is neighbored by face 
        // face_a[face_l1[l]] exists
        if (ifntempx[iintempx[face_l1[l]]] != -1) {
          nbe = ifntempx[iintempx[face_l1[l]]];
          // itemp is the last direction other than l and face_l1[l]
          itemp = face_l2[l];
          lc = local_corner[face_a[itemp]][j[face_l1[l]]];
          temp = cal_intempx[face_a[itemp]][lc];
          ii = cal_iijj[lc][0];
          jj = cal_iijj[lc][1];

          // ntemp records the last neighbor element neighbored by an edge
          // with element iel
          ntemp = sje[nbe][face_a[itemp]][jj][ii];
          // if conforming
          if (ntemp == -1) {
            ntemp = sje[nbe][face_a[itemp]][0][0];
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe, 
                         j[face_l1[l]])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                nnb[l] = ntemp;
              }
            }
            // if nonconforming
          } else {
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe,
                         j[face_l1[l]])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                nnb[l] = ntemp;
              }
            }
          }

          // if the neighbor element neighbored by face face_a[face_l2[l]] 
          // does not exist
        } else if (ifntempx[iintempx[face_l2[l]]] != -1) {
          nbe = ifntempx[iintempx[face_l2[l]]];
          itemp = face_l1[l];
          lc = local_corner[face_a[itemp]][j[face_l2[l]]];
          temp = cal_intempx[face_a[itemp]][lc];
          ii = cal_iijj[lc][0];
          jj = cal_iijj[lc][1];
          ntemp = sje[nbe][face_a[itemp]][jj][ii];
          if (ntemp == -1) {
            ntemp = sje[nbe][face_a[itemp]][0][0];
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe,
                         j[face_l2[l]])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                nnb[l] = ntemp;
              }
            }
          } else {
            if (ntemp != -1) {
              if (ifsame(ntemp, c_f[jjface[face_a[itemp]]][lc], nbe,
                         j[face_l2[l]])) {
                ntempx[temp] = ntemp;
                ifntempx[temp] = ntemp;
                nnb[l] = ntemp;
              }
            }
          }
        }
      }
    }
  }

  // check the neighbor element, neighbored by a vertex only

  // nnb are the three possible neighbor elements neighbored by an edge

  nnb[0] = ifntempx[cal_nnb[i][0]];
  nnb[1] = ifntempx[cal_nnb[i][1]];
  nnb[2] = ifntempx[cal_nnb[i][2]];
  ntemp = -1;

  // the neighbor element neighbored by a vertex must be a neighbor of
  // a valid(non-negative) nnb[i], neighbored by a face 

  if (nnb[0] != -1) {
    lc = oplc[local_corner[face_a[2]][i]];
    ii = cal_iijj[lc][0];
    jj = cal_iijj[lc][1];
    // ntemp records the neighbor of iel, neighbored by vertex i 
    ntemp = sje[nnb[0]][face_a[2]][jj][ii];
    // temp is the vertex index of i on ntemp
    temp = cal_intempx[face_a[2]][lc];
    if (ntemp == -1) {
      ntemp = sje[nnb[0]][face_a[2]][0][0];
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[2]]][lc], iel, i)) {
          ntempx[temp] = ntemp;
          ifntempx[temp] = ntemp;
        }
      }
    } else {
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[2]]][lc], iel, i)) {
          ntempx[temp] = ntemp;
          ifntempx[temp] = ntemp;
        }
      }
    }
  } else if (nnb[1] != -1) {
    lc = oplc[local_corner[face_a[0]][i]];
    ii = cal_iijj[lc][0];
    jj = cal_iijj[lc][1];
    ntemp = sje[nnb[1]][face_a[0]][jj][ii];
    temp = cal_intempx[face_a[0]][lc];
    if (ntemp == -1) {
      ntemp = sje[nnb[1]][face_a[0]][0][0];
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[0]]][lc], iel, i)) {
          ntempx[temp] = ntemp;
          ifntempx[temp] = ntemp;
        }
      }
    } else {
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[0]]][lc], iel, i)) {
          ntempx[temp] = ntemp;
          ifntempx[temp] = ntemp;
        }
      }
    }
  } else if (nnb[2] != -1) {
    lc = oplc[local_corner[face_a[1]][i]];
    ii = cal_iijj[lc][0];
    jj = cal_iijj[lc][1];
    ntemp = sje[nnb[2]][face_a[1]][jj][ii];
    temp = cal_intempx[face_a[1]][lc];
    if (ntemp == -1) {
      ntemp = sje[nnb[2]][face_a[1]][0][0];
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[1]]][lc],iel,i)) {
          ifntempx[temp] = ntemp;
          ntempx[temp] = ntemp;
        }
      }
    } else {
      if (ntemp != -1) {
        if (ifsame(ntemp, c_f[jjface[face_a[1]]][lc],iel,i)) {
          ifntempx[temp] = ntemp;
          ntempx[temp] = ntemp;
        }
      }
    }
  }

  // ifntempx records all elements sharing this vertex, assign count
  // to all these elements.
  if (ifntempx[0] != -1) {
    idmo[ntempx[0]][0][1][1][LX1-1][LX1-1] = count;
    idmo[ntempx[0]][2][1][1][LX1-1][LX1-1] = count;
    idmo[ntempx[0]][4][1][1][LX1-1][LX1-1] = count;
    get_emo(ntempx[0], count, 7);
  }

  if (ifntempx[1] != -1) {
    idmo[ntempx[1]][1][1][1][LX1-1][LX1-1] = count;
    idmo[ntempx[1]][2][0][1][LX1-1][0] = count;
    idmo[ntempx[1]][4][0][1][LX1-1][0] = count;
    get_emo(ntempx[1], count, 6);
  }

  if (ifntempx[2] != -1) {
    idmo[ntempx[2]][0][0][1][LX1-1][0] = count;
    idmo[ntempx[2]][3][1][1][LX1-1][LX1-1] = count;
    idmo[ntempx[2]][4][1][0][0][LX1-1] = count;
    get_emo(ntempx[2], count, 5);
  }
  if (ifntempx[3] != -1) {
    idmo[ntempx[3]][1][0][1][LX1-1][0] = count;
    idmo[ntempx[3]][3][0][1][LX1-1][0] = count;
    idmo[ntempx[3]][4][0][0][0][0] = count;
    get_emo(ntempx[3], count, 4);
  }

  if (ifntempx[4] != -1) {
    idmo[ntempx[4]][0][1][0][0][LX1-1] = count;
    idmo[ntempx[4]][2][1][0][0][LX1-1] = count;
    idmo[ntempx[4]][5][1][1][LX1-1][LX1-1] = count;
    get_emo(ntempx[4], count, 3);
  }


  if (ifntempx[5] != -1) {
    idmo[ntempx[5]][1][1][0][0][LX1-1] = count;
    idmo[ntempx[5]][2][0][0][0][0] = count;
    idmo[ntempx[5]][5][0][1][LX1-1][0] = count;
    get_emo(ntempx[5], count, 2);
  }

  if (ifntempx[6] != -1) {
    idmo[ntempx[6]][0][0][0][0][0] = count;
    idmo[ntempx[6]][3][1][0][0][LX1-1] = count;
    idmo[ntempx[6]][5][1][0][0][LX1-1] = count;
    get_emo(ntempx[6], count, 1);
  }

  if (ifntempx[7] != -1) {
    idmo[ntempx[7]][1][0][0][0][0] = count;
    idmo[ntempx[7]][3][0][0][0][0] = count;
    idmo[ntempx[7]][5][0][0][0][0] = count;
    get_emo(ntempx[7], count, 0);
  }
}

     
//---------------------------------------------------------------
// Copy the mortar points index  (mor_v + vertex mortar point) from
// edge'th local edge on face'th face on element ntemp to iel.
// ntemp is iel's neighbor, neighbored by this edge only. 
// This subroutine is for the situation that iel is of larger
// size than ntemp.  
// face, face2 are face indices
// edge and edge2 are local edge numbers of this edge on face and face2
// nn is edge motar index, which indicate whether this edge
// corresponds to the left/bottom or right/top part of the edge
// on iel.
//---------------------------------------------------------------
static void mor_ne(int mor_v[3], int nn, int edge, int face,
                   int edge2, int face2, int ntemp, int iel)
{
  int i, mor_s_v[4] = {0,};

  // get mor_s_v which is the mor_v + vertex mortar
  if (edge == 2) {
    if (nn == 0) {
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i-1] = mor_v[i-1];
      }
      mor_s_v[3] = idmo[ntemp][face][1][1][LX1-1][LX1-1];
    } else {
      mor_s_v[0] = idmo[ntemp][face][0][1][LX1-1][0];
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i] = mor_v[i-1];
      }
    }

  } else if (edge == 3) {
    if (nn == 0) {
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i-1] = mor_v[i-1];
      }
      mor_s_v[3] = idmo[ntemp][face][0][1][LX1-1][0];
    } else {
      mor_s_v[0] = idmo[ntemp][face][0][0][0][0];
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i] = mor_v[i-1];
      }
    }

  } else if (edge == 0) {
    if (nn == 0) {
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i-1] = mor_v[i-1];
      }
      mor_s_v[3] = idmo[ntemp][face][1][0][0][LX1-1];
    } else {
      mor_s_v[0] = idmo[ntemp][face][0][0][0][0];
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i] = mor_v[i-1];
      }
    }

  } else if (edge == 1) {
    if (nn == 0) {
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i-1] = mor_v[i-1];
      }
      mor_s_v[3] = idmo[ntemp][face][1][1][LX1-1][LX1-1];
    } else {
      mor_s_v[0] = idmo[ntemp][face][1][0][0][LX1-1];
      for (i = 1; i < LX1-1; i++) {
        mor_s_v[i] = mor_v[i-1];
      }
    }
  }

  // copy mor_s_v to iel's local edge(op[edge]), on face jjface[face]
  mor_s_e_nn(op[edge], jjface[face], iel, mor_s_v, nn);
  // copy mor_s_v to iel's local edge(op[edge2]),  on face jjface[face2]
  // since this edge is shared by two faces on iel
  mor_s_e_nn(op[edge2], jjface[face2], iel, mor_s_v, nn);
}
