#include "mdb.h"
#include "constants.h"
#include <math.h>
#define A_LIM 10.1
#define EPS1 1.0e-12
#define EPS2 1.0e-8
#define GAMMA_OF_NY 1.354117939426400463
#define NY 2.0/3.0

/*converted from Roger Dejus's fortran code k23.f */
/*it seems to be faster than our bessel functions */
  /*  C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C  Routine to calculate the Modified Bessel Function of Second Kind Kny(Z) for
C  ny = 2/3.
C 
C AUTHORS: 
C 
C  Roger J. Dejus
C 
C CREATION DATE: 
C 
C  28-OCT-1990
C 
C FUNCTION VALUE:
C 
C  Kny(Z) for ny = 2/3.
C 
C FORMAL PARAMETERS:
C  
C  Input arguments:
C  Z		Variable Z: 0.0 < Z <~ 60.0
C  
C DESIGN ISSUES:
C  
C  The algorithm is based on a series expansion for small arguments Z
C  (Abramowitz & Stegun Eq. 9.6.2 and 9.6.10) and an asymptotic expansion for
C  large arguments (Eq. 9.7.2).
C  Reference: Handbook of Mathematical Functions, Eds. Milton Abramowitz and
C  Irene A. Stegun, Ninth Printing, Dover Publications, New York (1970).
C  Average execution time is 0.12 ms on the ANLAPS (VAX 6510).
C  
C LINK/LIBRARY ISSUES:
C  
C  The Gamma function is needed. Uses the NAG Library routine S14AAF.  Uses
C  now the gammln routine from Numerical Recipes.
C 
C MODIFICATION HISTORY:
C 
C	 Date     | Name  | Description
C ----------------+-------+-----------------------------------------------------
C 14-MAY-1994     |  RJD  | Replaced call to NAG library routine S14AAF with   |
C                 |       | gammln from Numerical Recipes, 2nd ed.(1992),p207. |
C                 |       | Note: Value of "ny" and gamma(ny) is in single prec|
C 10-JUN-1994     |  RJD  | Removed call to gammln. Value for gamma(2/3) is    |
C                 |       | stored as a constant because of difficulty with    |
C                 |       | convergence for large arguments of Z (~9.3)        |
C 19-JAN-1995     |  RJD  | Modified value of gamma(2/3) to value from S14AAF  |
C                 |       | on DEC AXP alpha and Sun Sparcstation.             |
C ----------------+-------+-----------------------------------------------------
C [change_entry]
C-
C  NOTE: EPS2 cannot be made much smaller than 10**-8 (too small makes the
C  asymptotic expansion to diverge). EPS1 and EPS2 give the relative accuracy
C  in the two regions of expansions respectively. */

double k23(double z) {
  double c1, c2 , zs, ny_fac, neg_ny_fac, zm, zp, pm, pp, term, sum, ze, za, mu, pa;
  static double e2;
  long k;
  
  if (z<A_LIM) {
    /* series expansion*/
    c1 = PI/2.0/sin(PI*NY);
    c2 = 2*c1;
    zs = z*z/4.0;

    ny_fac     = NY*GAMMA_OF_NY; /* ! Factorial of (+ny)*/
    neg_ny_fac = c2/GAMMA_OF_NY; /*! Factorial of (-ny)*/
    e2 = pow(z/2.0, NY);
    zm = 1/e2/neg_ny_fac;
    zp = e2/ny_fac;
    /* first term */
    pm = pp = 1.0;
    sum = term = c1*(pm*zm-pp*zp);
    k=0;
    while (fabs(term) > EPS1*sum) {
      k    = k+1;
      pm   = pm*zs/(k*(k-NY));
      pp   = pp*zs/(k*(k+NY));
      term = c1*(pm*zm-pp*zp);
      sum  += term;
    }
  } else {
    ze   = sqrt(PI/2.0/z)*exp(-z);
    za   = 1.0/(8.0*z);
    mu   = 4.0*NY*NY;
    /* first term */
    pa = 1.0;
    sum = term = pa*ze;
    /* Additional terms */
    k = 0;
    while (fabs(term) > EPS2*sum) {
      k    = k+1;
      pa   = pa*za*(mu-(2*k-1)*(2*k-1))/k;
      term = pa*ze;
      sum  += term;
    }
  }
  return sum;
}
