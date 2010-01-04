/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: savitzkyGolay.c,v $
 Revision 1.10  2009/12/02 22:21:27  soliday
 Removed old complex.h header include statement.

 Revision 1.9  2007/10/01 20:38:02  ywang25
 fixed the bug for the cases when the derivative is not zero

 Revision 1.7  2007/09/05 14:38:10  ywang25
 The smoothing procedure will be done the time domain, which is more efficient than in the frequency domain, especially for order=1.

 Revision 1.6  2007/04/16 18:48:28  soliday
 Removed SDDS calls.

 Revision 1.5  2003/08/28 22:12:31  soliday
 Cleaned up the code.

 Revision 1.4  2002/08/14 16:48:28  soliday
 Added Open License

 Revision 1.3  2002/06/19 23:12:36  borland
 Fixed spelling of Savitzky-Golay.

 Revision 1.2  2000/08/09 22:15:32  soliday
 Commited the changes made my Borland.

 Revision 1.1  2000/04/11 16:23:45  soliday
 Moved here from mdbmth.

 Revision 1.2  1999/07/16 14:06:04  borland
 Added padding of the data prior to smoothing to avoid end effects.

 Revision 1.1  1999/06/21 20:39:27  borland
 First version.

* Michael Borland, 1999.
*/
#include "matlib.h"
#include "mdb.h"
#include "fftpackC.h"

long SavitzkyGolaySmooth(double *data, long rows,
                        long order, long nLeft, long nRight, long derivativeOrder)
{
  static double *FFTdata=NULL, *FFTfilter=NULL, *TMPdata=NULL;
  static long arraySize = 0, TMParraySize = 0;
  long i, nfreq, sizeNeeded;

  if (order<0) {
    fprintf(stderr, "order<0 (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if (nLeft<0) {
    fprintf(stderr, "nLeft<0 (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if (nRight<0) {
    fprintf(stderr, "nRight<0 (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if (derivativeOrder<0) {
    fprintf(stderr, "derivativeOrder<0 (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if (derivativeOrder>order) {
    fprintf(stderr, "derivativeOrder>order (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if ((nLeft+nRight)<order) {
    fprintf(stderr, "(nLeft+nRight)<order (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if ((nLeft+nRight)==0) {
    fprintf(stderr, "(nLeft+nRight)==0 (SavitzkyGolaySmooth)\n");
    return(0);
  }
  if (rows<(nLeft+nRight+1)) {
    fprintf(stderr, "rows<(nLeft+nRight+1) (SavitzkyGolaySmooth)\n");
    return(0);
  }

  if ((order==1) && (nLeft==nRight) && (derivativeOrder==0)) {  
    /* This is a special case of the filter. Sometimes called moving window averaging. */
    /* It requires that nLeft=nRight and this is the only option for elegant now */
    long np = nLeft+nRight+1;
    double scale = 1.0/np;

    sizeNeeded = rows;
    if (TMParraySize<sizeNeeded &&
	(!(TMPdata=realloc(TMPdata, sizeof(*TMPdata)*(TMParraySize=sizeNeeded)))) ) {
      fprintf(stderr, "Error: memory allocation failure (SavitzkyGolaySmooth)\n");
      exit(1);
    } 

    for (i=0; i<rows; i++) {
      data[i] = scale*data[i];
      TMPdata[i] = data[i];
    }

    /* Smooth the left side data with padding to eliminate end effects */
    for (i=1; i<=nRight; i++)
      data[0] += data[i];
    data[0] += nLeft*TMPdata[0];   
  
    for (i=1; i<=nLeft; i++) {
      data[i] = data[i-1]+data[i+nRight]-TMPdata[0];     
    } 

    /* Smooth the middle part of the array */ 
    for (i=nLeft+1; i<rows-nRight; i++) {
      data[i] = data[i-1]+data[i+nRight]-TMPdata[i-nLeft-1];
    }

    /* Smooth the right side data with padding to eliminate end effects */ 
    for (i=rows-nRight; i<rows; i++) {
      data[i] = data[i-1]+data[rows-1]-TMPdata[i-nLeft-1];
    }
     
    return (1); 
  }
  else { /* Smooth data in the time domain */ 
    long np = nLeft+nRight+1, j;
    static long coeffArraySize=0;
    static double *filterCoeff=NULL;

    sizeNeeded = rows+nLeft+nRight;
    if (TMParraySize<sizeNeeded &&
	(!(TMPdata=realloc(TMPdata, sizeof(*TMPdata)*(TMParraySize=sizeNeeded))))) {
      fprintf(stderr, "Error: memory allocation failure (SavitzkyGolaySmooth)\n");
      exit(1);
    } 
    if (coeffArraySize<np &&
	(!(filterCoeff=realloc(filterCoeff, sizeof(*filterCoeff)*(coeffArraySize=np))))) {
      fprintf(stderr, "Error: memory allocation failure (SavitzkyGolaySmooth)\n");
      exit(1);
    } 

    /* copy the data to a temporary array, with padding to eliminate end effects */
    for (i=0; i<rows; i++)
      TMPdata[i+nLeft] = data[i];
    for (i=0; i<nLeft; i++)
      TMPdata[i] = data[0];
    for (i=0; i<nRight; i++)
      TMPdata[rows+nLeft+i] = data[rows-1];

    /* store SG coefficients in wrap-around order  */
    SavitzkyGolayCoefficients(filterCoeff, np, order, nLeft, nRight, derivativeOrder, 1);

    for (i=0; i<rows; i++) {
      data[i] = data[i]*filterCoeff[0];
      for (j=1; j<=nLeft; j++)
        data[i] += TMPdata[i+nLeft-j]*filterCoeff[j]; 
      for (j=1; j<=nRight; j++)
	data[i] += TMPdata[i+nLeft+j]*filterCoeff[np-j];
    }
    return (1);   
  }

  /* Below are the old implementation in the frequency domain, which could be very time consuming */
  sizeNeeded = 2*(rows+1+nLeft+nRight);
  if (arraySize<sizeNeeded &&
      (!(FFTdata=realloc(FFTdata, sizeof(*FFTdata)*(arraySize=sizeNeeded))) ||
       !(FFTfilter=realloc(FFTfilter, sizeof(*FFTfilter)*(arraySize=sizeNeeded)))) ) {
    fprintf(stderr, "Error: memory allocation failure (SavitzkyGolaySmooth)\n");
    exit(1);
  }
  for (i=0; i<arraySize; i++)
    FFTdata[i] = FFTfilter[i] = 0;
  
  /* store SG coefficients in wrap-around order  */
  SavitzkyGolayCoefficients(FFTfilter, 2*(rows+nLeft+nRight), order, nLeft, nRight, derivativeOrder, 1); 

  /* copy the data to work array, with padding to eliminate end effects */
  for (i=0; i<rows; i++)
    FFTdata[i+nLeft] = data[i];
  for (i=0; i<nLeft; i++)
    FFTdata[i] = data[0];
  for (i=0; i<nRight; i++)
    FFTdata[rows+nLeft+i] = data[rows-1];
  
  /* Take FFTs of signal and filter */
  realFFT2(FFTdata, FFTdata, 2*(rows+nLeft+nRight), 0);
  realFFT2(FFTfilter, FFTfilter, 2*(rows+nLeft+nRight), 0);

  /* Multiply FFTs */
  nfreq = rows+nLeft+nRight+1;
  for (i=0; i<nfreq; i++) 
    complex_multiply(FFTdata+2*i, FFTdata+2*i+1,
                     FFTdata[2*i], FFTdata[2*i+1],
                     FFTfilter[2*i], FFTfilter[2*i+1]);

  /* Do inverse FFT */
  realFFT2(FFTdata, FFTdata, 2*(rows+nLeft+nRight), INVERSE_FFT);
  for (i=0; i<rows; i++)
    data[i] = FFTdata[i+nLeft]*2*(rows+nLeft+nRight);

  return(1);
}

typedef struct {
  double *coef;
  long order, right, left, derivOrder;
} SAVITZKYGOLAY_COEF;

static SAVITZKYGOLAY_COEF *svCoef=NULL;
static long nSVCoef = 0;

void SavitzkyGolayCoefficients(double *coef,
                              long maxCoefs,
                              long order,
                              long nLeft, long nRight,
                              long derivativeOrder, long wrapAround)
{
  MATRIX *A, *At, *AtA;
  long i, j, m, iStore;
  double factor;
  long iSave;
  
  if (!coef || order<0 || derivativeOrder<0 || derivativeOrder>order ||
      (nLeft+nRight)<order || nLeft<0 || nRight<0 || maxCoefs<(nLeft+nRight+1)) {
    fprintf(stderr, "Error: invalid arguments (savitzkyGolayCoefficients)\n");
    exit(1);
  }

  for (i=0; i<maxCoefs; i++)
    coef[i] = 0;

  /* see if these coefs are already stored.  if so, just copy them and return. */
  for (iSave=0; iSave<nSVCoef; iSave++) {
    if (order==svCoef[iSave].order &&
        nLeft==svCoef[iSave].left &&
        nRight==svCoef[iSave].right &&
        derivativeOrder==svCoef[iSave].derivOrder) {
      for (i=-nLeft; i<=nRight; i++) {
        if (wrapAround) {
          if (i<=0)
            iStore = -i; 
          else 
            iStore = maxCoefs-i;
        } else
          iStore = i+nLeft;
        coef[iStore] = svCoef[iSave].coef[i+nLeft];
      }
      return;
    }
  }

  m_alloc(&A, nLeft+nRight+1, order+1);
  m_alloc(&At, order+1, nLeft+nRight+1);
  m_alloc(&AtA, order+1, order+1);

  for (i= -nLeft; i<=nRight; i++) {
    factor = 1;
    for (j=0; j<=order; j++) {
      A->a[i+nLeft][j] = factor;
      factor *= i;
    }
  }
  
  if (!m_trans(At, A) || !m_mult(AtA, At, A) || !m_invert(AtA, AtA)) {
    fprintf(stderr, "Error: matrix manipulation problem (savitzkyGolayCoefficients)\n");
    exit(1);
  }

  if (!(svCoef = realloc(svCoef, sizeof(*svCoef)*(nSVCoef+1))) || 
      !(svCoef[nSVCoef].coef = malloc(sizeof(*svCoef[nSVCoef].coef)*(nRight+nLeft+1)))) {
    fprintf(stderr, "Error: memory allocation failure (savitzkyGolayCoefficients)\n");
    exit(1);
  }
  svCoef[nSVCoef].left = nLeft;
  svCoef[nSVCoef].right = nRight;
  svCoef[nSVCoef].derivOrder = derivativeOrder;
  svCoef[nSVCoef].order = order;
  
  for (i=-nLeft; i<=nRight; i++) {
    if (wrapAround) {
      if (i<=0)
        iStore = -i; 
      else 
        iStore = maxCoefs-i;
    } else
      iStore = i+nLeft;
    coef[iStore] = 0;
    factor = 1;
    for (m=0; m<=order; m++) {
      coef[iStore] += AtA->a[derivativeOrder][m]*factor;
      factor *= i;
      }
    svCoef[nSVCoef].coef[i+nLeft] = coef[iStore];
  }
  nSVCoef ++;
  m_free(&A);
  m_free(&At);
  m_free(&AtA);
}
