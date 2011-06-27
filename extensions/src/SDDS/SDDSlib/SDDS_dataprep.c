/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_dataprep.c
 * contents: user's SDDS routines for data set preparation
 *
 * M. Borland, November 1993
 $Log: SDDS_dataprep.c,v $
 Revision 1.26  2010/02/18 02:16:29  borland
 Added SDDS_SetError0() and modified SDDS_PrintErrors() to allow more easily composing
 multi-part error messages.  One side-effect is that stack traces come out in reverse
 order to previously.  Also modified SDDS_SetColumn() to use the new feature.

 Revision 1.25  2008/09/18 20:16:40  soliday
 Added SDDS_SetColumnFromDoubles

 Revision 1.24  2008/04/08 14:32:50  shang
 labeled the error message.

 Revision 1.23  2006/08/31 15:08:01  soliday
 Updated to work with SDDS2

 Revision 1.22  2005/11/04 22:46:51  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.21  2004/05/13 18:15:38  soliday
 Added SDDS_AppendToArrayVararg

 Revision 1.20  2004/01/16 18:35:52  borland
 SDDS_ClearPage() now frees string arrays to prevent memory leaks.

 Revision 1.19  2003/11/11 16:39:25  borland
 SDDS_SetColumn, SDDS_SetRowValues: when setting a string column,
 checks first that the column does not already have strings stored.  If
 it does, those strings are freed.

 Revision 1.18  2002/08/14 16:34:47  soliday
 Added Open License

 Revision 1.17  2002/04/25 17:42:37  shang
 fixed a bug in SDDS_SetArray() ehich returned error when the array->data
 is null

 Revision 1.16  2002/01/15 22:50:45  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.15  2001/11/30 15:34:36  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.14  2000/03/30 02:33:00  borland
 Added some debugging statements.

 Revision 1.13  2000/03/28 15:48:03  borland
 Improved error message for SDDS_SetRowValues.

 Revision 1.12  2000/03/27 20:24:22  borland
 Added SDDS_ShortenTable().

 Revision 1.11  1999/05/25 18:56:20  soliday
 Removed compiler warning on linux.

 Revision 1.10  1998/06/24 19:19:23  borland
 SDDS_StartPage now frees the individual strings stored for string
 columns.  Lack of this resulted in memory leaks.  Some SDDS programs
 may have relied on this "feature" to avoid having to set the values
 of a string column again for every page.

 Revision 1.9  1997/12/19 16:58:34  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.8  1997/09/12 01:32:29  borland
 Added check for valid array pointer to SDDS_ClearPage.

 Revision 1.7  1997/09/09 01:49:07  borland
 Added checks for NULL pointers in SDDS_ClearPage.

 Revision 1.6  1996/08/07 14:51:45  borland
 Modified SDDS_SetParametersFromDoubles to give more useful error messages
 for out-of-range indices.

 * Revision 1.5  1996/03/12  04:00:40  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.4  1996/01/11  20:33:58  borland
 * Added SDDS_ClearPage to zero out data in page; avoids hold-over data from
 * previous use of the page.
 *
 * Revision 1.3  1995/11/13  16:02:48  borland
 * Added support for partially flushed tabular data to SDDS_SetRowValues().
 *
 * Revision 1.2  1995/09/05  21:10:52  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"

#undef DEBUG

int32_t SDDS_AllocateColumnFlags(SDDS_DATASET *SDDS_target)
{
  if (SDDS_target->layout.n_columns &&
      ((!(SDDS_target->column_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_target->layout.n_columns)) ||
       !(SDDS_target->column_order=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_target->layout.n_columns))) ||
      (!SDDS_SetMemory(SDDS_target->column_flag, SDDS_target->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
       !SDDS_SetMemory(SDDS_target->column_order, SDDS_target->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1)))) {
    SDDS_SetError("Unable to allocate column flags--memory allocation failure (SDDS_AllocateColumnFlags)");
    return 0;
  }
  return 1;
}

int32_t SDDS_StartPage(SDDS_DATASET *SDDS_dataset, int32_t expected_n_rows)
{
  SDDS_LAYOUT *layout;
  int32_t i, size;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_StartPage"))
    return(0);
  if ((SDDS_dataset->writing_page) && (SDDS_dataset->layout.data_mode.fixed_row_count)) {
    if (!SDDS_UpdateRowCount(SDDS_dataset))
      return(0);
  }
  if (!SDDS_RestoreLayout(SDDS_dataset)) {
    SDDS_SetError("Unable to start page--couldn't restore layout (SDDS_StartPage)");
    return(0);
  }
  if (expected_n_rows<=0)
    expected_n_rows = 1;
  SDDS_dataset->n_rows_written = 0;
  SDDS_dataset->last_row_written = -1;
  SDDS_dataset->writing_page = 0;
  SDDS_dataset->first_row_in_mem = 0;
  layout = &SDDS_dataset->layout;
  if (SDDS_dataset->page_started==0) {
    if (layout->n_parameters) {
      if (!(SDDS_dataset->parameter=(void**)calloc(sizeof(*SDDS_dataset->parameter), layout->n_parameters))) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
      for (i=0; i<layout->n_parameters; i++) {
        if (!(SDDS_dataset->parameter[i] = (void*)calloc(SDDS_type_size[layout->parameter_definition[i].type-1], 1))) {
          SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
          return(0);
        }
      }
    }
    if (layout->n_arrays) {
      if (!(SDDS_dataset->array=(SDDS_ARRAY*)calloc(sizeof(*SDDS_dataset->array), layout->n_arrays))) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
    }
    if (layout->n_columns) {
      if (!(SDDS_dataset->data=(void**)calloc(sizeof(*SDDS_dataset->data), layout->n_columns))) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
      SDDS_dataset->row_flag = NULL;
      if (expected_n_rows) {
        if (!(SDDS_dataset->row_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*expected_n_rows))) {
          SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
          return(0);
        }
        for (i=0; i<layout->n_columns; i++) {
          if (!(SDDS_dataset->data[i] = 
                (void*)calloc(expected_n_rows, SDDS_type_size[layout->column_definition[i].type-1]))) {
            SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
            return(0);
          }
        }
      }
      SDDS_dataset->n_rows_allocated = expected_n_rows;
      if (!(SDDS_dataset->column_flag=(int32_t*)SDDS_Realloc(SDDS_dataset->column_flag, sizeof(int32_t)*layout->n_columns)) ||
          !(SDDS_dataset->column_order=(int32_t*)SDDS_Realloc(SDDS_dataset->column_order, sizeof(int32_t)*layout->n_columns)) ) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
    }
  }
  else if (SDDS_dataset->n_rows_allocated>=expected_n_rows && layout->n_columns) {
    for (i=0; i<layout->n_columns; i++) {
      if (SDDS_dataset->data[i] && layout->column_definition[i].type==SDDS_STRING)
        SDDS_FreeStringArray(SDDS_dataset->data[i], SDDS_dataset->n_rows_allocated);
    }
  }
  else if (SDDS_dataset->n_rows_allocated<expected_n_rows && layout->n_columns) {
    if (!SDDS_dataset->data) {
      if (!(SDDS_dataset->column_flag=(int32_t*)SDDS_Realloc(SDDS_dataset->column_flag, sizeof(int32_t)*layout->n_columns)) ||
          !(SDDS_dataset->column_order=(int32_t*)SDDS_Realloc(SDDS_dataset->column_order, sizeof(int32_t)*layout->n_columns)) ||
          !(SDDS_dataset->data = (void**)calloc(layout->n_columns, sizeof(*SDDS_dataset->data)))) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
    }
    for (i=0; i<layout->n_columns; i++) {
      size = SDDS_type_size[layout->column_definition[i].type-1];
      if (SDDS_dataset->data[i] && layout->column_definition[i].type==SDDS_STRING) 
        SDDS_FreeStringArray(SDDS_dataset->data[i], SDDS_dataset->n_rows_allocated);
      if (!(SDDS_dataset->data[i] = 
            (void*)SDDS_Realloc(SDDS_dataset->data[i], expected_n_rows*size))) {
        SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
        return(0);
      }
      SDDS_ZeroMemory((char*)SDDS_dataset->data[i] + size*SDDS_dataset->n_rows_allocated,
                      size*(expected_n_rows-SDDS_dataset->n_rows_allocated));
    }
    if (!(SDDS_dataset->row_flag=(int32_t*)SDDS_Realloc(SDDS_dataset->row_flag, sizeof(int32_t)*expected_n_rows))) {
      SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_StartPage)");
      return(0);
    }
    SDDS_dataset->n_rows_allocated = expected_n_rows;
  }
  if (SDDS_dataset->n_rows_allocated && layout->n_columns &&
      !SDDS_SetMemory(SDDS_dataset->row_flag, SDDS_dataset->n_rows_allocated, SDDS_LONG, (int32_t)1, (int32_t)0)) {
    SDDS_SetError("Unable to start page--memory initialization failure (SDDS_StartPage)");
    return(0);
  }
  if (layout->n_columns && 
      (!SDDS_SetMemory(SDDS_dataset->column_flag, layout->n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
       !SDDS_SetMemory(SDDS_dataset->column_order, layout->n_columns, SDDS_LONG, (int32_t)0, (int32_t)1)) ) {
    SDDS_SetError("Unable to start page--memory initialization failure (SDDS_StartPage)");
    return(0);
  }
  SDDS_dataset->n_of_interest = layout->n_columns;
  SDDS_dataset->page_number ++;
  SDDS_dataset->page_started=1;
  SDDS_dataset->n_rows = 0;
  return(1);
}

int32_t SDDS_ClearPage(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout;
  int32_t i, size;
  

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ClearPage"))
    return 0;
  layout = &SDDS_dataset->layout;
  
  if (layout->n_columns && 
      ((SDDS_dataset->column_flag && 
        !SDDS_SetMemory(SDDS_dataset->column_flag, layout->n_columns, SDDS_LONG, (int32_t)1, (int32_t)0)) ||
       ((SDDS_dataset->column_order &&
         !SDDS_SetMemory(SDDS_dataset->column_order, layout->n_columns, SDDS_LONG, (int32_t)0, (int32_t)1))))) {
    SDDS_SetError("Unable to start page--memory initialization failure (SDDS_ClearPage)");
    return 0;
  }
  SDDS_FreeStringData(SDDS_dataset);
  if (SDDS_dataset->data) {
    for (i=0; i<layout->n_columns; i++) {
      size = SDDS_type_size[layout->column_definition[i].type-1];
      if (SDDS_dataset->data[i])
        SDDS_ZeroMemory(SDDS_dataset->data[i], size*SDDS_dataset->n_rows_allocated);
    }
  }
  if (SDDS_dataset->parameter) {
    for (i=0; i<layout->n_parameters; i++) {
      size = SDDS_type_size[layout->parameter_definition[i].type-1];
      SDDS_ZeroMemory(SDDS_dataset->parameter[i], size);
    }
  }
  for (i=0; i<layout->n_arrays; i++) {
    size = SDDS_type_size[layout->array_definition[i].type-1];
    if (SDDS_dataset->array && 
        SDDS_dataset->array[i].data && SDDS_dataset->array[i].elements)
      SDDS_ZeroMemory(SDDS_dataset->array[i].data, size*SDDS_dataset->array[i].elements);
  }
  return 1;
}

int32_t SDDS_ShortenTable(SDDS_DATASET *SDDS_dataset, int32_t rows)
{
  SDDS_LAYOUT *layout;
  int32_t i, size;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ShortenTable"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!SDDS_dataset->data && 
      !(SDDS_dataset->data = (void**)calloc(layout->n_columns, sizeof(*SDDS_dataset->data)))) {
    SDDS_SetError("Unable to start  page--memory allocation failure (SDDS_ShortenTable)");
    return(0);
  }
  if (rows<=0)
    rows = 1;
  for (i=0; i<layout->n_columns; i++) {
    size = SDDS_type_size[layout->column_definition[i].type-1];
    if (SDDS_dataset->data[i])
      free(SDDS_dataset->data[i]);
    if (!(SDDS_dataset->data[i] = (void*)calloc(rows, size))) {
      SDDS_SetError("Unable to shorten page--memory allocation failure (SDDS_ShortenTable)");
      return(0);
    }
  }
  if (SDDS_dataset->row_flag)
    free(SDDS_dataset->row_flag);
  if (!(SDDS_dataset->row_flag = (int32_t*)malloc(rows*sizeof(int32_t)))) {
    SDDS_SetError("Unable to shorten page--memory allocation failure (SDDS_ShortenTable)");
    return(0);
  }
  SDDS_dataset->n_rows_allocated = rows;

  if (!SDDS_SetMemory(SDDS_dataset->row_flag, SDDS_dataset->n_rows_allocated, SDDS_LONG, (int32_t)1, (int32_t)0) ||
      !SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
      !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1) ) {
    SDDS_SetError("Unable to shorten page--memory initialization failure (SDDS_ShortenTable)");
    return(0);
  }
  return(1);
  
}

int32_t SDDS_LengthenTable(SDDS_DATASET *SDDS_dataset, int32_t n_additional_rows)
{
  SDDS_LAYOUT *layout;
  int32_t i, size;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_LengthenTable"))
    return(0);
  layout = &SDDS_dataset->layout;
#if defined(DEBUG)
  fprintf(stderr, "table size being increased from %" PRId32 " to %" PRId32 " rows\n",
          SDDS_dataset->n_rows_allocated, 
          SDDS_dataset->n_rows_allocated+n_additional_rows);
#endif
  if (!SDDS_dataset->data && !(SDDS_dataset->data = (void**)calloc(layout->n_columns, sizeof(*SDDS_dataset->data)))) {
    SDDS_SetError("Unable to start page--memory allocation failure1 (SDDS_LengthenTable)");
    return(0);
  }
  if (n_additional_rows<0)
    n_additional_rows = 0;
  for (i=0; i<layout->n_columns; i++) {
    size = SDDS_type_size[layout->column_definition[i].type-1];
    if (!(SDDS_dataset->data[i] = 
          (void*)SDDS_Realloc(SDDS_dataset->data[i], (SDDS_dataset->n_rows_allocated+n_additional_rows)*size))) {
      SDDS_SetError("Unable to lengthen page--memory allocation failure2 (SDDS_LengthenTable)");
      return(0);
    }
    SDDS_ZeroMemory((char*)SDDS_dataset->data[i] + size*SDDS_dataset->n_rows_allocated, size*n_additional_rows);
  }
  if (!(SDDS_dataset->row_flag=
        (int32_t*)SDDS_Realloc(SDDS_dataset->row_flag, (SDDS_dataset->n_rows_allocated+n_additional_rows)*sizeof(int32_t)))) {
    SDDS_SetError("Unable to lengthen page--memory allocation failure3 (SDDS_LengthenTable)");
    return(0);
  }
  SDDS_dataset->n_rows_allocated += n_additional_rows;

  if (!SDDS_SetMemory(SDDS_dataset->row_flag, SDDS_dataset->n_rows_allocated, SDDS_LONG, (int32_t)1, (int32_t)0) ||
      !SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
      !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1) ) {
    SDDS_SetError("Unable to lengthen page--memory initialization failure4 (SDDS_LengthenTable)");
    return(0);
  }
  return(1);
}

int32_t SDDS_SetParameters(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
{
  va_list argptr;
  int32_t index, retval;
  SDDS_LAYOUT *layout;
  char *name;
  static char s[SDDS_MAXLINE];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetParameters"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME) ||
      !(mode&SDDS_PASS_BY_VALUE || mode&SDDS_PASS_BY_REFERENCE)) {
    SDDS_SetError("Unable to set parameter values--unknown mode (SDDS_SetParameters)");
    return(0);
  }

  va_start(argptr, mode);
  layout = &SDDS_dataset->layout;

  /* variable arguments are pairs of (index, value), where index is a int32_t integer */
  retval = -1;
  do {
    if (mode&SDDS_SET_BY_INDEX) {
      if ((index=va_arg(argptr, int32_t))==-1) {
        retval = 1;
        break;
      }
      if (index<0 || index>=layout->n_parameters) {
        SDDS_SetError("Unable to set parameter values--index out of range (SDDS_SetParameters)");
        retval = 0;
        break;
      }
    }
    else {
      if ((name=va_arg(argptr, char *))==NULL) {
        retval = 1;
        break;
      }
      if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))<0) {
        sprintf(s, "Unable to set parameter values--name %s not recognized (SDDS_SetParameters)", name);
        SDDS_SetError(s);
        retval = 0;
        break;
      }
    }
    switch (layout->parameter_definition[index].type) {
    case SDDS_SHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *((short*)SDDS_dataset->parameter[index]) = (short)va_arg(argptr, int);
      else
        *((short*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, short *));
      break;
    case SDDS_USHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *((unsigned short*)SDDS_dataset->parameter[index]) = (unsigned short)va_arg(argptr, unsigned int);
      else
        *((unsigned short*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, unsigned short *));
      break;
    case SDDS_LONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *((int32_t*)SDDS_dataset->parameter[index]) = (int32_t)va_arg(argptr, int32_t);
      else
        *((int32_t*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, int32_t *));
      break;
    case SDDS_ULONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *((uint32_t*)SDDS_dataset->parameter[index]) = (uint32_t)va_arg(argptr, uint32_t);
      else
        *((uint32_t*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, uint32_t *));
      break;
    case SDDS_FLOAT:
      if (mode&SDDS_PASS_BY_VALUE)
        *((float*)SDDS_dataset->parameter[index]) = (float)va_arg(argptr, double);
      else
        *((float*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, float *));
      break;
    case SDDS_DOUBLE:
      if (mode&SDDS_PASS_BY_VALUE)
        *((double*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((double*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_STRING:
      if (*(char**)SDDS_dataset->parameter[index])
        free(*(char**)SDDS_dataset->parameter[index]);
      if (mode&SDDS_PASS_BY_VALUE) {
        if (!SDDS_CopyString((char**)SDDS_dataset->parameter[index],  va_arg(argptr, char *))) {
          SDDS_SetError("Unable to set string parameter value--allocation failure (SDDS_SetParameters)");
          retval = 0;
        }
      }
      else {
        if (!SDDS_CopyString((char**)SDDS_dataset->parameter[index],  *(va_arg(argptr, char **)))) {
          SDDS_SetError("Unable to set string parameter value--allocation failure (SDDS_SetParameters)");
          retval = 0;
        }
      }
      break;
    case SDDS_CHARACTER: 
      if (mode&SDDS_PASS_BY_VALUE)
        *((char*)SDDS_dataset->parameter[index]) = (char)va_arg(argptr, int);
      else
        *((char*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, char *));
      break;
    default:
      SDDS_SetError("Unknown data type encountered (SDDS_SetParameters)");
      retval = 0;
    }
  } while (retval==-1);
  va_end(argptr);
  return(retval);
}

int32_t SDDS_SetParametersFromDoubles(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
{
  va_list argptr;
  int32_t index, retval;
  SDDS_LAYOUT *layout;
  char *name;
  static char s[SDDS_MAXLINE];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetParametersFromDoubles"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME) ||
      !(mode&SDDS_PASS_BY_VALUE || mode&SDDS_PASS_BY_REFERENCE)) {
    SDDS_SetError("Unable to set parameter values--unknown mode (SDDS_SetParametersFromDoubles)");
    return(0);
  }

  va_start(argptr, mode);
  layout = &SDDS_dataset->layout;

  /* variable arguments are pairs of (index, value), where index is a int32_t integer */
  retval = -1;
  do {
    if (mode&SDDS_SET_BY_INDEX) {
      if ((index=va_arg(argptr, int32_t))==-1) {
        retval = 1;
        break;
      }
      if (index<0 || index>=layout->n_parameters) {
        sprintf(s, "Unable to set parameter values--index %" PRId32 " out of range [%d, %" PRId32 "] (SDDS_SetParametersFromDoubles)", 
                index, 0, layout->n_parameters);
        SDDS_SetError(s);
        retval = 0;
        break;
      }
    }
    else {
      if ((name=va_arg(argptr, char *))==NULL) {
        retval = 1;
        break;
      }
      if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))<0) {
        sprintf(s, "Unable to set parameter values--name %s not recognized (SDDS_SetParametersFromDoubles)", name);
        SDDS_SetError(s);
        retval = 0;
        break;
      }
    }
    switch (layout->parameter_definition[index].type) {
    case SDDS_SHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *((short*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((short*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_USHORT: 
      if (mode&SDDS_PASS_BY_VALUE)
        *((unsigned short*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((unsigned short*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_LONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *((int32_t*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((int32_t*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_ULONG:
      if (mode&SDDS_PASS_BY_VALUE)
        *((uint32_t*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((uint32_t*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_FLOAT:
      if (mode&SDDS_PASS_BY_VALUE)
        *((float*)SDDS_dataset->parameter[index]) = (float)va_arg(argptr, double);
      else
        *((float*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_DOUBLE:
      if (mode&SDDS_PASS_BY_VALUE)
        *((double*)SDDS_dataset->parameter[index]) = va_arg(argptr, double);
      else
        *((double*)SDDS_dataset->parameter[index]) = *(va_arg(argptr, double *));
      break;
    case SDDS_STRING:
    case SDDS_CHARACTER: 
      SDDS_SetError("Nonnumeric data type encountered (SDDS_SetParametersFromDoubles)");
      retval = 0;
      break;
    default:
      SDDS_SetError("Unknown data type encountered (SDDS_SetParametersFromDoubles)");
      retval = 0;
    }
  } while (retval==-1);
  va_end(argptr);
  return(retval);
}

int32_t SDDS_SetRowValues(SDDS_DATASET *SDDS_dataset, int32_t mode, int32_t row, ...)
{
  va_list argptr;
  int32_t index, retval;
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
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetRowValues"))
    return(0);
  row -= SDDS_dataset->first_row_in_mem;
  if (row>=SDDS_dataset->n_rows_allocated) {
    sprintf(buffer, "Unable to set column values--row number (%" PRId32 ") exceeds exceeds allocated memory (%" PRId32 ") (SDDS_SetRowValues)", row, SDDS_dataset->n_rows_allocated);
    SDDS_SetError(buffer);
    return(0);
  }
  if (row>SDDS_dataset->n_rows-1)
    SDDS_dataset->n_rows = row+1;
  
  va_start(argptr, row);
  layout = &SDDS_dataset->layout;

  /* variable arguments are pairs of (index, value), where index is a int32_t integer */
  retval = -1;
#ifdef DEBUG
  fprintf(stderr, "setting row %" PRId32 " (mem slot %" PRId32 ")\n",
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
      fprintf(stderr, "Setting values for column #%" PRId32 "\n", index);
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
        *(((float*)SDDS_dataset->data[index])+row) = *(va_arg(argptr, float *));
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

int32_t SDDS_SetArrayVararg(SDDS_DATASET *SDDS_dataset, char *array_name, int32_t mode, void *data_pointer, ...)
{
  va_list argptr;
  int32_t index, retval, i, size;
  static int32_t *counter;
  SDDS_LAYOUT *layout;
  SDDS_ARRAY *array;
  void *ptr;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetArrayVararg"))
    return(0);
  if (!(mode&SDDS_POINTER_ARRAY) && !(mode&SDDS_CONTIGUOUS_DATA)) {
    SDDS_SetError("Unable to set array--invalid mode (SDDS_SetArrayVararg)");
    return(0);
  }
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
    SDDS_SetError("Unable to set array--unknown array name given (SDDS_SetArrayVararg)");
    return(0);
  }
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to set array--internal array pointer is NULL (SDDS_SetArrayVararg)");
    return(0);
  }

  layout = &SDDS_dataset->layout;
  array = SDDS_dataset->array+index;
  if (!layout->array_definition) {
    SDDS_SetError("Unable to set array--internal array definition pointer is NULL (SDDS_SetArrayVararg)");
    return(0);
  }
  array->definition = layout->array_definition+index;
  if (!array->dimension && !(array->dimension = (int32_t*)SDDS_Malloc(sizeof(*array->dimension)*array->definition->dimensions))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArrayVararg)");
    return(0);
  }

  va_start(argptr, data_pointer);

  /* variable arguments are dimensions */
  retval = 1;
  index = 0;
  array->elements = 1;
  do {
    if ((array->dimension[index] = va_arg(argptr, int32_t))<0) {
      SDDS_SetError("Unable to set array--negative dimension given (SDDS_SetArrayVararg)");
      retval = 0;
      break;
    }
    array->elements *= array->dimension[index];
  } while (retval==1 && ++index<array->definition->dimensions);
  va_end(argptr);

  if (!retval)
    return(0);
  if (!array->elements) 
    return(1);
  if (!data_pointer) {
    SDDS_SetError("Unable to set array--data pointer is NULL (SDDS_SetArrayVararg)");
    return(0);
  }

  size = SDDS_type_size[array->definition->type-1];
  if (!(array->data = SDDS_Realloc(array->data, size*array->elements))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArrayVararg");
    return(0);
  }

  /* handle 1-d arrays and contiguous data as a special case */
  if (array->definition->dimensions==1 || mode&SDDS_CONTIGUOUS_DATA) {
    if (array->definition->type!=SDDS_STRING)
      memcpy(array->data, data_pointer, size*array->elements);
    else if (!SDDS_CopyStringArray(array->data, data_pointer, array->elements)) {
      SDDS_SetError("Unable to set array--string copy failure (SDDS_SetArrayVararg");
      return(0);
    }
    return(1);
  }

  if (!(counter = SDDS_Realloc(counter, sizeof(*counter)*(array->elements-1)))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArrayVararg");
    return(0);
  }
  SDDS_ZeroMemory(counter, sizeof(*counter)*(array->elements-1));
  index = 0;
  do {
    ptr = data_pointer;
    for (i=0; i<array->definition->dimensions-1; i++)
      ptr = ((void**)ptr)[counter[i]];
    if (array->definition->type!=SDDS_STRING) 
      memcpy((char*)array->data+size*index, ptr, size*array->dimension[i]);
    else if (!SDDS_CopyStringArray( ((char**)array->data)+index, ptr, array->dimension[i]) ) {
      SDDS_SetError("Unable to set array--string copy failure (SDDS_SetArrayVararg");
      return(0);
    }
    index += array->dimension[i];
  } while (SDDS_AdvanceCounter(counter, array->dimension, array->definition->dimensions-1)!=-1);
  return(1);
}

int32_t SDDS_SetArray(SDDS_DATASET *SDDS_dataset, char *array_name, int32_t mode, void *data_pointer, int32_t *dimension)
{
  int32_t index, i, size;
  static int32_t *counter;
  SDDS_LAYOUT *layout;
  SDDS_ARRAY *array;
  void *ptr;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetArray"))
    return(0);
  if (!(mode&SDDS_POINTER_ARRAY) && !(mode&SDDS_CONTIGUOUS_DATA)) {
    SDDS_SetError("Unable to set array--invalid mode (SDDS_SetArray)");
    return(0);
  }
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
    SDDS_SetError("Unable to set array--unknown array name given (SDDS_SetArray)");
    return(0);
  }
  
  if (!dimension) {
    SDDS_SetError("Unable to set array--dimension pointer is NULL (SDDS_SetArray)");
    return(0);
  }
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to set array--internal array pointer is NULL (SDDS_SetArray)");
    return(0);
  }

  layout = &SDDS_dataset->layout;
  array = SDDS_dataset->array+index;
  if (!layout->array_definition) {
    SDDS_SetError("Unable to set array--internal array definition pointer is NULL (SDDS_SetArray)");
    return(0);
  }
  array->definition = layout->array_definition+index;
  if (!array->dimension && !(array->dimension = (int32_t*)SDDS_Malloc(sizeof(*array->dimension)*array->definition->dimensions))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArray)");
    return(0);
  }
  array->elements = 1;
  for (i=0; i<array->definition->dimensions; i++) {
    if ((array->dimension[i] = dimension[i])<0) {
      SDDS_SetError("Unable to set array--negative dimension specified (SDDS_SetArray)");
      return(0);
    }
    array->elements *= dimension[i];
    if (array->elements && !data_pointer) {
      SDDS_SetError("Unable to set array--data pointer is NULL (SDDS_SetArray)");
      return(0);
    }
  }
  if (!array->elements) 
    return(1);

  size = SDDS_type_size[array->definition->type-1];
  if (!(array->data = SDDS_Realloc(array->data, size*array->elements))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArray)");
    return(0);
  }

  /* handle 1-d arrays and contiguous data as a special case */
  if (array->definition->dimensions==1 || mode&SDDS_CONTIGUOUS_DATA) {
    if (array->definition->type!=SDDS_STRING)
      memcpy(array->data, data_pointer, size*array->elements);
    else if (!SDDS_CopyStringArray(array->data, data_pointer, array->elements)) {
      SDDS_SetError("Unable to set array--string copy failure (SDDS_SetArrayVararg");
      return(0);
    }
    return(1);
  }


  if (!(counter = SDDS_Realloc(counter, sizeof(*counter)*(array->elements-1)))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArray)");
    return(0);
  }
  SDDS_ZeroMemory(counter, sizeof(*counter)*(array->elements-1));
  index = 0;
  do {
    ptr = data_pointer;
    for (i=0; i<array->definition->dimensions-1; i++)
      ptr = ((void**)ptr)[counter[i]];
    if (array->definition->type!=SDDS_STRING) 
      memcpy((char*)array->data+size*index, ptr, size*array->dimension[i]);
    else if (!SDDS_CopyStringArray( ((char**)array->data)+index, ptr, array->dimension[i]) ) {
      SDDS_SetError("Unable to set array--string copy failure (SDDS_SetArray)");
      return(0);
    }
    index += array->dimension[i];
  } while (SDDS_AdvanceCounter(counter, array->dimension, array->definition->dimensions-1)!=-1);
  return(1);
}

int32_t SDDS_AppendToArrayVararg(SDDS_DATASET *SDDS_dataset, char *array_name, int32_t mode, void *data_pointer, int32_t elements, ...)
{
  va_list argptr;
  int32_t index, retval, i, size, startIndex=0;
  static int32_t *counter;
  SDDS_LAYOUT *layout;
  SDDS_ARRAY *array;
  void *ptr;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_AppendToArrayVararg"))
    return(0);
  if (!(mode&SDDS_POINTER_ARRAY) && !(mode&SDDS_CONTIGUOUS_DATA)) {
    SDDS_SetError("Unable to set array--invalid mode (SDDS_AppendToArrayVararg)");
    return(0);
  }
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
    SDDS_SetError("Unable to set array--unknown array name given (SDDS_AppendToArrayVararg)");
    return(0);
  }
  if (!data_pointer) {
    SDDS_SetError("Unable to set array--data pointer is NULL (SDDS_AppendToArrayVararg)");
    return(0);
  }
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to set array--internal array pointer is NULL (SDDS_AppendToArrayVararg)");
    return(0);
  }

  layout = &SDDS_dataset->layout;
  array = SDDS_dataset->array+index;
  if (!layout->array_definition) {
    SDDS_SetError("Unable to set array--internal array definition pointer is NULL (SDDS_AppendToArrayVararg)");
    return(0);
  }
  array->definition = layout->array_definition+index;
  if (!array->dimension && !(array->dimension = (int32_t*)SDDS_Malloc(sizeof(*array->dimension)*array->definition->dimensions))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_SetArrayVararg)");
    return(0);
  }
  if (!(array->definition->dimensions==1 || mode&SDDS_CONTIGUOUS_DATA)) {
    SDDS_SetError("Unable to set array--append operation requires contiguous data (SDDS_AppendToArrayVararg)");
    return(0);    
  }

  va_start(argptr, data_pointer);

  /* variable arguments are dimensions */
  retval = 1;
  index = 0;
  array->elements = 1;
  do {
    if ((array->dimension[index] = va_arg(argptr, int32_t))<0) {
      SDDS_SetError("Unable to set array--negative dimension given (SDDS_AppendToArrayVararg)");
      retval = 0;
      break;
    }
    array->elements *= array->dimension[index];
  } while (retval==1 && ++index<array->definition->dimensions);
  va_end(argptr);

  if (!retval)
    return(0);
  if (!array->elements) 
    return(1);

  size = SDDS_type_size[array->definition->type-1];
  if (!(array->data = SDDS_Realloc(array->data, size*array->elements))) {
    SDDS_SetError("Unable to set array--allocation failure (SDDS_AppendToArrayVararg)");
    return(0);
  }

  startIndex = array->elements - elements;

  /* handle 1-d arrays and contiguous data as a special case */
  if (array->definition->dimensions==1 || mode&SDDS_CONTIGUOUS_DATA) {
    if (array->definition->type!=SDDS_STRING)
      memcpy((char*)array->data+size*startIndex, data_pointer, size*elements);
    else if (!SDDS_CopyStringArray(((char**)array->data)+startIndex, data_pointer, elements)) {
      SDDS_SetError("Unable to set array--string copy failure (SDDS_AppendToArrayVararg)");
      return(0);
    }
    return(1);
  }

  return(1);
}

int32_t SDDS_AdvanceCounter(int32_t *counter, int32_t *max_count, int32_t n_indices)
{
  int32_t i;

  for (i=n_indices-1; i>=0; i--)
    if (counter[i]!=(max_count[i]-1))
      break;
  if (i==-1)
    return(-1);

  for (i=n_indices-1; i>=0; i--) {
    if (counter[i]<(max_count[i]-1)) {
      counter[i]++;
      break;
    }
    else {
      counter[i] = 0;
    }
  }
  return(i);
}

int32_t SDDS_SetColumn(SDDS_DATASET *SDDS_dataset, int32_t mode, void *data, int32_t rows, ...)
{
  va_list argptr;
  int32_t index, retval;
  SDDS_LAYOUT *layout;
  char *name;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumn"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME)) {
    SDDS_SetError("Unable to set column values--unknown mode (SDDS_SetColumn)");
    return(0);
  }
  if (rows>SDDS_dataset->n_rows_allocated) {
    SDDS_SetError("Unable to set column values--number of rows exceeds allocated memory (SDDS_SetColumn)");
    return(0);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetColumn"))
    return(0);
  if (SDDS_dataset->n_rows!=0 && SDDS_dataset->n_rows!=rows) {
    SDDS_SetError("Number of rows in new column unequal to number in other columns (SDDS_SetColumn)");
    return(0);
  }
  SDDS_dataset->n_rows = rows;
  layout = &SDDS_dataset->layout;
  
  retval = 1;
  va_start(argptr, rows);
  if (mode&SDDS_SET_BY_INDEX) {
    index = va_arg(argptr, int32_t);
    if (index<0 || index>=layout->n_columns) {
      SDDS_SetError("Unable to set column values--index out of range (SDDS_SetColumn)");
      retval = 0;
    }
  }
  else {
    name = va_arg(argptr, char *);
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0) {
      SDDS_SetError0("Unable to set column values--name ");
      SDDS_SetError0(name);
      SDDS_SetError(" not recognized (SDDS_SetColumn)");
      retval = 0;
    }
  }
  va_end(argptr);
  if (!retval)
    return 0;

  if (layout->column_definition[index].type==SDDS_STRING) {
    if (SDDS_dataset->data[index]) {
      char *ptr;
      int32_t i;
      for (i=0; i<rows; i++) {
        ptr = *((char**)SDDS_dataset->data[index]+i);
        if (ptr)
          free(ptr);
        *((char**)SDDS_dataset->data[index]+i) = NULL;
      }
    }
    if (!SDDS_CopyStringArray((char**)(SDDS_dataset->data[index]), (char**)data, rows)) {
      SDDS_SetError("Unable to set column--error copying string data (SDDS_SetColumn)");
      return 0;
    }
  }
  else 
    memcpy(SDDS_dataset->data[index], data, rows*SDDS_type_size[layout->column_definition[index].type-1]);
  return 1;
}

int32_t SDDS_SetColumnFromDoubles(SDDS_DATASET *SDDS_dataset, int32_t mode, double *data, int32_t rows, ...)
{
  va_list argptr;
  int32_t index, retval, type, size, i;
  SDDS_LAYOUT *layout;
  char *name;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumnFromDoubles"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME)) {
    SDDS_SetError("Unable to set column values--unknown mode (SDDS_SetColumnFromDoubles)");
    return(0);
  }
  if (rows>SDDS_dataset->n_rows_allocated) {
    SDDS_SetError("Unable to set column values--number of rows exceeds allocated memory (SDDS_SetColumnFromDoubles)");
    return(0);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetColumnFromDoubles"))
    return(0);
  if (SDDS_dataset->n_rows!=0 && SDDS_dataset->n_rows!=rows) {
    SDDS_SetError("Number of rows in new column unequal to number in other columns (SDDS_SetColumnFromDoubles)");
    return(0);
  }
  SDDS_dataset->n_rows = rows;
  layout = &SDDS_dataset->layout;
  
  retval = 1;
  va_start(argptr, rows);
  if (mode&SDDS_SET_BY_INDEX) {
    index = va_arg(argptr, int32_t);
    if (index<0 || index>=layout->n_columns) {
      SDDS_SetError("Unable to set column values--index out of range (SDDS_SetColumnFromDoubles)");
      retval = 0;
    }
  }
  else {
    name = va_arg(argptr, char *);
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0) {
      SDDS_SetError("Unable to set column values--name not recognized (SDDS_SetColumnFromDoubles)");
      retval = 0;
    }
  }
  va_end(argptr);
  if (!retval)
    return 0;

  type = layout->column_definition[index].type;
  if (!SDDS_NUMERIC_TYPE(type)) {
    SDDS_SetError("Unable to set column--source type is nonnumeric (SDDS_SetColumnFromDoubles)");
    return 0;
  }

  size = SDDS_type_size[layout->column_definition[index].type-1];

  if (type==SDDS_DOUBLE) {
    memcpy((char*)SDDS_dataset->data[index], (char*)data, rows*size);
    return 1;
  }

  for (i=0; i<rows; i++)
    if (!SDDS_CastValue(data, i, SDDS_DOUBLE, type, (char*)(SDDS_dataset->data[index])+i*size)) {
      SDDS_SetError("Unable to set column--cast error (SDDS_SetColumnFromDoubles)");
      return 0;
    }

  return 1;
}

int32_t SDDS_SetColumnFromFloats(SDDS_DATASET *SDDS_dataset, int32_t mode, float *data, int32_t rows, ...)
{
  va_list argptr;
  int32_t index, retval, type, size, i;
  SDDS_LAYOUT *layout;
  char *name;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumnFromFloats"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME)) {
    SDDS_SetError("Unable to set column values--unknown mode (SDDS_SetColumnFromFloats)");
    return(0);
  }
  if (rows>SDDS_dataset->n_rows_allocated) {
    SDDS_SetError("Unable to set column values--number of rows exceeds allocated memory (SDDS_SetColumnFromFloats)");
    return(0);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetColumnFromFloats"))
    return(0);
  if (SDDS_dataset->n_rows!=0 && SDDS_dataset->n_rows!=rows) {
    SDDS_SetError("Number of rows in new column unequal to number in other columns (SDDS_SetColumnFromFloats)");
    return(0);
  }
  SDDS_dataset->n_rows = rows;
  layout = &SDDS_dataset->layout;
  
  retval = 1;
  va_start(argptr, rows);
  if (mode&SDDS_SET_BY_INDEX) {
    index = va_arg(argptr, int32_t);
    if (index<0 || index>=layout->n_columns) {
      SDDS_SetError("Unable to set column values--index out of range (SDDS_SetColumnFromFloats)");
      retval = 0;
    }
  }
  else {
    name = va_arg(argptr, char *);
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0) {
      SDDS_SetError("Unable to set column values--name not recognized (SDDS_SetColumnFromFloats)");
      retval = 0;
    }
  }
  va_end(argptr);
  if (!retval)
    return 0;

  type = layout->column_definition[index].type;
  if (!SDDS_NUMERIC_TYPE(type)) {
    SDDS_SetError("Unable to set column--source type is nonnumeric (SDDS_SetColumnFromFloats)");
    return 0;
  }

  size = SDDS_type_size[layout->column_definition[index].type-1];

  if (type==SDDS_FLOAT) {
    memcpy((char*)SDDS_dataset->data[index], (char*)data, rows*size);
    return 1;
  }

  for (i=0; i<rows; i++)
    if (!SDDS_CastValue(data, i, SDDS_FLOAT, type, (char*)(SDDS_dataset->data[index])+i*size)) {
      SDDS_SetError("Unable to set column--cast error (SDDS_SetColumnFromDoubles)");
      return 0;
    }

  return 1;
}

int32_t SDDS_SetColumnFromLongs(SDDS_DATASET *SDDS_dataset, int32_t mode, int32_t *data, int32_t rows, ...)
{
  va_list argptr;
  int32_t index, retval, type, size, i;
  SDDS_LAYOUT *layout;
  char *name;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetColumnFromLongs"))
    return(0);
  if (!(mode&SDDS_SET_BY_INDEX || mode&SDDS_SET_BY_NAME)) {
    SDDS_SetError("Unable to set column values--unknown mode (SDDS_SetColumnFromLongs)");
    return(0);
  }
  if (rows>SDDS_dataset->n_rows_allocated) {
    SDDS_SetError("Unable to set column values--number of rows exceeds allocated memory (SDDS_SetColumnFromLongs)");
    return(0);
  }
  if (!SDDS_CheckTabularData(SDDS_dataset, "SDDS_SetColumnFromLongs"))
    return(0);
  if (SDDS_dataset->n_rows!=0 && SDDS_dataset->n_rows!=rows) {
    SDDS_SetError("Number of rows in new column unequal to number in other columns (SDDS_SetColumnFromLongs)");
    return(0);
  }
  SDDS_dataset->n_rows = rows;
  layout = &SDDS_dataset->layout;
  
  retval = 1;
  va_start(argptr, rows);
  if (mode&SDDS_SET_BY_INDEX) {
    index = va_arg(argptr, int32_t);
    if (index<0 || index>=layout->n_columns) {
      SDDS_SetError("Unable to set column values--index out of range (SDDS_SetColumnFromLongs)");
      retval = 0;
    }
  }
  else {
    name = va_arg(argptr, char *);
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0) {
      SDDS_SetError("Unable to set column values--name not recognized (SDDS_SetColumnFromLongs)");
      retval = 0;
    }
  }
  va_end(argptr);
  if (!retval)
    return 0;

  type = layout->column_definition[index].type;
  if (!SDDS_NUMERIC_TYPE(type)) {
    SDDS_SetError("Unable to set column--source type is nonnumeric (SDDS_SetColumnFromLongs)");
    return 0;
  }

  size = SDDS_type_size[layout->column_definition[index].type-1];

  if (type==SDDS_LONG) {
    memcpy((char*)SDDS_dataset->data[index], (char*)data, rows*size);
    return 1;
  }

  for (i=0; i<rows; i++)
    if (!SDDS_CastValue(data, i, SDDS_LONG, type, (char*)(SDDS_dataset->data[index])+i*size)) {
      SDDS_SetError("Unable to set column--cast error (SDDS_SetColumnFromLongs)");
      return 0;
    }

  return 1;
}
