/* routine: cm_trans()
 * purpose: transpose a matrix 
 * usage  : cm_trans(B, A) ==> B=TRANS(A)
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

int cm_trans(CMATRIX *B, CMATRIX *A)
{
  register long i, j;
  register double *a_i, a_m, a_n;
  
  if (A->m!=B->n || A->n!=B->m)
    return(0);
  a_m = A->m;
  a_n = A->n;
  for (i=0; i<a_n; i++) {
    a_i = A->ar[i];
    for (j=0; j<a_m; j++)
      B->ar[j][i] = a_i[j];
    a_i = A->ai[i];
    for (j=0; j<a_m; j++)
      B->ai[j][i] = a_i[j];
  }
  return(1);
}
