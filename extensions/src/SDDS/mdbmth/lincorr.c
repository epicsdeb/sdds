/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: lincorr.c,v $
 Revision 1.10  2007/04/16 21:02:51  soliday
 Moved from mdbcommon after I removed SDDS calls.

 Revision 1.4  2007/04/16 18:41:04  soliday
 Removed SDDS call.

 Revision 1.3  2003/07/22 21:03:17  soliday
 IEEE functions are always used.

 Revision 1.2  2002/08/14 16:48:28  soliday
 Added Open License

 Revision 1.1  2000/04/11 16:22:20  soliday
 Moved here from mdbmth.

 Revision 1.8  1999/12/15 16:28:58  soliday
 Added SDDS.h include statement

 Revision 1.7  1999/07/16 14:10:54  borland
 Reconciled my version with WIN32-compatible version.  Don't think there
 were any real differences.

 Revision 1.6  1999/07/09 14:24:21  soliday
 Borland added shiftedLinearCorrelationCoefficient

 Revision 1.5  1999/07/08 22:14:40  soliday
 Added definition for isnan

 Revision 1.4  1998/08/26 14:53:45  borland
 Treatment of IEEE math function isinf is now uniform.  If on solaris
 and sunmath is missing, then modify mdb.h only.

 * Revision 1.3  1995/12/07  18:36:09  borland
 * Added point-by-point acceptance flags to linearCorrelationCoefficient().
 *
 * Revision 1.2  1995/09/05  21:20:15  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: lincorr.c
 * purpose: routines for computing linear correlation coefficient and
 *          its significance
 * M. Borland, 1994
 */
#include "mdb.h"

#if defined(_WIN32)
#define isnan(x) _isnan(x)
#endif

double linearCorrelationCoefficient(double *data1, double *data2, 
                                    short *accept1, short *accept2,
                                    long rows, long *count)
{
    double sum1[2]={0,0}, sum2[2]={0,0}, sum12=0;
    double d1, d2, r;
    long i;

    *count = 0;
    for (i=0; i<rows; i++) {
        if (isnan(data1[i]) || isnan(data2[i]) || isinf(data1[i]) || isinf(data2[i]))
            continue;
        if ((accept1 && !accept1[i]) || (accept2 && !accept2[i]))
            continue;
        *count += 1;
        sum1[0] += data1[i];
        sum1[1] += data1[i]*data1[i];
        sum2[0] += data2[i];
        sum2[1] += data2[i]*data2[i];
        sum12   += data1[i]*data2[i];
        }
    d1 = (*count)*sum1[1] - sum1[0]*sum1[0];
    d2 = (*count)*sum2[1] - sum2[0]*sum2[0];
    if (d1<=0 || d2<=0)
        return 0.0;
    if ((d1*=d2)<=0)
        return 0.0;
    r = ((*count)*sum12 - sum1[0]*sum2[0])/sqrt(d1);
    if (r<-1)
        r = -1;
    else if (r>1)
        r = 1;
    return r;
    }

double linearCorrelationSignificance(double r, long rows)
{
    if (rows<2)
        return 1.0;
    if ((r = fabs(r))>1)
        r = 1;
    return rSigLevel(r, rows-2);
    }

double shiftedLinearCorrelationCoefficient(double *data1, double *data2, 
                                    short *accept1, short *accept2,
                                    long rows, long *count, long shift)
{
    double sum1[2]={0,0}, sum2[2]={0,0}, sum12=0;
    double d1, d2, r;
    long i, i1, i2, is;

    if (shift>0) {
      i1 = shift;
      i2 = rows;
    }
    else {
      i1 = 0;
      i2 = rows + shift;
    }
    *count = 0;
    for (i=i1; i<i2; i++) {
      is = i-shift;
      if (is<0 || is>=rows) {
        fprintf(stderr, "shift limits set incorrectly\n");
        exit(1);
      }
      if (isnan(data1[i]) || isnan(data2[is]) || isinf(data1[i]) || isinf(data2[is]))
        continue;
      if ((accept1 && !accept1[i]) || (accept2 && !accept2[is]))
        continue;
      *count += 1;
      sum1[0] += data1[i];
      sum1[1] += data1[i]*data1[i];
      sum2[0] += data2[is];
      sum2[1] += data2[is]*data2[is];
      sum12   += data1[i]*data2[is];
    }
    if (!*count)
      return 0.0;
    d1 = (*count)*sum1[1] - sum1[0]*sum1[0];
    d2 = (*count)*sum2[1] - sum2[0]*sum2[0];
    if (d1<=0 || d2<=0)
      return 0.0;
    if ((d1*=d2)<=0)
      return 0.0;
    r = ((*count)*sum12 - sum1[0]*sum2[0])/sqrt(d1);
    if (r<-1)
      r = -1;
    else if (r>1)
      r = 1;
    return r;
  }

