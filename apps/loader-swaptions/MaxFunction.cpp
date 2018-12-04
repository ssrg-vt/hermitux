// MaxFunction.cpp
// Author: Mark Broadie

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "HJM_type.h"

FTYPE dMax( FTYPE dA, FTYPE dB );

FTYPE dMax( FTYPE dA, FTYPE dB )
{
  return (dA>dB ? dA:dB);
} // end of dMax
