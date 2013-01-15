/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: exp_notation()
 * purpose: convert a number into a string in exponential notation,
 *          suitable for use with MPL plotting program.
 * Michael Borland, 1989
 $Log: not supported by cvs2svn $
 Revision 1.5  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.4  2002/08/14 16:06:07  soliday
 Added Open License

 Revision 1.3  1999/05/25 21:52:37  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:22  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

char *exp_notation(double x, long n1, long n2)
{
    char t[100], s[100], format[100];
    char *ptr;

    sprintf(format, "%%%ld.%ldle", n1, n2);
    sprintf(s, format, x);
    if ((ptr = strchr(s, 'e'))) {
        *ptr++ = 0;
        if (*ptr=='+') {
            ptr++;
            while (*ptr=='0')
                ptr++;
            }
        else {
            while (*(ptr+1)=='0')
                strcpy_ss(ptr+1, ptr+2);
            }
        sprintf(t, "%sx10$a%s$b", s, ptr);
        cp_str(&ptr, t);
        return(ptr);
        }
    else {
        cp_str(&ptr, s);
        return(ptr);
        }
    }
    
