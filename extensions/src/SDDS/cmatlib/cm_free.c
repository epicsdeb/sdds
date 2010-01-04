/* routine: cm_free()
 * purpose: free storage allocated to a matrix 
 * usage  :
 *   CMATRIX *A;
 *   long n, m;
 *   cm_alloc(&A, n, m);
 *     ...
 *   cm_free(&A);
 *
 * Michael Borland, 1986.
 */
#include "cmatlib.h"
#include <malloc.h>

void cm_free(CMATRIX **A)
{
  register long i, n;

  n = (*A)->n;
  for (i=0; i<n; i++) {
    free((*A)->a[i]);
    (*A)->a[i] = NULL;
  }
  free((*A)->a);
  (*A)->a = NULL;
  free(*A);
  *A = NULL;
}
