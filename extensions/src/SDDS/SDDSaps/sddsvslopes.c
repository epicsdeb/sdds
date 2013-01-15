/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program sddsvslopes
 * makes lsf subroutine calls
 * for rows across pages in a sddsvexperiment output file.
 * The independent variable must be a defined parameter.
 * The output file contains one table of slopes and intercepts.
 * The independent parameter of the input file
 * is removed in the output file, but its name is
 * converted to a parameter string. 
 * L. Emery with some code from M. Borland, ANL (1995)
 $Log: not supported by cvs2svn $
 Revision 1.17  2008/02/08 18:29:16  shang
 finished the implementation of -sigma option for computing the slope errors.

 Revision 1.16  2006/12/14 22:22:01  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.15  2006/10/19 22:11:30  soliday
 Moved a free command because it was bombing on linux-x86_64

 Revision 1.14  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.13  2002/08/14 17:12:57  soliday
 Added Open License

 Revision 1.12  2001/01/10 19:35:49  soliday
 Standardized usage message.

 Revision 1.11  1999/05/25 19:17:31  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/01/06 19:54:59  borland
 Fixed the version number in the usage message.

 Revision 1.9  1996/05/22 23:30:31  emery
 Corrected a newline bug in the usage message.

 * Revision 1.8  1996/05/22  23:25:13  emery
 * Changed indentation of usage message.
 *
 * Revision 1.7  1996/02/27  20:59:23  emery
 * Column Index is created in the output file if not present in inputfile
 *
 * Revision 1.6  1996/02/22  18:37:32  emery
 * Checks for presence of column "Rootname".
 *
 * Revision 1.5  1996/02/13  20:43:21  emery
 * Use "m" as the filemode parameter (used to be NULL) for InitializeCopy function,
 * made necessary by changes made to the InitializeCopy function.
 *
 * Revision 1.4  1996/01/05  21:02:40  emery
 * Fixed SEGV error problem when no units are defined
 * for the columns of the input file.
 *
 * Revision 1.3  1995/09/06  14:57:22  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "SDDS.h"

#define CLO_INDEPENDENT_PARAMETER 0
#define CLO_COLUMNS 1
#define CLO_EXCLUDE 2
#define CLO_VERBOSE 3
#define CLO_SDDS_OUTPUT_ROOT 4
#define CLO_SLOPE_ERRORS 5
#define CLO_PIPE 6
#define COMMANDLINE_OPTIONS 7
char *commandline_option[COMMANDLINE_OPTIONS] = {
    "independentVariable", 
    "columns", 
    "excludeColumns", 
    "verbose", 
    "sddsOutputRoot", 
    "sigma",
    "pipe"
        };

#define DEFAULT_EXCLUDED_COLUMNS 3
char *defaultExcludedColumn[DEFAULT_EXCLUDED_COLUMNS] = {
    "Index",
    "ElapsedTime",
    "Rootname"
        };

#define DEFAULT_COPY_COLUMNS 2
char *defaultCopyColumn[DEFAULT_COPY_COLUMNS] = {
    "Index",
    "Rootname"
        };

static char *USAGE = "sddsvslopes <inputfile> <outputfile> [-pipe=[input][,output]]\n\
            -independentVariable=<parametername>\n\
            [-columns=<list-of-names>] [-excludeColumns=<list-of-names>] \n\
            [-sigma] [-verbose]\n\n\
Makes straight line fits of column data in the input file, using one particular \n\
             parameter as an independent variable.\n\
pipe         reads input or output from a pipe.\n\
inputfile    multiple data-set file with vectorized column data and an independent variable\n\
             parameter. The string column Rootname must be present. If a column \"Index\"\n\
             is present then it is copied to the output file.\n\
outputfile   file with the slope and intercept of each specified column of the SDDSinputfile.\n\
             The column Rootname is transferred to the outputfile.\n\
             For each double columns selected from the input file, two columns in the\n\
             output file are created with names created with Slopes and Intercept appended\n\
             to the original column name. Column Index is created if not present in inputfile.\n\
independentVariable\n\
             name of independent variable (default is the first valid parameter).\n\
columns      columns individually paired with independentVariable for straight line fitting.\n\
excludeColumns\n\
             columns to exclude from fitting.\n\
sigma        generates errors for slopes by interpreting column names Sigma<name> as sigma\n\
             of column <name>.\n\
verbose      prints output to stderr\n\
Program by Louis Emery, ANL (This is version 2, May 1996.)";

long set_multicolumn_flags(SDDS_TABLE *SDDSin, char ***column, int32_t *columns,
                           char **exclude, long excludes);

int main(int argc, char **argv)
{
  SCANNED_ARG *scanned;
  SDDS_TABLE inputPage, *copiedPage, outputPage;
  long copiedPages;
  char *inputfile, *outputfile;
  char **column, **excludeColumn=NULL;
  int32_t columns;
  long excludeColumns;
  char *indParameterName;
  char **copyColumn;
  int32_t copyColumns;
  long verbose;
  long slopeErrors;
  long iArg,i;
  double *indVar;
  char *indVarUnits;
  char **intColumn,**slopeColumn,**slopeSigmaColumn;
  char *Units,*slopeUnits;
  double *depVar;
  long order;
  double *coef,*coefsigma,*weight,*diff,chi;
  long iCol,iRow,iPage;
  long rows;
  double *slope, slope2, slopeAve, slopeSigma;
  unsigned long pipeFlags;
  long tmpfile_used, noWarnings;
  long generateIndex;

  copiedPage = NULL;
  slopeSigmaColumn = NULL;
  slopeUnits = Units = indVarUnits = NULL;
  rows = 0;
  slope = NULL;
  slope2 = 0;
  coef = coefsigma = weight = diff = slope = NULL;
  
  argc = scanargs(&scanned, argc, argv);
  if (argc == 1) 
    bomb(NULL, USAGE);

  inputfile = outputfile = NULL;
  columns = excludeColumns = 0;
  column = excludeColumn = NULL;
  indParameterName = NULL;
  verbose = 0;
  slopeErrors = 0;
  pipeFlags = 0;
  tmpfile_used=0;
  noWarnings=0;
  for (iArg = 1;  iArg<argc;  iArg++) {
    if (scanned[iArg].arg_type == OPTION) {
      delete_chars(scanned[iArg].list[0], "_");
      switch (match_string(scanned[iArg].list[0], commandline_option, COMMANDLINE_OPTIONS, UNIQUE_MATCH)) {
      case CLO_INDEPENDENT_PARAMETER:   
        if (!(indParameterName = scanned[iArg].list[1]))
          SDDS_Bomb("no string given for option -independentVariable");
        break;
      case CLO_COLUMNS:
        if (columns)
          SDDS_Bomb("only one -columns option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        column = tmalloc(sizeof(*column)*(columns = scanned[iArg].n_items-1));
        for (i = 0;  i<columns;  i++)
          column[i] = scanned[iArg].list[i+1];
        break;
      case CLO_EXCLUDE:
        if (excludeColumns)
          SDDS_Bomb("only one -excludecolumns option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -excludecolumns syntax");
        excludeColumn = tmalloc(sizeof(*excludeColumn)*(excludeColumns = scanned[iArg].n_items-1));
        for (i = 0;  i<excludeColumns;  i++)
          excludeColumn[i] = scanned[iArg].list[i+1];
        break;
      case CLO_VERBOSE:
        verbose = 1;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_SLOPE_ERRORS:
        slopeErrors = 1;
        break;
      default: 
        SDDS_Bomb("unrecognized option given");
        break;
      }
    }
    else {
      if (!inputfile)
        inputfile = scanned[iArg].list[0];
      else if (!outputfile)
        outputfile = scanned[iArg].list[0];
      else
        SDDS_Bomb("too many filenames given");
    }
  }

  processFilenames("sddsvslopes", &inputfile, &outputfile, pipeFlags, noWarnings, &tmpfile_used);

  if (!indParameterName)
    SDDS_Bomb("independentVariable not given");

  if (!excludeColumns) {
    excludeColumn = defaultExcludedColumn;
    excludeColumns = DEFAULT_EXCLUDED_COLUMNS;
  }
  
  if (verbose)
    fprintf(stderr,"Reading file %s.\n",inputfile);
  SDDS_InitializeInput(&inputPage, inputfile);
  copiedPages = 0;
  while (SDDS_ReadTable(&inputPage)>0) {
    if (!copiedPages) {
      copiedPage = (SDDS_TABLE*)malloc(sizeof(SDDS_TABLE));
      rows = SDDS_CountRowsOfInterest(&inputPage);
    }
    else {
      copiedPage = (SDDS_TABLE*)realloc(copiedPage, (copiedPages+1)*sizeof(SDDS_TABLE));
    }
    if (!SDDS_InitializeCopy(&copiedPage[copiedPages], &inputPage, NULL, "m"))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!SDDS_CopyTable(&copiedPage[copiedPages], &inputPage))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    copiedPages++;
  }
  if (copiedPages<2) {
    fprintf(stderr,"Insufficient data (i.e. number of data pages) to fit a straight line.\n");
    exit(1);
  }
  switch(SDDS_CheckColumn(&inputPage, "Rootname", NULL, SDDS_STRING, NULL)) {
  case SDDS_CHECK_WRONGUNITS:
  case SDDS_CHECK_OKAY:
    break;
  default:
    fprintf(stderr,"Something wrong with column %s.\n", "Rootname");
    exit(1);
  }
  switch(SDDS_CheckColumn(&inputPage, "Index", NULL, SDDS_LONG, NULL)) {
  case SDDS_CHECK_WRONGUNITS:
  case SDDS_CHECK_OKAY:
    generateIndex = 0;
    break;
  case SDDS_CHECK_NONEXISTENT:
    generateIndex = 1;
    break;
  default:
    fprintf(stderr,"Something wrong with column %s.\n", "Rootname");
    exit(1);
  }
  /****************\
   * make array of independent variable
   \**************/
  indVar = (double*)malloc(sizeof(*indVar)*copiedPages);
  switch(SDDS_CheckParameter(&inputPage, indParameterName, NULL, SDDS_DOUBLE, NULL)) {
  case SDDS_CHECK_WRONGUNITS:
  case SDDS_CHECK_OKAY:
    break;
  default:
    fprintf(stderr,"Something wrong with parameter %s.\n", indParameterName);
    exit(1);
  }
  for (iPage = 0; iPage<copiedPages; iPage++) {
    if (!SDDS_GetParameter(&copiedPage[iPage],indParameterName,&indVar[iPage])) 
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  if (!SDDS_GetParameterInformation(&inputPage, "units", &indVarUnits, SDDS_GET_BY_NAME, indParameterName))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!indVarUnits) {
    indVarUnits = (char *) malloc(sizeof(*indVarUnits));
    indVarUnits[0] = 0;
  }
  /************************************\
   * get columns of interest. use set_multicolumn_flags to simply
   * return new values for array column.
   \*************************************/
  if (!set_multicolumn_flags(&inputPage, &column, &columns, excludeColumn, excludeColumns)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  /************************************\
   * make  column names for the output
   \*************************************/
  intColumn = (char**)malloc((sizeof(char*)*columns));
  slopeColumn = (char**)malloc((sizeof(char*)*columns)); 
  if (slopeErrors)
    slopeSigmaColumn = (char**)malloc((sizeof(char*)*columns));
  for (i=0; i<columns; i++) {
    intColumn[i] = (char*)malloc((sizeof(char)*(strlen(column[i])+strlen("Intercept")+1)));
    strcat(strcpy(intColumn[i], column[i]), "Intercept");
    slopeColumn[i] = (char*)malloc((sizeof(char)*(strlen(column[i])+strlen("Slope")+1)));
    strcat(strcpy(slopeColumn[i], column[i]), "Slope");
    if (slopeErrors) {
      slopeSigmaColumn[i] = (char*)malloc((sizeof(char)*(strlen(column[i])+strlen("SlopeSigma")+1)));
      strcat(strcpy(slopeSigmaColumn[i], column[i]), "SlopeSigma");
    }
  }
  /************************************\
   * Write layout for output file
   \*************************************/
  if (verbose)
    fprintf(stderr,"Opening file %s.\n",outputfile);
  if(!SDDS_InitializeOutput(&outputPage,SDDS_BINARY,1,
                            "lsf of sddsvexperiment",NULL,outputfile) ||
     0>SDDS_DefineParameter(&outputPage, "InputFile", "InputFile", NULL,
                            "InputFile", NULL, SDDS_STRING, 0) ||
     0>SDDS_DefineParameter(&outputPage, "IndependentVariable", "IndependentVariable", NULL,
                            "IndependentVariable", NULL, SDDS_STRING, 0) ||
     (0>SDDS_DefineColumn(&outputPage,"Index",NULL,NULL,"Rootname index",NULL,SDDS_LONG,0))||
     (0>SDDS_DefineColumn(&outputPage,"Rootname",NULL,NULL,NULL,NULL,SDDS_STRING,0)))
    SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (iCol=0; iCol<columns; iCol++) {
    if (!SDDS_GetColumnInformation(&inputPage, "units", &Units, SDDS_GET_BY_NAME,column[iCol]))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!Units) {
      Units = (char*) malloc(sizeof(*Units));
      Units[0] = 0;
    }
    if (0>SDDS_DefineColumn(&outputPage, intColumn[iCol], NULL, Units, NULL, NULL, SDDS_DOUBLE,0))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    /* units for slopes columns */
    if (strlen(indVarUnits) && strlen(Units) ) {
      slopeUnits = (char*)malloc(sizeof(*slopeUnits)*(strlen(Units)+strlen(indVarUnits)+2));
      strcat( strcat( strcpy(slopeUnits, Units), "/"), indVarUnits);
    }
    if (strlen(indVarUnits) && !strlen(Units) ) {
      slopeUnits = (char*)malloc(sizeof(*slopeUnits)*(strlen(indVarUnits)+2));
      strcat( strcpy( slopeUnits, "1/"), indVarUnits);
    }
    if (!strlen(indVarUnits) && strlen(Units) ) {
      slopeUnits = (char*)malloc(sizeof(*slopeUnits)*(strlen(Units)+2));
      strcpy( slopeUnits, indVarUnits);
    }
    if (!strlen(indVarUnits) && !strlen(Units) ) {
      slopeUnits = (char*)malloc(sizeof(*slopeUnits));
      strcpy( slopeUnits, "");
    }
    if (0>SDDS_DefineColumn(&outputPage, slopeColumn[iCol], NULL, slopeUnits, NULL, NULL, SDDS_DOUBLE,0))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (slopeErrors) {
      if (0>SDDS_DefineColumn(&outputPage, slopeSigmaColumn[iCol], NULL, slopeUnits, NULL, NULL, SDDS_DOUBLE,0))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    free(slopeUnits);
  }
  if(!SDDS_WriteLayout(&outputPage) || 
     !SDDS_StartTable(&outputPage,rows) ||
     !SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         "InputFile",inputfile?inputfile:"pipe",NULL)  ||
     !SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 0,
                         "IndependentVariable", indParameterName, NULL) )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  /************************************\
   * Copy columns to output file (usually columns Index and Rootname)
   \*************************************/
  copyColumns = DEFAULT_COPY_COLUMNS;
  copyColumn = defaultCopyColumn;
  if (!set_multicolumn_flags(&inputPage, &copyColumn, &copyColumns, NULL, 0)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    exit(1);
  }
  if(!SDDS_CopyColumns(&outputPage, &inputPage))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  depVar = (double*)malloc(sizeof(*depVar)*copiedPages);
  weight = (double*)malloc(sizeof(*weight)*copiedPages);
  diff = (double*)malloc(sizeof(*weight)*copiedPages);
  order=1;
  coef = (double*)malloc(sizeof(*coef)*(order+1));
  coefsigma = (double*)malloc(sizeof(*coefsigma)*(order+1));
  if(slopeErrors)
    slope = (double*)malloc(sizeof(*slope)*copiedPages);
  for (iCol=0; iCol<columns; iCol++) {
    for (iPage=0; iPage<copiedPages; iPage++) 
      weight[iPage]=1;
    if (verbose)
      fprintf(stderr,"Doing column %s.\n", column[iCol]);
    for (iRow=0; iRow<rows; iRow++) {
      for (iPage=0; iPage<copiedPages; iPage++) {
        if (!SDDS_GetValue(&copiedPage[iPage], column[iCol], iRow, &depVar[iPage]))                     
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
      if (!(lsfn(indVar, depVar, weight, copiedPages, order, coef, coefsigma, &chi, diff))){
        fprintf(stderr,"Problem with call to lsfn\n.");
        exit(1);
      }
      if (generateIndex) {
        if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, iRow,
                               "Index",iRow,
                               intColumn[iCol], coef[0],
                               slopeColumn[iCol], coef[1], NULL))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      else {
        if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, iRow,
                               intColumn[iCol], coef[0],
                               slopeColumn[iCol], coef[1], NULL))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if(slopeErrors){
        /* recalculate the slope with a subset of points  */
        slopeAve= slope2 = 0;
        for (iPage=0; iPage<copiedPages; iPage++) {
          weight[iPage] = 1e10;
          if(iPage)
            weight[iPage-1] = 1; 
          if (!(lsfn(indVar, depVar, weight, copiedPages, order, coef, coefsigma, &chi, diff))){
            fprintf(stderr,"Problem with call to lsfn\n.");
            exit(1);
          }
          slope[iPage] = coef[1];
          slopeAve += slope[iPage];
          slope2 += sqr(slope[iPage]);
        }
        slopeSigma = sqrt(slope2/copiedPages - sqr(slopeAve/copiedPages));
        if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, iRow,
                               slopeSigmaColumn[iCol], slopeSigma, NULL))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
 
  if( !SDDS_WriteTable(&outputPage)|| SDDS_Terminate(&inputPage) )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (iPage=0; iPage<copiedPages; iPage++) {
    if( !SDDS_Terminate(&copiedPage[iPage]) )
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if( SDDS_Terminate(&outputPage) )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (tmpfile_used && !replaceFileAndBackUp(inputfile, outputfile)) 
    exit(1);
  SDDS_FreeStringArray(column, columns);
  free(column);
  SDDS_FreeStringArray(intColumn, columns);
  SDDS_FreeStringArray(slopeColumn, columns); 
  free(intColumn);
  free(slopeColumn);
  if (slopeErrors) {
    SDDS_FreeStringArray(slopeSigmaColumn, columns);
    free(slopeSigmaColumn);
  }
  
  free(copiedPage);
  free(indVar);
  free(depVar);
  if (weight)
    free(weight);
  if (diff)
    free(diff);
  if (slope)
    free(slope);
  if (coef) free(coef);
  if (coefsigma) free(coefsigma);
  if (Units) free(Units);
  if (indVarUnits) free(indVarUnits);
  
  free_scanargs(&scanned, argc);
  
  return(0);
}

long set_multicolumn_flags(SDDS_TABLE *SDDSin, char ***column, int32_t *columns,
                           char **exclude, long excludes)
{
  long index, i;
  
  if (*column) {
    SDDS_SetColumnFlags(SDDSin, 0);
    for (i=0; i< *columns; i++) {
      if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, (*column)[i], SDDS_OR))
	return 0;
    }
  }
  else {
    SDDS_SetColumnFlags(SDDSin, 1);
    if (!(*column = SDDS_GetColumnNames(SDDSin, columns)) || *columns==0) {
      SDDS_SetError("no columns found");
      return 0;
    }
    for (i=0; i< *columns; i++) {
      index = SDDS_GetColumnIndex(SDDSin, (*column)[i]);
      if (!SDDS_NUMERIC_TYPE(SDDS_GetColumnType(SDDSin, index)) && 
	  !SDDS_AssertColumnFlags(SDDSin, SDDS_INDEX_LIMITS, index, index, 0)) 
	return 0;
    }
    free(*column);
    *column=NULL;
  }
  
  
  for (i=0; i<excludes; i++)
    if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, exclude[i], SDDS_NEGATE_MATCH|SDDS_AND))
      return 0;
  
  if (!(*column = SDDS_GetColumnNames(SDDSin, columns)) || *columns==0) {
    SDDS_SetError("Selected columns not found.");
    return 0;
  }
  
  return 1; 
}

