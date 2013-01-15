/* file: SDDS_internal.h
 * contents: definitions for internal use by SDDS routines
 * 
 * Michael Borland, November 1993.
 $Log: not supported by cvs2svn $
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
    long offset;
    long type;
    } SDDS_FIELD_INFORMATION ;


#define TABLE_LENGTH_INCREMENT 100

#define SDDS_DESCRIPTION_COMMAND 0
#define SDDS_COLUMN_COMMAND 1
#define SDDS_PARAMETER_COMMAND 2
#define SDDS_ASSOCIATE_COMMAND 3
#define SDDS_DATA_COMMAND 4
#define SDDS_INCLUDE_COMMAND 5
#define SDDS_ARRAY_COMMAND 6
#define SDDS_NUM_COMMANDS 7

extern char *SDDS_command[SDDS_NUM_COMMANDS];

#endif
