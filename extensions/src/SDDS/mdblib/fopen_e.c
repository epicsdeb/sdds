/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: fopen_e()
 * purpose: open file with error checking, messages, and aborts
 *
 * Michael Borland, 1987.
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/05/25 21:53:01  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:30  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

FILE *fopen_e(char *file, char *open_mode, long mode)
{
    FILE *fp;
    static char buffer[1024];

    if ((mode&FOPEN_SAVE_IF_EXISTS) && fexists(file)) {
        sprintf(buffer, "%s~", file);
        if (rename(file, buffer)!=0) {
            fprintf(stderr, "error: cannot save previous version of %s--new file not opened.\n", file);
            if (mode&FOPEN_RETURN_ON_ERROR)
                return(NULL);
            exit(1);
	    }
        }
            
    if ((fp=fopen(file, open_mode))) {
        if (mode&FOPEN_INFORM_OF_OPEN)
            printf("%s opened in mode %s\n", file, open_mode);
    	return(fp);
        }
    
    sprintf(buffer, "unable to open %s in mode %s", file, open_mode);
    perror(buffer);

    if (!(mode&FOPEN_RETURN_ON_ERROR))
    	exit(1);

    return(NULL);
    }
