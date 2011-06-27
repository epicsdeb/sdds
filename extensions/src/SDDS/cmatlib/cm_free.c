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
    free((*A)->ar[i]);
    (*A)->ar[i] = NULL;
    free((*A)->ai[i]);
    (*A)->ai[i] = NULL;
  }
  free((*A)->ar);
  (*A)->ar = NULL;
  free((*A)->ai);
  (*A)->ai = NULL;
  free(*A);
  *A = NULL;
}
