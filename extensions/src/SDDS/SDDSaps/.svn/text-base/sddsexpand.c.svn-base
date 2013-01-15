/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsexpand
 * purpose: take a SDDS file and create a new file with the
 *          columns turned into parameters. Each page of the
 *          new file corresponds to a row of the old file.
 *          This is basically an inverse of sddscollapse.
 *
 * Michael Borland, 1999
 *
 $Log: not supported by cvs2svn $
 Revision 1.9  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.8  2002/08/14 17:12:45  soliday
 Added Open License

 Revision 1.7  2002/01/16 18:02:18  soliday
 Added missing SDDS_Terminate and free functions.

 Revision 1.6  2001/01/23 19:47:28  soliday
 Fixed Solaris compiler warnings.

 Revision 1.5  1999/05/25 19:09:37  soliday
 Removed compiler warning on linux.

 Revision 1.4  1999/03/11 14:42:04  soliday
 Fixed warning message.

 Revision 1.3  1999/01/06 20:28:41  borland
 Fixed the program name in the processFilenames call.

 Revision 1.2  1999/01/06 19:31:41  borland
 Now copies parameter data as well as column data from input to output.

 Revision 1.1  1999/01/06 19:04:56  borland
 First version.

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

char *USAGE="sddsexpand [-pipe=[input][,output]] [<SDDSinputfile>] [<SDDSoutputfile>]\n\
[-noWarnings]\n\n\
sddsexpand is the partial inverse of sddscollapse.  All columns of the input file \
are turned into parameters in the output file.  For each row of each page in \
the input file, sddsexpand emits a new page with parameter values equal to the \
column values for that page and row.\n\n\
Program by Michael Borland.  (This is version 1, January 1999.)\n";

#define ROW_INCREMENT 100

typedef struct {
  char *name;
  long size;
  long index;  /* index in the target data set */
  short doCopy;
} SOURCE_DATA;

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output;
  char *inputfile, *outputfile;
  long rows, irow, i, i_arg, noWarnings;
  SCANNED_ARG *s_arg;
  unsigned long pipeFlags;
  void **data;
  SOURCE_DATA *columnSource, *parameterSource;
  int32_t columnSources, parameterSources;
  char **name;
  char buffer[32];
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
    bomb(NULL, USAGE);

  inputfile = outputfile = NULL;
  pipeFlags = noWarnings = 0;
  data = NULL;
  columnSource = parameterSource = NULL;

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
  
  processFilenames("sddsexpand", &inputfile, &outputfile, pipeFlags, noWarnings, NULL);

  if (!SDDS_InitializeInput(&SDDS_input, inputfile) || 
      !SDDS_InitializeOutput(&SDDS_output, SDDS_BINARY, 1, NULL, NULL, outputfile)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(name = SDDS_GetColumnNames(&SDDS_input, &columnSources)) || 
      !(columnSource = SDDS_Malloc(sizeof(*columnSource)*columnSources)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<columnSources; i++) {
    columnSource[i].name = name[i];
    if (!SDDS_DefineParameterLikeColumn(&SDDS_output, &SDDS_input, columnSource[i].name, NULL) ||
        (columnSource[i].index = SDDS_GetParameterIndex(&SDDS_output, columnSource[i].name))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    columnSource[i].size = SDDS_GetTypeSize(SDDS_GetParameterType(&SDDS_output, columnSource[i].index));
  }

  if (!(name = SDDS_GetParameterNames(&SDDS_input, &parameterSources)) || 
      !(parameterSource = SDDS_Malloc(sizeof(*parameterSource)*parameterSources)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<parameterSources; i++) {
    parameterSource[i].name = name[i];
    if (SDDS_GetParameterIndex(&SDDS_output, parameterSource[i].name)>=0) {
      if (!noWarnings)
        fprintf(stderr, "Warning (sddsexpand): name %s used for parameter and column in input file.  Column data used.\n", parameterSource[i].name);
      parameterSource[i].doCopy = 0;
      continue;
    }
    parameterSource[i].doCopy = 1;
    if (!SDDS_TransferParameterDefinition(&SDDS_output, &SDDS_input, 
                                          parameterSource[i].name, NULL) ||
        (parameterSource[i].index = SDDS_GetParameterIndex(&SDDS_output, parameterSource[i].name))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    parameterSource[i].size = SDDS_GetTypeSize(SDDS_GetParameterType(&SDDS_output, parameterSource[i].index));
  }

  if (!SDDS_WriteLayout(&SDDS_output) ||
      !(data=SDDS_Malloc(sizeof(*data)*columnSources)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
  while (SDDS_ReadPage(&SDDS_input)>0) {
    if ((rows=SDDS_RowCount(&SDDS_input))<0)
      continue;
    for (i=0; i<columnSources; i++) 
      if (!(data[i] = SDDS_GetInternalColumn(&SDDS_input, columnSource[i].name)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (irow=0; irow<rows; irow++) {
      if (!SDDS_StartPage(&SDDS_output, 0)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<parameterSources; i++) {
        if (!parameterSource[i].doCopy)
          continue;
        if (!SDDS_GetParameter(&SDDS_input, parameterSource[i].name, buffer) ||
            !SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, 
                                parameterSource[i].index, buffer, -1)) 
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      for (i=0; i<columnSources; i++) {
        if (!SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,
                                columnSource[i].index, (((char*)data[i])+irow*columnSource[i].size), -1))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!SDDS_WritePage(&SDDS_output)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (columnSource) free(columnSource);
  if (parameterSource) free(parameterSource);
  return(0);
}




