/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_extract.c
 * contents: user's SDDS routines for data selection and extraction
 *
 * M. Borland, November 1993
 $Log: SDDS_extract.c,v $
 Revision 1.43  2008/09/18 20:15:51  soliday
 Added SDDS_GetColumnInFloats

 Revision 1.42  2008/07/01 14:37:16  shang
 modified SDDS_GetParameterAsLong to return integer for any numeric parameter

 Revision 1.41  2008/03/19 16:30:39  shang
 added printing parameter name if it is not found in the file.

 Revision 1.40  2006/08/31 15:08:01  soliday
 Updated to work with SDDS2

 Revision 1.39  2006/05/25 21:23:31  shang
 added SDDS_GetColumnInShort

 Revision 1.38  2005/11/04 22:46:52  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.37  2004/12/29 19:14:19  borland
 Added code to ensure that the declaration "int tolower();" isn't given if
 the tolower symbol is already defined (e.g., as a macro).

 Revision 1.36  2003/09/11 22:31:02  soliday
 Added SDDS_ConvertToLong and SDDS_GetParameterAsLong

 Revision 1.35  2003/08/28 21:24:57  soliday
 Cleaned up the code.

 Revision 1.34  2003/07/23 02:02:17  soliday
 Added missing return statement.

 Revision 1.33  2003/07/22 20:52:32  soliday
 IEEE functions are always used.

 Revision 1.32  2002/08/14 16:34:48  soliday
 Added Open License

 Revision 1.31  2002/06/05 14:54:29  shang
 added SDDS_GetParameterAsString()

 Revision 1.30  2001/10/28 14:21:09  borland
 Fixed bug in SDDS_AssertColumnFlags() (the column_order array was not
 set up properly).  Simplified code in SDDS_GetRowType().

 Revision 1.29  2001/02/19 18:00:40  borland
 Changed SDDS_GetParameterFromIndex() to SDDS_GetParameterByIndex() to be
 more consistent with other usage.

 Revision 1.28  2001/02/19 17:58:06  borland
 Added SDDS_GetParameterFromIndex() routine.

 Revision 1.27  2000/10/24 01:16:28  borland
 Added SDDS_GetArrayInDoubles().

 Revision 1.26  2000/10/11 21:51:47  soliday
 Changed definition of isinf so that sunmath is no longer needed.

 Revision 1.25  2000/08/10 21:15:26  soliday
 Added support for gcc on Solaris.

 Revision 1.24  2000/06/15 21:06:24  soliday
 Removed compiler warnings on Solaris.

 Revision 1.23  2000/04/04 05:03:36  soliday
 Added missing sunmath header include statement.

 Revision 1.22  2000/03/27 20:24:49  borland
 Disabled SDDS_DeleteColumn(), as it doesn't work.

 Revision 1.21  1999/07/08 21:55:03  soliday
 Added definition for isnan

 Revision 1.20  1999/05/25 18:56:41  soliday
 Removed compiler warning on linux.

 Revision 1.19  1999/02/04 21:12:07  soliday
 Added the SDDS_GetColumnInLong procedure

 Revision 1.18  1998/08/26 14:49:33  borland
 Treatment of IEEE math function isinf is now uniform.  If on solaris
 and sunmath is missing, then modify mdb.h.

 Revision 1.17  1998/08/25 01:48:20  borland
 SDDS_GetDescription now correctly allows giving NULL pointers for one
 or both arguments.

 Revision 1.16  1998/08/13 15:05:39  borland
 SDDS_ItemInsideWindow now does comparisons with doubles to prevent
 unexpected results.

 Revision 1.15  1998/04/21 21:23:42  borland
 Added some comments for SDDS_AssertRowFlags.

 Revision 1.14  1997/12/19 16:58:35  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.13  1997/10/29 21:24:13  borland
 Fixed problems that occured when an application attempted to copy
 rows into a dataset that has no columns defined.

 Revision 1.12  1997/07/08 14:40:47  borland
 SDDS_GetRowFlags now uses SDDS_SetError when there is a row count mismtach.

 Revision 1.11  1996/08/26 20:07:24  borland
 Changed SDDS_FilterRowsByNumScan to use tokenIsNumber.  Eliminates function
 of NUMSCANFILTER_STRICT flag. (Test is always strict.)

 Revision 1.10  1996/08/12 15:36:40  borland
 Removed memory leak in SDDS_SetRowsOfInterest by free'ing pointer in variable
 match_string.

 Revision 1.9  1996/05/29 15:02:07  borland
 Fixed error in SDDS_GetCastMatrixOfRows---was casting to double and ignoring
 type given in argument.

 * Revision 1.8  1996/03/28  23:48:29  borland
 * Added check for NaN for SOLARIS to filtering routines.
 *
 * Revision 1.7  1996/03/12  04:00:42  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.6  1996/02/12  17:15:27  borland
 * Optimized SDDS_GetColumnInDoubles by eliminating call to SDDS_CastValue
 * and hence effectively putting switch statement outside loop over rows.
 *
 * Revision 1.5  1996/01/18  04:31:26  borland
 * Added SDDS_GetInternalColumn() routine for access to internal tabular data
 * (without memory copy).
 *
 * Revision 1.4  1996/01/12  20:06:58  borland
 * Added SDDS_FilterRowsByNumScan() routine to support -numberTest option of
 * sddsprocess.
 *
 * Revision 1.3  1995/09/12  03:14:51  borland
 * Modified SDDS_SetRowsOfInterest and SDDS_MatchRowsOfInterest to
 * perform case-insensitive selection.
 *
 * Revision 1.2  1995/09/05  21:10:55  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"

#if defined(_WIN32)
#define isnan(x) _isnan(x)
#endif

int32_t SDDS_SetRowFlags(SDDS_DATASET *SDDS_dataset, int32_t row_flag_value)
{
  /*  int32_t i;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetRowFlags")) 
    return(0);
  if (!SDDS_SetMemory(SDDS_dataset->row_flag, SDDS_dataset->n_rows_allocated, SDDS_LONG, (int32_t)row_flag_value, (int32_t)0)) {
    SDDS_SetError("Unable to set row flags--memory filling failed (SDDS_SetRowFlags)");
    return(0);
  }
  return(1);
}

int32_t SDDS_GetRowFlag(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetRowFlag")) 
    return -1;
  if (row<0 || row>=SDDS_dataset->n_rows)
    return -1;
  return SDDS_dataset->row_flag[row];
}

int32_t SDDS_GetRowFlags(SDDS_DATASET *SDDS_dataset, int32_t *flag, int32_t rows)
{
  int32_t i;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetRowFlags")) 
    return 0;
  if (rows!=SDDS_dataset->n_rows) {
    SDDS_SetError("Row count mismatch (SDDS_GetRowFlags)");
    return 0;
  }
  for (i=0; i<rows; i++)
    flag[i] = SDDS_dataset->row_flag[i];
  return 1;
}

int32_t SDDS_AssertRowFlags(SDDS_DATASET *SDDS_dataset, uint32_t mode, ...)
/* usage:
   SDDS_AssertRowFlags(&SDDSset, SDDS_FLAG_ARRAY, int32_t *flagArray, int32_t rowsInArray)
   rowsInArray is normally equal to the number of rows in the table
   SDDS_AssertRowFlags(&SDDSset, SDDS_INDEX_LIMITS, int32_t start, int32_t end, int32_t value)
   */
{
  int32_t i, rows, startRow, endRow;
  va_list argptr;
  int32_t retval;
  int32_t *flagArray, flagValue;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_AssertRowFlags")) 
    return(0);

  va_start(argptr, mode);
  retval = 0;
  switch (mode) {
  case SDDS_FLAG_ARRAY:
    if (!(flagArray = va_arg(argptr, int32_t *))) 
      SDDS_SetError("NULL flag array pointer seen (SDDS_AssertRowFlags)");
    else if ((rows = va_arg(argptr, int32_t))<0) 
      SDDS_SetError("invalid row count seen (SDDS_AssertRowFlags)");
    else {
      if (rows>=SDDS_dataset->n_rows)
        rows = SDDS_dataset->n_rows;
      for (i=0; i<rows; i++)
        SDDS_dataset->row_flag[i] = flagArray[i];
      retval = 1;
    }
    break;
  case SDDS_INDEX_LIMITS:
    if ((startRow=va_arg(argptr, int32_t))<0 || (endRow=va_arg(argptr, int32_t))<startRow)
      SDDS_SetError("invalid start and end row values (SDDS_AssertRowFlags)");
    else {
      flagValue = va_arg(argptr, int32_t);
      if (endRow>=SDDS_dataset->n_rows || endRow<0)
        endRow = SDDS_dataset->n_rows-1;
      for (i=startRow; i<=endRow; i++)
        SDDS_dataset->row_flag[i] = flagValue;
      retval = 1;
    }
    break;
  default:
    SDDS_SetError("unknown mode passed (SDDS_AssertRowFlags)");
    break;
  }

  va_end(argptr);
  return retval;
}

int32_t SDDS_SetColumnFlags(SDDS_DATASET *SDDS_dataset, int32_t column_flag_value)
{
  int32_t i;
  /*  int32_t j;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumnFlags"))
    return 0;
  if ((!SDDS_dataset->column_flag || !SDDS_dataset->column_order) && !SDDS_AllocateColumnFlags(SDDS_dataset))
    return 0;
  if (!SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, 
                      (int32_t)column_flag_value, (int32_t)0)) {
    SDDS_SetError("Unable to set column flags--memory filling failed (SDDS_SetColumnFlags)");
    return(0);
  }
  SDDS_dataset->n_of_interest = column_flag_value?SDDS_dataset->layout.n_columns:0;
  for (i=0; i<SDDS_dataset->layout.n_columns; i++)
    SDDS_dataset->column_order[i] = column_flag_value?i:-1;
  return(1);
}

int32_t SDDS_AssertColumnFlags(SDDS_DATASET *SDDS_dataset, uint32_t mode, ...)
{
  int32_t i, j, columns, startColumn, endColumn;
  va_list argptr;
  int32_t retval;
  int32_t *flagArray, flagValue;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_AssertColumnFlags")) 
    return(0);
  if ((!SDDS_dataset->column_flag || !SDDS_dataset->column_order) && !SDDS_AllocateColumnFlags(SDDS_dataset))
    return 0;

  va_start(argptr, mode);
  retval = 0;
  switch (mode) {
  case SDDS_FLAG_ARRAY:
    if (!(flagArray = va_arg(argptr, int32_t *))) 
      SDDS_SetError("NULL flag array pointer seen (SDDS_AssertColumnFlags)");
    else if ((columns = va_arg(argptr, int32_t))<0) 
      SDDS_SetError("invalid column count seen (SDDS_AssertColumnFlags)");
    else {
      if (columns>=SDDS_dataset->layout.n_columns)
        columns = SDDS_dataset->layout.n_columns-1;
      for (i=0; i<columns; i++)
        SDDS_dataset->column_flag[i] = flagArray[i];
      retval = 1;
    }
    break;
  case SDDS_INDEX_LIMITS:
    if ((startColumn=va_arg(argptr, int32_t))<0 || (endColumn=va_arg(argptr, int32_t))<startColumn)
      SDDS_SetError("invalid start and end column values (SDDS_AssertColumnFlags)");
    else {
      flagValue = va_arg(argptr, int32_t);
      if (endColumn>=SDDS_dataset->layout.n_columns || endColumn<0)
        endColumn = SDDS_dataset->layout.n_columns-1;
      for (i=startColumn; i<=endColumn; i++)
        SDDS_dataset->column_flag[i] = flagValue;
      retval = 1;
    }
    break;
  default:
    SDDS_SetError("unknown mode passed (SDDS_AssertColumnFlags)");
    break;
  }
  va_end(argptr);

  for (i=j=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (SDDS_dataset->column_flag[i])
      SDDS_dataset->column_order[j++] = i;
  }

  SDDS_dataset->n_of_interest = j;
  
  return retval;
}

int32_t SDDS_CountColumnsOfInterest(SDDS_DATASET *SDDS_dataset) 
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_CountRowsOfInterest"))
    return(-1);
  return(SDDS_dataset->n_of_interest);
}

int32_t SDDS_CountRowsOfInterest(SDDS_DATASET *SDDS_dataset)
{
  int32_t n_rows, i;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_CountRowsOfInterest"))
    return(-1);
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_CountRowsOfInterest"))
    return(-1);
  if (!SDDS_dataset->layout.n_columns)
    return 0;
  for (i=n_rows=0; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i])
      n_rows += 1;
  }
  return(n_rows);
}


int32_t SDDS_SetColumnsOfInterest(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
/* This routine has 3 calling modes:
 * SDDS_SetColumnsOfInterest(&SDDS_dataset, SDDS_NAME_ARRAY, int32_t n_entries, char **name)
 * SDDS_SetColumnsOfInterest(&SDDS_dataset, SDDS_NAMES_STRING, char *names)
 * SDDS_SetColumnsOfInterest(&SDDS_dataset, SDDS_NAME_STRINGS, char *name1, char *name2, ..., NULL )
 * SDDS_SetColumnsOfInterest(&SDDS_dataset, SDDS_MATCH_STRING, char *name, int32_t logic_mode)
 */
{
  va_list argptr;
  int32_t i, j, index, n_names, retval;
  /*  int32_t type;*/
  char **name, *string, *match_string, *ptr;
  int32_t local_memory;    /* (0,1,2) --> (none, pointer array, pointer array + strings) locally allocated */
  static char buffer[SDDS_MAXLINE];
  int32_t logic;

  name = NULL;
  n_names = local_memory = logic = 0;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumnsOfInterest"))
    return(0);
  if ((!SDDS_dataset->column_flag || !SDDS_dataset->column_order) && !SDDS_AllocateColumnFlags(SDDS_dataset))
    return 0;
  va_start(argptr, mode);
  retval = -1;
  match_string = NULL;
  switch (mode) {
  case SDDS_NAME_ARRAY:
    local_memory = 0;
    n_names = va_arg(argptr, int32_t);
    name = va_arg(argptr, char **);
    break;
  case SDDS_NAMES_STRING:
    local_memory = 2;
    n_names = 0;
    name = NULL;
    ptr = va_arg(argptr, char *);
    SDDS_CopyString(&string, ptr);
    while ((ptr=strchr(string, ',')))
      *ptr = ' ';
    while (SDDS_GetToken(string, buffer, SDDS_MAXLINE)>0) {
      if (!(name = SDDS_Realloc(name, sizeof(*name)*(n_names+1))) || !SDDS_CopyString(name+n_names, buffer)) {
        SDDS_SetError("Unable to process column selection--memory allocation failure (SDDS_SetColumnsOfInterest)");
        retval = 0;
        break;
      }
      n_names++;
    }
    free(string);
    break;
  case SDDS_NAME_STRINGS:
    local_memory = 1;
    n_names = 0;
    name = NULL;
    while ((string=va_arg(argptr, char *))) {
      if (!(name = SDDS_Realloc(name, sizeof(*name)*(n_names+1)))) {
        SDDS_SetError("Unable to process column selection--memory allocation failure (SDDS_SetColumnsOfInterest)");
        retval = 0;
        break;
      }
      name[n_names++] = string;
    }
    break;
  case SDDS_MATCH_STRING:
    local_memory = 0;
    n_names = 1;
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process column selection--invalid matching string (SDDS_SetColumnsOfInterest)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  default:
    SDDS_SetError("Unable to process column selection--unknown mode (SDDS_SetColumnsOfInterest)");
    retval = 0;
    break;
  }

  va_end(argptr);
  if (retval!=-1)
    return(retval);
  
  if (n_names==0) {
    SDDS_SetError("Unable to process column selection--no names in call (SDDS_SetColumnsOfInterest)");
    return(0);
  }
  if (!SDDS_dataset->column_order) {
    SDDS_SetError(
                  "Unable to process column selection--'column_order' array in SDDS_DATASET is NULL (SDDS_SetColumnsOfInterest)");
    return(0);
  }

  if (mode!=SDDS_MATCH_STRING) {
    for (i=0; i<n_names; i++) {
      if ((index=SDDS_GetColumnIndex(SDDS_dataset, name[i]))<0) {
        sprintf(buffer, "Unable to process column selection--unrecognized column name %s seen (SDDS_SetColumnsOfInterest)", name[i]);
        SDDS_SetError(buffer);
        return(0);
      }
      for (j=0; j<SDDS_dataset->n_of_interest; j++)
        if (index==SDDS_dataset->column_order[j])
          break;
      if (j==SDDS_dataset->n_of_interest) {
        SDDS_dataset->column_flag[index] = 1;
        SDDS_dataset->column_order[j] = index;
        SDDS_dataset->n_of_interest++;
      }
    }
  }
  else {
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      if (SDDS_Logic(SDDS_dataset->column_flag[i], 
                     wild_match(SDDS_dataset->layout.column_definition[i].name, match_string), logic)) {
#if defined(DEBUG)
        fprintf(stderr, "logic match of %s to %s\n",
                SDDS_dataset->layout.column_definition[i].name, match_string);
#endif
        for (j=0; j<SDDS_dataset->n_of_interest; j++)
          if (i==SDDS_dataset->column_order[j])
            break;
        if (j==SDDS_dataset->n_of_interest) {
          SDDS_dataset->column_flag[i] = 1;
          SDDS_dataset->column_order[j] = i;
          SDDS_dataset->n_of_interest++;
        }
      }
      else {
#if defined(DEBUG)
        fprintf(stderr, "no logic match of %s to %s\n",
                SDDS_dataset->layout.column_definition[i].name, match_string);
#endif
        SDDS_dataset->column_flag[i] = 0;
        for (j=0; j<SDDS_dataset->n_of_interest; j++)
          if (i==SDDS_dataset->column_order[j])
            break;
        if (j!=SDDS_dataset->n_of_interest) {
          for (j++ ; j<SDDS_dataset->n_of_interest; j++) 
            SDDS_dataset->column_order[j-1] = SDDS_dataset->column_order[j];
        }
      }
    }
    free(match_string);
  }

#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->n_of_interest; i++) 
    fprintf(stderr, "column %" PRId32 " will be %s\n", i, SDDS_dataset->layout.column_definition[SDDS_dataset->column_order[i]].name);
#endif

  if (local_memory==2) {
    for (i=0; i<n_names; i++)
      free(name[i]);
  }
  if (local_memory>=1)
    free(name);

  return(1);
}

void *SDDS_GetColumn(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index, size, n_rows, type;
  void *data;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumn"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetColumn)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetColumn)");
    return(NULL);
  }
  if (!(type=SDDS_GetColumnType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get column--data type undefined (SDDS_GetColumn)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (!(data = SDDS_Malloc(size*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetColumn)");
    return(NULL);
  }
  for (i=j=0; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i]) {
      if (type!=SDDS_STRING)
        memcpy((char*)data+size*j++, (char*)SDDS_dataset->data[index]+size*i, size);
      else if (!SDDS_CopyString((char**)data+j++, ((char***)SDDS_dataset->data)[index][i]))
        return(NULL);
    }
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetColumn)");
    return(NULL);
  }
  return(data);
}

void *SDDS_GetInternalColumn(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t index;
  /*  int32_t i, j, size, n_rows, type;*/
  /*  void *data;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetInternalColumn"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetInternalColumn)");
    return(NULL);
  }
  return SDDS_dataset->data[index];
}

double *SDDS_GetColumnInDoubles(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index, size, n_rows, type;
  double *data;
  void *rawData;
  
  j = 0;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnInDoubles"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetColumnInDoubles)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetColumnInDoubles)");
    return(NULL);
  }
  if ((type=SDDS_GetColumnType(SDDS_dataset, index))<=0 || (size=SDDS_GetTypeSize(type))<=0 
      || (!SDDS_NUMERIC_TYPE(type) && type!=SDDS_CHARACTER)) {
    SDDS_SetError("Unable to get column--data size or type undefined or non-numeric (SDDS_GetColumnInDoubles)");
    return(NULL);
  }
  if (!(data = (double*)SDDS_Malloc(sizeof(double)*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetColumnInDoubles)");
    return(NULL);
  }
  rawData = SDDS_dataset->data[index];
  switch (type) {
  case SDDS_DOUBLE:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((double*)rawData)[i];
    }
    break;
  case SDDS_FLOAT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((float*)rawData)[i];
    }
    break;
  case SDDS_LONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((int32_t*)rawData)[i];
    }
    break;
  case SDDS_ULONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((uint32_t*)rawData)[i];
    }
    break;
  case SDDS_SHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((short*)rawData)[i];
    }
    break;
  case SDDS_USHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((unsigned short*)rawData)[i];
    }
    break;
  case SDDS_CHARACTER:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((char*)rawData)[i];
    }
    break;
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetColumnInDoubles)");
    return(NULL);
  }
  return(data);
}

float *SDDS_GetColumnInFloats(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index, size, n_rows, type;
  float *data;
  void *rawData;
  
  j = 0;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnInFloats"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetColumnInFloats)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetColumnInFloats)");
    return(NULL);
  }
  if ((type=SDDS_GetColumnType(SDDS_dataset, index))<=0 || (size=SDDS_GetTypeSize(type))<=0 
      || (!SDDS_NUMERIC_TYPE(type) && type!=SDDS_CHARACTER)) {
    SDDS_SetError("Unable to get column--data size or type undefined or non-numeric (SDDS_GetColumnInFloats)");
    return(NULL);
  }
  if (!(data = (float*)SDDS_Malloc(sizeof(float)*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetColumnInFloats)");
    return(NULL);
  }
  rawData = SDDS_dataset->data[index];
  switch (type) {
  case SDDS_DOUBLE:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((double*)rawData)[i];
    }
    break;
  case SDDS_FLOAT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((float*)rawData)[i];
    }
    break;
  case SDDS_LONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((int32_t*)rawData)[i];
    }
    break;
  case SDDS_ULONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((uint32_t*)rawData)[i];
    }
    break;
  case SDDS_SHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((short*)rawData)[i];
    }
    break;
  case SDDS_USHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((unsigned short*)rawData)[i];
    }
    break;
  case SDDS_CHARACTER:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((char*)rawData)[i];
    }
    break;
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetColumnInFloats)");
    return(NULL);
  }
  return(data);
}

int32_t *SDDS_GetColumnInLong(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index, size, n_rows, type;
  int32_t *data;
  void *rawData;
  j = 0;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnInLong"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetColumnInLong)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetColumnInLong)");
    return(NULL);
  }
  if ((type=SDDS_GetColumnType(SDDS_dataset, index))<=0 || (size=SDDS_GetTypeSize(type))<=0 
      || (!SDDS_NUMERIC_TYPE(type) && type!=SDDS_CHARACTER)) {
    SDDS_SetError("Unable to get column--data size or type undefined or non-numeric (SDDS_GetColumnInLong)");
    return(NULL);
  }
  if (!(data = (int32_t*)SDDS_Malloc(sizeof(int32_t)*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetColumnInLong)");
    return(NULL);
  }
  rawData = SDDS_dataset->data[index];
  switch (type) {
  case SDDS_DOUBLE:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((double*)rawData)[i];
    }
    break;
  case SDDS_FLOAT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((float*)rawData)[i];
    }
    break;
  case SDDS_LONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((int32_t*)rawData)[i];
    }
    break;
  case SDDS_ULONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((uint32_t*)rawData)[i];
    }
    break;
  case SDDS_SHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((short*)rawData)[i];
    }
    break;
  case SDDS_USHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((unsigned short*)rawData)[i];
    }
    break;
  case SDDS_CHARACTER:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((char*)rawData)[i];
    }
    break;
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetColumnInLong)");
    return(NULL);
  }
  return(data);
}

short *SDDS_GetColumnInShort(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index, size, n_rows, type;
  short *data;
  void *rawData;
  j = 0;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnInShort"))
    return(NULL);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetColumnInShort)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetColumnInShort)");
    return(NULL);
  }
  if ((type=SDDS_GetColumnType(SDDS_dataset, index))<=0 || (size=SDDS_GetTypeSize(type))<=0 
      || (!SDDS_NUMERIC_TYPE(type) && type!=SDDS_CHARACTER)) {
    SDDS_SetError("Unable to get column--data size or type undefined or non-numeric (SDDS_GetColumnInShort)");
    return(NULL);
  }
  if (!(data = (short*)SDDS_Malloc(sizeof(short)*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetColumnInShort)");
    return(NULL);
  }
  rawData = SDDS_dataset->data[index];
  switch (type) {
  case SDDS_DOUBLE:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((double*)rawData)[i];
    }
    break;
  case SDDS_FLOAT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((float*)rawData)[i];
    }
    break;
  case SDDS_LONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((int32_t*)rawData)[i];
    }
    break;
  case SDDS_ULONG:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((uint32_t*)rawData)[i];
    }
    break;
  case SDDS_SHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((short*)rawData)[i];
    }
    break;
  case SDDS_USHORT:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((unsigned short*)rawData)[i];
    }
    break;
  case SDDS_CHARACTER:
    for (i=j=0; i<SDDS_dataset->n_rows; i++) {
      if (SDDS_dataset->row_flag[i])
        data[j++] = ((char*)rawData)[i];
    }
    break;
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetColumnInShort)");
    return(NULL);
  }
  return(data);
}

void *SDDS_GetNumericColumn(SDDS_DATASET *SDDS_dataset, char *column_name, int32_t desiredType)
{
  int32_t i, j, index, size, n_rows, type, desiredTypeSize;
  void *data;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetNumericColumn"))
    return(NULL);
  if (!SDDS_NUMERIC_TYPE(desiredType) && desiredType!=SDDS_CHARACTER) {
    SDDS_SetError("Unable to get column--desired type is nonnumeric (SDDS_GetNumericColumn)");
    return(NULL);
  }
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get column--name is not recognized (SDDS_GetNumericColumn)");
    return(NULL);
  }
  if ((type=SDDS_GetColumnType(SDDS_dataset, index))<=0 || (size=SDDS_GetTypeSize(type))<=0 
      || (!SDDS_NUMERIC_TYPE(type) && type!=SDDS_CHARACTER)) {
    SDDS_SetError("Unable to get column--data size or type undefined or non-numeric (SDDS_GetNumericColumn)");
    return(NULL);
  }
  if (type==desiredType)
    return SDDS_GetColumn(SDDS_dataset, column_name);
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get column--no rows left (SDDS_GetNumericColumn)");
    return(NULL);
  }
  if (!(data = (void*)SDDS_Malloc((desiredTypeSize=SDDS_GetTypeSize(desiredType))*n_rows))) {
    SDDS_SetError("Unable to get column--memory allocation failure (SDDS_GetNumericColumn)");
    return(NULL);
  }
  for (i=j=0; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i] && 
        !SDDS_CastValue(SDDS_dataset->data[index], i, type, desiredType, (char*)data+desiredTypeSize*j++)) {
      SDDS_SetError("Unable to get column--cast to double failed (SDDS_GetNumericColumn)");
      return(NULL);
    }
  }
  if (j!=n_rows) {
    SDDS_SetError("Unable to get column--row number mismatch (SDDS_GetNumericColumn)");
    return(NULL);
  }
  return(data);
}

int32_t SDDS_GetSelectedRowIndex(SDDS_DATASET *SDDS_dataset, int32_t srow_index)
{
  int32_t i, j;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetSelectedRowIndex"))
    return(-1);
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_GetSelectedRowIndex"))
    return(-1);
  for (i=j=0; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i] && j++==srow_index)
      break;
  }
  if (i==SDDS_dataset->n_rows)
    return(-1);
  return(i);
}

void *SDDS_GetValue(SDDS_DATASET *SDDS_dataset, char *column_name, int32_t srow_index, void *memory)
{
  int32_t column_index, type, size, row_index;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetValue"))
    return(NULL);
  if ((column_index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get value--column name is not recognized (SDDS_GetValue)");
    return(NULL);
  }
  if (!(type=SDDS_GetColumnType(SDDS_dataset, column_index))) {
    SDDS_SetError("Unable to get value--data type undefined (SDDS_GetValue)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if ((row_index=SDDS_GetSelectedRowIndex(SDDS_dataset, srow_index))<0) {
    SDDS_SetError("Unable to get value--row index out of range (SDDS_GetValue)");
    return(NULL);
  }
  if (type!=SDDS_STRING) {
    if (!memory && !(memory = SDDS_Malloc(size))) {
      SDDS_SetError("Unable to get value--memory allocation failure (SDDS_GetValue)");
      return(NULL);
    }
    memcpy(memory, (char*)SDDS_dataset->data[column_index] + row_index*size, size);
    return(memory);
  }
  /* for character string data, a typical call would be
   * char *string;
   * SDDS_GetValue(&SDDS_dataset, "name", index, &string)    or
   * string = *(char**)SDDS_GetValue(&SDDS_dataset, "name", index, NULL)
   */
  if (!memory && !(memory = SDDS_Malloc(size))) {
    SDDS_SetError("Unable to get value--memory allocation failure (SDDS_GetValue)");
    return(NULL);
  }
  if (SDDS_CopyString(memory, ((char**)SDDS_dataset->data[column_index])[row_index]))
    return(memory);
  return(NULL);
}

void *SDDS_GetValueByIndex(SDDS_DATASET *SDDS_dataset, int32_t column_index, int32_t srow_index, void *memory)
{
  int32_t type, size, row_index;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetValueByIndex"))
    return(NULL);
  if (column_index<0 || column_index>=SDDS_dataset->layout.n_columns) {
    SDDS_SetError("Unable to get value--column index out of range (SDDS_GetValueByIndex)");
    return(NULL);
  }
  if (!(type=SDDS_GetColumnType(SDDS_dataset, column_index))) {
    SDDS_SetError("Unable to get value--data type undefined (SDDS_GetValueByIndex)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if ((row_index=SDDS_GetSelectedRowIndex(SDDS_dataset, srow_index))<0) {
    SDDS_SetError("Unable to get value--row index out of range (SDDS_GetValueByIndex)");
    return(NULL);
  }
  if (type!=SDDS_STRING) {
    if (memory) {
      memcpy(memory, (char*)SDDS_dataset->data[column_index] + row_index*size, size);
      return(memory);
    }
    return((char*)SDDS_dataset->data[column_index] + row_index*size);
  }
  /* for character string data, a typical call would be
   * char *string;
   * SDDS_GetValueByIndex(&SDDS_dataset, cindex, index, &string)    or
   * string = *(char**)SDDS_GetValue(&SDDS_dataset, cindex, index, NULL)
   */
  if (!memory)
    memory = SDDS_Malloc(size);
  if (SDDS_CopyString(memory, ((char**)SDDS_dataset->data[column_index])[row_index]))
    return(memory);
  return(NULL);
}

void *SDDS_GetValueByAbsIndex(SDDS_DATASET *SDDS_dataset, int32_t column_index, int32_t row_index, void *memory)
{
  int32_t type, size;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetValueByAbsIndex"))
    return(NULL);
  if (column_index<0 || column_index>=SDDS_dataset->layout.n_columns) {
    SDDS_SetError("Unable to get value--column index out of range (SDDS_GetValueByAbsIndex)");
    return(NULL);
  }
  if (row_index<0 || row_index>=SDDS_dataset->n_rows) {
    SDDS_SetError("Unable to get value--index out of range (SDDS_GetValueByAbsIndex)");
    return(NULL);
  }
  if (!(type=SDDS_GetColumnType(SDDS_dataset, column_index))) {
    SDDS_SetError("Unable to get value--data type undefined (SDDS_GetValueByAbsIndex)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (type!=SDDS_STRING) {
    if (memory) {
      memcpy(memory, (char*)SDDS_dataset->data[column_index] + row_index*size, size);
      return(memory);
    }
    return((char*)SDDS_dataset->data[column_index] + row_index*size);
  }
  /* for character string data, a typical call would be
   * char *string;
   * SDDS_GetValueByAbsIndex(&SDDS_dataset, cindex, index, &string)    or
   * string = *(char**)SDDS_GetValue(&SDDS_dataset, cindex, index, NULL)
   */
  if (!memory)
    memory = SDDS_Malloc(size);
  if (SDDS_CopyString(memory, ((char**)SDDS_dataset->data[column_index])[row_index]))
    return(memory);
  return(NULL);
}

int32_t SDDS_GetRowType(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, type;
  type = -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetRowType"))
    return(0);
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (!SDDS_dataset->column_flag[i])
      continue;
    if (type==-1)
      type = SDDS_dataset->layout.column_definition[i].type;
    else if (type!=SDDS_dataset->layout.column_definition[i].type) {
      SDDS_SetError("Unable to get row type--inconsistent data type for selected columns (SDDS_GetRowType)");
      return(0);
    }
  }
  return(type);
}

void *SDDS_GetRow(SDDS_DATASET *SDDS_dataset, int32_t srow_index, void *memory)
{
  void *data;
  int32_t i, size, type, row_index;
  /*  int32_t j;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetRow"))
    return(NULL);
  if ((row_index=SDDS_GetSelectedRowIndex(SDDS_dataset, srow_index))<0) {
    SDDS_SetError("Unable to get row--row index out of range (SDDS_GetRow)");
    return(NULL);
  }
  if (SDDS_dataset->n_of_interest<=0) {
    SDDS_SetError("Unable to get row--no columns selected (SDDS_GetRow)");
    return(NULL);
  }
  if ((type=SDDS_GetRowType(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get row--inconsistent data type in selected columns (SDDS_GetRow)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (memory)
    data = memory;
  else if (!(data = SDDS_Malloc(size*SDDS_dataset->n_of_interest))) {
    SDDS_SetError("Unable to get row--memory allocation failure (SDDS_GetRow)");
    return(NULL);
  }
  if (type!=SDDS_STRING)
    for (i=0; i<SDDS_dataset->n_of_interest; i++) 
      memcpy((char*)data+i*size, (char*)SDDS_dataset->data[SDDS_dataset->column_order[i]]+row_index*size, size);
  else
    for (i=0; i<SDDS_dataset->n_of_interest; i++) 
      if (!SDDS_CopyString((char**)data+i, ((char**)SDDS_dataset->data[SDDS_dataset->column_order[i]])[row_index]))
        return(NULL);
  return(data);
}


void *SDDS_GetMatrixOfRows(SDDS_DATASET *SDDS_dataset, int32_t *n_rows)
{
  void **data;
  int32_t i, j, k, size, type;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetMatrixOfRows"))
    return(NULL);
  if (SDDS_dataset->n_of_interest<=0) {
    SDDS_SetError("Unable to get matrix of rows--no columns selected (SDDS_GetMatrixOfRows)");
    return(NULL);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_GetMatrixOfRows"))
    return(NULL);
  if ((type=SDDS_GetRowType(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get row--inconsistent data type in selected columns (SDDS_GetMatrixOfRows)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if ((*n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get matrix of rows--no rows of interest (SDDS_GetMatrixOfRows)");
    return(NULL);
  }
  if (!(data = (void**)SDDS_Malloc(sizeof(*data)*(*n_rows)))) {
    SDDS_SetError("Unable to get matrix of rows--memory allocation failure (SDDS_GetMatrixOfRows)");
    return(NULL);
  }
  for (j=k=0; j<SDDS_dataset->n_rows; j++) {
    if (SDDS_dataset->row_flag[j]) {
      if (!(data[k] = SDDS_Malloc(size*SDDS_dataset->n_of_interest))) {
        SDDS_SetError("Unable to get matrix of rows--memory allocation failure (SDDS_GetMatrixOfRows)");
        return(NULL);
      }
      if (type!=SDDS_STRING)
        for (i=0; i<SDDS_dataset->n_of_interest; i++)
          memcpy((char*)data[k]+i*size, (char*)SDDS_dataset->data[SDDS_dataset->column_order[i]]+j*size, size);
      else
        for (i=0; i<SDDS_dataset->n_of_interest; i++)
          if (!SDDS_CopyString((char**)(data[k])+i, ((char**)SDDS_dataset->data[SDDS_dataset->column_order[i]])[j]))
            return(NULL);
      k++;
    }
  }
  return(data);
}

void *SDDS_GetCastMatrixOfRows(SDDS_DATASET *SDDS_dataset, int32_t *n_rows, int32_t sddsType)
{
  void **data;
  int32_t i, j, k, size;
  /*  int32_t type;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetCastMatrixOfRows"))
    return(NULL);
  if (!SDDS_NUMERIC_TYPE(sddsType)) {
    SDDS_SetError("Unable to get matrix of rows--no columns selected (SDDS_GetCastMatrixOfRows)");
    return NULL;
  }
  if (SDDS_dataset->n_of_interest<=0) {
    SDDS_SetError("Unable to get matrix of rows--no columns selected (SDDS_GetCastMatrixOfRows)");
    return(NULL);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_GetCastMatrixOfRows"))
    return(NULL);
  size = SDDS_type_size[sddsType-1];
  if ((*n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get matrix of rows--no rows of interest (SDDS_GetCastMatrixOfRows)");
    return(NULL);
  }
  if (!(data = (void**)SDDS_Malloc(sizeof(*data)*(*n_rows)))) {
    SDDS_SetError("Unable to get matrix of rows--memory allocation failure (SDDS_GetCastMatrixOfRows)");
    return(NULL);
  }
  for (i=0; i<SDDS_dataset->n_of_interest; i++) {
    if (!SDDS_NUMERIC_TYPE(SDDS_dataset->layout.column_definition[SDDS_dataset->column_order[i]].type)) {
      SDDS_SetError("Unable to get matrix of rows--not all columns are numeric (SDDS_GetCastMatrixOfRows)");
      return NULL;
    }
  }
  for (j=k=0; j<SDDS_dataset->n_rows; j++) {
    if (SDDS_dataset->row_flag[j]) {
      if (!(data[k] = SDDS_Malloc(size*SDDS_dataset->n_of_interest))) {
        SDDS_SetError("Unable to get matrix of rows--memory allocation failure (SDDS_GetCastMatrixOfRows)");
        return(NULL);
      }
      for (i=0; i<SDDS_dataset->n_of_interest; i++)
        SDDS_CastValue(SDDS_dataset->data[SDDS_dataset->column_order[i]], j,
                       SDDS_dataset->layout.column_definition[SDDS_dataset->column_order[i]].type,
                       sddsType, (char*)data[k]+i*sizeof(double));
      k++;
    }
  }
  return(data);
}

int32_t SDDS_GetParameters(SDDS_DATASET *SDDS_dataset, ...)
{
  va_list argptr;
  char *name;
  void *data;
  int32_t retval;
  char s[SDDS_MAXLINE];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameters"))
    return 0;
  va_start(argptr, SDDS_dataset);
  retval = 1;
  do {
    if (!(name = va_arg(argptr, char *))) 
      break;
    if (!(data = va_arg(argptr, void *)))
      retval = 0;
    if (!SDDS_GetParameter(SDDS_dataset, name, data))  {
      sprintf(s, "Unable to get value of parameter %s (SDDS_GetParameters)", name);
      SDDS_SetError(s);
    }
  } while (retval);
  va_end(argptr);
  return retval;
}

void *SDDS_GetParameter(SDDS_DATASET *SDDS_dataset, char *parameter_name, void *memory)
{
  int32_t index, type, size;
  char s[SDDS_MAXLINE];
  void *data;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameter"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetParameter)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    sprintf(s, "Unable to get parameter value--parameter name %s is unrecognized (SDDS_GetParameter)", parameter_name);
    SDDS_SetError(s);
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameter)");
    return(NULL);
  }
  if (!SDDS_dataset->parameter || !SDDS_dataset->parameter[index]) {
    SDDS_SetError("Unable to get parameter value--parameter data array is NULL (SDDS_GetParameter)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (memory)
    data = memory;
  else if (!(data = SDDS_Malloc(size))) {
    SDDS_SetError("Unable to get parameter value--parameter data size is invalid (SDDS_GetParameter)");
    return(NULL);
  }
  if (type!=SDDS_STRING)
    memcpy(data, SDDS_dataset->parameter[index], size);
  else if (!SDDS_CopyString((char**)data, *(char**)SDDS_dataset->parameter[index]))
    return(NULL);
  return(data);
}

void *SDDS_GetParameterByIndex(SDDS_DATASET *SDDS_dataset, int32_t index, void *memory)
{
  int32_t type, size;
  void *data;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameter"))
    return(NULL);
  if (index<0 || index>=SDDS_dataset->layout.n_parameters) {
    SDDS_SetError("Unable to get parameter value--parameter index is invalid (SDDS_GetParameterByIndex)");
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterByIndex)");
    return(NULL);
  }
  if (!SDDS_dataset->parameter || !SDDS_dataset->parameter[index]) {
    SDDS_SetError("Unable to get parameter value--parameter data array is NULL (SDDS_GetParameterByIndex)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (memory)
    data = memory;
  else if (!(data = SDDS_Malloc(size))) {
    SDDS_SetError("Unable to get parameter value--parameter data size is invalid (SDDS_GetParameterByIndex)");
    return(NULL);
  }
  if (type!=SDDS_STRING)
    memcpy(data, SDDS_dataset->parameter[index], size);
  else if (!SDDS_CopyString((char**)data, *(char**)SDDS_dataset->parameter[index]))
    return(NULL);
  return(data);
}

int32_t *SDDS_GetParameterAsLong(SDDS_DATASET *SDDS_dataset, char *parameter_name, int32_t *memory)
{
  int32_t index, type, size;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterAsLong"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    SDDS_SetError("Unable to get parameter value--parameter name is unrecognized (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  if (type==SDDS_STRING) {
    SDDS_SetError("Unable to get parameter value--parameter data type is SDDS_STRING (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  /*
  if (type==SDDS_FLOAT) {
    SDDS_SetError("Unable to get parameter value--parameter data type is SDDS_FLOAT (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  if (type==SDDS_DOUBLE) {
    SDDS_SetError("Unable to get parameter value--parameter data type is SDDS_DOUBLE (SDDS_GetParameterAsLong)");
    return(NULL);
  } */
  if (!SDDS_dataset->parameter || !SDDS_dataset->parameter[index]) {
    SDDS_SetError("Unable to get parameter value--parameter data array is NULL (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];

  if (!memory && !(memory=(int32_t*)SDDS_Malloc(sizeof(int32_t)))) {
    SDDS_SetError("Unable to get parameter value--memory allocation failure (SDDS_GetParameterAsLong)");
    return(NULL);
  }
  
  *memory = SDDS_ConvertToLong(type, SDDS_dataset->parameter[index], 0);
  return(memory);
}

double *SDDS_GetParameterAsDouble(SDDS_DATASET *SDDS_dataset, char *parameter_name, double *memory)
{
  int32_t index=-1, type=-1, size=-1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterAsDouble"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    SDDS_SetError("Unable to get parameter value--parameter name is unrecognized (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  if (type==SDDS_STRING) {
    SDDS_SetError("Unable to get parameter value--parameter data type is SDDS_STRING (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  if (!SDDS_dataset->parameter || !SDDS_dataset->parameter[index]) {
    SDDS_SetError("Unable to get parameter value--parameter data array is NULL (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];

  if (!memory && !(memory=(double*)SDDS_Malloc(sizeof(double)))) {
    SDDS_SetError("Unable to get parameter value--memory allocation failure (SDDS_GetParameterAsDouble)");
    return(NULL);
  }
  *memory = SDDS_ConvertToDouble(type, SDDS_dataset->parameter[index], 0);
  return(memory);
}

char *SDDS_GetParameterAsString(SDDS_DATASET *SDDS_dataset, char *parameter_name, char **memory)
{
  int32_t index, type;
  char buffer[SDDS_MAXLINE], *parValue;
  void *value;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterAsString"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetParameterAsString)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    SDDS_SetError("Unable to get parameter value--parameter name is unrecognized (SDDS_GetParameterAsString)");
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterAsString)");
    return(NULL);
  }
  value=SDDS_dataset->parameter[index];
  switch (type) {
  case SDDS_DOUBLE:
    sprintf(buffer,"%13.6le",*(double*)value);
    break;
  case SDDS_FLOAT:
    sprintf(buffer,"%13.6f",*(float*)value);
    break;
  case SDDS_LONG:
    sprintf(buffer,"%" PRId32,*(int32_t*)value);
    break;
  case SDDS_ULONG:
    sprintf(buffer,"%" PRIu32,*(uint32_t*)value);
    break;
  case SDDS_SHORT:
    sprintf(buffer,"%hd",*(short*)value);
    break;
  case SDDS_USHORT:
    sprintf(buffer,"%hu",*(unsigned short*)value);
    break;
  case SDDS_CHARACTER:
    sprintf(buffer,"%c",*(char*)value);
    break;
  case SDDS_STRING:
    sprintf(buffer, "%s",*(char**)value);
    break;
  default:
    SDDS_SetError("Unknow data type of parameter (SDDS_GetParameterAsString)");
    return(NULL);
  }
  if (!(parValue=malloc(sizeof(char)*(strlen(buffer)+1)))) {
    SDDS_SetError("Unable to get parameter value--memory allocation failure (SDDS_GetParameterAsString)");
    return(NULL);
  }
  strcpy(parValue, buffer);   
  if (memory)
    *memory=parValue;
  return parValue;   
}

void *SDDS_GetFixedValueParameter(SDDS_DATASET *SDDS_dataset, char *parameter_name, void *memory)
{
  int32_t index, type, size;
  void *data;
  static char s[SDDS_MAXLINE];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetFixValueParameter"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetFixedValueParameter)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    SDDS_SetError("Unable to get parameter value--parameter name is unrecognized (SDDS_GetFixedValueParameter)");
    return(NULL);
  }
  if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
    SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetFixedValueParameter)");
    return(NULL);
  }
  size = SDDS_type_size[type-1];
  if (memory)
    data = memory;
  else if (!(data = SDDS_Malloc(size))) {
    SDDS_SetError("Unable to get parameter value--parameter data size is invalid (SDDS_GetFixedValueParameter)");
    return(NULL);
  }
  strcpy(s, SDDS_dataset->layout.parameter_definition[index].fixed_value);
  if (!SDDS_ScanData(s, type, 0, data, 0, 1)) {
    SDDS_SetError("Unable to retrieve fixed-value paramter--scan failed (SDDS_GetFixedValueParameter)");
    return(NULL);
  }
  return(data);
}

void *SDDS_GetMatrixFromColumn(SDDS_DATASET *SDDS_dataset, char *column_name, int32_t dimension1, int32_t dimension2, int32_t mode)
{
  int32_t index, size, n_rows, i, j, type;
  void **data, *column;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetMatrixFromColumn"))
    return(NULL);
  if (!column_name) {
    SDDS_SetError("Unable to get matrix--column name is NULL (SDDS_GetMatrixFromColumn)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get matrix--no rows selected (SDDS_GetMatrixFromColumn)");
    return(NULL);
  }
  if (n_rows!=dimension1*dimension2) {
    char s[1024];
    sprintf(s, "Unable to get matrix--number of rows (%" PRId32 ") doesn't correspond to given dimensions (%" PRId32 " x %" PRId32 ") (SDDS_GetMatrixFromColumn)", n_rows, dimension1, dimension2);
    SDDS_SetError(s);
    return(NULL);
  }
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0 || (type=SDDS_GetColumnType(SDDS_dataset, index))<0 ||
      (size=SDDS_GetTypeSize(type))<=0) {
    SDDS_SetError("Unable to get matrix--column name is unrecognized (SDDS_GetMatrixFromColumn)");
    return(NULL);
  }
  if (!(column=SDDS_GetColumn(SDDS_dataset, column_name))) {
    SDDS_SetError("Unable to get matrix (SDDS_GetMatrixFromColumn)");
    return(NULL);
  }
  if (!(data = SDDS_AllocateMatrix(size, dimension1, dimension2))) {
    SDDS_SetError("Unable to allocate matrix (SDDS_GetMatrixFromColumn)"); 
    return(NULL);
  }
  if (mode&SDDS_ROW_MAJOR_DATA || !(mode&SDDS_COLUMN_MAJOR_DATA)) {
    for (i=0; i<dimension1; i++)
      memcpy(data[i], (char*)column+i*dimension2*size, dimension2*size); 
  }
  else {
    for (i=0; i<dimension1; i++) {
      for (j=0; j<dimension2; j++) {
        memcpy((char*)data[i]+size*j, (char*)column+(j*dimension1+i)*size, size); 
      }
    }
  }

  free(column);
  return(data);
}

void *SDDS_GetDoubleMatrixFromColumn(SDDS_DATASET *SDDS_dataset, char *column_name, int32_t dimension1, int32_t dimension2, int32_t mode)
{
  int32_t index, size, n_rows, i, j;
  void **data, *column;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetDoubleMatrixFromColumn"))
    return(NULL);
  if (!column_name) {
    SDDS_SetError("Unable to get matrix--column name is NULL (SDDS_GetDoubleMatrixFromColumn)");
    return(NULL);
  }
  if ((n_rows=SDDS_CountRowsOfInterest(SDDS_dataset))<=0) {
    SDDS_SetError("Unable to get matrix--no rows selected (SDDS_GetDoubleMatrixFromColumn)");
    return(NULL);
  }
  if (n_rows!=dimension1*dimension2) {
    char s[1024];
    sprintf(s, "Unable to get matrix--number of rows (%" PRId32 ") doesn't correspond to given dimensions (%" PRId32 " x %" PRId32 ") (SDDS_GetDoubleMatrixFromColumn)", n_rows, dimension1, dimension2);
    SDDS_SetError(s);
    return(NULL);
  }
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to get matrix--column name is unrecognized (SDDS_GetDoubleMatrixFromColumn)");
    return(NULL);
  }
  if (!(column=SDDS_GetColumnInDoubles(SDDS_dataset, column_name))) {
    SDDS_SetError("Unable to get matrix (SDDS_GetDoubleMatrixFromColumn)");
    return(NULL);
  }
  size = sizeof(double);
  if (!(data = SDDS_AllocateMatrix(size, dimension1, dimension2))) {
    SDDS_SetError("Unable to allocate matrix (SDDS_GetDoubleMatrixFromColumn)"); 
    return(NULL);
  }
  if (mode&SDDS_ROW_MAJOR_DATA || !(mode&SDDS_COLUMN_MAJOR_DATA)) {
    for (i=0; i<dimension1; i++)
      memcpy(data[i], (char*)column+i*dimension2*size, dimension2*size); 
  }
  else {
    for (i=0; i<dimension1; i++) {
      for (j=0; j<dimension2; j++) {
        memcpy((char*)data[i]+size*j, (char*)column+(j*dimension1+i)*size, size); 
      }
    }
  }

  free(column);
  return(data);
}

int32_t SDDS_SetRowsOfInterest(SDDS_DATASET *SDDS_dataset, char *selection_column, int32_t mode, ...)
/* This routine has 4 calling modes:
 * SDDS_SetRowsOfInterest(&SDDS_dataset, selection_column, SDDS_NAME_ARRAY, int32_t n_entries, char **name)
 * SDDS_SetRowsOfInterest(&SDDS_dataset, selection_column, SDDS_NAMES_STRING, char *names)
 * SDDS_SetRowsOfInterest(&SDDS_dataset, selection_column, SDDS_NAME_STRINGS, char *name1, char *name2, ..., NULL )
 * SDDS_SetRowsOfInterest(&SDDS_dataset, selection_column, SDDS_MATCH_STRING, char *name, int32_t logic_mode)
 */
{
  va_list argptr;
  int32_t i, j, index, n_names, retval, type;
  char **name, *string, *match_string, *ptr;
  int32_t local_memory;    /* (0,1,2) --> (none, pointer array, pointer array + strings) locally allocated */
  static char buffer[SDDS_MAXLINE];
  int32_t logic, count, caseSensitive;

  name = NULL;
  n_names = local_memory = logic = 0;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetRowsOfInterest"))
    return(-1);
  va_start(argptr, mode);
  retval = 1;
  caseSensitive = 1;
  match_string = NULL;
  switch (mode) {
  case SDDS_CI_NAME_ARRAY:
    caseSensitive = 0;
  case SDDS_NAME_ARRAY:
    local_memory = 0;
    n_names = va_arg(argptr, int32_t);
    name = va_arg(argptr, char **);
    break;
  case SDDS_CI_NAMES_STRING:
    caseSensitive = 0;
  case SDDS_NAMES_STRING:
    local_memory = 2;
    n_names = 0;
    name = NULL;
    ptr = va_arg(argptr, char *);
    SDDS_CopyString(&string, ptr);
    while (SDDS_GetToken(string, buffer, SDDS_MAXLINE)>0) {
      if (!(name = SDDS_Realloc(name, sizeof(*name)*(n_names+1))) || !SDDS_CopyString(name+n_names, buffer)) {
        SDDS_SetError("Unable to process row selection--memory allocation failure (SDDS_SetRowsOfInterest)");
        retval = -1;
        break;
      }
      n_names++;
    }
    free(string);
    break;
  case SDDS_CI_NAME_STRINGS:
    caseSensitive = 0;
  case SDDS_NAME_STRINGS:
    local_memory = 1;
    n_names = 0;
    name = NULL;
    while ((string=va_arg(argptr, char *))) {
      if (!(name = SDDS_Realloc(name, sizeof(*name)*(n_names+1)))) {
        SDDS_SetError("Unable to process row selection--memory allocation failure (SDDS_SetRowsOfInterest)");
        retval = -1;
        break;
      }
      name[n_names++] = string;
    }
    break;
  case SDDS_CI_MATCH_STRING:
    caseSensitive = 0;
  case SDDS_MATCH_STRING:
    local_memory = 0;
    n_names = 1;
    if ((string=va_arg(argptr, char *))) 
      match_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    if (logic&SDDS_NOCASE_COMPARE)
      caseSensitive = 0;
    break;
  default:
    SDDS_SetError("Unable to process row selection--unknown mode (SDDS_SetRowsOfInterest)");
    retval = -1;
    break;
  }

  va_end(argptr);
  if (retval!=1)
    return(-1);
  
  if (mode!=SDDS_MATCH_STRING && mode!=SDDS_CI_MATCH_STRING) {
    int (*stringCompare)(const char *s1, const char *s2);
    if (caseSensitive)
      stringCompare = strcmp;
    else
      stringCompare = strcmp_ci;
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, selection_column))<0) {
      SDDS_SetError("Unable to process row selection--unrecognized selection column name (SDDS_SetRowsOfInterest)");
      return(-1);
    }
    if ((type=SDDS_GetColumnType(SDDS_dataset, index))!=SDDS_STRING) {
      SDDS_SetError("Unable to select rows--selection column is not string type (SDDS_SetRowsOfInterest)");
      return(-1);
    }
    if (n_names==0) {
      SDDS_SetError("Unable to process row selection--no names in call (SDDS_SetRowsOfInterest)");
      return(-1);
    }
    for (j=0; j<n_names; j++) {
      for (i=0; i<SDDS_dataset->n_rows; i++) {
        if ((*stringCompare)(*((char**)SDDS_dataset->data[index]+i), name[j])==0)
          SDDS_dataset->row_flag[i] = 1;
      }
    }
  }
  else {
    if (selection_column)  {
      int (*wildMatch)(char *string, char *template);
      if (caseSensitive)
        wildMatch = wild_match;
      else
        wildMatch = wild_match_ci;
      if (!match_string) {
        SDDS_SetError("Unable to select rows--no matching string given (SDDS_SetRowsOfInterest)");
        return(-1);
      }
      if ((index=SDDS_GetColumnIndex(SDDS_dataset, selection_column))<0) {
        free(match_string);
        SDDS_SetError("Unable to process row selection--unrecognized selection column name (SDDS_SetRowsOfInterest)");
        return(-1);
      }
      if ((type=SDDS_GetColumnType(SDDS_dataset, index))!=SDDS_STRING) {
        free(match_string);
        SDDS_SetError("Unable to select rows--selection column is not string type (SDDS_SetRowsOfInterest)");
        return(-1);
      }
      for (i=0; i<SDDS_dataset->n_rows; i++) 
        SDDS_dataset->row_flag[i] = 
          SDDS_Logic(SDDS_dataset->row_flag[i], 
                     (*wildMatch)(*((char**)SDDS_dataset->data[index]+i), match_string), logic);
    } else {
      for (i=0; i<SDDS_dataset->n_rows; i++)
        SDDS_dataset->row_flag[i] 
          = SDDS_Logic(SDDS_dataset->row_flag[i], 0, logic&~(SDDS_AND|SDDS_OR));
    }
  }

  if (local_memory==2) {
    for (i=0; i<n_names; i++)
      free(name[i]);
  }
  if (match_string)
    free(match_string);
  if (local_memory>=1)
    free(name);

  for (i=count=0; i<SDDS_dataset->n_rows; i++)
    if (SDDS_dataset->row_flag[i])
      count++;
  return(count);
}

int32_t SDDS_MatchRowsOfInterest(SDDS_DATASET *SDDS_dataset, char *selection_column, char *label_to_match, int32_t logic)
{
  int32_t match, i, index, size, count, type;
  int32_t indirect_index;
  char *match_string;
#ifndef tolower
  int tolower();
#endif

  index = type = indirect_index = 0;

  match_string = NULL;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MatchRowsOfInterest"))
    return(-1);
  if (selection_column) {
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, selection_column))<0) {
      SDDS_SetError("Unable to select rows--column name is unrecognized (SDDS_MatchRowsOfInterest)");
      return(-1);
    }
    if ((type=SDDS_GetColumnType(SDDS_dataset, index))!=SDDS_STRING &&
        type!=SDDS_CHARACTER) {
      SDDS_SetError("Unable to select rows--selection column is not a string (SDDS_MatchRowsOfInterest)");
      return(-1);
    }
    size = SDDS_GetTypeSize(type);
    if (!label_to_match) {
      SDDS_SetError("Unable to select rows--selection label is NULL (SDDS_MatchRowsOfInterest)");
      return(-1);
    }
    if (!(logic&SDDS_INDIRECT_MATCH))
      match_string = expand_ranges(label_to_match);
    else {
      if ((indirect_index=SDDS_GetColumnIndex(SDDS_dataset, label_to_match))<0) {
        SDDS_SetError("Unable to select rows--indirect column name is unrecognized (SDDS_MatchRowsOfInterest)");
        return(-1);
      }
      if (SDDS_GetColumnType(SDDS_dataset, indirect_index)!=type) {
        SDDS_SetError("Unable to select rows--indirect column is not same type as main column (SDDS_MatchRowsOfInterest)");
        return(-1);
      }
    }
  }
  if (type==SDDS_STRING) {
    int (*stringCompare)(const char *s, const char *t);
    int (*wildMatch)(char *s, char *t);
    if (logic&SDDS_NOCASE_COMPARE) {
      stringCompare = strcmp_ci;
      wildMatch = wild_match_ci;
    }
    else {
      stringCompare = strcmp;
      wildMatch = wild_match;
    }
    for (i=count=0; i<SDDS_dataset->n_rows; i++) {
      if (selection_column) 
        match = SDDS_Logic
          (SDDS_dataset->row_flag[i], 
           (logic&SDDS_INDIRECT_MATCH?
            (*stringCompare)(*((char**)SDDS_dataset->data[index]+i), 
                             *((char**)SDDS_dataset->data[indirect_index]+i))==0:
            (*wildMatch)(*((char**)SDDS_dataset->data[index]+i), match_string)),
           logic);
      else
        match = SDDS_Logic(SDDS_dataset->row_flag[i], 0, logic&~(SDDS_AND|SDDS_OR));
      if ((SDDS_dataset->row_flag[i] = match))
        count++;
    }
  } else {
    char c1, c2;
    c2 = 0;
    if (!(logic&SDDS_INDIRECT_MATCH))
      c2 = *match_string;
    if (logic&SDDS_NOCASE_COMPARE) {
      c2 = tolower(c2);
      for (i=count=0; i<SDDS_dataset->n_rows; i++) {
        c1 = tolower(*((char*)SDDS_dataset->data[index]+i));
        if (selection_column)
          match = SDDS_Logic
            (SDDS_dataset->row_flag[i], 
             logic&SDDS_INDIRECT_MATCH
             ? c1==tolower(*((char*)SDDS_dataset->data[indirect_index]+i))
             : c1==c2,
             logic);
        else
          match = SDDS_Logic(SDDS_dataset->row_flag[i], 0, logic&~(SDDS_AND|SDDS_OR));
        if ((SDDS_dataset->row_flag[i] = match))
          count++;
      }
    }
    else {
      for (i=count=0; i<SDDS_dataset->n_rows; i++) {
        c1 = *((char*)SDDS_dataset->data[index]+i);
        if (selection_column)
          match = SDDS_Logic
            (SDDS_dataset->row_flag[i], 
             logic&SDDS_INDIRECT_MATCH
             ? c1==*((char*)SDDS_dataset->data[indirect_index]+i)
             : c1==c2,
             logic);
        else
          match = SDDS_Logic(SDDS_dataset->row_flag[i], 0, logic&~(SDDS_AND|SDDS_OR));
        if ((SDDS_dataset->row_flag[i] = match))
          count++;
      }
    }
  }
  if (match_string)
    free(match_string);
  return(count);
}

int32_t SDDS_FilterRowsOfInterest(SDDS_DATASET *SDDS_dataset, char *filter_column, double lower_limit, double upper_limit, int32_t logic)
{
  int32_t accept, i, index, count, type;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_FilterRowsOfInterest"))
    return(-1);
  if (!filter_column) {
    SDDS_SetError("Unable to filter rows--filter column name not given (SDDS_FilterRowsOfInterest)");
    return(-1);
  }
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, filter_column))<0) {
    SDDS_SetError("Unable to filter rows--column name is unrecognized (SDDS_FilterRowsOfInterest)");
    return(-1);
  }
  switch (type=SDDS_GetColumnType(SDDS_dataset, index)) {
  case SDDS_SHORT: case SDDS_USHORT: case SDDS_LONG: case SDDS_ULONG: case SDDS_FLOAT: case SDDS_DOUBLE:
    break;
  default:
    SDDS_SetError("Unable to filter rows--filter column is not a numeric type (SDDS_FilterRowsOfInterest)");
    return(-1);
  }
  for (i=count=0; i<SDDS_dataset->n_rows; i++) {
    if (logic&SDDS_NEGATE_PREVIOUS) 
      SDDS_dataset->row_flag[i] = !SDDS_dataset->row_flag[i];
    accept = SDDS_ItemInsideWindow(SDDS_dataset->data[index], i, type, lower_limit, upper_limit); 
    if (logic&SDDS_NEGATE_MATCH)
      accept = !accept;
    if (logic&SDDS_AND)
      accept = accept && SDDS_dataset->row_flag[i];
    else if (logic&SDDS_OR)
      accept = accept || SDDS_dataset->row_flag[i];
    if (logic&SDDS_NEGATE_EXPRESSION)
      accept = !accept;
    if ((SDDS_dataset->row_flag[i] = accept))
      count++;
  }
  return(count);
}

int32_t SDDS_FilterRowsByNumScan(SDDS_DATASET *SDDS_dataset, char *filter_column, uint32_t mode)
{
  int32_t accept, i, index, count;
  /*  int32_t length;*/
  short invert;
  /*  short first;*/
  /*  double value;*/
  char *ptr;
  /*  static int32_t bufferSize = 0;*/
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_FilterRowsByNumScan"))
    return(-1);
  if (!filter_column) {
    SDDS_SetError("Unable to filter rows--filter column name not given (SDDS_FilterRowsByNumScan)");
    return(-1);
  }
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, filter_column))<0) {
    SDDS_SetError("Unable to filter rows--column name is unrecognized (SDDS_FilterRowsByNumScan)");
    return(-1);
  }
  switch (SDDS_GetColumnType(SDDS_dataset, index)) {
  case SDDS_SHORT: case SDDS_USHORT: case SDDS_LONG: case SDDS_ULONG: case SDDS_FLOAT: case SDDS_DOUBLE: case SDDS_CHARACTER:
    SDDS_SetError("Unable to filter rows--filter column is not string type (SDDS_FilterRowsByNumScan)");
    return(-1);
  default:
    break;
  }
  invert = mode&NUMSCANFILTER_INVERT ? 1 : 0;
  for (i=count=0; i<SDDS_dataset->n_rows; i++) {
    ptr = ((char**)(SDDS_dataset->data[index]))[i];
    accept = !invert;
    if (!tokenIsNumber(ptr))
      accept = invert;
    if ((SDDS_dataset->row_flag[i] = accept))
      count++;
  }
  return(count);
}

int32_t SDDS_DeleteUnsetRows(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j;
  /*  int32_t k;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DeleteUnsetRows"))
    return(0);

  for (i=j=0; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i]) {
      if (i!=j) {
        SDDS_dataset->row_flag[j] = SDDS_dataset->row_flag[i];
        if (!SDDS_TransferRow(SDDS_dataset, j, i)) {
          SDDS_SetError("Unable to delete unset rows--problem copying row (SDDS_DeleteUnsetRows)");
          return(0);
        }
      }
      j++;
    }
  }
  SDDS_dataset->n_rows = j;
  return(1);
}

int32_t SDDS_TransferRow(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source)
{
  int32_t i, size;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_TransferRow"))
    return(0);
  for (i=0; i<SDDS_dataset->layout.n_columns; i++)  {
    if (SDDS_dataset->layout.column_definition[i].type!=SDDS_STRING) {
      size = SDDS_type_size[SDDS_dataset->layout.column_definition[i].type-1];
      memcpy((char*)SDDS_dataset->data[i]+target*size, (char*)SDDS_dataset->data[i]+source*size, size);
    }
    else {
      if (((char***)SDDS_dataset->data)[i][target])
        free(((char***)SDDS_dataset->data)[i][target]);
      ((char***)SDDS_dataset->data)[i][target] = NULL;
      if (!SDDS_CopyString(((char***)SDDS_dataset->data)[i]+target, ((char***)SDDS_dataset->data)[i][source]))
        return((int32_t)NULL);
    }
  }
  return(1);
}

int32_t SDDS_DeleteColumn(SDDS_DATASET *SDDS_dataset, char *column_name)
{
  int32_t i, j, index;

  SDDS_Bomb("SDDS_DeleteColumn is presently not functional.");
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DeleteColumn"))
    return(0);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
    SDDS_SetError("Unable to delete column--unrecognized column name (SDDS_DeleteColumn)");
    return(0);
  }
  for (i=index+1; i<SDDS_dataset->layout.n_columns; i++) {
    if (!SDDS_CopyColumn(SDDS_dataset, i-1, i)) {
      SDDS_SetError("Unable to delete column--error copying column (SDDS_DeleteColumn)");
      return(0);
    }
    for (j=0; j<SDDS_dataset->n_of_interest; j++)
      if (SDDS_dataset->column_order[j]==index) {
        memcpy((char*)(SDDS_dataset->column_order+j), (char*)(SDDS_dataset->column_order+j+1),
               sizeof(*SDDS_dataset->column_order)*(SDDS_dataset->n_of_interest-j-1));
        SDDS_dataset->n_of_interest--;
      }
      else if (SDDS_dataset->column_order[j]>index)
        SDDS_dataset->column_order[j] -= 1;
  }
  if ((SDDS_dataset->layout.n_columns -= 1)==0) 
    SDDS_dataset->n_rows = 0;
  return(1);
}

int32_t SDDS_DeleteUnsetColumns(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  /*  int32_t j, index;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DeleteUnsetColumns"))
    return(0);
  for (i=0; i<SDDS_dataset->layout.n_columns; i++)
    if (!SDDS_dataset->column_flag[i]) {
      if (!SDDS_DeleteColumn(SDDS_dataset, SDDS_dataset->layout.column_definition[i].name))
        return(0);
      else
        i--;
    }
  return(1);
}

int32_t SDDS_CopyColumn(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source)
{
  COLUMN_DEFINITION *cd_target, *cd_source;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_CopyColumn"))
    return(0);
  if (target<0 || source<0 || target>=SDDS_dataset->layout.n_columns || source>=SDDS_dataset->layout.n_columns) {
    SDDS_SetError("Unable to copy column--target or source index out of range (SDDS_CopyColumn");
    return(0);
  }
  cd_target = SDDS_dataset->layout.column_definition+target;
  cd_source = SDDS_dataset->layout.column_definition+source;
  SDDS_dataset->column_flag[target] = SDDS_dataset->column_flag[source] ;
  if (SDDS_dataset->n_rows_allocated) {
    if (cd_target->type!=cd_source->type) {
      if (!(SDDS_dataset->data[target] = 
            SDDS_Realloc(SDDS_dataset->data[target], SDDS_type_size[cd_source->type-1]*SDDS_dataset->n_rows_allocated))) {
        SDDS_SetError("Unable to copy column--memory allocation failure (SDDS_CopyColumn)");
        return(0);
      }
    }
    if (cd_source->type!=SDDS_STRING)
      memcpy(SDDS_dataset->data[target], SDDS_dataset->data[source], 
             SDDS_type_size[cd_source->type-1]*SDDS_dataset->n_rows);
    else if (!SDDS_CopyStringArray(SDDS_dataset->data[target], SDDS_dataset->data[source], SDDS_dataset->n_rows)) {
      SDDS_SetError("Unable to copy column--string copy failure (SDDS_CopyColumn)");
      return(0);
    }
  }
  memcpy((char*)cd_target, (char*)cd_source, sizeof(*cd_target));
  return(1);
}

int32_t SDDS_DeleteParameter(SDDS_DATASET *SDDS_dataset, char *parameter_name)
{
  int32_t i, index;
  /*  int32_t j;*/
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DeleteParameter"))
    return(0);
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    SDDS_SetError("Unable to delete parameter--unrecognized parameter name (SDDS_DeleteParameter)");
    return(0);
  }
  for (i=index+1; i<SDDS_dataset->layout.n_parameters; i++) {
    if (!SDDS_CopyParameter(SDDS_dataset, i-1, i)) {
      SDDS_SetError("Unable to delete parameter--error copying parameter (SDDS_DeleteParameter)");
      return(0);
    }
  }
  SDDS_dataset->layout.n_parameters -= 1;
  return(1);
}


int32_t SDDS_CopyParameter(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source)
{
  PARAMETER_DEFINITION *cd_target, *cd_source;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_CopyParameter"))
    return(0);
  if (target<0 || source<0 || target>=SDDS_dataset->layout.n_parameters || 
      source>=SDDS_dataset->layout.n_parameters) {
    SDDS_SetError("Unable to copy parameter--target or source index out of range (SDDS_CopyParameter");
    return(0);
  }
  cd_target = SDDS_dataset->layout.parameter_definition+target;
  cd_source = SDDS_dataset->layout.parameter_definition+source;
  if (SDDS_dataset->parameter) {
    if (cd_target->type!=cd_source->type) {
      if (!(SDDS_dataset->parameter[target] = 
            SDDS_Realloc(SDDS_dataset->data[target], SDDS_type_size[cd_source->type-1]))) {
        SDDS_SetError("Unable to copy parameter--memory allocation failure (SDDS_CopyParameter)");
        return(0);
      }
    }
    if (cd_source->type!=SDDS_STRING)
      memcpy(SDDS_dataset->parameter[target], SDDS_dataset->parameter[source], 
             SDDS_type_size[cd_source->type-1]);
    else if (!SDDS_CopyStringArray(SDDS_dataset->parameter[target], SDDS_dataset->parameter[source], 1)) {
      SDDS_SetError("Unable to copy parameter--string copy failure (SDDS_CopyParameter)");
      return(0);
    }
  }
  memcpy((char*)cd_target, (char*)cd_source, sizeof(*cd_target));
  return(1);
}


int32_t SDDS_ItemInsideWindow(void *data, int32_t index, int32_t type, double lower_limit, double upper_limit)
{
  short short_val;
  unsigned short ushort_val;
  int32_t long_val;
  uint32_t ulong_val;
  double double_val;
  float float_val;

  switch (type) {
  case SDDS_SHORT:
    if ((short_val=*((short*)data+index))<lower_limit || short_val>upper_limit)
      return(0);
    return(1);
  case SDDS_USHORT:
    if ((ushort_val=*((unsigned short*)data+index))<lower_limit || ushort_val>upper_limit)
      return(0);
    return(1);
  case SDDS_LONG:
    if ((long_val=*((int32_t*)data+index))<lower_limit || long_val>upper_limit)
      return(0);
    return(1);
  case SDDS_ULONG:
    if ((ulong_val=*((uint32_t*)data+index))<lower_limit || ulong_val>upper_limit)
      return(0);
    return(1);
  case SDDS_FLOAT:
    if ((float_val=*((float*)data+index))<lower_limit || float_val>upper_limit)
      return(0);
    if (isnan(float_val) || isinf(float_val))
      return 0;
    return(1);
  case SDDS_DOUBLE:
    if ((double_val=*((double*)data+index))<lower_limit || double_val>upper_limit)
      return 0; 
    if (isnan(double_val) || isinf(double_val))
      return 0;
    return(1);
  default:
    SDDS_SetError("Unable to complete window check--item type is non-numeric (SDDS_ItemInsideWindow)");
    return(0);
  }
}


int32_t SDDS_Logic(int32_t previous, int32_t match, uint32_t logic)
{
  if (logic&SDDS_0_PREVIOUS)
    previous = 0;
  else if (logic&SDDS_1_PREVIOUS)
    previous = 1;
  if (logic&SDDS_NEGATE_PREVIOUS)
    previous = !previous;
  if (logic&SDDS_NEGATE_MATCH)
    match = !match;
  if (logic&SDDS_AND)
    match = match && previous;
  else if (logic&SDDS_OR)
    match = match || previous;
  else
    match = previous;
  if (logic&SDDS_NEGATE_EXPRESSION)
    match = !match;
  return(match);
}

SDDS_ARRAY *SDDS_GetArray(SDDS_DATASET *SDDS_dataset, char *array_name, SDDS_ARRAY *memory)
{
  int32_t index, type, size;
  SDDS_ARRAY *copy, *original;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArray"))
    return(NULL);
  if (!array_name) {
    SDDS_SetError("Unable to get array--array name pointer is NULL (SDDS_GetArray)");
    return(NULL);
  }
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
    SDDS_SetError("Unable to get array--array name is unrecognized (SDDS_GetArray)");
    return(NULL);
  }
  if (memory)
    copy = memory;
  else if (!(copy = (SDDS_ARRAY*)calloc(1, sizeof(*copy)))) {
    SDDS_SetError("Unable to get array--allocation failure (SDDS_GetArray)");
    return(NULL);
  }
  original = SDDS_dataset->array+index;
  if (copy->definition && !SDDS_FreeArrayDefinition(copy->definition)) {
    SDDS_SetError("Unable to get array--array definition corrupted (SDDS_GetArray)");
    return(NULL);
  }
  if (!SDDS_CopyArrayDefinition(&copy->definition, original->definition)) {
    SDDS_SetError("Unable to get array--array definition missing (SDDS_GetArray)");
    return(NULL);
  }
  type = copy->definition->type;
  size = SDDS_type_size[copy->definition->type-1];
  if (!(copy->dimension = SDDS_Realloc(copy->dimension, sizeof(*copy->dimension)*copy->definition->dimensions))) {
    SDDS_SetError("Unable to get array--allocation failure (SDDS_GetArray)");
    return(NULL);
  }
  memcpy((void*)copy->dimension, (void*)original->dimension, sizeof(*copy->dimension)*copy->definition->dimensions);
  if (!(copy->elements = original->elements))
    return(copy);
  if (!(copy->data = SDDS_Realloc(copy->data, size*original->elements))) {
    SDDS_SetError("Unable to get array--allocation failure (SDDS_GetArray)");
    return(NULL);
  }
  
  if (copy->definition->type!=SDDS_STRING)
    memcpy(copy->data, original->data, size*copy->elements);
  else if (!SDDS_CopyStringArray((char**)copy->data, (char**)original->data, original->elements)) {
    SDDS_SetError("Unable to get array--string copy failure (SDDS_GetArray)");
    return(NULL);
  }

  /* should free existing subpointers here, but probably not worth the trouble */
  if (copy->pointer && copy->definition->dimensions!=1)
    free(copy->pointer);
  if (!(copy->pointer = 
        SDDS_MakePointerArray(copy->data, type, copy->definition->dimensions, copy->dimension))) {
    SDDS_SetError("Unable to get array--couldn't make pointer array (SDDS_GetArray)");
    return(NULL);
  }
  return(copy);
}

double *SDDS_GetArrayInDoubles(SDDS_DATASET *SDDS_dataset, char *array_name, int32_t *values)
{
  int32_t index, type, i, elements;
  SDDS_ARRAY *original;
  double *data;
  void *rawData;
  
  *values = 0;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayInDoubles"))
    return(NULL);
  if (!array_name) {
    SDDS_SetError("Unable to get array--array name pointer is NULL (SDDS_GetArrayInDoubles)");
    return(NULL);
  }
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
    SDDS_SetError("Unable to get array--array name is unrecognized (SDDS_GetArrayInDoubles)");
    return(NULL);
  }
  original = SDDS_dataset->array+index;
  if ((type = original->definition->type)==SDDS_STRING) {
    SDDS_SetError("Unable to get array--string type (SDDS_GetArrayInDoubles)");
    return(NULL);
  }
  elements = original->elements;
  if (!(data = SDDS_Malloc(sizeof(*data)*elements))) {
    SDDS_SetError("Unable to get array--allocation failure (SDDS_GetArrayInDoubles)");
    return(NULL);
  }
  rawData = original->data;
  switch (type) {
  case SDDS_DOUBLE:
    for (i=0; i<elements; i++) {
      data[i] = ((double*)rawData)[i];
    }
    break;
  case SDDS_FLOAT:
    for (i=0; i<elements; i++) {
      data[i] = ((float*)rawData)[i];
    }
    break;
  case SDDS_LONG:
    for (i=0; i<elements; i++) {
      data[i] = ((int32_t*)rawData)[i];
    }
    break;
  case SDDS_ULONG:
    for (i=0; i<elements; i++) {
      data[i] = ((uint32_t*)rawData)[i];
    }
    break;
  case SDDS_SHORT:
    for (i=0; i<elements; i++) {
      data[i] = ((short*)rawData)[i];
    }
    break;
  case SDDS_USHORT:
    for (i=0; i<elements; i++) {
      data[i] = ((unsigned short*)rawData)[i];
    }
    break;
  case SDDS_CHARACTER:
    for (i=0; i<elements; i++) {
      data[i] = ((char*)rawData)[i];
    }
    break;
  }
  *values = elements;
  return data;
}

int32_t SDDS_GetDescription(SDDS_DATASET *SDDS_dataset, char **text, char **contents)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetDescription"))
    return(0);
  if (text) {
    *text = NULL;
    if (!SDDS_CopyString(text, SDDS_dataset->layout.description)) {
      SDDS_SetError("Unable to retrieve description data (SDDS_GetDescription)");
      return(0);
    }
  }
  if (contents) {
    *contents = NULL;
    if (!SDDS_CopyString(contents, SDDS_dataset->layout.contents)) {
      SDDS_SetError("Unable to retrieve description data (SDDS_GetDescription)");
      return(0);
    }
  }
  
  return(1);
}
