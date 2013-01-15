/* file: SDDS.h
 * purpose: SDDS data types
 *          format
 *
 * Michael Borland, 1993
 $Log: not supported by cvs2svn $
 Revision 1.3  1995/09/06 14:12:01  saunders
 First test release of SDDS1.5

 */

#if !defined(_SDDSTYPES_)

#define _SDDSTYPES_ 1

#define SDDS_DOUBLE    1
#define SDDS_FLOAT     2
#define SDDS_LONG      3
#define SDDS_SHORT     4
#define SDDS_STRING    5
#define SDDS_CHARACTER 6
#define SDDS_NUM_TYPES 6
#define SDDS_INTEGER_TYPE(type) ((type)==SDDS_LONG || (type)==SDDS_SHORT)
#define SDDS_FLOATING_TYPE(type) ((type)==SDDS_DOUBLE || (type)==SDDS_FLOAT)
#define SDDS_NUMERIC_TYPE(type) (SDDS_INTEGER_TYPE(type) || SDDS_FLOATING_TYPE(type))
#define SDDS_VALID_TYPE(type) (type>=1 && type<=SDDS_NUM_TYPES)

/* used by SDDS_Check*() routines  */
#define SDDS_ANY_NUMERIC_TYPE (SDDS_NUM_TYPES+1)

#endif
