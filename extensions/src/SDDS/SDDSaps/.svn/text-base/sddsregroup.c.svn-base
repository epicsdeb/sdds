/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program sddsregroup.c
 * read a sddsfile with pages of equal number of rows and produce
 * an output file with reprouped pages.
 * If the input file has m pages of n rows each then
 * the output file had n pages of m rows each.
 * The parameters of the input file may become extra columns in the output file.
 * These extra columns will be duplicated in all pages.
 * On the command line, one may specify which columns of the input file
 * becomes parameters in the output file.
 $Log: not supported by cvs2svn $
 Revision 1.14  2002/08/14 17:12:52  soliday
 Added Open License

 Revision 1.13  2002/01/16 18:50:02  soliday
 Added missing SDDS_Terminate and free functions.

 Revision 1.12  2001/01/10 19:35:45  soliday
 Standardized usage message.

 Revision 1.11  1999/09/14 18:32:09  soliday
 init_stats is no longer defined locally

 Revision 1.10  1999/05/25 19:14:20  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/06 19:54:54  borland
 Fixed the version number in the usage message.

 Revision 1.8  1998/07/11 07:20:24  emery
 Changed error message for unequal numbers of rows in pages.

 Revision 1.7  1998/07/11 06:56:22  emery
 Make program exits when not all pages have the same number of rows.

 Revision 1.6  1997/01/31 22:56:51  emery
 Shortened some lines by inserting a <CR>.

 * Revision 1.5  1996/05/22  23:23:00  emery
 * indented the source code.
 *
 * Revision 1.4  1996/02/13  20:40:39  emery
 * Use "m" as the filemode parameter (used to be NULL) for InitializeCopy function,
 * made necessary by changes made to the InitializeCopy function.
 *
 * Revision 1.3  1995/09/06  14:57:03  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "matlib.h"
#include "SDDS.h"

#define CLO_NEWCOLUMNS 0
#define CLO_NEWPARAMETERS 1
#define CLO_WARNING 2
#define CLO_VERBOSE 3
#define CLO_PIPE 4
#define COMMANDLINE_OPTIONS 5
char *commandline_option[COMMANDLINE_OPTIONS]  =  {
  "newcolumns",
  "newparameters",
  "warning",
  "verbose",
  "pipe"
  };

static char *USAGE = "sddsregroup <inputfile> <outputfile> [-pipe=[input][,output]]\n\
 [-newparameters=<oldcolumnname>[,...]]\n\
 [-newcolumns=<oldparametername>[,...]] [-warning] [-verbose]\n\n\
Re-organizes the data in the input file by taking single rows from each page of the inputfile\n\
to form single pages of the output file.\n\
pipe                reads input from and/or writes output to a pipe.\n\
inputfile           sdds file with m pages of n rows each.\n\
outputfile          sdds file with n pages of m rows each.\n\
newparameters       specifies which columns of the input file will become\n\
                    parameters in the output file. By default no new parameters\n\
                    are created, and all columns of the input file are transfered\n\
                    to the output file.\n\
newcolumns          specifies which parameters of the input file will become\n\
                    columns in the output file. The columns will necessarily be\n\
                    duplicated in all pages. By default all parameters\n\
                    values are lost.\n\n\
Program by Louis Emery, ANL (This is version 2, July 1998.) \n";

int main(int argc, char **argv)
{
  SCANNED_ARG *s_arg;
  SDDS_TABLE inputPage, *inputPages, outputPage;
  
  char *inputfile, *outputfile;
  char *InputDescription, *InputContents;
  char *OutputDescription, *OutputContents;
  char **InputParameters, **InputColumns, **ColToParNames, **ParToColNames;
  char **OutputColumns;
  int32_t NInputParameters, NInputColumns;
  long NColToPar, NParToCol, NColToCol;
  long NOutputColumns;
  long NInputRows, NOutputRows, NInputPages, NOutputPages;
  long *ColToColInputIndex, *ColToParInputIndex, *ParToColInputIndex;
  long *ColToColOutputIndex, *ColToParOutputIndex, *ParToColOutputIndex;
  long pageIncrement = 20;
  
  long i, i_arg, j;
  long ipage, row;
  long verbose, warning;
  unsigned long pipeFlags;
  long tmpfile_used, noWarnings;
  inputPages = NULL;  

  SDDS_RegisterProgramName(argv[0]);
  argc  =  scanargs(&s_arg, argc, argv);
  if (argc == 1)
    bomb(NULL, USAGE);

  inputfile  =  outputfile  =  NULL;
  InputDescription = InputContents = NULL;
  OutputDescription = OutputContents = NULL;
  InputParameters = InputColumns = ColToParNames = ParToColNames = NULL;
  OutputColumns = NULL;
  NInputParameters = NInputColumns = NColToPar = NParToCol = NColToCol = 0;
  NOutputColumns = 0;
  NInputRows = NOutputRows = NInputPages = NOutputPages = 0;
  ColToColInputIndex = ColToParInputIndex = ParToColInputIndex = NULL;
  ColToColOutputIndex = ColToParOutputIndex = ParToColOutputIndex = NULL;
  
  verbose = 0;
  warning = 0;
  pipeFlags  =  0;
  tmpfile_used  =  0;
  noWarnings  =  0;
  for (i_arg = 1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type == OPTION) {
      switch(match_string(s_arg[i_arg].list[0], commandline_option, COMMANDLINE_OPTIONS, 
                          UNIQUE_MATCH)) {
      case CLO_VERBOSE:
        verbose = 1;
        break;
      case CLO_WARNING:
        warning = 1;
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_NEWCOLUMNS:
        NParToCol = s_arg[i_arg].n_items-1;
        if(!NParToCol) {
          SDDS_Bomb("No old parameter names given");
        }
        ParToColNames  =  (char**)malloc(NParToCol*sizeof(char*));
        for(i = 0;i<NParToCol;i++){
          ParToColNames[i] = s_arg[i_arg].list[i+1];
        }
        break;
      case CLO_NEWPARAMETERS:
        NColToPar = s_arg[i_arg].n_items-1;
        if(!NColToPar) {
          SDDS_Bomb("No old column names given");
        }
        ColToParNames  =  (char**)malloc(NColToPar*sizeof(char*));
        for(i = 0;i<NColToPar;i++){
          ColToParNames[i] = s_arg[i_arg].list[i+1];
        }
        break;
      default:
        SDDS_Bomb("unrecognized option given");
      }
    }
    else {
      if (!inputfile)
        inputfile  =  s_arg[i_arg].list[0];
      else if (!outputfile)
        outputfile  =  s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames given");
    }
  }

  processFilenames("sddsregroup",  &inputfile,  &outputfile,  pipeFlags,  noWarnings,  &tmpfile_used);

  if(!SDDS_InitializeInput(&inputPage, inputfile))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if(0<SDDS_ReadTable(&inputPage))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  InputColumns = (char**)SDDS_GetColumnNames(&inputPage, &NInputColumns);
  InputParameters = (char**)SDDS_GetParameterNames(&inputPage, &NInputParameters);
  InputDescription = InputContents = NULL;
  if(!SDDS_GetDescription(&inputPage, &InputDescription, &InputContents))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  NInputRows = SDDS_CountRowsOfInterest(&inputPage);
  ColToParInputIndex = (long*)malloc(NColToPar*sizeof(long));
  ColToParOutputIndex = (long*)malloc(NColToPar*sizeof(long));
  NColToCol = NInputColumns - NColToPar;
  ColToColInputIndex = (long*)malloc(NColToCol*sizeof(long));
  ColToColOutputIndex = (long*)malloc(NColToCol*sizeof(long));
  ParToColInputIndex = (long*)malloc(NParToCol*sizeof(long));
  ParToColOutputIndex = (long*)malloc(NParToCol*sizeof(long));

  
  /*******************************\
   * check existence of selected  *
   * columns and parameters       *
   \*******************************/
  for(i = 0;i<NColToPar;i++) {
    switch(SDDS_CheckColumn(&inputPage, ColToParNames[i], NULL, 0, NULL)) {
    case SDDS_CHECK_NONEXISTENT:
      fprintf(stderr,"Error: Input file doesn't contain column %s.\n", ColToParNames[i]);
      exit(1);
    }
  }
  for(i = 0;i<NParToCol;i++) {
    switch(SDDS_CheckParameter(&inputPage, ParToColNames[i], NULL, 0, NULL)) {
    case SDDS_CHECK_NONEXISTENT:
      fprintf(stderr,"Error: Input file doesn't contain parameter %s.\n", ParToColNames[i]);
      exit(1);
    }
  }

  /*****************************************\
   * make copies of pages of the input file *
   \*****************************************/
  NInputPages = 0;
  if ( verbose ) {
    init_stats();
  }
  
  do {
    if(!NInputPages) {
      inputPages = (SDDS_TABLE*)malloc(pageIncrement * sizeof(SDDS_TABLE));
    }
    else if ( !(NInputPages%pageIncrement) ) {
      inputPages = (SDDS_TABLE*) realloc(inputPages, (NInputPages + pageIncrement) * sizeof(SDDS_TABLE));
    }
    if(NInputRows != SDDS_CountRowsOfInterest(&inputPage)) {
      fprintf( stderr, "Number of rows in pages are not all equal.\n");
      exit(1);
    }
    if(!SDDS_InitializeCopy(&inputPages[NInputPages], &inputPage, NULL, "m"))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if(!SDDS_CopyTable(&inputPages[NInputPages], &inputPage))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (verbose) {
      fprintf( stderr, "Reading page  %ld...\n", NInputPages);
    }
    
    NInputPages++;
  } while(0<SDDS_ReadTable(&inputPage));

  if(!SDDS_Terminate(&inputPage))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

  if(InputDescription) {
    OutputDescription = (char*)malloc((strlen(InputDescription)+strlen(",  regrouped")+1)*sizeof(char));
    OutputDescription = strcat( strcpy(OutputDescription, InputDescription), ",  regrouped");
  }
  else {
    OutputDescription = 
      (char*)malloc((strlen("File  regrouped")+strlen(inputfile?inputfile:"from pipe")+1)*sizeof(char));
    sprintf(OutputDescription, "File %s regrouped", inputfile?inputfile:"from pipe");
  }
  if(InputContents) {
    OutputContents = (char*)malloc((strlen(InputContents)+strlen(",  regrouped")+1)*sizeof(char));
    OutputContents = strcat( strcpy(OutputContents, InputContents), ",  regrouped");
  }
  else {
    OutputContents = 
      (char*)malloc((strlen("File  regrouped")+strlen(inputfile?inputfile:"from pipe")+1)*sizeof(char));
    sprintf(OutputContents, "File %s regrouped", inputfile?inputfile:"from pipe");
  }
  if(!SDDS_InitializeOutput(&outputPage, SDDS_BINARY, 0, OutputDescription, OutputContents, outputfile))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  /*************************************************\
   * define columns and parameters and store indices * 
   \*************************************************/

  /******************************************************\
   *  selected input columns are transfered to parameters *
   \******************************************************/
  for(i = 0;i<NColToPar;i++){
    if(!SDDS_DefineParameterLikeColumn(&outputPage, &inputPages[0], ColToParNames[i], ColToParNames[i]))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    ColToParInputIndex[i] = SDDS_GetColumnIndex(&inputPages[0], ColToParNames[i]);
    ColToParOutputIndex[i] = SDDS_GetParameterIndex(&outputPage, ColToParNames[i]);
  }
  /****************************************************\
   * selected input parameters are transfered to column *
   \****************************************************/
  for(i = 0;i<NParToCol;i++){
    if(!SDDS_DefineColumnLikeParameter(&outputPage, &inputPages[0], ParToColNames[i], ParToColNames[i]))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    ParToColInputIndex[i] = SDDS_GetParameterIndex(&inputPages[0], ParToColNames[i]);
    ParToColOutputIndex[i] = SDDS_GetColumnIndex(&outputPage, ParToColNames[i]);
  }
  /***********************************\
   * columns are transfered to columns *
   \***********************************/
  j = 0;
  for(i = 0;i<NInputColumns;i++){
    if(0>match_string(InputColumns[i], ColToParNames, NColToPar, EXACT_MATCH)) {
      if(0>SDDS_TransferColumnDefinition(&outputPage, &inputPages[0], InputColumns[i], InputColumns[i]))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      ColToColInputIndex[j] = SDDS_GetColumnIndex(&inputPages[0], InputColumns[i]);
      ColToColOutputIndex[j] = SDDS_GetColumnIndex(&outputPage, InputColumns[i]);
      j++;
    }
  }
  if(j != NColToCol)
    SDDS_Bomb("Error: Something went wrong with counting the columns. Report to author.");
  if(!SDDS_WriteLayout(&outputPage))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);    
  /*******************************\
   * fill pages in the output file *
   \*******************************/
  NOutputPages = NInputRows;
  NOutputRows = NInputPages;
  for( ipage = 0; ipage < NOutputPages; ipage++){
    if(verbose)
      fprintf(stderr,"Starting page %ld...\n", ipage);
    SDDS_StartTable(&outputPage, NOutputRows);
    /* set parameters */
    for( i = 0; i < NColToPar; i++){
      if(!SDDS_SetParameters(&outputPage, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, 
                             ColToParOutputIndex[i], 
                             SDDS_GetValueByAbsIndex(&inputPages[0], ColToParInputIndex[i], ipage, NULL), 
                             -1))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    /* set columns  */
    for(i = 0;i<NParToCol;i++){
      /* transfer parameters of input file to columns of output */
      for(row = 0;row<NOutputRows;row++){
        if(!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, 
                              row, ParToColOutputIndex[i], 
                              SDDS_GetParameter(&inputPages[row], InputParameters[ParToColInputIndex[i]], NULL), 
                              -1))
          SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
    }
    for(i = 0;i<NColToCol;i++){
      for(row = 0;row<NOutputRows;row++){
        if(!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, 
                              row, ColToColOutputIndex[i], 
                              SDDS_GetValueByAbsIndex(&inputPages[row], ColToColInputIndex[i], ipage, NULL), 
                              -1))
          SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
    }
    
    if(!SDDS_WriteTable(&outputPage))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  
  for(i=0;i<NInputPages;i++){
    if(!SDDS_Terminate(&inputPages[i]))
      SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  if (inputPages) free(inputPages);
  if(!SDDS_Terminate(&outputPage))
    SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

  if (tmpfile_used && !replaceFileAndBackUp(inputfile,  outputfile)) 
    exit(1);
  return(0);
}
