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
  Revision 1.2  2009/06/04 15:41:38  soliday
  Updated to fix issue on solaris-x86_64.
  
  Revision 1.1  2009/04/21 20:32:29  shang
  converted from netlib.org Romberg integration
  
  converted by shang from http://netlib.org/toms/351 

*/
#include "mdb.h"

double qromb(double (*func)(), /* pointer to function*/
             long maxe, 
              double a, double b, /* upper, lower limits of integeration*/
              double eps /*error */)
{
  double T, R, BB, S, H, S1, S0, err, result;
  static long MAXE=0;
  static double *RM=NULL;
  long K, K1, N, N0, N1, KK, KKK, K0, J, L, K2;
  
  T = (b-a)*(func(a)+func(b))*.5; /* initial trapzoid rule */
  if (RM==NULL || maxe>MAXE) {
    if (RM) free(RM);
    RM = malloc(sizeof(*RM)*(maxe+2));
    MAXE = maxe;
  }
  RM[1] = (b-a)*func((a+b)*0.5);
  N = 2;
  R = 4.0;
  
  for (K=1; K<=maxe; K++) {
    BB = ( R * 0.5 - 1.0 ) / ( R - 1.0);
    /*improved trapzoid value */
    T = RM[1] + BB * ( T - RM[1] );
    /* double number of subdivision of a and b */
    N = 2 * N;
    S = 0;
    H = (b-a)/(N*1.0);
    /* calculate rectangle value */
    if (N<=32) {
      N0 = N1 = N;
    } else if (N<=512) {
      N0 = 32;
      N1 = N;
    } else {
      N0 = 32;
      N1 = 512;
    }
    for (K2=1;K2<=N;K2+=512) {
      S1 = 0.0;
      KK = K2 + N1 - 1;
      for (K1=K2; K1<=KK; K1+=32) {
        S0 = 0.0;
        KKK = K1 + N0 - 1;
        for (K0=K1; K0<=KKK; K0+=2) 
          S0 += func(a + K0*H);
        S1 += S0;
      }
      S += S1;
    }
    RM[K+1] = 2.0 * H * S;
    /* end of calculate rectangle value */
    R = 4.0;
    for (J=1; J<=K; J++) {
      L = K + 1 - J;
      RM[L] = RM[L+1] + ( RM[L+1] - RM[L] ) / ( R - 1.0 );
      R = 4.0 * R;
    }
    err = fabs(T - RM[1]) * 0.5;
    if (err<=eps) {
      K = 0;
      result = (T + RM[1])*0.5;
      N = N+1;
      return result;
    }
  }
  fprintf(stderr, "too many qrom steps\n");
  return 0;
}
