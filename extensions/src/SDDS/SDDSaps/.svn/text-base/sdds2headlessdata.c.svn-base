/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* 
 * Revision 1.0 Hairong Shang
 * first version, write SDDS column data into binary file without headers
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

#define ROW_ORDER 0
#define COLUMN_ORDER 1
#define ORDERS 2
static char *order_names[ORDERS] = {
    "rowMajor", "columnMajor",
    } ;

#define SET_COLUMN 0
#define SET_PIPE 1
#define SET_ORDER 2
#define N_OPTIONS 3

char *option[N_OPTIONS] = {
  "column", "pipe", "order",
};

char *USAGE = "sdds2data <input> <output> \n\t\
 [-order={rowMajor|columnMajor}] [-pipe=in|out] \n\t\
 [-column=<name>\n\n\
-order            Row major order is the default. Here each row consists of one element\n\
                  from each column. In column major order each column is written entirely\n\
                  on one row.\n\
-column           provide the columns whose data are to be written.\n\n\
Program by Hairong Shang.\n";

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
  
  long *columnType, *columnIndex;
  void **columnData;
  char *buffer;
  char **column, **column_match;
  long columns=0, column_matches;

  input = output = NULL;
  columnType = columnIndex = NULL;
  columnData = NULL;
  column_match = NULL;
  column_matches = 0;
  
  buffer = tmalloc(sizeof(char)*16);

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
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
      case SET_COLUMN:
	if ((s_arg[i_arg].n_items<2))
	  SDDS_Bomb("invalid -column syntax");
	column_matches = s_arg[i_arg].n_items - 1;
        column_match = tmalloc(sizeof(*column_match)*column_matches);
        for (i=0; i<column_matches; i++)
          column_match[i] = s_arg[i_arg].list[i+1];
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
  
  processFilenames("sdds2headlessdata", &input, &output, pipeFlags, noWarnings, &tmpfile_used);

  if (!column_matches)
    SDDS_Bomb("you must specify -column options");
  
  if (!SDDS_InitializeInput(&SDDS_dataset, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  column = getMatchingSDDSNames(&SDDS_dataset, column_match, column_matches, &columns, SDDS_MATCH_COLUMN);
  if (!columns)
    SDDS_Bomb("No columns found in the input file.");
  
  columnType = tmalloc(sizeof(*columnType)*columns);
  columnIndex = tmalloc(sizeof(*columnIndex)*columns);
  columnData = tmalloc(sizeof(*columnData)*columns);
  for (i=0; i<columns; i++) {
    if ((columnIndex[i]=SDDS_GetColumnIndex(&SDDS_dataset, column[i]))<0) {
      fprintf(stderr, "error: column %s does not exist\n", column[i]);
      exit(1);
    }
    if ((columnType[i]=SDDS_GetColumnType(&SDDS_dataset, columnIndex[i]))<=0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  
  if (!output) {
#if defined(_WIN32)
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
      fprintf(stderr, "error: unable to set stdout to binary mode\n");
      exit(1);
    }      
#endif
    fileID = stdout;
  } else {
    fileID = fopen(output, "wb");
  }
  if (fileID == NULL) {
    fprintf(stderr, "unable to open output file for writing\n");
    exit(1);
  }
  
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
  
  retval = -1;
 
  while (retval!=page_number && (retval=SDDS_ReadPage(&SDDS_dataset))>0) {
    if (page_number && retval!=page_number)
      continue;
    if ((rows=SDDS_CountRowsOfInterest(&SDDS_dataset))<0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (rows) {
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
    }
    if (retval==0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!SDDS_FlushBuffer(fileID, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteBinaryPage)");
    return 0;
  }
  fclose(fileID);

  if (!SDDS_Terminate(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  exit(0);
}

