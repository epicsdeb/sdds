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
 Revision 1.5  2001/11/09 19:33:00  borland
 Fixed error in equations in accumulateWeightedMoments().

 Revision 1.4  1999/05/28 14:56:51  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/09/06 14:13:34  saunders
 First test release of SDDS1.5

*/

/* file: moments.c
 * contents: standardDeviation(), computeMoments(), computeWeightedMoments(),
 *        accumulateMoments(), accumulateWeightedMoments(), computeCorrelations(),
 *        arithmeticAverage(), rmsValue(), meanAbsoluteDeviation(),
 *        weightedAverage(), weightedRMS(), weightedStDev(), weightedMAD()
 * Michael Borland, 1992, 1995
 */
#include "mdb.h"

double standardDeviation(double *x, long n)
{
    long i;
    double sum, sumSqr, value, mean;
    if (n<1)
        return(0.0);
    for (i=sum=0; i<n; i++) 
        sum += x[i];
    mean = sum/n;
    for (i=sumSqr=0; i<n; i++) {
        value = x[i]-mean;
        sumSqr += value*value;
        }
    return sqrt(sumSqr/(n-1)) ;
    }

long computeMoments(double *mean, double *rms, double *standDev, 
          double *meanAbsoluteDev, double *x, long n)
{
    long i;
    double sum, sumSqr, value, sum2;
    double lMean, lRms, lStDev, lMAD;

    if (!mean) mean = &lMean;
    if (!rms) rms  = &lRms;
    if (!standDev) standDev = &lStDev;
    if (!meanAbsoluteDev)
        meanAbsoluteDev = &lMAD;

    *mean = *standDev = *meanAbsoluteDev = DBL_MAX;

    if (n<1)
        return(0);

    for (i=sum=sumSqr=0; i<n; i++) {
        sum     += (value=x[i]);
        sumSqr += sqr(value);
        }

    *mean = sum/n;
    *rms  = sqrt(sumSqr/n);
    
    for (i=sum=sum2=0; i<n; i++) {
        value = x[i] - *mean;
        sum2 += value*value;
        sum += fabs(value);
        }
    if (n)
        *standDev = sqrt(sum2/(n-1));
    *meanAbsoluteDev = sum/n;

    return(n-1);
    }

long computeWeightedMoments(double *mean, double *rms, double *standDev, 
          double *meanAbsoluteDev, double *x, double *w, long n)
{
    long i;
    double sumW, sum, sumWx, sumSqrWx, sum2, value;

    double lMean, lRms, lStDev, lMAD;

    if (!mean) mean = &lMean;
    if (!rms) rms  = &lRms;
    if (!standDev) standDev = &lStDev;
    if (!meanAbsoluteDev)
        meanAbsoluteDev = &lMAD;

    *mean = *standDev = *meanAbsoluteDev = DBL_MAX;

    if (n<1)
        return(0);

    for (i=sumW=sumWx=sumSqrWx=0; i<n; i++) {
        sumW     += w[i];
        sumWx    += (value=x[i])*w[i];
        sumSqrWx += value*value*w[i];
        }

    if (sumW) {
        *mean = sumWx/sumW;
        *rms  = sqrt(sumSqrWx/sumW);
        for (i=sum=sum2=0; i<n; i++) {
            value = x[i] - *mean;
            sum += value*w[i];
            sum2 += value*value*w[i];
            }
        if (n)
            /* adjust for n-1 weighting */
            *standDev = sqrt((sum2*n)/(sumW*(n-1.0)));
        *meanAbsoluteDev = sum/sumW;
        return(1);
        }
    return(0);
    }

long accumulateMoments(double *mean, double *rms, double *standDev, 
          double *x, long n, long reset)
{
    long i;
    static double sum, sumSqr, value;
    static long nTotal;

    if (reset)
        nTotal = sum = sumSqr = 0;

    nTotal += n;
    if (nTotal<1)
        return(0);

    for (i=0; i<n; i++) {
        sum     += (value=x[i]);
        sumSqr += sqr(value);
        }

    *mean = sum/nTotal;
    *rms  = sqrt(sumSqr/nTotal);
    *standDev = sqrt( (sumSqr/nTotal-sqr(*mean))*nTotal/(nTotal-1.0) );

    return(1);
    }

long accumulateWeightedMoments(double *mean, double *rms, double *standDev, 
          double *x, double *w, long n, long reset)
{
    long i;
    static double sumW, sumWx, sumSqrWx;
    static long nTotal;

    nTotal += n;
    if (nTotal<1)
        return(0);

    if (reset)
        sumW = sumWx = sumSqrWx = nTotal = 0;

    for (i=0; i<n; i++) {
        sumW      += w[i];
        sumWx     += w[i]*x[i];
        sumSqrWx  += x[i]*x[i]*w[i];
        }

    if (sumW) {
        *mean = sumWx/sumW;
        *rms  = sqrt(sumSqrWx/sumW);
        *standDev = sqrt( (sumSqrWx/sumW-sqr(*mean))* (nTotal/(nTotal-1.0)) );
        return(1);
        }
    return(0);
    }

long computeCorrelations(double *C11, double *C12, double *C22, double *x, double *y, long n)
{
    long i;
    double xAve, yAve, dx, dy;

    *C11 = *C12 = *C22 = 0;
    if (!n)
        return(0);

    for (i=xAve=yAve=0; i<n; i++) {
        xAve += x[i];
        yAve += y[i];
        }
    xAve /= n;
    yAve /= n;

    for (i=0; i<n; i++) {
        dx = x[i]-xAve;
        dy = y[i]-yAve;
        *C11 += dx*dx;
        *C12 += dx*dy;
        *C22 += dy*dy;
        }
    *C11 /= n;
    *C12 /= n;
    *C22 /= n;

    return(1);
    }

double arithmeticAverage(double *y, long n)
{
    long i;
    double sum;

    if (!n)
        return(0.0);
    for (i=sum=0; i<n; i++) 
        sum += y[i];
    return(sum/n);
    }

double rmsValue(double *y, long n)
{
    long i;
    double sum;

    if (!n)
        return(0.0);
    for (i=sum=0; i<n; i++) 
        sum += y[i]*y[i];
    return(sqrt(sum/n));
    }


double meanAbsoluteDeviation(double *y, long n)
{
    long i;
    double ave, sum;

    if (!n)
        return(0.0);
    for (i=ave=0; i<n; i++)
        ave += y[i];
    ave /= n;
    for (i=sum=0; i<n; i++) 
        sum += fabs(y[i]-ave);
    return(sum/n);
    }

double weightedAverage(double *y, double *w, long n)
{
    long i;
    double ySum, wSum;

    if (!n)
        return 0.0;
    for (i=ySum=wSum=0; i<n; i++) {
        wSum += w[i];
        ySum += y[i]*w[i];
        }
    if (wSum)
        return ySum/wSum;
    return 0.0;
    }

double weightedRMS(double *y, double *w, long n)
{
    long i;
    double sum, wSum;

    if (!n)
        return(0.0);
    for (i=sum=wSum=0; i<n; i++) {
        sum += y[i]*y[i]*w[i];
        wSum += w[i];
        }
    if (wSum)
        return sqrt(sum/wSum);
    return 0.0;
    }

double weightedMAD(double *y, double *w, long n)
{
    long i;
    double mean, sum, wSum;

    if (!n)
        return(0.0);
    for (i=sum=wSum=0; i<n; i++) {
        sum += y[i]*w[i];
        wSum += w[i];
        }
    if (!wSum)
        return 0.0;
    mean = sum/wSum;
    for (i=sum=0; i<n; i++) 
        sum += fabs(y[i]-mean)*w[i];
    return sum/wSum;
    }

double weightedStDev(double *y, double *w, long n)
{
    long i;
    double mean, sum, wSum, value;

    if (!n)
        return(0.0);
    for (i=sum=wSum=0; i<n; i++) {
        sum += y[i]*w[i];
        wSum += w[i];
        }
    if (!wSum)
        return 0.0;
    mean = sum/wSum;
    for (i=sum=0; i<n; i++)  {
        value = y[i] - mean;
        sum += value*value*w[i];
        }
    return sqrt( (sum*n)/(wSum*(n-1.0)) );
    }


