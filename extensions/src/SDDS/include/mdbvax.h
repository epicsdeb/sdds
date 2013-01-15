/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file   : mdbvax.h
 * purpose: definitions for general use, for mdblib, and for mdbmth.
 *          These definitions are specific to VAX/VMS.
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:15:23  saunders
 First test release of the SDDS1.5 package.

 */
#ifndef _MDBVAX_
#define _MDBVAX_ 1

/* structure for use with get_date_time() and convert_date_time() */
typedef struct {
  long day, month, year;
  long hrs, mins, secs;
  } DATE_TIME;

/* get_date_time() puts the current date and time in a DATE_TIME
 * structure given a pointer to such a structure */
#define get_date_time(gdt_arg) convert_date_time(gdt_arg, mtime())
#define NO_TIME 2

/**************************** mdblib routines *****************************/

/*   -- Time utilies : */
extern double sys_time(void);
extern char *asc_time(long binary_time[2], long conversion_flag);
extern long bin_time(long binary_time[2], double time_in_seconds);
extern long convert_date_time(DATE_TIME *date_time, char *mtime_string);

/*   -- Run-time statistics: */
extern long cpu_time(void);
extern long page_faults(void);
extern long dio_count(void);
extern long bio_count(void);
extern long init_stats(void);
extern char *elapsed_time();

/*   -- Logical name routines: */
extern long get_logical(char *logical_name, char *value, char *table_name);
extern long set_logical(char *logical_name, char *value, char *table_name);

/*   -- Miscellaneous routines: */
extern char *get_username(void);
extern long system(char *command);
extern long sys_assign(char *device_name, unsigned long *channel,
                      unsigned long access_mode, char *mail_box_name);

/********************* end of mdblib routines *****************************/

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
#ifndef _MDBVAX_MTH_
#define _MDBVAX_MTH_ 1

/* largest float allowed */
#define FHUGE (1.70141173e38)
/* largest long allowed */
#define MAX_LONG (2147483647)
#define SQRT_HUGE (1.3043932E19)
#define HUGE HUGE_VAL
#define HUGE_VAL_CONST HUGE

#endif  /* _MDBVAX_MTH_ */

#endif  /* _MDBVAX_ */

