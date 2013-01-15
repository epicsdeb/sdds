/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: tmpname()
 * purpose: to supply a UNIQUE temporary filename
 * Michael Borland, 1986.
 $Log: not supported by cvs2svn $
 Revision 1.7  1999/12/21 17:54:41  soliday
 vxWorks does not support getpid

 Revision 1.6  1999/12/15 15:43:53  soliday
 Added vxWorks support.

 Revision 1.5  1999/09/24 20:44:27  borland
 No longer issues a warning if the filename is already in use.
 Prevents some problems on NT.

 Revision 1.4  1999/07/08 20:39:51  soliday
 Added include statements for process.h and unistd.h for the getpid function.

 Revision 1.3  1999/05/25 21:53:43  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:18:40  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"
#include <time.h>
#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif
char *tmpname(s)
char *s;
{
    static long i = 1;
    static long pid = -1;
    if (s==NULL) 
    	s = tmalloc((unsigned)(40*sizeof(*s)));
    if (pid<0) 
#if !defined(vxWorks)
        pid = getpid();
#endif
    do {
#if defined(vxWorks)
        sprintf(s, "tmp.%ld", i);
#else
        sprintf(s, "tmp%ld.%ld", pid, i);
#endif
        i += 1;
        if (!fexists(s)) 
            break;
        } while (1);
    return(s);
    }
