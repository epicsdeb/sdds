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
 Revision 1.3  1998/07/16 16:02:07  borland
 Fixed bug that occured when a string ended in a double-quote.

 * Revision 1.2  1995/09/05  21:17:42  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* routine: interpret_escapes()
 * purpose: interpret C escape sequences
 * 
 * \n = LF
 * \t = TAB
 * \ddd = octal specification for character 
 *
 * Michael Borland, 1988
 */
#include "mdb.h"

void interpret_escapes(char *s)
{
    char *ptr;
    long count;

    ptr = s;
    while (*s) {
        if (*s=='"') {
            do {
                *ptr++ = *s++;
                } while (*s!='"' && *s);
            if (*s)
              *ptr++ = *s++;
            }
        else if (*s!='\\')
            *ptr++ = *s++;
        else {
            s++;
            if (!*s) {
                *ptr++ = '\\';
                *ptr++ = 0;
                return;
                }
            switch (*s) {
                case '\\':
                    *ptr++ = '\\';
                    s++;
                    break;
                case 'n':
                    *ptr++ = '\n';
                    s++;
                    break;
                case 't':
                    *ptr++ = '\t';
                    s++;
                    break;
                default :
                    if (*s>='0' && *s<='9') {
                        *ptr = 0;
                        count = 0;
                        while (++count<=3 && *s>='0' && *s<='9') 
                            *ptr = 8*(*ptr) + *s++ - '0';
                        ptr++;
                        }
                    else {
                        *ptr++ = '\\';
                        }
                    break;
                }
            }
        }
    *ptr = 0;
    }
