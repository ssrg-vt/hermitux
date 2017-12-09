//HJM.cpp
//Routine to setup HJM framework.
//Authors: Mark Broadie, Jatin Dewanwala, Columbia University 
//Collaborator: Mikhail Smelyanskiy, Intel
//Based on hjm_simn.xls created by Mark Broadie

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "nr_routines.h"
#include "HJM.h"
#include "HJM_type.h"



int HJM_SimPath_Yield(FTYPE **ppdHJMPath, int iN, int iFactors, FTYPE dYears, FTYPE *pdYield, FTYPE **ppdFactors,
					  long *lRndSeed);
int HJM_SimPath_Forward(FTYPE **ppdHJMPath, int iN, int iFactors, FTYPE dYears, FTYPE *pdForward, FTYPE *pdTotalDrift,
						FTYPE **ppdFactors, long *lRndSeed);
int HJM_Yield_to_Forward(FTYPE *pdForward, int iN, FTYPE *pdYield);
int HJM_Factors(FTYPE **ppdFactors,int iN, int iFactors, FTYPE *pdVol, FTYPE **ppdFacBreak);
int HJM_Drifts(FTYPE *pdTotalDrift, FTYPE **ppdDrifts, int iN, int iFactors, FTYPE dYears, FTYPE **ppdFactors);
int HJM_Correlations(FTYPE **ppdHJMCorr, int iN, int iFactors, FTYPE **ppdFactors);
int HJM_Forward_to_Yield(FTYPE *pdYield, int iN, FTYPE *pdForward);
int Discount_Factors(FTYPE *pdDiscountFactors, int iN, FTYPE dYears, FTYPE *pdRatePath);
//int Discount_Factors_early_exit(FTYPE *pdDiscountFactors, int iN, FTYPE dYears, FTYPE *pdRatePath, int iSwapStartTimeIndex);

int HJM_SimPath_Yield(FTYPE **ppdHJMPath,  //Matrix that stores generated HJM path (Output)
					  int iN,				//Number of time-steps
					  int iFactors,			//Number of factors in the HJM framework
					  FTYPE dYears,		//Number of years
					  FTYPE *pdYield,		//Input yield curve (at t=0) for dYears (iN time steps)
					  FTYPE **ppdFactors,	//Matrix of Factor Volatilies
					  long *lRndSeed)
{
//This function returns a single generated HJM Path for the given inputs

	int iSuccess = 0;						//return variable
	
	FTYPE *pdForward;						//Vector that will store forward curve computed from given yield curve
	FTYPE **ppdDrifts;						//Matrix that will store drifts for different maturities for each factor
	FTYPE *pdTotalDrift;					//Vector that stores total drift for each maturity

	pdForward = dvector(0, iN-1);
	ppdDrifts = dmatrix(0, iFactors-1, 0, iN-2);
	pdTotalDrift = dvector(0, iN-2);
    	
	//generating forward curve at t=0 from supplied yield curve
	iSuccess = HJM_Yield_to_Forward(pdForward, iN, pdYield);
	if (iSuccess!=1)
	{
		free_dvector(pdForward, 0, iN-1);
		free_dmatrix(ppdDrifts, 0, iFactors-1, 0, iN-2);
		free_dvector(pdTotalDrift, 0, iN-1);
		return iSuccess;
	}

	//computation of drifts from factor volatilities
	iSuccess = HJM_Drifts(pdTotalDrift, ppdDrifts, iN, iFactors, dYears, ppdFactors);
	if (iSuccess!=1)
	{
		free_dvector(pdForward, 0, iN-1);
		free_dmatrix(ppdDrifts, 0, iFactors-1, 0, iN-2);
		free_dvector(pdTotalDrift, 0, iN-1);
		return iSuccess;
	}
	
	//generating HJM Path
	iSuccess = HJM_SimPath_Forward(ppdHJMPath, iN, iFactors, dYears, pdForward, pdTotalDrift,ppdFactors, lRndSeed);
	if (iSuccess!=1)
	{
		free_dvector(pdForward, 0, iN-1);
		free_dmatrix(ppdDrifts, 0, iFactors-1, 0, iN-2);
		free_dvector(pdTotalDrift, 0, iN-1);
		return iSuccess;
	}

	free_dvector(pdForward, 0, iN-1);
	free_dmatrix(ppdDrifts, 0, iFactors-1, 0, iN-2);
	free_dvector(pdTotalDrift, 0, iN-1);
	iSuccess = 1;
	return iSuccess;
}


int HJM_Yield_to_Forward (FTYPE *pdForward,	//Forward curve to be outputted
						 int iN,				//Number of time-steps
						 FTYPE *pdYield)		//Input yield curve 
{
//This function computes forward rates from supplied yield rates.

	int iSuccess=0;
	int i;

	//forward curve computation
	pdForward[0] = pdYield[0];
	for(i=1;i<=iN-1; ++i){
	  pdForward[i] = (i+1)*pdYield[i] - i*pdYield[i-1];	//as per formula
	  //printf("pdForward: %f = (%d+1)*%f - %d*%f \n", pdForward[i], i, pdYield[i], i, pdYield[i-1]);
	}
	iSuccess=1;
	return iSuccess;
}


int HJM_Factors(FTYPE **ppdFactors,	//Output matrix that stores factor volatilities for different maturities
				int iN,					
				int iFactors, 
				FTYPE *pdVol,			//Input vector of total volatilities for different maturities
				FTYPE **ppdFacBreak)	//Input matrix of factor weights for each maturity
{
//This function computes individual volatilities  for each factor for different maturities.
//The function is called when the user inputs total volatility data and the weight distribution
//according to which the total variance has to be split accross various factors.

//For instance, the user may supply				   Maturity:	1	   2	  3      4		
//total vol (pdVol) as								  Sigma:  1.35%, 1.30%, 1.25%, 1.20%,....
//and the weight breakdown (ppdFacBreak) as        Factor 1:   0.55,  0.60,  0.65,  0.69,....
//												   Factor 2:   0.44,  0.39,  0.34,  0.30,....
//												   Factor 3:   0.01,  0.01,  0.01,  0.01,....
//Note that the weights add up to 1 in each case. Also, the weights are based on variance not volatility.

//Based on these inputs, the function will calculate individual volatilties for each factor for each maturity.
//The output (ppdFactors) may look something like: Maturity:	1	   2	  3      4		
//												   Factor 1:  1.00%  1.00%  1.00%  1.00%
//												   Factor 2:  0.90%  0.82%  0.74%  0.67%
//												   Factor 3:  0.10%  0.08%  0.05%  0.03%
// (Please note that in this example the figures have been rounded and therefore may not be exact.)

	int i,j; //looping variables
	int iSuccess = 0;
	
	//Computation of factor volatilities
	for(i = 0; i<=iFactors-1; ++i)
		for(j=0; j<=iN-2;++j)
			ppdFactors[i][j] = sqrt((ppdFacBreak[i][j])*(pdVol[j])*(pdVol[j]));
	
	iSuccess =1;
	return iSuccess;
}


int HJM_Drifts(FTYPE *pdTotalDrift,	//Output vector that stores the total drift correction for each maturity
			   FTYPE **ppdDrifts,		//Output matrix that stores drift correction for each factor for each maturity
			   int iN, 
			   int iFactors,
			   FTYPE dYears,
			   FTYPE **ppdFactors)		//Input factor volatilities
{
//This function computes drift corrections required for each factor for each maturity based on given factor volatilities

	int iSuccess =0;
	int i, j, l; //looping variables
	FTYPE ddelt = (FTYPE) (dYears/iN);
	FTYPE dSumVol;
	
	//computation of factor drifts for shortest maturity
	for (i=0; i<=iFactors-1; ++i)
		ppdDrifts[i][0] = 0.5*ddelt*(ppdFactors[i][0])*(ppdFactors[i][0]);
	
	//computation of factor drifts for other maturities
	for (i=0; i<=iFactors-1;++i)
		for (j=1; j<=iN-2; ++j)
		{
			ppdDrifts[i][j] = 0;
			for(l=0;l<=j-1;++l)
				ppdDrifts[i][j] -= ppdDrifts[i][l];
			dSumVol=0;
			for(l=0;l<=j;++l)
				dSumVol += ppdFactors[i][l];
			ppdDrifts[i][j] += 0.5*ddelt*(dSumVol)*(dSumVol);
		}
	
	//computation of total drifts for all maturities
	for(i=0;i<=iN-2;++i)
	{
		pdTotalDrift[i]=0;
		for(j=0;j<=iFactors-1;++j)
			pdTotalDrift[i]+= ppdDrifts[j][i];
	}

	iSuccess=1;
	return iSuccess;
}

int HJM_SimPath_Forward(FTYPE **ppdHJMPath,	//Matrix that stores generated HJM path (Output)
						int iN,					//Number of time-steps
						int iFactors,			//Number of factors in the HJM framework
						FTYPE dYears,			//Number of years
						FTYPE *pdForward,		//t=0 Forward curve
						FTYPE *pdTotalDrift,	//Vector containing total drift corrections for different maturities
						FTYPE **ppdFactors,	//Factor volatilities
						long *lRndSeed)			//Random number seed
{	
//This function computes and stores an HJM Path for given inputs

	int iSuccess = 0;
	int i,j,l; //looping variables
	
	FTYPE ddelt; //length of time steps
	FTYPE dTotalShock; //total shock by which the forward curve is hit at (t, T-t)
	FTYPE *pdZ; //vector to store random normals
	
	ddelt = (FTYPE)(dYears/iN);
	
	pdZ = dvector(0, iFactors -1); //assigning memory

	for(i=0;i<=iN-1;++i)
		for(j=0;j<=iN-1;++j)
			ppdHJMPath[i][j]=0; //initializing HJMPath to zero

	//t=0 forward curve stored iN first row of ppdHJMPath
	for(i=0;i<=iN-1; ++i)
		ppdHJMPath[0][i] = pdForward[i]; 
	
	//Generation of HJM Path
	for (j=1;j<=iN-1;++j)
	{

	  for (l=0;l<=iFactors-1;++l)
	    pdZ[l]= CumNormalInv(RanUnif(lRndSeed)); //shocks to hit various factors for forward curve at t

		for (l=0;l<=iN-(j+1);++l)
		{
		  dTotalShock = 0;
		  for (i=0;i<=iFactors-1;++i)
		    dTotalShock += ppdFactors[i][l]* pdZ[i];
		  ppdHJMPath[j][l] = ppdHJMPath[j-1][l+1]+ pdTotalDrift[l]*ddelt + sqrt(ddelt)*dTotalShock;
		  //as per formula
		}
	}
	
	free_dvector(pdZ, 0, iFactors -1);
	iSuccess = 1;
	return iSuccess;
}




int HJM_Correlations(FTYPE **ppdHJMCorr,//Matrix that stores correlations among factor volatilities for different maturities
					 int iN, 
					 int iFactors, 
					 FTYPE **ppdFactors)
{
//This function is based on factor.xls created by Mark Broadie
//The function computes correlations between factor volatilities for different maturities

	int iSuccess = 0;
	int i, j, l; //looping variables
	FTYPE *pdTotalVol; //vector that stores total volatility data for different maturities
	FTYPE **ppdWeights; //matrix that stores ratio of each factor to total volatility for different maturities
	
	pdTotalVol = dvector(0,iN-2);
	ppdWeights = dmatrix(0, iFactors-1,0, iN-2);
	
	//Total Volatility computed from given factor volatilities
	for(i=0;i<=iN-2;++i)
	{
		pdTotalVol[i]=0;
		for(j=0;j<=iFactors-1;++j)
			pdTotalVol[i] += ppdFactors[j][i]*ppdFactors[j][i];
		pdTotalVol[i] = sqrt(pdTotalVol[i]);
	}
	
	//Weights computed
	for(i=0;i<=iN-2;++i)
		for(j=0;j<=iFactors-1;++j)
			ppdWeights[j][i] = ppdFactors[j][i]/pdTotalVol[i];

	//Output matrix initialized to zero
	for(i=0;i<=iN-2;++i)
		for(j=0;j<=iN-2;++j)
			ppdHJMCorr[i][j]=0;
	
	//Correlations computed
	for(i=0;i<=iN-2;++i)
		for(j=i;j<=iN-2;++j)
			for(l=0;l<=iFactors-1;++l)
				ppdHJMCorr[i][j] += ppdWeights[l][i]*ppdWeights[l][j];
	
	free_dvector(pdTotalVol, 0,iN-2);
	free_dmatrix(ppdWeights, 0, iFactors-1,0, iN-2);
	iSuccess = 1;
	return iSuccess;
}


int HJM_Forward_to_Yield (FTYPE *pdYield,	//Output yield curve
						 int iN,
						 FTYPE *pdForward)	//Input forward curve
{
//This function computes yield rates from supplied forward rates.

	int iSuccess=0;
	int i;

	//t=0 yield curve
	pdYield[0] = pdForward[0];
	for(i=1;i<=iN-1; ++i)
		pdYield[i] = (i*pdYield[i-1] + pdForward[i])/(i+1);

	iSuccess=1;
	return iSuccess;
}

int Discount_Factors(FTYPE *pdDiscountFactors, 
                     int iN, 
                     FTYPE dYears, 
                     FTYPE *pdRatePath)
{
        int i,j;                                //looping variables
        int iSuccess;                   //return variable

        FTYPE ddelt;                   //HJM time-step length
        ddelt = (FTYPE) (dYears/iN);

        //initializing the discount factor vector
        for (i=0; i<=iN-1; ++i)
                pdDiscountFactors[i] = 1.0;

        for (i=1; i<=iN-1; ++i)
          for (j=0; j<=i-1; ++j)
            pdDiscountFactors[i] *= exp(-pdRatePath[j]*ddelt);

        iSuccess = 1;
        return iSuccess;
}

int Discount_Factors_opt(FTYPE *pdDiscountFactors, 
		     int iN, 
		     FTYPE dYears, 
		     FTYPE *pdRatePath)
{
	int i,j;				//looping variables
	int iSuccess;			//return variable
	
	FTYPE ddelt;			//HJM time-step length
	ddelt = (FTYPE) (dYears/iN);

	FTYPE *pdexpRes;
	pdexpRes = dvector(0,iN-2);

	//initializing the discount factor vector
	for (i=0; i<=iN-1; ++i)
	  pdDiscountFactors[i] = 1.0;

	//precompute the exponientials
	for (j=0; j<=(i-2); ++j){ pdexpRes[j] = -pdRatePath[j]*ddelt; }
	for (j=0; j<=(i-2); ++j){ pdexpRes[j] = exp(pdexpRes[j]);  }
	
	for (i=1; i<=iN-1; ++i)
	  for (j=0; j<=i-1; ++j)
	    pdDiscountFactors[i] *= pdexpRes[j];
		
	free_dvector(pdexpRes, 0, iN-2);
	iSuccess = 1;
	return iSuccess;
}


// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
int Discount_Factors_Blocking(FTYPE *pdDiscountFactors, 
			      int iN, 
			      FTYPE dYears, 
			      FTYPE *pdRatePath,
			      int BLOCKSIZE)
{
	int i,j,b;				//looping variables
	int iSuccess;			//return variable
	
	FTYPE ddelt;			//HJM time-step length
	ddelt = (FTYPE) (dYears/iN);

	FTYPE *pdexpRes;
	pdexpRes = dvector(0,(iN-1)*BLOCKSIZE-1);
	//precompute the exponientials
	for (j=0; j<=(iN-1)*BLOCKSIZE-1; ++j){ pdexpRes[j] = -pdRatePath[j]*ddelt; }
	for (j=0; j<=(iN-1)*BLOCKSIZE-1; ++j){ pdexpRes[j] = exp(pdexpRes[j]);  }
	

	//initializing the discount factor vector
	for (i=0; i<(iN)*BLOCKSIZE; ++i)
	  pdDiscountFactors[i] = 1.0;

	for (i=1; i<=iN-1; ++i){
	  //printf("\nVisiting timestep %d : ",i);
	  for (b=0; b<BLOCKSIZE; b++){
	    //printf("\n");
	    for (j=0; j<=i-1; ++j){
	      pdDiscountFactors[i*BLOCKSIZE + b] *= pdexpRes[j*BLOCKSIZE + b];
	      //printf("(%f) ",pdexpRes[j*BLOCKSIZE + b]);
	    }
	  } // end Block loop
	} 

	free_dvector(pdexpRes, 0,(iN-1)*BLOCKSIZE-1);
	iSuccess = 1;
	return iSuccess;
}



