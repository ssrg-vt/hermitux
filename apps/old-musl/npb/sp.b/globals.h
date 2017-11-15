//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB CG code. This C        //
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

#include "npbparams.h"
#include "type.h"

//---------------------------------------------------------------------
//  Note: please observe that in the routine conj_grad three 
//  implementations of the sparse matrix-vector multiply have
//  been supplied.  The default matrix-vector multiply is not
//  loop unrolled.  The alternate implementations are unrolled
//  to a depth of 2 and unrolled to a depth of 8.  Please
//  experiment with these to find the fastest for your particular
//  architecture.  If reporting timing results, any of these three may
//  be used without penalty.
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//  Class specific parameters: 
//  It appears here for reference only.
//  These are their values, however, this info is imported in the npbparams.h
//  include file, which is written by the sys/setparams.c program.
//---------------------------------------------------------------------

//----------
//  Class S:
//----------
//#define NA        1400
//#define NONZER    7
//#define SHIFT     10
//#define NITER     15
//#define RCOND     1.0e-1

//----------
//  Class W:
//----------
//#define NA        7000
//#define NONZER    8
//#define SHIFT     12
//#define NITER     15
//#define RCOND     1.0e-1

//----------
//  Class A:
//----------
//#define NA        14000
//#define NONZER    11
//#define SHIFT     20
//#define NITER     15
//#define RCOND     1.0e-1

//----------
//  Class B:
//----------
//#define NA        75000
//#define NONZER    13
//#define SHIFT     60
//#define NITER     75
//#define RCOND     1.0e-1

//----------
//  Class C:
//----------
//#define NA        150000
//#define NONZER    15
//#define SHIFT     110
//#define NITER     75
//#define RCOND     1.0e-1

//----------
//  Class D:
//----------
//#define NA        1500000
//#define NONZER    21
//#define SHIFT     500
//#define NITER     100
//#define RCOND     1.0e-1

//----------
//  Class E:
//----------
//#define NA        9000000
//#define NONZER    26
//#define SHIFT     1500
//#define NITER     100
//#define RCOND     1.0e-1

#define NZ    (NA*(NONZER+1)*(NONZER+1))
#define NAZ   (NA*(NONZER+1))

#define T_init        0
#define T_bench       1
#define T_conj_grad   2
#define T_last        3

