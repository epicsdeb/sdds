/* routine: cm_adjoint()
 * purpose: adjointpose a matrix 
 * usage  : cm_adjoint(B, A) ==> B=ADJOINT(A)
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

int cm_adjoint(CMATRIX *B, CMATRIX *A)
{
  register long i, j, a_m, a_n;
  register double complex *a_i;
  
  if (A->m!=B->n || A->n!=B->m)
    return(0);
  a_m = A->m;
  a_n = A->n;
  for (i=0; i<a_n; i++) {
    a_i = A->a[i];
    for (j=0; j<a_m; j++) 
      B->a[j][i] = conj(a_i[j]);
  }
  return(1);
}
