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
 Revision 1.12  2011/01/12 02:02:18  ywang25
 Fixed bugs to generate optimal Halton sequence correctly and efficiently.

 Revision 1.11  2010/12/20 14:33:05  ywang25
 Added restartHaltonSequence and restartModHaltonSequence functions to reinitialize (optimized) Halton sequence when there is only one random bunch to be generated for the multi-step beam generation in elegant; Improved the inhalt function to make it resuable for reintialization; Fixed a bug in startModHaltonSequence, where 0 was returned as the HaltonID for the first dimension and the optimized Halton sequence was not generated properly in elegant.

 Revision 1.10  2010/01/14 16:58:10  shang
 modified the mod halton sequence start routine to have the same call as halton sequence.

 Revision 1.9  2010/01/14 16:35:39  shang
 added CHI's routines for improved halton sequence and added calling wrappers.

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

int32_t restartHaltonSequence(long ID, double value)
{
  ID -= 1;

  if (ID>sequencesInUse || ID<0)
    return -1;

  lastPointValue[ID] = value;

  return 1;
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

/*following code is from outside for improved halton sequence
   Alogrithm 659, Collected Algorithm from ACM
   This is the C version of halton sequences 
   Derandom Algorithm is added on 8/12/03
   by Hongmei CHI (CS/FSU)
   Modified (9.29.03)
*/
#define MAX_D 500
static int32_t sDim=12, nextPoint[12];
static double eError;
static int32_t prime[]={2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31,
                        37, 41, 43, 47, 53, 59, 61, 67, 71, 73,
                        79, 83, 89, 97, 101, 103, 107, 109, 113,
                        127, 131, 137, 139, 149, 151, 157, 163, 
                        167, 173, 179, 181,
                        191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 
                        263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 
                        347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 
                        421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499,
};
static double iprime[]={2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31,
                        37, 41, 43, 47, 53, 59, 61, 67, 71, 73,
                        79, 83, 89, 97, 101, 103, 107, 109, 113,
                        127, 131, 137, 139, 149, 151, 157, 163, 
                        167, 173, 179, 181,
                        191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 
                        263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 
                        347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 
                        421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499,
};

static int32_t modSequenceInUse = 0;
static int32_t  primroots[][10]={{1, 2, 3, 3, 8, 11,12,14, 7,18},
                             {12,13,17,18,29,14,18,43,41,44},
                             {40,30,47,65,71,28,40,60,79,89},
                             {56,50,52,61,108,56,66,63,60,66},
                             {104,76,111,142,71,154,118,84,127,142},
                             {84,105,186,178,188,152,165,159,103,205}, 
                             {166,173,188,181,91,233,210,217,153,212},
};

static int32_t warnockOpt[]={1,  2,  2,  5,  3,  7,  3,  10,  18, 11, 
                         17, 5, 17,  26, 40, 14, 40, 44, 12, 31,
                         45, 70,8,   38, 82, 8,  12, 38, 47, 70,
                         29, 57, 97, 110,32, 48, 84, 124,155,26,
                         69, 83, 157,171, 8, 22, 112,205, 15, 31,
                         61, 105,127,212,12, 57, 109,133,179,210,
                         231,34, 161,199,222,255,59, 120,218,237,
                         278,341,54, 110,176,218,280,369,17, 97, 
                         193,221,331,350,419,21, 85, 173,221,243,
                         288,424,45, 78, 173,213,288,426,455,138,
};
static double *quasi=NULL;

int32_t power(int32_t a, int32_t b, int32_t m)
{ int32_t i,c=1;
  for(i=0;i<b;i++)
    c=(c*a)%m;
  return c;
}

int32_t primes()
{
  int32_t i, j, a[MAX_D+1];
  for (a[1] = 0, i = 2; i <= MAX_D; i++)
    a[i] = 1;
  for (i = 2; i <= MAX_D/2; i++)
    for (j = 2; j <= MAX_D/i; j++)
      a[i*j] = 0;
  for (i = 1, j = 0; i <= MAX_D; i++){
    if (a[i]){
      prime[j] =i; iprime[j]=(double)i;
      j++;
    }
  }   
  return j;
}

int inhalt(int dimen, int atmost, double tiny, double *quasi)
{
  double delta;
  int i,m;

  /* check dimen */
  sDim=dimen;
  if (sDim<1||sDim>1000)
    return(-1);

  /* compute and check tolerance*/

  eError=0.9*(1.0/(atmost*prime[sDim-1])-10.0*tiny);
  delta=100*tiny*(double)(atmost+1)*log10((double)atmost);
  if (delta>=0.09*(eError-10.0*tiny))
    return(-2);

  /* now compute first vector */
 
  m=1;
  for (i=0;i<sDim;i++)
    {	    
      iprime[i]=1.0/prime[i];
      quasi[i]=iprime[i]; 
      m=i*prime[i];
      nextPoint[i] = 2;
    }
  
  /*printf("largest prime=%d, %d \n",prime[sDim-1], m);*/
  
  return 0;
}

/*radix is not used here */
int32_t startModHaltonSequence(int32_t *radix, double tiny)
{
  int32_t  modID, dimen=12, total_points=100000;
  tiny = 0;
  /*  check dimen*/
  if (!modSequenceInUse) {
    /*generate primes
      primes(); */
    /* commented out primes generation and put it as global constants to skip generate it every time when start the sequence*/
    if (!quasi)
      quasi = malloc(sizeof(*quasi)*sDim);
    if (inhalt(dimen, total_points, tiny, quasi)<0) {
      fprintf(stderr, "Unable to start modHalton sequence.\n");
      return -1;
    }
  }
  modID = modSequenceInUse;
  *radix = prime[modID];
  modSequenceInUse++;
  return modID+1;
}

int32_t restartModHaltonSequence(long ID, double tiny)
{
  int32_t  dimen=12, total_points=100000;

  tiny = 0;
  if (inhalt(dimen, total_points, tiny, quasi)<0) {
    fprintf(stderr, "Unable to start modHalton sequence.\n");
    return -1;
  }

  return 1;
}

int32_t generateModHaltSequence(double *quasi, double *dq, double *wq, long ID)
{
  int32_t i= (int32_t) ID, j, k, ytemp[40],xtemp[40], ltemp, mtemp;
  double t,f,g,h;
  /* generate quasi one compoment at a time using radix prime[k] for 
     component k */

    t=iprime[i];
    f=1.0-quasi[i];
    g=1.0;
    h=t;
    while ((f-h)<eError)
      /* this checks whether q+h>1-eError */
      {
        g=h;
        h*=t;
      }
    quasi[i]=g+h-f;
  
    k=0; mtemp=nextPoint[i]; 
    ltemp=prime[i]; 

    while(mtemp!=0){
      ytemp[k]=mtemp%ltemp;
      mtemp=mtemp/ltemp;
      k++; 
    }
    /*generating Optimal primitive root  */
    for(j=0;j<k;j++) {
      /* xtemp[j] = (ytemp[j]*power(primroots[i/10][i%10], nextn%ltemp, ltemp))%ltemp; */
      xtemp[j] =(warnockOpt[i]*power(primroots[i/10][i%10], ytemp[j], ltemp))%ltemp;
      xtemp[j] -= ytemp[j];
    }
    dq[i]=0;t=iprime[i]; 
    for(j=0;j<k; j++)  { 
      dq[i] += xtemp[j]*t;
      t *= iprime[i];
    }
    dq[i] += quasi[i];
    /* generating Warnock Optimal sequences */
    for(j=0;j<k;j++)  {      
      /*  if( i%2==0)xtemp[j]= (ytemp[j]*warnockOpt[i])%ltemp;
          else xtemp[j]= (ytemp[j]*warnockOpt[i]*warnockOpt[i])%ltemp;
      */ 
      xtemp[j]= (ytemp[j]*power(warnockOpt[i],i+1,ltemp))%ltemp;
      /* if(i>=3)
         xtemp[j]=((prime[i-1]-1)*power(warnockOpt[i],ytemp[j],ltemp))%ltemp;
	 else  xtemp[j]= (power(warnockOpt[i],ytemp[j],ltemp))%ltemp;
      */ 
      xtemp[j] -= ytemp[j];
    }

    wq[i]=0;t=iprime[i];
    for(j=0;j<k; j++) {
      wq[i] += xtemp[j]*t;
      t *= iprime[i];
    }
    wq[i] += quasi[i];

  nextPoint[i]++;
 
  return(0);
}

double nextModHaltonSequencePoint(long ID) {
  static double *dq=NULL, *wq=NULL;

  ID -= 1;
  if (dq==NULL)
    dq = malloc(sizeof(*dq)*sDim);
  if (wq==NULL)
    wq = malloc(sizeof(*wq)*sDim);
  if (!modSequenceInUse) {
    fprintf(stderr, "ModHalton sequence not started.\n");
    exit(1);
  }
  if (ID<0 || ID>sDim-1) {
    fprintf(stderr, "Invalid ID (%ld) provided\n", ID);
    exit(1);
  }
  generateModHaltSequence(quasi, dq, wq, ID);

  return wq[ID];
}
