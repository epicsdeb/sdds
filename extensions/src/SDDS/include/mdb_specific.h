/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file   : mdbsun.h
 * purpose: definitions for general use, for mdblib, and for mdbmth.
 *          These definitions are specific to the SUN.
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:15:16  saunders
 First test release of the SDDS1.5 package.

 */
#ifndef _MDBSUN_
#define _MDBSUN_ 1

/* escape sequences for VT100/200 series terminals */
#define CLEAR_SCREEN "\033[2J"
#define HOME_SCREEN  "\033[H"
#define HOME_CLEAR "\033[2J\033[H"

/* commonly needed terminal control characters */
#define CARRIAGE_RETURN '\015'
#define LINE_FEED '\012'
#define FORM_FEED '\014'
#define BACK_SPACE '\010'
#define ESCAPE '\033'

/************************ mathematical stuff *******************************/
#ifndef _MDBSUN_MTH_
#define _MDBSUN_MTH_ 1

/* largest float allowed */
#define FHUGE (1.70141173e38)
/* largest long allowed */
#define MAX_LONG (2147483647)
#define SQRT_HUGE (1.3043932E19)

#endif  /* _MDBSUN_MTH_ */

#endif  /* _MDBSUN_ */









