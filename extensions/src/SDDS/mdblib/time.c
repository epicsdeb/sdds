/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: time.c
 * contents: mtime(), convert_date_time()
 * 
 * Michael Borland, 1987
 $Log: time.c,v $
 Revision 1.6  2009/09/23 21:45:56  borland
 Added mtimes() (like mtime(), but with seconds).

 Revision 1.5  2002/08/14 16:06:16  soliday
 Added Open License

 Revision 1.4  1999/10/27 14:37:35  borland
 Removed unnecessary prototypes that caused problems with some compilers.

 Revision 1.3  1999/05/25 21:53:32  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:18:36  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"
#include <ctype.h>
#include <time.h>

#ifdef VAX_VMS

static char *month[12] = {
    "january", "february", "march",
    "april", "may", "june", "july",
    "august", "september", "october",
    "november", "december"
    };

/* routine: convert_date_time()
 * purpose: convert time string, as from mtime(), to DATE_TIME structure.
 * assumed format of input string: day month year hours:minutes
 * The month must be written out (e.g., "June", not 6).
 * Michael Borland, 1987
 */

convert_date_time(dt, ct0)
DATE_TIME *dt;
char *ct0;
{
    register char *ptr, *ct;
    register long i, l;
    
    /* copy the string */
    cp_str(&ct, ct0);

    /* shift everything to lower case */
    ptr = ct;
    while (*ptr) {
        *ptr = tolower(*ptr);
    	ptr++;
    	}

    /* scan the day */
    if (!get_long(&(dt->day), ct))
    	return(0);

    /* scan the month */
    ptr = get_token(ct);
    l = strlen(ptr);
    for (i=0; i<12; i++) 
    	if (strncmp(ptr, month[i], l)==0) 
    	    break;
    if (i==12) 
    	return(0);
    dt->month = i+1;

    if (!get_long(&(dt->year), ct))
    	return(0);
    if (dt->year<100)
    	dt->year += 1900;

    dt->hrs  = 0;
    dt->mins = 0;
    dt->secs = 0;
    while (*ct==' ' && *ct)
 	ct++;
    if (sscanf(ct, "%ld:%ld", &(dt->hrs), &(dt->mins))!=2)
    	return(NO_TIME);

    return(1);
    }
#endif

/* routine: mtime()
 * purpose: to return a more useful time string than does ctime()
 * 	ctime == wkd mmm dd hh:mm:ss 19yy\n
 * 	mtime == dd mmm yy hh:mm
 * Michael Borland, 1986.
 */

char *mtime(void)
{
    char *ct, *mt;
    char *month, *day, *t, *ptr;
    long i;
    time_t time();

    while ((mt = tmalloc((unsigned)30*sizeof(*mt)))==NULL)
    	puts("allocation failure in mtime()");
    time(&i);
    ct = ctime(&i) + 4;
    *(ct+strlen(ct)-1) = 0;

    month = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
    	*ct++ = 0;

    day = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
        *ct++ = 0;

    t = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
        *ct++ = 0;
    ptr = strrchr(t, ':');
    *ptr = 0;

    sprintf(mt, "%s %s %s %s", day, month, ct+2, t);    
    return(mt);
    }       


/* routine: mtimes()
 * purpose: to return a more useful time string than does ctime()
 * 	ctime == wkd mmm dd hh:mm:ss 19yy\n
 * 	mtimes == dd mmm yy hh:mm:ss
 * Michael Borland, 1986.
 */

char *mtimes(void)
{
    char *ct, *mt;
    char *month, *day, *t, *ptr;
    long i;
    time_t time();

    while ((mt = tmalloc((unsigned)30*sizeof(*mt)))==NULL)
    	puts("allocation failure in mtime()");
    time(&i);
    ct = ctime(&i) + 4;
    *(ct+strlen(ct)-1) = 0;

    month = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
    	*ct++ = 0;

    day = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
        *ct++ = 0;

    t = ct;
    ct = strchr(ct, ' ');
    while (*ct==' ')
        *ct++ = 0;
    
    sprintf(mt, "%s %s %s %s", day, month, ct+2, t);    
    return(mt);
    }       




