/* routine: cm_show()
 * purpose: to display a matrix on the screen 
 * usage:
 *   CMATRIX *A;
 *   ...
 *   cm_show(A, "%.4f  ", "label", stdout);   ! print w/a certain format 
 *   
 * Michael Borland, 1986.
 */
#include "mdb.h"
#include "cmatlib.h"

void cm_show(
             CMATRIX *A,
             char *format,
             char *label,
             FILE *fp
             )
{
  register long i, j;

  if (label)
    fputs(label, fp);
  for (i=0; i<A->n; i++) {
    for (j=0; j<A->m; j++)
      fprintf(fp, format, A->ar[i][j], A->ai[i][j]);
    fputc('\n', fp);
  }
}
