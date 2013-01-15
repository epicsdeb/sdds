/* routine: cm_scmul()
 * purpose: multiply a complex matrix by a scalar
 * usage  : scmul(B, A, a)  ==>  B=aA
 */
#include "cmatlib.h"

int cm_scmul(
             CMATRIX *B, CMATRIX *A,
             double a
             )
{
  register long i, j;
  long a_n, a_m;
  register double afast, *a_i, *b_i;

  afast = a;
  if ((a_n=A->n)!=B->n || (a_m=A->m)!=B->m)
    return(0);
  for (i=0; i<a_n; i++) {
    a_i = A->ar[i];
    b_i = B->ar[i];
    for (j=0; j<a_m; j++) 
      b_i[j] = afast*a_i[j];
    a_i = A->ai[i];
    b_i = B->ai[i];
    for (j=0; j<a_m; j++) 
      b_i[j] = afast*a_i[j];
  }
  return(1);
}
