/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* radb5.f -- translated by f2c (version of 30 January 1990  16:02:04).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
 $Log: radb5.c,v $
 Revision 1.3  2002/08/14 16:41:22  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:13:49  saunders
 First test release of the SDDS1.5 package.

*/

#include "f2c.h"

/* Subroutine */ int radb5_(integer *ido, integer *l1, doublereal *cc, 
	doublereal *ch, doublereal *wa1, doublereal *wa2, doublereal *wa3, 
	doublereal *wa4)
{
    /* Initialized data */

    static doublereal tr11 = .309016994374947;
    static doublereal ti11 = .951056516295154;
    static doublereal tr12 = -.809016994374947;
    static doublereal ti12 = .587785252292473;

    /* System generated locals */
    integer cc_dim1, cc_offset, ch_dim1, ch_dim2, ch_offset, i_1, i_2;

    /* Local variables */
    static integer i, k, ic;
    static doublereal ci2, ci3, ci4, ci5, di3, di4, di5, di2, cr2, cr3, cr5, 
	    cr4, ti2, ti3, ti4, ti5, dr3, dr4, dr5, dr2, tr2, tr3, tr4, tr5;
    static integer idp2;

    /* Parameter adjustments */
    cc_dim1 = *ido;
    cc_offset = cc_dim1 * 6 + 1;
    cc -= cc_offset;
    ch_dim1 = *ido;
    ch_dim2 = *l1;
    ch_offset = ch_dim1 * (ch_dim2 + 1) + 1;
    ch -= ch_offset;
    --wa1;
    --wa2;
    --wa3;
    --wa4;

    /* Function Body */
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	ti5 = cc[(k * 5 + 3) * cc_dim1 + 1] + cc[(k * 5 + 3) * cc_dim1 + 1];
	ti4 = cc[(k * 5 + 5) * cc_dim1 + 1] + cc[(k * 5 + 5) * cc_dim1 + 1];
	tr2 = cc[*ido + (k * 5 + 2) * cc_dim1] + cc[*ido + (k * 5 + 2) * 
		cc_dim1];
	tr3 = cc[*ido + (k * 5 + 4) * cc_dim1] + cc[*ido + (k * 5 + 4) * 
		cc_dim1];
	ch[(k + ch_dim2) * ch_dim1 + 1] = cc[(k * 5 + 1) * cc_dim1 + 1] + tr2 
		+ tr3;
	cr2 = cc[(k * 5 + 1) * cc_dim1 + 1] + tr11 * tr2 + tr12 * tr3;
	cr3 = cc[(k * 5 + 1) * cc_dim1 + 1] + tr12 * tr2 + tr11 * tr3;
	ci5 = ti11 * ti5 + ti12 * ti4;
	ci4 = ti12 * ti5 - ti11 * ti4;
	ch[(k + (ch_dim2 << 1)) * ch_dim1 + 1] = cr2 - ci5;
	ch[(k + ch_dim2 * 3) * ch_dim1 + 1] = cr3 - ci4;
	ch[(k + (ch_dim2 << 2)) * ch_dim1 + 1] = cr3 + ci4;
	ch[(k + ch_dim2 * 5) * ch_dim1 + 1] = cr2 + ci5;
/* L101: */
    }
    if (*ido == 1) {
	return 0;
    }
    idp2 = *ido + 2;
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	i_2 = *ido;
	for (i = 3; i <= i_2; i += 2) {
	    ic = idp2 - i;
	    ti5 = cc[i + (k * 5 + 3) * cc_dim1] + cc[ic + (k * 5 + 2) * 
		    cc_dim1];
	    ti2 = cc[i + (k * 5 + 3) * cc_dim1] - cc[ic + (k * 5 + 2) * 
		    cc_dim1];
	    ti4 = cc[i + (k * 5 + 5) * cc_dim1] + cc[ic + (k * 5 + 4) * 
		    cc_dim1];
	    ti3 = cc[i + (k * 5 + 5) * cc_dim1] - cc[ic + (k * 5 + 4) * 
		    cc_dim1];
	    tr5 = cc[i - 1 + (k * 5 + 3) * cc_dim1] - cc[ic - 1 + (k * 5 + 2) 
		    * cc_dim1];
	    tr2 = cc[i - 1 + (k * 5 + 3) * cc_dim1] + cc[ic - 1 + (k * 5 + 2) 
		    * cc_dim1];
	    tr4 = cc[i - 1 + (k * 5 + 5) * cc_dim1] - cc[ic - 1 + (k * 5 + 4) 
		    * cc_dim1];
	    tr3 = cc[i - 1 + (k * 5 + 5) * cc_dim1] + cc[ic - 1 + (k * 5 + 4) 
		    * cc_dim1];
	    ch[i - 1 + (k + ch_dim2) * ch_dim1] = cc[i - 1 + (k * 5 + 1) * 
		    cc_dim1] + tr2 + tr3;
	    ch[i + (k + ch_dim2) * ch_dim1] = cc[i + (k * 5 + 1) * cc_dim1] + 
		    ti2 + ti3;
	    cr2 = cc[i - 1 + (k * 5 + 1) * cc_dim1] + tr11 * tr2 + tr12 * tr3;

	    ci2 = cc[i + (k * 5 + 1) * cc_dim1] + tr11 * ti2 + tr12 * ti3;
	    cr3 = cc[i - 1 + (k * 5 + 1) * cc_dim1] + tr12 * tr2 + tr11 * tr3;

	    ci3 = cc[i + (k * 5 + 1) * cc_dim1] + tr12 * ti2 + tr11 * ti3;
	    cr5 = ti11 * tr5 + ti12 * tr4;
	    ci5 = ti11 * ti5 + ti12 * ti4;
	    cr4 = ti12 * tr5 - ti11 * tr4;
	    ci4 = ti12 * ti5 - ti11 * ti4;
	    dr3 = cr3 - ci4;
	    dr4 = cr3 + ci4;
	    di3 = ci3 + cr4;
	    di4 = ci3 - cr4;
	    dr5 = cr2 + ci5;
	    dr2 = cr2 - ci5;
	    di5 = ci2 - cr5;
	    di2 = ci2 + cr5;
	    ch[i - 1 + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 2] * dr2 - 
		    wa1[i - 1] * di2;
	    ch[i + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 2] * di2 + wa1[i 
		    - 1] * dr2;
	    ch[i - 1 + (k + ch_dim2 * 3) * ch_dim1] = wa2[i - 2] * dr3 - wa2[
		    i - 1] * di3;
	    ch[i + (k + ch_dim2 * 3) * ch_dim1] = wa2[i - 2] * di3 + wa2[i - 
		    1] * dr3;
	    ch[i - 1 + (k + (ch_dim2 << 2)) * ch_dim1] = wa3[i - 2] * dr4 - 
		    wa3[i - 1] * di4;
	    ch[i + (k + (ch_dim2 << 2)) * ch_dim1] = wa3[i - 2] * di4 + wa3[i 
		    - 1] * dr4;
	    ch[i - 1 + (k + ch_dim2 * 5) * ch_dim1] = wa4[i - 2] * dr5 - wa4[
		    i - 1] * di5;
	    ch[i + (k + ch_dim2 * 5) * ch_dim1] = wa4[i - 2] * di5 + wa4[i - 
		    1] * dr5;
/* L102: */
	}
/* L103: */
    }
    return 0;
} /* radb5_ */


