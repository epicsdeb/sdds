/*************************************************************************\
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 * National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 * Operator of Los Alamos National Laboratory.
 * This file is distributed subject to a Software License Agreement found
 * in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddscheck
 * purpose: checks an SDDS file to make sure there are no problems
 *
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.10  2007/06/14 20:53:57  soliday
 Reindented code.

 Revision 1.9  2002/08/14 17:12:41  soliday
 Added Open License

 Revision 1.8  2001/01/29 17:07:23  soliday
 Standardized usage message.

 Revision 1.7  2001/01/23 19:47:27  soliday
 Fixed Solaris compiler warnings.

 Revision 1.6  1999/05/25 19:05:47  soliday
 Removed compiler warning on linux.

 Revision 1.5  1999/01/06 19:54:38  borland
 Fixed the version number in the usage message.

 * Revision 1.4  1995/10/05  22:51:56  borland
 * Changed exit(1) to exit(0) in all places to correct status return.
 *
 * Revision 1.3  1995/09/06  14:56:11  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define CLO_PRINTERRORS 0
#define N_OPTIONS 1

static char *option[N_OPTIONS] = {
  "printErrors",
};

char *USAGE = "sddscheck <filename> [-printErrors]\n\n\
This program allows you to determine whether an SDDS file has been\n\
corrupted.  It reads the entire file, and prints a message to stdout.\n\
  If the file is ok, \"ok\" is printed.\n\
  If the file has a problem, one of the following will be printed:\n\
     \"nonexistent\"  \n\
     \"badHeader\"    \n\
     \"corrupted\"    \n\
If -printErrors is given, error messages are delivered to stderr.\n\
Program by Michael Borland. (This is version 3, October 1995.)\n";


int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input;
  char *input;
  long i_arg, retval, printErrors;
  SCANNED_ARG *s_arg;

  SDDS_RegisterProgramName(argv[0]);

  argc = scanargs(&s_arg, argc, argv);
  if (argc < 2)
    bomb(NULL, USAGE);

  input = NULL;
  printErrors = 0;

  for (i_arg = 1; i_arg < argc; i_arg++) {
    if (s_arg[i_arg].arg_type == OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case CLO_PRINTERRORS:
        printErrors = 1;
        break;
      default:
        SDDS_Bomb("unknown option given");
        break;
      }
    }
    else {
      if (input == NULL)
        input = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames");
    }
  }

  if (!fexists(input)) {
    puts("nonexistent");
    exit(0);
  }
  if (!SDDS_InitializeInput(&SDDS_input, input)) {
    puts("badHeader");
    if (printErrors)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(0);
  }

  while ((retval = SDDS_ReadPage(&SDDS_input)) > 0) {
  }

  if (retval == -1)
    /* EOF reached */
    puts("ok");
  else {
    if (printErrors)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    puts("corrupted");
  }
  return (0);
}
