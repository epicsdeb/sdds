/* routine: cassign_me()
 * purpose: return element from a complex matrix.
 */
#include "cmatlib.h"

COMPLEX cassign_me(CMATRIX *A, int row, int col) {
  return(cassign(A->ar[row][col],A->ai[row][col]));
}
