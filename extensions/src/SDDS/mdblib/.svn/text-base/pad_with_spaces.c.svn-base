/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: pad_with_spaces
 * purpose: add a specified number of spaces to the end of a string.
 * 
 * Michael Borland, 1990. 
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:01  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

char *pad_with_spaces(char *s, int n)
{
    char *ptr;

    ptr = s + strlen(s);
    while (n--)
        *ptr++ = ' ';
    *ptr = 0;
    return(s);
    }   
