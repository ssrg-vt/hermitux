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

// program ua

#include <stdio.h>
#include <math.h>

#include "header.h"
#include "timers.h"
#include "print_results.h"


/* common /usrdati/ */
int fre, niter, nmxh;

/* common /usrdatr/ */
double alpha, dlmin, dtime;

/* common /dimn/ */
int nelt, ntot, nmor, nvertex;

/* common /bench1/ */
double x0, _y0, z0, time;

// double arrays associated with collocation points
/* common /colldp/ */
double ta1   [LELT][LX1][LX1][LX1];
double ta2   [LELT][LX1][LX1][LX1];
double trhs  [LELT][LX1][LX1][LX1];
double t     [LELT][LX1][LX1][LX1];
double tmult [LELT][LX1][LX1][LX1];
double dpcelm[LELT][LX1][LX1][LX1];
double pdiff [LELT][LX1][LX1][LX1];
double pdiffp[LELT][LX1][LX1][LX1];

// double arays associated with mortar points
/* common /mortdp/ */
double umor   [LMOR];
double mormult[LMOR];
double tmort  [LMOR];
double tmmor  [LMOR];
double rmor   [LMOR];
double dpcmor [LMOR];
double pmorx  [LMOR];
double ppmor  [LMOR];

// integer arrays associated with element faces
/* common/facein/ */
int idmo    [LELT][NSIDES][LNJE][LNJE][LX1][LX1]; 
int idel    [LELT][NSIDES][LX1][LX1]; 
int sje     [LELT][NSIDES][2][2]; 
int sje_new [LELT][NSIDES][2][2];
int ijel    [LELT][NSIDES][2]; 
int ijel_new[LELT][NSIDES][2];
int cbc     [LELT][NSIDES]; 
int cbc_new [LELT][NSIDES];

// integer array associated with vertices
/* common /vin/ */
int vassign[LELT][8];
int emo    [8*LELT][8][2];
int nemo   [8*LELT];

// integer array associated with element edges
/* common /edgein/ */
int diagn[LELT][12][2];

// integer arrays associated with elements
/* common /eltin/ */
int tree        [LELT];
int treenew     [LELT];
int mt_to_id    [LELT];
int mt_to_id_old[LELT];
int id_to_mt    [LELT];
int newc        [LELT];
int newi        [LELT];
int newe        [LELT];
int ref_front_id[LELT];
int ich         [LELT];
int size_e      [LELT];
int front       [LELT];
int action      [LELT];

// logical arrays associated with vertices
/* common /vlg/ */
logical ifpcmor[8*LELT];

// logical arrays associated with edge
/* common /edgelg/ */
logical eassign  [LELT][12];
logical ncon_edge[LELT][12];
logical if_1_edge[LELT][12]; 

// logical arrays associated with elements
/* common /facelg/ */
logical skip    [LELT];
logical ifcoa   [LELT];
logical ifcoa_id[LELT];

// logical arrays associated with element faces
/* common /masonl/ */
logical fassign[LELT][NSIDES];
logical edgevis[LELT][NSIDES][4];

// small arrays
/* common /transr/ */
double qbnew[2][LX1][LX1-2];
double bqnew[2][LX1-2][LX1-2];

/* common /pcr/ */
double pcmor_nc1[REFINE_MAX][2][2][LX1][LX1];
double pcmor_nc2[REFINE_MAX][2][2][LX1][LX1];
double pcmor_nc0[REFINE_MAX][2][2][LX1][LX1];
double pcmor_c  [REFINE_MAX][LX1][LX1];
double tcpre    [LX1][LX1];
double pcmor_cor[REFINE_MAX][8];

// gauss-labotto and gauss points
/* common /gauss/ */
double zgm1[LX1];

// weights
/* common /wxyz/ */
double wxm1[LX1];
double w3m1[LX1][LX1][LX1];

// coordinate of element vertices
/* common /coord/ */
double xc[LELT][8];
double yc[LELT][8];
double zc[LELT][8];
double xc_new[LELT][8];
double yc_new[LELT][8];
double zc_new[LELT][8];

// dr/dx, dx/dr  and Jacobian
/* common /giso/ */
double jacm1_s[REFINE_MAX][LX1][LX1][LX1];
double rxm1_s[REFINE_MAX][LX1][LX1][LX1];
double xrm1_s[REFINE_MAX][LX1][LX1][LX1];

// mass matrices (diagonal)
/* common /mass/ */
double bm1_s[REFINE_MAX][LX1][LX1][LX1];

// dertivative matrices d/dr
/* common /dxyz/ */
double dxm1[LX1][LX1];
double dxtm1[LX1][LX1];
double wdtdr[LX1][LX1];

// interpolation operators
/* common /ixyz/ */
double ixm31 [LX1*2-1][LX1];
double ixtm31[LX1][LX1*2-1];
double ixmc1 [LX1][LX1];
double ixtmc1[LX1][LX1];
double ixmc2 [LX1][LX1];
double ixtmc2[LX1][LX1];
double map2  [LX1];
double map4  [LX1];

// collocation location within an element
/* common /xfracs/ */
double xfrac[LX1];

// used in laplacian operator
/* common /gmfact/ */
double g1m1_s[REFINE_MAX][LX1][LX1][LX1]; 
double g4m1_s[REFINE_MAX][LX1][LX1][LX1];
double g5m1_s[REFINE_MAX][LX1][LX1][LX1];
double g6m1_s[REFINE_MAX][LX1][LX1][LX1];
      
// We store some tables of useful topological constants
// These constants are intialized in a block data 'top_constants'
/* common /top_consts/ */
int f_e_ef[6][4];
int e_c[8][3];
int local_corner[6][8];
int cal_nnb[8][3];
int oplc[4];
int cal_iijj[4][2];
int cal_intempx[6][4];
int c_f[6][4];
int le_arr[3][2][4];
int jjface[6];
int e_face2[6][4];
int op[4];
int localedgenumber[12][6];
int edgenumber[6][4];
int f_c[8][3];
int e1v1[6][6];
int e2v1[6][6];
int e1v2[6][6];
int e2v2[6][6];
int children[6][4];
int iijj[4][2];
int v_end[2];
int face_l1[3];
int face_l2[3];
int face_ld[3];

// Timer parameters
/* common /timing/ */
logical timeron;


int main(int argc, char *argv[])
{
  int step, ie, iside, i, j, k;
  double mflops, tmax, nelt_tot = 0.0;
  char Class;
  logical ifmortar = false, verified;

  double t2, trecs[t_last+1];
  char *t_names[t_last+1];

  //---------------------------------------------------------------------
  // Read input file (if it exists), else take
  // defaults from parameters
  //---------------------------------------------------------------------
  FILE *fp;
  if ((fp = fopen("timer.flag", "r")) != NULL) {
    timeron = true;
    t_names[t_total] = "total";
    t_names[t_init] = "init";
    t_names[t_convect] = "convect";
    t_names[t_transfb_c] = "transfb_c";
    t_names[t_diffusion] = "diffusion";
    t_names[t_transf] = "transf";
    t_names[t_transfb] = "transfb";
    t_names[t_adaptation] = "adaptation";
    t_names[t_transf2] = "transf+b";
    t_names[t_add2] = "add2";
    fclose(fp);
  } else {
    timeron = false;
  }

  printf("\n\n NAS Parallel Benchmarks (NPB3.3-SER-C) - UA Benchmark\n\n");

  if ((fp = fopen("inputua.data", "r")) != NULL) {
    int result;
    printf(" Reading from input file inputua.data\n");
    result = fscanf(fp, "%d", &fre);
    while (fgetc(fp) != '\n');
    result = fscanf(fp, "%d", &niter);
    while (fgetc(fp) != '\n');
    result = fscanf(fp, "%d", &nmxh);
    while (fgetc(fp) != '\n');
    result = fscanf(fp, "%lf", &alpha);
    Class = 'U';
    fclose(fp);
  } else {
    printf(" No input file inputua.data. Using compiled defaults\n");
    fre   = FRE_DEFAULT;
    niter = NITER_DEFAULT;
    nmxh  = NMXH_DEFAULT;
    alpha = ALPHA_DEFAULT;
    Class = CLASS_DEFAULT;
  }

  dlmin = pow(0.5, REFINE_MAX);
  dtime = 0.04*dlmin;

  printf(" Levels of refinement: %8d\n", REFINE_MAX);
  printf(" Adaptation frequency: %8d\n", fre);
  printf(" Time steps:           %8d    dt: %15.6E\n", niter, dtime);
  printf(" CG iterations:        %8d\n", nmxh);
  printf(" Heat source radius:   %8.4f\n\n", alpha);

  top_constants();

  for (i = 1; i <= t_last; i++) {
    timer_clear(i);
  }
  if (timeron) timer_start(t_init);

  // set up initial mesh (single element) and solution (all zero)
  create_initial_grid();

  r_init((double *)ta1, ntot, 0.0);
  nr_init((int *)sje, 4*6*nelt, -1);

  // compute tables of coefficients and weights      
  coef();
  geom1();

  // compute the discrete laplacian operators
  setdef();

  // prepare for the preconditioner
  setpcmo_pre();

  // refine initial mesh and do some preliminary work
  time = 0.0;
  mortar();
  prepwork();
  adaptation(&ifmortar, 0);
  if (timeron) timer_stop(t_init);

  timer_clear(1);

  time = 0.0;
  for (step = 0; step <= niter; step++) {
    if (step == 1) {
      // reset the solution and start the timer, keep track of total no elms
      r_init((double *)ta1, ntot, 0.0);
      time = 0.0;
      nelt_tot = 0.0;
      for (i = 1; i <= t_last; i++) {
        if (i != t_init) timer_clear(i);
      }
      timer_start(1);
    }

    // advance the convection step 
    convect(ifmortar);

    if (timeron) timer_start(t_transf2);
    // prepare the intital guess for cg
    transf(tmort, (double *)ta1);

    // compute residual for diffusion term based on intital guess

    // compute the left hand side of equation, lapacian t
    for (ie = 0; ie < nelt; ie++) {
      laplacian(ta2[ie], ta1[ie], size_e[ie]);
    }

    // compute the residual 
    for (ie = 0; ie < nelt; ie++) {
      for (k = 0; k < LX1; k++) {
        for (j = 0; j < LX1; j++) {
          for (i = 0; i < LX1; i++) {
            trhs[ie][k][j][i] = trhs[ie][k][j][i] - ta2[ie][k][j][i];
          }
        }
      }
    }

    // get the residual on mortar 
    transfb(rmor, (double *)trhs);
    if (timeron) timer_stop(t_transf2);

    // apply boundary condition: zero out the residual on domain boundaries

    // apply boundary conidtion to trhs
    for (ie = 0; ie < nelt; ie++) {
      for (iside = 0; iside < NSIDES; iside++) {
        if (cbc[ie][iside] == 0) {
          facev(trhs[ie], iside, 0.0);
        }
      }
    }
    // apply boundary condition to rmor
    col2(rmor, tmmor, nmor);

    // call the conjugate gradient iterative solver
    diffusion(ifmortar);

    // add convection and diffusion
    if (timeron) timer_start(t_add2);
    add2((double *)ta1, (double *)t, ntot);
    if (timeron) timer_stop(t_add2);

    // perform mesh adaptation
    time = time + dtime;
    if ((step != 0) && (step/fre*fre == step)) {
      if (step != niter) {
        adaptation(&ifmortar, step);
      }
    } else {
      ifmortar = false;
    }
    nelt_tot = nelt_tot + (double)(nelt);
  }

  timer_stop(1);
  tmax = timer_read(1);

  verify(&Class, &verified);

  // compute millions of collocation points advanced per second.
  // diffusion: nmxh advancements, convection: 1 advancement
  mflops = nelt_tot*(double)(LX1*LX1*LX1*(nmxh+1))/(tmax*1.e6);

  print_results("UA", Class, REFINE_MAX, 0, 0, niter, 
                tmax, mflops, "    coll. point advanced", 
                verified, NPBVERSION, COMPILETIME, CS1, CS2, CS3, CS4, CS5, 
                CS6, "(none)");

  //---------------------------------------------------------------------
  // More timers
  //---------------------------------------------------------------------
  if (timeron) {
    for (i = 1; i <= t_last; i++) {
      trecs[i] = timer_read(i);
    }
    if (tmax == 0.0) tmax = 1.0;

    printf("  SECTION     Time (secs)\n");
    for (i = 1; i <= t_last; i++) {
      printf("  %-10s:%9.3f  (%6.2f%%)\n",
          t_names[i], trecs[i], trecs[i]*100./tmax);
      if (i == t_transfb_c) {
        t2 = trecs[t_convect] - trecs[t_transfb_c];
        printf("    --> %11s:%9.3f  (%6.2f%%)\n", 
            "sub-convect", t2, t2*100./tmax);
      } else if (i == t_transfb) {
        t2 = trecs[t_diffusion] - trecs[t_transf] - trecs[t_transfb];
        printf("    --> %11s:%9.3f  (%6.2f%%)\n", 
            "sub-diffuse", t2, t2*100./tmax);
      }
    }
  }

  return 0;
}
