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
 Revision 1.2  1995/09/05 21:17:39  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include <ctype.h>

/* routine: insert()
 * purpose: insert a string at some point in another string
 * caveat : the string is assumed to be long enough to accept additional
 *          characters.
 *
 * Michael Borland, 1989.
 */

char *insert(s, t)
char *s;        /* pointer to character in string to insert at */
char *t;
{
    register long i, n;

    n = strlen(t);
    if (n==0)
        return(s);

    for (i=strlen(s); i>=0; i--) {
        s[i+n] = s[i];
        }

    for (i=0; i<n; i++)
        s[i] = t[i];

    return(s);
    }

