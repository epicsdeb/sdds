/* routine: cm_identity()
 * purpose: set elements of a complex matrix to 1 if on the diagonal, zero otherwise.
 * usage:
 *  CMATRIX *A;
 *  ...
 *  cm_identity(A);
 *
 * Michael Borland, 1989.
 */
#include "cmatlib.h"

void cm_identity(CMATRIX *A)
{
  register long i, j, n, m;
  register double *a_i;

  n = A->n;
  m = A->m;
  for (i=0; i<n; i++) {
    a_i = A->ar[i];
    for (j=0; j<m; j++) 
      a_i[j] = (i==j?1:0);
    a_i = A->ai[i];
    for (j=0; j<m; j++) 
      a_i[j] = 0;
  }
}
