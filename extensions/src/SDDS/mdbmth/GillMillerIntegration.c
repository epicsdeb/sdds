/*************************************************************************\
* Copyright (c) 2011 The University of Chicago LLC, as Operator of Argonne
* National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: not supported by cvs2svn $
 Revision 1.1  2011/08/11 01:06:15  borland
 First version.  Didn't check error computation.

*/

/* routine: GillMillerIntegration()
 * purpose: integrate a function f(x) specified as arrays of x and f.
 * Based on P. E. Gill and G. F. Miller, The Computer Journal, Vol 15, No. 1, 80-83, 1972.
 * Michael Borland, 2011
 */

#include "mdb.h"

int GillMillerIntegration(double *integral, double *error, 
                          double *f, double *x, long n)
{
  double e, de1, de2;
  double h1, h2, h3, h4, r1, r2, r3, r4, d1, d2, d3, c, s, integ, dinteg1, dinteg2;
  long i, k;
  
  if (!integral || !f || !x)
    return 1;
  if (n<=1)
    return 2;

  integ = e = s = c = r4 = 0;
  n -= 1;  /* for consistency with GM paper */
  
  integral[0] = 0;
  if (error) 
    error[0] = 0;

  k = n - 1;
  dinteg2 = 0;
  de2 = 0;

  for (i=2; i<=k; i++) {
    dinteg1 = 0;
    if (i==2) {
      h2 = x[i-1] - x[i-2];
      d3 = (f[i-1] - f[i-2])/h2;
      h3 = x[i] - x[i-1];
      d1 = (f[i] - f[i-1])/h3;
      h1 = h2 + h3;
      d2 = (d1 - d3)/h1;
      h4 = x[i+1] - x[i];
      r1 = (f[i+1] - f[i])/h4;
      r2 = (r1 - d1)/(h4 + h3);
      h1 = h1 + h4;
      r3 = (r2 - d2)/h1;
      integ = h2*(f[0] + h2*(d3/2.0 - h2*(d2/6 - (h2 + 2*h3)*r3/12)));
      s = -ipow(h2,3)*(h2*(3*h2 +5*h4)+10*h3*h1)/60.0;
      integral[i-1] = integ;
      if (error)
        error[i-1] = 0;
    } else {
      h4 = x[i+1] - x[i];
      r1 = (f[i+1] - f[i])/h4;
      r4 = h4 + h3;
      r2 = (r1 - d1)/r4;
      r4 = r4 + h2;
      r3 = (r2 - d2)/r4;
      r4 = r4 + h1;
      r4 = (r3 - d3)/r4;
    }

    dinteg1 = h3*((f[i] + f[i-1])/2 - h3*h3*(d2 + r2 + (h2 - h4)*r3)/12.0);
    c = ipow(h3,3)*(2*h3*h3 + 5*(h3*(h4+h2) + 2*h4*h2))/120;
    de1 = (c+s)*r4;
    s = (i==2 ? 2*c + s : c);

    if (i==2)
      integral[i] = integ+dinteg1+e+de1;
    else 
      integral[i] = integ+dinteg2+e+de2;
    integ += dinteg1;

    if (error) {
      if (i==2)
	error[i] = e+de1;
      else
	error[i] = e+de2;
    }
    e += de1;

    dinteg2 = h4*(f[i+1] - h4*(r1/2 + h4*(r2/6 + (2*h3+h4)*r3/12)));
    de2 = s*r4 - ipow(h4,3)*r4*(h4*(3*h4 + 5*h2) + 10*h3*(h2 + h3 + h4))/60;

    if (i==k) {
      integ += dinteg2; 
      e += de2;
    } else {
      h1 = h2;
      h2 = h3;
      h3 = h4;
      d1 = r1;
      d2 = r2;
      d3 = r3;
    }

  }

  integral[i] = integ+e;
  if (error)
    error[i] = e;

  return 0;
}			













