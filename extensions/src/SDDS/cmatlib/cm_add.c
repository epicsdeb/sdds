/* routine: cm_add()
 * purpose: add two matrices
 * usage  : cm_add(C, A, B) ==> C = A+B
 * Michael Borland, 1986 and Louis Emery, 1992.
 */
#include "cmatlib.h"

int cm_add(CMATRIX *C, CMATRIX *A, CMATRIX *B)
{
  register int i, j;
  int a_n, a_m;
  register double complex *a_i, *b_i, *c_i;
  
  if ((a_n=A->n)!=B->n || B->n!=C->n || (a_m=A->m)!=B->m || B->m!=C->m)
    return(0);
  for (i=0; i<a_n; i++) {
    a_i = A->a[i];
    b_i = B->a[i];
    c_i = C->a[i];
    for (j=0; j<a_m; j++) 
      c_i[j] = a_i[j] + b_i[j];
  }
  return(1);
}

int cm_subtract(CMATRIX *C, CMATRIX *A, CMATRIX *B)
{
  register int i, j;
  int a_n, a_m;
  register double complex *a_i, *b_i, *c_i;
  
  if ((a_n=A->n)!=B->n || B->n!=C->n || (a_m=A->m)!=B->m || B->m!=C->m)
    return(0);
  for (i=0; i<a_n; i++) {
    a_i = A->a[i];
    b_i = B->a[i];
    c_i = C->a[i];
    for (j=0; j<a_m; j++) 
      c_i[j] = a_i[j] - b_i[j];
  }
  return(1);
}


