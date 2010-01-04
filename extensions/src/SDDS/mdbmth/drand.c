/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: drand.c,v $
 Revision 1.18  2006/02/09 16:37:13  soliday
 Updated gauss_rn_lim_oag so that it works correctly.

 Revision 1.17  2006/02/08 23:01:42  soliday
 Added functions to increment the seed as if the random function was called
 multiple times. The new functions are random_oag, gauss_rn_oag, gauss_rn_lim_oag

 Revision 1.16  2005/11/04 22:47:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.15  2003/08/28 19:56:26  soliday
 Cleaned up some of the code.

 Revision 1.14  2003/08/18 13:55:49  soliday
 Added changes for the rtems architecture.

 Revision 1.13  2002/08/14 16:18:54  soliday
 Added Open License

 Revision 1.12  2001/10/12 17:37:52  borland
 Now use RAND_MAX from stdlib.h to give range of values from system
 rand() routine.

 Revision 1.11  2001/02/19 21:45:15  soliday
 erf is not implemented on vxWorks.

 Revision 1.10  2000/12/06 20:16:21  soliday
 Added include statement for fdlibm.h on WIN32.

 Revision 1.9  2000/11/03 15:17:05  borland
 randomizeOrder can now randomize arrays of arbitrary-sized objects.

 Revision 1.8  2000/11/02 21:33:13  borland
 Added convertSequenceToGaussianDistribution function.

 Revision 1.7  2000/11/02 19:44:15  borland
 Added randomizeOrder, a function to randomize the order of items in an array.

 Revision 1.6  2000/05/08 02:34:02  borland
 Fixed stupid bug I introduced in seeding of random number generators.  Automatic
 seeding of random_2,3,4 was done incorrectly (using fixed values).

 Revision 1.5  2000/03/27 20:26:41  borland
 Fixed problems with seeding of random_2 and random_3.  Added random_4.

 Revision 1.4  1999/05/28 14:57:24  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1998/04/21 21:27:41  borland
 Fixed but in initialization of RN generators.

 * Revision 1.2  1995/09/05  21:19:35  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file    : drand.c
 * contents: drand(), rdrand(), tseed(), r_theta_rand(), 
 *           random_1() (etc.), gauss_rn(),  gauss_rn_lim()
 * purpose : random number generators
 *
 * Michael Borland, 1988, 1995.
 */ 
#include "mdb.h"
#include <time.h>
#include <stdlib.h>

#if defined(_WIN32)
#include "fdlibm.h"
#endif
#include "f2c.h"

extern double dlaran_(integer *seed);
extern double dlaran_oag(integer *seed, long increment);

#define MAX_RAND_INT (1.0*RAND_MAX)

/* routine: drand()
 * purpose: return a random number on the interval 0-1
 */

float drand(long dummy) 
{
    return((float)(1.0*rand()/MAX_RAND_INT));
    }

/* routine: rdrand()
 * purpose: return a random number on the interval lo-hi
 */

double rdrand(lo, hi)
double lo, hi;
{
    return(lo+((hi-lo)*rand())/MAX_RAND_INT);
    }

/* routine: tseed()
 * purpose: seed rand() with clock time
 */
 
void tseed()
{
    srand((int)time((time_t)0));
    }

/* routine: r_theta_rand()
 * purpose: return a random (r, theta) pair on an annulus 
 */
 
void r_theta_rand(double *r, double *theta, double r_min, double r_max)
{
    double area, sqr_r_min;

    *theta = rdrand(0.0, PIx2);
    sqr_r_min = sqr(r_min);
    area   = rdrand(0.0, sqr(r_max)-sqr_r_min);
    *r     = sqrt(area + sqr_r_min);
    }

double random_1(long iseed)
{
    static short initialized = 0;
    static integer seed[4] = {0,0,0,0};

    if (!initialized || iseed<0) {
        if (iseed<0)
          iseed = -iseed;
        random_2(-(iseed+2));
        random_3(-(iseed+4));
        random_4(-(iseed+6));
        seed[3] = ((iseed & 4095)/2)*2+1;
        seed[2] = (iseed >>= 12) & 4095;
        seed[1] = (iseed >>= 12) & 4095;
        seed[0] = (iseed >>= 12) & 4095;
        initialized = 1;
        }
    if (!initialized)
        bomb("random_1 not properly initialized", NULL);

    return dlaran_(seed);
    }

double random_2(long iseed)
{
    static short initialized = 0;
    static integer seed[4] = {0,0,0,0};

    if (!initialized || iseed<0) {
        if (iseed<0)
          iseed = -iseed;
        seed[3] = ((iseed & 4095)/2)*2+1;
        seed[2] = (iseed >>= 12) & 4095;
        seed[1] = (iseed >>= 12) & 4095;
        seed[0] = (iseed >>= 12) & 4095;
        initialized = 1;
        }
    if (!initialized)
        bomb("random_2 not properly initialized", NULL);

    return dlaran_(seed);
    }

double random_3(long iseed)
{
    static short initialized = 0;
    static integer seed[4] = {0,0,0,0};

    if (!initialized || iseed<0) {
        if (iseed<0)
          iseed = -iseed;
        seed[3] = ((iseed & 4095)/2)*2+1;
        seed[2] = (iseed >>= 12) & 4095;
        seed[1] = (iseed >>= 12) & 4095;
        seed[0] = (iseed >>= 12) & 4095;
        initialized = 1;
        }
    if (!initialized)
        bomb("random_3 not properly initialized", NULL);

    return dlaran_(seed);
    }

double random_4(long iseed)
{
    static short initialized = 0;
    static integer seed[4] = {0,0,0,0};

    if (!initialized || iseed<0) {
        if (iseed<0)
          iseed = -iseed;
        seed[3] = ((iseed & 4095)/2)*2+1;
        seed[2] = (iseed >>= 12) & 4095;
        seed[1] = (iseed >>= 12) & 4095;
        seed[0] = (iseed >>= 12) & 4095;
        initialized = 1;
        }
    if (!initialized)
        bomb("random_4 not properly initialized", NULL);
    
    return dlaran_(seed);
    }


/* routine: gauss_rn()
 * purpose: returns gaussian-distributed random deviates with mean zero
 *          and sigma of 1.
 * Michael Borland, 1989, 1995
 */

double gauss_rn(long iseed, double (*urandom)(long iseed1))
{
    static long valueSaved = 0;
    static double savedValue;
    double urn1, urn2, sine, cosine, factor;
    
    if (iseed<0)
        (*urandom)(iseed);
    if (!valueSaved) {
        urn1 = (*urandom)(0);
        urn2 = (*urandom)(0);
        factor = sqrt(-2*log(urn1));
        cosine = cos(PIx2*urn2);
        sine   = sin(PIx2*urn2);
        savedValue = factor*cosine;
        /* to use saved values, set this to 1 instead 
         * I've disabled this feature as it doesn't work properly with multiple
         * urandom's.
         */
        valueSaved = 0;
        return factor*sine;
        }
    else {
        valueSaved = 0;
        return savedValue;
        }
    }

/* routine: gauss_rn_lim()
 * purpose: return random deviates from a gaussian distribution with
 *          specified sigma, mean, and cutoff. 
 * note: Make a call like gauss_rn(iseed) first to initialize the random
 *       number generator.
 *
 * Michael Borland, 1989
 */

double gauss_rn_lim(
    double mean, double sigma,
    double limit_in_sigmas,     /* if <= 0, ignored. Otherwise, the distribution is
                                 * cut off at +/-limit_in_sigmas*sigma from the mean */
    double (*urandom)(long iseed)
    )
{
    double limit, value;

    if (limit_in_sigmas<=0)
        return(mean+sigma*gauss_rn(0, urandom));
    
    limit = limit_in_sigmas;
    do {
        value = gauss_rn(0, urandom);
        } while (FABS(value)>limit);
    return(sigma*value+mean);
    }


/* routine: convertSequenceToGaussianDistribution()
 * purpose: converts a (supposedly uniformly-distributed) sequence of values 
 *          between 0 and 1 to a gaussian-distributed sequence 
 */

long convertSequenceToGaussianDistribution(double *data, long points, double limit)
{
  double u1, u2, z=0;
  long i, j;
  
  if (!points)
    return 0;
  if (!data)
    return 0;
  for (i=j=0; i<points; i++) {
    u1 = 2*(data[i]-0.5);
    if (u1<0)
      u2 = -u1;
    else
      u2 = u1;
#if defined(vxWorks) || defined(__rtems__)
    fprintf(stderr,"erf function is not implemented on this architecture\n");
    exit(1);
#else
    z = zeroNewton(erf, u2, 0.5, 1e-6, 500, 1e-12);
#endif
    data[j] = z*SQRT2;
    if (limit<=0 || data[j]<limit) {
      if (u1<0)
        data[j] = -data[j];
      j++;
    }
  }
  return j;
}

typedef struct {
  char *buffer;
  double randomValue;
} RANDOMIZATION_HOLDER;

int randomizeOrderCmp(const void *p1, const void *p2)
{
  RANDOMIZATION_HOLDER *rh1, *rh2;
  double diff;
  rh1 = (RANDOMIZATION_HOLDER*)p1;
  rh2 = (RANDOMIZATION_HOLDER*)p2;
  if ((diff=rh1->randomValue-rh2->randomValue)>0)
    return 1;
  if (diff<0)
    return -1;
  return 0;
}

/* randomize the order of an array of elements */

long randomizeOrder(char *ptr, long size, long length, long iseed, double (*urandom)(long iseed1))
{
  RANDOMIZATION_HOLDER *rh;
  long i;
  if (length<2) 
    return 1;
  if (!ptr) 
    return 0;
  if (!(rh = malloc(sizeof(*rh)*length)))
    return 0;
  if (!urandom)
    return 0;
  if (iseed<0)
    (*urandom)(iseed);
  for (i=0; i<length; i++) {
    if (!(rh[i].buffer = malloc(size)))
      return 0;
    memcpy(rh[i].buffer, ptr+i*size, size);
    rh[i].randomValue = (*urandom)(0);
  }
  qsort((void*)rh, length, sizeof(*rh), randomizeOrderCmp);
    
  for (i=0; i<length; i++) {
    memcpy(ptr+i*size, rh[i].buffer, size);
    free(rh[i].buffer);
  }
  free(rh);
  return 1;
}



double random_oag(long iseed, long increment)
{
  static short initialized = 0;
  static long seed[4] = {0,0,0,0};
  
  if (!initialized || iseed<0) {
    if (iseed<0)
      iseed = -iseed;
    seed[3] = ((iseed & 4095)/2)*2+1;
    seed[2] = (iseed >>= 12) & 4095;
    seed[1] = (iseed >>= 12) & 4095;
    seed[0] = (iseed >>= 12) & 4095;
    initialized = 1;
  }
  if (!initialized) {
    fprintf(stderr, "random_oag not properly initialized\n");
    exit(1);
  }
  
  return dlaran_oag(seed, increment);
}


double gauss_rn_oag(long iseed, long increment, double (*urandom)(long iseed1, long increment)) {
  double urn1, urn2, sine, cosine, factor;
  
  if (increment < 1) 
    increment = 1;
  increment = ((increment - 1) * 2) + 1;
  urn1 = (*urandom)(iseed, increment);
  urn2 = (*urandom)(0, 1);
  factor = sqrt(-2*log(urn1));
  cosine = cos(PIx2*urn2);
  sine   = sin(PIx2*urn2);
  return factor*sine;
}

double gauss_rn_lim_oag(
    double mean, double sigma,
    double limit_in_sigmas,     /* if <= 0, ignored. Otherwise, the distribution is
                                 * cut off at +/-limit_in_sigmas*sigma from the mean */
    long increment,
    double (*urandom)(long iseed, long increment)
    ) {
  double limit, value;
  long i;

  if (limit_in_sigmas<=0)
    return(mean+sigma*gauss_rn_oag(0, increment, urandom));
  
  limit = limit_in_sigmas;
  i = 0;
  
  do {
    value = gauss_rn_oag(0, 1, urandom);
    if (FABS(value)<=limit)
      i++;
  } while ((FABS(value)>limit) || (i < increment));
  
  return(sigma*value+mean);
}
