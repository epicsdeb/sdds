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
 Revision 1.15  2009/08/04 19:46:09  soliday
 Updated to convert %ld format strings to %d for 64 bit systems.

 Revision 1.14  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.13  2005/11/04 22:46:11  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.12  2003/09/02 19:16:02  soliday
 Cleaned up code for Linux.

 Revision 1.11  2002/10/11 23:53:04  borland
 Fixed bug in previous change.

 Revision 1.10  2002/10/11 22:28:33  borland
 Added -labeled option, which is very helpful for making CSV files
 with labels giving the names and units.

 Revision 1.9  2002/08/14 17:12:39  soliday
 Added Open License

 Revision 1.8  2001/01/10 19:35:30  soliday
 Standardized usage message.

 Revision 1.7  2000/04/13 17:09:10  soliday
 Added Borland C define statement for setmode.

 Revision 1.6  1999/11/15 22:56:03  soliday
 It now defaults to row counts being added to the plaindata file

 Revision 1.5  1999/11/11 15:50:11  soliday
 Made some modifications requested by Borland

 Revision 1.4  1999/11/03 22:45:38  soliday
 Now works with WIN32

 Revision 1.3  1999/10/25 19:11:04  soliday
 Fixed Usage display

 Revision 1.2  1999/10/25 19:08:23  soliday
 Added the -rowPerColumn options

 Revision 1.1  1999/10/13 21:07:30  soliday
 First version os sdds2plaindata.c

 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif

#define ASCII_MODE 0
#define BINARY_MODE 1
#define MODES 2
static char *mode_name[MODES] = {
    "ascii", "binary",
    } ;

#define ROW_ORDER 0
#define COLUMN_ORDER 1
#define ORDERS 2
static char *order_names[ORDERS] = {
    "rowMajor", "columnMajor",
    } ;

#define SET_OUTPUTMODE 0
#define SET_SEPARATOR 1
#define SET_NOROWCOUNT 2
#define SET_PARAMETER 3
#define SET_COLUMN 4
#define SET_PIPE 5
#define SET_NOWARNINGS 6
#define SET_ORDER 7
#define SET_LABELED 8
#define N_OPTIONS 9

char *option[N_OPTIONS] = {
    "outputMode", "separator", "noRowCount",
    "parameter", "column", "pipe", "nowarnings", "order", "labeled",
};

char *USAGE = "sdds2plaindata <input> <output> \n\t\
 [-pipe=[input][,output]] \n\t\
 [-outputMode={ascii|binary}] \n\t\
 [-separator=<string>] \n\t\
 [-noRowCount] \n\t\
 [-order={rowMajor|columnMajor}] \n\t\
 [-parameter=<name>[,format=<string>]...] \n\t\
 [-column=<name>[,format=<string>]...] \n\t\
 [-labeled] [-nowarnings]\n\n\
-outputMode       The plain data file can be written in ascii or binary format.\n\
-separator        In ascii mode the columns can be separated by the given string.\n\
-noRowCount       The number of rows will not be included in the plain data file.\n\
                  If binary mode is used the number of rows will always be written to\n\
                  the file.\n\
-order            Row major order is the default. Here each row consists of one element\n\
                  from each column. In column major order each column is written entirely\n\
                  on one row.\n\
-parameter        Add this option for each parameter to add to the plain data file.\n\
-column           Add this option for each column to add to the plain data file.\n\
-labeled          Add this option to have labels generated for each parameter or column in\n\
                  ascii mode.  It is a good idea to give a separator when using this option!\n\n\
Program by Robert Soliday. (This is version 2, M. Borland, "__DATE__")\n";

/* ********** */

int main(int argc, char **argv)
{
  FILE *fileID;
  SDDS_LAYOUT *layout=NULL;
  SDDS_FILEBUFFER *fBuffer=NULL;

  SDDS_DATASET SDDS_dataset, SDDS_dummy;
  SCANNED_ARG *s_arg;
  long i, j, i_arg, retval, page_number=0, size, columnOrder=0;
  int32_t rows=0;
  char *input, *output;
  unsigned long pipeFlags=0, flags=0;
  long noWarnings=0, tmpfile_used=0;
  long labeled = 0;
  
  long *parameterType, *columnType, *parameterIndex, *columnIndex;
  char **parameterUnits;
  void **columnData;
  char *buffer;
  static char printBuffer[SDDS_MAXLINE*16];
  static char formatbuffer[100], formatbuffer2[100];

  long binary=0, noRowCount=0;
  char *separator;
  char **parameter, **parameterFormat;
  char **column, **columnFormat, **columnUnits;
  long parameters=0, columns=0;

  input = output = NULL;
  parameter = column = parameterFormat = columnFormat = NULL;
  separator = NULL;

  parameterType = columnType = parameterIndex = columnIndex = NULL;
  columnData = NULL;
  parameterUnits = columnUnits = NULL;

  buffer = tmalloc(sizeof(char)*16);

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
      case SET_SEPARATOR:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -separator syntax");
	separator = s_arg[i_arg].list[1];
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
	if ((s_arg[i_arg].n_items!=2) && (s_arg[i_arg].n_items!=4))
	  SDDS_Bomb("invalid -parameter syntax");
	parameter = trealloc(parameter, sizeof(*parameter)*(++parameters));
	parameterFormat = trealloc(parameterFormat, sizeof(*parameterFormat)*(parameters));
	parameter[parameters-1] = s_arg[i_arg].list[1];
	parameterFormat[parameters-1] = NULL;
	s_arg[i_arg].n_items -= 2;
	if (!scanItemList(&flags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
			  "format", SDDS_STRING, &(parameterFormat[parameters-1]), 1, 0,
			  NULL))
	  SDDS_Bomb("invalid -parameter syntax");
        if (parameterFormat[parameters-1]) {
          replaceString(formatbuffer, parameterFormat[parameters-1], "ld", PRId32, -1, 0);
          replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
          parameterFormat[parameters-1] = 
            malloc(sizeof(*(parameterFormat[parameters-1])) * (strlen(formatbuffer2) + 1));
          sprintf(parameterFormat[parameters-1], "%s", formatbuffer2);
        }
	break;
      case SET_COLUMN:
	if ((s_arg[i_arg].n_items<2))
	  SDDS_Bomb("invalid -column syntax");
	column = trealloc(column, sizeof(*column)*(++columns));
	columnFormat = trealloc(columnFormat, sizeof(*columnFormat)*(columns));
	column[columns-1] = s_arg[i_arg].list[1];
	columnFormat[columns-1] = NULL;
	s_arg[i_arg].n_items -= 2;
	if (!scanItemList(&flags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
			  "format", SDDS_STRING, &(columnFormat[columns-1]), 1, 0,
			  NULL))
	  SDDS_Bomb("invalid -columns syntax");
        if (columnFormat[columns-1]) {
          replaceString(formatbuffer, columnFormat[columns-1], "ld", PRId32, -1, 0);
          replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
          columnFormat[columns-1] = 
            malloc(sizeof(*(columnFormat[columns-1])) * (strlen(formatbuffer2) + 1));
          sprintf(columnFormat[columns-1], "%s", formatbuffer2);
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
      case SET_LABELED:
        labeled = 1;
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
  
  processFilenames("sdds2plaindata", &input, &output, pipeFlags, noWarnings, &tmpfile_used);

  if (!columns && !parameters)
    SDDS_Bomb("you must specify one of the -column or the -parameter options");
  if (!separator) {
    cp_str(&separator, "");
  }

  if (!SDDS_InitializeInput(&SDDS_dataset, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (parameters) {
    parameterType = tmalloc(sizeof(*parameterType)*parameters);
    parameterIndex = tmalloc(sizeof(*parameterIndex)*parameters);
    parameterUnits = tmalloc(sizeof(*parameterUnits)*parameters);
    for (i=0; i<parameters; i++) {
      if ((parameterIndex[i]=SDDS_GetParameterIndex(&SDDS_dataset, parameter[i]))<0) {
	fprintf(stderr, "error: parameter %s does not exist\n", parameter[i]);
	exit(1);
      }
      if ((parameterType[i]=SDDS_GetParameterType(&SDDS_dataset, parameterIndex[i]))<=0 ||
          SDDS_GetParameterInformation(&SDDS_dataset, "units", &parameterUnits[i], SDDS_GET_BY_INDEX, parameterIndex[i])!=SDDS_STRING) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
    }
  }
  if (columns) {
    columnType = tmalloc(sizeof(*columnType)*columns);
    columnIndex = tmalloc(sizeof(*columnIndex)*columns);
    columnData = tmalloc(sizeof(*columnData)*columns);
    columnUnits = tmalloc(sizeof(*columnUnits)*columns);
    for (i=0; i<columns; i++) {
      if ((columnIndex[i]=SDDS_GetColumnIndex(&SDDS_dataset, column[i]))<0) {
	fprintf(stderr, "error: column %s does not exist\n", column[i]);
	exit(1);
      }
      if ((columnType[i]=SDDS_GetColumnType(&SDDS_dataset, columnIndex[i]))<=0 ||
          SDDS_GetColumnInformation(&SDDS_dataset, "units", &columnUnits[i], SDDS_GET_BY_INDEX, columnIndex[i])!=SDDS_STRING) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
    }
  }

  if (!output) {
    if (binary) {
#if defined(_WIN32)
      if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
	fprintf(stderr, "error: unable to set stdout to binary mode\n");
	exit(1);
      }      
#endif
    }
    fileID = stdout;
  } else {
    if (binary) {
      fileID = fopen(output, "wb");
    } else {
      fileID = fopen(output, "w");
    }
  }
  if (fileID == NULL) {
    fprintf(stderr, "unable to open output file for writing\n");
    exit(1);
  }

  if (binary) {
    layout = &SDDS_dataset.layout;
    fBuffer = &SDDS_dummy.fBuffer;
    fBuffer->buffer = NULL;
    if (!fBuffer->buffer) {
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*SDDS_FILEBUFFER_SIZE))) {
	fprintf(stderr, "Unable to do buffered read--allocation failure\n");
	exit(1);
      }
      fBuffer->bufferSize = SDDS_FILEBUFFER_SIZE;
      fBuffer->bytesLeft = SDDS_FILEBUFFER_SIZE;
    }
  }

  retval = -1;
  while (retval!=page_number && (retval=SDDS_ReadPage(&SDDS_dataset))>0) {
    if (page_number && retval!=page_number)
      continue;
    if (columns != 0) {
      if ((rows=SDDS_CountRowsOfInterest(&SDDS_dataset))<0) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
    }
    if ((binary) && (!noRowCount)) {
      if (!SDDS_BufferedWrite(&rows, sizeof(rows), fileID, fBuffer)) {
	fprintf(stderr, "Unable to write page--failure writing number of rows\n");
	exit(1);
      }
    }

    for (i=0; i<parameters; i++) {
      if (binary) {
	/*
	if (layout->parameter_definition[i].fixed_value)
	  continue;
	*/
	if (parameterType[i]==SDDS_STRING) {
	  if (!SDDS_WriteBinaryString(*((char**)SDDS_dataset.parameter[parameterIndex[i]]), fileID, fBuffer)) {
	    fprintf(stderr, "Unable to write parameters--failure writing string\n");
	    exit(1);
	  }
	}
	else if (!SDDS_BufferedWrite(SDDS_dataset.parameter[parameterIndex[i]], 
				     SDDS_type_size[layout->parameter_definition[parameterIndex[i]].type-1],
				     fileID, fBuffer)) {
	  fprintf(stderr, "Unable to write parameters--failure writing value\n");
	  exit(1);
	}
      } else {
	if (!SDDS_GetParameter(&SDDS_dataset, parameter[i], buffer)) {
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	  exit(1);
	}
	SDDS_SprintTypedValue(buffer, 0, parameterType[i], 
			      parameterFormat[i], 
			      printBuffer,
			      0);
        if (labeled) {
          fputs(parameter[i], fileID);
          fputs(separator, fileID);
          if (parameterUnits[i])
            fputs(parameterUnits[i], fileID);
          fputs(separator, fileID);
        }
	fputs(printBuffer, fileID);
	fputc('\n', fileID);
      }
    }
    
    if (!binary) {
      if (!noRowCount)
	fprintf(fileID, "\t%" PRId32 "\n", rows);
      if (labeled && columns) {
        for (j=0; j<columns; j++) {
          fputs(column[j], fileID);
          if (j!=(columns-1))
            fputs(separator, fileID);
        }
        fputc('\n', fileID);
        for (j=0; j<columns; j++) {
          if (columnUnits[j])
            fputs(columnUnits[j], fileID);
          if (j!=(columns-1))
            fputs(separator, fileID);
        }
        fputc('\n', fileID);
      }
    }

    if (columns) {
      if (rows) {
	if (binary) {
	  if (columnOrder) {
	    for (j=0; j<columns; j++) {
	      if (columnType[j]==SDDS_STRING) {
		for (i=0; i<rows; i++) {	    
		  if (!SDDS_WriteBinaryString(*((char**)SDDS_dataset.data[columnIndex[j]]+i), fileID, fBuffer)) {
		    fprintf(stderr, "Unable to write rows--failure writing string\n");
		    exit(1);
		  }
		}
	      } else {
		size = SDDS_type_size[columnType[j]-1];
		for (i=0; i<rows; i++) {	    
		  if (!SDDS_BufferedWrite((char*)SDDS_dataset.data[columnIndex[j]]+i*size, size, fileID, fBuffer)) {
		    fprintf(stderr, "Unable to write rows--failure writing string\n");
		    exit(1);
		  }
		}
	      }
	    }
	  } else {
	    for (i=0; i<rows; i++) {	    
	      for (j=0; j<columns; j++) {
		if (columnType[j]==SDDS_STRING) {
		  if (!SDDS_WriteBinaryString(*((char**)SDDS_dataset.data[columnIndex[j]]+i), fileID, fBuffer)) {
		    fprintf(stderr, "Unable to write rows--failure writing string\n");
		    exit(1);
		  }
		} else {
		  size = SDDS_type_size[columnType[j]-1];
		  if (!SDDS_BufferedWrite((char*)SDDS_dataset.data[columnIndex[j]]+i*size, size, fileID, fBuffer)) {
		    fprintf(stderr, "Unable to write rows--failure writing string\n");
		    exit(1);
		  }
		}
	      }
	    }
	  }
	} else {
	  for (i=0; i<columns; i++) {
	    if (!(columnData[i]=SDDS_GetInternalColumn(&SDDS_dataset, column[i]))) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  }
	  if (columnOrder) {
	    for (i=0; i<columns; i++) {
	      for (j=0; j<rows; j++) {
		SDDS_SprintTypedValue(columnData[i], j, columnType[i], 
				      columnFormat[i], 
				      printBuffer, 
				      0);
		fputs(printBuffer, fileID);
		if (j!=rows-1)
		  fprintf(fileID, separator);
	      }
	      fputc('\n', fileID);
	    }
	  } else {
	    for (j=0; j<rows; j++) {
	      for (i=0; i<columns; i++) {
		SDDS_SprintTypedValue(columnData[i], j, columnType[i], 
				      columnFormat[i], 
				      printBuffer, 
				      0);
		fputs(printBuffer, fileID);
		if (i!=columns-1)
		  fprintf(fileID, separator);
	      }
	      fputc('\n', fileID);
	    }
	  }
	}
      }
    }
    
    if (retval==0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }

  if (binary) {
    if (!SDDS_FlushBuffer(fileID, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
  } else {
    fflush(fileID);
  }
  fclose(fileID);


  if (!SDDS_Terminate(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

exit(0);
}

