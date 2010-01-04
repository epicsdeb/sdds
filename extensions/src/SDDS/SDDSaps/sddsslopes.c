/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program sddsslopes1
 * makes lsf subroutine calls
 * for columns across pages in a sddsexperiment output file.
 * The independent variable must be a defined column.
 * The output file contains one table of slopes and intercepts.
 * The independent column of the input file
 * is removed in the output file, but its name is
 * converted to a parameter string. 
 * L. Emery with some code from M. Borland, ANL (1995)
 * added sigma for slopes and intercept 5/18/95
 $Log: sddsslopes.c,v $
 Revision 1.17  2005/11/04 22:46:18  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2002/08/14 17:12:55  soliday
 Added Open License

 Revision 1.15  2001/01/10 19:35:47  soliday
 Standardized usage message.

 Revision 1.14  1999/05/25 19:15:52  soliday
 Removed compiler warning on linux.

 Revision 1.13  1999/01/06 19:54:57  borland
 Fixed the version number in the usage message.

 Revision 1.12  1998/08/04 20:56:09  emery
 Chnged <parametername> to <columnName> in usage message.

 * Revision 1.11  1996/04/28  22:22:39  emery
 * Corrected "grammar" of usage message.
 *
 * Revision 1.10  1996/04/24  08:42:56  emery
 * Added an absent malloc for array depVar. Put some statements inside of
 * an if (ipage==1) block. Put a SDDS_CopyPage outside of an if (ipage==1) block.
 *
 * Revision 1.9  1996/04/12  18:43:32  emery
 * Added -range=<lower>,<upper> option, which allows the fitting restricted
 * over a range of independent variable values. The residual data file
 * still uses all original independent variable values.
 *
 * Revision 1.8  1996/03/21  05:14:25  emery
 * Now repeats slope calculation for each data page.
 *
 * Revision 1.7  1995/11/26  17:59:42  borland
 * Fixed bug introduced in previous modification; was setting weights array
 * to all 1's, which wiped out any other data in the array.
 *
 * Revision 1.6  1995/11/07  00:38:56  emery
 * Fixed bug: array weight was not initialized in some cases.
 *
 * Revision 1.5  1995/10/18  22:35:03  borland
 * Added an SDDS_CheckColumn call to report problems with independent column
 * in more detail.
 *
 * Revision 1.4  1995/10/16  18:36:07  borland
 * Fixed bugs with -sigma option (not setting weights to 1 before prelimiary fit.)
 * Rewrote some constructs of the form if (x) { } else if (!x) { }, as
 * if (x) { } else { }.
 *
 * Revision 1.3  1995/09/06  14:57:12  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "SDDS.h"

#define CLO_INDEPENDENT_COLUMN 0
#define CLO_COLUMNS 1
#define CLO_EXCLUDE 2
#define CLO_VERBOSE 3
#define CLO_SIGMA 4
#define CLO_ASCII 5
#define CLO_PIPE 6
#define CLO_RESIDUAL 7
#define CLO_RANGE 8
#define COMMANDLINE_OPTIONS 9
char *commandline_option[COMMANDLINE_OPTIONS] = {
  "independentVariable", 
  "columns", 
  "excludeColumns", 
  "verbose", 
  "sigma",
  "ascii",
  "pipe",
  "residual",
  "range"
  };

#define SIGMA_GENERATE 0
#define SIGMA_OPTIONS 1
char *sigma_option[SIGMA_OPTIONS] = {
  "generate"
  };

#define DEFAULT_EXCLUDED_COLUMNS 1
char *defaultExcludedColumn[DEFAULT_EXCLUDED_COLUMNS] = {
  "Time",
};

static char *USAGE = "sddsslopes <inputfile> <outputfile> [-pipe=[input][,output]]\n\
            -independentVariable=<columnName> [-range=<lower>,<upper>]\n\
            [-columns=<list-of-names>] [-excludeColumns=<list-of-names>] \n\
            [-sigma[=generate]] [-residual=<file>] [-ascii] [-verbose]\n\n\
Makes straight line fits of numerical column data in the input file, using one particular column\n\
as an independent variable.\n\
pipe                reads input or output from a pipe.\n\
inputfile           file with an independent variable column.\n\
outputfile          file with the slope and intercept of each column of the SDDSinputfile.\n\
                    The selected numerical columns for fitting appear with the names <name>Slopes\n\
                    and <name>Intercept.\n\
independentVariable name of independent variable (default is the first numerical column)\n\
range               specify range of independent variable to fit over.\n\
columns             columns individually paired with independentVariable for straight line fitting.\n\
excludeColumns      columns to exclude from fitting.\n\
sigma               calculates errors by interpreting column names <name>Sigma or Sigma<name> as\n\
                    sigma of column <name>. If these columns don't exist\n\
                    then the program generates a common sigma from the residual of a first fit,\n\
                    and refits with these sigmas. If option -sigma=generate is given,\n\
                    then sigmas are generated from the residual of a first fit for all columns,\n\
                    irrespective of the presence of columns <name>Sigma or Sigma<name>.\n\
residual            file in which the deviation from the linear fit is written.\n\
ascii               make slopes output file in ascii mode (binary is the default)\n\
verbose             prints some output to stderr\n\
Program by Louis Emery, ANL (This is version 2, August 1998.)";

long set_multicolumn_flags(SDDS_TABLE *SDDSin, char ***column, int32_t *columns,
                           char **exclude, long excludes);

int main(int argc, char **argv)
{
  SCANNED_ARG *scanned;
  SDDS_TABLE inputPage, outputPage, residualPage;
  char *inputfile, *outputfile;
  char **column, **excludeColumn;
  int32_t columns;
  long excludeColumns;
  char *indColumnName;
  long verbose;
  long iArg, i, j, ipage;
  double *indVar, *indVarOrig;
  char *indVarUnits;
  char **intColumn, **slopeColumn, **slopeSigmaColumn, **interceptSigmaColumn;
  char *Units,*slopeUnits;
  double *depVar, *depVarOrig;
  long order;
  double *coef, *coefsigma, *weight, *diff, *diffOrig, chi;
  long iCol, iRow;
  long rows, rowsOrig;
  double rmsResidual;
  double slopeSigma, interceptSigma;
  char **sigmaColumn, **chiSquaredColumn;
  long *sigmaColumnExists;
  long doSlopeSigma, generateSigma, doPreliminaryFit;
  long validSigmas;
  double sigmaSum, averageSigma;
  long ascii;
  char *residualFile;
  unsigned long pipeFlags;
  long tmpfile_used, noWarnings;
  double xMin, xMax;

  indVar = indVarOrig = depVar = depVarOrig = coef = coefsigma = weight = diff = NULL;
  intColumn = slopeColumn = slopeSigmaColumn = interceptSigmaColumn = sigmaColumn = chiSquaredColumn = NULL;
  slopeUnits = NULL;
  sigmaColumnExists = NULL;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv);
  if (argc == 1) 
    bomb(NULL, USAGE);

  inputfile = outputfile = NULL;
  columns = excludeColumns = 0;
  column = excludeColumn = NULL;
  indColumnName = NULL;
  verbose = 0;
  doSlopeSigma = 0;
  generateSigma = 0;
  doPreliminaryFit = 0;
  ascii = 0;
  pipeFlags = 0;
  tmpfile_used=0;
  noWarnings=0;
  residualFile = NULL;
  xMin = xMax = 0;

  for (iArg = 1;  iArg<argc;  iArg++) {
    if (scanned[iArg].arg_type == OPTION) {
      delete_chars(scanned[iArg].list[0], "_");
      switch (match_string(scanned[iArg].list[0], commandline_option, COMMANDLINE_OPTIONS, UNIQUE_MATCH)) {
      case CLO_INDEPENDENT_COLUMN:   
        if (!(indColumnName = scanned[iArg].list[1]))
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
      case CLO_ASCII:
        ascii = 1;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_SIGMA:
        doSlopeSigma = 1;
        if (scanned[iArg].n_items > 1 ) {
          switch (match_string(scanned[iArg].list[1], sigma_option, SIGMA_OPTIONS, UNIQUE_MATCH)) {
          case SIGMA_GENERATE:
            generateSigma = 1;
            break;
          default:
            SDDS_Bomb("unrecognized sigma option given");
            break;
          }
        }
        break;
      case CLO_RESIDUAL:
        if (!(residualFile=scanned[iArg].list[1])){
          fprintf(stderr,"No file specified in -residual option.\n");
          exit(1);
        }
        break;
      case CLO_RANGE:
        if (scanned[iArg].n_items!=3 ||
            1!=sscanf(scanned[iArg].list[1], "%lf", &xMin) ||
            1!=sscanf(scanned[iArg].list[2], "%lf", &xMax) ||
            xMin>=xMax)
          SDDS_Bomb("incorrect -range syntax");
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

  if (residualFile && outputfile) {
    if (!strcmp( residualFile, outputfile)) {
      fprintf( stderr, "Residual file can't be the same as the output file.\n");
      exit(1);
    }
  }
  
  processFilenames("sddsslopes", &inputfile, &outputfile, pipeFlags, noWarnings, &tmpfile_used);

  if (!indColumnName) {
    fprintf( stderr, "independentVariable not given\n");
    exit(1);
  }
  
  if (!excludeColumns) {
    excludeColumn = defaultExcludedColumn;
    excludeColumns = DEFAULT_EXCLUDED_COLUMNS;
  }
  
  if (verbose)
    fprintf(stderr,"Reading file %s.\n",inputfile);
  if ( !SDDS_InitializeInput( &inputPage, inputfile) )
    SDDS_PrintErrors( stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  while (0 < (ipage=SDDS_ReadTable( &inputPage))) {
    if (verbose) {
      fprintf(stderr, "working on page %ld\n", ipage);
    }
    rows = SDDS_CountRowsOfInterest(&inputPage);
    rowsOrig = rows;
    /*************************************\
     * make array of independent variable
     \*************************************/
    if (ipage==1) {
      indVar = (double*) malloc( sizeof(*indVar) * rows);
    }
    else {
      indVar = (double*) realloc( indVar, sizeof(*indVar) * rows);
    }
    if (ipage==1) {
      if (!SDDS_FindColumn(&inputPage, FIND_NUMERIC_TYPE, indColumnName, NULL)){
        fprintf(stderr,"Something wrong with column %s.\n", indColumnName);
        SDDS_CheckColumn(&inputPage, indColumnName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr);
        exit(1);
      }
    }
    /* filter out the specified range in independent variable */
    if (xMin!=xMax) {
      if (!(indVarOrig = SDDS_GetColumnInDoubles( &inputPage, indColumnName))) 
        SDDS_PrintErrors( stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      for (i=j=0; i<rowsOrig; i++) {
        if (indVarOrig[i]<=xMax && indVarOrig[i]>=xMin) {
          indVar[j] = indVarOrig[i];
          j++;
        }
      }
      rows = j;
    }
    else {
      if (!(indVar = SDDS_GetColumnInDoubles( &inputPage, indColumnName))) 
        SDDS_PrintErrors( stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if ( ipage == 1 ) {
      if (!SDDS_GetColumnInformation(&inputPage, "units", &indVarUnits, SDDS_GET_BY_NAME, indColumnName))
        SDDS_PrintErrors( stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!indVarUnits) {
        indVarUnits = (char *) malloc(sizeof(*indVarUnits));
        indVarUnits[0] = 0;
      }
    }
    /************************************\
     * initialize residual file 
     \************************************/
    if (residualFile) {
      if ( ipage == 1 ) {
        if(!SDDS_InitializeOutput(&residualPage,ascii?SDDS_ASCII:SDDS_BINARY,1,
                                  "Residual of 2-term fit",NULL,outputfile) ||
           !SDDS_InitializeCopy(&residualPage, &inputPage, residualFile, "w") ||
           !SDDS_WriteLayout(&residualPage) )
          SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!SDDS_CopyPage(&residualPage,&inputPage))
        SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
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
    if (ipage==1) {
      intColumn = (char**) malloc((sizeof(char*)*columns));
      slopeColumn = (char**) malloc((sizeof(char*)*columns));
      if (doSlopeSigma) {
        slopeSigmaColumn = (char**) malloc((sizeof(char*)*columns));
        interceptSigmaColumn = (char**) malloc((sizeof(char*)*columns));
        chiSquaredColumn = (char**) malloc((sizeof(char*)*columns));
      }
      for (i=0; i<columns; i++) {
        intColumn[i] = (char*) malloc((sizeof(char)*(strlen(column[i])+strlen("Intercept")+1)));
        strcat(strcpy(intColumn[i], column[i]), "Intercept");
        slopeColumn[i] = (char*) malloc((sizeof(char)*(strlen(column[i])+strlen("Slope")+1)));
        strcat(strcpy(slopeColumn[i], column[i]), "Slope");
        if (doSlopeSigma) {
          slopeSigmaColumn[i] = (char*) malloc((sizeof(char)*(strlen(column[i])+strlen("SlopeSigma")+1)));
          strcat(strcpy(slopeSigmaColumn[i], column[i]), "SlopeSigma");
          interceptSigmaColumn[i] = (char*) malloc((sizeof(char)*(strlen(column[i])+strlen("InterceptSigma")+1)));
          strcat(strcpy(interceptSigmaColumn[i], column[i]), "InterceptSigma");
          chiSquaredColumn[i] = (char*) malloc((sizeof(char)*(strlen(column[i])+strlen("ChiSquared")+1)));
          strcat(strcpy(chiSquaredColumn[i], column[i]), "ChiSquared");
        }
      }
    }
    /************************************\
     * Write layout for output file
     \*************************************/
    if (ipage==1) {
      if (verbose)
        fprintf(stderr,"Opening file %s.\n",outputfile);
      if(!SDDS_InitializeOutput(&outputPage,ascii?SDDS_ASCII:SDDS_BINARY,1,
                                "2-term fit",NULL,outputfile) ||
         0>SDDS_DefineParameter(&outputPage, "InputFile", "InputFile", NULL,
                                "InputFile", NULL, SDDS_STRING, 0) ||
         0>SDDS_DefineColumn(&outputPage, "IndependentVariable", NULL, NULL, NULL, NULL, SDDS_STRING,0) )
        SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (iCol=0; iCol<columns; iCol++) {
        if (!SDDS_GetColumnInformation(&inputPage, "units", &Units, SDDS_GET_BY_NAME, column[iCol]))
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
        if (doSlopeSigma) {
          if (0>SDDS_DefineColumn(&outputPage, interceptSigmaColumn[iCol], NULL, Units, 
                                  NULL, NULL, SDDS_DOUBLE,0) ||
              0>SDDS_DefineColumn(&outputPage, slopeSigmaColumn[iCol], NULL, slopeUnits, 
                                  NULL, NULL, SDDS_DOUBLE,0) ||
              0>SDDS_DefineColumn(&outputPage, chiSquaredColumn[iCol], NULL, NULL, 
                                  NULL, NULL, SDDS_DOUBLE,0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        free(slopeUnits);
      }
      if ( !SDDS_WriteLayout(&outputPage))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  
    if ( !SDDS_StartTable(&outputPage,1) ||
       !SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                           "InputFile",inputfile?inputfile:"pipe",NULL) ||
       !SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 0,
                          "IndependentVariable", indColumnName, NULL) )
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    /* determine which included columns has a Sigma column defined in the input file */
    if ( ipage == 1 ) {
      sigmaColumn = (char **) malloc( sizeof(*sigmaColumn)*columns);
      sigmaColumnExists = (long *) malloc(columns*sizeof(*sigmaColumnExists)); 
      for (iCol=0; iCol<columns; iCol++) {
        sigmaColumn[iCol] = (char *) malloc( sizeof(**sigmaColumn) * 
                                            (strlen(column[iCol]) + strlen("Sigma") + 1) );
        strcat( strcpy(sigmaColumn[iCol], column[iCol]), "Sigma");
        switch(SDDS_CheckColumn(&inputPage, sigmaColumn[iCol], NULL, SDDS_DOUBLE, NULL)) {
        case SDDS_CHECK_WRONGUNITS:
        case SDDS_CHECK_OKAY:
          sigmaColumnExists[iCol] = 1;
          break;
        default:
          /* try other possible spelling */
          strcat( strcpy(sigmaColumn[iCol] ,"Sigma"), column[iCol]);
          switch(SDDS_CheckColumn(&inputPage, sigmaColumn[iCol], NULL, SDDS_DOUBLE, NULL)) {
          case SDDS_CHECK_WRONGUNITS:
          case SDDS_CHECK_OKAY:
            sigmaColumnExists[iCol] = 1;
            break;
          default:
            sigmaColumnExists[iCol] = 0;
          }
          break;
        }
      }
    }
    
    if ( ipage == 1 ) {
      weight = (double*)malloc(sizeof(*weight)*rows);
      diff = (double*)malloc(sizeof(*diff)*rows);
      order=1;
      coef = (double*)malloc(sizeof(*coef)*(order+1));
      coefsigma = (double*)malloc(sizeof(*coefsigma)*(order+1));
    }
    else {
      weight = (double*)realloc( weight, sizeof(*weight)*rows);
      diff = (double*)realloc( diff, sizeof(*diff)*rows);
      order=1;
      coef = (double*)realloc( coef, sizeof(*coef)*(order+1));
      coefsigma = (double*)realloc( coefsigma, sizeof(*coefsigma)*(order+1));
    }
    
    if (ipage==1) {
      depVar = (double*) malloc( sizeof(*depVar) * rows);
    }
    else {
      depVar = (double*) realloc( depVar, sizeof(*depVar) * rows);
    }
    for (iCol=0; iCol<columns; iCol++) {
      if (verbose)
        fprintf(stderr,"Doing column %s.\n", column[iCol]);
      /* filter out the specified range in independent variable */
      if (xMin!=xMax) {
        if (!(depVarOrig = (double*) SDDS_GetColumnInDoubles(&inputPage, column[iCol])))
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        for (i=j=0; i<rowsOrig; i++) {
          if ( xMin <= indVarOrig[i] && indVarOrig[i] <= xMax ) {
            depVar[j] = depVarOrig[i];
            j++;
          }
        }
      }
      else {
        if (!(depVar=SDDS_GetColumnInDoubles(&inputPage, column[iCol])))
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
      
      /*********************
        three possibilities:
        1) don't do or write slope errors. (doSlopeSigma=0)
        do one lsf call with all weights = 1
        2) calculated slope errors from sigma columns in the input file. 
        (doSlopeSigma=1 && generateSigma=0 && sigmaColumnExists[iCol]=1 )
        do one lsf call with weights from sigma columns
        3) calculate slope errors from generated sigma from a preliminary fit. 
        (doSlopeSigma=1 && (generateSigma=1 || sigmaColumnExists[iCol]=NULL)
        do preliminary fit to generate sigma
        *********************/

      for (iRow=0; iRow<rows; iRow++)
        weight[iRow] = 1;
      if (doSlopeSigma) {
        /*********************
          check validity of sigma column values 
          *********************/
        if( !generateSigma && sigmaColumnExists[iCol]) {
          if (verbose)
            fprintf(stderr,"\tUsing column %s for sigma.\n",sigmaColumn[iCol]);
          if(!(weight=SDDS_GetColumnInDoubles(&inputPage, sigmaColumn[iCol])))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          /* check for zero weight values which will give lsfn problems */
          validSigmas = rows;
          sigmaSum = 0;
          for (iRow=0; iRow<rows; iRow++) {
            sigmaSum += weight[iRow];
            if(!weight[iRow]) {
              validSigmas--;
              /* 
                fprintf(stderr,"Warning: %s of row number %ld is zero. Using average sigma.\n",sigmaColumn[iCol],iRow); */
            }
          }
          if (!validSigmas) {
            fprintf(stderr,"Warning: All sigmas are zero.\n");
            doPreliminaryFit = 1;
          }
          else if (validSigmas!=rows) {
            /* fix some sigmas */
            averageSigma = sigmaSum/ validSigmas;
            fprintf(stderr, "Warning: replacing %ld invalid sigmas with average (%e)\n",
                    rows-validSigmas, averageSigma);
            for (iRow=0; iRow<rows; iRow++) {
              if(!weight[iRow]) {
                weight[iRow] = averageSigma;
              }
            }
          }
        }
        else {
          doPreliminaryFit = 1;
        }
      }
      
      if (doPreliminaryFit) {
        if (verbose)
          fprintf(stderr,"\tGenerating sigmas from rms residual of a preliminary fit.\n");
        if (!(lsfn(indVar, depVar, weight, rows, order, coef, coefsigma, &chi, diff))){
          fprintf(stderr,"Problem with call to lsfn\n.");
          exit(1);
        }
        rmsResidual = 0;
        /* calculate rms residual */
        for (iRow=0; iRow<rows; iRow++) {
          rmsResidual += sqr(diff[iRow]);
        }
        rmsResidual = sqrt(rmsResidual/(rows));
        for (iRow=0; iRow<rows; iRow++) {
          weight[iRow] = rmsResidual;
        }
      }
      
      if (!(lsfn(indVar, depVar, weight, rows, order, coef, coefsigma, &chi, diff))) {
        fprintf(stderr,"Problem with call to lsfn\n.");
        exit(1);
      }
      if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 0,
                             intColumn[iCol], coef[0],
                             slopeColumn[iCol], coef[1], NULL))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (doSlopeSigma) {
        interceptSigma = coefsigma[0];
        slopeSigma = coefsigma[1];
        if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 0,
                               chiSquaredColumn[iCol], chi,
                               interceptSigmaColumn[iCol], interceptSigma,
                               slopeSigmaColumn[iCol], slopeSigma, NULL))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (residualFile) {
        if (xMin!=xMax) {
          /* calculate the residuals for the whole column explicitly since there
             are points outside the range of which the lsf call did not calculate
             the difference. */
          diffOrig = (double*) malloc( rowsOrig * sizeof(double) );
          for (i=0; i<rowsOrig; i++) {
            diffOrig[i] = depVarOrig[i] - coef[0] - coef[1] * indVarOrig[i];
          }
          if( !SDDS_SetColumnFromDoubles(&residualPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,
                                         diffOrig,rowsOrig,column[iCol]))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        else {
          if( !SDDS_SetColumnFromDoubles(&residualPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,
                                         diff,rows,column[iCol]))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
    }

    if (residualFile) {
      if (!SDDS_WriteTable(&residualPage)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
    if (!SDDS_WriteTable(&outputPage)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (residualFile) {
    if (!SDDS_Terminate(&residualPage))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if(!SDDS_Terminate(&inputPage) || !SDDS_Terminate(&outputPage))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (tmpfile_used && !replaceFileAndBackUp(inputfile, outputfile)) 
    exit(1);
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
  }
  free(*column);


  for (i=0; i<excludes; i++)
    if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, exclude[i], SDDS_NEGATE_MATCH|SDDS_AND))
      return 0;

  if (!(*column = SDDS_GetColumnNames(SDDSin, columns)) || *columns==0) {
    SDDS_SetError("Selected columns not found.");
    return 0;
  }

  return 1; 
}
