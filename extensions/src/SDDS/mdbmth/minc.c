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
 Revision 1.2  1995/09/05 21:20:35  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: minc()
 * purpose: find minimum of a multi-parameter function, with constraints
 *          allowed on the range of the parameters
 *
 * Michael Borland, 1989
 */
#include "mdb.h"

double minc(fn, x, dx, dx_lim, xlo, xhi, np, ns_max, p_flag)
double (*fn)();      /* pointer to fn to be minimize */
double *x;           /* array of starting values of parameters */
double *dx;          /* array of step sizes */
double *dx_lim;      /* array of step size limits */
double *xlo;         /* array of lower limits */
double *xhi;         /* array of upper limits */
long np;             /* number of parameters */
long ns_max;         /* number of steps to take before increasing dx */
long p_flag;         /* if >= 0, information is printed during minimization */
{
  register long i, n_steps, pc;
  register double f0, f1, _dx;
  long flag, *constraint, at_upper, at_lower;

  pc = 0;
  constraint = tmalloc(sizeof(long)*np);
  for (i=0; i<np; i++)
    constraint[i] = xlo[i]!=xhi[i];

  f0 = (*fn)(x);

  do {
    for (i=flag=0; i<np; i++) {
      if (fabs(_dx=dx[i])<dx_lim[i]) {
        flag++;
        continue;
        }
      x[i] += _dx;
      if (constraint[i]) {
        if (x[i]<xlo[i]) {
          x[i] = xlo[i] + 2*(dx[i] = fabs(_dx)/2);
          continue;
          }
        if (x[i]>xhi[i]) {
          x[i] = xhi[i] + 2*(dx[i] = -fabs(_dx)/2);
          continue;
          }
        }
      f1 = (*fn)(x);
      n_steps = 0;
      if (f1>f0) {
        dx[i] = _dx = -_dx;
        x[i]  += 2*_dx;
        if (constraint[i]) {
          if (x[i]<xlo[i]) {
            x[i] = xlo[i] + 2*(dx[i] = fabs(_dx)/2);
            continue;
            }
          if (x[i]>xhi[i]) {
            x[i] = xhi[i] + 2*(dx[i] = -fabs(_dx)/2);
            continue;
            }
          }
        f1 = (*fn)(x);
        }
      while (f1<f0) {
        if (n_steps++==ns_max) {
          n_steps = 0;
          dx[i] = _dx = 2*_dx;
          }
        f0 = f1;
        x[i] += _dx;
        if (constraint[i]) {
          if (x[i]<xlo[i]) {
            x[i] = xlo[i] + 2*(dx[i] = fabs(_dx));
            break;
            }
          if (x[i]>xhi[i]) {
            x[i] = xhi[i] + 2*(dx[i] = -fabs(_dx));
            break;
            }
          }
        f1 = (*fn)(x);
        }
      dx[i] /= 2;
      x[i] -= _dx;
      }
    if (pc++ == p_flag) {
      printf("%.16le\n", f0);
      for (i=0; i<np; i++) 
        printf("%.16le\t%.16le\n", x[i], dx[i]);
      pc = 0;
      }
    } while (flag!=np);
  return(f0);
  }

