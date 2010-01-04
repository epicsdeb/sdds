#if !defined(_SDDS_)
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus 
extern "C" {
#endif


#define _SDDS_ 1

#define SDDS_VERSION 1

#include "SDDStypes.h"

#define RW_ASSSOCIATES 0

#define SDDS_BINARY 1
#define SDDS_ASCII  2
#define SDDS_NUM_DATA_MODES 2

extern char *SDDS_type_name[SDDS_NUM_TYPES];
extern long SDDS_type_size[SDDS_NUM_TYPES];
extern char *SDDS_data_mode[SDDS_NUM_DATA_MODES];

/* this shouldn't be changed without changing buffer sizes in namelist routines: */
#define SDDS_MAXLINE 1024

#define SDDS_NORMAL_DEFINITION 0
#define SDDS_WRITEONLY_DEFINITION 1

#define TERMINATE_DONT_FREE_TABLE_STRINGS 0x0001
#define TERMINATE_DONT_FREE_ARRAY_STRINGS 0x0002

#define SDDS_ALLOW_ANY_NAME 0x0001UL
#define SDDS_ALLOW_V15_NAME 0x0002UL

#define SDDS_CopyTable(a, b) SDDS_CopyPage(a, b)

extern long SDDS_DeferSavingLayout(long mode);

#define SDDS_BY_INDEX 1
#define SDDS_BY_NAME  2

#define SDDS_StartTable(a, b) SDDS_StartPage(a, b)

#define SDDS_SET_BY_INDEX SDDS_BY_INDEX
#define SDDS_SET_BY_NAME SDDS_BY_NAME
#define SDDS_PASS_BY_VALUE 4
#define SDDS_PASS_BY_REFERENCE 8
#define SDDS_PASS_BY_STRING 16
#define SDDS_WriteTable(a) SDDS_WritePage(a)
#define FLUSH_TABLE 0x1UL
#define SDDS_UpdateTable(a) SDDS_UpdatePage(a, 0)
#define SDDS_GET_BY_INDEX SDDS_BY_INDEX
#define SDDS_GET_BY_NAME SDDS_BY_NAME

#define SDDS_ReadTable(a) SDDS_ReadPage(a)

/* logic flags for SDDS_AssertRowFlags and SDDS_AssertColumnFlags */
#define SDDS_FLAG_ARRAY  0x001UL
#define SDDS_INDEX_LIMITS 0x002UL

/* modes for SDDS_SetColumnsOfInterest and SDDS_SetRowsOfInterest: */
#define SDDS_NAME_ARRAY 1
#define SDDS_NAMES_STRING 2
#define SDDS_NAME_STRINGS 3
#define SDDS_MATCH_STRING 4
#define SDDS_CI_NAME_ARRAY 5
#define SDDS_CI_NAMES_STRING 6
#define SDDS_CI_NAME_STRINGS 7
#define SDDS_CI_MATCH_STRING 8

/* logic flags for SDDS_SetColumnsOfInterest, SDDS_SetRowsOfInterest, and SDDS_MatchRowsOfInterest: */
#define SDDS_AND               0x0001UL
#define SDDS_OR                0x0002UL
#define SDDS_NEGATE_MATCH      0x0004UL
#define SDDS_NEGATE_PREVIOUS   0x0008UL
#define SDDS_NEGATE_EXPRESSION 0x0010UL
#define SDDS_INDIRECT_MATCH    0x0020UL
#define SDDS_1_PREVIOUS        0x0040UL
#define SDDS_0_PREVIOUS        0x0080UL
/* used by MatchRowsOfInterest only at this point: */
#define SDDS_NOCASE_COMPARE    0x0100UL

#define NUMSCANFILTER_INVERT 0x0001UL
#define NUMSCANFILTER_STRICT 0x0002UL

#define SDDS_ROW_MAJOR_DATA 1
#define SDDS_COLUMN_MAJOR_DATA 2
#define SDDS_POINTER_ARRAY 1
#define SDDS_CONTIGUOUS_DATA 2

#define SDDS_VERBOSE_PrintErrors 1
#define SDDS_EXIT_PrintErrors 2
#define SDDS_LAST_GetErrorMessages 0
#define SDDS_ALL_GetErrorMessages 1

#define FIND_ANY_TYPE       0
#define FIND_SPECIFIED_TYPE 1
#define FIND_NUMERIC_TYPE   2
#define FIND_INTEGER_TYPE   3
#define FIND_FLOATING_TYPE  4

#define SDDS_CHECK_OKAY 0
#define SDDS_CHECK_OK SDDS_CHECK_OKAY
#define SDDS_CHECK_NONEXISTENT 1
#define SDDS_CHECK_WRONGTYPE  2
#define SDDS_CHECK_WRONGUNITS  3

#define SDDS_PRINT_NOQUOTES 0x0001UL
#define SDDS_CheckTableStructureSize(a) SDDS_CheckDatasetStructureSize(a)

#define TABULAR_DATA_CHECKS 0x0001UL

#ifdef __cplusplus
}
#endif

#endif

