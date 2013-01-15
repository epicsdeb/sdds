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
 Revision 1.4  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:06  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:17:04  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: cp_str()
 * purpose: copy a string, allocating memory for storage
 */
#include "mdb.h"

char *cp_str(char **s, char *t)
{
    if (t==NULL)
        *s = NULL;
    else {
	*s = tmalloc(sizeof(*t)*(strlen(t)+1));
	strcpy_ss(*s, t);
        }
    return(*s);
    }

/* routine: cpn_str()
 * purpose: copy a certain number of characters from a string,
 */
	    
char *cpn_str(char **s, char *t, long n)
{
    if (t==NULL)
        *s = NULL;
    else {
        *s = tmalloc(sizeof(*t)*(n+1));
        strncpy(*s, t, n);
        (*s)[n] = 0;
        }
    return(*s);
    }
	
