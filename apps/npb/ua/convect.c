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

#include <math.h>
#include "header.h"
#include "timers.h"

//---------------------------------------------------------
// Advance the convection term using 4th order RK
// 1.ta1 is solution from last time step 
// 2.the heat source is considered part of d/dx
// 3.trhs is right hand side for the diffusion equation
// 4.tmor is solution on mortar points, which will be used
//   as the initial guess when advancing the diffusion term 
//---------------------------------------------------------
void convect(logical ifmortar)
{
  double alpha2, tempa[LX1][LX1][LX1], rdtime, pidivalpha; 
  double dtx1, dtx2, dtx3, src, rk1[LX1][LX1][LX1]; 
  double rk2[LX1][LX1][LX1], rk3[LX1][LX1][LX1], rk4[LX1][LX1][LX1];
  double temp[LX1][LX1][LX1], subtime[3], xx0[3], yy0[3], zz0[3];
  double dtime2, r2, sum, xloc[LX1], yloc[LX1], zloc[LX1];
  int k, iel, i, j, iside, isize, substep, ip;
  const double sixth = 1.0/6.0;

  if (timeron) timer_start(t_convect);
  pidivalpha = acos(-1.0)/alpha;
  alpha2     = alpha*alpha;
  dtime2     = dtime/2.0;
  rdtime     = 1.0/dtime;
  subtime[0] = time;
  subtime[1] = time+dtime2;
  subtime[2] = time+dtime;
  for (substep = 0; substep < 3; substep++) {
    xx0[substep] = X00+VELX*subtime[substep];
    yy0[substep] = Y00+VELY*subtime[substep];
    zz0[substep] = Z00+VELZ*subtime[substep];
  }

  for (iel = 0; iel < nelt; iel++) {
    isize = size_e[iel];
    /*
    xloc[i] is the location of i'th collocation in x direction in an element.
    yloc[i] is the location of j'th collocation in y direction in an element.
    zloc[i] is the location of k'th collocation in z direction in an element.
    */
    for (i = 0; i < LX1; i++) {
      xloc[i] = xfrac[i]*(xc[iel][1]-xc[iel][0])+xc[iel][0];
    }
    for (j = 0; j < LX1; j++) {
      yloc[j] = xfrac[j]*(yc[iel][3]-yc[iel][0])+yc[iel][0];
    }
    for (k = 0; k < LX1; k++) {
      zloc[k] = xfrac[k]*(zc[iel][4]-zc[iel][0])+zc[iel][0];
    }

    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          r2 = pow(xloc[i]-xx0[0],2.0)+pow(yloc[j]-yy0[0],2.0)+
               pow(zloc[k]-zz0[0],2.0);
          if (r2 <= alpha2) {
            src = cos(sqrt(r2)*pidivalpha)+1.0;
          } else {
            src = 0.0;
          }
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][i] * ta1[iel][k][j][ip];
          }
          dtx1 = -VELX*sum*xrm1_s[isize][k][j][i];
          sum  = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][j] * ta1[iel][k][ip][i];
          }
          dtx2 = -VELY*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][k] * ta1[iel][ip][j][i];
          }
          dtx3 = -VELZ*sum*xrm1_s[isize][k][j][i];

          rk1[k][j][i] = dtx1 + dtx2 + dtx3 + src;
          temp[k][j][i] = ta1[iel][k][j][i]+dtime2*rk1[k][j][i];
        }
      }
    }        

    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          r2 = pow(xloc[i]-xx0[1],2.0) + pow(yloc[j]-yy0[1],2.0) +
               pow(zloc[k]-zz0[1],2.0);
          if (r2 <= alpha2) {
            src = cos(sqrt(r2)*pidivalpha)+1.0;
          } else {
            src = 0.0;
          }
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][i] * temp[k][j][ip];
          }
          dtx1 = -VELX*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][j] * temp[k][ip][i];
          }
          dtx2 = -VELY*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][k] * temp[ip][j][i];
          }
          dtx3 = -VELZ*sum*xrm1_s[isize][k][j][i];

          rk2[k][j][i] = dtx1 + dtx2 + dtx3 + src;
          tempa[k][j][i] = ta1[iel][k][j][i]+dtime2*rk2[k][j][i];
        }
      }
    }        

    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          r2 = pow(xloc[i]-xx0[1],2.0) + pow(yloc[j]-yy0[1],2.0) +
               pow(zloc[k]-zz0[1],2.0);
          if (r2 <= alpha2) {
            src = cos(sqrt(r2)*pidivalpha)+1.0;
          } else {
            src = 0.0;
          }
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][i] * tempa[k][j][ip];
          }
          dtx1 = -VELX*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][j] * tempa[k][ip][i];
          }
          dtx2 = -VELY*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][k] * tempa[ip][j][i];
          }
          dtx3 = -VELZ*sum*xrm1_s[isize][k][j][i];

          rk3[k][j][i] = dtx1 + dtx2 + dtx3 + src;
          temp[k][j][i] = ta1[iel][k][j][i]+dtime*rk3[k][j][i];
        }
      }
    }        

    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          r2 = pow(xloc[i]-xx0[2],2.0) + pow(yloc[j]-yy0[2],2.0) +
               pow(zloc[k]-zz0[2],2.0);
          if (r2 <= alpha2) {
            src = cos(sqrt(r2)*pidivalpha)+1.0;
          } else {
            src = 0.0;
          }
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][i] * temp[k][j][ip];
          }
          dtx1 = -VELX*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][j] * temp[k][ip][i];
          }
          dtx2 = -VELY*sum*xrm1_s[isize][k][j][i];
          sum = 0.0;
          for (ip = 0; ip < LX1; ip++) {
            sum = sum + dxm1[ip][k] * temp[ip][j][i];
          }
          dtx3 = -VELZ*sum*xrm1_s[isize][k][j][i];

          rk4[k][j][i] = dtx1 + dtx2 + dtx3 + src;
          tempa[k][j][i] = sixth*(rk1[k][j][i]+2.0*
                           rk2[k][j][i]+2.0*rk3[k][j][i]+rk4[k][j][i]);
        }
      }
    }        

    // apply boundary condition
    for (iside = 0; iside < NSIDES; iside++) {
      if(cbc[iel][iside] == 0) {
        facev(tempa, iside, 0.0);
      }
    }

    for (k = 0; k < LX1; k++) {
      for (j = 0; j < LX1; j++) {
        for (i = 0; i < LX1; i++) {
          trhs[iel][k][j][i]=bm1_s[isize][k][j][i]*(ta1[iel][k][j][i]*rdtime+
                             tempa[k][j][i]);
          ta1[iel][k][j][i]=ta1[iel][k][j][i]+tempa[k][j][i]*dtime;
        }
      }
    }
  } 

  // get mortar for intial guess for CG
  if (timeron) timer_start(t_transfb_c);
  if (ifmortar) {
    transfb_c_2((double *)ta1);
  } else {
    transfb_c((double *)ta1);
  }
  if (timeron) timer_stop(t_transfb_c);

  for (i = 0; i < nmor; i++) {
    tmort[i] = tmort[i] / mormult[i];
  }
  if (timeron) timer_stop(t_convect);
}
