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
 Revision 1.2  1995/09/05 21:19:55  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

static double rho, tan_alpha, Xo, Yo;

double fn(double x)
{
    double d1, d2;
    static long initialized=0;
    double temp;

    if (!initialized) {
        double angle, l, W, L1;
        angle = query_double("bend angle (deg)", 75.0)*PI/180.;
        l = query_double("path length (m)", 0.16);
        rho = l/angle;
        L1 = query_double("L1 (m)", 0.114);
        W  = query_double("W (m)", 0.155);
        Yo = W/2 - rho + rho*(1-cos(angle/2))/2;
        tan_alpha = tan(query_double("alpha (deg)", -17.5)*PI/180.);
        Xo = L1/2*(tan_alpha<0?-1:1);
        printf("rho = %le, Yo = %le, Xo = %le\n", rho, Yo, Xo);
        initialized = 1;
        }

    temp = Yo+rho*cos(x) - (rho*sin(x)-Xo)/tan_alpha ;
    return(temp);
    }

double fn_after(double theta)
{
    double x, y;

    x = rho*sin(theta);
    printf("line: x = %le, y= %le\n", x, (x-Xo)/tan_alpha);
    printf("arc : x = %le, y= %le\n", x, Yo+rho*cos(theta));
    }

