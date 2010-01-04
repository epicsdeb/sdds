/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: halton.c,v $
 Revision 1.8  2005/11/04 22:47:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.7  2002/08/14 16:18:56  soliday
 Added Open License

 Revision 1.6  2000/11/04 17:52:28  borland
 The value argument to startHaltonSequence point is no longer a pointer.

 Revision 1.5  2000/11/04 17:48:17  borland
 Separated function nextHaltonSequencePoint into two functions.  The really
 new function is startHaltonSequence.  The new version of nextHaltonSequencePoint
 only does what the name suggests, rather than doubling as an initialization
 routine.  Also, the sequence IDs are now always positive integers.

 Revision 1.4  2000/11/03 15:18:27  borland
 The user can now specify what radix to use by giving it as a negative
 value in the initial call.

 Revision 1.3  2000/11/03 14:11:00  borland
 Removed the number 13 from the list of preset prime numbers.  Gives better
 results for the first 6 sequences.

 Revision 1.2  2000/11/02 21:32:18  borland
 Removed unneeded tolerance argument.

 Revision 1.1  2000/11/02 19:43:18  borland
 First version.

 */

/* file: halton.c
 * contents:
 * reference: Num. Math. 2, 84-90, 1960.
 *            
 *
 */ 
#include "mdb.h"

static double *lastPointValue = NULL;
static long *R = NULL;
static long sequencesInUse = 0;
/* These 12 primes work pretty well together.  
 * If more areneeded, they are generated on the fly.
 */
#define N_SEQ_PREDEFINED 12
static long Rvalues[N_SEQ_PREDEFINED] = {2, 3, 5, 7, 11, 19, 23, 29, 37, 47, 59, 67};

int32_t startHaltonSequence(int32_t *radix, double value)
{
  int32_t ID;
  if ((sequencesInUse==0 &&
       (!(lastPointValue=malloc(sizeof(*lastPointValue))) ||
        !(R=malloc(sizeof(*R))))) ||
      (!(lastPointValue=realloc(lastPointValue, (sequencesInUse+1)*sizeof(*lastPointValue))) ||
       !(R=realloc(R, (sequencesInUse+1)*sizeof(*R)))))
    return 0;
  if (*radix>0) {
    if (is_prime(*radix)!=1)
      return 0;
    R[sequencesInUse] = *radix;
    ID = sequencesInUse;
  } else {
    /* generate a new, unique prime number for use as the radix */
    long i, passed;
    if ((ID = sequencesInUse)<N_SEQ_PREDEFINED)
      /* try one of the favored values */
      *radix = Rvalues[ID];
    else 
      *radix = 2;
    passed = 0;
    while (!passed) {
      passed = 1;
      for (i=0; i<sequencesInUse; i++) {
        if (R[i]== *radix) {
          passed = 0;
          (*radix)++;
          while (is_prime(*radix)!=1)
            (*radix)++;
        }
      }
    }
    R[ID] = *radix;
  }
  lastPointValue[ID] = value;
  sequencesInUse++;
  return ID+1;
}
  
double nextHaltonSequencePoint(long ID)
{
  double r, f, value;

  ID -= 1;

  if (ID>sequencesInUse || ID<0)
    return -1;

  f = 1 - lastPointValue[ID];
  r = 1./R[ID];
  while (f<=r) 
    r = r/R[ID];
  value = lastPointValue[ID] + (R[ID]+1)*r - 1;
  lastPointValue[ID] = value;

  return value;
}

