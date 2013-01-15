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
 Revision 1.2  1995/09/05 21:17:45  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include <ctype.h>


/* routine: is_blank()
 * purpose: determine whether a string is all space characters or not 
 */

long is_blank(char *s)
{
    register char *ptr;

    ptr = s;
    while (*ptr && isspace(*ptr))
        ptr++;
    if (*ptr)
        return(0);
    return(1);
    }

