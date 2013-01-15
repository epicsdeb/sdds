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
 Revision 1.28  2003/01/21 18:55:05  borland
 simplexMin() has another argument: a factor by which to multiply the
 parameter ranges at the end of a pass to get the initial step sizes for
 the next pass.  A value of 1.0 reproduces the old behavior.

 Revision 1.27  2003/01/15 22:59:17  borland
 Added SIMPLEX_START_FROM_VERTEX1 flag for simplexMin().
 Added simplexDivisor argument for simplexMin(); default value is 3
 to reproduce old behavior.

 Revision 1.26  2002/08/14 16:19:00  soliday
 Added Open License

 Revision 1.25  2002/07/13 21:05:30  borland
 Added optional randomization of signs for the initial steps.

 Revision 1.24  2002/05/08 17:16:00  shang
 replaced the return value (or function value) by DBL_MAX in the case of
 out of limits and other invalid calculations.

 Revision 1.23  2002/03/30 16:31:43  borland
 Fixed logic around a debugging statement in simplexMinAbort().

 Revision 1.22  2002/02/22 22:41:00  borland
 Fixed potential uninitialized variable problem.

 Revision 1.21  2002/02/18 17:42:48  borland
 Added external abort feature and fixed a bug that sometimes resulted in
 an infinite loop.

 Revision 1.20  2002/01/07 22:18:42  borland
 Removed some spurious characters after #endif's.

 Revision 1.19  2002/01/02 22:08:33  borland
 Fixed problem with UMR related to usedLast variable.

 Revision 1.18  2001/07/23 16:36:08  borland
 Added maxDivisions argument and changed the divisor for 1d prescan from
 10 to 3.

 
 Revision 1.16  2001/05/15 16:39:13  borland
 Added flags argument to simplexMin().  At present, this permits disabling
 the 1D scans that are normally done prior to the actual simplex routine.

 Revision 1.15  2000/03/27 20:30:31  borland
 Error message now goes to stderr rather than stdout.

 Revision 1.14  1999/08/11 19:56:41  borland
 Added more checks for results below the target.  Should check more
 frequently in simplexMinimization, also.

 Revision 1.13  1999/06/12 19:12:54  borland
 Turned off debugging.

 Revision 1.12  1999/06/12 19:12:41  borland
 Inserted a test to break out of the simplex loop if no progress is made.
 Prevents an infinite loop when one of the variables hits a limit.

 Revision 1.11  1999/05/28 14:57:49  soliday
 Removed compiler warnings under Linux.

 Revision 1.10  1999/02/11 20:45:02  borland
 Now calls report function when terminating due to reaching the target.

 Revision 1.9  1999/01/21 15:03:02  borland
 Now chooses initial step sizes more carefully to avoid getting into unproductive
 loops.  Problem occurred when user gave step sizes that were >1/2 the distance
 from lower to upper limit.

 Revision 1.8  1999/01/07 21:52:59  borland
 Turned off debugging statements.

 Revision 1.7  1999/01/07 21:45:20  borland
 Added capability of removing some variables from the optimization, without
 the need to change the function being optimized.

 Revision 1.6  1999/01/05 19:38:52  borland
 Fixed bug in checks of boundaries of variables.

 Revision 1.5  1998/12/23 19:50:08  borland
 Improved method of finding initial simplex---now uses a series of 1-d
 scans to find a direction of improvement in each dimension; the
 initialization is done again at the start of each pass.  Method of
 implementing limits on variables was changed; limits are no longer
 perfectly hard-edged.  Fixed a bug in simplexMinimization that resulted
 in the wrong vector being returned as the optimum.

 Revision 1.4  1998/11/16 22:16:16  borland
 Fixed bug that resulted in infinite looping inside simplexMinimization.

 Revision 1.3  1998/11/12 17:44:54  borland
 Commented-out a warning message about too many iterations.

 * Revision 1.2  1995/09/05  21:20:52  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: simplex.c
 * contents: 
 *   simplexMin               convenient top-level routine 
 *   simplexMinimization      primarily used by simplexMin
 *   computeSimplexCenter     used by simplexMinimization
 *   trialSimplex             used by simplexMinimization
 *   enforceVariableLimits    used by simplexMin and simplexMinimization
 *
 * M.Borland, 1991, 1993, 1995.
 * Based partly on routines in Numerical Recipes in C.
 */
#include "mdb.h"
#include <time.h>

void enforceVariableLimits(double *x, double *xlo, double *xhi, long n);
long simplexMinimization(double **simplexVector, double *fValue, double *coordLowerLimit,
			 double *coordUpperLimit, short *disable, long dimensions, long activeDimensions, 
			 double target, double tolerance, long tolerance_mode,
			 double (*function)(double *x, long *invalid), long maxEvaluations, long *evaluations,
			 unsigned long flags);
void computeSimplexCenter(double *center, double **vector, long dimensions, long activeDimensions);
double trialSimplex(double **simplexVector, double *funcValue, double *simplexCenter, 
                    double *coordLowerLimit, double *coordUpperLimit, short *disable,
                    long dimensions, long activeDimensions, double (*func)(double *x, long *inval), long iWorst,
                    long *evaluations, double factor, short *usedLast, short *newPoint);
long checkVariableLimits(double *x, double *xlo, double *xhi, long n);
void simplexFindBestWorst(double *fValue, long points,
                          long *bestPointPtr, long *worstPointPtr,
                          long *nextWorstPointPtr);

#define DEFAULT_MAXEVALS 100
#define DEFAULT_MAXPASSES 5

#define DEBUG 0

#define SIMPLEX_ABORT 0x0001UL
static unsigned long simplexFlags = 0;

long simplexMinAbort(long abort)
{
  if (abort) {
    /* if zero, then operation is a query */
    simplexFlags |= SIMPLEX_ABORT;
#ifdef DEBUG
    fprintf(stderr, "simplexMin abort requested\n");
#endif
  }
  return simplexFlags&SIMPLEX_ABORT ? 1 : 0;
}

long simplexMin(
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
                long maxEvaluations,
                long maxPasses,
                long maxDivisions,
                double divisorFactor,       /* for old default behavior, set to 3 */
                double passRangeFactor,     /* for old default behavior, set to 1 */
                unsigned long flags
                )
{
  static double **simplexVector = NULL, *y = NULL, *trialVector = NULL, *dxLocal = NULL;
  static long lastDimensions = 0;
  static long *dimIndex = NULL;
  double yLast, dVector = 1, divisor, denominator, merit;
  long direction, point, evaluations, totalEvaluations = 0, isInvalid, pass=0, step, divisions;
  long activeDimensions, dimension, i;

  if (divisorFactor<=1.0)
    divisorFactor = 3;   /* old default value */
  simplexFlags = 0;
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

  if (flags&SIMPLEX_VERBOSE_LEVEL1)
    fprintf(stdout, "simplexMin: Active dimensions: %ld\n", activeDimensions);

  if (activeDimensions>lastDimensions) {
    if (simplexVector)
      free_zarray_2d((void**)simplexVector, lastDimensions+1, lastDimensions);
    simplexVector = (double**)zarray_2d(sizeof(**simplexVector), activeDimensions+1, dimensions);
    if (y)
      free(y);
    y = tmalloc(sizeof(*y)*(activeDimensions+1));
    if (trialVector)
      free(trialVector);
    trialVector = tmalloc(sizeof(*trialVector)*activeDimensions);
    if (dxLocal)
      free(dxLocal);
    dxLocal = tmalloc(sizeof(*dxLocal)*activeDimensions);
    if (dimIndex)
      free(dimIndex);
    dimIndex = tmalloc(sizeof(*dimIndex)*activeDimensions);
    lastDimensions = activeDimensions;
  }

  for (direction=i=0; direction<dimensions; direction++) {
    if (!disable || !disable[direction])
      dimIndex[i++] = direction;
  }
  if (i!=activeDimensions) {
    fprintf(stderr, "Fatal error (simplexMin): active dimensions not properly counted\n");
    exit(1);
  }
  
  if (!dxGuess) {
    dxGuess = dxLocal;
    for (direction=0; direction<dimensions; direction++)
      dxGuess[direction] = 0;
  }
  if (flags&SIMPLEX_RANDOM_SIGNS) {
    time_t intTime;
    time(&intTime);
    srand(intTime);
  }
  for (direction=0; direction<dimensions; direction++) {
    if (dxGuess[direction]==0) {
      if (xLowerLimit && xUpperLimit)
        dxGuess[direction] = (xUpperLimit[direction]-xLowerLimit[direction])/4;
      else if ((dxGuess[direction] = xGuess[direction]/4)==0)
        dxGuess[direction] = 1;
    }
    if (flags&SIMPLEX_RANDOM_SIGNS) {
      if (rand()>RAND_MAX/2.0)
        dxGuess[direction] *= -1;
    }
    if (xLowerLimit && xUpperLimit) {
      if ((dVector = fabs(xUpperLimit[direction]-xLowerLimit[direction])/4)<fabs(dxGuess[direction]))
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
  if (flags&SIMPLEX_VERBOSE_LEVEL1) {
    fprintf(stdout, "simplexMin: starting conditions:\n");
    for (direction=0; direction<dimensions; direction++) 
      fprintf(stdout, "direction %ld: guess=%le delta=%le disable=%hd\n",
	      direction, xGuess[direction], dxGuess[direction], 
	      disable?disable[direction]:(short)0);
  }
  
  if (maxPasses<=0)
    maxPasses = DEFAULT_MAXPASSES;

  /* need this to prevent problems when abort occurs before the
   * initial simplex is formed
   */
  for (point=0; point<activeDimensions+1; point++)
    y[point] = DBL_MAX;
  
  while (pass<maxPasses && !(simplexFlags&SIMPLEX_ABORT)) {
    /* Set up the initial simplex */
    /* The first vertex is just the starting point */
    for (direction=0; direction<dimensions; direction++)
      simplexVector[0][direction] = xGuess[direction];
    *yReturn = y[0] = (*func)(simplexVector[0], &isInvalid);
    totalEvaluations++;
    pass++;
    if (isInvalid) {
      fprintf(stderr, "error: initial guess is invalid in simplexMin()\n");
      return(-3);
    }
    if (y[0]<=target) {
      if (flags&SIMPLEX_VERBOSE_LEVEL1) {
	fprintf(stdout, "simplexMin: target value achieved in initial simplex setup.\n");
      }
      if (report)
        (*report)(y[0], simplexVector[0], pass, totalEvaluations, dimensions);
      return(totalEvaluations);
    }

    divisor = 1;
    divisions = 0;
    for (point=1; !(simplexFlags&SIMPLEX_ABORT) && point<activeDimensions+1; point++) {
      if (flags&SIMPLEX_VERBOSE_LEVEL1) {
	fprintf(stdout, "simplexMin: Setting initial simplex for direction %ld\n", point-1);
      }
      dimension = dimIndex[point-1];
      if (!(flags&SIMPLEX_NO_1D_SCANS)) {
        /* Set up the rest of the simplex.  Each vertex is found by doing a 1-D scan
         * starting with the first or the last vertex. 
         */
        for (direction=0; direction<dimensions; direction++) 
          simplexVector[point][direction] = simplexVector[(flags&SIMPLEX_START_FROM_VERTEX1)?0:point-1][direction];
        
        /* Scan direction point-1 until a direction of improvement is found. */
        divisions = 0;
        divisor = 1;
        yLast = y[point-1];
        while (divisions<maxDivisions && !(simplexFlags&SIMPLEX_ABORT)) {
	  if (flags&SIMPLEX_VERBOSE_LEVEL1) {
	    fprintf(stdout, "simplexMin: working on division %ld (divisor=%e) for direction %ld\n", 
		    divisions, divisor, point-1);
	  }
          simplexVector[point][dimension] = simplexVector[point-1][dimension]+dxGuess[dimension]/divisor;
          if ((xLowerLimit || xUpperLimit) && 
              !checkVariableLimits(simplexVector[point], xLowerLimit, xUpperLimit, dimensions)) {
#if DEBUG
            long idum;
            fprintf(stderr, " Point outside of bounds:\n");
            for (idum=0; idum<dimensions; idum++)
              fprintf(stderr, "    %le  %le, %le\n", simplexVector[point][idum],
                      xLowerLimit[idum], xUpperLimit[idum]);
#endif
           /* y[point] = fabs(y[0])*1e9;*/
            y[point]=DBL_MAX;
          }
          else {
            y[point] = (*func)(simplexVector[point], &isInvalid);
            totalEvaluations++;
            if (isInvalid) {
#if DEBUG
              fprintf(stderr, " Point is invalid\n");
#endif
            /*  y[point] = fabs(y[0])*1e9; */
              y[point]=DBL_MAX;
            }
            if (y[point]<=target) {
              for (direction=0; direction<dimensions; direction++)
                xGuess[direction] = simplexVector[point][direction];
              *yReturn = y[point];
              if (report)
                (*report)(*yReturn, xGuess, pass, totalEvaluations, dimensions);
	      if (flags&SIMPLEX_VERBOSE_LEVEL1) {
		fprintf(stdout, "simplexMin: invalid function status.  Returning.\n");
	      }
              return(totalEvaluations);
            }
          }
	  if (flags&SIMPLEX_VERBOSE_LEVEL1) {
	    fprintf(stdout, "simplexMin: New value: %le   Last value: %le\n", y[point], yLast);
	  }
          if (y[point]<yLast)
            /* decrease found */
            break;
          divisions++;
          if (divisions%2)
            /* reverse directions */
            divisor *= -1;
          else
            /* decrease the step size */
            divisor *= divisorFactor;
        }
      }
      if ((flags&SIMPLEX_NO_1D_SCANS) || divisions==maxDivisions) {
        for (direction=0; direction<dimensions; direction++) 
          simplexVector[point][direction] = simplexVector[0][direction];
        
        /* Try +/-step-size/divisor until a valid point is found */
        divisions = 0;
        divisor = 1;
        yLast = y[point-1];
        while (divisions<maxDivisions && !(simplexFlags&SIMPLEX_ABORT)) {
#if DEBUG
          fprintf(stderr, "Trying divisor %ld\n", divisions);
#endif
          simplexVector[point][dimension] = simplexVector[0][dimension] +
            dxGuess[dimension]/divisor;
          if ((xLowerLimit || xUpperLimit) && 
              !checkVariableLimits(simplexVector[point], xLowerLimit, xUpperLimit, dimensions)) {
            divisions++;
          } else {
            y[point] = (*func)(simplexVector[point], &isInvalid);
            totalEvaluations++;
            if (isInvalid) {
#if DEBUG
              fprintf(stderr, " Point is invalid\n");
#endif
             /* y[point] = fabs(y[0])*1e9; */
              y[point]=DBL_MAX;
              divisions++;
            } else
              break;
          }
          if (divisions%2)
            /* reverse directions */
            divisor *= -1;
          else
            /* decrease the step size */
            divisor *= 10;
        }
        if (divisions==maxDivisions) {
          fprintf(stderr, "error: can't find valid initial simplex in simplexMin()\n");
          return(-4);
        }
        
      } else {
	if (flags&SIMPLEX_VERBOSE_LEVEL1) 
	  fprintf(stdout, "simplexMin: decrease found---trying more steps\n");
        /* decrease found---try a few more steps in this direction */
        for (step=0; !(simplexFlags&SIMPLEX_ABORT) && step<3; step++) {
          divisor /= divisorFactor;   /* increase step size */
          simplexVector[point][dimension] += dxGuess[dimension]/divisor;
          if ((xLowerLimit || xUpperLimit) &&
              !checkVariableLimits(simplexVector[point], xLowerLimit, xUpperLimit, dimensions)) {
            simplexVector[point][dimension] -= dxGuess[dimension]/divisor;
            break;
          }
          yLast = y[point];
          y[point] = (*func)(simplexVector[point], &isInvalid);
          totalEvaluations++;
          if (isInvalid || y[point]>yLast) {
            simplexVector[point][dimension] -= dxGuess[dimension]/divisor;
              y[point] = yLast;
            break;
          }
          if (y[point]<=target) {
            for (direction=0; direction<dimensions; direction++)
              xGuess[direction] = simplexVector[point][direction];
            *yReturn = y[point];
            if (report)
                (*report)(*yReturn, xGuess, pass, totalEvaluations, dimensions);
	    if (flags&SIMPLEX_VERBOSE_LEVEL1) 
	      fprintf(stdout, "simplexMin: value below target during 1D scan---returning\n");
            return totalEvaluations;
          }
        }
      }
    }

    if (flags&SIMPLEX_VERBOSE_LEVEL1) {
      fprintf(stdout, "simplexMin: Starting simplex: \n");
      for (point=0; point<activeDimensions+1; point++) {
	fprintf(stdout, "V%2ld  %.5g: ", point, y[point]);
	for (direction=0; direction<dimensions; direction++)
	  fprintf(stdout, "%.5g ", simplexVector[point][direction]);
	fprintf(stdout, "\n");
      }
    }

    if (simplexFlags&SIMPLEX_ABORT) {
      long best = 0;
      for (point=1; point<activeDimensions+1; point++)
        if (y[point]<y[best])
          best = point;
      for (direction=0; direction<dimensions; direction++)
        xGuess[direction] = simplexVector[best][direction];
      if (flags&SIMPLEX_VERBOSE_LEVEL1) 
	fprintf(stdout, "simplexMin: abort received before simplex began---returning\n");
      return totalEvaluations;
    }
    
    evaluations = 0;
    simplexMinimization(simplexVector, y, xLowerLimit, xUpperLimit, disable,
                        dimensions, activeDimensions, target,
                        fabs(tolerance), (tolerance<0?0:1), func, maxEvaluations, &evaluations,
			flags);
    if (flags&SIMPLEX_VERBOSE_LEVEL1)
      fprintf(stdout, "simplexMin: returned from simplexMinimization after %ld evaluations\n",
	      evaluations);
    totalEvaluations += evaluations;
    for (point=1; point<activeDimensions+1; point++) {
      if (y[0]>y[point]) {
        bomb("problem with ordering of data from simplexMinimization", NULL);
      }
    }
    
    /* Copy the new best result into the guess vector (for return or re-use) */
    for (direction=0; direction<dimensions; direction++)
      xGuess[direction] = simplexVector[0][direction];

    if (report)
      (*report)(y[0], simplexVector[0], pass, totalEvaluations, dimensions);

    if (y[0]<=target || (simplexFlags&SIMPLEX_ABORT)) {
      *yReturn = y[0];
      if (flags&SIMPLEX_VERBOSE_LEVEL1)
	fprintf(stdout, "simplexMin: target value achieved---returning\n");
      return(totalEvaluations);
    }

    if (tolerance<=0) {
      denominator = (y[0]+(*yReturn))/2;
      if (denominator)
        merit = fabs(y[0]-(*yReturn))/denominator;
      else {
        fputs("error: divide-by-zero in fractional tolerance evaluation (simplexMin)\n", stderr);
        return -1;
      }
    }
    else
      merit = fabs(y[0]-(*yReturn));
    if (merit<=fabs(tolerance) || y[0]<=target)
      break;

    /* Set up step sizes for finding the new simplex */
    for (direction=0; direction<dimensions; direction++) {
      double min, max;
      min = max = simplexVector[0][direction];
      for (point=1; point<activeDimensions+1; point++) {
        if (simplexVector[point][direction]>max)
          max = simplexVector[point][direction];
        if (simplexVector[point][direction]<min)
          min = simplexVector[point][direction];
      }
      if (max>min)
        dxGuess[direction] = passRangeFactor*(max-min);
    } 
  }
  
  if (flags&SIMPLEX_VERBOSE_LEVEL1)
    fprintf(stdout, "simplexMin: iterations exhausted---returning\n");

  *yReturn = y[0];
  if (pass>maxPasses)
    return(-2);
  return(totalEvaluations);
}

long simplexMinimization(
                         double **simplexVector,        /* vectors defining the simplex */
                         double *fValue,                /* values of the function at the vertices of the simplex */
                         double *coordLowerLimit,       /* lower limits allowed for independent variables */
                         double *coordUpperLimit,       /* upper limits allowed for independent variables */
                         short *disable,                /* indicates coordinate not involved in optimization */
                         long dimensions,               /* number of variables in function */
                         long activeDimensions,         /* number of variables changed in optimization */
                         double target,                 /* will return with any value <= this */
                         double tolerance,              /* desired tolerance of minimum value */
                         long tolerance_mode,           /* 0==fractional, 1==absolute */
                         double (*function)(double *x, long *invalid),
                         long maxEvaluations,
                         long *evaluations,                  /* number of function evaluations done during minimization */
			 unsigned long flags
                         )
{
  long point, points, invalids, degenerates, isDegenerate, isInvalid;
  long direction, bestPoint, worstPoint, nextWorstPoint;
  double fTrial, fProblem, fWorst, fBest, merit, denominator;
  static long maxDimensions = 0;
  static double *simplexCenter = NULL, *tmpVector;
  short usedLast, usedLastCount=0, newPoint;
  long reflectionWorked=0, extensionWorked=0, contractionWorked=0, shrinkingDone=0;
  long progressMade;

  if (dimensions>maxDimensions) {
    if (simplexCenter)
      free(simplexCenter);
    simplexCenter = tmalloc(sizeof(*simplexCenter)*(maxDimensions=dimensions));
    if (tmpVector)
      free(tmpVector);
    tmpVector = tmalloc(sizeof(*tmpVector)*(maxDimensions=dimensions));
  }

  *evaluations = 0;
  if (maxEvaluations<=0)
    maxEvaluations = DEFAULT_MAXEVALS;

  computeSimplexCenter(simplexCenter, simplexVector, dimensions, activeDimensions);

  points = activeDimensions+1;
  while (*evaluations<maxEvaluations && !(simplexFlags&SIMPLEX_ABORT)) {
    /* find indices of lowest, highest, and next-to-highest y values .
       These starting values are to guarantee that worstPoint!=bestPoint even if
       all function values are the same.
       */
    if (flags&SIMPLEX_VERBOSE_LEVEL2)
      fprintf(stdout, "simplexMinimization: finding best and worst points\n");
    simplexFindBestWorst(fValue, points, &bestPoint, &worstPoint, &nextWorstPoint);
    fBest = fValue[bestPoint];
    fWorst = fValue[worstPoint];

    if (flags&SIMPLEX_VERBOSE_LEVEL2)
      fprintf(stdout, "simplexMinimization: evaluating present results\n");
    /* evaluate the merit of the present vectors */
    if (tolerance_mode==0) {
      /* fractional tolerance */
      if ((denominator = (fabs(fWorst)+fabs(fBest))/2))
        merit = fabs(fWorst-fBest)/denominator;
      else {
        fputs("error: divide-by-zero in fractional tolerance evaluation (simplexMinimization)\n", stderr);
        return 0;
      }
    }
    else
      /* absolute tolerance */
      merit = fabs(fWorst-fBest);
    if (merit<tolerance || fBest<=target) {
      /* tolerance exceeded, or value small enough */
      if (flags&SIMPLEX_VERBOSE_LEVEL2)
	fprintf(stdout, "simplexMinimization: tolerance exceed or value small enough\n");
      break;
    }
    
    progressMade = 0;
    if (flags&SIMPLEX_VERBOSE_LEVEL2)
      fprintf(stdout, "simplexMinimization: Reflecting simplex\n");
    /* Reflect the simplex through the high point */
    fTrial = trialSimplex(simplexVector, fValue, simplexCenter, coordLowerLimit, 
                          coordUpperLimit, disable, dimensions, activeDimensions, function, 
                          worstPoint, evaluations, -1.0, &usedLast, &newPoint);
    reflectionWorked += newPoint?1:0;
    progressMade += newPoint;
    if (usedLast)
      usedLastCount++;
    else
      usedLastCount = 0;
    if (usedLastCount>2) {
      if (flags&SIMPLEX_VERBOSE_LEVEL2)
	fprintf(stdout, "simplexMinization: simplex is loooing--ending iterations\n");
      /* stuck in some kind of loop */
      break;
    }
    if (fTrial<fValue[bestPoint]) {
      /* since this worked, extend the simplex by the same amount in that direction.
       * relies on the fact that the new point of the simplex is in the old "worstPoint"
       * slot
       */
      if (flags&SIMPLEX_VERBOSE_LEVEL2)
	fprintf(stdout, "simplexMinization: extending simplex\n");
      fTrial = trialSimplex(simplexVector, fValue, simplexCenter, coordLowerLimit, 
                            coordUpperLimit, disable, dimensions, activeDimensions, function, 
                            worstPoint, evaluations, 2.0, &usedLast, &newPoint);
      extensionWorked += newPoint?1:0;
      progressMade += newPoint;
    }
    else if (fTrial>fValue[nextWorstPoint]) {
      /* reflection through the simplex didn't help, so try contracting away from worst point without
       * going through the face opposite the worst point */
      if (flags&SIMPLEX_VERBOSE_LEVEL2)
	fprintf(stdout, "simplexMinization: contracting simplex\n");
      fProblem = fTrial;
      fTrial = trialSimplex(simplexVector, fValue, simplexCenter, coordLowerLimit,
                            coordUpperLimit, disable, dimensions, activeDimensions, function, 
                            worstPoint, evaluations, 0.5, &usedLast, &newPoint);
      contractionWorked += newPoint?1:0;
      progressMade += newPoint;
      if (fTrial>fProblem) {
        /* the new point is worse than the old trial point, so try moving the entire simplex in on the
           best point by averaging each vector with the vector to the best point.  Don't allow invalid points,
           however, and keep track of the number of degenerate points (those with the same vector or the
           same function value).
           */
	if (flags&SIMPLEX_VERBOSE_LEVEL2)
	  fprintf(stdout, "simplexMinimization: contracting on best point\n");

        invalids = degenerates = 0;
        for (point=0; point<points; point++) {
          if (point==bestPoint)
            continue;
          for (direction=0; direction<dimensions; direction++)
            tmpVector[direction] = 0.5*(simplexVector[point][direction]+simplexVector[bestPoint][direction]);
          for (direction=0; direction<dimensions; direction++)
            if (tmpVector[direction]!=simplexVector[point][direction])
              break;
          isInvalid = 0;
          if (!(isDegenerate = direction!=dimensions)) {
            fTrial = (*function)(tmpVector, &isInvalid);
            if (!isInvalid) {
              if (fTrial==fValue[point])
                isDegenerate = 1;
              for (direction=0; direction<dimensions; direction++)
                simplexVector[point][direction] = tmpVector[direction];
              fValue[point] = fTrial;
            }
          }
          if (isInvalid)
            invalids++;
          if (isDegenerate)
            degenerates++;
        }
        shrinkingDone++;
        if (invalids+degenerates>=points-1) {
          SWAP_PTR(simplexVector[0], simplexVector[bestPoint]);
          SWAP_DOUBLE(fValue[0], fValue[bestPoint]);
	  if (flags&SIMPLEX_VERBOSE_LEVEL2) 
	    fprintf(stdout, "simplexMinimization exiting: reflection: %ld   extension: %ld  contraction: %ld  shrinking: %ld\n",
		    reflectionWorked, extensionWorked, contractionWorked, shrinkingDone);
          return 0;
        }
        *evaluations += points;
        /* since the simplex was changed without using trialSimplex, the "center" must be recomputed
         */
        progressMade += 1;
        computeSimplexCenter(simplexCenter, simplexVector, dimensions, activeDimensions);
      }
    }
    if (!progressMade) {
      if (flags&SIMPLEX_VERBOSE_LEVEL2) 
	fprintf(stdout, "simplexMinimization: Breaking out of loop--no progress.\n");
      break;
    }
  }
  simplexFindBestWorst(fValue, points, &bestPoint, &worstPoint, &nextWorstPoint);
  if (*evaluations >= maxEvaluations) {
    SWAP_PTR(simplexVector[0], simplexVector[bestPoint]);
    SWAP_DOUBLE(fValue[0], fValue[bestPoint]);
    if (flags&SIMPLEX_VERBOSE_LEVEL2) 
      fprintf(stdout, "simplexMinimization: too many iterations\n");
    return 0;
  }
  SWAP_PTR(simplexVector[0], simplexVector[bestPoint]);
  SWAP_DOUBLE(fValue[0], fValue[bestPoint]);
  if (flags&SIMPLEX_VERBOSE_LEVEL2) 
    fprintf(stdout, "simplexMinimization exit report: reflection: %ld   extension: %ld  contraction: %ld  shrinking: %ld\n",
	    reflectionWorked, extensionWorked, contractionWorked, shrinkingDone);
  return(1);
}

void computeSimplexCenter(double *center, double **vector, long dimensions, long activeDimensions)
{
  long point, direction;
  for (direction=0; direction<dimensions; direction++) {
    /* outer loop over dimension */
    for (point=center[direction]=0; point<=activeDimensions; point++)
      /* inner loop over vectors */
      center[direction] += vector[point][direction];
    center[direction] /= activeDimensions;  /* sic--not activeDimensions+1, as one term will get
                                             * subtracted out later 
                                             */
  }
}

double trialSimplex(
                    double **simplexVector,
                    double *funcValue,
                    double *simplexCenter,
                    double *coordLowerLimit,
                    double *coordUpperLimit,
                    short *disable,
                    long dimensions,
                    long activeDimensions,
                    double (*func)(double *x, long *inval),
                    long worstPoint,
                    long *evaluations,
                    double factor,
                    short *usedLast, short *newPoint
                    )
{
  static long maxDimensions = 0, resultStored = 0;
  static double *trialVector = NULL, *lastTrialVector = NULL, lastTrialValue;
  long direction, isInvalid, useLast;
  double trialValue, center;

  *newPoint = *usedLast = 0;
  if (maxDimensions<dimensions) {
    if (trialVector) 
      free(trialVector);
    if (lastTrialVector)
      free(lastTrialVector);
    maxDimensions = dimensions;
    trialVector = tmalloc(sizeof(*trialVector)*dimensions);
    lastTrialVector = tmalloc(sizeof(*lastTrialVector)*dimensions);
    resultStored = 0;
  }

  for (direction=0; direction<dimensions; direction++) {
    /* compute the center of the simplex excluding the worst point */
    center = simplexCenter[direction]-simplexVector[worstPoint][direction]/activeDimensions;
    /* Move relative to that center by factor times the distance from it to the worst point. 
     * (In some cases, the "worst point" is actually just the new (improved) point put in the
     * slot of the previous worst point.)
     */
    if (!disable || !disable[direction]) 
      trialVector[direction] =
        center + factor*(simplexVector[worstPoint][direction]-center);
    else
      trialVector[direction] = simplexVector[worstPoint][direction];
  }

  /* check limits on the values of each coordinate of the trial vector */
  if (!checkVariableLimits(trialVector, coordLowerLimit, coordUpperLimit, dimensions)) {
   /* return 1e9*funcValue[worstPoint]; this is wrong 
      in the casue of funcValue[worstPoint]<0 */
    return DBL_MAX;
    /*return a positive value so that simplex will do contraction in case of exceeding limits*/
  }
  else {
    /* check to see if this is the same as the last point evaluated here */
    useLast = 0;
    if (resultStored) {
      for (direction=0; direction<dimensions; direction++)
        if (trialVector[direction]!=lastTrialVector[direction])
          break;
      if (direction==dimensions)
        useLast = 1;
    }
    *usedLast = useLast;
    
    if (!useLast) {
      trialValue = (*func)(trialVector, &isInvalid);
      ++(*evaluations);
      if (isInvalid)
        return DBL_MAX;
        /*return 1e9*fabs(funcValue[worstPoint]);*/
    }
    else
      trialValue = lastTrialValue;
  }
  
  lastTrialValue = trialValue;
  for (direction=0; direction<dimensions; direction++)
    lastTrialVector[direction] = trialVector[direction];
  resultStored = 1;

  if (trialValue < funcValue[worstPoint]) {
    /* this is better than the previous worst value, so replace the worst value */
    *newPoint = 1;
    funcValue[worstPoint] = trialValue;
    for (direction=0; direction<dimensions; direction++) {
      /* adjust the "center" values for the simplex and copy the new vector coordinates */
      simplexCenter[direction]  += (trialVector[direction]-simplexVector[worstPoint][direction])/activeDimensions;
      simplexVector[worstPoint][direction] = trialVector[direction];
    }
  }

  return(trialValue);
}

long checkVariableLimits(double *x, double *xlo, double *xhi, long n)
{
  long i;
  
  if (xlo)
    for (i=0; i<n; i++)
      if (xlo[i]!=xhi[i] && x[i]<xlo[i])
        return 0;

  if (xhi)
    for (i=0; i<n; i++)
      if (xlo[i]!=xhi[i] && x[i]>xhi[i])
        return 0;
  return 1;
}

/* No longer used---checkVariableLimits is used instead. */
void enforceVariableLimits(double *x, double *xlo, double *xhi, long n)
{
  long i;
  
  if (xlo)
    for (i=0; i<n; i++)
      if ((!xhi || xlo[i]!=xhi[i]) && x[i]<xlo[i])
        x[i] = xlo[i];

  if (xhi)
    for (i=0; i<n; i++)
      if ((!xlo || xlo[i]!=xhi[i]) && x[i]>xhi[i])
        x[i] = xhi[i];
}

void simplexFindBestWorst(double *fValue, long points,
                          long *bestPointPtr, long *worstPointPtr,
                          long *nextWorstPointPtr)
{
  long bestPoint, worstPoint, nextWorstPoint, point;
  double fBest, fNextWorst, fWorst;
  
  if (fValue[0]>fValue[1]) {
    bestPoint = nextWorstPoint = 1;
    worstPoint = 0;
  }
  else {
    bestPoint = nextWorstPoint = 0;
    worstPoint = 1;
  }
  fBest = fNextWorst = fValue[bestPoint];
  fWorst = fValue[worstPoint];
  for (point=1; point<points; point++) {
    if (fBest>fValue[point]) {
      bestPoint = point;
      fBest = fValue[point];
    }
    if (fWorst<fValue[point]) {
      worstPoint = point;
      fWorst = fValue[point];
    }
  }
  for (point=0; point<points; point++)
    if (fNextWorst<fValue[point] && point!=worstPoint) {
      fNextWorst = fValue[point];
      nextWorstPoint = point;
    }
  *bestPointPtr = bestPoint;
  *worstPointPtr = worstPoint;
  *nextWorstPointPtr = nextWorstPoint;
}



