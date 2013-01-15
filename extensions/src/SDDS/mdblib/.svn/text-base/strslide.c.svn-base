/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "mdb.h"

/* routine: strslide()
 * purpose: portable routine to slide character data within a string.
 *
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:34  saunders
 First test release of the SDDS1.5 package.

 */

char *strslide(char *s, long distance)
{
    char *source, *target;
    long i, length;

    if (!s || !distance)
        return s;
    if (distance>0) {
        /* slide toward higher index */
        source = s + (length=strlen(s));
        if (distance>length)
            return NULL;
        target = source + distance;
        for (i=length; i>=0; i--)
            *target-- = *source--;
        }
    else if (distance<0) {
        /* slide toward lower index */
        length = strlen(s);
        if ((distance = -distance)>=length) 
            *s = 0;
        else {
            source = s + distance;
            target = s;
            do {
                *target++ = *source++;
                } while (*source);
            *target = 0;
            }
        }
    return s;
    }
