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
 Revision 1.3  2000/01/18 21:30:20  soliday
 Changed compress to compressString to avoid conflict with ZLIB.

 Revision 1.2  1995/09/05 21:17:02  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include <ctype.h>


/* routine: compressString()
 * purpose: eliminate repeated occurences of all characters in string
 *          t from string s
 *
 * Michael Borland, 1989
 */

char *compressString(char *s, char *t)
{
    register char *ptr, *ptr0, *tptr;

    ptr = ptr0 = s;
    while (*ptr0) {
        tptr = t;
        while (*tptr) {
            if (*tptr!=*ptr0) {
                tptr++;
                continue;
                }
            while (*++ptr0==*tptr)
                ;
            tptr++;
            ptr0--;
            }
        *ptr++ = *ptr0++;
        }
    *ptr = 0;
    return(s);
    }

