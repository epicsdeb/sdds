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
 Revision 1.2  1995/09/05 21:20:05  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

main()
{
    double a, b, g, z, lim;
    double hyper();
    long n, pinter;

    a = b = g = z = 1;
    lim = 1e-5;
    n = pinter = 100;
    while (1) {
        a = query_double("alpha", a);
        b = query_double("beta", b);
        g = query_double("gamma", g);
        z = query_double("z", z);
        n = query_int("number of terms", n);
        pinter = query_int("printing interval", pinter);
        lim = query_double("limit on terms", lim);
        printf("F(%le, %le; %le; %le) = %le\n",
            a, b, g, z, hyper(a, b, g, z, n, lim, pinter));
        }
    }

