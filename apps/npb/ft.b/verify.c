//-------------------------------------------------------------------------//
//                                                                         //
//  This benchmark is a serial C version of the NPB FT code. This C        //
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

#include "global.h"


// FT verification routine.
void verify(int n1, int n2, int n3, int nt, dcomplex cksum[nt+1],
            logical *verified)
{
  // Local variables.
  int kt;
  dcomplex cexpd[25+1];
  double epsilon, err;

  // Initialize tolerance level and success flag.

  epsilon = 1.0e-12;
  *verified = true;

  if ((n1 == 64) && (n2 == 64) && (n3 == 64) && (nt == 6)) {
    // Class S reference values.
    cexpd[1] = dcmplx(554.6087004964, 484.5363331978);
    cexpd[2] = dcmplx(554.6385409189, 486.5304269511);
    cexpd[3] = dcmplx(554.6148406171, 488.3910722336);
    cexpd[4] = dcmplx(554.5423607415, 490.1273169046);
    cexpd[5] = dcmplx(554.4255039624, 491.7475857993);
    cexpd[6] = dcmplx(554.2683411902, 493.2597244941);
  } else if ((n1 == 128) && (n2 == 128) && (n3 == 32) && (nt == 6)) {
    // Class W reference values.
    cexpd[1] = dcmplx(567.3612178944, 529.3246849175);
    cexpd[2] = dcmplx(563.1436885271, 528.2149986629);
    cexpd[3] = dcmplx(559.4024089970, 527.0996558037);
    cexpd[4] = dcmplx(556.0698047020, 526.0027904925);
    cexpd[5] = dcmplx(553.0898991250, 524.9400845633);
    cexpd[6] = dcmplx(550.4159734538, 523.9212247086);
  } else if ((n1 == 256) && (n2 == 256) && (n3 == 128) && (nt == 6)) {
    // Class A reference values.
    cexpd[1] = dcmplx(504.6735008193, 511.4047905510);
    cexpd[2] = dcmplx(505.9412319734, 509.8809666433);
    cexpd[3] = dcmplx(506.9376896287, 509.8144042213);
    cexpd[4] = dcmplx(507.7892868474, 510.1336130759);
    cexpd[5] = dcmplx(508.5233095391, 510.4914655194);
    cexpd[6] = dcmplx(509.1487099959, 510.7917842803);
  } else if ((n1 == 512) && (n2 == 256) && (n3 == 256) && (nt == 20)) {
    // Class B reference values.
    cexpd[1]  = dcmplx(517.7643571579, 507.7803458597);
    cexpd[2]  = dcmplx(515.4521291263, 508.8249431599);
    cexpd[3]  = dcmplx(514.6409228649, 509.6208912659);
    cexpd[4]  = dcmplx(514.2378756213, 510.1023387619);
    cexpd[5]  = dcmplx(513.9626667737, 510.3976610617);
    cexpd[6]  = dcmplx(513.7423460082, 510.5948019802);
    cexpd[7]  = dcmplx(513.5547056878, 510.7404165783);
    cexpd[8]  = dcmplx(513.3910925466, 510.8576573661);
    cexpd[9]  = dcmplx(513.2470705390, 510.9577278523);
    cexpd[10] = dcmplx(513.1197729984, 511.0460304483);
    cexpd[11] = dcmplx(513.0070319283, 511.1252433800);
    cexpd[12] = dcmplx(512.9070537032, 511.1968077718);
    cexpd[13] = dcmplx(512.8182883502, 511.2616233064);
    cexpd[14] = dcmplx(512.7393733383, 511.3203605551);
    cexpd[15] = dcmplx(512.6691062020, 511.3735928093);
    cexpd[16] = dcmplx(512.6064276004, 511.4218460548);
    cexpd[17] = dcmplx(512.5504076570, 511.4656139760);
    cexpd[18] = dcmplx(512.5002331720, 511.5053595966);
    cexpd[19] = dcmplx(512.4551951846, 511.5415130407);
    cexpd[20] = dcmplx(512.4146770029, 511.5744692211);

  } else if ((n1 == 512) && (n2 == 512) && (n3 == 512) && (nt == 20)) {
    // Class C reference values.
    cexpd[1]  = dcmplx(519.5078707457, 514.9019699238);
    cexpd[2]  = dcmplx(515.5422171134, 512.7578201997);
    cexpd[3]  = dcmplx(514.4678022222, 512.2251847514);
    cexpd[4]  = dcmplx(514.0150594328, 512.1090289018);
    cexpd[5]  = dcmplx(513.7550426810, 512.1143685824);
    cexpd[6]  = dcmplx(513.5811056728, 512.1496764568);
    cexpd[7]  = dcmplx(513.4569343165, 512.1870921893);
    cexpd[8]  = dcmplx(513.3651975661, 512.2193250322);
    cexpd[9]  = dcmplx(513.2955192805, 512.2454735794);
    cexpd[10] = dcmplx(513.2410471738, 512.2663649603);
    cexpd[11] = dcmplx(513.1971141679, 512.2830879827);
    cexpd[12] = dcmplx(513.1605205716, 512.2965869718);
    cexpd[13] = dcmplx(513.1290734194, 512.3075927445);
    cexpd[14] = dcmplx(513.1012720314, 512.3166486553);
    cexpd[15] = dcmplx(513.0760908195, 512.3241541685);
    cexpd[16] = dcmplx(513.0528295923, 512.3304037599);
    cexpd[17] = dcmplx(513.0310107773, 512.3356167976);
    cexpd[18] = dcmplx(513.0103090133, 512.3399592211);
    cexpd[19] = dcmplx(512.9905029333, 512.3435588985);
    cexpd[20] = dcmplx(512.9714421109, 512.3465164008);
  } else if ((n1 == 2048) && (n2 == 1024) && (n3 == 1024) && (nt == 25)) {
    // Class D reference values.
    cexpd[1]  = dcmplx(512.2230065252, 511.8534037109);
    cexpd[2]  = dcmplx(512.0463975765, 511.7061181082);
    cexpd[3]  = dcmplx(511.9865766760, 511.7096364601);
    cexpd[4]  = dcmplx(511.9518799488, 511.7373863950);
    cexpd[5]  = dcmplx(511.9269088223, 511.7680347632);
    cexpd[6]  = dcmplx(511.9082416858, 511.7967875532);
    cexpd[7]  = dcmplx(511.8943814638, 511.8225281841);
    cexpd[8]  = dcmplx(511.8842385057, 511.8451629348);
    cexpd[9]  = dcmplx(511.8769435632, 511.8649119387);
    cexpd[10] = dcmplx(511.8718203448, 511.8820803844);
    cexpd[11] = dcmplx(511.8683569061, 511.8969781011);
    cexpd[12] = dcmplx(511.8661708593, 511.9098918835);
    cexpd[13] = dcmplx(511.8649768950, 511.9210777066);
    cexpd[14] = dcmplx(511.8645605626, 511.9307604484);
    cexpd[15] = dcmplx(511.8647586618, 511.9391362671);
    cexpd[16] = dcmplx(511.8654451572, 511.9463757241);
    cexpd[17] = dcmplx(511.8665212451, 511.9526269238);
    cexpd[18] = dcmplx(511.8679083821, 511.9580184108);
    cexpd[19] = dcmplx(511.8695433664, 511.9626617538);
    cexpd[20] = dcmplx(511.8713748264, 511.9666538138);
    cexpd[21] = dcmplx(511.8733606701, 511.9700787219);
    cexpd[22] = dcmplx(511.8754661974, 511.9730095953);
    cexpd[23] = dcmplx(511.8776626738, 511.9755100241);
    cexpd[24] = dcmplx(511.8799262314, 511.9776353561);
    cexpd[25] = dcmplx(511.8822370068, 511.9794338060);
  } else if ((n1 == 4096) && (n2 == 2048) && (n3 == 2048) && (nt == 25)) {
    // Class E reference values.
    cexpd[1]  = dcmplx(512.1601045346, 511.7395998266);
    cexpd[2]  = dcmplx(512.0905403678, 511.8614716182);
    cexpd[3]  = dcmplx(512.0623229306, 511.9074203747);
    cexpd[4]  = dcmplx(512.0438418997, 511.9345900733);
    cexpd[5]  = dcmplx(512.0311521872, 511.9551325550);
    cexpd[6]  = dcmplx(512.0226088809, 511.9720179919);
    cexpd[7]  = dcmplx(512.0169296534, 511.9861371665);
    cexpd[8]  = dcmplx(512.0131225172, 511.9979364402);
    cexpd[9]  = dcmplx(512.0104767108, 512.0077674092);
    cexpd[10] = dcmplx(512.0085127969, 512.0159443121);
    cexpd[11] = dcmplx(512.0069224127, 512.0227453670);
    cexpd[12] = dcmplx(512.0055158164, 512.0284096041);
    cexpd[13] = dcmplx(512.0041820159, 512.0331373793);
    cexpd[14] = dcmplx(512.0028605402, 512.0370938679);
    cexpd[15] = dcmplx(512.0015223011, 512.0404138831);
    cexpd[16] = dcmplx(512.0001570022, 512.0432068837);
    cexpd[17] = dcmplx(511.9987650555, 512.0455615860);
    cexpd[18] = dcmplx(511.9973525091, 512.0475499442);
    cexpd[19] = dcmplx(511.9959279472, 512.0492304629);
    cexpd[20] = dcmplx(511.9945006558, 512.0506508902);
    cexpd[21] = dcmplx(511.9930795911, 512.0518503782);
    cexpd[22] = dcmplx(511.9916728462, 512.0528612016);
    cexpd[23] = dcmplx(511.9902874185, 512.0537101195);
    cexpd[24] = dcmplx(511.9889291565, 512.0544194514);
    cexpd[25] = dcmplx(511.9876028049, 512.0550079284);
  } else {
    printf("  Verification test for FT not performed\n");
    *verified = false;
  }

  // Verification test for results.
  if (*verified) {
    for (kt = 1; kt <= nt; kt++) {
      err = dcmplx_abs(dcmplx_div(dcmplx_sub(cksum[kt], cexpd[kt]),
                                  cexpd[kt]));
      if (!(err <= epsilon)) {
        *verified = false;
        break;
      }
    }

    if (*verified) {
      printf(" Verification test for FT successful\n");
    } else {
      printf(" Verification test for FT failed\n");
    }
  }
}

