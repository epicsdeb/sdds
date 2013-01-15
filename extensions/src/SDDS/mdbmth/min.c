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
 Revision 1.2  1995/09/05 21:20:33  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: min()
 * purpose: find minimum of a multi-parameter function.
 *
 * Michael Borland, 1987
 */
#include "mdb.h"

double min(fn, x, dx, dx_lim, np, ns_max, p_flag)
double (*fn)();      /* pointer to fn to be minimize */
double *x;           /* array of starting values of parameters */
double *dx;          /* array of step sizes */
double *dx_lim;      /* array of step size limits */
long np;              /* number of parameters */
long ns_max;          /* number of steps to take before increasing dx */
long p_flag;          /* if >= 0, information is printed during minimization */
{
  register long i, n_steps, pc;
  register double f0, f1, _dx;
  long flag;

  f0 = (*fn)(x);

  pc = 0;
  do {
    for (i=flag=0; i<np; i++) {
      if (fabs(_dx=dx[i])<dx_lim[i]) {
        flag++;
        continue;
        }
      x[i] += _dx;
      f1 = (*fn)(x);
      n_steps = 0;
      if (f1>f0) {
        dx[i] = _dx = -_dx;
        x[i]  += 2*_dx;
        f1 = (*fn)(x);
        }
      while (f1<f0) {
        if (n_steps++==ns_max) {
          n_steps = 0;
          dx[i] = _dx = 2*_dx;
          }
        f0 = f1;
        x[i] += _dx;
        f1 = (*fn)(x);
        }
      dx[i] /= 2;
      x[i] -= _dx;
      }
    if (pc++ == p_flag) {
      printf("%.16le\n", f0);
      for (i=0; i<np; i++) 
        printf("%.16le\t%.16le\n",
          x[i], dx[i]);
      pc = 0;
      }
    } while (flag!=np);
  return(f0);
  }

