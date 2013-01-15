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

 Revision 1.4  2002/08/14 16:06:05  soliday
 Added Open License

 Revision 1.3  1999/05/25 21:52:10  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:00  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: clean_filename()
 * purpose: remove path and version specifications from a filename string
 * 
 * Michael Borland, 1989
 */

#include "mdb.h"
#include <ctype.h>


char *clean_filename(char *filename)
{
    register char *ptr;

    if ((ptr=strchr(filename, ']'))) 
        strcpy_ss(filename, ptr+1);
    if ((ptr=strchr(filename, ';')))
        *ptr = 0;
    return(filename);
    }

