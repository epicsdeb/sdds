/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: sortfunctions.c
 * contents: double_cmpasc(), double_cmpdes(), double_copy()
 *           float_cmpasc(), float_cmpdes(), float_copy()
 *           long_cmpasc(), long_cmpdes(), long_copy()
 *           string_cmpasc(), string_cmpdsc(), string_copy()
 *           unique()
 *           set_up_row_sort(), row_sort_cmp()
 *
 * purpose: useful routines for sorting, compatible with qsort()
 * Michael Borland, 1989, 1995
 $Log: sortfunctions.c,v $
 Revision 1.15  2007/10/25 22:36:08  borland
 Added strcmp_skip, which can compare strings while skipping characters
 in a third string.

 Revision 1.14  2007/07/13 15:02:45  soliday
 Renamed strcpy_s to strcpy_ss because Microsoft is already using this name.

 Revision 1.13  2007/02/02 18:45:28  borland
 Added str_copy.c, with strcpy_s() routine for safe string copies.
 Converted routines from strcpy() to strcpy_s().

 Revision 1.12  2006/08/31 15:06:26  soliday
 Updated to work with SDDS2

 Revision 1.11  2005/11/04 22:47:10  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.10  2004/12/21 20:11:19  shang
 added sort_and_return_index function for sorting data and returning the index of
 sorted data.

 Revision 1.9  2003/08/28 20:13:44  soliday
 Cleaned up the code.

 Revision 1.8  2003/01/08 19:32:55  borland
 Added bracketing option to binaryIndexSearch(), which adds an argument.

 Revision 1.7  2002/11/15 18:42:08  soliday
 Removed SDDS_Bomb call because the SDDS library is built after this library.

 Revision 1.6  2002/10/21 22:39:38  shang
 moved the sorting and comparing functions from sddsxref.c

 Revision 1.5  2002/08/14 16:06:14  soliday
 Added Open License

 Revision 1.4  1998/08/26 14:53:02  borland
 Fixed the integer sort functions, which I guess I never used until recently.

 * Revision 1.3  1995/11/13  16:18:56  borland
 * Added long cast to strlen() in comparisons to satisfy Solaris compiler.
 *
 * Revision 1.2  1995/09/05  21:18:25  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "SDDS.h"

int double_cmpasc(const void *a, const void *b)
{
    double diff;

    diff = *((double*)b) - *((double*)a);
    return( diff<0 ? 1 : (diff>0? -1: 0 ) );
    }

int double_cmpdes(const void *a, const void *b)
{
    double diff;

    diff = *((double*)b) - *((double*)a);
    return( diff>0 ? 1 : (diff<0? -1: 0 ) );
    }

void double_copy(void *a, void *b)
{

    *((double*)a) = *((double*)b);
    }



int float_cmpasc(const void *a, const void *b)
{
    float diff;

    diff = *((float*)b) - *((float*)a);
    return( diff<0 ? 1 : (diff>0? -1: 0 ) );
    }

int float_cmpdes(const void *a, const void *b)
{
    float diff;
    
    diff = *((float*)b) - *((float*)a);
    return( diff>0 ? 1 : (diff<0? -1: 0 ) );
    }

void float_copy(void *a, void *b)
{
    *((float*)a) = *((float*)b);
    }


int long_cmpasc(const void *a, const void *b)
{
    int32_t diff;
    diff = *((int32_t*)b) - *((int32_t*)a);
    return( diff<0 ? 1 : (diff>0? -1: 0 ) );
    }

int long_cmpdes(const void *a, const void *b)
{
    long diff;
    diff = *((long*)a) - *((long*)b);
    return( diff<0 ? 1 : (diff>0? -1: 0 ) );
    }

void long_copy(void *a, void *b)
{
    *((long*)a) = *((long*)b);
    }
 
int string_cmpasc(const void *a, const void *b)
{
    return( strcmp(*((char**)a), *((char**)b)) );
    }

int string_cmpdes(const void *a, const void *b)
{
    return( strcmp(*((char**)b), *((char**)a)) );
    }

void string_copy(void *a, void *b)
{
    if ((long)strlen(*((char**)a))>=(long)strlen(*((char**)b)))
        strcpy_ss(*((char**)a), *((char**)b));
    else
        cp_str(((char**)a), *((char**)b));
    }

int unique(void *base, size_t n_items, size_t size,
          int (*compare)(const void *a, const void *b),
          void (*copy)(void *a, void *b))
{
    long i, j;

    for (i=0; i<n_items-1; i++) {
        if ((*compare)((char*)base+i*size, (char*)base+(i+1)*size)==0) {
            for (j=i+1; j<n_items-1; j++)
                (*copy)((char*)base+j*size, (char*)base+(j+1)*size);
            n_items--;
            i--;
            }
        }
    return(n_items);
    }


/* routines: set_up_row_sort(), row_sort_cmp()
 * purpose: allow sorting of 2-d data by rows, using one column as the
 *          key.  The data must be in an array that is allocated as
 *          an array of pointers to pointers.
 *
 * Michael Borland, 1989, 1995.
 */

static int (*item_compare)(const void *a, const void *b);
static int column_to_compare;
static int size_of_element;
static int number_of_columns;

void set_up_row_sort(
    int sort_by_column,
    size_t n_columns,
    size_t element_size,
    int (*compare)(const void *a, const void *b)
    )
{
    if ((column_to_compare = sort_by_column)>=(number_of_columns=n_columns))
        bomb("column out of range in set_up_row_sort()", NULL);
    size_of_element = element_size;
    if (!(item_compare = compare))
        bomb("null function pointer in set_up_row_sort()", NULL);
    }

int row_compare(const void *av, const void *bv)
{
    char **a, **b;
    a = (char **)av;
    b = (char **)bv;
    return((*item_compare)(*a+size_of_element*column_to_compare,
                           *b+size_of_element*column_to_compare));
    }

void row_copy(void *av, void *bv)
{
    void **a, **b;
    void *ptr;
    a = (void **)av;
    b = (void **)bv;
    ptr = *a;
    *a  = *b;
    *b  = ptr;
  }

static long orderIndices;  /* compare source indices if keys are identical? */

int CompareStringKeyedIndex(const void *ki1, const void *ki2)
{
  int value;
  if ((value = strcmp((*(const KEYED_INDEX *)ki1).stringKey, (*(const KEYED_INDEX *)ki2).stringKey)))
    return value;
  if (orderIndices)
    return (*(const KEYED_INDEX *)ki1).rowIndex - (*(const KEYED_INDEX *)ki2).rowIndex;
  return value;
}

int CompareDoubleKeyedIndex(const void *ki1, const void *ki2)
{
  double diff;
  if ((diff = (*(const KEYED_INDEX *)ki1).doubleKey - (*(const KEYED_INDEX *)ki2).doubleKey)) {
    if (diff<0)
      return -1;
    return 1;
  }
  if (orderIndices)
    return (*(const KEYED_INDEX *)ki1).rowIndex - (*(const KEYED_INDEX *)ki2).rowIndex;
  return 0;
}

int CompareStringKeyedGroup(void *kg1, void *kg2)
{
  return strcmp((*(KEYED_EQUIVALENT *)kg1).equivalent[0]->stringKey, (*(KEYED_EQUIVALENT *)kg2).equivalent[0]->stringKey);
}

int CompareDoubleKeyedGroup(void *kg1, void *kg2)
{
  double diff;
  if ((diff = (*(KEYED_EQUIVALENT *)kg1).equivalent[0]->doubleKey - (*(KEYED_EQUIVALENT *)kg2).equivalent[0]->doubleKey)) {
    if (diff<0)
      return -1;
    return 1;
  }
  return 0;
}

KEYED_EQUIVALENT **MakeSortedKeyGroups(long *keyGroups, long keyType, void *data, long points)
{
  KEYED_EQUIVALENT **keyedEquiv=NULL;
  static KEYED_INDEX *keyedIndex = NULL;
  long iEquiv, i1, i2, j;
  
  if (!points)
    return 0;
  if (keyedIndex)
    free(keyedIndex);
  if (!(keyedIndex = (KEYED_INDEX*)malloc(sizeof(*keyedIndex)*points)) || 
      !(keyedEquiv = (KEYED_EQUIVALENT**)malloc(sizeof(*keyedEquiv)*points))) {
    fprintf(stderr, "memory allocation failure");
    exit(1);
  }
  if (keyType==SDDS_STRING) {
    char **string;
    string = data;
    for (i1=0; i1<points; i1++) {
      keyedIndex[i1].stringKey = string[i1];
      keyedIndex[i1].rowIndex = i1;
    }
    orderIndices = 1;    /* subsort by source row index */
    qsort((void*)keyedIndex, points, sizeof(*keyedIndex), CompareStringKeyedIndex);
    orderIndices = 0;    /* ignore index in comparisons */
    for (iEquiv=i1=0; i1<points; iEquiv++) {
      for (i2=i1+1; i2<points; i2++) {
        if (CompareStringKeyedIndex(keyedIndex+i1, keyedIndex+i2))
          break;
      }
      if (!(keyedEquiv[iEquiv]=(KEYED_EQUIVALENT*)malloc(sizeof(KEYED_EQUIVALENT))) ||
          !(keyedEquiv[iEquiv]->equivalent = (KEYED_INDEX**)malloc(sizeof(KEYED_INDEX*)*(i2-i1)))) {
        fprintf(stderr, "memory allocation failure");
	exit(1);
      }
      keyedEquiv[iEquiv]->equivalents = i2 - i1;
      keyedEquiv[iEquiv]->nextIndex = 0;
      for (j=0; i1<i2; i1++, j++) 
        keyedEquiv[iEquiv]->equivalent[j] = keyedIndex+i1;
    }
  } else {
    double *value;
    value = data;
    for (i1=0; i1<points; i1++) {
      keyedIndex[i1].doubleKey = value[i1];
      keyedIndex[i1].rowIndex = i1;
    }
    orderIndices = 1;    /* subsort by source row index */
    qsort((void*)keyedIndex, points, sizeof(*keyedIndex), CompareDoubleKeyedIndex);
    orderIndices = 0;    /* ignore index in comparisons */
    for (iEquiv=i1=0; i1<points; iEquiv++) {
      for (i2=i1+1; i2<points; i2++) {
        if (CompareDoubleKeyedIndex(keyedIndex+i1, keyedIndex+i2))
          break;
      }
      if (!(keyedEquiv[iEquiv]=(KEYED_EQUIVALENT*)malloc(sizeof(KEYED_EQUIVALENT))) ||
          !(keyedEquiv[iEquiv]->equivalent = (KEYED_INDEX**)malloc(sizeof(KEYED_INDEX*)*(i2-i1)))) {
        fprintf(stderr, "memory allocation failure");
	exit(1);
      }
      keyedEquiv[iEquiv]->equivalents = i2 - i1;
      keyedEquiv[iEquiv]->nextIndex = 0;
      for (j=0; i1<i2; i1++, j++) 
        keyedEquiv[iEquiv]->equivalent[j] = keyedIndex+i1;
    }
  }
  *keyGroups = iEquiv;
  return keyedEquiv;
}

long FindMatchingKeyGroup(KEYED_EQUIVALENT **keyGroup, long keyGroups, long keyType,
                          void *searchKeyData, long reuse)
{
  static KEYED_EQUIVALENT *searchKey = NULL;
  static KEYED_INDEX keyedIndex;
  long rowIndex, i;
  
  if (!searchKey) {
    searchKey = (KEYED_EQUIVALENT*)malloc(sizeof(*searchKey));
    searchKey->equivalent = (KEYED_INDEX**)malloc(sizeof(*(searchKey->equivalent)));
    searchKey->equivalent[0] = &keyedIndex;
    searchKey->equivalents = 1;
  }
  if (keyType==SDDS_STRING) {
    keyedIndex.stringKey = *(char**)searchKeyData;
    i = binaryIndexSearch((void**)keyGroup, keyGroups, (void*)searchKey, CompareStringKeyedGroup, 0);
  } else {
    keyedIndex.doubleKey = *(double*)searchKeyData;
    i = binaryIndexSearch((void**)keyGroup, keyGroups, (void*)searchKey, CompareDoubleKeyedGroup, 0);
  }
  if (i<0 || keyGroup[i]->nextIndex>=keyGroup[i]->equivalents)
    return -1;
  rowIndex = keyGroup[i]->equivalent[keyGroup[i]->nextIndex]->rowIndex;
  if (!reuse)
    keyGroup[i]->nextIndex += 1;
  return rowIndex;
}

/*return the index of sorted data, and the sorted data */
/*if increaseOrder is 1, sort by increasing order; else if 0, sort by decreasing order */
long *sort_and_return_index(void *data, long type, long rows, long increaseOrder)
{
  long *index=NULL;
  long i, keyGroups, i1, j, istart, iend,jstart,jend, i2, j2;
  KEYED_EQUIVALENT **keyGroup;
  char **tmpstring=NULL;
  double *tmpdata=NULL;
  
  if (!rows || !data)
    return 0;
  index=(long*)malloc(sizeof(*index)*rows);
  switch (type) {
  case SDDS_STRING:
    tmpstring=(char**)data;
    keyGroup=MakeSortedKeyGroups(&keyGroups, SDDS_STRING, tmpstring, rows);
    break;
  default:
    if (type==SDDS_DOUBLE)
      tmpdata=(double*)data;
    else {
      tmpdata=calloc(sizeof(*tmpdata),rows);
      for (i=0;i<rows;i++) {
        switch (type) {
        case SDDS_SHORT:
          tmpdata[i] = *((short*)data+i);
          break;
        case SDDS_USHORT:
          tmpdata[i] = *((unsigned short*)data+i);
          break;
        case SDDS_LONG:
          tmpdata[i] = *((int32_t*)data+i);
          break;
        case SDDS_ULONG:
          tmpdata[i] = *((uint32_t*)data+i);
          break;
        case SDDS_CHARACTER:
          tmpdata[i] = *((unsigned char*)data+i);
          break;
        case SDDS_FLOAT:
          tmpdata[i] = *((float*)data+i);
          break;
        default:
          fprintf(stderr,"Invalid data type given!\n");
          exit(1);
          break;
        }
      }
    }
    keyGroup=MakeSortedKeyGroups(&keyGroups, SDDS_DOUBLE, tmpdata, rows);
    if (type!=SDDS_DOUBLE)
      free(tmpdata);
    break;
  }
  i1=0;
  if (increaseOrder) {
    istart=0;
    iend=keyGroups-1;
  } else {
    istart=keyGroups-1;
    iend=0;
  }
  for (i=istart,i2=0; i2<keyGroups;i2++) {
    if (increaseOrder) {
      jstart=0;
      jend=keyGroup[i]->equivalents-1;
    } else {
      jstart=keyGroup[i]->equivalents-1;
      jend=0;
    }
    for (j=jstart, j2=0;j2<keyGroup[i]->equivalents;j2++) {
      switch (type) {
      case SDDS_STRING:
        ((char**)data)[i1]=keyGroup[i]->equivalent[j]->stringKey;
        break;
      case SDDS_DOUBLE:
        ((double*)data)[i1]=keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_FLOAT:
        ((float*)data)[i1]=(float)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_LONG:
        ((int32_t*)data)[i1]=(int32_t)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_ULONG:
        ((uint32_t*)data)[i1]=(uint32_t)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_SHORT:
        ((short*)data)[i1]=(short)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_USHORT:
        ((unsigned short*)data)[i1]=(unsigned short)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      case SDDS_CHARACTER:
        ((char*)data)[i1]=(unsigned char)keyGroup[i]->equivalent[j]->doubleKey;
        break;
      default:
        fprintf(stderr,"Invalid data type given!\n");
        exit(1);
        break;
      }
      index[i1]=keyGroup[i]->equivalent[j]->rowIndex;
      i1++;
      if (increaseOrder)
        j++;
      else
        j--;
    }
    if (increaseOrder)
      i++;
    else
      i--;
  }
  for (i=0;i<keyGroups;i++) {
    free(keyGroup[i]->equivalent);
    free(keyGroup[i]);
  }
  free(keyGroup);
  return index;
}

int strcmp_skip(const char *s1, const char *s2, const char *skip)
{
  do {
    if (*s1!=*s2) {
      while (*s1 && strchr(skip, *s1))
        s1 ++;
      while (*s2 && strchr(skip, *s2))
        s2 ++;
      if (*s1 != *s2)
        return *s1-*s2;
    }
    s1++;
    s2++;
  } while (*s1 && *s2);
  return *s1-*s2;
}

