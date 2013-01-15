/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_data.h
 * contents: data arrays use by SDDS routines
 * 
 * Michael Borland, November 1993.
 $Log: not supported by cvs2svn $
 Revision 1.6  2005/11/04 22:46:51  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.5  2002/08/14 16:34:47  soliday
 Added Open License

 Revision 1.4  1998/07/07 22:29:41  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

 Revision 1.3  1996/03/12 04:00:38  borland
 Reformatted code to 2-space indentation in K&R style.

 * Revision 1.2  1995/09/05  21:10:50  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"

char *SDDS_data_mode[SDDS_NUM_DATA_MODES] = {
  "binary", "ascii"
  };

char *SDDS_type_name[SDDS_NUM_TYPES] = {
  "double", "float", "long", "ulong", "short", "ushort", "string", "character"
  };
int32_t SDDS_type_size[SDDS_NUM_TYPES] = {
  sizeof(double), sizeof(float), sizeof(int32_t), sizeof(uint32_t), sizeof(short), sizeof(unsigned short), sizeof(char *), sizeof(char)
  };

char *SDDS_command[SDDS_NUM_COMMANDS] = {
  "description",
  "column",
  "parameter",
  "associate",
  "data",
  "include",
  "array",
} ;

/* Field name and type information:  This duplicates to some extent the
 * namelist description structures, but is easier to use and insulates 
 * the routines from the namelist stuff.  Eventually, this information
 * will be used to replace the namelist routines altogether.
 */
#include <stddef.h>

SDDS_FIELD_INFORMATION SDDS_DescriptionFieldInformation[SDDS_DESCRIPTION_FIELDS] = {
{"text", offsetof(SDDS_LAYOUT, description), SDDS_STRING, NULL},
{"contents", offsetof(SDDS_LAYOUT, contents), SDDS_STRING, NULL},
};

SDDS_ENUM_PAIR dataModeEnumPair[3] = {
{ "binary", SDDS_BINARY},
{ "ascii", SDDS_ASCII},
{ NULL, 0},
};

SDDS_ENUM_PAIR typeEnumPair[SDDS_NUM_TYPES+1] = {
{ "double", SDDS_DOUBLE },
{ "float", SDDS_FLOAT},
{ "short", SDDS_SHORT},
{ "ushort", SDDS_USHORT},
{ "long", SDDS_LONG},
{ "ulong", SDDS_ULONG},
{ "character", SDDS_CHARACTER},
{ "string", SDDS_STRING},
{ NULL, 0}
};

SDDS_FIELD_INFORMATION SDDS_DataFieldInformation[SDDS_DATA_FIELDS] = {
{"mode", offsetof(DATA_MODE, mode), SDDS_LONG, dataModeEnumPair},
{"lines_per_row", offsetof(DATA_MODE, lines_per_row), SDDS_LONG, NULL},
{"no_row_counts", offsetof(DATA_MODE, no_row_counts), SDDS_LONG, NULL},
{"additional_header_lines", offsetof(DATA_MODE, additional_header_lines), SDDS_LONG, NULL},
};

SDDS_FIELD_INFORMATION SDDS_ArrayFieldInformation[SDDS_ARRAY_FIELDS] = {
{"name",          offsetof(ARRAY_DEFINITION, name),          SDDS_STRING, NULL},
{"symbol",        offsetof(ARRAY_DEFINITION, symbol),        SDDS_STRING, NULL},
{"units",         offsetof(ARRAY_DEFINITION, units),         SDDS_STRING, NULL},
{"description",   offsetof(ARRAY_DEFINITION, description),   SDDS_STRING, NULL},
{"format_string", offsetof(ARRAY_DEFINITION, format_string), SDDS_STRING, NULL},
{"group_name",    offsetof(ARRAY_DEFINITION, group_name),    SDDS_STRING, NULL},
{"type",          offsetof(ARRAY_DEFINITION, type),          SDDS_LONG, typeEnumPair },
{"field_length",  offsetof(ARRAY_DEFINITION, field_length),  SDDS_LONG, NULL },
{"dimensions",    offsetof(ARRAY_DEFINITION, dimensions),    SDDS_LONG, NULL },
} ;

SDDS_FIELD_INFORMATION SDDS_ColumnFieldInformation[SDDS_COLUMN_FIELDS] = {
{"name",          offsetof(COLUMN_DEFINITION, name),          SDDS_STRING},
{"symbol",        offsetof(COLUMN_DEFINITION, symbol),        SDDS_STRING},
{"units",         offsetof(COLUMN_DEFINITION, units),         SDDS_STRING},
{"description",   offsetof(COLUMN_DEFINITION, description),   SDDS_STRING},
{"format_string", offsetof(COLUMN_DEFINITION, format_string), SDDS_STRING},
{"type",          offsetof(COLUMN_DEFINITION, type),          SDDS_LONG, typeEnumPair },
{"field_length",  offsetof(COLUMN_DEFINITION, field_length),  SDDS_LONG },
} ;

SDDS_FIELD_INFORMATION SDDS_ParameterFieldInformation[SDDS_PARAMETER_FIELDS] = {
{"name",          offsetof(PARAMETER_DEFINITION, name),          SDDS_STRING},
{"symbol",        offsetof(PARAMETER_DEFINITION, symbol),        SDDS_STRING},
{"units",         offsetof(PARAMETER_DEFINITION, units),         SDDS_STRING},
{"description",   offsetof(PARAMETER_DEFINITION, description),   SDDS_STRING},
{"format_string", offsetof(PARAMETER_DEFINITION, format_string), SDDS_STRING},
{"type",          offsetof(PARAMETER_DEFINITION, type),          SDDS_LONG, typeEnumPair },
{"fixed_value",   offsetof(PARAMETER_DEFINITION, fixed_value),   SDDS_STRING},
} ;

SDDS_FIELD_INFORMATION SDDS_AssociateFieldInformation[SDDS_ASSOCIATE_FIELDS] = {
{"name",          offsetof(ASSOCIATE_DEFINITION, name),        SDDS_STRING},
{"filename",      offsetof(ASSOCIATE_DEFINITION, filename),    SDDS_STRING},
{"path",          offsetof(ASSOCIATE_DEFINITION, path),        SDDS_STRING},
{"description",   offsetof(ASSOCIATE_DEFINITION, description), SDDS_STRING},
{"contents",      offsetof(ASSOCIATE_DEFINITION, contents),    SDDS_STRING},
{"sdds",          offsetof(ASSOCIATE_DEFINITION, sdds),        SDDS_LONG },
} ;

SDDS_FIELD_INFORMATION SDDS_IncludeFieldInformation[SDDS_INCLUDE_FIELDS] = {
{"filename",    0,      SDDS_STRING},
};
