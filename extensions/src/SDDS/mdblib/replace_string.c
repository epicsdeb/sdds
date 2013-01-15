/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: replace_string()
 * purpose: replace all occurances of one string with another string
 * return value: the number of replacements.
 * Michael Borland, 1991
 $Log: not supported by cvs2svn $
 Revision 1.6  2001/07/10 19:44:35  borland
 Removed debugging statement.

 Revision 1.5  2001/07/10 19:39:25  borland
 replace_string() and replace_stringn() now use a common routine, replaceString(),
 which has additional functionality (replacement "here only").  edit_string
 uses this new replaceString() to implement %h replacement command (replace
 a string, but only if it is at the CP).

 Revision 1.4  1999/05/25 21:53:57  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/09/06 14:11:02  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"

int replace_string(char *t, char *s, char *orig, char *repl)
{
  return replaceString(t, s, orig, repl, -1, 0);
}

int replace_stringn(char *t, char *s, char *orig, char *repl, long count_limit)
{
  return replaceString(t, s, orig, repl, count_limit, 0);
}

int replaceString(char *t, char *s, char *orig, char *repl, long count_limit, long here)
{
  char *ptr0, *ptr1;
  int count;
  char temp;

  ptr0 = s; 
  t[0] = 0;
  count = 0;
  while ((count_limit<0 || count<count_limit) && (ptr1=str_in(ptr0, orig)) ) {
    if (here && ptr1!=ptr0)
      break;
    count++;
    temp = *ptr1;
    *ptr1 = 0;
    strcat(t, ptr0);
    strcat(t, repl);
    ptr0 = ptr1+strlen(orig);
    *ptr1 = temp;
  }
  if (strlen(ptr0))
    strcat(t, ptr0);
  return(count);
}

