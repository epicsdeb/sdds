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
 Revision 1.4  2007/02/02 18:45:27  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:04  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:16:49  saunders
 First test release of the SDDS1.5 package.

*/

/* file: binary.c
 * contents: sbinary()
 * 
 * Michael Borland, 1990
 */
#include "mdb.h"

char *sbinary(char *s, int len, long number)
{
    long pow_of_2, n, i;
    char c;

    pow_of_2 = 1;
    n = 0;
    strcpy_ss(s, "0");
    while (pow_of_2>0 && pow_of_2<=number && n<len-1) {
        if (pow_of_2&number) {
            number -= pow_of_2;
            s[n] = '1';
            }
        else 
            s[n] = '0';
        s[++n] = 0;
        pow_of_2 = pow_of_2 << 1;
        }
    i = -1;
    while (--n > ++i) {
        c    = s[n];
        s[n] = s[i];
        s[i] = c;
        }
    return(s);
    }

/* routine: bitsSet
 * M. Borland, 1995
 */

long bitsSet(unsigned long data)
{
    long i = 0;
    while (data) {
        if (data&1)
            i++;
        data >>= 1;
        }
    return i;
    }
