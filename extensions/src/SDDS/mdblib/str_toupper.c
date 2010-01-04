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

/* routine: str_toupper()
 * purpose: convert a string to upper case 
 *
 * Michael Borland, 1989.
 $Log: str_toupper.c,v $
 Revision 1.3  2002/08/14 16:06:15  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:18:32  saunders
 First test release of the SDDS1.5 package.

 */

char *str_toupper(s)
char *s;
{
    register char *ptr;

    ptr = s;
    while (*ptr) {
        *ptr = toupper(*ptr);
        ptr++;
        }
    return(s);
    }

