/* routine: cm_alloc()
 * purpose: allocate storage for a complex matrix
 * usage  : 
 *   CMATRIX *A;
 *   int n, m;
 *   cm_alloc(&A, n, m)
 * 
 * Michael Borland, 1986 and Louis Emery, 1992.
 */
#include "mdb.h"
#include "cmatlib.h"

void cm_alloc(CMATRIX **A, int n, int m)
{
  register long i;
  
  if (n<=0)
    bomb("error in cm_alloc: 0xn array requested", NULL);
  if (*A = (CMATRIX*)tmalloc(sizeof(**A))) {
    if ( ((*A)->ar = (double**)tmalloc(sizeof(double*)*n)) && ((*A)->ai = (double**)tmalloc(sizeof(double*)*n))) {
      (*A)->n = n;
      (*A)->m = m;
      if (m!=0) {
        /* m==0 means only row pointers were wanted */
        for (i=0; i<n; i++) {
          if (!((*A)->ar[i] = (double*)tmalloc(m*sizeof(double))))
            break;
          if (!((*A)->ai[i] = (double*)tmalloc(m*sizeof(double))))
            break;
        }
        if (i==n) {
          cm_zero(*A);
          return;
        }
      }
    }
  }
  puts("Allocation failure in cm_alloc().");
  abort();
}
