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
 Revision 1.2  1995/09/05 21:16:45  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: backspace()
 * purpose: backspace by a specified number of characters 
 *
 * Michael Borland, 1991
 */
#include "mdb.h"

void backspace(long n)
{
    static char *bspace = NULL;
    static long n_bspace = 0;

    if (n>n_bspace) {
        register long i;
        bspace = trealloc(bspace, sizeof(*bspace)*(n+1));
        for (i=n_bspace; i<n; i++)
            bspace[i] = '\b';
        n_bspace = n;
        }
    bspace[n] = 0;
    fputs(bspace, stdout);
    bspace[n] = '\b';
    }

