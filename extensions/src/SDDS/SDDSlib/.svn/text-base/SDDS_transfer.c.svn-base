/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_transfer.c
 * contents: user's routines for transferring definitions from
 *           one data set to another
 * 
 * Michael Borland, September 1994
 $Log: not supported by cvs2svn $
 Revision 1.10  2005/01/13 16:47:59  shang
 added string support to rpn functions

 Revision 1.9  2002/08/14 16:34:50  soliday
 Added Open License

 Revision 1.8  1999/05/25 18:58:47  soliday
 Removed compiler warning on linux.

 Revision 1.7  1998/12/16 21:15:17  borland
 Added various transfer modes to SDDS_TransferAll*.  Allows keeping or
 overwriting existing definitions with the same name.

 Revision 1.6  1996/03/12 04:00:56  borland
 Reformatted code to 2-space indentation in K&R style.

 * Revision 1.5  1996/03/10  01:05:17  borland
 * Corrected subroutine names in error messages.
 *
 * Revision 1.4  1995/12/15  06:35:36  borland
 * Added SDDS_TransferAllXXXDefinitions() routines, where XXX is Column,
 * Parameter, and Array.
 *
 * Revision 1.3  1995/11/08  04:23:16  borland
 * Added DefineParameterLikeArray and DefineColumnLikeArray routines.
 *
 * Revision 1.2  1995/09/05  21:11:12  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"

int32_t SDDS_TransferColumnDefinition(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  COLUMN_DEFINITION *coldef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to transfer column definition--NULL or blank name passed (SDDS_TransferColumnDefinition)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(coldef = SDDS_GetColumnDefinition(source, name))) {
    SDDS_SetError("Unable to transfer column definition--unknown column named (SDDS_TransferColumnDefinition)");
    return 0;
  }
  if (SDDS_GetColumnIndex(target, newName)>=0) {
    SDDS_SetError("Unable to transfer column definition--column already present (SDDS_TransferColumnDefinition)");
    return 0;
  }
  if (SDDS_DefineColumn(target, newName, 
                        coldef->symbol, coldef->units, coldef->description, coldef->format_string,
                        coldef->type, coldef->field_length)<0) {
    SDDS_FreeColumnDefinition(coldef);
    SDDS_SetError("Unable to transfer column definition--call to define column failed (SDDS_TransferColumnDefinition)");
    return 0;
  }
  SDDS_FreeColumnDefinition(coldef);
  return 1;
}

int32_t SDDS_TransferParameterDefinition(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  PARAMETER_DEFINITION *pardef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to transfer parameter definition--NULL or blank name passed (SDDS_TransferParameterDefinition)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(pardef = SDDS_GetParameterDefinition(source, name))) {
    SDDS_SetError("Unable to transfer parameter definition--unknown parameter named (SDDS_TransferParameterDefinition)");
    return 0;
  }
  if (SDDS_GetParameterIndex(target, newName)>=0) {
    SDDS_SetError("Unable to transfer parameter definition--parameter already present (SDDS_TransferParameterDefinition)");
    return 0;
  }
  if (SDDS_DefineParameter(target, newName,
                           pardef->symbol, pardef->units, pardef->description, pardef->format_string,
                           pardef->type, NULL)<0) {
    SDDS_FreeParameterDefinition(pardef);
    SDDS_SetError("Unable to transfer parameter definition--call to define parameter failed (SDDS_TransferParameterDefinition)");
    return 0;
  }
  SDDS_FreeParameterDefinition(pardef);
  return 1;
}

int32_t SDDS_TransferArrayDefinition(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  ARRAY_DEFINITION *ardef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to transfer array definition--NULL or blank name passed (SDDS_TransferArrayDefinition)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(ardef = SDDS_GetArrayDefinition(source, name))) {
    SDDS_SetError("Unable to transfer array definition--unknown array named (SDDS_TransferArrayDefinition)");
    return 0;
  }
  if (SDDS_GetArrayIndex(target, newName)>=0) {
    SDDS_SetError("Unable to transfer array definition--array already present (SDDS_TransferArrayDefinition)");
    return 0;
  }
  if (SDDS_DefineArray(target, newName,
                       ardef->symbol, ardef->units, ardef->description, ardef->format_string,
                       ardef->type, ardef->field_length, ardef->dimensions, ardef->group_name)<0) {
    SDDS_FreeArrayDefinition(ardef);
    SDDS_SetError("Unable to transfer array definition--call to define array failed (SDDS_TransferArrayDefinition)");
    return 0;
  }
  SDDS_FreeArrayDefinition(ardef);
  return 1;
}

int32_t SDDS_TransferAssociateDefinition(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  ASSOCIATE_DEFINITION *asdef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to transfer associate definition--NULL or blank name passed (SDDS_TransferAssociateDefinition)");
    return 0;
  }
  if (!newName)
    newName = name;
  if ((asdef=SDDS_GetAssociateDefinition(target, name))) {
    SDDS_FreeAssociateDefinition(asdef);
    SDDS_SetError("Unable to transfer associate definition--associate already present (SDDS_TransferAssociateDefinition)");
    return 0;
  }
  if (!(asdef = SDDS_GetAssociateDefinition(source, newName))) {
    SDDS_SetError("Unable to transfer associate definition--unknown associate named (SDDS_TransferAssociateDefinition)");
    return 0;
  }
  if (SDDS_DefineAssociate(target, newName, asdef->filename,
                           asdef->path, asdef->description, asdef->contents, asdef->sdds)<0) {
    SDDS_FreeAssociateDefinition(asdef);
    SDDS_SetError("Unable to transfer associate definition--call to define associate failed (SDDS_TransferAssociateDefinition)");
    return 0;
  }
  SDDS_FreeAssociateDefinition(asdef);
  return 1;
}

int32_t SDDS_DefineParameterLikeColumn(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  COLUMN_DEFINITION *coldef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to define parameter--NULL or blank name passed (SDDS_DefineParameterLikeColumn)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(coldef = SDDS_GetColumnDefinition(source, name))) {
    SDDS_SetError("Unable to define parameter--unknown column named (SDDS_DefineParameterLikeColumn)");
    return 0;
  }
  if (SDDS_GetParameterIndex(target, newName)>=0) {
    SDDS_SetError("Unable to define parameter--already exists (SDDS_DefineParameterLikeColumn)");
    return 0;
  }
  if (SDDS_DefineParameter(target, newName,
                           coldef->symbol, coldef->units, coldef->description, coldef->format_string,
                           coldef->type, NULL)<0) {
    SDDS_FreeColumnDefinition(coldef);
    SDDS_SetError("Unable to define parameter--call to define parameter failed (SDDS_DefineParameterLikeColumn)");
    return 0;
  }
  SDDS_FreeColumnDefinition(coldef);
  return 1;
}

int32_t SDDS_DefineParameterLikeArray(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  ARRAY_DEFINITION *arrayDef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to define parameter--NULL or blank name passed (SDDS_DefineParameterLikeArray)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(arrayDef = SDDS_GetArrayDefinition(source, name))) {
    SDDS_SetError("Unable to define parameter--unknown array named (SDDS_DefineParameterLikeArray)");
    return 0;
  }
  if (SDDS_GetParameterIndex(target, newName)>=0) {
    SDDS_SetError("Unable to define parameter--already exists (SDDS_DefineParameterLikeArray)");
    return 0;
  }
  if (SDDS_DefineParameter(target, newName,
                           arrayDef->symbol, arrayDef->units, arrayDef->description, arrayDef->format_string,
                           arrayDef->type, NULL)<0) {
    SDDS_FreeArrayDefinition(arrayDef);
    SDDS_SetError("Unable to define parameter--call to define parameter failed (SDDS_DefineParameterLikeArray)");
    return 0;
  }
  SDDS_FreeArrayDefinition(arrayDef);
  return 1;
}

int32_t SDDS_DefineColumnLikeParameter(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  PARAMETER_DEFINITION *pardef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to define column--NULL or blank name passed (SDDS_DefineColumnLikeParameter)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(pardef = SDDS_GetParameterDefinition(source, name))) {
    SDDS_SetError("Unable to define column--unknown parameter named (SDDS_DefineColumnLikeParameter)");
    return 0;
  }
  if (SDDS_GetColumnIndex(target, newName)>=0) {
    SDDS_SetError("Unable to define column--already exists (SDDS_DefineColumnLikeParameter)");
    return 0;
  }
  if (SDDS_DefineColumn(target, newName,
                        pardef->symbol, pardef->units, pardef->description, pardef->format_string,
                        pardef->type, 0)<0) {
    SDDS_FreeParameterDefinition(pardef);
    SDDS_SetError("Unable to define column--call to define column failed (SDDS_DefineColumnLikeParameter)");
    return 0;
  }
  SDDS_FreeParameterDefinition(pardef);
  return 1;
}

int32_t SDDS_DefineColumnLikeArray(SDDS_DATASET *target, SDDS_DATASET *source, char *name, char *newName)
{
  ARRAY_DEFINITION *arrayDef;

  if (!name || SDDS_StringIsBlank(name)) {
    SDDS_SetError("Unable to define column--NULL or blank name passed (SDDS_DefineColumnLikeArray)");
    return 0;
  }
  if (!newName)
    newName = name;
  if (!(arrayDef = SDDS_GetArrayDefinition(source, name))) {
    SDDS_SetError("Unable to define column--unknown array named (SDDS_DefineColumnLikeArray)");
    return 0;
  }
  if (SDDS_GetColumnIndex(target, newName)>=0) {
    SDDS_SetError("Unable to define column--already exists (SDDS_DefineColumnLikeArray)");
    return 0;
  }
  if (SDDS_DefineColumn(target, newName,
                        arrayDef->symbol, arrayDef->units, arrayDef->description, arrayDef->format_string,
                        arrayDef->type, 0)<0) {
    SDDS_FreeArrayDefinition(arrayDef);
    SDDS_SetError("Unable to define column--call to define column failed (SDDS_DefineColumnLikeArray)");
    return 0;
  }
  SDDS_FreeArrayDefinition(arrayDef);
  return 1;
}

int32_t SDDS_TransferAllParameterDefinitions(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,
                                          uint32_t mode)
{
  SDDS_LAYOUT *target, *source;
  int32_t i, index;
  static char messBuffer[1024];
  
  if (!SDDS_CheckDataset(SDDS_target, "SDDS_TransferAllParameterDefinitions"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_TransferAllParameterDefinitions"))
    return(0);
  if (mode&SDDS_TRANSFER_KEEPOLD && mode&SDDS_TRANSFER_OVERWRITE) {
    SDDS_SetError("Inconsistent mode flags (SDDS_TransferAllParameterDefinitions)");
    return 0;
  }
  target = &SDDS_target->layout;
  source = &SDDS_source->layout;
  SDDS_DeferSavingLayout(1);
  for (i=0; i<source->n_parameters; i++) {
    if ((index=SDDS_GetParameterIndex(SDDS_target, source->parameter_definition[i].name))>=0) {
      /* already exists */
      if (mode&SDDS_TRANSFER_KEEPOLD) 
        continue;
      if (!(mode&SDDS_TRANSFER_OVERWRITE)) {
        sprintf(messBuffer,
                "Unable to define parameter %s---already exists (SDDS_TransferAllParameterDefinitions)",
                source->parameter_definition[i].name);
        SDDS_SetError(messBuffer);
        SDDS_DeferSavingLayout(0);
        return 0;
      }
      if (!SDDS_ChangeParameterInformation(SDDS_target, "symbol", 
                                           &source->parameter_definition[i].symbol,
                                           SDDS_BY_INDEX, index) ||
          !SDDS_ChangeParameterInformation(SDDS_target, "units", 
                                           &source->parameter_definition[i].units,
                                           SDDS_BY_INDEX, index) ||
          !SDDS_ChangeParameterInformation(SDDS_target, "description", 
                                           &source->parameter_definition[i].description,
                                           SDDS_BY_INDEX, index) ||
          !SDDS_ChangeParameterInformation(SDDS_target, "format_string", 
                                           &source->parameter_definition[i].format_string,
                                           SDDS_BY_INDEX, index) ||
          !SDDS_ChangeParameterInformation(SDDS_target, "type", 
                                           &source->parameter_definition[i].type,
                                           SDDS_BY_INDEX, index) ||
          (source->parameter_definition[i].fixed_value!=NULL &&
           !SDDS_ChangeParameterInformation(SDDS_target, "fixed_value", 
                                           &source->parameter_definition[i].fixed_value,
                                           SDDS_BY_INDEX, index))) {
        SDDS_SetError("Unable to define parameter---problem with overwrite (SDDS_TransferAllParameterDefinitions)");
        SDDS_DeferSavingLayout(0);
        return 0;
      }
      if (source->parameter_definition[i].fixed_value==NULL)
        target->parameter_definition[index].fixed_value = NULL;
      target->parameter_definition[index].definition_mode = 
        source->parameter_definition[index].definition_mode;
      if (target->parameter_definition[index].type==SDDS_STRING)
        target->parameter_definition[index].memory_number =
          SDDS_CreateRpnMemory(source->parameter_definition[i].name, 1);
      else
        target->parameter_definition[index].memory_number =
          SDDS_CreateRpnMemory(source->parameter_definition[i].name, 0);
    } else {
      if (SDDS_DefineParameter(SDDS_target, source->parameter_definition[i].name, 
                               source->parameter_definition[i].symbol,
                               source->parameter_definition[i].units, source->parameter_definition[i].description,
                               source->parameter_definition[i].format_string, source->parameter_definition[i].type,
                               source->parameter_definition[i].fixed_value)<0) {
        SDDS_SetError("Unable to define parameter (SDDS_TransferAllParameterDefinitions)");
        SDDS_DeferSavingLayout(0);
        return 0;
      }
    }
  }
  SDDS_DeferSavingLayout(0);
  return 1;
}

int32_t SDDS_TransferAllColumnDefinitions(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,
                                       uint32_t mode)
{
  SDDS_LAYOUT *target, *source;
  int32_t i, index;
  char messBuffer[1024];
  
  if (!SDDS_CheckDataset(SDDS_target, "SDDS_TransferAllColumnDefinitions"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_TransferAllColumnDefinitions"))
    return(0);
  if (mode&SDDS_TRANSFER_KEEPOLD && mode&SDDS_TRANSFER_OVERWRITE) {
    SDDS_SetError("Inconsistent mode flags (SDDS_TransferAllColumnDefinitions)");
    return 0;
  }
  target = &SDDS_target->layout;
  source = &SDDS_source->layout;
  SDDS_DeferSavingLayout(1);
  for (i=0; i<source->n_columns; i++)
    if ((index=SDDS_GetParameterIndex(SDDS_target, source->parameter_definition[i].name))>=0) {
      /* already exists */
      if (mode&SDDS_TRANSFER_KEEPOLD) 
        continue;
      if (!(mode&SDDS_TRANSFER_OVERWRITE)) {
        sprintf(messBuffer,
                "Unable to define parameter %s---already exists (SDDS_TransferAllParameterDefinitions)",
                source->parameter_definition[i].name);
        SDDS_SetError(messBuffer);
        SDDS_DeferSavingLayout(0);
        return 0;
      }
      if (source->column_definition[i].type!=target->column_definition[index].type 
          && SDDS_target->n_rows_allocated) {
        sprintf(messBuffer, 
                "Unable to define parameter %s---type mismatch and table already allocated (SDDS_TransferAllParameterDefinitions)",
                source->parameter_definition[i].name);
        SDDS_SetError(messBuffer);
        SDDS_DeferSavingLayout(0);
        return 0;
      }
      if (!SDDS_ChangeColumnInformation(SDDS_target, "symbol", 
                                        &source->column_definition[i].symbol,
                                        SDDS_BY_INDEX, index) ||
          !SDDS_ChangeColumnInformation(SDDS_target, "units", 
                                        &source->column_definition[i].units,
                                        SDDS_BY_INDEX, index) ||
          !SDDS_ChangeColumnInformation(SDDS_target, "description", 
                                        &source->column_definition[i].description,
                                        SDDS_BY_INDEX, index) ||
          !SDDS_ChangeColumnInformation(SDDS_target, "format_string", 
                                        &source->column_definition[i].format_string,
                                        SDDS_BY_INDEX, index) ||
          !SDDS_ChangeColumnInformation(SDDS_target, "type", 
                                        &source->column_definition[i].type,
                                        SDDS_BY_INDEX, index) ||
          !SDDS_ChangeColumnInformation(SDDS_target, "field_length", 
                                        &source->column_definition[i].field_length,
                                        SDDS_BY_INDEX, index)) {
        SDDS_SetError("Unable to define column---problem with overwrite (SDDS_TransferAllColumnDefinitions)");
        SDDS_DeferSavingLayout(0);
        return 0;
      }
      target->column_definition[index].definition_mode = 
        source->column_definition[index].definition_mode;
      if (target->column_definition[index].type==SDDS_STRING)
        target->column_definition[index].memory_number =
          SDDS_CreateRpnMemory(source->column_definition[i].name, 1);
      else
        target->column_definition[index].memory_number =
          SDDS_CreateRpnMemory(source->column_definition[i].name, 0);
    } else {
      if (SDDS_DefineColumn(SDDS_target, source->column_definition[i].name, source->column_definition[i].symbol,
                            source->column_definition[i].units, source->column_definition[i].description,
                            source->column_definition[i].format_string, source->column_definition[i].type,
                            source->column_definition[i].field_length)<0) {
        SDDS_SetError("Unable to define column (SDDS_TransferAllColumnDefinitions)");
        SDDS_DeferSavingLayout(0);
        return 0;
      }
    }
  SDDS_DeferSavingLayout(0);
  return 1;
}

int32_t SDDS_TransferAllArrayDefinitions(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,
                                      uint32_t mode)
{
  SDDS_LAYOUT *target, *source;
  int32_t i;

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_TransferAllArrayDefinitions"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_TransferAllArrayDefinitions"))
    return(0);
  if (mode) {
    /* haven't done this one yet */
    SDDS_SetError("Nonzero mode not supported for arrays (SDDS_TransferAllArrayDefinitions)");
    return 0;
  }
  target = &SDDS_target->layout;
  source = &SDDS_source->layout;
  SDDS_DeferSavingLayout(1);
  for (i=0; i<source->n_arrays; i++)
    if (SDDS_DefineArray(SDDS_target, source->array_definition[i].name, source->array_definition[i].symbol,
                         source->array_definition[i].units, source->array_definition[i].description,
                         source->array_definition[i].format_string, source->array_definition[i].type,
                         source->array_definition[i].field_length,
                         source->array_definition[i].dimensions,
                         source->array_definition[i].group_name)<0) {
      SDDS_SetError("Unable to define parameter (SDDS_TransferAllArrayDefinitions)");
      SDDS_DeferSavingLayout(0);
      return 0;
    }
  SDDS_DeferSavingLayout(0);
  return 1;
}
