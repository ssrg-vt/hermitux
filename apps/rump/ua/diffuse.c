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

#include "header.h"
#include "timers.h"

//---------------------------------------------------------------------
// advance the diffusion term using CG iterations
//---------------------------------------------------------------------
void diffusion(logical ifmortar)
{
  double rho_aux, rho1, rho2, beta, cona;
  int iter, ie, im, iside, i, j, k;

  if (timeron) timer_start(t_diffusion);
  // set up diagonal preconditioner
  if (ifmortar) {
    setuppc();
    setpcmo();
  }

  // arrays t and umor are accumlators of (am pm) in the CG algorithm
  // (see the specification)
  r_init((double *)t, ntot, 0.0);
  r_init((double *)umor, nmor, 0.0);

  // calculate initial am (see specification) in CG algorithm

  // trhs and rmor are combined to generate r0 in CG algorithm.
  // pdiff and pmorx are combined to generate q0 in the CG algorithm.
  // rho1 is  (qm,rm) in the CG algorithm.
  rho1 = 0.0;
  for (ie = 0; ie < nelt; ie++) {
    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          pdiff[ie][k][j][i] = dpcelm[ie][k][j][i]*trhs[ie][k][j][i];
          rho1               = rho1 + trhs[ie][k][j][i]*pdiff[ie][k][j][i]*
                                      tmult[ie][k][j][i];
        }
      }
    }
  }

  for (im = 0; im < nmor; im++) {
    pmorx[im] = dpcmor[im]*rmor[im];
    rho1      = rho1 + rmor[im]*pmorx[im];
  }

  //.................................................................
  // commence conjugate gradient iteration
  //.................................................................
  for (iter = 1; iter <= nmxh; iter++) {
    if (iter > 1) {
      rho_aux = 0.0;
      // pdiffp and ppmor are combined to generate q_m+1 in the specification
      // rho_aux is (q_m+1,r_m+1)
      for (ie = 0; ie < nelt; ie++) {
        for (k = 0; k < LX1; k++) {
          for (j = 0; j < LX1; j++) {
            for (i = 0; i < LX1; i++) {
              pdiffp[ie][k][j][i] = dpcelm[ie][k][j][i]*trhs[ie][k][j][i];
              rho_aux = rho_aux+trhs[ie][k][j][i]*pdiffp[ie][k][j][i]*
                                                  tmult[ie][k][j][i];
            }
          }
        }
      }

      for (im = 0; im < nmor; im++) {
        ppmor[im] = dpcmor[im]*rmor[im];
        rho_aux = rho_aux + rmor[im]*ppmor[im];
      }

      // compute bm (beta) in the specification
      rho2 = rho1;
      rho1 = rho_aux;
      beta = rho1/rho2;

      // update p_m+1 in the specification
      adds1m1((double *)pdiff, (double *)pdiffp, beta, ntot);
      adds1m1(pmorx, ppmor, beta, nmor);  
    }

    // compute matrix vector product: (theta pm) in the specification
    if (timeron) timer_start(t_transf);
    transf(pmorx, (double *)pdiff);
    if (timeron) timer_stop(t_transf);

    // compute pdiffp which is (A theta pm) in the specification
    for (ie = 0; ie < nelt; ie++) {
      laplacian(pdiffp[ie], pdiff[ie], size_e[ie]);
    }

    // compute ppmor which will be used to compute (thetaT A theta pm) 
    // in the specification
    if (timeron) timer_start(t_transfb);
    transfb(ppmor, (double *)pdiffp);
    if (timeron) timer_stop(t_transfb);

    // apply boundary condition
    for (ie = 0; ie < nelt; ie++) {
      for (iside = 0; iside < NSIDES; iside++) {
        if(cbc[ie][iside] == 0) {
          facev(pdiffp[ie], iside, 0.0);
        }
      }
    }

    // compute cona which is (pm,theta T A theta pm)
    cona = 0.0;
    for (ie = 0; ie < nelt; ie++) {
      for (k = 0; k < LX1; k++) {
        for (j = 0; j < LX1; j++) {
          for (i = 0; i < LX1; i++) {
            cona = cona + pdiff[ie][k][j][i]*
                   pdiffp[ie][k][j][i]*tmult[ie][k][j][i];
          }
        }
      }
    }

    for (im = 0; im < nmor; im++) {
      ppmor[im] = ppmor[im]*tmmor[im];
      cona = cona + pmorx[im]*ppmor[im];
    }

    // compute am
    cona = rho1/cona;

    // compute (am pm)
    adds2m1((double *)t, (double *)pdiff, cona, ntot);
    adds2m1(umor, pmorx, cona, nmor);

    // compute r_m+1
    adds2m1((double *)trhs, (double *)pdiffp, -cona, ntot);
    adds2m1(rmor, ppmor,  -cona, nmor);
  }

  if (timeron) timer_start(t_transf);
  transf(umor, (double *)t);
  if (timeron) timer_stop(t_transf);
  if (timeron) timer_stop(t_diffusion);
}


//------------------------------------------------------------------
// compute  r = visc*[A]x +[B]x on a given element.
//------------------------------------------------------------------
void laplacian(double r[LX1][LX1][LX1], double u[LX1][LX1][LX1], int sizei)
{
  double rdtime;
  int i, j, k, iz;

  double tm1[LX1][LX1][LX1], tm2[LX1][LX1][LX1];

  rdtime = 1.0/dtime;

  r_init((double *)tm1, NXYZ, 0.0);
  for (iz = 0; iz < LX1; iz++) {
    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          tm1[iz][j][i] = tm1[iz][j][i]+wdtdr[k][i]*u[iz][j][k];
        }
      }
    }                           
  }

  r_init((double *)tm2, NXYZ, 0.0);
  for (iz = 0; iz < LX1; iz++) {
    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          tm2[iz][j][i] = tm2[iz][j][i]+u[iz][k][i]*wdtdr[j][k];
        }
      }
    }
  }

  r_init((double *)r, NXYZ, 0.0);
  for (k = 0; k < LX1; k++) {
    for (iz = 0; iz < LX1; iz++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          r[iz][j][i] = r[iz][j][i]+u[k][j][i]*wdtdr[iz][k];
        }
      }
    }
  }

  // collocate with remaining weights and sum to complete factorization.
  for (k = 0; k < LX1; k++) {
    for (j = 0; j < LX1; j++) {
      for (i = 0; i < LX1; i++) {
        r[k][j][i] = VISC*(tm1[k][j][i]*g4m1_s[sizei][k][j][i]+
                           tm2[k][j][i]*g5m1_s[sizei][k][j][i]+
                           r[k][j][i]*g6m1_s[sizei][k][j][i])+
                     bm1_s[sizei][k][j][i]*rdtime*u[k][j][i];
      }
    }
  }
}
