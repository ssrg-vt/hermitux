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

//---------------------------------------------------------------------
// subtracts bvec=bvec - ablock*avec
//---------------------------------------------------------------------
void matvec_sub(double ablock[5][5], double avec[5], double bvec[5])
{
  //---------------------------------------------------------------------
  // rhs[kc][jc][ic][i] = rhs[kc][jc][ic][i] 
  // $                  - lhs[ia][ablock][0][i]*
  //---------------------------------------------------------------------
  bvec[0] = bvec[0] - ablock[0][0]*avec[0]
                    - ablock[1][0]*avec[1]
                    - ablock[2][0]*avec[2]
                    - ablock[3][0]*avec[3]
                    - ablock[4][0]*avec[4];
  bvec[1] = bvec[1] - ablock[0][1]*avec[0]
                    - ablock[1][1]*avec[1]
                    - ablock[2][1]*avec[2]
                    - ablock[3][1]*avec[3]
                    - ablock[4][1]*avec[4];
  bvec[2] = bvec[2] - ablock[0][2]*avec[0]
                    - ablock[1][2]*avec[1]
                    - ablock[2][2]*avec[2]
                    - ablock[3][2]*avec[3]
                    - ablock[4][2]*avec[4];
  bvec[3] = bvec[3] - ablock[0][3]*avec[0]
                    - ablock[1][3]*avec[1]
                    - ablock[2][3]*avec[2]
                    - ablock[3][3]*avec[3]
                    - ablock[4][3]*avec[4];
  bvec[4] = bvec[4] - ablock[0][4]*avec[0]
                    - ablock[1][4]*avec[1]
                    - ablock[2][4]*avec[2]
                    - ablock[3][4]*avec[3]
                    - ablock[4][4]*avec[4];
}


//---------------------------------------------------------------------
// subtracts a(i,j,k) X b(i,j,k) from c(i,j,k)
//---------------------------------------------------------------------
void matmul_sub(double ablock[5][5], double bblock[5][5], double cblock[5][5])
{
  cblock[0][0] = cblock[0][0] - ablock[0][0]*bblock[0][0]
                              - ablock[1][0]*bblock[0][1]
                              - ablock[2][0]*bblock[0][2]
                              - ablock[3][0]*bblock[0][3]
                              - ablock[4][0]*bblock[0][4];
  cblock[0][1] = cblock[0][1] - ablock[0][1]*bblock[0][0]
                              - ablock[1][1]*bblock[0][1]
                              - ablock[2][1]*bblock[0][2]
                              - ablock[3][1]*bblock[0][3]
                              - ablock[4][1]*bblock[0][4];
  cblock[0][2] = cblock[0][2] - ablock[0][2]*bblock[0][0]
                              - ablock[1][2]*bblock[0][1]
                              - ablock[2][2]*bblock[0][2]
                              - ablock[3][2]*bblock[0][3]
                              - ablock[4][2]*bblock[0][4];
  cblock[0][3] = cblock[0][3] - ablock[0][3]*bblock[0][0]
                              - ablock[1][3]*bblock[0][1]
                              - ablock[2][3]*bblock[0][2]
                              - ablock[3][3]*bblock[0][3]
                              - ablock[4][3]*bblock[0][4];
  cblock[0][4] = cblock[0][4] - ablock[0][4]*bblock[0][0]
                              - ablock[1][4]*bblock[0][1]
                              - ablock[2][4]*bblock[0][2]
                              - ablock[3][4]*bblock[0][3]
                              - ablock[4][4]*bblock[0][4];
  cblock[1][0] = cblock[1][0] - ablock[0][0]*bblock[1][0]
                              - ablock[1][0]*bblock[1][1]
                              - ablock[2][0]*bblock[1][2]
                              - ablock[3][0]*bblock[1][3]
                              - ablock[4][0]*bblock[1][4];
  cblock[1][1] = cblock[1][1] - ablock[0][1]*bblock[1][0]
                              - ablock[1][1]*bblock[1][1]
                              - ablock[2][1]*bblock[1][2]
                              - ablock[3][1]*bblock[1][3]
                              - ablock[4][1]*bblock[1][4];
  cblock[1][2] = cblock[1][2] - ablock[0][2]*bblock[1][0]
                              - ablock[1][2]*bblock[1][1]
                              - ablock[2][2]*bblock[1][2]
                              - ablock[3][2]*bblock[1][3]
                              - ablock[4][2]*bblock[1][4];
  cblock[1][3] = cblock[1][3] - ablock[0][3]*bblock[1][0]
                              - ablock[1][3]*bblock[1][1]
                              - ablock[2][3]*bblock[1][2]
                              - ablock[3][3]*bblock[1][3]
                              - ablock[4][3]*bblock[1][4];
  cblock[1][4] = cblock[1][4] - ablock[0][4]*bblock[1][0]
                              - ablock[1][4]*bblock[1][1]
                              - ablock[2][4]*bblock[1][2]
                              - ablock[3][4]*bblock[1][3]
                              - ablock[4][4]*bblock[1][4];
  cblock[2][0] = cblock[2][0] - ablock[0][0]*bblock[2][0]
                              - ablock[1][0]*bblock[2][1]
                              - ablock[2][0]*bblock[2][2]
                              - ablock[3][0]*bblock[2][3]
                              - ablock[4][0]*bblock[2][4];
  cblock[2][1] = cblock[2][1] - ablock[0][1]*bblock[2][0]
                              - ablock[1][1]*bblock[2][1]
                              - ablock[2][1]*bblock[2][2]
                              - ablock[3][1]*bblock[2][3]
                              - ablock[4][1]*bblock[2][4];
  cblock[2][2] = cblock[2][2] - ablock[0][2]*bblock[2][0]
                              - ablock[1][2]*bblock[2][1]
                              - ablock[2][2]*bblock[2][2]
                              - ablock[3][2]*bblock[2][3]
                              - ablock[4][2]*bblock[2][4];
  cblock[2][3] = cblock[2][3] - ablock[0][3]*bblock[2][0]
                              - ablock[1][3]*bblock[2][1]
                              - ablock[2][3]*bblock[2][2]
                              - ablock[3][3]*bblock[2][3]
                              - ablock[4][3]*bblock[2][4];
  cblock[2][4] = cblock[2][4] - ablock[0][4]*bblock[2][0]
                              - ablock[1][4]*bblock[2][1]
                              - ablock[2][4]*bblock[2][2]
                              - ablock[3][4]*bblock[2][3]
                              - ablock[4][4]*bblock[2][4];
  cblock[3][0] = cblock[3][0] - ablock[0][0]*bblock[3][0]
                              - ablock[1][0]*bblock[3][1]
                              - ablock[2][0]*bblock[3][2]
                              - ablock[3][0]*bblock[3][3]
                              - ablock[4][0]*bblock[3][4];
  cblock[3][1] = cblock[3][1] - ablock[0][1]*bblock[3][0]
                              - ablock[1][1]*bblock[3][1]
                              - ablock[2][1]*bblock[3][2]
                              - ablock[3][1]*bblock[3][3]
                              - ablock[4][1]*bblock[3][4];
  cblock[3][2] = cblock[3][2] - ablock[0][2]*bblock[3][0]
                              - ablock[1][2]*bblock[3][1]
                              - ablock[2][2]*bblock[3][2]
                              - ablock[3][2]*bblock[3][3]
                              - ablock[4][2]*bblock[3][4];
  cblock[3][3] = cblock[3][3] - ablock[0][3]*bblock[3][0]
                              - ablock[1][3]*bblock[3][1]
                              - ablock[2][3]*bblock[3][2]
                              - ablock[3][3]*bblock[3][3]
                              - ablock[4][3]*bblock[3][4];
  cblock[3][4] = cblock[3][4] - ablock[0][4]*bblock[3][0]
                              - ablock[1][4]*bblock[3][1]
                              - ablock[2][4]*bblock[3][2]
                              - ablock[3][4]*bblock[3][3]
                              - ablock[4][4]*bblock[3][4];
  cblock[4][0] = cblock[4][0] - ablock[0][0]*bblock[4][0]
                              - ablock[1][0]*bblock[4][1]
                              - ablock[2][0]*bblock[4][2]
                              - ablock[3][0]*bblock[4][3]
                              - ablock[4][0]*bblock[4][4];
  cblock[4][1] = cblock[4][1] - ablock[0][1]*bblock[4][0]
                              - ablock[1][1]*bblock[4][1]
                              - ablock[2][1]*bblock[4][2]
                              - ablock[3][1]*bblock[4][3]
                              - ablock[4][1]*bblock[4][4];
  cblock[4][2] = cblock[4][2] - ablock[0][2]*bblock[4][0]
                              - ablock[1][2]*bblock[4][1]
                              - ablock[2][2]*bblock[4][2]
                              - ablock[3][2]*bblock[4][3]
                              - ablock[4][2]*bblock[4][4];
  cblock[4][3] = cblock[4][3] - ablock[0][3]*bblock[4][0]
                              - ablock[1][3]*bblock[4][1]
                              - ablock[2][3]*bblock[4][2]
                              - ablock[3][3]*bblock[4][3]
                              - ablock[4][3]*bblock[4][4];
  cblock[4][4] = cblock[4][4] - ablock[0][4]*bblock[4][0]
                              - ablock[1][4]*bblock[4][1]
                              - ablock[2][4]*bblock[4][2]
                              - ablock[3][4]*bblock[4][3]
                              - ablock[4][4]*bblock[4][4];
}


void binvcrhs(double lhs[5][5], double c[5][5], double r[5])
{
  double pivot, coeff;

  pivot = 1.00/lhs[0][0];
  lhs[1][0] = lhs[1][0]*pivot;
  lhs[2][0] = lhs[2][0]*pivot;
  lhs[3][0] = lhs[3][0]*pivot;
  lhs[4][0] = lhs[4][0]*pivot;
  c[0][0] = c[0][0]*pivot;
  c[1][0] = c[1][0]*pivot;
  c[2][0] = c[2][0]*pivot;
  c[3][0] = c[3][0]*pivot;
  c[4][0] = c[4][0]*pivot;
  r[0]   = r[0]  *pivot;

  coeff = lhs[0][1];
  lhs[1][1]= lhs[1][1] - coeff*lhs[1][0];
  lhs[2][1]= lhs[2][1] - coeff*lhs[2][0];
  lhs[3][1]= lhs[3][1] - coeff*lhs[3][0];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][0];
  c[0][1] = c[0][1] - coeff*c[0][0];
  c[1][1] = c[1][1] - coeff*c[1][0];
  c[2][1] = c[2][1] - coeff*c[2][0];
  c[3][1] = c[3][1] - coeff*c[3][0];
  c[4][1] = c[4][1] - coeff*c[4][0];
  r[1]   = r[1]   - coeff*r[0];

  coeff = lhs[0][2];
  lhs[1][2]= lhs[1][2] - coeff*lhs[1][0];
  lhs[2][2]= lhs[2][2] - coeff*lhs[2][0];
  lhs[3][2]= lhs[3][2] - coeff*lhs[3][0];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][0];
  c[0][2] = c[0][2] - coeff*c[0][0];
  c[1][2] = c[1][2] - coeff*c[1][0];
  c[2][2] = c[2][2] - coeff*c[2][0];
  c[3][2] = c[3][2] - coeff*c[3][0];
  c[4][2] = c[4][2] - coeff*c[4][0];
  r[2]   = r[2]   - coeff*r[0];

  coeff = lhs[0][3];
  lhs[1][3]= lhs[1][3] - coeff*lhs[1][0];
  lhs[2][3]= lhs[2][3] - coeff*lhs[2][0];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][0];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][0];
  c[0][3] = c[0][3] - coeff*c[0][0];
  c[1][3] = c[1][3] - coeff*c[1][0];
  c[2][3] = c[2][3] - coeff*c[2][0];
  c[3][3] = c[3][3] - coeff*c[3][0];
  c[4][3] = c[4][3] - coeff*c[4][0];
  r[3]   = r[3]   - coeff*r[0];

  coeff = lhs[0][4];
  lhs[1][4]= lhs[1][4] - coeff*lhs[1][0];
  lhs[2][4]= lhs[2][4] - coeff*lhs[2][0];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][0];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][0];
  c[0][4] = c[0][4] - coeff*c[0][0];
  c[1][4] = c[1][4] - coeff*c[1][0];
  c[2][4] = c[2][4] - coeff*c[2][0];
  c[3][4] = c[3][4] - coeff*c[3][0];
  c[4][4] = c[4][4] - coeff*c[4][0];
  r[4]   = r[4]   - coeff*r[0];


  pivot = 1.00/lhs[1][1];
  lhs[2][1] = lhs[2][1]*pivot;
  lhs[3][1] = lhs[3][1]*pivot;
  lhs[4][1] = lhs[4][1]*pivot;
  c[0][1] = c[0][1]*pivot;
  c[1][1] = c[1][1]*pivot;
  c[2][1] = c[2][1]*pivot;
  c[3][1] = c[3][1]*pivot;
  c[4][1] = c[4][1]*pivot;
  r[1]   = r[1]  *pivot;

  coeff = lhs[1][0];
  lhs[2][0]= lhs[2][0] - coeff*lhs[2][1];
  lhs[3][0]= lhs[3][0] - coeff*lhs[3][1];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][1];
  c[0][0] = c[0][0] - coeff*c[0][1];
  c[1][0] = c[1][0] - coeff*c[1][1];
  c[2][0] = c[2][0] - coeff*c[2][1];
  c[3][0] = c[3][0] - coeff*c[3][1];
  c[4][0] = c[4][0] - coeff*c[4][1];
  r[0]   = r[0]   - coeff*r[1];

  coeff = lhs[1][2];
  lhs[2][2]= lhs[2][2] - coeff*lhs[2][1];
  lhs[3][2]= lhs[3][2] - coeff*lhs[3][1];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][1];
  c[0][2] = c[0][2] - coeff*c[0][1];
  c[1][2] = c[1][2] - coeff*c[1][1];
  c[2][2] = c[2][2] - coeff*c[2][1];
  c[3][2] = c[3][2] - coeff*c[3][1];
  c[4][2] = c[4][2] - coeff*c[4][1];
  r[2]   = r[2]   - coeff*r[1];

  coeff = lhs[1][3];
  lhs[2][3]= lhs[2][3] - coeff*lhs[2][1];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][1];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][1];
  c[0][3] = c[0][3] - coeff*c[0][1];
  c[1][3] = c[1][3] - coeff*c[1][1];
  c[2][3] = c[2][3] - coeff*c[2][1];
  c[3][3] = c[3][3] - coeff*c[3][1];
  c[4][3] = c[4][3] - coeff*c[4][1];
  r[3]   = r[3]   - coeff*r[1];

  coeff = lhs[1][4];
  lhs[2][4]= lhs[2][4] - coeff*lhs[2][1];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][1];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][1];
  c[0][4] = c[0][4] - coeff*c[0][1];
  c[1][4] = c[1][4] - coeff*c[1][1];
  c[2][4] = c[2][4] - coeff*c[2][1];
  c[3][4] = c[3][4] - coeff*c[3][1];
  c[4][4] = c[4][4] - coeff*c[4][1];
  r[4]   = r[4]   - coeff*r[1];


  pivot = 1.00/lhs[2][2];
  lhs[3][2] = lhs[3][2]*pivot;
  lhs[4][2] = lhs[4][2]*pivot;
  c[0][2] = c[0][2]*pivot;
  c[1][2] = c[1][2]*pivot;
  c[2][2] = c[2][2]*pivot;
  c[3][2] = c[3][2]*pivot;
  c[4][2] = c[4][2]*pivot;
  r[2]   = r[2]  *pivot;

  coeff = lhs[2][0];
  lhs[3][0]= lhs[3][0] - coeff*lhs[3][2];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][2];
  c[0][0] = c[0][0] - coeff*c[0][2];
  c[1][0] = c[1][0] - coeff*c[1][2];
  c[2][0] = c[2][0] - coeff*c[2][2];
  c[3][0] = c[3][0] - coeff*c[3][2];
  c[4][0] = c[4][0] - coeff*c[4][2];
  r[0]   = r[0]   - coeff*r[2];

  coeff = lhs[2][1];
  lhs[3][1]= lhs[3][1] - coeff*lhs[3][2];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][2];
  c[0][1] = c[0][1] - coeff*c[0][2];
  c[1][1] = c[1][1] - coeff*c[1][2];
  c[2][1] = c[2][1] - coeff*c[2][2];
  c[3][1] = c[3][1] - coeff*c[3][2];
  c[4][1] = c[4][1] - coeff*c[4][2];
  r[1]   = r[1]   - coeff*r[2];

  coeff = lhs[2][3];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][2];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][2];
  c[0][3] = c[0][3] - coeff*c[0][2];
  c[1][3] = c[1][3] - coeff*c[1][2];
  c[2][3] = c[2][3] - coeff*c[2][2];
  c[3][3] = c[3][3] - coeff*c[3][2];
  c[4][3] = c[4][3] - coeff*c[4][2];
  r[3]   = r[3]   - coeff*r[2];

  coeff = lhs[2][4];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][2];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][2];
  c[0][4] = c[0][4] - coeff*c[0][2];
  c[1][4] = c[1][4] - coeff*c[1][2];
  c[2][4] = c[2][4] - coeff*c[2][2];
  c[3][4] = c[3][4] - coeff*c[3][2];
  c[4][4] = c[4][4] - coeff*c[4][2];
  r[4]   = r[4]   - coeff*r[2];


  pivot = 1.00/lhs[3][3];
  lhs[4][3] = lhs[4][3]*pivot;
  c[0][3] = c[0][3]*pivot;
  c[1][3] = c[1][3]*pivot;
  c[2][3] = c[2][3]*pivot;
  c[3][3] = c[3][3]*pivot;
  c[4][3] = c[4][3]*pivot;
  r[3]   = r[3]  *pivot;

  coeff = lhs[3][0];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][3];
  c[0][0] = c[0][0] - coeff*c[0][3];
  c[1][0] = c[1][0] - coeff*c[1][3];
  c[2][0] = c[2][0] - coeff*c[2][3];
  c[3][0] = c[3][0] - coeff*c[3][3];
  c[4][0] = c[4][0] - coeff*c[4][3];
  r[0]   = r[0]   - coeff*r[3];

  coeff = lhs[3][1];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][3];
  c[0][1] = c[0][1] - coeff*c[0][3];
  c[1][1] = c[1][1] - coeff*c[1][3];
  c[2][1] = c[2][1] - coeff*c[2][3];
  c[3][1] = c[3][1] - coeff*c[3][3];
  c[4][1] = c[4][1] - coeff*c[4][3];
  r[1]   = r[1]   - coeff*r[3];

  coeff = lhs[3][2];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][3];
  c[0][2] = c[0][2] - coeff*c[0][3];
  c[1][2] = c[1][2] - coeff*c[1][3];
  c[2][2] = c[2][2] - coeff*c[2][3];
  c[3][2] = c[3][2] - coeff*c[3][3];
  c[4][2] = c[4][2] - coeff*c[4][3];
  r[2]   = r[2]   - coeff*r[3];

  coeff = lhs[3][4];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][3];
  c[0][4] = c[0][4] - coeff*c[0][3];
  c[1][4] = c[1][4] - coeff*c[1][3];
  c[2][4] = c[2][4] - coeff*c[2][3];
  c[3][4] = c[3][4] - coeff*c[3][3];
  c[4][4] = c[4][4] - coeff*c[4][3];
  r[4]   = r[4]   - coeff*r[3];


  pivot = 1.00/lhs[4][4];
  c[0][4] = c[0][4]*pivot;
  c[1][4] = c[1][4]*pivot;
  c[2][4] = c[2][4]*pivot;
  c[3][4] = c[3][4]*pivot;
  c[4][4] = c[4][4]*pivot;
  r[4]   = r[4]  *pivot;

  coeff = lhs[4][0];
  c[0][0] = c[0][0] - coeff*c[0][4];
  c[1][0] = c[1][0] - coeff*c[1][4];
  c[2][0] = c[2][0] - coeff*c[2][4];
  c[3][0] = c[3][0] - coeff*c[3][4];
  c[4][0] = c[4][0] - coeff*c[4][4];
  r[0]   = r[0]   - coeff*r[4];

  coeff = lhs[4][1];
  c[0][1] = c[0][1] - coeff*c[0][4];
  c[1][1] = c[1][1] - coeff*c[1][4];
  c[2][1] = c[2][1] - coeff*c[2][4];
  c[3][1] = c[3][1] - coeff*c[3][4];
  c[4][1] = c[4][1] - coeff*c[4][4];
  r[1]   = r[1]   - coeff*r[4];

  coeff = lhs[4][2];
  c[0][2] = c[0][2] - coeff*c[0][4];
  c[1][2] = c[1][2] - coeff*c[1][4];
  c[2][2] = c[2][2] - coeff*c[2][4];
  c[3][2] = c[3][2] - coeff*c[3][4];
  c[4][2] = c[4][2] - coeff*c[4][4];
  r[2]   = r[2]   - coeff*r[4];

  coeff = lhs[4][3];
  c[0][3] = c[0][3] - coeff*c[0][4];
  c[1][3] = c[1][3] - coeff*c[1][4];
  c[2][3] = c[2][3] - coeff*c[2][4];
  c[3][3] = c[3][3] - coeff*c[3][4];
  c[4][3] = c[4][3] - coeff*c[4][4];
  r[3]   = r[3]   - coeff*r[4];
}


void binvrhs(double lhs[5][5], double r[5])
{
  double pivot, coeff;

  pivot = 1.00/lhs[0][0];
  lhs[1][0] = lhs[1][0]*pivot;
  lhs[2][0] = lhs[2][0]*pivot;
  lhs[3][0] = lhs[3][0]*pivot;
  lhs[4][0] = lhs[4][0]*pivot;
  r[0]   = r[0]  *pivot;

  coeff = lhs[0][1];
  lhs[1][1]= lhs[1][1] - coeff*lhs[1][0];
  lhs[2][1]= lhs[2][1] - coeff*lhs[2][0];
  lhs[3][1]= lhs[3][1] - coeff*lhs[3][0];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][0];
  r[1]   = r[1]   - coeff*r[0];

  coeff = lhs[0][2];
  lhs[1][2]= lhs[1][2] - coeff*lhs[1][0];
  lhs[2][2]= lhs[2][2] - coeff*lhs[2][0];
  lhs[3][2]= lhs[3][2] - coeff*lhs[3][0];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][0];
  r[2]   = r[2]   - coeff*r[0];

  coeff = lhs[0][3];
  lhs[1][3]= lhs[1][3] - coeff*lhs[1][0];
  lhs[2][3]= lhs[2][3] - coeff*lhs[2][0];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][0];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][0];
  r[3]   = r[3]   - coeff*r[0];

  coeff = lhs[0][4];
  lhs[1][4]= lhs[1][4] - coeff*lhs[1][0];
  lhs[2][4]= lhs[2][4] - coeff*lhs[2][0];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][0];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][0];
  r[4]   = r[4]   - coeff*r[0];


  pivot = 1.00/lhs[1][1];
  lhs[2][1] = lhs[2][1]*pivot;
  lhs[3][1] = lhs[3][1]*pivot;
  lhs[4][1] = lhs[4][1]*pivot;
  r[1]   = r[1]  *pivot;

  coeff = lhs[1][0];
  lhs[2][0]= lhs[2][0] - coeff*lhs[2][1];
  lhs[3][0]= lhs[3][0] - coeff*lhs[3][1];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][1];
  r[0]   = r[0]   - coeff*r[1];

  coeff = lhs[1][2];
  lhs[2][2]= lhs[2][2] - coeff*lhs[2][1];
  lhs[3][2]= lhs[3][2] - coeff*lhs[3][1];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][1];
  r[2]   = r[2]   - coeff*r[1];

  coeff = lhs[1][3];
  lhs[2][3]= lhs[2][3] - coeff*lhs[2][1];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][1];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][1];
  r[3]   = r[3]   - coeff*r[1];

  coeff = lhs[1][4];
  lhs[2][4]= lhs[2][4] - coeff*lhs[2][1];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][1];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][1];
  r[4]   = r[4]   - coeff*r[1];


  pivot = 1.00/lhs[2][2];
  lhs[3][2] = lhs[3][2]*pivot;
  lhs[4][2] = lhs[4][2]*pivot;
  r[2]   = r[2]  *pivot;

  coeff = lhs[2][0];
  lhs[3][0]= lhs[3][0] - coeff*lhs[3][2];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][2];
  r[0]   = r[0]   - coeff*r[2];

  coeff = lhs[2][1];
  lhs[3][1]= lhs[3][1] - coeff*lhs[3][2];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][2];
  r[1]   = r[1]   - coeff*r[2];

  coeff = lhs[2][3];
  lhs[3][3]= lhs[3][3] - coeff*lhs[3][2];
  lhs[4][3]= lhs[4][3] - coeff*lhs[4][2];
  r[3]   = r[3]   - coeff*r[2];

  coeff = lhs[2][4];
  lhs[3][4]= lhs[3][4] - coeff*lhs[3][2];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][2];
  r[4]   = r[4]   - coeff*r[2];


  pivot = 1.00/lhs[3][3];
  lhs[4][3] = lhs[4][3]*pivot;
  r[3]   = r[3]  *pivot;

  coeff = lhs[3][0];
  lhs[4][0]= lhs[4][0] - coeff*lhs[4][3];
  r[0]   = r[0]   - coeff*r[3];

  coeff = lhs[3][1];
  lhs[4][1]= lhs[4][1] - coeff*lhs[4][3];
  r[1]   = r[1]   - coeff*r[3];

  coeff = lhs[3][2];
  lhs[4][2]= lhs[4][2] - coeff*lhs[4][3];
  r[2]   = r[2]   - coeff*r[3];

  coeff = lhs[3][4];
  lhs[4][4]= lhs[4][4] - coeff*lhs[4][3];
  r[4]   = r[4]   - coeff*r[3];


  pivot = 1.00/lhs[4][4];
  r[4]   = r[4]  *pivot;

  coeff = lhs[4][0];
  r[0]   = r[0]   - coeff*r[4];

  coeff = lhs[4][1];
  r[1]   = r[1]   - coeff*r[4];

  coeff = lhs[4][2];
  r[2]   = r[2]   - coeff*r[4];

  coeff = lhs[4][3];
  r[3]   = r[3]   - coeff*r[4];
}
