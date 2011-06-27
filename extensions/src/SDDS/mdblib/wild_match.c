/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: wild_match.c
 * contents: wild_match(), expand_ranges(), has_wildcards(), wild_list()
 *
 * Michael Borland, 1991-2
 */


/* function: wild_match()
 * purpose: determine whether one string is a wild-card match for another
 *
 * Known problems:
 *    the sequence [\]] is not handled properly.
 *
 * Michael Borland, 1991.
 $Log: wild_match.c,v $
 Revision 1.17  2011/01/27 18:34:04  shang
 made a copy of template in expand_ranges() to fix the core dump error when dereference it (*ptr1)

 Revision 1.16  2007/07/13 15:02:45  soliday
 Renamed strcpy_s to strcpy_ss because Microsoft is already using this name.

 Revision 1.15  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.14  2005/11/18 19:47:22  soliday
 Fixed a bug with strcmp_nh when the strings end with numbers.

 Revision 1.13  2003/12/19 19:34:11  soliday
 Added strcmp_nh function that ranks the numberic characters higher than
 other characters. It also ranks numeric character sets with fewer characters
 below numeric character sets with more characters.

 Revision 1.12  2003/08/28 20:13:44  soliday
 Cleaned up the code.

 Revision 1.11  2003/06/23 21:32:01  borland
 Fixed bug in expand_ranges().  When there were two ranges specified, it
 would read garbage from the end of a string.  Apparently this is an old
 bug.

 Revision 1.10  2002/08/14 16:06:18  soliday
 Added Open License

 Revision 1.9  2000/04/05 21:36:10  soliday
 Added proper include header statement for tolower fuction.

 Revision 1.8  1999/05/25 21:55:08  soliday
 Removed compiler warnings under Linux.

 Revision 1.7  1999/01/27 03:32:37  borland
 Fixed bug in strcmp_ci().

 Revision 1.6  1997/03/28 19:40:41  borland
 Fixed bugs in wild_match_ci()---basically brought it up to date with
 wild_match().

 * Revision 1.5  1996/03/24  23:18:53  borland
 * Fixed bug that caused access error when either template or candidate string
 * was \0.
 *
 * Revision 1.4  1995/10/11  00:24:50  borland
 * Fixed bug with templates ending in **.
 *
 * Revision 1.3  1995/09/12  03:21:21  borland
 * Added routines:
 *         wild_match_ci (case-insensitive wild-card matching)
 *         strcmp_ci     (case-insensitive string compare)
 *         strchr_ci     (case-insensitive string search )
 *
 * Revision 1.2  1995/09/05  21:18:47  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#if defined(_WIN32)
#include <stdlib.h>
#endif
#include <ctype.h>
#include "mdb.h"
#define MATCH_INVERT '^'
#define MATCH_MANY '*'
#define MATCH_SET1 '['
#define MATCH_SET2 ']'
#define ESCAPE_CHAR '\\'
#define SET_MATCH_INVERT '^'

#ifdef VAX_VMS
#define MATCH_ONE  '%'
#else
#define MATCH_ONE  '?'
#endif

int wild_match(char *string, char *template)
{
    char *s, *t, *ptr;
    int len, at_least, invert_set_match, invert_match;

#ifdef DEBUG
    printf("wild_match(%s, %s)\n", string, template);
#endif

    s = string;    /* string to check for match */
    len = strlen(s);
    t = template;  /* template string, with possible wildcards */
    if (*t==MATCH_INVERT) {
        invert_match = -1;
        t++;
        }
    else 
        invert_match = 0;

    if (!*s) {
      if (!*t)
        return(!invert_match);
      return invert_match;
    } else if (!*t) {
      return invert_match;
    }
    
    do {
#ifdef DEBUG
        printf("s = %s,  t = %s\n", s, t);
#endif
        switch (*t) {
            case ESCAPE_CHAR:
                t++;
                if (*t++ != *s++)
                    return(invert_match);
                len--;
                break;
            case MATCH_MANY:
                at_least = 0;
                while (*t) {
                    if (*t==MATCH_MANY)
                        t++;
                    else if (*t==MATCH_ONE) {
                        at_least++;
                        t++;
                        }
                    else
                        break;
                    }
                if (at_least>len)
                    return(invert_match);
                s += at_least;
                if (*t==0 && *(t-1)==MATCH_MANY) {
#ifdef DEBUG
                    printf("return(1)\n");
#endif
                    return(!invert_match);
                    }
                ptr = s;
                while (*ptr) {
                    if (wild_match(ptr, t)) {
#ifdef DEBUG
                        printf("return(2)\n");
#endif
                        return(!invert_match);
                        }
                    ptr++;
                    }
                ptr = s;
                while ((ptr=strchr(ptr, *t))) {
                    if (wild_match(ptr+1, t+1)) {
#ifdef DEBUG
                        printf("return(3)\n");
#endif
                        return(!invert_match);
                        }
                    if (*++ptr)
                        ++ptr;
                    }
                return(invert_match);
            case MATCH_ONE:
                s++;
                t++;
                len--;
                break;
            case MATCH_SET1:
                ptr = NULL;
                if (!*(t+1) || !(ptr=strchr(t+1, MATCH_SET2))) {
                    if (*++t != *++s)
                        return(invert_match);
                    len--;
                    }
                else {
                    *ptr++ = 0;
                    t++;
                    SWAP_PTR(ptr, t);
                    invert_set_match = 0;
                    if (*ptr==SET_MATCH_INVERT && strlen(ptr)!=1) {
                        invert_set_match = 1;
                        ptr++;
                        }
                    if (strchr(ptr, *s)) {
                        *(t-1) = MATCH_SET2;
                        if (invert_set_match)
                            return(invert_match);
                        s++;
                        len--;
                        }
                    else {
                        *(t-1) = MATCH_SET2;
                        if (!invert_set_match)
                            return(invert_match);
                        s++;
                        len--;
                        }
                    }
                break;
            default:
                if (*s++ != *t++)
                    return(invert_match);
                len--;
                break;
            }
        } while (*s && *t);
#ifdef DEBUG
    printf("s = %s,  t = %s\n", s, t);
#endif
    if (!*s && !*t) {
#ifdef DEBUG
        printf("return(5)\n");
#endif
        return(!invert_match);
        }
    if (!*s && *t==MATCH_MANY && !*(t+1)) {
#ifdef DEBUG
        printf("return(4)\n");
#endif
        return(!invert_match);
        }
    if (*s && !*t) {
#ifdef DEBUG
        printf("return(6)\n");
#endif
        return(invert_match);
        }
    if (!*s && *t) {
        while (*t) {
            if (*t!=MATCH_MANY) {
#ifdef DEBUG
                printf("return(7)\n");
#endif
                return(invert_match);
                }
            t++;
            }
#ifdef DEBUG
        printf("return(8)\n");
#endif
        return !invert_match;
        }
    bomb("the impossible has happened (wild_match)", NULL);
    exit(1);
    }

int wild_match_ci(char *string, char *template)
{
    char *s, *t, *ptr;
    int len, at_least, invert_set_match, invert_match;

    s = string;    /* string to check for match */
    len = strlen(s);
    t = template;  /* template string, with possible wildcards */
    if (*t==MATCH_INVERT) {
        invert_match = -1;
        t++;
        }
    else 
        invert_match = 0;

    if (!*s) {
      if (!*t)
        return(!invert_match);
      return invert_match;
    } else if (!*t) {
      return invert_match;
    }
    
    do {
#ifdef DEBUG
        printf("s = %s,  t = %s\n", s, t);
#endif
        switch (*t) {
            case ESCAPE_CHAR:
                t++;
                if (tolower(*t) != tolower(*s))
                    return(invert_match);
                t++;
                s++;
                len--;
                break;
            case MATCH_MANY:
                at_least = 0;
                while (*t) {
                    if (*t==MATCH_MANY)
                        t++;
                    else if (*t==MATCH_ONE) {
                        at_least++;
                        t++;
                        }
                    else
                        break;
                    }
                if (at_least>len)
                    return(invert_match);
                s += at_least;
                if (*t==0 && *(t-1)==MATCH_MANY) {
#ifdef DEBUG
                    printf("return(1)\n");
#endif
                    return(!invert_match);
                    }
                ptr = s;
                while (*ptr) {
                    if (wild_match_ci(ptr, t)) {
#ifdef DEBUG
                        printf("return(2)\n");
#endif
                        return(!invert_match);
                        }
                    ptr++;
                    }
                ptr = s;
                while ((ptr=strchr_ci(ptr, *t))) {
                    if (wild_match_ci(ptr+1, t+1)) {
#ifdef DEBUG
                        printf("return(3)\n");
#endif
                        return(!invert_match);
                        }
                    if (*++ptr)
                        ++ptr;
                    }
                return(invert_match);
            case MATCH_ONE:
                s++;
                t++;
                len--;
                break;
            case MATCH_SET1:
                ptr = NULL;
                if (!*(t+1) || !(ptr=strchr_ci(t+1, MATCH_SET2))) {
                    if (tolower(*t) != tolower(*s))
                        return(invert_match);
                    t++;
                    s++;
                    len--;
                    }
                else {
                    *ptr++ = 0;
                    t++;
                    SWAP_PTR(ptr, t);
                    invert_set_match = 0;
                    if (*ptr==SET_MATCH_INVERT && strlen(ptr)!=1) {
                        invert_set_match = 1;
                        ptr++;
                        }
                    if (strchr_ci(ptr, *s)) {
                        *(t-1) = MATCH_SET2;
                        if (invert_set_match)
                            return(invert_match);
                        s++;
                        len--;
                        }
                    else {
                        *(t-1) = MATCH_SET2;
                        if (!invert_set_match)
                            return(invert_match);
                        s++;
                        len--;
                        }
                    }
                break;
            default:
                if (tolower(*s) != tolower(*t))
                    return(invert_match);
                t++;
                s++;
                len--;
                break;
            }
        } while (*s && *t);
#ifdef DEBUG
    printf("s = %s,  t = %s\n", s, t);
#endif
    if (!*s && !*t) {
#ifdef DEBUG
        printf("return(5)\n");
#endif
        return(!invert_match);
        }
    if (!*s && *t==MATCH_MANY && !*(t+1)) {
#ifdef DEBUG
        printf("return(4)\n");
#endif
        return(!invert_match);
        }
    if (*s && !*t) {
#ifdef DEBUG
        printf("return(6)\n");
#endif
        return(invert_match);
        }
    if (!*s && *t) {
        while (*t) {
            if (*t!=MATCH_MANY) {
#ifdef DEBUG
                printf("return(7)\n");
#endif
                return(invert_match);
                }
            t++;
            }
#ifdef DEBUG
        printf("return(8)\n");
#endif
        return !invert_match;
        }
    bomb("the impossible has happened (wild_match_ci)", NULL);
    exit(1);
    }

int strcmp_ci(const char *s, const char *t)
{
    char sc, tc;
    while (*s && *t) {
        if ((sc=tolower(*s))<(tc=tolower(*t)))
            return -1;
        if (sc>tc)
            return 1;
        s++;
        t++;
        }
    return 0;
    }

char *strchr_ci(char *s, char c)
{
    c = tolower(c);
    while (*s) {
        if (tolower(*s)==c)
            return s;
        s++;
        }
    return NULL;
    }

/* routine: expand_ranges()
 * purpose: expand range specifiers (e.g., [a-e0-5]) into explicit lists (e.g., [abcde012345])
 *          should be called before calling wild_match()
 *
 * Michael Borland, 1991
 */

char *expand_ranges(char *template)
{
    char *new, *ptr, *ptr1, *ptr2, *end_new;
    int n_in_range;

    end_new = new = tmalloc(sizeof(*new)*(strlen(template)+1));
    *new = 0;
    ptr = tmalloc(sizeof(*new)*(strlen(template)+1));
    strcpy(ptr, template);
    while (*ptr) {
        if (*ptr==ESCAPE_CHAR) {
            *end_new++ = *ptr++;
            if (*ptr)
                *end_new++ = *ptr++;
            *end_new = 0;
            }
        else if (*ptr==MATCH_SET1) {
            *end_new++ = *ptr++;
            if ((ptr1=strchr(ptr, MATCH_SET2))) {
                *ptr1 = 0;
                ptr2 = ptr;
                while (*ptr2) {
                    *end_new++ = *ptr2++;
                    *end_new = 0;
                    if (*ptr2=='-') {
                        if (*(ptr2-1)==ESCAPE_CHAR) {
                            *(end_new-1) = '-';
                            *end_new = 0;
                            ptr2++;
                            }
                        else {
                            ptr2++;
                            n_in_range = (*ptr2)-(*(ptr2-2));
                            if (n_in_range<=0) {
                                fprintf(stderr, "error: bad range syntax: %s\n", ptr-2);
                                exit(1);
                                }
                            new = trealloc(new, sizeof(*new)*(strlen(new)+n_in_range+strlen(ptr1+1)+2));
                            end_new = new + strlen(new);
                            while (n_in_range--)
                                *end_new++ = *ptr2 - n_in_range;
                            *end_new = 0;
                            ptr2++;
                            }
                        }
                    }
                *end_new++ = *ptr1 = MATCH_SET2;
                *end_new = 0;
                ptr = ptr1+1;
                }
            else {
                *end_new++ = *(ptr-1);
                *end_new = 0;
                }
            }
        else {
            *end_new++ = *ptr++;
            *end_new = 0;
          }
        }
    *end_new = 0;
    return(new);
    }

/* routine: has_wildcards()
 * purpose: determine whether a string has wildcard characters
 *
 * Michael Borland, 1991
 */

int has_wildcards(char *template)
{
    char *ptr;

    ptr = template;
    while ((ptr=strchr(ptr, MATCH_MANY))) {
        if (ptr==template || *(ptr-1)!=ESCAPE_CHAR)
            return(1);
        ptr++;
        }

    ptr = template;
    while ((ptr=strchr(ptr, MATCH_ONE))) {
        if (ptr==template || *(ptr-1)!=ESCAPE_CHAR)
            return(1);
        ptr++;
        }

    ptr = template;
    while ((ptr=strchr(ptr, MATCH_SET1))) {
        if (ptr==template || *(ptr-1)!=ESCAPE_CHAR)
            return(1);
        ptr++;
        }

    return(0);
    }

char *unescape_wildcards(char *template)
{
    char *ptr;

    ptr = template;
    while ((ptr=strchr(ptr, MATCH_MANY))) {
        if (ptr!=template && *(ptr-1)==ESCAPE_CHAR) {
            strcpy_ss(ptr-1, ptr);
            }
        ptr++;
        }
    ptr = template;
    while ((ptr=strchr(ptr, MATCH_ONE))) {
        if (ptr!=template && *(ptr-1)==ESCAPE_CHAR) {
            strcpy_ss(ptr-1, ptr);
            }
        ptr++;
        }
    ptr = template;
    while ((ptr=strchr(ptr, MATCH_SET1))) {
        if (ptr!=template && *(ptr-1)==ESCAPE_CHAR) {
            strcpy_ss(ptr-1, ptr);
            }
        ptr++;
        }
    ptr = template;
    while ((ptr=strchr(ptr, MATCH_SET2))) {
        if (ptr!=template && *(ptr-1)==ESCAPE_CHAR) {
            strcpy_ss(ptr-1, ptr);
            }
        ptr++;
        }
    return(template);
    }

/* strcmp_nh is similar to strcmp but the
   character ranking is different. 
   The non-numeric characters are first
   [0-9] are last.
   It also puts single numberic characters
   before double numberic characters and so on. */
int strcmp_nh(const char *s1, const char *s2) {
  int n1, n2, n3, n4;
  int i;
  for ( ; *s1 && *s2; s1++,s2++) {
    if (((*s1 >= '0') && (*s1 <= '9')))
      n1 = 1;
    else
      n1 = 0;
    if (((*s2 >= '0') && (*s2 <= '9')))
      n2 = 1;
    else
      n2 = 0;
    if ((n1 == 1) && (n2 == 1)) {
      i=1;
      while ((*(s1 + i)) && (*(s2 + i))) {
        if (((*(s1 + i) >= '0') && (*(s1 + i) <= '9')))
          n3 = 1;
        else
          n3 = 0;
        if (((*(s2 + i) >= '0') && (*(s2 + i) <= '9')))
          n4 = 1;
        else
          n4 = 0;
        if ((n3 == 1) && (n4 == 0))
          return(1);
        else if ((n3 == 0) && (n4 == 1))
          return(-1);
        else if ((n3 == 0) && (n4 == 0))
          break;
        i++;
      }
      if (((*(s1 + i)) == NULL) && (*(s2 + i))) {
        if (((*(s2 + i) >= '0') && (*(s2 + i) <= '9')))
          return(-1);
      } else if (((*(s2 + i)) == NULL) && (*(s1 + i))) {
        if (((*(s1 + i) >= '0') && (*(s1 + i) <= '9')))
          return(1);
      }
    }

    if (n1 == n2) {
      if (*s1 < *s2)
        return(-1);
      else if (*s1 > *s2)
        return(1);
    } else if (n1 == 0) {
      return(-1);
    } else {
      return(1);
    }
  }
  if (*s1)
    return(1);
  else if (*s2)
    return(-1);
  return(0);
}
