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

#include <stdio.h>
#include <math.h>
#include "header.h"

void verify(char *Class, logical *verified)
{
  double norm, epsilon, norm_dif, norm_ref;

  // tolerance level
  epsilon = 1.0e-08;

  // compute the temperature integral over the whole domain
  norm = calc_norm();

  *verified = true;
  if ( *Class == 'S' ) {
    norm_ref = 0.1890013110962E-02;
  } else if ( *Class == 'W' ) {
    norm_ref = 0.2569794837076E-04;
  } else if ( *Class == 'A' ) {
    norm_ref = 0.8939996281443E-04;
  } else if ( *Class == 'B' ) {
    norm_ref = 0.4507561922901E-04;
  } else if ( *Class == 'C' ) {
    norm_ref = 0.1544736587100E-04;
  } else if ( *Class == 'D' ) {
    norm_ref = 0.1577586272355E-05;
  } else {
    *Class = 'U';
    norm_ref = 1.0;
    *verified = false;
  }         

  norm_dif = fabs((norm - norm_ref)/norm_ref);

  //---------------------------------------------------------------------
  // Output the comparison of computed results to known cases.
  //---------------------------------------------------------------------
  printf("\n");

  if (*Class != 'U') {
    printf(" Verification being performed for class %c\n", *Class);
    printf(" accuracy setting for epsilon = %20.13E\n", epsilon);
  } else { 
    printf(" Unknown class\n");
  }

  if (*Class != 'U') {
    printf(" Comparison of temperature integrals\n");
  } else {
    printf(" Temperature integral\n");
  }

  if (*Class == 'U') {
    printf("          %20.13E\n", norm);
  } else if (norm_dif <= epsilon) {
    printf("          %20.13E%20.13E%20.13E\n", norm, norm_ref, norm_dif);
  } else { 
    *verified = false;
    printf(" FAILURE: %20.13E%20.13E%20.13E\n", norm, norm_ref, norm_dif);
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
