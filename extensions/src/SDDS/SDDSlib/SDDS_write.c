/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_write.c
 * contents: internal routines for writing layout headers
 * purpose: These routines translate between the internal structure that the layout is stored in
 *          the namelist structure that is used for input/output.
 *  
 * Michael Borland, November 1993
 $Log: SDDS_write.c,v $
 Revision 1.13  2005/11/04 22:46:53  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.12  2002/08/14 16:34:50  soliday
 Added Open License

 Revision 1.11  2000/01/18 20:08:54  soliday
 Added support for ZLIB.

 Revision 1.10  1999/06/01 18:51:44  borland
 SDDS_WriteDataMode now writes the no_row_counts parameter if it is nonzero.
 This fixes a bug that resulted when a program tried to write a file with
 no row counts.

 Revision 1.9  1999/05/25 18:59:14  soliday
 Removed compiler warning on linux.

 Revision 1.8  1998/12/16 21:18:38  borland
 Escapes quotation marks in namelist values before printing them.

 Revision 1.7  1998/07/20 20:12:48  borland
 Added several more characters to the list of those that are quoted by
 namelist printing routine, because some of them caused the old version
 to choke when not quoted.

 Revision 1.6  1998/07/07 22:29:43  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

 Revision 1.5  1996/11/05 18:49:14  borland
 Now sets field_length for arrays to zero upon output.  This is consistent
 with what is done for columns.  If not used, then any ASCII files written
 with nonzero field_length will be unreadable by SDDS!

 * Revision 1.4  1996/03/12  04:01:02  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.3  1995/12/12  04:26:41  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.2  1995/09/05  21:11:20  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"

char *SDDS_BlankToNull(char *string);

/* There are two copies of the data elements defined in the next include file.
 * One copy is here and the other is in SDDS_write.c .  The data elements are
 * all static and don't communicate with each other.
 */
/*#include "SDDSnl.h"*/


/**************************************************************************/
/* This routine writes the protocol version of the file, and should       */
/* never be changed!                                                      */
/**************************************************************************/

int32_t SDDS_WriteVersion(int32_t version_number, FILE *fp)
{
  if (!fp)
    return(0);
  fprintf(fp, "SDDS%" PRId32 "\n", version_number);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteVersion(int32_t version_number, gzFile *gzfp)
{
  if (!gzfp)
    return(0);
  gzprintf(gzfp, "SDDS%" PRId32 "\n", version_number);
  return(1);
}
#endif

/**************************************************************/
/* SDDS protocol version 1 routines begin here.               */
/* There are no routers for output since only the most recent */
/* protocol will ever be emitted.                             */
/**************************************************************/

int32_t SDDS_PrintNamelistField(FILE *fp, char *name, char *value)
{
  char *buffer = NULL, *bPtr, *vPtr;
  if (!name)
    return 0;
  if (!value || !strlen(name))
    return 1;
  if (!strlen(value))
    fprintf(fp, "%s=\"\", ", name);
  else {
    if (strchr(value, '"')) {
      if (!(buffer=SDDS_Malloc(sizeof(*buffer)*2*strlen(value))))
        return 0;
      vPtr = value;
      bPtr = buffer;
      while (*vPtr) {
        if (*vPtr=='"')
          *bPtr++ = '\\';
        *bPtr++ = *vPtr++;
      }
      *bPtr = 0;
      value = buffer;
    }
    if (strpbrk(value, " ,*$\t\n\b"))
      fprintf(fp, "%s=\"%s\", ", name, value);
    else
      fprintf(fp, "%s=%s, ", name, value);
    if (buffer)
      free(buffer);
  }
  return 1;
}

#if defined(zLib)
int32_t SDDS_GZipPrintNamelistField(gzFile *gzfp, char *name, char *value)
{
  char *buffer = NULL, *bPtr, *vPtr;
  if (!name)
    return 0;
  if (!value || !strlen(name))
    return 1;
  if (!strlen(value))
    gzprintf(gzfp, "%s=\"\", ", name);
  else {
    if (strchr(value, '"')) {
      if (!(buffer=SDDS_Malloc(sizeof(*buffer)*2*strlen(value))))
        return 0;
      vPtr = value;
      bPtr = buffer;
      while (*vPtr) {
        if (*vPtr=='"')
          *bPtr++ = '\\';
        *bPtr++ = *vPtr++;
      }
      *bPtr = 0;
      value = buffer;
    }
    if (strpbrk(value, " ,*$\t\n\b"))
      gzprintf(gzfp, "%s=\"%s\", ", name, value);
    else
      gzprintf(gzfp, "%s=%s, ", name, value);
    if (buffer)
      free(buffer);
  }
  return 1;
}
#endif

int32_t SDDS_WriteDescription(char *text, char *contents, FILE *fp)
{
  if (!fp)
    return 0;
  if (!text && !contents)
    return 1;
  fputs("&description ", fp);
  SDDS_PrintNamelistField(fp, "text", text);
  SDDS_PrintNamelistField(fp, "contents", contents);
  fputs("&end\n", fp);
  return 1;
}

#if defined(zLib)
int32_t SDDS_GZipWriteDescription(char *text, char *contents, gzFile *gzfp)
{
  if (!gzfp)
    return 0;
  if (!text && !contents)
    return 1;
  gzputs(gzfp,"&description ");
  SDDS_GZipPrintNamelistField(gzfp, "text", text);
  SDDS_GZipPrintNamelistField(gzfp, "contents", contents);
  gzputs(gzfp,"&end\n");
  return 1;
}
#endif

int32_t SDDS_WriteColumnDefinition(COLUMN_DEFINITION *column_definition, FILE *fp) 
{
  if (!fp ||
      column_definition->type<=0 || column_definition->type>SDDS_NUM_TYPES)
    return(0);

  fputs("&column ", fp);
  SDDS_PrintNamelistField(fp, "name", column_definition->name);
  SDDS_PrintNamelistField(fp, "symbol", SDDS_BlankToNull(column_definition->symbol));
  SDDS_PrintNamelistField(fp, "units", SDDS_BlankToNull(column_definition->units));
  SDDS_PrintNamelistField(fp, "description", SDDS_BlankToNull(column_definition->description));
  SDDS_PrintNamelistField(fp, "format_string", SDDS_BlankToNull(column_definition->format_string));
  SDDS_PrintNamelistField(fp, "type", 
                          SDDS_type_name[column_definition->type-1]);
  fputs(" &end\n", fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteColumnDefinition(COLUMN_DEFINITION *column_definition, gzFile *gzfp) 
{
  if (!gzfp ||
      column_definition->type<=0 || column_definition->type>SDDS_NUM_TYPES)
    return(0);

  gzputs(gzfp,"&column ");
  SDDS_GZipPrintNamelistField(gzfp, "name", column_definition->name);
  SDDS_GZipPrintNamelistField(gzfp, "symbol", SDDS_BlankToNull(column_definition->symbol));
  SDDS_GZipPrintNamelistField(gzfp, "units", SDDS_BlankToNull(column_definition->units));
  SDDS_GZipPrintNamelistField(gzfp, "description", SDDS_BlankToNull(column_definition->description));
  SDDS_GZipPrintNamelistField(gzfp, "format_string", SDDS_BlankToNull(column_definition->format_string));
  SDDS_GZipPrintNamelistField(gzfp, "type", 
                          SDDS_type_name[column_definition->type-1]);
  gzputs(gzfp," &end\n");
  return(1);
}
#endif

int32_t SDDS_WriteParameterDefinition(PARAMETER_DEFINITION *parameter_definition, FILE *fp) 
{
  if (!fp || 
      parameter_definition->type<=0 || parameter_definition->type>SDDS_NUM_TYPES)
    return(0);
  fputs("&parameter ", fp);
  SDDS_PrintNamelistField(fp, "name", parameter_definition->name);
  SDDS_PrintNamelistField(fp, "symbol", SDDS_BlankToNull(parameter_definition->symbol));
  SDDS_PrintNamelistField(fp, "units", SDDS_BlankToNull(parameter_definition->units));
  SDDS_PrintNamelistField(fp, "description", SDDS_BlankToNull(parameter_definition->description));
  SDDS_PrintNamelistField(fp, "format_string", SDDS_BlankToNull(parameter_definition->format_string));
  SDDS_PrintNamelistField(fp, "type",
                          SDDS_type_name[parameter_definition->type-1]);
  SDDS_PrintNamelistField(fp, "fixed_value", parameter_definition->fixed_value);
  fputs("&end\n", fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteParameterDefinition(PARAMETER_DEFINITION *parameter_definition, gzFile *gzfp) 
{
  if (!gzfp || 
      parameter_definition->type<=0 || parameter_definition->type>SDDS_NUM_TYPES)
    return(0);
  gzputs(gzfp,"&parameter ");
  SDDS_GZipPrintNamelistField(gzfp, "name", parameter_definition->name);
  SDDS_GZipPrintNamelistField(gzfp, "symbol", SDDS_BlankToNull(parameter_definition->symbol));
  SDDS_GZipPrintNamelistField(gzfp, "units", SDDS_BlankToNull(parameter_definition->units));
  SDDS_GZipPrintNamelistField(gzfp, "description", SDDS_BlankToNull(parameter_definition->description));
  SDDS_GZipPrintNamelistField(gzfp, "format_string", SDDS_BlankToNull(parameter_definition->format_string));
  SDDS_GZipPrintNamelistField(gzfp, "type",
                          SDDS_type_name[parameter_definition->type-1]);
  SDDS_GZipPrintNamelistField(gzfp, "fixed_value", parameter_definition->fixed_value);
  gzputs(gzfp,"&end\n");
  return(1);
}
#endif

int32_t SDDS_WriteAssociateDefinition(ASSOCIATE_DEFINITION *associate_definition, FILE *fp) 
{
  if (!fp)
    return(0);

  fputs("&associate ", fp);
  SDDS_PrintNamelistField(fp, "name", associate_definition->name);
  SDDS_PrintNamelistField(fp, "filename", SDDS_BlankToNull(associate_definition->filename));
  SDDS_PrintNamelistField(fp, "contents", SDDS_BlankToNull(associate_definition->contents));
  SDDS_PrintNamelistField(fp, "path", SDDS_BlankToNull(associate_definition->path));
  SDDS_PrintNamelistField(fp, "description", SDDS_BlankToNull(associate_definition->description));
  fprintf(fp, "sdds=%" PRId32 , associate_definition->sdds);
  fputs(" &end\n", fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteAssociateDefinition(ASSOCIATE_DEFINITION *associate_definition, gzFile *gzfp) 
{
  if (!gzfp)
    return(0);

  gzputs(gzfp,"&associate ");
  SDDS_GZipPrintNamelistField(gzfp, "name", associate_definition->name);
  SDDS_GZipPrintNamelistField(gzfp, "filename", SDDS_BlankToNull(associate_definition->filename));
  SDDS_GZipPrintNamelistField(gzfp, "contents", SDDS_BlankToNull(associate_definition->contents));
  SDDS_GZipPrintNamelistField(gzfp, "path", SDDS_BlankToNull(associate_definition->path));
  SDDS_GZipPrintNamelistField(gzfp, "description", SDDS_BlankToNull(associate_definition->description));
  gzprintf(gzfp, "sdds=%" PRId32 , associate_definition->sdds);
  gzputs(gzfp," &end\n");
  return(1);
}
#endif

int32_t SDDS_WriteDataMode(DATA_MODE *data_mode, FILE *fp) 
{
  if (!fp || data_mode->mode<0 || data_mode->mode>SDDS_NUM_DATA_MODES)
    return(0);

  fputs("&data ", fp);
  SDDS_PrintNamelistField(fp, "mode", SDDS_data_mode[data_mode->mode-1]);
  if (data_mode->lines_per_row>1)
    fprintf(fp, "lines_per_row=%" PRId32 ", ", data_mode->lines_per_row);
  if (data_mode->no_row_counts)
    fprintf(fp, "no_row_counts=1, ");
  fputs("&end\n", fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteDataMode(DATA_MODE *data_mode, gzFile *gzfp) 
{
  if (!gzfp || data_mode->mode<0 || data_mode->mode>SDDS_NUM_DATA_MODES)
    return(0);

  gzputs(gzfp,"&data ");
  SDDS_GZipPrintNamelistField(gzfp, "mode", SDDS_data_mode[data_mode->mode-1]);
  if (data_mode->lines_per_row>1)
    gzprintf(gzfp, "lines_per_row=%" PRId32 ", ", data_mode->lines_per_row);
  if (data_mode->no_row_counts)
    gzprintf(gzfp, "no_row_counts=1, ");
  gzputs(gzfp,"&end\n");
  return(1);
}
#endif

int32_t SDDS_WriteArrayDefinition(ARRAY_DEFINITION *array_definition, FILE *fp) 
{
  if (!fp || array_definition->type<=0 || array_definition->type>SDDS_NUM_TYPES)
    return(0);

  fputs("&array ", fp);
  SDDS_PrintNamelistField(fp, "name", array_definition->name);
  SDDS_PrintNamelistField(fp, "symbol", SDDS_BlankToNull(array_definition->symbol));
  SDDS_PrintNamelistField(fp, "units", SDDS_BlankToNull(array_definition->units));
  SDDS_PrintNamelistField(fp, "description", SDDS_BlankToNull(array_definition->description));
  SDDS_PrintNamelistField(fp, "format_string", SDDS_BlankToNull(array_definition->format_string));
  SDDS_PrintNamelistField(fp, "group_name", SDDS_BlankToNull(array_definition->group_name));
  SDDS_PrintNamelistField(fp, "type", SDDS_type_name[array_definition->type-1]);
  if (array_definition->dimensions!=1)  /* 1 is default */
    fprintf(fp, "dimensions=%" PRId32 ", ", array_definition->dimensions); 
  fputs(" &end\n", fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteArrayDefinition(ARRAY_DEFINITION *array_definition, gzFile *gzfp) 
{
  if (!gzfp || array_definition->type<=0 || array_definition->type>SDDS_NUM_TYPES)
    return(0);

  gzputs(gzfp,"&array ");
  SDDS_GZipPrintNamelistField(gzfp, "name", array_definition->name);
  SDDS_GZipPrintNamelistField(gzfp, "symbol", SDDS_BlankToNull(array_definition->symbol));
  SDDS_GZipPrintNamelistField(gzfp, "units", SDDS_BlankToNull(array_definition->units));
  SDDS_GZipPrintNamelistField(gzfp, "description", SDDS_BlankToNull(array_definition->description));
  SDDS_GZipPrintNamelistField(gzfp, "format_string", SDDS_BlankToNull(array_definition->format_string));
  SDDS_GZipPrintNamelistField(gzfp, "group_name", SDDS_BlankToNull(array_definition->group_name));
  SDDS_GZipPrintNamelistField(gzfp, "type", SDDS_type_name[array_definition->type-1]);
  if (array_definition->dimensions!=1)  /* 1 is default */
    gzprintf(gzfp, "dimensions=%" PRId32 ", ", array_definition->dimensions); 
  gzputs(gzfp," &end\n");
  return(1);
}
#endif

char *SDDS_BlankToNull(char *string)
{
  if (!string || SDDS_StringIsBlank(string))
    return NULL;
  return string;
}
