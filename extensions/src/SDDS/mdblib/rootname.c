/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: root_filename()
 * purpose: remove extension specification from a filename string
 * 
 * Michael Borland, 1989
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/05/25 21:53:24  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:18:19  saunders
 First test release of the SDDS1.5 package.

 */

#include "mdb.h"
#include <ctype.h>


char *root_filename(filename)
char *filename;
{
    register char *ptr;

    if (!(ptr=strchr(filename, ']')))
        ptr = filename;
    if ((ptr=strchr(ptr, '.')))
        *ptr = 0;
    return(filename);
    }

