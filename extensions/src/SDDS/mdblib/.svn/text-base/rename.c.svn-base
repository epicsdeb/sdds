/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: rename()
 * purpose: to rename files
 * Michael Borland, 1986.
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:18:09  saunders
 First test release of the SDDS1.5 package.

 */

#include "mdb.h"
#include <descrip.h>

rename(old, new)
char *old, *new;
{
    struct dsc$descriptor_s old_dsc, new_dsc;
        
    make_string_dsc(&old_dsc, old);
    make_string_dsc(&new_dsc, new);
    return(lib$rename_file(&old_dsc, &new_dsc)-1);
    }
