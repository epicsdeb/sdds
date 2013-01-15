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
 Revision 1.5  2002/08/14 16:23:48  soliday
 Added Open License

 Revision 1.4  1999/06/01 19:13:02  borland
 Replaced CNL_CutOutComments with version from SDDS library.

 Revision 1.3  1999/05/28 14:53:33  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:21:33  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include "namelist.h"
#include <ctype.h>

char *CNL_fgetsSkipComments(char *s, long slen, FILE *fp, char skip_char);
void CNL_CutOutComments(char *s, char cc);

/* routine: get_namelist()
 * purpose: get a namelist string from a file 
 */

#define DOLLARSIGN_TOO 0

char *get_namelist(
    char *s,
    long n,
    FILE *fp
    )
{
    register char *flag, *ptr, *ptr1;
    register long l;

    while ((flag=CNL_fgetsSkipComments(s, n, fp, '!'))) {
        if ((ptr=strchr(s, '&'))) {
            if (!is_quoted(s, ptr, '"'))
                break;
            }
#if DOLLARSIGN_TOO
        if (ptr=strchr(s, '$')) {
            if (!is_quoted(s, ptr, '"'))
                break;
            }
#endif
        }
#ifdef DEBUG
        printf("get_namelist: s = <%s>\n", s);
#endif

    if (flag==NULL)
        return(NULL);

#if DOLLARSIGN_TOO
    if ((count_occurences(s, '$', "")+count_occurences(s, '&', ""))>=2)
        return(s);
#else
    if (count_occurences(s, '&', "")>=2)
        return(s);
#endif

    s[strlen(s)-1] = ' ';
/*    chop_nl(s); */
    ptr = s;
    do {
        l = strlen(ptr);
        ptr += l;
        if ((n-=l)<=1) {
            puts("error: namelist text buffer too small");
            abort();
            }
        do {
            if (!CNL_fgetsSkipComments(ptr, n, fp, '!'))
                return(s);
            if (ptr[0]!='!')
                break;
            } while (1);
        *(ptr+strlen(ptr)-1) = ' ';
/*        chop_nl(ptr); */
        if ((ptr1=strrchr(ptr, '&'))) {
            if (!is_quoted(s, ptr1, '"'))
                break;
            }
#if DOLLARSIGN_TOO
        if (ptr1=strrchr(ptr, '$')) {
            if (!is_quoted(s, ptr1, '"'))
                break;
            }
#endif
        } while (1) ;
    
    return(s);
    }          

char *CNL_fgetsSkipComments(char *s, long slen, FILE *fp,
    char skip_char    /* ignore lines that begin with this character */
    )
{
  if (!s)
    return NULL;
  s[0] = 0;
  while (fgets(s, slen-1, fp)) {
    if (s[0]!=skip_char) {
      CNL_CutOutComments(s, skip_char);
      return(s);
    }
  } 
  return(NULL);
}

void CNL_CutOutComments(char *s, char cc)
{
  long length, hasNewline;
  char *s0;
  
  hasNewline = 0;
  length = strlen(s);
  if (s[length-1]=='\n')
    hasNewline = 1;

  if (*s==cc) {
    *s = 0;
    return;
  }
  s0 = s;
  while (*s) {
    if (*s=='"') {
      while (*++s && *s!='"')
        ;
      if (!*s)
        return;
      s++;
      continue;
    }
    if (*s==cc) {
      if (s!=s0 && *(s-1)=='\\')
        strcpy_ss(s-1, s);
      else {
        if (hasNewline) {
          *s = '\n';
          *(s+1) = 0;
        }
        else
          *s = 0;
        return;
      }
    }
    s++;
  }
}


