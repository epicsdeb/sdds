/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file    : query.c
 * contents: query_double(), query_int()
 *
 * Michael Borland,1987
 $Log: not supported by cvs2svn $
 Revision 1.4  2002/08/14 16:06:11  soliday
 Added Open License

 Revision 1.3  1999/05/25 21:53:12  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:18:03  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

/* routine: query_double()
 * purpose: prompt terminal for double precision number, displaying
 *	    default value and returning that value if no number is
 *	    entered.
 */

double query_double(char *prompt, double def)
{
    static char s[100];
    static double val;

    printf("%s [%g]: ", prompt, def);
    fgets(s, 99, stdin);
    val = def;
    if (*s) 
    	sscanf(s, "%lf", &val);
    return(val);
    }

/* routine: query_float()
 * purpose: prompt terminal for float precision number, displaying
 *	    default value and returning that value if no number is
 *	    entered.
 */

float query_float(char *prompt, float def)
{
    static char s[100];
    static float val;

    printf("%s [%g]: ", prompt, def);
    fgets(s, 99, stdin);
    val = def;
    if (*s) 
    	sscanf(s, "%f", &val);
    return(val);
    }

/* routine: query_long()
 * purpose: prompt terminal for a long integer, displaying
 *	    default value and returning that value if no number is
 *	    entered.
 */

long query_long(char *prompt, long def)
{
    static char s[100];
    static long val;

    printf("%s [%ld]: ", prompt, def);
    fgets(s, 99, stdin);
    val = def;
    if (*s) 
    	sscanf(s, "%ld", &val);
    return(val);
    }

/* routine: query_int()
 * purpose: prompt terminal for an integer, displaying
 *	    default value and returning that value if no number is
 *	    entered.
 */

int query_int(char *prompt, int def)
{
    static char s[100];
    static int val;

    printf("%s [%d]: ", prompt, def);
    fgets(s, 99, stdin);
    val = def;
    if (*s) 
    	sscanf(s, "%d", &val);
    return(val);
    }

/* routine: query_short()
 * purpose: prompt terminal for a short integer, displaying
 *	    default value and returning that value if no number is
 *	    entered.
 */

short query_short(char *prompt, short def)
{
    static char s[100];
    static short val;

    printf("%s [%d]: ", prompt, def);
    fgets(s, 99, stdin);
    val = def;
    if (*s) 
    	sscanf(s, "%hd", &val);
    return(val);
    }

