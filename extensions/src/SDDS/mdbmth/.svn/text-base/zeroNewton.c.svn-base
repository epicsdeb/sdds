/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: zeroNewton()
 * purpose: find zero of a function using newton's method and numerical computation
 *          of derivatives.
 *
 * Michael Borland, 1994, 1995
 $Log: not supported by cvs2svn $
 Revision 1.3  2000/11/02 21:25:27  borland
 Added "value" option that allows solving for fn=value instead of just
 fn=0.

 * Revision 1.2  1995/09/05  21:21:02  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"

double zeroNewton(
    double (*fn)(),	 /* pointer to function to be zeroed */
    double value,        /* value of function to find */
    double x_i,          /* initial value for independent variable */ 
    double dx,           /* increment for taking numerical derivatives */
    long n_passes,       /* number of times to iterate */
    double _zero	 /* value acceptably close to true zero */
    )
{
    double f1, f2;
    double x1, x2;			
    double dfdx;
    long i;

    x1 = x2 = x_i;
    for (i=0; i<n_passes; i++) {
        if (fabs(f1 = (*fn)(x1)-value)<_zero)
            return(x1);
        if (i==n_passes-1)
            return((x1+x2)/2);
        f2 = (*fn)(x2=x1+dx)-value;
        dfdx = ((f2-f1)/dx);
        x1 += -f1/dfdx;
        }
    return(DBL_MAX);  /* shouldn't happen */
    }                                
        

