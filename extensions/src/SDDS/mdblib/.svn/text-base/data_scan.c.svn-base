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
 Revision 1.7  2007/07/13 15:02:43  soliday
 Renamed strcpy_s to strcpy_ss because Microsoft is already using this name.

 Revision 1.6  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.5  2002/08/14 16:06:06  soliday
 Added Open License

 Revision 1.4  2000/02/22 21:52:00  borland
 get_token now handles escaped quotation marks.

 Revision 1.3  1996/08/26 20:09:23  borland
 Inserted code for tokenIsNumber() and tokenIsInteger() (formerly in SDDSaps.c
 in ../SDDSaps directory).

 * Revision 1.2  1995/09/05  21:17:07  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: data_scan.c
 * contents: get_double(), get_float(), get_long(), get_token(), get_token_buf()
 * purpose: to allow easy scanning of data in free format
 *
 * Michael Borland, 1987
 */
#include "mdb.h"
#include <ctype.h>

#define float_start(m_c) (isdigit(*(m_c)) || *(m_c)=='.' || ((*(m_c)=='-' || *(m_c)=='+') && (isdigit(*((m_c)+1)) || *((m_c)+1)=='.')))
#define int_start(m_c) (isdigit(*(m_c)) || ((*(m_c)=='-' || *(m_c)=='+') && isdigit(*((m_c)+1))))

#define skip_it(m_c) (isspace(m_c) || (m_c)==',' || (m_c)==';')
#define nskip_it(m_c) (!isspace(m_c) && (m_c)!=',' && (m_c)!=';')

int get_double(double *dptr, char *s)
{
  register char *ptr0;
  register int was_point=0;

  /* skip leading non-number characters */  
  ptr0 = s;
  while (!float_start(s) && *s)
    s++;
  if (*s==0) return(0);

  /* scan the number */
  sscanf(s, "%lf", dptr);
  
  /* skip to first white-space following number */

  /* skip sign, if any */
  if (*s=='-' || *s=='+')
    s++;

  /* skip mantissa */
  while (isdigit(*s) || (*s=='.' && !was_point)) 
    if (*s++=='.')
      was_point = 1;

  if (*s=='e' || *s=='E') {	/* skip exponent, if any */
    s++;
    if (*s=='+' || *s=='-')
      s++;
    while (isdigit(*s))
      s++;
    }

  strcpy_ss(ptr0, s);
  return(1);
  }

int get_float(float *fptr, char *s)
{
  register char *ptr0;
  register int was_point=0;

  /* skip leading non-number characters */  
  ptr0 = s;
  while (!float_start(s) && *s)
    s++;
  if (*s==0) return(0);

  /* scan the number */
  sscanf(s, "%f", fptr);
  
  /* skip to first white-space following number */

  /* skip sign, if any */
  if (*s=='-' || *s=='+')
    s++;

  /* skip mantissa */
  while (isdigit(*s) || (*s=='.' && !was_point)) 
    if (*s++=='.')
      was_point = 1;

  if (*s=='e' || *s=='E') {	/* skip exponent, if any */
    s++;
    if (*s=='+' || *s=='-')
      s++;
    while (isdigit(*s))
      s++;
    }

  strcpy_ss(ptr0, s);
  return(1);
  }

int get_long(long *iptr, char *s)
{
  char *ptr0;
  
  /* skip leading white-space and commas */  
  ptr0 = s;
  while (!int_start(s) && *s)
    s++;
  if (*s==0) return(0);

  /* scan the number */
  sscanf(s, "%ld", iptr);
  
  /* skip to first white-space following number */
  if (*s=='-' || *s=='+') 
    s++;
  while (isdigit(*s))
    s++;

  strcpy_ss(ptr0, s);
  return(1);
  }

/*s string do not change after integer is scanned.*/
int get_long1(long *iptr, char *s)
{
 
  /* skip leading white-space and commas */
  while (!int_start(s) && *s)
    s++;
  if (*s==0) return(0);
  
  /* scan the number */
  sscanf(s, "%ld", iptr);
  
  /* skip to first white-space following number */
  if (*s=='-' || *s=='+') 
    s++;
  while (isdigit(*s))
    s++;
  return(1);
}


int get_short(short *iptr, char *s)
{
  char *ptr0;
  
  /* skip leading white-space and commas */  
  ptr0 = s;
  while (!int_start(s) && *s)
    s++;
  if (*s==0) return(0);

  /* scan the number */
  sscanf(s, "%hd", iptr);
  
  /* skip to first white-space following number */
  if (*s=='-' || *s=='+') 
    s++;
  while (isdigit(*s))
    s++;

  strcpy_ss(ptr0, s);
  return(1);
  }

int get_int(int *iptr, char *s)
{
  char *ptr0;
  
  /* skip leading white-space and commas */  
  ptr0 = s;
  while (!int_start(s) && *s)
    s++;
  if (*s==0) return(0);

  /* scan the number */
  sscanf(s, "%d", iptr);
  
  /* skip to first white-space following number */
  if (*s=='-' || *s=='+') 
    s++;
  while (isdigit(*s))
    s++;

  strcpy_ss(ptr0, s);
  return(1);
  }


char *get_token(char *s)
{
  char *ptr0, *ptr1, *ptr;
  
  /* skip leading white-space and commas */  
  ptr0 = s;
  while (skip_it(*s))
    s++;
  if (*s==0) 
    return(NULL);
  ptr1 = s;

  if (*s=='"' && (ptr0==s || *(s-1)!='\\')) {
    ptr1 = s+1;
    /* quoted string, so skip to next quotation mark */
    do {
      s++;
      } while (*s && (*s!='"' || *(s-1)=='\\'));
    if (*s=='"' && *(s-1)!='\\')
        *s = ' ';
    }
  else {
    /* skip to first white-space following token */
    do {
      s++;
      if (*s=='"' && *(s-1)!='\\') {
          while (*++s && (*s!='"' || *(s-1)=='\\'))
              ;
          }
      } while (*s && nskip_it(*s));
    }

  ptr = tmalloc((unsigned)sizeof(*ptr)*(s-ptr1+1));
  strncpy(ptr, ptr1, s-ptr1);
  ptr[s-ptr1]=0;
 
  strcpy_ss(ptr0, s);
  return(ptr);
  }

char *get_token_buf(char *s, char *buf, long lbuf)
{
  char *ptr0, *ptr1;
  
  /* skip leading white-space and commas */  
  ptr0 = s;
  while (skip_it(*s))
    s++;
  if (*s==0) 
    return(NULL);
  ptr1 = s;

  if (*s=='"') {
    ptr1 = s+1;
    /* if quoted string, skip to next quotation mark */
    do {
      s++;
      } while (*s!='"' && *s);
    if (*s=='"')
        *s = ' ';
    }
  else {
    /* skip to first white-space following token */
    do {
      s++;
      } while (*s && nskip_it(*s));
    }

  if ((s-ptr1+1)>lbuf) {
    printf("buffer overflow in get_token_buf()\nstring was %s\n", ptr0);
    exit(1);
    }
  strncpy(buf, ptr1, s-ptr1);
  buf[s-ptr1]=0;
 
  strcpy_ss(ptr0, s);
  return(buf);
  }

long tokenIsInteger(char *token)
{
  if (!isdigit(*token) && *token!='-' && *token!='+')
    return 0;
  if (!isdigit(*token) && !isdigit(*(token+1)))
    return 0;
  token++;
  while (*token)
    if (!isdigit(*token++))
      return 0;
  return 1;
}

long tokenIsNumber(char *token)
{
  long pointSeen, digitSeen;
  
  if (!(digitSeen=isdigit(*token)) && *token!='-' && *token!='+' && *token!='.')
    return 0;
  pointSeen = *token == '.';
  token++;

  while (*token) {
    if (isdigit(*token)) {
      digitSeen = 1;
      token++;
    }
    else if (*token=='.') {
      if (pointSeen)
        return 0;
      pointSeen = 1;
      token++;
    } else if (*token=='e' || *token=='E') {
      if (!digitSeen)
        return 0;
      return tokenIsInteger(token+1);
    } else
      return 0;
    }
  return digitSeen;
}


