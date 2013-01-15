/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_rpn.c
 * contents: internal routines for rpn definitions of columns and parameters
 *  
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.13  2006/08/31 15:08:02  soliday
 Updated to work with SDDS2

 Revision 1.12  2005/11/04 22:46:53  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.11  2005/01/13 16:47:59  shang
 added string support to rpn functions

 Revision 1.10  2003/09/11 22:31:02  soliday
 Added SDDS_ConvertToLong and SDDS_GetParameterAsLong

 Revision 1.9  2003/08/28 21:24:58  soliday
 Cleaned up the code.

 Revision 1.8  2002/08/14 16:34:49  soliday
 Added Open License

 Revision 1.7  2000/06/15 21:07:39  soliday
 Removed compiler warnings under Solaris.

 Revision 1.6  1999/05/25 18:58:33  soliday
 Removed compiler warning on linux.

 Revision 1.5  1996/03/12 04:00:54  borland
 Reformatted code to 2-space indentation in K&R style.

 * Revision 1.4  1996/02/12  17:08:35  borland
 * Optimized routines for better performance.  Eliminated unnecessary error
 * checking.  Added new type conversion routines with function pointer array.
 * Moved some operations outside of loops.
 *
 * Revision 1.3  1995/10/03  06:34:15  borland
 * Modified SDDS_CreateRpnArray to check for existence of array first, and
 * reuse it if it exists.
 *
 * Revision 1.2  1995/09/05  21:11:10  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"
#include "rpn.h"

double SDDS_ConvertDoubleToDouble(void *data, int32_t index)
{
  return *((double*)data+index);
}

double SDDS_ConvertFloatToDouble(void *data, int32_t index)
{
  return *((float*)data+index);
}

double SDDS_ConvertLongToDouble(void *data, int32_t index)
{
  return *((int32_t*)data+index);
}

double SDDS_ConvertULongToDouble(void *data, int32_t index)
{
  return *((uint32_t*)data+index);
}

double SDDS_ConvertShortToDouble(void *data, int32_t index)
{
  return *((short*)data+index);
}

double SDDS_ConvertUShortToDouble(void *data, int32_t index)
{
  return *((unsigned short*)data+index);
}

double SDDS_ConvertStringToDouble(void *data, int32_t index)
{
  return atof(*((char**)data+index));
}

double SDDS_ConvertCharToDouble(void *data, int32_t index)
{
  return *((char*)data+index);
}

double SDDS_ConvertToDouble(int32_t type, void *data, int32_t index)
{
  if (!data) {
    SDDS_SetError("NULL data pointer passed (SDDS_ConvertToDouble)");
    return(0.0);
  }
  switch (type) {
  case SDDS_SHORT:
    return((double) *((short*)data+index));
  case SDDS_USHORT:
    return((double) *((unsigned short*)data+index));
  case SDDS_LONG:
    return((double) *((int32_t*)data+index));
  case SDDS_ULONG:
    return((double) *((uint32_t*)data+index));
  case SDDS_FLOAT:
    return((double) *((float*)data+index));
  case SDDS_DOUBLE:
    return(*((double*)data+index));
  case SDDS_CHARACTER:
    return((double) *((unsigned char*)data+index));
  default:
    SDDS_SetError("Invalid data type seen (SDDS_ConvertToDouble)");
    return(0.0);
  }
}

int32_t SDDS_ConvertToLong(int32_t type, void *data, int32_t index)
{
  if (!data) {
    SDDS_SetError("NULL data pointer passed (SDDS_ConvertToLong)");
    return(0.0);
  }
  switch (type) {
  case SDDS_DOUBLE:
    return ((int32_t) *((double*)data+index));
  case SDDS_FLOAT:
    return ((int32_t) *((float*)data+index));
  case SDDS_SHORT:
    return((int32_t) *((short*)data+index));
  case SDDS_USHORT:
    return((int32_t) *((unsigned short*)data+index));
  case SDDS_LONG:
    return(*((int32_t*)data+index));
  case SDDS_ULONG:
    return((int32_t) *((uint32_t*)data+index));
  case SDDS_CHARACTER:
    return((int32_t) *((unsigned char*)data+index));
  default:
    SDDS_SetError("Invalid data type seen (SDDS_ConvertToLong)");
    return(0.0);
  }
}


#if defined(RPN_SUPPORT)

static double (*SDDS_ConvertTypeToDouble[SDDS_NUM_TYPES+1])(void *data, int32_t index) = {
  NULL, SDDS_ConvertDoubleToDouble, SDDS_ConvertFloatToDouble,
  SDDS_ConvertLongToDouble, SDDS_ConvertULongToDouble, 
  SDDS_ConvertShortToDouble, SDDS_ConvertUShortToDouble,
  SDDS_ConvertStringToDouble, SDDS_ConvertCharToDouble
  };

static int32_t table_number_mem=-1, i_page_mem=-1, n_rows_mem=-1, i_row_mem=-1;

int32_t SDDS_CreateRpnMemory(char *name, short is_string)
{
  if (!name)
    return(-1);
  return(rpn_create_mem(name, is_string));
}

int32_t SDDS_CreateRpnArray(char *name)
{
  int32_t memnum;
  double dummy;
  char *dummy1=NULL;
  short is_string=0;
  
  if (!name)
    return(-1);
  if ((memnum=is_memory(&dummy, &dummy1, &is_string, name))>=0)
    return memnum;
  if ((memnum = rpn_create_mem(name, is_string))>=0)
    rpn_store((double)rpn_createarray(1), NULL, memnum);
  return memnum;
}
int32_t SDDS_ComputeParameter(SDDS_DATASET *SDDS_dataset, int32_t parameter, char *equation)
{
  /*  int32_t i;*/
  SDDS_LAYOUT *layout;
  double value;
  /*  int32_t equations_present;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ComputeParameter"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (parameter<0 || parameter>=layout->n_parameters)
    return(0);

  if (!equation) {
    SDDS_SetError("Unable to compute defined parameter--no equation for named parameter (SDDS_ComputeParameter)");
    return(0);
  }

  if (!SDDS_StoreParametersInRpnMemories(SDDS_dataset))
    return(0);
  if (!SDDS_StoreColumnsInRpnArrays(SDDS_dataset))
    return 0;

  value = rpn(equation);
  rpn_store(value, NULL, layout->parameter_definition[parameter].memory_number);
  if (rpn_check_error()) {
    SDDS_SetError("Unable to compute rpn expression--rpn error (SDDS_ComputeParameter)");
    return(0);
  }
#if defined(DEBUG)
  fprintf(stderr, "computed parameter value %s with equation %s: %e\n", layout->parameter_definition[parameter].name, equation,
          value);
#endif
  switch (layout->parameter_definition[parameter].type) {
  case SDDS_CHARACTER:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (char)value, -1);
    break;
  case SDDS_SHORT:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (short)value, -1);
    break;
  case SDDS_USHORT:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (unsigned short)value, -1);
    break;
  case SDDS_LONG:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (int32_t)value, -1);
    break;
  case SDDS_ULONG:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (uint32_t)value, -1);
    break;
  case SDDS_FLOAT:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (float)value, -1);
    break;
  case SDDS_DOUBLE:
    SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, parameter, (double)value, -1);
    break;
  }

  return(1);
}

int32_t SDDS_ComputeColumn(SDDS_DATASET *SDDS_dataset, int32_t column, char *equation)
{
  int32_t j;
  /*  int32_t i;*/
  SDDS_LAYOUT *layout;
  double value;
  /*  int32_t equations_present;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ComputeColumn"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (column<0 || column>=layout->n_columns)
    return(0);

  if (!SDDS_StoreParametersInRpnMemories(SDDS_dataset))
    return(0);
  if (!SDDS_StoreColumnsInRpnArrays(SDDS_dataset))
    return 0;

  if (table_number_mem==-1) {
    table_number_mem = rpn_create_mem("table_number", 0);
    i_page_mem = rpn_create_mem("i_page", 0);
    n_rows_mem = rpn_create_mem("n_rows", 0);
    i_row_mem = rpn_create_mem("i_row", 0);
  }

  rpn_store((double)SDDS_dataset->page_number, NULL, table_number_mem);
  rpn_store((double)SDDS_dataset->page_number, NULL, i_page_mem);
  rpn_store((double)SDDS_dataset->n_rows, NULL, n_rows_mem);
#if defined(DEBUG)
  fprintf(stderr, "computing %s using equation %s\n", layout->column_definition[column].name, equation);
#endif

  for (j=0; j<SDDS_dataset->n_rows; j++) {
    rpn_clear();
    if (!SDDS_StoreRowInRpnMemories(SDDS_dataset, j))
      return(0);
    rpn_store((double)j, NULL, i_row_mem);
    value = rpn(equation);
    rpn_store(value, NULL, layout->column_definition[column].memory_number);
    if (rpn_check_error()) {
      SDDS_SetError("Unable to compute rpn expression--rpn error (SDDS_ComputeDefinedColumn)");
      return(0);
    }
#if defined(DEBUG)
    fprintf(stderr, "computed row value: %s = %e\n", layout->column_definition[column].name, value);
#endif
    switch (layout->column_definition[column].type) {
    case SDDS_CHARACTER:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (char)value, -1);
      break;
    case SDDS_SHORT:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (short)value, -1);
      break;
    case SDDS_USHORT:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (unsigned short)value, -1);
      break;
    case SDDS_LONG:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (int32_t)value, -1);
      break;
    case SDDS_ULONG:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (uint32_t)value, -1);
      break;
    case SDDS_FLOAT:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (float)value, -1);
      break;
    case SDDS_DOUBLE:
      SDDS_SetRowValues(SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, column, (double)value, -1);
      break;
    }
  }

  return(1);
}

int32_t SDDS_FilterRowsWithRpnTest(SDDS_DATASET *SDDS_dataset, char *rpn_test)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  int32_t accept, n_columns;
  static int32_t table_number_mem=-1, n_rows_mem=-1, i_page_mem=-1;
  COLUMN_DEFINITION *coldef;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ComputeRpnEquations"))
    return(0);
  layout = &SDDS_dataset->layout;
  
  if (table_number_mem==-1) {
    table_number_mem = rpn_create_mem("table_number", 0);
    i_page_mem = rpn_create_mem("page_number", 0);
    n_rows_mem = rpn_create_mem("n_rows", 0);
    i_row_mem = rpn_create_mem("i_row", 0);
  }

  rpn_store((double)SDDS_dataset->page_number, NULL, table_number_mem);
  rpn_store((double)SDDS_dataset->page_number, NULL, i_page_mem);
  rpn_store((double)SDDS_dataset->n_rows, NULL, n_rows_mem);

  for (i=0; i<layout->n_columns; i++) {
    if (layout->column_definition[i].memory_number<0) {
      SDDS_SetError("Unable to compute equations--column lacks rpn memory number (SDDS_FilterRowsWithRpnTest)");
      return(0);
    }
  }
  
  n_columns = layout->n_columns;  

  for (j=0; j<SDDS_dataset->n_rows; j++) {
    rpn_clear();
    rpn_store((double)j, NULL, i_row_mem);
    /* store values in memories */
    coldef = layout->column_definition;
    for (i=0; i<n_columns; i++, coldef++) {
      if (coldef->type!=SDDS_STRING) {
        rpn_quick_store
          ((*SDDS_ConvertTypeToDouble[coldef->type])(SDDS_dataset->data[i], j), NULL,
           coldef->memory_number);
      } else {
        rpn_quick_store(0, ((char**)SDDS_dataset->data[i])[j], coldef->memory_number);
      }
    }
    rpn(rpn_test);
    if (rpn_check_error()) {
      SDDS_SetError("Unable to compute rpn expression--rpn error (SDDS_FilterRowsWithRpnTest)");
      return(0);
    }
    if (!pop_log(&accept)) {
      SDDS_SetError("rpn column-based test expression problem");
      return(0);
    }
    if (!accept)
      SDDS_dataset->row_flag[j] = 0;
  }
  rpn_clear();
  return(1);
}

int32_t SDDS_StoreParametersInRpnMemories(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_StoreParametersInRpnMemories"))
    return(0);
  layout = &SDDS_dataset->layout;
  
  rpn_clear();
#if defined(DEBUG)
  fprintf(stderr, "storing parameters: ");
#endif
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].memory_number<0) {
        SDDS_SetError("Unable to compute equations--parameter lacks rpn memory number (SDDS_StoreParametersInRpnMemories");
        return(0);
      }
    if (layout->parameter_definition[i].type!=SDDS_STRING) {
      rpn_quick_store((*SDDS_ConvertTypeToDouble[layout->parameter_definition[i].type])
                      (SDDS_dataset->parameter[i], 0), NULL, 
                      layout->parameter_definition[i].memory_number);
#if defined(DEBUG)
      fprintf(stderr, "%s = %le ", layout->parameter_definition[i].name, rpn_recall(layout->parameter_definition[i].memory_number));
#endif
    } else {
      rpn_quick_store(0,((char**)SDDS_dataset->parameter[i])[0],
                      layout->parameter_definition[i].memory_number);
#if defined(DEBUG)
      fprintf(stderr, "%s = %s ", layout->parameter_definition[i].name, rpn_str_recall(layout->parameter_definition[i].memory_number));
#endif

    }
  }
  if (SDDS_NumberOfErrors())
    return(0);
  if (rpn_check_error()) {
    SDDS_SetError("Unable to compute rpn expression--rpn error (SDDS_StoreParametersInRpnMemories)");
    return(0);
  }
#if defined(DEBUG)
  fputc('\n', stderr);
#endif
  return(1);
}

int32_t SDDS_StoreRowInRpnMemories(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  int32_t i, columns;
  COLUMN_DEFINITION *coldef;
  
  columns = SDDS_dataset->layout.n_columns;
  if (row==0) {
    coldef = SDDS_dataset->layout.column_definition;
    for (i=0; i<columns; i++, coldef++) {
      if (coldef->memory_number<0) {
        SDDS_SetError("Unable to compute equations--column lacks rpn memory number (SDDS_StoreRowInRpnMemories)");
        return(0);
      }
    }
  }
  coldef = SDDS_dataset->layout.column_definition;
  for (i=0; i<columns; i++, coldef++) {
    if (coldef->type!=SDDS_STRING) {
      rpn_quick_store((*SDDS_ConvertTypeToDouble[coldef->type])
                      (SDDS_dataset->data[i], row), NULL,
                      coldef->memory_number);
    } else {
      rpn_quick_store(0, ((char**)SDDS_dataset->data[i])[row],
                      coldef->memory_number);
    }
  }
  return(1);
}

int32_t SDDS_StoreColumnsInRpnArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, arraysize;
  SDDS_LAYOUT *layout;
  double *arraydata;
  int32_t *longPtr;
  uint32_t *ulongPtr;
  short *shortPtr;
  unsigned short *ushortPtr;
  /*  double *doublePtr;*/
  float *floatPtr;
  char *charPtr;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_StoreColumnsRpnArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
  rpn_clear();
  for (i=0; i<layout->n_columns; i++) {
    if (layout->column_definition[i].type!=SDDS_STRING) {
      if (layout->column_definition[i].pointer_number<0) {
        SDDS_SetError("Unable to compute equations--column lacks rpn pointer number (SDDS_StoreColumnsInRpnArrays)");
        return(0);
      }
      if (!rpn_resizearray((int32_t)rpn_recall(layout->column_definition[i].pointer_number),
                           SDDS_dataset->n_rows)) {
        SDDS_SetError("Unable to compute equations--couldn't resize rpn arrays (SDDS_StoreColumnsInRpnArrays)");
        return 0;
      }
      if (!(arraydata=rpn_getarraypointer(layout->column_definition[i].pointer_number,
                                          &arraysize)) || arraysize!=SDDS_dataset->n_rows) {
        SDDS_SetError("Unable to compute equations--couldn't retrieve rpn arrays (SDDS_StoreColumnsInRpnArrays)");
        return 0;
      }
      switch (layout->column_definition[i].type) {
      case SDDS_DOUBLE:
        memcpy((char*)arraydata, (char*)(SDDS_dataset->data[i]), 
               SDDS_dataset->n_rows*sizeof(double));
#if 0
        doublePtr = (double*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = doublePtr[j];
#endif
        break;
      case SDDS_FLOAT:
        floatPtr = (float*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = floatPtr[j];
        break;
      case SDDS_LONG:
        longPtr = (int32_t*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = longPtr[j];
        break;
      case SDDS_ULONG:
        ulongPtr = (uint32_t*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = ulongPtr[j];
        break;
      case SDDS_SHORT:
        shortPtr = (short*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = shortPtr[j];
        break;
      case SDDS_USHORT:
        ushortPtr = (unsigned short*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = ushortPtr[j];
        break;
      case SDDS_CHARACTER:
        charPtr = (char*)(SDDS_dataset->data[i]);
        for (j=0; j<SDDS_dataset->n_rows; j++)
          arraydata[j] = charPtr[j];
        break;
      }
    }
  }    
  return 1;
}

#else    /* end of RPN_SUPPORT section */

int32_t SDDS_CreateRpnMemory(char *name, short is_string)
{
  return(1);
}

int32_t SDDS_CreateRpnArray(char *name)
{
  return(1);
}

#endif
