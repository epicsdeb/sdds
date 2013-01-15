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
 Revision 1.5  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.4  2002/08/14 16:06:06  soliday
 Added Open License

 Revision 1.3  1999/05/25 21:54:35  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:15  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include <ctype.h>

/* routine: delete_chars()
 * purpose: delete from one string all occurences of characters from another string
 *
 * Michael Borland, 1993.
 */

char *delete_chars(char *s, char *t)
{
    char *ps, *pt;

    ps = s;
    while (*ps) {
        pt = t;
        while (*pt) {
            if (*pt==*ps) {
                strcpy_ss(ps, ps+1);
                ps--;
                break;
                }
            pt++;
            }
        ps++;
        }
    return(s);
    }
