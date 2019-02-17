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

#include <math.h>
#include "header.h"

void create_initial_grid()
{
  int i;

  nelt = 1;
  ntot = nelt*LX1*LX1*LX1;
  tree[0] = 1;
  mt_to_id[0] = 0;
  for (i = 0; i < 7; i += 2) {
    xc[0][i] = 0.0;
    xc[0][i+1] = 1.0;
  }

  for (i = 0; i < 2; i++) {
    yc[0][i] = 0.0;
    yc[0][2+i] = 1.0;
    yc[0][4+i] = 0.0;
    yc[0][6+i] = 1.0;
  }

  for (i = 0; i < 4; i++) {
    zc[0][i] = 0.0;
    zc[0][4+i] = 1.0;
  }

  for (i = 0; i < 6; i++) {
    cbc[0][i] = 0;
  }
}


//-----------------------------------------------------------------
//
// generate 
//
//        - collocation points
//        - weights
//        - derivative matrices 
//        - projection matrices
//        - interpolation matrices 
//
// associated with the 
//
//        - gauss-legendre lobatto mesh (suffix m1)
//
//----------------------------------------------------------------
void coef()
{
  int i, j, k;

  // for gauss-legendre lobatto mesh (suffix m1)
  // generate collocation points and weights 
  zgm1[0] = -1.0;
  zgm1[1] = -0.65465367070797710;
  zgm1[2] = 0.0;
  zgm1[3] =  0.65465367070797710;
  zgm1[4] = 1.0;
  wxm1[0] = 0.10;
  wxm1[1] = 49.0/90.0;
  wxm1[2] = 32.0/45.0;
  wxm1[3] = wxm1[1];
  wxm1[4] = 0.1;

  for (k = 0; k < LX1; k++) {
    for (j = 0; j < LX1; j++) {
      for (i = 0; i < LX1; i++) {
        w3m1[k][j][i] = wxm1[i]*wxm1[j]*wxm1[k];
      }
    }
  }

  // generate derivative matrices
  dxm1[0][0] = -5.0;
  dxm1[0][1] = -1.240990253030982;
  dxm1[0][2] =  0.375;
  dxm1[0][3] = -0.2590097469690172;
  dxm1[0][4] =  0.5;
  dxm1[1][0] =  6.756502488724238;
  dxm1[1][1] =  0.0;
  dxm1[1][2] = -1.336584577695453;
  dxm1[1][3] =  0.7637626158259734;
  dxm1[1][4] = -1.410164177942427;
  dxm1[2][0] = -2.666666666666667;
  dxm1[2][1] =  1.745743121887939;
  dxm1[2][2] =  0.0;
  dxm1[2][3] = -dxm1[2][1];
  dxm1[2][4] = -dxm1[2][0];
  for (j = 3; j < LX1; j++) {
    for (i = 0; i < LX1; i++) {
      dxm1[j][i] = -dxm1[LX1-1-j][LX1-1-i];
    }
  }
  for (j = 0; j < LX1; j++) {
    for (i = 0; i < LX1; i++) {
      dxtm1[j][i] = dxm1[i][j];
    }
  }

  // generate projection (mapping) matrices
  qbnew[0][0][0] = -0.1772843218615690;
  qbnew[0][0][1] = 9.375e-02;
  qbnew[0][0][2] = -3.700139242414530e-02;
  qbnew[0][1][0] =  0.7152146412463197;
  qbnew[0][1][1] = -0.2285757930375471;
  qbnew[0][1][2] =  8.333333333333333e-02;
  qbnew[0][2][0] =  0.4398680650316104;
  qbnew[0][2][1] =  0.2083333333333333;
  qbnew[0][2][2] = -5.891568407922938e-02;
  qbnew[0][3][0] =  8.333333333333333e-02;
  qbnew[0][3][1] =  0.3561799597042137;
  qbnew[0][3][2] = -4.854797457965334e-02;
  qbnew[0][4][0] =  0.0;
  qbnew[0][4][1] = 7.03125e-02;
  qbnew[0][4][2] = 0.0;

  for (j = 0; j < LX1; j++) {
    for (i = 0; i < 3; i++) {
      qbnew[1][j][i] = qbnew[0][LX1-1-j][2-i];
    }
  } 

  // generate interpolation matrices for mesh refinement
  ixtmc1[0][0] = 1.0;
  ixtmc1[0][1] = 0.0;
  ixtmc1[0][2] = 0.0;
  ixtmc1[0][3] = 0.0;
  ixtmc1[0][4] = 0.0;
  ixtmc1[1][0] =  0.3385078435248143;
  ixtmc1[1][1] =  0.7898516348912331;
  ixtmc1[1][2] = -0.1884018684471238;
  ixtmc1[1][3] =  9.202967302175333e-02;
  ixtmc1[1][4] = -3.198728299067715e-02;
  ixtmc1[2][0] = -0.1171875;
  ixtmc1[2][1] =  0.8840317166357952;
  ixtmc1[2][2] =  0.3125;
  ixtmc1[2][3] = -0.118406716635795;
  ixtmc1[2][4] =  0.0390625;
  ixtmc1[3][0] = -7.065070066767144e-02;
  ixtmc1[3][1] =  0.2829703269782467;
  ixtmc1[3][2] =  0.902687582732838;
  ixtmc1[3][3] = -0.1648516348912333;
  ixtmc1[3][4] =  4.984442584781999e-02;
  ixtmc1[4][0] = 0.0;
  ixtmc1[4][1] = 0.0;
  ixtmc1[4][2] = 1.0;
  ixtmc1[4][3] = 0.0;
  ixtmc1[4][4] = 0.0;
  for (j = 0; j < LX1; j++) {
    for (i = 0; i < LX1; i++) {
      ixmc1[j][i] = ixtmc1[i][j];
    }
  }

  for (j = 0; j < LX1; j++) {
    for (i = 0; i < LX1; i++) {
      ixtmc2[j][i] = ixtmc1[LX1-1-j][LX1-1-i];
    }
  }

  for (j = 0; j < LX1; j++) {
    for (i = 0; i < LX1; i++) {
      ixmc2[j][i] = ixtmc2[i][j];
    }
  }

  // solution interpolation matrix for mesh coarsening
  map2[0] = -0.1179652785083428;
  map2[1] =  0.5505046330389332;
  map2[2] =  0.7024534364259963;
  map2[3] = -0.1972224518285866;
  map2[4] =  6.222966087199998e-02;

  for (i = 0; i < LX1; i++) {
    map4[i] = map2[LX1-1-i];
  }
}


//-------------------------------------------------------------------
//
// routine to generate elemental geometry information on mesh m1,
// (gauss-legendre lobatto mesh).
//
//     xrm1_s   -   dx/dr, dy/dr, dz/dr
//     rxm1_s   -   dr/dx, dr/dy, dr/dz
//     g1m1_s  geometric factors used in preconditioner computation
//     g4m1_s  g5m1_s  g6m1_s :
//     geometric factors used in lapacian opertor
//     jacm1    -   jacobian
//     bm1      -   mass matrix
//     xfrac    -   will be used in prepwork for calculating collocation
//                      coordinates
//     idel     -   collocation points index on element boundaries 
//------------------------------------------------------------------
void geom1()
{
  double temp, temp1, temp2, dtemp;
  int isize, i, j, k, ntemp, iel;

  for (i = 0; i < LX1; i++) {
    xfrac[i] = zgm1[i]*0.5 + 0.5;
  }

  for (isize = 0; isize < REFINE_MAX; isize++) {
    temp = pow(2.0, (-isize-2));
    dtemp = 1.0/temp;
    temp1 = temp*temp*temp;
    temp2 = temp*temp;
    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          xrm1_s[isize][k][j][i] = dtemp;
          jacm1_s[isize][k][j][i] = temp1;
          rxm1_s[isize][k][j][i] = temp2;
          g1m1_s[isize][k][j][i] = w3m1[k][j][i]*temp;
          bm1_s[isize][k][j][i] = w3m1[k][j][i]*temp1;
          g4m1_s[isize][k][j][i] = g1m1_s[isize][k][j][i]/wxm1[i];
          g5m1_s[isize][k][j][i] = g1m1_s[isize][k][j][i]/wxm1[j];
          g6m1_s[isize][k][j][i] = g1m1_s[isize][k][j][i]/wxm1[k];
        }
      }
    }
  }

  for (iel = 0; iel < LELT; iel++) {
    ntemp = LX1*LX1*LX1*iel;
    for (j = 0; j < LX1; j++) {
      for (i = 0; i < LX1; i++) {
        idel[iel][0][j][i] = ntemp+i*LX1 + j*LX1*LX1+LX1 - 1;
        idel[iel][1][j][i] = ntemp+i*LX1 + j*LX1*LX1;
        idel[iel][2][j][i] = ntemp+i*1 + j*LX1*LX1+LX1*(LX1-1);
        idel[iel][3][j][i] = ntemp+i*1 + j*LX1*LX1;
        idel[iel][4][j][i] = ntemp+i*1 + j*LX1+LX1*LX1*(LX1-1);
        idel[iel][5][j][i] = ntemp+i*1 + j*LX1;
      }
    }
  }
}


//------------------------------------------------------------------
// compute the discrete laplacian operators
//------------------------------------------------------------------
void setdef()
{
  int i, j, ip;

  r_init(wdtdr[0], LX1*LX1, 0.0);

  for (i = 0; i < LX1; i++) {
    for (j = 0; j < LX1; j++) {
      for (ip = 0; ip < LX1; ip++) {
        wdtdr[j][i] = wdtdr[j][i] + wxm1[ip]*dxm1[i][ip]*dxm1[j][ip];
      }
    }
  }
}


//------------------------------------------------------------------
// mesh information preparations: calculate refinement levels of
// each element, mask matrix for domain boundary and element 
// boundaries
//------------------------------------------------------------------
void prepwork()
{
  int i, j, iel, iface, cb;
  double rdlog2;

  ntot = nelt*NXYZ;
  rdlog2 = 1.0/log(2.0);

  // calculate the refinement levels of each element
  for (iel = 0; iel < nelt; iel++) {
    size_e[iel] = (int)(-log(xc[iel][1]-xc[iel][0])*rdlog2+1.e-8) - 1;
  }

  // mask matrix for element boundary
  for (iel = 0; iel < nelt; iel++) {
    r_init(tmult[iel][0][0], NXYZ, 1.0);
    for (iface = 0; iface < NSIDES; iface++) {
      facev(tmult[iel], iface, 0.0);
    }
  }

  // masks for domain boundary at mortar 
  r_init(tmmor, nmor, 1.0);

  for (iel = 0; iel < nelt; iel++) {
    for (iface = 0; iface < NSIDES; iface++) {
      cb = cbc[iel][iface];
      if (cb == 0) {
        for (j = 1; j < LX1-1; j++) {
          for (i = 1; i < LX1-1; i++) {
            tmmor[idmo[iel][iface][0][0][j][i]] = 0.0;
          }
        }

        j = 0;
        for (i = 0; i < LX1-1; i++) {
          tmmor[idmo[iel][iface][0][0][j][i]] = 0.0;
        }

        if (idmo[iel][iface][0][0][0][LX1-1] == -1) {
          tmmor[idmo[iel][iface][1][0][0][LX1-1]] = 0.0;
        } else {
          tmmor[idmo[iel][iface][0][0][0][LX1-1]] = 0.0;
          for (i = 0; i < LX1; i++) {
            tmmor[idmo[iel][iface][1][0][j][i]] = 0.0;
          }
        }

        i = LX1-1;
        if (idmo[iel][iface][1][0][1][LX1-1] == -1) {
          for (j = 1; j < LX1-1; j++) {
            tmmor[idmo[iel][iface][0][0][j][i]] = 0.0;
          }
          tmmor[idmo[iel][iface][1][1][LX1-1][LX1-1]] = 0.0;
        } else {
          for (j = 1; j < LX1; j++) {
            tmmor[idmo[iel][iface][1][0][j][i]] = 0.0;
          }
          for (j = 0; j < LX1; j++) {
            tmmor[idmo[iel][iface][1][1][j][i]] = 0.0;
          }
        }

        j = LX1-1;
        tmmor[idmo[iel][iface][0][1][LX1-1][0]] = 0.0;
        if (idmo[iel][iface][0][1][LX1-1][1] == -1) {
          for (i = 1; i < LX1-1; i++) {
            tmmor[idmo[iel][iface][0][0][j][i]] = 0.0;
          }
        } else {
          for (i = 1; i < LX1; i++) {
            tmmor[idmo[iel][iface][0][1][j][i]] = 0.0;
          }
          for (i = 0; i < LX1-1; i++) {
            tmmor[idmo[iel][iface][1][1][j][i]] = 0.0;
          }
        }

        i = 0;
        for (j = 1; j < LX1-1; j++) {
          tmmor[idmo[iel][iface][0][0][j][i]] = 0.0;
        }
        if (idmo[iel][iface][0][0][LX1-1][0] != -1) {
          tmmor[idmo[iel][iface][0][0][LX1-1][i]] = 0.0;
          for (j = 0; j < LX1-1; j++) {
            tmmor[idmo[iel][iface][0][1][j][i]] = 0.0;
          }
        }
      }
    }
  }
}
            

//------------------------------------------------------------------
// We store some tables of useful topological constants
//------------------------------------------------------------------
void top_constants()
{
  //f_e_ef[f][e] returns the other face sharing the e'th local edge of face f.
  f_e_ef[0][0] = 5;
  f_e_ef[0][1] = 2;
  f_e_ef[0][2] = 4;
  f_e_ef[0][3] = 3;
  f_e_ef[1][0] = 5;
  f_e_ef[1][1] = 2;
  f_e_ef[1][2] = 4;
  f_e_ef[1][3] = 3;
  f_e_ef[2][0] = 5;
  f_e_ef[2][1] = 0;
  f_e_ef[2][2] = 4;
  f_e_ef[2][3] = 1;
  f_e_ef[3][0] = 5;
  f_e_ef[3][1] = 0;
  f_e_ef[3][2] = 4;
  f_e_ef[3][3] = 1;
  f_e_ef[4][0] = 3;
  f_e_ef[4][1] = 0;
  f_e_ef[4][2] = 2;
  f_e_ef[4][3] = 1;
  f_e_ef[5][0] = 3;
  f_e_ef[5][1] = 0;
  f_e_ef[5][2] = 2;
  f_e_ef[5][3] = 1;

  // e_c[j][n] returns n'th edge sharing the vertex j of an element
  e_c[0][0] = 4;
  e_c[0][1] = 7;
  e_c[0][2] = 10;
  e_c[1][0] = 0;
  e_c[1][1] = 3;
  e_c[1][2] = 10;
  e_c[2][0] = 4;
  e_c[2][1] = 5;
  e_c[2][2] = 8;
  e_c[3][0] = 0;
  e_c[3][1] = 1;
  e_c[3][2] = 8;
  e_c[4][0] = 6;
  e_c[4][1] = 7;
  e_c[4][2] = 11;
  e_c[5][0] = 2;
  e_c[5][1] = 3;
  e_c[5][2] = 11;
  e_c[6][0] = 5;
  e_c[6][1] = 6;
  e_c[6][2] = 9;
  e_c[7][0] = 1;
  e_c[7][1] = 2;
  e_c[7][2] = 9;

  // local_corner[i][n] returns the local corner index of vertex n on face i
  local_corner[0][0] = -1;
  local_corner[0][1] = 0;
  local_corner[0][2] = -1;
  local_corner[0][3] = 1;
  local_corner[0][4] = -1;
  local_corner[0][5] = 2;
  local_corner[0][6] = -1;
  local_corner[0][7] = 3;
  local_corner[1][0] = 0;
  local_corner[1][1] = -1;
  local_corner[1][2] = 1;
  local_corner[1][3] = -1;
  local_corner[1][4] = 2;
  local_corner[1][5] = -1;
  local_corner[1][6] = 3;
  local_corner[1][7] = -1;
  local_corner[2][0] = -1;
  local_corner[2][1] = -1;
  local_corner[2][2] = 0;
  local_corner[2][3] = 1;
  local_corner[2][4] = -1;
  local_corner[2][5] = -1;
  local_corner[2][6] = 2;
  local_corner[2][7] = 3;
  local_corner[3][0] = 0;
  local_corner[3][1] = 1;
  local_corner[3][2] = -1;
  local_corner[3][3] = -1;
  local_corner[3][4] = 2;
  local_corner[3][5] = 3;
  local_corner[3][6] = -1;
  local_corner[3][7] = -1;
  local_corner[4][0] = -1;
  local_corner[4][1] = -1;
  local_corner[4][2] = -1;
  local_corner[4][3] = -1;
  local_corner[4][4] = 0;
  local_corner[4][5] = 1;
  local_corner[4][6] = 2;
  local_corner[4][7] = 3;
  local_corner[5][0] = 0;
  local_corner[5][1] = 1;
  local_corner[5][2] = 2;
  local_corner[5][3] = 3;
  local_corner[5][4] = -1;
  local_corner[5][5] = -1;
  local_corner[5][6] = -1;
  local_corner[5][7] = -1;

  // cal_nnb[i][n] returns the neighbor elements neighbored by n'th edge
  // among the three edges sharing vertex i
  // the elements are the eight children elements ordered as 0 to 7.
  cal_nnb[0][0] = 4;
  cal_nnb[0][1] = 1;
  cal_nnb[0][2] = 2;
  cal_nnb[1][0] = 5;
  cal_nnb[1][1] = 0;
  cal_nnb[1][2] = 3;
  cal_nnb[2][0] = 6;
  cal_nnb[2][1] = 3;
  cal_nnb[2][2] = 0;
  cal_nnb[3][0] = 7;
  cal_nnb[3][1] = 2;
  cal_nnb[3][2] = 1;
  cal_nnb[4][0] = 0;
  cal_nnb[4][1] = 5;
  cal_nnb[4][2] = 6;
  cal_nnb[5][0] = 1;
  cal_nnb[5][1] = 4;
  cal_nnb[5][2] = 7;
  cal_nnb[6][0] = 2;
  cal_nnb[6][1] = 7;
  cal_nnb[6][2] = 4;
  cal_nnb[7][0] = 3;
  cal_nnb[7][1] = 6;
  cal_nnb[7][2] = 5;

  // returns the opposite local corner index: 0-3,1-2
  oplc[0] = 3;
  oplc[1] = 2;
  oplc[2] = 1;
  oplc[3] = 0;

  // cal_iijj[n][i] returns the location of local corner number n on a face 
  // i =0  to get ii, i=1 to get jj
  // (ii,jj) is defined the same as in mortar location (ii,jj)
  cal_iijj[0][0] = 0;
  cal_iijj[0][1] = 0;
  cal_iijj[1][0] = 0;
  cal_iijj[1][1] = 1;
  cal_iijj[2][0] = 1;
  cal_iijj[2][1] = 0;
  cal_iijj[3][0] = 1;
  cal_iijj[3][1] = 1;

  // returns the adjacent(neighbored by a face) element's children,
  // assumming a vertex is shared by eight child elements 0-7. 
  // index n is local corner number on the face which is being 
  // assigned the mortar index number
  cal_intempx[0][0] = 7;
  cal_intempx[0][1] = 5;
  cal_intempx[0][2] = 3;
  cal_intempx[0][3] = 1;
  cal_intempx[1][0] = 6;
  cal_intempx[1][1] = 4;
  cal_intempx[1][2] = 2;
  cal_intempx[1][3] = 0;
  cal_intempx[2][0] = 7;
  cal_intempx[2][1] = 6;
  cal_intempx[2][2] = 3;
  cal_intempx[2][3] = 2;
  cal_intempx[3][0] = 5;
  cal_intempx[3][1] = 4;
  cal_intempx[3][2] = 1;
  cal_intempx[3][3] = 0;
  cal_intempx[4][0] = 7;
  cal_intempx[4][1] = 6;
  cal_intempx[4][2] = 5;
  cal_intempx[4][3] = 4;
  cal_intempx[5][0] = 3;
  cal_intempx[5][1] = 2;
  cal_intempx[5][2] = 1;
  cal_intempx[5][3] = 0;

  // c_f[f][i] returns the vertex number of i'th local corner on face f
  c_f[0][0] = 1;
  c_f[0][1] = 3;
  c_f[0][2] = 5;
  c_f[0][3] = 7;
  c_f[1][0] = 0;
  c_f[1][1] = 2;
  c_f[1][2] = 4;
  c_f[1][3] = 6;
  c_f[2][0] = 2;
  c_f[2][1] = 3;
  c_f[2][2] = 6;
  c_f[2][3] = 7;
  c_f[3][0] = 0;
  c_f[3][1] = 1;
  c_f[3][2] = 4;
  c_f[3][3] = 5;
  c_f[4][0] = 4;
  c_f[4][1] = 5;
  c_f[4][2] = 6;
  c_f[4][3] = 7;
  c_f[5][0] = 0;
  c_f[5][1] = 1;
  c_f[5][2] = 2;
  c_f[5][3] = 3;

  // on each face of the parent element, there are four children element.
  //le_arr[n][j][i] returns the i'th elements among the four children elements
  // n refers to the direction: 1 for x, 2 for y and 3 for z direction. 
  // j refers to positive(0) or negative(1) direction on x, y or z direction.
  // n=1,j=0 refers to face 1 and n=1, j=1 refers to face 2, n=2,j=0 refers to
  // face 3.... 
  // The current eight children are ordered as 8,1,2,3,4,5,6,7 
  le_arr[0][0][0] = 7;
  le_arr[0][0][1] = 1;
  le_arr[0][0][2] = 3;
  le_arr[0][0][3] = 5;
  le_arr[0][1][0] = 0;
  le_arr[0][1][1] = 2;
  le_arr[0][1][2] = 4;
  le_arr[0][1][3] = 6;
  le_arr[1][0][0] = 7;
  le_arr[1][0][1] = 0;
  le_arr[1][0][2] = 3;
  le_arr[1][0][3] = 4;
  le_arr[1][1][0] = 1;
  le_arr[1][1][1] = 2;
  le_arr[1][1][2] = 5;
  le_arr[1][1][3] = 6;
  le_arr[2][0][0] = 7;
  le_arr[2][0][1] = 0;
  le_arr[2][0][2] = 1;
  le_arr[2][0][3] = 2;
  le_arr[2][1][0] = 3;
  le_arr[2][1][1] = 4;
  le_arr[2][1][2] = 5;
  le_arr[2][1][3] = 6;

  // jjface[n] returns the face opposite to face n
  jjface[0] = 1;
  jjface[1] = 0;
  jjface[2] = 3;
  jjface[3] = 2;
  jjface[4] = 5;
  jjface[5] = 4;

  // edgeface[f][n] returns OTHER face which shares local edge n on face f
  // int edgeface[6][4];
  //  edgeface[0][0] = 5;
  //  edgeface[0][1] = 2;
  //  edgeface[0][2] = 4;
  //  edgeface[0][3] = 3;
  //  edgeface[1][0] = 5;
  //  edgeface[1][1] = 2;
  //  edgeface[1][2] = 4;
  //  edgeface[1][3] = 3;
  //  edgeface[2][0] = 5;
  //  edgeface[2][1] = 0;
  //  edgeface[2][2] = 4;
  //  edgeface[2][3] = 1;
  //  edgeface[3][0] = 5;
  //  edgeface[3][1] = 0;
  //  edgeface[3][2] = 4;
  //  edgeface[3][3] = 1;
  //  edgeface[4][0] = 3;
  //  edgeface[4][1] = 0;
  //  edgeface[4][2] = 2;
  //  edgeface[4][3] = 1;
  //  edgeface[5][0] = 3;
  //  edgeface[5][1] = 0;
  //  edgeface[5][2] = 2;
  //  edgeface[5][3] = 1;

  // e_face2[f][n] returns the local edge number of edge n on the
  // other face sharing local edge n on face f
  e_face2[0][0] = 1;
  e_face2[0][1] = 1;
  e_face2[0][2] = 1;
  e_face2[0][3] = 1;
  e_face2[1][0] = 3;
  e_face2[1][1] = 3;
  e_face2[1][2] = 3;
  e_face2[1][3] = 3;
  e_face2[2][0] = 2;
  e_face2[2][1] = 1;
  e_face2[2][2] = 2;
  e_face2[2][3] = 1;
  e_face2[3][0] = 0;
  e_face2[3][1] = 3;
  e_face2[3][2] = 0;
  e_face2[3][3] = 3;
  e_face2[4][0] = 2;
  e_face2[4][1] = 2;
  e_face2[4][2] = 2;
  e_face2[4][3] = 2;
  e_face2[5][0] = 0;
  e_face2[5][1] = 0;
  e_face2[5][2] = 0;
  e_face2[5][3] = 0;

  // op[n] returns the local edge number of the edge which 
  // is opposite to local edge n on the same face
  op[0] = 2;
  op[1] = 3;
  op[2] = 0;
  op[3] = 1;

  // localedgenumber[e][f] returns the local edge number for edge e
  // on face f. A minus result value signifies illegal input
  localedgenumber[0][0] = 0;
  localedgenumber[0][1] = -1;
  localedgenumber[0][2] = -1;
  localedgenumber[0][3] = -1;
  localedgenumber[0][4] = -1;
  localedgenumber[0][5] = 1;
  localedgenumber[1][0] = 1;
  localedgenumber[1][1] = -1;
  localedgenumber[1][2] = 1;
  localedgenumber[1][3] = -1;
  localedgenumber[1][4] = -1;
  localedgenumber[1][5] = -1;
  localedgenumber[2][0] = 2;
  localedgenumber[2][1] = -1;
  localedgenumber[2][2] = -1;
  localedgenumber[2][3] = -1;
  localedgenumber[2][4] = 1;
  localedgenumber[2][5] = -1;
  localedgenumber[3][0] = 3;
  localedgenumber[3][1] = -1;
  localedgenumber[3][2] = -1;
  localedgenumber[3][3] = 1;
  localedgenumber[3][4] = -1;
  localedgenumber[3][5] = -1;
  localedgenumber[4][0] = -1;
  localedgenumber[4][1] = 0;
  localedgenumber[4][2] = -1;
  localedgenumber[4][3] = -1;
  localedgenumber[4][4] = -1;
  localedgenumber[4][5] = 3;
  localedgenumber[5][0] = -1;
  localedgenumber[5][1] = 1;
  localedgenumber[5][2] = 3;
  localedgenumber[5][3] = -1;
  localedgenumber[5][4] = -1;
  localedgenumber[5][5] = -1;
  localedgenumber[6][0] = -1;
  localedgenumber[6][1] = 2;
  localedgenumber[6][2] = -1;
  localedgenumber[6][3] = -1;
  localedgenumber[6][4] = 3;
  localedgenumber[6][5] = -1;
  localedgenumber[7][0] = -1;
  localedgenumber[7][1] = 3;
  localedgenumber[7][2] = -1;
  localedgenumber[7][3] = 3;
  localedgenumber[7][4] = -1;
  localedgenumber[7][5] = -1;
  localedgenumber[8][0] = -1;
  localedgenumber[8][1] = -1;
  localedgenumber[8][2] = 0;
  localedgenumber[8][3] = -1;
  localedgenumber[8][4] = -1;
  localedgenumber[8][5] = 2;
  localedgenumber[9][0] = -1;
  localedgenumber[9][1] = -1;
  localedgenumber[9][2] = 2;
  localedgenumber[9][3] = -1;
  localedgenumber[9][4] = 2;
  localedgenumber[9][5] = -1;
  localedgenumber[10][0] = -1;
  localedgenumber[10][1] = -1;
  localedgenumber[10][2] = -1;
  localedgenumber[10][3] = 0;
  localedgenumber[10][4] = -1;
  localedgenumber[10][5] = 0;
  localedgenumber[11][0] = -1;
  localedgenumber[11][1] = -1;
  localedgenumber[11][2] = -1;
  localedgenumber[11][3] = 2;
  localedgenumber[11][4] = 0;
  localedgenumber[11][5] = -1;

  // edgenumber[f][e] returns the edge index of local edge e on face f
  edgenumber[0][0] = 0;
  edgenumber[0][1] = 1;
  edgenumber[0][2] = 2;
  edgenumber[0][3] = 3;
  edgenumber[1][0] = 4;
  edgenumber[1][1] = 5;
  edgenumber[1][2] = 6;
  edgenumber[1][3] = 7;
  edgenumber[2][0] = 8;
  edgenumber[2][1] = 1;
  edgenumber[2][2] = 9;
  edgenumber[2][3] = 5;
  edgenumber[3][0] = 10;
  edgenumber[3][1] = 3;
  edgenumber[3][2] = 11;
  edgenumber[3][3] = 7;
  edgenumber[4][0] = 11;
  edgenumber[4][1] = 2;
  edgenumber[4][2] = 9;
  edgenumber[4][3] = 6;
  edgenumber[5][0] = 10;
  edgenumber[5][1] = 0;
  edgenumber[5][2] = 8;
  edgenumber[5][3] = 4;

  // f_c[n][c] returns the face index of i'th face sharing vertex n 
  f_c[0][0] = 1;
  f_c[0][1] = 3;
  f_c[0][2] = 5;
  f_c[1][0] = 0;
  f_c[1][1] = 3;
  f_c[1][2] = 5;
  f_c[2][0] = 1;
  f_c[2][1] = 2;
  f_c[2][2] = 5;
  f_c[3][0] = 0;
  f_c[3][1] = 2;
  f_c[3][2] = 5;
  f_c[4][0] = 1;
  f_c[4][1] = 3;
  f_c[4][2] = 4;
  f_c[5][0] = 0;
  f_c[5][1] = 3;
  f_c[5][2] = 4;
  f_c[6][0] = 1;
  f_c[6][1] = 2;
  f_c[6][2] = 4;
  f_c[7][0] = 0;
  f_c[7][1] = 2;
  f_c[7][2] = 4;

  // if two elements are neighbor by one edge, 
  // e1v1[f2][f1] returns the smaller index of the two vertices on this 
  // edge on one element
  // e1v2 returns the larger index of the two vertices of this edge on 
  // on element. exfor a vertex on element 
  // e2v1 returns the smaller index of the two vertices on this edge on 
  // another element
  // e2v2 returns the larger index of the two vertiex on this edge on
  // another element
  //e1v1
  e1v1[0][0] = -1;
  e1v1[0][1] = -1;
  e1v1[0][2] = 3;
  e1v1[0][3] = 1;
  e1v1[0][4] = 5;
  e1v1[0][5] = 1;
  e1v1[1][0] = -1;
  e1v1[1][1] = -1;
  e1v1[1][2] = 2;
  e1v1[1][3] = 0;
  e1v1[1][4] = 4;
  e1v1[1][5] = 0;
  e1v1[2][0] = 3;
  e1v1[2][1] = 2;
  e1v1[2][2] = -1;
  e1v1[2][3] = -1;
  e1v1[2][4] = 6;
  e1v1[2][5] = 2;
  e1v1[3][0] = 1;
  e1v1[3][1] = 0;
  e1v1[3][2] = -1;
  e1v1[3][3] = -1;
  e1v1[3][4] = 4;
  e1v1[3][5] = 0;
  e1v1[4][0] = 5;
  e1v1[4][1] = 4;
  e1v1[4][2] = 6;
  e1v1[4][3] = 4;
  e1v1[4][4] = -1;
  e1v1[4][5] = -1;
  e1v1[5][0] = 1;
  e1v1[5][1] = 0;
  e1v1[5][2] = 2;
  e1v1[5][3] = 0;
  e1v1[5][4] = -1;
  e1v1[5][5] = -1;

  //e2v1
  e2v1[0][0] = -1;
  e2v1[0][1] = -1;
  e2v1[0][2] = 0;
  e2v1[0][3] = 2;
  e2v1[0][4] = 0;
  e2v1[0][5] = 4;
  e2v1[1][0] = -1;
  e2v1[1][1] = -1;
  e2v1[1][2] = 1;
  e2v1[1][3] = 3;
  e2v1[1][4] = 1;
  e2v1[1][5] = 5;
  e2v1[2][0] = 0;
  e2v1[2][1] = 1;
  e2v1[2][2] = -1;
  e2v1[2][3] = -1;
  e2v1[2][4] = 0;
  e2v1[2][5] = 4;
  e2v1[3][0] = 2;
  e2v1[3][1] = 3;
  e2v1[3][2] = -1;
  e2v1[3][3] = -1;
  e2v1[3][4] = 2;
  e2v1[3][5] = 6;
  e2v1[4][0] = 0;
  e2v1[4][1] = 1;
  e2v1[4][2] = 0;
  e2v1[4][3] = 2;
  e2v1[4][4] = -1;
  e2v1[4][5] = -1;
  e2v1[5][0] = 4;
  e2v1[5][1] = 5;
  e2v1[5][2] = 4;
  e2v1[5][3] = 6;
  e2v1[5][4] = -1;
  e2v1[5][5] = -1;

  //e1v2
  e1v2[0][0] = -1;
  e1v2[0][1] = -1;
  e1v2[0][2] = 7;
  e1v2[0][3] = 5;
  e1v2[0][4] = 7;
  e1v2[0][5] = 3;
  e1v2[1][0] = -1;
  e1v2[1][1] = -1;
  e1v2[1][2] = 6;
  e1v2[1][3] = 4;
  e1v2[1][4] = 6;
  e1v2[1][5] = 2;
  e1v2[2][0] = 7;
  e1v2[2][1] = 6;
  e1v2[2][2] = -1;
  e1v2[2][3] = -1;
  e1v2[2][4] = 7;
  e1v2[2][5] = 3;
  e1v2[3][0] = 5;
  e1v2[3][1] = 4;
  e1v2[3][2] = -1;
  e1v2[3][3] = -1;
  e1v2[3][4] = 5;
  e1v2[3][5] = 1;
  e1v2[4][0] = 7;
  e1v2[4][1] = 6;
  e1v2[4][2] = 7;
  e1v2[4][3] = 5;
  e1v2[4][4] = -1;
  e1v2[4][5] = -1;
  e1v2[5][0] = 3;
  e1v2[5][1] = 2;
  e1v2[5][2] = 3;
  e1v2[5][3] = 1;
  e1v2[5][4] = -1;
  e1v2[5][5] = -1;

  //e2v2
  e2v2[0][0] = -1;
  e2v2[0][1] = -1;
  e2v2[0][2] = 4;
  e2v2[0][3] = 6;
  e2v2[0][4] = 2;
  e2v2[0][5] = 6;
  e2v2[1][0] = -1;
  e2v2[1][1] = -1;
  e2v2[1][2] = 5;
  e2v2[1][3] = 7;
  e2v2[1][4] = 3;
  e2v2[1][5] = 7;
  e2v2[2][0] = 4;
  e2v2[2][1] = 5;
  e2v2[2][2] = -1;
  e2v2[2][3] = -1;
  e2v2[2][4] = 1;
  e2v2[2][5] = 5;
  e2v2[3][0] = 6;
  e2v2[3][1] = 7;
  e2v2[3][2] = -1;
  e2v2[3][3] = -1;
  e2v2[3][4] = 3;
  e2v2[3][5] = 7;
  e2v2[4][0] = 2;
  e2v2[4][1] = 3;
  e2v2[4][2] = 1;
  e2v2[4][3] = 3;
  e2v2[4][4] = -1;
  e2v2[4][5] = -1;
  e2v2[5][0] = 6;
  e2v2[5][1] = 7;
  e2v2[5][2] = 5;
  e2v2[5][3] = 7;
  e2v2[5][4] = -1;
  e2v2[5][5] = -1;

  // children[n][n1] returns the four elements among the eight children 
  // elements to be merged on face n of the parent element
  // the IDs for the eight children are 0,1,2,3,4,5,6,7
  children[0][0] = 1;
  children[0][1] = 3;
  children[0][2] = 5;
  children[0][3] = 7;
  children[1][0] = 0;
  children[1][1] = 2;
  children[1][2] = 4;
  children[1][3] = 6;
  children[2][0] = 2;
  children[2][1] = 3;
  children[2][2] = 6;
  children[2][3] = 7;
  children[3][0] = 0;
  children[3][1] = 1;
  children[3][2] = 4;
  children[3][3] = 5;
  children[4][0] = 4;
  children[4][1] = 5;
  children[4][2] = 6;
  children[4][3] = 7;
  children[5][0] = 0;
  children[5][1] = 1;
  children[5][2] = 2;
  children[5][3] = 3;

  // iijj[n][n1] returns the location of n's mortar on an element face
  // n1=0 refers to x direction location and n1=1 refers to y direction
  iijj[0][0] = 0;
  iijj[0][1] = 0;
  iijj[1][0] = 0;
  iijj[1][1] = 1;
  iijj[2][0] = 1;
  iijj[2][1] = 0;
  iijj[3][0] = 1;
  iijj[3][1] = 1;

  // v_end[n] returns the index of collocation points at two ends of each
  // direction
  v_end[0] = 0;
  v_end[1] = LX1-1;

  //face_l1,face_l2,face_ld return for start,end,stride for a loop over faces 
  // used on subroutine  mortar_vertex
  face_l1[0] = 1;
  face_l1[1] = 2;
  face_l1[2] = 0;

  face_l2[0] = 2;
  face_l2[1] = 0;
  face_l2[2] = 1;

  face_ld[0] = 1;
  face_ld[1] = -2;
  face_ld[2] = 1;
}
