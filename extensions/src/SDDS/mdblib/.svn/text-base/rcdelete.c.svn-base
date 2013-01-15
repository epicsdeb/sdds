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

/* routine: rcdelete
 * purpose: delete from a string every instance of any character from the
 *          set of characters between two specified characters in the 
 *          ASCII table, including those characters.
 *
 * Michael Borland, 1989.
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:07  saunders
 First test release of the SDDS1.5 package.

 */

char *rcdelete(char *s, char c0, char c1)
{
    register char *ptr0, *ptr1;
    
    ptr0 = ptr1 = s;
    while (*ptr1) {
        if (*ptr1<c0 || *ptr1>c1)
            *ptr0++ = *ptr1++;
        else ptr1++;
        }
    *ptr0 = 0;
    return(s);
    }


