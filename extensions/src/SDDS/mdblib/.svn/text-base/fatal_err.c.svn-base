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
 Revision 1.2  1995/09/05 21:17:24  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#ifdef VAX_VMS
#include <descrip.h>
#include <ssdef.h>
#endif

/* routine: fatal_err()
 * purpose: Reports a fatal error to the terminal and aborts execution.
 *
 * Michael Borland, 1986.
 */

void fatal_err(long err_code, char *error_mess)
{
    fprintf(stderr, "error: %s \n", error_mess);
#ifdef VAX_VMS 
    if (err_code!=1) 
    	lib$signal(err_code, 0, 0);
#endif
    exit(1);
    }
