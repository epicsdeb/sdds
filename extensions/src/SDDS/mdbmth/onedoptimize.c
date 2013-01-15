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
 Revision 1.6  2004/04/07 02:01:10  borland
 OneDParabolicOptimization now keeps track of the best point and is guaranteed
 to return it.

 Revision 1.5  2004/01/24 18:18:07  borland
 Changed the meaning of the nEvalMax option, which was not actually used.
 It is now used as the maximum number of steps to take in one direction
 when the value is found to improve.

 Revision 1.4  2003/08/28 19:56:28  soliday
 Cleaned up some of the code.

 Revision 1.3  2002/08/14 16:18:59  soliday
 Added Open License

 Revision 1.2  2002/02/26 03:12:22  borland
 Added OneDParabolicOptimization, which used to reside in sddsnaff.c

 Revision 1.1  2001/07/23 16:38:47  borland
 New procedure by H. Shang.

*/
#include "mdb.h"

#define DEFAULT_MAXEVALS 100
#define DEFAULT_MAXPASSES 5

long checkVariableLimits(double *x, double *xlo, double *xhi, long n);

long OneDScanOptimize (
                       double *yReturn, 
                       double *xGuess,
                       double *dxGuess,
                       double *xLowerLimit,
                       double *xUpperLimit,
                       short *disable,
                       long dimensions, 
                       double target,              /* will return if any value is <= this */
                       double tolerance,           /* <0 means fractional, >0 means absolute */
                       double (*func)(double *x, long *invalid), 
                       void (*report)(double ymin, double *xmin, long pass, long evals, long dims),
                       long maxSteps,   /* number of steps to try in good direction */
                       long maxDivisions,
                       long maxRepeats,
                       unsigned long flags)
{
  static double *dxLocal = NULL;
  static long lastDimensions = 0;
  double yLast, dVector = 1, yNew,xLocal;
  long point, totalEvaluations = 0, isInvalid, repeats=0,divisions;
  long activeDimensions, direction, found=0, stepsTaken, decreaseSeen;
  double *divisor, *minimum, min; 

  min=1e9;
  if (dimensions<=0)
    return(-3);
  if (disable) {
    activeDimensions = 0;
    for (direction=0; direction<dimensions; direction++)
      if (!disable[direction])
        activeDimensions++;
  }
  else 
    activeDimensions = dimensions;
  if (activeDimensions<=0)
    return -3;

  if (activeDimensions>lastDimensions) {
    if (dxLocal)
      free(dxLocal);
    dxLocal = tmalloc(sizeof(*dxLocal)*activeDimensions);
    lastDimensions = activeDimensions;
  }
  divisor=(double*)malloc(sizeof(*divisor)*activeDimensions);
  minimum=(double*)malloc(sizeof(*minimum)*activeDimensions);
  for (point=0;point<activeDimensions;point++)
    minimum[point]=target; /*important!!!!*/
  if (!dxGuess) {
    dxGuess = dxLocal;
    for (direction=0; direction<dimensions; direction++)
      dxGuess[direction] = 0;
  }
  else {
    for (direction=0; direction<dimensions; direction++)
      dxLocal[direction]=dxGuess[direction];
  }
  
  for (direction=0; direction<dimensions; direction++) {
    if (dxGuess[direction]==0) {
      if (xLowerLimit && xUpperLimit)
        dxGuess[direction] = (xUpperLimit[direction]-xLowerLimit[direction])/4;
      else if ((dxGuess[direction] = xGuess[direction]/4)==0)
        dxGuess[direction] = 1;
    }
    if (xLowerLimit && xUpperLimit) {
      if ((dVector = fabs(xUpperLimit[direction]-xLowerLimit[direction])/4)
          <fabs(dxGuess[direction]))
        dxGuess[direction] = dVector;
    }
    if (disable && disable[direction])
      dxGuess[direction] = 0;
  }
  
  if (xLowerLimit) {   
    /* if start is at lower limit, make sure initial step is positive */
    for (direction=0; direction<dimensions; direction++) 
      if (xLowerLimit[direction]>=xGuess[direction])
        dxGuess[direction] = fabs(dxGuess[direction]);
  }
  if (xUpperLimit) {
    /* if start is at upper limit, make sure initial step is negative */
    for (direction=0; direction<dimensions; direction++) 
      if (xUpperLimit[direction]<=xGuess[direction])
        dxGuess[direction] = -fabs(dxGuess[direction]);
  }
#if DEBUG
  for (direction=0; direction<dimensions; direction++) 
    fprintf(stderr, "direction %ld: guess=%le delta=%le disable=%hd\n",
            direction, xGuess[direction], dxGuess[direction], 
            disable?disable[direction]:(short)0);
#endif

  if (maxRepeats<=0)
    maxRepeats = DEFAULT_MAXPASSES;
  yLast = (*func)(xGuess, &isInvalid);
  totalEvaluations++;
  if (isInvalid) {
    fprintf(stderr, "error: initial guess is invalid in simplexMin()\n");
    return(-3);
  }
  if ( yLast <=target) {
    *yReturn=yLast;
    if (report)
      (*report)(*yReturn, xGuess, repeats, totalEvaluations, dimensions);
    return(totalEvaluations);
  }
  for (point=0;point<activeDimensions;point++) 
    divisor[point]=1;
  while (repeats<maxRepeats) {
    repeats++;
    for (point=0;point<activeDimensions;point++) {
      /* fprintf(stderr,"scan the %ldth variable\n",point+1); */
      divisions = 0;
      found=0;
      xLocal=xGuess[point];
      while (divisions<maxDivisions) {
        stepsTaken = 0;
        decreaseSeen = 0;
        while (stepsTaken<maxSteps) {
          xGuess[point]=xGuess[point]+dxGuess[point]/divisor[point];
          if ((xLowerLimit || xUpperLimit) && 
              !checkVariableLimits(xGuess, xLowerLimit, xUpperLimit, dimensions)) {
#if DEBUG
            long idum;
            fprintf(stderr, " Point outside of bounds:\n"); 
            for (idum=0; idum<dimensions; idum++)
              fprintf(stderr, "    %le  %le, %le\n", xGuess[idum],
                      xLowerLimit[idum], xUpperLimit[idum]);
#endif
            yNew = yLast*1e9;
            /* breaks us out of this loop */
            stepsTaken = maxSteps;
          }
          else {
            /* measure the function value */
            yNew = (*func)(xGuess, &isInvalid);
            totalEvaluations++;
            if (isInvalid) {
#if DEBUG
              fprintf(stderr, " Point is invalid\n");
#endif
              yNew = yLast*1e9;
            }
            if (yNew <= target) { 
              /* if the value reaches target, success */
              if (yNew <=yLast) 
                *yReturn=yNew;
              else {
                *yReturn=yLast;
                xGuess[point]=xLocal;
              }
              if (report)
                (*report)(*yReturn, xGuess, repeats, totalEvaluations, dimensions);
              return(totalEvaluations); 
            } 
            if (fabs(yNew-yLast)<=tolerance) { 
              /* if the difference reaches tolerance, reach the local minimum */
              if (yNew <=yLast)
                minimum[point]=yNew;
              else {
                minimum[point]=yLast;
                xGuess[point]=xLocal;
              }
              found = 1;
              /* fprintf(stderr,"min[%ld]=%e\n",point,minimum[point]); */
              if (fabs(minimum[point]-min)<=tolerance) {
                /* if the difference of two minimums reaches tolerance, success */
                *yReturn=minimum[point];
                if (report)
                  (*report)(*yReturn, xGuess, repeats, totalEvaluations, dimensions);
                return(totalEvaluations); 
              }
              else 
                break;
            }
            if (yNew<=yLast) { 
              /*decrease found*/
#ifdef DEBUG
              fprintf(stderr,"decrease found, yNew=%le \n",yNew); 
#endif
              yLast = yNew;
              xLocal = xGuess[point];
              decreaseSeen = 1;
            } else {
              /* new value is greater than the old, set the PVs back 
               * and break out of this loop.  Will result (normally)
               * in re-entering the loop with opposite sign on step size
               */
              xGuess[point] = xLocal; 
              stepsTaken = maxSteps;
#ifdef DEBUG
              fprintf(stderr, "Increase seen: setting back to %e\n", xLocal);
#endif
              if (flags&ONEDSCANOPTIMIZE_REFRESH) {
                yLast = (*func)(xGuess, &isInvalid);/*read the measurements again */
                totalEvaluations++;
              }
              if (isInvalid) {
                fprintf(stderr, "error: initial guess is invalid in OneDScanOptimize()\n");
                return(-3);
              }
            }
          } /* end of branch for trial points that are within limits */
          stepsTaken++;
        } /* end of loop over allowed steps in one direction */
        divisions++;
        if ((divisions%2))
          /* reverse directions */
          divisor[point] *= decreaseSeen?-2:-1;
        else
          /* decrease the step size */
          divisor[point] *= 3 ;
      } /* end of "while  (divisions<maxDivisions)" */
      dxGuess[point] /= 2; /*decrease the delta by 2 for next cycle */
    }   /*end of "for (point=0;point<activeDimensions;point++)" */
    if (found) {
      if (minimum[activeDimensions-1]<min)
        min=minimum[activeDimensions-1];
    /*  fprintf(stderr,"min=%e\n",min); */
    }
  } /*end of "while (repeats<maxRepeats)" */
  *yReturn=yLast;
  if (*yReturn <=target) {
    if (report)
      (*report)(*yReturn, xGuess, repeats, totalEvaluations, dimensions);
    return(totalEvaluations); 
  }
  else {
    if (report)
      (*report)(*yReturn, xGuess, repeats, totalEvaluations, dimensions);
   /* fprintf(stderr,"The minimum (may not be the best) is %le after %ld cycles of 1dscan.\n",
            *yReturn, repeats); */
    return 0;
  }
}


/* 
 * optimize (e.g., minimize) a function of 1 variable using parabolic interpolation 
 *
 * M. Borland, ANL/APS, 2001
 */

long OneDParabolicOptimization(double *yReturn, double *xGuess, double dx,
			       double xLower, double xUpper, 
			       double (*func)(double x, long *invalid),
			       long maxCycles, double dxLimit,
			       double tolerance, long maximize)
{
  double maxFactor, fBest, xBest;
  long invalid, cycle;
  double x0, x1=0, x2, f0, f1=0, f2;

  maxFactor = maximize?-1:1;
  invalid = 0;

  x0 = *xGuess;
  f0 = maxFactor*(*func)(x0, &invalid);
  xBest = x0;
  fBest = f0;

  *yReturn = maxFactor*f0;
  if (invalid)
    return -1;

  /* find direction in which function decreases */
  for (cycle=0; cycle<2*maxCycles; cycle++) {
    x1 = x0 + dx;
    if (x1==x0)
      break;
    if (x1>xUpper || x1<xLower)
      return -2;
    f1 = maxFactor*(*func)(x1, &invalid);
    if (invalid)
      return -1;
#ifdef DEBUG
    fprintf(stderr, "cycle = %ld, maxCycles=%ld, f1 = %21.15e, fBest = %21.15e\n", 
	    cycle, 2*maxCycles, f1, fBest);
#endif
    if (f1<fBest) {
#ifdef DEBUG
      fprintf(stderr, "f1<fBest\n");
#endif
      fBest = f1;
      xBest = x1;
    }
    if (f1<f0) {
#ifdef DEBUG
      fprintf(stderr, "f1<f0, breaking\n");
#endif
      break;
    }
    dx = dx*(cycle%2==0 ? -1 : -0.5);
  }
  if (x1==x0)
    return 1;
  if (cycle==2*maxCycles) {
    if (fabs(dx)<dxLimit) 
      return 1;
    return -3;
  }
#ifdef DEBUG
  fprintf(stderr, "Function decreases with dx=%e, f0=%21.15e, f1=%21.15e, cycle=%ld\n", dx, f0, f1, cycle);
#endif

  /* take steps until passing through minimum */
  while (1) {
    x2 = x1 + dx;
    if (x2>xUpper || x2<xLower)
      return -4;
    f2 = maxFactor*(*func)(x2, &invalid);
    if (invalid)
      return -1;
    if (f2<fBest) {
      fBest = f2;
      xBest = x2;
    }
#ifdef DEBUG
    fprintf(stderr, "fBest = %21.15e, f1 = %21.15e, f2 = %21.15e\n",
	    fBest, f1, f2);
#endif
    if (f2>f1) 
      break;
    if (x1==x2)
      break;
    x0 = x1; f0 = f1;
    x1 = x2; f1 = f2;
  }
  if (x0>x2) {
    /* arrange in increasing order */
    SWAP_DOUBLE(x0, x2);
    SWAP_DOUBLE(f0, f2);
  }

  /* now f0 > f1 and f2 > f1 */
  for (cycle=0; cycle<maxCycles; cycle++) {
    double numer, denom, x3, f3, scale;
    long failed;
#ifdef DEBUG
    fprintf(stderr, "Cycle %ld:  f(%e)=%e,  f(%e)=%e,  f(%e)=%e\n",
	    cycle, x0, f0, x1, f1, x2, f2);
#endif

    if (x2==x0 || (x2-x0)<dxLimit || (MAX(f2,f0)-f1)<tolerance)
      break;
    /* try parabolic interpolation */
    numer = sqr(x1-x0)*(f1-f2) - sqr(x1-x2)*(f1-f0);
    denom =    (x1-x0)*(f1-f2) -    (x1-x2)*(f1-f0);
    x3 = x1 - numer/denom/2.0;
    failed = 1;
    scale = x2-x0;
#ifdef DEBUG
    fprintf(stderr,"parabolic parameters: x3 = %e, f3 = %e, scale=%e, x0=%e, x2=%e\n", 
            x3, 
            isinf(x3)?DBL_MAX:maxFactor*(*func)(x3, &invalid), scale, x0, x2);
#endif
    if (!isinf(x3) && x0<x3 && x3<x2 && 
	fabs(x3-x0)>1e-6*scale && fabs(x3-x1)>1e-6*scale &&
	fabs(x3-x2)>1e-6*scale) {
      /* evaluate at parabolic interpolation point */
      failed = 0;
      f3 = maxFactor*(*func)(x3, &invalid);
      if (invalid)
	failed = 1;
      else {
	if (f3<fBest) {
	  fBest = f3;
	  xBest = x3;
	}
        if (f3<f1) {
          /* replace point 1 */
          f1 = f3;
          x1 = x3;
        } else if (f2>f0 && f3<f2) {
	  /* replace point 2 */
	  f2 = f3;
	  x2 = x3;
	  if (x2<x1) {
	    SWAP_DOUBLE(x1, x2);
	    SWAP_DOUBLE(f1, f2);
	  }
	} else if (f2<f0 && f3<f0) {
	  /* replace point 0 */
	  f0 = f3;
	  x0 = x3;
	  if (x0>x1) {
	    SWAP_DOUBLE(x0, x1);
	    SWAP_DOUBLE(f0, f1);
	  }
	} else 
	  failed = 1;
      }
    }
#ifdef DEBUG
    if (!failed) 
      fprintf(stderr, "Parabolic interpolation succeeded\n");
#endif
    if (failed) {
      long right, other;
      for (other=0; other<2; other++) {
	/* try dividing one of the intervals */
	if (fabs(x0-x1)<fabs(x1-x2)) {
	  if (!other) {
	    x3 = (x1+x2)/2;
	    right = 1;
	  } else {
	    x3 = (x0+x1)/2;
	    right = 0;
	  }
	} else {
	  if (!other) {
	    x3 = (x0+x1)/2;
	    right = 0;
	  } else {
	    x3 = (x1+x2)/2;
	    right = 1;
	  }
	}
	f3 = maxFactor*(*func)(x3, &invalid);
	if (invalid)
	  return -1;
	if (f3<fBest) {
	  fBest = f3;
	  xBest = x3;
	}
#ifdef DEBUG
	fprintf(stderr, "f3 = %e at x3=%e\n", f3, x3);
#endif
	if (f3<f1) {
#ifdef DEBUG
	  fprintf(stderr, "Replacing point 1\n");
#endif
	  f1 = f3;
	  x1 = x3;
	  break;
	}
	if (right && f3<f2) {
	  /* replace point 2 */
#ifdef DEBUG
	  fprintf(stderr, "Replacing point 2\n");
#endif
	  f2 = f3;
	  x2 = x3;
	  if (x2<x1) {
	    SWAP_DOUBLE(x1, x2);
	    SWAP_DOUBLE(f1, f2);
	  }
	  break;
	} else if (!right && f3<f0) {
	  /* replace point 0 */
#ifdef DEBUG
	  fprintf(stderr, "Replacing point 0\n");
#endif
	  f0 = f3;
	  x0 = x3;
	  if (x0>x1) {
	    SWAP_DOUBLE(x0, x1);
	    SWAP_DOUBLE(f0, f1);
	  }
	  break;
	}
      }
#ifdef DEBUG
      fprintf(stderr, "Sectioning succeeded\n");
#endif
    }
  }
#ifdef DEBUG
  fprintf(stderr, "Returning: x=%21.15e, y=%21.15e\n",
	  xBest, maxFactor*fBest);
#endif
  *yReturn = maxFactor*fBest;
  *xGuess = xBest;
  return 1;
}
