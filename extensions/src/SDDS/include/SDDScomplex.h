/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: complex.h
 *       structures and functions for complex number arithmetic
 * Michael Borland, 1992
 $Log: not supported by cvs2svn $
 Revision 1.12  2003/07/22 20:01:17  soliday
 Added support for Kylix.

 Revision 1.11  2002/08/14 15:40:14  soliday
 Added Open License

 Revision 1.10  2001/10/25 22:01:13  borland
 Added prototype for croot().

 Revision 1.9  1999/09/14 18:03:51  soliday
 Added export commands for WIN32 dll files.

 Revision 1.8  1999/07/01 19:25:18  borland
 Added prototypes for complex_multiply and complex_divide.

 Revision 1.7  1999/04/13 17:22:58  emery
 Added #include <stdio.h> since the FILE data type is present.

 Revision 1.6  1998/04/13 22:23:16  emery
 Added prototypes for simple complex matrix operation.

 Revision 1.5  1998/04/13 22:05:40  emery
 Added prototypes for csroot and cassign_me.

 Revision 1.4  1998/04/13 22:02:50  emery
 Corrected _copmlex_ spelling mistake.

 Revision 1.3  1998/04/13 22:00:58  emery
 Added #define _COMPLEX_ within a #ifndef _COMPLEX_ block
 in order to preovent this include file from being read more
 than once.

 * Revision 1.2  1995/09/05  21:14:56  saunders
 * First test release of the SDDS1.5 package.
 *
 */

#ifndef _COMPLEX_
#define _COMPLEX_

#undef epicsShareFuncMDBMTH
#if (defined(_WIN32) && !defined(__CYGWIN32__)) || (defined(__BORLANDC__) && defined(__linux__))
#if defined(EXPORT_MDBMTH)
#define epicsShareFuncMDBMTH  __declspec(dllexport)
#else
#define epicsShareFuncMDBMTH
#endif
#else
#define epicsShareFuncMDBMTH
#endif

typedef struct {
    double r, i;
    } COMPLEX, *COMPTR;

typedef struct {
	double **ar, **ai;
	int n, m;
	} CMATRIX;

#include <stdio.h>

/* prototypes for code in file complex2.c */
epicsShareFuncMDBMTH COMPLEX cadd(COMPLEX a, COMPLEX b);
epicsShareFuncMDBMTH COMPLEX csub(COMPLEX a, COMPLEX b);
epicsShareFuncMDBMTH COMPLEX cmul(COMPLEX a, COMPLEX b);
epicsShareFuncMDBMTH COMPLEX cdiv(COMPLEX a, COMPLEX b);
/*epicsShareFuncMDBMTH COMPLEX cexp(COMPLEX a);
epicsShareFuncMDBMTH COMPLEX clog(COMPLEX a);
epicsShareFuncMDBMTH COMPLEX csin(COMPLEX a);
epicsShareFuncMDBMTH COMPLEX ccos(COMPLEX a);*/
epicsShareFuncMDBMTH COMPLEX cmulr(COMPLEX a, double r);
epicsShareFuncMDBMTH COMPLEX cexpi(double p);        /* returns e^(i*p) */
epicsShareFuncMDBMTH COMPLEX cconj(COMPLEX a);
epicsShareFuncMDBMTH double cmod(COMPLEX a);
epicsShareFuncMDBMTH COMPLEX cipowr(COMPLEX a, int n);    /* returns a^n */
epicsShareFuncMDBMTH COMPLEX cassign(double real, double imag);
epicsShareFuncMDBMTH COMPLEX cassign_me(CMATRIX *A, int row, int col);
epicsShareFuncMDBMTH COMPLEX csroot(COMPLEX x);
epicsShareFuncMDBMTH COMPLEX croot(COMPLEX x, long root);

epicsShareFuncMDBMTH COMPLEX cassign_me(CMATRIX *A, int row, int col);
epicsShareFuncMDBMTH void cm_error(char *message);
epicsShareFuncMDBMTH int cm_add(CMATRIX *C, CMATRIX *A, CMATRIX *B);
epicsShareFuncMDBMTH void cm_alloc(CMATRIX **A, int n, int m);
epicsShareFuncMDBMTH int cm_copy(CMATRIX *A, CMATRIX *B);
epicsShareFuncMDBMTH COMPLEX cm_det(CMATRIX *D);
epicsShareFuncMDBMTH void cm_free(CMATRIX **A);
epicsShareFuncMDBMTH int cm_invert(CMATRIX *A, CMATRIX *B);
epicsShareFuncMDBMTH int cm_mult(CMATRIX *C, CMATRIX *A, CMATRIX *B);
epicsShareFuncMDBMTH int cm_scmul(CMATRIX *B, CMATRIX *A, double a);
epicsShareFuncMDBMTH void cm_show(CMATRIX *A, char *format, char *label, FILE *fp);
epicsShareFuncMDBMTH void cm_get(CMATRIX *A);
epicsShareFuncMDBMTH void cm_rand(CMATRIX *A, double lo, double hi);
epicsShareFuncMDBMTH int cm_trans(CMATRIX *B, CMATRIX *A);
epicsShareFuncMDBMTH void cm_zero(CMATRIX *A);
epicsShareFuncMDBMTH void cm_identity(CMATRIX *A);
epicsShareFuncMDBMTH int cm_check(CMATRIX *A);

epicsShareFuncMDBMTH void complex_multiply(
                      double *r0, double *i0,    /* result */
                      double  r1, double  i1,
                      double  r2, double  i2
                      );
epicsShareFuncMDBMTH void complex_divide(
                    double *r0, double *i0,    /* result */
                    double  r1, double  i1,
                    double  r2, double  i2,
                    double threshold
                    );
#endif
