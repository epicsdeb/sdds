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
 Revision 1.7  2003/08/28 19:56:27  soliday
 Cleaned up some of the code.

 Revision 1.6  2002/08/14 16:18:58  soliday
 Added Open License

 Revision 1.5  2002/01/07 21:31:11  borland
 Adde approximate_percentiles() function.

 Revision 1.4  1999/05/28 14:56:40  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/12/07 18:35:32  borland
 Added compute_percentiles() routine.

 * Revision 1.2  1995/09/05  21:20:31  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: median.c 
 * contents: compute_median(), compute_percentile(), compute_average(), compute_middle()
 * see also: find_XX() routines in rowmedian.c  These return the position of the
 *     median, etc.
 * Michael Borland, 1991, 1995
 */
#include "mdb.h"
 
long compute_median(double *value, double *x, long n)
{
    static double *data = NULL;
    static long last_n=0;
    long i;

    if (n<=0)
        return 0;
    if (n>last_n) {
        data = trealloc(data, sizeof(*data)*n);
        last_n = n;
        }
    for (i=0; i<n; i++)
        data[i] = x[i];
    qsort((void*)data, n, sizeof(*data), double_cmpasc);
    *value = data[n/2];
    return 1;
    }

long compute_percentile(double *value, double *x, long n, double percentile)
{
    static double *data = NULL;
    static long last_n=0;
    long i;

    if (n<=0 || percentile<0 || percentile>100)
        return 0;
    if (n>last_n) {
        data = trealloc(data, sizeof(*data)*n);
        last_n = n;
        }
    for (i=0; i<n; i++)
        data[i] = x[i];
    qsort((void*)data, n, sizeof(*data), double_cmpasc);
    *value = data[(long)((n-1)*(percentile/100.0))];
    return 1;
    }

long compute_percentiles(double *position, double *percent, long positions, double *x, long n)
{
    static double *data = NULL;
    static long last_n=0;
    long ip;

    if (n<=0 || positions<=0)
        return 0;
    if (n>last_n) {
        data = trealloc(data, sizeof(*data)*n);
        last_n = n;
        }
    memcpy((char*)data, (char*)x, sizeof(*x)*n);
    qsort((void*)data, n, sizeof(*data), double_cmpasc);
    for (ip=0; ip<positions; ip++) 
        position[ip] = data[(long)((n-1)*(percent[ip]/100.0))];
    return 1;
    }

long compute_average(double *value, double *data, long n)
{
    double sum;
    long i;

    if (n<=0)
        return 0;

    for (i=sum=0; i<n; i++)
        sum += data[i];
    *value = sum/n;
    return 1;
    }

long compute_middle(double *value, double *data, long n)
{
    double min, max;
    if (n<=0)
        return 0;

    if (!find_min_max(&min, &max, data, n))
        return 0;
    *value = (min+max)/2;
    return 1;
    }

long approximate_percentiles(double *position, double *percent, long positions, double *x, long n, 
                             long bins)
{
  double *hist, *cdf, xMin, xMax, xCenter, xRange;
  long i, j, k;
  if (bins<2 || positions<=0 || n<=0)
    return 0;
  if (!(hist = malloc(sizeof(*hist)*bins)))
    return 0;
  find_min_max(&xMin, &xMax, x, n);
  xCenter = (xMax+xMin)/2;
  xRange = (xMax-xMin)*(1+1./bins)/2;
  xMin = xCenter-xRange;
  xMax = xCenter+xRange;
  make_histogram(hist, bins, xMin, xMax, x, n, 1);
  
  cdf = hist;
  for (i=1; i<bins; i++)
    cdf[i] += cdf[i-1];
  for (i=0; i<bins; i++)
    cdf[i] /= cdf[bins-1];

  for (j=0; j<positions; j++) {
    for (i=k=0; i<bins; i++) {
      if (cdf[i]<percent[j]/100.0)
        k = i;
      else
        break;
    }
    position[j] = xMin + (k*(xMax-xMin))/bins;
  }
  free(hist);
  return 1;               
}
