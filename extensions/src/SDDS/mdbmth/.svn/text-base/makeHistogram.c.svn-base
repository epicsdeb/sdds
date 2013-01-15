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
 Revision 1.3  2000/10/07 01:15:47  borland
 Added computeMedian function.

 * Revision 1.2  1995/09/05  21:20:27  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* routine: make_histogram()
 * purpose: compiles a histogram.  Returns the number of points binned.
 *
 * Michael Borland, 1987.
 */
#include "mdb.h"

long make_histogram(
    double *hist, long n_bins, double lo, double hi, double *data,
    long n_pts, long new_start
    )
{
    static long bin, i;
    static double bin_size, dbin;

    if (new_start) {
        bin_size = (hi-lo)/n_bins;
        for (i=0; i<n_bins; i++)
                hist[i] = 0;
        }
    
    for (i=0; i<n_pts; i++) {
        bin = (dbin=(data[i]-lo)/bin_size);
        if (dbin<0)
            continue;
        if (bin<0 || bin>=n_bins) 
            continue;
        hist[bin] += 1;
        }

    for (i=bin=0; i<n_bins; i++)
        bin += hist[i];
    
    return(bin);
    }


long make_histogram_weighted(
    double *hist, long n_bins, double lo, double hi, double *data,
    long n_pts, long new_start, double *weight
    )
{
    static long bin, i, count;
    static double bin_size, dbin;

    if (new_start) {
        count = 0;
        bin_size = (hi-lo)/n_bins;
        for (i=0; i<n_bins; i++)
                hist[i] = 0;
        }
    
    for (i=0; i<n_pts; i++) {
        bin = (dbin=(data[i]-lo)/bin_size);
        if (dbin<0)
            continue;
        if (bin<0 || bin>=n_bins) 
            continue;
        hist[bin] += weight[i];
        count++;
        }

    return(count);
    }

long computeMode(double *result, double *data, long pts, double binSize, long bins)
{
  double min, max;
  long imin, imax;
  double *histogram;
  
  if ((binSize<=0 && bins<=2) || (binSize>0 && bins>2))
    return -1;
  if (pts<=0)
    return -2;
  if (!data)
    return -3;
  if (!result)
    return -4;
  if (pts==1) {
    *result = data[0];
    return 1;
  }
  find_min_max(&min, &max, data, pts);
  /* add buffer bins and compute bin size or number of bins */
  if (binSize>0) {
    max += binSize;
    min -= binSize;
    bins = (max-min)/binSize+0.5;
  } else {
    binSize = (max-min)/bins;
    max += binSize;
    min -= binSize;
    bins += 2;
    binSize = (max-min)/bins;
  }
  if (!(histogram=malloc(sizeof(*histogram)*bins)))
    bomb("memory allocation failure (computeMode)", NULL);
  make_histogram(histogram, bins, min, max, data, pts, 1);
  index_min_max(&imin, &imax, histogram, bins);
  free(histogram);
  *result = (imax+0.5)*binSize+min;
  return 1;
}

