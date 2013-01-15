/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_copy.c
 * contents: user's routines for copying data sets internally
 * 
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.36  2006/04/07 21:40:02  soliday
 Freed memory that I forgot to with the last change.

 Revision 1.35  2006/04/07 21:36:08  soliday
 Improved the execution speed of SDDS_CopyRowsOfInterest

 Revision 1.34  2005/11/04 22:46:51  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.33  2002/08/14 16:34:47  soliday
 Added Open License

 Revision 1.32  2002/05/28 18:16:42  shang
 fixed a bug in SDDS_CopyArrays()

 Revision 1.31  2002/01/23 00:26:00  shang
 modified SDDS_CopyArrays() such that it will cast the source data with target
 array type into target data when the source array and target array have different types.

 Revision 1.30  2002/01/15 22:50:44  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.29  2001/11/30 15:34:36  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.28  2000/09/19 20:13:22  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.27  2000/06/15 21:05:59  soliday
 Removed compiler warnings on vxWorks.

 Revision 1.26  2000/04/14 16:39:36  soliday
 Now check to make sure gzFile is the same size as a void*

 Revision 1.25  2000/04/06 15:59:37  soliday
 Added support for Borland C.

 Revision 1.24  2000/01/18 20:12:29  soliday
 Added support for ZLIB.

 Revision 1.23  1999/09/16 22:09:49  soliday
 SDDS_InitializeCopy changes the filemode to FOPEN_READ_MODE or FOPEN_WRITE_MODE

 Revision 1.22  1999/08/10 15:09:14  soliday
 Changed stdin and stdout to binary mode for WIN32

 Revision 1.21  1999/07/22 15:37:17  soliday
 Uses macros for fopen mode.

 Revision 1.20  1999/05/25 18:56:00  soliday
 Removed compiler warning on linux.

 Revision 1.19  1999/04/14 13:58:05  borland
 Fixed some function types and returns.

 Revision 1.18  1998/12/07 14:45:03  borland
 Improved error messages for casting parameters and columns.

 Revision 1.17  1998/07/16 16:00:38  borland
 SDDS_copy: fixed error statement (named the wrong routine).
 SDDS_input: SDDS_InitializeInput now puts filenames in quotes for
 error messages.

 Revision 1.16  1998/06/12 16:10:36  borland
 Fixed memory leak due to failure to free sorted indices when terminating.
 Also, wasn't copying sorted indices when saving the layout.

 Revision 1.15  1997/12/19 16:58:32  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.14  1997/10/29 21:24:12  borland
 Fixed problems that occured when an application attempted to copy
 rows into a dataset that has no columns defined.

 Revision 1.13  1997/09/09 01:49:38  borland
 SDDS_CopyAdditionalRows and SDDS_CopyRowsOfInterest now do type casting.

 * Revision 1.12  1996/03/12  04:00:37  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.11  1996/03/10  01:06:17  borland
 * CopyLayout will not copy no_row_counts=1.  Always asserts no_row_counts=0.
 *
 * Revision 1.10  1996/01/21  17:32:11  borland
 * Added "m" filemode to SDDS_InitializeCopy; permits making memory copy of a
 * data page and manipulating it with all the usual routines.
 *
 * Revision 1.9  1996/01/20  05:37:44  borland
 * Added SDDS_AppendLayout() to support new capability in sddscombine (merge
 * inconsistent files without loss of data).
 *
 * Revision 1.8  1996/01/19  00:13:39  borland
 * Switched from SDDS_UnpackFopen() to UnpackFopen(), removing former from
 * SDDS_input.c .  Using popenUsed entry in layout to keep track of whether
 * input files is a pipe or a bonafide file.
 *
 * Revision 1.7  1996/01/10  20:48:20  borland
 * Added SDDS_FileIsLocked() routine. Use it prior to fopen() calls to avoid
 * truncating the file.
 *
 * Revision 1.6  1996/01/10  17:21:00  borland
 * Added SDDS_LockFile() and code to use it in SDDS_InitializeCopy(),
 * SDDS_InitializeOutput(), and SDDS_InitializeAppend().
 *
 * Revision 1.5  1995/12/12  04:26:35  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.4  1995/12/10  02:56:25  borland
 * Fixed bug in previous change; added more informative error message with
 * type inconsistency.
 *
 * Revision 1.3  1995/12/10  02:35:33  borland
 * Added code to CopyAdditionalRows to detect inconsistent data types in
 * source and target structures; changed call to SDDS_LengthTable in
 * same routine so that it doesn't allocate more memory than needed.
 *
 * Revision 1.2  1995/09/05  21:10:47  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"
#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif

int32_t SDDS_InitializeCopy(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source, char *filename, char *filemode)
{
  char s[SDDS_MAXLINE];
#if defined(zLib)
  char *extension;
#endif

  if (sizeof(gzFile) != sizeof(void*)) {
    SDDS_SetError("gzFile is not the same size as void *, possible corruption of the SDDS_LAYOUT structure");
    return(0);
  }
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_InitializeCopy"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_target, "SDDS_InitializeCopy"))
    return(0);
  if (!SDDS_ZeroMemory((void *)SDDS_target, sizeof(SDDS_DATASET))) {
    SDDS_SetError("Unable to copy layout--can't zero SDDS_DATASET structure (SDDS_InitializeCopy)");
    return(0);
  }
  if (strcmp(filemode, "r")==0) {
    filemode = FOPEN_READ_MODE;
    SDDS_target->mode=SDDS_READMODE;
  } else if (strcmp(filemode, "w")==0) {
    filemode = FOPEN_WRITE_MODE;
    SDDS_target->mode=SDDS_WRITEMODE;
  }
  SDDS_target->pagecount_offset=NULL;
  if (!(strcmp(filemode, "r")==0 || strcmp(filemode, "w")==0 ||
	strcmp(filemode, "rb")==0 || strcmp(filemode, "wb")==0 ||
	strcmp(filemode, "m")==0)) {
    SDDS_SetError("Programming error--invalid file mode (SDDS_InitializeCopy)");
    return(0);
  }

  SDDS_target->layout.popenUsed = 0;
  SDDS_target->layout.gzipFile = 0;
  if (filename) {
    if (SDDS_FileIsLocked(filename)) {
      sprintf(s, "unable to open file %s for copy--file is locked (SDDS_InitializeCopy)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!filemode || !(SDDS_target->layout.fp=fopen(filename, filemode))) {
      sprintf(s, "Unable to open file %s (SDDS_InitializeCopy)", filename);
      SDDS_SetError(s);
      return(0);
    }
    if ((strcmp(filemode, "w")==0 || strcmp(filemode, "wb")==0) && 
	!SDDS_LockFile(SDDS_target->layout.fp, filename, "SDDS_InitializeCopy"))
      return 0;
    if (!SDDS_CopyString(&SDDS_target->layout.filename, filename)) {
      SDDS_SetError("Memory allocation failure (SDDS_InitializeCopy)");
      return(0);
    }
#if defined(zLib)
    if ((extension = strrchr(filename, '.')) && strcmp(extension, ".gz")==0) {
      SDDS_target->layout.gzipFile = 1;
      if ((SDDS_target->layout.gzfp=gzdopen(fileno(SDDS_target->layout.fp), filemode)) == NULL) {
	sprintf(s, "Unable to open compressed file %s for writing (SDDS_InitializeCopy)", filename);
	SDDS_SetError(s);
	return 0;
      }
    }
#endif
  }
  else {
    SDDS_target->layout.filename = NULL;
    SDDS_target->layout.fp = NULL;
    SDDS_target->mode=SDDS_MEMMODE;
    if (filemode) {
      if (strcmp(filemode, "w")==0 || strcmp(filemode, "wb")==0) 
        SDDS_target->layout.fp = stdout;
      else if (strcmp(filemode, "r")==0 || strcmp(filemode, "rb")==0)
        SDDS_target->layout.fp = stdin;
      
      /*      else if (strcmp(filemode, "m")!=0) {
        SDDS_SetError("Unknown filemode (SDDS_InitializeCopy)");
        return(0);
	}*/
#if defined(_WIN32)
      if (strcmp(filemode, "m")!=0) {
	if (_setmode(_fileno(SDDS_target->layout.fp), _O_BINARY) == -1) {
	  sprintf(s, "unable to set stdout or stdin to binary mode");
	  SDDS_SetError(s);
	  return 0;
	}      
      }
#endif

    }
  }
  SDDS_target->page_number =SDDS_target->page_started= 0;
  if (!SDDS_CopyLayout(SDDS_target, SDDS_source))
    return(0);
  return(1);
}

int32_t SDDS_AppendLayout(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source, uint32_t mode)
{
  SDDS_LAYOUT *target, *source;
  int32_t i;

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_AppendLayout"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_AppendLayout"))
    return(0);
  target = &SDDS_target->layout;
  source = &SDDS_source->layout;
  SDDS_DeferSavingLayout(1);

  for (i=0; i<source->n_columns; i++)
    if (SDDS_GetColumnIndex(SDDS_target, source->column_definition[i].name)<0 &&
        SDDS_DefineColumn(SDDS_target, source->column_definition[i].name, 
                          source->column_definition[i].symbol,
                          source->column_definition[i].units, source->column_definition[i].description,
                          source->column_definition[i].format_string, source->column_definition[i].type,
                          source->column_definition[i].field_length)<0) {
      SDDS_DeferSavingLayout(0);
      SDDS_SetError("Unable to define column (SDDS_AppendLayout)");
      return(0);
    }

  for (i=0; i<source->n_parameters; i++)
    if (SDDS_GetParameterIndex(SDDS_target, source->parameter_definition[i].name)<0 &&
        SDDS_DefineParameter(SDDS_target, source->parameter_definition[i].name, 
                             source->parameter_definition[i].symbol,
                             source->parameter_definition[i].units, 
                             source->parameter_definition[i].description,
                             source->parameter_definition[i].format_string, 
                             source->parameter_definition[i].type,
                             source->parameter_definition[i].fixed_value)<0) {
      SDDS_DeferSavingLayout(0);
      SDDS_SetError("Unable to define parameter (SDDS_AppendLayout)");
      return(0);
    }
  
  for (i=0; i<source->n_associates; i++) 
    if (SDDS_GetAssociateIndex(SDDS_target,source->associate_definition[i].name)<0 &&
        SDDS_DefineAssociate(SDDS_target, source->associate_definition[i].name, 
                             source->associate_definition[i].filename, 
                             source->associate_definition[i].path,
                             source->associate_definition[i].description,
                             source->associate_definition[i].contents, 
                             source->associate_definition[i].sdds)<0) {
      SDDS_DeferSavingLayout(0);
      SDDS_SetError("Unable to define associate (SDDS_AppendLayout)");
      return(0);
    }
  
  for (i=0; i<source->n_arrays; i++)
    if (SDDS_GetArrayIndex(SDDS_target, source->array_definition[i].name)<0 &&
        SDDS_DefineArray(SDDS_target, source->array_definition[i].name,
                         source->array_definition[i].symbol,
                         source->array_definition[i].units, source->array_definition[i].description,
                         source->array_definition[i].format_string, source->array_definition[i].type,
                         source->array_definition[i].field_length, 
                         source->array_definition[i].dimensions,
                         source->array_definition[i].group_name)<0) {
      SDDS_DeferSavingLayout(0);
      SDDS_SetError("Unable to define array (SDDS_AppendLayout)");
      return(0);
    }
  SDDS_DeferSavingLayout(0);
  if (!SDDS_SaveLayout(SDDS_target))  {
    SDDS_SetError("Unable to save layout (SDDS_AppendLayout)");
    return(0);
  }
  return(1);
}


int32_t SDDS_CopyLayout(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  SDDS_LAYOUT *target, *source;
  int32_t i;

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_CopyLayout"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_CopyLayout"))
    return(0);
  target = &SDDS_target->layout;
  source = &SDDS_source->layout;
  target->version = source->version;
  target->data_mode = source->data_mode;
  target->data_mode.no_row_counts = 0;
  target->data_mode.fixed_row_count = 0;
  target->layout_written = 0;
  target->byteOrderDeclared = 0;
  if (source->description)
    SDDS_CopyString(&target->description, source->description);
  if (source->contents)
    SDDS_CopyString(&target->contents, source->contents);        
  SDDS_DeferSavingLayout(1);
  for (i=0; i<source->n_columns; i++)
    if (SDDS_DefineColumn(SDDS_target, source->column_definition[i].name, source->column_definition[i].symbol,
                          source->column_definition[i].units, source->column_definition[i].description,
                          source->column_definition[i].format_string, source->column_definition[i].type,
                          source->column_definition[i].field_length)<0) {
      SDDS_SetError("Unable to define column (SDDS_CopyLayout)");
      return(0);
    }
  for (i=0; i<source->n_parameters; i++)
    if (SDDS_DefineParameter(SDDS_target, source->parameter_definition[i].name, source->parameter_definition[i].symbol,
                             source->parameter_definition[i].units, source->parameter_definition[i].description,
                             source->parameter_definition[i].format_string, source->parameter_definition[i].type,
                             source->parameter_definition[i].fixed_value)<0) {
      SDDS_SetError("Unable to define parameter (SDDS_CopyLayout)");
      return(0);
    }
  
  for (i=0; i<source->n_associates; i++) 
    if (SDDS_DefineAssociate(SDDS_target, source->associate_definition[i].name, 
                             source->associate_definition[i].filename, 
                             source->associate_definition[i].path,
                             source->associate_definition[i].description,
                             source->associate_definition[i].contents, source->associate_definition[i].sdds)<0) {
      SDDS_SetError("Unable to define associate (SDDS_CopyLayout)");
      return(0);
    }
  
  for (i=0; i<source->n_arrays; i++)
    if (SDDS_DefineArray(SDDS_target, source->array_definition[i].name, source->array_definition[i].symbol,
                         source->array_definition[i].units, source->array_definition[i].description,
                         source->array_definition[i].format_string, source->array_definition[i].type,
                         source->array_definition[i].field_length, source->array_definition[i].dimensions,
                         source->array_definition[i].group_name)<0) {
      SDDS_SetError("Unable to define array (SDDS_CopyLayout)");
      return(0);
    }
  SDDS_DeferSavingLayout(0);
  if (!SDDS_SaveLayout(SDDS_target))  {
    SDDS_SetError("Unable to save layout (SDDS_CopyLayout)");
    return(0);
  }
  return(1);
}

int32_t SDDS_CopyParameters(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  int32_t i, target_index;
  static char *buffer = NULL; /* will be sized to hold any SDDS data type with room to spare */
  char messageBuffer[1024];
  
  if (!buffer && !(buffer=SDDS_Malloc(sizeof(char)*16))) {
    SDDS_SetError("Allocation failure (SDDS_CopyParameters)");
    return 0;
  }

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_CopyParameters"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_CopyParameters"))
    return(0);

  for (i=0; i<SDDS_source->layout.n_parameters; i++) {
    if ((target_index = SDDS_GetParameterIndex(SDDS_target, SDDS_source->layout.parameter_definition[i].name))<0)
      continue;
    if (SDDS_source->layout.parameter_definition[i].type!=
        SDDS_target->layout.parameter_definition[target_index].type) {
      if (!SDDS_NUMERIC_TYPE(SDDS_source->layout.parameter_definition[i].type) ||
          !SDDS_NUMERIC_TYPE(SDDS_target->layout.parameter_definition[target_index].type)) {
        sprintf(messageBuffer, 
                "Can't cast between nonnumeric types for parameters %s and %s (SDDS_CopyParameters)", 
                SDDS_source->layout.parameter_definition[i].name, 
                SDDS_target->layout.parameter_definition[target_index].name);
        SDDS_SetError(messageBuffer);
        return 0;
      }
      if (!SDDS_SetParameters(SDDS_target, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, target_index,
                              SDDS_CastValue(SDDS_source->parameter[i], 0, 
                                             SDDS_source->layout.parameter_definition[i].type,
                                             SDDS_target->layout.parameter_definition[target_index].type,
                                             buffer), -1)) {
        sprintf(messageBuffer, 
                "Error setting parameter with cast value for parameters %s and %s (SDDS_CopyParameters)", 
                SDDS_source->layout.parameter_definition[i].name, 
                SDDS_target->layout.parameter_definition[target_index].name);
        SDDS_SetError(messageBuffer);
        return 0;
      }
    }
    else if (!SDDS_SetParameters(SDDS_target, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, target_index,
                                 SDDS_source->parameter[i], -1)) {
      sprintf(messageBuffer, 
              "Unable to copy parameters for parameters %s and %s (SDDS_CopyParameters)",
              SDDS_source->layout.parameter_definition[i].name, 
              SDDS_target->layout.parameter_definition[target_index].name);
      SDDS_SetError(messageBuffer);
      return(0);
    }
  }
  return(1);
}

int32_t SDDS_CopyArrays(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  int32_t i, j, target_index;
  char messageBuffer[1024];
  
  for (i=0; i<SDDS_source->layout.n_arrays; i++) {
    if ((target_index=SDDS_GetArrayIndex(SDDS_target, SDDS_source->layout.array_definition[i].name))<0)
      continue;
    SDDS_target->array[target_index].definition = SDDS_target->layout.array_definition+target_index;
    SDDS_target->array[target_index].elements = SDDS_source->array[i].elements;
    if (!(SDDS_target->array[target_index].dimension 
          = (int32_t*)SDDS_Malloc(sizeof(*SDDS_target->array[i].dimension)*SDDS_target->array[target_index].definition->dimensions)) ||
        !(SDDS_target->array[target_index].data 
          = SDDS_Realloc(SDDS_target->array[target_index].data, 
                         SDDS_type_size[SDDS_target->array[target_index].definition->type-1]*
                         SDDS_target->array[target_index].elements))) {
      SDDS_SetError("Unable to copy arrays--allocation failure (SDDS_CopyArrays)");
      return(0);
    }
    
    for (j=0; j<SDDS_target->array[target_index].definition->dimensions; j++)
      SDDS_target->array[target_index].dimension[j] = SDDS_source->array[i].dimension[j];
    if (!SDDS_source->array[i].data) {
      SDDS_target->array[target_index].data=NULL;
      continue;
    }
    if (SDDS_source->layout.array_definition[i].type!=SDDS_target->layout.array_definition[target_index].type) {
      if (!SDDS_NUMERIC_TYPE(SDDS_source->layout.array_definition[i].type) ||
          !SDDS_NUMERIC_TYPE(SDDS_target->layout.array_definition[target_index].type)) {
        sprintf(messageBuffer, 
                "Can't cast between nonnumeric types for parameters %s and %s (SDDS_CopyArrays)", 
                SDDS_source->layout.array_definition[i].name, 
                SDDS_target->layout.array_definition[target_index].name);
        SDDS_SetError(messageBuffer);
        return 0;
      }
      for (j=0;j<SDDS_source->array[i].elements;j++) {
        if (!SDDS_CastValue(SDDS_source->array[i].data, j, 
                            SDDS_source->layout.array_definition[i].type, 
                            SDDS_target->layout.array_definition[target_index].type,
                            (char*)(SDDS_target->array[target_index].data)
                            +j*SDDS_type_size[SDDS_target->layout.array_definition[target_index].type-1])) {
          SDDS_SetError("Problem with cast (SDDS_CopyArrays)");
          return 0;
        }
      }
    } else {
      if (SDDS_target->array[target_index].definition->type!=SDDS_STRING)
        memcpy(SDDS_target->array[target_index].data, SDDS_source->array[i].data, 
               SDDS_type_size[SDDS_target->array[target_index].definition->type-1]*SDDS_target->array[target_index].elements);
      else if (!SDDS_CopyStringArray(SDDS_target->array[target_index].data, SDDS_source->array[i].data, 
                                     SDDS_target->array[target_index].elements)) {
        SDDS_SetError("Unable to copy arrays (SDDS_CopyArrays)");
        return(0);
      }
    }
  }
  return(1);
}


int32_t SDDS_CopyColumns(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  int32_t i, j, target_index;
  SDDS_target->n_rows = 0;
  if (SDDS_target->layout.n_columns && 
      SDDS_target->n_rows_allocated<SDDS_source->n_rows) {
    SDDS_SetError("Unable to copy columns--insufficient memory allocated to target table");
    return(0);
  }
  if (!SDDS_target->layout.n_columns)
    return 1;
  for (i=0; i<SDDS_source->layout.n_columns; i++) {
    if ((target_index=SDDS_GetColumnIndex(SDDS_target, SDDS_source->layout.column_definition[i].name))<0)
      continue;
    if (SDDS_source->layout.column_definition[i].type!=SDDS_STRING) {
      if (SDDS_source->layout.column_definition[i].type==
          SDDS_target->layout.column_definition[target_index].type)
        memcpy(SDDS_target->data[target_index], SDDS_source->data[i], 
               SDDS_type_size[SDDS_source->layout.column_definition[i].type-1]*SDDS_source->n_rows);
      else {
        /* Do a cast between the source and target types, if they are both numeric */
        if (!SDDS_NUMERIC_TYPE(SDDS_source->layout.column_definition[i].type) ||
            !SDDS_NUMERIC_TYPE(SDDS_target->layout.column_definition[target_index].type)) {
          SDDS_SetError("Can't cast between nonnumeric types (SDDS_CopyColumns)");
          return 0;
        }
        for (j=0; j<SDDS_source->n_rows; j++) {
          if (!SDDS_CastValue(SDDS_source->data[i], j, 
                              SDDS_source->layout.column_definition[i].type, 
                              SDDS_target->layout.column_definition[target_index].type,
                              (char*)(SDDS_target->data[target_index])
                              +j*SDDS_type_size[SDDS_target->layout.column_definition[target_index].type-1])) {
            SDDS_SetError("Problem with cast (SDDS_CopyColumns)");
            return 0;
          }
        }
      }
    }
    else if (!SDDS_CopyStringArray(SDDS_target->data[target_index], SDDS_source->data[i], SDDS_source->n_rows)) {
      SDDS_SetError("Unable to copy columns (SDDS_CopyColumns)");
      return(0);
    }
    SDDS_target->column_flag[target_index] = 1;
    SDDS_target->column_order[target_index] = target_index;
  } 
  SDDS_target->n_rows = SDDS_source->n_rows;
  if (SDDS_target->row_flag)
    for (i=0; i<SDDS_target->n_rows; i++)
      SDDS_target->row_flag[i] = 1;
  return(1);
}

int32_t SDDS_CopyRowsOfInterest(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  int32_t i, j, k, target_index, size;
  /*  int32_t rows;*/
  char buffer[1024];
  int32_t *rowList, roi;
  k = 0;

  if (!SDDS_target->layout.n_columns)
    return 1;
  roi = SDDS_CountRowsOfInterest(SDDS_source);
  if (roi > SDDS_target->n_rows_allocated) {
    SDDS_SetError("Unable to copy rows of interest--insufficient memory allocated to target page (SDDS_CopyRowsOfInterest)");
    return 0;
  }

  rowList = malloc(sizeof(*rowList) * roi);
  k=0;
  for (j=0; j<SDDS_source->n_rows; j++) {
    if (SDDS_source->row_flag[j]) {
      rowList[k] = j;
      k++;
    }
  }

  for (i=0; i<SDDS_source->layout.n_columns; i++) {
    if ((target_index=SDDS_GetColumnIndex(SDDS_target, SDDS_source->layout.column_definition[i].name))<0)
      continue;
    if (SDDS_source->layout.column_definition[i].type!=SDDS_STRING) {
      if (SDDS_source->layout.column_definition[i].type==SDDS_target->layout.column_definition[target_index].type) {
        size = SDDS_type_size[SDDS_source->layout.column_definition[i].type-1];
        for (k=0; k<roi; k++) {
          memcpy((char*)SDDS_target->data[target_index]+k*size,
                 (char*)SDDS_source->data[i]+rowList[k]*size,
                 SDDS_type_size[SDDS_source->layout.column_definition[i].type-1]);
        }
      } else {
        for (k=0; k<roi; k++) {
          if (!SDDS_CastValue(SDDS_source->data[i], rowList[k], 
                              SDDS_source->layout.column_definition[i].type, 
                              SDDS_target->layout.column_definition[target_index].type,
                              (char*)(SDDS_target->data[target_index])
                              +k*SDDS_type_size[SDDS_target->layout.column_definition[target_index].type-1])) {
            sprintf(buffer, "Problem with cast for column %s (SDDS_CopyRowsOfInterest)", 
                    SDDS_source->layout.column_definition[i].name);
            SDDS_SetError(buffer);
            return 0;
          }
        }
      }
    }
    else {
      if (SDDS_source->layout.column_definition[i].type!=SDDS_target->layout.column_definition[target_index].type) {
          sprintf(buffer, "Unable to copy columns---inconsistent data types for %s (SDDS_CopyRowsOfInterest)", 
              SDDS_source->layout.column_definition[i].name);
          SDDS_SetError(buffer);
          return(0);
        }
      for (k=0; k<roi; k++) {
        if (((char **)SDDS_target->data[target_index])[k])
          free(((char **)SDDS_target->data[target_index])[k]);
        if (!SDDS_CopyString(&((char **)SDDS_target->data[target_index])[k],
                             ((char **)SDDS_source->data[i])[rowList[k]])) {
          SDDS_SetError("Unable to copy rows (SDDS_CopyRowsOfInterest)");
          return(0);
        }
      }
    }
    SDDS_target->column_flag[target_index] = 1;
    SDDS_target->column_order[target_index] = target_index;
  }
  free(rowList);
  SDDS_target->n_rows = k;
  if (SDDS_target->row_flag)
    for (i=0; i<SDDS_target->n_rows; i++)
      SDDS_target->row_flag[i] = 1;

  return(1);
}

int32_t SDDS_CopyAdditionalRows(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  int32_t i, j, target_index, size, sum;
  char buffer[1024];
  
  if (SDDS_target->n_rows_allocated<(sum=SDDS_target->n_rows+SDDS_source->n_rows) && 
      !SDDS_LengthenTable(SDDS_target, sum-SDDS_target->n_rows_allocated)) {
    SDDS_SetError("Unable to copy additional rows (SDDS_CopyAdditionalRows)");
    return(0);
  }
  if (SDDS_target->layout.n_columns==0)
    return 1;
  for (i=0; i<SDDS_source->layout.n_columns; i++) {
    if ((target_index=SDDS_GetColumnIndex(SDDS_target, SDDS_source->layout.column_definition[i].name))<0)
      continue;
    size = SDDS_GetTypeSize(SDDS_source->layout.column_definition[i].type);
    if (SDDS_source->layout.column_definition[i].type!=SDDS_STRING) {
      if (SDDS_source->layout.column_definition[i].type==SDDS_target->layout.column_definition[target_index].type) {
        memcpy((char*)SDDS_target->data[target_index]+size*SDDS_target->n_rows, SDDS_source->data[i], 
               SDDS_type_size[SDDS_source->layout.column_definition[i].type-1]*SDDS_source->n_rows);
      } else {
        for (j=0; j<SDDS_source->n_rows; j++) {
          if (!SDDS_CastValue(SDDS_source->data[i], j,
                           SDDS_source->layout.column_definition[i].type,
                           SDDS_target->layout.column_definition[target_index].type,
                           (char*)(SDDS_target->data[target_index])
                           +(j+SDDS_target->n_rows)*SDDS_type_size[SDDS_target->layout.column_definition[target_index].type-1])) {
            sprintf(buffer, "Problem with cast for column %s (SDDS_CopyAdditionalRows)",
                    SDDS_source->layout.column_definition[i].name);
            SDDS_SetError(buffer);
            return 0;
          }
        }
      }
    } else {
      if (SDDS_source->layout.column_definition[i].type!=SDDS_target->layout.column_definition[target_index].type) {
        sprintf(buffer, "Unable to copy columns---inconsistent data types for %s (SDDS_CopyAdditionalRows)", 
                SDDS_source->layout.column_definition[i].name);
        SDDS_SetError(buffer);
        return(0);
      }
      if (!SDDS_CopyStringArray((char**)((char*)SDDS_target->data[target_index]+size*SDDS_target->n_rows),
                                SDDS_source->data[i], SDDS_source->n_rows)) {
        SDDS_SetError("Unable to copy columns (SDDS_CopyAdditionalRows)");
        return(0);
      }
    }
    SDDS_target->column_flag[target_index] = 1;
    SDDS_target->column_order[target_index] = target_index;
  }
  SDDS_target->n_rows += SDDS_source->n_rows;
  if (SDDS_target->row_flag)
    for (i=0; i<SDDS_target->n_rows; i++)
      SDDS_target->row_flag[i] = 1;

  return(1);
}

int32_t SDDS_CopyPage(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source)
{
  /*  int32_t i, index, column_count, j;*/
  /*  SDDS_LAYOUT target_layout, source_layout;*/
  if (!SDDS_CheckDataset(SDDS_target, "SDDS_CopyPage"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_CopyPage"))
    return(0);

  if (!SDDS_StartPage(SDDS_target, SDDS_target->layout.n_columns?SDDS_source->n_rows:0)) {
    SDDS_SetError("Unable to copy page (SDDS_CopyPage)");
    return(0);
  }
  if (!SDDS_CopyParameters(SDDS_target, SDDS_source))
    return(0);
  if (!SDDS_CopyArrays(SDDS_target, SDDS_source))
    return(0);
  if (!SDDS_CopyColumns(SDDS_target, SDDS_source))
    return(0);
  return(1);
}


static int32_t deferSavingLayout = 0;

void SDDS_DeferSavingLayout(int32_t mode)
{
  deferSavingLayout = mode;
}

int32_t SDDS_SaveLayout(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *source, *target;

  if (deferSavingLayout)
    return 1;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SaveLayout"))
    return(0);

  if ((source = &SDDS_dataset->layout)==(target = &SDDS_dataset->original_layout)) {
    SDDS_SetError("\"original\" and working page layouts share memory!");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }

  /* copy pointer elements of structure into new memory */
  if (source->n_columns) {
    if (!(target->column_definition =
          (COLUMN_DEFINITION*)SDDS_Realloc((void*)target->column_definition, 
                                           sizeof(COLUMN_DEFINITION)*source->n_columns)) ||
        !(target->column_index = 
          (SORTED_INDEX**)SDDS_Realloc((void*)target->column_index,
                                      sizeof(SORTED_INDEX*)*source->n_columns))) {
      SDDS_SetError("Unable to save layout--allocation failure (SDDS_SaveLayout)");
      return(0);
    }
    memcpy((char*)target->column_definition, (char*)source->column_definition, 
           sizeof(COLUMN_DEFINITION)*source->n_columns);
    memcpy((char*)target->column_index, (char*)source->column_index, 
           sizeof(SORTED_INDEX*)*source->n_columns);
  }
  if (source->n_parameters) {
    if (!(target->parameter_definition =
          (PARAMETER_DEFINITION*)SDDS_Realloc((void*)target->parameter_definition, 
                                              sizeof(PARAMETER_DEFINITION)*source->n_parameters)) || 
        !(target->parameter_index = 
          (SORTED_INDEX**)SDDS_Realloc((void*)target->parameter_index,
                                      sizeof(SORTED_INDEX*)*source->n_parameters))) {
      SDDS_SetError("Unable to save layout--allocation failure (SDDS_SaveLayout)");
      return(0);
    }
    memcpy((char*)target->parameter_definition, (char*)source->parameter_definition, 
           sizeof(PARAMETER_DEFINITION)*source->n_parameters);
    memcpy((char*)target->parameter_index, (char*)source->parameter_index, 
           sizeof(SORTED_INDEX*)*source->n_parameters);
  }
  if (source->n_arrays) {
    if (!(target->array_definition =
          (ARRAY_DEFINITION*)SDDS_Realloc((void*)target->array_definition, 
                                          sizeof(ARRAY_DEFINITION)*source->n_arrays)) ||
        !(target->array_index = 
          (SORTED_INDEX**)SDDS_Realloc((void*)target->array_index,
                                      sizeof(SORTED_INDEX*)*source->n_arrays))) {
      SDDS_SetError("Unable to save layout--allocation failure (SDDS_SaveLayout)");
      return(0);
    }
    memcpy((char*)target->array_definition, (char*)source->array_definition, sizeof(ARRAY_DEFINITION)*source->n_arrays);
    memcpy((char*)target->array_index, (char*)source->array_index, sizeof(SORTED_INDEX*)*source->n_arrays);
  }
  if (source->n_associates) {
    if (!(target->associate_definition =
          (ASSOCIATE_DEFINITION*)SDDS_Realloc((void*)target->associate_definition, 
                                              sizeof(ASSOCIATE_DEFINITION)*source->n_associates))) {
      SDDS_SetError("Unable to save layout--allocation failure (SDDS_SaveLayout)");
      return(0);
    }
    memcpy((char*)target->associate_definition, (char*)source->associate_definition, 
           sizeof(ASSOCIATE_DEFINITION)*source->n_associates);
  }

  target->n_columns = source->n_columns;
  target->n_parameters = source->n_parameters;
  target->n_associates = source->n_associates;
  target->n_arrays = source->n_arrays;
  target->description = source->description;
  target->contents = source->contents;
  target->version = source->version;
  target->data_mode = source->data_mode;
  target->filename = source->filename;
  target->fp = source->fp;
  target->popenUsed = source->popenUsed;

  return(1);
}

int32_t SDDS_RestoreLayout(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *source, *target;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_RestoreLayout"))
    return(0);

  source = &SDDS_dataset->original_layout;
  target = &SDDS_dataset->layout;

  /* copy pointer elements of structure into new memory */
  if (source->n_columns) {
    if (target->column_definition==source->column_definition) {
      SDDS_SetError("Unable to restore layout--column definition pointers are the same  (SDDS_RestoreLayout)");
      return(0);
    }
    if (!(target->column_definition =
          (COLUMN_DEFINITION*)SDDS_Realloc((void*)target->column_definition, 
                                           sizeof(COLUMN_DEFINITION)*source->n_columns))) {
      SDDS_SetError("Unable to restore layout--allocation failure (SDDS_RestoreLayout)");
      return(0);
    }
    memcpy((char*)target->column_definition, (char*)source->column_definition, sizeof(COLUMN_DEFINITION)*source->n_columns);
  }
  if (source->n_parameters) {
    if (target->parameter_definition==source->parameter_definition) {
      SDDS_SetError("Unable to restore layout--parameter definition pointers are the same  (SDDS_RestoreLayout)");
      return(0);
    }
    if (!(target->parameter_definition =
          (PARAMETER_DEFINITION*)SDDS_Realloc((void*)target->parameter_definition, 
                                              sizeof(PARAMETER_DEFINITION)*source->n_parameters))) {
      SDDS_SetError("Unable to restore layout--allocation failure (SDDS_RestoreLayout)");
      return(0);
    }
    memcpy((char*)target->parameter_definition, (char*)source->parameter_definition, 
           sizeof(PARAMETER_DEFINITION)*source->n_parameters);
  }
  if (source->n_arrays) {
    if (target->array_definition==source->array_definition) {
      SDDS_SetError("Unable to restore layout--array definition pointers are the same  (SDDS_RestoreLayout)");
      return(0);
    }
    if (!(target->array_definition =
          (ARRAY_DEFINITION*)SDDS_Realloc((void*)target->array_definition, 
                                          sizeof(ARRAY_DEFINITION)*source->n_arrays))) {
      SDDS_SetError("Unable to restore layout--allocation failure (SDDS_RestoreLayout)");
      return(0);
    }
    memcpy((char*)target->array_definition, (char*)source->array_definition, sizeof(ARRAY_DEFINITION)*source->n_arrays);
  }
  if (source->n_associates) {
    if (target->associate_definition==source->associate_definition) {
      SDDS_SetError("Unable to restore layout--associate definition pointers are the same  (SDDS_RestoreLayout)");
      return(0);
    }
    if (!(target->associate_definition =
          (ASSOCIATE_DEFINITION*)SDDS_Realloc((void*)target->associate_definition, 
                                              sizeof(ASSOCIATE_DEFINITION)*source->n_associates))) {
      SDDS_SetError("Unable to restore layout--allocation failure (SDDS_RestoreLayout)");
      return(0);
    }
    memcpy((char*)target->associate_definition, (char*)source->associate_definition, 
           sizeof(ASSOCIATE_DEFINITION)*source->n_associates);
  }

  target->n_columns = source->n_columns;
  target->n_parameters = source->n_parameters;
  target->n_associates = source->n_associates;
  target->n_arrays = source->n_arrays;
  target->description = source->description;
  target->contents = source->contents;
  target->version = source->version;
  target->data_mode = source->data_mode;
  target->filename = source->filename;
  target->fp = source->fp;
  
  return(1);
}


int32_t SDDS_CopyRow(SDDS_DATASET *SDDS_target, int32_t target_row, SDDS_DATASET *SDDS_source, int32_t source_srow)
{
  int32_t i, j, source_row, size, type;

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_CopyRow"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_CopyRow"))
    return(0);
  
  if (target_row>=SDDS_target->n_rows_allocated) {
    SDDS_SetError("Unable to copy row--target page not large enough");
    return(0);
  }
  if (SDDS_target->n_rows<=target_row)
    SDDS_target->n_rows = target_row+1;

  source_row = -1;
  for (i=j=0; i<SDDS_source->n_rows; i++)
    if (SDDS_source->row_flag[i] && j++==source_srow) {
      source_row = i;
      break;
    }

  if (source_row==-1) {
    SDDS_SetError("Unable to copy row--source selected-row does not exist");
    return(0);
  }

  for (i=0; i<SDDS_target->layout.n_columns; i++) {
    if ((j=SDDS_GetColumnIndex(SDDS_source, SDDS_target->layout.column_definition[i].name))<0 ||
        !SDDS_source->column_flag[j])
      continue;
    if ((type=SDDS_GetColumnType(SDDS_target, i))==SDDS_STRING) {
      if (!SDDS_CopyString(((char***)SDDS_target->data)[i]+target_row, ((char***)SDDS_source->data)[j][source_row])) {
        SDDS_SetError("Unable to copy row--string copy failed (SDDS_CopyRow)");
        return(0);
      }
    }
    else {
      size = SDDS_type_size[type-1];
      memcpy((char*)SDDS_target->data[i]+size*target_row, (char*)SDDS_source->data[j]+size*source_row, size);
    }
    SDDS_target->row_flag[target_row] = 1;
  }
  return(1);
}

int32_t SDDS_CopyRowDirect(SDDS_DATASET *SDDS_target, int32_t target_row, SDDS_DATASET *SDDS_source, int32_t source_row)
{
  int32_t i, j, size, type;

  if (!SDDS_CheckDataset(SDDS_target, "SDDS_CopyRow"))
    return(0);
  if (!SDDS_CheckDataset(SDDS_source, "SDDS_CopyRow"))
    return(0);
  
  if (target_row>=SDDS_target->n_rows_allocated) {
    SDDS_SetError("Unable to copy row--target page not large enough");
    return(0);
  }
  if (SDDS_target->n_rows<=target_row)
    SDDS_target->n_rows = target_row+1;
  if (source_row>=SDDS_source->n_rows_allocated) {
    SDDS_SetError("Unable to copy row--source row non-existent");
    return(0);
  }

  for (i=0; i<SDDS_target->layout.n_columns; i++) {
    if ((j=SDDS_GetColumnIndex(SDDS_source, SDDS_target->layout.column_definition[i].name))<0 ||
        !SDDS_source->column_flag[j])
      continue;
    if ((type=SDDS_GetColumnType(SDDS_target, i))==SDDS_STRING) {
      if (!SDDS_CopyString(((char***)SDDS_target->data)[i]+target_row, ((char***)SDDS_source->data)[j][source_row])) {
        SDDS_SetError("Unable to copy row--string copy failed (SDDS_CopyRow)");
        return(0);
      }
    }
    else {
      size = SDDS_type_size[type-1];
      memcpy((char*)SDDS_target->data[i]+size*target_row, (char*)SDDS_source->data[j]+size*source_row, size);
    }
    SDDS_target->row_flag[target_row] = 1;
  }
  return(1);
}
