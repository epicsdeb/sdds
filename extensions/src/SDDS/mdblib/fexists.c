/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: fexists.c,v $
 Revision 1.5  2002/08/14 16:06:08  soliday
 Added Open License

 Revision 1.4  1999/07/22 15:35:48  soliday
 Uses macro for fopen mode.

 Revision 1.3  1999/05/25 21:52:48  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:26  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

long fexists(char *filename)
{
    FILE *fp;

    if ((fp=fopen(filename, FOPEN_READ_MODE))) {
        fclose(fp);
        return(1);
        }
    return(0);
    }
