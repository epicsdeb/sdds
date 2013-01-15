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
 Revision 1.2  1995/09/05 21:17:20  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#ifdef VAX_VMS
#include <descrip.h>
#include <ssdef.h>
#endif

/* routine: err_mess(), err_mess_sys()
 * purpose: print an error message to the terminal in a standard format
 *	    allowing easy returns in the calling routine
 *	    In addition, err_mess_sys() makes a call to the system
 *	    error message facility lib$signal().
 *
 * Michael Borland, 1986.
 * usage example:
    	if (status!=1)
    		return(err_mess(status, "routine", "message"));
 */

#ifdef VAX_VMS
long err_mess_sys(long status, char *routine, char *message)
{
    long lib$signal();
    
    if (status==1) return(1);
    printf("error: %s (in %s)\n", message, routine);
    lib$signal(status, 0, 0);
    return(status);
    }
#endif

long err_mess(long status, char *routine, char *message)
{
    if (status==1) return(1);
    printf("error: %s (in %s)\n", message, routine);
    return(status);
    }

