/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_info.c
 * contents: user's routines for getting information about definitions.
 * 
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.5  2002/08/14 16:34:48  soliday
 Added Open License

 Revision 1.4  1996/03/21 16:38:35  borland
 Major revision.
 Added buffered binary reading (SDDS_binary.c).
 Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 Added sorted column, parameter, and array name lists (SDDS_info.c,
 SDDS_output.c, SDDS_utils.c).

 * Revision 1.3  1996/03/12  04:00:45  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.2  1995/09/05  21:10:57  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"

int32_t SDDS_GetColumnInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, column_index;
  COLUMN_DEFINITION *columndef;
  char *column_name;
  va_list argptr;
  int32_t retval;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_GetColumnInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_GET_BY_INDEX) {
    if ((column_index = va_arg(argptr, int32_t))<0 || column_index>=SDDS_dataset->layout.n_columns) {
      SDDS_SetError("Invalid column index passed. (SDDS_GetColumnInformation)");
      retval = 0;
    }
  }
  else {
    if (!(column_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL column name passed. (SDDS_GetColumnInformation)");
      retval = 0;
    }
    if ((column_index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
      SDDS_SetError("Unknown column name given (SDDS_GetColumnInformation)");
      retval = 0;
    }
  }
  columndef = SDDS_dataset->layout.column_definition+column_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_COLUMN_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ColumnFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_COLUMN_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_GetColumnInformation)");
    return(0);
  }
  type = SDDS_ColumnFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString((char**)memory, *((char**)((char*)columndef+SDDS_ColumnFieldInformation[field_index].offset)))) {
      SDDS_SetError("Unable to copy field data (SDDS_GetColumnInformation)");
      return(0);
    }
  }
  else
    memcpy(memory, (char*)columndef+SDDS_ColumnFieldInformation[field_index].offset, SDDS_type_size[type-1]);
  return(type);
}

int32_t SDDS_GetParameterInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, parameter_index;
  PARAMETER_DEFINITION *parameterdef;
  char *parameter_name;
  va_list argptr;
  int32_t retval;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_GetParameterInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_GET_BY_INDEX) {
    if ((parameter_index = va_arg(argptr, int32_t))<0 || parameter_index>=SDDS_dataset->layout.n_parameters) {
      SDDS_SetError("Invalid parameter index passed. (SDDS_GetParameterInformation)");
      retval = 0;
    }
  }
  else {
    if (!(parameter_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL parameter name passed. (SDDS_GetParameterInformation)");
      retval = 0;
    }
    if ((parameter_index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
      SDDS_SetError("Unknown parameter name given (SDDS_GetParameterInformation)");
      retval = 0;
    }
  }
  parameterdef = SDDS_dataset->layout.parameter_definition+parameter_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_PARAMETER_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ParameterFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_PARAMETER_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_GetParameterInformation)");
    return(0);
  }
  type = SDDS_ParameterFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString((char**)memory, 
                         *((char**)((char*)parameterdef+SDDS_ParameterFieldInformation[field_index].offset)))) {
      SDDS_SetError("Unable to copy field data (SDDS_GetParameterInformation)");
      return(0);
    }
  }
  else
    memcpy(memory, (char*)parameterdef+SDDS_ParameterFieldInformation[field_index].offset, SDDS_type_size[type-1]);
  return(type);
}

int32_t SDDS_GetArrayInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, array_index;
  ARRAY_DEFINITION *arraydef;
  char *array_name;
  va_list argptr;
  int32_t retval;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_GetArrayInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_GET_BY_INDEX) {
    if ((array_index = va_arg(argptr, int32_t))<0 || array_index>=SDDS_dataset->layout.n_arrays) {
      SDDS_SetError("Invalid array index passed. (SDDS_GetArrayInformation)");
      retval = 0;
    }
  }
  else {
    if (!(array_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL array name passed. (SDDS_GetArrayInformation)");
      retval = 0;
    }
    if ((array_index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
      SDDS_SetError("Unknown array name given (SDDS_GetArrayInformation)");
      retval = 0;
    }
  }
  arraydef = SDDS_dataset->layout.array_definition+array_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_ARRAY_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ArrayFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_ARRAY_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_GetArrayInformation)");
    return(0);
  }
  type = SDDS_ArrayFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString((char**)memory, *((char**)((char*)arraydef+SDDS_ArrayFieldInformation[field_index].offset)))) {
      SDDS_SetError("Unable to copy field data (SDDS_GetArrayInformation)");
      return(0);
    }
  }
  else
    memcpy(memory, (char*)arraydef+SDDS_ArrayFieldInformation[field_index].offset, SDDS_type_size[type-1]);
  return(type);
}

int32_t SDDS_GetAssociateInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, associate_index;
  ASSOCIATE_DEFINITION *associatedef;
  char *associate_name;
  va_list argptr;
  int32_t retval;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetAssociateInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_GetAssociateInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_GET_BY_INDEX) {
    if ((associate_index = va_arg(argptr, int32_t))<0 || associate_index>=SDDS_dataset->layout.n_associates) {
      SDDS_SetError("Invalid associate index passed. (SDDS_GetAssociateInformation)");
      retval = 0;
    }
  }
  else {
    if (!(associate_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL associate name passed. (SDDS_GetAssociateInformation)");
      retval = 0;
    }
    if ((associate_index=SDDS_GetAssociateIndex(SDDS_dataset, associate_name))<0) {
      SDDS_SetError("Unknown associate name given (SDDS_GetAssociateInformation)");
      retval = 0;
    }
  }
  associatedef = SDDS_dataset->layout.associate_definition+associate_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_ASSOCIATE_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_AssociateFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_ASSOCIATE_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_GetAssociateInformation)");
    return(0);
  }
  type = SDDS_AssociateFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString((char**)memory, 
                         *((char**)((char*)associatedef+SDDS_AssociateFieldInformation[field_index].offset)))) {
      SDDS_SetError("Unable to copy field data (SDDS_GetAssociateInformation)");
      return(0);
    }
  }
  else
    memcpy(memory, (char*)associatedef+SDDS_AssociateFieldInformation[field_index].offset, SDDS_type_size[type-1]);
  return(type);
}

int32_t SDDS_ChangeColumnInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, column_index, givenType;
  COLUMN_DEFINITION *columndef;
  char *column_name;
  va_list argptr;
  int32_t retval, i;
  double buffer[4]; 

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ChangeColumnInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_ChangeColumnInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_SET_BY_INDEX) {
    if ((column_index = va_arg(argptr, int32_t))<0 || column_index>=SDDS_dataset->layout.n_columns) {
      SDDS_SetError("Invalid column index passed. (SDDS_ChangeColumnInformation)");
      retval = 0;
    }
  }
  else {
    if (!(column_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL column name passed. (SDDS_ChangeColumnInformation)");
      retval = 0;
    }
    if ((column_index=SDDS_GetColumnIndex(SDDS_dataset, column_name))<0) {
      SDDS_SetError("Unknown column name given (SDDS_ChangeColumnInformation)");
      retval = 0;
    }
  }
  columndef = SDDS_dataset->layout.column_definition+column_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_COLUMN_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ColumnFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_COLUMN_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_ChangeColumnInformation)");
    return(0);
  }
  type = SDDS_ColumnFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString(((char**)((char*)columndef+SDDS_ColumnFieldInformation[field_index].offset)),
                         (char*)memory)) {
      SDDS_SetError("Unable to copy field data (SDDS_ChangeColumnInformation)");
      return(0);
    }
    if (strcmp(field_name, "name")==0) {
      for (i=0; i<SDDS_dataset->layout.n_columns; i++)
        if (column_index==SDDS_dataset->layout.column_index[i]->index)
          break;
      if (i==SDDS_dataset->layout.n_columns) {
        SDDS_SetError("Unable to copy field data--column indexing problem (SDDS_ChangeColumnInformation)");
        return(0);
      }
      SDDS_dataset->layout.column_index[i]->name = SDDS_dataset->layout.column_definition[column_index].name;
      qsort((char*)SDDS_dataset->layout.column_index, SDDS_dataset->layout.n_columns,
            sizeof(*SDDS_dataset->layout.column_index), SDDS_CompareIndexedNamesPtr);
    }
  }
  else {
    if (mode&SDDS_PASS_BY_STRING) {
      if (strcmp(field_name, "type")==0 && (givenType=SDDS_IdentifyType((char*)memory))>0)
        /* the type has been passed as a string (e.g., "double") */
        memcpy((char*)buffer, (char*)&givenType, sizeof(givenType));
      else if (!SDDS_ScanData((char*)memory, type, 0, (void*)buffer, 0, 0)) {
        SDDS_SetError("Unable to scan string data (SDDS_ChangeColumnInformation)");
        return(0);
      }
      memcpy((char*)columndef+SDDS_ColumnFieldInformation[field_index].offset, (void*)buffer, SDDS_type_size[type-1]);
    }
    else
      memcpy((char*)columndef+SDDS_ColumnFieldInformation[field_index].offset, memory, SDDS_type_size[type-1]);
  }
  return(type);
}


int32_t SDDS_ChangeParameterInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, parameter_index, givenType;
  PARAMETER_DEFINITION *parameterdef;
  char *parameter_name;
  va_list argptr;
  int32_t retval;
  double buffer[4];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ChangeParameterInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_ChangeParameterInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_SET_BY_INDEX) {
    if ((parameter_index = va_arg(argptr, int32_t))<0 || parameter_index>=SDDS_dataset->layout.n_parameters) {
      SDDS_SetError("Invalid parameter index passed. (SDDS_ChangeParameterInformation)");
      retval = 0;
    }
  }
  else {
    if (!(parameter_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL parameter name passed. (SDDS_ChangeParameterInformation)");
      retval = 0;
    }
    if ((parameter_index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
      SDDS_SetError("Unknown parameter name given (SDDS_ChangeParameterInformation)");
      retval = 0;
    }
  }
  parameterdef = SDDS_dataset->layout.parameter_definition+parameter_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_PARAMETER_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ParameterFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_PARAMETER_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_ChangeParameterInformation)");
    return(0);
  }
  type = SDDS_ParameterFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString(((char**)((char*)parameterdef+SDDS_ParameterFieldInformation[field_index].offset)),
                         (char*)memory)) {
      SDDS_SetError("Unable to copy field data (SDDS_ChangeParameterInformation)");
      return(0);
    }
    if (strcmp(field_name, "name")==0)
      qsort((char*)SDDS_dataset->layout.parameter_index, SDDS_dataset->layout.n_parameters,
            sizeof(*SDDS_dataset->layout.parameter_index), SDDS_CompareIndexedNamesPtr);
  }
  else {
    if (mode&SDDS_PASS_BY_STRING) {
      if (strcmp(field_name, "type")==0 && (givenType=SDDS_IdentifyType((char*)memory))>0)
        /* the type has been passed as a string (e.g., "double") */
        memcpy((char*)buffer, (char*)&givenType, sizeof(givenType));
      else if (!SDDS_ScanData((char*)memory, type, 0, (void*)buffer, 0, 0)) {
        SDDS_SetError("Unable to scan string data (SDDS_ChangeParameterInformation)");
        return(0);
      }
      memcpy((char*)parameterdef+SDDS_ParameterFieldInformation[field_index].offset, (void*)buffer, SDDS_type_size[type-1]);
    }
    else
      memcpy((char*)parameterdef+SDDS_ParameterFieldInformation[field_index].offset, memory, SDDS_type_size[type-1]);
  }

  return(type);
}

int32_t SDDS_ChangeArrayInformation(SDDS_DATASET *SDDS_dataset, char *field_name, void *memory, int32_t mode, ...)
{
  int32_t field_index, type, array_index, givenType;
  ARRAY_DEFINITION *arraydef;
  char *array_name;
  va_list argptr;
  int32_t retval;
  double buffer[4];

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ChangeArrayInformation"))
    return(0);
  
  if (!field_name) {
    SDDS_SetError("NULL field name passed. (SDDS_ChangeArrayInformation)");
    return(0);
  }

  va_start(argptr, mode);
  retval = 1;
  if (mode&SDDS_SET_BY_INDEX) {
    if ((array_index = va_arg(argptr, int32_t))<0 || array_index>=SDDS_dataset->layout.n_arrays) {
      SDDS_SetError("Invalid array index passed. (SDDS_ChangeArrayInformation)");
      retval = 0;
    }
  }
  else {
    if (!(array_name = va_arg(argptr, char *))) {
      SDDS_SetError("NULL array name passed. (SDDS_ChangeArrayInformation)");
      retval = 0;
    }
    if ((array_index=SDDS_GetArrayIndex(SDDS_dataset, array_name))<0) {
      SDDS_SetError("Unknown array name given (SDDS_ChangeArrayInformation)");
      retval = 0;
    }
  }
  arraydef = SDDS_dataset->layout.array_definition+array_index;
  va_end(argptr);
  if (!retval)
    return(0);

  for (field_index=0; field_index<SDDS_ARRAY_FIELDS; field_index++)
    if (strcmp(field_name, SDDS_ArrayFieldInformation[field_index].name)==0)
      break;
  if (field_index==SDDS_ARRAY_FIELDS) {
    SDDS_SetError("Unknown field name given (SDDS_ChangeArrayInformation)");
    return(0);
  }
  type = SDDS_ArrayFieldInformation[field_index].type;
  if (!memory)
    return(type);
  if (type==SDDS_STRING) {
    if (!SDDS_CopyString(((char**)((char*)arraydef+SDDS_ArrayFieldInformation[field_index].offset)),
                         (char*)memory)) {
      SDDS_SetError("Unable to copy field data (SDDS_ChangeArrayInformation)");
      return(0);
    }
    if (strcmp(field_name, "name")==0)
      qsort((char*)SDDS_dataset->layout.array_index, SDDS_dataset->layout.n_arrays,
            sizeof(*SDDS_dataset->layout.array_index), SDDS_CompareIndexedNamesPtr);
  }
  else {
    if (mode&SDDS_PASS_BY_STRING) {
      if (strcmp(field_name, "type")==0 && (givenType=SDDS_IdentifyType((char*)memory))>0)
        /* the type has been passed as a string (e.g., "double") */
        memcpy((char*)buffer, (char*)&givenType, sizeof(givenType));
      else if (!SDDS_ScanData((char*)memory, type, 0, (void*)buffer, 0, 0)) {
        SDDS_SetError("Unable to scan string data (SDDS_ChangeArrayInformation)");
        return(0);
      }
      memcpy((char*)arraydef+SDDS_ArrayFieldInformation[field_index].offset, (void*)buffer, SDDS_type_size[type-1]);
    }
    else
      memcpy((char*)arraydef+SDDS_ArrayFieldInformation[field_index].offset, memory, SDDS_type_size[type-1]);
  }

  return(type);
}
