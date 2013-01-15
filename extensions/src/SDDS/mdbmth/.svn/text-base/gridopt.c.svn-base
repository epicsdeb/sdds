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
 Revision 1.7  2003/01/09 19:42:08  borland
 Fixed bug in random walk minimizer.

 Revision 1.6  2002/10/14 00:06:09  borland
 Made code more efficient.

 Revision 1.5  2002/09/24 21:00:17  borland
 Added randomWalkMin() and randomSampleMin().  Fixed bugs in grid_sample_min().

 Revision 1.4  2002/08/14 16:18:56  soliday
 Added Open License

 Revision 1.3  1999/05/25 16:52:50  borland
 Fixed a bug in while loop test statement that was from using an older
 version of advance_values.

 * Revision 1.2  1995/09/05  21:20:02  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* routine: grid_search_min()
 * purpose: implement grid search for the minimum value of a function
 *
 * Michael Borland, 1991.
 */

#include "mdb.h"

#define OPTIM_ABORT 0x0001UL
static unsigned long optimFlags = 0;

long optimAbort(long abort)
{
  if (abort) {
    /* if zero, then operation is a query */
    optimFlags |= OPTIM_ABORT;
  }
  return optimFlags&OPTIM_ABORT ? 1 : 0;
}


long grid_search_min(
    double *best_result,
    double *xReturn,
    double *lower,
    double *upper,
    double *step,
    long n_dimen,
    double target,
    double (*func)(double *x, long *invalid)
    )
{
    static double *x = NULL, *best_x = NULL;
    static long last_n_dimen = 0;
    static long *index, *counter, *maxcount;
    double result;
    long flag, i, best_found;

    optimFlags = 0;

    if (last_n_dimen<n_dimen) {
        if (x)
            tfree(x);
        if (index) 
            tfree(index);
        if (counter) 
            tfree(counter);
        if (maxcount) 
            tfree(maxcount);
        x        = tmalloc(sizeof(*x)*n_dimen);
        best_x   = tmalloc(sizeof(*best_x)*n_dimen);
        index    = tmalloc(sizeof(*index)*n_dimen);
        counter  = tmalloc(sizeof(*counter)*n_dimen);
        maxcount = tmalloc(sizeof(*maxcount)*n_dimen);
        last_n_dimen = n_dimen;
        }

    *best_result = DBL_MAX;
    for (i=0; i<n_dimen; i++) {
        index[i] = i;
        counter[i] = 0;
        x[i] = lower[i];
        if (lower[i]>=upper[i]) {
            step[i] = 0;
            maxcount[i] = 0;
            }
        else {
            maxcount[i] = (upper[i]-lower[i])/step[i] + 1.5;
            if (maxcount[i]<=1)
                maxcount[i] = 2;
            step[i] = (upper[i]-lower[i])/(maxcount[i]-1);
            }
        }

    best_found = 0;
    do {
        if ((result=(*func)(x, &flag)) < *best_result && flag==0) {
            *best_result = result;
            for (i=0; i<n_dimen; i++)
                best_x[i] = x[i];
            best_found = 1;
            if (result<target)
              break;
            }
        if (optimFlags&OPTIM_ABORT)
          break;
        } while (advance_values(x, index, lower, step, n_dimen, counter, maxcount, n_dimen)>=0);
    
    if (best_found)
      for (i=0; i<n_dimen; i++)
        xReturn[i] = best_x[i];
    
    return(best_found);
    }


long grid_sample_min(
    double *best_result,
    double *xReturn,
    double *lower,
    double *upper,
    double *step,
    long n_dimen,
    double target,
    double (*func)(double *x, long *invalid),
    double sample_fraction,
    double (*random_f)(long iseed)
    )
{
    static double *x = NULL, *best_x = NULL;
    static long last_n_dimen = 0;
    static long *index, *counter, *maxcount;
    double result;
    long flag, i, best_found;

    optimFlags = 0;

    if (random_f==NULL)
      random_f = random_1;

    if (last_n_dimen<n_dimen) {
        if (x)
            tfree(x);
        if (index) 
            tfree(index);
        if (counter) 
            tfree(counter);
        if (maxcount) 
            tfree(maxcount);
        x        = tmalloc(sizeof(*x)*n_dimen);
        best_x   = tmalloc(sizeof(*best_x)*n_dimen);
        index    = tmalloc(sizeof(*index)*n_dimen);
        counter  = tmalloc(sizeof(*counter)*n_dimen);
        maxcount = tmalloc(sizeof(*maxcount)*n_dimen);
        last_n_dimen = n_dimen;
        }

    *best_result = DBL_MAX;
    for (i=0; i<n_dimen; i++) {
        index[i] = i;
        counter[i] = 0;
        x[i] = lower[i];
        if (lower[i]>=upper[i]) {
            step[i] = 0;
            maxcount[i] = 0;
            }
        else {
            maxcount[i] = (upper[i]-lower[i])/step[i] + 1.5;
            if (maxcount[i]<=1)
                maxcount[i] = 2;
            step[i] = (upper[i]-lower[i])/(maxcount[i]-1);
            }
        }

    if (sample_fraction>=1) {
        double npoints = 1;
        for (i=0; i<n_dimen; i++)
            npoints *= maxcount[i];
        sample_fraction /= npoints;
        }

    best_found = 0;
    do {
      if (sample_fraction<(*random_f)(1))
            continue;
        if ((result=(*func)(x, &flag)) < *best_result && flag==0) {
            *best_result = result;
            for (i=0; i<n_dimen; i++)
                best_x[i] = x[i];
            best_found = 1;
            if (result<target)
              break;
            }
        if (optimFlags&OPTIM_ABORT)
          break;
        } while (advance_values(x, index, lower, step, n_dimen, counter, maxcount, n_dimen)>=0);

    if (best_found)
      for (i=0; i<n_dimen; i++)
        xReturn[i] = best_x[i];
    
    return(best_found);
    }


/* routine: randomSampleMin()
 * purpose: implement random search in a N-dimensional hypercube for a minimum
 * of a function.  Possibly useful if the function is very badly behaved and a
 * starting point is needed for a real optimization run.
 *
 * Michael Borland, 2002.
 */

long randomSampleMin(
                     double *best_result,
                     double *xReturn,
                     double *lower,
                     double *upper,
                     long n_dimen,
                     double target,
                     double (*func)(double *x, long *invalid),
                     long nSamples,
                     double (*random_f)(long iseed)
                     )
{
  double *x, *xBest;
  double result;
  long flag, i, best_found = 0;

  optimFlags = 0;
  if (random_f==NULL)
    random_f = random_1;

  x = tmalloc(sizeof(*x)*n_dimen);
  xBest = tmalloc(sizeof(*xBest)*n_dimen);
  for (i=0; i<n_dimen; i++)
    xBest[i] = xReturn[i];
  *best_result = DBL_MAX;
  while (nSamples--) {
    for (i=0; i<n_dimen; i++)
      x[i] = lower[i] + (upper[i]-lower[i])*(*random_f)(0);
    if ((result=(*func)(x, &flag)) < *best_result && flag==0) {
      *best_result = result;
      for (i=0; i<n_dimen; i++)
        xBest[i] = x[i];
      best_found = 1;
      if (result<target)
        break;
    }
    if (optimFlags&OPTIM_ABORT)
      break;
  } 
  if (best_found) {
    for (i=0; i<n_dimen; i++)
      xReturn[i] = xBest[i];
  }
  free(x);
  free(xBest);
  return(best_found);
}


/* routine: randomWalkMin()
 * purpose: implement random search starting at a user-supplied point and 
 *  randomly exploring the surroundings.
 *  Possibly useful if the function is very badly behaved and a
 *  starting point is needed for a real optimization run.
 *
 * Michael Borland, 2002.
 */

long randomWalkMin(
                     double *best_result,
                     double *xReturn,
                     double *lower,
                     double *upper,
                     double *stepSize,
                     long n_dimen,
                     double target,
                     double (*func)(double *x, long *invalid),
                     long nSamples,
                     double (*random_f)(long iseed)
                     )
{
  double *x, *xBest;
  double result;
  long flag, i, best_found = 0;

  optimFlags = 0;

  if (random_f==NULL)
    random_f = random_1;

  x = tmalloc(sizeof(*x)*n_dimen);
  xBest = tmalloc(sizeof(*xBest)*n_dimen);
  for (i=0; i<n_dimen; i++)
    xBest[i] = xReturn[i];
  *best_result = DBL_MAX;
  while (nSamples--) {
    for (i=0; i<n_dimen; i++) {
      x[i] = xBest[i] + 2*stepSize[i]*(0.5-random_f(0));
      if (lower && x[i]<lower[i])
        x[i] = lower[i];
      if (upper && x[i]>upper[i])
        x[i] = upper[i];
    }
    result = (*func)(x, &flag);
    if (flag==0 && result<*best_result) {
      *best_result = result;
      for (i=0; i<n_dimen; i++)
        xBest[i] = x[i];
      best_found = 1;
      if (result<target)
        break;
    }
    if (optimFlags&OPTIM_ABORT)
      break;
  } 
  if (best_found) {
    for (i=0; i<n_dimen; i++)
      xReturn[i] = xBest[i];
  }
  free(x);
  free(xBest);
  return(best_found);
}


