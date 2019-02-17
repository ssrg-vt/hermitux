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

#include <stdio.h>
#include <math.h>

#include "header.h"

//---------------------------------------------------------------------
// verification routine                         
//---------------------------------------------------------------------
void verify(int no_time_steps, char *Class, logical *verified)
{
  double xcrref[5], xceref[5], xcrdif[5], xcedif[5];
  double epsilon, xce[5], xcr[5], dtref = 0.0;
  int m;

  //---------------------------------------------------------------------
  // tolerance level
  //---------------------------------------------------------------------
  epsilon = 1.0e-08;


  //---------------------------------------------------------------------
  // compute the error norm and the residual norm, and exit if not printing
  //---------------------------------------------------------------------
  error_norm(xce);
  compute_rhs();

  rhs_norm(xcr);

  for (m = 0; m < 5; m++) {
    xcr[m] = xcr[m] / dt;
  }

  *Class = 'U';
  *verified = true;

  for (m = 0; m < 5; m++) {
    xcrref[m] = 1.0;
    xceref[m] = 1.0;
  }

  //---------------------------------------------------------------------
  // reference data for 12X12X12 grids after 100 time steps, 
  // with DT = 1.50e-02
  //---------------------------------------------------------------------
  if ( (grid_points[0] == 12) && (grid_points[1] == 12) &&
       (grid_points[2] == 12) && (no_time_steps == 100) ) {
    *Class = 'S';
    dtref = 1.5e-2;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 2.7470315451339479e-02;
    xcrref[1] = 1.0360746705285417e-02;
    xcrref[2] = 1.6235745065095532e-02;
    xcrref[3] = 1.5840557224455615e-02;
    xcrref[4] = 3.4849040609362460e-02;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 2.7289258557377227e-05;
    xceref[1] = 1.0364446640837285e-05;
    xceref[2] = 1.6154798287166471e-05;
    xceref[3] = 1.5750704994480102e-05;
    xceref[4] = 3.4177666183390531e-05;

    //---------------------------------------------------------------------
    // reference data for 36X36X36 grids after 400 time steps, 
    // with DT = 1.5e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 36) && (grid_points[1] == 36) &&
              (grid_points[2] == 36) && (no_time_steps == 400) ) {
    *Class = 'W';
    dtref = 1.5e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 0.1893253733584e-02;
    xcrref[1] = 0.1717075447775e-03;
    xcrref[2] = 0.2778153350936e-03;
    xcrref[3] = 0.2887475409984e-03;
    xcrref[4] = 0.3143611161242e-02;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 0.7542088599534e-04;
    xceref[1] = 0.6512852253086e-05;
    xceref[2] = 0.1049092285688e-04;
    xceref[3] = 0.1128838671535e-04;
    xceref[4] = 0.1212845639773e-03;

    //---------------------------------------------------------------------
    // reference data for 64X64X64 grids after 400 time steps, 
    // with DT = 1.5e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 64) && (grid_points[1] == 64) &&
              (grid_points[2] == 64) && (no_time_steps == 400) ) {
    *Class = 'A';
    dtref = 1.5e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 2.4799822399300195;
    xcrref[1] = 1.1276337964368832;
    xcrref[2] = 1.5028977888770491;
    xcrref[3] = 1.4217816211695179;
    xcrref[4] = 2.1292113035138280;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 1.0900140297820550e-04;
    xceref[1] = 3.7343951769282091e-05;
    xceref[2] = 5.0092785406541633e-05;
    xceref[3] = 4.7671093939528255e-05;
    xceref[4] = 1.3621613399213001e-04;

    //---------------------------------------------------------------------
    // reference data for 102X102X102 grids after 400 time steps,
    // with DT = 1.0e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 102) && (grid_points[1] == 102) &&
              (grid_points[2] == 102) && (no_time_steps == 400) ) {
    *Class = 'B';
    dtref = 1.0e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 0.6903293579998e+02;
    xcrref[1] = 0.3095134488084e+02;
    xcrref[2] = 0.4103336647017e+02;
    xcrref[3] = 0.3864769009604e+02;
    xcrref[4] = 0.5643482272596e+02;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 0.9810006190188e-02;
    xceref[1] = 0.1022827905670e-02;
    xceref[2] = 0.1720597911692e-02;
    xceref[3] = 0.1694479428231e-02;
    xceref[4] = 0.1847456263981e-01;

    //---------------------------------------------------------------------
    // reference data for 162X162X162 grids after 400 time steps,
    // with DT = 0.67e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 162) && (grid_points[1] == 162) &&
              (grid_points[2] == 162) && (no_time_steps == 400) ) {
    *Class = 'C';
    dtref = 0.67e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 0.5881691581829e+03;
    xcrref[1] = 0.2454417603569e+03;
    xcrref[2] = 0.3293829191851e+03;
    xcrref[3] = 0.3081924971891e+03;
    xcrref[4] = 0.4597223799176e+03;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 0.2598120500183e+00;
    xceref[1] = 0.2590888922315e-01;
    xceref[2] = 0.5132886416320e-01;
    xceref[3] = 0.4806073419454e-01;
    xceref[4] = 0.5483377491301e+00;

    //---------------------------------------------------------------------
    // reference data for 408X408X408 grids after 500 time steps,
    // with DT = 0.3e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 408) && (grid_points[1] == 408) &&
              (grid_points[2] == 408) && (no_time_steps == 500) ) {
    *Class = 'D';
    dtref = 0.30e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 0.1044696216887e+05;
    xcrref[1] = 0.3204427762578e+04;
    xcrref[2] = 0.4648680733032e+04;
    xcrref[3] = 0.4238923283697e+04;
    xcrref[4] = 0.7588412036136e+04;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 0.5089471423669e+01;
    xceref[1] = 0.5323514855894e+00;
    xceref[2] = 0.1187051008971e+01;
    xceref[3] = 0.1083734951938e+01;
    xceref[4] = 0.1164108338568e+02;

    //---------------------------------------------------------------------
    // reference data for 1020X1020X1020 grids after 500 time steps,
    // with DT = 0.1e-03
    //---------------------------------------------------------------------
  } else if ( (grid_points[0] == 1020) && (grid_points[1] == 1020) &&
              (grid_points[2] == 1020) && (no_time_steps == 500) ) {
    *Class = 'E';
    dtref = 0.10e-3;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual.
    //---------------------------------------------------------------------
    xcrref[0] = 0.6255387422609e+05;
    xcrref[1] = 0.1495317020012e+05;
    xcrref[2] = 0.2347595750586e+05;
    xcrref[3] = 0.2091099783534e+05;
    xcrref[4] = 0.4770412841218e+05;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error.
    //---------------------------------------------------------------------
    xceref[0] = 0.6742735164909e+02;
    xceref[1] = 0.5390656036938e+01;
    xceref[2] = 0.1680647196477e+02;
    xceref[3] = 0.1536963126457e+02;
    xceref[4] = 0.1575330146156e+03;
  } else {
    *verified = false;
  }

  //---------------------------------------------------------------------
  // verification test for residuals if gridsize is one of 
  // the defined grid sizes above (class .ne. 'U')
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // Compute the difference of solution values and the known reference values.
  //---------------------------------------------------------------------
  for (m = 0; m < 5; m++) {
    xcrdif[m] = fabs((xcr[m]-xcrref[m])/xcrref[m]);
    xcedif[m] = fabs((xce[m]-xceref[m])/xceref[m]);
  }

  //---------------------------------------------------------------------
  // Output the comparison of computed results to known cases.
  //---------------------------------------------------------------------
  if (*Class != 'U') {
    printf(" Verification being performed for class %c\n", *Class);
    printf(" accuracy setting for epsilon = %20.13E\n", epsilon);
    *verified = (fabs(dt-dtref) <= epsilon);
    if (!(*verified)) {  
      *Class = 'U';
      printf(" DT does not match the reference value of %15.8E\n", dtref);
    } 
  } else {
    printf(" Unknown class\n");
  }

  if (*Class != 'U') {
    printf(" Comparison of RMS-norms of residual\n");
  } else {
    printf(" RMS-norms of residual\n");
  }

  for (m = 0; m < 5; m++) {
    if (*Class == 'U') {
      printf("          %2d%20.13E\n", m+1, xcr[m]);
    } else if (xcrdif[m] <= epsilon) {
      printf("          %2d%20.13E%20.13E%20.13E\n",
          m+1, xcr[m], xcrref[m], xcrdif[m]);
    } else  {
      *verified = false;
      printf(" FAILURE: %2d%20.13E%20.13E%20.13E\n",
          m+1, xcr[m], xcrref[m], xcrdif[m]);
    }
  }

  if (*Class != 'U') {
    printf(" Comparison of RMS-norms of solution error\n");
  } else {
    printf(" RMS-norms of solution error\n");
  }
        
  for (m = 0; m < 5; m++) {
    if (*Class == 'U') {
      printf("          %2d%20.13E\n", m+1, xce[m]);
    } else if (xcedif[m] <= epsilon) {
      printf("          %2d%20.13E%20.13E%20.13E\n",
          m+1, xce[m], xceref[m], xcedif[m]);
    } else {
      *verified = false;
      printf(" FAILURE: %2d%20.13E%20.13E%20.13E\n",
          m+1, xce[m], xceref[m], xcedif[m]);
    }
  }
        
  if (*Class == 'U') {
    printf(" No reference values provided\n");
    printf(" No verification performed\n");
  } else if (*verified) {
    printf(" Verification Successful\n");
  } else {
    printf(" Verification failed\n");
  }
}

