/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "mdb.h"
#include <descrip.h>

/*  routine:	make_string_dsc
 *  purpose:	fill a string descriptor structure for use with a
 *		system service routine.
 * Michael Borland, 1986
 $Log: not supported by cvs2svn $
 Revision 1.4  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.3  2002/08/14 16:06:10  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:17:52  saunders
 First test release of the SDDS1.5 package.

 */

make_string_dsc(dsc, s)
struct dsc$descriptor_s *dsc;
char *s;
{
    static long l;
    dsc->dsc$w_length=(l=strlen(s));
    dsc->dsc$a_pointer=tmalloc((unsigned)(l+1)*sizeof(*s));
    strcpy_ss(dsc->dsc$a_pointer, s);
    dsc->dsc$b_class=DSC$K_CLASS_S;
    dsc->dsc$b_dtype=DSC$K_DTYPE_T;
    }

