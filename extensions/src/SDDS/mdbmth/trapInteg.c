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
 Revision 1.4  2004/02/27 16:28:50  borland
 Added trapizoidIntegration1(), which gives the integral vs the independet variable.

 Revision 1.3  2002/08/14 16:19:01  soliday
 Added Open License

 Revision 1.2  2002/07/02 16:31:27  soliday
 Added missing include statement

 Revision 1.1  2002/06/18 13:43:22  borland
 Added trapInteg.c (trapazoidIntegration function).

*/
#include "mdb.h"

long trapazoidIntegration(double *x, double *y, long n, double *integral)
{
  double sum;
  long i;
  
  if (!x || !y || !integral || n<=1)
    return 0;
  sum = y[n-1]*x[n-1]-y[0]*x[0];
  for (i=0; i<n-1; i++)
    sum += y[i]*x[i+1]-y[i+1]*x[i];
  *integral = sum/2;
  return 1;
}

/* this version returns the integral as a function of x */
long trapazoidIntegration1(double *x, double *y, long n, double *integral)
{
  long i;
  
  if (!x || !y || !integral || n<=1)
    return 0;
  integral[0] = 0;
  for (i=1; i<n; i++)
    integral[i] = integral[i-1] + (y[i]+y[i-1])*(x[i]-x[i-1])/2;
  return 1;
}
