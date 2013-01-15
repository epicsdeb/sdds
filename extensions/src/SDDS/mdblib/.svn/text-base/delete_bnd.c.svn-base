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
 Revision 1.4  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:06  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:17:13  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include <ctype.h>

/* routine: delete_bounding()
 * purpose: delete from string all bounding characters from another string
 *
 * Michael Borland, 1989.
 */

char *delete_bounding(s, t)
char *s, *t;
{
    register char *ptr1, *ptr0, *ptrt;

    if (!s)
        return(s);
    ptr0 = s;
    while (*ptr0) {
        ptrt = t;
        while (*ptrt && *ptrt!=*ptr0)
            ptrt++;
        if (*ptrt==*ptr0)
            ptr0++;
        else 
            break;
        }

    ptr1 = ptr0 + strlen(ptr0) - 1;
    while (ptr1!=ptr0) {
        ptrt = t;
        while (*ptrt && *ptrt!=*ptr1)
            ptrt++;
        if (*ptrt==*ptr1)
            ptr1--;
        else 
            break;
        }

    *++ptr1 = 0;
    strcpy_ss(s, ptr0);
    return(s);
    }

