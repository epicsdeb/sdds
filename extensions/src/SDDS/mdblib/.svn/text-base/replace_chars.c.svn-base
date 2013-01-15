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

/* routine: replace_chars()
 * purpose: map one set of characters into another
 *
 * Michael Borland, 1989.
 $Log: not supported by cvs2svn $
 Revision 1.4  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:12  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:18:11  saunders
 First test release of the SDDS1.5 package.

 */

char *replace_chars(s, from, to)
char *s, *from, *to;
{
    long lt, lf;
    char *ptr, *ptr_to, *ptr_from;

    if ( (lt=strlen(to)) < (lf=strlen(from)) ) {
        char *to_temp;
        to_temp = tmalloc(sizeof(*to_temp)*(lf+1));
        strcpy_ss(to_temp, to);
        for ( ; lt<lf; lt++)
            to_temp[lt] = ' ';
        to_temp[lf] = 0;
        }
    else if (lt>lf) {
        char *from_temp;
        from_temp = tmalloc(sizeof(*from_temp)*(lt+1));
        strcpy_ss(from_temp, from);
        for ( ; lf<lt; lf++)
            from_temp[lf] = ' ';
        from_temp[lt] = 0;
        }

    ptr = s;
    while (*ptr) {
        ptr_to = to;
        ptr_from = from;
        while (*ptr_from)
            if (*ptr_from!=*ptr) {
                ptr_from++;
                ptr_to++;
                }
            else {
                *ptr = *ptr_to;
                break;
                }
        ptr++;
        }
    return(s);
    }
