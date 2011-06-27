/*************************************************************************\
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 * National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 * Operator of Los Alamos National Laboratory.
 * This file is distributed subject to a Software License Agreement found
 * in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsconvertlogonchange
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_BINARY 0
#define SET_ASCII 1
#define SET_FLOAT 2
#define SET_DOUBLE 3
#define SET_SNAPSHOT 4
#define SET_PIPE 5
#define SET_MININTERVAL 6
#define SET_TIME 7
#define SET_DELETE 8
#define SET_RETAIN 9
#define N_OPTIONS 10

#if !defined(MAXDOUBLE)
#define	MAXDOUBLE 1.79769313486231570e+308
#endif

char *option[N_OPTIONS] = {
  "binary", "ascii", "float", "double", "snapshot", "pipe", 
  "minimuminterval", "time", "delete", "retain"
};

char *USAGE = "sddsconvertlogonchange [<input-file>] [<output-file>]\n\
  [-pipe=[input][,output]]\n\
  [-binary] [-ascii]\n\
  [-double] [-float]\n\
  [-minimumInterval=<seconds>]\n\
  [-snapshot=<epochtime>]\n\
  [-time=[start=<epochtime>][,end=<epochtime>]]\n\
  [-delete={<matching-string>[,...]]\n\
  [-retain={<matching-string>[,...]]\n\n\
  Program by Robert Soliday. ("__DATE__")\n";

long SDDS_SetRowValuesMod(SDDS_DATASET *SDDS_dataset, long mode, long row, ...);
char **process_name_options(char **orig_name, long orig_names, 
                            char **delete, long deletes,
                            char **retain, long retains,
                            long *num_matched, long **origToNewIndex);


int main(int argc, char **argv) {
  SDDS_DATASET SDDS_input, SDDS_output;
  long i, i_arg;
  SCANNED_ARG *s_arg;
  long tmpfile_used, noWarnings;
  char *input, *output;
  long ascii_output, binary_output;
  unsigned long pipeFlags;
  long output_type=SDDS_DOUBLE;
  long snapshot=0, snapshotrow=0;
  double epochtime;

  long readbackNameIndex=-1, controlNameIndex=-1;
  long timeIndex=-1, valueIndex=-1, controlNameIndexIndex=-1, previousRowIndex=-1;
  double *timeData=NULL, *valueData=NULL, *previousRowData=NULL;
  int32_t *controlNameIndexData=NULL;
  SDDS_ARRAY *readbackNameArray=NULL;
  SDDS_ARRAY *readbackNameArray2=NULL;
  long page=0, row, rows, outrow, initRow=0;
  double *rowdata=NULL;
  long *origToNewIndex=NULL;
  double minimumInterval=-1;
  double timeInterval, previousTime=0;
  long totalRows=0, currentRows=0;
  short filterTime=0;
  double startTime=0, endTime=MAXDOUBLE;
  long startTimeRow=0, endTimeRow=0;
  unsigned long flags;
  char **ColumnName=NULL;
  long ColumnNames;
  char **retain_name=NULL;
  long retain_names=0;
  char **delete_name=NULL;
  long delete_names=0;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s", USAGE);
    exit(1);
  }
  input = output = NULL;
  ascii_output = binary_output = noWarnings = 0;
    
  tmpfile_used = 0;
  pipeFlags = 0;
    
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_BINARY:
        binary_output = 1;
        ascii_output = 0;
        break;
      case SET_ASCII:
        ascii_output = 1;
        binary_output = 0;
        break;
      case SET_FLOAT:
        output_type = SDDS_FLOAT;
        break;
      case SET_DOUBLE:
        output_type = SDDS_DOUBLE;
        break;
      case SET_SNAPSHOT:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "invalid -snapshot syntax\n");
          exit(1);
        }
        snapshot = 1;
        if (sscanf(s_arg[i_arg].list[1], "%lf", &epochtime)!=1) {
          fprintf(stderr, "invalid -toPage syntax or value\n");
          exit(1);
        }
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags)) {
          fprintf(stderr, "invalid -pipe syntax\n");
          exit(1);
        }
        break;
      case SET_MININTERVAL:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "invalid -minimumInterval syntax\n");
          exit(1);
        }
        if (sscanf(s_arg[i_arg].list[1], "%lf", &minimumInterval)!=1) {
          fprintf(stderr, "invalid -minimumInterval syntax or value\n");
          exit(1);
        }
        break;
      case SET_TIME:
        s_arg[i_arg].n_items -= 1;
        filterTime=1;
        if (!scanItemList(&flags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "start", SDDS_DOUBLE, &startTime, 1, 0,
                          "end", SDDS_DOUBLE, &endTime, 1, 0,
                          NULL)) {
          fprintf(stderr, "invalid -description syntax\n");
          exit(1);
        }
        s_arg[i_arg].n_items += 1;
        break;
      case SET_RETAIN:
        retain_name = trealloc(retain_name, 
                               sizeof(*retain_name)*(retain_names+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          retain_name[i-1+retain_names] = s_arg[i_arg].list[i];
        retain_names += s_arg[i_arg].n_items-1;
        break;
      case SET_DELETE:
        delete_name = trealloc(delete_name, 
                               sizeof(*delete_name)*(delete_names+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          delete_name[i-1+delete_names] = s_arg[i_arg].list[i];
        delete_names += s_arg[i_arg].n_items-1;
        break;
      default:
        fprintf(stderr, "Error (%s): unknown switch: %s\n", 
                argv[0], s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    } else {
      if (input==NULL)
        input = s_arg[i_arg].list[0];
      else if (output==NULL)
        output = s_arg[i_arg].list[0];
      else {
        fprintf(stderr, "too many filenames\n");
        exit(1);
      }
    }
  }

  if ((snapshot==1) && (minimumInterval >= 0)) {
    fprintf(stderr, "-snapshot and -mimimumInterval options cannot be used together\n");
    exit(1);
  }
  if ((snapshot==1) && (filterTime == 1)) {
    fprintf(stderr, "-snapshot and -time options cannot be used together\n");
    exit(1);
  }

  processFilenames("sddsconvertlogonchange", &input, &output, pipeFlags, noWarnings, &tmpfile_used);

  if (!SDDS_InitializeInput(&SDDS_input, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
    
  readbackNameIndex = SDDS_VerifyArrayExists(&SDDS_input, FIND_SPECIFIED_TYPE, SDDS_STRING, "ReadbackName");
  controlNameIndex = SDDS_VerifyArrayExists(&SDDS_input, FIND_SPECIFIED_TYPE, SDDS_STRING, "ControlName");
  previousRowIndex = SDDS_VerifyColumnExists(&SDDS_input, FIND_NUMERIC_TYPE, "PreviousRow");
  timeIndex = SDDS_VerifyColumnExists(&SDDS_input, FIND_NUMERIC_TYPE, "Time");
  valueIndex = SDDS_VerifyColumnExists(&SDDS_input, FIND_NUMERIC_TYPE, "Value");
  controlNameIndexIndex = SDDS_VerifyColumnExists(&SDDS_input, FIND_INTEGER_TYPE, "ControlNameIndex");
  if ((readbackNameIndex == -1) && (controlNameIndex == -1)) {
    fprintf(stderr, "Error: ReadbackName and ControlName arrays are both missing from the input file.\n");
    exit(1);
  }
  if (timeIndex == -1) {
    fprintf(stderr, "Error: Time column is missing\n");
    exit(1);
  }
  if (valueIndex == -1) {
    fprintf(stderr, "Error: Value column is missing\n");
    exit(1);
  }
  if (controlNameIndexIndex == -1) {
    fprintf(stderr, "Error: ControlNameIndex column is missing\n");
    exit(1);
  }

  if (!SDDS_InitializeOutput(&SDDS_output, 
                             ascii_output?SDDS_ASCII:(binary_output?SDDS_BINARY:SDDS_input.layout.data_mode.mode), 
                             1, NULL, NULL, output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  outrow = 0;
  while (SDDS_ReadTable(&SDDS_input) > 0) {
    page++;
    rows = SDDS_RowCount(&SDDS_input);
    if (page==1) {
      if (readbackNameIndex != -1) {
        readbackNameArray = SDDS_GetArray(&SDDS_input, "ReadbackName", NULL);
      } else {
        readbackNameArray = SDDS_GetArray(&SDDS_input, "ControlName", NULL);
      }
      if (readbackNameArray == NULL) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }

      origToNewIndex = malloc(sizeof(long) * readbackNameArray->elements);
      for (i=0 ; i < readbackNameArray->elements ; i++) {
        origToNewIndex[i] = -1;
      }

      ColumnName=process_name_options(((char**)readbackNameArray->data), 
                                      readbackNameArray->elements, 
                                      delete_name, delete_names, 
                                      retain_name, retain_names, 
                                      &ColumnNames, &origToNewIndex);
      rowdata = malloc(sizeof(double) * ColumnNames);
      for (i=0 ; i < ColumnNames ; i++) {
        rowdata[i] = 0;
      }

      if (SDDS_DefineSimpleColumn(&SDDS_output, "Time", NULL, SDDS_DOUBLE) == 0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (SDDS_DefineSimpleColumns(&SDDS_output, ColumnNames, ColumnName, NULL, output_type) == 0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (SDDS_WriteLayout(&SDDS_output) == 0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (snapshot) {
        if (SDDS_StartTable(&SDDS_output, 1) == 0) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      } else {
        if (SDDS_StartTable(&SDDS_output, 100) == 0) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        totalRows = 100;
      }
    } else {
      if (readbackNameIndex != -1) {
        readbackNameArray2 = SDDS_GetArray(&SDDS_input, "ReadbackName", NULL);
      } else {
        readbackNameArray2 = SDDS_GetArray(&SDDS_input, "ControlName", NULL);
      }
      if (readbackNameArray2 == NULL) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (readbackNameArray->elements != readbackNameArray2->elements) {
        fprintf(stderr, "Error: Unable to process multiple pages with different ReadbackName and/or ControlName columns\n");
        exit(1);
      }
      for (i=0 ; i < readbackNameArray->elements ; i++) {
        if (strcmp((char*)((char**)readbackNameArray->data)[i], (char*)((char**)readbackNameArray2->data)[i]) != 0) {
          fprintf(stderr, "Error: Unable to process multiple pages with different ReadbackName and/or ControlName columns\n");
          exit(1);
        }
      }
      SDDS_FreeArray(readbackNameArray2);
    }
    if ((timeData = SDDS_GetColumnInDoubles(&SDDS_input, "Time")) == NULL) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if ((valueData = SDDS_GetColumnInDoubles(&SDDS_input, "Value")) == NULL) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if ((controlNameIndexData = SDDS_GetColumnInLong(&SDDS_input, "ControlNameIndex")) == NULL) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (snapshot) {
      snapshotrow = 0;
      for (row =0 ; row < rows ; row++) {
        if (timeData[row] <= epochtime) {
          snapshotrow = row;
        } else {
          break;
        }
      }
    }
    if (filterTime) {
      for (row = 0 ; row < rows ; row++) {
        if (timeData[row] <= startTime) {
          startTimeRow = row;
        } else {
          break;
        }
      }
      for (row = 0 ; row < rows ; row++) {
        if (timeData[row] <= endTime) {
          endTimeRow = row;
        } else {
          break;
        }
      }
    }
    if (previousRowIndex != -1) {
      if ((previousRowData = SDDS_GetColumnInDoubles(&SDDS_input, "PreviousRow")) == NULL) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      for (row = rows - 1; row >= 0; row--) {
        if (previousRowData[row] == -2) {
          if (origToNewIndex[controlNameIndexData[row]] == -1) {
            continue;
          }
          initRow = row;
          break;
        }
      }
      free(previousRowData);
    }

    if (minimumInterval > 0) {
      previousTime = timeData[0] - minimumInterval - 1;
    }
    for (row = 0 ; row < rows ; row++) {
      if ((readbackNameArray->elements < controlNameIndexData[row]) || (controlNameIndexData[row] < 0)) {
        /*sometimes there is invalid data in the original file*/
        continue;
      }
      if (origToNewIndex[controlNameIndexData[row]] == -1) {
        continue;
      }
      rowdata[origToNewIndex[controlNameIndexData[row]]] = valueData[row];
      if (previousRowIndex != -1) {
        if (row < initRow) {
           continue;
        }
      }
      if (((snapshot==0) && (filterTime==0)) || 
          ((snapshot==1) && (row == snapshotrow)) || 
          ((filterTime==1) && (row >= startTimeRow) && (row <= endTimeRow))) {
        if (minimumInterval > 0) {
          timeInterval = timeData[row] - previousTime;
          if (timeInterval <= minimumInterval) {
            continue;
          } else {
            previousTime = timeData[row];
          }
        }
        if ((snapshot == 0) && (totalRows == currentRows)) {
          if (SDDS_LengthenTable(&SDDS_output, 100) == 0) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
          totalRows += 100;
        }
        currentRows++;
        if (SDDS_SetRowValuesMod(&SDDS_output, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, outrow, 0, &timeData[row], -1) == 0) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        for (i=0 ; i < ColumnNames ; i++) {
          if (SDDS_SetRowValuesMod(&SDDS_output, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, outrow, i+1, &rowdata[i], -1) == 0) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
        }
        if (snapshot)
          break;
        outrow++;
      }
    }
    if (timeData) free(timeData);
    if (valueData) free(valueData);
    if (controlNameIndexData) free(controlNameIndexData);
  }
  if (rowdata) free(rowdata);
  if (origToNewIndex) free(origToNewIndex);

  if (ColumnName) {
    for (i=0 ; i<ColumnNames ; i++) {
      if (ColumnName[i]) free(ColumnName[i]);
    }
    free(ColumnName);
  }

  SDDS_FreeArray(readbackNameArray);
  if (SDDS_WriteTable(&SDDS_output) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }


  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
    
  if (tmpfile_used && !replaceFileAndBackUp(input, output))
    exit(1);

  free_scanargs(&s_arg,argc);
  return(0);
}


long SDDS_SetRowValuesMod(SDDS_DATASET *SDDS_dataset, long mode, long row, ...)
{
  va_list argptr;
  int32_t index;
  long retval;
  SDDS_LAYOUT *layout;
  char *name;
  char buffer[200];
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetRowValues"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME) ||
      !(mode&SDDS_PASS_BY_VALUE || mode&SDDS_PASS_BY_REFERENCE)) {
    SDDS_SetError("Unable to set column values--unknown mode (SDDS_SetRowValues)");
    return(0);
  }
  /*
    if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetRowValues"))
    return(0);
  */
  row -= SDDS_dataset->first_row_in_mem;
  if (row>=SDDS_dataset->n_rows_allocated) {
    sprintf(buffer, "Unable to set column values--row number (%ld) exceeds exceeds allocated memory (%" PRId32 ") (SDDS_SetRowValues)", row, SDDS_dataset->n_rows_allocated);
    SDDS_SetError(buffer);
    return(0);
  }
  if (row>SDDS_dataset->n_rows-1)
    SDDS_dataset->n_rows = row+1;
  
  va_start(argptr, row);
  layout = &SDDS_dataset->layout;

  /* variable arguments are pairs of (index, value), where index is a long integer */
  retval = -1;
#ifdef DEBUG
  fprintf(stderr, "setting row %ld (mem slot %ld)\n",
          row+SDDS_dataset->first_row_in_mem, row);
#endif
  do {
    if (mode&SDDS_SET_BY_INDEX) {
      if ((index=va_arg(argptr, int32_t))==-1) {
        retval = 1;
        break;
      }
      if (index<0 || index>=layout->n_columns) {
        SDDS_SetError("Unable to set column values--index out of range (SDDS_SetRowValues)");
        retval = 0;
        break;
      }
#ifdef DEBUG
      fprintf(stderr, "Setting values for column #%ld\n", index);
#endif
    }
    else {
      if ((name=va_arg(argptr, char *))==NULL) {
        retval = 1;
        break;
      }
#ifdef DEBUG
      fprintf(stderr, "Setting values for column %s\n", name);
#endif
      if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0) {
        SDDS_SetError("Unable to set column values--name not recognized (SDDS_SetRowValues)");
        retval = 0;
        break;
      }
    }
    switch (layout->column_definition[index].type) {
    case SDDS_SHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *(((short*)SDDS_dataset->data[index])+row) = (short)va_arg(argptr, int);
      else
        *(((short*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, short *));
      break;
    case SDDS_USHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *(((unsigned short*)SDDS_dataset->data[index])+row) = (unsigned short)va_arg(argptr, unsigned int);
      else
        *(((unsigned short*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, unsigned short *));
      break;
    case SDDS_LONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *(((int32_t*)SDDS_dataset->data[index])+row) = va_arg(argptr, int32_t);
      else
        *(((int32_t*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, int32_t *));
      break;
    case SDDS_ULONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *(((uint32_t*)SDDS_dataset->data[index])+row) = va_arg(argptr, uint32_t);
      else
        *(((uint32_t*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, uint32_t *));
      break;
    case SDDS_FLOAT:
      if (mode&SDDS_PASS_BY_VALUE)
        *(((float*)SDDS_dataset->data[index])+row) = (float)va_arg(argptr, double);
      else
        *(((float*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, double *));
      break;
    case SDDS_DOUBLE:
      if (mode&SDDS_PASS_BY_VALUE)
        *(((double*)SDDS_dataset->data[index])+row) = va_arg(argptr, double);
      else
        *(((double*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, double *));
      break;
    case SDDS_STRING:
      if (((char**)SDDS_dataset->data[index])[row]) {
        free(((char**)SDDS_dataset->data[index])[row]);
        ((char**)SDDS_dataset->data[index])[row] = NULL;
      }
      if (mode&SDDS_PASS_BY_VALUE) {
        if (!SDDS_CopyString((char**)SDDS_dataset->data[index]+row,  va_arg(argptr, char *))) {
          SDDS_SetError("Unable to set string column value--allocation failure (SDDS_SetRowValues)");
          retval = 0;
        }
      }
      else {
        if (!SDDS_CopyString((char**)SDDS_dataset->data[index]+row,  *(va_arg(argptr, char **)))) {
          SDDS_SetError("Unable to set string column value--allocation failure (SDDS_SetRowValues)");
          retval = 0;
        }
      }
      break;
    case SDDS_CHARACTER: 
      if (mode&SDDS_PASS_BY_VALUE)
        *(((char*)SDDS_dataset->data[index])+row) = (char)va_arg(argptr, int);
      else
        *(((char*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, char *));
      break;
    default:
      SDDS_SetError("Unknown data type encountered (SDDS_SetRowValues");
      retval=0;
      break;
    }
  } while (retval==-1);
  va_end(argptr);
  return(retval);
}

char **process_name_options(char **orig_name, long orig_names, 
                            char **delete, long deletes,
                            char **retain, long retains,
                            long *num_matched, long **origToNewIndex)
{
  long i, j;
  char **new_name;
  char *ptr=NULL;
  long *orig_flag=NULL;

  orig_flag = tmalloc(sizeof(*orig_flag)*orig_names);
  for (i=0; i<orig_names; i++)
    (orig_flag)[i] = 1;

  if (deletes) {
    for (i=0; i<deletes; i++) {
      ptr = expand_ranges(delete[i]);
      /*free(delete[i]);*/ /* freed by free_scanargs */
      delete[i] = ptr;
    }
    for (j=0; j<orig_names; j++) {
      for (i=0; i<deletes; i++) {
        if (wild_match(orig_name[j], delete[i])) {
          (orig_flag)[j] = 0;
          break;
        }
      }
    }
    for (i=0; i<deletes; i++) {
      if (delete[i]) free(delete[i]);
    }
  }

  if (retains) {
    for (i=0; i<retains; i++) {
      ptr = expand_ranges(retain[i]);
      /*free(retain[i]);*/ /* freed by free_scanargs */
      retain[i] = ptr;
    }
    if (!deletes)
      for (j=0; j<orig_names; j++)
        (orig_flag)[j] = 0;
    for (j=0; j<orig_names; j++) {
      if ((orig_flag)[j])
        continue;
      for (i=0; i<retains; i++) {
        if (wild_match(orig_name[j], retain[i])) {
          (orig_flag)[j] = 1;
          break;
        }
      }
    }
    for (i=0; i<retains; i++) {
      if (retain[i]) free(retain[i]);
    }
  }


  *num_matched = 0;
  for (j=0; j<orig_names; j++) {
    if (orig_flag[j])
      (*num_matched)++;
  }
  new_name = tmalloc(sizeof(*new_name)*(*num_matched));
  i=0;
  for (j=0; j<orig_names; j++) {
    if (orig_flag[j]) {
      (*origToNewIndex)[j] = i;
      SDDS_CopyString(new_name+i, orig_name[j]);
      i++;
    }
  }
  free(orig_flag);

  return(new_name);
}
