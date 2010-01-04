/* routine: cm_det()
 * purpose: compute determinant of a matrix 
 * usage:
 *   CMATRIX *A;
 *   COMPLEX det;
 *   ...
 *   det = cm_det(A);
 *
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

#define TMP a_j

double complex cm_det(CMATRIX *D)
{
  register long i, j, k, n;
  register double complex *a_i, *a_j, a_i_i, det;
  CMATRIX *A;

  det = 1.;
  if ((n=D->n)!=D->m)
    return(0.);

  cm_alloc(&A, n, n);
  if (!cm_copy(A, D)) {
    cm_free(&A);
    return(0.);
  }
  
  /* first diagonalize the CMATRIX */
  for (i=0; i<(n-1); i++) {
    if (A->a[i][i]==0.) {
      for (j=0; j<n; j++) 
        if (A->a[j][i]!=0.)  
          break;
      if (j==n)
        return(0.);
      TMP     = A->a[i];
      A->a[i] = A->a[j];
      A->a[j] = TMP;
      det *= -1.0;
    }
    det *= A->a[i][i];
    a_i = A->a[i];
    a_i_i = a_i[i];
    for (j=i+1; j<n; j++) {
      a_j = A->a[j];
      for (k=i+1; k<n; k++) 
        a_j[k] = a_j[k] - a_i[k]*a_j[i]/a_i_i;
    }
  }
  det = det * A->a[i][i];
  cm_free(&A);
  return(det);
}
