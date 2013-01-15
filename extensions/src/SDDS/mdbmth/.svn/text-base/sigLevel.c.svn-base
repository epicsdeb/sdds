/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: sigLevel.c
 * purpose: routines for Student's t distribution, the F distribution,
 *          the linear-correlation coefficient distribution
 *
 * M. Borland, 1995.
 * Based on Abramowitz and Stegun, 26.5.8, 26.6 
 $Log: not supported by cvs2svn $
 Revision 1.13  2002/08/14 16:19:00  soliday
 Added Open License

 Revision 1.12  1999/12/15 16:28:08  soliday
 Added vxWorks support.

 Revision 1.11  1999/07/12 20:38:25  soliday
 Added call to fdlibm library for erfc function on WIN32

 Revision 1.10  1999/07/08 22:15:38  soliday
 Added definition for isnan and added "erfc function is not implemented
 on WIN32" error message

 Revision 1.9  1996/11/07 15:11:13  borland
 poisson significance level calculation now checks for result<0 and returns
 0 instead.

 Revision 1.8  1996/11/05 21:54:30  borland
 Fixed poisson distribution calcs for 0 actual events, hopefully for the
 last time...

 Revision 1.7  1996/11/05 00:21:15  borland
 Fixed conditional statements for error conditions.  Corrected behavior
 for 0 actual counts.

 Revision 1.6  1996/11/04 23:42:22  borland
 Fixed gaussian stats branch for poisson significance level.  Improved
 behavior for 0 or negative arguments.

 Revision 1.5  1996/11/04 23:03:57  borland
 poissonSigLevel now behaves well when the mean is large.  Switches to
 gaussian.

 Revision 1.4  1996/10/29 19:34:31  borland
 poisson significance level now returns 1 for 0 or fewer events.

 Revision 1.3  1996/10/22 18:47:50  borland
 Added poissonSigLevel().

 * Revision 1.2  1995/09/05  21:20:50  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"

#if defined(_WIN32)
#include "fdlibm.h"
#define isnan(x) _isnan(x)
#endif

#if defined(vxWorks) /* This may not be very compatible */
#include <private/mathP.h>
#define isnan(x) isNan(x)
#endif

double normSigLevel(double z0, long tails)
/* return the probability that z will exceed z0 */
{
    if (z0<0) 
        z0 = -z0;
    if (tails!=1 && tails!=2)
        return -1;
#if defined(vxWorks)
    fprintf(stderr,"erfc function is not implemented on vxWorks\n");
    exit(1);
#else
    return erfc(z0/SQRT2)/(tails==1?2:1);
#endif
    }

double ChiSqrSigLevel(double ChiSquared0, long nu)
/* return the probability that ChiSquared will exceed ChiSquared0 for nu degrees of freedom */
{
    if (ChiSquared0<0)
        return -1;
    return gammaQ(nu/2.0, ChiSquared0/2);
    }

double tTailSigLevel(double t0, long nu, long tails)
/* return the probability that |t| will exceed t0 for nu degrees of freedom */
{
    if (tails!=1 && tails!=2)
        return -1;
    return betaInc(nu/2.0, 0.5, nu/(nu+t0*t0))/(tails==1?2:1) ;
    }

double FSigLevel(double var1, double var2, long nu1, long nu2)
/* return the probability that F will exceed F0=Max(var1,var2)/Min(var1,var2) 
   for nu1 and nu2 degrees of freedom */
{
    if (var1<var2) {
        SWAP_DOUBLE(var1, var2);
        SWAP_LONG(nu1, nu2);
        }
    return betaInc(nu2/2.0, nu1/2.0, nu2/(nu2+nu1*var1/var2));
    }


double rSigLevel(double r0, long nu)
/* return the probability that the linear correlation coefficient will
   exceed |r0| for nu degrees of freedom */
{
    if (r0<0)
        r0 = -r0;
    if (nu<2)
        return -1;
    if (r0>=1)
        return 0;
    return tTailSigLevel(r0*sqrt(nu/(1-r0*r0)), nu, 2);
    }

double poissonSigLevel(long n, double n0)
/* return the probability that n or more events will be seen given
   a poisson distribution with n0 expected events
 */
{
  double sum, term, result=0;
  long i;
  if (n==0)
    return 1;
  if (n<0 || n0<=0) {
    return n0<n ? 0 : 1;
  }
  if (n0>200) {
    result = normSigLevel((n-n0)/sqrt(n0), 1);
    if (n<n0)
      return 1-result;
    return result;
  }
  if (!exp(-n0)) 
    return n0<n ? 0 : 1;
  sum = 1;
  term = 1;
  i = 0;
  while (++i<n) {
    term *= n0/i;
    sum += term;
  }
  result = 1-sum*exp(-n0);
  if (isnan(result)) {
    return n0<n ? 0 : 1;
  }
  if (result<0)
    return 0;
  return result;
}
