/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "mdb.h"
#include <ctype.h>

/* routine: str_inn()
 * purpose: return pointer to the position at which string
 *          t occurs in string s.  Check only the first n characters
 *          of string s.
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:29  saunders
 First test release of the SDDS1.5 package.

 */

char *str_inn(s, t, n)
char *s, *t;
long n;
{
    register char *ps0, *pt, *ps;
    register long i;

    if (s==NULL || t==NULL)
        return(NULL);

    ps0 = s;
    i = strlen(t);
    while (*ps0 && n>=i) {
        i++;
        if (*t==*ps0) {
            ps = ps0 + 1;
            pt = t   + 1;
            while (*pt && *ps==*pt) {
                pt++;
                ps++;
                }
            if (*pt==0) 
                return(ps0);
            }
        ps0++;
        }
    return(NULL);
    }

