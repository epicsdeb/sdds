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
 Revision 1.6  2005/04/07 19:33:22  borland
 Added WILDCARD_MATCH ability to match_string.  Used by sddsxref -wildMatch option.

 Revision 1.5  2002/08/14 16:06:11  soliday
 Added Open License

 Revision 1.4  2002/07/24 15:32:16  borland
 Prototypes for strcmp_case_insensitive() and strncmp_case_insensitive() are
 now in match_string.h.

 * Revision 1.3  1995/11/13  16:18:50  borland
 * Added long cast to strlen() in comparisons to satisfy Solaris compiler.
 *
 * Revision 1.2  1995/09/05  21:17:54  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* routine: match_string()
 * purpose: return the number in the string array corresponding
 *          to the string given.  Various matching modes are available.
 */
#include "match_string.h"
#include "mdb.h"
#include <ctype.h>


long match_string(
    char *string,         /* string to find match for */
    char **option,        /* strings to match with */
    long n_options,       /* number of strings to match with */
    long mode             /* matching mode flags */
    )
{
    register long i, i_match, l;

    if (string==NULL)
        return(-1);

    if (mode&WILDCARD_MATCH) {
      for (i=0; i<n_options; i++) 
	if (wild_match(string, option[i])) 
	  return i;
      return -1;
    }

    if (!(mode&MATCH_WHOLE_STRING)) {
        l = strlen(string);
        i_match = -1;
        if (mode&CASE_SENSITIVE) {
            for (i=0; i<n_options; i++) {
                if (strncmp(string, option[i], l)==0) {
                    if (mode&RETURN_FIRST_MATCH)
                        return(i);
                    if (i_match!=-1)
                        return(-1);
                    i_match = i;
                    }
                }
            return(i_match);
            }
        else {
            for (i=0; i<n_options; i++) {
                if (strncmp_case_insensitive(string, option[i], MIN(l,(long)strlen(option[i])))==0) {
                    if (mode&RETURN_FIRST_MATCH)
                        return(i);
                    if (i_match!=-1)
                        return(-1);
                    i_match = i;
                    }
                }
            return(i_match);
            }
        }

    if (mode&MATCH_WHOLE_STRING) {
        i_match = -1;
        if (mode&CASE_SENSITIVE) {
            for (i=0; i<n_options; i++) {
                if (strcmp(string, option[i])==0) {
                    if (mode&RETURN_FIRST_MATCH)
                        return(i);
                    if (i_match!=-1)
                        return(-1);
                    i_match = i;
                    }
                }
            return(i_match);
            }
        else {
            for (i=0; i<n_options; i++) {
                if (strcmp_case_insensitive(string, option[i])==0) {
                    if (mode&RETURN_FIRST_MATCH)
                        return(i);
                    if (i_match!=-1)
                        return(-1);
                    i_match = i;
                    }
                }
            return(i_match);
            }
        }

    /* unknown set of flags */
    puts("error: unknown flag combination in match_string()");
    puts("       contact programmer!");
    exit(1);
    }

int strcmp_case_insensitive(char *s1, char *s2)
{
    register char *ptr1, *ptr2;

    ptr1 = s1;
    ptr2 = s2;
    while (*ptr1 && *ptr2 && tolower(*ptr1)==tolower(*ptr2)) {
        ptr1++;
        ptr2++;
        }
    return((int)(*ptr1-*ptr2));
    }

int strncmp_case_insensitive(char *s1, char *s2, long n)
{
    register char *ptr1, *ptr2;
    register long i;

    ptr1 = s1;
    ptr2 = s2;
    i = 0;
    while (i<n && *ptr1 && *ptr2 && tolower(*ptr1)==tolower(*ptr2)) {
        ptr1++;
        ptr2++;
        i++;
        }

    if (i==n)
        return(0);

    return((int)(*ptr1-*ptr2));
    }

