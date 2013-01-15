/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* 
 $Log: not supported by cvs2svn $
 Revision 1.22  2009/01/30 18:07:42  soliday
 Updated to accept lines 10 times as long as before.

 Revision 1.21  2008/11/20 16:12:37  soliday
 Modified the commentCharacters code again to fix an issue compiling on Linux.

 Revision 1.20  2008/11/20 15:51:09  soliday
 The commentCharacters variable is now set to NULL by default.

 Revision 1.19  2008/10/16 16:36:01  soliday
 Added the skipcolumn and commentCharacters options.

 Revision 1.18  2006/02/13 21:55:09  soliday
 Fixed problem reading input files if they do not end with a newline character.

 Revision 1.17  2005/11/04 22:46:11  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2004/04/27 17:26:28  soliday
 Added the count suboption to the -column and -parameter options. This is used
 to create multiple columns and/or parameters with the same prefix and
 incrementing numerical suffixes.

 Revision 1.15  2004/01/06 23:25:03  soliday
 Updated so that it only allocates the required memory for string values
 instead of the maximum allowed for an SDDS string value.

 Revision 1.14  2003/12/22 21:14:27  soliday
 By default it now uses any white space as the separator. This is useful if
 there is a mix of white space characters in the ascii plaindata file.

 Revision 1.13  2003/09/02 19:16:02  soliday
 Cleaned up code for Linux.

 Revision 1.12  2002/08/14 17:12:38  soliday
 Added Open License

 Revision 1.11  2002/06/04 21:34:00  soliday
 Added the -skiplines option.

 Revision 1.10  2001/11/28 23:52:38  shang
 added -fillin feature for ascii input file

 Revision 1.9  2001/11/28 19:40:15  borland
 Added code to insert terminating 0 in strings read from binary mode input files.
 Also added code to test strings to see if they are too long for the buffer.

 Revision 1.8  2001/01/23 19:14:56  soliday
 Standardized usage message.

 Revision 1.7  2000/04/13 17:08:57  soliday
 Added Borland C define statement for setmode.

 Revision 1.6  2000/02/10 19:16:04  soliday
 Added SDDS_LengthenTable command for files with more than 10000 rows.

 Revision 1.5  2000/02/08 17:46:26  soliday
 Fixed problem with separator now working unless it was a space.

 Revision 1.4  1999/12/07 17:20:32  soliday
 Now works with WIN32

 Revision 1.3  1999/11/15 22:55:25  soliday
 It now accepts plaindata files without row counts.

 Revision 1.2  1999/11/11 15:50:53  soliday
 Fixed some bugs and made some changes requested by Borland

 Revision 1.1  1999/11/02 19:39:47  soliday
 First c version

 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include <ctype.h>
#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif

#define ASCII_MODE 0
#define BINARY_MODE 1
#define MODES 2
static char *mode_name[MODES] = {
  "ascii", "binary",
};

#define TYPE_SHORT 0
#define TYPE_LONG 1
#define TYPE_FLOAT 2
#define TYPE_DOUBLE 3
#define TYPE_STRING 4
#define TYPE_CHARACTER 5
#define DATATYPES 6
static char *type_name[DATATYPES] = {
  "short", "long", "float", "double", "string", "character",
};

#define HEADER_UNITS 0
#define HEADER_DESCRIPTION 1
#define HEADER_SYMBOL 2
#define HEADER_COUNT 3
#define HEADERELEMENTS 4
static char *header_elements[HEADERELEMENTS] = {
  "units", "description", "symbol", "count"
};

typedef struct {
  void *values;
  long elements;
  char **stringValues;
  char *units;
  char *description;
  char *symbol;
  char *name;
  long type;
  short skip;
} COLUMN_DATA_STRUCTURES;

typedef struct {
  char *units;
  char *description;
  char *symbol;
  char *name;
  long type;
} PARAMETER_DATA_STRUCTURES;

#define SET_INPUTMODE 0
#define SET_OUTPUTMODE 1
#define SET_SEPARATOR 2
#define SET_NOROWCOUNT 3
#define SET_PARAMETER 4
#define SET_COLUMN 5
#define SET_PIPE 6
#define SET_NOWARNINGS 7
#define SET_ORDER 8
#define SET_FILLIN 9
#define SET_SKIPLINES 10
#define SET_SKIPCOLUMN 11
#define SET_COMMENT 12
#define N_OPTIONS 13

char *option[N_OPTIONS] = {
    "inputMode", "outputMode", "separator", "noRowCount", 
    "parameter", "column", "pipe", "nowarnings", "order","fillin",
    "skiplines", "skipcolumn", "commentCharacters",
};

#define ROW_ORDER 0
#define COLUMN_ORDER 1
#define ORDERS 2
static char *order_names[ORDERS] = {
  "rowMajor", "columnMajor",
};

char *USAGE = "plaindata2sdds <input> <output> \n\t\
 [-pipe=[input][,output]] \n\t\
 [-inputMode=<ascii|binary>] \n\t\
 [-outputMode=<ascii|binary>] \n\t\
 [-separator=<char>] \n\t\
 [-commentCharacters=<chars>] \n\t\
 [-noRowCount] \n\t\
 [-order=<rowMajor|columnMajor>] \n\t\
 [-parameter=<name>,<type>[,units=<string>][,description=<string>][,symbol=<string>][,count=<integer>]...] \n\t\
 [-column=<name>,<type>[,units=<string>][,description=<string>][,symbol=<string>][,count=<integer>]...] \n\t\
 [-skipcolumn=type] \n\t\
 [-nowarnings] [-fillin]\n\t\
 [-skiplines=<integer>]\n\n\
-inputMode        The plain data file can be read in ascii or binary format.\n\
-outputMode       The SDDS data file can be written in ascii or binary format.\n\
-separator        In ascii mode the columns of the plain data file are\n\
                  separated by the given character. Be default it uses any\n\
                  combination of whitespace characters.\n\
-noRowCount       The number of rows is not included in the plain data file.\n\
                  If the plain data file is a binary file then the row count must be included.\n\
-order            Row major order is the default. Here each row of the plain data file\n\
                  consists of one element from each column. In column major order\n\
                  each column is located entirely on one row.\n\
-parameter        Add this option for each parameter in the plain data file.\n\
-fillin           fill in blank for string column or 0 for numeric column when there \n\
                  is no data for it.\n\
-column           Add this option for each column in the plain data file.\n\
-skipcolumn       Add this option to skip over a column in the plain data file.\n\
-skiplines        Add this option to skip header lines.\n\n\
Program by Robert Soliday. ("__DATE__")\n";

void SetColumnData(long type, SDDS_DATASET *dataset, void *values, long rows, long index);
void *AllocateColumnData(long type, void *values, long rows);
char **AllocateColumnStringData(char **values, long rows, long previous_rows);
long getToken(char *s, char *buffer, long buflen, char separator, long whitespace);
void ConvertDNotationToENotation (char *line);


/* ********** */

int main(int argc, char **argv)
{
  FILE *fileID;
  COLUMN_DATA_STRUCTURES *columnValues;
  PARAMETER_DATA_STRUCTURES *parameterValues;

  SDDS_DATASET SDDS_dataset;
  SCANNED_ARG *s_arg;
  long i, j, k, n, i_arg, rows;
  long maxRows=10000, initRows=10000, row, par, col, page, size, readline=1, fillin=0;
  int32_t ptrSize=0;
  char *input, *output, s[1024], *ptr, *ptr2, data[10240],temp[10240];
  unsigned long pipeFlags=0;
  long noWarnings=0, tmpfile_used=0, columnOrder=0, whitespace=1;
  short shortValue;
  int32_t longValue;
  float floatValue;
  double doubleValue;
  char stringValue[SDDS_MAXLINE];
  char characterValue;
  char buffer[124], buffer2[124];
  long *parameterIndex, *columnIndex;

  long binary=0, noRowCount=0, inputBinary=0, count=0;
  char separator;
  char commentCharacters[20];
  short checkComment=0;
  short commentFound;
  long parameters=0, columns=0;
  long skiplines=0;
  short abort=0, recover=1;

  input = output = NULL;
  separator = ' ';
  columnValues = NULL;
  parameterValues = NULL;

  parameterIndex = columnIndex = NULL;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_OUTPUTMODE:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -outputMode syntax");
	switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
	case ASCII_MODE:
	  binary = 0;
	  break;
	case BINARY_MODE:
	  binary = 1;
	  break;
	default:
	  SDDS_Bomb("invalid -outputMode syntax");
	  break;
	}
	break;
      case SET_INPUTMODE:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -inputMode syntax");
	switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
	case ASCII_MODE:
	  inputBinary = 0;
	  break;
	case BINARY_MODE:
	  inputBinary = 1;
	  break;
	default:
	  SDDS_Bomb("invalid -inputMode syntax");
	  break;
	}
	break;
      case SET_SEPARATOR:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -separator syntax");
	separator = s_arg[i_arg].list[1][0];
        whitespace = 0;
	break;
      case SET_COMMENT:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -commentCharacters syntax");
	sprintf(commentCharacters, "%s", s_arg[i_arg].list[1]);
        checkComment=1;
	break;
      case SET_FILLIN:
        fillin=1;
        break;
      case SET_NOROWCOUNT:
	if (s_arg[i_arg].n_items!=1)
	  SDDS_Bomb("invalid -noRowCount syntax");
	noRowCount = 1;
	break;
      case SET_ORDER:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -order syntax");
	switch (match_string(s_arg[i_arg].list[1], order_names, ORDERS, 0)) {
	case ROW_ORDER:
	  columnOrder = 0;
	  break;
	case COLUMN_ORDER:
	  columnOrder = 1;
	  break;
	default:
	  SDDS_Bomb("invalid -order syntax");
	  break;
	}
	break;
      case SET_PARAMETER:
	if (s_arg[i_arg].n_items<3)
	  SDDS_Bomb("invalid -parameter syntax");
        count=1;
	parameters++;
	parameterValues = trealloc(parameterValues, sizeof(*parameterValues)*(parameters));
        SDDS_CopyString(&parameterValues[parameters-1].name, s_arg[i_arg].list[1]);
	parameterValues[parameters-1].units = NULL;
	parameterValues[parameters-1].description = NULL;
	parameterValues[parameters-1].symbol = NULL;
	switch (match_string(s_arg[i_arg].list[2], type_name, DATATYPES, 0)) {
	case TYPE_SHORT:
	  parameterValues[parameters-1].type = SDDS_SHORT;
	  break;
	case TYPE_LONG:
	  parameterValues[parameters-1].type = SDDS_LONG;
	  break;
	case TYPE_FLOAT:
	  parameterValues[parameters-1].type = SDDS_FLOAT;
	  break;
	case TYPE_DOUBLE:
	  parameterValues[parameters-1].type = SDDS_DOUBLE;
	  break;
	case TYPE_STRING:
	  parameterValues[parameters-1].type = SDDS_STRING;
	  break;
	case TYPE_CHARACTER:
	  parameterValues[parameters-1].type = SDDS_CHARACTER;
	  break;
	default:
	  SDDS_Bomb("invalid -parameter type");
	  break;
	}
	for (i=3;i<s_arg[i_arg].n_items;i++) {
	  if (!(ptr=strchr(s_arg[i_arg].list[i], '=')))
	    SDDS_Bomb("invalid -parameter syntax");
	  *ptr++ = 0;	
	  switch (match_string(s_arg[i_arg].list[i], header_elements, HEADERELEMENTS, 0)) {
	  case HEADER_UNITS:
            SDDS_CopyString(&parameterValues[parameters-1].units, ptr);
	    break;
	  case HEADER_DESCRIPTION:
            SDDS_CopyString(&parameterValues[parameters-1].description, ptr);
	    break;
	  case HEADER_SYMBOL:
            SDDS_CopyString(&parameterValues[parameters-1].symbol, ptr);
	    break;
	  case HEADER_COUNT:
	    if (sscanf(ptr, "%ld", &count)!=1 ||
                count<=0)
              SDDS_Bomb("invalid parameter count value");
            sprintf(buffer, "%s", parameterValues[parameters-1].name);
            sprintf(buffer2, "%s1", buffer);
            free(parameterValues[parameters-1].name);
            SDDS_CopyString(&parameterValues[parameters-1].name, buffer2);
	    break;
	  default:
	    SDDS_Bomb("invalid -parameter syntax");
	    break;
	  } 
	}
        
        for (i=2;i<=count;i++) {
          parameters++;
          parameterValues = trealloc(parameterValues, sizeof(*parameterValues)*(parameters));
          sprintf(buffer2, "%s%ld", buffer, i);
          SDDS_CopyString(&parameterValues[parameters-1].name, buffer2);
          parameterValues[parameters-1].units = NULL;
          parameterValues[parameters-1].description = NULL;
          parameterValues[parameters-1].symbol = NULL;
          parameterValues[parameters-1].type = parameterValues[parameters-2].type;
          if (parameterValues[parameters-2].units)
            SDDS_CopyString(&parameterValues[parameters-1].units, parameterValues[parameters-2].units);
          if (parameterValues[parameters-2].description)
            SDDS_CopyString(&parameterValues[parameters-1].description, parameterValues[parameters-2].description);
          if (parameterValues[parameters-2].symbol)
            SDDS_CopyString(&parameterValues[parameters-1].symbol, parameterValues[parameters-2].symbol);
        }
        
	break;
      case SET_COLUMN:
	if (s_arg[i_arg].n_items<3)
	  SDDS_Bomb("invalid -column syntax");
        count=1;
	columns++;
	columnValues = trealloc(columnValues, sizeof(*columnValues)*(columns));
        SDDS_CopyString(&columnValues[columns-1].name, s_arg[i_arg].list[1]);
	columnValues[columns-1].elements = 0;
	columnValues[columns-1].values = NULL;
	columnValues[columns-1].stringValues = NULL;
	columnValues[columns-1].units = NULL;
	columnValues[columns-1].description = NULL;
	columnValues[columns-1].symbol = NULL;
	columnValues[columns-1].skip = 0;

	switch (match_string(s_arg[i_arg].list[2], type_name, DATATYPES, 0)) {
	case TYPE_SHORT:
	  columnValues[columns-1].type = SDDS_SHORT;
	  break;
	case TYPE_LONG:
	  columnValues[columns-1].type = SDDS_LONG;
	  break;
	case TYPE_FLOAT:
	  columnValues[columns-1].type = SDDS_FLOAT;
	  break;
	case TYPE_DOUBLE:
	  columnValues[columns-1].type = SDDS_DOUBLE;
	  break;
	case TYPE_STRING:
	  columnValues[columns-1].type = SDDS_STRING;
	  break;
	case TYPE_CHARACTER:
	  columnValues[columns-1].type = SDDS_CHARACTER;
	  break;
	default:
	  SDDS_Bomb("invalid -column type");
	  break;
	}
	for (i=3;i<s_arg[i_arg].n_items;i++) {
	  if (!(ptr=strchr(s_arg[i_arg].list[i], '=')))
	    SDDS_Bomb("invalid -column syntax");
	  *ptr++ = 0;	
	  switch (match_string(s_arg[i_arg].list[i], header_elements, HEADERELEMENTS, 0)) {
	  case HEADER_UNITS:
            SDDS_CopyString(&columnValues[columns-1].units, ptr);
	    break;
	  case HEADER_DESCRIPTION:
            SDDS_CopyString(&columnValues[columns-1].description, ptr);
	    break;
	  case HEADER_SYMBOL:
            SDDS_CopyString(&columnValues[columns-1].symbol, ptr);
	    break;
	  case HEADER_COUNT:
	    if (sscanf(ptr, "%ld", &count)!=1 ||
                count<=0)
              SDDS_Bomb("invalid column count value");
            sprintf(buffer, "%s", columnValues[columns-1].name);
            sprintf(buffer2, "%s1", buffer);
            free(columnValues[columns-1].name);
            SDDS_CopyString(&columnValues[columns-1].name, buffer2);
	    break;
	  default:
	    SDDS_Bomb("invalid -column syntax");
	    break;
	  }
	}
        
        for (i=2;i<=count;i++) {
          columns++;
          columnValues = trealloc(columnValues, sizeof(*columnValues)*(columns));
          sprintf(buffer2, "%s%ld", buffer, i);
          SDDS_CopyString(&columnValues[columns-1].name, buffer2);
          columnValues[columns-1].elements = 0;
          columnValues[columns-1].values = NULL;
          columnValues[columns-1].stringValues = NULL;
          columnValues[columns-1].units = NULL;
          columnValues[columns-1].description = NULL;
          columnValues[columns-1].symbol = NULL;
          columnValues[columns-1].type = columnValues[columns-2].type;
          if (columnValues[columns-2].units)
            SDDS_CopyString(&columnValues[columns-1].units, columnValues[columns-2].units);
          if (columnValues[columns-2].description)
            SDDS_CopyString(&columnValues[columns-1].description, columnValues[columns-2].description);
          if (columnValues[columns-2].symbol)
            SDDS_CopyString(&columnValues[columns-1].symbol, columnValues[columns-2].symbol);
        }
        
	break;
      case SET_SKIPCOLUMN:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -skipcolumn syntax");
        count=1;
	columns++;
	columnValues = trealloc(columnValues, sizeof(*columnValues)*(columns));
        columnValues[columns-1].name = NULL;
	columnValues[columns-1].elements = 0;
	columnValues[columns-1].values = NULL;
	columnValues[columns-1].stringValues = NULL;
	columnValues[columns-1].units = NULL;
	columnValues[columns-1].description = NULL;
	columnValues[columns-1].symbol = NULL;
	columnValues[columns-1].skip = 1;

	switch (match_string(s_arg[i_arg].list[1], type_name, DATATYPES, 0)) {
	case TYPE_SHORT:
	  columnValues[columns-1].type = SDDS_SHORT;
	  break;
	case TYPE_LONG:
	  columnValues[columns-1].type = SDDS_LONG;
	  break;
	case TYPE_FLOAT:
	  columnValues[columns-1].type = SDDS_FLOAT;
	  break;
	case TYPE_DOUBLE:
	  columnValues[columns-1].type = SDDS_DOUBLE;
	  break;
	case TYPE_STRING:
	  columnValues[columns-1].type = SDDS_STRING;
	  break;
	case TYPE_CHARACTER:
	  columnValues[columns-1].type = SDDS_CHARACTER;
	  break;
	default:
	  SDDS_Bomb("invalid -skipcolumn type");
	  break;
	}        
	break;
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_NOWARNINGS:
	if (s_arg[i_arg].n_items!=1)
	  SDDS_Bomb("invalid -nowarnings syntax");
	noWarnings = 1;
	break;
      case SET_SKIPLINES:
	if (s_arg[i_arg].n_items!=2 ||
	    sscanf(s_arg[i_arg].list[1], "%ld", &skiplines)!=1 ||
	    skiplines<=0)
	  SDDS_Bomb("invalid -skiplines syntax");
	break;
      default:
	fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
	exit(1);
	break;
      }
    } else {
      if (input == NULL) {
	input = s_arg[i_arg].list[0];
      } else if (output == NULL) {
	output = s_arg[i_arg].list[0];
      } else {
	fprintf(stderr, "too many filenames");
	exit(1);
      }
    }
  }
  
  processFilenames("plaindata2sdds", &input, &output, pipeFlags, noWarnings, &tmpfile_used);

  if (!columns && !parameters)
    SDDS_Bomb("you must specify one of the -column or the -parameter options");

  if (skiplines && inputBinary)
    SDDS_Bomb("-skiplines does not work with binary input files");
  
  if (!input) {
    if (inputBinary) {
#if defined(_WIN32)
      if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
	fprintf(stderr, "error: unable to set stdin to binary mode\n");
	exit(1);
      }      
#endif
    }
    fileID = stdin;
  } else {
    if (!fexists(input)) {
      fprintf(stderr, "input file not found\n");
      exit(1);
    }
    if (inputBinary) {
      fileID = fopen(input, "rb");
    } else {
      fileID = fopen(input, "r");
    }
  }
  if (fileID == NULL) {
    fprintf(stderr, "unable to open input file for reading\n");
    exit(1);
  }
  
  if (!SDDS_InitializeOutput(&SDDS_dataset, binary?SDDS_BINARY:SDDS_ASCII,
                             1, NULL, NULL, output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    

  if (parameters) {
    parameterIndex = tmalloc(sizeof(*parameterIndex)*parameters);
  }
  if (columns) {
    columnIndex = tmalloc(sizeof(*columnIndex)*columns);
  }

  for (i=0; i<parameters; i++) {
    if ((parameterIndex[i] = 
         SDDS_DefineParameter(&SDDS_dataset, parameterValues[i].name, parameterValues[i].symbol,
			      parameterValues[i].units, parameterValues[i].description, NULL, 
                           parameterValues[i].type, 0))<0) {
      sprintf(s, "Problem defining parameter %s.", parameterValues[i].name);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  for (i=0; i<columns; i++) {
    if (columnValues[i].skip)
      continue;
    if ((columnIndex[i] = 
         SDDS_DefineColumn(&SDDS_dataset, columnValues[i].name, columnValues[i].symbol, \
			   columnValues[i].units, columnValues[i].description, NULL, 
                           columnValues[i].type, 0))<0) {
      sprintf(s, "Problem defining column %s.", columnValues[i].name);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }

  if (!SDDS_WriteLayout(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_StartPage(&SDDS_dataset, initRows))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);


  row = par = col = page = 0;
  while (inputBinary) {
    row = par = col = 0;
    
    if (fread(&rows,sizeof(rows),1,fileID) != 1) {
      if (page == 0) {
	SDDS_SetError("Unable to read number of rows");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
      } else {
        if (!SDDS_Terminate(&SDDS_dataset))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (k=0;k<columns;k++) {
          if (columnValues[k].type == SDDS_STRING) {
            SDDS_FreeStringArray(columnValues[k].stringValues,columnValues[k].elements);
          } else {
            free(columnValues[k].values);
          }
        }
	return 0;
      }
    }
    page++;

    for (par=0;par<parameters;par++) {
      
      switch (parameterValues[par].type) {
      case SDDS_SHORT:
	if (fread(&shortValue,2,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read short parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,shortValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_LONG:
	if (fread(&longValue,4,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read long parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,longValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_FLOAT:
	if (fread(&floatValue,4,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read float parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,floatValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_DOUBLE:
	if (fread(&doubleValue,8,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read double parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,doubleValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_STRING:
	if (fread(&size,4,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read string parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
        if (size>SDDS_MAXLINE-1)
          SDDS_Bomb("String is too long");
	if (size > 0) {
	  if (fread(&stringValue,size,1,fileID) != 1) {
	    SDDS_SetError("Unable to read string parameter");
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	  }
          stringValue[size] = 0;
	} else {
	  strcpy(stringValue,"");
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,stringValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_CHARACTER:
	if (fread(&characterValue,1,1,fileID) != 1) {
     	  SDDS_SetError("Unable to read character parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,characterValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      }
    }
    for (i=0;i<columns;i++) {
	if (rows > columnValues[i].elements) {
	  if (columnValues[i].type == SDDS_STRING) {
	    columnValues[i].stringValues = AllocateColumnStringData(columnValues[i].stringValues, rows, columnValues[i].elements);
	  } else {
	    columnValues[i].values = AllocateColumnData(columnValues[i].type, columnValues[i].values, rows);
          }
	  columnValues[i].elements = rows;
	}
    }
    if (columnOrder) {
      for (col=0;col<columns;col++) {
	switch (columnValues[col].type) {
	case SDDS_SHORT:
	  for (i=0;i<rows;i++) {
	    if (fread((short*)(columnValues[col].values)+i,2,1,fileID) != 1) {
	      SDDS_SetError("Unable to read short column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	  }
	  break;
	case SDDS_LONG:
	  for (i=0;i<rows;i++) {
	    if (fread((int32_t*)(columnValues[col].values)+i,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read long column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	  }
	  break;
	case SDDS_FLOAT:
	  for (i=0;i<rows;i++) {
	    if (fread((float*)(columnValues[col].values)+i,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read float column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }      
	  }
	  break;
	case SDDS_DOUBLE:
	  for (i=0;i<rows;i++) {
	    if (fread((double*)(columnValues[col].values)+i,8,1,fileID) != 1) {
	      SDDS_SetError("Unable to read double double");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	  }
	  break;
	case SDDS_STRING:
	  for (i=0;i<rows;i++) {
	    if (fread(&size,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read string column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
            if (size>SDDS_MAXLINE-1)
              SDDS_Bomb("String is too long");
            columnValues[col].stringValues[i] = malloc(size + 1);
	    if (size > 0) {
	      if (fread(columnValues[col].stringValues[i],size,1,fileID) != 1) {
		SDDS_SetError("Unable to read string column");
		SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	      }
              columnValues[col].stringValues[i][size] = 0;
	    } else {
	      strcpy(columnValues[col].stringValues[i],"");
	    }
	  }
	  break;
	case SDDS_CHARACTER:
	  for (i=0;i<rows;i++) {
	    if (fread((char*)(columnValues[col].values)+i,1,1,fileID) != 1) {
	      SDDS_SetError("Unable to read character column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	  }
	  break;
	}
      }
    } else {
      for (i=0;i<rows;i++) {
	for (col=0;col<columns;col++) {
	  switch (columnValues[col].type) {
	  case SDDS_SHORT:
	    if (fread((short*)(columnValues[col].values)+i,2,1,fileID) != 1) {
	      SDDS_SetError("Unable to read short column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	    break;
	  case SDDS_LONG:
	    if (fread((int32_t*)(columnValues[col].values)+i,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read long column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	    break;
	  case SDDS_FLOAT:
	    if (fread((float*)(columnValues[col].values)+i,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read float column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }      
	    break;
	  case SDDS_DOUBLE:
	    if (fread(((double*)(columnValues[col].values)+i),8,1,fileID) != 1) {
	      SDDS_SetError("Unable to read double column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	    break;
	  case SDDS_STRING:
	    if (fread(&size,4,1,fileID) != 1) {
	      SDDS_SetError("Unable to read string column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
            if (size>SDDS_MAXLINE-1)
              SDDS_Bomb("String is too long");
            columnValues[col].stringValues[i] = malloc(size + 1);
	    if (size > 0) {
	      if (fread(columnValues[col].stringValues[i],size,1,fileID) != 1) {
		SDDS_SetError("Unable to read string column");
		SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	      }
              columnValues[col].stringValues[i][size] = 0;
	    } else {
	      strcpy(columnValues[col].stringValues[i],"");
	    }
	    break;
	  case SDDS_CHARACTER:
	    if (fread(((char*)(columnValues[col].values)+i),1,1,fileID) != 1) {
	      SDDS_SetError("Unable to read character column");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	    break;
	  }
	}
      }
    }
    if (rows > maxRows) {
      if (!SDDS_LengthenTable(&SDDS_dataset, rows - maxRows)) {
	SDDS_SetError("Unable to lengthen table");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
      }
      maxRows = rows;
    }
    j=n;
    for (i=0;i<columns;i++) {
      if (columnValues[i].skip)
	continue;
      if (columnValues[i].type == SDDS_STRING) {
	SetColumnData(columnValues[i].type, &SDDS_dataset, columnValues[i].stringValues, rows, n);
      } else {
	SetColumnData(columnValues[i].type, &SDDS_dataset, columnValues[i].values, rows, n);
      }
      n++;
    }
    
    if (!SDDS_WritePage(&SDDS_dataset)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    maxRows = 10000;
    if (!SDDS_StartPage(&SDDS_dataset, initRows))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  row = par = col = n = 0;
  rows = -1;
  ptr = NULL;
  ptr = SDDS_Malloc(sizeof(*ptr)*(ptrSize=2048));
  /*  ptr2 = NULL;
      ptr2 = SDDS_Malloc(sizeof(*ptr)*(ptrSize=2048));*/
  ptr[0] = 0;
  while (1) {
    if (readline) {
      while (skiplines > 0) {
	fgets(ptr, ptrSize, fileID);
	skiplines--;
      }
      if (!fgetsSkipCommentsResize(&ptr,&ptrSize,fileID, '!'))
	break;
      commentFound=0;
      if (checkComment) {
        for (i=0; i<strlen(commentCharacters); i++) {
          if (ptr[0] == commentCharacters[i]) {
            commentFound=1;
          }
        }
      }
      if (commentFound == 1) {
        continue;
      }
      if (ptr[strlen(ptr)-1] == '\n')
        ptr[strlen(ptr)-1] = 0;
      strcpy(temp,ptr);
      /*skip empty lines*/
      if (getToken(temp,data,10240,separator,whitespace)<0)
        continue;
    } else {
      readline = 1;
    }
    if (par < parameters) {
      switch (parameterValues[par].type) {
      case SDDS_SHORT:
	if (sscanf(ptr,"%hd",&shortValue) != 1) {
	  SDDS_SetError("Invalid short parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,shortValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_LONG:
	if (sscanf(ptr,"%" SCNd32 ,&longValue) != 1) {
	  SDDS_SetError("Invalid long parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,longValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_FLOAT:
        ConvertDNotationToENotation(ptr);
	if (sscanf(ptr,"%f",&floatValue) != 1) {
	  SDDS_SetError("Invalid float parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,floatValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_DOUBLE:
        ConvertDNotationToENotation(ptr);
	if (sscanf(ptr,"%lf",&doubleValue) != 1) {
	  SDDS_SetError("Invalid double parameter");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	}
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,doubleValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_STRING:
	SDDS_GetToken(ptr,stringValue,SDDS_MAXLINE);
	SDDS_InterpretEscapes(stringValue);
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,stringValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      case SDDS_CHARACTER:
	SDDS_InterpretEscapes(ptr);
	characterValue = ptr[0];
	if (!SDDS_SetParameters(&SDDS_dataset,SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,par,characterValue,-1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	break;
      }
      par++;
    } else if ((rows == -1) && (!noRowCount)) {
      if (sscanf(ptr,"%ld",&rows) != 1) {
	SDDS_SetError("Invalid row count");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    } else if ((columns > 0) && ((row < rows) || (noRowCount)))  {

      if (columnOrder) {

	if (noRowCount) {
	  cp_str(&ptr2,ptr);
	  rows = 0;
	  while (getToken(ptr2,data,10240,separator,whitespace) >= 0) {
	    rows++;
          }
          free(ptr2);
	}

	if (rows > columnValues[col].elements) {
	  if (columnValues[col].type == SDDS_STRING) {
	    columnValues[col].stringValues = AllocateColumnStringData(columnValues[col].stringValues, rows, columnValues[col].elements);
	  } else {
	    columnValues[col].values = AllocateColumnData(columnValues[col].type, columnValues[col].values, rows);
          }
	  columnValues[col].elements = rows;
	}
	switch (columnValues[col].type) {
	case SDDS_SHORT:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid short column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	    if (sscanf(data,"%hd",((short*)(columnValues[col].values)+row)) != 1) {
	      SDDS_SetError("Invalid short column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	  }
	  break;
	case SDDS_LONG:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid long column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	    if (sscanf(data,"%" SCNd32,((int32_t*)(columnValues[col].values)+row)) != 1) {
	      SDDS_SetError("Invalid long column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	  }
	  break;
	case SDDS_FLOAT:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid float column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
            ConvertDNotationToENotation(data);
	    if (sscanf(data,"%f",((float*)(columnValues[col].values)+row)) != 1) {
	      SDDS_SetError("Invalid float column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	  }
	  break;
	case SDDS_DOUBLE:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid double column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
            ConvertDNotationToENotation(data);
	    if (sscanf(data,"%lf",((double*)(columnValues[col].values)+row)) != 1) {
	      SDDS_SetError("Invalid double column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	  }
	  break;
	case SDDS_STRING:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid string column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	    SDDS_InterpretEscapes(data);
            columnValues[col].stringValues[row] = malloc(strlen(data) + 1);
	    strcpy(columnValues[col].stringValues[row],data);
	  }
	  break;
	case SDDS_CHARACTER:
	  for (row=0;row<rows;row++) {
	    if (getToken(ptr,data,10240,separator,whitespace) < 0) {
	      SDDS_SetError("Invalid character column element");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	    }
	    SDDS_InterpretEscapes(data);
	    *((char*)(columnValues[col].values)+row) = data[0];
	  }
	  break;
	}
	if (rows > maxRows) {
	  if (!SDDS_LengthenTable(&SDDS_dataset, rows - maxRows)) {
	    SDDS_SetError("Unable to lengthen table");
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	  }
	  maxRows = rows;
	}
	if (columnValues[col].skip == 0) {
	  if (columnValues[col].type == SDDS_STRING) {
	    SetColumnData(columnValues[col].type, &SDDS_dataset, columnValues[col].stringValues, rows, col);
	  } else {
	    SetColumnData(columnValues[col].type, &SDDS_dataset, columnValues[col].values, rows, col);
	  }
	  n++;
	}
	col++;
	row = 0;
      } else {
	if (noRowCount) {
	  if (row == 0) {
	    rows = 3;
	  } else if (row == rows - 1) {
	    rows = rows + 3;
	    for (i=0;i<columns;i++) {
	      if (rows > columnValues[i].elements) {
		if (columnValues[i].type == SDDS_STRING) {
		  columnValues[i].stringValues = AllocateColumnStringData(columnValues[i].stringValues, rows, columnValues[i].elements);
		} else {
		  columnValues[i].values = AllocateColumnData(columnValues[i].type, columnValues[i].values, rows);
                }
	      }
	      columnValues[i].elements = rows;
	    }
	  }
	}
	if (row == 0) 
	  for (i=0;i<columns;i++) {
	    if (rows > columnValues[i].elements) {
	      if (columnValues[i].type == SDDS_STRING) {
		columnValues[i].stringValues = AllocateColumnStringData(columnValues[i].stringValues, rows, columnValues[i].elements);
	      } else {
		columnValues[i].values = AllocateColumnData(columnValues[i].type, columnValues[i].values, rows);
              }
	    }
	    columnValues[i].elements = rows;
	  }

	if (noRowCount) {
	  cp_str(&ptr2,ptr);
	  i = 0;
	  while (getToken(ptr2,data,10240,separator,whitespace) >= 0)
	    i++;
          free(ptr2);
	  if ((i != columns) && (parameters>0 && i==1)) {
	    if (row > 0) {
	      if (row > maxRows) {
		if (!SDDS_LengthenTable(&SDDS_dataset, row - maxRows)) {
		  SDDS_SetError("Unable to lengthen table");
		  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
		}
		maxRows = row;
	      }
	      n=0;
	      for (j=0;j<columns;j++) {
		if (columnValues[j].skip)
		  continue;
		if (columnValues[j].type == SDDS_STRING) {
		  SetColumnData(columnValues[j].type, &SDDS_dataset, columnValues[j].stringValues, row, n);
		} else {
		  SetColumnData(columnValues[j].type, &SDDS_dataset, columnValues[j].values, row, n);
		}
		n++;
	      }
	      if (!SDDS_WritePage(&SDDS_dataset))
		SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	      maxRows = 10000;
	      if (!SDDS_StartPage(&SDDS_dataset, initRows))
		SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	      row = par = col = 0;
	      rows = -1;
	    }
	    readline = 0;
	    continue;
	  }
	}

	for (i=0;i<columns;i++) {
	  if (getToken(ptr,data,10240,separator,whitespace) < 0) {
            if (!fillin) {
              SDDS_SetError("Invalid column element");
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            } else {
              switch (columnValues[i].type) {
              case SDDS_SHORT:
              case SDDS_LONG:
              case SDDS_FLOAT:
              case SDDS_DOUBLE:
                data[0]='0';
                data[1]='\0';
                break;
              case SDDS_STRING:  
              case SDDS_CHARACTER:
                data[0]='\0';
                break;
              }
            }
	  }

	  switch (columnValues[i].type) {
	  case SDDS_SHORT:
	    if (sscanf(data,"%hd",((short*)(columnValues[i].values)+row)) != 1) {
              if (recover) {
                abort=1;
                row--;
              } else {
                SDDS_SetError("Invalid short column element");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
	    break;
	  case SDDS_LONG:
	    if (sscanf(data,"%" SCNd32,((int32_t*)(columnValues[i].values)+row)) != 1) {
              if (recover) {
                abort=1;
                row--;
              } else {
                SDDS_SetError("Invalid long column element");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
	    break;
	  case SDDS_FLOAT:
            ConvertDNotationToENotation(data);
	    if (sscanf(data,"%f",((float*)(columnValues[i].values)+row)) != 1) {
              if (recover) {
                abort=1;
                row--;
              } else {
                SDDS_SetError("Invalid float column element");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
	    break;
	  case SDDS_DOUBLE:
            ConvertDNotationToENotation(data);
	    if (sscanf(data,"%lf",((double*)(columnValues[i].values)+row)) != 1) {
              if (recover) {
                abort=1;
                row--;
              } else {
                SDDS_SetError("Invalid double column element");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
	    }
	    break;
	  case SDDS_STRING:
	    SDDS_InterpretEscapes(data);
            columnValues[i].stringValues[row] = malloc(strlen(data) + 1);
	    strcpy(columnValues[i].stringValues[row],data);
	    break;
	  case SDDS_CHARACTER:
	    SDDS_InterpretEscapes(data);
	    *((char*)(columnValues[i].values)+row) = data[0];
	    break;
	  }

          if (recover && abort) {
            break;
          }
        }
	row++;
	if ((row == rows) && (!noRowCount)) {
	  if (rows > maxRows) {
	    if (!SDDS_LengthenTable(&SDDS_dataset, rows - maxRows)) {
	      SDDS_SetError("Unable to lengthen table");
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	    }
	    maxRows = rows;
	  }
	  n=0;
	  for (i=0;i<columns;i++) {
	    if (columnValues[i].skip)
	      continue;
	    if (columnValues[i].type == SDDS_STRING) {
	      SetColumnData(columnValues[i].type, &SDDS_dataset, columnValues[i].stringValues, rows, n);
	    } else {
	      SetColumnData(columnValues[i].type, &SDDS_dataset, columnValues[i].values, rows, n);
	    }
	    n++;
	  }
	}
      }
    }
    if ((par == parameters) && 
	(((!noRowCount) && (rows != -1)) || (noRowCount)) && 
	(((columnOrder) && (col == columns)) || ((columns > 0) && (row == rows)) || (columns == 0))) {
      if (!SDDS_WritePage(&SDDS_dataset)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      maxRows = 10000;
      if (!SDDS_StartPage(&SDDS_dataset, initRows))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      
      row = par = col = 0;
      rows = -1;
    }
    ptr[0] = 0;
  }

  if (noRowCount) {
    if (row > 0) {
      if (row > maxRows) {
	if (!SDDS_LengthenTable(&SDDS_dataset, row - maxRows)) {
	  SDDS_SetError("Unable to lengthen table");
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
	}
	maxRows = row;
      }
      n=0;
      for (j=0;j<columns;j++) {
	if (columnValues[j].skip)
	  continue;
	if (columnValues[j].type == SDDS_STRING) {
	  SetColumnData(columnValues[j].type, &SDDS_dataset, columnValues[j].stringValues, row, n);
	} else {
	  SetColumnData(columnValues[j].type, &SDDS_dataset, columnValues[j].values, row, n);
	}
	n++;
      }
      if (!SDDS_WritePage(&SDDS_dataset)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      maxRows = 10000;
    }
  }
  for (i=0;i<columns;i++) {
    if (columnValues[i].type == SDDS_STRING) {
      for (j=0;j<columnValues[i].elements;j++) {
	free(columnValues[i].stringValues[j]);
      }
      free(columnValues[i].stringValues);
    } else {
      free(columnValues[i].values);
    }
    if (columnValues[i].name) free(columnValues[i].name);
    if (columnValues[i].units) free(columnValues[i].units);
    if (columnValues[i].description) free(columnValues[i].description);
    if (columnValues[i].symbol) free(columnValues[i].symbol);

  }
  for (i=0;i<parameters;i++) {
    free(parameterValues[i].name);
    if (parameterValues[i].units) free(parameterValues[i].units);
    if (parameterValues[i].description) free(parameterValues[i].description);
    if (parameterValues[i].symbol) free(parameterValues[i].symbol);
  }
  if (columnValues) free(columnValues);
  if (parameterValues) free(parameterValues);
  if (columnIndex) free(columnIndex);
  if (parameterIndex) free(parameterIndex);
  if (ptr) free(ptr);
  if (!SDDS_Terminate(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free_scanargs(&s_arg,argc);
  return 0;
}



void SetColumnData(long type, SDDS_DATASET *dataset, void *values, long rows, long index)
{
  switch (type) {
  case SDDS_SHORT:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(short*)values,rows,index,NULL)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    break;
  case SDDS_LONG:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(int32_t*)values,rows,index,NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    break;
  case SDDS_FLOAT:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(float*)values,rows,index,NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    break;
  case SDDS_DOUBLE:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(double*)values,rows,index,NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    break;
  case SDDS_STRING:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(char**)values,rows,index,NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    break;
  case SDDS_CHARACTER:
    if (!SDDS_SetColumn(dataset,SDDS_SET_BY_INDEX,(char*)values,rows,index,NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    break;
  }
}

void *AllocateColumnData(long type, void *values, long rows)
{
  switch (type) {
  case SDDS_SHORT:
    return SDDS_Realloc(values,rows*(sizeof(short)));
  case SDDS_LONG:
    return SDDS_Realloc(values,rows*(sizeof(int32_t)));
  case SDDS_FLOAT:
    return SDDS_Realloc(values,rows*(sizeof(float)));
  case SDDS_DOUBLE:
    return SDDS_Realloc(values,rows*(sizeof(double)));
  case SDDS_CHARACTER:
    return SDDS_Realloc(values,rows*2*(sizeof(char)));
  }
  return values;
}

char **AllocateColumnStringData(char **values, long rows, long previous_rows)
{
  long i;
  values = SDDS_Realloc(values,rows*(sizeof(char*)));
  for (i=previous_rows;i<rows;i++) {
    values[i] = NULL;
    /*    values[i] = malloc(SDDS_MAXLINE);*/
  }
  return values;
}

long getToken(char *s, char *buffer, long buflen, char separator, long whitespace)
{
  char *ptr0, *ptr1, *escptr;
  long n;
  /* save the pointer to the head of the string */
  ptr0 = s;

  /* skip leading white-space */  
  while (isspace(*s))
    s++;
  if (*s==0) 
    return(-1);
  ptr1 = s;

  if (*s=='"') {
    /* if quoted string, skip to next quotation mark */
    ptr1 = s+1;    /* beginning of actual token */
    do {
      s++;
      escptr = NULL;
      if (*s=='\\' && *(s+1)=='\\') {
        /* skip and remember literal \ (indicated by \\ in the string) */
        escptr = s+1;
        s += 2;
      }
    } while (*s && (*s!='"' || (*(s-1)=='\\' && (s-1)!=escptr)));
    /* replace trailing quotation mark with a space */
    if (*s && *s=='"')
      *s = ' ';
    n = (long)(s-ptr1);
    if (whitespace) {
      while (*s && (!isspace(*s))) {
        s++;
      }
      if (*s && (isspace(*s)))
        *s = ' ';    
    } else {
      while (*s && (*s!=separator)) {
        s++;
      }
      if (*s && (*s==separator))
        *s = ' ';    
    }
  }
  else {
    /* skip to first separator following token */
    if (whitespace) {
      do {
        s++;
        /* imbedded quotation marks are handled here */
        if (*s=='"' && *(s-1)!='\\') {
          while (*++s && !(*s=='"' && *(s-1)!='\\'))
            ;
        }
      } while (*s && (!isspace(*s)));
      if (*s && (isspace(*s)))
        *s = ' ';
    } else {
      do {
        s++;
        /* imbedded quotation marks are handled here */
        if (*s=='"' && *(s-1)!='\\') {
          while (*++s && !(*s=='"' && *(s-1)!='\\'))
            ;
        }
      } while (*s && (*s!=separator));
      if (*s && (*s==separator))
        *s = ' ';
    }
    n = (long)(s-ptr1);
  }
  if (n>=buflen)
    return(-1);
  strncpy(buffer, ptr1, n);
  buffer[n]=0;
  
  /* update the original string to delete the token */
  strcpy(ptr0, s);

  /* return the string length */
  return(n);
}

/* Description: Converts Fortran D notation to C++ e notation */
void ConvertDNotationToENotation (char *line)
{
  char *ptr = line;

  while (*ptr && (ptr = strstr (ptr, "D+")))
    {
    *ptr = 'e'; ptr++;
    *ptr = '+'; ptr++;
    }

  ptr = line;
  while (*ptr && (ptr = strstr (ptr, "D-")))
    {
    *ptr = 'e'; ptr++;
    *ptr = '-'; ptr++;
    }
}
