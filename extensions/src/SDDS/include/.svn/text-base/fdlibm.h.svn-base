/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.5  2002/08/14 15:40:14  soliday
 Added Open License

 Revision 1.4  2000/04/21 20:16:33  soliday
 Added Bessel functions for Borland C.

 Revision 1.3  2000/04/06 22:24:22  soliday
 Added support for Borland C.

 Revision 1.2  1999/09/14 18:04:02  soliday
 Added export commands for WIN32 dll files.

 Revision 1.1  1999/07/12 19:54:30  soliday
 Used with customized fdlibm library for WIN32

 *
 */

/* @(#)fdlibm.h 1.5 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#undef epicsShareFuncFDLIBM
#if (defined(_WIN32) && !defined(__CYGWIN32__)) || (defined(__BORLANDC__) && defined(__linux__))
#if defined(EXPORT_FDLIBM)
#define epicsShareFuncFDLIBM  __declspec(dllexport)
#else
#define epicsShareFuncFDLIBM
#endif
#endif

#define __LITTLE_ENDIAN 1
#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#endif

#if defined(__STDC__) || defined(__BORLANDC__)
#define __P(p)  p
#else
#define __P(p)  ()
#endif

extern int signgam;

#define MAXFLOAT 	((float)3.40282346638528860e+38)

#if defined(__BORLANDC__) && defined(__linux__)
#else
enum fdversion {fdlibm_ieee = -1, fdlibm_svid, fdlibm_xopen, fdlibm_posix};
#define _fdlib_version _IEEE_
#define _LIB_VERSION_TYPE enum fdversion
#define _LIB_VERSION _fdlib_version

extern _LIB_VERSION_TYPE _LIB_VERSION;

#define _IEEE_ fdlibm_ieee
#define _SVID_  fdlibm_svid
#define _POSIX_ fdlibm_posix
#endif

/* 
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS		1.41484755040568800000e+16 

epicsShareFuncFDLIBM double erf __P((double));
epicsShareFuncFDLIBM double erfc __P((double));
epicsShareFuncFDLIBM double lgamma __P((double));
#if defined(__BORLANDC__)
epicsShareFuncFDLIBM double j0 __P((double));
epicsShareFuncFDLIBM double j1 __P((double));
epicsShareFuncFDLIBM double jn __P((int, double));
epicsShareFuncFDLIBM double y0 __P((double));
epicsShareFuncFDLIBM double y1 __P((double));
epicsShareFuncFDLIBM double yn __P((int, double));
#endif
extern double rint __P((double));

/* ieee style elementary functions */

extern double __ieee754_log __P((double));	
extern double __ieee754_exp __P((double));
extern double __ieee754_lgamma_r __P((double,int *));
#if defined(__BORLANDC__)
extern double __ieee754_j0 __P((double));
extern double __ieee754_j1 __P((double));
extern double __ieee754_y0 __P((double));
extern double __ieee754_y1 __P((double));
extern double __ieee754_jn __P((int,double));
extern double __ieee754_yn __P((int,double));
#endif
/* fdlibm kernel function */
extern double __kernel_standard __P((double,double,int));
extern double __kernel_sin __P((double,double,int));
extern double __kernel_cos __P((double,double));

