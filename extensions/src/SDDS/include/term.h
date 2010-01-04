/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: term.h
 * 	 definitions for use with getti() and setti() terminal
 *	 mode characteristic getting and setting functions 
 *
 * Michael Borland, 1987
 $Log: term.h,v $
 Revision 1.4  2003/06/15 19:41:03  soliday
 Removed include lines.

 Revision 1.3  2002/08/14 15:40:18  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:15:45  saunders
 First test release of the SDDS1.5 package.

 */
/*
#include ttdef
#include tt2def
*/
typedef struct {
    short type;
    short class;
    short page_width;
    short page_length;
    long  term_char;
    long  ext_term_char;
    } TERM_MODE;

