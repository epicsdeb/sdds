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
 Revision 1.5  2004/07/16 16:28:10  shang
 added countLimit argument to despikeData function to avoid despiking if
 the number of readings that exceeds the threshold is greater than the
 countLimit

 Revision 1.4  2002/08/14 16:19:00  soliday
 Added Open License

 Revision 1.3  1999/05/28 15:00:32  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1996/10/07 17:28:24  borland
 despikeData now returns a long integer giving the number of spikes removed
 on the final pass.

 Revision 1.1  1996/10/04 18:35:57  borland
 First version. Routines formerly in sddssmooth.c

 *
* Michael Borland, 1996.
*/
#include "mdb.h"



void smoothData(double *data, long rows, long smoothPoints, long smoothPasses)
{
    long lower, upper, row, pass, smoothPoints2, terms;
    double sum;
    static double *smoothedData = NULL;

    smoothedData = trealloc(smoothedData, rows*sizeof(*smoothedData));

    smoothPoints2 = smoothPoints/2;

    for (pass=0; pass<smoothPasses; pass++) {
        for (row=sum=0; row<smoothPoints2; row++) 
            sum += data[row];
        
        terms = row;
        lower = -smoothPoints2;
        upper = smoothPoints2;
        for (row=0; row<rows; row++, lower++, upper++) {
            if (upper<rows) {
                sum += data[upper];
                terms += 1;
                }
            smoothedData[row] = sum/terms;
            if (lower>=0) {
                sum -= data[lower];
                terms -= 1;
                }
            }
        
        for (row=0; row<rows; row++) 
            data[row] = smoothedData[row];
        
        }
    }

long despikeData(double *data, long rows, long neighbors, long passes, long averageOf,
                 double threshold, long countLimit)
{
  long i0, i1, i2, i, j, imin, imax, i1a, i2a;
  double *deltaSum, sum, *tempdata;
  long despikeCount;
  
  neighbors = 2*((neighbors+1)/2);
  if (!(tempdata = (double*)malloc(sizeof(*tempdata)*rows)) ||
      !(deltaSum = (double*)malloc(sizeof(*deltaSum)*(neighbors+1))))
    bomb("despikeData: memory allocation failure", NULL);
  memcpy(tempdata, data, sizeof(*data)*rows);
  despikeCount = 0;
  while (passes-- > 0) {
    despikeCount = 0;
    for (i0=0; i0<rows; i0+=neighbors/2) {
      i1 = i0-neighbors/2;
      i2 = i0+neighbors/2;
      if (i1<0)
        i1 = 0;
      if (i2>=rows)
        i2 = rows-1;
      if (i2-i1==0)
        continue;
      for (i=i1; i<=i2; i++) {
        deltaSum[i-i1] = 0;
        for (j=i1; j<=i2; j++)
          deltaSum[i-i1] += fabs(tempdata[i]-tempdata[j]);
      }
      if (index_min_max(&imin, &imax, deltaSum, i2-i1+1)) {
        if ((imax += i1)<0 || imax>rows) {
          fprintf(stderr, "Error: index out of range in despikeData (sddssmooth)\n");
          fprintf(stderr, "  imax = %ld, rows=%ld, i1=%ld, i2=%ld, neighbors=%ld\n",
                  imax-1, rows, i1, i2, neighbors);
          exit(1);
        }
        if (threshold==0 || threshold*neighbors<deltaSum[imax-i1]) {
          if ((i1a = imax-averageOf/2)<0)
            i1a = 0;
          if ((i2a = imax+averageOf/2)>=rows)
            i2a = rows-1;
          for (sum=0, i=i1a; i<=i2a; i++) {
            if (i!=imax)
              sum += tempdata[i];
          }
          despikeCount++;
          tempdata[imax] = sum/(i2a-i1a);
        }
      }
    }
    if (!despikeCount || (countLimit>0 && despikeCount>countLimit))
      break;
  }
  if (countLimit<=0 || despikeCount<countLimit)
    for (i=0;i<rows;i++)
      data[i] = tempdata[i];
  else
    despikeCount = 0;
  free(tempdata);
  free(deltaSum);
  return despikeCount;
}
