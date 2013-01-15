/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: trim_spaces()
 * M.Borland 1993
 $Log: not supported by cvs2svn $
 Revision 1.4  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:17  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:18:42  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

char *trim_spaces(char *s)
{
    char *ptr;

    ptr = s;
    while (*ptr==' ')
        ptr++;
    if (ptr!=s)
        strcpy_ss(s, ptr);
    ptr = s + strlen(s) - 1;
    while (*ptr==' ' && ptr!=s)
        ptr--;
    *++ptr = 0;
    return(s);
    }
