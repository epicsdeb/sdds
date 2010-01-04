/* routine: cm_error()
 * purpose: report an error condition from a matrix routine
 * usage:
 * if (cm_xxxx(...)!=1)
 *   cm_error("...");	! exits from program 
 *
 * Michael Borland, 1986.
 */
#include "mdb.h"
#include "cmatlib.h"

void cm_error(char *s)
{
  printf("Complex matrix error: %s\n", s);
  exit(1);
}
