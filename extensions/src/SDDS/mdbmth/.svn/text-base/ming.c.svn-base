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
 Revision 1.2  1995/09/05 21:20:37  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: ming()
 * purpose: find minimum of a multi-parameter function, using a grid
 *          search
 *
 * Michael Borland, 1989
 */
#include "mdb.h"

double ming(
    double (*fn)(),      /* pointer to fn to be minimize */
    double *x,           /* array to return location of minimum in */
    double *xlo,         /* array of lower limits */
    double *xhi,         /* array of upper limits */
    long *n_steps,       /* array of numbers of steps */
    long np              /* number of parameters */
    )
{
    register double f0;
    double *dx, *xbest, min_val;
    long i, *counter;

    xbest   = tmalloc(sizeof(double)*np);
    dx      = tmalloc(sizeof(double)*np);
    counter = tmalloc(sizeof(long)*np);
    for (i=0; i<np; i++) {
        xbest[i] = x[i] = xlo[i];
        dx[i] = (xhi[i]-xlo[i])/(n_steps[i]-1);
        counter[i]  = 0;
        }
    min_val = (*fn)(xbest);
  
    while (advance_values(x, counter, n_steps, xlo, dx, np)) {
        if ((f0=(*fn)(x))<min_val) {
            for (i=0; i<np; i++)
                xbest[i] = x[i];
            min_val = f0;
            }
        }
    for (i=0; i<np; i++)
        x[i] = xbest[i];
    return(min_val);
    }
  
/* routine: advance_values()
 * purpose: sequence an array of values in a systematic way such that
 *          a specified n-dimensional grid is covered.
 *
 * Michael Borland, 1988.
 */

long advance_values(
    double *value, 
    long *counter,
    long *max_count,
    double *lower,
    double *step_size,
    long n_values
    )
{
    long i;

    for (i=0; i<n_values; i++)
        if (counter[i]!=(max_count[i]-1))
            break;
    if (i==n_values)
        return 0;

    for (i=0; i<n_values; i++) {
        if (counter[i]<(max_count[i]-1)) {
            counter[i]++;
            value[i] += step_size[i];
            break;
            }
        else {
            counter[i] = 0;
            value[i]   = lower[i];
            }
        }
    return 1;
    }

