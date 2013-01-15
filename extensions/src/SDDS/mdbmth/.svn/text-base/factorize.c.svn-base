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
 Revision 1.2  1995/09/05 21:19:41  saunders
 First test release of the SDDS1.5 package.

*/

/* file: factorize.c
 * routines: is_prime, smallest_factor
 *
 * Michael Borland, 1995
 */
#include "mdb.h"

long is_prime(long number)
/* returns 1 if number is prime
   otherwise returns -(smallest-prime-factor)
   */
{
    long i, n;

    n = sqrt(number*1.0)+1;
    if (n*n>number)
        n--;
    for (i=2; i<=n; i++)
        if (number%i==0)
            return(-i);
    return(1);
    }


long smallest_factor(long number)
/* returns smallest factor of number */
{
    long i;

    if (number==1)
        return(1);
    if ((i=is_prime(number))==1)
        return(number);
    return(-i);
    }

long next_prime_factor(long *number)
{
    long factor;
    if ((factor=smallest_factor(*number))>1) {
        *number /= factor;
        while (*number%factor==0)
            *number /= factor;
        return factor;
        }
    return 1;
    }

long largest_prime_factor(long number)
{
    long factor, lastFactor;
    lastFactor = 1;
    while ((factor=next_prime_factor(&number))>1)
        lastFactor = factor;
    return lastFactor;
    }

