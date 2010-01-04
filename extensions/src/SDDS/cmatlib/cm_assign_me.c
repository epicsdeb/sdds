/* routine: cassign_me()
 * purpose: return element from a complex matrix.
 */
#include "cmatlib.h"

double complex cassign_me(CMATRIX *A, int row, int col) {
  return A->a[row][col];
}
