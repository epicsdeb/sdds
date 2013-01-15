/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: gauss_fit()
 * purpose: find sigma, mean, normalization, and baseline to fit a gaussian 
 *          to data. Returns weighted chi_squared.
 *          This version considers errors in the dependent variable.
 *
 * Michael Borland, 1988, 1990
 $Log: not supported by cvs2svn $
 Revision 1.9  2003/01/16 18:39:11  soliday
 Changed because the arguments to simplexMin changed.

 Revision 1.8  2002/08/14 16:18:55  soliday
 Added Open License

 Revision 1.7  2001/07/27 19:21:20  borland
 Added argument for simplexMin().

 Revision 1.6  2001/05/15 16:39:13  borland
 Added flags argument to simplexMin().  At present, this permits disabling
 the 1D scans that are normally done prior to the actual simplex routine.

 Revision 1.5  1999/05/28 14:58:43  soliday
 Removed compiler warnings under Linux.

 Revision 1.4  1999/04/30 13:22:01  borland
 Removed unneeded declaration of min().

 Revision 1.3  1999/02/02 16:46:15  borland
 Uses new syntax for simplexMin().

 * Revision 1.2  1995/09/05  21:20:00  saunders
 * First test release of the SDDS1.5 package.
 *
 */ 
#include "mdb.h"

double deviation(
    double *param,         /* sigma, mean/sigma, base, norm */
    long *invalid
    );

static double *xd, *yd, *yerr;
static long nd;

#define MAX_ITERATIONS 50000
#define MAX_PASSES        10

double gauss_fit(
    /* y = N*exp(-(x/sigma-mean/sigma)^2/2))+baseline */
    double *y, double *x,
    /* sigmas for y values */
    double *sy,
    /* number of data points */
    long n,
    /* parameters to be computed */
    double *sigma, double *mean, double *N, double *base,
    /* factors for computing step sizes (try 100,1e6) */
    double f1, double f2    
    )
{    
    double param[4], dp[4], dplim[4], chi;
    double xhalf, dhalf, ymin, ymax, xcenter, tmp, xmax;
    long i;
    

    if (n<=3) {
        *sigma = *mean = *N = 0;
        return(-1.0);
        }

    /* get some estimates of parameters to start things off with */
    /* first find maximum y value and corresponding x value, plus max x */
    xcenter = 0;
    ymax    = -DBL_MAX;
    xmax    = -DBL_MAX;
    ymin    = DBL_MAX;
    for (i=0; i<n; i++) {
        if (xmax<(tmp=fabs(x[i])))
            xmax = tmp;
        if (ymax<y[i]) {
            ymax    = y[i];
            xcenter = x[i];
            }
        if (ymin>y[i])
            ymin = y[i];
        }
    if (fabs(ymax)<1e-36) {
        *sigma = *mean = *N = 0;
        return(-1.0);
        }

    /*  now find approximate half-max point */
    xhalf = 0;
    dhalf = DBL_MAX;
    for (i=0; i<n; i++) {
        tmp = fabs(fabs(y[i]-ymax)/ymax-.5);
        if (tmp<dhalf) {
            xhalf = x[i];
            dhalf = tmp;    
            }
        }
    param[0] = fabs(xhalf-xcenter)/1.177;    /* starting sigma */
    param[1] = xcenter/param[0];             /* starting mean over sigma  */
    param[2] = ymin;                         /* starting baseline */
    param[3] = ymax - ymin;

    if (*sigma) {
        /* use guesses provided */
        param[0] = *sigma;
        param[1] = *mean/(*sigma);
        param[2] = *base;
        param[3] = *N;
        }

    /* step sizes */
    dp[0]    = fabs(param[0]/f1);
    dp[1]    = fabs(1/f1);
    if ((dp[2]= fabs(param[2]/f1))==0) 
        dp[2] = (ymax-ymin)/f1/100;
    dp[3] = fabs(param[3]/f1);

    /* step size limits */
    dplim[0] = dp[0]*f1/f2;
    dplim[1] = dp[1]*f1/f2;
    dplim[2] = dp[2]*f1/f2;
    dplim[3] = dp[3]*f1/f2;

    /* copy data pointers and n to global variables */
    nd = n;
    xd = x;
    yd = y;
    yerr = tmalloc(sizeof(*yerr)*nd);
    if (sy==NULL)
        /* use equal weights */
        for (i=0; i<nd; i++)
            yerr[i] = 1;
    else
        for (i=0; i<nd; i++)
            yerr[i] = sqr(sy[i]);

    simplexMin(&chi, param, dp, NULL, NULL, NULL, 4, -DBL_MAX, 1.0/f2, 
               deviation, NULL, MAX_ITERATIONS, MAX_PASSES, 12, 3.0, 1.0, 0);

    *sigma = param[0];
    *mean  = param[0]*param[1];
    *base  = param[2];
    *N     = param[3];

    return(chi);
    }


double deviation(
    double *param,         /* sigma, mean/sigma, base, norm */
    long *invalid
    )
{
    double sum, tmp, mean, sigma, base, norm;
    long i;
    
    *invalid = 0;
    sigma = param[0];
    mean  = param[1];
    base  = param[2];
    norm  = param[3];

    for (i=sum=0; i<nd; i++) {
        tmp = xd[i]/sigma - mean;
        tmp = exp(-sqr(tmp)/2);
        tmp = yd[i] - base - norm*tmp;
        sum += sqr(tmp)/yerr[i];
        }
    return(sum/(nd-4));
    }


