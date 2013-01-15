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
 Revision 1.2  1995/09/05 21:19:29  saunders
 First test release of the SDDS1.5 package.

*/

/* routine: advance_values()
 * purpose: sequence an array of values in a systematic way such that
 *          a specified n-dimensional grid is covered.
 *
 * Michael Borland, 1988.
 */
#include "mdb.h"

long advance_values(double *value, long *value_index, double *initial, double *step, long n_values,
    long *counter, long *max_count, long n_indices
    )
{
    long i, counter_changed;

    if ((counter_changed=advance_counter(counter, max_count, n_indices))<0)
        return(-1);

    for (i=0; i<n_values; i++) 
        value[i] = initial[i] + counter[value_index[i]]*step[i];
    return(counter_changed);
    }

long advance_counter(long *counter, long *max_count, long n_indices)
{
    long i;

    for (i=0; i<n_indices; i++)
        if (counter[i]!=(max_count[i]-1))
            break;
    if (i==n_indices)
        return(-1);

    for (i=0; i<n_indices; i++) {
        if (counter[i]<(max_count[i]-1)) {
            counter[i]++;
            break;
            }
        else {
            counter[i] = 0;
            }
        }
    return(i);
    }



