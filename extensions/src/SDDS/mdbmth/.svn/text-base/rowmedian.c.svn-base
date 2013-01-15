/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: find_median(), find_percentile(), find_median_of_row()
 * purpose: compute median of array of double values stored in a 2D array
 *          and return index of closest point.  Requires libsort.a routines.
 * See also: median.c (doesn't require libsort.a, but doesn't return index).
 *
 * Michael Borland, 1991, 1995
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:20:48  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

long find_median(double *value, double *x, long n)
{
    static double **data = NULL;
    static long last_n=0;
    long i;

    if (n<=0)
        return(-1);
    if (n>last_n) {
        if (data)
            free_zarray_2d((void**)data, last_n, 2);
        data = (double**)zarray_2d(sizeof(**data), n, 2);
        last_n = n;
        }

    for (i=0; i<n; i++) {
        data[i][0] = x[i];
        data[i][1] = i;
        }
    set_up_row_sort(0, 2, sizeof(**data), double_cmpasc);
    qsort((void*)data, n, sizeof(*data), row_compare);

    *value = data[n/2][0];
    i      = data[n/2][1];

    return(i);
    }

long find_percentile(double *value, double *x, long n, double percentile)
{
    static double **data = NULL;
    static long last_n=0;
    long i;

    if (n<=0)
        return(-1);
    if (percentile<0 || percentile>100)
        return -1;
    if (n>last_n) {
        if (data)
            free_zarray_2d((void**)data, last_n, 2);
        data = (double**)zarray_2d(sizeof(**data), n, 2);
        last_n = n;
        }

    for (i=0; i<n; i++) {
        data[i][0] = x[i];
        data[i][1] = i;
        }
    set_up_row_sort(0, 2, sizeof(**data), double_cmpasc);
    qsort((void*)data, n, sizeof(*data), row_compare);

    *value = data[(long)((n-1)*(percentile/100.0))][0];
    i      = data[(long)((n-1)*(percentile/100.0))][1];

    return(i);
    }


long find_median_of_row(double *value, double **x, long index, long n)
{
    static double **data = NULL;
    static long last_n = 0;
    long i;

    if (index<0 && n<=0)
        return(-1);

    if (n>last_n) {
        if (data)
            free_zarray_2d((void**)data, last_n, 2);
        data = (double**)zarray_2d(sizeof(**data), n, 2);
        last_n = n;
        }

    for (i=0; i<n; i++) {
        data[i][0] = x[i][index];
        data[i][1] = i;
        }
    set_up_row_sort(0, 2, sizeof(**data), double_cmpasc);
    qsort((void*)data, n, sizeof(*data), row_compare);

    *value = data[n/2][0];
    i      = data[n/2][1];

    return(i);
    }


long find_average(double *value, double *data, long n)
{
    double sum, min_dist, dist;
    long i, i_best;

    if (n<=0)
        return(-1);

    for (i=sum=0; i<n; i++)
        sum += data[i];
    sum /= n;

    min_dist = DBL_MAX;
    i_best = -1;
    for (i=0; i<n; i++)
        if ((dist=fabs(data[i]-sum))<min_dist) {
            min_dist = dist;
            i_best = i;
            }    
    *value = sum;
    return(i_best);
    }

long find_middle(double *value, double *data, long n)
{
    double target, min_dist, dist, min, max;
    long i, i_best;

    if (n<=0)
        return(-1);

    if (!find_min_max(&min, &max, data, n))
        return(-1);
    target = (min+max)/2;

    min_dist = DBL_MAX;
    i_best = -1;
    for (i=0; i<n; i++)
        if ((dist=fabs(data[i]-target))<min_dist) {
            min_dist = dist;
            i_best = i;
            }    
    *value = target;
    return(i_best);
    }


