//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is an OpenMP C version of the NPB MG code. This OpenMP  //
//  C version is developed by the Center for Manycore Programming at Seoul //
//  National University and derived from the OpenMP Fortran versions in    //
//  "NPB3.3-OMP" developed by NAS.                                         //
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
//  Send comments or suggestions for this OpenMP C version to              //
//  cmp@aces.snu.ac.kr                                                     //
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
// Parameter lm (declared and set in "npbparams.h") is the log-base2 of 
// the edge size max for the partition on a given node, so must be changed 
// either to save space (if running a small case) or made bigger for larger 
// cases, for example, 512^3. Thus lm=7 means that the largest dimension 
// of a partition that can be solved on a node is 2^7 = 128. lm is set 
// automatically in npbparams.h
// Parameters ndim1, ndim2, ndim3 are the local problem dimensions. 
//---------------------------------------------------------------------
#include "npbparams.h"

#include "type.h"

// actual dimension including ghost cells for communications
#define NM          (2+(1<<LM))

// size of rhs array
#define NV          (ONE*(2+(1<<NDIM1))*(2+(1<<NDIM2))*(2+(1<<NDIM3)))

// size of residual array
#define NR          (((NV+NM*NM+5*NM+7*LM+6)/7)*8)

// maximum number of levels
#define MAXLEVEL    (LT_DEFAULT+1)


//---------------------------------------------------------------------
/* common /mg3/ */
static int nx[MAXLEVEL+1];
static int ny[MAXLEVEL+1];
static int nz[MAXLEVEL+1];

/* common /ClassType/ */
static char Class;

/* common /my_debug/ */
static int debug_vec[8];

/* common /fap/ */
static int m1[MAXLEVEL+1];
static int m2[MAXLEVEL+1];
static int m3[MAXLEVEL+1];
static int ir[MAXLEVEL+1];
static int lt, lb;


//---------------------------------------------------------------------
//  Set at m=1024, can handle cases up to 1024^3 case
//---------------------------------------------------------------------
#define M   (NM+1)


/* common /timers/ */
static logical timeron;
#define T_init      0
#define T_bench     1
#define T_mg3P      2
#define T_psinv     3
#define T_resid     4
#define T_resid2    5
#define T_rprj3     6
#define T_interp    7
#define T_norm2     8
#define T_comm3     9
#define T_last      10
