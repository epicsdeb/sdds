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
 Revision 1.2  1995/09/05 21:16:53  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: bomb()
 * purpose: report messages to terminal and abort.
 * usage:
 *  bomb(error_description, usage_description)
 *  char *error_description, *usage_description;
 */
#include "mdb.h"

void bomb(char *error, char *usage)
{
    if (error)
    	fprintf(stderr, "error: %s\n", error);
    if (usage)
    	fprintf(stderr, "usage: %s\n", usage);
    exit(1);
    }

long bombre(char *error, char *usage, long return_value)
{
    if (error)
    	fprintf(stderr, "error: %s\n", error);
    if (usage)
    	fprintf(stderr, "usage: %s\n", usage);
    return return_value;
    }
