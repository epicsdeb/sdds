/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_input.c
 * contents: user's routines for reading SDDS files
 *           The header namelists are read and scanned, but not "processed".
 * 
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.57  2009/08/12 22:21:50  borland
 Added better error message when file is not found in search path.

 Revision 1.56  2009/06/01 20:56:26  soliday
 Updated so that it can handle SDDS_MPI_IO=0

 Revision 1.55  2008/07/29 21:11:04  shang
 modified the parallel and serial initialize input so that they do not interfere with each other.

 Revision 1.54  2008/04/14 18:27:26  shang
 added parallel SDDS.

 Revision 1.53  2008/02/25 21:56:53  soliday
 Fixed but with the Include statement in SDDS header files.

 Revision 1.52  2006/09/22 19:57:48  soliday
 Updated SDDS_CheckEndOfFile to use the proper gzeof function instead of feof
 when using a GunZip file.

 Revision 1.51  2006/09/15 18:16:06  borland
 Added SDDS_CheckEndOfFile.

 Revision 1.50  2005/11/04 22:46:52  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.49  2005/08/30 19:11:45  soliday
 Fixed problem when writting output from programs such as sddslogger in
 a non-native endian mode.

 Revision 1.48  2005/02/07 21:46:51  soliday
 Commited M. Borland's changes. There may have been a problem when calling
 SDDS_Terminate after calling SDDS_ReconnectFile.

 Revision 1.47  2003/08/28 21:24:58  soliday
 Cleaned up the code.

 Revision 1.46  2002/09/09 18:27:49  soliday
 Fixed a problem in InitializeInputFromSearchPath so that it set the error correctly.
 This code cannot be used in VxWorks

 Revision 1.45  2002/08/14 16:34:48  soliday
 Added Open License

 Revision 1.44  2002/07/24 20:20:57  shang
 added SDDS_InitializeInputFromSearchPath()

 Revision 1.43  2002/02/04 19:57:05  borland
 SDDS_UpdateBinaryPage() and SDDS_UpdateAsciiPage() now free string memory
 from the columns if in FLUSH_TABLE mode.  This prevents a big memory leak.

 Revision 1.42  2002/01/15 22:50:45  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.41  2002/01/11 22:31:35  soliday
 Commited versions for Shang.

 Revision 1.40  2002/01/07 21:34:42  borland
 Added SDDS_SetRowLimit() and SDDS_GetRowLimit(), which permit the user to
 specify that SDDS tables with more than a certain number of rows are to
 be ignored.  This helps to deal with corrupted files, which sometimes
 occurs when the file server is slow.  Added code to SDDS_ReadBinaryPage()
 and SDDS_ReadAsciiPage() to recognize this specification.

 Revision 1.39  2002/01/03 17:40:28  soliday
 Fixed RW_ASSOCIATES problem.

 Revision 1.38  2001/12/01 03:27:09  borland
 Fixed problem reading nonnative binary gzip files.  Wasn't setting the
 endian flag.

 Revision 1.37  2001/11/30 15:34:37  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.36  2000/11/28 14:19:01  borland
 Added SDDS_FreeStringData().

 Revision 1.35  2000/09/19 20:13:22  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.34  2000/06/26 21:13:45  soliday
 Fixed bug with new free statements.

 Revision 1.33  2000/06/15 21:10:05  soliday
 Added memory freeing statements that were suggested by Sev Binello.

 Revision 1.32  2000/04/14 16:39:24  soliday
 Now check to make sure gzFile is the same size as a void*

 Revision 1.31  2000/04/06 15:59:50  soliday
 Added support for Borland C.

 Revision 1.30  2000/03/30 19:55:10  borland
 Added SDDS_DisconnectFile() and SDDS_ReconnectFile() plus changes to
 support this function.

 Revision 1.29  2000/03/21 17:15:54  soliday
 Modified SDDS_GetNamelist and SDDS_GetGZipNamelist so that they accept WIN32
 style linefeeds.

 Revision 1.28  2000/01/18 20:13:16  soliday
 Added support for ZLIB.

 Revision 1.27  1999/12/21 17:48:35  soliday
 vxWorks does not support the pclose command

 Revision 1.26  1999/12/15 16:47:40  soliday
 Added support for vxWorks.

 Revision 1.25  1999/08/10 15:09:33  soliday
 Changed stdin to binary mode for WIN32

 Revision 1.24  1999/07/22 15:36:49  soliday
 Uses macro for fopen mode.

 Revision 1.23  1999/07/08 21:55:31  soliday
 Added definition for pclose

 Revision 1.22  1999/05/25 18:57:24  soliday
 Removed compiler warning on linux.

 Revision 1.21  1999/05/11 17:06:49  soliday
 Changed the way SDDS_Terminate frees SDDS_dataset->array memory.

 Revision 1.20  1999/05/04 14:46:08  borland
 SDDS_Terminate now has conditional compilation for WIN32 that removes
 reference to pclose.

 Revision 1.19  1999/01/13 22:23:02  borland
 Modifed SDDS_GetNamelist so that it doesn't replace ending newlines with
 nulls.  This causes errors when parsing some legitimate headers.  Instead
 it replaces the ending newlines with spaces.

 Revision 1.18  1998/12/16 21:19:52  borland
 Changes to debugging statements.

 Revision 1.17  1998/07/16 16:00:39  borland
 SDDS_copy: fixed error statement (named the wrong routine).
 SDDS_input: SDDS_InitializeInput now puts filenames in quotes for
 error messages.

 Revision 1.16  1998/07/07 22:29:42  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

 Revision 1.15  1998/06/12 16:10:37  borland
 Fixed memory leak due to failure to free sorted indices when terminating.
 Also, wasn't copying sorted indices when saving the layout.

 Revision 1.14  1997/12/19 16:58:37  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.13  1996/11/13 00:57:19  borland
 Now reads to end of file being delivered on a pipe to avoid Broken
 Pipe errors in Solaris.

 Revision 1.12  1996/09/27 21:40:24  saunders
 Removed "read to EOF" prior to pclose(). Not needed now since "Broken
 Pipe" error output is now redirected to /dev/null.

 Revision 1.11  1996/06/11 19:57:45  borland
 Added tests for NULL pointers for each of group of four free() calls
 at end of SDDS_Terminate.  This shouldn't be needed if free() were
 ANSI compliant.

 * Revision 1.10  1996/03/21  20:07:10  borland
 * Added mode argument to SDDS_ReadPageSparse().
 *
 * Revision 1.9  1996/03/21  16:38:37  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.8  1996/03/12  04:00:47  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.7  1996/01/21  00:15:13  borland
 * Fixed bug in file lock routines (wasn't closing file); using new version
 * of UnpackFopen(), with same functionality; SDDS_Terminate now seeks to
 * end of pipe for Solaris, to avoid stupid warning messages.
 *
 * Revision 1.6  1996/01/19  00:13:40  borland
 * Switched from SDDS_UnpackFopen() to UnpackFopen(), removing former from
 * SDDS_input.c .  Using popenUsed entry in layout to keep track of whether
 * input files is a pipe or a bonafide file.
 *
 * Revision 1.5  1996/01/18  05:44:47  borland
 * Added SDDS_UnpackFopen() and put in call to it from SDDS_InitializeInput();
 * now get automatic unpacking of compressed files.  Modified SDDS_ReadLayout
 * to require SDDS<n> as first line (as per the manual, no leading comments are
 * allowed).
 *
 * Revision 1.4  1995/12/12  10:01:55  borland
 * Fixed bugs having to do with updates of no_row_counts=1 files; won't allow
 * changing no_row_counts after writing layout or for an input file.
 *
 * Revision 1.3  1995/12/12  04:26:37  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.2  1995/09/05  21:10:59  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "match_string.h"
#include "SDDS.h"
#include "SDDS_internal.h"
#include "namelist.h"
#include "scan.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#define pclose(x) _pclose(x)
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif

#define DEBUG 0

int32_t SDDS_InitializeInput(SDDS_DATASET *SDDS_dataset, char *filename) 
{
  /*  char *ptr, *datafile, *headerfile;*/
  static char s[SDDS_MAXLINE];
#if defined(zLib)
  char *extension;
#endif
  if (sizeof(gzFile) != sizeof(void*)) {
    SDDS_SetError("gzFile is not the same size as void *, possible corruption of the SDDS_LAYOUT structure");
    return(0);
  }
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeInput"))
    return(0);
  
  if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    sprintf(s, "Unable to initialize input for file %s--can't zero SDDS_DATASET structure (SDDS_InitializeInput)",
            filename);
    SDDS_SetError(s);
    return(0);
  }
  SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = SDDS_dataset->layout.popenUsed = 0;
  if (!filename)
    SDDS_dataset->layout.filename = NULL;
  else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    sprintf(s, "Memory allocation failure initializing file \"%s\" (SDDS_InitializeInput)", filename);
    SDDS_SetError(s);
    return(0);
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
  } else {
#if defined(zLib)
    if (!(extension = strrchr(filename, '.')) || strcmp(extension, ".gz")!=0) {
#endif
      if (!(SDDS_dataset->layout.fp
	    =UnpackFopen(filename, UNPACK_REQUIRE_SDDS|UNPACK_USE_PIPE, \
			 &SDDS_dataset->layout.popenUsed, NULL))) {
	sprintf(s, "Unable to open file \"%s\" for reading (SDDS_InitializeInput)", filename);
	SDDS_SetError(s);
	return(0);
      }
#if defined(zLib)
    } else {
      SDDS_dataset->layout.gzipFile = 1;
      if (!(SDDS_dataset->layout.gzfp
	    =UnpackGZipOpen(filename))) {
	sprintf(s, "Unable to open file \"%s\" for reading (SDDS_InitializeInput)", filename);
	SDDS_SetError(s);
	return(0);
      }
    }
#endif
  }
  SDDS_dataset->page_number =SDDS_dataset->page_started= 0;
  SDDS_dataset->file_had_data = 0;
  SDDS_DeferSavingLayout(1);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_GZipReadLayout(SDDS_dataset, SDDS_dataset->layout.gzfp))
      return(0);
  } else {
#endif
    if (!SDDS_ReadLayout(SDDS_dataset, SDDS_dataset->layout.fp))
      return(0);
#if defined(zLib)
  }
#endif
  SDDS_dataset->layout.layout_written = 0;
  SDDS_DeferSavingLayout(0);
  if (!SDDS_SaveLayout(SDDS_dataset))
    return 0;
  if (SDDS_dataset->layout.n_columns &&
      ((!(SDDS_dataset->column_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
       !(SDDS_dataset->column_order=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns))) ||
      (!SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
       !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1)))) {
    SDDS_SetError("Unable to initialize input--memory allocation failure (SDDS_InitializeInput)");
    return(0);
  }
  SDDS_dataset->mode=SDDS_READMODE; /*reading*/
  SDDS_dataset->pagecount_offset=NULL;
  if (!SDDS_dataset->layout.gzipFile && !SDDS_dataset->layout.popenUsed &&
      SDDS_dataset->layout.filename) {
    /* Data is not:
       1. from a gzip file 
       2. from a file that is being internally decompressed by a command executed with popen()
       3. from a pipe set up externally (e.g., -pipe=in on commandline)
     */
    SDDS_dataset->pages_read=0;
    SDDS_dataset->pagecount_offset=malloc(sizeof(*SDDS_dataset->pagecount_offset));
    SDDS_dataset->pagecount_offset[0]=ftell(SDDS_dataset->layout.fp);
    fseek(SDDS_dataset->layout.fp,0,2); /*point to the end of the file*/
    SDDS_dataset->endOfFile_offset=ftell(SDDS_dataset->layout.fp);
    fseek(SDDS_dataset->layout.fp,SDDS_dataset->pagecount_offset[0],0);
    /*point to the beginning of the first page*/
  }
  return(1);
}

int32_t SDDS_InitializeHeaderlessInput(SDDS_DATASET *SDDS_dataset, char *filename)
{
  /*  char *ptr, *datafile;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeInput"))
    return(0);
  if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
    SDDS_SetError("Unable to initialize input--can't zero SDDS_DATASET structure (SDDS_InitializeInput)");
    return(0);
  }
  SDDS_dataset->layout.gzipFile = 0;
  if (!(SDDS_dataset->layout.fp=fopen(filename, FOPEN_READ_MODE))) {
    SDDS_SetError("Unable to open file (SDDS_InitializeInput)");
    return(0);
  }
  if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
    SDDS_SetError("Memory allocation failure (SDDS_InitializeInput)");
    return(0);
  }
  SDDS_dataset->mode=SDDS_READMODE; /*reading*/
  SDDS_dataset->page_number = SDDS_dataset->page_started = 0;
  SDDS_dataset->pages_read=0;
  SDDS_dataset->pagecount_offset=malloc(sizeof(*SDDS_dataset->pagecount_offset));
  SDDS_dataset->pagecount_offset[0]=ftell(SDDS_dataset->layout.fp);
  fseek(SDDS_dataset->layout.fp,0,2); /*point to the end of the file*/
  SDDS_dataset->endOfFile_offset=ftell(SDDS_dataset->layout.fp);
  fseek(SDDS_dataset->layout.fp,SDDS_dataset->pagecount_offset[0],0);
  /*point to the beginning of the first page*/
  return(1);
}

int32_t SDDS_IsQuoted(char *string, char *position, char quotation_mark)
{
  int32_t in_quoted_section;
  char *string0;

  if (*position==quotation_mark)
    return(1);

  in_quoted_section = 0;
  string0 = string;
  while (*string) {
    if (*string==quotation_mark && (string==string0 || *(string-1)!='\\')) 
      in_quoted_section = !in_quoted_section; 
    else if (string==position)
      return(in_quoted_section);
    string++;
  }
  return(0);
}

int32_t SDDS_GetNamelist(char *buffer, int32_t buflen, FILE *fp)
{
  char *ptr, *flag, *buffer0;
  /*  char *ptr1*/
  int32_t n;
  /*  int32_t namelistStarted;*/

  while ((flag=fgetsSkipComments(buffer, buflen, fp, '!'))) {
    if ((ptr=strchr(buffer, '&')) && !SDDS_IsQuoted(buffer, ptr, '"'))
      break;
  }
  if (!flag)
    return 0;
  n = strlen(buffer)-1;
  if (buffer[n]=='\n') {
    buffer[n] = ' ';
    if ((n-1 >= 0) && (buffer[n-1]=='\r'))
      buffer[n-1] = ' ';
  }
  
  /* check for the beginning of a namelist (an unquoted &) */
  ptr = buffer;
  while (*ptr) {
    if (*ptr=='"') {
      /* skip quoted section */
      ptr++;
      while (*ptr!='"' && *ptr)
        ptr++;
      if (*ptr)
        ptr++;
      continue;
    }
    if (*ptr=='&') {
      if (strncmp(ptr, "&end", 4)==0)
        return 0;
      break;
    }
    ptr++;
  }
  if (!*ptr)
    return 0;

  /* remove the trailing &end if there is one */
  if ((n=strlen(buffer))>=4) {
    ptr = buffer+n-4;
    while (1) {
      if (*ptr=='&' && (ptr==buffer || *(ptr-1)!='\\') && strncmp(ptr, "&end", 4)==0 
          && !SDDS_IsQuoted(buffer, ptr, '"')) {
        *ptr = 0;
        return 1;
      }
      if (ptr==buffer)
        break;
      ptr--;
    }
  }

  /* read in the remainder of the namelist */
  buffer0 = buffer;
  buflen -= strlen(buffer);
  buffer += strlen(buffer);
  while ((flag=fgetsSkipComments(buffer, buflen, fp, '!'))) {
    n = strlen(buffer)-1;
    if (buffer[n]=='\n') {
      buffer[n] = ' ';
      if ((n-1 >= 0) && (buffer[n-1]=='\r'))
	buffer[n-1] = ' ';
    }
    if ((ptr=strstr(buffer, "&end")) && !SDDS_IsQuoted(buffer0, ptr, '"'))
      return 1;
    buflen -= strlen(buffer);
    buffer += strlen(buffer);
    if (buflen==0)
      return 0;
  }
  return 0;
}

#if defined(zLib)
int32_t SDDS_GetGZipNamelist(char *buffer, int32_t buflen, gzFile *gzfp)
{
  char *ptr, *flag, *buffer0;
  /*  char *ptr1*/
  int32_t n;
  /*  int32_t namelistStarted;*/

  while ((flag=fgetsGZipSkipComments(buffer, buflen, gzfp, '!'))) {
    if ((ptr=strchr(buffer, '&')) && !SDDS_IsQuoted(buffer, ptr, '"'))
      break;
  }
  if (!flag)
    return 0;
  n = strlen(buffer)-1;
  if (buffer[n]=='\n') {
    buffer[n] = ' ';
    if ((n-1 >= 0) && (buffer[n-1]=='\r'))
      buffer[n-1] = ' ';
  }
  
  /* check for the beginning of a namelist (an unquoted &) */
  ptr = buffer;
  while (*ptr) {
    if (*ptr=='"') {
      /* skip quoted section */
      ptr++;
      while (*ptr!='"' && *ptr)
        ptr++;
      if (*ptr)
        ptr++;
      continue;
    }
    if (*ptr=='&') {
      if (strncmp(ptr, "&end", 4)==0)
        return 0;
      break;
    }
    ptr++;
  }
  if (!*ptr)
    return 0;

  /* remove the trailing &end if there is one */
  if ((n=strlen(buffer))>=4) {
    ptr = buffer+n-4;
    while (1) {
      if (*ptr=='&' && (ptr==buffer || *(ptr-1)!='\\') && strncmp(ptr, "&end", 4)==0 
          && !SDDS_IsQuoted(buffer, ptr, '"')) {
        *ptr = 0;
        return 1;
      }
      if (ptr==buffer)
        break;
      ptr--;
    }
  }

  /* read in the remainder of the namelist */
  buffer0 = buffer;
  buflen -= strlen(buffer);
  buffer += strlen(buffer);
  while ((flag=fgetsGZipSkipComments(buffer, buflen, gzfp, '!'))) {
    n = strlen(buffer)-1;
    if (buffer[n]=='\n') {
      buffer[n] = ' ';
      if ((n-1 >= 0) && (buffer[n-1]=='\r'))
	buffer[n-1] = ' ';
    }
    if ((ptr=strstr(buffer, "&end")) && !SDDS_IsQuoted(buffer0, ptr, '"'))
      return 1;
    buflen -= strlen(buffer);
    buffer += strlen(buffer);
    if (buflen==0)
      return 0;
  }
  return 0;
}
#endif

int32_t SDDS_ReadLayout(SDDS_DATASET *SDDS_dataset, FILE *fp)
{
  static char s[SDDS_MAXLINE], buffer[SDDS_MAXLINE];
  char *groupName, *ptr;
  FILE *fp1;
  int32_t retval, bigEndianMachine;
  static int32_t depth = 0, data_command_seen=0;
  uint32_t commentFlags;
  
  if (!fp) {
    SDDS_SetError("Unable to read layout--NULL file pointer (SDDS_ReadLayout)");
    return(0);
  }
  if (depth==0) {
    if (SDDS_dataset->layout.disconnected) {
      SDDS_SetError("Can't read layout--file is disconnected (SDDS_ReadLayout)");
      return 0;
    }
    if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadLayout")) {
      fclose(fp);
      return(0);
    }
    SDDS_dataset->layout.layout_written = 1; /* it is already in the file */
    if (!fgets(s, SDDS_MAXLINE, fp)) {
      fclose(fp);
      SDDS_SetError("Unable to read layout--no header lines found (SDDS_ReadLayout)");
      return(0);
    }
    if (strncmp(s, "SDDS", 4)!=0) {
      fclose(fp);
      SDDS_SetError("Unable to read layout--no header lines found (SDDS_ReadLayout)");
      return(0);
    }
    if (sscanf(s+4, "%" SCNd32, &SDDS_dataset->layout.version)!=1) {
      fclose(fp);
      SDDS_SetError("Unable to read layout--no version number on first line (SDDS_ReadLayout)");
      return(0);
    }
    SDDS_ResetSpecialCommentsModes();
    data_command_seen = 0;
  }
  while (SDDS_GetNamelist(s, SDDS_MAXLINE, fp)) {
#if DEBUG
    strcpy(buffer, s);
#endif
    groupName = s+1;
    if (!(ptr=strpbrk(s, " \t"))) {
      SDDS_SetError("Unable to read layout---no groupname in namelist (SDDS_ReadLayout)");
      return 0;
    }
    *ptr = 0; 
    switch (match_string(groupName, SDDS_command, SDDS_NUM_COMMANDS, EXACT_MATCH)) {
    case SDDS_DESCRIPTION_COMMAND:
      if (!SDDS_ProcessDescription(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process description (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_COLUMN_COMMAND:
      if (!SDDS_ProcessColumnDefinition(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process column definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_PARAMETER_COMMAND:
      if (!SDDS_ProcessParameterDefinition(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process parameter definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_ASSOCIATE_COMMAND:
#if RW_ASSOCIATES != 0
      if (!SDDS_ProcessAssociateDefinition(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process associate definition (SDDS_ReadLayout)");
        return(0);
      }
#endif
      break;
    case SDDS_DATA_COMMAND:
      if (!SDDS_ProcessDataMode(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process data mode (SDDS_ReadLayout)");
        return(0);
      }
      if (data_command_seen) {
        /* should never happen */
        fclose(fp);
        SDDS_SetError("Unable to read layout--multiple data commands (SDDS_ReadLayout)");
        return(0);
      }
      if (!SDDS_SaveLayout(SDDS_dataset)) {
        SDDS_SetError("Unable to read layout--couldn't save layout (SDDS_ReadLayout)");
        return(0);
      }
      data_command_seen = 1;
      commentFlags = SDDS_GetSpecialCommentsModes();
      if ((commentFlags&SDDS_BIGENDIAN_SEEN) && (commentFlags&SDDS_LITTLEENDIAN_SEEN)) {
        SDDS_SetError("Unable to read data as it says it is both big and little endian (SDDS_ReadLayout)");
        return(0);
      }
      bigEndianMachine = SDDS_IsBigEndianMachine();
      SDDS_dataset->swapByteOrder = SDDS_dataset->layout.byteOrderDeclared = 0;
      SDDS_dataset->autoRecover = 0;
      if (commentFlags&SDDS_BIGENDIAN_SEEN)
        SDDS_dataset->layout.byteOrderDeclared = SDDS_BIGENDIAN_SEEN;
      if (commentFlags&SDDS_LITTLEENDIAN_SEEN)
        SDDS_dataset->layout.byteOrderDeclared = SDDS_LITTLEENDIAN_SEEN;
      if (((commentFlags&SDDS_BIGENDIAN_SEEN) && !bigEndianMachine) ||
	  ((commentFlags&SDDS_LITTLEENDIAN_SEEN) && bigEndianMachine) ) 
        SDDS_dataset->swapByteOrder = 1;
      if (commentFlags&SDDS_FIXED_ROWCOUNT_SEEN)
	if (!SDDS_SetAutoReadRecovery(SDDS_dataset, SDDS_AUTOREADRECOVER))
	  return(0);
      return(1);
    case SDDS_INCLUDE_COMMAND:
      if (!(fp1=SDDS_ProcessIncludeCommand(SDDS_dataset, ptr+1))) {
        fclose(fp);
        SDDS_SetError("Unable to process include command (SDDS_ReadLayout)");
        return(0);
      }
      depth += 1;
      retval = SDDS_ReadLayout(SDDS_dataset, fp1);
      depth -= 1;
      fclose(fp1);
      if (retval==0) {
        return(0);
      }
      if (data_command_seen) {
        return(1);
      }
      break;
    case SDDS_ARRAY_COMMAND:
      if (!SDDS_ProcessArrayDefinition(SDDS_dataset, ptr+1)) {
        fclose(fp);
        SDDS_SetError("Unable to process array definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    default:
      fclose(fp);
      sprintf(buffer, "Unknown layout entry %s given (SDDS_ReadLayout)", groupName);
      SDDS_SetError(buffer); 
      return(0);
    }
  }
  /* on recursive calls, it's okay to hit EOF */
  if ((feof(fp) && depth!=0) || data_command_seen)
    return(1);
  return(0);
}

#if defined(zLib)
int32_t SDDS_GZipReadLayout(SDDS_DATASET *SDDS_dataset, gzFile *gzfp)
{
  static char s[SDDS_MAXLINE], buffer[SDDS_MAXLINE];
  char *groupName, *ptr;
  FILE *fp1;
  int32_t retval, bigEndianMachine;
  static int32_t depth = 0, data_command_seen=0;
  uint32_t commentFlags;
  
  if (!gzfp) {
    SDDS_SetError("Unable to read layout--NULL file pointer (SDDS_GZipReadLayout)");
    return(0);
  }
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't read layout--file is disconnected (SDDS_GZipReadLayout)");
    return 0;
  }
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GZipReadLayout")) {
    gzclose(gzfp);
    return(0);
  }
  SDDS_dataset->layout.layout_written = 1; /* it is already in the file */
  if (!gzgets(gzfp, s, SDDS_MAXLINE)) {
    gzclose(gzfp);
    SDDS_SetError("Unable to read layout--no header lines found (SDDS_GZipReadLayout)");
    return(0);
  }
  if (strncmp(s, "SDDS", 4)!=0) {
    gzclose(gzfp);
    SDDS_SetError("Unable to read layout--no header lines found (SDDS_GZipReadLayout)");
    return(0);
  }
  if (sscanf(s+4, "%" SCNd32, &SDDS_dataset->layout.version)!=1) {
    gzclose(gzfp);
    SDDS_SetError("Unable to read layout--no version number on first line (SDDS_GZipReadLayout)");
    return(0);
  }
  SDDS_ResetSpecialCommentsModes();
  if (depth==0)
    data_command_seen = 0;
  while (SDDS_GetGZipNamelist(s, SDDS_MAXLINE, gzfp)) {
#if DEBUG
    strcpy(buffer, s);
#endif
    groupName = s+1;
    if (!(ptr=strpbrk(s, " \t"))) {
      SDDS_SetError("Unable to read layout---no groupname in namelist (SDDS_GZipReadLayout)");
      return 0;
    }
    *ptr = 0; 
    switch (match_string(groupName, SDDS_command, SDDS_NUM_COMMANDS, EXACT_MATCH)) {
    case SDDS_DESCRIPTION_COMMAND:
      if (!SDDS_ProcessDescription(SDDS_dataset, ptr+1)) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process description (SDDS_GZipReadLayout)");
        return(0);
      }
      break;
    case SDDS_COLUMN_COMMAND:
      if (!SDDS_ProcessColumnDefinition(SDDS_dataset, ptr+1)) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process column definition (SDDS_GZipReadLayout)");
        return(0);
      }
      break;
    case SDDS_PARAMETER_COMMAND:
      if (!SDDS_ProcessParameterDefinition(SDDS_dataset, ptr+1)) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process parameter definition (SDDS_GZipReadLayout)");
        return(0);
      }
      break;
    case SDDS_ASSOCIATE_COMMAND:
#if RW_ASSOCIATES != 0
      if (!SDDS_ProcessAssociateDefinition(SDDS_dataset, ptr+1)) {
        gzclose(fp);
        SDDS_SetError("Unable to process associate definition (SDDS_GZipReadLayout)");
        return(0);
      }
#endif
      break;
    case SDDS_DATA_COMMAND:
      if (!SDDS_ProcessDataMode(SDDS_dataset, ptr+1)) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process data mode (SDDS_GZipReadLayout)");
        return(0);
      }
      if (data_command_seen) {
        /* should never happen */
        gzclose(gzfp);
        SDDS_SetError("Unable to read layout--multiple data commands (SDDS_GZipReadLayout)");
        return(0);
      }
      if (!SDDS_SaveLayout(SDDS_dataset)) {
        SDDS_SetError("Unable to read layout--couldn't save layout (SDDS_GZipReadLayout)");
        return(0);
      }
      data_command_seen = 1;
      commentFlags = SDDS_GetSpecialCommentsModes();
      if ((commentFlags&SDDS_BIGENDIAN_SEEN) && (commentFlags&SDDS_LITTLEENDIAN_SEEN)) {
        SDDS_SetError("Unable to read data as it says it is both big and little endian (SDDS_ReadLayout)");
        return(0);
      }
      bigEndianMachine = SDDS_IsBigEndianMachine();
      SDDS_dataset->swapByteOrder = SDDS_dataset->layout.byteOrderDeclared = 0;
      SDDS_dataset->autoRecover = 0;
      if (commentFlags&SDDS_BIGENDIAN_SEEN)
        SDDS_dataset->layout.byteOrderDeclared = SDDS_BIGENDIAN_SEEN;
      if (commentFlags&SDDS_LITTLEENDIAN_SEEN)
        SDDS_dataset->layout.byteOrderDeclared = SDDS_LITTLEENDIAN_SEEN;
      if (((commentFlags&SDDS_BIGENDIAN_SEEN) && !bigEndianMachine) ||
	  ((commentFlags&SDDS_LITTLEENDIAN_SEEN) && bigEndianMachine) ) 
        SDDS_dataset->swapByteOrder = 1;
      if (commentFlags&SDDS_FIXED_ROWCOUNT_SEEN)
	if (!SDDS_SetAutoReadRecovery(SDDS_dataset, SDDS_AUTOREADRECOVER))
	  return(0);
      return(1);
    case SDDS_INCLUDE_COMMAND:
      if (!(fp1=SDDS_ProcessIncludeCommand(SDDS_dataset, ptr+1))) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process include command (SDDS_GZipReadLayout)");
        return(0);
      }
      depth += 1;
      retval = SDDS_ReadLayout(SDDS_dataset, fp1);
      depth -= 1;
      fclose(fp1);
      if (retval==0) {
        return(0);
      }
      if (data_command_seen) {
        return(1);
      }
      break;
    case SDDS_ARRAY_COMMAND:
      if (!SDDS_ProcessArrayDefinition(SDDS_dataset, ptr+1)) {
        gzclose(gzfp);
        SDDS_SetError("Unable to process array definition (SDDS_GZipReadLayout)");
        return(0);
      }
      break;
    default:
      gzclose(gzfp);
      sprintf(buffer, "Unknown layout entry %s given (SDDS_GZipReadLayout)", groupName);
      SDDS_SetError(buffer); 
      return(0);
    }
  }
  /* on recursive calls, it's okay to hit EOF */
  if ((gzeof(gzfp) && depth!=0) || data_command_seen)
    return(1);
  return(0);
}
#endif

int32_t SDDS_ReadPage(SDDS_DATASET *SDDS_dataset)
{
#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_ReadPage(SDDS_dataset);
#endif
  return SDDS_ReadPageSparse(SDDS_dataset, 0, 1, 0);
}

int32_t SDDS_CheckEndOfFile(SDDS_DATASET *SDDS_dataset)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_EndOfFile"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't check status--file is disconnected (SDDS_EndOfFile)");
    return 2;
  }
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_dataset->layout.gzfp) {
      SDDS_SetError("Unable to check status--NULL file pointer (SDDS_EndOfFile)");
      return 2;
    }
  } else {
#endif
    if (!SDDS_dataset->layout.fp) {
      SDDS_SetError("Unable to check status--NULL file pointer (SDDS_EndOfFile)");
      return 2;
    }
#if defined(zLib)
  }
#endif
  if (SDDS_dataset->fBuffer.bufferSize && SDDS_dataset->fBuffer.bytesLeft) {
    return 0;
  }

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (gzeof(SDDS_dataset->layout.gzfp))
      return 1;
  } else {
#endif
    if (feof(SDDS_dataset->layout.fp))
      return 1;
#if defined(zLib)
  }
#endif
  return 0;
}


int32_t SDDS_ReadPageSparse(SDDS_DATASET *SDDS_dataset, uint32_t mode,
                         int32_t sparse_interval,
                         int32_t sparse_offset)
/* the mode argument is to support future expansion */
{
  int32_t retval;
  /*  SDDS_LAYOUT layout_copy;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadPageSparse"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't read page--file is disconnected (SDDS_ReadPageSparse)");
    return 0;
  }
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_dataset->layout.gzfp) {
      SDDS_SetError("Unable to read page--NULL file pointer (SDDS_ReadPageSparse)");
      return(0);
    }
  } else {
#endif
    if (!SDDS_dataset->layout.fp) {
      SDDS_SetError("Unable to read page--NULL file pointer (SDDS_ReadPageSparse)");
      return(0);
    }
#if defined(zLib)
  }
#endif
  if (SDDS_dataset->original_layout.data_mode.mode==SDDS_ASCII) {
    if ((retval=SDDS_ReadAsciiPage(SDDS_dataset, sparse_interval, sparse_offset))<1) {
      return(retval);
    }
  }
  else if (SDDS_dataset->original_layout.data_mode.mode==SDDS_BINARY) {
    if ((retval=SDDS_ReadBinaryPage(SDDS_dataset, sparse_interval, sparse_offset))<1) {
      return(retval);
    }
  } else {
    SDDS_SetError("Unable to read page--unrecognized data mode (SDDS_ReadPageSparse)");
    return(0);
  }
  if (!SDDS_dataset->layout.gzipFile && !SDDS_dataset->layout.popenUsed &&
      SDDS_dataset->layout.filename && SDDS_dataset->pagecount_offset) {
    /* Data is not:
       1. from a gzip file 
       2. from a file that is being internally decompressed by a command executed with popen()
       3. from a pipe set up externally (e.g., -pipe=in on commandline)
       and pagecount_offset has been allocate memory from SDDS_initializeInput()
     */
    if (SDDS_dataset->pagecount_offset[SDDS_dataset->pages_read]<SDDS_dataset->endOfFile_offset) {
      SDDS_dataset->pages_read++;
      if (!(SDDS_dataset->pagecount_offset=realloc(SDDS_dataset->pagecount_offset,
                                                   sizeof(int32_t)*(SDDS_dataset->pages_read+1)))) {
        SDDS_SetError("Unable to allocate memory for pagecount_offset (SDDS_ReadPageSparse)");
        exit(1);
      }
      SDDS_dataset->pagecount_offset[SDDS_dataset->pages_read]=ftell(SDDS_dataset->layout.fp);
    }
  }
  return(retval);
}

static int32_t SDDS_RowLimit  = INT32_MAX;
int32_t SDDS_SetRowLimit(int32_t limit) 
{
  int32_t previous;
  previous = SDDS_RowLimit;
  if (limit<=0)
    SDDS_RowLimit = LONG_MAX;
  else
    SDDS_RowLimit = limit;
  return previous;
}

int32_t SDDS_GetRowLimit() 
{
  return SDDS_RowLimit;
}

/*this procedure can only work for a non-zip file, it does not work for pipe input either*/
int32_t SDDS_GotoPage(SDDS_DATASET *SDDS_dataset, int32_t page_number)
{
  int32_t offset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GotoPage"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't go to page--file is disconnected (SDDS_GotoPage)");
    return 0;
  }
  if (SDDS_dataset->layout.popenUsed || !SDDS_dataset->layout.filename) {
    SDDS_SetError("Can't go to page of pipe is used (SDDS_GotoPage)");
    return 0;
  }
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    SDDS_SetError("Can not go to page of a gzip file (SDDS_GotoPage)");
    return(0);
  } else {
#endif
    if (!SDDS_dataset->layout.fp) {
      SDDS_SetError("Unable to go to page--NULL file pointer (SDDS_GotoPage)");
      return(0);
    }
#if defined(zLib)
  }
#endif
  if (!SDDS_dataset->layout.filename) {
    SDDS_SetError("Can't go to page--NULL filename pointer (SDDS_GotoPage)");
    return 0;
  }
  if (SDDS_dataset->mode!=SDDS_READMODE) {
    SDDS_SetError("Can't go to page--file mode has to be reading mode (SDDS_GotoPage)");
    return 0;
  }
  if (SDDS_dataset->fBuffer.bufferSize) {
    SDDS_SetError("Can't go to page--file buffering is turned on (SDDS_GotoPage)");
    return 0;
  }
  if (page_number<1) {
    SDDS_SetError("The page_number can not be less than 1 (SDDS_GotoPage)");
    return(0);
  }
  if (page_number>SDDS_dataset->pages_read) {
    offset=SDDS_dataset->pagecount_offset[SDDS_dataset->pages_read]-ftell(SDDS_dataset->layout.fp);
    fseek(SDDS_dataset->layout.fp,offset,1);
    SDDS_dataset->page_number=SDDS_dataset->pages_read;
    while (SDDS_dataset->pages_read<page_number) {
      if (SDDS_ReadPageSparse(SDDS_dataset,0,10000,0)<=0) {
        SDDS_SetError("The page_number is greater than the total pages (SDDS_GotoPage)");
        return(0);
      }
    }
  } else {
    offset=SDDS_dataset->pagecount_offset[page_number-1]-ftell(SDDS_dataset->layout.fp);
    fseek(SDDS_dataset->layout.fp,offset,1); /*seek to the position from current offset*/
    SDDS_dataset->page_number=page_number-1; 
  }
  return 1;
}


static int32_t terminateMode = 0;

void SDDS_SetTerminateMode(uint32_t mode)
{
  terminateMode = mode;
}

#include <signal.h>
static char termBuffer[16384];

int32_t SDDS_FreeStringData(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout;
  char **ptr;
  int32_t i, j;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_Terminate"))
    return(0);
  layout = &SDDS_dataset->original_layout;

  if (SDDS_dataset->parameter) {
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].type==SDDS_STRING) {
        free(*(char**)(SDDS_dataset->parameter[i]));
        *(char**)(SDDS_dataset->parameter[i]) = NULL;
      }
    }
  }
  if (SDDS_dataset->array) {
    for (i=0; i<layout->n_arrays; i++) {
      if (layout->array_definition[i].type==SDDS_STRING) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++)
	  if (((char**)SDDS_dataset->array[i].data)[j])  {
	    free(((char**)SDDS_dataset->array[i].data)[j]);
            ((char**)SDDS_dataset->array[i].data)[j] = NULL;
          }
      }
    }
  }
  if (SDDS_dataset->data) {
    for (i=0; i<layout->n_columns; i++)
      if (SDDS_dataset->data[i]) {
        if (layout->column_definition[i].type==SDDS_STRING) {
          ptr = (char**)SDDS_dataset->data[i];
          for (j=0; j<SDDS_dataset->n_rows_allocated; j++, ptr++)
            if (*ptr) {
              free(*ptr);
              *ptr = NULL;
            }
        }
      }
  }
  return(1);
}

void SDDS_FreeTableStrings(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j;
  char **ptr;
  /* free stored strings */
  if (!SDDS_dataset)
    return;
  for (i=0; i<SDDS_dataset->layout.n_columns; i++)
    if (SDDS_dataset->layout.column_definition[i].type==SDDS_STRING) {
      ptr = (char**)SDDS_dataset->data[i];
      for (j=0; j<SDDS_dataset->n_rows; j++, ptr++)
        if (*ptr) {
          free(*ptr);
          *ptr = NULL;
        }
    }
}

int32_t SDDS_Terminate(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout;
  char **ptr;
  int32_t i, j;
  FILE *fp;
#if SDDS_MPI_IO
  if (SDDS_dataset->parallel_io)
    return SDDS_MPI_Terminate(SDDS_dataset);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_Terminate"))
    return(0);
  layout = &SDDS_dataset->original_layout;
  
  fp = SDDS_dataset->layout.fp;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (SDDS_dataset->layout.gzfp && layout->filename) {
      if ((SDDS_dataset->writing_page) && (SDDS_dataset->layout.data_mode.fixed_row_count)) {
	if (!SDDS_UpdateRowCount(SDDS_dataset))
	  return(0);
      }
      gzclose(SDDS_dataset->layout.gzfp);
    }
  } else {
#endif
    if (fp && layout->filename) {
      if ((SDDS_dataset->writing_page) && (SDDS_dataset->layout.data_mode.fixed_row_count)) {
	if (!SDDS_UpdateRowCount(SDDS_dataset))
	  return(0);
      }
      if (layout->popenUsed) {
	while (fread(termBuffer, sizeof(*termBuffer), 16384, fp)) {
	}
#if defined(vxWorks)
	fprintf(stderr, "pclose is not supported in vxWorks\n");
	exit(1);
#else
	pclose(fp);
#endif
      } else {
	fclose(fp);
      }
    }
#if defined(zLib)
  }
#endif

#if DEBUG
  fprintf(stderr, "Freeing data for file %s\n",
          SDDS_dataset->layout.filename?SDDS_dataset->layout.filename:"NULL");
#endif

  if (SDDS_dataset->pagecount_offset) free(SDDS_dataset->pagecount_offset);
  if (SDDS_dataset->row_flag) free(SDDS_dataset->row_flag);
  if (SDDS_dataset->column_order) free(SDDS_dataset->column_order);
  if (SDDS_dataset->column_flag) free(SDDS_dataset->column_flag);
  if (SDDS_dataset->fBuffer.buffer) free(SDDS_dataset->fBuffer.buffer);
#if DEBUG 
  fprintf(stderr, "freeing parameter data...\n");
#endif
  if (SDDS_dataset->parameter) {
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].type==SDDS_STRING && *(char**)(SDDS_dataset->parameter[i]))
        free(*(char**)(SDDS_dataset->parameter[i]));
      if (SDDS_dataset->parameter[i]) 
        free(SDDS_dataset->parameter[i]);
    }
    free(SDDS_dataset->parameter);
  }
#if DEBUG 
  fprintf(stderr, "freeing array data...\n");
#endif
  if (SDDS_dataset->array) {
    for (i=0; i<layout->n_arrays; i++) {
      if (layout->array_definition[i].type==SDDS_STRING &&
	  !(terminateMode&TERMINATE_DONT_FREE_ARRAY_STRINGS)) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++)
	  if (((char**)SDDS_dataset->array[i].data)[j]) 
	    free(((char**)SDDS_dataset->array[i].data)[j]);
      }
      /*
      if (SDDS_dataset->array[i].definition->type==SDDS_STRING &&
          !(terminateMode&TERMINATE_DONT_FREE_ARRAY_STRINGS)) {
        for (j=0; j<SDDS_dataset->array[i].elements; j++)
          if (((char**)SDDS_dataset->array[i].data)[j]) 
            free(((char**)SDDS_dataset->array[i].data)[j]);
      }
      */
      if (SDDS_dataset->array[i].data) 
        free(SDDS_dataset->array[i].data);
      /* should free the subpointers too, but it would be a lot of trouble for little benefit: */
      if (SDDS_dataset->array[i].pointer && SDDS_dataset->array[i].definition->dimensions!=1) 
        free(SDDS_dataset->array[i].pointer);
      if (SDDS_dataset->array[i].dimension) 
        free(SDDS_dataset->array[i].dimension);
      /* don't touch this--it's done below */
      if (SDDS_dataset->array[i].definition && SDDS_dataset->array[i].definition->name) {
	if (SDDS_dataset->array[i].definition->name != layout->array_definition[i].name)
	  SDDS_FreeArrayDefinition(SDDS_dataset->array[i].definition);
      }
      SDDS_dataset->array[i].definition = NULL;
    }
    free(SDDS_dataset->array);
  }
#if DEBUG 
  fprintf(stderr, "freeing tabular data...\n");
#endif
  if (SDDS_dataset->data) {
    for (i=0; i<layout->n_columns; i++)
      if (SDDS_dataset->data[i]) {
        if (layout->column_definition[i].type==SDDS_STRING &&
            !(terminateMode&TERMINATE_DONT_FREE_TABLE_STRINGS)) {
          ptr = (char**)SDDS_dataset->data[i];
          for (j=0; j<SDDS_dataset->n_rows_allocated; j++, ptr++)
            if (*ptr)
              free(*ptr);
        }
        free(SDDS_dataset->data[i]);
      }
    free(SDDS_dataset->data);
  }
#if DEBUG 
  fprintf(stderr, "freeing layout data...\n");
#endif
  if (layout->description) free(layout->description);
  if (layout->contents == (&SDDS_dataset->layout)->contents)
    (&SDDS_dataset->layout)->contents = NULL;
  if (layout->contents) free(layout->contents);
  if (layout->filename) free(layout->filename);
  if (layout->column_definition) {
    for (i=0; i<layout->n_columns; i++) {
      if (layout->column_index[i]) free(layout->column_index[i]);
      if (layout->column_definition[i].name) free(layout->column_definition[i].name);
      if (layout->column_definition[i].symbol) free(layout->column_definition[i].symbol);
      if (layout->column_definition[i].units) free(layout->column_definition[i].units);
      if (layout->column_definition[i].description) free(layout->column_definition[i].description);
      if (layout->column_definition[i].format_string) free(layout->column_definition[i].format_string);
    }
    free(layout->column_definition);
    free(layout->column_index);
  }
  if (layout->parameter_definition) {
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_index[i]) free(layout->parameter_index[i]);
      if (layout->parameter_definition[i].name) free(layout->parameter_definition[i].name);
      if (layout->parameter_definition[i].symbol) free(layout->parameter_definition[i].symbol);
      if (layout->parameter_definition[i].units) free(layout->parameter_definition[i].units);
      if (layout->parameter_definition[i].description) free(layout->parameter_definition[i].description);
      if (layout->parameter_definition[i].format_string) free(layout->parameter_definition[i].format_string);
      if (layout->parameter_definition[i].fixed_value) free(layout->parameter_definition[i].fixed_value);
    }
    free(layout->parameter_definition);
    free(layout->parameter_index);
  }
  if (layout->array_definition) {
    for (i=0; i<layout->n_arrays; i++) {
      if (layout->array_index[i]) free(layout->array_index[i]);
      if (layout->array_definition[i].name) free(layout->array_definition[i].name);
      if (layout->array_definition[i].symbol) free(layout->array_definition[i].symbol);
      if (layout->array_definition[i].units) free(layout->array_definition[i].units);
      if (layout->array_definition[i].description) free(layout->array_definition[i].description);
      if (layout->array_definition[i].format_string) free(layout->array_definition[i].format_string);
      if (layout->array_definition[i].group_name) free(layout->array_definition[i].group_name);
    }
    free(layout->array_definition);
    free(layout->array_index);
  }
  if (layout->associate_definition) {
    for (i=0; i<layout->n_associates; i++) {
      if (layout->associate_definition[i].name) free(layout->associate_definition[i].name);
      if (layout->associate_definition[i].filename) free(layout->associate_definition[i].filename);
      if (layout->associate_definition[i].path) free(layout->associate_definition[i].path);
      if (layout->associate_definition[i].description) free(layout->associate_definition[i].description);
      if (layout->associate_definition[i].contents) free(layout->associate_definition[i].contents);
    }
    free(layout->associate_definition);
  }
  SDDS_ZeroMemory(&SDDS_dataset->original_layout, sizeof(SDDS_LAYOUT));
  layout = &SDDS_dataset->layout;
  if (layout->contents) free(layout->contents);
  if (layout->column_definition) free(layout->column_definition);
  if (layout->array_definition) free(layout->array_definition);
  if (layout->associate_definition) free(layout->associate_definition);
  if (layout->parameter_definition) free(layout->parameter_definition);
  if (layout->column_index) free(layout->column_index);
  if (layout->parameter_index) free(layout->parameter_index);
  if (layout->array_index) free(layout->array_index);
  SDDS_ZeroMemory(&SDDS_dataset->layout, sizeof(SDDS_LAYOUT));
  SDDS_ZeroMemory(SDDS_dataset, sizeof(SDDS_DATASET));
#if DEBUG 
  fprintf(stderr, "done\n");
#endif
  return(1);
}

int32_t SDDS_UpdateRowCount(SDDS_DATASET *SDDS_dataset) {
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
  int32_t offset, rows;
  char *outputEndianess = NULL;

  if (SDDS_dataset->layout.gzipFile)
    return(1);
  if (!(fp=SDDS_dataset->layout.fp)) {
    SDDS_SetError("Unable to update page--file pointer is NULL (SDDS_UpdateRowCount)");
    return(0);
  }
#if DEBUG
  fprintf(stderr, "Updating rowcount in file %s with pointer %p\n",
          SDDS_dataset->layout.filename?SDDS_dataset->layout.filename:"NULL",
          fp);
#endif
  fBuffer= &SDDS_dataset->fBuffer;
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateRowCount)");
    return(0);
  }
  offset = ftell(fp);
  if (SDDS_fseek(fp, SDDS_dataset->rowcount_offset, 0)==-1) {
    SDDS_SetError("Unable to update page--failure doing fseek (SDDS_UpdateRowCount)");
    return(0);
  }
  rows = SDDS_CountRowsOfInterest(SDDS_dataset)+SDDS_dataset->first_row_in_mem;
  if (SDDS_dataset->layout.data_mode.mode==SDDS_ASCII) {
    fprintf(fp, "%20" PRId32 "\n", rows);
  } else {
  
    if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
      if (((strncmp(outputEndianess, "big", 3) == 0) && (SDDS_IsBigEndianMachine() == 0)) || \
          ((strncmp(outputEndianess, "little", 6) == 0) && (SDDS_IsBigEndianMachine() == 1)))
        SDDS_SwapLong(&rows);
    }
    if (fwrite(&rows, sizeof(rows), 1, fp)!=1) {
      SDDS_SetError("Unable to update page--failure writing number of rows (SDDS_UpdateRowCount)");
      return(0);
    }
  }
  if (SDDS_fseek(fp, offset, 0)==-1) {
    SDDS_SetError("Unable to update page--failure doing fseek to end of page (SDDS_UpdateRowCount)");
    return(0);
  }
  return(1);
}

int32_t SDDS_SetAutoReadRecovery(SDDS_DATASET *SDDS_dataset, uint32_t mode) {
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_SetAutoReadRecovery"))
    return 0;
  if (mode&SDDS_AUTOREADRECOVER) {
    SDDS_dataset->autoRecover = 1;
  } else if (mode&SDDS_NOAUTOREADRECOVER) {
    SDDS_dataset->autoRecover = 0;
  } else {
    SDDS_SetError("Invalid Auto Read Recovery mode (SDDS_SetAutoReadRecovery).");
    return 0;
  }
  return 1;
}

int32_t SDDS_InitializeInputFromSearchPath(SDDS_DATASET *SDDSin, char *file)
{
  char *filename;
  int32_t value;
  if (!(filename=findFileInSearchPath(file))) {
    char *s;
    if (!(s = SDDS_Malloc(sizeof(*s)*(strlen(file)+100))))
      SDDS_SetError("file does not exist in search path (InitializeInputFromSearchPath)");
    else  {
      sprintf(s, "file %s does not exist in search path (InitializeInputFromSearchPath)", file);
      SDDS_SetError(s);
      free(s);
    }
    return 0;
  }
  value = SDDS_InitializeInput(SDDSin, filename);
  free(filename);
  return value;
}
