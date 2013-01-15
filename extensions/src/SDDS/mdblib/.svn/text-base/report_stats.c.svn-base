/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* ROUTINE: report_stats()
 * PURPOSE: report elapsed time, cpu time, bio/dio, and page faults to file
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/05/25 21:54:23  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:18:17  saunders
 First test release of the SDDS1.5 package.
 
 */
#include "mdb.h"

#if !defined(__TURBOC__)
void report_stats(FILE *fp, char *label)
{
    char s[200];
    extern char *elapsed_time();
    long cpu_time(), bio_count(), dio_count(), page_faults(), memory_count();

    sprintf(s, "ET:%13s CP:%8.2f BIO:%ld DIO:%ld PF:%ld MEM:%ld", elapsed_time(),
            cpu_time()/100.0, bio_count(), dio_count(), page_faults(), memory_count());

    fprintf(fp, "%s   %s\n", label, s);
    fflush(fp);
    }
#else
void report_stats(FILE *fp, char *label)
{
    }
#endif
