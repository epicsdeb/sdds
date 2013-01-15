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
 Revision 1.2  1995/09/05 21:17:32  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

main()
{
    char *ptr;
    long i;
    malloc_debug(1);
    for (i=0; i<10; i++)
        ptr = tmalloc(sizeof(*ptr)*1000);
    free(ptr+1);
    malloc_verify();
    }
