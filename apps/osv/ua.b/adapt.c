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
#include "header.h"
#include "timers.h"

static void do_coarsen(logical *if_coarsen, int *icoarsen, int neltold);
static void do_refine(logical *ifmortar, int *irefine);
static logical ifcor(int n1, int n2, int i, int iface);
static logical icheck(int ie, int n);
static void find_coarsen(logical *if_coarsen, int neltold);
static void find_refine(logical *if_refine);
static void check_refine(logical *ifrepeat);
static logical iftouch(int iel);
static void remap(double y[LX1][LX1][LX1], double y1[7][LX1][LX1][LX1],
                  double x[LX1][LX1][LX1]);
static void merging(int iela[8]);
static void remap2(int iela[8], int ielnew);
static void remapz(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1]);
static void remapy(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1]);
static void remapx(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1]);


//-----------------------------------------------------------
// For 3-D mesh adaptation (refinement+ coarsening)
//-----------------------------------------------------------
void adaptation(logical *ifmortar, int step)
{
  logical if_coarsen, if_refine, ifrepeat;
  int iel, miel, irefine, icoarsen, neltold;

  if (timeron) timer_start(t_adaptation);
  *ifmortar = false;
  // compute heat source center(x0,y0,z0)
  x0 = X00+VELX*time;
  _y0 = Y00+VELY*time;
  z0 = Z00+VELZ*time;

  // Search elements to be refined. Check with restrictions. Perform
  // refinement repeatedly until all desired refinements are done.

  // ich[iel]=0 no grid change on element iel
  // ich[iel]=2 iel is marked to be coarsened
  // ich[iel]=4 iel is marked to be refined

  // irefine records how many elements got refined
  irefine = 0;

  // check whether elements need to be refined because they have overlap
  // with the  heat source
  while (true) {
    find_refine(&if_refine);

    if (if_refine) {
      ifrepeat = true;
      while (ifrepeat) {
        // Check with restriction, unmark elements that cannot be refined.
        //Elements preventing desired refinement will be marked to be refined.
        check_refine(&ifrepeat);
      }
      // perform refinement
      do_refine(ifmortar, &irefine);
    } else {
      break;
    }
  }

  // Search for elements to be coarsened. Check with restrictions,
  // Perform coarsening repeatedly until all possible coarsening
  // is done.

  // icoarsen records how many elements got coarsened 
  icoarsen = 0;

  // skip[iel]=true indicates an element no longer exists (because it
  // got merged)
  l_init(skip, nelt, false);

  neltold = nelt;

  // Check whether elements need to be coarsened because they don't have
  // overlap with the heat source. Only elements that don't have a larger 
  // size neighbor can be marked to be coarsened

  while (true) {
    find_coarsen(&if_coarsen, neltold);

    if (if_coarsen) {
      // Perform coarsening, however subject to restriction. Only possible 
      // coarsening will be performed. if_coarsen=true indicates that
      // actual coarsening happened
      do_coarsen(&if_coarsen, &icoarsen, neltold);
      if (if_coarsen) {
        // ifmortar=true indicates the grid changed, i.e. the mortar points 
        // indices need to be regenerated on the new grid.
        *ifmortar = true;
      } else {
        break;
      } 
    }
  }

  printf("Step %4d: elements refined, merged, total:%6d %6d %6d\n",
      step, irefine, icoarsen, nelt);

  // mt_to_id[miel] takes as argument the morton index  and returns the actual
  //                element index
  // id_to_mt(iel)  takes as argument the actual element index and returns the
  //                morton index
  for (miel = 0; miel < nelt; miel++) {
    iel = mt_to_id[miel];
    id_to_mt[iel] = miel;
  }

  // Reorder the elements in the order of the morton curve. After the move 
  // subroutine the element indices are  the same as the morton indices
  move();

  // if the grid changed, regenerate mortar indices and update variables
  // associated to grid.
  if (*ifmortar) {
    mortar();
    prepwork();
  }
  if (timeron) timer_stop(t_adaptation);
}


//---------------------------------------------------------------
// Coarsening procedure: 
// 1) check with restrictions
// 2) perform coarsening
//---------------------------------------------------------------
static void do_coarsen(logical *if_coarsen, int *icoarsen, int neltold)
{
  logical test, test1, test2, test3;
  int iel, ntp[8], ic, parent, mielnew, miel;
  int i, index, num_coarsen; 

  *if_coarsen = false;

  // If an element has been merged, it will be skipped afterwards
  // skip[iel]=true for elements that will be skipped.
  // ifcoa_id[iel]=true indicates that element iel will be coarsened
  // ifcoa[miel]=true refers to element miel(mortar index) will be
  //                  coarsened

  ncopy(mt_to_id_old, mt_to_id, nelt);
  nr_init(mt_to_id, nelt, -1);
  l_init(ifcoa_id, neltold, false);

  // Check whether the potential coarsening will make neighbor, 
  // and neighbor's neighbor....break grid restriction
  for (miel = 0; miel < nelt; miel++) {
    ifcoa[miel] = false;
    front[miel] = 0;
    iel = mt_to_id_old[miel];
    // if an element is marked to be coarsened
    if (ich[iel] == 2) {

      // If the current  element is the "first" child (front-left-
      // bottom) of its parent (tree[iel] mod 8 equals 0), then 
      // find all its neighbors. Check whether they are from the same 
      // parent.

      ic = tree[iel];
      if (!btest(ic,0) && !btest(ic,1) && !btest(ic,2)) {
        ntp[0] = iel;
        ntp[1] = sje[iel][0][0][0];
        ntp[2] = sje[iel][2][0][0];
        ntp[3] = sje[ntp[2]][0][0][0];
        ntp[4] = sje[iel][4][0][0];
        ntp[5] = sje[ntp[4]][0][0][0];
        ntp[6] = sje[ntp[4]][2][0][0];
        ntp[7] = sje[ntp[6]][0][0][0];

        parent = tree[iel] >> 3;
        test = false;

        test1 = true;
        for (i = 0; i < 8; i++) {
          if ((tree[ntp[i]] >> 3) != parent) test1 = false;
        }

        // check whether all child elements are marked to be coarsened
        if (test1) {
          test2 = true;
          for (i = 0; i < 8; i++) {
            if (ich[ntp[i]] != 2) test2 = false;
          }

          // check whether all child elements can be coarsened or not.
          if (test2) {
            test3 = true;
            for (i = 0; i < 8; i++) {
              if (!icheck(ntp[i],i)) test3 = false;
            }
            if (test3) test = true;
          }
        }
        // if the eight child elements are eligible to be coarsened
        // mark the first children ifcoa[miel]=true
        // mark them all ifcoa_id[]=true
        // front[miel] will be used to calculate (potentially in parallel) 
        //             how many elements with seuqnece numbers less than
        //             miel will be coarsened.
        // skip[]      marks that an element will no longer exist after merge.

        if (test) {
          ifcoa[miel] = true;
          for (i = 0; i < 8; i++) {
            ifcoa_id[ntp[i]] = true;
          }
          front[miel] = 1;
          for (i = 0; i < 7; i++) {
            skip[ntp[i+1]] = true;
          }
          *if_coarsen = true;
        }
      } 
    } 
  } 

  // compute front[iel], how many elements will be coarsened before iel
  // (including iel)
  parallel_add(front);

  // num_coarsen is the total number of elements that will be coarsened
  num_coarsen = front[nelt-1];

  // action[i] records the morton index of the i'th element (if it is an
  // element's front-left-bottom-child) to be coarsened.

  // create array mt_to_id to convert actual element index to morton index
  for (miel = 0; miel < nelt; miel++) {
    iel = mt_to_id_old[miel];
    if (!skip[iel]) {
      if (ifcoa[miel]) {
        action[front[miel]-1] = miel;
        mielnew = miel-(front[miel]-1)*7;
      } else { 
        mielnew = miel-front[miel]*7;
      }
      mt_to_id[mielnew] = iel;
    }
  }

  // perform the coarsening procedure (potentially in parallel)
  for (index = 0; index < num_coarsen; index++) {
    miel = action[index];
    iel = mt_to_id_old[miel];
    // find eight child elements to be coarsened
    ntp[0] = iel;
    ntp[1] = sje[iel][0][0][0];
    ntp[2] = sje[iel][2][0][0];
    ntp[3] = sje[ntp[2]][0][0][0];
    ntp[4] = sje[iel][4][0][0];
    ntp[5] = sje[ntp[4]][0][0][0];
    ntp[6] = sje[ntp[4]][2][0][0];
    ntp[7] = sje[ntp[6]][0][0][0];
    // merge them to be the parent
    merging(ntp);
  }

  nelt = nelt - num_coarsen*7;
  *icoarsen = *icoarsen + num_coarsen*8;
}


//-------------------------------------------------------
// Refinement procedure
//--------------------------------------------------------
static void do_refine(logical *ifmortar, int *irefine)
{
  double xctemp[8], yctemp[8], zctemp[8], xleft, xright;
  double yleft, yright, zleft, zright, ta1temp[LX1][LX1][LX1];
  double xhalf, yhalf, zhalf;
  int iel, i, j, jface;
  int ntemp, ndir, facedir, k, le[4], ne[4], mielnew;
  int miel, num_refine, index, treetemp;
  int ijeltemp[6][2], sjetemp[6][2][2], n1, n2, nelttemp;
  int cb, cbctemp[6];

  // initialize
  ncopy(mt_to_id_old, mt_to_id, nelt);
  nr_init(mt_to_id, nelt, -1);
  nr_init(action, nelt, -1);
  for (miel = 0; miel < nelt; miel++) {
    if (ich[mt_to_id_old[miel]] != 4) {
      front[miel] = 0;
    } else {
      front[miel] = 1;
    }
  }

  // front[iel] records how many elements with sequence numbers less than
  // or equal to iel will be refined
  parallel_add(front);

  // num_refine is the total number of elements that will be refined
  num_refine = front[nelt-1];

  // action[i] records the morton index of the  i'th element to be refined
  for (miel = 0; miel < nelt; miel++) {
    iel = mt_to_id_old[miel];
    if (ich[iel] == 4) {
      action[front[miel]-1] = miel;
    }
  }

  // Compute array mt_to_id to convert the element index to morton index.
  // ref_front_id[iel] records how many elements with index less than
  // iel (actual element index, not morton index), will be refined.
  for (miel = 0; miel < nelt; miel++) {
    iel = mt_to_id_old[miel];
    if (ich[iel] == 4) {
      ntemp = (front[miel]-1)*7;
      mielnew = miel+ntemp;
    } else {
      ntemp = front[miel]*7;
      mielnew = miel+ntemp;
    }

    mt_to_id[mielnew] = iel;
    ref_front_id[iel] = nelt+ntemp;
  }


  // Perform refinement (potentially in parallel): 
  // - Cut an element into eight children.
  // - Assign them element index  as iel, nelt+1,...., nelt+7.
  // - Update neighboring information.

  nelttemp = nelt;

  if (num_refine > 0) {
    *ifmortar = true;
  }

  for (index = 0; index < num_refine; index++) {
    // miel is old morton index and mielnew is new morton index after refinement.
    miel = action[index];
    mielnew = miel+(front[miel]-1)*7;
    iel = mt_to_id_old[miel];
    nelt = nelttemp+(front[miel]-1)*7;
    // save iel's information in a temporary array
    treetemp = tree[iel];
    copy(xctemp, xc[iel], 8);
    copy(yctemp, yc[iel], 8);
    copy(zctemp, zc[iel], 8);
    ncopy(cbctemp, cbc[iel], 6);
    ncopy((int *)ijeltemp, ijel[iel][0], 12);
    ncopy((int *)sjetemp, sje[iel][0][0], 24);
    copy((double *)ta1temp, ta1[iel][0][0], NXYZ);

    // zero out iel here
    tree[iel] = 0;
    nr_init(cbc[iel], 6, 0);
    nr_init(sje[iel][0][0], 24, -1);
    nr_init(ijel[iel][0], 12, -1);
    r_init(ta1[iel][0][0], NXYZ, 0.0);

    // initialize new child elements:iel and nelt+1~nelt+7
    for (j = 0; j < 7; j++) {
      mt_to_id[mielnew+j+1] = nelt+j;
      tree[nelt+j] = 0;
      nr_init(cbc[nelt+j], 6, 0);
      nr_init(sje[nelt+j][0][0], 24, -1);
      nr_init(ijel[nelt+j][0], 12, -1);
      r_init(ta1[nelt+j][0][0], NXYZ, 0.0);
    }

    // update the tree[]
    ntemp = treetemp << 3;
    tree[iel] = ntemp;
    for (i = 0; i < 7; i++) {
      tree[nelt+i] = ntemp + ((i + 1) % 8);
    }
    // update the children's vertices' coordinates
    xhalf  = xctemp[0]+(xctemp[1]-xctemp[0])/2.0;
    xleft  = xctemp[0];
    xright = xctemp[1];
    yhalf  = yctemp[0]+(yctemp[2]-yctemp[0])/2.0;
    yleft  = yctemp[0];
    yright = yctemp[2];
    zhalf  = zctemp[0]+(zctemp[4]-zctemp[0])/2.0;
    zleft  = zctemp[0];
    zright = zctemp[4];

    for (j = 0; j < 7; j += 2) {
      for (i = 0; i < 7; i += 2) {
        xc[nelt+j][i]   = xhalf;
        xc[nelt+j][i+1] = xright; 
      }
    }

    for (j = 1; j < 6; j += 2) {
      for (i = 0; i < 7; i += 2) {
        xc[nelt+j][i]   = xleft;
        xc[nelt+j][i+1] = xhalf;
      }
    }

    for (i = 0; i < 7; i += 2) {
      xc[iel][i] = xleft;
      xc[iel][i+1] = xhalf;
    }

    for (i = 0; i < 2; i++) {
      yc[nelt+0][i] = yleft;
      yc[nelt+3][i] = yleft;
      yc[nelt+4][i] = yleft;
      yc[nelt+0][i+4] = yleft;
      yc[nelt+3][i+4] = yleft;
      yc[nelt+4][i+4] = yleft;
    }
    for (i = 2; i < 4; i++) {
      yc[nelt+0][i] = yhalf;
      yc[nelt+3][i] = yhalf;
      yc[nelt+4][i] = yhalf;
      yc[nelt+0][i+4] = yhalf;
      yc[nelt+3][i+4] = yhalf;
      yc[nelt+4][i+4] = yhalf;
    }
    for (j = 1; j < 3; j++) {
      for (i = 0; i < 2; i++) {
        yc[nelt+j][i] = yhalf;
        yc[nelt+j+4][i] = yhalf;
        yc[nelt+j][i+4] = yhalf;
        yc[nelt+j+4][i+4] = yhalf;
      }
      for (i = 2; i < 4; i++) {
        yc[nelt+j][i] = yright;
        yc[nelt+j+4][i] = yright;
        yc[nelt+j][i+4] = yright;
        yc[nelt+j+4][i+4] = yright;
      }
    }

    for (i = 0; i < 2; i++) {
      yc[iel][i] = yleft;
      yc[iel][i+4] = yleft;
    }
    for (i = 2; i < 4; i++) {
      yc[iel][i] = yhalf;
      yc[iel][i+4] = yhalf;
    }

    for (j = 0; j < 3; j++) {
      for (i = 0; i < 4; i++) {
        zc[nelt+j][i] = zleft;
        zc[nelt+j][i+4] = zhalf;
      }
    }
    for (j = 3; j < 7; j++) {
      for (i = 0; i < 4; i++) {
        zc[nelt+j][i] = zhalf;
        zc[nelt+j][i+4] = zright;
      }
    }
    for (i = 0; i < 4; i++) {
      zc[iel][i] = zleft;
      zc[iel][i+4] = zhalf;
    }

    // update the children's neighbor information

    // ndir refers to the x,y,z directions, respectively.
    // facedir refers to the orientation of the face in each direction, 
    // e.g. ndir=0, facedir=0 refers to face 1,
    // and ndir =0, facedir=1 refers to face 2.

    for (ndir = 0; ndir < 3; ndir++) {
      for (facedir = 0; facedir <= 1; facedir++) {
        i = 2*ndir+facedir;
        jface = jjface[i];
        cb = cbctemp[i];

        // find the new element indices of the four children on each
        // face of the parent element
        for (k = 0; k < 4; k++) {
          le[k] = le_arr[ndir][facedir][k]+nelt;
          ne[k] = le_arr[ndir][1-facedir][k]+nelt;
        }
        if (facedir == 0) {
          le[0] = iel;
        } else {
          ne[0] = iel;
        }
        // update neighbor information of the four child elements on each 
        // face of the parent element
        for (k = 0; k < 4; k++) {
          cbc[le[k]][i] = 2;
          sje[le[k]][i][0][0] = ne[k];
          ijel[le[k]][i][0] = 0;
          ijel[le[k]][i][1] = 0;
        }

        // if the face type of the parent element is type 2
        if (cb == 2 ) {
          ntemp = sjetemp[i][0][0];

          // if the neighbor ntemp is not marked to be refined
          if (ich[ntemp] != 4) {
            cbc[ntemp][jface] = 3;
            ijel[ntemp][jface][0] = 0;
            ijel[ntemp][jface][1] = 0;

            for (k = 0; k < 4; k++) {
              cbc[ne[k]][i] = 1;
              sje[ne[k]][i][0][0] = ntemp;
              if (k == 0) {
                ijel[ne[k]][i][0] = 0;
                ijel[ne[k]][i][1] = 0;
                sje[ntemp][jface][0][0] = ne[k];
              } else if (k == 1) {
                ijel[ne[k]][i][0] = 0;
                ijel[ne[k]][i][1] = 1;
                sje[ntemp][jface][1][0] = ne[k];
              } else if (k == 2) {
                ijel[ne[k]][i][0] = 1;
                ijel[ne[k]][i][1] = 0;
                sje[ntemp][jface][0][1] = ne[k];
              } else if (k == 3) {
                ijel[ne[k]][i][0] = 1;
                ijel[ne[k]][i][1] = 1;
                sje[ntemp][jface][1][1] = ne[k];
              }
            }

            // if the neighbor ntemp is also marked to be refined
          } else {
            n1 = ref_front_id[ntemp];

            for (k = 0; k < 4; k++) {
              cbc[ne[k]][i] = 2;
              n2 = n1 + le_arr[ndir][facedir][k];
              if (n2 == n1+7) n2 = ntemp;
              sje[ne[k]][i][0][0] = n2;
              ijel[ne[k]][i][0] = 0;
            }
          }
          // if the face type of the parent element is type 3
        } else if (cb == 3) {
          for (k = 0; k < 4; k++) {
            cbc[ne[k]][i] = 2;
            if (k == 0) {
              ntemp = sjetemp[i][0][0];
            } else if (k == 1) {
              ntemp = sjetemp[i][1][0];
            } else if (k == 2) {
              ntemp = sjetemp[i][0][1];
            } else if (k == 3) {
              ntemp = sjetemp[i][1][1];
            }
            ijel[ne[k]][i][0] = 0;
            ijel[ne[k]][i][1] = 0;
            sje[ne[k]][i][0][0] = ntemp;
            cbc[ntemp][jface] = 2;
            sje[ntemp][jface][0][0] = ne[k];
            ijel[ntemp][jface][0] = 0;
            ijel[ntemp][jface][1] = 0;
          }

          // if the face type of the parent element is type 0
        } else if (cb == 0) {
          for (k = 0; k < 4; k++) {
            cbc[ne[k]][i] = cb;
          }
        }
      } 
    } 

    // map solution from parent element to children
    remap(ta1[iel], &ta1[ref_front_id[iel]], ta1temp);
  }

  nelt = nelttemp + num_refine*7;
  *irefine = *irefine + num_refine;
  ntot = nelt*LX1*LX1*LX1;
}


//-----------------------------------------------------------
// returns whether element n1's face i and element n2's 
// jjface[iface] have intersections, i.e. whether n1 and 
// n2 are neighbored by an edge.
//-----------------------------------------------------------
static logical ifcor(int n1, int n2, int i, int iface)
{
  logical ret;

  ret = false;

  if (ifsame(n1, e1v1[i][iface], n2, e2v1[i][iface]) ||
      ifsame(n1, e1v2[i][iface], n2, e2v2[i][iface])) {
    ret = true;
  }

  return ret;
}


//-----------------------------------------------------------
// Check whether element ie's three faces (sharing vertex n)
// are nonconforming. This will prevent it from being coarsened.
// Also check ie's neighbors on those three faces, whether ie's
// neighbors by only an edge have a size smaller than ie's,
// which also prevents ie from being coarsened.
//-----------------------------------------------------------
static logical icheck(int ie, int n)
{
  int ntemp1, ntemp2, ntemp3, n1, n2, n3;
  int cb2_1, cb3_1, cb1_2, cb3_2, cb1_3, cb2_3;
  logical ret;

  ret = true;
  cb2_1 = 0;
  cb3_1 = 0;
  cb1_2 = 0;
  cb3_2 = 0;
  cb1_3 = 0;
  cb2_3 = 0;

  n1 = f_c[n][0];
  n2 = f_c[n][1];
  n3 = f_c[n][2];
  if ((cbc[ie][n1] == 3) || (cbc[ie][n2] == 3) || (cbc[ie][n3] == 3)) {
    ret = false;
  } else {
    ntemp1 = sje[ie][n1][0][0];
    ntemp2 = sje[ie][n2][0][0];
    ntemp3 = sje[ie][n3][0][0];
    if (ntemp1 != 0) {
      cb2_1 = cbc[ntemp1][n2];
      cb3_1 = cbc[ntemp1][n3];
    }
    if (ntemp2 != 0) {
      cb3_2 = cbc[ntemp2][n3];
      cb1_2 = cbc[ntemp2][n1];
    }
    if (ntemp3 != 0) {
      cb1_3 = cbc[ntemp3][n1];
      cb2_3 = cbc[ntemp3][n2];
    }
    if ((cbc[ie][n1] == 2 && (cb2_1 == 3 || cb3_1 == 3)) ||
        (cbc[ie][n2] == 2 && (cb3_2 == 3 || cb1_2 == 3)) ||
        (cbc[ie][n3] == 2 && (cb1_3 == 3 || cb2_3 == 3))) {
      ret = false;
    }
  }

  return ret;
}


//-----------------------------------------------------------
// Search elements to be coarsened. Check with restrictions.
// This subroutine only checks the element itself, not its
// neighbors.
//-----------------------------------------------------------
static void find_coarsen(logical *if_coarsen, int neltold)
{
  logical iftemp;
  int iel, i;

  *if_coarsen = false;

  for (iel = 0; iel < neltold; iel++) {
    if (!skip[iel]) {
      ich[iel] = 0;
      if (!iftouch(iel)) {
        iftemp = false;
        for (i = 0; i < NSIDES; i++) {
          // if iel has a larger size than its face neighbors, it
          // can not be coarsened
          if (cbc[iel][i] == 3) {
            iftemp = true;
          }
        }
        if(!iftemp) {
          *if_coarsen = true;
          ich[iel] = 2;
        }
      }
    }
  }
}


//-----------------------------------------------------------
// search elements to be refined based on whether they
// have overlap with the heat source
//-----------------------------------------------------------
static void find_refine(logical *if_refine)
{
  int iel;

  *if_refine = false;

  for (iel = 0; iel < nelt; iel++) {
    ich[iel] = 0;
    if (iftouch(iel)) {
      if ((xc[iel][1] - xc[iel][0]) > dlmin) {
        *if_refine = true;
        ich[iel] = 4;
      }
    }
  }
}


//-----------------------------------------------------------------
// Check whether the potential refinement will violate the
// restriction. If so, mark the neighbor and unmark the
// original element, and set ifrepeat true. i.e. this procedure
// needs to be repeated until no further check is needed
//-----------------------------------------------------------------
static void check_refine(logical *ifrepeat)
{
  int iel, iface, ntemp, nntemp, i, jface;

  *ifrepeat = false;

  for (iel = 0; iel < nelt; iel++) {
    // if iel is marked to be refined
    if (ich[iel] == 4) {
      // check its six faces
      for (i = 0; i < NSIDES; i++) {
        jface = jjface[i];
        ntemp = sje[iel][i][0][0];
        // if one face neighbor is larger in size than iel
        if (cbc[iel][i] == 1) {
          // unmark iel
          ich[iel] = 0;
          // the large size neighbor ntemp is marked to be refined
          if (ich[ntemp] != 4) {
            *ifrepeat = true;
            ich[ntemp] = 4;
          }
          // check iel's neighbor, neighbored by an edge on face i, which
          // must be a face neighbor of ntemp
          for (iface = 0; iface < NSIDES; iface++) {
            if (iface != i && iface != jface) {
              //if edge neighbors are larger than iel, mark them to be refined
              if (cbc[ntemp][iface] == 2) {
                nntemp = sje[ntemp][iface][0][0];
                // ifcor is to make sure the edge neighbor exist
                if (ich[nntemp] !=4 && ifcor(iel, nntemp, i, iface)) {
                  ich[nntemp] = 4;
                }
              }
            }
          }
          //if face neighbor are of the same size of iel, check edge neighbors
        } else if (cbc[iel][i] == 2) {
          for (iface = 0; iface < NSIDES; iface++) {
            if (iface != i && iface != jface) {
              if (cbc[ntemp][iface] == 1) {
                nntemp = sje[ntemp][iface][0][0];
                ich[nntemp] = 4;
                ich[iel] = 0;
                *ifrepeat = true;
              }
            }
          }
        }
      }
    }
  }
}


//-----------------------------------------------------------------
// check whether element iel has overlap with the heat source
//-----------------------------------------------------------------
static logical iftouch(int iel)
{
  double dis, dis1, dis2, dis3, alpha2;

  alpha2 = alpha*alpha;

  if (x0 < xc[iel][0]) {
    dis1 = xc[iel][0] - x0;
  } else if (x0 > xc[iel][1]) {
    dis1 = x0 - xc[iel][1];
  } else {
    dis1 = 0.0;
  }

  if (_y0 < yc[iel][0]) {
    dis2 = yc[iel][0] - _y0;
  } else if (_y0 > yc[iel][2]) {
    dis2 = _y0 - yc[iel][2];
  } else {
    dis2 = 0.0;
  }

  if (z0 < zc[iel][0]) {
    dis3 = zc[iel][0] - z0;
  } else if (z0 > zc[iel][4]) {
    dis3 = z0 - zc[iel][4];
  } else {
    dis3 = 0.0;
  }

  dis = dis1*dis1 + dis2*dis2 + dis3*dis3;

  if (dis < alpha2) {
    return true;
  } else {
    return false;
  }
}


//-----------------------------------------------------------------
// After a refinement, map the solution  from the parent (x) to
// the eight children. y is the solution on the first child
// (front-bottom-left) and y1 is the solution on the next 7 
// children.
//-----------------------------------------------------------------
static void remap(double y[LX1][LX1][LX1], double y1[7][LX1][LX1][LX1],
                  double x[LX1][LX1][LX1])
{
  double yone[2][LX1][LX1][LX1], ytwo[4][LX1][LX1][LX1];
  int i, iz, ii, jj, kk;

  r_init((double *)y, LX1*LX1*LX1, 0.0);
  r_init((double *)y1, LX1*LX1*LX1*7, 0.0);
  r_init((double *)yone, LX1*LX1*LX1*2, 0.0);
  r_init((double *)ytwo, LX1*LX1*LX1*4, 0.0);

  for (i = 0; i < LX1; i++) {
    for (kk = 0; kk < LX1; kk++) {
      for (jj = 0; jj < LX1; jj++) {
        for (ii = 0; ii < LX1; ii++) {
          yone[0][i][jj][ii] = yone[0][i][jj][ii] +ixmc1[kk][ii]*x[i][jj][kk];
          yone[1][i][jj][ii] = yone[1][i][jj][ii] +ixmc2[kk][ii]*x[i][jj][kk];
        }
      }
    }

    for (kk = 0; kk < LX1; kk++) {
      for (jj = 0; jj < LX1; jj++) {
        for (ii = 0; ii < LX1; ii++) {
          ytwo[0][jj][i][ii] = ytwo[0][jj][i][ii] + 
                               yone[0][i][kk][ii]*ixtmc1[jj][kk];
          ytwo[1][jj][i][ii] = ytwo[1][jj][i][ii] + 
                               yone[0][i][kk][ii]*ixtmc2[jj][kk];
          ytwo[2][jj][i][ii] = ytwo[2][jj][i][ii] + 
                               yone[1][i][kk][ii]*ixtmc1[jj][kk];
          ytwo[3][jj][i][ii] = ytwo[3][jj][i][ii] + 
                               yone[1][i][kk][ii]*ixtmc2[jj][kk];
        }
      }
    }
  }

  for (iz = 0; iz < LX1; iz++) {
    for (kk = 0; kk < LX1; kk++) {
      for (jj = 0; jj < LX1; jj++) {
        for (ii = 0; ii < LX1; ii++) {
          y[jj][iz][ii] = y[jj][iz][ii] +
                          ytwo[0][iz][kk][ii]*ixtmc1[jj][kk];
          y1[0][jj][iz][ii] = y1[0][jj][iz][ii] +
                              ytwo[2][iz][kk][ii]*ixtmc1[jj][kk];
          y1[1][jj][iz][ii] = y1[1][jj][iz][ii] +
                              ytwo[1][iz][kk][ii]*ixtmc1[jj][kk];
          y1[2][jj][iz][ii] = y1[2][jj][iz][ii] +
                              ytwo[3][iz][kk][ii]*ixtmc1[jj][kk];
          y1[3][jj][iz][ii] = y1[3][jj][iz][ii] +
                              ytwo[0][iz][kk][ii]*ixtmc2[jj][kk];
          y1[4][jj][iz][ii] = y1[4][jj][iz][ii] +
                              ytwo[2][iz][kk][ii]*ixtmc2[jj][kk];
          y1[5][jj][iz][ii] = y1[5][jj][iz][ii] +
                              ytwo[1][iz][kk][ii]*ixtmc2[jj][kk];
          y1[6][jj][iz][ii] = y1[6][jj][iz][ii] +
                              ytwo[3][iz][kk][ii]*ixtmc2[jj][kk];           
        }
      }
    }
  }
}


//-----------------------------------------------------------------------
// This subroutine is to merge the eight child elements and map 
// the solution from eight children to the  merged element. 
// iela array records the eight elements to be merged.
//-----------------------------------------------------------------------
static void merging(int iela[8])
{
  double x1, x2, y1, y2, z1, z2;
  int ielnew, i, ntemp, jface, ii, cb, ntempa[4], ielold, ntema[4];

  ielnew = iela[0];

  tree[ielnew] = tree[ielnew] >> 3;

  // element vertices 
  x1 = xc[iela[0]][0];
  x2 = xc[iela[1]][1];
  y1 = yc[iela[0]][0];
  y2 = yc[iela[2]][2];
  z1 = zc[iela[0]][0];
  z2 = zc[iela[4]][4];

  for (i = 0; i < 7; i += 2) {
    xc[ielnew][i] = x1;
  }
  for (i = 1; i < 8; i += 2) {
    xc[ielnew][i] = x2;
  }
  for (i = 0; i < 2; i++) {
    yc[ielnew][i] = y1;
    yc[ielnew][i+4] = y1;
  }
  for (i = 2; i < 4; i++) {
    yc[ielnew][i] = y2;
    yc[ielnew][i+4] = y2;
  }
  for (i = 0; i < 4; i++) {
    zc[ielnew][i] = z1;
  }
  for (i = 4; i < 8; i++) {
    zc[ielnew][i] = z2;
  }

  // update neighboring information
  for (i = 0; i < NSIDES; i++) {
    jface = jjface[i];
    ielold = iela[children[i][0]];
    for (ii = 0; ii < 4; ii++) {
      ntempa[ii] = iela[children[i][ii]];
    }

    cb = cbc[ielold][i];

    if (cb == 2) {
      // if the neighbor elements also will be coarsened
      if (ifcoa_id[sje[ielold][i][0][0]]) {
        if (i == 1 || i == 3 || i == 5) {
          ntemp = sje[sje[ntempa[0]][i][0][0]][i][0][0];
        } else {
          ntemp = sje[ntempa[0]][i][0][0];
        } 
        sje[ielnew][i][0][0] = ntemp;
        ijel[ielnew][i][0] = 0;
        ijel[ielnew][i][1] = 0;
        cbc[ielnew][i] = 2;

        // if the neighbor elements will not be coarsened
      } else {
        for (ii = 0; ii < 4; ii++) {
          ntema[ii] = sje[ntempa[ii]][i][0][0];
          cbc[ntema[ii]][jface] = 1;
          sje[ntema[ii]][jface][0][0] = ielnew;
          ijel[ntema[ii]][jface][0] = iijj[ii][0];
          ijel[ntema[ii]][jface][1] = iijj[ii][1];
          sje[ielnew][i][iijj[ii][1]][iijj[ii][0]] = ntema[ii];
          ijel[ielnew][i][0] = 0;
          ijel[ielnew][i][1] = 0;
        }
        cbc[ielnew][i] = 3;
      }       
    } else if (cb == 1) {

      ntemp = sje[ielold][i][0][0];
      cbc[ntemp][jface] = 2;
      ijel[ntemp][jface][0] = 0;
      ijel[ntemp][jface][1] = 0;
      sje[ntemp][jface][0][0] = ielnew;
      sje[ntemp][jface][1][0] = -1;
      sje[ntemp][jface][0][1] = -1;
      sje[ntemp][jface][1][1] = -1;

      cbc[ielnew][i] = 2;
      ijel[ielnew][i][0] = 0;
      ijel[ielnew][i][1] = 0;
      sje[ielnew][i][0][0] = ntemp;

    } else if (cb == 0) {
      cbc[ielnew][i] = 0;
      sje[ielnew][i][0][0] = -1;
      sje[ielnew][i][1][0] = -1;
      sje[ielnew][i][0][1] = -1;
      sje[ielnew][i][1][1] = -1;
    }
  }

  // map solution from children to the merged element
  remap2(iela, ielnew);
}


//-----------------------------------------------------------------
// Map the solution from the children to the parent.
// iela array records the eight elements to be merged.
// ielnew is the element index of the merged element.
//-----------------------------------------------------------------
static void remap2(int iela[8], int ielnew)
{
  double temp1[LX1][LX1][LX1], temp2[LX1][LX1][LX1];
  double temp3[LX1][LX1][LX1], temp4[LX1][LX1][LX1];
  double temp5[LX1][LX1][LX1], temp6[LX1][LX1][LX1];

  remapx(ta1[iela[0]], ta1[iela[1]], temp1);
  remapx(ta1[iela[2]], ta1[iela[3]], temp2);
  remapx(ta1[iela[4]], ta1[iela[5]], temp3);
  remapx(ta1[iela[6]], ta1[iela[7]], temp4);
  remapy(temp1, temp2, temp5);
  remapy(temp3, temp4, temp6);
  remapz(temp5, temp6, ta1[ielnew]);
}


//-----------------------------------------------------------------
// z direction mapping after the merge.
// Map solution from x1 & x2 to y.
//-----------------------------------------------------------------
static void remapz(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1])
{
  int ix, iy, ip;

  for (iy = 0; iy < LX1; iy++) {
    for (ix = 0; ix < LX1; ix++) {
      y[0][iy][ix] = x1[0][iy][ix];

      y[1][iy][ix] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[1][iy][ix] = y[1][iy][ix]+map2[ip]*x1[ip][iy][ix];
      }

      y[2][iy][ix] = x1[LX1-1][iy][ix];

      y[3][iy][ix] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[3][iy][ix] = y[3][iy][ix]+map4[ip]*x2[ip][iy][ix];
      }

      y[LX1-1][iy][ix] = x2[LX1-1][iy][ix];
    }
  }
}


//-----------------------------------------------------------------
// y direction mapping after the merge.
// Map solution from x1 & x2 to y.
//-----------------------------------------------------------------
static void remapy(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1])
{
  int ix, iz, ip;

  for (iz = 0; iz < LX1; iz++) {
    for (ix = 0; ix < LX1; ix++) {
      y[iz][0][ix] = x1[iz][0][ix];

      y[iz][1][ix] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[iz][1][ix] = y[iz][1][ix]+map2[ip]*x1[iz][ip][ix];
      }

      y[iz][2][ix] = x1[iz][LX1-1][ix];

      y[iz][3][ix] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[iz][3][ix] = y[iz][3][ix]+map4[ip]*x2[iz][ip][ix];
      }

      y[iz][LX1-1][ix] = x2[iz][LX1-1][ix];
    }
  }
}


//-----------------------------------------------------------------
// x direction mapping after the merge.
// Map solution from x1 & x2 to y.
//-----------------------------------------------------------------
static void remapx(double x1[LX1][LX1][LX1], double x2[LX1][LX1][LX1],
                   double y[LX1][LX1][LX1])
{
  int iy, iz, ip;

  for (iz = 0; iz < LX1; iz++) {
    for (iy = 0; iy < LX1; iy++) {
      y[iz][iy][0] = x1[iz][iy][0];

      y[iz][iy][1] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[iz][iy][1] = y[iz][iy][1]+map2[ip]*x1[iz][iy][ip];
      }

      y[iz][iy][2] = x1[iz][iy][LX1-1];

      y[iz][iy][3] = 0.0;
      for (ip = 0; ip < LX1; ip++) {
        y[iz][iy][3] = y[iz][iy][3]+map4[ip]*x2[iz][iy][ip];
      }

      y[iz][iy][LX1-1] = x2[iz][iy][LX1-1];
    }
  }
}
