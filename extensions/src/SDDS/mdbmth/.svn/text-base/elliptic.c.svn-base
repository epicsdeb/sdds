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
 Revision 1.2  1995/09/05 21:19:37  saunders
 First test release of the SDDS1.5 package.

*/

/* file: elliptic.c
 * purpose: evaluation of complete elliptic integrals K and E as a 
 *          function of the modulus k, where the integrands are
 *          1/sqrt(1-k^2*(sin theta)^2) and sqrt(1-k^2*(sin theta)^2).
 *          Also: evaluation of the total derivatives of these functions
 *          with respect to modulus.
 *
 * routines: K_cei(), E_cei(), KE_cei(), dK_cei(), dE_cei()
 * reference: Abramowitz and Stegun, 17.6  (Note that k = sqrt(m) = sin(alpha))
 * Michael Borland, 1995.
 */
#include "mdb.h"

static double ceiAccuracy = 1e-14;

void setCeiAccuracy(double newAccuracy)
{
    ceiAccuracy = newAccuracy;
    }

double K_cei(double k)
{
    double a0, b0, c0, a1, b1, c1;
    
    a0 = 1;
    b0 = sqrt(1-sqr(k));
    c0 = k;

    do {
        /* do two steps of recurrence per pass in the loop */
        a1 = (a0+b0)/2;
        b1 = sqrt(a0*b0);
        c1 = (a0-b0)/2;
        a0 = (a1+b1)/2;
        b0 = sqrt(a1*b1);
        c0 = (a1-b1)/2;
        } while (fabs(c0)>ceiAccuracy);
    return PI/(2*a0);
    }

double E_cei(double k)
{
    double a0, b0, c0, a1, b1, c1, K, sum, powerOf2;

    a0 = 1;
    b0 = sqrt(1-sqr(k));
    c0 = k;
    sum = sqr(c0);
    powerOf2 = 1;

    do {
        /* do two steps of recurrence per pass in the loop */
        a1 = (a0+b0)/2;
        b1 = sqrt(a0*b0);
        c1 = (a0-b0)/2;
        sum += sqr(c1)*(powerOf2 *= 2);;

        a0 = (a1+b1)/2;
        b0 = sqrt(a1*b1);
        c0 = (a1-b1)/2;
        sum += sqr(c0)*(powerOf2 *= 2);
        } while (fabs(c0)>ceiAccuracy);
    
    K = PI/(2*a0);
    return K*(1-sum/2);
    }

double *KE_cei(double k, double *buffer)
{
    double a0, b0, c0, a1, b1, c1, K, sum, powerOf2;

    if (!buffer)
        buffer = tmalloc(sizeof(*buffer)*2);

    a0 = 1;
    b0 = sqrt(1-sqr(k));
    c0 = k;
    sum = sqr(c0);
    powerOf2 = 1;

    do {
        /* do two steps of recurrence per pass in the loop */
        a1 = (a0+b0)/2;
        b1 = sqrt(a0*b0);
        c1 = (a0-b0)/2;
        sum += sqr(c1)*(powerOf2 *= 2);;

        a0 = (a1+b1)/2;
        b0 = sqrt(a1*b1);
        c0 = (a1-b1)/2;
        sum += sqr(c0)*(powerOf2 *= 2);
        } while (fabs(c0)>ceiAccuracy);
    
    buffer[0] = K = PI/(2*a0);
    buffer[1] = K*(1-sum/2);
    return buffer;
    }

/* These two functions rely on formulae quoted in Landau and Lifshitz,
   ELECTRODYNAMICS OF CONTINUOUS MEDIA, pg 112.
 */
   
double dK_cei(double k)
{
    double buffer[2];
    KE_cei(k, buffer);
    return (buffer[1]/(1-k*k) - buffer[0])/k ;
    }

double dE_cei(k)
double k;
{
    double buffer[2];
    KE_cei(k, buffer);
    return (buffer[1]-buffer[0])/k ;
    }


