/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: getpri
 * purpose: get process priority
 *
 * Michael Borland,1989
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:17:35  saunders
 First test release of the SDDS1.5 package.

 */
#include stdlib
#include <ssdef.h>
#include <jpidef.h>

typedef struct {
    unsigned long code_and_length;
    long *buffer_address;
    long *return_length_address;
    } ITEM_LIST;

long getpri(pid, base)
long pid, base;
{
    static ITEM_LIST item;
    static long buffer, return_length, status;
    
    if (base) {
        /* set up to request the base priority */
        item.code_and_length = 4 + (JPI$_PRIB<<16);
        }
    else {
        /* set up to request the base priority */
        item.code_and_length = 4 + (JPI$_PRI<<16);
        }        
    item.buffer_address = &buffer;
    item.return_length_address = &return_length;

    /* get the priority */
    if ((status = sys$getjpiw(0, &pid, 0, &item, 0, 0, 0))!=SS$_NORMAL) {
        lib$signal(status, 0, 0);
        return(-1);
        }

    return(buffer);
    }
