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
 Revision 1.1  2000/04/11 16:21:55  soliday
 Moved here from mdbmth.

 Revision 1.2  1995/09/05 21:20:09  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: interpolate_minimum()
 * purpose: do quadratic interpolation to find the minimum value
 *          of a function given as a list of values at equally-spaced
 *          points.
 *
 * Michael Borland, 1988
 */
#include "mdb.h"
#include "matlib.h"
 
int interpolate_minimum(
    double *fmin,         /* for returning interpolated minimum */
    double *zmin,         /* for returning position of interpolated minimum */
    double *value,        /* array of function values at z_lo+dz*i */
    double z_lo, double z_hi,    /* z for value[0], value[n_values-1], respectively */
    long n_values         /* number of values in array value */
    )
{
    double f1, f2, f3, z1, z2, z3, dz, a, b, c;
    register long i, i_min_value;
    MATRIX *Z, *Zi, *A, *F;

    /* first find the minimum value in the array */
    f1 = DBL_MAX;
    i_min_value = -1;
    for (i=0; i<n_values; i++)
        if ((f2=value[i])<f1) {
            f1 = f2;
            i_min_value = i;
            }

    if (i_min_value==-1)
        return(0);
    
    if (i_min_value==0 || i_min_value==n_values-1) {
        *fmin = value[i_min_value];
        if (n_values>=2) 
            *zmin = i_min_value*(z_hi-z_lo)/(n_values-1)+z_lo;
        else if (i_min_value==0)
            *zmin = z_lo;
        else 
            *zmin = z_hi;
        return(1);
        }
        
    /* interpolate to find minimum */

    dz = (z_hi-z_lo)/(n_values-1);
    f1 = value[--i_min_value];
    z1 = dz*i_min_value + z_lo;
    f2 = value[++i_min_value];
    z2 = dz*i_min_value + z_lo;
    f3 = value[++i_min_value];
    z3 = dz*i_min_value + z_lo;
    
    /* set up matrix problem to solve for parabola that fits the points
     * (z1, f1), (z2, f2), (z3, f3).
     */
    m_alloc(&Z, 3, 3);
    m_alloc(&Zi, 3, 3);
    m_alloc(&F, 3, 1);
    m_alloc(&A, 3, 1);

    Z->a[0][0] = z1*z1;
    Z->a[0][1] = z1;
    Z->a[0][2] = 1;

    Z->a[1][0] = z2*z2;
    Z->a[1][1] = z2;
    Z->a[1][2] = 1;

    Z->a[2][0] = z3*z3;
    Z->a[2][1] = z3;
    Z->a[2][2] = 1;

    F->a[0][0] = f1;
    F->a[1][0] = f2;
    F->a[2][0] = f3;

    m_invert(Zi, Z);
    m_mult(A, Zi, F);

    /* f = a.z^2 + b.z + c */
    a = A->a[0][0];
    b = A->a[1][0];
    c = A->a[2][0];

    *zmin = -b/(2*a);
    *fmin = a*sqr(*zmin) + b*(*zmin) + c;
    return(1);
    }

