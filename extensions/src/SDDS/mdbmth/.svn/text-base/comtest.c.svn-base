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
 Revision 1.6  2009/10/30 14:07:05  borland
 Added test for clog().

 Revision 1.5  2003/06/18 19:52:32  borland
 Removed the query() and query_e() macros, and replaced with queryn()
 and queryn_e() macros.  The former used gets() whereas the latter do
 not.

 Revision 1.4  2002/08/14 16:18:53  soliday
 Added Open License

 Revision 1.3  2001/10/22 23:04:45  borland
 Fixed random number generation and added tests of croot() and csroot().

 * Revision 1.2  1995/09/05  21:19:27  saunders
 * First test release of the SDDS1.5 package.
 *
*/

#include "mdb.h"
#include <complex.h>

#define MAXRAND 2147483647.

int set_drand(double low, double high, int seed);
double complex crand(); 
double complex cexpi(double x);
double complex cipowr(double complex a, int n);

int main(void)
{
  double complex a, b, c, d, e, f;
  int seed;
  int i, n;
  char s[100];
    
  n = query_int("number of points", 10);
  a = 0;
  for (i=0; i<n; i++)
    a = a + cexpi(2*PI*i/n);
  printf("SUM exp(i*2*PI*j/n) = %lf, %lf\n", creal(a), cimag(a));

  queryn("seed: ", s, 100);
  sscanf(s, "%lf", &seed);
  set_drand(-10.0, 10.0, seed);
  while (1) {
    a = crand(); 
    b = crand();
    c = crand();
    printf("a = %lf, %lf\n", creal(a), cimag(a));
    printf("b = %lf, %lf\n", creal(b), cimag(b));
    printf("c = %lf, %lf\n", creal(c), cimag(c));
    e = cipowr(csin(a), 2) + cipowr(ccos(a), 2);
    printf("1 = %lf, %lf\n", creal(e), cimag(e));
    e = a/b;
    printf("a/b = %lf, %lf\n", creal(e), cimag(e));
    f = c/b;
    printf("c/b = %lf, %lf\n", creal(f), cimag(f));
    d = e/f;
    printf("d = %lf, %lf\n", creal(d), cimag(d));
    f = a/c - d;
    printf("0 = %lf, %lf\n", creal(f), cimag(f));
    a = clog(-3*I);
    printf("log(-3*i) = %lf, %lf\n", creal(a), cimag(a));
    a = cexpi(PI);
    printf("exp(i*pi) = %lf, %lf\n", creal(a), cimag(a));
    a = cexpi(PI/2);
    printf("exp(i*pi/2) = %lf, %lf\n", creal(a), cimag(a));
    a = cexpi(PI)*cexpi(PI/2)*cexpi(PI/2);
    printf("exp(i*pi)*(exp(i*pi/2)^2) = %lf, %lf\n", creal(a), cimag(a));
    a = cipowr(csqrt(b), 2)-b;
    printf("(sqrt(X))^2-X = %lf, %lf\n", creal(a), cimag(a));
    a = cipowr(cpow(b, 1./3.), 3) - b;
    printf("(cuberoot(X))^3-X = %lf, %lf\n", creal(a), cimag(a));
    getchar();
    }
  }
        

double low, high;

int set_drand(double l, double h, int seed)
{
  srand(seed);
  low = l;
  high = h;
}

double complex crand()
{
    double complex c;
    c = rdrand(low, high) + I*rdrand(low, high);
    return(c);
    }
