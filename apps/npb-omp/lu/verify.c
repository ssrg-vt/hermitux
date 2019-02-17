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

#include <stdio.h>
#include <math.h>
#include "applu.incl"

//---------------------------------------------------------------------
// verification routine                         
//---------------------------------------------------------------------
void verify(double xcr[5], double xce[5], double xci, 
            char *Class, logical *verified)
{
  double xcrref[5], xceref[5], xciref;
  double xcrdif[5], xcedif[5], xcidif;
  double epsilon, dtref = 0.0;
  int m;

  //---------------------------------------------------------------------
  // tolerance level
  //---------------------------------------------------------------------
  epsilon = 1.0e-08;

  *Class = 'U';
  *verified = true;

  for (m = 0; m < 5; m++) {
    xcrref[m] = 1.0;
    xceref[m] = 1.0;
  }
  xciref = 1.0;

  if ((nx0 == 12) && (ny0 == 12) && (nz0 == 12) && (itmax == 50)) {

    *Class = 'S';
    dtref = 5.0e-1;
    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (12X12X12) grid,
    // after 50 time steps, with  DT = 5.0e-01
    //---------------------------------------------------------------------
    xcrref[0] = 1.6196343210976702e-02;
    xcrref[1] = 2.1976745164821318e-03;
    xcrref[2] = 1.5179927653399185e-03;
    xcrref[3] = 1.5029584435994323e-03;
    xcrref[4] = 3.4264073155896461e-02;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, 
    // for the (12X12X12) grid,
    // after 50 time steps, with  DT = 5.0e-01
    //---------------------------------------------------------------------
    xceref[0] = 6.4223319957960924e-04;
    xceref[1] = 8.4144342047347926e-05;
    xceref[2] = 5.8588269616485186e-05;
    xceref[3] = 5.8474222595157350e-05;
    xceref[4] = 1.3103347914111294e-03;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (12X12X12) grid,
    // after 50 time steps, with DT = 5.0e-01
    //---------------------------------------------------------------------
    xciref = 7.8418928865937083e+00;

  } else if ((nx0 == 33) && (ny0 == 33) && (nz0 == 33) && (itmax == 300)) {

    *Class = 'W';   //SPEC95fp size
    dtref = 1.5e-3;
    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (33x33x33) grid,
    // after 300 time steps, with  DT = 1.5e-3
    //---------------------------------------------------------------------
    xcrref[0] = 0.1236511638192e+02;
    xcrref[1] = 0.1317228477799e+01;
    xcrref[2] = 0.2550120713095e+01;
    xcrref[3] = 0.2326187750252e+01;
    xcrref[4] = 0.2826799444189e+02;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, 
    // for the (33X33X33) grid,
    //---------------------------------------------------------------------
    xceref[0] = 0.4867877144216e+00;
    xceref[1] = 0.5064652880982e-01;
    xceref[2] = 0.9281818101960e-01;
    xceref[3] = 0.8570126542733e-01;
    xceref[4] = 0.1084277417792e+01;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (33X33X33) grid,
    // after 300 time steps, with  DT = 1.5e-3
    //---------------------------------------------------------------------
    xciref    = 0.1161399311023e+02;

  } else if ((nx0 == 64) && (ny0 == 64) && (nz0 == 64) && (itmax == 250)) {

    *Class = 'A';
    dtref = 2.0e+0;
    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (64X64X64) grid,
    // after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xcrref[0] = 7.7902107606689367e+02;
    xcrref[1] = 6.3402765259692870e+01;
    xcrref[2] = 1.9499249727292479e+02;
    xcrref[3] = 1.7845301160418537e+02;
    xcrref[4] = 1.8384760349464247e+03;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, 
    // for the (64X64X64) grid,
    // after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xceref[0] = 2.9964085685471943e+01;
    xceref[1] = 2.8194576365003349e+00;
    xceref[2] = 7.3473412698774742e+00;
    xceref[3] = 6.7139225687777051e+00;
    xceref[4] = 7.0715315688392578e+01;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (64X64X64) grid,
    // after 250 time steps, with DT = 2.0e+00
    //---------------------------------------------------------------------
    xciref = 2.6030925604886277e+01;

  } else if ((nx0 == 102) && (ny0 == 102) && (nz0 == 102) && (itmax == 250)) {

    *Class = 'B';
    dtref = 2.0e+0;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (102X102X102) grid,
    // after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xcrref[0] = 3.5532672969982736e+03;
    xcrref[1] = 2.6214750795310692e+02;
    xcrref[2] = 8.8333721850952190e+02;
    xcrref[3] = 7.7812774739425265e+02;
    xcrref[4] = 7.3087969592545314e+03;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, for the (102X102X102) 
    // grid, after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xceref[0] = 1.1401176380212709e+02;
    xceref[1] = 8.1098963655421574e+00;
    xceref[2] = 2.8480597317698308e+01;
    xceref[3] = 2.5905394567832939e+01;
    xceref[4] = 2.6054907504857413e+02;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (102X102X102) grid,
    // after 250 time steps, with DT = 2.0e+00
    //---------------------------------------------------------------------
    xciref = 4.7887162703308227e+01;

  } else if ((nx0 == 162) && (ny0 == 162) && (nz0 == 162) && (itmax == 250)) {

    *Class = 'C';
    dtref = 2.0e+0;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (162X162X162) grid,
    // after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xcrref[0] = 1.03766980323537846e+04;
    xcrref[1] = 8.92212458801008552e+02;
    xcrref[2] = 2.56238814582660871e+03;
    xcrref[3] = 2.19194343857831427e+03;
    xcrref[4] = 1.78078057261061185e+04;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, for the (162X162X162) 
    // grid, after 250 time steps, with  DT = 2.0e+00
    //---------------------------------------------------------------------
    xceref[0] = 2.15986399716949279e+02;
    xceref[1] = 1.55789559239863600e+01;
    xceref[2] = 5.41318863077207766e+01;
    xceref[3] = 4.82262643154045421e+01;
    xceref[4] = 4.55902910043250358e+02;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (162X162X162) grid,
    // after 250 time steps, with DT = 2.0e+00
    //---------------------------------------------------------------------
    xciref = 6.66404553572181300e+01;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (162X162X162) grid,
    // after 250 time steps, with DT = 2.0e+00
    //---------------------------------------------------------------------
    xciref = 6.66404553572181300e+01;

  } else if ((nx0 == 408) && (ny0 == 408) && (nz0 == 408) && (itmax == 300)) {

    *Class = 'D';
    dtref = 1.0e+0;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, for the (408X408X408) grid,
    // after 300 time steps, with  DT = 1.0e+00
    //---------------------------------------------------------------------
    xcrref[0] = 0.4868417937025e+05;
    xcrref[1] = 0.4696371050071e+04;
    xcrref[2] = 0.1218114549776e+05;
    xcrref[3] = 0.1033801493461e+05;
    xcrref[4] = 0.7142398413817e+05;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, for the (408X408X408) 
    // grid, after 300 time steps, with  DT = 1.0e+00
    //---------------------------------------------------------------------
    xceref[0] = 0.3752393004482e+03;
    xceref[1] = 0.3084128893659e+02;
    xceref[2] = 0.9434276905469e+02;
    xceref[3] = 0.8230686681928e+02;
    xceref[4] = 0.7002620636210e+03;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (408X408X408) grid,
    // after 300 time steps, with DT = 1.0e+00
    //---------------------------------------------------------------------
    xciref =    0.8334101392503e+02;

  } else if ((nx0 == 1020) && (ny0 == 1020) && (nz0 == 1020) && 
             (itmax == 300)) {

    *Class = 'E';
    dtref = 0.5e+0;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of residual, 
    // for the (1020X1020X1020) grid,
    // after 300 time steps, with  DT = 0.5e+00
    //---------------------------------------------------------------------
    xcrref[0] = 0.2099641687874e+06;
    xcrref[1] = 0.2130403143165e+05;
    xcrref[2] = 0.5319228789371e+05;
    xcrref[3] = 0.4509761639833e+05;
    xcrref[4] = 0.2932360006590e+06;

    //---------------------------------------------------------------------
    // Reference values of RMS-norms of solution error, 
    // for the (1020X1020X1020) 
    // grid, after 300 time steps, with  DT = 0.5e+00
    //---------------------------------------------------------------------
    xceref[0] = 0.4800572578333e+03;
    xceref[1] = 0.4221993400184e+02;
    xceref[2] = 0.1210851906824e+03;
    xceref[3] = 0.1047888986770e+03;
    xceref[4] = 0.8363028257389e+03;

    //---------------------------------------------------------------------
    // Reference value of surface integral, for the (1020X1020X1020) grid,
    // after 300 time steps, with DT = 0.5e+00
    //---------------------------------------------------------------------
    xciref =    0.9512163272273e+02;

  } else {
    *verified = false;
  }

  //---------------------------------------------------------------------
  // verification test for residuals if gridsize is one of 
  // the defined grid sizes above (*Class != 'U')
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // Compute the difference of solution values and the known reference values.
  //---------------------------------------------------------------------
  for (m = 0; m < 5; m++) {
    xcrdif[m] = fabs((xcr[m]-xcrref[m])/xcrref[m]);
    xcedif[m] = fabs((xce[m]-xceref[m])/xceref[m]);
  }
  xcidif = fabs((xci - xciref)/xciref);


  //---------------------------------------------------------------------
  // Output the comparison of computed results to known cases.
  //---------------------------------------------------------------------
  if (*Class != 'U') {
    printf("\n Verification being performed for class %c\n", *Class);
    printf(" Accuracy setting for epsilon = %20.13E\n", epsilon);
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
      printf("          %2d  %20.13E\n", m+1, xcr[m]);
    } else if (xcrdif[m] <= epsilon) {
      printf("          %2d  %20.13E%20.13E%20.13E\n", 
          m+1 ,xcr[m], xcrref[m], xcrdif[m]);
    } else { 
      *verified = false;
      printf(" FAILURE: %2d  %20.13E%20.13E%20.13E\n",
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
      printf("          %2d  %20.13E\n", m+1, xce[m]);
    } else if (xcedif[m] <= epsilon) {
      printf("          %2d  %20.13E%20.13E%20.13E\n", 
          m+1, xce[m], xceref[m], xcedif[m]);
    } else {
      *verified = false;
      printf(" FAILURE: %2d  %20.13E%20.13E%20.13E\n",
          m+1, xce[m], xceref[m], xcedif[m]);
    }
  }

  if (*Class != 'U') {
    printf(" Comparison of surface integral\n");
  } else {
    printf(" Surface integral\n");
  }

  if (*Class == 'U') {
    printf("              %20.13E\n", xci);
  } else if (xcidif <= epsilon) {
    printf("              %20.13E%20.13E%20.13E\n", xci, xciref, xcidif);
  } else {
    *verified = false;
    printf(" FAILURE:     %20.13E%20.13E%20.13E\n", xci, xciref, xcidif);
  }

  if (*Class == 'U') {
    printf(" No reference values provided\n");
    printf("No verification performed\n");
  } else if (*verified) {
    printf(" Verification Successful\n");
  } else {
    printf(" Verification failed\n");
  }
}

