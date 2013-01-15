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
 Revision 1.4  2002/08/14 16:18:53  soliday
 Added Open License

 Revision 1.3  1999/05/28 14:59:54  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:19:23  saunders
 First test release of the SDDS1.5 package.

*/

/* file: bs_ode.c
 * contents: bs_step()
 *           bs_odeint(), bs_odeint1(), bs_odeint2(), bs_odeint3(), bs_odeint4()
 *
 * purpose: Bulirsch-Stoer method for ODE's using polynomial extrapolation
 *
 * Michael Borland, 1990
 */
#include "mdb.h"

void mmid(double *y, double *dydx, long nvar, double xs, double htot,
    long nstep, double *yout, void (*derivs)(double *dydx, double *y, double x)
    );
void new_scale_factors_dp(double *yscale, double *y0, double *dydx0, 
    double h_start, double *tiny, long *accmode, double *accuracy, 
    long n_eq);
void initial_scale_factors_dp(double *yscale, double *y0, double *dydx0, 
    double h_start, double *tiny, long *accmode, double *accuracy,
    double *accur, double x0, double xf, long n_eq);
long bs_step(double *yf, double *xx, double *yi, double *dydx,
    double htry, double *hdid, double *hnext, double *yscal, long nv,
    void (*derivs)(double *dydx, double *y, double x), long *misses );


/* routine: bs_odeint()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached, or
 *          until a user-supplied function is zero
 * returns: 0<= on failure, >=1 on success
 */
#define TINY 1.0e-30

long bs_odeint(
    double *y0,           /* initial/final values of dependent variables */
    void (*derivs)(double *dydx, double *y, double x),    /* (*derivs)(dydx, y, x) */
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,     /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,         /* small value relative to what's important */
    long *misses,        /* number of times each variable caused reset
                            of step size */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_start,       /* suggested starting step size */
    double h_max,         /* maximum step size allowed */
    double *h_rec,        /* recommended step size for continuation */
    /* function for determining when to stop integration: */
    double (*exit_func)(double *dydx, double *y, double x), 
            /* function that is to be zeroed */
    double exit_accuracy,  /* how close to zero to get */
    long n_to_skip,       /* number of zeros of exit function to skip before
                             returning */
    void (*store_data)(double *dydx, double *y, double x, double exf)  /* function to store points */
    )
{
    double *y_return, *accur;
    double *dydx0, *y1, *dydx1, *dydx2, *y2;
    double ex0, ex1, ex2, x1, x2, *yscale;
    double h_used, h_next, xdiff;
    long i, n_step_ups=0, is_zero;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (FABS(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);

    /* Meaning of accmode:
     * accmode = 0 -> accuracy[i] is desired fractional accuracy at
     *                each step for ith variable.  tiny[i] is lower limit 
     *                of significance for the ith variable.
     * accmode = 1 -> same as accmode=0, except that the accuracy is to be
     *                satisfied globally, not locally.
     * accmode = 2 -> accuracy[i] is the desired absolute accuracy per
     *                step for the ith variable.  tiny[i] is ignored.
     * accmode = 3 -> samed as accmode=2, except that the accuracy is to 
     *                be satisfied globally, not locally.
     */
    for (i=0; i<n_eq; i++) {
        if (accmode[i]<0 || accmode[i]>3)
            bomb("accmode must be on [0, 3] (bs_odeint)", NULL);
        if (accmode[i]<2 && tiny[i]<TINY)
            tiny[i] = TINY;
        misses[i] = 0;
        }

    y_return = y0;
    dydx0  = tmalloc(sizeof(double)*n_eq);
    y1     = tmalloc(sizeof(double)*n_eq);
    dydx1  = tmalloc(sizeof(double)*n_eq);
    y2     = tmalloc(sizeof(double)*n_eq);
    dydx2  = tmalloc(sizeof(double)*n_eq);
    yscale = tmalloc(sizeof(double)*n_eq);

    /* calculate derivatives and exit function at the initial point */
    (*derivs)(dydx0, y0, *x0);

    /* set the scales for evaluating accuracy.  yscale[i] is the
     * absolute level of accuracy required of the next integration step
     */
    accur  = tmalloc(sizeof(double)*n_eq);
    initial_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accuracy, accur, *x0, xf, n_eq);

    ex0 = exit_func?(*exit_func)(dydx0, y0, *x0):0;
    if (store_data)
        (*store_data)(dydx0, y0, *x0, ex0);
    is_zero = 0;

    do {
        /* check for zero of exit function */
        if (exit_func && FABS(ex0)<exit_accuracy) {
	    if (!is_zero) {
                if (n_to_skip==0) {
                    if (store_data) 
                        (*store_data)(dydx0, y0, *x0, ex0);
                    for (i=0; i<n_eq; i++)
                        y_return[i] = y0[i];
                    *h_rec = h_start;
                    tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
                    tfree(accur);
                    if (y0!=y_return) tfree(y0);
                    if (y1!=y_return) tfree(y1);
                    if (y2!=y_return) tfree(y2);
                    return(DIFFEQ_ZERO_FOUND);
                    }
                else {
                    is_zero = 1;
                    --n_to_skip;
                    }
                }
            }
        else
            is_zero = 0;
        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_start)
            h_start = xdiff;
        /* take a step */
        x1 = *x0;
        if (!bs_step(y1, &x1, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses)) {
            if (n_step_ups++ > MAX_N_STEP_UPS)
                bomb("error: cannot take initial step (bs_odeint--1)", NULL);
            h_start = (n_step_ups-1?h_start*10:h_used*10);
            continue;
            }
        /* calculate derivatives and exit function at new point */
        (*derivs)(dydx1, y1, x1);
        ex1 = exit_func?(*exit_func)(dydx1, y1, x1):0;
        if (store_data)
            (*store_data)(dydx1, y1, x1, ex1);
        /* check for change in sign of exit function */
        if (exit_func && SIGN(ex0)!=SIGN(ex1) && !is_zero) {
            if (n_to_skip==0)
                break;
            else {
                --n_to_skip;
                is_zero = 1;
                }
            }
        /* check for end of interval */
        if (FABS(xdiff=xf-x1)<x_accuracy) {
            /* end of the interval */
            if (store_data) {
                (*derivs)(dydx1, y1, x1);
                ex1 = exit_func?(*exit_func)(dydx1, y1, x1):0;
                (*store_data)(dydx1, y1, x1, ex1);
                }
            for (i=0; i<n_eq; i++)
                y_return[i] = y1[i];
            *x0 = x1;
            *h_rec = h_start;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        ex0 = ex1;
        *x0 = x1;
        /* adjust the step size as recommended by bs_step() */
        h_start = (h_next>h_max?(h_max?h_max:h_next):h_next);
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        } while (1);
    *h_rec = h_start;

    if (!exit_func) {
        printf("failure in bs_odeint():  solution stepped outside interval\n");
        tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
        tfree(accur);
        if (y0!=y_return) tfree(y0);
        if (y1!=y_return) tfree(y1);
        if (y2!=y_return) tfree(y2);
        return(DIFFEQ_OUTSIDE_INTERVAL);
        }

    if (FABS(ex1)<exit_accuracy) {
        for (i=0; i<n_eq; i++)
            y_return[i] = y1[i];
        *x0 = x1;
        tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
        tfree(accur);
        if (y0!=y_return) tfree(y0);
        if (y1!=y_return) tfree(y1);
        if (y2!=y_return) tfree(y2);
        return(DIFFEQ_ZERO_FOUND);
        }

    /* The root has been bracketed. */
    do {
        /* try to take a step to the position where the zero is expected */
        h_start = -ex0*(x1-*x0)/(ex1-ex0+TINY);
        x2 = *x0;
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        if (!bs_step(y2, &x2, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses))
            bomb("step size too small (bs_odeint--2)", NULL);
        /* check the exit function at the new position */
        (*derivs)(dydx2, y2, x2);
        ex2 = (*exit_func)(dydx2, y2, x2);
        if (FABS(ex2)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                y_return[i] = y2[i];
            *x0 = x2;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
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
        } while (1);
    }


/* routine: bs_odeint1()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached
 * note: this is the same as bs_odeint(), but doesn't look for the zero
 *       of a function of the solution, or provide intermediate output
 *       It is about 20% faster than bs_odeint() called with NULL's for
 *       exit_func and store_data.
 *
 * returns: 0<= on failure, 1 on success
 */

long bs_odeint1(
    double *y0,           /* initial/final values of dependent variables */
    void (*derivs)(),    /* (*derivs)(dydx, y, x) */
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,     /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,         /* small value relative to what's important */
    long *misses,        /* number of times each variable caused reset
                            of step size */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_start,       /* suggested starting step size */
    double h_max,         /* maximum step size allowed */
    double *h_rec         /* recommended step size for continuation */
    )
{
    double *y_return;
    double *dydx0, *y1, *dydx1, *dydx2, *y2;
    double x1, *yscale, *accur;
    double h_used, h_next, xdiff;
    long i, n_step_ups=0;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (fabs(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);

    /* Meaning of accmode:
     * accmode = 0 -> accuracy[i] is desired fractional accuracy at
     *                each step for ith variable.  tiny[i] is lower limit 
     *                of significance for the ith variable.
     * accmode = 1 -> same as accmode=0, except that the accuracy is to be
     *                satisfied globally, not locally.
     * accmode = 2 -> accuracy[i] is the desired absolute accuracy per
     *                step for the ith variable.  tiny[i] is ignored.
     * accmode = 3 -> samed as accmode=2, except that the accuracy is to 
     *                be satisfied globally, not locally.
     */
    for (i=0; i<n_eq; i++) {
        if (accmode[i]<0 || accmode[i]>3)
            bomb("accmode must be on [0, 3] (bs_odeint)", NULL);
        if (accmode[i]<2 && tiny[i]<TINY)
            tiny[i] = TINY;
        misses[i] = 0;
        }

    y_return = y0;
    dydx0  = tmalloc(sizeof(double)*n_eq);
    y1     = tmalloc(sizeof(double)*n_eq);
    dydx1  = tmalloc(sizeof(double)*n_eq);
    y2     = tmalloc(sizeof(double)*n_eq);
    dydx2  = tmalloc(sizeof(double)*n_eq);
    yscale = tmalloc(sizeof(double)*n_eq);

    /* calculate derivatives at the initial point */
    (*derivs)(dydx0, y0, *x0);

    /* set the scales for evaluating accuracy.  yscale[i] is the
     * absolute level of accuracy required of the next integration step
     */
    accur  = tmalloc(sizeof(double)*n_eq);
    initial_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accuracy, accur, *x0, xf, n_eq);

    do {
        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_start)
            h_start = xdiff;
        /* take a step */
        x1 = *x0;
        if (!bs_step(y1, &x1, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses)) {
            if (n_step_ups++ > MAX_N_STEP_UPS) 
                bomb("error: cannot take initial step (bs_odeint1--1)", NULL);
            h_start = (n_step_ups-1?h_start*10:h_used*10);
            continue;
            }
        /* check for end of interval */
        if (fabs(xdiff=xf-x1)<x_accuracy) {
            /* end of the interval */
            for (i=0; i<n_eq; i++)
                y_return[i] = y1[i];
            *x0 = x1;
            *h_rec = h_start;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* calculate derivatives at new point */
        (*derivs)(dydx1, y1, x1);
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        *x0 = x1;
        /* adjust the step size as recommended by bs_step() */
        h_start = (h_next>h_max?(h_max?h_max:h_next):h_next);
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        } while (1);
    }


/* routine: bs_odeint2()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached or
 *          until a specified component of the solution reaches a 
 *          specified within a specified accuracy
 * note: this is the same as bs_odeint(), but doesn't look for the zero
 *       of a function of the solution, or provide intermediate output.
 *       It integrates until a specified component of the solution reaches
 *       a specified value.
 *       It is about 20% faster than bs_odeint() called with NULL's for
 *       exit_func and store_data.
 *
 * returns: 0<= on failure, 1 on success
 */

long bs_odeint2(
    double *y0,           /* initial/final values of dependent variables */
    /* (*derivs)(dydx, y, x): */
    void (*derivs)(double *dydx, double *y, double x),    
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,     /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,         /* small value relative to what's important */
    long *misses,        /* number of times each variable caused reset
                            of step size */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_start,       /* suggested starting step size */
    double h_max,         /* maximum step size allowed */
    double *h_rec,        /* recommended step size for continuation */
    /* for determining when to stop integration: */
    double exit_value,    /* value to be obtained */
    long i_exit_value,   /* index of independent variable this pertains to */
    double exit_accuracy,  /* how close to get */
    long n_to_skip        /* number of zeros to skip before returning */
    )
{
    double *y_return, *accur;
    double *dydx0, *y1, *dydx1, *dydx2, *y2;
    double ex0, ex1, ex2, x1, x2, *yscale;
    double h_used, h_next, xdiff;
    long i, n_step_ups=0, is_zero;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (fabs(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);
    if (i_exit_value<0 || i_exit_value>=n_eq)
        bomb("index of variable for exit testing is out of range (bs_odeint2)", NULL);

    /* Meaning of accmode:
     * accmode = 0 -> accuracy[i] is desired fractional accuracy at
     *                each step for ith variable.  tiny[i] is lower limit 
     *                of significance for the ith variable.
     * accmode = 1 -> same as accmode=0, except that the accuracy is to be
     *                satisfied globally, not locally.
     * accmode = 2 -> accuracy[i] is the desired absolute accuracy per
     *                step for the ith variable.  tiny[i] is ignored.
     * accmode = 3 -> samed as accmode=2, except that the accuracy is to 
     *                be satisfied globally, not locally.
     */
    for (i=0; i<n_eq; i++) {
        if (accmode[i]<0 || accmode[i]>3)
            bomb("accmode must be on [0, 3] (bs_odeint2)", NULL);
        if (accmode[i]<2 && tiny[i]<TINY)
            tiny[i] = TINY;
        misses[i] = 0;
        }

    y_return = y0;
    dydx0  = tmalloc(sizeof(double)*n_eq);
    y1     = tmalloc(sizeof(double)*n_eq);
    dydx1  = tmalloc(sizeof(double)*n_eq);
    y2     = tmalloc(sizeof(double)*n_eq);
    dydx2  = tmalloc(sizeof(double)*n_eq);
    yscale = tmalloc(sizeof(double)*n_eq);

    /* calculate derivatives and exit function at the initial point */
    (*derivs)(dydx0, y0, *x0);

    /* set the scales for evaluating accuracy.  yscale[i] is the
     * absolute level of accuracy required of the next integration step
     */
    accur  = tmalloc(sizeof(double)*n_eq);
    initial_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accuracy, accur, *x0, xf, n_eq);

    ex0 = exit_value - y0[i_exit_value];
    is_zero = 0;
    do {
        /* check for zero of exit function */
        if (fabs(ex0)<exit_accuracy) {
	    if (!is_zero) {
                if (n_to_skip==0) {
                    for (i=0; i<n_eq; i++)
                        y_return[i] = y0[i];
                    *h_rec = h_start;
                    tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
                    tfree(accur);
                    if (y0!=y_return) tfree(y0);
                    if (y1!=y_return) tfree(y1);
                    if (y2!=y_return) tfree(y2);
                    return(DIFFEQ_ZERO_FOUND);
                    }
                else {
                    is_zero = 1;
                    --n_to_skip;
                    }
                }
            }
        else
            is_zero = 0;
        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_start)
            h_start = xdiff;
        /* take a step */
        x1 = *x0;
        if (!bs_step(y1, &x1, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses)) {
            if (n_step_ups++ > MAX_N_STEP_UPS) {
                bomb("error: cannot take initial step (bs_odeint2--1)", NULL);
                }
            h_start = (n_step_ups-1?h_start*10:h_used*10);
            continue;
            }
        /* calculate derivatives and exit function at new point */
        (*derivs)(dydx1, y1, x1);
        ex1 = exit_value - y1[i_exit_value];
        /* check for change in sign of exit function */
        if (SIGN(ex0)!=SIGN(ex1) && !is_zero) {
            if (n_to_skip==0)
                break;
            else {
                --n_to_skip;
                is_zero = 1;
                }
            }
        /* check for end of interval */
        if (fabs(xdiff=xf-x1)<x_accuracy) {
            /* end of the interval */
            for (i=0; i<n_eq; i++)
                y_return[i] = y1[i];
            *x0 = x1;
            *h_rec = h_start;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        ex0 = ex1;
        *x0 = x1;
        /* adjust the step size as recommended by bs_step() */
        h_start = (h_next>h_max?(h_max?h_max:h_next):h_next);
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        } while (1);
    *h_rec = h_start;

    /* The root has been bracketed. */
    do {
        /* try to take a step to the position where the zero is expected */
        h_start = -ex0*(x1-*x0)/(ex1-ex0+TINY);
        x2 = *x0;
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        if (!bs_step(y2, &x2, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses))
            bomb("step size too small (bs_odeint2--2)", NULL);
        /* check the exit function at the new position */
        (*derivs)(dydx2, y2, x2);
        ex2 = exit_value - y2[i_exit_value];
        if (fabs(ex2)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                y_return[i] = y2[i];
            *x0 = x2;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
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
        } while (1);
    }

/* routine: bs_odeint3()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached, or
 *          until a user-supplied function is zero
 * returns: 0<= on failure, >=1 on success
 */
#define TINY 1.0e-30

long bs_odeint3(
    double *yif,          /* initial/final values of dependent variables */
    void (*derivs)(double *dydx, double *y, double x),/* (*derivs)(dydx, y, x) */
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,     /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,         /* small value relative to what's important */
    long *misses,        /* number of times each variable caused reset
                            of step size */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_start,       /* suggested starting step size */
    double h_max,         /* maximum step size allowed */
    double *h_rec,        /* recommended step size for continuation */
    /* function for determining when to stop integration: */
    double (*exit_func)(double *dydx, double *y, double x), 
            /* function that is to be zeroed */
    double exit_accuracy   /* how close to zero to get */
    )
{
    static double *yscale;
    static double *dydx0, *y1, *dydx1, *dydx2, *y2, *accur, *y0;
    static long last_neq = 0;
    double ex0, ex1, ex2, x1, x2;
    double h_used, h_next, xdiff;
    long i, n_step_ups = 0;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (FABS(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);

    /* Meaning of accmode:
     * accmode = 0 -> accuracy[i] is desired fractional accuracy at
     *                each step for ith variable.  tiny[i] is lower limit 
     *                of significance for the ith variable.
     * accmode = 1 -> same as accmode=0, except that the accuracy is to be
     *                satisfied globally, not locally.
     * accmode = 2 -> accuracy[i] is the desired absolute accuracy per
     *                step for the ith variable.  tiny[i] is ignored.
     * accmode = 3 -> samed as accmode=2, except that the accuracy is to 
     *                be satisfied globally, not locally.
     */
    for (i=0; i<n_eq; i++) {
        if (accmode[i]<0 || accmode[i]>3)
            bomb("accmode must be on [0, 3] (bs_odeint)", NULL);
        if (accmode[i]<2 && tiny[i]<TINY)
            tiny[i] = TINY;
        misses[i] = 0;
        }

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
        yscale = tmalloc(sizeof(double)*n_eq);
        accur  = tmalloc(sizeof(double)*n_eq);
        last_neq = n_eq;
        }

    for (i=0; i<n_eq; i++)
        y0[i] = yif[i];

    /* calculate derivatives and exit function at the initial point */
    (*derivs)(dydx0, y0, *x0);

    /* set the scales for evaluating accuracy.  yscale[i] is the
     * absolute level of accuracy required of the next integration step
     */
    initial_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
            accuracy, accur, *x0, xf, n_eq);
     
    ex0 = (*exit_func)(dydx0, y0, *x0);

    do {
        /* check for zero of exit function */
        if (FABS(ex0)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                yif[i] = y0[i];
            *h_rec = h_start;
            return(DIFFEQ_ZERO_FOUND);
            }

        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_start)
            h_start = xdiff;
        /* take a step */
        x1 = *x0;
        if (!bs_step(y1, &x1, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses)) {
            if (n_step_ups++ > MAX_N_STEP_UPS)
                bomb("error: cannot take initial step (bs_odeint3--1)", NULL);
            h_start = (n_step_ups-1?h_start*10:h_used*10);
            continue;
            }
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
            *h_rec = h_start;
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        ex0 = ex1;
        *x0 = x1;
        /* adjust the step size as recommended by bs_step() */
        h_start = (h_next>h_max?(h_max?h_max:h_next):h_next);
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        } while (1);
    *h_rec = h_start;

    if (!exit_func) {
        printf("failure in bs_odeint3():  solution stepped outside interval\n");
        return(DIFFEQ_OUTSIDE_INTERVAL);
        }

    if (FABS(ex1)<exit_accuracy) {
        for (i=0; i<n_eq; i++)
            yif[i] = y1[i];
        *x0 = x1;
        return(DIFFEQ_ZERO_FOUND);
        }

    /* The root has been bracketed. */
    do {
        /* try to take a step to the position where the zero is expected */
        h_start = -ex0*(x1-*x0)/(ex1-ex0+TINY);
        x2 = *x0;
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        if (!bs_step(y2, &x2, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses))
            bomb("step size too small (bs_odeint3--2)", NULL);
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
        } while (1);
    }


/* routine: bs_odeint4()
 * purpose: integrate a set of ordinary differential equations until
 *          the upper limit of the independent variable is reached or
 *          until a specified component of the solution reaches a 
 *          specified within a specified accuracy.  Allows storage of
 *          intermediate values.
 *
 * returns: 0<= on failure, 1 on success
 */

long bs_odeint4(
    double *y0,           /* initial/final values of dependent variables */
    /* (*derivs)(dydx, y, x): */
    void (*derivs)(double *dydx, double *y, double x),    
    long n_eq,           /* number of equations */
    /* for each dependent variable: */
    double *accuracy,     /* desired accuracy--see below for meaning */
    long *accmode,       /* desired accuracy-control mode */
    double *tiny,         /* small value relative to what's important */
    long *misses,        /* number of times each variable caused reset
                            of step size */
    /* for the dependent variable: */
    double *x0,           /* initial/final value */
    double xf,            /* upper limit of integration */
    double x_accuracy,    /* accuracy of final value */
    double h_start,       /* suggested starting step size */
    double h_max,         /* maximum step size allowed */
    double *h_rec,        /* recommended step size for continuation */
    /* for determining when to stop integration: */
    double exit_value,    /* value to be obtained */
    long i_exit_value,   /* index of independent variable this pertains to */
    double exit_accuracy,  /* how close to get */
    long n_to_skip,       /* number of zeros to skip before returning */
    void (*store_data)(double *dydx, double *y, double x, double exf)  /* function to store points */
    )
{
    double *y_return, *accur;
    double *dydx0, *y1, *dydx1, *dydx2, *y2;
    double ex0, ex1, ex2, x1, x2, *yscale;
    double h_used, h_next, xdiff;
    long i, n_step_ups=0, is_zero;
#define MAX_N_STEP_UPS 10

    if (*x0>xf)
        return(DIFFEQ_XI_GT_XF);
    if (fabs(*x0-xf)<x_accuracy)
        return(DIFFEQ_SOLVED_ALREADY);
    if (i_exit_value<0 || i_exit_value>=n_eq)
        bomb("index of variable for exit testing is out of range (bs_odeint4)", NULL);

    /* Meaning of accmode:
     * accmode = 0 -> accuracy[i] is desired fractional accuracy at
     *                each step for ith variable.  tiny[i] is lower limit 
     *                of significance for the ith variable.
     * accmode = 1 -> same as accmode=0, except that the accuracy is to be
     *                satisfied globally, not locally.
     * accmode = 2 -> accuracy[i] is the desired absolute accuracy per
     *                step for the ith variable.  tiny[i] is ignored.
     * accmode = 3 -> samed as accmode=2, except that the accuracy is to 
     *                be satisfied globally, not locally.
     */
    for (i=0; i<n_eq; i++) {
        if (accmode[i]<0 || accmode[i]>3)
            bomb("accmode must be on [0, 3] (bs_odeint4)", NULL);
        if (accmode[i]<2 && tiny[i]<TINY)
            tiny[i] = TINY;
        misses[i] = 0;
        }

    y_return = y0;
    dydx0  = tmalloc(sizeof(double)*n_eq);
    y1     = tmalloc(sizeof(double)*n_eq);
    dydx1  = tmalloc(sizeof(double)*n_eq);
    y2     = tmalloc(sizeof(double)*n_eq);
    dydx2  = tmalloc(sizeof(double)*n_eq);
    yscale = tmalloc(sizeof(double)*n_eq);

    /* calculate derivatives and exit function at the initial point */
    (*derivs)(dydx0, y0, *x0);

    /* set the scales for evaluating accuracy.  yscale[i] is the
     * absolute level of accuracy required of the next integration step
     */
    accur  = tmalloc(sizeof(double)*n_eq);
    initial_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accuracy, accur, *x0, xf, n_eq);

    ex0 = exit_value - y0[i_exit_value];
    if (store_data)
        (*store_data)(dydx0, y0, *x0, ex0);
    is_zero = 0;
    do {
        /* check for zero of exit function */
        if (fabs(ex0)<exit_accuracy) {
	    if (!is_zero) {
                if (n_to_skip==0) {
                    if (store_data) 
                        (*store_data)(dydx0, y0, *x0, ex0);
                    for (i=0; i<n_eq; i++)
                        y_return[i] = y0[i];
                    *h_rec = h_start;
                    tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
                    tfree(accur);
                    if (y0!=y_return) tfree(y0);
                    if (y1!=y_return) tfree(y1);
                    if (y2!=y_return) tfree(y2);
                    return(DIFFEQ_ZERO_FOUND);
                    }
                else {
                    is_zero = 1;
                    --n_to_skip;
                    }
                }
            }
        else
            is_zero = 0;
        /* adjust step size to stay within interval */
        if ((xdiff=xf-*x0)<h_start)
            h_start = xdiff;
        /* take a step */
        x1 = *x0;
        if (!bs_step(y1, &x1, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses)) {
            if (n_step_ups++ > MAX_N_STEP_UPS) {
                bomb("error: cannot take initial step (bs_odeint4--1)", NULL);
                }
            h_start = (n_step_ups-1?h_start*10:h_used*10);
            continue;
            }
        /* calculate derivatives and exit function at new point */
        (*derivs)(dydx1, y1, x1);
        ex1 = exit_value - y1[i_exit_value];
        if (store_data)
            (*store_data)(dydx1, y1, x1, ex1);
        /* check for change in sign of exit function */
        if (SIGN(ex0)!=SIGN(ex1) && !is_zero) {
            if (n_to_skip==0)
                break;
            else {
                --n_to_skip;
                is_zero = 1;
                }
            }
        /* check for end of interval */
        if (fabs(xdiff=xf-x1)<x_accuracy) {
            /* end of the interval */
            if (store_data) {
                (*derivs)(dydx1, y1, x1);
                ex1 = exit_value - y0[i_exit_value];
                (*store_data)(dydx1, y1, x1, ex1);
                }
            for (i=0; i<n_eq; i++)
                y_return[i] = y1[i];
            *x0 = x1;
            *h_rec = h_start;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
            return(DIFFEQ_END_OF_INTERVAL);
            }
        /* copy the new solution into the old variables */
        SWAP_PTR(dydx0, dydx1);
        SWAP_PTR(y0, y1);
        ex0 = ex1;
        *x0 = x1;
        /* adjust the step size as recommended by bs_step() */
        h_start = (h_next>h_max?(h_max?h_max:h_next):h_next);
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        } while (1);
    *h_rec = h_start;

    /* The root has been bracketed. */
    do {
        /* try to take a step to the position where the zero is expected */
        h_start = -ex0*(x1-*x0)/(ex1-ex0+TINY);
        x2 = *x0;
        /* calculate new scale factors */
        new_scale_factors_dp(yscale, y0, dydx0, h_start, tiny, accmode, 
                accur, n_eq);
        if (!bs_step(y2, &x2, y0, dydx0, h_start, &h_used, &h_next, 
                    yscale, n_eq, derivs, misses))
            bomb("step size too small (bs_odeint4--2)", NULL);
        /* check the exit function at the new position */
        (*derivs)(dydx2, y2, x2);
        ex2 = exit_value - y2[i_exit_value];
        if (fabs(ex2)<exit_accuracy) {
            for (i=0; i<n_eq; i++)
                y_return[i] = y2[i];
            *x0 = x2;
            tfree(dydx0); tfree(dydx1); tfree(dydx2); tfree(yscale);
            tfree(accur);
            if (y0!=y_return) tfree(y0);
            if (y1!=y_return) tfree(y1);
            if (y2!=y_return) tfree(y2);
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
        } while (1);
    }


/* routine: bs_step
 * purpose: perform a quality-control Burlisch-Stoer step
 * Based on Numerical Recipes in C.
 * M. Borland, 1995
 */

static double stepIncreaseFactor = 0.50;
static double stepDecreaseFactor = 0.95;

void bs_qctune(double newStepIncreaseFactor, double newStepDecreaseFactor)
{
    if (newStepIncreaseFactor>0)
        stepIncreaseFactor = newStepIncreaseFactor;
    if (newStepDecreaseFactor>0)
        stepDecreaseFactor = newStepDecreaseFactor;
    }


#define DEBUG 0

#define IMAX 11
#define NUSE 7

long bs_step(
    double *yFinal,        /* final values of the dependent variables */
    double *x,             /* initial value of the independent variable */
    double *yInitial,      /* initial values of dependent variables */
    double *dydxInitial,   /* derivatives at x */
    double step,              /* step to try */
    double *stepUsed,         /* step used */
    double *stepRecommended,  /* step recommended for next step */
    double *yScale,        /* allowable absolute error for each component */
    long equations,        /* number of equations */
    void (*derivs)(double *dydx, double *y, double x),
                    /* function to return dy/dx at x for given y */
    long *misses    /* number of failures caused by each component */
    )
{
    static double **solution, *hSqr, *yLast, *yError;
    long i, j, iWorst=0, code, nuse;
    double maxError, error, yInterp;
    static long mmidSteps[IMAX]={2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96};
    static long lastEquations=0;

    if (equations>lastEquations) {
        if (lastEquations!=0) {
            free(yLast); free(yError); free(hSqr);
            free_array_2d((void**)solution, sizeof(*solution), 0, lastEquations-1, 0, NUSE-1);
            }
        yLast = tmalloc(sizeof(double)*equations);
        yError  = tmalloc(sizeof(double)*equations);
        hSqr  = tmalloc(sizeof(double)*IMAX);
        solution = (double**)array_2d(sizeof(double), 0, equations-1, 0, NUSE-1);
        lastEquations = equations;
        }

    do {
#if DEBUG
        printf("step = %e\n", step);
#endif
        for (i=0; i<IMAX; i++) {
            mmid(yInitial, dydxInitial, equations, *x, step, mmidSteps[i], yFinal, derivs);
            hSqr[i%NUSE] = sqr(step/mmidSteps[i]);
            nuse = i>NUSE ? NUSE: i;
            for (j=0; j<equations; j++) {
                /* store the jth component of the new solution, possibly throwing out the oldest solution */
                solution[j][i%NUSE] = yFinal[j];
                if (nuse>1)
                    /* Interpolate the solution value at h^2 = 0 */
                    yInterp = LagrangeInterp(hSqr, solution[j], nuse, 0.0, &code);
                else
                    /* just copy modified midpoint value */
                    yInterp = yFinal[j];
                if (i)
                    /* compute difference between new solution and that from last step */
                    yError[j] = yInterp - yLast[j];
                /* save the new solution for the next iteration */
                yLast[j] = yInterp;
#if DEBUG
                printf("i = %ld, j = %ld:  yFinal = %.10e, yError = %.10e, yScale = %.10e\n",
                       i, j, yFinal[j], yInterp, yError[j], yScale[j]);
#endif
                }
            if (i) {
                maxError=0.0;
                for (j=0; j<equations; j++)
                    if (maxError < (error=FABS(yError[j]/yScale[j]))) {
                        iWorst = j;
                        maxError  = error;
                        }
#if DEBUG
                printf("maxError = %e, iWorst = %ld\n", maxError, iWorst);
#endif
                if (maxError < 1.0) {
                    *x += step;
                    *stepRecommended = *stepUsed = step;
                    if (i==NUSE-1) 
                        /* had a hard time, so recommend smaller step */
                        *stepRecommended *= stepDecreaseFactor;
                    else {
                        /* increase the step to make better use of extrapolation */
                        *stepRecommended *= stepIncreaseFactor/sqrt(maxError);
                        }
#if DEBUG
                    printf("returning with i=%ld, stepUsed=%e, stepRec=%e\n", i, *stepUsed, *stepRecommended);
#endif
                    for (j=0; j<equations; j++)
                        yFinal[j] = yLast[j];
                    return(1);
                    }
                misses[iWorst]++;
                }
            }

        /* method failed, so reduce the step of the modified-midpoint integrations */
        step *= 0.25;
        for (i=0; i<(IMAX-NUSE)/2; i++) 
            step /= 2.0;
        } while ((*x+step) != *x) ;
    fprintf(stderr, "error: step size underflow in bs_step()--step reduced to %e\n", step);
    return 0;
    }


