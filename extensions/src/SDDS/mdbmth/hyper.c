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
 Revision 1.2  1995/09/05 21:20:03  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: hyper()
 * purpose: evaluate hypergeometric function by summing series
 * reference: Gradshteyn and Rizhik, 9.1
 *
 * Michael Borland, 1988
 */
#include "mdb.h"


double hyper(a, b, g, z, n, lim, pinter)
double a, b, g, z;      /* alpha, beta, gamma, z */
long n;                  /* number of terms to take */
double lim;             /* value of smallest term to be used */
long pinter;             /* print interval */
{
    double sum, pra, prb, prg, prz, term;
    long i, iprint;

    sum  = 1;
    term = a*b/g*z;
    i    = 1;
    iprint = 0;
    while (i!=n && fabs(term)>lim) {
        sum += term;
        if ((iprint++)%pinter==0)
            printf("%ld   %le   %le\n", i, sum, term);
        a += 1;
        b += 1;
        g += 1;
        i++;
        term *= a*b*z/(g*i);
        }
    return(sum);
    }

