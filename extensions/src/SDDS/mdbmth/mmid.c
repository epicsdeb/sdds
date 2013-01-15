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
 Revision 1.4  2002/08/14 16:18:58  soliday
 Added Open License

 Revision 1.3  1999/07/29 21:26:24  borland
 Added mmid_odeint3_na() for nonadaptive integration with modified midpoint
 method.  Calling is the same as rk_odeint3_na().

 * Revision 1.2  1995/09/05  21:20:39  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: mmid.c
 * contents: mmid()
 *           mmid2()
 * purpose: modified midpoint method for integrating ODEs
 * Based on Numerical Recipes in C .
 * Michael Borland, 1995
 */
#include "mdb.h"

#define MAX_EXIT_ITERATIONS 400
#define ITER_FACTOR 0.995
#define TINY 1.0e-30

void mmid(
    double *yInitial,         /* starting values of dependent variables */
    double *dydxInitial,      /* and their derivatives */
    long equations,           /* number of equations */
    double xInitial,          /* starting value of independent variable */
    double interval,   /* size of interval in x */
    long steps,        /* number of steps to divide interval into */
    double *yFinal,      /* final values of dependent variables */
    /* function return derivatives */
    void (*derivs)(double *_dydx, double *_y, double _x)
    )
{
    long i, j;
    double x=0, ynSave, h, hTimes2;
    static double *ym, *yn;
    static long last_equations=0;
    double *dydxTemp;

    if (equations>last_equations) {
        if (last_equations) {
            free(ym);
            free(yn);
            }
        /* allocate arrays for solutions at two adjacent points in x */
        ym = tmalloc(sizeof(*ym)*equations);
        yn = tmalloc(sizeof(*yn)*equations);
        last_equations = equations;
        }
    
    hTimes2 = (h=interval/steps)*2;

    /* Copy starting values and compute first set of estimated values */
    for (i=0; i<equations; i++) {
        ym[i] = yInitial[i];
        yn[i] = yInitial[i] + h*dydxInitial[i];
        }

    dydxTemp = yFinal;   /* use yFinal for temporary storage */
    for (j=1; j<steps; j++) {
        x = xInitial + h*j;
        (*derivs)(dydxTemp, yn, x);
        for (i=0; i<equations; i++) {
            ynSave = yn[i];
            yn[i] = ym[i] + hTimes2*dydxTemp[i];
            ym[i] = ynSave;
            }
        }
    
    /* Compute final values */
    (*derivs)(dydxTemp, yn, x+interval);
    for (i=0; i<equations; i++)
        yFinal[i] = (ym[i] + yn[i] + h*dydxTemp[i])/2;
    }

void mmid2(
    double *y,    /* starting values of dependent variables */
    double *dydx, /* and their derivatives */
    long equations,    /* number of variables */
    double x0,    /* starting value of independent variable */
    double interval,  /* size of interval in x */
    long steps,   /* number of steps to divide interval into */
    double *yFinal, /* final values of dependent variables */
    /* function return derivatives */
    void (*derivs)(double *_dydx, double *_y, double _x)
    )
{
    static double *yFinal2;
    static long i, last_equations=0;

    if (steps%2)
        steps += 1;
    if (steps<8)
        steps = 8;

    if (equations>last_equations) {
        if (last_equations) {
            free(yFinal2);
            }
        /* allocate arrays for second solution */
        yFinal2 = tmalloc(sizeof(*yFinal2)*equations);
        last_equations = equations;
        }

    mmid(y, dydx, equations, x0, interval, steps, yFinal, derivs);
    mmid(y, dydx, equations, x0, interval, steps/2, yFinal2, derivs);
    for (i=0; i<equations; i++)
        yFinal[i] = (4*yFinal[i] - yFinal2[i])/3;
    }

/* routine: mmid_odeint3_na()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached, or
 *          until a user-supplied function is zero
 * returns: 0<= on failure, >=1 on success
 */

long mmid_odeint3_na(
    double *yif,         /* initial/final values of dependent variables */
    void (*derivs)(double *dydx, double *y, double x),/* (*derivs)(dydx, y, x) */
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,    /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,        /* ignored */
    long *misses,        /* ignored */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_step,        /* step size */
    double h_max,         /* ignored */
    double *h_rec,        /* ignored */
    /* function for determining when to stop integration: */
    double (*exit_func)(double *dydx, double *y, double x), 
            /* function that is to be zeroed */
    double exit_accuracy   /* how close to zero to get */
    )
{
    static double *y0, *yscale;
    static double *dydx0, *y1, *dydx1, *dydx2, *y2, *accur;
    static long last_neq = 0;
    double ex0, ex1, ex2, x1, x2;
    double xdiff;
    long i, n_exit_iterations;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (FABS(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);

    if (last_neq<n_eq) {
        if (last_neq!=0) {
            tfree(y0); tfree(dydx0); tfree(y1); tfree(dydx1); tfree(y2); tfree(dydx2); tfree(yscale); tfree(accur);
            }
        y0     = tmalloc(sizeof(double)*n_eq);
        dydx0  = tmalloc(sizeof(double)*n_eq);
        y1     = tmalloc(sizeof(double)*n_eq);
        dydx1  = tmalloc(sizeof(double)*n_eq);
        y2     = tmalloc(sizeof(double)*n_eq);
        dydx2  = tmalloc(sizeof(double)*n_eq);
        last_neq = n_eq;
        }

    for (i=0; i<n_eq; i++)
        y0[i] = yif[i];

    /* calculate derivatives and exit function at the initial point */
    (*derivs)(dydx0, y0, *x0);
    ex0 = (*exit_func)(dydx0, y0, *x0);

    do {
        /* check for zero of exit function */
        if (FABS(ex0)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                yif[i] = y0[i];
            return(DIFFEQ_ZERO_FOUND);
            }

        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_step)
            h_step = xdiff;
        /* take a step */
        x1 = *x0;
        mmid2(y0, dydx0, n_eq, x1, h_step, 8, y1, derivs);
        x1 += h_step;
        /* calculate derivatives and exit function at new point */
        (*derivs)(dydx1, y1, x1);
        ex1 = (*exit_func)(dydx1, y1, x1);
        if (SIGN(ex0)!=SIGN(ex1))
            break;
        /* check for end of interval */
        if (FABS(xdiff=xf-x1)<x_accuracy) {
            /* end of the interval */
            for (i=0; i<n_eq; i++)
                yif[i] = y1[i];
            *x0 = x1;
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        ex0 = ex1;
        *x0 = x1;
        } while (1);

    if (!exit_func) {
        printf("failure in mmid_odeint3_na():  solution stepped outside interval\n");
        return(DIFFEQ_OUTSIDE_INTERVAL);
        }

    if (FABS(ex1)<exit_accuracy) {
        for (i=0; i<n_eq; i++)
            yif[i] = y1[i];
        *x0 = x1;
        return(DIFFEQ_ZERO_FOUND);
        }

    /* The root has been bracketed. */
    n_exit_iterations = MAX_EXIT_ITERATIONS;
    do {
        /* try to take a step to the position where the zero is expected */
        h_step = -ex0*(x1-*x0)/(ex1-ex0)*ITER_FACTOR; 
        x2 = *x0;
        mmid2(y0, dydx0, n_eq, x2, h_step, 8, y2, derivs);
        x2 += h_step;
        /* check the exit function at the new position */
        (*derivs)(dydx2, y2, x2);
        ex2 = (*exit_func)(dydx2, y2, x2);
        if (FABS(ex2)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                yif[i] = y2[i];
            *x0 = x2;
            return(DIFFEQ_ZERO_FOUND);
            }
        /* rebracket the root */
        if (SIGN(ex1)==SIGN(ex2)) {
            SWAP_PTR(y1, y2);
            SWAP_PTR(dydx1, dydx2);
            x1 = x2;
            ex1 = ex2;
            }
        else {
            SWAP_PTR(y0, y2);
            SWAP_PTR(dydx0, dydx2);
            *x0 = x2;
            ex0 = ex2;
            }
        } while (n_exit_iterations--);
    return(DIFFEQ_EXIT_COND_FAILED);
    }

