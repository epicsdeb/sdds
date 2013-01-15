/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: not supported by cvs2svn $
 Revision 1.10  2011/02/21 16:06:57  shang
 changed the type of returnCode argument in interp_short to "unsigned long" to be consistent with interp()

 Revision 1.9  2011/02/17 20:51:10  shang
 added setting returnCode to zero when it finds the low position, otherwise, it gives error which is not true.

 Revision 1.8  2010/11/29 23:22:16  shang
 improved interp_short to return the corresponding function value if the independent value is found in the input array.

 Revision 1.7  2010/06/23 18:48:03  shang
 added interp_short routine

 Revision 1.6  2003/10/10 18:16:04  borland
 Replaced SDDS_Bomb() with bomb() so that libraries are not interdependent.

 Revision 1.5  2003/10/07 16:22:56  shang
 moved the interpolate() from sddsinterp.c to interp.c

 Revision 1.4  2002/08/14 16:18:56  soliday
 Added Open License

 Revision 1.3  1999/05/28 14:58:01  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:20:07  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: interp()
 * purpose: do simple linear interpolation of data
 *
 * Michael Borland, 1987, 1990, 1995
 * Louis Emery, 1990
 */
#include "mdb.h"

#define MIN(x,y) ( ((x)>(y)) ? (y) : (x))
#define MAX(x,y) ( ((x)<(y)) ? (y) : (x))


double interp(double *f, double *x, long n, double xo, long warnings, long order, long *returnCode)
{
    long hi, lo, mid, offset;

    lo = 0;
    hi = n-1;
    if (lo==hi) {
        if (warnings)
            printf("warning: only one point--returning value for that point\n");
        *returnCode = 0;
        return(f[0]);
        }
    if (x[lo]<x[hi]) {/* indep variable ordered from small to large */    
        if (xo<x[lo=0]) {
            if (warnings)
                printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
                    xo, x[0], x[n-1]);
            *returnCode = 0;
            return(f[lo]);
            }
        if (xo>x[hi=n-1]) {
            if (warnings)
                printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
                    xo, x[0], x[n-1]);
            *returnCode = 0;
            return(f[hi]);
            }

        /* do binary search for closest point */
        while ((hi-lo)>1) {
            mid = (lo+hi)/2;
            if (xo<x[mid]) 
                hi = mid;
            else lo = mid;
            }
        }
    else {/* indep variable ordered from large to small */
        if (xo>x[lo=0]) {
            if (warnings)
                printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
                    xo, x[n-1], x[0]);
            *returnCode = 0;
            return(f[lo]);
            }
        if (xo<x[hi=n-1]) {
            if (warnings)
                printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
                    xo, x[n-1], x[0]);
            *returnCode = 0;
            return(f[hi]);
            }

        /* do binary search for closest point */
        while ((hi-lo)>1) {
            mid = (lo+hi)/2;
            if (xo>x[mid]) 
                hi = mid;
            else lo = mid;
            }
        }
    /* L.Emery's contribution */
    offset = lo - (order-1)/2;   /* offset centers the argument in the set of points. */
    offset = MAX( offset, 0 );
    offset = MIN( offset, n-order-1); 
    offset = MAX( offset, 0 );
    return LagrangeInterp(x+offset, f+offset, order+1, xo, returnCode);
    }

/* routine: LagrangeInterp()
 * purpose: perform Lagrange interpolation of data.
 *          uses "order1" points starting with the first in the array
 *
 * M. Borland, 1995
 */

double LagrangeInterp(double *x, double *f, long order1, double x0, long *returnCode)
{
  long i, j;
  double denom, numer, sum;
  
  for (i=sum=0; i<order1; i++) {
    denom = 1;
    numer = 1;
    for (j=0; j<order1; j++) {
      if (i!=j) {
        denom *= (x[i] - x[j]);
        numer *= (x0   - x[j]);
        if (numer==0) {
          *returnCode = 1;
          return f[j];
        }
      }
    }
    if (denom==0) {
      *returnCode = 0;
      return 0.0;
    }
    sum += f[i]*numer/denom;
  }
  *returnCode = 1;
  return sum;
}


double interpolate(double *f, double *x, long n, double xo, OUTRANGE_CONTROL *belowRange, 
                   OUTRANGE_CONTROL *aboveRange, long order, unsigned long *returnCode, long M)
{
  long hi, lo, mid, offset, code;
  double result,below,above;
  
  *returnCode = 0;
  
  lo = 0;
  hi = n-1;
  if (M>0) {
    above=f[n-1];
    below=f[0];
  } else {
    above=f[0];
    below=f[n-1];
  }
  if ((xo*M>x[hi]*M && M>0) || (xo*M<x[lo]*M && M<0)) {
    if (aboveRange->flags&OUTRANGE_SKIP) {
      *returnCode = OUTRANGE_SKIP;
      return above;
    }
    else if (aboveRange->flags&OUTRANGE_ABORT) {
      *returnCode = OUTRANGE_ABORT;
      return above;
    }
    else if (aboveRange->flags&OUTRANGE_WARN)
      *returnCode = OUTRANGE_WARN;
    if (aboveRange->flags&OUTRANGE_VALUE) {
      *returnCode |= OUTRANGE_VALUE;
      return aboveRange->value;
    }
    if (aboveRange->flags&OUTRANGE_WRAP) {
      double delta;
      *returnCode |= OUTRANGE_WRAP;
      if ((delta = x[hi]-x[lo])==0) 
        return f[0];
      while (xo*M>x[hi]*M)
        xo -= delta;
    }
    else if (aboveRange->flags&OUTRANGE_SATURATE || !(aboveRange->flags&OUTRANGE_EXTRAPOLATE)) {
      *returnCode |= OUTRANGE_SATURATE;
      return above;
    }
  }
  if ((xo*M<x[lo]*M && M>0) || (xo*M>x[hi]*M && M<0)) {
    if (belowRange->flags&OUTRANGE_SKIP) {
      *returnCode = OUTRANGE_SKIP;
      return below;
    }
    else if (belowRange->flags&OUTRANGE_ABORT) {
      *returnCode = OUTRANGE_ABORT;
      return below;
    }
    else if (belowRange->flags&OUTRANGE_WARN)
      *returnCode = OUTRANGE_WARN;
    if (belowRange->flags&OUTRANGE_VALUE) {
      *returnCode |= OUTRANGE_VALUE;
      return belowRange->value;
    }
    if (belowRange->flags&OUTRANGE_WRAP) {
      double delta;
      *returnCode |= OUTRANGE_WRAP;
      if ((delta = x[hi]-x[lo])==0) 
        return below;
      while (xo*M<x[lo]*M)
        xo += delta;
    }
    else if (belowRange->flags&OUTRANGE_SATURATE || !(belowRange->flags&OUTRANGE_EXTRAPOLATE)) {
      *returnCode |= OUTRANGE_SATURATE;
      return below;
    }
  }
  
  if (lo==hi) {
    if (xo==x[lo]) {
      if (aboveRange->flags&OUTRANGE_WARN || belowRange->flags&OUTRANGE_WARN)
        *returnCode = OUTRANGE_WARN;
    }
    return f[0];
  }
  
  lo = 0;
  hi = n - 1;
  if (xo*M<x[0]*M)
    hi = 1;
  else if (xo*M>x[n-1]*M)
    lo = hi-1;
  else {
    /* do binary search for closest point */
    while ((hi-lo)>1) {
      mid = (lo+hi)/2;
      if (xo*M<x[mid]*M) 
        hi = mid;
      else lo = mid;
    }
  }
  
  /* L.Emery's contribution */
  if (order>n-1)
    order = n-1;
  offset = lo - (order-1)/2;   /* offset centers the argument in the set of points. */
  offset = MAX( offset, 0 );
  offset = MIN( offset, n-order-1); 
  result =  LagrangeInterp(x+offset, f+offset, order+1, xo, &code);
  if (!code)
    bomb("zero denominator in LagrangeInterp", NULL);
  return result;
}

short interp_short(short *f, double *x, long n, double xo, long warnings, short order, 
                unsigned long *returnCode, long *next_start_pos)
{
  long hi, lo, mid, i;
  short value;
  
  lo = 0;
  hi = n-1;

  *returnCode = 0;
  /*if the interpolate point is less or equal to the start point, return the value of the start point */
  if (xo<=x[0]) {
    *next_start_pos=0;
    return f[0];
  }
  
  /*if the value is in one of the indepent, return the corresponding f value */
  for (i=0; i<n; i++) 
    if (xo==x[i]) {
      *next_start_pos=i;
      return f[i];
    }
  if (lo==hi) {
    if (warnings)
      printf("warning: only one point--returning value for that point\n");
    *returnCode = 0;
    *next_start_pos = lo;
    return(f[0]);
  }
  if (x[lo]<x[hi]) {/* indep variable ordered from small to large */    
    if (xo<x[lo=0]) {
      if (warnings)
        printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
               xo, x[0], x[n-1]);
      *returnCode = 0;
      *next_start_pos = lo;
      return(f[lo]);
    }
    if (xo>x[hi=n-1]) {
      if (warnings)
        printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
               xo, x[0], x[n-1]);
      *returnCode = 0;
      *next_start_pos = hi;
      return(f[hi]);
    }
    
    /* do binary search for closest point */
    while ((hi-lo)>1) {
      mid = (lo+hi)/2;
      if (xo<x[mid]) 
        hi = mid;
      else lo = mid;
    }
  }
  else {/* indep variable ordered from large to small */
    if (xo>x[lo=0]) {
      if (warnings)
        printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
               xo, x[n-1], x[0]);
      *returnCode = 0;
      *next_start_pos = lo;
      return(f[lo]);
    }
    if (xo<x[hi=n-1]) {
      if (warnings)
        printf("warning: %22.15e outside [%22.15e,%22.15e] (interp)\n",
               xo, x[n-1], x[0]);
      *returnCode = 0;
      *next_start_pos = hi;
      return(f[hi]);
    }
    
    /* do binary search for closest point */
    while ((hi-lo)>1) {
      mid = (lo+hi)/2;
      if (xo>x[mid]) 
        hi = mid;
      else lo = mid;
    }
  }
  /*remember this position so that one can start from her to speed up the interp of
    other points assume the interping points are sorted.*/
  *next_start_pos = lo;
  *returnCode =0;
  if (order==-1) {
    /* inherit value from previous point*/
    value=(short)f[lo];
  }
  else if (order==-2) {
    /* inherit value from next point */
    value=(short)f[hi];
  } 
  else {
    value = (f[hi]-f[lo])/(xo-x[lo]);
  }
  return value;
}
