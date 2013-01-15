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
 Revision 1.8  2003/08/28 20:13:43  soliday
 Cleaned up the code.

 Revision 1.7  2003/06/17 23:36:44  borland
 Fixed (improved?) handling of boundary conditions.  E.g., items that
 are above or below all the items already in the list.

 Revision 1.5  2003/01/08 22:41:48  borland
 Added binaryArraySearch().

 Revision 1.4  2003/01/08 19:32:55  borland
 Added bracketing option to binaryIndexSearch(), which adds an argument.

 Revision 1.3  2002/08/14 16:06:05  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:16:51  saunders
 First test release of the SDDS1.5 package.

*/

/* file: binsert.c
 * purpose: binary search and insertion 
 * M. Borland, 1995
 */

#include "mdb.h"

/* The data for binaryInsert and binaryIndexSearch is an array of pointers to the
 * values.  To search a simple array of values, use binaryArraySearch.
 */

long binaryInsert(void **array, long members, void *newMember, 
                  int (*compare)(void *c1, void *c2), int32_t *duplicate)
{
  long ih, il, im, comparison;

  *duplicate = 0;
  if (members==0) {
    *array = newMember;
    return 0;
  }

  il = 0;
  ih = members - 1;
  if ((comparison=(*compare)(array[il], newMember))>=0) {
    if (comparison==0) {
      *duplicate = 1;
      return 0;
    }
    im = 0;
  }
  else if ((comparison=(*compare)(array[ih], newMember))<=0) {
    if (comparison==0) {
      *duplicate = 1;
      return ih;
    }
    im = members;
  }
  else {
    while ((ih-il)>1) {
      im = (il+ih)/2;
      if ((comparison=(*compare)(array[im], newMember))==0) {
        *duplicate = 1;
        return im;
      }
      if (comparison>0)
        ih = im;
      else
        il = im;
    }
    im = ih;
  }
  for (il=members; il>im; il--)
    array[il] = array[il-1];
  array[im] = newMember;
  return im;
}

long binaryIndexSearch(void **array, long members, void *key, 
                       int (*compare)(void *c1, void *c2), long bracket)
{
  long ih, il, im, comparison;

  if (members==0)
    return -1;

  il = 0;
  ih = members - 1;
  if ((comparison=(*compare)(array[il], key))>=0) {
    if (comparison==0)
      return il;
    im = 0;
  }
  else if ((comparison=(*compare)(array[ih], key))<=0) {
    if (comparison==0)
      return ih;
    im = members;
  }
  else {
    while ((ih-il)>1) {
      im = (il+ih)/2;
      if ((comparison=(*compare)(array[im], key))==0)
        return im;
      if (comparison>0)
        ih = im;
      else
        il = im;
    }
    im = ih;
  }
  if (!bracket)
    return -1;

  /* return index of nearest point less than or equal to the key */
  comparison = (*compare)(array[im], key);
  if (comparison<=0)
    return im;
  comparison = (*compare)(array[il], key);
  if (comparison<=0)
    return il;
  return -1;
}


long binaryArraySearch(void *array, size_t elemSize, long members, void *key, 
                       int (*compare)(void *c1, void *c2), long bracket)
/* Same as binaryIndexSearch, but the array is an array of data values,
 * rather than an array of pointers 
 */
{
  long ih, il, im, comparison;

  if (members==0)
    return -1;

  il = 0;
  ih = members - 1;
  if ((comparison=(*compare)((void*)((char*)array+il*elemSize), key))>=0) {
    if (comparison==0)
      return il;
    im = 0;
  }
  else if ((comparison=(*compare)((void*)((char*)array+ih*elemSize), key))<=0) {
    if (comparison==0)
      return ih;
    im = ih;
  }
  else {
    while ((ih-il)>1) {
      im = (il+ih)/2;
      if ((comparison=(*compare)((void*)((char*)array+im*elemSize), key))==0)
        return im;
      if (comparison>0)
        ih = im;
      else
        il = im;
    }
    im = ih;
  }
  if (!bracket)
    return -1;

  /* return index of nearest point less than or equal to the key */
  comparison = (*compare)((void*)((char*)array+im*elemSize), key);
  if (comparison<=0)
    return im;
  comparison = (*compare)((void*)(((char*)array)+il*elemSize), key);
  if (comparison<=0)
    return il;
  return -1;
}


