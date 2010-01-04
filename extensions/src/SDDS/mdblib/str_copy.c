/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/


#include <string.h>
#include "mdb.h"

/* routine: strcpy_ss()
 * purpose: copy string safely in the case of memory overlapping
 *
 * Yusong Wang, 2006.
 */

char *strcpy_ss(char *dest, const char *src)
{
  return (char*) memmove(dest, src, strlen(src)+1);
}

