/*convert from Roger Dejus's gy.f */
/* 	REAL*8 FUNCTION GY(N,Y)
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 					 inf.
C  Function to calculate the integral = I  K5/3 (t) dt multiplied by y**n.
C					 y
C AUTHORS: 
C 
C  Roger J. Dejus
C 
C  The Advanced Photon Source
C  Experimental Facilities Division
C  Argonne National Laboratory
C 
C CREATION DATE: 
C 
C  17-DEC-1991
C 
C FUNCTION VALUE:
C 		inf.
C  GY = y**n x I  K5/3 (t) dt
C		y
C 
C FORMAL PARAMETERS:
C  
C  Input arguments:
C  N		Value of exponent
C  Y		Argument
C  
C DESIGN ISSUES:
C  
C  From: V. O. Kostroun, Nucl. Instr. and Methods, 172 (1980) 371.
C  Good for about six decimal figures for Z < 4 for DT = 0.5, and
C  and up to Z = 40 for DT = 0.1.  NY = 5/3.
C 
C KEYWORDS:
C
C  Modified Bessel function, Integration
C
C [optional_header_tags]...
C
C MODIFICATION HISTORY:
C
C        Date     | Name  | Description
C ----------------+-------+-----------------------------------------------------
C 29-JAN-1995     | RJD   | Based on routine by V. O. Kostroun. For general 
C		  |	  | value of NY, use function IKNY.
C ----------------+-------+----------------------------------------------------- */

#include "mdb.h"
#include <math.h>
#undef EPS
#define EPS 1.0e-8
#define NY 5.0/3.0

double gy(long n, double y)
{
  double p, sum, term, dt, gy;
  
  p = 1.0;
  sum = 0.0;
  dt = 0.1;
  term = exp(-y*cosh(p*dt)) * cosh(NY*p*dt) / cosh(p*dt);
  while (term > EPS*sum) {
    sum += term;
    p = p+1;
    term = exp(-y*cosh(p*dt)) * cosh(NY*p*dt) / cosh(p*dt);
  }
  gy = pow(y, n)*dt*(sum + 0.5*exp(-y));
  return gy;
}
