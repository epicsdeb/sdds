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
 Revision 1.3  2002/08/14 16:18:57  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:20:11  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: ipow()
 * purpose: compute integer powers by straight-forward multiplication.
 *            This is much faster than using pow().
 *
 * Michael Borland, July 1987.
 */
#include "mdb.h"

double ipow(double x, long p)
{
    register double hp;
    register long n;

    if (!x)  {
        if (p<0)
            bomb("Floating divide by zero in ipow().", NULL);
        return(p==0?1.:0.);
        }

    if (p<0)
        return(1./ipow(x, -p));

    switch (p) {
        case 0: 
            return(1.);
        case 1: 
            return(x);
        case 2: 
            return(x*x);
        case 3: 
            hp = x*x;
            return(hp*x);
        case 4: 
            hp = x*x;
            return(hp*hp);
        case 5: 
            hp = x*x;
            return(hp*hp*x);
        case 6: 
            hp = x*x;
            return(hp*hp*hp);
        case 7: 
            hp = x*x*x;
            return(hp*hp*x);
        case 8: 
            hp = x*x;
            hp = hp*hp;
            return(hp*hp);
        default:
            n = p/2;
	    hp = ipow(x, n);
            switch (p-2*n) {
                case 0:
                    return(hp*hp);
                case 1:
                    return(hp*hp*x);
                }
            break;
        }
    return(0.);  /* never actually executed--keeps compiler happy */
    }

