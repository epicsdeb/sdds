/* routine: cm_mult()
 * purpose: multiply two matrices
 * usage  : cm_mult(C, A, B)  ==>  C= AB
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

int cm_mult( CMATRIX *C, CMATRIX *A, CMATRIX *B )
{
  register long i, j, k;
  long n, m, p;
  register double complex *a_i, *c_i, c_i_j;

  if ((m=A->m)!=B->n || (n=A->n)!=C->n || (p=B->m)!=C->m) 
    return(0);
  for (i=0; i<n; i++) {
    a_i = A->a[i];
    c_i = C->a[i];
    for (j=0; j<p; j++) {
      c_i_j = 0;
      for (k=0; k<m; k++) 
        c_i_j += a_i[k] * B->a[k][j];
      c_i[j] = c_i_j;
    }
  }
  return(1);
}
