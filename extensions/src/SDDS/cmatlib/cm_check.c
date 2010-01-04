/* routine: cm_check()
 * purpose: check a matrix for proper structure
 * usage:
 *   CMATRIX *A;
 *   ...
 *   cm_check(A);
 * Michael Borland, 1986.
 */
#include "cmatlib.h"

int cm_check(CMATRIX *A)
{
  register int i, a_n;
  
  a_n = A->n;
  if (!(A->a))
    return(0);
  for (i=0; i<a_n; i++) {
    if (!(A->a)[i])
      return(0);
  }
  return(1);
}
