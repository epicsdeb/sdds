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
 Revision 1.6  2003/12/03 05:44:19  borland
 find_min_max and index_min_max no longer require both results pointers to be
 non-NULL.

 Revision 1.5  2002/08/14 16:18:55  soliday
 Added Open License

 Revision 1.4  1996/05/09 15:09:52  borland
 Fixed bug in min_in_array.c

 * Revision 1.3  1996/02/17  19:23:03  borland
 * index_min_max now sets output variables to 0 to guarantee that they are
 * set to something sensible upon exit.
 *
 * Revision 1.2  1995/09/05  21:19:48  saunders
 * First test release of the SDDS1.5 package.
 *
*/

/* file: find_min_max
 * contents: find_min_max(), assign_min_max()
 *           find_min_max_2d(), find_min_max_2d_float(),
 *           find_min(), find_max(), max_in_array(), min_in_array()
 *
 * M.Borland, 1989, 1990.
 */

/* routine: find_min_max()
 * purpose: find the minimum and maximum values in a list of values
 *
 * Michael Borland, 1987
 */
#include "mdb.h"

int find_min_max(
    double *min, double *max, double *list,
    long n
    )
{
    register long i;
    register double lo, hi, val;

    if (!n || !list) 
        return(0);
    if (!min && !max)
      return 0;
    
    lo =  DBL_MAX;
    hi = -DBL_MAX;
    for (i=0; i<n; i++) {
        if ((val=list[i])<lo)
            lo = val;
        if (val>hi)
            hi = val;
        }
    if (min)
      *min = lo;
    if (max)
      *max = hi;
    return(1);
    }

int index_min_max(
    long *imin, long *imax, double *list, long n
    )
{
    register long i;
    register double lo, hi, val;
    long iMin, iMax;
    
    if (!n || !list) 
        return(0);
    if (!imin && !imax)
      return 0;

    lo =  DBL_MAX;
    hi = -DBL_MAX;
    iMin = iMax = 0;
    for (i=0; i<n; i++) {
        if ((val=list[i])<lo) {
            iMin = i;
            lo = val;
            }
        if (val>hi) {
            hi = val;
            iMax = i;
            }
        }
    if (imin)
      *imin = iMin;
    if (imax)
      *imax = iMax;
    
    return(1);
    }

int index_min_max_long(
    long *imin, long *imax, long *list, long n
    )
{
    register long i;
    register long lo, hi, val;
    long iMin, iMax;
    
    if (!n || !list) 
        return(0);
    if (!imin && !imax)
      return 0;

    lo =  LONG_MAX;
    hi = -LONG_MAX;
    iMin = iMax = 0;
    for (i=0; i<n; i++) {
        if ((val=list[i])<lo) {
            iMin = i;
            lo = val;
            }
        if (val>hi) {
            hi = val;
            iMax = i;
            }
        }
    if (imin)
      *imin = iMin;
    if (imax)
      *imax = iMax;
    
    return(1);
    }

/* routine: assign_min_max()
 * purpose: compare a value to running minimum and maximum values,
 *          and assign these values accordingly.
 */

int assign_min_max(double *min, double *max, double val)
{
    int flag = 0;

    if (!min || !max)
        return(0);
    flag |= 1;
    if (*min>val) {
        *min = val;
        flag |= 2;
        }
    if (*max<val) {
        *max = val;
        flag |= 4;
        }
    return(flag);
    }

/* routine: find_min_max_2d()
 * purpose: find minimum and maximum values in a 2-d array (array
 *          of pointers).
 */

int find_min_max_2d(double *min, double *max, double **value, 
        long n1, long n2)
{
    double data, rmin, rmax, *value_i1;
    long i1, i2;

    if (!n1 || !n2 || !min || !max || !value)
        return(0);

    rmin = DBL_MAX;
    rmax = -DBL_MAX;
    for (i1=0; i1<n1; i1++) {
        if (!(value_i1 = value[i1]))
            return(0);
        for (i2=0; i2<n2; i2++) {
            if ((data=value_i1[i2])>rmax)
                rmax = data;
            if (data<rmin)
                rmin = data;
            }
        }
    return(1);
    }

/* routine: find_min_max_2d_float()
 * purpose: find minimum and maximum values in a 2-d array (array
 *          of pointers) of floats.
 */

int find_min_max_2d_float(float *min, float *max, float **value,
    long n1, long n2
    )
{
    float data, rmin, rmax, *value_i1;
    long i1, i2;
    
    if (!n1 || !n2 || !min || !max || !value)
        return(0);

    rmin =  FLT_MAX;
    rmax = -FLT_MAX;
    for (i1=0; i1<n1; i1++) {
        if (!(value_i1 = value[i1]))
            return(0);
        for (i2=0; i2<n2; i2++) {
            if ((data=value_i1[i2])>rmax)
                rmax = data;
            if (data<rmin)
                rmin = data;
            }
        }
    *min = rmin;
    *max = rmax;
    return(1);
    }

int find_min(
    double *min, double *loc, double *c1, double *c2,
    long n
    )
{
    long i;
    double val;

    if (!n || !loc || !c1 || !c2)
        return(0);

    *min =  DBL_MAX;
    for (i=0; i<n; i++) {
        if ((val=c2[i])< *min) {
            *min = val;
            *loc = c1[i];
            }
        }
    return(1);
    }

int find_max(
    double *max, double *loc, double *c1, double *c2,
    long n
    )
{
    long i;
    double val;

    if (!n || !c1 || !c2 || !loc || !max)
        return(0);
    *max =  -DBL_MAX;
    for (i=0; i<n; i++) {
        if ((val=c2[i])> *max) {
            *max = val;
            *loc = c1[i];
            }
        }
    return(1);
    }


double max_in_array(double *array, long n)
{
    double max = -DBL_MAX;

    while (n--)
        if (array[n]>max)
            max = array[n];
    return(max);
    }

double min_in_array(double *array, long n)
{
    double min = DBL_MAX;

    while (n--)
        if (array[n]<min)
            min = array[n];
    return(min);
    }

