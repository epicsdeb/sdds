
#include <stdio.h>
#include "sddsTable.hh"

int myParse(FILE *fp);

main(int argc, char *argv[]) {
  sddsTable t;
  FILE *fp;
  long foo;

  if (!t.initializeInput(argv[1])) {
	(t.errorStack).SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
  }

  printf("input initialized\n");
  t.terminate();
  printf("done\n");
}
