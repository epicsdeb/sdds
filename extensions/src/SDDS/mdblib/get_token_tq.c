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
 Revision 1.8  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.7  2003/08/28 20:13:43  soliday
 Cleaned up the code.

 Revision 1.6  2002/08/14 16:06:09  soliday
 Added Open License

 Revision 1.5  2000/04/05 21:33:12  soliday
 Added prototype for seek_level

 Revision 1.4  1999/09/14 18:12:18  soliday
 interpret_escaped_quotes is defined in a header file instead of locally

 Revision 1.3  1999/05/25 21:55:25  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:17:34  saunders
 First test release of the SDDS1.5 package.

*/

/* file: get_token_t.c
 * contents: get_token_t(), in_charset(), get_token_tq()
 *           seek_level()
 * Michael Borland, 1988 
 */
#include "mdb.h"
#include <ctype.h>

char *seek_level(char *s, char qs, char qe);

int in_charset(char c, char *s);

/* routine: get_token_t()
 * purpose: get a token from a character string, where a token is defined
 *          as anything bounded by two of the characters from the token
 *          string.
 */

char *get_token_t(char *s, char *t)
{
  char *ptr0, *ptr1, *ptr;
  
  /* skip all leading characters of s found in string t */  
  ptr0 = s;
  while (in_charset(*s, t) && *s)
    s++;
  if (*s==0) return(NULL);
  ptr1 = s;

  /* skip to next character of s found in t, skipping over quoted */
  /* portions */
  do {
    if (*s=='"' && !(s!=ptr0 && *(s-1)=='\\')) {
        s++;
        while (*s && !(*s=='"' && *(s-1)!='\\') )
            s++;
        if (*s=='"')
            s++;
        }
    else
        s++;
    } while (*s && !in_charset(*s, t));
  
  ptr = tmalloc(sizeof(*ptr)*(s-ptr1+1));
  strncpy(ptr, ptr1, s-ptr1);
  ptr[s-ptr1]=0;
 
  strcpy_ss(ptr0, s);

  interpret_escaped_quotes(ptr);
  return(ptr);
  }

/* routine: in_charset()
 * purpose: determine if character is a member of a set of characters.
 *          Returns the number of the character that matches, or zero
 *          for no match.
 */

int in_charset(char c, char *set)
{
    register int i;

    i = 1;
    while (*set) {
        if (*set==c) 
            return(i);
        set++;
        i++;
        }
    return(0);
    }

/* routine: get_token_tq()
 * purpose: get a token from a character string, where a token is defined
 *          as anything bounded by from the front by one of the characters
 *          from the token first string and from the end by one of the 
 *          characters from the second token string.  This version also 
 *          recognizes generalized quotation marks, using two parallel strings
 *          of such (beginning and ending) marks supplied by the caller.  
 *          Beginning quotation characters are construed as token-start 
 *          characters and ending quotation characters as token-end characters,
 *          but anything inside of a quoted section can not end a token.  
 *          Bounding token start/end characters are deleted from the token 
 *          before it is returned, as are PAIRS of corresponding quote
 *          characters.
 *
 *          For example, to parse MAD beamline definitions, one could use:
 *          ts = ","    te = ","   qs = "("   qe = ")" .
 *          This would section the string into sub-strings of items 
 *          separated by commas, with parenthesized expressions kept
 *          intact.
 */


char *get_token_tq(s, ts, te, qs, qe)
char *s;	/* the string to be scanned */
char *ts, *te;  /* strings of token start and end characters */
char *qs, *qe;  /* strings of starting and ending quotation characters */
{
    register char *ptr0, *ptr1, *ptr;
    register int in_quotes;

    /* skip all leading characters of s found in string t */  
    ptr0 = s;
    while (*s && in_charset(*s, ts) && !in_charset(*s, qs)) {
        s++;
        }
    if (*s==0) 
        return(NULL);
    ptr1 = s;
    if ((in_quotes=in_charset(*s, qs)))
        s++;

    /* skip to next character of s found in t */
    do {
        if (in_quotes) {
            if ((ptr = seek_level(s, *(qs+in_quotes-1), *(qe+in_quotes-1)))) {
                s = ptr;
                in_quotes = 0;
                }
            else {
                s += strlen(s);
                in_quotes = 0;
                }
            }
        else {
            in_quotes = in_charset(*s, qs);
            s++;
            }
        } while (*s && (in_quotes || !in_charset(*s, te)));
  
    ptr = tmalloc((unsigned)sizeof(*ptr)*(s-ptr1+1));
    strncpy(ptr, ptr1, s-ptr1);
    ptr[s-ptr1]=0;
 
    if (*s) 
        strcpy_ss(ptr0, s+1);
    else 
        *ptr0 = *s;

    interpret_escaped_quotes(ptr);
    return(ptr);
    }


/* routine: seek_level()
 * purpose: seek through a string to find the place where a parenthesis
 *          or quotation-mark nesting of zero occurs, assuming that
 *          a nesting of 1 existed prior to the call.  Note that if
 *          qs=qe, the routine simply finds the next quotation-mark.
 *          The routine returns a pointer to the place in the string
 *          where the nesting is zero (after the last quotation-mark
 *          or parenthesis).
 */

char *seek_level(char *s, char qs, char qe)
{
    register int qlevel;
    char *ptr0;

    ptr0 = s;
    qlevel = 1;
    while (*s && qlevel) {
        if (*s==qe && !(s!=ptr0 && *(s-1)=='\\'))
            qlevel--;
        else if (*s==qs && !(s!=ptr0 && *(s-1)=='\\'))
            qlevel++;
        s++;
        }
    if (qlevel==0)
        return(s);
    return(NULL);
    }

void interpret_escaped_quotes(char *s)
{
    char *ptr;

    ptr = s;
    while (*ptr) {
        if (*ptr=='\\' && *(ptr+1)=='"')
            strcpy_ss(ptr, ptr+1);
        else
            ptr++;
        }
    }
