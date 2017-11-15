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

//---------------------------------------------------------------------
// program SP
//---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "print_results.h"

/* common /global/ */
int grid_points[3], nx2, ny2, nz2;
logical timeron;

/* common /constants/ */
double tx1, tx2, tx3, ty1, ty2, ty3, tz1, tz2, tz3, 
       dx1, dx2, dx3, dx4, dx5, dy1, dy2, dy3, dy4, 
       dy5, dz1, dz2, dz3, dz4, dz5, dssp, dt, 
       ce[5][13], dxmax, dymax, dzmax, xxcon1, xxcon2, 
       xxcon3, xxcon4, xxcon5, dx1tx1, dx2tx1, dx3tx1,
       dx4tx1, dx5tx1, yycon1, yycon2, yycon3, yycon4,
       yycon5, dy1ty1, dy2ty1, dy3ty1, dy4ty1, dy5ty1,
       zzcon1, zzcon2, zzcon3, zzcon4, zzcon5, dz1tz1, 
       dz2tz1, dz3tz1, dz4tz1, dz5tz1, dnxm1, dnym1, 
       dnzm1, c1c2, c1c5, c3c4, c1345, conz1, c1, c2, 
       c3, c4, c5, c4dssp, c5dssp, dtdssp, dttx1, bt,
       dttx2, dtty1, dtty2, dttz1, dttz2, c2dttx1, 
       c2dtty1, c2dttz1, comz1, comz4, comz5, comz6, 
       c3c4tx3, c3c4ty3, c3c4tz3, c2iv, con43, con16;

/* common /fields/ */
double u      [KMAX][JMAXP+1][IMAXP+1][5];
double us     [KMAX][JMAXP+1][IMAXP+1];
double vs     [KMAX][JMAXP+1][IMAXP+1];
double ws     [KMAX][JMAXP+1][IMAXP+1];
double qs     [KMAX][JMAXP+1][IMAXP+1];
double rho_i  [KMAX][JMAXP+1][IMAXP+1];
double speed  [KMAX][JMAXP+1][IMAXP+1];
double square [KMAX][JMAXP+1][IMAXP+1];
double rhs    [KMAX][JMAXP+1][IMAXP+1][5];
double forcing[KMAX][JMAXP+1][IMAXP+1][5];

/* common /work_1d/ */
double cv  [PROBLEM_SIZE];
double rhon[PROBLEM_SIZE];
double rhos[PROBLEM_SIZE];
double rhoq[PROBLEM_SIZE];
double cuf [PROBLEM_SIZE];
double q   [PROBLEM_SIZE];
double ue [PROBLEM_SIZE][5];
double buf[PROBLEM_SIZE][5];

/* common /work_lhs/ */
double lhs [IMAXP+1][IMAXP+1][5];
double lhsp[IMAXP+1][IMAXP+1][5];
double lhsm[IMAXP+1][IMAXP+1][5];


int main(int argc, char *argv[])
{
  int i, niter, step, n3;
  double mflops, t, tmax, trecs[t_last+1];
  logical verified;
  char Class;
  char *t_names[t_last+1];

  //---------------------------------------------------------------------
  // Read input file (if it exists), else take
  // defaults from parameters
  //---------------------------------------------------------------------
  FILE *fp;
  if ((fp = fopen("timer.flag", "r")) != NULL) {
    timeron = true;
    t_names[t_total] = "total";
    t_names[t_rhsx] = "rhsx";
    t_names[t_rhsy] = "rhsy";
    t_names[t_rhsz] = "rhsz";
    t_names[t_rhs] = "rhs";
    t_names[t_xsolve] = "xsolve";
    t_names[t_ysolve] = "ysolve";
    t_names[t_zsolve] = "zsolve";
    t_names[t_rdis1] = "redist1";
    t_names[t_rdis2] = "redist2";
    t_names[t_tzetar] = "tzetar";
    t_names[t_ninvr] = "ninvr";
    t_names[t_pinvr] = "pinvr";
    t_names[t_txinvr] = "txinvr";
    t_names[t_add] = "add";
    fclose(fp);
  } else {
    timeron = false;
  }

  printf("\n\n NAS Parallel Benchmarks (NPB3.3-SER-C) - SP Benchmark\n\n");

  if ((fp = fopen("inputsp.data", "r")) != NULL) {
    int result;
    printf(" Reading from input file inputsp.data\n");
    result = fscanf(fp, "%d", &niter);
    while (fgetc(fp) != '\n');
    result = fscanf(fp, "%lf", &dt);
    while (fgetc(fp) != '\n');
    result = fscanf(fp, "%d%d%d", &grid_points[0], &grid_points[1], &grid_points[2]);
    fclose(fp);
  } else {
    printf(" No input file inputsp.data. Using compiled defaults\n");
    niter = NITER_DEFAULT;
    dt    = DT_DEFAULT;
    grid_points[0] = PROBLEM_SIZE;
    grid_points[1] = PROBLEM_SIZE;
    grid_points[2] = PROBLEM_SIZE;
  }

  printf(" Size: %4dx%4dx%4d\n", 
      grid_points[0], grid_points[1], grid_points[2]);
  printf(" Iterations: %4d    dt: %10.6f\n", niter, dt);
  printf("\n");

  if ((grid_points[0] > IMAX) ||
      (grid_points[1] > JMAX) ||
      (grid_points[2] > KMAX) ) {
    printf(" %d, %d, %d\n", grid_points[0], grid_points[1], grid_points[2]);
    printf(" Problem size too big for compiled array sizes\n");
    return 0;
  }
  nx2 = grid_points[0] - 2;
  ny2 = grid_points[1] - 2;
  nz2 = grid_points[2] - 2;

  set_constants();

  for (i = 1; i <= t_last; i++) {
    timer_clear(i);
  }

  exact_rhs();

  initialize();

  //---------------------------------------------------------------------
  // do one time step to touch all code, and reinitialize
  //---------------------------------------------------------------------
  adi();
  initialize();

  for (i = 1; i <= t_last; i++) {
    timer_clear(i);
  }
  timer_start(1);

  for (step = 1; step <= niter; step++) {
    if ((step % 20) == 0 || step == 1) {
      printf(" Time step %4d\n", step);
    }

    adi();
  }

  timer_stop(1);
  tmax = timer_read(1);

  verify(niter, &Class, &verified);

  if (tmax != 0.0) {
    n3 = grid_points[0]*grid_points[1]*grid_points[2];
    t = (grid_points[0]+grid_points[1]+grid_points[2])/3.0;
    mflops = (881.174 * (double)n3
             - 4683.91 * (t * t)
             + 11484.5 * t
             - 19272.4) * (double)niter / (tmax*1000000.0);
  } else {
    mflops = 0.0;
  }

  print_results("SP", Class, grid_points[0], 
                grid_points[1], grid_points[2], niter, 
                tmax, mflops, "          floating point", 
                verified, NPBVERSION,COMPILETIME, CS1, CS2, CS3, CS4, CS5, 
                CS6, "(none)");

  //---------------------------------------------------------------------
  // More timers
  //---------------------------------------------------------------------
  if (timeron) {
    for (i = 1; i <= t_last; i++) {
      trecs[i] = timer_read(i);
    }
    if (tmax == 0.0) tmax = 1.0;

    printf("  SECTION   Time (secs)\n");
    for (i = 1; i <= t_last; i++) {
      printf("  %-8s:%9.3f  (%6.2f%%)\n", 
          t_names[i], trecs[i], trecs[i]*100./tmax);
      if (i == t_rhs) {
        t = trecs[t_rhsx] + trecs[t_rhsy] + trecs[t_rhsz];
        printf("    --> %8s:%9.3f  (%6.2f%%)\n", "sub-rhs", t, t*100./tmax);
        t = trecs[t_rhs] - t;
        printf("    --> %8s:%9.3f  (%6.2f%%)\n", "rest-rhs", t, t*100./tmax);
      } else if (i == t_zsolve) {
        t = trecs[t_zsolve] - trecs[t_rdis1] - trecs[t_rdis2];
        printf("    --> %8s:%9.3f  (%6.2f%%)\n", "sub-zsol", t, t*100./tmax);
      } else if (i == t_rdis2) {
        t = trecs[t_rdis1] + trecs[t_rdis2];
        printf("    --> %8s:%9.3f  (%6.2f%%)\n", "redist", t, t*100./tmax);
      }
    }
  }

  return 0;
}

