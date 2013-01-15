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
 Revision 1.10  2009/12/02 22:20:41  soliday
 Added complex number support for non C99 compilers.

 Revision 1.9  2009/11/06 23:37:19  borland
 Removed routines that are in C99 library.

 Revision 1.8  2009/10/30 14:04:58  borland
 Fixed bug in clog(), which resulted in wrong imaginary part.

 Revision 1.7  2002/08/14 16:18:53  soliday
 Added Open License

 Revision 1.6  2001/10/22 23:04:19  borland
 Added procedure croot() for taking roots of complex numbers.

 Revision 1.5  1999/06/21 20:40:14  borland
 Moved complex_multiply and complex_divide from sddsconvolve.c

 Revision 1.4  1998/07/15 22:52:53  emery
 Removed function COMPLEX cassign_me since it has an argument
 of type CMATRIX which isn't defined complex.h

 Revision 1.3  1998/04/13 22:07:14  emery
 Added functions csroot and cassign_me.

 * Revision 1.2  1995/09/05  21:19:25  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: complex2.c
 * contents: ...
 *
 * Initial coding: Michael Borland, April 1986. 
 * Redone using structure returns instead of pointers, 11/92
 */

#include <complex>
#include "mdb.h"

std::complex <double> complexErf(std::complex <double> z, long *flag)
{
  double xi, yi;
  double u, v;
  long lflag;
  xi = z.real();
  yi = z.imag();
  wofz(&xi, &yi, &u, &v, &lflag);
  *flag = lflag;
  return std::complex <double> (u , v);
}

std::complex <double> cexpi(double p)
{
  std::complex <double> a;
  a = cos(p) + std::complex<double>(0,1)*sin(p);
  return(a);
}

std::complex <double> cipowr(std::complex <double> a, int n)
{
  int i;
  std::complex <double> p(1,0);
    
  if (n>=0) {
    for (i=0; i<n; i++)
      p = p*a;
    return(p);
  }
  a = p/a;
  return(cipowr(a, -n));
}

/* These routines are obsolete, really, but some code uses them. */

void complex_multiply(
                      double *r0, double *i0,    /* result */
                      double  r1, double  i1,
                      double  r2, double  i2
                      )
{
  double tempr;

  tempr = r1*r2 - i1*i2;
  *i0   = r1*i2 + i1*r2;
  *r0   = tempr;
}

void complex_divide(
                    double *r0, double *i0,    /* result */
                    double  r1, double  i1,
                    double  r2, double  i2,
                    double threshold
                    )
{
  double tempr, denom;

  if ((denom = sqr(r2)+sqr(i2))<threshold)
    denom = threshold;
  i2 = -i2;
  tempr = (r1*r2 - i1*i2)/denom;
  *i0   = (r1*i2 + i1*r2)/denom;
  *r0   = tempr;
}

