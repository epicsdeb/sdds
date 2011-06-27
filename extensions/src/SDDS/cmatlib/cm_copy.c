/* routine: cm_copy()
 * purpose: copy a matrix
 * usage:
 *   CMATRIX *A, *B;
 *   ...
 *   cm_copy(A, B);	! copy B to A 
 * Michael Borland, 1986 and Louis Emery, 1992.
 */
#include "cmatlib.h"

int cm_copy(CMATRIX *A, CMATRIX *B)
{
  register int i, j, a_m, a_n;
  register double *a_i, *b_i;
  
  if ((a_n=A->n)!=B->n || (a_m=A->m)!=B->m) 
    return(0);
  for (i=0; i<a_n; i++) {
    a_i = (A->ar)[i];
    b_i = (B->ar)[i];
    for (j=0; j<a_m; j++)
      a_i[j] = b_i[j];
    a_i = (A->ai)[i];
    b_i = (B->ai)[i];
    for (j=0; j<a_m; j++)
      a_i[j] = b_i[j];
  }
  return(1);
}
