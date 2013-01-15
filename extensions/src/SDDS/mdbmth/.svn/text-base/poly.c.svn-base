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
 Revision 1.4  2002/08/14 16:18:59  soliday
 Added Open License

 Revision 1.3  2000/04/05 22:18:13  soliday
 Renamed poly when using Borland C. It conflicted with a built-in function.

 Revision 1.2  1995/09/05 21:20:43  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: poly()
 * purpose: evaluate polynomial given array of coefficients
 *
 * Michael Borland, 1987.
 */
#include "mdb.h"

#if defined(__BORLANDC__)
double poly2(double *a, long n, double x)
#else
double poly(double *a, long n, double x)
#endif
{
    register long i;
    register double sum, xp;

    sum = 0;
    xp  = 1;
    for (i=0; i<n; i++) {
        sum += xp*a[i];
        xp *= x;
        }
    return(sum);
    }

/* routine: dpoly()
 * purpose: evaluate the derivative of a polynomial given array of coefficients
 *
 * Michael Borland, 1989.
 */


double dpoly(double *a, long n, double x)
{
    register long i;
    register double sum, xp;

    sum = 0;
    xp  = 1;
    for (i=1; i<n; i++) {
        sum += i*xp*a[i];
        xp *= x;
        }
    return(sum);
    }

/* routine: polyp()
 * purpose: evaluate polynomial given array of coefficients and powers
 *
 * Michael Borland, 1989.
 */

double polyp(double *a, long *power, long n, double x)
{
    register long i;
    register double sum, xp;

    xp  = ipow(x, power[0]);
    sum = xp*a[0];
    for (i=1; i<n; i++) {
        xp  *= ipow(x, power[i]-power[i-1]);
        sum += xp*a[i];
        }
    return(sum);
    }

/* routine: dpolyp()
 * purpose: evaluate the derivative of a polynomial given array of coefficients
 *          and powers
 * Michael Borland, 1989.
 */


double dpolyp(double *a, long *power, long n, double x)
{
    register long i;
    register double sum, xp;

    xp  = ipow(x, power[0]-1);
    sum = power[0]*xp*a[0];
    for (i=1; i<n; i++) {
        xp  *= ipow(x, power[i]-power[i-1]);
        sum += power[i]*xp*a[i];
        }
    return(sum);
    }

/* routine: solveQuadratic
 * purpose: return real solutions to quadratic equation
 * 
 * M. Borland, 2003
 */

int solveQuadratic(double a, double b, double c, double *solution)
{
  double det;
  if (a==0) {
    if (b==0)
      return 0;
    solution[0] = -c/b;
    return 1;
  }
  if ((det = b*b-4*a*c)<0)
    return 0;
  if (det==0) {
    solution[0] = -b/a;
    return 1;
  }
  solution[0] = (-b - sqrt(det))/(2*a);
  solution[1] = (-b + sqrt(det))/(2*a);
  return 2;
}

