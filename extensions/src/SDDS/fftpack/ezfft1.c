/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* ezfft1.f -- translated by f2c (version of 30 January 1990  16:02:04).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
 $Log: ezfft1.c,v $
 Revision 1.3  2002/08/14 16:41:19  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:13:05  saunders
 First test release of the SDDS1.5 package.

*/

#include "f2c.h"

/* Subroutine */ int ezfft1_(integer *n, doublereal *wa, integer *ifac)
{
    /* Initialized data */

    static integer ntryh[4] = { 4,2,3,5 };
    static doublereal tpi = 6.28318530717959;

    /* System generated locals */
    integer i_1, i_2, i_3;

    /* Builtin functions */
    double cos(doublereal), sin(doublereal);

    /* Local variables */
    static doublereal argh;
    static integer ntry, i, j, k1, l1, l2, ib, ii, nf, ip, nl, is, nq, nr;
    static doublereal ch1, sh1;
    static integer ido, ipm;
    static doublereal dch1, ch1h, arg1, dsh1;
    static integer nfm1;

    /* Parameter adjustments */
    --wa;
    --ifac;

    /* Function Body */
    nl = *n;
    nf = 0;
    j = 0;
L101:
    ++j;
    if (j - 4 <= 0) {
	goto L102;
    } else {
	goto L103;
    }
L102:
    ntry = ntryh[j - 1];
    goto L104;
L103:
    ntry += 2;
L104:
    nq = nl / ntry;
    nr = nl - ntry * nq;
    if (nr != 0) {
	goto L101;
    } else {
	goto L105;
    }
L105:
    ++nf;
    ifac[nf + 2] = ntry;
    nl = nq;
    if (ntry != 2) {
	goto L107;
    }
    if (nf == 1) {
	goto L107;
    }
    i_1 = nf;
    for (i = 2; i <= i_1; ++i) {
	ib = nf - i + 2;
	ifac[ib + 2] = ifac[ib + 1];
/* L106: */
    }
    ifac[3] = 2;
L107:
    if (nl != 1) {
	goto L104;
    }
    ifac[1] = *n;
    ifac[2] = nf;
    argh = tpi / (doublereal) (*n);
    is = 0;
    nfm1 = nf - 1;
    l1 = 1;
    if (nfm1 == 0) {
	return 0;
    }
    i_1 = nfm1;
    for (k1 = 1; k1 <= i_1; ++k1) {
	ip = ifac[k1 + 2];
	l2 = l1 * ip;
	ido = *n / l2;
	ipm = ip - 1;
	arg1 = (doublereal) l1 * argh;
	ch1 = 1.;
	sh1 = 0.;
	dch1 = cos(arg1);
	dsh1 = sin(arg1);
	i_2 = ipm;
	for (j = 1; j <= i_2; ++j) {
	    ch1h = dch1 * ch1 - dsh1 * sh1;
	    sh1 = dch1 * sh1 + dsh1 * ch1;
	    ch1 = ch1h;
	    i = is + 2;
	    wa[i - 1] = ch1;
	    wa[i] = sh1;
	    if (ido < 5) {
		goto L109;
	    }
	    i_3 = ido;
	    for (ii = 5; ii <= i_3; ii += 2) {
		i += 2;
		wa[i - 1] = ch1 * wa[i - 3] - sh1 * wa[i - 2];
		wa[i] = ch1 * wa[i - 2] + sh1 * wa[i - 3];
/* L108: */
	    }
L109:
	    is += ido;
/* L110: */
	}
	l1 = l2;
/* L111: */
    }
    return 0;
} /* ezfft1_ */


