/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: scan_item_list
 * M. Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.21  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.20  2006/10/19 17:55:41  soliday
 Updated to work with linux-x86_64.

 Revision 1.19  2006/09/05 02:44:53  soliday
 Updated to work on WIN32

 Revision 1.18  2006/08/31 15:06:25  soliday
 Updated to work with SDDS2

 Revision 1.17  2005/11/04 22:47:10  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2005/02/22 17:31:05  shang
 added scanItemListLong() to deal with unsigned long long type flags

 Revision 1.15  2004/03/16 23:27:15  borland
 Uses SCANITEMLIST_IGNORE_VALUELESS macro.  If given, means that a qualifier with
 no value attached will be ignored if a value is required.

 Revision 1.14  2002/08/14 16:06:14  soliday
 Added Open License

 Revision 1.13  2002/07/25 19:38:58  borland
 Replaced an occurrence of strncasecmp() with strncmp_case_insensitive().

 Revision 1.12  2002/07/24 16:03:37  borland
 Replaced strncasecmp() with strncmp_case_insensitive(), since the former
 does not exist on VxWorks.

 Revision 1.11  2000/04/13 16:14:43  soliday
 Removed WIN32 define statments that were moved to mdb.h

 Revision 1.10  2000/04/05 21:34:36  soliday
 Added Borland C definition for strncasecmp

 Revision 1.9  1999/12/21 17:53:56  soliday
 vxWorks does not have a case insensitive string compare

 Revision 1.8  1999/07/08 20:37:44  soliday
 WIN32 uses _strnicmp instead of strncasecmp

 Revision 1.7  1999/05/25 21:55:36  soliday
 Removed compiler warnings under Linux.

 Revision 1.6  1997/08/25 19:17:37  borland
 Fixed problem with va_start call (wrong variabled named for last
 variable in fixed arg list).

 Revision 1.5  1996/10/04 16:01:10  borland
 Better error handling for keywords with qualifiers supplied when the keyword
 doesn't take any qualifiers.

 * Revision 1.4  1996/05/29  21:45:52  borland
 * Added implementation of modes for scanItemList: allows unknown values
 * if set, removes used items if set.
 *
 * Revision 1.3  1996/02/14  01:01:51  borland
 * Added scanItemList() variant of scan_item_list() (which is obsolete now).
 *
 * Revision 1.2  1995/09/05  21:18:23  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include <stdarg.h>

long scanItemList(unsigned long *flags, char **item, long *items, unsigned long mode, ...)
/* usage: scanItemList(&flags, item, &items, mode,
   <keyword>, <SDDS-type>, <pointer>, <number-required>, <set-flag>, etc.
   NULL)
   * The mode argument is for future expansion.
   */
{
  va_list argptr;
  long i, j, type, flag, retval, length, number, match;
  char *keyword;
  void *data;
  static char **valueptr = NULL;
  static long *keylength = NULL;
  static short *item_matched = NULL, *has_equals = NULL;
  static long maxitems = 0;
  
  if (!flags)
    return bombre("null flags pointer seen (scanItemList)", NULL, 0);
  if (!item)
    return bombre("null item pointer seen (scanItemList)", NULL, 0);
  if (!items)
    return bombre("null items pointer seen (scanItemList)", NULL, 0);
  if (*items<=0)  {
    *flags = 0;
    return 1;
  }
  if (*items>maxitems) {
    valueptr = trealloc(valueptr, sizeof(*valueptr)*(maxitems= *items));
    keylength = trealloc(keylength, sizeof(*keylength)*maxitems);
    item_matched = trealloc(item_matched, sizeof(*item_matched)*maxitems);
    has_equals = trealloc(has_equals, sizeof(*has_equals)*maxitems);
  }
  *flags = 0;
  for (i=0; i<*items; i++) {
    item_matched[i] = 0;
#if DEBUG
    fprintf(stderr, "item %ld: %s\n", i, item[i]);
#endif
    if ((valueptr[i]=strchr(item[i], '='))) {
      if ((keylength[i] = valueptr[i]-item[i])<=0)
        return bombre("zero-length keyword seen (scanItemList)", NULL, 0);
      *valueptr[i]++ = 0;
      has_equals[i] = 1;
    }
    else {
      keylength[i] = strlen(item[i]);
      has_equals[i] = 0;
    }
    
#if DEBUG
    fprintf(stderr, "  keyword: %s   valueptr: %s\n", item[i], valueptr[i]);
#endif
  }

  va_start(argptr, mode);
  retval = 1;
  do {
    if (!(keyword = va_arg(argptr, char *)))
      break;
    type = va_arg(argptr, int32_t);
    data = va_arg(argptr, void *);
    number = va_arg(argptr, int32_t);
    flag = va_arg(argptr, uint32_t);
    length = strlen(keyword);
    match = -1;
    for (i=0; i<*items; i++) 
      if (strncmp_case_insensitive(item[i], keyword, MIN(length, keylength[i]))==0) {
        if (match!=-1) {
          fprintf(stderr, "ambiguous item %s seen\n", keyword);
          retval = 0;
        }
        match = i;
      }
#if defined(DEBUG)        
    if (match!=-1)
      fprintf(stderr, "keyword %s and item %ld (%s) match\n", keyword, match, item[match]);
    else
      fprintf(stderr, "no match for keyword %s\n", keyword);
#endif
    if (!retval)
      break;
    if (match==-1)
      continue;
    if (!has_equals[match] && number && mode&SCANITEMLIST_IGNORE_VALUELESS)
      continue;
    if (item_matched[match]) {
      fprintf(stderr, "error: ambiguous qualifier %s seen\n", item[match]);
      retval = 0;
      break;
    }
    item_matched[match] = 1;
    *flags |= flag;
    if (!valueptr[match]) {
      if (type==-1)
        continue;
      fprintf(stderr, "error: value not given for %s\n", keyword);
      retval = 0;
      break;
    }
    switch (type) {
    case SDDS_DOUBLE:
      *((double*)data) = atof(valueptr[match]);
      break;
    case SDDS_FLOAT:
      *((float*)data) = (float)atof(valueptr[match]);
      break;
    case SDDS_LONG:
      *((int32_t*)data) = atol(valueptr[match]);
      break;
    case SDDS_ULONG:
#if defined(_WIN32)
      *((uint32_t*)data) = (uint32_t)_atoi64(valueptr[match]);
#else
      *((uint32_t*)data) = (uint32_t)atoll(valueptr[match]);
#endif
      break;
    case SDDS_SHORT:
      *((short*)data) = (short)atol(valueptr[match]);
      break;
    case SDDS_USHORT:
      *((unsigned short*)data) = (unsigned short)atol(valueptr[match]);
      break;
    case SDDS_STRING:
      cp_str((char**)data, valueptr[match]);
      break;
    case SDDS_CHARACTER:
      *((char*)data) = valueptr[match][0];
      break;
    default:
      fprintf(stderr, "Error: value not accepted for qualifier %s\n",
              item[match]);
      break;
    }
  } while (retval==1);
  va_end(argptr);
  for (i=0; i<*items; i++) {
    if (!item_matched[i]) {
      if (!has_equals[i] && mode&SCANITEMLIST_UNKNOWN_VALUE_OK)
        continue;
      if (has_equals[i] && mode&SCANITEMLIST_UNKNOWN_KEYVALUE_OK)
        continue;
      fprintf(stderr, "unknown keyword/value given: %s\n", item[i]);
      return 0;
    }
  }
  if (mode&SCANITEMLIST_REMOVE_USED_ITEMS) {
    for (i=j=0; i<*items; i++) {
      if (!item_matched[i]) {
        if (i!=j) {
          item_matched[j] = 1;
          item[j] = item[i];
        }
        j++;
      }
    }
    *items = j;
  }
  return retval;
}

long scanItemListLong(unsigned long long *flags, char **item, long *items, unsigned long mode, ...)
/* usage: scanItemList(&flags, item, &items, mode,
   <keyword>, <SDDS-type>, <pointer>, <number-required>, <set-flag>, etc.
   NULL)
   * The mode argument is for future expansion.
   */
{
  va_list argptr;
  long i, j, type, retval, length, number, match;
  long long flag;
  char *keyword;
  void *data;
  static char **valueptr = NULL;
  static long *keylength = NULL;
  static short *item_matched = NULL, *has_equals = NULL;
  static long maxitems = 0;
  
  if (!flags)
    return bombre("null flags pointer seen (scanItemList)", NULL, 0);
  if (!item)
    return bombre("null item pointer seen (scanItemList)", NULL, 0);
  if (!items)
    return bombre("null items pointer seen (scanItemList)", NULL, 0);
  if (*items<=0)  {
    *flags = 0;
    return 1;
  }
  if (*items>maxitems) {
    valueptr = trealloc(valueptr, sizeof(*valueptr)*(maxitems= *items));
    keylength = trealloc(keylength, sizeof(*keylength)*maxitems);
    item_matched = trealloc(item_matched, sizeof(*item_matched)*maxitems);
    has_equals = trealloc(has_equals, sizeof(*has_equals)*maxitems);
  }
  *flags = 0;
  for (i=0; i<*items; i++) {
    item_matched[i] = 0;
#if DEBUG
    fprintf(stderr, "item %ld: %s\n", i, item[i]);
#endif
    if ((valueptr[i]=strchr(item[i], '='))) {
      if ((keylength[i] = valueptr[i]-item[i])<=0)
        return bombre("zero-length keyword seen (scanItemList)", NULL, 0);
      *valueptr[i]++ = 0;
      has_equals[i] = 1;
    }
    else {
      keylength[i] = strlen(item[i]);
      has_equals[i] = 0;
    }
    
#if DEBUG
    fprintf(stderr, "  keyword: %s   valueptr: %s\n", item[i], valueptr[i]);
#endif
  }

  va_start(argptr, mode);
  retval = 1;
  do {
    if (!(keyword = va_arg(argptr, char *)))
      break;
    type = va_arg(argptr, int32_t);
    data = va_arg(argptr, void *);
    number = va_arg(argptr, int32_t);
    flag = va_arg(argptr, uint64_t);
    length = strlen(keyword);
    match = -1;
    for (i=0; i<*items; i++) 
      if (strncmp_case_insensitive(item[i], keyword, MIN(length, keylength[i]))==0) {
        if (match!=-1) {
          fprintf(stderr, "ambiguous item %s seen\n", keyword);
          retval = 0;
        }
        match = i;
      }
#if defined(DEBUG)        
    if (match!=-1)
      fprintf(stderr, "keyword %s and item %ld (%s) match\n", keyword, match, item[match]);
    else
      fprintf(stderr, "no match for keyword %s\n", keyword);
#endif
    if (!retval)
      break;
    if (match==-1)
      continue;
    if (!has_equals[match] && number && mode&SCANITEMLIST_IGNORE_VALUELESS)
      continue;
    if (item_matched[match]) {
      fprintf(stderr, "error: ambiguous qualifier %s seen\n", item[match]);
      retval = 0;
      break;
    }
    item_matched[match] = 1;
    *flags |= flag;
    if (!valueptr[match]) {
      if (type==-1)
        continue;
      fprintf(stderr, "error: value not given for %s\n", keyword);
      retval = 0;
      break;
    }
    switch (type) {
    case SDDS_DOUBLE:
      *((double*)data) = atof(valueptr[match]);
      break;
    case SDDS_FLOAT:
      *((float*)data) = (float)atof(valueptr[match]);
      break;
    case SDDS_LONG:
      *((int32_t*)data) = atol(valueptr[match]);
      break;
    case SDDS_ULONG:
#if defined(_WIN32)
      *((uint32_t*)data) = (uint32_t)_atoi64(valueptr[match]);
#else
      *((uint32_t*)data) = (uint32_t)atoll(valueptr[match]);
#endif
      break;
    case SDDS_SHORT:
      *((short*)data) = (short)atol(valueptr[match]);
      break;
    case SDDS_USHORT:
      *((unsigned short*)data) = (unsigned short)atol(valueptr[match]);
      break;
    case SDDS_STRING:
      cp_str((char**)data, valueptr[match]);
      break;
    case SDDS_CHARACTER:
      *((char*)data) = valueptr[match][0];
      break;
    default:
      fprintf(stderr, "Error: value not accepted for qualifier %s\n",
              item[match]);
      break;
    }
  } while (retval==1);
  va_end(argptr);
  for (i=0; i<*items; i++) {
    if (!item_matched[i]) {
      if (!has_equals[i] && mode&SCANITEMLIST_UNKNOWN_VALUE_OK)
        continue;
      if (has_equals[i] && mode&SCANITEMLIST_UNKNOWN_KEYVALUE_OK)
        continue;
      fprintf(stderr, "unknown keyword/value given: %s\n", item[i]);
      return 0;
    }
  }
  if (mode&SCANITEMLIST_REMOVE_USED_ITEMS) {
    for (i=j=0; i<*items; i++) {
      if (!item_matched[i]) {
        if (i!=j) {
          item_matched[j] = 1;
          item[j] = item[i];
        }
        j++;
      }
    }
    *items = j;
  }
  return retval;
}


/* This version doesn't have the functionality I really want, but it is used in many
 * places.   Primary problem is that it doesn't flag entries that don't match any
 * keywords; some routines may depend on this behavior.
 */

long scan_item_list(unsigned long *flags, char **item, long *items, ...)
/* usage: scan_item_list(&flags, item, &items, 
   <keyword>, <SDDS-type>, <pointer>, <number-required>, <set-flag>, etc.
   NULL)
   */
{
  va_list argptr;
  long i, j, type, flag, retval, length, number, match;
  char *keyword;
  void *data;
  static char **valueptr = NULL;
  static long *keylength = NULL, *item_matched = NULL;
  static long maxitems = 0;
  
  if (!flags)
    return bombre("null flags pointer seen (scan_item_list)", NULL, 0);
  if (!item)
    return bombre("null item pointer seen (scan_item_list)", NULL, 0);
  if (!items)
    return bombre("null items pointer seen (scan_item_list)", NULL, 0);
  if (*items<=0)  {
    *flags = 0;
    return 1;
  }
  if (*items>maxitems) {
    valueptr = trealloc(valueptr, sizeof(*valueptr)*(maxitems= *items));
    keylength = trealloc(keylength, sizeof(*keylength)*maxitems);
    item_matched = trealloc(item_matched, sizeof(*item_matched)*maxitems);
  }
  *flags = 0;
  for (i=0; i<*items; i++) {
    item_matched[i] = 0;
#if DEBUG
    fprintf(stderr, "item %ld: %s\n", i, item[i]);
#endif
    if ((valueptr[i]=strchr(item[i], '='))) {
      if ((keylength[i] = valueptr[i]-item[i])<=0)
        return bombre("zero-length keyword seen (scan_item_list)", NULL, 0);
      *valueptr[i]++ = 0;
    }
    else
      keylength[i] = strlen(item[i]);
#if DEBUG
    fprintf(stderr, "  keyword: %s   valueptr: %s\n", item[i], valueptr[i]);
#endif
  }

  va_start(argptr, items);
  retval = 1;
  do {
    if (!(keyword = va_arg(argptr, char *)))
      break;
    type = va_arg(argptr, int32_t);
    data = va_arg(argptr, void *);
    number = va_arg(argptr, int32_t);
    flag = va_arg(argptr, uint32_t);
    length = strlen(keyword);
    match = -1;
    for (i=0; i<*items; i++) 
      if (strncmp_case_insensitive(item[i], keyword, MIN(length, keylength[i]))==0) {
        if (match!=-1) {
          fprintf(stderr, "ambiguous item %s seen\n", keyword);
          retval = 0;
        }
        match = i;
      }
#if defined(DEBUG)        
    if (match!=-1)
      fprintf(stderr, "keyword %s and item %ld (%s) match\n", keyword, match, item[match]);
    else
      fprintf(stderr, "no match for keyword %s\n", keyword);
#endif
    if (!retval)
      break;
    if (match==-1)
      continue;
    if (item_matched[match]) {
      fprintf(stderr, "error: ambiguous qualifier %s seen\n", item[match]);
      retval = 0;
      break;
    }
    item_matched[match] = 1;
    *flags |= flag;
    if (!valueptr[match]) {
      if (type==-1)
        continue;
      fprintf(stderr, "error: value not given for %s\n", keyword);
      retval = 0;
      break;
    }
    switch (type) {
    case SDDS_DOUBLE:
      *((double*)data) = atof(valueptr[match]);
      break;
    case SDDS_FLOAT:
      *((float*)data) = (float)atof(valueptr[match]);
      break;
    case SDDS_LONG:
      *((int32_t*)data) = atol(valueptr[match]);
      break;
    case SDDS_ULONG:
#if defined(_WIN32)
      *((uint32_t*)data) = (uint32_t)_atoi64(valueptr[match]);
#else
      *((uint32_t*)data) = (uint32_t)atoll(valueptr[match]);
#endif
      break;
    case SDDS_SHORT:
      *((short*)data) = (short)atol(valueptr[match]);
      break;
    case SDDS_USHORT:
      *((unsigned short*)data) = (unsigned short)atol(valueptr[match]);
      break;
    case SDDS_STRING:
      cp_str((char**)data, valueptr[match]);
      break;
    case SDDS_CHARACTER:
      *((char*)data) = valueptr[match][0];
      break;
    default:
      fprintf(stderr, "Error: value not accepted for qualifier %s\n", item[match]);
      break;
    }
    free(item[match]);
    (*items)--;
    for (j=match; j<*items; j++) {
      item[j] = item[j+1];
      valueptr[j] = valueptr[j+1];
      keylength[j] = keylength[j+1];
    }
  } while (retval==1 && *items);
  va_end(argptr);
  return retval;
}

long contains_keyword_phrase(char *string)
{
  char *ptr, *ptr0;
  ptr0 = string;
  while ((ptr=strchr(string, '='))) {
    if (ptr!=ptr0 && *(ptr-1)!='\\') 
      return 1;
    if (ptr!=ptr0 && *(ptr-1)=='\\')
      strcpy_ss(ptr-1, ptr);
    string = ptr+1;
  }
  return 0;
}

