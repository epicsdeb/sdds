/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_ascii.c
 * contents: SDDS ascii data input and output routines
 *
 * M. Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.32  2006/12/07 16:41:53  soliday
 Added SDDS_ScanData2 which is now used when reading ascii array data.

 Revision 1.31  2006/08/31 15:08:01  soliday
 Updated to work with SDDS2

 Revision 1.30  2005/11/04 22:46:51  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.29  2002/08/14 16:34:46  soliday
 Added Open License

 Revision 1.28  2002/05/29 19:41:31  soliday
 Cleared out the errors when an auto recovery occurs.

 Revision 1.27  2002/05/26 02:06:37  borland
 1. Call SDDS_SaveLayout() after changing the row count mode, just in case the
 user doesn't do this.  Otherwise, the row count mode gets restored in SDDS_StartPage.
 2. Set new autoRecovered flag whenever auto read recovery is done.  This prevents
 trying to read any more data, which might happen if the file is updated between
 two reads.

 Revision 1.26  2002/02/04 19:57:04  borland
 SDDS_UpdateBinaryPage() and SDDS_UpdateAsciiPage() now free string memory
 from the columns if in FLUSH_TABLE mode.  This prevents a big memory leak.

 Revision 1.25  2002/01/28 21:22:05  shang
 Removed free statement that was causing problems

 Revision 1.24  2002/01/28 20:30:32  soliday
 Shang noticed the array data was not being freed. This is fixed now.

 Revision 1.23  2002/01/22 18:43:39  soliday
 Added missing if defined zlib statement

 Revision 1.22  2002/01/15 22:50:43  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.21  2002/01/07 21:34:41  borland
 Added SDDS_SetRowLimit() and SDDS_GetRowLimit(), which permit the user to
 specify that SDDS tables with more than a certain number of rows are to
 be ignored.  This helps to deal with corrupted files, which sometimes
 occurs when the file server is slow.  Added code to SDDS_ReadBinaryPage()
 and SDDS_ReadAsciiPage() to recognize this specification.

 Revision 1.20  2001/11/30 15:34:35  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.19  2001/10/08 21:39:03  borland
 Fixed bug that resulted in accessing past the end of a string in SDDS_ScanData
 in some rare cases.

 Revision 1.18  2000/06/15 21:05:28  soliday
 Removed compiler warning on Solaris.

 Revision 1.17  2000/03/30 14:10:51  borland
 Added a test to ensure that page updates are not attempted on gziped files.

 Revision 1.16  2000/03/30 02:32:41  borland
 Now tracks rowcount offset as file is being read.  For use with
 SDDS_InitializeAppendToPage.  Only done for non-gzip files.

 Revision 1.15  2000/03/28 15:48:30  borland
 Added page update capability for ASCII files.

 Revision 1.14  2000/03/24 17:57:31  soliday
 SDDS_ScanData now can read parameters correctly that have a WIN32 style
 linefeed character.

 Revision 1.13  2000/01/18 20:11:08  soliday
 Added support for ZLIB.

 Revision 1.12  1999/07/06 21:34:39  soliday
 Added ctype.h include line

 Revision 1.11  1999/05/25 18:55:29  soliday
 Removed compiler warning on linux.

 Revision 1.10  1997/12/19 16:58:30  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.9  1996/11/05 18:50:16  borland
 No longer uses format_string fields for formatting ascii output.  This
 prevents loss of precision.

 * Revision 1.8  1996/03/21  16:38:31  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.7  1996/03/12  04:00:32  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.6  1996/01/15  21:40:22  borland
 * Added test for negative number of rows when reading file.
 *
 * Revision 1.5  1995/12/12  10:01:53  borland
 * Fixed bugs having to do with updates of no_row_counts=1 files; won't allow
 * changing no_row_counts after writing layout or for an input file.
 *
 * Revision 1.4  1995/12/12  04:26:31  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.3  1995/11/13  16:04:46  borland
 * Added int32_t case to strlen() in comparison to satisfy Solaris compiler.
 *
 * Revision 1.2  1995/09/05  21:10:41  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"
#include <ctype.h>

#undef DEBUG

#define COMMENT_POSITION 40

#if SDDS_VERSION!=2
#error "SDDS_VERSION does not match the version number of this file"
#endif

/* Define a dynamically expanded text buffer for use by ReadAsciiParameters,
   ReadAsciiArrays, and ReadAsciiPage.  Since these routines never call each
   other, it is okay that they share this buffer.
   */
#define INITIAL_BIG_BUFFER_SIZE SDDS_MAXLINE
static char *bigBuffer = NULL;
static int32_t bigBufferSize = 0;

int32_t SDDS_WriteTypedValue(void *data, int32_t index, int32_t type, char *format, FILE *fp)
{
  char c, *s;
  short hasWhitespace;

  if (!data) {
    SDDS_SetError("Unable to write value--data pointer is NULL (SDDS_WriteTypedValue)");
    return(0);
  }
  if (!fp) {
    SDDS_SetError("Unable to print value--file pointer is NULL (SDDS_WriteTypedValue)");
    return(0);
  }
  switch (type) {
  case SDDS_SHORT:
    fprintf(fp, format?format:"%hd", *((short*)data+index));
    break;
  case SDDS_USHORT:
    fprintf(fp, format?format:"%hu", *((unsigned short*)data+index));
    break;
  case SDDS_LONG:
    fprintf(fp, format?format:"%" PRId32, *((int32_t*)data+index));
    break;
  case SDDS_ULONG:
    fprintf(fp, format?format:"%" PRIu32, *((uint32_t*)data+index));
    break;
  case SDDS_FLOAT:
    fprintf(fp, format?format:"%15.8e", *((float*)data+index));
    break;
  case SDDS_DOUBLE:
    fprintf(fp, format?format:"%22.15e", *((double*)data+index));
    break;
  case SDDS_STRING:
    /* ignores format string */
    s = *((char**)data+index);
    hasWhitespace = 0;
    if (SDDS_HasWhitespace(s) || SDDS_StringIsBlank(s)) {
      fputc('"', fp);
      hasWhitespace = 1;
    }
    while (s && *s) {
      c = *s++;
      if (c=='!') 
        fputs("\\!", fp);
      else if (c=='\\')
        fputs("\\\\", fp);
      else if (c=='"')
        fputs("\\\"", fp);
      else if (c==' ')
        fputc(' ', fp);   /* don't escape plain spaces */
      else if (isspace(c) || !isprint(c))
        fprintf(fp, "\\%03o", c);
      else
        fputc(c, fp);
    }
    if (hasWhitespace)
      fputc('"', fp);
    break;
  case SDDS_CHARACTER:
    /* ignores format string */
    c = *((char*)data+index);
    if (c=='!')
      fputs("\\!", fp);
    else if (c=='\\')
      fputs("\\\\", fp);
    else if (c=='"')
      fputs("\\\"", fp);
    else if (!c || isspace(c) || !isprint(c))
      fprintf(fp, "\\%03o", c);
    else
      fputc(c, fp);
    break;
  default:
    SDDS_SetError("Unable to write value--unknown data type (SDDS_WriteTypedValue)");
    return(0);
  }
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteTypedValue(void *data, int32_t index, int32_t type, char *format, gzFile *gzfp)
{
  char c, *s;
  short hasWhitespace;

  if (!data) {
    SDDS_SetError("Unable to write value--data pointer is NULL (SDDS_WriteTypedValue)");
    return(0);
  }
  if (!gzfp) {
    SDDS_SetError("Unable to print value--file pointer is NULL (SDDS_WriteTypedValue)");
    return(0);
  }
  switch (type) {
  case SDDS_SHORT:
    gzprintf(gzfp, format?format:"%hd", *((short*)data+index));
    break;
  case SDDS_USHORT:
    gzprintf(gzfp, format?format:"%hu", *((unsigned short*)data+index));
    break;
  case SDDS_LONG:
    gzprintf(gzfp, format?format:"%" PRId32, *((int32_t*)data+index));
    break;
  case SDDS_ULONG:
    gzprintf(gzfp, format?format:"%" PRIu32, *((uint32_t*)data+index));
    break;
  case SDDS_FLOAT:
    gzprintf(gzfp, format?format:"%15.8e", *((float*)data+index));
    break;
  case SDDS_DOUBLE:
    gzprintf(gzfp, format?format:"%22.15e", *((double*)data+index));
    break;
  case SDDS_STRING:
    /* ignores format string */
    s = *((char**)data+index);
    hasWhitespace = 0;
    if (SDDS_HasWhitespace(s) || SDDS_StringIsBlank(s)) {
      gzputc(gzfp,'"');
      hasWhitespace = 1;
    }
    while (s && *s) {
      c = *s++;
      if (c=='!') 
        gzputs(gzfp,"\\!");
      else if (c=='\\')
        gzputs(gzfp,"\\\\");
      else if (c=='"')
        gzputs(gzfp,"\\\"");
      else if (c==' ')
        gzputc(gzfp,' ');   /* don't escape plain spaces */
      else if (isspace(c) || !isprint(c))
        gzprintf(gzfp, "\\%03o", c);
      else
        gzputc(gzfp,c);
    }
    if (hasWhitespace)
      gzputc(gzfp,'"');
    break;
  case SDDS_CHARACTER:
    /* ignores format string */
    c = *((char*)data+index);
    if (c=='!')
      gzputs(gzfp,"\\!");
    else if (c=='\\')
      gzputs(gzfp,"\\\\");
    else if (c=='"')
      gzputs(gzfp,"\\\"");
    else if (!c || isspace(c) || !isprint(c))
      gzprintf(gzfp, "\\%03o", c);
    else
      gzputc(gzfp,c);
    break;
  default:
    SDDS_SetError("Unable to write value--unknown data type (SDDS_WriteTypedValue)");
    return(0);
  }
  return(1);
}
#endif


int32_t SDDS_WriteAsciiPage(SDDS_DATASET *SDDS_dataset)
{
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  int32_t i, rows;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiPage"))
    return(0);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!(gzfp=SDDS_dataset->layout.gzfp)) {
      SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteAsciiPage)");
      return(0);
    }
    if (SDDS_dataset->layout.data_mode.no_row_counts && 
	(SDDS_dataset->page_number>1 || SDDS_dataset->file_had_data))
      gzputc(gzfp,'\n');
    gzprintf(gzfp, "! page number %" PRId32 "\n", SDDS_dataset->page_number);
    
    if (!SDDS_GZipWriteAsciiParameters(SDDS_dataset, gzfp) || !SDDS_GZipWriteAsciiArrays(SDDS_dataset, gzfp))
      return 0;
    rows = 0;
    if (SDDS_dataset->layout.n_columns) {
      rows = SDDS_CountRowsOfInterest(SDDS_dataset);
      if (!SDDS_dataset->layout.data_mode.no_row_counts) {
	SDDS_dataset->rowcount_offset = gztell(gzfp);
	if (SDDS_dataset->layout.data_mode.fixed_row_count) {
	  gzprintf(gzfp, "%20" PRId32 "\n", ((rows / 500) + 1) * 500);
	}
	else
	  gzprintf(gzfp, "%20" PRId32 "\n", rows);
      }
      for (i=0; i<SDDS_dataset->n_rows; i++)
	if (SDDS_dataset->row_flag[i] && !SDDS_GZipWriteAsciiRow(SDDS_dataset, i, gzfp))
	  return 0;
    }
    SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
    SDDS_dataset->n_rows_written = rows;
    SDDS_dataset->writing_page = 1;
    /*gzflush(gzfp, Z_FULL_FLUSH);*/
  } else {
#endif
    if (!(fp=SDDS_dataset->layout.fp)) {
      SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteAsciiPage)");
      return(0);
    }
    if (SDDS_dataset->layout.data_mode.no_row_counts && 
	(SDDS_dataset->page_number>1 || SDDS_dataset->file_had_data))
      fputc('\n', fp);
    fprintf(fp, "! page number %" PRId32 "\n", SDDS_dataset->page_number);
    
    if (!SDDS_WriteAsciiParameters(SDDS_dataset, fp) || !SDDS_WriteAsciiArrays(SDDS_dataset, fp))
      return 0;
    rows = 0;
    if (SDDS_dataset->layout.n_columns) {
      rows = SDDS_CountRowsOfInterest(SDDS_dataset);
      if (!SDDS_dataset->layout.data_mode.no_row_counts) {
	SDDS_dataset->rowcount_offset = ftell(fp);
	if (SDDS_dataset->layout.data_mode.fixed_row_count)
	  fprintf(fp, "%20" PRId32 "\n",  ((rows / 500) + 1) * 500);
	else
	  fprintf(fp, "%20" PRId32 "\n", rows);
      }
      for (i=0; i<SDDS_dataset->n_rows; i++)
	if (SDDS_dataset->row_flag[i] && !SDDS_WriteAsciiRow(SDDS_dataset, i, fp))
	  return 0;
    }
    SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
    SDDS_dataset->n_rows_written = rows;
    SDDS_dataset->writing_page = 1;
    fflush(fp);
#if defined(zLib)
  }
#endif 

  return(1);
}

int32_t SDDS_WriteAsciiParameters(SDDS_DATASET *SDDS_dataset, FILE *fp)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      continue;
    if (!SDDS_WriteTypedValue(SDDS_dataset->parameter[i], 0, layout->parameter_definition[i].type,
                              NULL, fp)) {
      SDDS_SetError("Unable to write ascii parameters (SDDS_WriteAsciiParameters)");
      return 0;
    }
    fputc('\n', fp);
  }
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteAsciiParameters(SDDS_DATASET *SDDS_dataset, gzFile *gzfp)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      continue;
    if (!SDDS_GZipWriteTypedValue(SDDS_dataset->parameter[i], 0, layout->parameter_definition[i].type,
                              NULL, gzfp)) {
      SDDS_SetError("Unable to write ascii parameters (SDDS_WriteAsciiParameters)");
      return 0;
    }
    gzputc(gzfp,'\n');
  }
  return(1);
}
#endif

int32_t SDDS_WriteAsciiArrays(SDDS_DATASET *SDDS_dataset, FILE *fp)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  ARRAY_DEFINITION *array_definition;
  SDDS_ARRAY *array;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiArray"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (j=0; j<layout->n_arrays; j++) {
    array_definition = layout->array_definition+j;
    array = &SDDS_dataset->array[j];
    for (i=0; i<array_definition->dimensions; i++)
      fprintf(fp, "%" PRId32 " ", array->dimension[i]);
    fprintf(fp, "          ! %" PRId32 "-dimensional array %s:\n", array_definition->dimensions, array_definition->name);
    for (i=0; i<array->elements; ) {
      if (!SDDS_WriteTypedValue(array->data, i, array_definition->type, NULL, fp)) {
        SDDS_SetError("Unable to write array--couldn't write ASCII data (SDDS_WriteAsciiArrays)");
        return(0);
      }
      i++;
      if (i%6==0 || i==array->elements) 
        fputc('\n', fp);
      else
        fputc(' ', fp);
    }
  }
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteAsciiArrays(SDDS_DATASET *SDDS_dataset, gzFile *gzfp)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  ARRAY_DEFINITION *array_definition;
  SDDS_ARRAY *array;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiArray"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (j=0; j<layout->n_arrays; j++) {
    array_definition = layout->array_definition+j;
    array = &SDDS_dataset->array[j];
    for (i=0; i<array_definition->dimensions; i++)
      gzprintf(gzfp, "%" PRId32 " ", array->dimension[i]);
    gzprintf(gzfp, "          ! %" PRId32 "-dimensional array %s:\n", array_definition->dimensions, array_definition->name);
    for (i=0; i<array->elements; ) {
      if (!SDDS_GZipWriteTypedValue(array->data, i, array_definition->type, NULL, gzfp)) {
        SDDS_SetError("Unable to write array--couldn't write ASCII data (SDDS_WriteAsciiArrays)");
        return(0);
      }
      i++;
      if (i%6==0 || i==array->elements) 
        gzputc(gzfp,'\n');
      else
        gzputc(gzfp,' ');
    }
  }
  return(1);
}
#endif

int32_t SDDS_WriteAsciiRow(SDDS_DATASET *SDDS_dataset, int32_t row, FILE *fp)
{
  int32_t i, n_per_line, line, newline_needed;
  /*  int32_t  embedded_quotes_present;*/
  SDDS_LAYOUT *layout;
  /*  char *predefined_format, *s;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (SDDS_dataset->layout.data_mode.lines_per_row<=0)
    SDDS_dataset->layout.data_mode.lines_per_row = 1;
  n_per_line = SDDS_dataset->layout.n_columns/SDDS_dataset->layout.data_mode.lines_per_row;
  line = 1;
  newline_needed = 0;
  for (i=0; i<layout->n_columns; i++) {
    if (!SDDS_WriteTypedValue(SDDS_dataset->data[i], row, layout->column_definition[i].type,
                              NULL, fp)) {
      SDDS_SetError("Unable to write ascii row (SDDS_WriteAsciiRow)");
      return 0;
    }
    if ((i+1)%n_per_line==0 && line!=SDDS_dataset->layout.data_mode.lines_per_row) {
      newline_needed = 0;
      fputc('\n', fp);
      line++;
    }
    else {
      fputc(' ', fp);
      newline_needed = 1;
    }
  }
  if (newline_needed)
    fputc('\n', fp);
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteAsciiRow(SDDS_DATASET *SDDS_dataset, int32_t row, gzFile *gzfp)
{
  int32_t i, n_per_line, line, newline_needed;
  /*  int32_t  embedded_quotes_present;*/
  SDDS_LAYOUT *layout;
  /*  char *predefined_format, *s;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteAsciiRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (SDDS_dataset->layout.data_mode.lines_per_row<=0)
    SDDS_dataset->layout.data_mode.lines_per_row = 1;
  n_per_line = SDDS_dataset->layout.n_columns/SDDS_dataset->layout.data_mode.lines_per_row;
  line = 1;
  newline_needed = 0;
  for (i=0; i<layout->n_columns; i++) {
    if (!SDDS_GZipWriteTypedValue(SDDS_dataset->data[i], row, layout->column_definition[i].type,
                              NULL, gzfp)) {
      SDDS_SetError("Unable to write ascii row (SDDS_WriteAsciiRow)");
      return 0;
    }
    if ((i+1)%n_per_line==0 && line!=SDDS_dataset->layout.data_mode.lines_per_row) {
      newline_needed = 0;
      gzputc(gzfp,'\n');
      line++;
    }
    else {
      gzputc(gzfp,' ');
      newline_needed = 1;
    }
  }
  if (newline_needed)
    gzputc(gzfp,'\n');
  return(1);
}
#endif

int32_t SDDS_ReadAsciiParameters(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, first_read;
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;

  layout = &SDDS_dataset->layout;
  first_read = 1;
  if (bigBuffer==NULL)  {
    if (!(bigBuffer = SDDS_Malloc(sizeof(*bigBuffer)*(bigBufferSize=INITIAL_BIG_BUFFER_SIZE)))) {
      SDDS_SetError("Unable to read parameters--buffer allocation failure (SDDS_ReadAsciiParameters)");
      return(0);
    }
  }
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      bigBuffer[0] = 0;
      if (!layout->parameter_definition[i].fixed_value) {
	if (!fgetsGZipSkipCommentsResize(&bigBuffer, &bigBufferSize, gzfp, '!')) {
	  if (first_read)
	    return(-1);
	  SDDS_SetError("Unable to read parameters--data ends prematurely (SDDS_ReadAsciiParameters)");
	  return(0);
	}
	first_read = 0;
	bigBuffer[strlen(bigBuffer)-1] = 0;
      }
      else 
	strcpy(bigBuffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(bigBuffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
	SDDS_SetError("Unable to read page--parameter scanning error (SDDS_ReadAsciiParameters)");
	return(0);
      }
    }
  } else {
#endif
    fp = layout->fp;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      bigBuffer[0] = 0;
      if (!layout->parameter_definition[i].fixed_value) {
	if (!fgetsSkipCommentsResize(&bigBuffer, &bigBufferSize, fp, '!')) {
	  if (first_read)
	    return(-1);
	  SDDS_SetError("Unable to read parameters--data ends prematurely (SDDS_ReadAsciiParameters)");
	  return(0);
	}
	first_read = 0;
	bigBuffer[strlen(bigBuffer)-1] = 0;
      }
      else 
	strcpy(bigBuffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(bigBuffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
	SDDS_SetError("Unable to read page--parameter scanning error (SDDS_ReadAsciiParameters)");
	return(0);
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_ReadAsciiArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, length;
  SDDS_LAYOUT *layout;
  static char *buffer = NULL;
  static int32_t bufferSize = 0;
  char *bigBufferCopy, *p0;
  int32_t bigBufferCopySize;
  /*  ARRAY_DEFINITION *array_definition;*/
  SDDS_ARRAY *array;
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;

  layout = &SDDS_dataset->layout;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
  } else {
#endif
    fp = layout->fp;
#if defined(zLib)
  }
#endif
  if (bigBuffer==NULL)  {
    if (!(bigBuffer = SDDS_Malloc(sizeof(*bigBuffer)*(bigBufferSize=INITIAL_BIG_BUFFER_SIZE)))) {
      SDDS_SetError("Unable to read parameters--buffer allocation failure (SDDS_ReadAsciiArrays)");
      return(0);
    }
  }
  for (i=0; i<layout->n_arrays; i++) {
#if defined(zLib)
    if (SDDS_dataset->layout.gzipFile) {
      if (!fgetsGZipSkipCommentsResize(&bigBuffer, &bigBufferSize, gzfp, '!') || SDDS_StringIsBlank(bigBuffer)) {
	if (i==0)
	  return(-1);
	SDDS_SetError("Unable to read array--dimensions missing (SDDS_ReadAsciiArrays)");
	return(0);
      }
    } else {
#endif
      if (!fgetsSkipCommentsResize(&bigBuffer, &bigBufferSize, fp, '!') || SDDS_StringIsBlank(bigBuffer)) {
	if (i==0)
          return(-1);
	SDDS_SetError("Unable to read array--dimensions missing (SDDS_ReadAsciiArrays)");
	return(0);
      }
#if defined(zLib)
    }
#endif
    if (!(array = SDDS_dataset->array+i)) {
      SDDS_SetError("Unable to read array--pointer to structure storage area is NULL (SDDS_ReadAsciiArrays)");
      return(0);
    }
    if (array->definition && !SDDS_FreeArrayDefinition(array->definition)) {
      SDDS_SetError("Unable to get array--array definition corrupted (SDDS_ReadAsciiArrays)");
      return(0);
    }
    if (!SDDS_CopyArrayDefinition(&array->definition, layout->array_definition+i)) {
      SDDS_SetError("Unable to read array--definition copy failed (SDDS_ReadAsciiArrays)");
      return(0);
    }
    /*if (array->dimension) free(array->dimension);*/
    if (!(array->dimension = SDDS_Realloc(array->dimension, sizeof(*array->dimension)*array->definition->dimensions))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadAsciiArrays)");
      return(0);
    }
    array->elements = 1;
    if ((length = strlen(bigBuffer))>=bufferSize) {
      if (!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*(bufferSize = 2*length)))) {
        SDDS_SetError("Unable to scan data--allocation failure (SDDS_ReadAsciiArrays");
        return(0);
      }
    }
    for (j=0; j<array->definition->dimensions; j++) {
      if (SDDS_GetToken(bigBuffer, buffer, SDDS_MAXLINE)<=0 || sscanf(buffer, "%" SCNd32, array->dimension+j)!=1 ||
          array->dimension[j]<0) {
        SDDS_SetError("Unable to read array--dimensions missing or negative (SDDS_ReadAsciiArrays)");
        return(0);
      }
      array->elements *= array->dimension[j];
    }
    if (!array->elements)
      continue;
    if (array->data) free(array->data);
    array->data = array->pointer = NULL;
    if (!(array->data = SDDS_Realloc(array->data, array->elements*SDDS_type_size[array->definition->type-1]))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadAsciiArrays)");
      return(0);
    }
    SDDS_ZeroMemory(array->data, array->elements*SDDS_type_size[array->definition->type-1]);
    j = 0;
    bigBuffer[0] = 0;
    do {
      if (SDDS_StringIsBlank(bigBuffer)) {
        bigBuffer[0] = 0;
#if defined(zLib)
	if (SDDS_dataset->layout.gzipFile) {
	  if (!fgetsGZipSkipCommentsResize(&bigBuffer, &bigBufferSize, gzfp, '!') || SDDS_StringIsBlank(bigBuffer)) {
	    SDDS_SetError("Unable to read array--dimensions missing (SDDS_ReadAsciiArrays)");
	    return(0);
	  }
	} else {
#endif
	  if (!fgetsSkipCommentsResize(&bigBuffer, &bigBufferSize, fp, '!') || SDDS_StringIsBlank(bigBuffer)) {
	    SDDS_SetError("Unable to read array--dimensions missing (SDDS_ReadAsciiArrays)");
	    return(0);
	  }
#if defined(zLib)
	}
#endif
      }
      /* copy the bigBuffer because SDDS_ScanData2 will change the bigBufferCopy pointer that SDDS_ScanData 
         cannot do becuase bigBuffer is a static variable. This change was implemented to greatly improve
         the speed of reading a very large line of array elements. The previous version just did a lot of
         strcpy commands to these huge lines which was really slow. */
      bigBufferCopy = bigBuffer;
      bigBufferCopySize = strlen(bigBufferCopy);
      do {
        if (!SDDS_ScanData2(bigBufferCopy, &bigBufferCopy, &bigBufferCopySize, array->definition->type, array->definition->field_length, array->data, j, 0)) {
          SDDS_SetError("Unable to read array--error scanning data element (SDDS_ReadAsciiArrays)");
          return(0);
        }
      } while (++j<array->elements && !SDDS_StringIsBlank(bigBufferCopy));
      bigBuffer[0] = 0;
    } while (j<array->elements);
  }
  return(1);
}


int32_t SDDS_ReadAsciiPage(SDDS_DATASET *SDDS_dataset, int32_t sparse_interval, int32_t sparse_offset)
{
  SDDS_LAYOUT *layout;
  int32_t n_rows, i, j, k, no_row_counts, end_of_data, retval, lineCount, rows_to_store;
  /*  int32_t page_number;*/
  char s[SDDS_MAXLINE];
  char *dataRead, *bigBufferCopy;
  int32_t bigBufferCopySize;
  /*  char *ptr;*/
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  /*  double value;*/

  if (SDDS_dataset->autoRecovered)
    return -1;

  SDDS_SetReadRecoveryMode(0);
  
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = SDDS_dataset->layout.gzfp;
  } else {
#endif
    fp = SDDS_dataset->layout.fp;
#if defined(zLib)
  }
#endif
  if (SDDS_dataset->page_number==-1)
    /* end of file already hit */
    return -1;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (gzeof(gzfp) && SDDS_dataset->page_number>0)
      /* end of file and not first page about to be read */
      return SDDS_dataset->page_number = -1;
  } else {
#endif
    if (feof(fp) && SDDS_dataset->page_number>0)
      /* end of file and not first page about to be read */
      return SDDS_dataset->page_number = -1;
#if defined(zLib)
  }
#endif
  if (bigBuffer==NULL)  {
    if (!(bigBuffer = SDDS_Malloc(sizeof(*bigBuffer)*(bigBufferSize=INITIAL_BIG_BUFFER_SIZE)))) {
      SDDS_SetError("Unable to read parameters--buffer allocation failure (SDDS_ReadAsciiPage)");
      return(0);
    }
  }
  if (!SDDS_AsciiDataExpected(SDDS_dataset) && SDDS_dataset->page_number!=0)
    /* if no columns or arrays and only fixed value parameters, then only one "page" allowed */
    return(SDDS_dataset->page_number=-1);

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (SDDS_dataset->page_number==0)
      for (i=0; i<SDDS_dataset->layout.data_mode.additional_header_lines; i++) {
	if (!fgetsGZipSkipComments(s, SDDS_MAXLINE, gzfp, '!'))
	  return( SDDS_dataset->page_number= -1 );  /* indicates end of data */
      }
  } else {
#endif
    if (SDDS_dataset->page_number==0)
      for (i=0; i<SDDS_dataset->layout.data_mode.additional_header_lines; i++) {
	if (!fgetsSkipComments(s, SDDS_MAXLINE, fp, '!'))
	  return( SDDS_dataset->page_number= -1 );  /* indicates end of data */
      }
#if defined(zLib)
  }
#endif

  /* the next call increments the page number */
  if (!SDDS_StartPage(SDDS_dataset, 0)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_ReadAsciiPage)");
    return(0);
  }

  layout = &SDDS_dataset->layout;

  if ((retval=SDDS_ReadAsciiParameters(SDDS_dataset))<1) {
    if (retval)
      return(SDDS_dataset->page_number=retval);
    SDDS_SetError("Unable to read page--couldn't read parameters (SDDS_ReadAsciiPage)");
    return(0);
  }
  if ((retval=SDDS_ReadAsciiArrays(SDDS_dataset))<1) {
    if (retval)
      return(SDDS_dataset->page_number=retval);
    SDDS_SetError("Unable to read page--couldn't read arrays (SDDS_ReadAsciiPage)");
    return(0);
  }

  if (sparse_interval<=0)
    sparse_interval = 1;
  if (sparse_offset<0)
    sparse_offset = 0;
  
  SDDS_dataset->rowcount_offset = -1;
  if (layout->n_columns) {
    n_rows = 0;
    no_row_counts = 0;
    if (!SDDS_dataset->layout.data_mode.no_row_counts) {
      /* read the number of rows */
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!fgetsGZipSkipComments(s, SDDS_MAXLINE, gzfp, '!')) 
	  return(SDDS_dataset->page_number=-1);   /* indicates end of data */
      } else {
#endif
      do {
        SDDS_dataset->rowcount_offset = ftell(fp);
        if (!fgets(s, SDDS_MAXLINE, fp))
          return(SDDS_dataset->page_number=-1);   /* indicates end of data */
        } while (s[0]=='!');
#if defined(zLib)
      }
#endif
      if (sscanf(s, "%" SCNd32, &n_rows)!=1 || n_rows<0) {
        SDDS_SetError("Unable to read page--file has no (valid) number-of-rows entry (SDDS_ReadAsciiPage)");
        return(0);
      }
      if (n_rows>SDDS_GetRowLimit()) {
        /* number of rows is "unreasonably" large---treat like end-of-file */
        return(SDDS_dataset->page_number=-1);
      }
      rows_to_store = n_rows/sparse_interval+2;
    }
    else {
      no_row_counts = 1;
      n_rows = TABLE_LENGTH_INCREMENT;
      rows_to_store = n_rows;
    }
    
    
    if (rows_to_store>=SDDS_dataset->n_rows_allocated) {
      /* lengthen the page */
      if (!SDDS_LengthenTable(SDDS_dataset, rows_to_store-SDDS_dataset->n_rows_allocated)) {
        SDDS_SetError("Unable to read page--couldn't lengthen data page (SDDS_ReadAsciiPage)");
        return(0);
      }
    }

    /* read the page values */
    j = end_of_data = k = 0;
    s[0] = 0;
    if (!no_row_counts && n_rows==0) {
      SDDS_dataset->n_rows = 0;
      return(SDDS_dataset->page_number);
    }
    bigBuffer[0] = 0;
    bigBufferCopy = bigBuffer;
    do {
      if (j>=SDDS_dataset->n_rows_allocated) {
        /* lengthen the page */
        if (!SDDS_LengthenTable(SDDS_dataset, TABLE_LENGTH_INCREMENT)) {
          SDDS_SetError("Unable to read page--couldn't lengthen data page (SDDS_ReadAsciiPage)");
          return(0);
        }
      }
      lineCount = 0;
      dataRead = NULL;
      for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
        if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
          continue;
        if (SDDS_StringIsBlank(bigBufferCopy)) {
          bigBuffer[0] = 0;
          bigBufferCopy = bigBuffer;
          dataRead = NULL;
#if defined(zLib)
	  if (SDDS_dataset->layout.gzipFile) {
	    if (!(dataRead=fgetsGZipSkipCommentsResize(&bigBuffer, &bigBufferSize, gzfp, '!')) || 
		SDDS_StringIsBlank(bigBuffer)) {
	      SDDS_dataset->n_rows = j;
	      if (no_row_counts) {
		/* legitmate end of data set */
		end_of_data = 1;
		break;
	      }
	      /* error, but may be recoverable */
	      gzseek(gzfp, 0L, 2);
	      if (SDDS_dataset->autoRecover) {
                SDDS_dataset->autoRecovered = 1;
		SDDS_ClearErrors();
		return(SDDS_dataset->page_number);
              }
	      SDDS_SetReadRecoveryMode(1);
	      return(0);
	    }
	  } else {
#endif
	    if (!(dataRead=fgetsSkipCommentsResize(&bigBuffer, &bigBufferSize, fp, '!')) || 
		SDDS_StringIsBlank(bigBuffer)) {
	      SDDS_dataset->n_rows = j;
	      if (no_row_counts) {
		/* legitmate end of data set */
		end_of_data = 1;
		break;
	      }
	      /* error, but may be recoverable */
	      fseek(fp, 0L, 2);
	      if (SDDS_dataset->autoRecover) {
                SDDS_dataset->autoRecovered = 1;
		SDDS_ClearErrors();
		return(SDDS_dataset->page_number);
              }
	      SDDS_SetReadRecoveryMode(1);
	      return(0);
	    }
#if defined(zLib)
	  }
#endif
          lineCount++;
          bigBufferCopy = bigBuffer;
          bigBufferCopySize = strlen(bigBufferCopy);
        }
        if (!SDDS_ScanData2(bigBufferCopy, &bigBufferCopy, &bigBufferCopySize, 
                            layout->column_definition[i].type, layout->column_definition[i].field_length,
                            SDDS_dataset->data[i], j, 0)) {
          /* error, but may be recoverable */
          SDDS_dataset->n_rows = j;
#if defined(zLib)
	  if (SDDS_dataset->layout.gzipFile) {
	    gzseek(gzfp, 0L, 2);
	  } else {
#endif
	    fseek(fp, 0L, 2);
#if defined(zLib)
	  }
#endif
	  if (SDDS_dataset->autoRecover) {
            SDDS_dataset->autoRecovered = 1;
	    SDDS_ClearErrors();
	    return(SDDS_dataset->page_number);
          }
          SDDS_SetReadRecoveryMode(1);
          SDDS_SetError("Unable to read page--scanning error (SDDS_ReadAsciiPage)");
          return(0);
        }
#if defined(DEBUG)
        fprintf(stderr, "line remaining = %s\n", bigBuffer);
#endif
      }
      if (end_of_data)
        /* ran out of data for no_row_counts=1 */
        break;
      if (layout->data_mode.lines_per_row!=0 && lineCount!=layout->data_mode.lines_per_row) {
        sprintf(s, "Unable to read page--line layout error at line %" PRId32 " of page %" PRId32 " (SDDS_ReadAsciiPage)",
                j+1, SDDS_dataset->page_number);
        SDDS_SetError(s);
        /* data ends prematurely, which is an error that may be recoverable */
#if defined(zLib)
	if (SDDS_dataset->layout.gzipFile) {
	  gzseek(gzfp, 0L, 2);
	} else {
#endif
	  fseek(fp, 0L, 2);
#if defined(zLib)
	}
#endif
	if (SDDS_dataset->autoRecover) {
          SDDS_dataset->autoRecovered = 1;
	  SDDS_ClearErrors();
	  return(SDDS_dataset->page_number);
        }
        SDDS_SetReadRecoveryMode(1);
        SDDS_dataset->n_rows = j;
        return(0);
      }
      if (layout->data_mode.lines_per_row!=0) {
        bigBuffer[0] = 0;
        bigBufferCopy = bigBuffer;
      }
      if  (--sparse_offset<0 && k%sparse_interval==0)
        j++;
      k++;
    } while (k<n_rows || no_row_counts) ;
    if (end_of_data && !(SDDS_dataset->page_number==1) && j==0 && !dataRead)
      /* For end of data in no_row_counts=1 mode for any page other than the first,
       * an end-of-file is not a valid way to end an empty page (only an incomplete line is)
       */
      return SDDS_dataset->page_number=-1;
    SDDS_dataset->n_rows = j;
  }

  return(SDDS_dataset->page_number);
}

int32_t SDDS_ScanData(char *string, int32_t type, int32_t field_length, void *data, int32_t index, int32_t is_parameter)
{
  static char *buffer = NULL;
  int32_t abs_field_length, length;
  static int32_t bufferSize = 0;

  abs_field_length = abs(field_length);
  if (!string) {
    SDDS_SetError("Unable to scan data--input string is NULL (SDDS_ScanData)");
    return(0);
  }
  if (!data) {
    SDDS_SetError("Unable to scan data--data pointer is NULL (SDDS_ScanData)");
    return(0);
  }
  if (bufferSize==0) {
    if (!(buffer = SDDS_Malloc(sizeof(*buffer)*(bufferSize=SDDS_MAXLINE)))) {
      SDDS_SetError("Unable to scan data--allocation failure (SDDS_ScanData)");
      return(0);
    }
  }
  if ((length = strlen(string))<abs_field_length)
    length = abs_field_length;
  if (bufferSize<=length) {
    if (!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*(bufferSize = 2*length)))) {
      /* I allocate 2*length in the hopes that I won't need to realloc too often if I do this */
      SDDS_SetError("Unable to scan data--allocation failure (SDDS_ScanData)");
      return(0);
    }
  }
  if (type!=SDDS_STRING) {
    /* for non-string data, fill buffer with string to be scanned */
    if (field_length) {
      /* fill with fixed number of characters */
      if (abs_field_length>(int32_t)strlen(string)) {
        strcpy(buffer, string);
        *string = 0;
      }
      else {
        strncpy(buffer, string, abs_field_length);
        buffer[field_length] = 0;
        strcpy(string, string+abs_field_length);
      }
    }
    else if (SDDS_GetToken(string, buffer, bufferSize)<0) {
      SDDS_SetError("Unable to scan data--tokenizing error (SDDS_ScanData)");
      return(0);
    }
  }
  switch (type) {
  case SDDS_SHORT:
    if (sscanf(buffer, "%hd", ((short*)data)+index)==1)
      return(1);
    break;
  case SDDS_USHORT:
    if (sscanf(buffer, "%hu", ((unsigned short*)data)+index)==1)
      return(1);
    break;
  case SDDS_LONG:
    if (sscanf(buffer, "%" SCNd32, ((int32_t*)data)+index)==1)
      return(1);
    break;
  case SDDS_ULONG:
    if (sscanf(buffer, "%" SCNu32, ((uint32_t*)data)+index)==1)
      return(1);
    break;
  case SDDS_FLOAT:
    if (sscanf(buffer, "%f", ((float*)data)+index)==1)
      return 1;
    break;
  case SDDS_DOUBLE:
    if (sscanf(buffer, "%lf", ((double*)data)+index)==1)
      return 1;
    break;
  case SDDS_STRING:
    if (is_parameter) {
      int32_t len;
      if (((char**)data)[index]) {
        free(((char**)data)[index]);
        ((char**)data)[index] = NULL;
      }
      if ((len = strlen(string))>0) {
        if (string[len-1] == '\r')
          string[len-1] = 0;
      }
      if (string[0]=='"') 
        SDDS_GetToken(string, buffer, bufferSize);
      else
        strcpy(buffer, string);
      SDDS_InterpretEscapes(buffer);
      if (SDDS_CopyString(((char**)data)+index, buffer))
        return(1);
    }
    else {
      if (field_length) {
        if (abs_field_length>(int32_t)strlen(string)) {
          strcpy(buffer, string);
          *string = 0;
        }
        else {
          strncpy(buffer, string, abs_field_length);
          buffer[abs_field_length] = 0;
          strcpy(string, string+abs_field_length);
        }
        if (field_length<0)
          SDDS_RemovePadding(buffer);
      }
      else if (SDDS_GetToken(string, buffer, bufferSize)<0)
        break;
      if (((char**)data)[index]) {
        free(((char**)data)[index]);
        ((char**)data)[index] = NULL;
      }
      SDDS_InterpretEscapes(buffer);
      if (SDDS_CopyString(((char**)data)+index, buffer)) 
        return(1);
    }
    break;
  case SDDS_CHARACTER:
    SDDS_InterpretEscapes(buffer);
    *(((char*)data)+index) = buffer[0];
    return 1;
  default:
    SDDS_SetError("Unknown data type encountered (SDDS_ScanData)");
    return(0);
  }
  SDDS_SetError("Unable to scan data--scanning or allocation error (SDDS_ScanData)");
  return(0);
}

int32_t SDDS_ScanData2(char *string, char **pstring, int32_t *strlength, int32_t type, int32_t field_length, void *data, int32_t index, int32_t is_parameter)
{
  static char *buffer = NULL;
  int32_t abs_field_length, length;
  static int32_t bufferSize = 0;

  abs_field_length = abs(field_length);
  if (!string) {
    SDDS_SetError("Unable to scan data--input string is NULL (SDDS_ScanData2)");
    return(0);
  }
  if (!data) {
    SDDS_SetError("Unable to scan data--data pointer is NULL (SDDS_ScanData2)");
    return(0);
  }
  if (bufferSize==0) {
    if (!(buffer = SDDS_Malloc(sizeof(*buffer)*(bufferSize=SDDS_MAXLINE)))) {
      SDDS_SetError("Unable to scan data--allocation failure (SDDS_ScanData2)");
      return(0);
    }
  }
  length = *strlength;
  if (length<abs_field_length)
    length = abs_field_length;
  if (bufferSize<=length) {
    if (!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*(bufferSize = 2*length)))) {
      /* I allocate 2*length in the hopes that I won't need to realloc too often if I do this */
      SDDS_SetError("Unable to scan data--allocation failure (SDDS_ScanData2)");
      return(0);
    }
  }
  if (type!=SDDS_STRING) {
    /* for non-string data, fill buffer with string to be scanned */
    if (field_length) {
      /* fill with fixed number of characters */
      if (abs_field_length>*strlength) {
        strcpy(buffer, string);
        **pstring = 0;
        *strlength = 0;
      }
      else {
        strncpy(buffer, string, abs_field_length);
        buffer[abs_field_length] = 0;
        *pstring += abs_field_length;
        *strlength -= abs_field_length;
      }
    }
    else if (SDDS_GetToken2(string, pstring, strlength, buffer, bufferSize)<0) {
      SDDS_SetError("Unable to scan data--tokenizing error (SDDS_ScanData2)");
      return(0);
    }
  }
  switch (type) {
  case SDDS_SHORT:
    if (sscanf(buffer, "%hd", ((short*)data)+index)==1)
      return(1);
    break;
  case SDDS_USHORT:
    if (sscanf(buffer, "%hu", ((unsigned short*)data)+index)==1)
      return(1);
    break;
  case SDDS_LONG:
    if (sscanf(buffer, "%" SCNd32, ((int32_t*)data)+index)==1)
      return(1);
    break;
  case SDDS_ULONG:
    if (sscanf(buffer, "%" SCNu32, ((uint32_t*)data)+index)==1)
      return(1);
    break;
  case SDDS_FLOAT:
    if (sscanf(buffer, "%f", ((float*)data)+index)==1)
      return 1;
    break;
  case SDDS_DOUBLE:
    if (sscanf(buffer, "%lf", ((double*)data)+index)==1)
      return 1;
    break;
  case SDDS_STRING:
    if (is_parameter) {
      int32_t len;
      if (((char**)data)[index]) {
        free(((char**)data)[index]);
        ((char**)data)[index] = NULL;
      }
      if ((len = *strlength)>0) {
        if (*pstring[len-1] == '\r') {
          *pstring[len-1] = 0;
          *strlength -= 1;
        }
      }
      if (*pstring[0]=='"') 
        SDDS_GetToken2(*pstring, pstring, strlength, buffer, bufferSize);
      else
        strcpy(buffer, string);
      SDDS_InterpretEscapes(buffer);
      if (SDDS_CopyString(((char**)data)+index, buffer))
        return(1);
    }
    else {
      if (field_length) {
        if (abs_field_length>*strlength) {
          strcpy(buffer, string);
          **pstring = 0;
          *strlength = 0;
        }
        else {
          strncpy(buffer, string, abs_field_length);
          buffer[abs_field_length] = 0;
          *pstring += abs_field_length;
          *strlength -= abs_field_length;
        }
        if (field_length<0)
          SDDS_RemovePadding(buffer);
      }
      else if (SDDS_GetToken2(string, pstring, strlength, buffer, bufferSize)<0)
        break;
      if (((char**)data)[index]) {
        free(((char**)data)[index]);
        ((char**)data)[index] = NULL;
      }
      SDDS_InterpretEscapes(buffer);
      if (SDDS_CopyString(((char**)data)+index, buffer)) 
        return(1);
    }
    break;
  case SDDS_CHARACTER:
    SDDS_InterpretEscapes(buffer);
    *(((char*)data)+index) = buffer[0];
    return 1;
  default:
    SDDS_SetError("Unknown data type encountered (SDDS_ScanData2)");
    return(0);
  }
  SDDS_SetError("Unable to scan data--scanning or allocation error (SDDS_ScanData2)");
  return(0);
}

int32_t SDDS_AsciiDataExpected(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  if (SDDS_dataset->layout.n_columns || SDDS_dataset->layout.n_arrays)
    return(1);
  for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
    if (!SDDS_dataset->layout.parameter_definition[i].fixed_value)
      return(1);
  return(0);
}

int32_t SDDS_UpdateAsciiPage(SDDS_DATASET *SDDS_dataset, uint32_t mode)
{
  FILE *fp;
  int32_t i, rows, offset, code;
  SDDS_FILEBUFFER *fBuffer;

#ifdef DEBUG
  fprintf(stderr, "%" PRId32 " virtual rows present, first=%" PRId32 "\n", 
          SDDS_CountRowsOfInterest(SDDS_dataset),
          SDDS_dataset->first_row_in_mem);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_UpdateAsciiPage"))
    return(0);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    SDDS_SetError("Unable to perform page updates on a gzip file (SDDS_UpdateAsciiPage)");
    return 0;
  }
#endif
  if (!SDDS_dataset->writing_page) {
    if (!(code = SDDS_WriteAsciiPage(SDDS_dataset)))
      return 0;
    if (mode&FLUSH_TABLE) {
      SDDS_FreeTableStrings(SDDS_dataset);
      SDDS_dataset->first_row_in_mem = SDDS_CountRowsOfInterest(SDDS_dataset);
      SDDS_dataset->last_row_written = -1;
      SDDS_dataset->n_rows = 0;
    }
    return code;
  }
  if (!(fp=SDDS_dataset->layout.fp)) {
    SDDS_SetError("Unable to update page--file pointer is NULL (SDDS_UpdateAsciiPage)");
    return(0);
  }
  fBuffer= &SDDS_dataset->fBuffer;
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateAsciiPage)");
    return 0;
  }
  offset = ftell(fp);
  
  rows = SDDS_CountRowsOfInterest(SDDS_dataset)+SDDS_dataset->first_row_in_mem;
  if (rows==SDDS_dataset->n_rows_written)
    return(1);
  if (rows<SDDS_dataset->n_rows_written) {
    SDDS_SetError("Unable to update page--new number of rows less than previous number (SDDS_UpdateAsciiPage)");
    return(0);
  }
  if ((!SDDS_dataset->layout.data_mode.fixed_row_count) || 
      ((SDDS_dataset->n_rows_written / 500) != (rows / 500))) {
    if (!SDDS_dataset->layout.data_mode.no_row_counts) {
      if (SDDS_fseek(fp, SDDS_dataset->rowcount_offset, 0)==-1) {
	SDDS_SetError("Unable to update page--failure doing fseek (SDDS_UpdateAsciiPage)");
	return(0);
      }
      /* overwrite the existing row count */
      if (SDDS_dataset->layout.data_mode.fixed_row_count)
	fprintf(fp, "%20" PRId32 "\n", ((rows / 500) + 1) * 500);
      else
	fprintf(fp, "%20" PRId32 "\n", rows);
      if (SDDS_fseek(fp, offset, 0)==-1) {
	SDDS_SetError("Unable to update page--failure doing fseek to end of page (SDDS_UpdateAsciiPage)");
	return(0);
      }
    }
  }
  for (i=SDDS_dataset->last_row_written+1; i<SDDS_dataset->n_rows; i++)
    if (SDDS_dataset->row_flag[i])
      SDDS_WriteAsciiRow(SDDS_dataset, i, fp);
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateAsciiPage)");
    return 0;
  }
  SDDS_dataset->last_row_written = SDDS_dataset->n_rows-1;
  SDDS_dataset->n_rows_written = rows;
  if (mode&FLUSH_TABLE) {
    SDDS_FreeTableStrings(SDDS_dataset);
    SDDS_dataset->first_row_in_mem = rows;
    SDDS_dataset->last_row_written = -1;
    SDDS_dataset->n_rows = 0;
  }
  return(1);
  
}
