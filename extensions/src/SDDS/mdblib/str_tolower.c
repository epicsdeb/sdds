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

/* routine: str_tolower()
 * purpose: convert a string to lower case 
 *
 * Michael Borland, 1989.
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:30  saunders
 First test release of the SDDS1.5 package.

 */

char *str_tolower(s)
char *s;
{
    register char *ptr;

    ptr = s;
    while (*ptr) {
        *ptr = tolower(*ptr);
        ptr++;
        }
    return(s);
    }
