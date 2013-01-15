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
 Revision 1.3  1999/09/14 18:13:43  soliday
 Added mdb.h include statement

 Revision 1.2  1995/09/05 21:19:39  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: factorial()
 *
 * Michael Borland, 1991 
 */
#include "mdb.h"

long factorial(long n)
{
    register long prod=1;

    while (n>0)
        prod *= n--;

    return(prod);
    }


double dfactorial(long n)
{
    register double prod=1;

    while (n>0)
        prod *= n--;

    return(prod);
    }

