/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_binary.c
 * contents: SDDS binary data input and output routines
 *
 * M. Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.56  2007/10/15 23:59:27  soliday
 Fixed a problem when combining SDDS files with different numbers of arrays
 and writing it to a non-native binary file.

 Revision 1.55  2006/10/19 22:15:20  soliday
 Fixed a problem with non-native endian binary files that contain fixed
 parameters.

 Revision 1.54  2006/08/31 15:08:01  soliday
 Updated to work with SDDS2

 Revision 1.53  2005/11/04 22:46:51  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.52  2005/09/28 19:29:28  soliday
 Fixed another problem with writting non-native endian files.

 Revision 1.51  2005/08/30 19:11:44  soliday
 Fixed problem when writting output from programs such as sddslogger in
 a non-native endian mode.

 Revision 1.50  2005/08/01 21:33:32  soliday
 Fixed issue with non native endian ordered files when using auto recovery.

 Revision 1.49  2005/07/11 18:50:52  soliday
 Fixed issue when changing the default IO buffer size. The new size was
 not being used everywhere.

 Revision 1.48  2004/11/15 17:02:39  soliday
 Moved and renamed the procedures to write non native binary data from
 sddsendian to SDDSlib.
 Updated so that the endian type can be set to big or little by
 using the environment variable SDDS_OUTPUT_ENDIANESS

 Revision 1.47  2004/01/20 22:32:22  soliday
 Fixed an issue with the auto recovery removing an extra row than was required.

 Revision 1.46  2003/01/08 19:32:18  borland
 Changed syntax for binaryIndexSearch() calls.
 Added some error reporting in SDDS_binary.c (flush buffer).

 Revision 1.45  2002/08/14 16:34:47  soliday
 Added Open License

 Revision 1.44  2002/05/29 19:41:32  soliday
 Cleared out the errors when an auto recovery occurs.

 Revision 1.43  2002/05/26 02:36:44  borland
 Removed debugging statements.

 Revision 1.42  2002/05/26 02:06:37  borland
 1. Call SDDS_SaveLayout() after changing the row count mode, just in case the
 user doesn't do this.  Otherwise, the row count mode gets restored in SDDS_StartPage.
 2. Set new autoRecovered flag whenever auto read recovery is done.  This prevents
 trying to read any more data, which might happen if the file is updated between
 two reads.

 Revision 1.41  2002/02/04 19:57:05  borland
 SDDS_UpdateBinaryPage() and SDDS_UpdateAsciiPage() now free string memory
 from the columns if in FLUSH_TABLE mode.  This prevents a big memory leak.

 Revision 1.40  2002/01/28 21:22:06  shang
 Removed free statement that was causing problems

 Revision 1.39  2002/01/28 20:30:34  soliday
 Shang noticed the array data was not being freed. This is fixed now.

 Revision 1.38  2002/01/22 18:42:42  soliday
 Added missing if defined zlib statement

 Revision 1.37  2002/01/15 22:50:44  soliday
 Added fixed row count, and auto recovery features.

 Revision 1.36  2002/01/10 15:57:03  soliday
 Fixed bug in SDDS_SwapEndsArrayData.

 Revision 1.35  2002/01/07 21:34:41  borland
 Added SDDS_SetRowLimit() and SDDS_GetRowLimit(), which permit the user to
 specify that SDDS tables with more than a certain number of rows are to
 be ignored.  This helps to deal with corrupted files, which sometimes
 occurs when the file server is slow.  Added code to SDDS_ReadBinaryPage()
 and SDDS_ReadAsciiPage() to recognize this specification.

 Revision 1.34  2001/11/30 15:34:36  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.33  2000/10/25 14:01:47  borland
 Modified SDDS_BufferedWrite so that it works more efficiently.  In particular,
 it no longer calls itself recursively for large chunks of data. I also commented
 the code.

 Revision 1.32  2000/10/24 20:49:07  borland
 Modified SDDS_BufferedRead() so it no longer reallocs the file buffer when
 asked to read a large item.  Instead, it simply reads directly into the
 user's memory.  This prevents messing up programs that have set a small
 buffer to reduce memory usage.

 Revision 1.31  2000/10/24 14:53:17  borland
 Added some comments.

 Revision 1.30  2000/10/21 22:14:15  borland
 Now test return values from fflush().

 Revision 1.29  2000/09/19 20:13:21  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.28  2000/08/24 20:03:43  soliday
 Same as previous update except this time it will also work with gzip files.

 Revision 1.27  2000/08/24 19:36:58  soliday
 When reading data, the buffer size is no longer fixed. Instead it can be
 allocated if needed.

 Revision 1.26  2000/06/20 18:04:02  borland
 Added SDDS_SetDefaultIOBufferSize and modified binary read/write
 routines to optionally allow unbuffered read/write.  Removed
 SDDS_SetBufferedRead routine.

 Revision 1.25  2000/04/06 22:16:06  soliday
 Changed include header file for WIN32.

 Revision 1.24  2000/03/30 19:54:14  borland
 In SDDS_WriteBinaryPage(), now flush the buffer prior to determining the
 row count offset.  This is kind of a belt-and-suspenders thing.

 Revision 1.23  2000/03/30 02:34:39  borland
 Added a way to disable buffered reading and revert to unbuffered I/O;
 this is needed by SDDS_InitializeAppendToPage.  Ideally, would modify
 buffered I/O routines to provide ftell() function.  This doesn't work
 for gzip files.

 Revision 1.22  2000/01/18 20:11:53  soliday
 Added support for ZLIB.

 Revision 1.21  1999/12/15 16:47:52  soliday
 Added support for vxWorks.

 Revision 1.20  1999/07/08 21:54:22  soliday
 Added definition for sleep function

 Revision 1.19  1999/07/06 21:35:30  soliday
 Added unistd.h and Winbase.h include lines for sleep function
 
 Revision 1.18  1999/05/25 18:55:46  soliday
 Removed compiler warning on linux.

 Revision 1.17  1998/08/31 21:25:19  borland
 Modified SDDS_WriteBinaryArrays so that it doesn't bomb if an array structure
 has no dimensions array.  This may happen if a SDDS_CopyPage is done from
 a dataset that lacks the array to one that has it.

 Revision 1.16  1998/08/25 01:47:31  borland
 Removed definition of SDDS_FILEBUFFER_SIZE from this file.  Now in
 SDDS.h

 Revision 1.15  1997/12/19 16:58:31  borland
 Added SDDS_Malloc, replaced malloc with SDDS_Malloc in all calls.
 Implemented SDDS_ANY_FLOATING_TYPE as a possible type class for
 column, parameter, and array checks.
 Fixed some problems with lack of checks on array dimension pointers.

 Revision 1.14  1997/10/29 21:24:10  borland
 Fixed problems that occured when an application attempted to copy
 rows into a dataset that has no columns defined.

 Revision 1.13  1996/09/15 14:34:24  borland
 Removed check on file size using fseeks, since it is incompatible with
 buffering.

 Revision 1.12  1996/06/13 19:40:56  borland
 Added fflush call to SDDS_FlushBuffer, plus SDDS_FlushBuffer calls in
 SDDS_UpdateBinaryPage.  Fixed bug that resulted in corrupted data
 from programs that used SDDS_UpdateBinarPage.

 * Revision 1.11  1996/05/02  00:48:24  borland
 * Added buffering to binary output.
 *
 * Revision 1.10  1996/03/21  16:38:34  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.9  1996/03/12  04:00:35  borland
 * Reformatted code to 2-space indentation in K&R style.
 *
 * Revision 1.8  1996/02/15  22:26:33  borland
 * Added test for negative string length to SDDS_ReadBinaryString().
 *
 * Revision 1.7  1996/02/15  00:41:10  borland
 * Added test of popenUsed prior to doing fseek on "unusually long" files;
 * was leading to erroneous indication of a corrupted file when unpacking
 * on a pipe.
 *
 * Revision 1.6  1995/11/20  22:07:34  borland
 * Moved the debugging statement in SDDS_UpdateBinaryPage() to the top of the
 * routine.
 *
 * Revision 1.5  1995/11/13  16:03:51  borland
 * Added support for partially flushed tabular data to SDDS_UpdateBinaryPage
 * and SDDS_WritePage.  Required adding a mode argument to SDDS_UpdateBinaryPage.
 *
 * Revision 1.4  1995/11/01  02:30:05  borland
 * SDDS_binary: Improved checks for large tables; won't check data from pipe,
 * since this doesn't work; checks return codes from fseek and ftell.
 * SDDS_utils: Change SDDS_GetToken to properly handle literal backslash at
 * the end of a quoted token.
 *
 * Revision 1.3  1995/10/31  02:44:13  borland
 * Added code to SDDS_ReadBinaryPage to do a check on any table over 100k points;
 * ftell is used to check file size and make sure that it is at least adequate
 * for a table full of chars.
 *
 * Revision 1.2  1995/09/05  21:10:43  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "mdb.h"
#include <string.h>
#include <errno.h>

#undef DEBUG

#if defined(_WIN32)
#include <windows.h>
#define sleep(sec) Sleep(sec * 1000)
#else
#include <unistd.h>
#endif
#if defined(vxWorks)
#include <time.h>
#endif

#if SDDS_VERSION!=2
#error "SDDS_VERSION does not match the version number of this file"
#endif

static int32_t defaultIOBufferSize = SDDS_FILEBUFFER_SIZE;

/* this routine is obsolete.  Use SDDS_SetDefaultIOBufferSize(0) to effectively turn 
 * off buffering.
 */
int32_t SDDS_SetBufferedRead(int32_t dummy)
{
  return 0;
}

int32_t SDDS_SetDefaultIOBufferSize(int32_t newValue)
{
  int32_t previous;
  if (newValue<0)
    return defaultIOBufferSize;
  if (newValue<128) /* arbitrary limit */
    newValue = 0;
  previous = defaultIOBufferSize;
  defaultIOBufferSize = newValue;
  return previous;
}

int32_t SDDS_BufferedRead(void *target, size_t targetSize, FILE *fp, SDDS_FILEBUFFER *fBuffer)
{
  if (!fBuffer->bufferSize) {
    /* just read into users buffer or seek if no buffer given */
    if (!target) 
      return !fseek(fp, targetSize, SEEK_CUR);
    else
      return fread(target, 1, targetSize, fp)==targetSize;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    /* sufficient data is already in the buffer */
    if (target) {
      memcpy((char*)target, (char*)fBuffer->data, targetSize);
    }
    fBuffer->data += targetSize;
    return 1;
  } else {
    /* need to read additional data into buffer */
    int32_t bytesNeeded, offset;
    fBuffer->bytesLeft += targetSize;  /* adds back amount subtracted above */

    /* first, use the data that is already available. this cleans out the buffer */
    if ((offset=fBuffer->bytesLeft)) {
      /* some data is available in the buffer */
      if (target) {
        memcpy((char*)target, (char*)fBuffer->data, offset);
      }
      bytesNeeded = targetSize - offset;
      fBuffer->bytesLeft = 0;
    } else {
      bytesNeeded = targetSize;
    }
    fBuffer->data = fBuffer->buffer;

    if (fBuffer->bufferSize < bytesNeeded) {
      /* just read what is needed directly into user's memory or seek */
      if (!target)
        return !fseek(fp, bytesNeeded, SEEK_CUR);
      else
        return fread((char*)target+offset, 1, bytesNeeded, fp)==bytesNeeded;
    }

    /* fill the buffer */
    if ((fBuffer->bytesLeft = fread(fBuffer->data, 1, fBuffer->bufferSize, fp))<bytesNeeded)
      return 0;
    if (target)
      memcpy((char*)target+offset, (char*)fBuffer->data, bytesNeeded);
    fBuffer->data += bytesNeeded;
    fBuffer->bytesLeft -= bytesNeeded;
    return 1;
  }
}

#if defined(zLib)
int32_t SDDS_GZipBufferedRead(void *target, size_t targetSize, gzFile *gzfp, SDDS_FILEBUFFER *fBuffer)
{
  if (!fBuffer->bufferSize) {
    SDDS_SetError("You must presently have a nonzero file buffer to use zLib (reading/writing .gz files");
    return 0;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    if (target) {
      memcpy((char*)target, (char*)fBuffer->data, targetSize);
    }
    fBuffer->data += targetSize;
    return 1;
  } else {
    int32_t bytesNeeded, offset;
    fBuffer->bytesLeft += targetSize;
    if ((offset=fBuffer->bytesLeft)) {
      if (target) {
        memcpy((char*)target, (char*)fBuffer->data, offset);
      }
      bytesNeeded = targetSize - offset;
      fBuffer->bytesLeft = 0;
    } else {
      bytesNeeded = targetSize;
    }
    fBuffer->data = fBuffer->buffer;
    
    if (fBuffer->bufferSize < bytesNeeded) {
      /* just read what is needed directly into user's memory or seek */
      if (!target)
        return !gzseek(gzfp, targetSize, SEEK_CUR);
      else
        return gzread(gzfp, target, targetSize)==targetSize;
    }

    if ((fBuffer->bytesLeft = gzread(gzfp, fBuffer->data, fBuffer->bufferSize))<bytesNeeded)
      return 0;
    if (target) {
      memcpy((char*)target+offset, (char*)fBuffer->data, bytesNeeded);
    }
    fBuffer->data += bytesNeeded;
    fBuffer->bytesLeft -= bytesNeeded;
    return 1;
  }
}
#endif

int32_t SDDS_BufferedWrite(void *target, size_t targetSize, FILE *fp, SDDS_FILEBUFFER *fBuffer)
{
  if (!fBuffer->bufferSize) {
    return fwrite(target, 1, targetSize, fp)==targetSize;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    memcpy((char*)fBuffer->data, (char*)target, targetSize);
    fBuffer->data += targetSize;
#ifdef DEBUG
    fprintf(stderr, "SDDS_BufferedWrite of %" PRId32 " bytes done in-memory, %" PRId32 " bytes left\n", targetSize, fBuffer->bytesLeft);
#endif
    return 1;
  } else {
    int32_t lastLeft;
    /* add back what was subtracted in test above.
     * lastLeft is the number of bytes left in the buffer before doing anything 
     * and also the number of bytes from the users data that get copied into the buffer.
     */
    lastLeft = (fBuffer->bytesLeft += targetSize);  
    /* copy part of the data into the buffer and write the buffer out */
    memcpy((char*)fBuffer->data, (char*)target, fBuffer->bytesLeft);
    if (fwrite(fBuffer->buffer, 1, fBuffer->bufferSize, fp)!=fBuffer->bufferSize)
      return 0;
    if (fflush(fp)) {
      SDDS_SetError("Problem flushing file (SDDS_BufferedWrite)");
      SDDS_SetError(strerror(errno));
      return 0;
    }
    /* reset the data pointer and the bytesLeft value.
     * also, determine if the remaining data is too large for the buffer.
     * if so, just write it out.
     */
    fBuffer->data = fBuffer->buffer;
    if ((targetSize -= lastLeft)>(fBuffer->bytesLeft = fBuffer->bufferSize)) {
      return fwrite((char*)target+lastLeft, 1, targetSize, fp)==targetSize;
    }
    /* copy remaining data into the buffer.
     * could do this with a recursive call, but this is more efficient.
     */
    memcpy((char*)fBuffer->data, (char*)target+lastLeft, targetSize);
    fBuffer->data += targetSize;
    fBuffer->bytesLeft -= targetSize;
    return 1;
  }
}

#if defined(zLib)
int32_t SDDS_GZipBufferedWrite(void *target, size_t targetSize, gzFile *gzfp, SDDS_FILEBUFFER *fBuffer)
{
  if (!fBuffer->bufferSize) {
    SDDS_SetError("You must presently have a nonzero file buffer to use zLib (reading/writing .gz files");
    return 0;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    memcpy((char*)fBuffer->data, (char*)target, targetSize);
    fBuffer->data += targetSize;
    return 1;
  } else {
    int32_t lastLeft;
    lastLeft = (fBuffer->bytesLeft + targetSize);
    memcpy((char*)fBuffer->data, (char*)target, lastLeft);
    if (gzwrite(gzfp, fBuffer->buffer, fBuffer->bufferSize)!=fBuffer->bufferSize)
      return 0;
    /*gzflush(gzfp, Z_FULL_FLUSH);*/
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
    return SDDS_GZipBufferedWrite((char*)target+lastLeft, targetSize-lastLeft, gzfp, fBuffer);
  }
}
#endif

int32_t SDDS_FlushBuffer(FILE *fp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t writeBytes;
  if (!fp) {
    SDDS_SetError("Unable to flush buffer: file pointer is NULL. (SDDS_FlushBuffer)");
    return 0;
  }
  if (!fBuffer->bufferSize) {
    if (fflush(fp)) {
      SDDS_SetError("Problem flushing file (SDDS_FlushBuffer.1)");
      SDDS_SetError(strerror(errno));
      return 0;
    }
    return 1;
  }
  if (!fBuffer) {
    SDDS_SetError("Unable to flush buffer: buffer pointer is NULL. (SDDS_FlushBuffer)");
    return 0;
  }
  if ((writeBytes = fBuffer->bufferSize - fBuffer->bytesLeft)) {
    if (writeBytes<0) {
      SDDS_SetError("Unable to flush buffer: negative byte count (SDDS_FlushBuffer).");
      return 0;
    }
#ifdef DEBUG
    fprintf(stderr, "Writing %" PRId32 " bytes to disk\n", writeBytes);
#endif
    if (fwrite(fBuffer->buffer, 1, writeBytes, fp)!=writeBytes) {
      SDDS_SetError("Unable to flush buffer: write operation failed (SDDS_FlushBuffer).");
      return 0;
    }
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
  }
  if (fflush(fp)) {
    SDDS_SetError("Problem flushing file (SDDS_FlushBuffer.2)");
    SDDS_SetError(strerror(errno));
    return 0;
  }
  return 1;
}

#if defined(zLib)
int32_t SDDS_GZipFlushBuffer(gzFile *gzfp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t writeBytes;
  if ((writeBytes = fBuffer->bufferSize - fBuffer->bytesLeft)) {
    if (gzwrite(gzfp,fBuffer->buffer, writeBytes)!=writeBytes)
      return 0;
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
  }
  /*gzflush(gzfp, Z_FULL_FLUSH);*/
  return 1;
}
#endif
  
int32_t SDDS_WriteBinaryPage(SDDS_DATASET *SDDS_dataset)
{
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  int32_t i, rows, fixed_rows;
  /*  static char buffer[SDDS_MAXLINE];*/
  SDDS_FILEBUFFER *fBuffer;
  char *outputEndianess = NULL;
  
  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (((strncmp(outputEndianess, "big", 3) == 0) && (SDDS_IsBigEndianMachine() == 0)) || \
        ((strncmp(outputEndianess, "little", 6) == 0) && (SDDS_IsBigEndianMachine() == 1)))
      return SDDS_WriteNonNativeBinaryPage(SDDS_dataset);
  }

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryPage"))
    return(0);

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!(gzfp=SDDS_dataset->layout.gzfp)) {
      SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteBinaryPage)");
      return(0);
    }
    fBuffer= &SDDS_dataset->fBuffer;
    
    if (!fBuffer->buffer) {
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(defaultIOBufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
	return 0;
      }
      fBuffer->bufferSize = defaultIOBufferSize;
      fBuffer->bytesLeft = defaultIOBufferSize;
    }
    
    rows = SDDS_CountRowsOfInterest(SDDS_dataset); 
    SDDS_dataset->rowcount_offset = gztell(gzfp);
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
      if (!SDDS_GZipBufferedWrite(&fixed_rows, sizeof(fixed_rows), gzfp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteBinaryPage)");
	return(0);
      }
    } else {
      if (!SDDS_GZipBufferedWrite(&rows, sizeof(rows), gzfp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteBinaryPage)");
	return(0);
      }
    }     
    if (!SDDS_WriteBinaryParameters(SDDS_dataset)) {
      SDDS_SetError("Unable to write page--parameter writing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    if (!SDDS_WriteBinaryArrays(SDDS_dataset)) {
      SDDS_SetError("Unable to write page--array writing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    if (SDDS_dataset->layout.n_columns) {
      for (i=0; i<SDDS_dataset->n_rows; i++)
	if (SDDS_dataset->row_flag[i] && !SDDS_WriteBinaryRow(SDDS_dataset, i)) {
	  SDDS_SetError("Unable to write page--row writing problem (SDDS_WriteBinaryPage)");
	  return 0;
	}
    }
    if (!SDDS_GZipFlushBuffer(gzfp, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
    SDDS_dataset->n_rows_written = rows;
    SDDS_dataset->writing_page = 1;
  } else {
#endif
    if (!(fp=SDDS_dataset->layout.fp)) {
      SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteBinaryPage)");
      return(0);
    }
    fBuffer= &SDDS_dataset->fBuffer;
    
    if (!fBuffer->buffer) {
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(defaultIOBufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
	return 0;
      }
      fBuffer->bufferSize = defaultIOBufferSize;
      fBuffer->bytesLeft = defaultIOBufferSize;
    }

    /* Flush any existing data in the output buffer so we can determine the 
     * row count offset for the file.  This is probably unnecessary.
     */
    if (!SDDS_FlushBuffer(fp, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteBinaryPage)");
      return 0;
    }

    /* output the row count and determine its byte offset in the file */
    rows = SDDS_CountRowsOfInterest(SDDS_dataset); 
    SDDS_dataset->rowcount_offset = ftell(fp);
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
#if defined(DEBUG)
      fprintf(stderr, "setting %" PRId32 " fixed rows\n", fixed_rows);
#endif
      if (!SDDS_BufferedWrite(&fixed_rows, sizeof(fixed_rows), fp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteBinaryPage)");
	return(0);
      }
    } else {
#if defined(DEBUG)
      fprintf(stderr,  "setting %" PRId32 " rows\n", rows);
#endif
      if (!SDDS_BufferedWrite(&rows, sizeof(rows), fp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteBinaryPage)");
	return(0);
      }
    }

    /* write the data, using buffered I/O */
    if (!SDDS_WriteBinaryParameters(SDDS_dataset)) {
      SDDS_SetError("Unable to write page--parameter writing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    if (!SDDS_WriteBinaryArrays(SDDS_dataset)) {
      SDDS_SetError("Unable to write page--array writing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    if (SDDS_dataset->layout.n_columns) {
      for (i=0; i<SDDS_dataset->n_rows; i++)
	if (SDDS_dataset->row_flag[i] && !SDDS_WriteBinaryRow(SDDS_dataset, i)) {
	  SDDS_SetError("Unable to write page--row writing problem (SDDS_WriteBinaryPage)");
	  return 0;
	}
    }
    /* flush the page */
    if (!SDDS_FlushBuffer(fp, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteBinaryPage)");
      return 0;
    }
    SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
    SDDS_dataset->n_rows_written = rows;
    SDDS_dataset->writing_page = 1;
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_UpdateBinaryPage(SDDS_DATASET *SDDS_dataset, uint32_t mode)
{
  FILE *fp;
  int32_t i, rows, offset, code, fixed_rows;
  SDDS_FILEBUFFER *fBuffer;
  char *outputEndianess = NULL;
  
  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (((strncmp(outputEndianess, "big", 3) == 0) && (SDDS_IsBigEndianMachine() == 0)) || \
        ((strncmp(outputEndianess, "little", 6) == 0) && (SDDS_IsBigEndianMachine() == 1)))
      return SDDS_UpdateNonNativeBinaryPage(SDDS_dataset, mode);
  }
  
#ifdef DEBUG
  fprintf(stderr, "%" PRId32 " virtual rows present, first=%" PRId32 "\n", 
	  SDDS_CountRowsOfInterest(SDDS_dataset),
	  SDDS_dataset->first_row_in_mem);
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_UpdateBinaryPage"))
    return(0);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    SDDS_SetError("Unable to perform page updates on a gzip file (SDDS_UpdateBinaryPage)");
    return 0;
  }
#endif
  if (!SDDS_dataset->writing_page) {
#ifdef DEBUG
    fprintf(stderr, "Page not being written---calling SDDS_UpdateBinaryPage\n");
#endif
    if (!(code = SDDS_WriteBinaryPage(SDDS_dataset)))
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
    SDDS_SetError("Unable to update page--file pointer is NULL (SDDS_UpdateBinaryPage)");
    return(0);
  }
  fBuffer= &SDDS_dataset->fBuffer;
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateBinaryPage)");
    return 0;
  }
  offset = ftell(fp);
  
  rows = SDDS_CountRowsOfInterest(SDDS_dataset)+SDDS_dataset->first_row_in_mem;
#ifdef DEBUG
  fprintf(stderr, "%" PRId32 " rows stored in table, %" PRId32 " already written\n",
	  rows, SDDS_dataset->n_rows_written);
#endif
  if (rows==SDDS_dataset->n_rows_written)
    return(1);
  if (rows<SDDS_dataset->n_rows_written) {
    SDDS_SetError("Unable to update page--new number of rows less than previous number (SDDS_UpdateBinaryPage)");
    return(0);
  }
  if ((!SDDS_dataset->layout.data_mode.fixed_row_count) ||
      ((SDDS_dataset->n_rows_written / 500) != (rows / 500))) {
    if (SDDS_fseek(fp, SDDS_dataset->rowcount_offset, 0)==-1) {
      SDDS_SetError("Unable to update page--failure doing fseek (SDDS_UpdateBinaryPage)");
      return(0);
    }
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
#if defined(DEBUG)
      fprintf(stderr, "Setting %" PRId32 " fixed rows\n", fixed_rows);
#endif
      if (fwrite(&fixed_rows, sizeof(fixed_rows), 1, fp)!=1) {
	SDDS_SetError("Unable to update page--failure writing number of rows (SDDS_UpdateBinaryPage)");
	return(0);
      }
    } else {
#if defined(DEBUG)
      fprintf(stderr, "Setting %" PRId32 " rows\n", rows);
#endif
      if (fwrite(&rows, sizeof(rows), 1, fp)!=1) {
	SDDS_SetError("Unable to update page--failure writing number of rows (SDDS_UpdateBinaryPage)");
	return(0);
      }
    }
    if (SDDS_fseek(fp, offset, 0)==-1) {
      SDDS_SetError("Unable to update page--failure doing fseek to end of page (SDDS_UpdateBinaryPage)");
      return(0);
    }
  }
  for (i=SDDS_dataset->last_row_written+1; i<SDDS_dataset->n_rows; i++)
    if (SDDS_dataset->row_flag[i] && !SDDS_WriteBinaryRow(SDDS_dataset, i)) {
      SDDS_SetError("Unable to update page--failure writing row (SDDS_UpdateBinaryPage)");
      return(0);
    }
#ifdef DEBUG
  fprintf(stderr, "Flushing buffer\n");
#endif
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateBinaryPage)");
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

#define FSEEK_TRIES 10
int32_t SDDS_fseek(FILE *fp, int32_t offset, int32_t dir)
{
  int32_t try;
#if defined(vxWorks)
  struct timespec rqtp;
  rqtp.tv_sec = 1;
  rqtp.tv_nsec = 0;
#endif
  for (try=0; try<FSEEK_TRIES; try++) {
    if (fseek(fp, offset, dir)==-1) {
#if defined(vxWorks)
      nanosleep(&rqtp,NULL);
#else
      sleep(1);
#endif
    }	
    else
      break;
  }
  if (try==0)
    return 0;
  if (try==FSEEK_TRIES) {
    fputs("warning: fseek problems--unable to recover\n", stderr);
    return -1;
  }
  fputs("warning: fseek problems--recovered\n", stderr);
  return 0;
}

#if defined(zLib)
int32_t SDDS_gzseek(gzFile *gzfp, int32_t offset, int32_t dir)
{
  int32_t try;
#if defined(vxWorks)
  struct timespec rqtp;
  rqtp.tv_sec = 1;
  rqtp.tv_nsec = 0;
#endif
  for (try=0; try<FSEEK_TRIES; try++) {
    if (gzseek(gzfp, offset, dir)==-1) {
#if defined(vxWorks)
      nanosleep(&rqtp,NULL);
#else
      sleep(1);
#endif
    }
    else
      break;
  }
  if (try==0)
    return 0;
  if (try==FSEEK_TRIES) {
    fputs("warning: gzseek problems--unable to recover\n", stderr);
    return -1;
  }
  fputs("warning: gzseek problems--recovered\n", stderr);
  return 0;
}
#endif

int32_t SDDS_WriteBinaryParameters(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  /*  static char buffer[SDDS_MAXLINE];*/
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp =  layout->gzfp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].fixed_value)
	continue;
      if (layout->parameter_definition[i].type==SDDS_STRING) {
	if (!SDDS_GZipWriteBinaryString(*((char**)SDDS_dataset->parameter[i]), gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write parameters--failure writing string (SDDS_WriteBinaryParameters)");
	  return(0);
	}
      }
      else if (!SDDS_GZipBufferedWrite(SDDS_dataset->parameter[i], 
				       SDDS_type_size[layout->parameter_definition[i].type-1],
				       gzfp, fBuffer)) {
	SDDS_SetError("Unable to write parameters--failure writing value (SDDS_WriteBinaryParameters)");
	return(0);
      }
    }
  } else {
#endif
    fp =  layout->fp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].fixed_value)
	continue;
      if (layout->parameter_definition[i].type==SDDS_STRING) {
	if (!SDDS_WriteBinaryString(*((char**)SDDS_dataset->parameter[i]), fp, fBuffer)) {
	  SDDS_SetError("Unable to write parameters--failure writing string (SDDS_WriteBinaryParameters)");
	  return(0);
	}
      }
      else if (!SDDS_BufferedWrite(SDDS_dataset->parameter[i], 
				   SDDS_type_size[layout->parameter_definition[i].type-1],
				   fp, fBuffer)) {
	SDDS_SetError("Unable to write parameters--failure writing value (SDDS_WriteBinaryParameters)");
	return(0);
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_WriteBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, zero=0;
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp =  layout->gzfp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_arrays; i++) {
      if (!SDDS_dataset->array[i].dimension) {
	for (j=0; j<layout->array_definition[i].dimensions; j++) 
	  if (!SDDS_GZipBufferedWrite(&zero, sizeof(zero), gzfp, fBuffer)) {
	    SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_WriteBinaryArrays)");
	    return 0;
	  }
	continue;
      }
      if (!SDDS_GZipBufferedWrite(SDDS_dataset->array[i].dimension, 
				  sizeof(*(SDDS_dataset->array)[i].dimension)*
				  layout->array_definition[i].dimensions, 
				  gzfp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_WriteBinaryArrays)");
	return(0);
      }
      if (layout->array_definition[i].type==SDDS_STRING) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	  if (!SDDS_GZipWriteBinaryString(((char**)SDDS_dataset->array[i].data)[j], gzfp, fBuffer)) {
	    SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteBinaryArrays)");
	    return(0);
	  }
	}
      }
      else if (!SDDS_GZipBufferedWrite(SDDS_dataset->array[i].data, 
				       SDDS_type_size[layout->array_definition[i].type-1]* 
				       SDDS_dataset->array[i].elements, 
				       gzfp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_WriteBinaryArrays)");
	return(0);
      }
    }
  } else {
#endif
    fp =  layout->fp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_arrays; i++) {
      if (!SDDS_dataset->array[i].dimension) {
	for (j=0; j<layout->array_definition[i].dimensions; j++) 
	  if (!SDDS_BufferedWrite(&zero, sizeof(zero), fp, fBuffer)) {
	    SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_WriteBinaryArrays)");
	    return 0;
	  }
	continue;
      }
      if (!SDDS_BufferedWrite(SDDS_dataset->array[i].dimension, 
			      sizeof(*(SDDS_dataset->array)[i].dimension)*
			      layout->array_definition[i].dimensions, 
			      fp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_WriteBinaryArrays)");
	return(0);
      }
      if (layout->array_definition[i].type==SDDS_STRING) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	  if (!SDDS_WriteBinaryString(((char**)SDDS_dataset->array[i].data)[j], fp, fBuffer)) {
	    SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteBinaryArrays)");
	    return(0);
	  }
	}
      }
      else if (!SDDS_BufferedWrite(SDDS_dataset->array[i].data, 
				   SDDS_type_size[layout->array_definition[i].type-1]* 
				   SDDS_dataset->array[i].elements, 
				   fp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_WriteBinaryArrays)");
	return(0);
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_WriteBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_columns; i++) {
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!SDDS_GZipWriteBinaryString(*((char**)SDDS_dataset->data[i]+row), gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write rows--failure writing string (SDDS_WriteBinaryRows)");
	  return(0);
	}
      }
      else {
	size = SDDS_type_size[type-1];
	if (!SDDS_GZipBufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write row--failure writing value (SDDS_WriteBinaryRow)");
	  return(0);
	}
      }
    }
  } else {
#endif
    fp = layout->fp;
    fBuffer = &SDDS_dataset->fBuffer;
    for (i=0; i<layout->n_columns; i++) {
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!SDDS_WriteBinaryString(*((char**)SDDS_dataset->data[i]+row), fp, fBuffer)) {
	  SDDS_SetError("Unable to write rows--failure writing string (SDDS_WriteBinaryRows)");
	  return(0);
	}
      }
      else {
	size = SDDS_type_size[type-1];
	if (!SDDS_BufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, fp, fBuffer)) {
	  SDDS_SetError("Unable to write row--failure writing value (SDDS_WriteBinaryRow)");
	  return(0);
	}
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

static int32_t readRecoveryPossible = 0;
int32_t SDDS_ReadRecoveryPossible(void)
{
  int32_t returnValue;

  returnValue = readRecoveryPossible;
  readRecoveryPossible = 0;
  return returnValue;
}

void SDDS_SetReadRecoveryMode(int32_t mode)
{
  readRecoveryPossible = mode;
}

int32_t SDDS_ReadBinaryPage(SDDS_DATASET *SDDS_dataset, int32_t sparse_interval, int32_t sparse_offset)
{
  SDDS_LAYOUT *layout;
  int32_t n_rows, j, k, alloc_rows, rows_to_store, mod;

  /*  int32_t page_number, i;*/
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_FILEBUFFER *fBuffer;

  if (SDDS_dataset->autoRecovered)
    return -1;
  if (SDDS_dataset->swapByteOrder) {
    return SDDS_ReadNonNativePageSparse(SDDS_dataset, 0,
                                        sparse_interval, sparse_offset);
  }
  
  /*  static char s[SDDS_MAXLINE];*/
  n_rows = 0;
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
  fBuffer = &SDDS_dataset->fBuffer;
  if (!fBuffer->buffer) {
    if (defaultIOBufferSize==0 && 
        (SDDS_dataset->layout.popenUsed || !SDDS_dataset->layout.filename) &&
        (sparse_interval>1 || sparse_offset>0)) {
      SDDS_SetError("The IO buffer size is 0 for data being read from a pipe with sparsing.  This is not supported.");
      return 0;
    }
    if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(defaultIOBufferSize+1)))) {
      SDDS_SetError("Unable to do buffered read--allocation failure");
      return 0;
    }
    fBuffer->bufferSize = defaultIOBufferSize;
    fBuffer->bytesLeft = 0;
  }
  SDDS_dataset->rowcount_offset = -1;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_GZipBufferedRead(&n_rows, sizeof(n_rows), gzfp, &SDDS_dataset->fBuffer)) {
      if (gzeof(gzfp))
	return(SDDS_dataset->page_number=-1);
      SDDS_SetError("Unable to read page--failure reading number of rows (SDDS_ReadBinaryPage)");
      return(0);
    }
  } else {
#endif
    /* This value will only be valid if read buffering is turned off, which is done for
     * certain append operations!   Should really modify SDDS_BufferedRead and SDDS_BufferedWrite
     * to provide ftell capability.
     */
    SDDS_dataset->rowcount_offset = ftell(fp);
    if (!SDDS_BufferedRead(&n_rows, sizeof(n_rows), fp, &SDDS_dataset->fBuffer)) {
      if (feof(fp))
	return(SDDS_dataset->page_number=-1);
      SDDS_SetError("Unable to read page--failure reading number of rows (SDDS_ReadBinaryPage)");
      return(0);
    }
#if defined(zLib)
  }
#endif

#if defined(DEBUG)
  fprintf(stderr, "Expect %" PRId32 " rows of data\n", n_rows);
#endif
  if (n_rows<0) {
    SDDS_SetError("Unable to read page--negative number of rows (SDDS_ReadBinaryPage)");
    return(0);
  }
  if (n_rows>SDDS_GetRowLimit()) {
    /* the number of rows is "unreasonably" large---treat like end-of-file */
    return(SDDS_dataset->page_number=-1);
  }
  
  if (sparse_interval<=0)
    sparse_interval = 1;
  if (sparse_offset<0)
    sparse_offset = 0;

  rows_to_store = n_rows/sparse_interval+2;
  alloc_rows = rows_to_store-SDDS_dataset->n_rows_allocated;

  if (!SDDS_StartPage(SDDS_dataset, 0) || !SDDS_LengthenTable(SDDS_dataset, alloc_rows)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_ReadBinaryPage)");
    return(0);
  }

  layout = &SDDS_dataset->layout;
  /* read the parameter values */
  if (!SDDS_ReadBinaryParameters(SDDS_dataset)) {
    SDDS_SetError("Unable to read page--parameter reading error (SDDS_ReadBinaryPage)");
    return(0);
  }

  /* read the array values */
  if (!SDDS_ReadBinaryArrays(SDDS_dataset)) {
    SDDS_SetError("Unable to read page--array reading error (SDDS_ReadBinaryPage)");
    return(0);
  }

  if (sparse_interval<=1) {
    for (j=0; j<n_rows; j++) {      
      if (!SDDS_ReadBinaryRow(SDDS_dataset, j, 0)) {
        SDDS_dataset->n_rows = j;
	if (SDDS_dataset->autoRecover) {
#if defined(DEBUG)
          fprintf(stderr, "Doing auto-read recovery\n");
#endif
          SDDS_dataset->autoRecovered = 1;
	  SDDS_ClearErrors();
	  return(SDDS_dataset->page_number);
        }
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
    }
    SDDS_dataset->n_rows = j;
    return(SDDS_dataset->page_number);
  } else {
    for (j=0; j<sparse_offset; j++) {
      if (!SDDS_ReadBinaryRow(SDDS_dataset, 0, 1)) {
        SDDS_dataset->n_rows = 0;
	if (SDDS_dataset->autoRecover) {
          SDDS_dataset->autoRecovered = 1;
	  SDDS_ClearErrors();
	  return(SDDS_dataset->page_number);
        }
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
    }
    n_rows -= sparse_offset;
    for (j=k=0; j<n_rows; j++) {
      if (!SDDS_ReadBinaryRow(SDDS_dataset, k, mod=j%sparse_interval)) {
        SDDS_dataset->n_rows = k;
	if (SDDS_dataset->autoRecover) {
          SDDS_dataset->autoRecovered = 1;
	  SDDS_ClearErrors();
	  return(SDDS_dataset->page_number);
        }
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
      k += mod?0:1;
    }
    SDDS_dataset->n_rows = k;
    return(SDDS_dataset->page_number);
  }
}

int32_t SDDS_WriteBinaryString(char *string, FILE *fp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t length;
  static char *dummy_string = "";
  if (!string)
    string = dummy_string;
  length = strlen(string);
  if (!SDDS_BufferedWrite(&length, sizeof(length), fp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing length");
    return(0);
  }
  if (length && !SDDS_BufferedWrite(string, sizeof(*string)*length, fp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing contents");
    return(0);
  }
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteBinaryString(char *string, gzFile *gzfp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t length;
  static char *dummy_string = "";
  if (!string)
    string = dummy_string;
  length = strlen(string);
  if (!SDDS_GZipBufferedWrite(&length, sizeof(length), gzfp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing length");
    return(0);
  }
  if (length && !SDDS_GZipBufferedWrite(string, sizeof(*string)*length, gzfp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing contents");
    return(0);
  }
  return(1);
}
#endif

char *SDDS_ReadBinaryString(FILE *fp, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string;

  if (!SDDS_BufferedRead(&length, sizeof(length), fp, fBuffer) || length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL);
  if (length && !SDDS_BufferedRead(skip?NULL:string, sizeof(*string)*length, fp, fBuffer))
    return(NULL);
  string[length] = 0;
  return(string);
}

#if defined(zLib)
char *SDDS_ReadGZipBinaryString(gzFile *gzfp, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string;

  if (!SDDS_GZipBufferedRead(&length, sizeof(length), gzfp, fBuffer) || length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL);
  if (length && !SDDS_GZipBufferedRead(skip?NULL:string, sizeof(*string)*length, gzfp, fBuffer))
    return(NULL);
  string[length] = 0;
  return(string);
}
#endif

int32_t SDDS_ReadBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row, int32_t skip)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    for (i=0; i<layout->n_columns; i++) {
      if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!skip) {
	  if (((char***)SDDS_dataset->data)[i][row])
	    free((((char***)SDDS_dataset->data)[i][row]));
	  if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_ReadGZipBinaryString(gzfp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadBinaryRows)");
	    return(0);
	  }
	} else {
	  if (!SDDS_ReadGZipBinaryString(gzfp, fBuffer, 1)) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadBinaryRows)");
	    return 0;
	  }
	}
      } else {
	size = SDDS_type_size[type-1];
	if (!SDDS_GZipBufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, gzfp, 
				   fBuffer)) {
	  SDDS_SetError("Unable to read row--failure reading value (SDDS_ReadBinaryRow)");
	  return(0);
	}
      }
    }
  } else {
#endif
    fp =  layout->fp;
    for (i=0; i<layout->n_columns; i++) {
      if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!skip) {
	  if (((char***)SDDS_dataset->data)[i][row])
	    free((((char***)SDDS_dataset->data)[i][row]));
	  if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_ReadBinaryString(fp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadBinaryRows)");
	    return(0);
	  }
	} else {
	  if (!SDDS_ReadBinaryString(fp, fBuffer, 1)) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadBinaryRows)");
	    return 0;
	  }
	}
      } else {
	size = SDDS_type_size[type-1];
	if (!SDDS_BufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, fp, 
			       fBuffer)) {
	  SDDS_SetError("Unable to read row--failure reading value (SDDS_ReadBinaryRow)");
	  return(0);
	}
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_ReadBinaryParameters(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  static char buffer[SDDS_MAXLINE];
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_parameters)
    return(1);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
  } else {
#endif
    fp =  layout->fp;
#if defined(zLib)
  }
#endif
  fBuffer = &SDDS_dataset->fBuffer;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if (layout->parameter_definition[i].fixed_value) {
      strcpy(buffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(buffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
        SDDS_SetError("Unable to read page--parameter scanning error (SDDS_ReadBinaryParameters)");
        return(0);
      }
    }
    else if (layout->parameter_definition[i].type==SDDS_STRING) {
      if (*(char**)SDDS_dataset->parameter[i])
        free(*(char**)SDDS_dataset->parameter[i]);
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_ReadGZipBinaryString(gzfp, fBuffer, 0))) {
	  SDDS_SetError("Unable to read parameters--failure reading string (SDDS_ReadBinaryParameters)");
	  return(0);
	}
      } else {
#endif
	if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_ReadBinaryString(fp, fBuffer, 0))) {
	  SDDS_SetError("Unable to read parameters--failure reading string (SDDS_ReadBinaryParameters)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    } else {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!SDDS_GZipBufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
				   gzfp, fBuffer)) {
	  SDDS_SetError("Unable to read parameters--failure reading value (SDDS_ReadBinaryParameters)");
	  return(0);
	}
      } else {
#endif
	if (!SDDS_BufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
			       fp, fBuffer)) {
	  SDDS_SetError("Unable to read parameters--failure reading value (SDDS_ReadBinaryParameters)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    }
  }
  return(1);
}

int32_t SDDS_ReadBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  /*  static char buffer[SDDS_MAXLINE];*/
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_ARRAY *array;
  SDDS_FILEBUFFER *fBuffer;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadBinaryArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_arrays)
    return(1);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
  } else {
#endif
    fp =  layout->fp;
#if defined(zLib)
  }
#endif
  fBuffer = &SDDS_dataset->fBuffer;
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to read array--pointer to structure storage area is NULL (SDDS_ReadBinaryArrays)");
    return(0);
  }
  for (i=0; i<layout->n_arrays; i++) {
    array = SDDS_dataset->array+i;
    if (array->definition && !SDDS_FreeArrayDefinition(array->definition)) {
      SDDS_SetError("Unable to get array--array definition corrupted (SDDS_ReadBinaryArrays)");
      return(0);
    }
    if (!SDDS_CopyArrayDefinition(&array->definition, layout->array_definition+i)) {
      SDDS_SetError("Unable to read array--definition copy failed (SDDS_ReadBinaryArrays)");
      return(0);
    }
    /*if (array->dimension) free(array->dimension); */
    if (!(array->dimension = SDDS_Realloc(array->dimension, 
                                          sizeof(*array->dimension)*array->definition->dimensions))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadBinaryArrays)");
      return(0);
    }
#if defined(zLib)
    if (SDDS_dataset->layout.gzipFile) {
      if (!SDDS_GZipBufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
				 gzfp, fBuffer)) {
	SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_ReadBinaryArrays)");
	return(0);
      }
    } else {
#endif
      if (!SDDS_BufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
			     fp, fBuffer)) {
	SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_ReadBinaryArrays)");
	return(0);
      }
#if defined(zLib)
    }
#endif
    array->elements = 1;
    for (j=0; j<array->definition->dimensions; j++)
      array->elements *= array->dimension[j];
    if (array->data) free(array->data);
    array->data = array->pointer = NULL;
    if (array->elements==0)
      continue;
    if (array->elements<0) {
      SDDS_SetError("Unable to read array--number of elements is negative (SDDS_ReadBinaryArrays)");
      return(0);
    }
    if (!(array->data = SDDS_Realloc(array->data, array->elements*SDDS_type_size[array->definition->type-1]))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadBinaryArrays)");
      return(0);
    }
    if (array->definition->type==SDDS_STRING) {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	for (j=0; j<array->elements; j++) {
	  if (!( ((char**)(array->data))[j] = SDDS_ReadGZipBinaryString(gzfp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read arrays--failure reading string (SDDS_ReadBinaryArrays)");
	    return(0);
	  }
	}
      } else {
#endif
	for (j=0; j<array->elements; j++) {
	  if (!( ((char**)(array->data))[j] = SDDS_ReadBinaryString(fp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read arrays--failure reading string (SDDS_ReadBinaryArrays)");
	    return(0);
	  }
	}
#if defined(zLib)
      }
#endif
    } else {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!SDDS_GZipBufferedRead(array->data, 
				   SDDS_type_size[array->definition->type-1]*array->elements, 
				   gzfp, fBuffer)) {
	  SDDS_SetError("Unable to read arrays--failure reading values (SDDS_ReadBinaryArrays)");
	  return(0);
	}
      } else {
#endif
	if (!SDDS_BufferedRead(array->data, 
			       SDDS_type_size[array->definition->type-1]*array->elements, 
			       fp, fBuffer)) {
	  SDDS_SetError("Unable to read arrays--failure reading values (SDDS_ReadBinaryArrays)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    }
  }
  return(1);
}

int32_t SDDS_SwapEndsColumnData(SDDS_DATASET *SDDSin)
{
  int32_t i, row;
  SDDS_LAYOUT *layout;
  short *sData;
  unsigned short *suData;
  int32_t *lData;
  uint32_t *luData;
  float *fData;
  double *dData;
  
  layout = &SDDSin->layout;
  for (i=0; i<layout->n_columns; i++) {
    switch (layout->column_definition[i].type) {
    case SDDS_SHORT:
      sData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapShort(sData+row);
      break;
    case SDDS_USHORT:
      suData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapUShort(suData+row);
      break;
    case SDDS_LONG:
      lData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapLong(lData+row);
      break;
    case SDDS_ULONG:
      luData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapULong(luData+row);
      break;
    case SDDS_DOUBLE:
      dData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapDouble(dData+row);
      break;
    case SDDS_FLOAT:
      fData = SDDSin->data[i];
      for (row=0; row<SDDSin->n_rows; row++)
        SDDS_SwapFloat(fData+row);
      break;
    default:
      break;
    }
  }
  return(1);
}

int32_t SDDS_SwapEndsParameterData(SDDS_DATASET *SDDSin) 
{
  int32_t i;
  SDDS_LAYOUT *layout;
  short *sData;
  unsigned short *suData;
  int32_t *lData;
  uint32_t *luData;
  float *fData;
  double *dData;

  layout = &SDDSin->layout;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value) {
      continue;
    }
    switch (layout->parameter_definition[i].type) {
    case SDDS_SHORT:
      sData = SDDSin->parameter[i];
      SDDS_SwapShort(sData);
      break;
    case SDDS_USHORT:
      suData = SDDSin->parameter[i];
      SDDS_SwapUShort(suData);
      break;
    case SDDS_LONG:
      lData = SDDSin->parameter[i];
      SDDS_SwapLong(lData);
      break;
    case SDDS_ULONG:
      luData = SDDSin->parameter[i];
      SDDS_SwapULong(luData);
      break;
    case SDDS_DOUBLE:
      dData = SDDSin->parameter[i];
      SDDS_SwapDouble(dData);
      break;
    case SDDS_FLOAT:
      fData = SDDSin->parameter[i];
      SDDS_SwapFloat(fData);
      break;
    default:
      break;
    }
  }
  return(1);
}

int32_t SDDS_SwapEndsArrayData(SDDS_DATASET *SDDSin)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  short *sData;
  unsigned short *suData;
  int32_t *lData;
  uint32_t *luData;
  float *fData;
  double *dData;

  layout = &SDDSin->layout;

  for (i=0; i<layout->n_arrays; i++) {
    switch (layout->array_definition[i].type) {
    case SDDS_SHORT:
      sData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapShort(sData+j);
      break;
    case SDDS_USHORT:
      suData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapUShort(suData+j);
      break;
    case SDDS_LONG:
      lData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapLong(lData+j);
      break;
    case SDDS_ULONG:
      luData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapULong(luData+j);
      break;
    case SDDS_DOUBLE:
      dData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapDouble(dData+j);
      break;
    case SDDS_FLOAT:
      fData = SDDSin->array[i].data;
      for (j=0; j<SDDSin->array[i].elements; j++)
        SDDS_SwapFloat(fData+j);
      break;
    default:
      break;
    }
  }
  return(1);
}

void SDDS_SwapShort(short *data)
{
  unsigned char c1;
  c1 = *((char*)data);
  *((char*)data) = *(((char*)data)+1);
  *(((char*)data)+1) = c1;
}

void SDDS_SwapUShort(unsigned short *data)
{
  unsigned char c1;
  c1 = *((char*)data);
  *((char*)data) = *(((char*)data)+1);
  *(((char*)data)+1) = c1;
}

void SDDS_SwapLong(int32_t *data)
{
  int32_t copy;
  short i, j;
  copy = *data;
  for (i=0, j=3; i<4; i++, j--) 
    *(((char*)data)+i) = *(((char*)&copy)+j);
}

void SDDS_SwapULong(uint32_t *data)
{
  uint32_t copy;
  short i, j;
  copy = *data;
  for (i=0, j=3; i<4; i++, j--) 
    *(((char*)data)+i) = *(((char*)&copy)+j);
}

void SDDS_SwapFloat(float *data)
{
  float copy;
  short i, j;
  copy = *data;
  for (i=0, j=3; i<4; i++, j--) 
    *(((char*)data)+i) = *(((char*)&copy)+j);
}

void SDDS_SwapDouble(double *data)
{
  double copy;
  short i, j;
  copy = *data;
  for (i=0, j=7; i<8; i++, j--) 
    *(((char*)data)+i) = *(((char*)&copy)+j);
}

int32_t SDDS_ReadNonNativePage(SDDS_DATASET *SDDS_dataset)
{
  return SDDS_ReadNonNativePageSparse(SDDS_dataset, 0, 1, 0);
}

int32_t SDDS_ReadNonNativePageSparse(SDDS_DATASET *SDDS_dataset, uint32_t mode,
				  int32_t sparse_interval,
				  int32_t sparse_offset)
     /* the mode argument is to support future expansion */
{
  int32_t retval;
  /*  SDDS_LAYOUT layout_copy;*/

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadNonNativePageSparse"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't read page--file is disconnected (SDDS_ReadNonNativePageSparse)");
    return 0;
  }
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_dataset->layout.gzfp) {
      SDDS_SetError("Unable to read page--NULL file pointer (SDDS_ReadNonNativePageSparse)");
      return(0);
    }
  } else {
#endif
    if (!SDDS_dataset->layout.fp) {
      SDDS_SetError("Unable to read page--NULL file pointer (SDDS_ReadNonNativePageSparse)");
      return(0);
    }
#if defined(zLib)
  }
#endif
  if (SDDS_dataset->original_layout.data_mode.mode==SDDS_ASCII) {
    if ((retval=SDDS_ReadAsciiPage(SDDS_dataset, sparse_interval, sparse_offset))<1) {
      return(retval);
    }
  } else if (SDDS_dataset->original_layout.data_mode.mode==SDDS_BINARY) {
    if ((retval=SDDS_ReadNonNativeBinaryPage(SDDS_dataset, sparse_interval, sparse_offset))<1) {
      return(retval);
    }
  } else {
    SDDS_SetError("Unable to read page--unrecognized data mode (SDDS_ReadNonNativePageSparse)");
    return(0);
  }
  return(retval);
}

int32_t SDDS_ReadNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset, int32_t sparse_interval, int32_t sparse_offset)
{
  SDDS_LAYOUT *layout;
  int32_t n_rows, j, k, alloc_rows, rows_to_store, mod;
  /*  int32_t page_number, i;*/
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_FILEBUFFER *fBuffer;

  /*  static char s[SDDS_MAXLINE];*/
  n_rows = 0;
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
  fBuffer = &SDDS_dataset->fBuffer;
  if (!fBuffer->buffer) {
    if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*defaultIOBufferSize))) {
      SDDS_SetError("Unable to do buffered read--allocation failure");
      return 0;
    }
    fBuffer->bufferSize = defaultIOBufferSize;
    fBuffer->bytesLeft = 0;
  }
  SDDS_dataset->rowcount_offset = -1;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_GZipBufferedRead(&n_rows, sizeof(n_rows), gzfp, &SDDS_dataset->fBuffer)) {
      if (gzeof(gzfp))
	return(SDDS_dataset->page_number=-1);
      SDDS_SetError("Unable to read page--failure reading number of rows (SDDS_ReadNonNativeBinaryPage)");
      return(0);
    }
  } else {
#endif
    /* This value will only be valid if read buffering is turned off, which is done for
     * certain append operations!   Should really modify SDDS_BufferedRead and SDDS_BufferedWrite
     * to provide ftell capability.
     */
    SDDS_dataset->rowcount_offset = ftell(fp);
    if (!SDDS_BufferedRead(&n_rows, sizeof(n_rows), fp, &SDDS_dataset->fBuffer)) {
      if (feof(fp))
	return(SDDS_dataset->page_number=-1);
      SDDS_SetError("Unable to read page--failure reading number of rows (SDDS_ReadNonNativeBinaryPage)");
      return(0);
    }
#if defined(zLib)
  }
#endif
  SDDS_SwapLong(&n_rows);
  if (n_rows<0) {
    SDDS_SetError("Unable to read page--negative number of rows (SDDS_ReadNonNativeBinaryPage)");
    return(0);
  }
  if (sparse_interval<=0)
    sparse_interval = 1;
  if (sparse_offset<0)
    sparse_offset = 0;

  rows_to_store = n_rows/sparse_interval+2;
  alloc_rows = rows_to_store-SDDS_dataset->n_rows_allocated;
  if (!SDDS_StartPage(SDDS_dataset, 0) || !SDDS_LengthenTable(SDDS_dataset, alloc_rows)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_ReadNonNativeBinaryPage)");
    return(0);
  }

  layout = &SDDS_dataset->layout;
  /* read the parameter values */
  if (!SDDS_ReadNonNativeBinaryParameters(SDDS_dataset)) {
    SDDS_SetError("Unable to read page--parameter reading error (SDDS_ReadNonNativeBinaryPage)");
    return(0);
  }

  /* read the array values */
  if (!SDDS_ReadNonNativeBinaryArrays(SDDS_dataset)) {
    SDDS_SetError("Unable to read page--array reading error (SDDS_ReadNonNativeBinaryPage)");
    return(0);
  }
  if (sparse_interval<=1) {
    for (j=0; j<n_rows; j++) {
      if (!SDDS_ReadNonNativeBinaryRow(SDDS_dataset, j, 0)) {
        SDDS_dataset->n_rows = j - 1;
	if (SDDS_dataset->autoRecover) {
	  SDDS_ClearErrors();
          SDDS_SwapEndsColumnData(SDDS_dataset);
	  return(SDDS_dataset->page_number);
	}
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadNonNativeBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
    }
    SDDS_dataset->n_rows = j;
    SDDS_SwapEndsColumnData(SDDS_dataset);
    return(SDDS_dataset->page_number);
  } else {
    for (j=0; j<sparse_offset; j++) {
      if (!SDDS_ReadNonNativeBinaryRow(SDDS_dataset, 0, 1)) {
        SDDS_dataset->n_rows = 0;
	if (SDDS_dataset->autoRecover) {
	  SDDS_ClearErrors();
          SDDS_SwapEndsColumnData(SDDS_dataset);
	  return(SDDS_dataset->page_number);
	}
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadNonNativeBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
    }
    n_rows -= sparse_offset;
    for (j=k=0; j<n_rows; j++) {
      if (!SDDS_ReadNonNativeBinaryRow(SDDS_dataset, k, mod=j%sparse_interval)) {
        SDDS_dataset->n_rows = k - 1;
	if (SDDS_dataset->autoRecover) {
	  SDDS_ClearErrors();
          SDDS_SwapEndsColumnData(SDDS_dataset);
	  return(SDDS_dataset->page_number);
	}
        SDDS_SetError("Unable to read page--error reading data row (SDDS_ReadNonNativeBinaryPage)");
        SDDS_SetReadRecoveryMode(1);
        return(0);
      }
      k += mod?0:1;
    }
    SDDS_dataset->n_rows = k;
    SDDS_SwapEndsColumnData(SDDS_dataset);
    return(SDDS_dataset->page_number);
  }
}

int32_t SDDS_ReadNonNativeBinaryParameters(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  static char buffer[SDDS_MAXLINE];
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadNonNativeBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_parameters)
    return(1);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
  } else {
#endif
    fp =  layout->fp;
#if defined(zLib)
  }
#endif
  fBuffer = &SDDS_dataset->fBuffer;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if (layout->parameter_definition[i].fixed_value) {
      strcpy(buffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(buffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
        SDDS_SetError("Unable to read page--parameter scanning error (SDDS_ReadNonNativeBinaryParameters)");
        return(0);
      }
    }
    else if (layout->parameter_definition[i].type==SDDS_STRING) {
      if (*(char**)SDDS_dataset->parameter[i])
        free(*(char**)SDDS_dataset->parameter[i]);
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_ReadNonNativeGZipBinaryString(gzfp, fBuffer, 0))) {
	  SDDS_SetError("Unable to read parameters--failure reading string (SDDS_ReadNonNativeBinaryParameters)");
	  return(0);
	}
      } else {
#endif
	if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_ReadNonNativeBinaryString(fp, fBuffer, 0))) {
	  SDDS_SetError("Unable to read parameters--failure reading string (SDDS_ReadNonNativeBinaryParameters)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    } else {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!SDDS_GZipBufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
				   gzfp, fBuffer)) {
	  SDDS_SetError("Unable to read parameters--failure reading value (SDDS_ReadNonNativeBinaryParameters)");
	  return(0);
	}
      } else {
#endif
	if (!SDDS_BufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
			       fp, fBuffer)) {
	  SDDS_SetError("Unable to read parameters--failure reading value (SDDS_ReadNonNativeBinaryParameters)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    }
  }
  SDDS_SwapEndsParameterData(SDDS_dataset);
  return(1);
}

int32_t SDDS_ReadNonNativeBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  /*  static char buffer[SDDS_MAXLINE];*/
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif
  FILE *fp=NULL;
  SDDS_ARRAY *array;
  SDDS_FILEBUFFER *fBuffer;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadNonNativeBinaryArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_arrays)
    return(1);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
  } else {
#endif
    fp =  layout->fp;
#if defined(zLib)
  }
#endif
  fBuffer = &SDDS_dataset->fBuffer;
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to read array--pointer to structure storage area is NULL (SDDS_ReadNonNativeBinaryArrays)");
    return(0);
  }
  for (i=0; i<layout->n_arrays; i++) {
    array = SDDS_dataset->array+i;
    if (array->definition && !SDDS_FreeArrayDefinition(array->definition)) {
      SDDS_SetError("Unable to get array--array definition corrupted (SDDS_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (!SDDS_CopyArrayDefinition(&array->definition, layout->array_definition+i)) {
      SDDS_SetError("Unable to read array--definition copy failed (SDDS_ReadNonNativeBinaryArrays)");
      return(0);
    }
    /*if (array->dimension) free(array->dimension);*/
    if (!(array->dimension = SDDS_Realloc(array->dimension, 
                                          sizeof(*array->dimension)*array->definition->dimensions))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadNonNativeBinaryArrays)");
      return(0);
    }
#if defined(zLib)
    if (SDDS_dataset->layout.gzipFile) {
      if (!SDDS_GZipBufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
				 gzfp, fBuffer)) {
	SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_ReadNonNativeBinaryArrays)");
	return(0);
      }
    } else {
#endif
      if (!SDDS_BufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
			     fp, fBuffer)) {
	SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_ReadNonNativeBinaryArrays)");
	return(0);
      }
#if defined(zLib)
    }
#endif
    array->elements = 1;
    for (j=0; j<array->definition->dimensions; j++) {
      SDDS_SwapLong(&(array->dimension[j]));
      array->elements *= array->dimension[j];
    }
    if (array->data) free(array->data);
    array->data = array->pointer = NULL;
    if (array->elements==0)
      continue;
    if (array->elements<0) {
      SDDS_SetError("Unable to read array--number of elements is negative (SDDS_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (!(array->data = SDDS_Realloc(array->data, array->elements*SDDS_type_size[array->definition->type-1]))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (array->definition->type==SDDS_STRING) {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	for (j=0; j<array->elements; j++) {
	  if (!( ((char**)(array->data))[j] = SDDS_ReadNonNativeGZipBinaryString(gzfp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read arrays--failure reading string (SDDS_ReadNonNativeBinaryArrays)");
	    return(0);
	  }
	}
      } else {
#endif
	for (j=0; j<array->elements; j++) {
	  if (!( ((char**)(array->data))[j] = SDDS_ReadNonNativeBinaryString(fp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read arrays--failure reading string (SDDS_ReadNonNativeBinaryArrays)");
	    return(0);
	  }
	}
#if defined(zLib)
      }
#endif
    } else {
#if defined(zLib)
      if (SDDS_dataset->layout.gzipFile) {
	if (!SDDS_GZipBufferedRead(array->data, 
				   SDDS_type_size[array->definition->type-1]*array->elements, 
				   gzfp, fBuffer)) {
	  SDDS_SetError("Unable to read arrays--failure reading values (SDDS_ReadNonNativeBinaryArrays)");
	  return(0);
	}
      } else {
#endif
	if (!SDDS_BufferedRead(array->data, 
			       SDDS_type_size[array->definition->type-1]*array->elements, 
			       fp, fBuffer)) {
	  SDDS_SetError("Unable to read arrays--failure reading values (SDDS_ReadNonNativeBinaryArrays)");
	  return(0);
	}
#if defined(zLib)
      }
#endif
    }
  }
  SDDS_SwapEndsArrayData(SDDS_dataset);
  return(1);
}

int32_t SDDS_ReadNonNativeBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row, int32_t skip)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout;
#if defined(zLib)
  gzFile *gzfp;
#endif
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadNonNativeBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    for (i=0; i<layout->n_columns; i++) {
      if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!skip) {
	  if (((char***)SDDS_dataset->data)[i][row])
	    free((((char***)SDDS_dataset->data)[i][row]));
	  if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_ReadNonNativeGZipBinaryString(gzfp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadNonNativeBinaryRow)");
	    return(0);
	  }
	} else {
	  if (!SDDS_ReadNonNativeGZipBinaryString(gzfp, fBuffer, 1)) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadNonNativeBinaryRow)");
	    return 0;
	  }
	}
      } else {
	size = SDDS_type_size[type-1];
	if (!SDDS_GZipBufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, gzfp, 
				   fBuffer)) {
	  SDDS_SetError("Unable to read row--failure reading value (SDDS_ReadNonNativeBinaryRow)");
	  return(0);
	}
      }
    }
  } else {
#endif
    fp =  layout->fp;
    for (i=0; i<layout->n_columns; i++) {
      if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
	continue;
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!skip) {
	  if (((char***)SDDS_dataset->data)[i][row])
	    free((((char***)SDDS_dataset->data)[i][row]));
	  if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_ReadNonNativeBinaryString(fp, fBuffer, 0))) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadNonNativeBinaryRow)");
	    return(0);
	  }
	} else {
	  if (!SDDS_ReadNonNativeBinaryString(fp, fBuffer, 1)) {
	    SDDS_SetError("Unable to read rows--failure reading string (SDDS_ReadNonNativeBinaryRow)");
	    return 0;
	  }
	}
      } else {
	size = SDDS_type_size[type-1];
	if (!SDDS_BufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, fp, 
			       fBuffer)) {
	  SDDS_SetError("Unable to read row--failure reading value (SDDS_ReadNonNativeBinaryRow)");
	  return(0);
	}
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

char *SDDS_ReadNonNativeBinaryString(FILE *fp, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string;

  if (!SDDS_BufferedRead(&length, sizeof(length), fp, fBuffer))
    return(0);
  SDDS_SwapLong(&length);
  if (length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL);
  if (length && !SDDS_BufferedRead(skip?NULL:string, sizeof(*string)*length, fp, fBuffer))
    return(NULL);
  string[length] = 0;
  return(string);
}

#if defined(zLib)
char *SDDS_ReadNonNativeGZipBinaryString(gzFile *gzfp, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string;

  if (!SDDS_GZipBufferedRead(&length, sizeof(length), gzfp, fBuffer))
    return(0);
  SDDS_SwapLong(&length);
  if (length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL);
  if (length && !SDDS_GZipBufferedRead(skip?NULL:string, sizeof(*string)*length, gzfp, fBuffer))
    return(NULL);
  string[length] = 0;
  return(string);
}
#endif




















int32_t SDDS_WriteNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset)
/* Write binary page with byte order reversed.  Used for little-to-big
 * and big-to-little endian conversion
 */
{
  FILE *fp;
  int32_t i, rows, fixed_rows;
  SDDS_FILEBUFFER *fBuffer;
#if defined(zLib)
  gzFile *gzfp=NULL;
#endif

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteNonNativeBinaryPage"))
    return(0);
  if (!(fp=SDDS_dataset->layout.fp)) {
    SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteNonNativeBinaryPage)");
    return(0);
  }
  fBuffer= &SDDS_dataset->fBuffer;
  
  if (!fBuffer->buffer) {
    if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*defaultIOBufferSize))) {
      SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteNonNativeBinaryPage)");
      return 0;
    }
    fBuffer->bufferSize = defaultIOBufferSize;
    fBuffer->bytesLeft = defaultIOBufferSize;
  }
  
  rows = SDDS_CountRowsOfInterest(SDDS_dataset); 
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!(gzfp=SDDS_dataset->layout.gzfp)) {
      SDDS_SetError("Unable to write page--file pointer is NULL (SDDS_WriteNonNativeBinaryPage)");
      return(0);
    }
    SDDS_dataset->rowcount_offset = gztell(gzfp);
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
      SDDS_SwapLong(&fixed_rows);
      if (!SDDS_GZipBufferedWrite(&fixed_rows, sizeof(fixed_rows), gzfp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteNonNativeBinaryPage)");
	return(0);
      }
      SDDS_SwapLong(&fixed_rows);
    } else {
      SDDS_SwapLong(&rows);
      if (!SDDS_GZipBufferedWrite(&rows, sizeof(rows), gzfp, fBuffer)) {
        SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteNonNativeBinaryPage)");
        return(0);
      }
      SDDS_SwapLong(&rows);
    }
  } else {
#endif
    SDDS_dataset->rowcount_offset = ftell(fp);
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
      SDDS_SwapLong(&fixed_rows);
      if (!SDDS_BufferedWrite(&fixed_rows, sizeof(fixed_rows), fp, fBuffer)) {
	SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteNonNativeBinaryPage)");
	return(0);
      }
      SDDS_SwapLong(&fixed_rows);
    } else {
      SDDS_SwapLong(&rows);
      if (!SDDS_BufferedWrite(&rows, sizeof(rows), fp, fBuffer)) {
        SDDS_SetError("Unable to write page--failure writing number of rows (SDDS_WriteNonNativeBinaryPage)");
        return(0);
      }
      SDDS_SwapLong(&rows);
    }
#if defined(zLib)
  }
#endif
  if (!SDDS_WriteNonNativeBinaryParameters(SDDS_dataset)) {
    SDDS_SetError("Unable to write page--parameter writing problem (SDDS_WriteNonNativeBinaryPage)");
    return 0;
  }
  if (!SDDS_WriteNonNativeBinaryArrays(SDDS_dataset)) {
    SDDS_SetError("Unable to write page--array writing problem (SDDS_WriteNonNativeBinaryPage)");
    return 0;
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
  if (SDDS_dataset->layout.n_columns) {
    for (i=0; i<SDDS_dataset->n_rows; i++)
      if (SDDS_dataset->row_flag[i])
        if (!SDDS_WriteNonNativeBinaryRow(SDDS_dataset, i)) {
          SDDS_SetError("Unable to write page--row writing problem (SDDS_WriteNonNativeBinaryPage)");
          return 0;
        }
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!SDDS_GZipFlushBuffer(gzfp, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteNonNativeBinaryPage)");
      return 0;
    }
  } else {
#endif
    if (!SDDS_FlushBuffer(fp, fBuffer)) {
      SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_WriteNonNativeBinaryPage)");
      return 0;
    }
#if defined(zLib)
  }
#endif
  SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
  SDDS_dataset->n_rows_written = rows;
  SDDS_dataset->writing_page = 1;
  return(1);
}

int32_t SDDS_WriteNonNativeBinaryParameters(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  SDDS_LAYOUT *layout;
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
#if defined(zLib)
  gzFile *gzfp;
#endif

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteNonNativeBinaryParameters"))
    return(0);

  SDDS_SwapEndsParameterData(SDDS_dataset);

#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    if (!(gzfp=SDDS_dataset->layout.gzfp)) {
      SDDS_SetError("Unable to write parameters--file pointer is NULL (SDDS_WriteNonNativeBinaryParameters)");
      return(0);
    }
  }
#endif

  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].fixed_value)
	continue;
      if (layout->parameter_definition[i].type==SDDS_STRING) {
	if (!SDDS_GZipWriteNonNativeBinaryString(*((char**)SDDS_dataset->parameter[i]), gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write parameters--failure writing string (SDDS_WriteNonNativeBinaryParameters)");
	  SDDS_SwapEndsParameterData(SDDS_dataset);
	  return(0);
	}
      }
      else if (!SDDS_GZipBufferedWrite(SDDS_dataset->parameter[i], 
				   SDDS_type_size[layout->parameter_definition[i].type-1],
				   gzfp, fBuffer)) {
	SDDS_SetError("Unable to write parameters--failure writing value (SDDS_WriteBinaryParameters)");
	SDDS_SwapEndsParameterData(SDDS_dataset);
	return(0);
      }
    }
  } else {
#endif
    fp =  layout->fp;
    for (i=0; i<layout->n_parameters; i++) {
      if (layout->parameter_definition[i].fixed_value)
	continue;
      if (layout->parameter_definition[i].type==SDDS_STRING) {
	if (!SDDS_WriteNonNativeBinaryString(*((char**)SDDS_dataset->parameter[i]), fp, fBuffer)) {
	  SDDS_SetError("Unable to write parameters--failure writing string (SDDS_WriteNonNativeBinaryParameters)");
	  SDDS_SwapEndsParameterData(SDDS_dataset);
	  return(0);
	}
      }
      else if (!SDDS_BufferedWrite(SDDS_dataset->parameter[i], 
				   SDDS_type_size[layout->parameter_definition[i].type-1],
				   fp, fBuffer)) {
	SDDS_SetError("Unable to write parameters--failure writing value (SDDS_WriteBinaryParameters)");
	SDDS_SwapEndsParameterData(SDDS_dataset);
	return(0);
      }
    }
#if defined(zLib)
  }
#endif

  SDDS_SwapEndsParameterData(SDDS_dataset);
  return(1);
}

int32_t SDDS_WriteNonNativeBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, dimension, zero=0;
  SDDS_LAYOUT *layout;
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
#if defined(zLib)
  gzFile *gzfp;
#endif
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteNonNativeBinaryArrays"))
    return(0);
  SDDS_SwapEndsArrayData(SDDS_dataset);
  
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp =  layout->gzfp;
    for (i=0; i<layout->n_arrays; i++) {
      if (!SDDS_dataset->array[i].dimension) {
        for (j=0; j<layout->array_definition[i].dimensions; j++)
          if (!SDDS_GZipBufferedWrite(&zero, sizeof(zero), gzfp, fBuffer)) {
            SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_WriteNonNativeBinaryArrays)");
            SDDS_SwapEndsArrayData(SDDS_dataset);
            return 0;
          }
        continue;
      }

      for (j=0; j<layout->array_definition[i].dimensions; j++) {
	dimension = SDDS_dataset->array[i].dimension[j];
	SDDS_SwapLong(&dimension);
	if (!SDDS_GZipBufferedWrite(&dimension, sizeof(dimension),
				gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_WriteNonNativeBinaryArrays)");
	  SDDS_SwapEndsArrayData(SDDS_dataset);
	  return(0);
	}
      }
      if (layout->array_definition[i].type==SDDS_STRING) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	  if (!SDDS_GZipWriteNonNativeBinaryString(((char**)SDDS_dataset->array[i].data)[j], gzfp, fBuffer)) {
	    SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteNonNativeBinaryArrays)");
	    SDDS_SwapEndsArrayData(SDDS_dataset);
	    return(0);
	  }
	}
      }
      else if (!SDDS_GZipBufferedWrite(SDDS_dataset->array[i].data, 
				   SDDS_type_size[layout->array_definition[i].type-1]* 
				   SDDS_dataset->array[i].elements, 
				   gzfp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_WriteNonNativeBinaryArrays)");
	SDDS_SwapEndsArrayData(SDDS_dataset);
	return(0);
      }
    }
  } else {
#endif
    fp =  layout->fp;
    for (i=0; i<layout->n_arrays; i++) {
      if (!SDDS_dataset->array[i].dimension) {
        for (j=0; j<layout->array_definition[i].dimensions; j++)
          if (!SDDS_BufferedWrite(&zero, sizeof(zero), fp, fBuffer)) {
            SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_WriteNonNativeBinaryArrays)");
            SDDS_SwapEndsArrayData(SDDS_dataset);
            return 0;
          }
        continue;
      }

      for (j=0; j<layout->array_definition[i].dimensions; j++) {
	dimension = SDDS_dataset->array[i].dimension[j];
	SDDS_SwapLong(&dimension);
	if (!SDDS_BufferedWrite(&dimension, sizeof(dimension),
				fp, fBuffer)) {
	  SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_WriteNonNativeBinaryArrays)");
	  SDDS_SwapEndsArrayData(SDDS_dataset);
	  return(0);
	}
      }
      if (layout->array_definition[i].type==SDDS_STRING) {
	for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	  if (!SDDS_WriteNonNativeBinaryString(((char**)SDDS_dataset->array[i].data)[j], fp, fBuffer)) {
	    SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteNonNativeBinaryArrays)");
	    SDDS_SwapEndsArrayData(SDDS_dataset);
	    return(0);
	  }
	}
      }
      else if (!SDDS_BufferedWrite(SDDS_dataset->array[i].data, 
				   SDDS_type_size[layout->array_definition[i].type-1]* 
				   SDDS_dataset->array[i].elements, 
				   fp, fBuffer)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_WriteNonNativeBinaryArrays)");
	SDDS_SwapEndsArrayData(SDDS_dataset);
	return(0);
      }
    }
#if defined(zLib)
  }
#endif
  SDDS_SwapEndsArrayData(SDDS_dataset);
  return(1);
}

int32_t SDDS_WriteNonNativeBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout;
  FILE *fp;
  SDDS_FILEBUFFER *fBuffer;
#if defined(zLib)
  gzFile *gzfp;
#endif
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteNonNativeBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    gzfp = layout->gzfp;
    for (i=0; i<layout->n_columns; i++) {
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!SDDS_GZipWriteNonNativeBinaryString(*((char**)SDDS_dataset->data[i]+row), gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write rows--failure writing string (SDDS_WriteNonNativeBinaryRows)");
	  return(0);
	}
      }
      else {
	size = SDDS_type_size[type-1];
	if (!SDDS_GZipBufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, gzfp, fBuffer)) {
	  SDDS_SetError("Unable to write row--failure writing value (SDDS_WriteNonNativeBinaryRow)");
	  return(0);
	}
      }
    }
  } else {
#endif
    fp = layout->fp;
    for (i=0; i<layout->n_columns; i++) {
      if ((type=layout->column_definition[i].type)==SDDS_STRING) {
	if (!SDDS_WriteNonNativeBinaryString(*((char**)SDDS_dataset->data[i]+row), fp, fBuffer)) {
	  SDDS_SetError("Unable to write rows--failure writing string (SDDS_WriteNonNativeBinaryRows)");
	  return(0);
	}
      }
      else {
	size = SDDS_type_size[type-1];
	if (!SDDS_BufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, fp, fBuffer)) {
	  SDDS_SetError("Unable to write row--failure writing value (SDDS_WriteNonNativeBinaryRow)");
	  return(0);
	}
      }
    }
#if defined(zLib)
  }
#endif
  return(1);
}

int32_t SDDS_WriteNonNativeBinaryString(char *string, FILE *fp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t length;
  static char *dummy_string = "";
  if (!string)
    string = dummy_string;
  length = strlen(string);
  SDDS_SwapLong(&length);
  if (!SDDS_BufferedWrite(&length, sizeof(length), fp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing length");
    return(0);
  }
  SDDS_SwapLong(&length);
  if (length && !SDDS_BufferedWrite(string, sizeof(*string)*length, fp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing contents");
    return(0);
  }
  return(1);
}

#if defined(zLib)
int32_t SDDS_GZipWriteNonNativeBinaryString(char *string, gzFile *gzfp, SDDS_FILEBUFFER *fBuffer)
{
  int32_t length;
  static char *dummy_string = "";
  if (!string)
    string = dummy_string;
  length = strlen(string);
  SDDS_SwapLong(&length);
  if (!SDDS_GZipBufferedWrite(&length, sizeof(length), gzfp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing length");
    return(0);
  }
  SDDS_SwapLong(&length);
  if (length && !SDDS_GZipBufferedWrite(string, sizeof(*string)*length, gzfp, fBuffer)) {
    SDDS_SetError("Unable to write string--error writing contents");
    return(0);
  }
  return(1);
}
#endif

int32_t SDDS_UpdateNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset, uint32_t mode)
{
  FILE *fp;
  int32_t i, rows, offset, code, fixed_rows;
  SDDS_FILEBUFFER *fBuffer;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_UpdateNonNativeBinaryPage"))
    return(0);
#if defined(zLib)
  if (SDDS_dataset->layout.gzipFile) {
    SDDS_SetError("Unable to perform page updates on a gzip file (SDDS_UpdateNonNativeBinaryPage)");
    return 0;
  }
#endif
  if (!SDDS_dataset->writing_page) {
#ifdef DEBUG
    fprintf(stderr, "Page not being written---calling SDDS_UpdateNonNativeBinaryPage\n");
#endif
    if (!(code = SDDS_WriteNonNativeBinaryPage(SDDS_dataset))) {
      return 0;
    }
    if (mode&FLUSH_TABLE) {
      SDDS_FreeTableStrings(SDDS_dataset);
      SDDS_dataset->first_row_in_mem = SDDS_CountRowsOfInterest(SDDS_dataset);
      SDDS_dataset->last_row_written = -1;
      SDDS_dataset->n_rows = 0;
    }
    return code;
  }

  if (!(fp=SDDS_dataset->layout.fp)) {
    SDDS_SetError("Unable to update page--file pointer is NULL (SDDS_UpdateNonNativeBinaryPage)");
    return(0);
  }
  fBuffer= &SDDS_dataset->fBuffer;
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateNonNativeBinaryPage)");
    return 0;
  }
  offset = ftell(fp);
  
  rows = SDDS_CountRowsOfInterest(SDDS_dataset)+SDDS_dataset->first_row_in_mem;
#ifdef DEBUG
  fprintf(stderr, "%" PRId32 " rows stored in table, %" PRId32 " already written\n",
	  rows, SDDS_dataset->n_rows_written);
#endif
  if (rows==SDDS_dataset->n_rows_written) {
    return(1);
  }
  if (rows<SDDS_dataset->n_rows_written) {
    SDDS_SetError("Unable to update page--new number of rows less than previous number (SDDS_UpdateNonNativeBinaryPage)");
    return(0);
  }
  if ((!SDDS_dataset->layout.data_mode.fixed_row_count) ||
      ((SDDS_dataset->n_rows_written / 500) != (rows / 500))) {
    if (SDDS_fseek(fp, SDDS_dataset->rowcount_offset, 0)==-1) {
      SDDS_SetError("Unable to update page--failure doing fseek (SDDS_UpdateNonNativeBinaryPage)");
      return(0);
    }
    if (SDDS_dataset->layout.data_mode.fixed_row_count) {
      fixed_rows = ((rows / 500) + 1) * 500;
#if defined(DEBUG)
      fprintf(stderr, "Setting %" PRId32 " fixed rows\n", fixed_rows);
#endif
      SDDS_SwapLong(&fixed_rows);
      if (fwrite(&fixed_rows, sizeof(fixed_rows), 1, fp)!=1) {
	SDDS_SetError("Unable to update page--failure writing number of rows (SDDS_UpdateNonNativeBinaryPage)");
	return(0);
      }
      SDDS_SwapLong(&fixed_rows);
    } else {
#if defined(DEBUG)
      fprintf(stderr, "Setting %" PRId32 " rows\n", rows);
#endif
      SDDS_SwapLong(&rows);
      if (fwrite(&rows, sizeof(rows), 1, fp)!=1) {
	SDDS_SetError("Unable to update page--failure writing number of rows (SDDS_UpdateNonNativeBinaryPage)");
	return(0);
      }
      SDDS_SwapLong(&rows);
    }
    if (SDDS_fseek(fp, offset, 0)==-1) {
      SDDS_SetError("Unable to update page--failure doing fseek to end of page (SDDS_UpdateNonNativeBinaryPage)");
      return(0);
    }
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
  for (i=SDDS_dataset->last_row_written+1; i<SDDS_dataset->n_rows; i++) {
    if (SDDS_dataset->row_flag[i] && !SDDS_WriteNonNativeBinaryRow(SDDS_dataset, i)) {
      SDDS_SetError("Unable to update page--failure writing row (SDDS_UpdateNonNativeBinaryPage)");
      return(0);
    }
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
#ifdef DEBUG
  fprintf(stderr, "Flushing buffer\n");
#endif
  if (!SDDS_FlushBuffer(fp, fBuffer)) {
    SDDS_SetError("Unable to write page--buffer flushing problem (SDDS_UpdateNonNativeBinaryPage)");
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
