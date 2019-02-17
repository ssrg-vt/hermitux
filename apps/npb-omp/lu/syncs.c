//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is an OpenMP C version of the NPB LU code. This OpenMP  //
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

#include "applu.incl"

//---------------------------------------------------------------------
// Thread synchronization for pipeline operation
//---------------------------------------------------------------------
void sync_left(int ldmx, int ldmy, int ldmz,
               double v[ldmz][ldmy/2*2+1][ldmx/2*2+1][5])
{
  int neigh;

  if (iam > 0 && iam <= mthreadnum) {
    neigh = iam - 1;
    while (isync[neigh] == 0) {
      #pragma omp flush(isync)
    }
    isync[neigh] = 0;
    #pragma omp flush(isync,v)
  }
}


//---------------------------------------------------------------------
// Thread synchronization for pipeline operation
//---------------------------------------------------------------------
void sync_right(int ldmx, int ldmy, int ldmz,
                double v[ldmz][ldmy/2*2+1][ldmx/2*2+1][5])
{
  if (iam < mthreadnum) {
    #pragma omp flush(isync,v)
    while (isync[iam] == 1) {
      #pragma omp flush(isync)
    }
    isync[iam] = 1;
    #pragma omp flush(isync)
  }
}
