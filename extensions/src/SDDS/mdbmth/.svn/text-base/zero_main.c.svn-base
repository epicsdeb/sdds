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
 Revision 1.3  2002/08/14 16:19:02  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:21:04  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

main()
{
  double xi, xf, dx, _zero, zero();
  double zero_h(), zero_i();
  double fn();
  char s[100];

  while (1) {
    queryn("xi, xf, dx: ", s, 100);
    sscanf(s, "%lf, %lf, %lf",
      &xi, &xf, &dx);
    queryn("_zero: ", s, 100);
    sscanf(s, "%lf", &_zero);
    xi = zero_i(fn,xi,xf,dx,_zero);
    printf("fn(%lg) = %lg\n",
      xi, fn(xi));
    if (xi<=xf)
        fn_after(xi);
    }
  }

