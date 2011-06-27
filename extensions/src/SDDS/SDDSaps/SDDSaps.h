/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDSaps.h
 * contents: header file for routines used by SDDS command-line applications
 * 
 * M. Borland, 1993.
   $Log: SDDSaps.h,v $
   Revision 1.38  2010/12/09 16:13:26  borland
   The ifnot and ifis options now respect the -nowarnings request.

   Revision 1.37  2010/12/02 22:51:28  borland
   Added signedlargest and signedsmallest processing modes.

   Revision 1.36  2009/07/10 21:03:21  shang
   modified the filter option to accept string as parameter name for defining lower and upper limit of the filter

   Revision 1.35  2008/01/15 22:19:17  shang
   Added -process mode for percentile range computation (prange).

   Revision 1.34  2005/11/15 20:53:58  soliday
   Updated some routines used by sddsprocess that were causing trouble when
   compiled with a 64bit compiler.

   Revision 1.33  2005/11/04 22:46:10  soliday
   Updated code to be compiled by a 64 bit processor.

   Revision 1.32  2003/10/28 19:44:45  borland
   Added edit qualifier to scan option.  Allows pre-editing a string before
   scanning it, which may save creation of an intermediate parameter or
   column.

   Revision 1.31  2003/09/23 22:02:56  soliday
   Added the exclude option in the expandDefinitions function.

   Revision 1.30  2003/07/22 15:21:33  soliday
   Added the product process.

   Revision 1.29  2003/06/02 21:39:12  soliday
   Added the function to add parentheses around a string before calling if2pf

   Revision 1.28  2003/04/04 00:49:19  shang
   added structure for define timefileter and related functions

   Revision 1.27  2003/03/21 23:51:51  borland
   Added "integral" processing mode.

   Revision 1.26  2002/08/14 17:12:34  soliday
   Added Open License

   Revision 1.25  2002/04/05 21:55:51  shang
   remove definitions related to pick function

   Revision 1.24  2002/03/25 21:00:38  shang
   added FCLIP structure and modified PROCESSING_DEFINITION structure,
   and defined constants for fclip option, pick process mode, match column
   and match value

   Revision 1.23  2000/10/07 01:20:38  borland
   Added 'mode' processing mode. (The mode is the value that occurs most often.)

   Revision 1.22  2000/04/07 15:04:42  soliday
   Added a lot of missing prototypes.

   Revision 1.21  1999/07/09 15:06:03  soliday
   Added definitions for isnan, sleep, popen, and pclose for WIN32

   Revision 1.20  1999/06/03 17:04:07  soliday
   Commented out definition_type_name because it is not used.

   Revision 1.19  1999/05/25 19:01:14  soliday
   Removed compiler warning on linux.

   Revision 1.18  1998/12/16 21:22:45  borland
   Renamed SDDS_RedefineParameter and SDDS_RedefineColumn by appending
   "CL" to the names.

   Revision 1.17  1998/06/24 19:23:48  borland
   The -process option now allows specifying a string column with the
   functionOf qualifier for some types of processing.  This is allowed
   if the position qualifier is also given, resulting in the ability to
   pick out string data based on processing of numeric data.

   Revision 1.16  1998/06/03 17:33:01  borland
   Added support for templates in -print option.  Printing to columns is
   now faster and also implements printing of information from column
   metadata (e.g., name, symbol, units).

   Revision 1.15  1998/02/18 23:44:25  borland
   Added -reprint option.

   Revision 1.14  1996/10/30 21:57:22  borland
   Added -process mode for percentile computation.

 * Revision 1.13  1996/07/02  01:38:09  borland
 * Added -cast option to convert between numerical types.
 *
 * Revision 1.12  1996/05/23  21:19:14  borland
 * Added -format option and supporting routines.
 *
 * Revision 1.11  1996/02/15  16:16:16  borland
 * Added -reedit option.
 *
 * Revision 1.10  1996/02/12  17:24:53  borland
 * Added prototype for expandDefinitions().  Added select and editSelection
 * fields to EQUATION_DEFINITION structure.
 *
 * Revision 1.9  1996/01/17  16:44:49  borland
 * Added support for wildcard strings in units conversion (-convertUnits option).
 *
 * Revision 1.8  1996/01/12  19:02:16  borland
 * Added -numberTest option to sddsprocess, with supporting code in SDDSaps.*.
 *
 * Revision 1.7  1995/12/10  02:34:34  borland
 * Added quartile and decile range -process modes.
 *
 * Revision 1.6  1995/10/07  22:43:02  borland
 * Changed code to use topLimit and bottomLimit consistently rather than
 * Threshold; redo of earlier change that was somehow lost.
 *
 * Revision 1.5  1995/10/07  18:40:44  borland
 * Added argument to process_column() to carry -nowarnings option from
 * sddsprocess commandline.
 *
 * Revision 1.4  1995/10/06  18:02:45  borland
 * Added topLimit and bottomLimit qualifiers for sddsprocess -process option;
 * added code to support this in SDDSaps.
 *
 * Revision 1.3  1995/09/12  03:07:51  borland
 * Changed logic elements of MATCH_TERM and FILTER_TERM to unsigned long.
 *
 * Revision 1.2  1995/09/05  21:05:48  saunders
 * First test release of the SDDS1.5 package.
 *
 */

#define COLUMN_BASED 0
#define PARAMETER_BASED 1
#define ARRAY_BASED 2
#define DATA_CLASS_KEYWORDS 3

#if defined(_WIN32)
#define isnan(x) _isnan(x)
#define sleep(sec) Sleep(sec * 1000)
#define popen(x,y) _popen(x,y)
#define pclose(x) _pclose(x)
#endif

extern char *data_class_keyword[DATA_CLASS_KEYWORDS];

typedef struct {
    char **name;
    long *type, items;
    } IFITEM_LIST ;


typedef struct {
    char *name, *format;
    } LABEL_PARAMETER;

typedef struct {
    char *text, *name, *equation, *udf_name, *select, *editSelection, *exclude;
    long is_parameter;
    long redefinition;
    char **argv;
    long argc;
    } EQUATION_DEFINITION;
#define IS_EQUATION_DEFINITION 0

typedef struct {
    char *text, *sscanf_string, *source, *new_name, *edit;
    long is_parameter;
    } SCAN_DEFINITION;
#define IS_SCAN_DEFINITION 1

typedef struct {
    char *text, *edit_command, *source, *new_name;
    long is_parameter, reedit;
    char **argv;
    long argc;
    } EDIT_DEFINITION;
#define IS_EDIT_DEFINITION 2

typedef struct {
    char *text, *printf_string, *new_name, **source;
    long sources, is_parameter, reprint;
    char *select, *editSelection, *exclude;
    } PRINT_DEFINITION;
#define IS_PRINT_DEFINITION 3

typedef struct {
    char *parameter_name, *column_name, *description, *symbol; 
    char *functionOf, *weightBy, *match_value, *match_column;
    double lowerLimit, upperLimit, offset, factor, fhead, ftail, topLimit, bottomLimit;
    double percentileLevel, binSize;
    int32_t head, tail;
    long type, outputType;
    long mode, memory_number;
    unsigned long flags;
    } PROCESSING_DEFINITION;
#define IS_PROCESSING_DEFINITION 4
#define PROCESSING_LOLIM_GIVEN   0x00001
#define PROCESSING_UPLIM_GIVEN   0x00002
#define PROCESSING_INVERT        0x00004
#define PROCESSING_DESCRIP_GIVEN 0x00008
#define PROCESSING_FUNCOF_GIVEN  0x00010
#define PROCESSING_TAIL_GIVEN    0x00020
#define PROCESSING_HEAD_GIVEN    0x00040
#define PROCESSING_SYMBOL_GIVEN  0x00080
#define PROCESSING_WEIGHT_GIVEN  0x00100
#define PROCESSING_POSITION_GIVEN 0x00200
#define PROCESSING_OFFSET_GIVEN   0x00400
#define PROCESSING_FACTOR_GIVEN   0x00800
#define PROCESSING_FTAIL_GIVEN    0x01000
#define PROCESSING_FHEAD_GIVEN    0x02000
#define PROCESSING_TOPLIM_GIVEN    0x04000
#define PROCESSING_BOTLIM_GIVEN    0x08000
#define PROCESSING_PERCLEVEL_GIVEN 0x10000
#define PROCESSING_BINSIZE_GIVEN   0x20000
#define PROCESSING_MATCHCOLUMN_GIVEN 0x40000
#define PROCESSING_MATCHVALUE_GIVEN 0x80000

typedef struct {
    char *name, *new_units, *old_units;
    double factor;
    long is_parameter;
    } CONVERSION_DEFINITION;
#define IS_CONVERSION_DEFINITION 5

typedef struct {
    char *name, *string;
    unsigned long logic;
    } MATCH_TERM;

typedef struct {
  char *name, *upperPar, *lowerPar;
    double lower, upper;
    unsigned long logic;
    } FILTER_TERM;

typedef struct {
    FILTER_TERM *filter_term;
    long filter_terms, is_parameter;
    } FILTER_DEFINITION;
#define IS_FILTER_DEFINITION 6

typedef struct {
    MATCH_TERM *match_term;
    long match_terms, is_parameter;
    } MATCH_DEFINITION;
#define IS_MATCH_DEFINITION 7

typedef struct {
    char *expression;
    long autostop, is_parameter;
    } RPNTEST_DEFINITION;
#define IS_RPNTEST_DEFINITION 8

typedef struct {
    char *text, *source, *new_name;
    long is_parameter;
    } SYSTEM_DEFINITION;
#define IS_SYSTEM_DEFINITION 9

typedef struct {
    char *expression;
    long repeat;
    } RPNEXPRESSION_DEFINITION ;
#define IS_RPNEXPRESSION_DEFINITION 10

typedef struct {
    long head, tail;
    short invert;
    } CLIP_DEFINITION;
#define IS_CLIP_DEFINITION 11

typedef struct {
    long interval, offset;
    } SPARSE_DEFINITION;
#define IS_SPARSE_DEFINITION 12

typedef struct {
    double fraction;
    } SAMPLE_DEFINITION;
#define IS_SAMPLE_DEFINITION 13

typedef struct {
    char *name;
    short is_parameter;
    unsigned long flags;
    } NUMBERTEST_DEFINITION;
#define IS_NUMBERTEST_DEFINITION 14

typedef struct {
  char *target, *source, *stringFormat, *doubleFormat, *longFormat;
  long is_parameter;
} FORMAT_DEFINITION;
#define IS_FORMAT_DEFINITION 15

typedef struct {
  char *source, *newName, *newTypeName;
  long isParameter;
  int32_t newType;
} CAST_DEFINITION;
#define IS_CAST_DEFINITION 16

typedef struct {
  long head, tail;
  double fhead, ftail;
  short invert;
} FCLIP_DEFINITION;
#define IS_FCLIP_DEFINITION 17

typedef struct {
  char *name;
  double before, after;
  unsigned long flags;
  long is_parameter;
} TIME_FILTER_DEFINITION;
#define TIMEFILTER_BEFORE_GIVEN   0x00001
#define TIMEFILTER_AFTER_GIVEN   0x00002
#define TIMEFILTER_INVERT_GIVEN  0x00004

#define IS_TIME_FILTER_DEFINITION 18

#define DEFINITION_TYPES 19

/*
static char *definition_type_name[DEFINITION_TYPES] = {
  "define", "scan", "edit", "print", "process", "convert", "filter", "match", "test", 
  "system", "rpn_expression", "clip", "sparse", "sample", "numbertest", "format",
  "cast",
  } ;
*/

typedef struct {
    long type;
    void *structure;
    } DEFINITION;

typedef struct {
    FILE *fp;
    char *item[4];
    long columns;
    long points;
    long parameter_output;
    void **definitions;
    long counter;
    } OUTPUT_REQUEST;


#define PROCESS_COLUMN_MEAN 0
#define PROCESS_COLUMN_RMS  1
#define PROCESS_COLUMN_SUM  2
#define PROCESS_COLUMN_STAND_DEV  3
#define PROCESS_COLUMN_MAD  4
#define PROCESS_COLUMN_MINIMUM  5
#define PROCESS_COLUMN_MAXIMUM  6
#define PROCESS_COLUMN_SMALLEST  7
#define PROCESS_COLUMN_LARGEST  8
#define PROCESS_COLUMN_FIRST  9
#define PROCESS_COLUMN_LAST  10
#define PROCESS_COLUMN_COUNT  11
#define PROCESS_COLUMN_SPREAD  12
#define PROCESS_COLUMN_MEDIAN  13
#define PROCESS_COLUMN_BASELEVEL  14
#define PROCESS_COLUMN_TOPLEVEL  15
#define PROCESS_COLUMN_AMPLITUDE  16
#define PROCESS_COLUMN_RISETIME 17
#define PROCESS_COLUMN_FALLTIME 18
#define PROCESS_COLUMN_FWHM 19
#define PROCESS_COLUMN_FWTM 20
#define PROCESS_COLUMN_CENTER 21
#define PROCESS_COLUMN_ZEROCROSSING 22
#define PROCESS_COLUMN_FWHA 23
#define PROCESS_COLUMN_FWTA 24
#define PROCESS_COLUMN_SIGMA  25
#define PROCESS_COLUMN_SLOPE  26
#define PROCESS_COLUMN_INTERCEPT  27
#define PROCESS_COLUMN_LFSD  28
#define PROCESS_COLUMN_QRANGE 29
#define PROCESS_COLUMN_DRANGE 30
#define PROCESS_COLUMN_PERCENTILE 31
#define PROCESS_COLUMN_MODE 32
#define PROCESS_COLUMN_INTEGRAL 33
#define PROCESS_COLUMN_PRODUCT  34
#define PROCESS_COLUMN_PRANGE 35
#define PROCESS_COLUMN_SIGNEDSMALLEST 36
#define PROCESS_COLUMN_SIGNEDLARGEST  37
#define N_PROCESS_COLUMN_MODES 38
#if 0
extern char *process_column_mode[N_PROCESS_COLUMN_MODES];
extern char *process_column_name[N_PROCESS_COLUMN_MODES];
extern char *process_column_description[N_PROCESS_COLUMN_MODES];
#endif

extern char *addOuterParentheses(char *arg) ;
extern void show_process_modes(FILE *fp);
extern long process_column(SDDS_DATASET *Table, PROCESSING_DEFINITION *processing_ptr, double *result, 
                           char **stringResult, long warnings);
extern char *process_string_column(SDDS_DATASET *Dataset, PROCESSING_DEFINITION *processing_ptr,long warnings);
extern long process_filter_request(FILTER_TERM **filter, char **argument, long arguments);
extern long process_match_request(MATCH_TERM **match, char **argument, long arguments);
extern void scan_label_parameter(LABEL_PARAMETER *label, char *string);
extern void show_matches(char *type, MATCH_TERM *match, long matches);
extern void show_filters(char *type, FILTER_TERM *filter, long filters);
extern EQUATION_DEFINITION *process_new_equation_definition(char **argument, long arguments);
extern SCAN_DEFINITION *process_new_scan_definition(char **argument, long arguments);
extern CAST_DEFINITION *process_new_cast_definition(char **argument, long arguments);
extern EDIT_DEFINITION *process_new_edit_definition(char **argument, long arguments, short reedit);
extern PRINT_DEFINITION *process_new_print_definition(char **argument, long arguments);
extern FORMAT_DEFINITION *process_new_format_definition(char **argument, long arguments);
extern PROCESSING_DEFINITION *record_processing_definition(char **argument, long arguments);
extern PROCESSING_DEFINITION *copyProcessingDefinition(PROCESSING_DEFINITION *source);
extern void expandProcessingDefinitions(DEFINITION **definition, long *definitions, SDDS_DATASET *SDDS_dataset);
extern CONVERSION_DEFINITION *copyConversionDefinition(CONVERSION_DEFINITION *source);
extern void expandConversionDefinitions(DEFINITION **definition, long *definitions, SDDS_DATASET *SDDS_dataset);
extern void expandDefinitions(DEFINITION **definition, long *definitions, SDDS_DATASET *SDDS_dataset);

extern CONVERSION_DEFINITION *process_conversion_definition(char **argument, long arguments);
extern FILTER_DEFINITION *process_new_filter_definition(char **argument, long arguments);
extern TIME_FILTER_DEFINITION *process_new_time_filter_definition(char **argument, long arguments);
extern MATCH_DEFINITION *process_new_match_definition(char **argument, long arguments);
extern RPNTEST_DEFINITION *process_new_rpntest_definition(char **argument, long arguments);
extern NUMBERTEST_DEFINITION *process_new_numbertest_definition(char **argument, long arguments);
extern RPNEXPRESSION_DEFINITION *process_new_rpnexpression_definition(char **argument, long arguments);
extern CLIP_DEFINITION *process_new_clip_definition(char **argument, long arguments);
extern FCLIP_DEFINITION *process_new_fclip_definition(char **argument, long arguments);
extern SPARSE_DEFINITION *process_new_sparse_definition(char **argument, long arguments);
extern SAMPLE_DEFINITION *process_new_sample_definition(char **argument, long arguments);
extern SYSTEM_DEFINITION *process_new_system_definition(char **argument, long arguments);
extern OUTPUT_REQUEST *process_output_request(char **argument, long arguments, OUTPUT_REQUEST *last_request);
extern char *determine_item_name(char **argument, OUTPUT_REQUEST *last_request, long index);
extern void set_up_output(char *filename, OUTPUT_REQUEST *output, LABEL_PARAMETER *label_parameter, long label_parameters, 
                   long separate_tables, long announce_openings, SDDS_DATASET *SDDS_dataset);
extern long complete_processing_definitions(PROCESSING_DEFINITION **processing_definition, long processing_definitions, 
                                     SDDS_DATASET *SDDS_dataset);

extern long system_column_value(SDDS_DATASET *SDDS_dataset, char *target, char *source);
extern long system_parameter_value(SDDS_DATASET *SDDS_dataset, char *target, char *source);
extern long run_on_pipe(char *command, char *buffer, long buffer_length);

/* used for redefining parameters and columns using sddsprocess-style commandline arguments */
extern long SDDS_RedefineParameterCL(SDDS_DATASET *SDDS_dataset, char *parameter, char **argv, long argc);
extern long SDDS_RedefineColumnCL(SDDS_DATASET *SDDS_dataset, char *column, char **argv, long argc);

extern long edit_string(char *text, char *edit);

extern long reformatString(char *buffer, long bufferSize, char *string, char *stringFormat,
                    char *doubleFormat, char *longFormat);

extern long cast_column_value(SDDS_DATASET *SDDS_dataset, CAST_DEFINITION *cast);
extern long cast_parameter_value(SDDS_DATASET *SDDS_dataset, CAST_DEFINITION *cast);

void add_definition(DEFINITION **definition, long *definitions, void *structure, long type);
long check_ifitems(SDDS_DATASET *SDDS_dataset, IFITEM_LIST *ifitem, long desired, long announce);
long complete_cast_definition(SDDS_DATASET *SDDSout, CAST_DEFINITION *defi,
                              SDDS_DATASET *SDDSin);
long edit_parameter_value(SDDS_DATASET *SDDS_dataset, char *target, char *source, char *edit_command);
long edit_column_value(SDDS_DATASET *SDDS_dataset, char *target, char *source, char *edit_command);
long scan_parameter_value(SDDS_DATASET *SDDS_dataset, char *target, char *source, char *format,
                          char *edit);
long scan_column_value(SDDS_DATASET *SDDS_dataset, char *target, char *source, char *format,
                       char *edit);
long print_parameter_value(SDDS_DATASET *SDDS_dataset, char *target, char **source, long sources, char *format);
long print_column_value(SDDS_DATASET *SDDS_dataset, char *target, char **source, long sources, char *format);
long format_parameter_value(SDDS_DATASET *SDDS_dataset, FORMAT_DEFINITION *definition);
long format_column_value(SDDS_DATASET *SDDS_dataset, FORMAT_DEFINITION *definition);
long ParameterScansAsNumber(SDDS_DATASET *dataset, char *name, short invert);


/* rpn function for getting logical values */
extern long pop_log(int32_t *logical);

#define is_logic_character(c) ((c)=='|' || (c)=='&' || (c)=='!')

void add_ifitem(IFITEM_LIST *ifitem, char **name, long names);

#include "scan.h"
long add_sddsfile_arguments(SCANNED_ARG **scanned, int argc);

