/* routine: cm_zero()
 * purpose: set elements of a complex matrix to zero.
 * usage:
 *  CMATRIX *A;
 *  ...
 *  cm_zero(A);
 *
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

void cm_zero(CMATRIX *A)
{
  register long i, j, n, m;
  register double *a_i;

  if (!A || !A->ar || !A->ai)
    cm_error("null CMATRIX passed to cm_zero()");
  n = A->n;
  m = A->m;
  for (i=0; i<n; i++) {
    if (!(a_i = A->ar[i]))    /* real part */
      cm_error("null pointer in CMATRIX passed to cm_zero()");
    for (j=0; j<m; j++) 
      a_i[j] = 0;
    if (!(a_i = A->ai[i]))    /* imaginary part */
      cm_error("null pointer in CMATRIX passed to cm_zero()");
    for (j=0; j<m; j++) 
      a_i[j] = 0;
  }
}
