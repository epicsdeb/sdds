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
 Revision 1.15  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.14  2003/11/04 22:22:03  shang
 freed the memory allocated for local variable editSeq in edit_string() function

 Revision 1.13  2003/06/23 21:33:32  borland
 Added 'x' command, which is another 'kill' command.
 Added 'c' command, to clear the kill buffer.
 No set flag after each command to clear the kill buffer automatically before
 the next kill command; this will change the behavior of commands with multiple
 kills.

 Revision 1.12  2003/06/18 19:52:31  borlandBinxin
 Removed the query() and query_e() macros, and replaced with queryn()
 and queryn_e() macros.  The former used gets() whereas the latter do
 not.

 Revision 1.11  2002/08/14 16:06:07  soliday
 Added Open License

 Revision 1.10  2001/07/10 19:39:25  borland
 replace_string() and replace_stringn() now use a common routine, replaceString(),
 which has additional functionality (replacement "here only").  edit_string
 uses this new replaceString() to implement %h replacement command (replace
 a string, but only if it is at the CP).

 Revision 1.9  2001/07/09 22:54:32  borland
 Substantial revision to implement command sequences that can be repeated.

 Revision 1.8  2000/08/30 20:53:42  soliday
 Increased edit_string buffer size from 1024 to 4096.

 Revision 1.7  1999/09/14 18:11:24  soliday
 replace_stringn is defined in a header file instead of locally

 Revision 1.6  1999/07/08 20:35:16  soliday
 Added ctype.h in include statement

 Revision 1.5  1999/05/25 21:52:22  soliday
 Removed compiler warnings under Linux.

 Revision 1.4  1996/02/12 17:04:30  borland
 Changed erroneous free of edit buffer in edit_strings to free of source
 string.

 * Revision 1.3  1995/11/13  16:18:48  borland
 * Added long cast to strlen() in comparisons to satisfy Solaris compiler.
 *
 * Revision 1.2  1995/09/05  21:17:19  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: edit_string.c
 * purpose: simple single line text-driven editor to allow user-directed
 *          data editing
 *
 * Michael Borland, 1994.
 */
#include "mdb.h"
#include <ctype.h>
long edit_string(char *text, char *edit);

#if defined(TEST_EDIT_STRING)
main()
{
    char s[200], t[200];

    do {
        queryn("text string: ", t, 200);
        queryn("edit string: ", s, 200);
        edit_string(t, s);
        printf(">%s<\n", t);
        } while (1);
    }
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define IS_WORD_END(c) ((c)==' ' || (c)=='\t' || (c)=='_' || (c)=='-')

/* edit commands:
 *      <n>(<seq>)   perform command sequence <seq>, <n> times
 *      <n>d   delete <n> characters
 *      <n>f   forward <n> characters 
 *      <n>b   backward <n> characters
 *      <n>D   delete <n> words
 *      <n>F   forward <n> words
 *      <n>B   backward <n> words
 *         a   go to beginning of line
 *         e   go to end of line
 *      <n>i{delim}text{delim}   insert text <n> times
 *         s{delim}text{delim}   search for text, CP to end
 *         S{delim}text{delim}   search for text, CP to start
 *        s?{delim}text{delim}   search for text, CP to end, end edit if absent
 *        S?{delim}text{delim}   search for text, CP to start, end edit if absent
 *      <n>k   kill <n> characters
 *      <n>K   kill <n> words
 *      <n>z<c> kill up to <c>, <n> times
 *      <n>Z<c> kill up to and including <c>, <n> times
 * x[-]{delim}text{delim}  kill characters as long as they are [not] in text.
 *         c    clear the kill buffer
 *      <n>y    yank kill buffer
 *      <n>%[gh]{delim}text1{delim}text2{delim}  replace text1 with text2 <n> times
 *                                               g=global, h=here (at CP) only
 */

typedef struct {
  char *editPtr, *editText;
  long count, pending;
} EDIT_SEQUENCE;

long edit_string(char *text, char *edit0)
{
  short count, new_kill;
  char *ptr, *text_start, delimiter, *ptr1, *ptr2;
  char *delimLoc, *editNext, *charList;
  static char kill[4096], buffer[4096];
  char *orig, *repl; 
  int global, conditional, conditionalReturn, here;
  char *edit, cSave;
  EDIT_SEQUENCE *editSeq;
  int stackLevel, stackDepth, doPush, parenCount, invert;
  
#if DEBUG
  printf("text = %s\nedit = %s\n", text, edit0);
#endif
  text_start = text;
  kill[0] =0;
  if (!(editSeq = malloc(sizeof(*editSeq)*(stackDepth=10))))
    return 0;
  cp_str(&editSeq[0].editText, edit0);
  editSeq[0].count = 1;
  editSeq[0].pending = 0;
  editSeq[0].editPtr = NULL;
  stackLevel = 0;

  new_kill = 1;
  while (stackLevel>=0) {
#if DEBUG
    printf("stackLevel = %ld\n", stackLevel);
#endif
    while (editSeq[stackLevel].pending || editSeq[stackLevel].count--) {
      if (!editSeq[stackLevel].pending)
        editSeq[stackLevel].editPtr = editSeq[stackLevel].editText;
      editSeq[stackLevel].pending = 0;
      if (!editSeq[stackLevel].editPtr)
        continue;
#if DEBUG
      printf("Count = %ld\n", editSeq[stackLevel].count+1);
      printf("EditPtr = %s\n", editSeq[stackLevel].editPtr);
#endif
      edit = editSeq[stackLevel].editPtr;
      while (*(editSeq[stackLevel].editPtr=edit)) {
        count = 0;
        while (isdigit(*edit))
          count = count*10 + (*edit++ -'0');
        if (!count)
          count = 1;
        if (!*edit)
          break;
#if DEBUG
        printf("count = %ld  command = %c\n", count, *edit);
#endif
        doPush = 0;
        /* this was commented out to avoid reseting the kill buffer before each command except yanks
          new_kill = 1; 
          */
        switch (*edit) {
        case '(':
          /* expression */
          parenCount = 1;
          ptr = edit+1;
          while (parenCount && *++edit) {
            if (*edit=='(') 
              parenCount++;
            else if (*edit==')')
              parenCount--;
          }
          if (*edit) {
            editSeq[stackLevel].pending = 1;
            editSeq[stackLevel].editPtr = edit+1;
          }
          else {
            editSeq[stackLevel].pending = 1;
            editSeq[stackLevel].editPtr = NULL;
          }
          if (stackLevel>=stackDepth && 
              !(editSeq = realloc(editSeq, sizeof(*editSeq)*(stackDepth+=10)))) {
            fprintf(stderr, "memory allocation failure (edit_string)");
            return 0;
          }
          stackLevel ++;
          editSeq[stackLevel].count = count;
          cSave = *edit;
          *edit = 0;
          cp_str(&editSeq[stackLevel].editText, ptr);
          *edit = cSave;
          editSeq[stackLevel].editPtr = NULL;
          doPush = 1;
#if DEBUG
          printf("stack pushing: %ld*>%s<\n", count, 
                 editSeq[stackLevel].editText);
#endif
          break;
        case 'c':
          /* clear the kill buffer */
          kill[0] = 0;
          break;
        case 'd':
          /* delete N characters */
          if (count>(long)strlen(text))
            count = strlen(text);
          strcpy_ss(text, text+count);
          new_kill = 1;
          break;
        case 'f':
          /* move forward N chars */
          if (count>(long)strlen(text))
            count = strlen(text);
          text += count;
          new_kill = 1;
          break;
        case 'b':
          /* move backward N chars */
          if ((text -= count)<text_start)
            text = text_start;
          new_kill = 1;
          break;
        case 'D':
          /* delete N words */
          while (count-- && (ptr=strpbrk(text, " \t_-"))) {
            while (IS_WORD_END(*ptr))
              ptr ++;
            strcpy_ss(text, ptr);
          }
          if (count>=0)
            *text = 0;
          new_kill = 1;
          break;
        case 'F':
          /* move forward N words */
          while (count-- && (ptr=strpbrk(text, " \t_-"))) {
            while (IS_WORD_END(*ptr))
              ptr ++;
            text = ptr;
          }
          if (count>=0)
            text += strlen(text);
          new_kill = 1;
          break;
        case 'B':
          /* move backward N words */
          while (count--) {
            while (IS_WORD_END(*text) && text>text_start)
              text--;
            while (!IS_WORD_END(*text) && text>text_start)
              text--;
          }
          if (IS_WORD_END(*text))
            text++;
          new_kill = 1;
          break;
        case 'a':
          /* go to start of line */
          text = text_start;
          break;
        case 'e':
          /* go to end of line */
          text += strlen(text);
          break;
        case 'i':
          /* insert characters */
          delimiter = *++edit;
          ptr1 = NULL;
          if ((ptr=strchr(++edit, delimiter))) {
            ptr1 = ptr;
            *ptr = 0;
          }
          else {
            ptr = edit+strlen(edit)-1;
          }
#if DEBUG
          printf("insert string = >%s<\n", edit);
#endif
          while (count--) {
            insert(text, edit);
            text += strlen(edit);
          }
          if (ptr1)
            *ptr1 = delimiter;
          edit = ptr;
          new_kill = 1;
          break;
        case 'x':
          /* kill anything in the given string */
          delimiter = *++edit;
          if (!*edit)
            break;
          invert = 0;
          if (delimiter=='-') {
            invert = 1;
            delimiter = *++edit;
            if (!*edit)
              break;
          }
          delimLoc = NULL;
          if ((editNext=strchr(++edit, delimiter))) {
            delimLoc = editNext;
            *editNext = 0;
          }
          else {
            editNext = edit+strlen(edit)-1;
          }
          charList = expand_ranges(edit);
          if (*charList=='[' && *(charList+strlen(charList)-1)==']') {
/*
            strcpy_ss(charList, charList+1);
            *(charList+strlen(charList)-1) = 0;
*/
          }
#if DEBUG
          printf("x-kill string = >%s<  (was >%s<)\n", charList, edit);
          printf("text position: >%s<\n", text);
#endif
          if (new_kill)
            kill[0] = 0;
          new_kill = 0;
          if (invert) {
            /* kill anything up to a character in the string */
            if ((ptr=strpbrk(text, charList))) {
              strncat(kill, text, ptr-text);
              strcpy_ss(text, ptr);
            } else
              /* no occurrence, kill whole string */
              *text = 0;
          } else {
            /* kill anything in the given string */
            int i, length, found;
            found = 1;
            length = strlen(charList);
            ptr = text;
            while (found && *ptr) {
              found = 0;
              for (i=0; i<length; i++) {
                if (*ptr==*(charList+i)) {
                  found = 1;
                  ptr ++;
                  break;
                }
              }
            }
            if (ptr!=text) {
              strncat(kill, text, ptr-text);
              strcpy_ss(text, ptr);
            }
          }
          free(charList);
          if (delimLoc)
            *delimLoc = delimiter;
          edit = editNext;
          break;
        case 's': 
          /* search forward for characters, leaving CP at end of matching section */
          if (!(delimiter = *++edit))
            return(0);
          conditional = 0;
          if (delimiter=='?') {
            conditional = 1;
            if (!(delimiter = *++edit))
              return(0);
          }
          ptr1 = NULL;
          if ((ptr=strchr(++edit, delimiter))) {
            ptr1 = ptr;
            *ptr = 0;
          }
          else {
            ptr = edit+strlen(edit)-1;
          }
#if DEBUG
          printf("search string = >%s<\n", edit);
#endif
          conditionalReturn = 0;
          while (count--) {
            if ((ptr2=strstr(text, edit))) 
              text = ptr2+strlen(edit);
            else {
              conditionalReturn = conditional;
              break;
            }
          }
          if (ptr1)
            *ptr1 = delimiter;
          edit = ptr;
          new_kill = 1;
          if (conditionalReturn)
            return 1;
          break;
        case 'S':
          /* search forward for characters, but leave cursor at start of matching section */
          if (!(delimiter = *++edit))
            return(0);
          conditional = 0;
          if (delimiter=='?') {
            conditional = 1;
            if (!(delimiter = *++edit))
              return(0);
          }
          ptr1 = NULL;
          if ((ptr=strchr(++edit, delimiter))) {
            ptr1 = ptr;
            *ptr = 0;
          }
          else {
            ptr = edit+strlen(edit)-1;
          }
#if DEBUG
          printf("search string = >%s<\n", edit);
#endif
          conditionalReturn = 0;
          while (count--) {
            if ((ptr2=strstr(text, edit))) {
              if (count!=0)
                text = ptr2+strlen(edit);
              else
                text = ptr2;
            }
            else {
              conditionalReturn = conditional;
              break;
            }
          }
          if (ptr1)
            *ptr1 = delimiter;
          edit = ptr;
          new_kill = 1;
          if (conditionalReturn)
            return 1;
          break;
        case 'k':
          /* kill N characters */
          if (new_kill)
            kill[0] = 0;
          strncat(kill, text, count);
          strcpy_ss(text, text+count);
          new_kill = 0;
#if DEBUG
          printf("kill buffer: >%s<\n", kill);
#endif
          break;
        case 'K':
          /* kill N words */
          if (new_kill)
            kill[0] = 0;
          while (count-- && (ptr=strpbrk(text, " \t_-"))) {
            while (IS_WORD_END(*ptr))
              ptr ++;
            strncat(kill, text, ptr-text);
            strcpy_ss(text, ptr);
          }
          if (count>=0)
            *text = 0;
          new_kill = 0;
#if DEBUG
          printf("kill buffer: >%s<\n", kill);
#endif
          break;
        case 'z': case 'Z':
          if (!(delimiter = *++edit))
            return(0);
          while (count--) {
            /* kill up to next character */
            if (new_kill)
              kill[0] = 0;
            if ((ptr=strchr(text, delimiter))) {
              if (*(edit-1)=='Z')
                ptr++;  /* delete delimiter */
              strncat(kill, text, ptr-text);
              strcpy_ss(text, ptr);
            }
            new_kill = 0;
#if DEBUG
            printf("kill buffer: >%s<\n", kill);
#endif
            if (*(edit-1)=='z')
              break;
          }
          break;
        case 'y':
          /* yank kill buffer */
#if DEBUG
          printf("yank string = >%s<\n", kill);
#endif
          while (count--) {
            insert(text, kill);
            text += strlen(kill);
          }
          break;
        case '%':
          /* replace one string with another */
          global = here = 0;
          if (*(edit+1)=='g') {
            global = 1;
            edit++;
          } 
          if (*(edit+1)=='h') {
            here = 1;
            edit++;
          }
          if (!(delimiter = *++edit))
            return(0);
#if DEBUG
          printf("delimiter = %c\n", delimiter);
#endif
          orig = edit+1;
          if (!(repl=strchr(orig+1, delimiter)))
            return(0);
          *repl++ = 0;
          if (!(ptr=strchr(repl, delimiter)))
            return(0);
          *ptr = 0;
#if DEBUG
          printf("orig: >%s<   repl: >%s<\n", orig, repl);
#endif
          if (here) {
            if (!global)
              replaceString(buffer, text, orig, repl, count, 1);
            else
              replaceString(buffer, text, orig, repl, -1, 1);
          }         
          else {
            if (!global)
              replace_stringn(buffer, text, orig, repl, count);
            else
              replace_string(buffer, text, orig, repl);
          }
          strcpy_ss(text, buffer);
          edit = ptr;
          *(repl-1) = delimiter;
          *ptr = delimiter;
          break;
        default:
          break;
        }
        if (doPush)
          break;
        edit++;
      }
    }
    free(editSeq[stackLevel].editText);
    editSeq[stackLevel].editPtr = NULL;
    stackLevel --;
  }
  free(editSeq);
  return 1;
}

void edit_strings(char **string, long strings, char *buffer, char *edit)
{
  while (strings--) {
    strcpy_ss(buffer, string[strings]);
    edit_string(buffer, edit);
    free(string[strings]);
    cp_str(string+strings, buffer);
  }
}

