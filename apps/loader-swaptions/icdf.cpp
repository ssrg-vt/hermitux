#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HJM_type.h"




void icdf_baseline(const int N, FTYPE *in, FTYPE *out){
      
  register FTYPE z, r;
  
 const FTYPE
    a1 = -3.969683028665376e+01,
    a2 =  2.209460984245205e+02,
    a3 = -2.759285104469687e+02,
    a4 =  1.383577518672690e+02,
    a5 = -3.066479806614716e+01,
    a6 =  2.506628277459239e+00;
    
  const FTYPE
    b1 = -5.447609879822406e+01,
    b2 =  1.615858368580409e+02,
    b3 = -1.556989798598866e+02,
    b4 =  6.680131188771972e+01,
    b5 = -1.328068155288572e+01;
    
   const FTYPE
    c1 = -7.784894002430293e-03,
    c2 = -3.223964580411365e-01,
    c3 = -2.400758277161838e+00,
    c4 = -2.549732539343734e+00,
    c5 =  4.374664141464968e+00,
    c6 =  2.938163982698783e+00;
    
  const FTYPE
    //d0 =  0.0,
    d1 =  7.784695709041462e-03, 
    d2 =  3.224671290700398e-01, 
    d3 =  2.445134137142996e+00, 
    d4 =  3.754408661907416e+00; 

  // Limits of the approximation region. 
#define U_LOW 0.02425 
 
  const FTYPE u_low   = U_LOW, u_high  = 1.0 - U_LOW; 
  
  for(int i=0; i<N; i++){
    FTYPE u = in[i];
    // Rational approximation for the lower region. ( 0 < u < u_low )
    if( u < u_low ){
      z = sqrt(-2.0*log(u));
      z = (((((c1*z+c2)*z+c3)*z+c4)*z+c5)*z+c6) / ((((d1*z+d2)*z+d3)*z+d4)*z+1.0);
    }
    // Rational approximation for the central region. ( u_low <= u <= u_high )
    else if( u <= u_high ){
      z = u - 0.5;
      r = z*z;
      z = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*z / (((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1.0);
    }
    // Rational approximation for the upper region. ( u_high < u < 1 )
    else {
      z = sqrt(-2.0*log(1.0-u));
      z = -(((((c1*z+c2)*z+c3)*z+c4)*z+c5)*z+c6) /  ((((d1*z+d2)*z+d3)*z+d4)*z+1.0);
    }    
    out[i] = z;
  }
  return;
}

