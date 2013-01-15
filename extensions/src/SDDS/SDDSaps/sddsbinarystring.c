/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsbinarystring.c
 * purpose: convert integer type columns into binary string
 * future capabilities: convert to octal and hex
 *                      convert back to binary
 *                      create separate string columns for sections of the binary data.
 $Log: not supported by cvs2svn $
 Revision 1.3  2007/03/28 22:59:45  emery
 Allows SDDS_ULONG as integer type for conversion.

 Revision 1.2  2006/08/08 17:06:12  emery
 Fixed the cvs $log comment. Removed all but the *data
 variable from the int32_t type statement. Initialized
 character values with 0 instead of NULL (to remove
 complication warnings.)

 Revision 1.1  2006/08/08 16:38:21  emery
 Shang's first version.
 */


#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include "ctype.h"

#define SET_COLUMN 0
#define SET_PIPE 1
#define N_OPTIONS 2

char *option[N_OPTIONS] = {
  "column", "pipe",
    } ;

char *USAGE = "sddsbinarystring [<source-file>] [<target-file>]\n\
[-pipe=[input][,output]] [-column=<list of column names>] \n\n\
sddsbinarystring   converts the integer columns into binary string values.\n\
-column            list of columns to be convert, wildcards accepted. The binary string will be \n\
                   written into <oldColumnName>BinaryString columns. \n\n\
Program by Hairong. (This is version 1, August 2006.)\n";


int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_dataset, SDDS_orig;
  long i, j, i_row, i_arg;
  SCANNED_ARG *s_arg;
  long tmpfile_used, noWarnings, columnMatches, integerColumns, digits;
  long index, type, rows, columns;
  int32_t *data;
  short *integerType;
  char *input, *output, **columnName, **columnMatch, **integerColumn, buff[1024], **binaryString;
  unsigned long pipeFlags;
  
  tmpfile_used = 0;
  pipeFlags = 0;
  input = output = NULL;
  data = NULL;
  noWarnings = 0;
  
  binaryString = NULL;
  buff[0] = 0;
  
  columns = columnMatches = integerColumns = 0;
  columnName = columnMatch = integerColumn = NULL;
  integerType = NULL;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2)
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COLUMN:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        columnMatch = tmalloc(sizeof(*columnMatch)*(columnMatches = s_arg[i_arg].n_items-1));
        for (i=0; i<columnMatches; i++)
          columnMatch[i] = s_arg[i_arg].list[i+1];
        break; 
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break; 
      default:
        fprintf(stderr, "Error (%s): unknown switch: %s\n", argv[0], s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    }  else {
      if (input==NULL)
        input = s_arg[i_arg].list[0];
      else if (output==NULL)
        output = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames");
    }
  }
  
  processFilenames("sddsbinarystring", &input, &output, pipeFlags, noWarnings, &tmpfile_used);
  
#ifdef DEBUG
  fprintf(stderr, "Initializing input and output files.\n");
#endif
  if (!SDDS_InitializeInput(&SDDS_orig, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!columnMatch) 
    columnName = SDDS_GetColumnNames(&SDDS_orig, &columns);
  else {
    columnName = getMatchingSDDSNames(&SDDS_orig, columnMatch, columnMatches, &columns, SDDS_MATCH_COLUMN);
    free(columnMatch);
  }
  
  for (i=0; i<columns; i++) {
    index = SDDS_GetColumnIndex(&SDDS_orig, columnName[i]);
    type = SDDS_GetColumnType(&SDDS_orig, index);
    if (type==SDDS_SHORT || type==SDDS_LONG || type==SDDS_ULONG) {
      integerColumn = SDDS_Realloc(integerColumn, sizeof(*integerColumn)*(integerColumns+1));
      integerType = SDDS_Realloc(integerType, sizeof(*integerType)*(integerColumns+1));
      integerColumn[integerColumns] = NULL;
      
      SDDS_CopyString(&integerColumn[integerColumns], columnName[i]);
      integerType[integerColumns] = type;
      integerColumns ++;
    }
  }
  SDDS_FreeStringArray(columnName, columns);
  free(columnName);
  
  if (!integerColumns) {
    fprintf(stderr, "There are no integer columns in %s for converting.\n", input);
    exit (1);
  }
  if (!SDDS_InitializeCopy(&SDDS_dataset, &SDDS_orig, output, "w")) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  for (i=0; i<integerColumns; i++) {
    sprintf(buff, "%sBinaryString", integerColumn[i]);
    if (!SDDS_DefineSimpleColumn(&SDDS_dataset, buff, NULL, SDDS_STRING))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_WriteLayout(&SDDS_dataset)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
#ifdef DEBUG
  fprintf(stderr, "Reading integer data from input file.\n");
#endif
  while (SDDS_ReadPage(&SDDS_orig)>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDS_orig))<0)  
      continue;
    binaryString = malloc(sizeof(*binaryString)*rows);
    if (!SDDS_CopyPage(&SDDS_dataset, &SDDS_orig))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<integerColumns; i++) {
      if (!(data=SDDS_GetColumnInLong(&SDDS_orig, integerColumn[i])))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (integerType[i]==SDDS_SHORT)
        digits = 16;
      else
        digits = 32;
      for (i_row=0; i_row<rows; i_row++) {
        binaryString[i_row] = NULL;
        binaryString[i_row] = (char*)malloc(sizeof(**binaryString)*(digits+1));
        
        for (j=0; j<digits; j++) 
          binaryString[i_row][digits-1-j] = (data[i_row] >> j & 0x1) ? '1':'0';
        binaryString[i_row][digits] = 0;
      }
      sprintf(buff, "%sBinaryString", integerColumn[i]);
      if (!SDDS_SetColumn(&SDDS_dataset, SDDS_BY_NAME,
                          binaryString, rows, buff))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      free(data);
      data = NULL;
      SDDS_FreeStringArray(binaryString, rows);
    }
    free(binaryString);
    binaryString = NULL;
    if (!SDDS_WritePage(&SDDS_dataset))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDS_orig) || !SDDS_Terminate(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (tmpfile_used && !replaceFileAndBackUp(input, output))
    exit(1);
  SDDS_FreeStringArray(integerColumn, integerColumns);
  free(integerColumn);
  free_scanargs(&s_arg, argc);
  return 0;
  
}

