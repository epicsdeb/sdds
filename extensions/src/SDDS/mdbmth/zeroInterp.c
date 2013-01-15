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
 Revision 1.5  2001/10/12 21:08:36  soliday
 Added include statement.

 Revision 1.4  2000/11/02 21:25:27  borland
 Added "value" option that allows solving for fn=value instead of just
 fn=0.

 Revision 1.3  1997/11/04 02:47:23  borland
 Fixed an egregious bug---was calling itself recursively with the wrong
 name.  This wasn't in the function library since the name change.

 * Revision 1.2  1995/09/05  21:21:00  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* routine: zeroInterp()
 * purpose: find zero of a function in an interval using successive
 *            interpolation 
 *
 * Michael Borland, 1994, 1995
 */
#include <math.h>
#include <stdio.h>
#include "mdb.h"

#define sign(x) ( (x)>0 ? 1 : ((x)==0 ? 0 : -1) )

double zeroInterp(
                    double (*fn)(),	/* pointer to function to be zeroed */
                    double value,       /* sovle for fn=value */
                    double x_i,         /* initial, final values for independent variable */ 
                    double x_f,	
                    double dx,          /* size of steps in independent variable */
                    double _zero)       /* value acceptably close to true zero */
{
	double xa, xb, xm, x_b;
	double fa, fb, fm;
	double f_abs, f_bdd;
	long s_fa, s_fb, s_fm;

	xa = x_i;
	fa = (*fn)(xa)-value;
	s_fa = sign(fa);
	xb = xa +dx;

	while (xb <= x_f) {	
		fb = (*fn)(xb)-value;
		s_fb = sign(fb);		
		if (s_fb == s_fa) {
			fa = fb;
			xa = xb;
			s_fa = s_fb;
			xb = xb + dx;
			}
		else {
			/* function has passed through zero    */
			/* so interpolate to find zero, repeat */
			f_bdd = 1000*fabs(fa);
                        xm    = xa - fa*(xa-xb)/(fa-fb);
			fm    = (*fn)(xm)-value;
			s_fm  = sign(fm);
			x_b   = xb;
			do {
				if (s_fm == 0) 
					return(xm);
				else if (s_fm != s_fa) {
					xb   = xm;
					fb   = fm;
					s_fb = s_fm;
					}
				else {
					xa   = xm;
					fa   = fm;
					s_fa = s_fm;
					}
                                xm    = xa - fa*(xa-xb)/(fa-fb);
				fm    = (*fn)(xm)-value;
				s_fm  = sign(fm);
				f_abs = fabs(fm);
				} while (f_abs > _zero && f_abs < f_bdd);
			if (f_abs < _zero)
				 return(xm);
			/* Function had a tan(x)-like singularity, which
			 * looked like a zero.  So step beyond singularity
			 * and continue search.
			 */
			return(zeroInterp(fn, value, x_b, x_f, dx, _zero));
			}
		}
	return(x_f + dx);
	}			

