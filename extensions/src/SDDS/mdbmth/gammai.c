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
 Revision 1.6  2002/08/14 16:18:55  soliday
 Added Open License

 Revision 1.5  1999/12/15 16:29:24  soliday
 Added support for vxWorks.

 Revision 1.4  1999/07/12 20:37:53  soliday
 Added call to fdlibm library for lgamma function on WIN32

 Revision 1.3  1999/07/08 22:13:16  soliday
 Added "lgamma function not implemented in WIN32" error message

 Revision 1.2  1995/09/05 21:19:58  saunders
 First test release of the SDDS1.5 package.

*/

/* file: gammai.c
 * purpose: routines for incomplete gamma function
 * M. Borland, 1995.
 * Based on Abramowitz and Stegun and Numerical Recipes
 */
#include "mdb.h"

#if defined(_WIN32)
#include "fdlibm.h"
#endif

double gammaIncSeries(double a, double x);
double gammaIncCFrac(double a, double x);

#define GAMMAI_ACCURACY 1e-12

double gammaP(double a, double x)
{
    if (a<=0 || x<0)
        return -1;
    if (x==0)
        return 0;
    if (x<a+1) {
        /* use series */
        return gammaIncSeries(a, x);
        }
    else {
        /* use continued fraction */
        return 1 - gammaIncCFrac(a, x);
        }
    }

double gammaQ(double a, double x)
{
    if (a<=0 || x<0)
        return -1;
    if (x==0)
        return 0;
    if (x<a+1) {
        /* use series */
        return 1-gammaIncSeries(a, x);
        }
    else {
        /* use continued fraction */
        return gammaIncCFrac(a, x);
        }
    }

#define MAX_SERIES 1000

double gammaIncSeries(double a, double x)
{
    double term=0, sum;
    long n;
#if defined(vxWorks)
    fprintf(stderr, "lgamma function not implemented in vxWorks");
    exit(1);
#else
    term = exp(-x)*pow(x, a)/exp(lgamma(a + 1));
#endif
    sum = 0;
    n = 0;
    do {
        sum += term;
        n++;
        term *= x/(a+n);
        } while (term>GAMMAI_ACCURACY && n<MAX_SERIES);
    return (sum+term);
    }

double gammaIncCFrac(double a, double x)
{
    double A0, B0, A1, B1, A_1, B_1;
    double an, bn, f1, f2, accuracy=0, factor=0;
    long m;

#if defined(vxWorks)
    fprintf(stderr, "lgamma function not implemented in vxWorks");
    exit(1);
#else
    accuracy = GAMMAI_ACCURACY/(factor=exp(-x-lgamma(a)+a*log(x)));
#endif
    A0 = 0 ; B0 = 1;
    A_1 = 1; B_1 = 0;
    an = 1;
    bn = x + 1 - a;
    A1 = bn*A0 + an*A_1;
    B1 = bn*B0 + an*B_1;
    f2 = A1/B1;
    m = 1;
    do {
        A_1 = A0; B_1 = B0;
        A0  = A1; B0  = B1;
        f1 = f2;
        an = -m*(m-a);
        bn += 2;
        A1 = bn*A0 + an*A_1;
        B1 = bn*B0 + an*B_1;
        f2 = A1/B1;
        if (B1) {
            /* rescale the recurrence to avoid over/underflow */
            A0 /= B1;
            B0 /= B1;
            A1 /= B1;
            B1 = 1;
            }
        m++;
        } while (m<MAX_SERIES && fabs(f1-f2)>accuracy);
    return factor*f2;
    }
