/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file transpose.c
 * read a sddsfile with one page or more.
 * find all numerical columns, and transpose them
 * as though they were a matrix.
 * If there are more than one pages, the other pages are transposed
 * only if they have the same number of rows as the first page.
 * string columns become string parameters.
 * string parameters listed in parameter "OldStringColumnNames" become string columns.
 * If any string columns exist, the data in the first string column
 * becomes the names of the columns in the output file.
 * If no string columns exists, then names are made from command line options.
 * The names of the columns of the input file are saved as a string column with name specified
 * in the command line or the default "OldColumnName".
 $Log: not supported by cvs2svn $
 Revision 1.20  2006/12/14 22:22:01  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.19  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.18  2002/08/14 17:12:57  soliday
 Added Open License

 Revision 1.17  2001/01/29 17:38:48  soliday
 An error message is displayed when there are no numerical columns.

 Revision 1.16  2001/01/10 19:35:49  soliday
 Standardized usage message.

 Revision 1.15  2000/04/13 17:12:41  soliday
 Removed WIN32 define statements that were moved to mdb.h

 Revision 1.14  1999/07/09 15:11:30  soliday
 Added definition for strcasecmp on WIN32

 Revision 1.13  1999/05/25 19:17:17  soliday
 Removed compiler warning on linux.

 Revision 1.12  1999/04/16 14:04:01  borland
 Fixed line that checks for existence of InputFile parameter in output file.

 Revision 1.11  1999/03/04 17:42:49  borland
 Optimized code by rewriting JoinString.  Also disabled SDDS table checks
 and saving of the layout during creation of the file.

 Revision 1.10  1998/07/08 22:29:23  emery
 Previous version assigned values to the column name string column
 only for the first page. Now the string values to be assigned
 explicitly for each page.

 * Revision 1.9  1996/07/03  15:51:19  emery
 * If the input file has only one row, and a rootname is specified
 * in the command line, the columne nam created for the output file
 * no longer has 000 appended to the rootname.
 *
 * Revision 1.8  1996/04/28  21:32:49  emery
 * Added option -digits. Options are made consistent with those
 * of sddspseudoinverse.
 *
 * Revision 1.7  1996/03/28  17:16:36  borland
 * Fixed bug in JoinStrings(): wasn't allocating enough memory to guarantee that
 * the new string would fit in the buffer.
 *
 * Revision 1.6  1996/01/09  21:03:00  borland
 * Added -newColumnNames option for specifying the name of the string column
 * from which to take the names for the new column.
 *
 * Revision 1.5  1995/12/06  23:15:40  borland
 * Made -root option override default use of first string column contents
 * for creating new column names; added 1 to length of a malloc'd string to
 * make it the right length.
 *
 * Revision 1.4  1995/11/13  15:57:46  borland
 * Added long cast for strlen() in comparison to satisfy Solaris compiler.
 *
 * Revision 1.3  1995/09/06  14:57:20  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "matlib.h"
#include "SDDS.h"
#include "SDDSutils.h"

#define CLO_VERBOSE 0
#define CLO_COLUMNROOT 1
#define CLO_SYMBOL 2
#define CLO_ASCII 3
#define CLO_PIPE 4
#define CLO_OLDCOLUMNNAMES 5
#define CLO_NEWCOLUMNNAMES 6
#define CLO_DIGITS 7
#define CLO_MATCH_COLUMN 8
#define CLO_INDEX_COLUMN 9
#define CLO_NO_OLDCOLUMNNAMES 10
#define COMMANDLINE_OPTIONS 11
char *commandline_option[COMMANDLINE_OPTIONS] = {
  "verbose", 
  "root", 
  "symbol", 
  "ascii", 
  "pipe", 
  "oldColumnNames",
  "newColumnNames",
  "digits",
  "matchColumn",
  "indexColumn",
  "noOldColumnNames",
  };

static char *OLD_STRING_COLUMN_NAMES="OldStringColumnNames";

static char *USAGE="sddstranspose <inputfile> <outputfile> [-pipe=[input][,output]]\n\
     [-oldColumnNames=<string>] [{-root=<string> [-digits=<integer>] | \n\
     -newColumnNames=<column>}] [-matchColumn=<string>[,<string>,...] [-indexColumn] [-noOldColumnNames] \n\
     [-symbol=<string>] [-ascii] [-verbose]\n\n\
pipe                reads input from and/or writes output to a pipe.\n\
oldColumnNames      specifies name for the output file string column created for\n\
                    the input file column names.\n\
root                use the string specified to generate column names of the output file.\n\
                    Default for column names is the first string column in\n\
                    <inputfile>. If there is no string column, then the column\n\
                    names are formed with the root \"Column\";\n\
digits              minimum number of digits used in the number appended to the root\n\
                    part of the column names. Default is value 3.\n\
newColumnNames      use the column specified as the source for new column names.\n\
symbol              use the string specified for the symbol field for all column definitions;\n\
ascii               output file in ascii. Default is binary;\n\
matchColumn         only transpose the matching columns.\n\
indexColumn         added index column to the output file.\n\
noOldColumnNames    Do not make a new column of old column names.\n\
verbose             prints out incidental information to stderr;\n";

char **TokenizeString(char *source, long n_items);
char *JoinStrings(char **source, long n_items, long buflen_increment);

int main(int argc, char **argv)
{
  SCANNED_ARG *s_arg;
  SDDS_DATASET inputPage, outputPage;
  
  char *inputfile, *outputfile;
  char **inputColumnName, **inputStringColumnName, **inputDoubleColumnName;
  char **outputStringColumnName, **outputDoubleColumnName, **matchColumn=NULL;
  long inputRows, inputDoubleColumns, inputStringColumns, indexColumn=0, matchColumns=0, noOldColumnNamesColumn=0;
  long outputRows, outputDoubleColumns, outputStringColumns;
  char **inputParameterName;
  int32_t inputParameters, inputColumns;
  char *inputDescription, *inputContents;
  char *outputDescription;
  long i, i_arg, col;
  char *buffer;
  char **columnOfStrings;
  long buffer_size;
#define BUFFER_SIZE_INCREMENT 16384
  MATRIX *R, *RInv;
  long OldStringColumnsDefined;
  char *inputStringRows, *outputStringRows;
  char **stringArray, *stringParameter;
  long token_length;
  long verbose;
  char format[32];
  long digits;
  char *Symbol, *Root;
  void *parameterPointer;
  long ascii;
  unsigned long pipeFlags;
  long tmpfile_used, noWarnings;
  long ipage=0, columnType;
  char *oldColumnNames, *newColumnNamesColumn;
  
  inputColumnName = outputStringColumnName = outputDoubleColumnName = inputParameterName = NULL;
  outputRows = outputDoubleColumns = outputStringColumns = OldStringColumnsDefined = 0;

  SDDS_RegisterProgramName(argv[0]);

  argc = scanargs(&s_arg, argc, argv);
  if (argc==1) 
    bomb(NULL,  USAGE);

  inputfile = outputfile = NULL;
  verbose = 0;
  Symbol = Root = NULL;
  ascii = 0;
  digits=3;
  pipeFlags = 0;
  tmpfile_used = 0;
  noWarnings = 0;
  oldColumnNames = NULL;
  newColumnNamesColumn = NULL;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch(match_string(s_arg[i_arg].list[0], commandline_option, COMMANDLINE_OPTIONS, 
                          UNIQUE_MATCH)) {
      case CLO_MATCH_COLUMN:
        matchColumns = s_arg[i_arg].n_items-1;
        matchColumn = s_arg[i_arg].list+1;
        break;
      case CLO_INDEX_COLUMN:
        indexColumn = 1;
        break;
      case CLO_NO_OLDCOLUMNNAMES:
        noOldColumnNamesColumn = 1;
        break;
      case CLO_VERBOSE:
        verbose=1;
        break;
      case CLO_ASCII:
        ascii=1;
        break;
      case CLO_DIGITS:
        if (!(get_long(&digits, s_arg[i_arg].list[1])))
          bomb("no string given for option -digits", USAGE);
        break;
      case CLO_COLUMNROOT:
        if (!(Root=s_arg[i_arg].list[1]))
          SDDS_Bomb("No root string given");
        break;
      case CLO_SYMBOL:
        if (!(Symbol=s_arg[i_arg].list[1]))
          SDDS_Bomb("No symbol string given");
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_OLDCOLUMNNAMES:
        if (!(oldColumnNames=s_arg[i_arg].list[1]))
          SDDS_Bomb("No oldColumnNames string given");
        break;
      case CLO_NEWCOLUMNNAMES:
        if (s_arg[i_arg].n_items!=2 ||
            SDDS_StringIsBlank(newColumnNamesColumn = s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalid -newColumnNames syntax/value");
        break;
      default:
        bomb("unrecognized option given", USAGE);
      }
    }
    else {
      if (!inputfile)
        inputfile = s_arg[i_arg].list[0];
      else if (!outputfile)
        outputfile = s_arg[i_arg].list[0];
      else
        bomb("too many filenames given", USAGE);
    }
  }

  processFilenames("sddstranpose", &inputfile, &outputfile, pipeFlags, noWarnings, &tmpfile_used);
  if (newColumnNamesColumn && Root) 
    SDDS_Bomb("-root and -newColumnNames are incompatible");
  
  if (!SDDS_InitializeInput(&inputPage, inputfile) ||
      !(inputParameterName=(char**)SDDS_GetParameterNames(&inputPage, &inputParameters)) ||
      !SDDS_GetDescription(&inputPage, &inputDescription, &inputContents))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if (matchColumns) 
    inputColumnName = getMatchingSDDSNames(&inputPage, matchColumn, matchColumns, &inputColumns, SDDS_MATCH_COLUMN); 
  else {
    if (!(inputColumnName=(char**)SDDS_GetColumnNames(&inputPage, &inputColumns)))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  
  inputDoubleColumns=0;
  inputStringColumns=0;
  inputDoubleColumnName=(char**)malloc(inputColumns*sizeof(char*));
  inputStringColumnName=(char**)malloc(inputColumns*sizeof(char*));
  inputRows = 0;
  /***********                                  \
   * read data *
   \***********/
  while (0<SDDS_ReadTable(&inputPage)) {
    ipage ++;
#if defined(DEBUG)
    fprintf(stderr, "working on page %ld\n", ipage);
#endif
    if (ipage==1) {
      SDDS_DeferSavingLayout(1);
      if( !SDDS_SetColumnFlags(&inputPage, 0))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      /* count the string and numerical columns in the input file */
      for (i=0;i<inputColumns;i++) {
        if ( SDDS_NUMERIC_TYPE( columnType = SDDS_GetColumnType( &inputPage, i))) {
          inputDoubleColumnName[inputDoubleColumns]=inputColumnName[i];
          inputDoubleColumns++;
        }
      }
      for (i=0; i<inputPage.layout.n_columns; i++) {
        if (inputPage.layout.column_definition[i].type == SDDS_STRING ) {
          inputStringColumnName[inputStringColumns] = inputPage.layout.column_definition[i].name;
          inputStringColumns++;
        }
      }
      if( !(inputRows=SDDS_CountRowsOfInterest(&inputPage)))
        SDDS_Bomb("No rows in dataset.");
    }
    else {
      /* these statements are executed on the subsequent pages */
      if (inputRows != SDDS_CountRowsOfInterest(&inputPage)) {
        SDDS_Bomb("Datasets don't have the same number of rows.\nProcessing stopped before reaching the end of the input file.");
      }
    }
#if defined(DEBUG)
    fprintf(stderr, "row flags set\n");
#endif
    if (inputDoubleColumns == 0)
      SDDS_Bomb("No numerical columns in file.");
    if ((ipage==1) && verbose) {
      fprintf(stderr, "No. of double/float/integer columns: %ld.\n", inputDoubleColumns);
      fprintf(stderr, "No. of string columns: %ld.\n", inputStringColumns);
      fprintf(stderr, "No. of rows: %ld.\n", inputRows);
    }
    /****************\
     * transpose data *
     \****************/
    if (inputDoubleColumns) {
      if (ipage == 1) {
        m_alloc(&RInv, inputRows, inputDoubleColumns);
        m_alloc(&R, inputDoubleColumns, inputRows);
      }
      for (col=0;col<inputDoubleColumns;col++){
        if (!(R->a[col]=(double*)SDDS_GetColumnInDoubles(&inputPage, inputDoubleColumnName[col]))) {
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (verbose) {
        m_show(R, "%9.6le ", "Transpose of input matrix:\n", stdout);
      }
      m_trans(RInv, R);
    }
    /***************************\
     * determine existence of    *
     * transposed string columns *
     \***************************/
    if (ipage == 1) {
      OldStringColumnsDefined=0;
      switch(SDDS_CheckParameter(&inputPage, OLD_STRING_COLUMN_NAMES, NULL, SDDS_STRING, NULL)){
      case SDDS_CHECK_OKAY:
        OldStringColumnsDefined=1;
        break;
      case SDDS_CHECK_NONEXISTENT:
        break;
      case SDDS_CHECK_WRONGTYPE:
      case SDDS_CHECK_WRONGUNITS:
        fprintf(stderr, "Something wrong with parameter OldStringColumns.\n"); 
        exit(1);
        break;
      }
      if (OldStringColumnsDefined){
        /* decompose OldStringColumns into names of string columns for the output file */
        if (!SDDS_GetParameter(&inputPage, OLD_STRING_COLUMN_NAMES, &inputStringRows))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (verbose) {
          fprintf(stderr, "Parameter OldStringColumns: %s.\n", inputStringRows);
        }
        outputStringColumnName=(char**)malloc(sizeof(char*));
        outputStringColumns=0;
        buffer_size=BUFFER_SIZE_INCREMENT;
        buffer=(char*)malloc(sizeof(char)*buffer_size);
        while ( 0 <= (token_length = SDDS_GetToken(inputStringRows, buffer, BUFFER_SIZE_INCREMENT))){
          if (!token_length)
            SDDS_Bomb("A null string was detected in parameter OldStringColumns.\n");
          if (!SDDS_CopyString(&outputStringColumnName[outputStringColumns], buffer))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (verbose) {
            fprintf(stderr, "Output string column: %s\n", outputStringColumnName[outputStringColumns]);
          }
          outputStringColumns++;
        }
      }
    }
    
    /*********************\
     * define output page *
     \*********************/
    if ( ipage == 1 ) {
      outputRows = inputDoubleColumns;
      outputDoubleColumns = inputRows;
      if (inputDescription){
        outputDescription = (char*) malloc( sizeof(char) * (strlen("Transpose of ") + strlen(inputDescription) + 1));
        strcat(strcpy(outputDescription, "Transpose of "), inputDescription);
        if (!SDDS_InitializeOutput(&outputPage, ascii?SDDS_ASCII:SDDS_BINARY, 1, outputDescription, inputContents, outputfile))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      else {
        if (!SDDS_InitializeOutput(&outputPage, ascii?SDDS_ASCII:SDDS_BINARY, 1, NULL, NULL, outputfile))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }    
      
      /***********************************\
       * define names for double columns *
       \***********************************/
      if (!Root && inputStringColumns ) {
        /* use specified string column, or first string column encountered */
        if (!newColumnNamesColumn)
          /* first string column encountered */
          outputDoubleColumnName = (char**) SDDS_GetColumn(&inputPage, inputStringColumnName[0]);
        else {
          /* use specified string column */
          if (SDDS_CheckColumn(&inputPage, newColumnNamesColumn, NULL, SDDS_STRING, stderr)!=SDDS_CHECK_OKAY)
            SDDS_Bomb("column named with -newColumnNames does not exist in input");
          outputDoubleColumnName = (char**)SDDS_GetColumn(&inputPage, newColumnNamesColumn);
        }
        for (i=1; i<inputRows; i++) {
          
          if (match_string(outputDoubleColumnName[i-1], outputDoubleColumnName+i, inputRows-i, EXACT_MATCH)>=0) {
            fprintf(stderr, "Error, duplicate %s found in input file string column %s, can not be used as output column names\n",  outputDoubleColumnName[i-1], newColumnNamesColumn ? newColumnNamesColumn : inputStringColumnName[0]);
            exit(1);
          }
        }
      }
      else {
        /* use command line options to produce column names in the output file */
        outputDoubleColumnName = (char**) malloc( outputDoubleColumns * sizeof(char*) );
        digits = MAX(digits, log10(inputRows) + 1);
        if (!Root){
          Root = (char*) malloc( sizeof(char) * (strlen("Column")+1) );
          strcpy(Root, "Column");
        }
        if (outputDoubleColumns!=1) {
          for ( i=0; i < outputDoubleColumns; i++){
            outputDoubleColumnName[i] = (char*) malloc( sizeof(char) * (strlen(Root)+digits+1));
            sprintf(format, "%s%%0%ldld", Root, digits);
            sprintf(outputDoubleColumnName[i], format, i);
          }
        }
        else {/* only one row to transpose */
          outputDoubleColumnName[0] = (char*) malloc( sizeof(char) * (strlen(Root)+1));
          strcpy( outputDoubleColumnName[0], Root);
        }
      }
      
      
      /*************************\
       * define string columns *
       \*************************/
      if (OldStringColumnsDefined) {
        if (!SDDS_DefineSimpleColumns(&outputPage, outputStringColumns, 
                                      outputStringColumnName, NULL, SDDS_STRING))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      else {
        /* by default there should be at least one string column, that of the old column names. */
        if (!noOldColumnNamesColumn) {
          outputStringColumns = 1;
          outputStringColumnName = (char**) malloc( sizeof(char*));
          if (oldColumnNames) {
            /* commanline option specification */
            outputStringColumnName[0] = oldColumnNames;
          }
          else {
            outputStringColumnName[0] = (char*) malloc( sizeof(char) * (strlen("OldColumnNames") + 1));
            strcpy(outputStringColumnName[0], "OldColumnNames");
          }
          if ( 0 > SDDS_DefineColumn(&outputPage, outputStringColumnName[0], NULL, NULL, NULL, NULL, SDDS_STRING, 0))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (indexColumn && !SDDS_DefineSimpleColumn(&outputPage, "Index", NULL, SDDS_LONG))
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      /*************************\
       * define double columns *
       \*************************/
      for ( i=0; i < outputDoubleColumns; i++)
        if (Symbol){
          if (0>SDDS_DefineColumn(&outputPage, outputDoubleColumnName[i], Symbol, NULL, NULL, 
                                  NULL, SDDS_DOUBLE, 0))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        else {
          if (0>SDDS_DefineColumn(&outputPage, outputDoubleColumnName[i], NULL, NULL, NULL, 
                                  NULL, SDDS_DOUBLE, 0))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      /********************************\
       * define string parameters       *
       * i.e. transposed string columns *
       \********************************/
      if ( inputStringColumns>1 ) {
        if (0>SDDS_DefineParameter(&outputPage, OLD_STRING_COLUMN_NAMES, 
                                   NULL, NULL, "Transposed string columns", NULL, SDDS_STRING, 
                                   NULL))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for ( i=0; i < inputStringColumns; i++){
          if (0>SDDS_DefineParameter(&outputPage, inputStringColumnName[i], NULL, NULL, "Transposed string column data", NULL, 
                                     SDDS_STRING, NULL))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      
      /*************************\
       * transfer parameters not *
       * associated with old     *
       * string columns          *
       \*************************/
      if (inputParameters) {
        for ( i=0; i < inputParameters; i++) {
          if ( (0 > match_string(inputParameterName[i], outputStringColumnName, outputStringColumns, 0) &&
                strcasecmp(inputParameterName[i], OLD_STRING_COLUMN_NAMES)))
            if ( 0 > SDDS_TransferParameterDefinition(&outputPage, &inputPage, inputParameterName[i], NULL))
              SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      
      /***************\
       * write layout *
       \***************/
      SDDS_DeferSavingLayout(0);
      /* if InputFile is not already transfered ot the output file, then create it. */
      switch( SDDS_CheckParameter(&outputPage, "InputFile", NULL, SDDS_STRING, NULL) ) {
      case SDDS_CHECK_NONEXISTENT:
        if (0>SDDS_DefineParameter(&outputPage, "InputFile", NULL, NULL, "Original matrix file", NULL, SDDS_STRING, NULL))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      default:
        break;
      }
      if (!SDDS_WriteLayout(&outputPage) )
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
#if defined(DEBUG)
    fprintf(stderr, "table layout defined\n");
#endif

    if (!SDDS_StartTable(&outputPage, outputRows) )
      SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (ipage == 1) {
      if (!SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 
                              "InputFile", inputfile?inputfile:"pipe", NULL))
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    /***************************************\
     * assign string columns    from input *
     * to     string parameters in output  *
     \**************************************/
    if ( inputStringColumns > 1) {
      for ( i=0; i < inputStringColumns; i++){
        columnOfStrings = (char**) SDDS_GetColumn(&inputPage, inputStringColumnName[i]);
        stringParameter = JoinStrings(columnOfStrings, inputRows, BUFFER_SIZE_INCREMENT);
        if ( !SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 
                                 inputStringColumnName[i], stringParameter, NULL))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        free(columnOfStrings);
        free(stringParameter);
      }
      outputStringRows = JoinStrings(inputStringColumnName, inputStringColumns, BUFFER_SIZE_INCREMENT);
      if (!SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 
                              OLD_STRING_COLUMN_NAMES, outputStringRows, NULL))
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
#if defined(DEBUG)
    fprintf(stderr, "string parameters assigned\n");
#endif
    
    if (inputParameters){
      for ( i=0; i < inputParameters; i++){
        if ( (0 > match_string(inputParameterName[i], outputStringColumnName, outputStringColumns, 0) &&
              strcasecmp(inputParameterName[i], OLD_STRING_COLUMN_NAMES))) {
          parameterPointer = (void*) SDDS_GetParameter(&inputPage, inputParameterName[i], NULL);
          if (!SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, 
                                  inputParameterName[i], parameterPointer, NULL))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          free(parameterPointer);
        }
      }
    }
#if defined(DEBUG)
    fprintf(stderr, "input parameters assigned\n");
#endif
    
    /**********************************\
     * assign data to                   *
     * output table part of data set    *
     \**********************************/
    if (outputRows) {
      /***************************\
       * assign string column data *
       \***************************/
      if (OldStringColumnsDefined){
        for ( i=0 ; i < outputStringColumns; i++){
          if (!SDDS_GetParameter(&inputPage, outputStringColumnName[i], &stringParameter))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          stringArray=TokenizeString(stringParameter, outputRows);
          if (!SDDS_SetColumn(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, 
                              stringArray, outputRows, outputStringColumnName[i]))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      else {
        if (!noOldColumnNamesColumn && !SDDS_SetColumn(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, 
                            inputDoubleColumnName, outputRows, outputStringColumnName[0]))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
#if defined(DEBUG)
      fprintf(stderr, "string data columns assigned\n");
#endif
      /***************************\
       * assign double column data *
       \***************************/
      for ( i=0 ; i < outputDoubleColumns; i++) /* i is the row index */
        if (!SDDS_SetColumn(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, 
                            RInv->a[i], outputRows, outputDoubleColumnName[i]))
          SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (indexColumn) {
        for (i=0; i<outputRows; i++)
          if (!SDDS_SetRowValues(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "Index", i, NULL))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
#if defined(DEBUG)
      fprintf(stderr, "double data columns assigned\n");
#endif
    }
    
#if defined(DEBUG)
    fprintf(stderr, "data assigned\n");
#endif
    if (!SDDS_WriteTable(&outputPage))
      SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
#if defined(DEBUG)
    fprintf(stderr, "data written out\n");
#endif
  }    
  if (inputDoubleColumns) {
    m_free(&RInv);
    m_free(&R);
  }
  if (inputColumnName) {
    SDDS_FreeStringArray(inputColumnName, inputColumns);
    free(inputColumnName);
  }
  if (inputStringColumns)
    free(inputStringColumnName);
  if (inputDescription)
    free(inputDescription);
  if (inputParameterName) {
    SDDS_FreeStringArray(inputParameterName, inputParameters);
    free(inputParameterName);
  }
  if (outputDoubleColumns) {
    SDDS_FreeStringArray(outputDoubleColumnName, outputDoubleColumns);
    free(outputDoubleColumnName);
  }
  
  if (!SDDS_Terminate(&inputPage) || !SDDS_Terminate(&outputPage))
    SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (tmpfile_used && !replaceFileAndBackUp(inputfile, outputfile)) 
    exit(1);
  return(0);
}

char **TokenizeString(char *source, long n_items){
  char *buffer;
  long buflen;
  char *ptr;
  char **string_array;
  long i;
  
  if (!source)
    return(NULL);
  ptr=source;
  string_array=(char**)malloc(sizeof(char*)*n_items);
  buflen=strlen(source)+1;
  buffer=(char*)malloc(sizeof(char)*(buflen));
  for (i=0;i<n_items;i++){
    if (SDDS_GetToken(ptr, buffer, buflen)!=-1)
      SDDS_CopyString(&string_array[i], buffer);
    else
      SDDS_CopyString(&string_array[i], "");
  }
  return(string_array);
}

char *JoinStrings(char **source, long n_items, long buflen_increment) {
  char *buffer;
  char *ptr;
  long buffer_size, bufferLeft, bufferUsed;
  long i, slen;
  char *sptr;
  
  buffer_size = buflen_increment;
  buffer = (char*)malloc(sizeof(char)*buffer_size);
  buffer[0]=0;
  ptr = buffer;
  bufferLeft = buffer_size-2;
  bufferUsed = 0;
  for (i=0; i<n_items; i++) {
    sptr = source[i];
    slen = strlen(sptr);
    while ((slen+5)>bufferLeft) {
      buffer = trealloc(buffer, sizeof(char)*(buffer_size+=buflen_increment));
      bufferLeft += buflen_increment;
      ptr = buffer+bufferUsed;
    }
    if (i) {
      slen++;
      *ptr++ = ' ';
    }
    *ptr++ = '"';
    while (*sptr)
      *ptr++ = *sptr++;
    *ptr++ = '"';
    *ptr = 0;
    bufferLeft -= slen+2;
    bufferUsed += slen+2;
  }
  
  return(buffer);
}

