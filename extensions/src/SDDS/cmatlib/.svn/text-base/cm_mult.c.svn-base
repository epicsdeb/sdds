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
  register double *ar_i, *ai_i, *cr_i, *ci_i, cr_i_j, ci_i_j;

  if ((m=A->m)!=B->n || (n=A->n)!=C->n || (p=B->m)!=C->m) 
    return(0);
  for (i=0; i<n; i++) {
    ar_i = A->ar[i];
    ai_i = A->ai[i];
    cr_i = C->ar[i];
    ci_i = C->ai[i];
    for (j=0; j<p; j++) {
      cr_i_j = 0;
      ci_i_j = 0;
      for (k=0; k<m; k++) {
        cr_i_j += ar_i[k]*B->ar[k][j] - ai_i[k]*B->ai[k][j];
        ci_i_j += ar_i[k]*B->ai[k][j] + ai_i[k]*B->ar[k][j];
      }
      cr_i[j] = cr_i_j;
      ci_i[j] = ci_i_j;
    }
  }
  return(1);
}
