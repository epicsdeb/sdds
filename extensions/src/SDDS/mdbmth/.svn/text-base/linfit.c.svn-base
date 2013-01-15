/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: linfit.c 
 * contents: unweightedLinearFit()
 * purpose: simple linear fit
 * M. Borland 1995.
 $Log: not supported by cvs2svn $
 Revision 1.4  2002/08/14 16:18:57  soliday
 Added Open License

 Revision 1.3  1999/05/28 14:59:21  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:20:17  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

long unweightedLinearFit(double *xData, double *yData, long nData, 
                         double *slope, double *intercept, double *variance)
{
  return unweightedLinearFitSelect(xData, yData, NULL, nData, slope, intercept, variance);
}


long unweightedLinearFitSelect(double *xData, double *yData, short *select, long nData, 
                               double *slope, double *intercept, double *variance
                               )
{
  long i;
  double x, y, sum_x2, sum_x, sum_xy, sum_y, D, residual1, nUsed;

  if (nData<2)
    return 0;

  /* compute linear fit and associated parameters */
  /* linear fit to y = a + bx:
     a = (S x^2 Sy - S x S xy)/D
     b = (N S xy  - Sx Sy)/D
     D = N S x^2 - (S x)^2
     */
  sum_x = sum_x2 = sum_y = sum_xy = 0;
  nUsed = 0;
  for (i=0; i<nData; i++) {
    if (!select || select[i]) {
      nUsed ++;
      sum_x  += (x=xData[i]);
      sum_x2 += x*x;
      sum_y  += (y=yData[i]);
      sum_xy += x*y;
    }
  }
  if (nUsed<2)
    return 0;
  
  if ((D = nUsed*sum_x2 - sum_x*sum_x)) {
    *slope = (nUsed*sum_xy - sum_x*sum_y)/D;
    *intercept = (sum_x2*sum_y - sum_x*sum_xy)/D;
    *variance = 0;
    for (i=0; i<nData; i++) {
      if (!select || select[i]) {
        residual1 = (yData[i] - (xData[i]*(*slope) + (*intercept)));
        *variance += residual1*residual1;
      }
    }
    if (nUsed>2)
      *variance /= (nUsed-2);
    else 
      *variance = DBL_MAX;
    return 1;
  }
  else
    return 0;
}


