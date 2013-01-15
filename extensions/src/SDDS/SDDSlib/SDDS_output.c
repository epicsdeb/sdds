/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_output.c
 * contents: user's SDDS routines for data set setup and output, SDDS version 1
 *
 * M. Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.58  2009/03/12 15:30:45  shang
 included SDDS_MPI_DisconnectFile into SDDS_DisconnectFile and SDDS_MPI_ReconnectFile into SDDS_ReconnectFile.

 Revision 1.57  2008/04/14 18:27:26  shang
 added parallel SDDS.

 Revision 1.56  2008/03/18 18:59:47  soliday
 Fixed a bug when appending to a non-native endian file.

 Revision 1.55  2007/09/26 20:32:14  shang
 added SDDS_Parallel_InitializeOutput

 Revision 1.54  2006/12/14 22:12:02  soliday
 Added a call to SDDS_SaveLayout in SDDS_WriteLayout. Removed calls to
 SDDS_SaveLayout from procedures that define parameters, columns and arrays.
 This will speed up the code.

 Revision 1.53  2006/08/31 15:08:02  soliday
 Updated to work with SDDS2

 Revision 1.52  2005/11/04 22:46:52  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.51  2005/01/13 16:47:59  shang
 added string support to rpn functions

 Revision 1.50  2004/11/15 17:01:35  soliday
 Updated so that the endian type can be set to big or little by
 using the environment variable SDDS_OUTPUT_ENDIANESS

 Revision 1.49  2003/04/27 18:31:55  borland
 Fixed problems with recent changes to SDDS_DoFSync:
 1. Now returns 1 (not 0) on Windows, VxWorks, and Apple, without doing
  anything.
 2. Linux platforms try to FSync.

 Revision 1.48  2003/02/19 18:03:26  soliday
 Fixed problem with last change on Windows

 Revision 1.47  2003/02/15 22:54:42  borland
 Added SDDS_DoFSync().

 Revision 1.46  2002/08/14 16:34:49  soliday
 Added Open License

 Revision 1.45  2002/08/08 17:21:32  soliday
 Added support for OSX.

 Revision 1.44  2002/07/01 19:49:26  soliday
 fsync is no longer the default mode because of performance issues.

 Revision 1.43  2002/05/26 02:06:38  borland
 1. Call SDDS_SaveLayout() after changing the row count mode, just in case the
 user doesn't do this.  Otherwise, the row count mode gets restored in SDDS_StartPage.
 2. Set new autoRecovered flag whenever auto read recovery is done.  This prevents
 trying to read any more data, which might happen if the file is updated between
 two reads.

 Revision 1.42  2002/02/18 19:30:28  soliday
 Added the ability to disable the fsync command.

 Revision 1.41  2002/01/22 20:39:33  soliday
 If fsync reports an error it is now ignored because this is not fatal.

 Revision 1.40  2002/01/15 22:50:45  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.39  2002/01/11 22:31:35  soliday
 Commited versions for Shang.

 Revision 1.38  2002/01/11 20:16:53  soliday
 Added SDDS_SyncDataSet so that flushed data can bypass the system buffer.

 Revision 1.37  2002/01/03 17:40:28  soliday
 Fixed RW_ASSOCIATES problem.

 Revision 1.36  2001/11/30 15:34:37  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.35  2001/10/02 15:35:25  borland
 Fixed problem with SDDS_InitializeAppendToPage() resulting from changes
 in SDDS_UpdatePage().

 Revision 1.34  2001/08/07 14:56:53  borland
 Fixed some file-disconnect-related bugs.

 Revision 1.33  2001/06/19 15:18:09  soliday
 Fixed endian problem with gzip files.

 Revision 1.32  2000/09/19 20:13:23  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.31  2000/06/20 18:04:03  borland
 Added SDDS_SetDefaultIOBufferSize and modified binary read/write
 routines to optionally allow unbuffered read/write.  Removed
 SDDS_SetBufferedRead routine.

 Revision 1.30  2000/06/15 21:06:50  soliday
 Removed compiler warnings on vxWorks.

 Revision 1.29  2000/04/14 16:38:53  soliday
 Now checks to make sure the gzFile is the same size as a void*

 Revision 1.28  2000/04/06 16:00:00  soliday
 Added support for Borland C.

 Revision 1.27  2000/03/30 19:55:10  borland
 Added SDDS_DisconnectFile() and SDDS_ReconnectFile() plus changes to
 support this function.

 Revision 1.26  2000/03/30 02:35:05  borland
 Added SDDS_InitializeAppendToPage().

 Revision 1.25  2000/03/28 15:48:30  borland
 Added page update capability for ASCII files.

 Revision 1.24  2000/01/18 20:14:47  soliday
 Added support for ZLIB.

 Revision 1.23  1999/08/10 15:09:54  soliday
 Changed stdout to binary mode for WIN32

 Revision 1.22  1999/07/22 15:37:02  soliday
 Uses macros for fopen mode.

 Revision 1.21  1999/07/06 21:36:01  soliday
 Added ctype.h include line

 Revision 1.20  1999/06/01 19:10:04  borland
 Reorgnized a logic statement to make it more transparent.

 Revision 1.19  1999/05/25 18:58:04  soliday
 Removed compiler warning on linux.

 Revision 1.18  1999/04/14 13:58:05  borland
 Fixed some function types and returns.

 Revision 1.17  1998/12/07 14:46:03  borland
 Fixed a typo in previous mod.

 Revision 1.16  1998/12/07 14:45:24  borland
 Added [ and ] to the list of valid characters in an element name.

 Revision 1.15  1997/12/19 16:58:40  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.14  1996/07/05 15:36:44  borland
 Modified SDDS_SprintTypedValue and SDDS_PrintTypedValue to
 include mode argument; now support omission of quotes around
 string data.  SDDS_PrintTypedValue now calls SDDS_SprintTypedValue
 to format string data.

 * Revision 1.13  1996/06/27  14:38:45  borland
 * Added fflush to the end of SDDS_WriteLayout, to adjust for removal of
 * fflush from print_namelist in namelist library.
 *
 * Revision 1.12  1996/03/21  16:38:40  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.11  1996/03/12  04:00:50  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.10  1996/01/19  00:13:42  borland
 * Switched from SDDS_UnpackFopen() to UnpackFopen(), removing former from
 * SDDS_input.c .  Using popenUsed entry in layout to keep track of whether
 * input files is a pipe or a bonafide file.
 *
 * Revision 1.9  1996/01/10  20:48:22  borland
 * Added SDDS_FileIsLocked() routine. Use it prior to fopen() calls to avoid
 * truncating the file.
 *
 * Revision 1.8  1996/01/10  17:21:04  borland
 * Added SDDS_LockFile() and code to use it in SDDS_InitializeCopy(),
 * SDDS_InitializeOutput(), and SDDS_InitializeAppend().
 *
 * Revision 1.7  1995/12/12  10:01:58  borland
 * Fixed bugs having to do with updates of no_row_counts=1 files; won't allow
 * changing no_row_counts after writing layout or for an input file.
 *
 * Revision 1.6  1995/12/12  04:26:39  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.5  1995/11/13  16:04:21  borland
 * Added mode argument to SDDS_UpdatePage to support partially flushed tabular
 * data in SDDS_UpdateBinaryPage.
 *
 * Revision 1.4  1995/09/15  16:20:12  borland
 * Modified IsValidName to permit . and : as first characters.
 *
 * Revision 1.3  1995/09/12  03:16:13  borland
 * Added SDDS_SetNameValidityFlags() and modified SDDS_IsValidName() to
 * support backward compatibility for "illegal" names.
 *
 * Revision 1.2  1995/09/05  21:11:06  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"
#include <ctype.h> 

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#else
#include <unistd.h>
#endif

#if SDDS_VERSION != 2
#error "SDDS_VERSION does not match version of this file"
#endif

#undef DEBUG

/* Allows "temporarily" closing a file.  Use SDDS_ReconnectFile() to open it
 * again in the same position.  Updates the present page and flushes the
 * table.
 */

int32_t SDDS_DisconnectFile(SDDS_DATASET *SDDS_dataset)
{
#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_DisconnectFile(SDDS_dataset);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DisconnectFile"))
    return 0;
  if (!SDDS_dataset->layout.filename || SDDS_dataset->layout.gzipFile) {
    SDDS_SetError("Can't disconnect file.  No filename or gzip file. (SDDS_DisconnectFile)");
    return 0;
  }
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't disconnect file.  Already disconnected. (SDDS_DisconnectFile)");
    return 0;
  }
  if (SDDS_dataset->page_started && !SDDS_UpdatePage(SDDS_dataset, FLUSH_TABLE)) {
    SDDS_SetError("Can't disconnect file.  Problem updating page. (SDDS_DisconnectFile)");
    return 0;
  }
  if (fclose(SDDS_dataset->layout.fp)) {
    SDDS_SetError("Can't disconnect file.  Problem closing file. (SDDS_DisconnectFile)");
    return 0;
  }
  SDDS_dataset->layout.disconnected = 1;
  return 1;
}

int32_t SDDS_ReconnectFile(SDDS_DATASET *SDDS_dataset)
{
#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_ReconnectFile(SDDS_dataset);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReconnectFile"))
    return 0;
  if (!SDDS_dataset->layout.disconnected || !SDDS_dataset->layout.filename) {
    SDDS_SetError("Can't reconnect file.  Not disconnected or missing filename. (SDDS_ReconnectFile)");
    return 0;
  }
  if (!(SDDS_dataset->layout.fp=fopen(SDDS_dataset->layout.filename,  FOPEN_READ_AND_WRITE_MODE))) {
    char s[1024];
    sprintf(s, "Unable to open file %s (SDDS_ReconnectFile)", SDDS_dataset->layout.filename);
    SDDS_SetError(s);
    return 0;
  }
  if (fseek(SDDS_dataset->layout.fp, 0, 2)==-1) {
    SDDS_SetError("Can't reconnect file.  Fseek failed. (SDDS_ReconnectFile)");
    return 0;
  }
  SDDS_dataset->layout.disconnected = 0;
  return 1;
}

/* appends to a file by adding a new page */

int32_t SDDS_InitializeAppend(SDDS_DATASET *SDDS_dataset, char *filename)
{
  /*  char *ptr, *datafile, *headerfile;*/
  static char s[SDDS_MAXLINE];
  int32_t endOfLayoutOffset, endOfFileOffset;
  char *extension;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeAppend"))
    return 0;
  if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    sprintf(s, "Unable to initialize input for file %s--can't zero SDDS_DATASET structure (SDDS_InitializeAppend)",
            filename);
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->layout.popenUsed = SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = 0;
  if (!filename)
    SDDS_dataset->layout.filename = NULL;
  else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeAppend)", filename);
    SDDS_SetError(s);
    return 0;
  } else if ((extension = strrchr(filename, '.')) && strcmp(extension, ".gz")==0) {
    sprintf(s, "Cannot append to a compressed file %s (SDDS_InitializeAppend)", filename);
    SDDS_SetError(s);
    return 0;
  }

  SDDS_dataset->layout.popenUsed = 0;
  if (!filename) {
#if defined(_WIN32)
    if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
      sprintf(s, "unable to set stdin to binary mode");
      SDDS_SetError(s);
      return 0;
    }     
#endif
    SDDS_dataset->layout.fp = stdin;
  }
  else {
    if (SDDS_FileIsLocked(filename)) {
      sprintf(s, "unable to open file %s for appending--file is locked (SDDS_InitializeAppend)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!(SDDS_dataset->layout.fp=fopen(filename, FOPEN_READ_AND_WRITE_MODE))) {
      sprintf(s, "Unable to open file %s for appending (SDDS_InitializeAppend)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!SDDS_LockFile(SDDS_dataset->layout.fp, filename, "SDDS_InitializeAppend")) 
      return 0;
  }

  if (!SDDS_ReadLayout(SDDS_dataset, SDDS_dataset->layout.fp))
    return 0;
  endOfLayoutOffset = ftell(SDDS_dataset->layout.fp);
  if (SDDS_dataset->layout.n_columns &&
      (!(SDDS_dataset->column_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
       !(SDDS_dataset->column_order=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
       !SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
       !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1))) {
    SDDS_SetError("Unable to initialize input--memory allocation failure (SDDS_InitializeAppend)");
    return 0;
  }
  if (fseek(SDDS_dataset->layout.fp, 0, 2)==-1) {
    SDDS_SetError("Unable to initialize append--seek failure (SDDS_InitializeAppend)");
    return 0;
  }
  endOfFileOffset = ftell(SDDS_dataset->layout.fp);
  if (endOfFileOffset==endOfLayoutOffset)
    SDDS_dataset->file_had_data = 0;  /* appending to empty file */
  else
    SDDS_dataset->file_had_data = 1;  /* appending to nonempty file */
  SDDS_dataset->layout.layout_written = 1;  /* its already in the file */
  SDDS_dataset->mode=SDDS_WRITEMODE; /*writing*/
  return 1;
}

/* appends to a file by adding to the last page */

int32_t SDDS_InitializeAppendToPage(SDDS_DATASET *SDDS_dataset, char *filename, int32_t updateInterval,
                                 int32_t *rowsPresentReturn)
{
  /*  char *ptr, *datafile, *headerfile;*/
  static char s[SDDS_MAXLINE];
  int32_t endOfLayoutOffset, endOfFileOffset, rowCountOffset, offset;
  int32_t rowsPresent;
  char *extension;
  int32_t previousBufferSize;
  
  *rowsPresentReturn = -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeAppendToPage"))
    return 0;
  if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    sprintf(s, "Unable to initialize input for file %s--can't zero SDDS_DATASET structure (SDDS_InitializeAppendToPage)",
            filename);
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->layout.popenUsed = SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = 0;
  if (!filename)
    SDDS_dataset->layout.filename = NULL;
  else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeAppendToPage)", filename);
    SDDS_SetError(s);
    return 0;
  } else if ((extension = strrchr(filename, '.')) && strcmp(extension, ".gz")==0) {
    sprintf(s, "Cannot append to a compressed file %s (SDDS_InitializeAppendToPage)", filename);
    SDDS_SetError(s);
    return 0;
  }

  if (!filename) {
#if defined(_WIN32)
    if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
      sprintf(s, "unable to set stdin to binary mode");
      SDDS_SetError(s);
      return 0;
    }     
#endif
    SDDS_dataset->layout.fp = stdin;
  }
  else {
    if (SDDS_FileIsLocked(filename)) {
      sprintf(s, "unable to open file %s for appending--file is locked (SDDS_InitializeAppendToPage)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!(SDDS_dataset->layout.fp=fopen(filename, FOPEN_READ_AND_WRITE_MODE))) {
      sprintf(s, "Unable to open file %s for appending (SDDS_InitializeAppendToPage)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!SDDS_LockFile(SDDS_dataset->layout.fp, filename, "SDDS_InitializeAppendToPage"))  {
      return 0;
    }
  }

  if (!SDDS_ReadLayout(SDDS_dataset, SDDS_dataset->layout.fp)) {
    return 0;
  }
  endOfLayoutOffset = ftell(SDDS_dataset->layout.fp);
  if (SDDS_dataset->layout.n_columns &&
      (!(SDDS_dataset->column_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
       !(SDDS_dataset->column_order=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
       !SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
       !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1))) {
    SDDS_SetError("Unable to initialize input--memory allocation failure (SDDS_InitializeAppendToPage)");
    return 0;
  }
  rowCountOffset = -1;
  rowsPresent = 0;
#ifdef DEBUG
  fprintf(stderr, "Data mode is %s\n", SDDS_data_mode[SDDS_dataset->layout.data_mode.mode-1]);
#endif
  SDDS_dataset->pagecount_offset=NULL;
  previousBufferSize = SDDS_SetDefaultIOBufferSize(0);
  if (!SDDS_dataset->layout.data_mode.no_row_counts) {
    /* read pages to get to the last page */
    while (SDDS_ReadPageSparse(SDDS_dataset, 0, 10000, 0)>0) {
      rowCountOffset = SDDS_dataset->rowcount_offset;
      offset = ftell(SDDS_dataset->layout.fp);
      fseek(SDDS_dataset->layout.fp, rowCountOffset, 0);
      if (SDDS_dataset->layout.data_mode.mode==SDDS_BINARY) {
        fread(&rowsPresent, 1, sizeof(rowsPresent), SDDS_dataset->layout.fp);
	if (SDDS_dataset->swapByteOrder) {
	  SDDS_SwapLong(&rowsPresent);
	}
      } else {
        char buffer[30];
        if (!fgets(buffer, 30, SDDS_dataset->layout.fp) || strlen(buffer)!=21 ||
            sscanf(buffer, "%" SCNd32, &rowsPresent)!=1) {
#ifdef DEBUG
          fprintf(stderr, "buffer for row count data: >%s<\n", buffer);
#endif
          SDDS_SetError("Unable to initialize input--row count not present or not correct length (SDDS_InitializeAppendToPage)");
          SDDS_SetDefaultIOBufferSize(previousBufferSize);
          return 0;
        }
      }
      fseek(SDDS_dataset->layout.fp, offset, 0);
#ifdef DEBUG
      fprintf(stderr, "%" PRId32 " rows present\n", rowsPresent);
#endif
    }
    if (rowCountOffset==-1) {
      SDDS_SetDefaultIOBufferSize(previousBufferSize);
      SDDS_SetError("Unable to initialize input--problem finding row count offset (SDDS_InitializeAppendToPage)");
      return 0;
    }
  }
  SDDS_SetDefaultIOBufferSize(previousBufferSize);
  SDDS_dataset->fBuffer.bytesLeft = SDDS_dataset->fBuffer.bufferSize;
  
#ifdef DEBUG
  fprintf(stderr, "Starting page with %" PRId32 " rows\n", updateInterval);
#endif
  if (!SDDS_StartPage(SDDS_dataset, updateInterval)) {
    SDDS_SetError("Unable to initialize input--problem starting page (SDDS_InitializeAppendToPage)");
    return 0;
  }

  /* seek to the end of the file */
  if (fseek(SDDS_dataset->layout.fp, 0, 2)==-1) {
    SDDS_SetError("Unable to initialize append--seek failure (SDDS_InitializeAppendToPage)");
    return 0;
  }
  endOfFileOffset = ftell(SDDS_dataset->layout.fp);
  if (endOfFileOffset==endOfLayoutOffset)
    SDDS_dataset->file_had_data = 0;  /* appending to empty file */
  else {
    SDDS_dataset->file_had_data = 1;  /* appending to nonempty file */
    if (rowCountOffset!=-1) {
      SDDS_dataset->rowcount_offset = rowCountOffset;
      SDDS_dataset->n_rows_written = rowsPresent;
      SDDS_dataset->first_row_in_mem = rowsPresent;
      SDDS_dataset->last_row_written = -1;
      *rowsPresentReturn = rowsPresent;
      SDDS_dataset->writing_page = 1;
    }
  }
#ifdef DEBUG
  fprintf(stderr, "rowcount_offset = %" PRId32 ", n_rows_written = %" PRId32 ", first_row_in_mem = %" PRId32 ", last_row_written = %" PRId32 "\n",
          SDDS_dataset->rowcount_offset, SDDS_dataset->n_rows_written , SDDS_dataset->first_row_in_mem,
          SDDS_dataset->last_row_written);
#endif
  SDDS_dataset->page_number = 1; 
  SDDS_dataset->layout.layout_written = 1;  /* its already in the file */
  SDDS_dataset->mode=SDDS_WRITEMODE; /*writing*/
  return 1;
}


int32_t SDDS_InitializeOutput(SDDS_DATASET *SDDS_dataset, int32_t data_mode, int32_t lines_per_row, 
                           char *description, char *contents, char *filename)
{
  static char s[SDDS_MAXLINE];
#if defined(zLib)
  char *extension;
#endif
  char *outputEndianess = NULL;
  
  if (data_mode == SDDS_PARALLEL) 
    return SDDS_Parallel_InitializeOutput(SDDS_dataset, description, contents, filename); 
  
  if (sizeof(gzFile) != sizeof(void*)) {
    SDDS_SetError("gzFile is not the same size as void *, possible corruption of the SDDS_LAYOUT structure");
    return(0);
  }
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeOutput"))
    return 0;
  if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    sprintf(s, 
            "Unable to initialize output for file %s--can't zero SDDS_DATASET structure (SDDS_InitializeOutput)",
            filename);
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->layout.popenUsed = SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = 0;
  if (!filename) {
#if defined(_WIN32)
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
      sprintf(s, "unable to set stdout to binary mode");
      SDDS_SetError(s);
      return 0;
    }      
#endif
    SDDS_dataset->layout.fp = stdout;
  }
  else {
    if (SDDS_FileIsLocked(filename)) {
      sprintf(s, "unable to open file %s for writing--file is locked (SDDS_InitializeOutput)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!(SDDS_dataset->layout.fp=fopen(filename, FOPEN_WRITE_MODE))) {
      sprintf(s, "Unable to open file %s for writing (SDDS_InitializeOutput)", filename);
      SDDS_SetError(s);
      return 0;
    }
    if (!SDDS_LockFile(SDDS_dataset->layout.fp, filename, "SDDS_InitializeOutput"))
      return 0;
#if defined(zLib)
    if ((extension = strrchr(filename, '.')) && strcmp(extension, ".gz")==0) {
      SDDS_dataset->layout.gzipFile = 1;
      if ((SDDS_dataset->layout.gzfp=gzdopen(fileno(SDDS_dataset->layout.fp), FOPEN_WRITE_MODE)) == NULL) {
	sprintf(s, "Unable to open compressed file %s for writing (SDDS_InitializeOutput)", filename);
	SDDS_SetError(s);
	return 0;
      }
    }
#endif
  }
  SDDS_dataset->page_number =SDDS_dataset->page_started= 0;
  SDDS_dataset->file_had_data = SDDS_dataset->layout.layout_written = 0;
  if (!filename)
    SDDS_dataset->layout.filename = NULL;
  else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)", filename);
    SDDS_SetError(s);
    return 0;
  }
  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (strncmp(outputEndianess, "big", 3) == 0)
      SDDS_dataset->layout.byteOrderDeclared = SDDS_BIGENDIAN;
    else if (strncmp(outputEndianess, "little", 6) == 0)
      SDDS_dataset->layout.byteOrderDeclared = SDDS_LITTLEENDIAN; 
  } else {
    SDDS_dataset->layout.byteOrderDeclared = SDDS_IsBigEndianMachine()?SDDS_BIGENDIAN:SDDS_LITTLEENDIAN;
  }

  if (data_mode<0 || data_mode>SDDS_NUM_DATA_MODES) {
    sprintf(s, "Invalid data mode for file %s (SDDS_InitializeOutput)", 
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  if (data_mode==SDDS_ASCII && lines_per_row<=0) {
    sprintf(s, "Invalid number of lines per row for file %s (SDDS_InitializeOutput)", 
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->layout.version = SDDS_VERSION;
  SDDS_dataset->layout.data_mode.mode = data_mode;
  SDDS_dataset->layout.data_mode.lines_per_row = lines_per_row;
  SDDS_dataset->layout.data_mode.no_row_counts = 0;
  SDDS_dataset->layout.data_mode.fixed_row_count = 0;
  SDDS_dataset->layout.data_mode.fsync_data = 0;
  if (description && !SDDS_CopyString(&SDDS_dataset->layout.description, description)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)",
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  if (contents && !SDDS_CopyString(&SDDS_dataset->layout.contents, contents)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)",
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->mode=SDDS_WRITEMODE; /*writing*/
  SDDS_dataset->pagecount_offset=NULL;
  SDDS_dataset->parallel_io = 0;
  return(1);
}

/* right now for parallel SDDS, do not open gz file and always written with binary mode*/
int32_t SDDS_Parallel_InitializeOutput(SDDS_DATASET *SDDS_dataset, char *description,
						     char *contents, char *filename)
{
  /*  SDDS_DATASET *SDDS_dataset;*/
  static char s[SDDS_MAXLINE];
  char *outputEndianess = NULL;
  
  /* SDDS_dataset = &(MPI_dataset->sdds_dataset);*/
  if (sizeof(gzFile) != sizeof(void*)) {
    SDDS_SetError("gzFile is not the same size as void *, possible corruption of the SDDS_LAYOUT structure");
    return(0);
  }
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeOutput"))
    return 0;
  /* if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    sprintf(s, 
            "Unable to initialize output for file %s--can't zero SDDS_DATASET structure (SDDS_InitializeOutput)",
            filename);
    SDDS_SetError(s);
    return 0;
    } */ /*the sdds dataset memory has been zeroed in the SDDS_MPI_Setup */
  SDDS_dataset->layout.popenUsed = SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = 0;
  SDDS_dataset->layout.fp = NULL;
  
  SDDS_dataset->page_number =SDDS_dataset->page_started= 0;
  SDDS_dataset->file_had_data = SDDS_dataset->layout.layout_written = 0;
  if (!filename)
    SDDS_dataset->layout.filename = NULL;
  else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)", filename);
    SDDS_SetError(s);
    return 0;
  }
  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (strncmp(outputEndianess, "big", 3) == 0)
      SDDS_dataset->layout.byteOrderDeclared = SDDS_BIGENDIAN;
    else if (strncmp(outputEndianess, "little", 6) == 0)
      SDDS_dataset->layout.byteOrderDeclared = SDDS_LITTLEENDIAN; 
  } else {
    SDDS_dataset->layout.byteOrderDeclared = SDDS_IsBigEndianMachine()?SDDS_BIGENDIAN:SDDS_LITTLEENDIAN;
  } 
  /* set big-endian for binary files, since it is the only type of MPI binary file.
     SDDS_dataset->layout.byteOrderDeclared = SDDS_BIGENDIAN; */
  SDDS_dataset->layout.version = SDDS_VERSION;
  /* it turned out that hard to write ascii file in parallel, fixed it as SDDS_BINARY */
  SDDS_dataset->layout.data_mode.mode = SDDS_BINARY;
  SDDS_dataset->layout.data_mode.lines_per_row = 0;
  SDDS_dataset->layout.data_mode.no_row_counts = 0;
  SDDS_dataset->layout.data_mode.fixed_row_count = 0;
  SDDS_dataset->layout.data_mode.fsync_data = 0;
  if (description && !SDDS_CopyString(&SDDS_dataset->layout.description, description)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)",
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  if (contents && !SDDS_CopyString(&SDDS_dataset->layout.contents, contents)) {
    sprintf(s, "Memory allocation failure initializing file %s (SDDS_InitializeOutput)",
            filename?filename:"stdout");
    SDDS_SetError(s);
    return 0;
  }
  SDDS_dataset->layout.n_parameters = SDDS_dataset->layout.n_columns = SDDS_dataset->layout.n_arrays 
    = SDDS_dataset->layout.n_associates = 0;
  SDDS_dataset->mode=SDDS_WRITEMODE; /*writing*/
  SDDS_dataset->pagecount_offset=NULL;
  SDDS_dataset->parallel_io = 1;
  return(1);
}

int32_t SDDS_SetNoRowCounts(SDDS_DATASET *SDDS_dataset, int32_t value)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetNoRowCounts"))
    return 0;
  if (SDDS_dataset->layout.layout_written) {
    SDDS_SetError("Can't change no_row_counts after writing the layout, or for a file you are reading.");
    return 0;
  }
  SDDS_dataset->layout.data_mode.no_row_counts = value?1:0;
  return 1;
}

int32_t SDDS_WriteLayout(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  int32_t i;
  char *outputEndianess = NULL;

#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_WriteLayout(SDDS_dataset);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteLayout"))
    return 0;

  if (!SDDS_SaveLayout(SDDS_dataset))
    return 0;

  layout = &SDDS_dataset->layout;

  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't write layout--file is disconnected (SDDS_WriteLayout)");
    return 0;
  }
  
  if (layout->layout_written) {
    SDDS_SetError("Can't write layout--already written to file (SDDS_WriteLayout)");
    return 0;
  }

  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (strncmp(outputEndianess, "big", 3) == 0)
      layout->byteOrderDeclared = SDDS_BIGENDIAN;
    else if (strncmp(outputEndianess, "little", 6) == 0)
      layout->byteOrderDeclared = SDDS_LITTLEENDIAN; 
  }

  if (!layout->byteOrderDeclared)
    layout->byteOrderDeclared = 
      SDDS_IsBigEndianMachine()?SDDS_BIGENDIAN:SDDS_LITTLEENDIAN;

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!(gzfp=layout->gzfp)) {
      SDDS_SetError("Can't write SDDS layout--file pointer is NULL (SDDS_WriteLayout)");
      return 0;
    }
    
    /* write out the layout data */
    if (!SDDS_GZipWriteVersion(SDDS_VERSION, gzfp)) {
      SDDS_SetError("Can't write SDDS layout--error writing version (SDDS_WriteLayout)");
      return 0;
    }
    if (SDDS_dataset->layout.data_mode.mode==SDDS_BINARY) {
      if (layout->byteOrderDeclared==SDDS_BIGENDIAN) 
	gzprintf(gzfp, "!# big-endian\n");
      else
        gzprintf(gzfp, "!# little-endian\n");
    }
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      gzprintf(gzfp, "!# fixed-rowcount\n");
    }
    if (!SDDS_GZipWriteDescription(layout->description, layout->contents, gzfp)) {
      SDDS_SetError("Can't write SDDS layout--error writing description (SDDS_WriteLayout)");
      return 0;
    }
  
    for (i=0; i<layout->n_parameters; i++)
      if (!SDDS_GZipWriteParameterDefinition(layout->parameter_definition+i, gzfp)) {
	SDDS_SetError("Unable to write layout--error writing parameter definition (SDDS_WriteLayout)");
	return 0;
      }

    for (i=0; i<layout->n_arrays; i++)
      if (!SDDS_GZipWriteArrayDefinition(layout->array_definition+i, gzfp)) {
	SDDS_SetError("Unable to write layout--error writing array definition (SDDS_WriteLayout)");
	return 0;
      }

    for (i=0; i<layout->n_columns; i++)
      if (!SDDS_GZipWriteColumnDefinition(layout->column_definition+i, gzfp)) {
	SDDS_SetError("Unable to write layout--error writing column definition (SDDS_WriteLayout)");
	return 0;
      }

#if RW_ASSOCIATES != 0
    for (i=0; i<layout->n_associates; i++)
      if (!SDDS_GZipWriteAssociateDefinition(layout->associate_definition+i, gzfp)) {
	SDDS_SetError("Unable to write layout--error writing associated file data (SDDS_WriteLayout)");
	return 0;
      }
#endif

    if (!SDDS_GZipWriteDataMode(&layout->data_mode, gzfp)) {
      SDDS_SetError("Unable to write layout--error writing data mode (SDDS_WriteLayout)");
      return 0;
    }

    layout->layout_written = 1;
    /*gzflush(gzfp, Z_FULL_FLUSH);*/
  } else {
#endif
    if (!(fp=layout->fp)) {
      SDDS_SetError("Can't write SDDS layout--file pointer is NULL (SDDS_WriteLayout)");
      return 0;
    }

    /* write out the layout data */
    if (SDDS_VERSION == 2) {
      layout->version = 1;
      for (i=0; i<layout->n_parameters; i++) {
        if ((layout->parameter_definition[i].type == SDDS_ULONG) || 
            (layout->parameter_definition[i].type == SDDS_USHORT)) {
          layout->version = SDDS_VERSION;
          break;
        }
      }
      for (i=0; i<layout->n_arrays; i++) {
        if ((layout->array_definition[i].type == SDDS_ULONG) || 
            (layout->array_definition[i].type == SDDS_USHORT)) {
          layout->version = SDDS_VERSION;
          break;
        }
      }
      for (i=0; i<layout->n_columns; i++) {
        if ((layout->column_definition[i].type == SDDS_ULONG) || 
            (layout->column_definition[i].type == SDDS_USHORT)) {
          layout->version = SDDS_VERSION;
          break;
        }
      }
      if (!SDDS_WriteVersion(layout->version, fp)) {
        SDDS_SetError("Can't write SDDS layout--error writing version (SDDS_WriteLayout)");
        return 0;
      }
    } else {
      if (!SDDS_WriteVersion(SDDS_VERSION, fp)) {
        SDDS_SetError("Can't write SDDS layout--error writing version (SDDS_WriteLayout)");
        return 0;
      }
    }
    if (SDDS_dataset->layout.data_mode.mode==SDDS_BINARY) {
      if (layout->byteOrderDeclared==SDDS_BIGENDIAN)
        fprintf(fp, "!# big-endian\n");
      else
        fprintf(fp, "!# little-endian\n");
    }
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fprintf(fp, "!# fixed-rowcount\n");
    }
    if (!SDDS_WriteDescription(layout->description, layout->contents, fp)) {
      SDDS_SetError("Can't write SDDS layout--error writing description (SDDS_WriteLayout)");
      return 0;
    }
    
    for (i=0; i<layout->n_parameters; i++)
      if (!SDDS_WriteParameterDefinition(layout->parameter_definition+i, fp)) {
	SDDS_SetError("Unable to write layout--error writing parameter definition (SDDS_WriteLayout)");
	return 0;
      }
    
    for (i=0; i<layout->n_arrays; i++)
      if (!SDDS_WriteArrayDefinition(layout->array_definition+i, fp)) {
	SDDS_SetError("Unable to write layout--error writing array definition (SDDS_WriteLayout)");
	return 0;
      }
    
    for (i=0; i<layout->n_columns; i++)
      if (!SDDS_WriteColumnDefinition(layout->column_definition+i, fp)) {
	SDDS_SetError("Unable to write layout--error writing column definition (SDDS_WriteLayout)");
	return 0;
      }
    
#if RW_ASSOCIATES != 0
    for (i=0; i<layout->n_associates; i++)
      if (!SDDS_WriteAssociateDefinition(layout->associate_definition+i, fp)) {
	SDDS_SetError("Unable to write layout--error writing associated file data (SDDS_WriteLayout)");
	return 0;
      }
#endif
    
    if (!SDDS_WriteDataMode(&layout->data_mode, fp)) {
      SDDS_SetError("Unable to write layout--error writing data mode (SDDS_WriteLayout)");
      return 0;
    }
    
    layout->layout_written = 1;
    fflush(fp);
#if defined(zLib)
  }
#endif
  if (SDDS_SyncDataSet(SDDS_dataset) != 0)
    return 0;
  return(1);
}

int32_t SDDS_WritePage(SDDS_DATASET *SDDS_dataset)
{
  int32_t result;
#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_WritePage(SDDS_dataset);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WritePage"))
    return 0;
  if (!SDDS_dataset->layout.layout_written) {
    SDDS_SetError("Unable to write page--layout not written (SDDS_WritePage)");
    return 0;
  }
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't write page--file is disconnected (SDDS_WritePage)");
    return 0;
  }
  if (SDDS_dataset->layout.data_mode.mode==SDDS_ASCII)
    result = SDDS_WriteAsciiPage(SDDS_dataset);
  else if (SDDS_dataset->layout.data_mode.mode==SDDS_BINARY)
    result = SDDS_WriteBinaryPage(SDDS_dataset);
  else {
    SDDS_SetError("Unable to write page--unknown data mode (SDDS_WritePage)");
    return 0;
  }
  if (result == 1)
    if (SDDS_SyncDataSet(SDDS_dataset) != 0)
      return 0;
  return(result);
}

int32_t SDDS_UpdatePage(SDDS_DATASET *SDDS_dataset, uint32_t mode)
{
  int32_t result;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_UpdatePage"))
    return 0;
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't write page--file is disconnected (SDDS_UpdatePage)");
    return 0;
  }
  if (SDDS_dataset->page_started==0) {
    SDDS_SetError("Can't update page--no page started (SDDS_UpdatePage)");
    return 0;
  }
  if (SDDS_dataset->layout.data_mode.mode==SDDS_ASCII)
    result = SDDS_UpdateAsciiPage(SDDS_dataset, mode);
  else if (SDDS_dataset->layout.data_mode.mode==SDDS_BINARY)
    result = SDDS_UpdateBinaryPage(SDDS_dataset, mode);
  else {
    SDDS_SetError("Unable to update page--unknown data mode (SDDS_UpdatePage)");
    return 0;
  }
  if (result == 1)
    if (SDDS_SyncDataSet(SDDS_dataset) != 0)
      return 0;
  return(result);
}

int32_t SDDS_SyncDataSet(SDDS_DATASET *SDDS_dataset)
{
#if defined(vxWorks) || defined(_WIN32) || defined(linux) || defined(__APPLE__)
  return(0);
#else
  if (!(SDDS_dataset->layout.fp)) {
    SDDS_SetError("Unable to sync file--file pointer is NULL (SDDS_SyncDataSet)");
    return(-1);
  }
  if (SDDS_dataset->layout.data_mode.fsync_data == 0)
    return(0);
  if (fsync(fileno(SDDS_dataset->layout.fp)) == 0)
    return(0);
  /*
  SDDS_SetError("Unable to sync file (SDDS_SyncDataSet)");
  return(-1);
  */
  /* This error should not be fatal */
  return(0);
#endif
}

int32_t SDDS_DefineParameter1(SDDS_DATASET *SDDS_dataset, char *name, char *symbol, char *units, char *description, 
                           char *format_string, int32_t type, void *fixed_value)
{
  char buffer[SDDS_MAXLINE];
  if (!SDDS_IsValidName(name, "parameter"))
    return -1;
  if (!fixed_value || type==SDDS_STRING)
    return SDDS_DefineParameter(SDDS_dataset, name, symbol, units, description, format_string, type, fixed_value);
  if (type<=0 || type>SDDS_NUM_TYPES) {
    SDDS_SetError("Unknown data type (SDDS_DefineParameter1)");
    return(-1);
  }
  buffer[SDDS_MAXLINE-1] = 0;
  if (!SDDS_SprintTypedValue(fixed_value, 0, type, format_string, buffer, 0) || buffer[SDDS_MAXLINE-1]!=0) {
    SDDS_SetError("Unable to define fixed value for parameter (SDDS_DefineParameter1)");
    return(-1);
  }
  return SDDS_DefineParameter(SDDS_dataset, name, symbol, units, description, format_string, type, buffer);
}


int32_t SDDS_DefineParameter(SDDS_DATASET *SDDS_dataset, char *name, char *symbol, 
                          char *units, char *description, 
                          char *format_string, int32_t type, char *fixed_value)
{
  SDDS_LAYOUT *layout;
  PARAMETER_DEFINITION *definition;
  static char s[SDDS_MAXLINE];
  SORTED_INDEX *new_indexed_parameter;
  int32_t index, duplicate;

  if (!SDDS_IsValidName(name, "parameter"))
    return -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DefineParameter"))
    return(-1);
  if (!name) {
    SDDS_SetError("NULL name not allowed for parameter definition");
    return(-1);
  }
  layout = &SDDS_dataset->layout;
  if (!(layout->parameter_definition = 
        SDDS_Realloc(layout->parameter_definition, sizeof(*layout->parameter_definition)*(layout->n_parameters+1))) || 
      !(layout->parameter_index = 
        SDDS_Realloc(layout->parameter_index, sizeof(*layout->parameter_index)*(layout->n_parameters+1))) ||
      !(new_indexed_parameter=(SORTED_INDEX*)SDDS_Malloc(sizeof(*new_indexed_parameter)))) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineParameter)");
    return(-1);
  }
  if (!SDDS_CopyString(&new_indexed_parameter->name, name))
    return -1;
  index = binaryInsert((void**)layout->parameter_index, layout->n_parameters,
                       new_indexed_parameter, SDDS_CompareIndexedNames, &duplicate);
  if (duplicate) {
    sprintf(s, "Parameter %s already exists (SDDS_DefineParameter)", name);
    SDDS_SetError(s);
    return(-1);
  }
  layout->parameter_index[index]->index = layout->n_parameters;

  if (!SDDS_ZeroMemory(definition = layout->parameter_definition+layout->n_parameters, sizeof(PARAMETER_DEFINITION))) {
    SDDS_SetError("Unable to define parameter--can't zero memory for parameter definition (SDDS_DefineParameter)");
    return(-1);
  }
  definition->name = new_indexed_parameter->name;
  if (symbol && !SDDS_CopyString(&definition->symbol, symbol)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineParameter)");
    return(-1);
  }
  if (units && !SDDS_CopyString(&definition->units, units)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineParameter)");
    return(-1);
  }
  if (description && !SDDS_CopyString(&definition->description, description)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineParameter)");
    return(-1);
  }
  if (type<=0 || type>SDDS_NUM_TYPES) {
    SDDS_SetError("Unknown data type (SDDS_DefineParameter)");
    return(-1);
  }
  definition->type = type;
  if (format_string) {
    if (!SDDS_VerifyPrintfFormat(format_string, type)) {
      SDDS_SetError("Invalid format string (SDDS_DefineParameter)");
      return(-1);
    }
    if (!SDDS_CopyString(&definition->format_string, format_string)) {
      SDDS_SetError("Memory allocation failure (SDDS_DefineParameter)");
      return(-1);
    }
  }
  if (fixed_value && !SDDS_CopyString(&(definition->fixed_value), fixed_value)) {
    SDDS_SetError("Couldn't copy fixed_value string (SDDS_DefineParameter)");
    return(-1);
  }
  definition->definition_mode = SDDS_NORMAL_DEFINITION;
  if (type==SDDS_STRING)
    definition->memory_number = SDDS_CreateRpnMemory(name, 1);
  else
    definition->memory_number = SDDS_CreateRpnMemory(name, 0);
  layout->n_parameters += 1;
  return(layout->n_parameters-1);
}

int32_t SDDS_DefineArray(SDDS_DATASET *SDDS_dataset, char *name, char *symbol, char *units, 
                      char *description, char *format_string, int32_t type, int32_t field_length, 
                      int32_t dimensions, char *group_name)
{
  SDDS_LAYOUT *layout;
  ARRAY_DEFINITION *definition;
  static char s[SDDS_MAXLINE];
  SORTED_INDEX *new_indexed_array;
  int32_t index, duplicate;

  if (!SDDS_IsValidName(name, "array"))
    return -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DefineArray"))
    return(-1);
  if (!name) {
    SDDS_SetError("NULL name not allowed for array definition");
    return(-1);
  }
  layout = &SDDS_dataset->layout;
  if (!(layout->array_definition = 
        SDDS_Realloc(layout->array_definition, sizeof(*layout->array_definition)*(layout->n_arrays+1))) ||
      !(layout->array_index = 
        SDDS_Realloc(layout->array_index, sizeof(*layout->array_index)*(layout->n_arrays+1))) ||
      !(new_indexed_array=(SORTED_INDEX*)SDDS_Malloc(sizeof(*new_indexed_array)))) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineArray)");
    return(-1);
  }

  if (!SDDS_CopyString(&new_indexed_array->name, name))
    return -1;
  index = binaryInsert((void**)layout->array_index, layout->n_arrays,
                       new_indexed_array, SDDS_CompareIndexedNames, &duplicate);
  if (duplicate) {
    sprintf(s, "Array %s already exists (SDDS_DefineArray)", name);
    SDDS_SetError(s);
    return(-1);
  }
  layout->array_index[index]->index = layout->n_arrays;

  if (!SDDS_ZeroMemory(definition = layout->array_definition+layout->n_arrays, sizeof(ARRAY_DEFINITION))) {
    SDDS_SetError("Unable to define array--can't zero memory for array definition (SDDS_DefineArray)");
    return(-1);
  }
  definition->name = new_indexed_array->name;
  if ((symbol && !SDDS_CopyString(&definition->symbol, symbol)) ||
      (units && !SDDS_CopyString(&definition->units, units)) ||
      (description && !SDDS_CopyString(&definition->description, description)) ||
      (group_name && !SDDS_CopyString(&definition->group_name, group_name))) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineArray)");
    return(-1);
  }
  if (type<=0 || type>SDDS_NUM_TYPES) {
    SDDS_SetError("Unknown data type (SDDS_DefineArray)");
    return(-1);
  }
  definition->type = type;
  if (format_string) {
    if (!SDDS_VerifyPrintfFormat(format_string, type)) {
      SDDS_SetError("Invalid format string (SDDS_DefineArray)");
      return(-1);
    }
    if (!SDDS_CopyString(&definition->format_string, format_string)) {
      SDDS_SetError("Memory allocation failure (SDDS_DefineArray)");
      return(-1);
    }
  }
  if ((definition->field_length = field_length)<0 && type!=SDDS_STRING) {
    SDDS_SetError("Invalid field length (SDDS_DefineArray)");
    return(-1);
  }
  if ((definition->dimensions=dimensions)<1) {
    SDDS_SetError("Invalid number of dimensions for array (SDDS_DefineArray)");
    return(-1);
  }
  layout->n_arrays += 1;
  return(layout->n_arrays-1);
}

int32_t SDDS_DefineColumn(SDDS_DATASET *SDDS_dataset, char *name, char *symbol, char *units, char *description, 
                       char *format_string, int32_t type, int32_t field_length)
{
  SDDS_LAYOUT *layout;
  COLUMN_DEFINITION *definition;
  static char s[SDDS_MAXLINE];
  SORTED_INDEX *new_indexed_column;
  int32_t index, duplicate;
  
  if (!SDDS_IsValidName(name, "column"))
    return -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DefineColumn"))
    return(-1);
  if (!name) {
    SDDS_SetError("NULL name not allowed for column definition");
    return(-1);
  }
  layout = &SDDS_dataset->layout;
  if (!(layout->column_definition = 
        SDDS_Realloc(layout->column_definition, sizeof(*layout->column_definition)*(layout->n_columns+1))) ||
      !(layout->column_index = 
        SDDS_Realloc(layout->column_index, sizeof(*layout->column_index)*(layout->n_columns+1))) ||
      !(new_indexed_column = (SORTED_INDEX*)SDDS_Malloc(sizeof(*new_indexed_column)))) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
    return(-1);
  }
  if (!SDDS_CopyString(&new_indexed_column->name, name))
    return -1;
  index = binaryInsert((void**)layout->column_index, layout->n_columns,
                       new_indexed_column, SDDS_CompareIndexedNames, &duplicate);
  if (duplicate) {
    sprintf(s, "Column %s already exists (SDDS_DefineColumn)", name);
    SDDS_SetError(s);
    return(-1);
  }
  layout->column_index[index]->index = layout->n_columns;
  if (!SDDS_ZeroMemory(definition = layout->column_definition+layout->n_columns, sizeof(COLUMN_DEFINITION))) {
    SDDS_SetError("Unable to define column--can't zero memory for column definition (SDDS_DefineColumn)");
    return(-1);
  }
  definition->name = new_indexed_column->name;
  if (symbol && !SDDS_CopyString(&definition->symbol, symbol)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
    return(-1);
  }
  if (units && !SDDS_CopyString(&definition->units, units)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
    return(-1);
  }
  if (description && !SDDS_CopyString(&definition->description, description)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
    return(-1);
  }
  if (type<=0 || type>SDDS_NUM_TYPES) {
    SDDS_SetError("Unknown data type (SDDS_DefineColumn)");
    return(-1);
  }
  definition->type = type;
  if (format_string) {
    if (!SDDS_VerifyPrintfFormat(format_string, type)) {
      SDDS_SetError("Invalid format string (SDDS_DefineColumn)");
      return(-1);
    }
    if (!SDDS_CopyString(&definition->format_string, format_string)) {
      SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
      return(-1);
    }
  }
  if ((definition->field_length = field_length)<0 && type!=SDDS_STRING) {
    SDDS_SetError("Invalid field length (SDDS_DefineColumn)");
    return(-1);
  }

  if (SDDS_dataset->n_rows_allocated) {
    if (!SDDS_dataset->data) {
      SDDS_SetError("data array NULL but rows have been allocated! (SDDS_DefineColumn)");
      return(-1);
    }
    /* data already present--must resize data and parameter memory */
    if (!(SDDS_dataset->data = SDDS_Realloc(SDDS_dataset->data, sizeof(*SDDS_dataset->data)*(layout->n_columns+1))) || 
        !(SDDS_dataset->data[layout->n_columns] = calloc(SDDS_dataset->n_rows_allocated, SDDS_type_size[type-1]))) {
      SDDS_SetError("Memory allocation failure (SDDS_DefineColumn)");
      return(-1);
    }
  }

  /* not part of output: */
  definition->definition_mode = SDDS_NORMAL_DEFINITION;
  if (type==SDDS_STRING)
    definition->memory_number = SDDS_CreateRpnMemory(name, 1);
  else {
    definition->memory_number = SDDS_CreateRpnMemory(name, 0);
  }
  sprintf(s, "&%s", name);
  definition->pointer_number = SDDS_CreateRpnArray(s);

  layout->n_columns += 1;
  return(layout->n_columns-1);
}

int32_t SDDS_DefineSimpleColumn(SDDS_DATASET *SDDS_dataset, char *name, char *unit, int32_t type)
{
  if (SDDS_DefineColumn(SDDS_dataset, name, NULL, unit, NULL, NULL, type, 0)<0)
    return 0;
  return(1);
}

int32_t SDDS_DefineSimpleParameter(SDDS_DATASET *SDDS_dataset, char *name, char *unit, int32_t type)
{
  if (SDDS_DefineParameter(SDDS_dataset, name, NULL, unit, NULL, NULL, type, NULL)<0)
    return 0;
  return(1);
}

int32_t SDDS_DefineSimpleColumns(SDDS_DATASET *SDDS_dataset, int32_t number, char **name, char **unit, int32_t type)
{
  int32_t i;
  if (!number)
    return(1);
  if (!name)
    return 0;
  for (i=0; i<number; i++)
    if (SDDS_DefineColumn(SDDS_dataset, name[i], NULL, unit?unit[i]:NULL, NULL, NULL, type, 0)<0)
      return 0;
  return(1);
}

int32_t SDDS_DefineSimpleParameters(SDDS_DATASET *SDDS_dataset, int32_t number, char **name, char **unit, int32_t type)
{
  int32_t i;
  if (!number)
    return(1);
  if (!name)
    return 0;
  for (i=0; i<number; i++)
    if (SDDS_DefineParameter(SDDS_dataset, name[i], NULL, unit?unit[i]:NULL, NULL, NULL, type, NULL)<0)
      return 0;
  return(1);
}


static uint32_t nameValidityFlags = 0;
int32_t SDDS_SetNameValidityFlags(uint32_t flags)
{
  uint32_t oldFlags;
  oldFlags = nameValidityFlags;
  nameValidityFlags = flags;
  return oldFlags;
}

int32_t SDDS_IsValidName(char *name, char *class)
{
  char *ptr;
  int32_t isValid = 1;
  static char s[SDDS_MAXLINE];
  static char *validChars = "@:#+%-._$&/[]";
  static char *startChars = ".:";

  if (nameValidityFlags&SDDS_ALLOW_ANY_NAME)
    return 1;
  ptr = name;
  if (strlen(name)==0)
    isValid = 0;
  else if (!(nameValidityFlags&SDDS_ALLOW_V15_NAME)) {
    /* post V1.5 allows only alpha and startChars members as first character */
    /* V1.5 allows alpha, digits, and any validChars members */
    if (!(isalpha(*ptr) || strchr(startChars, *ptr)))
      isValid = 0;
  }
  while (isValid && *ptr) {
    if (!(isalnum(*ptr) || strchr(validChars, *ptr)))
      isValid = 0;
    ptr++;
  }
  if (!isValid) {
    sprintf(s, "The following %s name is invalid: >%s<\n(sddsconvert may be used to change the name)\n", class, name);
    SDDS_SetError(s);
    return 0;
  }
  return 1;
}


int32_t SDDS_DefineAssociate(SDDS_DATASET *SDDS_dataset, char *name,
                          char *filename, char *path, char *description, char *contents, int32_t sdds)
{

#if RW_ASSOCIATES == 0
  return 0;
#else
  SDDS_LAYOUT *layout;
  ASSOCIATE_DEFINITION *definition;
  static char s[SDDS_MAXLINE];
  if (!SDDS_IsValidName(name, "associate"))
    return -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DefineAssociate"))
    return(-1);
  layout = &SDDS_dataset->layout;
  if (!(layout->associate_definition = 
        SDDS_Realloc(layout->associate_definition, sizeof(*layout->associate_definition)*(layout->n_associates+1)))) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  if (!name) {
    SDDS_SetError("NULL name not allowed for associate file (SDDS_DefineAssociate)");
    return(-1);
  }
  if (!filename) {
    SDDS_SetError("NULL filename not allowed for associate file (SDDS_DefineAssociate)");
    return(-1);
  }
  if (SDDS_GetAssociateIndex(SDDS_dataset, name)>=0) {
    sprintf(s, "Associate with name %s already exists (SDDS_DefineAssociate)", name);
    SDDS_SetError(s);
    return(-1);
  }
  if (!SDDS_ZeroMemory(definition = layout->associate_definition+layout->n_associates, sizeof(ASSOCIATE_DEFINITION))) {
    SDDS_SetError("Unable to define associate--can't zero memory for associate (SDDS_DefineAssociate)");
    return(-1);
  }

  if (!SDDS_CopyString(&definition->name, name)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  if (!SDDS_CopyString(&definition->filename, filename)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  if (path && !SDDS_CopyString(&definition->path, path)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  if (contents && !SDDS_CopyString(&definition->contents, contents)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  if (description && !SDDS_CopyString(&definition->description, description)) {
    SDDS_SetError("Memory allocation failure (SDDS_DefineAssociate)");
    return(-1);
  }
  definition->sdds = sdds;
  layout->n_associates += 1;
  return(layout->n_associates-1);
#endif
}

int32_t SDDS_EraseData(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout;
  int32_t i, j;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_EraseData"))
    return 0;
  layout = &SDDS_dataset->layout;
  if (SDDS_dataset->data) {
    for (i=0; i<layout->n_columns; i++) {
      if (!SDDS_dataset->data[i])
        continue;
      if (layout->column_definition[i].type==SDDS_STRING) {
        for (j=0; j<SDDS_dataset->n_rows; j++) {
          if (((char**)SDDS_dataset->data[i])[j]) {
            free(((char**)SDDS_dataset->data[i])[j]);
            ((char**)SDDS_dataset->data[i])[j] = NULL;
          }
        }
      }
    }
  }
  SDDS_dataset->n_rows = 0;

  if (SDDS_dataset->parameter) {
    for (i=0; i<layout->n_parameters; i++) {
      if (!SDDS_dataset->parameter[i])
        continue;
      if (layout->parameter_definition[i].type==SDDS_STRING && *(char**)(SDDS_dataset->parameter[i])) {
        free(*(char**)(SDDS_dataset->parameter[i]));
        *(char**)SDDS_dataset->parameter[i] = NULL;
      }
    }
  }

  if (SDDS_dataset->array) {
    for (i=0; i<layout->n_arrays; i++) {
      if (SDDS_dataset->array[i].definition->type==SDDS_STRING) {
        for (j=0; j<SDDS_dataset->array[i].elements; j++) {
          if (((char**)SDDS_dataset->array[i].data)[j]) {
            free(((char**)SDDS_dataset->array[i].data)[j]);
            ((char**)SDDS_dataset->array[i].data)[j] = NULL;
          }
        }
      }
    }
  }

  return(1);
}

int32_t SDDS_SetRowCountMode(SDDS_DATASET *SDDS_dataset, uint32_t mode) {
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetRowCountMode"))
    return 0;
  if (SDDS_dataset->layout.layout_written) {
    SDDS_SetError("Can't change row count mode after writing the layout, or for a file you are reading.");
    return 0;
  }
  if (mode&SDDS_VARIABLEROWCOUNT) {
    SDDS_dataset->layout.data_mode.fixed_row_count = 0;
    SDDS_dataset->layout.data_mode.no_row_counts = 0;
  } else if (mode&SDDS_FIXEDROWCOUNT) {
    SDDS_dataset->layout.data_mode.fixed_row_count = 1;
    SDDS_dataset->layout.data_mode.no_row_counts = 0;
    SDDS_dataset->layout.data_mode.fsync_data = 0;
  } else if (mode&SDDS_NOROWCOUNT) {
    SDDS_dataset->layout.data_mode.fixed_row_count = 0;
    SDDS_dataset->layout.data_mode.no_row_counts = 1;
  } else {
    SDDS_SetError("Invalid row count mode (SDDS_SetRowCountMode).");
    return 0;
  }
  if (!SDDS_SaveLayout(SDDS_dataset))
    return 0;
  return 1;
}

void SDDS_DisableFSync(SDDS_DATASET *SDDS_dataset) {
  SDDS_dataset->layout.data_mode.fsync_data = 0;
}

void SDDS_EnableFSync(SDDS_DATASET *SDDS_dataset) {
  SDDS_dataset->layout.data_mode.fsync_data = 1;
}

int32_t SDDS_DoFSync(SDDS_DATASET *SDDS_dataset) {
#if defined(vxWorks) || defined(_WIN32) || defined(__APPLE__)
  return 1;
#else
  if (SDDS_dataset && SDDS_dataset->layout.fp) 
    return fsync(fileno(SDDS_dataset->layout.fp))==0;
  return 0;
#endif
}
