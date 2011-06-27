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
#include "complex.h"

#define TMP ar_j

COMPLEX cm_det(CMATRIX *D)
{
  register long i, j, k, n;
  register double *ar_i, *ai_i, *ar_j, *ai_j, ar_i_i, ai_i_i;
  register COMPLEX det;
  CMATRIX *A;
  register COMPLEX  c;
  COMPLEX zero;

  zero.r = 0.0;
  zero.i = 0.0;
  det.r = 1.;
  det.i = 0.;
  if ((n=D->n)!=D->m)
    return(zero);

  cm_alloc(&A, n, n);
  if (!cm_copy(A, D))
    return(zero);
  
  /* first diagonalize the CMATRIX */
  for (i=0; i<(n-1); i++) {
    if (A->ar[i][i]==0. && A->ai[i][i]==0.) {
      /* find row with non-zero diagonal element */
      for (j=0; j<n; j++) 
        if (A->ar[j][j]!=0 || A->ai[j][j]!=0)  
          break;
      if (j==n)
        return(zero);
      TMP     = A->ar[i];
      A->ar[i] = A->ar[j];
      A->ar[j] = TMP;
      TMP     = A->ai[i];
      A->ai[i] = A->ai[j];
      A->ai[j] = TMP;
      det = cmulr(det, -1.0);
    }
    /*        det *= A->a[i][i];    */
    det = cmul( det, cassign(A->ar[i][i], A->ai[i][i]));
    ar_i = A->ar[i];
    ai_i = A->ai[i];
    ar_i_i = ar_i[i];
    ai_i_i = ai_i[i];
    for (j=i+1; j<n; j++) {
      ar_j = A->ar[j];
      ai_j = A->ai[j];
      for (k=i+1; k<n; k++) {
        /*                a_j[k] = a_j[k] - a_i[k]*a_j[i]/a_i_i;    */
        c = csub( cassign(ar_j[k], ai_j[k]), 
                 cdiv( cmul( cassign( ar_i[k], ai_i[k]), cassign(ar_j[i], ai_j[i])), cassign(ar_i_i, ai_i_i)));
        ar_j[k] = c.r;
        ai_j[k] = c.i;
      }
    }
  }
  return( cmul(det,cassign( A->ar[i][i], A->ai[i][i])));
}
