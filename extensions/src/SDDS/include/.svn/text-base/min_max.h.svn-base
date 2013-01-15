/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: min_max.h
 * contents: macros for finding minimum and maximum of numbers
 * 
 * Michael Borland, 1989
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:15:29  saunders
 First test release of the SDDS1.5 package.

 */

static double tmp$max1, tmp$max2;

#define MAX2(x, y) ((x)>(y)?(x):(y))
#define MIN2(x, y) ((x)<(y)?(x):(y))

#define MAX3(x, y, z) (MAX2(MAX2(x,y),z))
#define MIN3(x, y, z) (MIN2(MIN2(x,y),z))

#define MAX4(x, y, z, a) (MAX2(MAX2(x,y),MAX2(a,z)))
#define MIN4(x, y, z, a) (MIN2(MIN2(x,y),MIN2(a,z)))


