#ifndef __HJM_SECURITIES__
#define __HJM_SECURITIES__

#include "HJM_type.h"

int HJM_SimPath_Yield(FTYPE **ppdHJMPath, int iN, int iFactors, FTYPE dYears, FTYPE *pdYield, FTYPE **ppdFactors);
int HJM_SimPath_Forward(FTYPE **ppdHJMPath, int iN, int iFactors, FTYPE dYears, FTYPE *pdForward, FTYPE *pdTotalDrift,
						FTYPE **ppdFactors, long *iSeed);
int HJM_Yield_to_Forward(FTYPE *pdForward, int iN, FTYPE *pdYield);
int HJM_Factors(FTYPE **ppdFactors,int iN, int iFactors, FTYPE *pdVol, FTYPE **ppdFacBreak);
int HJM_Drifts(FTYPE *pdTotalDrift, FTYPE **ppdDrifts, int iN, int iFactors, FTYPE dYears, FTYPE **ppdFactors);
int HJM_Correlations(FTYPE **ppdHJMCorr, int iN, int iFactors, FTYPE **ppdFactors);
int HJM_Forward_to_Yield(FTYPE *pdYield, int iN, FTYPE *pdForward);
int Discount_Factors(FTYPE *pdDiscountFactors, int iN, FTYPE dYears, FTYPE *pdRatePath);
FTYPE dMax( FTYPE dA, FTYPE dB );

#endif //__HJM_SECURITIES__
