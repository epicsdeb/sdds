/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddscollapse
 * purpose: take a SDDS file and create a new file with the
 *          parameters turned into columns.  Each row of the
 *          new table is for a different table of the original
 *          file.  The columns of the original file are ignored.
 *
 * Michael Borland, 1994
 $Log: sddscollapse.c,v $
 Revision 1.15  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.14  2002/08/14 17:12:41  soliday
 Added Open License

 Revision 1.13  2002/01/16 18:39:20  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.12  2001/01/10 19:35:32  soliday
 Standardized usage message.

 Revision 1.11  1999/05/25 19:05:58  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/01/06 19:54:39  borland
 Fixed the version number in the usage message.

 Revision 1.9  1998/08/24 21:47:05  borland
 Removed the "<whatever> processed by sddscollapse" description text
 from the output.

 Revision 1.8  1997/10/29 21:25:19  borland
 Now uses SDDS_ReadPageParse with huge sparsing factor, so that only
 one row is ever read per page of an input file.  Reduces memory requires
 and increases speed.

 Revision 1.7  1997/09/15 15:34:34  borland
 Fixed bug in SDDS_SetRowValues call.  Was passing &buffer instead of
 buffer.  Can't see how it worked before...

 Revision 1.6  1996/10/30 18:08:19  borland
 Slight modification to check for pre-existing parameter.

 Revision 1.5  1996/10/12 17:31:22  borland
 Now detects pre-existence of PageNumber column in output and doesn't die
 when it can't create it for itself.  Leaves the data in the column alone.

 * Revision 1.4  1995/11/13  19:28:54  borland
 * Fixed bug that involved using inputfile pointer in sprintf() without
 * testing whether it was NULL or not.
 *
 * Revision 1.3  1995/09/06  14:56:13  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_PIPE 0
#define SET_NOWARNINGS 1
#define N_OPTIONS 2

char *option[N_OPTIONS] = {
  "pipe", "nowarnings",
} ;

char *USAGE="sddscollapse [<SDDSinputfile>] [<SDDSoutputfile>]\n\
[-pipe=[input][,output]]\n\
[-noWarnings]\n\n\
sddscollapse reads data pages from a SDDS file and writes a new SDDS file \
containing a single data page.  This data page contains the parameters, \
with each parameter forming a column of the tabular data.\n\n\
Program by Michael Borland.  (This is version 3, August 1998.)\n";

#define ROW_INCREMENT 100

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output;
  char *inputfile, *outputfile, **column;
  long i, i_arg;
  long page_number, allocated_rows, noWarnings, setPageNumber;
  int32_t columns;
  SCANNED_ARG *s_arg;
  char s[SDDS_MAXLINE];
  unsigned long pipeFlags;
  long buffer[16];

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
    bomb(NULL, USAGE);

  inputfile = outputfile = NULL;
  pipeFlags = noWarnings = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_NOWARNINGS:
	noWarnings = 1;
	break;
      default:
	fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
	exit(1);
	break;
      }
    }
    else {
      if (inputfile==NULL)
	inputfile = s_arg[i_arg].list[0];
      else if (outputfile==NULL)
	outputfile = s_arg[i_arg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }
    
#ifdef DEBUG
  fprintf(stderr, "About to process filename arguments\n");
#endif

  processFilenames("sddscollapse", &inputfile, &outputfile, pipeFlags, noWarnings, NULL);

#ifdef DEBUG
  fprintf(stderr, "About to initialize input\n");
#endif
  if (!SDDS_InitializeInput(&SDDS_input, inputfile)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

#ifdef DEBUG
  fprintf(stderr, "About to initialize output\n");
#endif
  if (!SDDS_InitializeOutput(&SDDS_output, SDDS_input.layout.data_mode.mode, 1, 
			     NULL, NULL, outputfile) ) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

#ifdef DEBUG
  fprintf(stderr, "About to get parameter names\n");
#endif
  if (!(column = SDDS_GetParameterNames(&SDDS_input, &columns))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
#ifdef DEBUG
  fprintf(stderr, "About to define parameters\n");
#endif
  for (i=0; i<columns; i++) {
    if (!SDDS_DefineColumnLikeParameter(&SDDS_output, &SDDS_input, column[i], NULL)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }

  sprintf(s, "corresponding page number of %s for this row", inputfile?inputfile:"stdin");
  if (SDDS_GetColumnIndex(&SDDS_output, "PageNumber")<0) {
    if (SDDS_DefineColumn(&SDDS_output, "PageNumber", NULL, NULL, s,
			  NULL, SDDS_LONG, 0)<0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    setPageNumber = 1;
  } else {
    setPageNumber = 0;
  }
    
  if (!SDDS_WriteLayout(&SDDS_output) || !SDDS_StartPage(&SDDS_output, allocated_rows=ROW_INCREMENT)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  while ((page_number=SDDS_ReadPageSparse(&SDDS_input, 0, LONG_MAX-1, 0))>0) {
#ifdef DEBUG
    fprintf(stderr, "working on page %ld\n", page_number);
#endif
    if (page_number>allocated_rows) {
      if (!SDDS_LengthenTable(&SDDS_output, ROW_INCREMENT)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
      allocated_rows += ROW_INCREMENT;
    }
    for (i=0; i<columns; i++) {
#ifdef DEBUG
      fprintf(stderr, "Getting param %s\n", column[i]);
#endif
      if (!SDDS_GetParameter(&SDDS_input, column[i], buffer)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
#ifdef DEBUG
      fprintf(stderr, "Setting column %s\n", column[i]);
#endif
      if (!SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, page_number-1,
			     column[i], buffer, NULL)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
#ifdef DEBUG 
      fprintf(stderr, "Done setting %s\n", column[i]);
#endif
    }
#ifdef DEBUG
    fprintf(stderr, "Setting page number\n");
#endif
    if (setPageNumber &&
	!SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, page_number-1,
			   "PageNumber", page_number, NULL)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
        
  if (!SDDS_WritePage(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (page_number==0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}
