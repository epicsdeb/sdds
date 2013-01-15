/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.2  2000/04/05 22:14:22  soliday
 Added Borland C support.

 Revision 1.1  1999/07/12 19:50:07  soliday
 Customized fdlibm library used with WIN32

 *
 */

/* @(#)w_lgamma.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* double lgamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgamma_r
 */

#include "fdlibm.h"
#include <math.h>
#include <float.h>

#if defined(__BORLANDC__) && defined(__linux__)
#define finite(x) __finite(x)
#else
#define finite(x) _finite(x)
#endif

extern int signgam;

#if defined(__STDC__) || defined(__BORLANDC__)
	double lgamma(double x)
#else
	double lgamma(x)
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_lgamma_r(x,&signgam);
#else
        double y;
        y = __ieee754_lgamma_r(x,&signgam);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!finite(y)&&finite(x)) {
            if(floor(x)==x&&x<=0.0)
                return __kernel_standard(x,x,15); /* lgamma pole */
            else
                return __kernel_standard(x,x,14); /* lgamma overflow */
        } else
            return y;
#endif
}             
