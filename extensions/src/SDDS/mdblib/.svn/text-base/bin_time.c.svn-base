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
 Revision 1.2  1995/09/05 21:16:47  saunders
 First test release of the SDDS1.5 package.

*/

#include <descrip.h>
#include <ssdef.h>
#include "mdb.h"

/* routine: bin_time
 * purpose: to convert time to system binary format
 * Michael Borland, 1986.
 */

bin_time(btime, time)
long btime[2];		/* array to put system format time into */
double time;		/* time in seconds */
{
    struct dsc$descriptor_s t_desc;
    char t_asc[15];
    long h, m;
    long code;

    /* prepare ascii representation of time in "0 hh:mm:ss.ss" format */
    h = (int)(time/3600);
    time -= h*3600;
    m = (int)(time/60);
    time -= m*60;
    sprintf(t_asc, "0 %d:%d:%.2f\0", h, m, time);

    /* create a string descriptor acceptable to sys$bintime() */
    make_string_dsc(&t_desc, t_asc);    

    if ((code = sys$bintim(&t_desc, btime))!=SS$_NORMAL)
        lib$signal(code, 0, 0);
    }

