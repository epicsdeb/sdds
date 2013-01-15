/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDS_internal.h
 * contents: definitions for internal use by SDDS routines
 * 
 * Michael Borland, November 1993.
 $Log: not supported by cvs2svn $
 Revision 1.20  2005/11/04 22:46:52  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.19  2005/08/30 19:11:45  soliday
 Fixed problem when writting output from programs such as sddslogger in
 a non-native endian mode.

 Revision 1.18  2004/02/05 22:58:38  soliday
 Moved SDDS_CheckTabularData to SDDS.h because it is used externaly now.

 Revision 1.17  2002/08/14 16:34:48  soliday
 Added Open License

 Revision 1.16  2002/02/04 19:57:06  borland
 SDDS_UpdateBinaryPage() and SDDS_UpdateAsciiPage() now free string memory
 from the columns if in FLUSH_TABLE mode.  This prevents a big memory leak.

 Revision 1.15  2001/11/30 15:34:38  borland
 Changes by H. Shang: Addition of SDDS_GotoPage() and changes required to
 support this.

 Revision 1.14  2000/11/19 19:02:28  borland
 Fixed bug in SDDS_FreeArray and added SDDS_FreePointerArray.

 Revision 1.13  2000/09/19 20:13:22  borland
 Added ability to read files with big and little endian byte order.  Took
 swapped-read routines from sddsendian.c.

 Revision 1.12  2000/06/20 18:04:03  borland
 Added SDDS_SetDefaultIOBufferSize and modified binary read/write
 routines to optionally allow unbuffered read/write.  Removed
 SDDS_SetBufferedRead routine.

 Revision 1.11  2000/06/20 14:29:20  soliday
 Moved some definitions to SDDS.h so that sddsendian can access them.

 Revision 1.10  2000/04/04 05:04:58  soliday
 Added SDDS_SetBufferedRead and SDDS_UpdateAsciiPage

 Revision 1.9  2000/01/18 20:14:05  soliday
 Added support for ZLIB.

 Revision 1.8  1999/11/03 22:44:22  soliday
 Moved WriteBinaryString to SDDS.h

 Revision 1.7  1999/04/14 14:05:05  borland
 Works with new build system.  Fixed some missing prototypes and function type mismatches.

 Revision 1.6  1998/07/07 22:29:42  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

 Revision 1.5  1996/05/02 00:48:27  borland
 Added buffering to binary output.

 * Revision 1.4  1996/03/21  16:38:38  borland
 * Major revision.
 * Added buffered binary reading (SDDS_binary.c).
 * Added sparsed table reading (SDDS_binary.c, SDDS_ascii.c).
 * Added sorted column, parameter, and array name lists (SDDS_info.c,
 * SDDS_output.c, SDDS_utils.c).
 *
 * Revision 1.3  1995/11/13  16:05:37  borland
 * Revised prototype for  SDDS_UpdateBinaryPage to include mode argument.
 *
 * Revision 1.2  1995/09/05  21:11:01  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "SDDS.h"

#if !defined(_SDDS_internal_)
#define _SDDS_internal_ 1


typedef struct {
  char *name;
  int32_t value;
} SDDS_ENUM_PAIR;

typedef struct {
    char *name;
    int32_t offset;
    int32_t type;
    SDDS_ENUM_PAIR *enumPair;
    } SDDS_FIELD_INFORMATION ;

#define SDDS_DESCRIPTION_FIELDS 2
#define SDDS_DATA_FIELDS 4
#define SDDS_INCLUDE_FIELDS 1

extern SDDS_FIELD_INFORMATION SDDS_ArrayFieldInformation[SDDS_ARRAY_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_ColumnFieldInformation[SDDS_COLUMN_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_ParameterFieldInformation[SDDS_PARAMETER_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_AssociateFieldInformation[SDDS_ASSOCIATE_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_DescriptionFieldInformation[SDDS_DESCRIPTION_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_IncludeFieldInformation[SDDS_INCLUDE_FIELDS];
extern SDDS_FIELD_INFORMATION SDDS_DataFieldInformation[SDDS_DATA_FIELDS];

#define TABLE_LENGTH_INCREMENT 100

/* binary input/output routines */
extern int32_t SDDS_WriteBinaryArrays(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_WriteBinaryParameters(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_WriteBinaryPage(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_WriteBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row);
extern int32_t SDDS_UpdateBinaryPage(SDDS_DATASET *SDDS_dataset, uint32_t mode);
extern int32_t SDDS_UpdateNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset, uint32_t mode);
extern int32_t SDDS_fseek(FILE *fp, int32_t offset, int32_t dir);
#if defined(zLib)
extern int32_t SDDS_gzseek(gzFile *gzfp, int32_t offset, int32_t dir);
#endif

extern char *SDDS_ReadBinaryString(FILE *fp, SDDS_FILEBUFFER *fBuffer, int32_t skip);
extern int32_t SDDS_ReadBinaryArrays(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_ReadBinaryParameters(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_ReadBinaryPage(SDDS_DATASET *SDDS_dataset, int32_t sparse_interval,
                                int32_t sparse_offset);
extern int32_t SDDS_ReadBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row, int32_t skip);

extern int32_t SDDS_AllocateColumnFlags(SDDS_DATASET *SDDS_target);

/* ascii input/output routines */
extern int32_t SDDS_WriteAsciiArrays(SDDS_DATASET *SDDS_dataset, FILE *fp);
extern int32_t SDDS_WriteAsciiParameters(SDDS_DATASET *SDDS_dataset, FILE *fp);
extern void SDDS_AppendParameterComment(PARAMETER_DEFINITION *definition, char *text, char *string);
extern int32_t SDDS_WriteAsciiRow(SDDS_DATASET *SDDS_dataset, int32_t row, FILE *fp);
extern int32_t SDDS_WriteAsciiPage(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_ReadAsciiArrays(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_ReadAsciiParameters(SDDS_DATASET *SDDS_dataset);
extern int32_t SDDS_AsciiDataExpected(SDDS_DATASET *SDDS_dataset);
#if defined(zLib)
extern int32_t SDDS_GZipWriteAsciiParameters(SDDS_DATASET *SDDS_dataset, gzFile *gzfp);
extern int32_t SDDS_GZipWriteAsciiArrays(SDDS_DATASET *SDDS_dataset, gzFile *gzfp);
extern int32_t SDDS_GZipWriteAsciiRow(SDDS_DATASET *SDDS_dataset, int32_t row, gzFile *gzfp);
#endif

/* routines from SDDS_extract.c : */
extern int32_t SDDS_CopyColumn(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source);
extern int32_t SDDS_CopyParameter(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source);
extern int32_t SDDS_TransferRow(SDDS_DATASET *SDDS_dataset, int32_t target, int32_t source);
extern int32_t SDDS_GetSelectedRowIndex(SDDS_DATASET *SDDS_dataset, int32_t srow_index);

/* routines from SDDS_utils.c : */
extern int32_t SDDS_CheckTable(SDDS_DATASET *SDDS_dataset, const char *caller);
extern int32_t SDDS_AdvanceCounter(int32_t *counter, int32_t *max_count, int32_t n_indices);
extern void SDDS_FreePointerArray(void **data, int32_t dimensions, int32_t *dimension);

/* routines from SDDS_output.c : */
extern int32_t SDDS_WriteVersion(int32_t version_number, FILE *fp);
extern int32_t SDDS_WriteDescription(char *description, char *contents, FILE *fp);
extern int32_t SDDS_WriteColumnDefinition(COLUMN_DEFINITION *column, FILE *fp);
extern int32_t SDDS_WriteParameterDefinition(PARAMETER_DEFINITION *parameter, FILE *fp);
extern int32_t SDDS_WriteAssociateDefinition(ASSOCIATE_DEFINITION *associate, FILE *fp);
extern int32_t SDDS_WriteArrayDefinition(ARRAY_DEFINITION *array_definition, FILE *fp);
extern int32_t SDDS_WriteDataMode(DATA_MODE *data_mode, FILE *fp);
#if defined(zLib)
extern int32_t SDDS_GZipWriteVersion(int32_t version_number, gzFile *gzfp);
extern int32_t SDDS_GZipWriteDescription(char *description, char *contents, gzFile *gzfp);
extern int32_t SDDS_GZipWriteColumnDefinition(COLUMN_DEFINITION *column, gzFile *gzfp);
extern int32_t SDDS_GZipWriteParameterDefinition(PARAMETER_DEFINITION *parameter, gzFile *gzfp);
extern int32_t SDDS_GZipWriteAssociateDefinition(ASSOCIATE_DEFINITION *associate, gzFile *gzfp);
extern int32_t SDDS_GZipWriteArrayDefinition(ARRAY_DEFINITION *array_definition, gzFile *gzfp);
extern int32_t SDDS_GZipWriteDataMode(DATA_MODE *data_mode, gzFile *gzfp);
#endif

/* -- routines for routing data processing to routines for appropriate version */
extern int32_t SDDS_ProcessDescription(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS_ProcessColumnDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS_ProcessParameterDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS_ProcessArrayDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern FILE  *SDDS_ProcessIncludeCommand(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS_ProcessAssociateDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS_ProcessDataMode(SDDS_DATASET *SDDS_dataset, char *s);

/* -- protocol version 1 routines */
extern int32_t SDDS1_ProcessDescription(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS1_ProcessColumnDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS1_ProcessParameterDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS1_ProcessArrayDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern FILE  *SDDS1_ProcessIncludeCommand(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS1_ProcessAssociateDefinition(SDDS_DATASET *SDDS_dataset, char *s);
extern int32_t SDDS1_ProcessDataMode(SDDS_DATASET *SDDS_dataset, char *s);

/* internal input routines */
extern int32_t SDDS_ReadLayout(SDDS_DATASET *SDDS_dataset, FILE *fp);
extern int32_t SDDS_UpdateAsciiPage(SDDS_DATASET *SDDS_dataset, uint32_t mode);
#if defined(zLib)
extern int32_t SDDS_GZipReadLayout(SDDS_DATASET *SDDS_dataset, gzFile *gzfp);
#endif

#define SDDS_DESCRIPTION_COMMAND 0
#define SDDS_COLUMN_COMMAND 1
#define SDDS_PARAMETER_COMMAND 2
#define SDDS_ASSOCIATE_COMMAND 3
#define SDDS_DATA_COMMAND 4
#define SDDS_INCLUDE_COMMAND 5
#define SDDS_ARRAY_COMMAND 6
#define SDDS_NUM_COMMANDS 7

extern char *SDDS_command[SDDS_NUM_COMMANDS];

uint32_t SDDS_GetSpecialCommentsModes();
void SDDS_ResetSpecialCommentsModes();
void SDDS_ParseSpecialComments(char *s);

void SDDS_FreeTableStrings(SDDS_DATASET *SDDS_dataset);

#endif
