/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_utils.c
 * contents: SDDS utility routines
 *
 * M. Borland, November 1993
 $Log: SDDS_utils.c,v $
 Revision 1.69  2010/08/27 16:31:32  soliday
 Applied patch supplied by outside user to fix a problem on Fedora 13.

 Revision 1.68  2010/03/18 17:28:03  borland
 Added flag to allow disabling file locking.  Useful for PVFS filesystem.

 Revision 1.67  2010/03/13 22:10:44  borland
 SDDS_Bomb() now calls SDDS_PrintErrors() to give more useful error messages.

 Revision 1.66  2010/02/18 02:16:29  borland
 Added SDDS_SetError0() and modified SDDS_PrintErrors() to allow more easily composing
 multi-part error messages.  One side-effect is that stack traces come out in reverse
 order to previously.  Also modified SDDS_SetColumn() to use the new feature.

 Revision 1.65  2009/08/04 19:49:23  soliday
 Updated SDDS_VerifyPrintfFormat to not require the l (long) format specifier
 because this causes problems on 64bit linux that can't use %ld to format
 a 32bit SDDS_LONG variable.

 Revision 1.64  2006/12/14 22:14:39  soliday
 Changed the default setting of AutoCheckMode so that it does not
 perform checking by default. Most program never turn this off so it was
 slowing down the code. Added a call to SDDS_SaveLayout in
 SDDS_DeleteParameterFixedValues.

 Revision 1.63  2006/12/07 16:42:35  soliday
 Added SDDS_GetToken2 which is now used by SDDS_ScanData2 when reading
 ascii array data.

 Revision 1.62  2006/08/31 15:08:02  soliday
 Updated to work with SDDS2

 Revision 1.61  2006/05/02 16:53:22  soliday
 Fixed problems for WIN32

 Revision 1.60  2006/03/16 18:50:29  soliday
 Added the SDDS_BreakIntoLockedFile function.

 Revision 1.59  2005/11/14 19:15:50  soliday
 Updated SDDS_MakePointerArrayRecursively to fix an array bounds write error.

 Revision 1.58  2005/11/04 22:46:53  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.57  2004/02/27 21:14:32  soliday
 Modified SDDS_RegisterProgramName

 Revision 1.56  2003/10/17 16:12:30  soliday
 Added SDDS_VerifyColumnExists, SDDS_VerifyParameterExists,
 and SDDS_VerifyArrayExists.

 Revision 1.55  2003/09/23 21:59:09  soliday
 Added the exclude mode in the match columns, parameters, and array function.

 Revision 1.54  2003/09/11 21:41:09  soliday
 Fixed issue with FIND_SPECIFIED_TYPE.

 Revision 1.53  2003/09/02 18:01:44  soliday
 Cleaned up code for Linux.

 Revision 1.52  2003/08/28 15:07:50  soliday
 Added missing lockf declaration for OSX

 Revision 1.51  2003/01/08 19:32:18  borland
 Changed syntax for binaryIndexSearch() calls.
 Added some error reporting in SDDS_binary.c (flush buffer).

 Revision 1.50  2002/08/14 16:34:50  soliday
 Added Open License

 Revision 1.49  2002/07/25 00:11:17  borland
 SDDS_*PrintTypedValue now uses SDDS_PRINT_BUFLEN for buffer length instead
 of SDDS_MAXLINE.

 Revision 1.48  2002/06/15 02:27:37  borland
 Added SDDS_ForceInactive().

 Revision 1.47  2002/02/21 15:39:20  borland
 Fixed potential memory leak in SDDS_CheckColumn()

 Revision 1.46  2002/01/28 20:30:34  soliday
 Shang noticed the array data was not being freed. This is fixed now.

 Revision 1.45  2002/01/15 22:50:45  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.44  2000/11/19 19:02:28  borland
 Fixed bug in SDDS_FreeArray and added SDDS_FreePointerArray.

 Revision 1.43  2000/09/19 20:13:23  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.42  2000/06/15 21:09:31  soliday
 Added memory freeing statements that were suggested by Sev Binello.

 Revision 1.41  2000/04/11 18:07:05  borland
 Added a more informative message for SDDS_CheckDataStructureSize.

 Revision 1.40  2000/03/30 21:26:06  soliday
 SDDS_IdentifyType no longer accepts partial names because of possible future
 changes.

 Revision 1.39  2000/03/30 18:03:52  soliday
 SDDS_IdentifyType now accepts partial names.

 Revision 1.38  2000/02/25 23:17:27  evans
 Added SDDS_Free.  Memory allocated with SDDS_Malloc and others should
 be freed with SDDS_Free.  It is necessary for WIN32 when degugging
 SDDS applications using the release version of the SDDS library
 because the memory allocation models for debug and release are
 inconsistent.

 Revision 1.37  2000/01/18 20:09:21  soliday
 Added support for ZLIB.

 Revision 1.36  1999/11/19 21:38:41  borland
 SDDS_CutOutComments now returns immediately if the string or comment
 character are blank.

 Revision 1.35  1999/10/27 18:32:59  borland
 Will no longer try to realloc 0 bytes.

 Revision 1.34  1999/07/29 21:22:46  borland
 Removed a memory leak.

 Revision 1.33  1999/07/22 15:37:31  soliday
 Uses macros for fopen mode.

 Revision 1.32  1999/07/08 21:56:42  soliday
 Changed defined(WIN32) to defined(_WIN32)

 Revision 1.31  1999/06/01 19:09:38  borland
 Added SDDS_Calloc.

 Revision 1.30  1999/05/25 18:58:59  soliday
 Removed compiler warning on linux.

 Revision 1.29  1999/05/14 02:41:21  borland
 One change to make SDDS_CutOutComments more robust, one to quiet an
 apparently bogus purify complaint about UMR.

 Revision 1.28  1999/04/14 14:07:28  borland
 Added a conditional to eliminate reference to unistd.h on WIN32.

 Revision 1.27  1999/04/14 13:58:06  borland
 Fixed some function types and returns.

 Revision 1.26  1999/02/19 22:53:16  borland
 Added support for SDDS_ANY_INTEGER_TYPE to SDDS_CheckXXX procedures.

 Revision 1.25  1998/08/11 18:32:15  borland
 SDDS_Get*Definition procedures now use SDDS_Get*Index procedures to
 search for pre-existing entities.  Faster than old linear search.

 Revision 1.24  1998/06/24 19:17:05  borland
 SDDS_FreeStringArray now sets individual string pointers to NULL
 after freeing memory.

 Revision 1.23  1997/12/19 16:58:41  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.22  1997/10/29 21:24:15  borland
 Fixed problems that occured when an application attempted to copy
 rows into a dataset that has no columns defined.

 Revision 1.21  1997/08/25 14:43:34  borland
 Added test for existence of F_TEST macro, so that code will compile on
 systems that don't support file locking.  Also fixed incorrect arguments
 to an instance of va_start.

 Revision 1.20  1996/11/13 00:56:40  borland
 Added conditinoal statements for printing registeredProgramName to avoid
 Solaris problems with *printf.

 Revision 1.19  1996/08/31 22:59:42  borland
 Fixed bug in fgetsSkipCommentsResize that caused a big memory leak when
 the file ended without a newline.

 * Revision 1.18  1996/07/05  15:36:47  borland
 * Modified SDDS_SprintTypedValue and SDDS_PrintTypedValue to
 * include mode argument; now support omission of quotes around
 * string data.  SDDS_PrintTypedValue now calls SDDS_SprintTypedValue
 * to format string data.
 *
 * Revision 1.17  1996/03/27  17:50:20  borland
 * Added SDDS_GetErrorMessages() routine.
 *
 * Revision 1.16  1996/03/21  16:38:43  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.15  1996/03/12  04:00:59  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.14  1996/02/27  04:18:23  borland
 * Use memset() in SDDS_ZeroMemory for improved speed.
 *
 * Revision 1.13  1996/02/15  00:08:42  borland
 * Changed default format string for doubles from %22.15e to %21.15e, to
 * avoid troublesome blank space that was emitted (SDDS_SprintTypedValue
 * and SDDS_PrintTypedValue).
 *
 * Revision 1.12  1996/02/12  17:16:43  borland
 * Added SDDS_SetAutoCheckMode() plus static mode variable and if statement
 * in SDDS_CheckDataset().  This improves performance of programs when
 * checking is not needed (i.e., program is debugged).
 *
 * Revision 1.11  1996/01/21  00:15:20  borland
 * Fixed bug in file lock routines (wasn't closing file); using new version
 * of UnpackFopen(), with same functionality; SDDS_Terminate now seeks to
 * end of pipe for Solaris, to avoid stupid warning messages.
 *
 * Revision 1.10  1996/01/10  20:48:24  borland
 * Added SDDS_FileIsLocked() routine. Use it prior to fopen() calls to avoid
 * truncating the file.
 *
 * Revision 1.9  1996/01/10  17:21:06  borland
 * Added SDDS_LockFile() and code to use it in SDDS_InitializeCopy(),
 * SDDS_InitializeOutput(), and SDDS_InitializeAppend().
 *
 * Revision 1.8  1995/11/13  16:01:05  borland
 * Fixed bug in SDDS_FreeArray (was freeing something twice under some
 * circumstances).
 *
 * Revision 1.7  1995/11/08  04:23:56  borland
 * Added SDDS_FreeArray routine.
 *
 * Revision 1.6  1995/11/01  02:30:08  borland
 * SDDS_binary: Improved checks for large tables; won't check data from pipe,
 * since this doesn't work; checks return codes from fseek and ftell.
 * SDDS_utils: Change SDDS_GetToken to properly handle literal backslash at
 * the end of a quoted token.
 *
 * Revision 1.5  1995/09/20  22:49:15  borland
 * Changed SDDS_CheckX to interpret units=NULL as "don't care about units".
 *
 * Revision 1.4  1995/09/15  21:33:06  borland
 * Changed SDDS_Bomb to behave better for NULL message string.
 *
 * Revision 1.3  1995/09/06  14:09:10  saunders
 * First test release of SDDS1.5
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"
#include <ctype.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif
#if defined(__APPLE__)
/*lockf is not defined by unistd.h like it should be*/
int lockf(int filedes, int function, off_t size);
#endif

int32_t SDDS_PrintTypedValue(void *data, int32_t index, int32_t type, char *format, FILE *fp, uint32_t mode)
{
  char buffer[SDDS_PRINT_BUFLEN], *s;

  if (!data) {
    SDDS_SetError("Unable to print value--data pointer is NULL (SDDS_PrintTypedValue)");
    return(0);
  }
  if (!fp) {
    SDDS_SetError("Unable to print value--file pointer is NULL (SDDS_PrintTypedValue)");
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
    fprintf(fp, format?format:"%21.15e", *((double*)data+index));
    break;
  case SDDS_STRING:
    s = *((char**)data+index);
    if ((int32_t)strlen(s)>SDDS_PRINT_BUFLEN-3) {
      SDDS_SetError("Buffer size overflow (SDDS_PrintTypedValue)");
      return 0;
    }
    SDDS_SprintTypedValue(data, index, type, format, buffer, mode);
    fputs(buffer, fp);
    break;
  case SDDS_CHARACTER:
    fprintf(fp, format?format:"%c", *((char*)data+index));
    break;
  default:
    SDDS_SetError("Unable to print value--unknown data type (SDDS_PrintTypedValue)");
    return(0);
  }
  return(1);
}

int32_t SDDS_SprintTypedValue(void *data, int32_t index, int32_t type, char *format, char *buffer, uint32_t mode)
{
  char buffer2[SDDS_PRINT_BUFLEN], *s;
  short printed;
  
  if (!data) {
    SDDS_SetError("Unable to print value--data pointer is NULL (SDDS_SprintTypedValue)");
    return(0);
  }
  if (!buffer) {
    SDDS_SetError("Unable to print value--buffer pointer is NULL (SDDS_SprintTypedValue)");
    return(0);
  }
  switch (type) {
  case SDDS_SHORT:
    sprintf(buffer, format?format:"%hd", *((short*)data+index));
    break;
  case SDDS_USHORT:
    sprintf(buffer, format?format:"%hu", *((unsigned short*)data+index));
    break;
  case SDDS_LONG:
    sprintf(buffer, format?format:"%" PRId32, *((int32_t*)data+index));
    break;
  case SDDS_ULONG:
    sprintf(buffer, format?format:"%" PRIu32, *((uint32_t*)data+index));
    break;
  case SDDS_FLOAT:
    sprintf(buffer, format?format:"%15.8e", *((float*)data+index));
    break;
  case SDDS_DOUBLE:
    sprintf(buffer, format?format:"%21.15e", *((double*)data+index));
    break;
  case SDDS_STRING:
    s = *((char**)data+index);
    if ((int32_t)strlen(s)>SDDS_PRINT_BUFLEN-3) {
      SDDS_SetError("Buffer size overflow (SDDS_SprintTypedValue)");
      return(0);
    }
    if (!(mode&SDDS_PRINT_NOQUOTES)) {
      printed = 0;
      if (!s || SDDS_StringIsBlank(s))
        sprintf(buffer, "\"\"");
      else if (strchr(s, '"')) {
        strcpy(buffer2, s);
        SDDS_EscapeQuotes(buffer2, '"');
        if (SDDS_HasWhitespace(buffer2))
          sprintf(buffer, "\"%s\"", buffer2);
        else
          strcpy(buffer, buffer2);
      }
      else if (SDDS_HasWhitespace(s))
        sprintf(buffer, "\"%s\"", s);
      else {
        sprintf(buffer, format?format:"%s", s);
        printed = 1;
      }
      if (!printed) {
        sprintf(buffer2, format?format:"%s", buffer);
        strcpy(buffer, buffer2);
      }
    } else {
      sprintf(buffer, format?format:"%s", s);
    }
    break;
  case SDDS_CHARACTER:
    sprintf(buffer, format?format:"%c", *((char*)data+index));
    break;
  default:
    SDDS_SetError("Unable to print value--unknown data type (SDDS_SprintTypedValue)");
    return(0);
  }
  return(1);
}

static int32_t n_errors = 0;
static int32_t n_errors_max = 0;
static char **error_description = NULL;
static char *registeredProgramName = NULL;

void SDDS_RegisterProgramName(const char *name)
{
  if (name)
    SDDS_CopyString(&registeredProgramName, (char*)name);
  else
    registeredProgramName = NULL;
}

int32_t SDDS_NumberOfErrors()
{
  return(n_errors);
}

void SDDS_ClearErrors()
{
  n_errors = 0;
}

void SDDS_Bomb(char *message)
{
  if (registeredProgramName)
    fprintf(stderr, "Error (%s): %s\n", registeredProgramName, message?message:"?");
  else
    fprintf(stderr, "Error: %s\n", message?message:"?");
  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
  exit(1);
}

void SDDS_Warning(char *message)
{
  if (registeredProgramName)
    fprintf(stderr, "Warning (%s): %s\n", registeredProgramName, message?message:"?");
  else
    fprintf(stderr, "Warning: %s\n", message?message:"?");
}

void SDDS_SetError(char *error_text) 
{
  SDDS_SetError0(error_text);
  SDDS_SetError0("\n");
}


void SDDS_SetError0(char *error_text)
{
  if (n_errors>=n_errors_max) {
    if (!(error_description = SDDS_Realloc(error_description, (n_errors_max+=10)*sizeof(*error_description)))) {
      fputs("Error trying to allocate additional error description string (SDDS_SetError)\n", stderr);
      fprintf(stderr, "Most recent error text:\n%s\n", error_text);
      abort();
    }
  }
  if (!error_text)
    fprintf(stderr, "warning: error text is NULL (SDDS_SetError)\n");
  else {
    if (!SDDS_CopyString(&error_description[n_errors], error_text)) {
      fputs("Error trying to copy additional error description text (SDDS_SetError)\n", stderr);
      fprintf(stderr, "Most recent error text: %s\n", error_text);
      abort();
    }
    n_errors++;
  }
}

void SDDS_PrintErrors(FILE *fp, int32_t mode)
{
  int32_t i, depth;

  if (!n_errors)
    return;
  if (!fp) {
    n_errors = 0;
    return;
  }
  if (mode&SDDS_VERBOSE_PrintErrors)
    depth = n_errors;
  else
    depth = 1;
  if (registeredProgramName)
    fprintf(fp, "Error for %s:\n", registeredProgramName);
  else
    fputs("Error:\n", fp);
  if (!error_description)
    fprintf(stderr, "warning: internal error: error_description pointer is unexpectedly NULL\n");
  else
    for (i=0; i<depth; i++) {
      if (!error_description[i])
        fprintf(stderr, "warning: internal error: error_description[%" PRId32 "] is unexpectedly NULL\n",
                i);
      fprintf(fp, "%s", error_description[i]);
    }
  fflush(fp);
  n_errors = 0;
  if (mode&SDDS_EXIT_PrintErrors)
    exit(1);
}

char **SDDS_GetErrorMessages(int32_t *number, int32_t mode)
{
  int32_t i, depth;
  char **message;

  if (!number)
    return NULL;
  
  *number = 0;
  if (!n_errors) 
    return NULL;

  if (mode&SDDS_ALL_GetErrorMessages)
    depth = n_errors;
  else
    depth = 1;
  if (!(message = (char**)SDDS_Malloc(sizeof(*message)*depth))) 
    return NULL;
  if (!error_description) {
    fprintf(stderr, "warning: internal error: error_description pointer is unexpectedly NULL (SDDS_GetErrorMessages)\n");
    return NULL;
  } else {
    for (i=depth-1; i>=0; i--) {
      if (!error_description[i]) {
        fprintf(stderr, "internal error: error_description[%" PRId32 "] is unexpectedly NULL (SDDS_GetErrorMessages)\n", i);
        return NULL;
      }
      if (!SDDS_CopyString(message+i, error_description[i])) {
        fprintf(stderr, "unable to copy error message text (SDDS_GetErrorMessages)\n");
        return NULL;
      }
    }
  }
  *number = depth;
  return message;
}


/*static uint32_t AutoCheckMode = TABULAR_DATA_CHECKS ;*/
static uint32_t AutoCheckMode = 0x0000UL ;

uint32_t SDDS_SetAutoCheckMode(uint32_t newMode)
{
  uint32_t oldMode;
  oldMode = AutoCheckMode;
  AutoCheckMode = newMode;
  return oldMode;
}

int32_t SDDS_CheckDataset(SDDS_DATASET *SDDS_dataset, const char *caller)
{
  static char buffer[100];
  if (!SDDS_dataset) {
    sprintf(buffer, "NULL SDDS_DATASET pointer passed to %s", caller);
    SDDS_SetError(buffer);
    return(0);
  }
  return(1);
}

int32_t SDDS_CheckTabularData(SDDS_DATASET *SDDS_dataset, const char *caller)
{
  int32_t i;
  static char buffer[100];
  if (!(AutoCheckMode&TABULAR_DATA_CHECKS))
    return 1;
  if (SDDS_dataset->layout.n_columns && (!SDDS_dataset->row_flag || !SDDS_dataset->data)) {
    sprintf(buffer, "tabular data is invalid in %s (columns but no row flags or data array)", caller);
    SDDS_SetError(buffer);
    return(0);
  }
  if (SDDS_dataset->layout.n_columns==0 && SDDS_dataset->n_rows) {
    sprintf(buffer, "tabular data is invalid in %s (no columns present but nonzero row count)", caller);
    SDDS_SetError(buffer);
    return(0);
  }
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (!SDDS_dataset->data[i]) {
      sprintf(buffer, "tabular data is invalid in %s (null data pointer for column %" PRId32 ")", caller, i);
      SDDS_SetError(buffer);
      return(0);
    }
  }
  return(1);
}

void *SDDS_Calloc(size_t nelem, size_t elem_size)
{
  if (elem_size<=0)
    elem_size = 4;
  if (nelem<=0)
    nelem = 1;
  return calloc(nelem, elem_size);
}

void *SDDS_Malloc(size_t size)
{
  if (size<=0)
    size = 4;
  return malloc(size);
}

void SDDS_Free(void *mem)
{
  /* this is required so the free will be consistent with the malloc.
     On WIN32 the release (optimized) version of malloc is different
     from the debug (unoptimized) version, so debug programs freeing
     memory that was allocated by release, library routines encounter
     problems. */
    free(mem);
}

void *SDDS_Realloc(void *old_ptr, size_t new_size)
{
  /* this is required because some realloc's don't behave properly when asked to return a
   * pointer to 0 memory.  They return NULL.
   */
  if (new_size<=0)
    new_size = 4;
  /* this is required because some realloc's don't behave properly when given a NULL pointer */
  if (!old_ptr)
    return(SDDS_Malloc(new_size));
  else
    return(realloc(old_ptr, new_size));
}

int32_t SDDS_VerifyPrintfFormat(char *string, int32_t type)
{
  char *percent, *s;
  int32_t len, tmp;

  s = string;
  do {
    if ((percent = strchr(s, '%'))) {
      if (*(percent+1)!='%')
        break;
      s = percent+1;
    }
  } while (percent);
  if (!percent || !*++percent)
    return(0);

  string = percent;
  switch (type) {
  case SDDS_DOUBLE:
  case SDDS_FLOAT:
    if ((len = strcspn(string, "fegEG"))==strlen(string))
      return(0);
    if (len==0)
      return(1);
    if ((tmp=strspn(string, "-+.0123456789 "))<len)
      return(0);
    break;
  case SDDS_LONG:
    if ((len = strcspn(string, "d"))==strlen(string))
      return(0);
    /*    if (*(string+len-1)!='l')
          return(0);*/
    if (--len==0)
      return(1);
    if ((tmp=strspn(string, "-+.0123456789 "))<len)
      return(0);
    break;
  case SDDS_ULONG:
    if ((len = strcspn(string, "u"))==strlen(string))
      return(0);
    /*    if (*(string+len-1)!='l')
          return(0);*/
    if (--len==0)
      return(1);
    if ((tmp=strspn(string, "-+.0123456789 "))<len)
      return(0);
    break;
  case SDDS_SHORT:
    if ((len = strcspn(string, "d"))==strlen(string))
      return(0);
    if (*(string+len-1)!='h')
      return(0);
    if (--len==0)
      return(1);
    if ((tmp=strspn(string, "-+.0123456789 "))<len)
      return(0);
    break;
  case SDDS_USHORT:
    if ((len = strcspn(string, "u"))==strlen(string))
      return(0);
    if (*(string+len-1)!='h')
      return(0);
    if (--len==0)
      return(1);
    if ((tmp=strspn(string, "-+.0123456789 "))<len)
      return(0);
    break;
  case SDDS_STRING:
    if ((len = strcspn(string, "s"))==strlen(string))
      return(0);
    if (len==0)
      return(1);
    if ((tmp=strspn(string, "-0123456789"))<len)
      return(0);
    break;
  case SDDS_CHARACTER:
    if ((len = strcspn(string, "c"))==strlen(string))
      return(0);
    if (len!=0)
      return(0);
    break;
  default:
    return(0);
  }
  /* no errors found--its probably okay */
  return(1);
}

int32_t SDDS_CopyString(char **target, char *source)
{
  if (!source)
    *target = NULL;
  else {
    if (!(*target=SDDS_Malloc(sizeof(**target)*(strlen(source)+1))))
      return(0);
    strcpy(*target, source);
  }
  return(1);
}

ASSOCIATE_DEFINITION *SDDS_GetAssociateDefinition(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  ASSOCIATE_DEFINITION *assdef;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetAssociateDefinition"))
    return(NULL);
  if (!name) {
    SDDS_SetError("Unable to get associate definition--name is NULL (SDDS_GetAssociateDefinition)");
    return(NULL);
  }
  for (i=0; i<SDDS_dataset->layout.n_associates; i++) {
    if (strcmp(SDDS_dataset->layout.associate_definition[i].name, name)==0) {
      if (!SDDS_CopyAssociateDefinition(&assdef, SDDS_dataset->layout.associate_definition+i)) {
        SDDS_SetError("Unable to get associate definition--copy failure  (SDDS_GetAssociateDefinition)");
        return(NULL);
      }
      return(assdef);
    }
  }
  return(NULL);
}

ASSOCIATE_DEFINITION *SDDS_CopyAssociateDefinition(ASSOCIATE_DEFINITION **target, ASSOCIATE_DEFINITION *source)
{
  if (!source)
    return(*target = NULL);
  if (!(*target = (ASSOCIATE_DEFINITION *)SDDS_Malloc(sizeof(**target))) ||
      !SDDS_CopyString(&(*target)->name, source->name) ||
      !SDDS_CopyString(&(*target)->filename, source->filename) ||
      !SDDS_CopyString(&(*target)->path, source->path) ||
      !SDDS_CopyString(&(*target)->description, source->description) ||
      !SDDS_CopyString(&(*target)->contents, source->contents) )
    return(NULL);
  (*target)->sdds = source->sdds;
  return(*target);
}

int32_t SDDS_FreeAssociateDefinition(ASSOCIATE_DEFINITION *source)
{
  if (!source->name)
    return(0);
  free(source->name);
  if (!source->filename)
    return(0);
  free(source->filename);
  if (source->path) free(source->path);
  if (source->description) free(source->description);
  if (source->contents) free(source->contents);
  SDDS_ZeroMemory(source, sizeof(*source));
  free(source);
  return(1);
}

COLUMN_DEFINITION *SDDS_GetColumnDefinition(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  COLUMN_DEFINITION *coldef;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnDefinition"))
    return(NULL);
  if (!name) {
    SDDS_SetError("Unable to get column definition--name is NULL (SDDS_GetColumnDefinition)");
    return(NULL);
  }
  if ((i=SDDS_GetColumnIndex(SDDS_dataset, name))<0)
    return NULL;
  if (!SDDS_CopyColumnDefinition(&coldef, SDDS_dataset->layout.column_definition+i)) {
    SDDS_SetError("Unable to get column definition--copy failure  (SDDS_GetColumnDefinition)");
    return(NULL);
  }
  return(coldef);
}

COLUMN_DEFINITION *SDDS_CopyColumnDefinition(COLUMN_DEFINITION **target, COLUMN_DEFINITION *source)
{
  if (!target)
    return NULL;
  if (!source)
    return(*target = NULL);
  if (!(*target=(COLUMN_DEFINITION *)SDDS_Malloc(sizeof(**target))) ||
      !SDDS_CopyString(&(*target)->name, source->name) ||
      !SDDS_CopyString(&(*target)->symbol, source->symbol) ||
      !SDDS_CopyString(&(*target)->units, source->units) ||
      !SDDS_CopyString(&(*target)->description, source->description) ||
      !SDDS_CopyString(&(*target)->format_string, source->format_string))
    return(NULL);
  (*target)->type = source->type;
  (*target)->field_length = source->field_length;
  (*target)->definition_mode = source->definition_mode;
  (*target)->memory_number = source->memory_number;
  return(*target);
}

int32_t SDDS_FreeColumnDefinition(COLUMN_DEFINITION *source)
{
  if (!source || !source->name)
    return(0);
  free(source->name);
  if (source->symbol) free(source->symbol);
  if (source->units) free(source->units);
  if (source->description) free(source->description);
  if (source->format_string) free(source->format_string);
  SDDS_ZeroMemory(source, sizeof(*source));
  free(source);
  return(1);
}

PARAMETER_DEFINITION *SDDS_GetParameterDefinition(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  PARAMETER_DEFINITION *pardef;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterDefinition"))
    return(NULL);
  if (!name) {
    SDDS_SetError("Unable to get parameter definition--name is NULL (SDDS_GetParameterDefinition)");
    return(NULL);
  }
  if ((i=SDDS_GetParameterIndex(SDDS_dataset, name))<0)
    return NULL;
  if (!SDDS_CopyParameterDefinition(&pardef, SDDS_dataset->layout.parameter_definition+i)) {
    SDDS_SetError("Unable to get parameter definition--copy failure  (SDDS_GetParameterDefinition)");
    return(NULL);
  }
  return(pardef);
}

PARAMETER_DEFINITION *SDDS_CopyParameterDefinition(PARAMETER_DEFINITION **target, PARAMETER_DEFINITION *source)
{
  if (!target)
    return NULL;
  if (!source)
    return(*target = NULL);
  if (!(*target=(PARAMETER_DEFINITION *)SDDS_Malloc(sizeof(**target))) ||
      !SDDS_CopyString(&(*target)->name, source->name) ||
      !SDDS_CopyString(&(*target)->symbol, source->symbol) ||
      !SDDS_CopyString(&(*target)->units, source->units) ||
      !SDDS_CopyString(&(*target)->description, source->description) ||
      !SDDS_CopyString(&(*target)->format_string, source->format_string) ||
      !SDDS_CopyString(&(*target)->fixed_value, source->fixed_value))
    return(NULL);
  (*target)->type = source->type;
  (*target)->definition_mode = source->definition_mode;
  (*target)->memory_number = source->memory_number;
  return(*target);
}

int32_t SDDS_FreeParameterDefinition(PARAMETER_DEFINITION *source)
{
  if (!source || !source->name)
    return(0);
  free(source->name);
  if (source->symbol) free(source->symbol);
  if (source->units) free(source->units);
  if (source->description) free(source->description);
  if (source->format_string) free(source->format_string);
  if (source->fixed_value) free(source->fixed_value);
  SDDS_ZeroMemory(source, sizeof(*source));
  free(source);
  return(1);
}

ARRAY_DEFINITION *SDDS_GetArrayDefinition(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  ARRAY_DEFINITION *arraydef;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayDefinition"))
    return(NULL);
  if (!name) {
    SDDS_SetError("Unable to get array definition--name is NULL (SDDS_GetArrayDefinition)");
    return(NULL);
  }
  if ((i=SDDS_GetArrayIndex(SDDS_dataset, name))<0)
    return NULL;
  if (!SDDS_CopyArrayDefinition(&arraydef, SDDS_dataset->layout.array_definition+i)) {
    SDDS_SetError("Unable to get array definition--copy failure  (SDDS_GetArrayDefinition)");
    return(NULL);
  }
  return(arraydef);
}


ARRAY_DEFINITION *SDDS_CopyArrayDefinition(ARRAY_DEFINITION **target, ARRAY_DEFINITION *source)
{
  if (!target)
    return NULL;
  if (!source)
    return(*target = NULL);
  if (!(*target=(ARRAY_DEFINITION *)SDDS_Malloc(sizeof(**target))) ||
      !SDDS_CopyString(&(*target)->name, source->name) ||
      !SDDS_CopyString(&(*target)->symbol, source->symbol) ||
      !SDDS_CopyString(&(*target)->units, source->units) ||
      !SDDS_CopyString(&(*target)->description, source->description) ||
      !SDDS_CopyString(&(*target)->format_string, source->format_string) ||
      !SDDS_CopyString(&(*target)->group_name, source->group_name))
    return(NULL);
  (*target)->type = source->type;
  (*target)->field_length = source->field_length;
  (*target)->dimensions = source->dimensions;
  return(*target);
}

int32_t SDDS_FreeArrayDefinition(ARRAY_DEFINITION *source)
{
  if (!source)
    return(0);
  if (source->name) free(source->name);
  if (source->symbol) free(source->symbol);
  if (source->units) free(source->units);
  if (source->description) free(source->description);
  if (source->format_string) free(source->format_string);
  if (source->group_name) free(source->group_name);
  SDDS_ZeroMemory(source, sizeof(*source));
  free(source);
  source = NULL;
  return(1);
}

int SDDS_CompareIndexedNames(void *s1, void *s2)
{
  return strcmp(((SORTED_INDEX*)s1)->name, ((SORTED_INDEX*)s2)->name);
}

/* This routine is used with qsort.  Use const void * to avoid warning
 * message from SUN Solaris compiler.
 */
int SDDS_CompareIndexedNamesPtr(const void *s1, const void *s2)
{
  return strcmp( (*((SORTED_INDEX**)s1))->name, (*((SORTED_INDEX**)s2))->name );
}

int32_t SDDS_GetColumnIndex(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  SORTED_INDEX key;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnIndex"))
    return(-1);
  if (!name) {
    SDDS_SetError("Unable to get column index--name is NULL (SDDS_GetColumnIndex)");
    return(-1);
  }
  key.name = name;
  if ((i = binaryIndexSearch((void**)SDDS_dataset->layout.column_index,
                             SDDS_dataset->layout.n_columns,
                             &key, SDDS_CompareIndexedNames, 0))<0)
    return -1;
  return SDDS_dataset->layout.column_index[i]->index;
}

int32_t SDDS_GetParameterIndex(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  SORTED_INDEX key;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterIndex"))
    return(-1);
  if (!name) {
    SDDS_SetError("Unable to get parameter index--name is NULL (SDDS_GetParameterIndex)");
    return(-1);
  }
  key.name = name;
  if ((i=binaryIndexSearch((void**)SDDS_dataset->layout.parameter_index,
                           SDDS_dataset->layout.n_parameters,
                           &key, SDDS_CompareIndexedNames, 0))<0)
    return -1;
  return SDDS_dataset->layout.parameter_index[i]->index;
}

int32_t SDDS_GetArrayIndex(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  SORTED_INDEX key;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayIndex"))
    return(-1);
  if (!name) {
    SDDS_SetError("Unable to get array index--name is NULL (SDDS_GetArrayIndex)");
    return(-1);
  }
  key.name = name;
  if ((i=binaryIndexSearch((void**)SDDS_dataset->layout.array_index,
                           SDDS_dataset->layout.n_arrays,
                           &key, SDDS_CompareIndexedNames, 0))<0)
    return -1;
  return SDDS_dataset->layout.array_index[i]->index;
}


int32_t SDDS_GetAssociateIndex(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t i;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetAssociateIndex"))
    return(-1);
  if (!name) {
    SDDS_SetError("Unable to get associate index--name is NULL (SDDS_GetAssociateIndex)");
    return(-1);
  }
  for (i=0; i<SDDS_dataset->layout.n_associates; i++) {
    if (strcmp(SDDS_dataset->layout.associate_definition[i].name, name)==0)
      return(i);
  }
  return(-1);
}

int32_t SDDS_HasWhitespace(char *string)
{
  if (!string)
    return(0);
  while (*string) {
    if (isspace(*string))
      return(1);
    string++;
  }
  return(0);
}


char *fgetsSkipComments(char *s, int32_t slen, FILE *fp,
                        char skip_char    /* ignore lines that begin with this character */
                        )
{
  while (fgets(s, slen, fp)) {
    if (s[0]!=skip_char) {
      SDDS_CutOutComments(s, skip_char);
      return(s);
    } else if (s[1]=='#') {
      SDDS_ParseSpecialComments(s+2);
    }
  } 
  return(NULL);
}

char *fgetsSkipCommentsResize(char **s, int32_t *slen, FILE *fp,
                              char skip_char    /* ignore lines that begin with this character */
                              )
{
  int32_t spaceLeft, length, newLine;
  char *sInsert, *fgetsReturn;

  sInsert = *s;
  spaceLeft = *slen;
  newLine = 1;
  while ((fgetsReturn=fgets(sInsert, spaceLeft, fp))) {
    if (newLine && sInsert[0]=='!')
      continue;
    SDDS_CutOutComments(sInsert, skip_char);
    length = strlen(sInsert);
    if (sInsert[length-1]!='\n' && !feof(fp)) {
      /* buffer wasn't int32_t enough to get the whole line.  Resize and add more data. */
      spaceLeft = *slen;
      *slen = *slen*2;
      *s = SDDS_Realloc(*s, sizeof(**s)**slen);
      sInsert = *s + strlen(*s);
      newLine = 0;
    }
    else 
      break;
  }
  if (!fgetsReturn)
    return NULL;
  return(*s);
}

#if defined(zLib)
char *fgetsGZipSkipComments(char *s, int32_t slen, gzFile *gzfp,
                        char skip_char    /* ignore lines that begin with this character */
                        )
{
  while (gzgets(gzfp, s, slen)) {
    if (s[0]!=skip_char) {
      SDDS_CutOutComments(s, skip_char);
      return(s);
    } else if (s[1]=='#') {
      SDDS_ParseSpecialComments(s+2);
    }
  } 
  return(NULL);
}

char *fgetsGZipSkipCommentsResize(char **s, int32_t *slen, gzFile *gzfp,
                              char skip_char    /* ignore lines that begin with this character */
                              )
{
  int32_t spaceLeft, length, newLine;
  char *sInsert, *fgetsReturn;

  sInsert = *s;
  spaceLeft = *slen;
  newLine = 1;
  while ((fgetsReturn=gzgets(gzfp, sInsert, spaceLeft))) {
    if (newLine && sInsert[0]=='!')
      continue;
    SDDS_CutOutComments(sInsert, skip_char);
    length = strlen(sInsert);
    if (sInsert[length-1]!='\n' && !gzeof(gzfp)) {
      /* buffer wasn't int32_t enough to get the whole line.  Resize and add more data. */
      spaceLeft = *slen;
      *slen = *slen*2;
      *s = SDDS_Realloc(*s, sizeof(**s)**slen);
      sInsert = *s + strlen(*s);
      newLine = 0;
    }
    else 
      break;
  }
  if (!fgetsReturn)
    return NULL;
  return(*s);
}
#endif

void SDDS_CutOutComments(char *s, char cc)
{
  int32_t length, hasNewline;
  char *s0;

  if (!cc || !s)
    return;
  
  hasNewline = 0;
  length = strlen(s);
  if (s[length-1]=='\n')
    hasNewline = 1;

  if (*s==cc) {
    /* check for special information */
    if (*(s+1)=='#')
      SDDS_ParseSpecialComments(s+2);
    *s = 0;
    return;
  }
  s0 = s;
  while (*s) {
    if (*s=='"') {
      while (*++s && *s!='"')
        ;
      if (!*s)
        return;
      s++;
      continue;
    }
    if (*s==cc) {
      if (s!=s0 && *(s-1)=='\\')
        strcpy(s-1, s);
      else {
        if (hasNewline) {
          *s = '\n';
          *(s+1) = 0;
        }
        else
          *s = 0;
        return;
      }
    }
    s++;
  }
}

int32_t SDDS_GetToken(char *s, char *buffer, int32_t buflen)
{
  char *ptr0, *ptr1, *escptr, *temp;
  
  /* save the pointer to the head of the string */
  ptr0 = s;

  /* skip leading white-space */  
  while (isspace(*s))
    s++;
  if (*s==0) 
    return(-1);
  ptr1 = s;

  if (*s=='"') {
    /* if quoted string, skip to next quotation mark */
    ptr1 = s+1;    /* beginning of actual token */
    do {
      s++;
      escptr = NULL;
      if (*s=='\\' && *(s+1)=='\\') {
        /* skip and remember literal \ (indicated by \\ in the string) */
        escptr = s+1;
        s += 2;
      }
    } while (*s && (*s!='"' || (*(s-1)=='\\' && (s-1)!=escptr)));
    /* replace trailing quotation mark with a space */
    if (*s=='"')
      *s = ' ';
  }
  else {
    /* skip to first white-space following token */
    do {
      s++;
      /* imbedded quotation marks are handled here */
      if (*s=='"' && *(s-1)!='\\') {
        while (*++s && !(*s=='"' && *(s-1)!='\\'))
          ;
      }
    } while (*s && !isspace(*s));
  }

  if ((int32_t)(s-ptr1)>=buflen)
    return(-1);
  strncpy(buffer, ptr1, s-ptr1);
  buffer[s-ptr1]=0;
  
  /* update the original string to delete the token */
  temp = malloc(sizeof(char) * (strlen(s) + 1));
  strcpy(temp, s);
  strcpy(ptr0, temp);
  free(temp);

  /* return the string length */
  return((int32_t)(s-ptr1));
}

int32_t SDDS_GetToken2(char *s, char **st, int32_t *strlength, char *buffer, int32_t buflen)
{
  char *ptr0, *ptr1, *escptr;
  
  /* save the pointer to the head of the string */
  ptr0 = s;

  /* skip leading white-space */  
  while (isspace(*s))
    s++;
  if (*s==0) 
    return(-1);
  ptr1 = s;

  if (*s=='"') {
    /* if quoted string, skip to next quotation mark */
    ptr1 = s+1;    /* beginning of actual token */
    do {
      s++;
      escptr = NULL;
      if (*s=='\\' && *(s+1)=='\\') {
        /* skip and remember literal \ (indicated by \\ in the string) */
        escptr = s+1;
        s += 2;
      }
    } while (*s && (*s!='"' || (*(s-1)=='\\' && (s-1)!=escptr)));
    /* replace trailing quotation mark with a space */
    if (*s=='"')
      *s = ' ';
  }
  else {
    /* skip to first white-space following token */
    do {
      s++;
      /* imbedded quotation marks are handled here */
      if (*s=='"' && *(s-1)!='\\') {
        while (*++s && !(*s=='"' && *(s-1)!='\\'))
          ;
      }
    } while (*s && !isspace(*s));
  }

  if ((int32_t)(s-ptr1)>=buflen)
    return(-1);
  strncpy(buffer, ptr1, s-ptr1);
  buffer[s-ptr1]=0;
  
  /* update the original string to delete the token */
  *st += s-ptr0;
  *strlength -= s-ptr0;

  /* return the string length including whitespace */
  return((int32_t)(s-ptr1));
}

int32_t SDDS_PadToLength(char *string, int32_t length)
{
  int32_t i;
  if (!string || (i=strlen(string))>length)
    return(0);
  while (i<length)
    string[i++] = ' ';
  string[i] = 0;
  return(1);
}

void SDDS_EscapeQuotes(char *s, char quote_char)
{
  char *ptr, *bptr;
  static char *buffer=NULL;
  
  ptr = s;
  buffer = trealloc(buffer, sizeof(*buffer)*(4*(strlen(s)+1)));
  bptr = buffer;

  while (*ptr) {
    if (*ptr==quote_char && (ptr==s || *(ptr-1)!='\\'))
      *bptr++ = '\\';
    *bptr++ = *ptr++;
  }
  *bptr = 0;
  strcpy(s, buffer);
}

void SDDS_UnescapeQuotes(char *s, char quote_char)
{
  char *ptr;
  ptr = s;
  while (*ptr) {
    if (*ptr==quote_char && ptr!=s && *(ptr-1)=='\\')
      strcpy(ptr-1, ptr);
    else
      ptr++;
  }
}

void SDDS_EscapeCommentCharacters(char *string, char cc)
{
  char *ptr, *s0;
  s0 = string;
  while (*string) {
    if (*string==cc && (string==s0 || *(string-1)!='\\')) {
      ptr = string+strlen(string)+1;
      while (ptr!=string) {
        *ptr = *(ptr-1);
        ptr--;
      }
      *string++ = '\\';
    }
    string++;
  }
}

int32_t SDDS_ZeroMemory(void *mem, int32_t n_bytes)
{
  if (mem) {
    memset(mem, 0, n_bytes);
    return 1;
  }
  return 0;
  
  /*
    char *c;

    if (!(c = (char*)mem))
    return(0);
    while (n_bytes--)
    *c++ = 0;
    return(1);
    */
}

int32_t SDDS_SetMemory(void *mem, int32_t n_elements, int32_t data_type, ...)
/* usage is SDDS_SetMemory(ptr, n_elements, type, start_value, increment_value) */
{
  va_list argptr;
  int32_t i, retval;
  short short_val, short_dval, *short_ptr;
  unsigned short ushort_val, ushort_dval, *ushort_ptr;
  int32_t long_val, long_dval, *long_ptr;
  uint32_t ulong_val, ulong_dval, *ulong_ptr;
  float float_val, float_dval, *float_ptr;
  double double_val, double_dval, *double_ptr;
  char char_val, *char_ptr;

  retval = 1;
  va_start(argptr, data_type);
  switch (data_type) {
  case SDDS_SHORT:
    short_val  = (short)va_arg(argptr, int);
    short_dval = (short)va_arg(argptr, int);
    short_ptr = (short*)mem;
    for (i=0; i<n_elements; i++, short_val+=short_dval)
      *short_ptr++ = short_val;
    break;
  case SDDS_USHORT:
    ushort_val  = (unsigned short)va_arg(argptr, int);
    ushort_dval = (unsigned short)va_arg(argptr, int);
    ushort_ptr = (unsigned short*)mem;
    for (i=0; i<n_elements; i++, ushort_val+=ushort_dval)
      *ushort_ptr++ = ushort_val;
    break;
  case SDDS_LONG:
    long_val  = (int32_t)va_arg(argptr, int32_t);
    long_dval = (int32_t)va_arg(argptr, int32_t);
    long_ptr = (int32_t*)mem;
    for (i=0; i<n_elements; i++, long_val+=long_dval)
      *long_ptr++ = long_val;
    break;
  case SDDS_ULONG:
    ulong_val  = (uint32_t)va_arg(argptr, uint32_t);
    ulong_dval = (uint32_t)va_arg(argptr, uint32_t);
    ulong_ptr = (uint32_t*)mem;
    for (i=0; i<n_elements; i++, ulong_val+=ulong_dval)
      *ulong_ptr++ = ulong_val;
    break;
  case SDDS_FLOAT:
    float_val  = (float)va_arg(argptr, double);
    float_dval = (float)va_arg(argptr, double);
    float_ptr = (float*)mem;
    for (i=0; i<n_elements; i++, float_val+=float_dval)
      *float_ptr++ = float_val;
    break;
  case SDDS_DOUBLE:
    double_val  = (double)va_arg(argptr, double);
    double_dval = (double)va_arg(argptr, double);
    double_ptr = (double*)mem;
    for (i=0; i<n_elements; i++, double_val+=double_dval)
      *double_ptr++ = double_val;
    break;
  case SDDS_CHARACTER:
    char_val  = (char)va_arg(argptr, int);
    short_dval = (short)va_arg(argptr, int);
    char_ptr = (char*)mem;
    for (i=0; i<n_elements; i++, char_val+=short_dval)
      *char_ptr++ = char_val;
    break;
  default:
    SDDS_SetError("Unable to set memory--unknown or invalid data type (SDDS_SetMemory)");
    retval = 0;
    break;
  }
  va_end(argptr);
  return(retval);
}

int32_t SDDS_GetColumnType(SDDS_DATASET *SDDS_dataset, int32_t index)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnType"))
    return(0);
  if (index<0 || index>=SDDS_dataset->layout.n_columns) {
    SDDS_SetError("Unable to get column type--column index is out of range (SDDS_GetColumnType)");
    return(0);
  }
  return(SDDS_dataset->layout.column_definition[index].type);
}

int32_t SDDS_GetNamedColumnType(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t index;
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0 || index>=SDDS_dataset->layout.n_columns) {
    SDDS_SetError("Unable to get column type--column index is out of range (SDDS_GetNamedColumnType)");
    return(0);
  }
  return(SDDS_dataset->layout.column_definition[index].type);
}

int32_t SDDS_GetArrayType(SDDS_DATASET *SDDS_dataset, int32_t index)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayType"))
    return(0);
  if (index<0 || index>=SDDS_dataset->layout.n_arrays) {
    SDDS_SetError("Unable to get array type--array index is out of range (SDDS_GetArrayType)");
    return(0);
  }
  return(SDDS_dataset->layout.array_definition[index].type);
}

int32_t SDDS_GetNamedArrayType(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t index;
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, name))<0 || index>=SDDS_dataset->layout.n_arrays) {
    SDDS_SetError("Unable to get array type--array index is out of range (SDDS_GetNamedArrayType)");
    return(0);
  }
  return(SDDS_dataset->layout.array_definition[index].type);
}

int32_t SDDS_GetParameterType(SDDS_DATASET *SDDS_dataset, int32_t index)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterType"))
    return(0);
  if (index<0 || index>=SDDS_dataset->layout.n_parameters) {
    SDDS_SetError("Unable to get parameter type--parameter index is out of range (SDDS_GetParameterType)");
    return(0);
  }
  return(SDDS_dataset->layout.parameter_definition[index].type);
}

int32_t SDDS_GetNamedParameterType(SDDS_DATASET *SDDS_dataset, char *name)
{
  int32_t index;
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))<0 || index>=SDDS_dataset->layout.n_parameters) {
    SDDS_SetError("Unable to get parameter type--parameter index is out of range (SDDS_GetNamedParameterType)");
    return(0);
  }
  return(SDDS_dataset->layout.parameter_definition[index].type);
}

int32_t SDDS_GetTypeSize(int32_t type)
{
  if (!SDDS_VALID_TYPE(type))
    return(-1);
  return(SDDS_type_size[type-1]);
}

char *SDDS_GetTypeName(int32_t type)
{
  char *name;
  if (!SDDS_VALID_TYPE(type))
    return NULL;
  if (!SDDS_CopyString(&name, SDDS_type_name[type-1]))
    return NULL;
  return name;
}

int32_t SDDS_IdentifyType(char *typeName)
{
  int32_t i;
  for (i=0; i<SDDS_NUM_TYPES; i++)
    if (strcmp(typeName, SDDS_type_name[i])==0)
      return i+1;
  return 0;
}

void SDDS_RemovePadding(char *s)
{
  char *ptr;
  ptr = s;
  while (isspace(*ptr))
    ptr++;
  if (ptr!=s)
    strcpy(s, ptr);
  ptr = s+strlen(s)-1;
  while (isspace(*ptr))
    *ptr-- = 0;
}

int32_t SDDS_StringIsBlank(char *s)
{
  if (!s)
    return 1;
  while (*s)
    if (!isspace(*s++))
      return(0);
  return(1);
}

int32_t SDDS_ColumnIsOfInterest(SDDS_DATASET *SDDS_dataset, char *name) 
{
  int32_t i;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ColumnIsOfInterest"))
    return -1;
  if (!SDDS_dataset->column_flag) 
    return 0;
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (SDDS_dataset->column_flag[i] && strcmp(name, SDDS_dataset->layout.column_definition[i].name)==0)
      return 1;
  }
  return 0;
}

char **SDDS_GetColumnNames(SDDS_DATASET *SDDS_dataset, int32_t *number)
{
  int32_t i;
  char **name;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetColumnNames"))
    return(NULL);
  *number = 0;
  if (!(name=(char**)SDDS_Malloc(sizeof(*name)*SDDS_dataset->layout.n_columns))) {
    SDDS_SetError("Unable to get column names--allocation failure (SDDS_GetColumnNames)");
    return(NULL);
  }
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (!SDDS_dataset->column_flag || SDDS_dataset->column_flag[i]) {
      if (!SDDS_CopyString(name+ *number, SDDS_dataset->layout.column_definition[i].name)) {
        free(name);
        return(NULL);
      }
      *number += 1;
    }
  }
  return(name);
}

char **SDDS_GetParameterNames(SDDS_DATASET *SDDS_dataset, int32_t *number)
{
  int32_t i;
  char **name;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterNames"))
    return(NULL);
  *number = SDDS_dataset->layout.n_parameters;
  if (!(name=(char**)SDDS_Malloc(sizeof(*name)*SDDS_dataset->layout.n_parameters))) {
    SDDS_SetError("Unable to get parameter names--allocation failure (SDDS_GetParameterNames)");
    return(NULL);
  }
  for (i=0; i<SDDS_dataset->layout.n_parameters; i++) {
    if (!SDDS_CopyString(name+i, SDDS_dataset->layout.parameter_definition[i].name)) {
      free(name);
      return(NULL);
    }
  }
  return(name);
}

char **SDDS_GetArrayNames(SDDS_DATASET *SDDS_dataset, int32_t *number)
{
  int32_t i;
  char **name;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetArrayNames"))
    return(NULL);
  *number = SDDS_dataset->layout.n_arrays;
  if (!(name=(char**)SDDS_Malloc(sizeof(*name)*SDDS_dataset->layout.n_arrays))) {
    SDDS_SetError("Unable to get array names--allocation failure (SDDS_GetArrayNames)");
    return(NULL);
  }
  for (i=0; i<SDDS_dataset->layout.n_arrays; i++) {
    if (!SDDS_CopyString(name+i, SDDS_dataset->layout.array_definition[i].name)) {
      free(name);
      return(NULL);
    }
  }
  return(name);
}

char **SDDS_GetAssociateNames(SDDS_DATASET *SDDS_dataset, int32_t *number)
{
  int32_t i;
  char **name;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetAssociateNames"))
    return(NULL);
  if (!(name=(char**)SDDS_Malloc(sizeof(*name)*SDDS_dataset->layout.n_associates))) {
    SDDS_SetError("Unable to get associate names--allocation failure (SDDS_GetAssociateNames)");
    return(NULL);
  }
  *number = SDDS_dataset->layout.n_associates;
  for (i=0; i<SDDS_dataset->layout.n_associates; i++) {
    if (!SDDS_CopyString(name+i, SDDS_dataset->layout.associate_definition[i].name)) {
      free(name);
      return(NULL);
    }
  }
  return(name);
}

void *SDDS_CastValue(void *data, int32_t index, int32_t data_type, int32_t desired_type, void *memory)
{
  long long integer_value;
  double fp_value;
  if (!data || !memory || data_type==SDDS_STRING || desired_type==SDDS_STRING)
    return(NULL);
  if (data_type==desired_type) {
    memcpy(memory, (char*)data+SDDS_type_size[data_type-1]*index, SDDS_type_size[data_type-1]);
    return(memory);
  }
  switch (data_type) {
  case SDDS_SHORT:
    integer_value = *((short*)data+index);
    fp_value = integer_value;
    break;
  case SDDS_USHORT:
    integer_value = *((unsigned short*)data+index);
    fp_value = integer_value;
    break;
  case SDDS_LONG:
    integer_value = *((int32_t*)data+index);
    fp_value = integer_value;
    break;
  case SDDS_ULONG:
    integer_value = *((uint32_t*)data+index);
    fp_value = integer_value;
    break;
  case SDDS_CHARACTER:
    integer_value = *((unsigned char*)data+index);
    fp_value = integer_value;
    break;
  case SDDS_FLOAT:
    fp_value = *((float*)data+index);
    integer_value = fp_value;
    break;
  case SDDS_DOUBLE:
    fp_value = *((double*)data+index);
    integer_value = fp_value;
    break;
  default:
    return(NULL);
  }
  switch (desired_type) {
  case SDDS_CHARACTER:
    *((char*)memory) = integer_value;
    break;
  case SDDS_SHORT:
    *((short*)memory) = integer_value;
    break;
  case SDDS_USHORT:
    *((unsigned short*)memory) = integer_value;
    break;
  case SDDS_LONG:
    *((int32_t*)memory) = integer_value;
    break;
  case SDDS_ULONG:
    *((uint32_t*)memory) = integer_value;
    break;
  case SDDS_FLOAT:
    *((float*)memory) = fp_value;
    break;
  case SDDS_DOUBLE:
    *((double*)memory) = fp_value;
    break;
  default:
    SDDS_SetError("The impossible has happened (SDDS_CastValue)");
    return(NULL);
  }
  return(memory);
}

void *SDDS_AllocateMatrix(int32_t size, int32_t dim1, int32_t dim2)
{
  int32_t i;
  void **data;

  if (!(data = (void**)SDDS_Malloc(sizeof(*data)*dim1)))
    return(NULL);
  for (i=0; i<dim1; i++)
    if (!(data[i] = (void*)calloc(dim2, size)))
      return(NULL);
  return(data);
}

void SDDS_FreeArray(SDDS_ARRAY *array) 
{
  int i;
  if (!array)
    return;
  if (array->definition) {
    if ((array->definition->type == SDDS_STRING) && (array->data)) {
      char** str = (char**) array->data;
      for (i=0; i<array->elements; i++) {
        if (str[i])
          free(str[i]);
        str[i] = NULL;
      }
    }
  }
  if (array->definition && array->pointer)
    SDDS_FreePointerArray(array->pointer, array->definition->dimensions, array->dimension);
  if (array->data)
    free(array->data);
  array->pointer = array->data = NULL;
  if (array->dimension)
    free(array->dimension);
  if (array->definition)
    SDDS_FreeArrayDefinition(array->definition);
  array->definition = NULL;
  free(array);
  array = NULL;
}

void SDDS_FreeMatrix(void **ptr, int32_t dim1)
{
  int32_t i;
  if (!ptr)
    return;
  for (i=0; i<dim1; i++)
    free(ptr[i]);
  free(ptr);
}

int32_t SDDS_CopyStringArray(char **target, char **source, int32_t n_strings)
{
  if (!source || !target)
    return(0);
  while (n_strings--) {
    if (!SDDS_CopyString(target+n_strings, source[n_strings])) 
      return(0);
  }
  return(1);
}

int32_t SDDS_FreeStringArray(char **string, int32_t strings)
{
  int32_t i;
  if (!string)
    return 0;
  for (i=0; i<strings; i++)
    if (string[i]) {
      free(string[i]);
      string[i] = NULL;
    }
  return 1;
}

void *SDDS_MakePointerArrayRecursively(void *data, int32_t size, int32_t dimensions, int32_t *dimension);

void *SDDS_MakePointerArray(void *data, int32_t type, int32_t dimensions, int32_t *dimension)
{
  int32_t i;

  if (!data) {
    SDDS_SetError("Unable to make pointer array--NULL data array (SDDS_MakePointerArray)");
    return(NULL);
  }
  if (!dimension || !dimensions) {
    SDDS_SetError("Unable to make pointer array--NULL or zero-length dimension array (SDDS_MakePointerArray)");
    return(NULL);
  }
  if (type<=0 || type>SDDS_NUM_TYPES) {
    SDDS_SetError("Unable to make pointer array--unknown data type (SDDS_MakePointerArray)");
    return(NULL);
  }
  for (i=0; i<dimensions; i++)
    if (dimension[i]<=0) {
      SDDS_SetError("Unable to make pointer array--number of elements invalid (SDDS_MakePointerArray)");
      return(NULL);
    }
  if (dimensions==1)
    return(data);
  return(SDDS_MakePointerArrayRecursively(data, SDDS_type_size[type-1], dimensions, dimension));
}

void *SDDS_MakePointerArrayRecursively(void *data, int32_t size, int32_t dimensions, int32_t *dimension)
{
  void **pointer;
  int32_t i, elements;
  static int32_t depth = 0;
  static char s[200];

  depth += 1;
  if (!data) {
    sprintf(s, "Unable to make pointer array--NULL data array (SDDS_MakePointerArrayRecursively, recursion %" PRId32 ")",
            depth);
    SDDS_SetError(s);
    return(NULL);
  }
  if (!dimension || !dimensions) {
    sprintf(s, 
            "Unable to make pointer array--NULL or zero-length dimension array (SDDS_MakePointerArrayRecursively, recursion %" PRId32 ")",
            depth);
    SDDS_SetError(s);
    return(NULL);
  }
  if (size<=0) {
    sprintf(s, "Unable to make pointer array--invalid data size (SDDS_MakePointerArrayRecursively, recursion %" PRId32 ")", depth);
    SDDS_SetError(s);
    return(NULL);
  }
  if (dimensions==1) {
    depth -= 1;
    return(data);
  }
  elements = 1;
  for (i=0; i<dimensions-1; i++)
    elements *= dimension[i];
  if (!(pointer = (void**)SDDS_Malloc(sizeof(void*)*elements))) {
    sprintf(s, "Unable to make pointer array--allocation failure (SDDS_MakePointerArrayRecursively, recursion %" PRId32 ")",
            depth);
    SDDS_SetError(s);
    return(NULL);
  }
  for (i=0; i<elements; i++)
    pointer[i] = (char*)data+i*size*dimension[dimensions-1];
  return(SDDS_MakePointerArrayRecursively(pointer, sizeof(*pointer), dimensions-1, dimension));
}

void SDDS_FreePointerArray(void **data, int32_t dimensions, int32_t *dimension)
/* This procedure is specifically for freeing the pointer arrays made by SDDS_MakePointerArray
 * and *will not* work with general pointer arrays
 */
{
  if (!data || !dimension || !dimensions)
    return;
  if (dimensions>1) {
    SDDS_FreePointerArray((void**)(data[0]), dimensions-1, dimension+1);
    free(data);
  }
}

int32_t SDDS_ApplyFactorToParameter(SDDS_DATASET *SDDS_dataset, char *name, double factor)
{
  int32_t type, index;
  void *data;

  if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))<0)
    return(0);
  type = SDDS_dataset->layout.parameter_definition[index].type;
  if (!SDDS_NUMERIC_TYPE(type)) {
    SDDS_SetError("Unable to apply factor to non-numeric parameter (SDDS_ApplyFactorToParameter)");
    return(0);
  }
  if (!SDDS_dataset->parameter) {
    SDDS_SetError("Unable to apply factor to parameter--no parameter data array (SDDS_ApplyFactorToParameter)");
    return(0);
  }
  if (!(data = SDDS_dataset->parameter[index])) {
    SDDS_SetError("Unable to apply factor to parameter--no data array (SDDS_ApplyFactorToParameter)");
    return(0);
  }
  switch (type) {
  case SDDS_SHORT:
    *((short*)data) *= factor;
    break;
  case SDDS_USHORT:
    *((unsigned short*)data) *= factor;
    break;
  case SDDS_LONG:
    *((int32_t*)data) *= factor;
    break;
  case SDDS_ULONG:
    *((uint32_t*)data) *= factor;
    break;
  case SDDS_CHARACTER:
    *((char*)data) *= factor;
    break;
  case SDDS_FLOAT:
    *((float*)data) *= factor;
    break;
  case SDDS_DOUBLE:
    *((double*)data) *= factor;
    break;
  default:
    return(0);
  }
  return(1);
}

int32_t SDDS_ApplyFactorToColumn(SDDS_DATASET *SDDS_dataset, char *name, double factor)
{
  int32_t type, index, i;
  void *data;

  if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0)
    return(0);
  type = SDDS_dataset->layout.column_definition[index].type;
  if (!SDDS_NUMERIC_TYPE(type)) {
    SDDS_SetError("Unable to apply factor to non-numeric column (SDDS_ApplyFactorToColumn)");
    return(0);
  }
  data = SDDS_dataset->data[index];
  for (i=0; i<SDDS_dataset->n_rows; i++) {
    switch (type) {
    case SDDS_SHORT:
      *((short*)data+i) *= factor;
      break;
    case SDDS_USHORT:
      *((unsigned short*)data+i) *= factor;
      break;
    case SDDS_LONG:
      *((int32_t*)data+i) *= factor;
      break;
    case SDDS_ULONG:
      *((uint32_t*)data+i) *= factor;
      break;
    case SDDS_CHARACTER:
      *((char*)data+i) *= factor;
      break;
    case SDDS_FLOAT:
      *((float*)data+i) *= factor;
      break;
    case SDDS_DOUBLE:
      *((double*)data+i) *= factor;
      break;
    default:
      return(0);
    }
  }
  return(1);
}

void SDDS_EscapeNewlines(char *s)
{
  char *ptr;
  while (*s) {
    if (*s=='\n') {
      ptr = s+strlen(s);
      *(ptr+1) = 0;
      while (ptr!=s) {
        *ptr = *(ptr-1);
        ptr--;
      }
      *s++ = '\\';
      *s++ = 'n';
    }
    else
      s++;
  }
}

int32_t SDDS_ForceInactive(SDDS_DATASET *SDDS_dataset)
{
  if (!SDDS_dataset) {
    SDDS_SetError("NULL SDDS_DATASET passed (SDDS_ForceInactive)");
    return(-1);
  }
  SDDS_dataset->layout.fp = NULL;
  return(1);
}

int32_t SDDS_IsActive(SDDS_DATASET *SDDS_dataset)
{
  if (!SDDS_dataset) {
    SDDS_SetError("NULL SDDS_DATASET passed (SDDS_IsActive)");
    return(-1);
  }
  if (!SDDS_dataset->layout.fp)
    return(0);
  return(1);
}

int32_t SDDS_FileIsLocked(char *filename)
{
#if defined(F_TEST) && ALLOW_FILE_LOCKING
  FILE *fp;
  if (!(fp = fopen(filename, "rb")))
    return 0;
  if (lockf(fileno(fp), F_TEST, 0)==-1) {
    fclose(fp);
    return 1;
  }
  fclose(fp);
  return 0;
#else
  return 0;
#endif
}

int32_t SDDS_LockFile(FILE *fp, char *filename, char *caller)
{
#if defined(F_TEST) && ALLOW_FILE_LOCKING
  char s[1024];
  if (lockf(fileno(fp), F_TEST, 0)==-1) {
    sprintf(s, "Unable to access file %s--file is locked (%s)", filename, caller);
    SDDS_SetError(s);
    return 0;
  }
  if (lockf(fileno(fp), F_TLOCK, 0)==-1) {
    sprintf(s, "Unable to establish lock on file %s (%s)", filename, caller);
    SDDS_SetError(s);
    return 0;
  }
  return 1;
#else
  return 1;
#endif
}

int32_t SDDS_BreakIntoLockedFile(char *filename) {
#if defined(_WIN32)
  fprintf(stderr, "Unable to break into locked file\n");
  return(1);
#else
  char buffer[1024];
  int i=1000, j=0;
  FILE *fp;
  
  /* limit filename length to 500 so we don't overflow the buffer variable */
  if (strlen(filename) > 500) {
    fprintf(stderr, "Unable to break into locked file\n");
    return(1);
  }

  /* find a temporary file name that is not already in use */
  for (i=1000;i<1020;i++) {
    sprintf(buffer, "%s.bl%d", filename, i);
    if (fp = fopen(buffer, "r")) {
      fclose(fp);
    } else {
      j=i;
      break;
    }
  }

  /* if no temporary file names could be found then return with an error message */
  if (j == 0) {
    fprintf(stderr, "Unable to break into locked file\n");
    return(1);
  }

  /* copy the original file to the temp file name and preserve the attributes */
  /* the temp file name has to be in the same directory to preserve ACL settings */
  sprintf(buffer, "cp -p %s %s.bl%d", filename, filename, j);
  if (system(buffer) == -1) {
    fprintf(stderr, "Unable to break into locked file\n");
    return(1);
  }

  /* move the temp file on top of the original file */
  sprintf(buffer, "mv -f %s.bl%d %s", filename, j, filename);
  if (system(buffer) == -1) {
    fprintf(stderr, "Unable to break into locked file\n");
    return(1);
  }
  return(0);
#endif
}

int32_t SDDS_MatchColumns(SDDS_DATASET *SDDS_dataset, char ***nameReturn, int32_t matchMode, int32_t typeMode, ... )
/* This routine has 4 calling modes:
 * SDDS_MatchColumns(&SDDS_dataset, &matchName, SDDS_NAME_ARRAY  , int32_t typeMode [,long type], int32_t n_entries, char **name)
 * SDDS_MatchColumns(&SDDS_dataset, &matchName, SDDS_NAMES_STRING, int32_t typeMode [,long type], char *names)
 * SDDS_MatchColumns(&SDDS_dataset, &matchName, SDDS_NAME_STRINGS, int32_t typeMode [,long type], char *name1, char *name2, ..., NULL )
 * SDDS_MatchColumns(&SDDS_dataset, &matchName, SDDS_MATCH_STRING, int32_t typeMode [,long type], char *name, int32_t logic_mode)
 * SDDS_MatchColumns(&SDDS_dataset, &matchName, SDDS_MATCH_EXCLUDE_STRING, int32_t typeMode [,long type], char *name, char *exclude, int32_t logic_mode)
 */
{
  static int32_t flags=0, *flag=NULL;
  char **name, *string, *match_string, *ptr, *exclude_string;
  va_list argptr;
  int32_t i, j, index, n_names, retval, requiredType, matches;
  /*  int32_t type;*/
  int32_t local_memory;    /* (0,1,2) --> (none, pointer array, pointer array + strings) locally allocated */
  static char buffer[SDDS_MAXLINE];
  int32_t logic;

  name = NULL;
  match_string = exclude_string = NULL;
  n_names = requiredType = local_memory = logic = 0;

  matches = -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MatchColumns"))
    return -1;
  if (nameReturn)
    *nameReturn = NULL;

  retval = 1;
  va_start(argptr, typeMode);
  if (typeMode==FIND_SPECIFIED_TYPE)
    requiredType = va_arg(argptr, int32_t);
  switch (matchMode) {
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
        SDDS_SetError("Unable to process column selection--memory allocation failure (SDDS_MatchColumns)");
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
        SDDS_SetError("Unable to process column selection--memory allocation failure (SDDS_MatchColumns)");
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
      SDDS_SetError("Unable to process column selection--invalid matching string (SDDS_MatchColumns)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  case SDDS_MATCH_EXCLUDE_STRING:
    local_memory = 0;
    n_names = 1;
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process column selection--invalid matching string (SDDS_MatchColumns)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process column exclusion--invalid matching string (SDDS_MatchColumns)");
      retval = 0;
      break;
    }
    exclude_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  default:
    SDDS_SetError("Unable to process column selection--unknown match mode (SDDS_MatchColumns)");
    retval = 0;
    break;
  }
  va_end(argptr);
  if (retval==0)
    return -1;
  
  if (n_names==0) {
    SDDS_SetError("Unable to process column selection--no names in call (SDDS_MatchColumns)");
    return -1;
  }

  if (SDDS_dataset->layout.n_columns!=flags) {
    flags = SDDS_dataset->layout.n_columns;
    if (flag)
      free(flag);
    if (!(flag = (int32_t*)calloc(flags, sizeof(*flag)))) {
      SDDS_SetError("Memory allocation failure (SDDS_MatchColumns)");
      return -1;
    }
  }

  if ((matchMode!=SDDS_MATCH_STRING) && (matchMode!=SDDS_MATCH_EXCLUDE_STRING)) {
    for (i=0; i<n_names; i++) {
      if ((index=SDDS_GetColumnIndex(SDDS_dataset, name[i]))>=0)
        flag[index] = 1;
      else
        flag[index] = 0;
    }
  }
  else {
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      if (SDDS_Logic(flag[i], 
                     wild_match(SDDS_dataset->layout.column_definition[i].name, match_string), logic)) {
        if (exclude_string != NULL) {
          if (SDDS_Logic(flag[i], 
                         wild_match(SDDS_dataset->layout.column_definition[i].name, exclude_string), logic))
            flag[i]=0;
          else
            flag[i]=1;
        } else {
          flag[i] = 1;
        }
      } else {
#if defined(DEBUG)
        fprintf(stderr, "no logic match of %s to %s\n",
                SDDS_dataset->layout.column_definition[i].name, match_string);
#endif
        flag[i] = 0;
      }
    }
  }
  if (match_string)
    free(match_string);
  if (exclude_string)
    free(exclude_string);
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_columns; i++)
    fprintf(stderr, "flag[%" PRId32 "] = %" PRId32 " : %s\n",
            i, flag[i], SDDS_dataset->layout.column_definition[i].name);
#endif

  if (local_memory==2) {
    for (i=0; i<n_names; i++)
      free(name[i]);
  }
  if (local_memory>=1)
    free(name);

  switch (typeMode) {
  case FIND_SPECIFIED_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_columns; i++)
      if (SDDS_dataset->layout.column_definition[i].type!=requiredType)
        flag[i] = 0;
    break;
  case FIND_NUMERIC_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_columns; i++)
      if (!SDDS_NUMERIC_TYPE(SDDS_dataset->layout.column_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_FLOATING_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_columns; i++)
      if (!SDDS_FLOATING_TYPE(SDDS_dataset->layout.column_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_INTEGER_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_columns; i++)
      if (!SDDS_INTEGER_TYPE(SDDS_dataset->layout.column_definition[i].type))
        flag[i] = 0;
    break;
  default:
    break;
  }
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) 
    if (flag[i])
      fprintf(stderr, "column %s matched\n", SDDS_dataset->layout.column_definition[i].name);
#endif

  for (i=matches=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (flag[i])
      matches ++;
  }
  if (!matches || !nameReturn)
    return matches;
  if (!((*nameReturn) = (char**)SDDS_Malloc(matches*sizeof(**nameReturn)))) {
    SDDS_SetError("Memory allocation failure (SDDS_MatchColumns)");
    return -1;
  }
  for (i=j=0; i<SDDS_dataset->layout.n_columns; i++) {
    if (flag[i]) {
      if (!SDDS_CopyString((*nameReturn)+j, SDDS_dataset->layout.column_definition[i].name)) {
        SDDS_SetError("String copy failure (SDDS_MatchColumns)");
        return -1;
      }
      j++;
    }
  }
  return matches;
}

int32_t SDDS_MatchParameters(SDDS_DATASET *SDDS_dataset, char ***nameReturn, int32_t matchMode, int32_t typeMode, ... )
/* This routine has 4 calling modes:
 * SDDS_MatchParameters(&SDDS_dataset, &matchName, SDDS_NAME_ARRAY  , int32_t typeMode [,long type], int32_t n_entries, char **name)
 * SDDS_MatchParameters(&SDDS_dataset, &matchName, SDDS_NAMES_STRING, int32_t typeMode [,long type], char *names)
 * SDDS_MatchParameters(&SDDS_dataset, &matchName, SDDS_NAME_STRINGS, int32_t typeMode [,long type], char *name1, char *name2, ..., NULL )
 * SDDS_MatchParameters(&SDDS_dataset, &matchName, SDDS_MATCH_STRING, int32_t typeMode [,long type], char *name, int32_t logic_mode)
 * SDDS_MatchParameters(&SDDS_dataset, &matchName, SDDS_MATCH_EXCLUDE_STRING, int32_t typeMode [,long type], char *name, char *exclude, int32_t logic_mode)
 */
{
  static int32_t flags=0, *flag=NULL;
  char **name, *string, *match_string, *ptr, *exclude_string;
  va_list argptr;
  int32_t i, j, index, n_names, retval, requiredType, matches;
  /*  int32_t type;*/
  int32_t local_memory;    /* (0,1,2) --> (none, pointer array, pointer array + strings) locally allocated */
  static char buffer[SDDS_MAXLINE];
  int32_t logic;

  name = NULL;
  match_string = exclude_string = NULL;
  n_names = requiredType = local_memory = logic = 0;

  matches = -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MatchParameters"))
    return -1;
  if (nameReturn)
    *nameReturn = NULL;

  retval = 1;
  va_start(argptr, typeMode);
  if (typeMode==FIND_SPECIFIED_TYPE)
    requiredType = va_arg(argptr, int32_t);
  switch (matchMode) {
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
        SDDS_SetError("Unable to process parameter selection--memory allocation failure (SDDS_MatchParameters)");
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
        SDDS_SetError("Unable to process parameter selection--memory allocation failure (SDDS_MatchParameters)");
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
      SDDS_SetError("Unable to process parameter selection--invalid matching string (SDDS_MatchParameters)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  case SDDS_MATCH_EXCLUDE_STRING:
    local_memory = 0;
    n_names = 1;
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process parameter selection--invalid matching string (SDDS_MatchParameters)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process parameter exclusion--invalid matching string (SDDS_MatchParameters)");
      retval = 0;
      break;
    }
    exclude_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  default:
    SDDS_SetError("Unable to process parameter selection--unknown match mode (SDDS_MatchParameters)");
    retval = 0;
    break;
  }
  va_end(argptr);
  if (retval==0)
    return -1;
  
  if (n_names==0) {
    SDDS_SetError("Unable to process parameter selection--no names in call (SDDS_MatchParameters)");
    return -1;
  }

  if (SDDS_dataset->layout.n_parameters!=flags) {
    flags = SDDS_dataset->layout.n_parameters;
    if (flag)
      free(flag);
    if (!(flag = (int32_t*)calloc(flags, sizeof(*flag)))) {
      SDDS_SetError("Memory allocation failure (SDDS_MatchParameters)");
      return -1;
    }
  }

  if ((matchMode!=SDDS_MATCH_STRING) && (matchMode!=SDDS_MATCH_EXCLUDE_STRING)) {
    for (i=0; i<n_names; i++) {
      if ((index=SDDS_GetParameterIndex(SDDS_dataset, name[i]))>=0)
        flag[index] = 1;
      else
        flag[index] = 0;
    }
  }
  else {
    for (i=0; i<SDDS_dataset->layout.n_parameters; i++) {
      if (SDDS_Logic(flag[i], 
                     wild_match(SDDS_dataset->layout.parameter_definition[i].name, match_string), logic)) {
        if (exclude_string != NULL) {
          if (SDDS_Logic(flag[i], 
                         wild_match(SDDS_dataset->layout.parameter_definition[i].name, exclude_string), logic))
            flag[i]=0;
          else
            flag[i]=1;            
        } else {
          flag[i] = 1;
        }
      }
      else {
#if defined(DEBUG)
        fprintf(stderr, "no logic match of %s to %s\n",
                SDDS_dataset->layout.parameter_definition[i].name, match_string);
#endif
        flag[i] = 0;
      }
    }
  }
  if (match_string)
    free(match_string);
  if (exclude_string)
    free(exclude_string);
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
    fprintf(stderr, "flag[%" PRId32 "] = %" PRId32 " : %s\n",
            i, flag[i], SDDS_dataset->layout.parameter_definition[i].name);
#endif

  if (local_memory==2) {
    for (i=0; i<n_names; i++)
      free(name[i]);
  }
  if (local_memory>=1)
    free(name);

  switch (typeMode) {
  case FIND_SPECIFIED_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
      if (SDDS_dataset->layout.parameter_definition[i].type!=requiredType)
        flag[i] = 0;
    break;
  case FIND_NUMERIC_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
      if (!SDDS_NUMERIC_TYPE(SDDS_dataset->layout.parameter_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_FLOATING_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
      if (!SDDS_FLOATING_TYPE(SDDS_dataset->layout.parameter_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_INTEGER_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_parameters; i++)
      if (!SDDS_INTEGER_TYPE(SDDS_dataset->layout.parameter_definition[i].type))
        flag[i] = 0;
    break;
  default:
    break;
  }
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_parameters; i++) 
    if (flag[i])
      fprintf(stderr, "parameter %s matched\n", SDDS_dataset->layout.parameter_definition[i].name);
#endif

  for (i=matches=0; i<SDDS_dataset->layout.n_parameters; i++) {
    if (flag[i])
      matches ++;
  }
  if (!matches || !nameReturn) 
    return matches;
  if (!((*nameReturn) = (char**)SDDS_Malloc(matches*sizeof(**nameReturn)))) {
    SDDS_SetError("Memory allocation failure (SDDS_MatchParameters)");
    return -1;
  }
  for (i=j=0; i<SDDS_dataset->layout.n_parameters; i++) {
    if (flag[i]) {
      if (!SDDS_CopyString((*nameReturn)+j, SDDS_dataset->layout.parameter_definition[i].name)) {
        SDDS_SetError("String copy failure (SDDS_MatchParameters)");
        return -1;
      }
      j++;
    }
  }
  
  return matches;
}

int32_t SDDS_MatchArrays(SDDS_DATASET *SDDS_dataset, char ***nameReturn, int32_t matchMode, int32_t typeMode, ... )
/* This routine has 4 calling modes:
 * SDDS_MatchArrays(&SDDS_dataset, &matchName, SDDS_NAME_ARRAY  , int32_t typeMode [,long type], int32_t n_entries, char **name)
 * SDDS_MatchArrays(&SDDS_dataset, &matchName, SDDS_NAMES_STRING, int32_t typeMode [,long type], char *names)
 * SDDS_MatchArrays(&SDDS_dataset, &matchName, SDDS_NAME_STRINGS, int32_t typeMode [,long type], char *name1, char *name2, ..., NULL )
 * SDDS_MatchArrays(&SDDS_dataset, &matchName, SDDS_MATCH_STRING, int32_t typeMode [,long type], char *name, int32_t logic_mode)
 * SDDS_MatchArrays(&SDDS_dataset, &matchName, SDDS_MATCH_EXCLUDE_STRING, int32_t typeMode [,long type], char *name, char *exclude, int32_t logic_mode)
 */
{
  static int32_t flags=0, *flag=NULL;
  char **name, *string, *match_string, *ptr, *exclude_string;
  va_list argptr;
  int32_t i, j, index, n_names, retval, requiredType, matches;
  /*  int32_t type;*/
  int32_t local_memory;    /* (0,1,2) --> (none, pointer array, pointer array + strings) locally allocated */
  static char buffer[SDDS_MAXLINE];
  int32_t logic;

  name = NULL;
  match_string = exclude_string = NULL;
  n_names = requiredType = local_memory = logic = 0;

  matches = -1;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MatchArrays"))
    return -1;
  if (nameReturn)
    *nameReturn = NULL;

  retval = 1;
  va_start(argptr, typeMode);
  if (typeMode==FIND_SPECIFIED_TYPE)
    requiredType = va_arg(argptr, int32_t);
  switch (matchMode) {
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
    while ((SDDS_GetToken(string, buffer, SDDS_MAXLINE)>0)) {
      if (!(name = SDDS_Realloc(name, sizeof(*name)*(n_names+1))) || !SDDS_CopyString(name+n_names, buffer)) {
        SDDS_SetError("Unable to process array selection--memory allocation failure (SDDS_MatchArrays)");
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
        SDDS_SetError("Unable to process array selection--memory allocation failure (SDDS_MatchArrays)");
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
      SDDS_SetError("Unable to process array selection--invalid matching string (SDDS_MatchArrays)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  case SDDS_MATCH_EXCLUDE_STRING:
    local_memory = 0;
    n_names = 1;
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process array selection--invalid matching string (SDDS_MatchArrays)");
      retval = 0;
      break;
    }
    match_string = expand_ranges(string);
    if (!(string=va_arg(argptr, char *))) {
      SDDS_SetError("Unable to process array exclusion--invalid matching string (SDDS_MatchArrays)");
      retval = 0;
      break;
    }
    exclude_string = expand_ranges(string);
    logic = va_arg(argptr, int32_t);
    break;
  default:
    SDDS_SetError("Unable to process array selection--unknown match mode (SDDS_MatchArrays)");
    retval = 0;
    break;
  }
  va_end(argptr);
  if (retval==0)
    return -1;
  
  if (n_names==0) {
    SDDS_SetError("Unable to process array selection--no names in call (SDDS_MatchArrays)");
    return -1;
  }

  if (SDDS_dataset->layout.n_arrays!=flags) {
    flags = SDDS_dataset->layout.n_arrays;
    if (flag)
      free(flag);
    if (!(flag = (int32_t*)calloc(flags, sizeof(*flag)))) {
      SDDS_SetError("Memory allocation failure (SDDS_MatchArrays)");
      return -1;
    }
  }

  if ((matchMode!=SDDS_MATCH_STRING) && (matchMode!=SDDS_MATCH_EXCLUDE_STRING)) {
    for (i=0; i<n_names; i++) {
      if ((index=SDDS_GetArrayIndex(SDDS_dataset, name[i]))>=0)
        flag[index] = 1;
      else
        flag[index] = 0;
    }
  }
  else {
    for (i=0; i<SDDS_dataset->layout.n_arrays; i++) {
      if (SDDS_Logic(flag[i], 
                     wild_match(SDDS_dataset->layout.array_definition[i].name, match_string), logic)) {
        if (exclude_string != NULL) {
          if (SDDS_Logic(flag[i], 
                         wild_match(SDDS_dataset->layout.array_definition[i].name, exclude_string), logic))
            flag[i]=0;
          else
            flag[i]=1;
        } else {
          flag[i] = 1;
        }
      }
      else {
#if defined(DEBUG)
        fprintf(stderr, "no logic match of %s to %s\n",
                SDDS_dataset->layout.array_definition[i].name, match_string);
#endif
        flag[i] = 0;
      }
    }
  }
  if (match_string)
    free(match_string);
  if (exclude_string)
    free(exclude_string);
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_arrays; i++)
    fprintf(stderr, "flag[%" PRId32 "] = %" PRId32 " : %s\n",
            i, flag[i], SDDS_dataset->layout.array_definition[i].name);
#endif

  if (local_memory==2) {
    for (i=0; i<n_names; i++)
      free(name[i]);
  }
  if (local_memory>=1)
    free(name);

  switch (typeMode) {
  case FIND_SPECIFIED_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_arrays; i++)
      if (SDDS_dataset->layout.array_definition[i].type!=requiredType)
        flag[i] = 0;
    break;
  case FIND_NUMERIC_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_arrays; i++)
      if (!SDDS_NUMERIC_TYPE(SDDS_dataset->layout.array_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_FLOATING_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_arrays; i++)
      if (!SDDS_FLOATING_TYPE(SDDS_dataset->layout.array_definition[i].type))
        flag[i] = 0;
    break;
  case FIND_INTEGER_TYPE:
    for (i=0; i<SDDS_dataset->layout.n_arrays; i++)
      if (!SDDS_INTEGER_TYPE(SDDS_dataset->layout.array_definition[i].type))
        flag[i] = 0;
    break;
  default:
    break;
  }
#if defined(DEBUG)
  for (i=0; i<SDDS_dataset->layout.n_arrays; i++) 
    if (flag[i])
      fprintf(stderr, "array %s matched\n", SDDS_dataset->layout.array_definition[i].name);
#endif

  for (i=matches=0; i<SDDS_dataset->layout.n_arrays; i++) {
    if (flag[i])
      matches ++;
  }
  if (!matches || !nameReturn) 
    return matches;
  if (!((*nameReturn) = (char**)SDDS_Malloc(matches*sizeof(**nameReturn)))) {
    SDDS_SetError("Memory allocation failure (SDDS_MatchArrays)");
    return -1;
  }
  for (i=j=0; i<SDDS_dataset->layout.n_arrays; i++) {
    if (flag[i]) {
      if (!SDDS_CopyString((*nameReturn)+j, SDDS_dataset->layout.array_definition[i].name)) {
        SDDS_SetError("String copy failure (SDDS_MatchArrays)");
        return -1;
      }
      j++;
    }
  }
  
  return matches;
}

char *SDDS_FindColumn(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
{
  int32_t index, error, type, thisType;
  va_list argptr;
  char *name, *buffer;

  va_start(argptr, mode);
  buffer = NULL;
  error = type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  while ((name = va_arg(argptr, char *))) {
    if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))>=0) {
      thisType = SDDS_GetColumnType(SDDS_dataset, index);
      if (mode==FIND_ANY_TYPE ||
          (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
          (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
          (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
          (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
        if (!SDDS_CopyString(&buffer, name)) {
          SDDS_SetError("unable to return string from SDDS_FindColumn");
          error = 1;
          break;
        }
        error = 0;
        break;
      }
    }
  }
  va_end(argptr);
  if (error)
    return NULL;
  return buffer;
}

char *SDDS_FindParameter(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
{
  int32_t index, error, type, thisType;
  va_list argptr;
  char *name, *buffer;

  va_start(argptr, mode);
  buffer = NULL;
  error = type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  while ((name = va_arg(argptr, char *))) {
    if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))>=0) {
      thisType = SDDS_GetParameterType(SDDS_dataset, index);
      if (mode==FIND_ANY_TYPE ||
          (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
          (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
          (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
          (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
        if (!SDDS_CopyString(&buffer, name)) {
          SDDS_SetError("unable to return string from SDDS_FindParameter");
          error = 1;
          break;
        }
        error = 0;
        break;
      }
    }
  }
  va_end(argptr);
  if (error)
    return NULL;
  return buffer;
}

char *SDDS_FindArray(SDDS_DATASET *SDDS_dataset, int32_t mode, ...)
{
  int32_t index, error, type, thisType;
  va_list argptr;
  char *name, *buffer;

  va_start(argptr, mode);
  buffer = NULL;
  error = type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  while ((name = va_arg(argptr, char *))) {
    if ((index=SDDS_GetArrayIndex(SDDS_dataset, name))>=0) {
      thisType = SDDS_GetArrayType(SDDS_dataset, index);
      if (mode==FIND_ANY_TYPE ||
          (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
          (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
          (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
          (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
        if (!SDDS_CopyString(&buffer, name)) {
          SDDS_SetError("unable to return string from SDDS_FindArray");
          error = 1;
          break;
        }
        error = 0;
        break;
      }
    }
  }
  va_end(argptr);
  if (error)
    return NULL;
  return buffer;
}


int32_t SDDS_CheckColumn(SDDS_DATASET *SDDS_dataset, char *name, char *units, int32_t type, FILE *fp_message)
{
  char *units1;
  int32_t index;
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))<0)
    return(SDDS_PrintCheckText(fp_message, name, units, type, "column", SDDS_CHECK_NONEXISTENT));
  if (SDDS_VALID_TYPE(type)) {
    if (type!=SDDS_GetColumnType(SDDS_dataset, index))
      return(SDDS_PrintCheckText(fp_message, name, units, type,"column", SDDS_CHECK_WRONGTYPE));
  }
  else {
    switch (type) {
    case 0:
      break;
    case SDDS_ANY_NUMERIC_TYPE:
      if (!SDDS_NUMERIC_TYPE(SDDS_GetColumnType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"column", SDDS_CHECK_WRONGTYPE));
      break;
    case SDDS_ANY_FLOATING_TYPE:
      if (!SDDS_FLOATING_TYPE(SDDS_GetColumnType(SDDS_dataset, index))) {
        return(SDDS_PrintCheckText(fp_message, name, units, type,"column", SDDS_CHECK_WRONGTYPE));
      }
      break;
    case SDDS_ANY_INTEGER_TYPE:
      if (!SDDS_INTEGER_TYPE(SDDS_GetColumnType(SDDS_dataset, index))) {
        return(SDDS_PrintCheckText(fp_message, name, units, type,"column", SDDS_CHECK_WRONGTYPE));
      }
      break;
    default:
      return(SDDS_PrintCheckText(fp_message, name, units, type, "column", SDDS_CHECK_WRONGTYPE));
    }
  }
  if (!units) {
    /* don't care about units */
    return SDDS_CHECK_OKAY;
  }
  if (SDDS_GetColumnInformation(SDDS_dataset, "units", &units1, SDDS_GET_BY_NAME, name)!=SDDS_STRING) {
    SDDS_SetError("units field of column has wrong data type!");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  if (!units1) {
    if (SDDS_StringIsBlank(units))
      return(SDDS_CHECK_OKAY);
    return(SDDS_PrintCheckText(fp_message, name, units, type, "column", SDDS_CHECK_WRONGUNITS));
  }
  if (strcmp(units, units1)==0) {
    free(units1);
    return(SDDS_CHECK_OKAY);
  }
  free(units1);
  return(SDDS_PrintCheckText(fp_message, name, units, type, "column", SDDS_CHECK_WRONGUNITS));
}

int32_t SDDS_CheckParameter(SDDS_DATASET *SDDS_dataset, char *name, char *units, int32_t type, FILE *fp_message)
{
  char *units1;
  int32_t index;
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))<0)
    return(SDDS_PrintCheckText(fp_message, name, units, type, "parameter", SDDS_CHECK_NONEXISTENT));
  if (SDDS_VALID_TYPE(type)) {
    if (type!=SDDS_GetParameterType(SDDS_dataset, index))
      return(SDDS_PrintCheckText(fp_message, name, units, type,"parameter", SDDS_CHECK_WRONGTYPE));
  }
  else {
    switch (type) {
    case 0:
      break;
    case SDDS_ANY_NUMERIC_TYPE:
      if (!SDDS_NUMERIC_TYPE(SDDS_GetParameterType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"parameter", SDDS_CHECK_WRONGTYPE));
      break;
    case SDDS_ANY_FLOATING_TYPE:
      if (!SDDS_FLOATING_TYPE(SDDS_GetParameterType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"parameter", SDDS_CHECK_WRONGTYPE));
      break;
    case SDDS_ANY_INTEGER_TYPE:
      if (!SDDS_INTEGER_TYPE(SDDS_GetParameterType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"parameter", SDDS_CHECK_WRONGTYPE));
      break;
    default:
      return(SDDS_PrintCheckText(fp_message, name, units, type, "parameter", SDDS_CHECK_WRONGTYPE));
    }
  }
  if (SDDS_GetParameterInformation(SDDS_dataset, "units", &units1, SDDS_GET_BY_NAME, name)!=SDDS_STRING) {
    SDDS_SetError("units field of parameter has wrong data type!");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  if (!units) {
    /* don't care about units */
    return(SDDS_CHECK_OKAY);
  }
  if (!units1) {
    if (SDDS_StringIsBlank(units))
      return(SDDS_CHECK_OKAY);
    return(SDDS_PrintCheckText(fp_message, name, units, type, "parameter", SDDS_CHECK_WRONGUNITS));
  }
  if (strcmp(units, units1)==0) {
    free(units1);
    return(SDDS_CHECK_OKAY);
  }
  free(units1);
  return(SDDS_PrintCheckText(fp_message, name, units, type, "parameter", SDDS_CHECK_WRONGUNITS));
}

int32_t SDDS_CheckArray(SDDS_DATASET *SDDS_dataset, char *name, char *units, int32_t type, FILE *fp_message)
{
  char *units1;
  int32_t index;
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, name))<0)
    return(SDDS_PrintCheckText(fp_message, name, units, type, "array", SDDS_CHECK_NONEXISTENT));
  if (SDDS_VALID_TYPE(type)) {
    if (type!=SDDS_GetArrayType(SDDS_dataset, index))
      return(SDDS_PrintCheckText(fp_message, name, units, type,"array", SDDS_CHECK_WRONGTYPE));
  }
  else {
    switch (type) {
    case 0:
      break;
    case SDDS_ANY_NUMERIC_TYPE:
      if (!SDDS_NUMERIC_TYPE(SDDS_GetArrayType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"array", SDDS_CHECK_WRONGTYPE));
      break;
    case SDDS_ANY_FLOATING_TYPE:
      if (!SDDS_FLOATING_TYPE(SDDS_GetArrayType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"array", SDDS_CHECK_WRONGTYPE));
      break;
    case SDDS_ANY_INTEGER_TYPE:
      if (!SDDS_INTEGER_TYPE(SDDS_GetArrayType(SDDS_dataset, index))) 
        return(SDDS_PrintCheckText(fp_message, name, units, type,"array", SDDS_CHECK_WRONGTYPE));
      break;
    default:
      return(SDDS_PrintCheckText(fp_message, name, units, type, "array", SDDS_CHECK_WRONGTYPE));
    }
  }
  if (SDDS_GetArrayInformation(SDDS_dataset, "units", &units1, SDDS_GET_BY_NAME, name)!=SDDS_STRING) {
    SDDS_SetError("units field of array has wrong data type!");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }
  if (!units) {
    /* don't care about units */
    return(SDDS_CHECK_OKAY);
  }
  if (!units1) {
    if (SDDS_StringIsBlank(units))
      return(SDDS_CHECK_OKAY);
    return(SDDS_CHECK_OKAY);
  }
  if (strcmp(units, units1)==0) {
    free(units1);
    return(SDDS_CHECK_OKAY);
  }
  free(units1);
  return(SDDS_PrintCheckText(fp_message, name, units, type, "array", SDDS_CHECK_WRONGUNITS));
}

int32_t SDDS_PrintCheckText(FILE *fp, char *name, char *units, int32_t type, char *class_name, int32_t error_code)
{
  if (!fp || !name || !class_name)
    return(error_code);
  switch (error_code) {
  case SDDS_CHECK_OKAY:
    break;
  case SDDS_CHECK_NONEXISTENT:
    fprintf(fp, "Problem with %s %s: nonexistent (%s)\n", class_name, name, registeredProgramName?registeredProgramName:"?"); 
    break;
  case SDDS_CHECK_WRONGTYPE:
    if (SDDS_VALID_TYPE(type))
      fprintf(fp, "Problem with %s %s: wrong data type--expected %s (%s)\n",
              class_name, name, SDDS_type_name[type-1], registeredProgramName?registeredProgramName:"?");
    else if (type==SDDS_ANY_NUMERIC_TYPE)
      fprintf(fp, "Problem with %s %s: wrong data type--expected numeric data (%s)\n", 
              class_name, name, registeredProgramName?registeredProgramName:"?");
    else if (type==SDDS_ANY_FLOATING_TYPE)
      fprintf(fp, "Problem with %s %s: wrong data type--expected floating point data (%s)\n", 
              class_name, name, registeredProgramName?registeredProgramName:"?");
    else if (type==SDDS_ANY_INTEGER_TYPE)
      fprintf(fp, "Problem with %s %s: wrong data type--expected integer data (%s)\n", 
              class_name, name, registeredProgramName?registeredProgramName:"?");
    else if (type)
      fprintf(fp, "Problem with %s %s: invalid data type code seen---may be a programming error (%s)\n",
              class_name, name, registeredProgramName?registeredProgramName:"?");
    break;
  case SDDS_CHECK_WRONGUNITS:
    fprintf(fp, "Problem with %s %s: wrong units--expected %s (%s)\n", class_name, name, units?units:"none",
            registeredProgramName?registeredProgramName:"?"); 
    break;
  default:
    fprintf(stderr, "Problem with call to SDDS_PrintCheckText--invalid error code (%s)\n", registeredProgramName?registeredProgramName:"?");
    return(SDDS_CHECK_OKAY);
  }
  return(error_code);
}

int32_t SDDS_DeleteParameterFixedValues(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout, *orig_layout;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_DeleteFixedValueParameters")) 
    return 0;
  if (!SDDS_SaveLayout(SDDS_dataset))
    return 0;
  layout = &SDDS_dataset->layout;
  orig_layout = &SDDS_dataset->original_layout;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      free(layout->parameter_definition[i].fixed_value);
    if (orig_layout->parameter_definition[i].fixed_value &&
        (!layout->parameter_definition[i].fixed_value ||
         orig_layout->parameter_definition[i].fixed_value!=layout->parameter_definition[i].fixed_value))
      free(orig_layout->parameter_definition[i].fixed_value);
    orig_layout->parameter_definition[i].fixed_value = NULL;
    layout->parameter_definition[i].fixed_value = NULL;
  }
  return 1;
}

int32_t SDDS_SetDataMode(SDDS_DATASET *SDDS_dataset, int32_t newmode)
{
  if (newmode==-SDDS_BINARY) {
    /* will write with bytes swapped.
     * provided for compatibility with sddsendian program, which writes the
     * data itself 
     */
    SDDS_dataset->layout.byteOrderDeclared = 
      SDDS_IsBigEndianMachine()?SDDS_LITTLEENDIAN:SDDS_BIGENDIAN;
    newmode = SDDS_BINARY;
  }
  if (newmode!=SDDS_ASCII && newmode!=SDDS_BINARY) {
    SDDS_SetError("Invalid data mode (SDDS_SetDataMode)");
    return 0;
  }
  if (newmode==SDDS_dataset->layout.data_mode.mode)
    return 1;
  if (!SDDS_dataset) {
    SDDS_SetError("NULL page pointer (SDDS_SetDataMode)");
    return 0;
  }
  if (SDDS_dataset->page_number!=0 && (SDDS_dataset->page_number>1 || SDDS_dataset->n_rows_written!=0)) {
    SDDS_SetError("Can't change the mode of a file that's been written to (SDDS_SetDataMode)");
    return 0;
  }
  SDDS_dataset->layout.data_mode.mode = SDDS_dataset->original_layout.data_mode.mode = newmode;
  return 1;
}

int32_t SDDS_CheckDatasetStructureSize(int32_t size)
{
  char buffer[100];
  if (size!=sizeof(SDDS_DATASET)) {
    SDDS_SetError("passed size is not equal to expected size for SDDS_DATASET structure");
    sprintf(buffer, "Passed size is %" PRId32 ", library size is %" PRId32 "\n",
            size, (int32_t)sizeof(SDDS_DATASET));
    SDDS_SetError(buffer);
    return 0;
  }
  return 1;
}

int32_t SDDS_ColumnCount(SDDS_DATASET *page)
{
  if (!page)
    return 0;
  return page->layout.n_columns;
}

int32_t SDDS_ParameterCount(SDDS_DATASET *page)
{
  if (!page)
    return 0;
  return page->layout.n_parameters;
}

int32_t SDDS_ArrayCount(SDDS_DATASET *page)
{
  if (!page)
    return 0;
  return page->layout.n_arrays;
}


void SDDS_InterpretEscapes(char *s)
/* \ddd = octal value
 * ANSI escape codes (n t b r f v \ ' " a ?
 * SDDS escape codes (! )
 */
{
  char *ptr;
  int32_t count;
  
  ptr = s;
  while (*s) {
    if (*s!='\\')
      *ptr++ = *s++;
    else {
      s++;
      if (!*s) {
        *ptr++ = '\\';
        *ptr++ = 0;
        return;
      }
      switch (*s) {
      case 'n':
        *ptr++ = '\n';
        s++;
        break;
      case 't':
        *ptr++ = '\t';
        s++;
        break;
      case 'b':
        *ptr++ = '\b';
        s++;
        break;
      case 'r':
        *ptr++ = '\r';
        s++;
        break;
      case 'f':
        *ptr++ = '\f';
        s++;
        break;
      case 'v':
        *ptr++ = '\v';
        s++;
        break;
      case '\\':
        *ptr++ = '\\';
        s++;
        break;
      case '\'':
        *ptr++ = '\'';
        s++;
        break;
      case '"':
        *ptr++ = '\"';
        s++;
        break;
      case 'a':
        *ptr++ = '\a';
        s++;
        break;
      case '?':
        *ptr++ = '\?';
        s++;
        break;
      case '!':
        *ptr++ = '!';
        s++;
        break;
      default:
        if (*s>='0' && *s<='9') {
          *ptr = 0;
          count = 0;
          while (++count<=3 && *s>='0' && *s<='9') 
            *ptr = 8*(*ptr) + *s++ - '0';
          ptr++;
        }
        else {
          *ptr++ = '\\';
        }
        break;
      }
    }
  }
  *ptr = 0;
}

#define COMMENT_COMMANDS 3
static char *commentCommandName[COMMENT_COMMANDS] = {
  "big-endian", "little-endian", 
  "fixed-rowcount",
};
static uint32_t commentCommandFlag[COMMENT_COMMANDS] = {
  SDDS_BIGENDIAN_SEEN, SDDS_LITTLEENDIAN_SEEN,
  SDDS_FIXED_ROWCOUNT_SEEN,
};

static uint32_t commentFlags = 0;

uint32_t SDDS_GetSpecialCommentsModes()
{
  return commentFlags;
}

void SDDS_ResetSpecialCommentsModes()
{
  commentFlags = 0;
}

void SDDS_ParseSpecialComments(char *s)
{
  char buffer[SDDS_MAXLINE];
  int32_t i;

  while (SDDS_GetToken(s, buffer, SDDS_MAXLINE)>0) {
    for (i=0; i<COMMENT_COMMANDS; i++) {
      if (strcmp(buffer, commentCommandName[i])==0) {
        commentFlags |= commentCommandFlag[i];
        break;
      }
    }
  }
}

int32_t SDDS_IsBigEndianMachine() 
{
  int32_t x = 1;
  if ( *((char*)&x) )
    return 0;
  return 1;
}

int32_t SDDS_VerifyArrayExists(SDDS_DATASET *SDDS_dataset, int32_t mode, ...) {
  int32_t index, type, thisType;
  va_list argptr;
  char *name;

  va_start(argptr, mode);
  type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  name = va_arg(argptr, char *);
  if ((index=SDDS_GetArrayIndex(SDDS_dataset, name))>=0) {
    thisType = SDDS_GetArrayType(SDDS_dataset, index);
    if (mode==FIND_ANY_TYPE ||
        (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
        (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
        (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
        (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
      va_end(argptr);
      return(index);
    }
  }
  va_end(argptr);
  return(-1);
}

int32_t SDDS_VerifyColumnExists(SDDS_DATASET *SDDS_dataset, int32_t mode, ...) {
  int32_t index, type, thisType;
  va_list argptr;
  char *name;

  va_start(argptr, mode);
  type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  name = va_arg(argptr, char *);
  if ((index=SDDS_GetColumnIndex(SDDS_dataset, name))>=0) {
    thisType = SDDS_GetColumnType(SDDS_dataset, index);
    if (mode==FIND_ANY_TYPE ||
        (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
        (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
        (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
        (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
      va_end(argptr);
      return(index);
    }
  }
  va_end(argptr);
  return(-1);
}

int32_t SDDS_VerifyParameterExists(SDDS_DATASET *SDDS_dataset, int32_t mode, ...) {
  int32_t index, type, thisType;
  va_list argptr;
  char *name;

  va_start(argptr, mode);
  type = 0;

  if (mode==FIND_SPECIFIED_TYPE)
    type = va_arg(argptr, int32_t);
  name = va_arg(argptr, char *);
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, name))>=0) {
    thisType = SDDS_GetParameterType(SDDS_dataset, index);
    if (mode==FIND_ANY_TYPE ||
        (mode==FIND_SPECIFIED_TYPE &&  thisType==type) ||
        (mode==FIND_NUMERIC_TYPE && SDDS_NUMERIC_TYPE(thisType)) ||
        (mode==FIND_FLOATING_TYPE && SDDS_FLOATING_TYPE(thisType)) ||
        (mode==FIND_INTEGER_TYPE && SDDS_INTEGER_TYPE(thisType))) {
      va_end(argptr);
      return(index);
    }
  }
  va_end(argptr);
  return(-1);
}
