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
 Revision 1.4  2000/08/08 20:21:15  borland
 Now use midpoint rather than mean in top/base analysis to section the
 data into two halves.

 Revision 1.3  1999/05/28 14:57:13  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:20:57  saunders
 First test release of the SDDS1.5 package.

*/

/* file: topbase.c
 * purpose: routine to find toplevel and baselevel of data 
 *
 * Michael Borland, 1995
 */
#include "mdb.h"

#define DEFAULT_BINFACTOR 0.05
#define DEFAULT_SIGMAS 2

long findTopBaseLevels(double *top, double *base, double *data, long points,
                       long bins, double sigmasRequired)
{
    long binned, iMaximum, i;
    static long maxBins = 0;
    static double *histogram = NULL;
    double min, max, midpoint, maxHistogram, meanBinned;
    double lower, upper, delta;

    if (bins<=0)
        bins = DEFAULT_BINFACTOR*points;
    if (sigmasRequired<=0)
        sigmasRequired = DEFAULT_SIGMAS;
    if (points<2)
        return 0;

    if (bins>maxBins)
        histogram = tmalloc(sizeof(*histogram)*(maxBins=bins));

    if (!find_min_max(&min, &max, data, points))
        return 0;

    *base = min;
    *top  = max;
    midpoint = (min+max)/2;
    if (points<10)
        return 1;
    
    delta = (midpoint-min)/(bins-1);
    lower = min  - delta/2;
    upper = midpoint + delta/2;
    if ((binned = make_histogram(histogram, bins, lower, upper, data, points, 1))) {
        maxHistogram = 0;
        iMaximum = -1;
        for (i=0; i<bins; i++) {
            if (maxHistogram<histogram[i]) {
                iMaximum = i;
                maxHistogram = histogram[i];
                }
            }
        meanBinned = ((double)binned)/bins;
        if (maxHistogram>(meanBinned+sigmasRequired*sqrt(meanBinned)))
            *base = lower + (iMaximum+0.5)*((upper-lower)/bins);
        }
    
    delta = (max-midpoint)/(bins-1);
    lower = midpoint - delta/2;
    upper = max  + delta/2;
    if ((binned = make_histogram(histogram, bins, lower, upper, data, points, 1))) {
        maxHistogram = 0;
        iMaximum = -1;
        for (i=0; i<bins; i++) {
            if (maxHistogram<histogram[i]) {
                iMaximum = i;
                maxHistogram = histogram[i];
                }
            }
        meanBinned = ((double)binned)/bins;
        if (maxHistogram>(meanBinned+sigmasRequired*sqrt(meanBinned)))
            *top = lower + (iMaximum+0.5)*((upper-lower)/bins);
        }
    
    if (*top==*base) {
        *base = min;
        *top = max;
        }
    return 1;
    }

long findCrossingPoint(long start, double *data, long points, double level, long direction, 
                       double *indepData, double *location)
{
    double diff;
    long i, transitionPossible;

    if (start<0 || start>(points-1))
        return -1;
    transitionPossible = 0;
    for (i=start; i<points; i++) {
        diff = direction*(data[i] - level);
        if (diff<=0)
            transitionPossible = 1;
        if (diff>0 && transitionPossible) 
            break;
        }
    if (i==points)
        return -1;
    if (!indepData || !location)
        return i;
    if (i==0 || data[i]==data[i-1]) {
        *location = indepData[i];
        return i;
        }
    *location = indepData[i-1] + (indepData[i]-indepData[i-1])/(data[i]-data[i-1])*(level-data[i-1]);
    return i;
    }

