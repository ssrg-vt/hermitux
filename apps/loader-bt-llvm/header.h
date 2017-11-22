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
//---------------------------------------------------------------------
//
//  header.h
//
//---------------------------------------------------------------------
//---------------------------------------------------------------------
 
//---------------------------------------------------------------------
// The following include file is generated automatically by the
// "setparams" utility. It defines 
//      maxcells:      the square root of the maximum number of processors
//      problem_size:  12, 64, 102, 162 (for class T, A, B, C)
//      dt_default:    default time step for this problem size if no
//                     config file
//      niter_default: default number of iterations for this problem size
//---------------------------------------------------------------------

#include "npbparams.h"
#include "type.h"

#define AA            0
#define BB            1
#define CC            2
#define BLOCK_SIZE    5

/* common /global/ */
extern double elapsed_time;
extern int grid_points[3];
extern logical timeron;

/* common /constants/ */
extern double tx1, tx2, tx3, ty1, ty2, ty3, tz1, tz2, tz3, 
              dx1, dx2, dx3, dx4, dx5, dy1, dy2, dy3, dy4, 
              dy5, dz1, dz2, dz3, dz4, dz5, dssp, dt, 
              ce[5][13], dxmax, dymax, dzmax, xxcon1, xxcon2, 
              xxcon3, xxcon4, xxcon5, dx1tx1, dx2tx1, dx3tx1,
              dx4tx1, dx5tx1, yycon1, yycon2, yycon3, yycon4,
              yycon5, dy1ty1, dy2ty1, dy3ty1, dy4ty1, dy5ty1,
              zzcon1, zzcon2, zzcon3, zzcon4, zzcon5, dz1tz1, 
              dz2tz1, dz3tz1, dz4tz1, dz5tz1, dnxm1, dnym1, 
              dnzm1, c1c2, c1c5, c3c4, c1345, conz1, c1, c2, 
              c3, c4, c5, c4dssp, c5dssp, dtdssp, dttx1,
              dttx2, dtty1, dtty2, dttz1, dttz2, c2dttx1, 
              c2dtty1, c2dttz1, comz1, comz4, comz5, comz6, 
              c3c4tx3, c3c4ty3, c3c4tz3, c2iv, con43, con16;

#define IMAX      PROBLEM_SIZE
#define JMAX      PROBLEM_SIZE
#define KMAX      PROBLEM_SIZE
#define IMAXP     IMAX/2*2
#define JMAXP     JMAX/2*2


// to improve cache performance, grid dimensions padded by 1 
// for even number sizes only.
/* common /fields/ */
extern double us     [KMAX][JMAXP+1][IMAXP+1];
extern double vs     [KMAX][JMAXP+1][IMAXP+1];
extern double ws     [KMAX][JMAXP+1][IMAXP+1];
extern double qs     [KMAX][JMAXP+1][IMAXP+1];
extern double rho_i  [KMAX][JMAXP+1][IMAXP+1];
extern double square [KMAX][JMAXP+1][IMAXP+1];
extern double forcing[KMAX][JMAXP+1][IMAXP+1][5];
extern double u      [KMAX][JMAXP+1][IMAXP+1][5];
extern double rhs    [KMAX][JMAXP+1][IMAXP+1][5];

/* common /work_1d/ */
extern double cuf[PROBLEM_SIZE+1];
extern double q  [PROBLEM_SIZE+1];
extern double ue [PROBLEM_SIZE+1][5];
extern double buf[PROBLEM_SIZE+1][5];
      

//-----------------------------------------------------------------------
// Timer constants
//-----------------------------------------------------------------------
#define t_total     1
#define t_rhsx      2
#define t_rhsy      3
#define t_rhsz      4
#define t_rhs       5
#define t_xsolve    6
#define t_ysolve    7
#define t_zsolve    8
#define t_rdis1     9
#define t_rdis2     10
#define t_add       11
#define t_last      11


void initialize();
void lhsinit(double lhs[][3][5][5], int size);
void exact_solution(double xi, double eta, double zeta, double dtemp[5]);
void exact_rhs();
void set_constants();
void adi();
void compute_rhs();
void x_solve();
void y_solve();
void matvec_sub(double ablock[5][5], double avec[5], double bvec[5]);
void matmul_sub(double ablock[5][5], double bblock[5][5], double cblock[5][5]);
void binvcrhs(double lhs[5][5], double c[5][5], double r[5]);
void binvrhs(double lhs[5][5], double r[5]);
void z_solve();
void add();
void error_norm(double rms[5]);
void rhs_norm(double rms[5]);
void verify(int no_time_steps, char *class, logical *verified);

