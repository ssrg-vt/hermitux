#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "HJM_type.h"
#include "HJM.h"
#include "nr_routines.h"

#ifdef TBB_VERSION
#include <pthread.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/cache_aligned_allocator.h"


#define PARALLEL_B_GRAINSIZE 8


struct ParallelB {
  __volatile__ int l;
   FTYPE **pdZ;
  FTYPE **randZ;
  int BLOCKSIZE;
  int iN;

  ParallelB(FTYPE **pdZ_, FTYPE **randZ_, int BLOCKSIZE_, int iN_)//:
  //    pdZ(pdZ_), randZ(randZ_), BLOCKSIZE(BLOCKSIZE_), iN(iN_)
  {
    pdZ = pdZ_;
    randZ = randZ_;
    BLOCKSIZE = BLOCKSIZE_;
    iN = iN_; 
    /*fprintf(stderr,"(Construction object) pdZ=0x%08x, randZ=0x%08x\n",
      pdZ, randZ);*/

  }
  void set_l(int l_){l = l_;}

  void operator()(const tbb::blocked_range<int> &range) const {
    int begin = range.begin();
    int end   = range.end();
    int b,j;
    /*fprintf(stderr,"B: Thread %d from %d to %d. l=%d pdZ=0x%08x, BLOCKSIZE=%d, iN=%d\n",
      pthread_self(), begin, end, l,(int)pdZ,BLOCKSIZE,iN); */

    for(b=begin; b!=end; b++) {
      for (j=1;j<=iN-1;++j){
	pdZ[l][BLOCKSIZE*j + b]= CumNormalInv(randZ[l][BLOCKSIZE*j + b]);  /* 18% of the total executition time */
	//fprintf(stderr,"%d (%d, %d): [%d][%d]=%e\n",pthread_self(), begin, end,  l,BLOCKSIZE*j+b,pdZ[l][BLOCKSIZE*j + b]);
      }
    }

  }

};


#endif // TBB_VERSION

void serialB(FTYPE **pdZ, FTYPE **randZ, int BLOCKSIZE, int iN, int iFactors)
{

  
  for(int l=0;l<=iFactors-1;++l){
    for(int b=0; b<BLOCKSIZE; b++){
      for (int j=1;j<=iN-1;++j){
	pdZ[l][BLOCKSIZE*j + b]= CumNormalInv(randZ[l][BLOCKSIZE*j + b]);  /* 18% of the total executition time */
      }
    }
  }
}

int HJM_SimPath_Forward_Blocking(FTYPE **ppdHJMPath,	//Matrix that stores generated HJM path (Output)
				 int iN,					//Number of time-steps
				 int iFactors,			//Number of factors in the HJM framework
				 FTYPE dYears,			//Number of years
				 FTYPE *pdForward,		//t=0 Forward curve
				 FTYPE *pdTotalDrift,	//Vector containing total drift corrections for different maturities
				 FTYPE **ppdFactors,	//Factor volatilities
				 long *lRndSeed,			//Random number seed
				 int BLOCKSIZE)
{	
//This function computes and stores an HJM Path for given inputs

	int iSuccess = 0;
	int i,j,l; //looping variables
	FTYPE **pdZ; //vector to store random normals
	FTYPE **randZ; //vector to store random normals
	FTYPE dTotalShock; //total shock by which the forward curve is hit at (t, T-t)
	FTYPE ddelt, sqrt_ddelt; //length of time steps	

	ddelt = (FTYPE)(dYears/iN);
	sqrt_ddelt = sqrt(ddelt);

	pdZ   = dmatrix(0, iFactors-1, 0, iN*BLOCKSIZE -1); //assigning memory
	randZ = dmatrix(0, iFactors-1, 0, iN*BLOCKSIZE -1); //assigning memory

	// =====================================================
	// t=0 forward curve stored iN first row of ppdHJMPath
	// At time step 0: insert expected drift 
	// rest reset to 0
	for(int b=0; b<BLOCKSIZE; b++){
	  for(j=0;j<=iN-1;j++){
	    ppdHJMPath[0][BLOCKSIZE*j + b] = pdForward[j]; 

	    for(i=1;i<=iN-1;++i)
	      { ppdHJMPath[i][BLOCKSIZE*j + b]=0; } //initializing HJMPath to zero
	  }
	}
	// -----------------------------------------------------
	
        // =====================================================
        // sequentially generating random numbers


        for(int b=0; b<BLOCKSIZE; b++){
          for(int s=0; s<1; s++){
            for (j=1;j<=iN-1;++j){
              for (l=0;l<=iFactors-1;++l){
                //compute random number in exact same sequence
                randZ[l][BLOCKSIZE*j + b + s] = RanUnif(lRndSeed);  /* 10% of the total executition time */
              }
            }
          }
        }

	// =====================================================
	// shocks to hit various factors for forward curve at t

#ifdef TBB_VERSION
	ParallelB B(pdZ, randZ, BLOCKSIZE, iN);
	for(l=0;l<=iFactors-1;++l){
	  B.set_l(l);
	  tbb::parallel_for(tbb::blocked_range<int>(0, BLOCKSIZE, PARALLEL_B_GRAINSIZE),B);
	}

#else
	/* 18% of the total executition time */
	serialB(pdZ, randZ, BLOCKSIZE, iN, iFactors);
#endif

	// =====================================================
	// Generation of HJM Path1
	for(int b=0; b<BLOCKSIZE; b++){ // b is the blocks
	  for (j=1;j<=iN-1;++j) {// j is the timestep
	    
	    for (l=0;l<=iN-(j+1);++l){ // l is the future steps
	      dTotalShock = 0;
	      
	      for (i=0;i<=iFactors-1;++i){// i steps through the stochastic factors
		dTotalShock += ppdFactors[i][l]* pdZ[i][BLOCKSIZE*j + b];		  		
	      }	      	   

	      ppdHJMPath[j][BLOCKSIZE*l+b] = ppdHJMPath[j-1][BLOCKSIZE*(l+1)+b]+ pdTotalDrift[l]*ddelt + sqrt_ddelt*dTotalShock;
	      //as per formula
	    }
	  }
	} // end Blocks
	// -----------------------------------------------------

	free_dmatrix(pdZ, 0, iFactors -1, 0, iN*BLOCKSIZE -1);
	free_dmatrix(randZ, 0, iFactors -1, 0, iN*BLOCKSIZE -1);
	iSuccess = 1;
	return iSuccess;
}
	


