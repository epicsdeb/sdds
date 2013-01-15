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
 Revision 1.4  2002/08/14 16:18:57  soliday
 Added Open License

 Revision 1.3  1999/05/28 14:58:17  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:20:19  saunders
 First test release of the SDDS1.5 package.

*/

/* file: lsf_basis_fns.c
 * contents: basis functions for LSFs with ordinary polynomials and Chebyshev polynomials.
 *           A basis function is of the form func(value, order), where order is an integer.
 *                    
 */
#include "mdb.h"

static double x_offset = 0;
static double x_scale = 1;

void set_argument_offset(double offset)
{
    x_offset = offset;
    }

void set_argument_scale(double scale)
{
    if (!(x_scale = scale))
        bomb("argument scale factor is zero", NULL);
    }

double tcheby(double x, long n)
{
    x = (x-x_offset)/x_scale;
    if (x>1 || x<-1) {
        fprintf(stderr, "warning: argument %e is out of range for tcheby()\n", x);
        x = SIGN(x);
        }
    return(cos(n*acos(x)));
    }

double dtcheby(double x, long n)
{
    x = (x-x_offset)/x_scale;
    if (x>1 || x<-1) {
        fprintf(stderr, "warning: argument %e is out of range for tcheby()\n", x);
        x = SIGN(x);
        }
    if (x!=1 && x!=-1)
        return(n*sin(n*acos(x))/sqrt(1-sqr(x)));
    return(1.0*n*n);
    }

double ipower(double x, long n)
{
    x = (x-x_offset)/x_scale; 
    return(ipow(x, n));
    }
   
double dipower(double x, long n)
{
    x = (x-x_offset)/x_scale;
    return(n*ipow(x, n-1));
    }

double eval_sum(double (*fn)(double x, long ord), double *coef, int32_t *order, long n_coefs, double x0)
{
    double sum = 0;
    long i;

    for (i=sum=0; i<n_coefs; i++)
        sum += (fn)(x0, order[i])*coef[i];
    return(sum);
    }



