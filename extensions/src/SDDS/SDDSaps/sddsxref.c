/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsxref
 * purpose: creates a SDDS data set from two other data sets
 *          by adding data from one data set to the other
 *          based on matching or filtering of column data columns
 * This is a fast version of sddsmxref, with less capability.
 *
 * For example:  suppose file.1 contains the columns (name,x,y) and
 * file.2 contains (name,z,betax,betay).  Then
 *   sddsxref file.1 file.2 -take=z -match=name file.3 
 * would result in (name,x,y,z) being put in file.3.
 *
 * Michael Borland, 1994, 1995
 $Log: sddsxref.c,v $
 Revision 1.43  2009/11/30 19:07:47  soliday
 Updated to fix a an issue with keyGroups being uninitialized.

 Revision 1.42  2009/04/01 15:46:07  soliday
 Fixed a bug with three or more input files. Under certain conditions
 it was possible to get the wrong results due to CopyRowToNewColumn and
 SDDS_AssertRowFlags expecting the real row index and not the index
 of rows of interest.

 Revision 1.41  2005/11/07 21:48:11  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.40  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.39  2005/08/30 15:53:51  shang
 fixed the bug for renaming arrays

 Revision 1.38  2005/04/07 19:34:02  borland
 Added -wildMatch option.  Equivalent to -match, but uses wildcards in the
 data from the second file.

 Revision 1.37  2004/06/11 21:47:15  borland
 Fixed bug that resulted in having no rows in the output when the xref file
 had no rows, even when -fillIn was given.

 Revision 1.36  2003/09/02 19:16:08  soliday
 Cleaned up code for Linux.

 Revision 1.35  2002/10/21 22:44:48  shang
 moved the sorting and comparison functions to sortfunctions.c in mdblib area

 Revision 1.34  2002/08/14 17:12:57  soliday
 Added Open License

 Revision 1.33  2002/03/28 00:30:33  shang
 fixed bug for -rename=parameter option

 Revision 1.32  2001/10/22 23:06:54  borland
 Fixed problem with file replacement due to premature freeing of two strings.

 Revision 1.31  2001/10/17 21:29:35  shang
 added -rename and -editnames option, fixed the memory leak problem and the memory errors in
 the case of no-matches

 Revision 1.30  2001/05/08 18:54:25  soliday
 The fillIn option now works when -match and -equate are not used.

 Revision 1.29  2001/05/03 20:55:36  soliday
 Standardized usage messages.

 Revision 1.28  2001/01/10 19:35:50  soliday
 Standardized usage message.

 Revision 1.27  2000/10/31 14:15:32  borland
 Fixed the usage message, which was corrupted by an over-zealous indentation
 of the code.

 Revision 1.26  2000/09/27 17:10:39  emery
 Fix a bus error that occured when the first input file had zero rows.
 The fix is testing for the number of rows before attempting to free
 a pointer which has not been assigned a value.

 Revision 1.25  2000/07/20 21:00:38  emery
 Corrected bug where the parameters of the xref file
 were automatically transfer (fix by D. Blachowicz)
 and indented code.

 Revision 1.24  2000/06/01 15:45:58  borland
 Several bug fixes per D. Blachowicz.

 Revision 1.22  2000/04/13 18:04:40  soliday
 Removed invalid prototype.

 Revision 1.21  1999/07/09 15:08:54  soliday
 Seperated USAGE string into two strings to fix problem on WIN32

 Revision 1.20  1999/05/25 19:17:41  soliday
 Removed compiler warning on linux.

 Revision 1.19  1999/01/06 19:55:00  borland
 Fixed the version number in the usage message.

 Revision 1.18  1997/10/20 22:20:46  borland
 Fixed problem for xrefing when there is no match or equate option given.
 Wasn't properly dealing with reusing of rows or case when file2 had too
 few rows.

 Revision 1.17  1997/09/08 23:41:44  borland
 Fixed a logic bug in checking for existence of output columns.

 Revision 1.16  1997/03/19 19:20:57  borland
 Check that columns are being transferred before trying to get list of
 output column names.  Avoids incorrect program termination.

 * Revision 1.15  1996/06/03  22:45:35  borland
 * Added SDDS_Terminate calls to avoid stupid Solaris "Broken Pipe" messages.
 *
 * Revision 1.14  1996/04/07  00:42:53  borland
 * Fixed bug that resulted in -reuse=row not working.
 *
 * Revision 1.13  1996/04/05  22:55:03  borland
 * Now uses sorted index lists for better performance.
 *
 * Revision 1.12  1996/03/12  04:55:02  borland
 * Added initialization for leave_all_columns.
 *
 * Revision 1.11  1996/02/12  17:25:22  borland
 * Removed erroneous free'ing of take_column array.
 *
 * Revision 1.10  1996/01/29  22:30:28  borland
 * Added code to avoid attempts to transfer column and parameter definitions
 * when the element already exists.
 *
 * Revision 1.9  1996/01/17  16:05:34  borland
 * Added test for lack of columns in input2, so that parameters and arrays can
 * be transfered even if input2 has no column data.
 *
 * Revision 1.8  1996/01/15  22:35:55  borland
 * Now keeps tabular data in input1 if no columns are being taken from
 * input2, even if input2 has an empty tabular data section.
 *
 * Revision 1.7  1996/01/15  18:21:56  borland
 * Changed behavior when second file has fewer pages than first; now keeps the
 * parameter and array data while erasing tabular data; this is consistent
 * with the program behavior when there is tabular data in both files that
 * doesn't yield any matches.
 *
 * Revision 1.6  1996/01/15  04:09:11  borland
 * Modified behavior when <input2> has an empty page; now prints a warning
 * instead of exiting.  Will pass the <input1> page through instead of
 * emitting an empty page.
 *
 * Revision 1.5  1996/01/11  20:34:46  borland
 * Added -fillIn option and supporting code.
 *
 * Revision 1.4  1995/11/20  16:24:03  borland
 * Added code to prevent aborts when there are no rows in the input page.
 *
 * Revision 1.3  1995/09/06  14:57:24  saunders
 * First test release of SDDS1.5
 * 
 */
#include "mdb.h"
#include "SDDS.h"
#include "SDDSaps.h"
#include "scan.h"
#include <ctype.h>

#define SET_TAKE_COLUMNS 0
#define SET_LEAVE_COLUMNS 1
#define SET_MATCH_COLUMN 2
#define SET_EQUATE_COLUMN 3
#define SET_TRANSFER 4
#define SET_REUSE 5
#define SET_IFNOT 6
#define SET_NOWARNINGS 7
#define SET_IFIS 8
#define SET_PIPE 9
#define SET_FILLIN 10
#define SET_RENAME 11
#define SET_EDIT_NAMES 12
#define SET_WILD_MATCH 13
#define N_OPTIONS 14

#define COLUMN_MODE 0
#define PARAMETER_MODE 1
#define ARRAY_MODE 2
#define MODES 3
static char *mode_name[MODES] = {
    "column", "parameter", "array",
  } ;


#define PARAMETER_TRANSFER 0
#define ARRAY_TRANSFER 1
#define TRANSFER_TYPES 2
static char *transfer_type[TRANSFER_TYPES] = {
  "parameter", "array" 
  };
typedef struct {
  char *name;
  long type;
} TRANSFER_DEFINITION;

typedef struct {
  char **new_column;
  char **new_parameter;
  char **new_array;
  char **orig_column;
  char **orig_parameter;
  char **orig_array;
  int32_t columns;
  int32_t parameters;
  int32_t arrays;
} REFDATA;

/*structure for getting editnames */
typedef struct {
  char *match_string, *edit_string;
} EDIT_NAME_REQUEST;
  
long expandTransferRequests(char ***match, int32_t *matches, 
                            long type, TRANSFER_DEFINITION *transfer, long transfers,
                            SDDS_DATASET *inSet);

void add_newnames(SDDS_DATASET *SDDS_dataset, REFDATA *new_data, REFDATA rename_data,
                   EDIT_NAME_REQUEST *edit_column_request, long edit_column_requests,
                   EDIT_NAME_REQUEST *edit_parameter_request, long edit_parameter_requests,
                   EDIT_NAME_REQUEST *edit_array_request,long edit_array_requests,long filenumber);

char **process_editnames(char **orig_name, long **orig_flags, long orig_names,
                         EDIT_NAME_REQUEST *edit_request, long edit_requests,long filenumber);
long CopyRowToNewColumn(SDDS_DATASET *target, long target_row, SDDS_DATASET *source, long source_row,
              REFDATA new_data, long columns,char *input2);
long CopyParametersFromSecondInput(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source, REFDATA new_data);
long CopyArraysFromSecondInput (SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,REFDATA new_data);

typedef char *STRING_PAIR[2];

char *option[N_OPTIONS] = {
  "take", "leave", "match", "equate", "transfer", "reuse", "ifnot",
  "nowarnings", "ifis", "pipe", "fillin", "rename","editnames", "wildmatch"
} ;

char *USAGE1 = "sddsxref  [<input1>] <input2> <input3> [<input4>...] [<output>] \n\
[-pipe[=input][,output]]\n\
[-ifis={column|parameter},<name>[,...]]\n\
[-ifnot={parameter|column|array},<name>[,...]]\n\
[-transfer={parameter|array},<name>[,...]]\n\
[-take=<column-name>[,...]] [-leave=<column-name>[,...]]\n\
[-fillIn] [-reuse[=[rows][,page]]\n\
[-match=<column-name>[=<column-name>]]\n\
[-wildMatch=<column-name>[=<column-name>]]\n\
[-rename={column|parameter|array},<oldname>=<newname>[,...]]\n\
[-editnames={column|parameter|array},<wildcard-string>,<edit-string>]\n\
[-equate=<column-name>[=<column-name>]]\n\n\
sddsxref takes columns, parameters, and arrays from succesive pages\n\
from the list of files separated by space <input2> <input3> <input4...> \n\
and adds them to successive pages from <input1>.\n\
If <output> is given, the result is placed there; otherwise, <input1>\n\
is replaced. By default, all columns are taken from <input2>.\n\n\
ifnot      specifies names of parameters, arrays, and columns that may not\n\
           exist in <input1> if the program is to run as asked.\n\
ifis       specifies names of parameters, arrays, and columns that must\n\
           exist in <input1> if the program is to run as asked.\n\
transfer   specifies names of parameters or arrays to transfer from <input2>.\n\
take       specifies names of columns to take from <input2>.\n\
leave      specifies names of columns not to take from <input2>.\n\
           Overrides -take if both name a given column.\n\
           -leave=* results in no columns being taken.";
char *USAGE2 = "fillIn     specifies filling in NULL and 0 values in rows for which\n\
           no match is found.  By default, such rows are omitted.\n\
match      specifies names of columns to match between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.  In general, the first unused match from <input2>\n\
           is taken.\n\
wildMatch  similar to -match, but the data in <input2> may contain wildcards.\n\
           The first row in <input2> that matches each row in <input1> is used.\n\
equate     specifies names of columns to equate between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.  In general, the first unused match from <input2>\n\
           is taken.\n\
rename     Specifies new names for entities in the output data set.\n\
           The entities must still be referred to by their old names in \n\
           the other commandline options. \n\
editnames  Specifies creation of new names for entities of the specified \n\
           type with names matching the specified wildcard string. Editing \n\
           is performed using commands reminiscent of emacs keystrokes. \n\
           if -editnames=<entity>{column|parameter|array},wildcard,ei/%ld/ \n\
           is specified, the entity names will be changed to <name>N, N is the \n\
           position of input files in the command line. \n\
reuse      specifies that rows of <input2> may be reused, i.e., matched\n\
           with more than one row of <input1>.  Also, -reuse=page specifies\n\n\
           that only the first page of <input2> is used.\n\
           nowarnings specifies that warning messages should be suppressed.\n\
Program by Michael Borland. (This is version 8, April 2005, M. Borland)\n";


int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_1, SDDS_output;
  SDDS_DATASET *SDDS_ref;
  REFDATA *new_data,rename_data, *take_RefData;
  long i, j, i_arg, rows1, rows2, rows2Max, reuse, reusePage, endWarning,k;
  int32_t i1, i2, i3;
  SCANNED_ARG *s_arg;
  char s[200], *ptr;

  char **take_column, **leave_column, **output_column=NULL;
  char **inputfile, **referfile;
  char **match_column, **equate_column;
  long take_columns, leave_columns, match_columns, equate_columns, leave_all_columns;
  int32_t output_columns=0;
  char *input1, *input2, *output;
  long tmpfile_used, retval1, retval2, inputfiles, referfiles;
  long *row_used, wildMatch;
  TRANSFER_DEFINITION *transfer;
  long transfers;
  long warnings;
  IFITEM_LIST ifnot_item, ifis_item;
  unsigned long pipeFlags;
  long fillIn, keyGroups=0;
  KEYED_EQUIVALENT **keyGroup=NULL;
  long outputInitialized;
  int z, it, itm;
  long col;
  int firstRun, copyInput1Only;
  char **string1, **string2;
  double *value1, *value2;
  long matched;
  long equated;

  EDIT_NAME_REQUEST *edit_column_request,*edit_parameter_request,*edit_array_request;
  long edit_column_requests,edit_parameter_requests,edit_array_requests;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s\n%s", USAGE1,USAGE2);
    exit(1);
  }

  SDDS_ref = NULL;
  take_RefData = NULL;
  new_data=NULL;
 
  rename_data.columns=rename_data.parameters=rename_data.arrays=0;
  rename_data.new_column=rename_data.orig_column=rename_data.new_parameter
    =rename_data.orig_parameter=rename_data.new_array=rename_data.orig_array=NULL;
  edit_column_request=edit_parameter_request=edit_array_request=NULL;
  edit_column_requests=edit_parameter_requests=edit_array_requests=0; 

  input1 = input2 = output = NULL;
  take_column  = leave_column  = NULL;
  match_column  = equate_column  = NULL;
  inputfile = referfile = NULL;
  take_columns = leave_columns = match_columns = equate_columns = reuse = reusePage = 0;
  tmpfile_used = inputfiles = referfiles = 0;
  transfer = NULL;
  transfers = 0;
  ifnot_item.items = ifis_item.items = 0;
  warnings = 1;
  pipeFlags = 0;
  fillIn = 0;
  outputInitialized = 0;
  rows1=rows2=output_columns=0;
  string1=string2=NULL;
  wildMatch = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_LEAVE_COLUMNS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -leave syntax");
        leave_column = trealloc(leave_column, 
                                sizeof(*leave_column)*(leave_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
		    leave_column[i-1+leave_columns] = s_arg[i_arg].list[i];
        leave_columns += s_arg[i_arg].n_items-1;
        break;
      case SET_TAKE_COLUMNS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -take syntax");
        take_column = trealloc(take_column, 
                               sizeof(*take_column)*(take_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
		    take_column[i-1+take_columns] = s_arg[i_arg].list[i];
        take_columns += s_arg[i_arg].n_items-1;
        break;
      case SET_WILD_MATCH:
	wildMatch = 1;
	/* case fall-through intended here! */
      case SET_MATCH_COLUMN:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -match or -wildMatch syntax");
        if (match_columns!=0)
          SDDS_Bomb("only one -match or -wildMatch option may be given");
        match_column = tmalloc(sizeof(*match_column)*2);
        if ((ptr=strchr(s_arg[i_arg].list[1], '=')))
          *ptr++ = 0;
        else 
          ptr = s_arg[i_arg].list[1];
        match_column[0] = s_arg[i_arg].list[1];
        match_column[1] = ptr;
        match_columns = 1;
        break;
      case SET_EQUATE_COLUMN:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -equate syntax");
        if (equate_columns!=0)
          SDDS_Bomb("only one -equate option may be given");
        equate_column = tmalloc(sizeof(*equate_column)*2);
        if ((ptr=strchr(s_arg[i_arg].list[1], '=')))
          *ptr++ = 0;
        else 
          ptr = s_arg[i_arg].list[1];
        equate_column[0] = s_arg[i_arg].list[1];
        equate_column[1] = ptr;
        equate_columns = 1;
        break;
      case SET_TRANSFER:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -transfer syntax");
        transfer = trealloc(transfer, sizeof(*transfer)*(transfers+s_arg[i_arg].n_items-2));
        switch (match_string(s_arg[i_arg].list[1], transfer_type, TRANSFER_TYPES, 0)) {
        case PARAMETER_TRANSFER:
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            transfer[i-2+transfers].type = PARAMETER_TRANSFER;
            transfer[i-2+transfers].name = s_arg[i_arg].list[i];
          }
          break;
        case ARRAY_TRANSFER:
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            transfer[i-2+transfers].type = ARRAY_TRANSFER;
            transfer[i-2+transfers].name = s_arg[i_arg].list[i];
          }
          break;
        default:
          SDDS_Bomb("unknown type of transfer");
          break;
        }
        transfers += s_arg[i_arg].n_items-2;
        break;
      case SET_REUSE:
        if (s_arg[i_arg].n_items==1)
          reuse = 1;
        else {
          char *reuseOptions[2] = {"rows", "page"};
          for (i=1; i<s_arg[i_arg].n_items; i++) {
            switch (match_string(s_arg[i_arg].list[i], reuseOptions, 2, 0)) {
            case 0:
              reuse = 1;
              break;
            case 1:
              reusePage = 1;
              break;
            default:
              SDDS_Bomb("unknown reuse keyword");
              break;
            }
          }
        }
        break;
      case SET_IFNOT:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -ifnot usage");
        add_ifitem(&ifnot_item, s_arg[i_arg].list+1, s_arg[i_arg].n_items-1);
        break;
      case SET_NOWARNINGS:
        warnings = 0;
        break;
      case SET_IFIS:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -ifis usage");
        add_ifitem(&ifis_item, s_arg[i_arg].list+1, s_arg[i_arg].n_items-1);
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_FILLIN:
        fillIn = 1;
        break;
      case SET_RENAME:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -rename syntax");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          k=rename_data.columns;
          rename_data.new_column = 
            trealloc(rename_data.new_column,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          rename_data.orig_column = 
            trealloc(rename_data.orig_column,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '=')))
              SDDS_Bomb("invalid -rename syntax");
            *ptr++ = 0;
            rename_data.orig_column[k+i-2] = s_arg[i_arg].list[i];
            rename_data.new_column[k+i-2] = ptr;
          }
          rename_data.columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          k=rename_data.parameters;
          rename_data.new_parameter =
            trealloc(rename_data.new_parameter,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          rename_data.orig_parameter =
            trealloc(rename_data.orig_parameter,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '=')))
              SDDS_Bomb("invalid -rename syntax");
            *ptr++ = 0;
            rename_data.orig_parameter[k+i-2]= s_arg[i_arg].list[i];
            rename_data.new_parameter[k+i-2] = ptr;
          }
          rename_data.parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          k=rename_data.arrays;
          rename_data.new_array =
            trealloc(rename_data.new_array,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          rename_data.orig_array =
            trealloc(rename_data.orig_array,sizeof(char*)*(k+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '=')))
              SDDS_Bomb("invalid -rename syntax");
            *ptr++ = 0;
            rename_data.orig_array[k+i-2]= s_arg[i_arg].list[i];
            rename_data.new_array[k+i-2] = ptr;
          }
          rename_data.arrays += s_arg[i_arg].n_items-2;         
          break;
        default:
          SDDS_Bomb("invalid -rename syntax: specify column or parameter keyword");
          break;
        }
        break;
      case SET_EDIT_NAMES: 
        if (s_arg[i_arg].n_items!=4)
          SDDS_Bomb("invalid -editnames syntax");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          edit_column_request =
            trealloc(edit_column_request, sizeof(*edit_column_request)*(edit_column_requests+1));
          edit_column_request[edit_column_requests].match_string = s_arg[i_arg].list[2];
          edit_column_request[edit_column_requests].edit_string  = s_arg[i_arg].list[3];
          edit_column_requests++;
          break;
        case PARAMETER_MODE:
          edit_parameter_request =
            trealloc(edit_parameter_request, sizeof(*edit_parameter_request)*(edit_parameter_requests+1));
          edit_parameter_request[edit_parameter_requests].match_string = s_arg[i_arg].list[2];
          edit_parameter_request[edit_parameter_requests].edit_string  = s_arg[i_arg].list[3];
          edit_parameter_requests++;
          break;
        case ARRAY_MODE:
          edit_array_request =
            trealloc(edit_array_request, sizeof(*edit_array_request)*(edit_array_requests+1));
          edit_array_request[edit_array_requests].match_string = s_arg[i_arg].list[2];
          edit_array_request[edit_array_requests].edit_string  = s_arg[i_arg].list[3];
          edit_array_requests++;
          break;
        default:
          SDDS_Bomb("invalid -editnames syntax: specify column or parameter keyword");
          break;
        }
        break;
      default:
        fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
        SDDS_Bomb(NULL);
        break;
      }
    }
    else {
      inputfile = trealloc(inputfile, sizeof(*inputfile)*(inputfiles+1));
      inputfile[inputfiles++] = s_arg[i_arg].list[0];
    }
  }

  if (inputfiles == 0) {
    SDDS_Bomb("too few reference files given");
  }
  else {
    if (!(pipeFlags&USE_STDIN) && !(pipeFlags&USE_STDOUT)) {
      if (inputfiles < 2) {
        SDDS_Bomb("too few reference files given");
      }
      else if (inputfiles == 2) {
        input1 = output = inputfile[0];
        referfile = trealloc(referfile, sizeof(*referfile)*(referfiles+1));
        referfile[0] = inputfile[1];
        referfiles ++;
      }
      else {
        input1 = inputfile[0];
        output = inputfile[--inputfiles];
        for (z=1; z < inputfiles; z++) {
          referfile = trealloc(referfile, sizeof(*referfile)*(referfiles+1));
          referfile[z - 1] = inputfile[z];
          referfiles++ ;
        }
      }
    }
    else if (!(pipeFlags&USE_STDIN) && (pipeFlags&USE_STDOUT)) {
      if (inputfiles < 2) {
        SDDS_Bomb("too few reference files given");
      }
      else {
        input1 = inputfile[0];
        for (z=1; z<inputfiles; z++) {
          referfile = trealloc(referfile, sizeof(*referfile)*(referfiles+1));
          referfile[z - 1] = inputfile[z];
          referfiles++ ;
        }
      }
    }
    else if ((pipeFlags&USE_STDIN) && !(pipeFlags&USE_STDOUT)) {
      if (inputfiles < 2) {
        SDDS_Bomb("too few reference files given");
      }
      else {
        output = inputfile[--inputfiles];
        for (z=0; z<inputfiles; z++) {
          referfile = trealloc(referfile, sizeof(*referfile)*(referfiles+1));
          referfile[z] = inputfile[z];
          referfiles++ ;
        }
      }
    }
    else {
      for (z=0; z<inputfiles; z++) {
        referfile = trealloc(referfile, sizeof(*referfile)*(referfiles+1));
        referfile[z] = inputfile[z];
        referfiles++ ;
      }
    }
  }
  
  processFilenames("sddsxref", &input1, &output, pipeFlags, !warnings, &tmpfile_used);

  if (equate_columns && match_columns)
    SDDS_Bomb("only one of -equate or -match may be given");

  if (!SDDS_InitializeInput(&SDDS_1, input1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (!check_ifitems(&SDDS_1, &ifnot_item, 0) || !check_ifitems(&SDDS_1, &ifis_item, 1))
    exit(0);

  for (it=0; it<ifnot_item.items; it++) {
    switch (ifnot_item.type[it]) {
    case COLUMN_BASED:
      leave_column = trealloc(leave_column, 
                              sizeof(*leave_column)*(leave_columns+1));
      leave_column[leave_columns] = ifnot_item.name[it];
      leave_columns++;
      break;
    case PARAMETER_BASED:
      for (itm=0; itm<transfers; itm++) {
        if (strcmp(transfer[itm].name,ifnot_item.name[it])==0) {
          SDDS_Bomb("Excluded item is a part of -transfer list.");
          exit(1);
        }
      }
      break;
    case ARRAY_BASED:
      for (itm=0; itm<transfers; itm++) {
        if (strcmp(transfer[itm].name,ifnot_item.name[it])==0) {
          SDDS_Bomb("Excluded item is a part of -transfer list.");
          exit(1);
        }
      }
      break;
    default:
      SDDS_Bomb("internal error---unknown ifitem type");
      exit(1);
      break;
    }
  }

  /*allocate memory for new_data */
  new_data=malloc(sizeof(*new_data)*referfiles); 
  for (z=0; z<referfiles; z++) {
    SDDS_ref = trealloc(SDDS_ref, sizeof(*SDDS_ref)*(z+1));
    input2 = referfile[z];  
    if (!SDDS_InitializeInput(&SDDS_ref[z], input2)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    take_RefData = trealloc(take_RefData,sizeof(*take_RefData)*(z+1));
    take_RefData[z].columns=0;
   
    add_newnames(&SDDS_ref[z],&new_data[z],rename_data,edit_column_request,edit_column_requests,
                  edit_parameter_request,edit_parameter_requests,
                  edit_array_request,edit_array_requests,z+1);
    
    if (SDDS_ColumnCount(&SDDS_ref[z])) {
      SDDS_SetColumnFlags(&SDDS_ref[z], 1);
      if (take_columns) {
        SDDS_SetColumnFlags(&SDDS_ref[z], 0);
        for (i=0; i<take_columns; i++) {
          if (!has_wildcards(take_column[i]) &&
              SDDS_GetColumnIndex(&SDDS_ref[z], take_column[i])<0) {
            sprintf(s, "error: column %s not found in file %s take_columns %ld SDDS_ref[z] %" PRId32 "\n", take_column[i], input2, take_columns, SDDS_ref[z].n_rows);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (!SDDS_SetColumnsOfInterest(&SDDS_ref[z], SDDS_MATCH_STRING, take_column[i], SDDS_OR))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      
      leave_all_columns = 0;
      if (leave_columns==1 && strcmp(leave_column[0], "*")==0)
        leave_all_columns = 1;
      else {
        if (!take_columns)
          SDDS_SetColumnFlags(&SDDS_ref[z], 1);
        for (i=0; i<leave_columns; i++) {
          if (!has_wildcards(leave_column[i]) && SDDS_GetColumnIndex(&SDDS_ref[z], leave_column[i])<0) 
            continue;
          if (!SDDS_SetColumnsOfInterest(&SDDS_ref[z], SDDS_MATCH_STRING, leave_column[i], SDDS_AND|SDDS_NEGATE_MATCH))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        
        if (!(take_RefData[z].orig_column = (char**)SDDS_GetColumnNames(&SDDS_ref[z],
                                                                        &take_RefData[z].columns))) {
          SDDS_SetError("error: no columns selected to take from input file");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      
      if (match_columns) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, match_column[0]))<0 || SDDS_GetColumnType(&SDDS_1, j)!=SDDS_STRING) {
          sprintf(s, "error: column %s not found or not string type in file %s", match_column[0], 
                  input1?input1:"stdin");
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if ((j=SDDS_GetColumnIndex(&SDDS_ref[z], match_column[1]))<0 || SDDS_GetColumnType(&SDDS_ref[z], j)!=SDDS_STRING) {
          sprintf(s, "error: column %s not found or not string type in file %s", match_column[1], input2);
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (equate_columns) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, equate_column[0]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_1, j))) {
          sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[0], 
                  input1?input1:"stdin");
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if ((j=SDDS_GetColumnIndex(&SDDS_ref[z], equate_column[1]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_ref[z], j))) {
          sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[1], input2);
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
    }
    else {
      take_RefData[z].columns = 0;
      leave_all_columns = 1;
    }
    if (!take_RefData[z].columns && !leave_all_columns && warnings)
      fprintf(stderr, "warning: there are no columns being taken from %s that are not already in %s\n", 
              input2, input1?input1:"stdin");
    /*
    if (!leave_all_columns && !take_RefData[z].columns 
        && !(take_RefData[z].orig_column 
             = SDDS_GetColumnNames(&SDDS_ref[z],&take_RefData[z].columns))) {
      sprintf(s, "Problem getting list of columns names for file %s", input2);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    } */
    if (leave_all_columns)
      take_RefData[z].columns = 0;
    
    if (!outputInitialized) {    
      if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_1, output, "w")) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      outputInitialized = 1;
    }
    
    /*get the new name for new_data if there is a match of original name*/
    if (take_RefData[z].columns) 
      take_RefData[z].new_column=(char**)malloc(sizeof(char*)*take_RefData[z].columns);
   
    for (i=0; i<take_RefData[z].columns; i++) {
      k=0;
      /*if there are new names (renamed or edited), find the corresponding orignal name index,
        and assign the new name to take_RefData*/
      if (new_data[z].columns) {
        k=match_string(take_RefData[z].orig_column[i],new_data[z].orig_column,
                       new_data[z].columns,EXACT_MATCH);
        if (k==-1)
          SDDS_CopyString(&take_RefData[z].new_column[i],take_RefData[z].orig_column[i]);
        else
          SDDS_CopyString(&take_RefData[z].new_column[i],new_data[z].new_column[k]);
      }
      else
         SDDS_CopyString(&take_RefData[z].new_column[i],take_RefData[z].orig_column[i]);
      if (SDDS_GetColumnIndex(&SDDS_output, take_RefData[z].new_column[i])>=0) {
        free(take_RefData[z].new_column[i]);
        free(take_RefData[z].orig_column[i]); 
        for (j=i; j<take_RefData[z].columns-1; j++) 
          take_RefData[z].orig_column[j] = take_RefData[z].orig_column[j+1];
        take_RefData[z].columns -=1;
        i--;
        if (take_RefData[z].columns==0)
          break;
      } 
      else {
        /*define column for ouput*/
        if (!SDDS_TransferColumnDefinition(&SDDS_output, &SDDS_ref[z],take_RefData[z].orig_column[i],
                                           take_RefData[z].new_column[i]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
   
    if (!take_RefData[z].columns && !leave_all_columns && warnings)
      fprintf(stderr, "warning: there are no columns being taken from %s that are not already in %s\n", 
              input2, input1?input1:"stdin");
    output_columns=0;
    if (take_RefData[z].columns && 
        (!(output_column = (char**)SDDS_GetColumnNames(&SDDS_output, &output_columns)) 
         || output_columns==0)) {
      SDDS_SetError("Problem getting output column names");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (output_columns) {
      for (i=0;i<output_columns;i++)
        free(output_column[i]);
      free(output_column);
    } 
    
    take_RefData[z].parameters = take_RefData[z].arrays = 0;
    if (transfers) {
      if (!expandTransferRequests(&take_RefData[z].orig_parameter, &take_RefData[z].parameters, 
                                  PARAMETER_TRANSFER, transfer, transfers, &SDDS_ref[z]) ||
          !expandTransferRequests(&take_RefData[z].orig_array, &take_RefData[z].arrays, 
                                  ARRAY_TRANSFER, transfer, transfers, &SDDS_ref[z]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
    /*get the new name for parameters, define parameters for output */
    if (take_RefData[z].parameters)
      take_RefData[z].new_parameter=(char**)malloc(sizeof(char*)*take_RefData[z].parameters);
    if (take_RefData[z].arrays)
      take_RefData[z].new_array=(char**)malloc(sizeof(char*)*take_RefData[z].arrays);  
    
    for (i=0;i<take_RefData[z].parameters;i++) {
      k=0;
      if (new_data[z].parameters) {
        k=match_string(take_RefData[z].orig_parameter[i],new_data[z].orig_parameter,
                       new_data[z].parameters,EXACT_MATCH);
        if (k!=-1)
          SDDS_CopyString(&take_RefData[z].new_parameter[i],new_data[z].new_parameter[k]);
        else 
          SDDS_CopyString(&take_RefData[z].new_parameter[i],take_RefData[z].orig_parameter[i]);
      }
      else
        SDDS_CopyString(&take_RefData[z].new_parameter[i],take_RefData[z].orig_parameter[i]);
      if (SDDS_GetParameterIndex(&SDDS_output, take_RefData[z].new_parameter[i])>=0) {
        free(take_RefData[z].orig_parameter[i]);
        free(take_RefData[z].new_parameter[i]);
        for (col=i; col<take_RefData[z].parameters-1; col++)
          take_RefData[z].orig_parameter[col]=take_RefData[z].orig_parameter[col+1];
        take_RefData[z].parameters -= 1;
        i--;
        if (take_RefData[z].parameters==0 )
          break;
      }
      else {
        if (!SDDS_TransferParameterDefinition(&SDDS_output, &SDDS_ref[z],
                                              take_RefData[z].orig_parameter[i],
                                              take_RefData[z].new_parameter[i]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    /*get the new name for arrays, and define arrays for output */
    for (i=0;i<take_RefData[z].arrays;i++) {
      k=0;
      if (new_data[z].arrays) {
        k=match_string(take_RefData[z].orig_array[i],new_data[z].orig_array,
                       new_data[z].arrays,EXACT_MATCH);
        if (k==-1)
          SDDS_CopyString(&take_RefData[z].new_array[i],take_RefData[z].orig_array[i]);
        else
          SDDS_CopyString(&take_RefData[z].new_array[i],new_data[z].new_array[k]);
      }
      else
        SDDS_CopyString(&take_RefData[z].new_array[i],take_RefData[z].orig_array[i]);
      if (SDDS_GetArrayIndex(&SDDS_output, take_RefData[z].new_array[i])>=0) {
        free(take_RefData[z].orig_array[i]);
        free(take_RefData[z].new_array[i]);
        for (col=i; col<take_RefData[z].arrays-1; col++)
          take_RefData[z].orig_array[col]=take_RefData[z].orig_array[col+1];
        take_RefData[z].arrays -= 1;
        i--;
        if (take_RefData[z].arrays==0 )
          break;
      }
      else {
        if (!SDDS_TransferArrayDefinition(&SDDS_output, &SDDS_ref[z],take_RefData[z].orig_array[i],
                                          take_RefData[z].new_array[i]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  
  if (!SDDS_WriteLayout(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  free(leave_column);
  if (take_columns) {
    SDDS_FreeStringArray(take_column, take_columns);
    free(take_column);
  }
  
  row_used = NULL;
  endWarning = 0;

  while ((retval1=SDDS_ReadPage(&SDDS_1))>0) {
    copyInput1Only = 0;
    rows1 = SDDS_CountRowsOfInterest(&SDDS_1);
    if (!SDDS_StartPage(&SDDS_output, rows1)) {
      SDDS_SetError("Problem starting output page");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (fillIn && !SDDS_ClearPage(&SDDS_output)) {
      SDDS_SetError("Problem clearing output page");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    for (z=0; z<referfiles; z++) {
      input2 = referfile[z];
      if (!reusePage) {
        if ((retval2=SDDS_ReadPage(&SDDS_ref[z]))<=0 && !endWarning) {
          if (warnings)
            fprintf(stderr, "warning: <input2> ends before <input1>\n");
          endWarning = 1;
        }
      }
      else {
        if (retval1==1 && (retval2=SDDS_ReadPage(&SDDS_ref[z]))<=0) {
          if (!endWarning && warnings)
            fprintf(stderr, "warning: <input2> has no data\n");
          endWarning = 1;
        }
        else
          SDDS_SetRowFlags(&SDDS_ref[z], 1);
      }
      if (take_RefData[z].columns && (!SDDS_SetColumnFlags(&SDDS_ref[z], 0) ||
                                      !SDDS_SetColumnsOfInterest(&SDDS_ref[z], SDDS_NAME_ARRAY, take_RefData[z].columns, take_RefData[z].orig_column)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      
      /*copy parameters and arrays */
      if (!CopyParametersFromSecondInput(&SDDS_output, &SDDS_ref[z],take_RefData[z])) {
        SDDS_SetError("Problem copying parameter from second input file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!CopyArraysFromSecondInput(&SDDS_output, &SDDS_ref[z],take_RefData[z])) {
        SDDS_SetError("Problem copying parameter from second input file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    if (!SDDS_CopyParameters(&SDDS_output, &SDDS_1) || !SDDS_CopyArrays(&SDDS_output, &SDDS_1)) {
      SDDS_SetError("Problem copying parameter or array data from first input file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
    firstRun = 1;
    for (z=rows2Max=0; z<referfiles; z++) {
      input2 = referfile[z];
      rows2 = SDDS_CountRowsOfInterest(&SDDS_ref[z]);
      rows2Max = rows2>rows2Max ? rows2 : rows2Max;
    
      if (!firstRun) {
        /* DO NOT USE SDDS_CountRowsOfInterest because 
           CopyRowToNewColumn and SDDS_AssertRowFlags expect
           the real row index and not the index of rows of interest */
        rows1=SDDS_RowCount(&SDDS_output);
      }
      if (take_RefData[z].columns) {
        if (!rows2) {
          if (!SDDS_SetRowFlags(&SDDS_output, fillIn)) {
            SDDS_SetError("Problem setting row flags for output file.");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        }
        else if (rows1) {
          if (match_columns) {
            if (firstRun) {
              if (!(string1 = (char**)SDDS_GetColumn(&SDDS_1, match_column[0]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        match_column[0], input1?input1:"stdin");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
            else {
              if (!(string1 = (char**)SDDS_GetColumn(&SDDS_output, match_column[0]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        match_column[0], input1?input1:"stdin");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
            if (!(string2 = (char**)SDDS_GetColumn(&SDDS_ref[z], match_column[1]))) {
              fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                      match_column[1], input2);
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
	    if (!wildMatch)
	      keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_STRING, string2, rows2);
            i3 = 0;
            for (i1=0; i1<rows1; i1++) {
              if (firstRun) {
                if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
                  sprintf(s, "Problem copying row %ld of first data set", i1);
                  SDDS_SetError(s);
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              matched = 0;
              if ((&SDDS_output)->row_flag[i1]) {
                if (!wildMatch) {
                  if ((i2 = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_STRING, string1+i3, reuse))>=0)
                    matched = 1;
                } else {
                  if ((i2 = match_string(string1[i3], string2, rows2, WILDCARD_MATCH))>=0)
                    matched = 1;
                }
                if (matched) {
                  if (!CopyRowToNewColumn(&SDDS_output,i1, &SDDS_ref[z], i2,take_RefData[z],
                                          take_RefData[z].columns,input2)) {
                    fprintf(stderr,"error in copying data to output!\n");
                    exit(1);
                  }
                } else {
                  if (!fillIn && !SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, (int32_t)0)) 
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                  if (warnings)
                    fprintf(stderr, "warning: no match for row %ld (%s = \"%s\")\n", i3,
                            match_column[0], string1[i3]);
                }
                i3++;
              }
            }
            firstRun = 0;
            if (string1) {
              for (i=0;i<i3;i++) 
                free(string1[i]);
              free(string1);
            }
            if (string2) {
              for (i=0;i<rows2;i++) 
                free(string2[i]);
              free(string2);
            }
            
            for (i=0;i<keyGroups;i++) {
              free(keyGroup[i]->equivalent);
              free(keyGroup[i]);
            }
            free(keyGroup);
          }
          else if (equate_columns) {
            if (firstRun) {
              if (!(value1 = SDDS_GetColumnInDoubles(&SDDS_1, equate_column[0]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        equate_column[0], input1?input1:"stdin");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
            else {
              if (!(value1 = SDDS_GetColumnInDoubles(&SDDS_output, equate_column[0]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        equate_column[0], input1?input1:"stdin");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
            }
            if (!(value2 = SDDS_GetColumnInDoubles(&SDDS_ref[z], equate_column[1]))) {
              fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                      equate_column[1], input2);
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
            
            i3 = 0;
            keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_DOUBLE, value2, rows2);
            for (i1=0; i1<rows1; i1++) {
              if (firstRun) {
                if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
                  sprintf(s, "Problem copying row %ld of first data set", i1);
                  SDDS_SetError(s);
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              equated = 0;
              if ((&SDDS_output)->row_flag[i1]) {
                if ((i2 = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_DOUBLE, value1+i3, reuse))>=0) {
                  equated = 1;                  
                  if (!CopyRowToNewColumn(&SDDS_output,i1, &SDDS_ref[z], i2,take_RefData[z],
                                          take_RefData[z].columns,input2)) {
                    fprintf(stderr,"error in copying data to output!\n");
                    exit(1);
                  }
                } else {
                  if (!fillIn && !SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, (int32_t)0)) 
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                  if (warnings)
                    fprintf(stderr, "warning: no equal for row %ld (%s = %g)\n", i3,
                            equate_column[0], value1[i3]);
                }
                i3++;
              }
            }
            firstRun=0;
            if (i3 && equate_columns)
              free(value1);
            if (rows2 && equate_columns)
              free(value2);
            for (i=0;i<keyGroups;i++) {
              free(keyGroup[i]->equivalent);
              free(keyGroup[i]);
            }
            free(keyGroup);
          }
          else {
            for (i1=0; i1<rows1; i1++) {
              i2 = i1;
              if (i2>=rows2) {
                if (!reuse) {
		  if (fillIn) {
		    if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
		      sprintf(s, "Problem copying row %ld of first data set", i1);
		      SDDS_SetError(s);
		      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
		    }
		  }
                  if (warnings)
                    fprintf(stderr, "warning: no row in file 2 for row %ld in file 1\n",i1);
                  continue;
                } else
                  i2 = rows2-1;
              }
              if (firstRun) {
                if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
                  sprintf(s, "Problem copying row %ld of first data set", i1);
                  SDDS_SetError(s);
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                } 
              }
              if (take_RefData[z].columns &&
                  !CopyRowToNewColumn(&SDDS_output,i1, &SDDS_ref[z], 
                                      i2,take_RefData[z],take_RefData[z].columns,input2)) {
                fprintf(stderr,"error in copying data to output!\n");
                exit(1);
              }
            }
            firstRun=0;
          }
        }
      }
      else {
        if (rows2) {
          if (rows1) {
            if (match_columns) {
              if (firstRun) {
                if (!(string1 = (char**)SDDS_GetColumn(&SDDS_1, match_column[0]))) {
                  fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                          match_column[0], input1?input1:"stdin");
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              else {
                if (!(string1 = (char**)SDDS_GetColumn(&SDDS_output, match_column[0]))) {
                  fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                          match_column[0], input1?input1:"stdin");
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              if (!(string2 = (char**)SDDS_GetColumn(&SDDS_ref[z], match_column[1]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        match_column[1], input2);
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
              keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_STRING, string2, rows2);
              i3=0;
              for (i1=0; i1<rows1; i1++) {
                if (firstRun) {
                  if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
                    sprintf(s, "Problem copying row %ld of first data set", i1);
                    SDDS_SetError(s);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                  }
                }
                if ((&SDDS_output)->row_flag[i1]) {
                  if ((FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_STRING, string1+i3, reuse))<0) {
                    if (!fillIn && !SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, (int32_t)0)) 
                      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    if (warnings)
                      fprintf(stderr, "warning: no match for row %ld (%s = \"%s\")\n", i3,
                              match_column[0], string1[i3]);
                  }
                  i3++;
                }
              }
              firstRun=0;
              if (string1) {
                for (i=0;i<i3;i++) 
                  free(string1[i]);
                free(string1);
              }
              if (string2) {
                for (i=0;i<rows2;i++) 
                  free(string2[i]);
                free(string2);
              }
              
              for (i=0;i<keyGroups;i++) {
                free(keyGroup[i]->equivalent);
                free(keyGroup[i]);
              }
              free(keyGroup);
            }    
            else if (equate_columns) {
              if (firstRun) {
                if (!(value1 = SDDS_GetColumnInDoubles(&SDDS_1, equate_column[0]))) {
                  fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                          equate_column[0], input1?input1:"stdin");
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              else {
                if (!(value1 = SDDS_GetColumnInDoubles(&SDDS_output, equate_column[0]))) {
                  fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                          equate_column[0], input1?input1:"stdin");
                  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
              }
              if (!(value2 = SDDS_GetColumnInDoubles(&SDDS_ref[z], equate_column[1]))) {
                fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                        equate_column[1], input2);
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
              keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_DOUBLE, value2, rows2);
              i3 = 0;
              for (i1=0; i1<rows1; i1++) {
                if (firstRun) {
                  if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
                    sprintf(s, "Problem copying row %ld of first data set", i1);
                    SDDS_SetError(s);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                  }
                }
                if ((&SDDS_output)->row_flag[i1]) {
                  if ((FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_DOUBLE, value1+i3, reuse))<0) {
                    if (!fillIn && !SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, (int32_t)0)) 
                      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    if (warnings)
                      fprintf(stderr, "warning: no equal for row %ld (%s = %g)\n", i3,
                              equate_column[0], value1[i3]);
                  }
                  i3++;
                }
              }
              firstRun=0;
              if (i3 && equate_columns)
                free(value1);
              if (rows2 && equate_columns)
                free(value2);
              for (i=0;i<keyGroups;i++) {
                free(keyGroup[i]->equivalent);
                free(keyGroup[i]);
              }
              free(keyGroup);
            }
          }         
        }
        copyInput1Only++;
      }
    }
    if ((rows2Max==0 && fillIn) || (copyInput1Only==referfiles && !match_columns && !equate_columns)) {
      if (!SDDS_CopyColumns(&SDDS_output, &SDDS_1)) {
        SDDS_SetError("Problem copying tabular data for output file.");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    
    if (!SDDS_WritePage(&SDDS_output)) {
      SDDS_SetError("Problem writing data to output file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  
  
  for (z=0;z<referfiles;z++) {
    free(referfile[z]);
    
    if (take_RefData[z].columns) {
      for (i=0;i<take_RefData[z].columns;i++) {
        free(take_RefData[z].new_column[i]);
        free(take_RefData[z].orig_column[i]);
      }
      free(take_RefData[z].new_column);
      free(take_RefData[z].orig_column);
    }
    
    if (take_RefData[z].parameters) {
      for (i=0;i<take_RefData[z].parameters;i++) {
        free(take_RefData[z].new_parameter[i]);
        free(take_RefData[z].orig_parameter[i]);
      }
      free(take_RefData[z].new_parameter);
      free(take_RefData[z].orig_parameter);
    }
    
    if (take_RefData[z].arrays) {
      for (i=0;i<take_RefData[z].arrays;i++) {
        free(take_RefData[z].new_array[i]);
        free(take_RefData[z].orig_array[i]);
      }
      free(take_RefData[z].new_array);
      free(take_RefData[z].orig_array);
    }
    
    if (new_data[z].columns) {
      for (i=0;i<new_data[z].columns;i++) {
        free(new_data[z].new_column[i]);
        free(new_data[z].orig_column[i]);
      }
      free(new_data[z].new_column);
      free(new_data[z].orig_column); 
    }
    
    if (new_data[z].parameters) {
      for (i=0;i<new_data[z].parameters;i++) {
        free(new_data[z].new_parameter[i]);
        free(new_data[z].orig_parameter[i]);
      }
      free(new_data[z].new_parameter);
      free(new_data[z].orig_parameter); 
    }
    if (new_data[z].arrays) {
      for (i=0;i<new_data[z].arrays;i++) {
        free(new_data[z].new_array[i]);
        free(new_data[z].orig_array[i]);
      }
      free(new_data[z].new_array);
      free(new_data[z].orig_array); 
    }
  }
  if (new_data)
    free(new_data); 
 
  if (edit_column_requests) {
    for (i=0;i<edit_column_requests;i++) {
      free(edit_column_request[i].match_string);
      free(edit_column_request[i].edit_string);
    }
    free(edit_column_request);
  }  
  if (edit_parameter_requests) {
    for (i=0;i<edit_parameter_requests;i++) {
      free(edit_parameter_request[i].match_string);
      free(edit_parameter_request[i].edit_string);
    }
    free(edit_parameter_request);
  } 

  if (edit_array_requests) {
    for (i=0;i<edit_array_requests;i++) {
      free(edit_array_request[i].match_string);
      free(edit_array_request[i].edit_string);
    }
    free(edit_array_request);
  }  
  
  free(take_RefData);
  free(referfile);
  free(inputfile);

  if (match_columns)
    free(match_column);
  if (equate_columns)
    free(equate_column);
  
/*#ifdef SOLARIS */
  if (!SDDS_Terminate(&SDDS_output) || !SDDS_Terminate(&SDDS_1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  for (z=0; z<referfiles; z++) {
    if (!SDDS_Terminate(&SDDS_ref[z])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  free(SDDS_ref);
/*#endif */
  if (tmpfile_used && !replaceFileAndBackUp(input1, output))
    exit(1);
  free(input1);
  free(output);
  return 0;
}

long expandTransferRequests(char ***match, int32_t *matches, 
                            long type, TRANSFER_DEFINITION *transfer, long transfers,
                            SDDS_DATASET *inSet)
{
  long i, first;
  int32_t (*matchRoutine)(SDDS_DATASET *SDDS_dataset, char ***nameReturn, int32_t matchMode, int32_t typeMode, ... );

  *matches = 0;
  *match = NULL;
  if (!transfers)
    return 1;
  switch (type) {
  case PARAMETER_TRANSFER:
    matchRoutine = SDDS_MatchParameters;
    break;
  case ARRAY_TRANSFER:
    matchRoutine = SDDS_MatchArrays;
    break;
  default:
    SDDS_Bomb("invalid transfer type--this shouldn't happen");
    exit(1);
    break;
  }
  first = 0;
  for (i=0; i<transfers; i++) {
    if (transfer[i].type==type) {
      if ((*matches=(*matchRoutine)(inSet, match, SDDS_MATCH_STRING, FIND_ANY_TYPE,
                                    transfer[i].name, SDDS_OR|(first?SDDS_0_PREVIOUS:0)))==-1) {
        return 0;
      }
      first = 0;
    }
  }
  return 1;
}

void add_newnames(SDDS_DATASET *SDDS_dataset, REFDATA *new_data, REFDATA rename_data,
                   EDIT_NAME_REQUEST *edit_column_request, long edit_column_requests,
                   EDIT_NAME_REQUEST *edit_parameter_request, long edit_parameter_requests,
                   EDIT_NAME_REQUEST *edit_array_request,long edit_array_requests, long filenumber)
{
  long i, k=0, *orig_columnflags;
  int32_t columns,parameters,arrays;
  long *orig_parameterflags,*orig_arrayflags;
  char **column_names,**parameter_names,**array_names,**new_names;

  columns=parameters=arrays=0;
  column_names=parameter_names=array_names=new_names=NULL;
  orig_columnflags=orig_parameterflags=orig_arrayflags=NULL;
  new_data->columns=new_data->parameters=new_data->arrays=0;
  new_data->new_column=new_data->orig_column=NULL;
  new_data->new_parameter=new_data->orig_parameter=NULL;
  new_data->new_array=new_data->orig_array=NULL;

  /*no edit requests at all */
  if (!edit_column_requests && !edit_parameter_requests && !edit_array_requests &&
      !rename_data.columns && !rename_data.parameters && !rename_data.arrays)
    return;

  /*transfer renames to new_data*/
  (*new_data).columns=rename_data.columns;
  (*new_data).parameters=rename_data.parameters;
  (*new_data).arrays=rename_data.arrays;
  if (rename_data.columns) {
    (*new_data).new_column=(char**)malloc(sizeof(char*)*rename_data.columns);
    (*new_data).orig_column=(char**)malloc(sizeof(char*)*rename_data.columns);
    for (i=0;i<rename_data.columns;i++) {
      SDDS_CopyString(&(*new_data).new_column[i],rename_data.new_column[i]);
      SDDS_CopyString(&(*new_data).orig_column[i],rename_data.orig_column[i]);
    }
  }
  if (rename_data.parameters) {
    (*new_data).new_parameter=(char**)malloc(sizeof(char*)*rename_data.parameters);
    (*new_data).orig_parameter=(char**)malloc(sizeof(char*)*rename_data.parameters);
    for (i=0;i<rename_data.parameters;i++) {
      SDDS_CopyString(&(*new_data).new_parameter[i],rename_data.new_parameter[i]);
      SDDS_CopyString(&(*new_data).orig_parameter[i],rename_data.orig_parameter[i]);
    }
  }
  if (rename_data.arrays) {
    (*new_data).new_array=(char**)malloc(sizeof(char*)*rename_data.arrays);
    (*new_data).orig_array=(char**)malloc(sizeof(char*)*rename_data.arrays);
    for (i=0;i<rename_data.arrays;i++) {
      SDDS_CopyString(&(*new_data).new_array[i],rename_data.new_array[i]);
      SDDS_CopyString(&(*new_data).orig_array[i],rename_data.orig_array[i]);
    }
  }

  if (!(column_names=SDDS_GetColumnNames(SDDS_dataset, &columns))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!(parameter_names=SDDS_GetParameterNames(SDDS_dataset, &parameters))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (!(array_names=SDDS_GetArrayNames(SDDS_dataset, &arrays))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  } 

  /*process edit names */
  if (edit_column_requests) {
    if ((new_names=process_editnames(column_names,&orig_columnflags,columns,
                                     edit_column_request,edit_column_requests, filenumber))) {
      for (i=0;i<columns;i++)
      {
        if (orig_columnflags[i]) {
          k=(*new_data).columns;
          (*new_data).new_column=trealloc((*new_data).new_column,sizeof(char*)*(k+1));
          (*new_data).orig_column=trealloc((*new_data).orig_column,sizeof(char*)*(k+1));
          SDDS_CopyString(&(*new_data).new_column[k],new_names[i]);
          SDDS_CopyString(&(*new_data).orig_column[k],column_names[i]); 
          (*new_data).columns++;
        }
        free(new_names[i]);
      }
      free(new_names);
    }
  }
  
  if (edit_parameter_requests) {
    if ((new_names=process_editnames(parameter_names,&orig_parameterflags,parameters,
                                     edit_parameter_request,edit_parameter_requests,filenumber))) {
      for (i=0;i<parameters;i++)
      {
        if (orig_parameterflags[i]) {
          k=(*new_data).parameters;
          (*new_data).new_parameter=trealloc((*new_data).new_parameter,sizeof(char*)*(k+1));
          (*new_data).orig_parameter=trealloc((*new_data).orig_parameter,sizeof(char*)*(k+1));
          SDDS_CopyString(&(*new_data).new_parameter[k],new_names[i]);
          SDDS_CopyString(&(*new_data).orig_parameter[k],parameter_names[i]); 
          (*new_data).parameters++;
        }
        free(new_names[i]);
      }
      free(new_names);
    }
  }
  
  if (edit_array_requests) {
    if ((new_names=process_editnames(array_names,&orig_arrayflags,arrays,
                                     edit_array_request,edit_array_requests,filenumber))) {
      for (i=0;i<arrays;i++)
      {
        if (orig_arrayflags[i]) {
          k=(*new_data).arrays;
          (*new_data).new_array=trealloc((*new_data).new_array,sizeof(char*)*(k+1));
          (*new_data).orig_array=trealloc((*new_data).orig_array,sizeof(char*)*(k+1));
          SDDS_CopyString(&(*new_data).new_array[k],new_names[i]);
          SDDS_CopyString(&(*new_data).orig_array[k],array_names[i]);
          (*new_data).arrays++;
        }
        free(new_names[i]);
      }
      free(new_names);
    }
  }
 
  if (orig_columnflags)
    free(orig_columnflags);
  if (orig_parameterflags)
    free(orig_parameterflags);
  if (orig_arrayflags)
    free(orig_arrayflags);
  for (i=0;i<columns;i++)
    free(column_names[i]);
  free(column_names);
  for (i=0;i<parameters;i++)
    free(parameter_names[i]);
  free(parameter_names);
  for (i=0;i<arrays;i++)
    free(array_names[i]);
  free(array_names);
  
}


char **process_editnames(char **orig_name, long **orig_flags, long orig_names,
                            EDIT_NAME_REQUEST *edit_request, long edit_requests,long filenumber)
{
  long i, j;
  char **new_name, s[1024];
  char *ptr, **editstr;
  char edit_buffer[1024];

  *orig_flags=NULL;
  
  *orig_flags=tmalloc(sizeof(**orig_flags)*orig_names);
  new_name = tmalloc(sizeof(*new_name)*orig_names);
  
  editstr=(char**)malloc(sizeof(*editstr)*edit_requests);
  ptr=malloc(sizeof(char)*256);
  sprintf(s,"%ld",filenumber);
 
  for (i=0;i<edit_requests;i++) {
    SDDS_CopyString(&editstr[i],edit_request[i].edit_string);
    if (strstr(editstr[i],"%%ld"))
      replace_string(ptr,editstr[i],"%%ld","%ld");
    else if (strstr(editstr[i],"%ld")) 
      replace_string(ptr,editstr[i],"%ld",s);
    else 
      continue;
    free(editstr[i]);
    SDDS_CopyString(&editstr[i],ptr);
  }
  free(ptr);
  ptr=NULL;
  for (j=0; j<orig_names; j++) {
    (*orig_flags)[j]=0;
    SDDS_CopyString(new_name+j,orig_name[j]);
    for (i=0; i<edit_requests; i++) {
      ptr = expand_ranges(edit_request[i].match_string);
      free(edit_request[i].match_string);
      edit_request[i].match_string = ptr;
      if (wild_match(new_name[j],edit_request[i].match_string)) {
        strcpy(edit_buffer, new_name[j]);
        if (!edit_string(edit_buffer, editstr[i]))
          SDDS_Bomb("error editing name");
        free(new_name[j]);
        SDDS_CopyString(&new_name[j], edit_buffer);
        (*orig_flags)[j]=1;
      }
    }
  } 
 
  for (i=0;i<edit_requests;i++) 
    free(editstr[i]);
  free(editstr);
  return new_name;
}




long CopyRowToNewColumn(SDDS_DATASET *target, long target_row, SDDS_DATASET *source, long source_row,
                   REFDATA new_data, long columns, char *input2)
{
  long i,j,k,type,size;
  char s[1024];
  
  if (!columns)
    return 1;
  
  for (i=0; i<columns; i++) {
    if ((j=SDDS_GetColumnIndex(source, new_data.orig_column[i]))<0) {
      sprintf(s, "error: column %s not found in file %s\n", new_data.orig_column[i], input2);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      continue;
    }
    if ((k=SDDS_GetColumnIndex(target, new_data.new_column[i]))<0) {
      sprintf(s, "error: column %s not defined in output\n", new_data.new_column[i]);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      continue;
    }
    
    if ((type=SDDS_GetColumnType(target, k))==SDDS_STRING) {
      if (!SDDS_CopyString(((char***)target->data)[k]+target_row, ((char***)source->data)[j][source_row])) {
        SDDS_SetError("Unable to copy row--string copy failed (SDDS_CopyRow)");
        return(0);
      }
    }
    else {
      size = SDDS_type_size[type-1];
      memcpy((char*)target->data[k]+size*target_row, (char*)source->data[j]+size*source_row, size);
    }
   
  }
  return(1);
}

long CopyParametersFromSecondInput(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,REFDATA new_data)
{
  long i,j,k;
  char s[1024];
  
  if (new_data.parameters==0)
    return 1;
  if (new_data.parameters) {
    for (i=0;i<new_data.parameters;i++) {
      if ((j=SDDS_GetParameterIndex(SDDS_source,new_data.orig_parameter[i]))<0) {
        continue;
      }
      
      if ((k=SDDS_GetParameterIndex(SDDS_target,new_data.new_parameter[i]))<0) {
        fprintf(stderr,"Warning, parameter %s not defined in output.\n",new_data.new_parameter[i]);
        continue;
      }
      if (!SDDS_SetParameters(SDDS_target, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, k,
                              SDDS_source->parameter[j], -1)) {
        sprintf(s,  "Unable to copy parameters for parameters %s",new_data.new_parameter[i]);
        SDDS_SetError(s);
        return(0);
      }
    }
  }
  return 1;
}

long CopyArraysFromSecondInput (SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source, 
                                REFDATA new_data)
{
  long i,j,k,m;
  char s[1024];
  
  if (new_data.arrays==0)
    return 1;
  for (i=0;i<new_data.arrays;i++) {
    if ((j=SDDS_GetArrayIndex(SDDS_source,new_data.orig_array[i]))<0) 
      continue;
    if ((k=SDDS_GetArrayIndex(SDDS_target,new_data.new_array[i]))<0) {
      sprintf(s,"Warning, array %s not defined in output.\n",new_data.new_array[i]);
      SDDS_SetError(s);
      continue;
    }
    if (SDDS_source->layout.array_definition[j].type
        !=SDDS_target->layout.array_definition[k].type) {
      SDDS_SetError("Can't copy arrays between different types (SDDS_CopyArrays");
      return 0;
    }
    SDDS_target->array[k].definition = SDDS_target->layout.array_definition+k;
    SDDS_target->array[k].elements = SDDS_source->array[j].elements;
    if (!(SDDS_target->array[k].dimension 
          = (int32_t*)SDDS_Malloc(sizeof(*SDDS_target->array[k].dimension)*
                               SDDS_target->array[k].definition->dimensions)) ||
        !(SDDS_target->array[k].data 
          = SDDS_Realloc(SDDS_target->array[k].data, 
                         SDDS_type_size[SDDS_target->array[k].definition->type-1]*
                         SDDS_target->array[k].elements))) {
      SDDS_SetError("Unable to copy arrays--allocation failure (SDDS_CopyArrays)");
      return(0);
    }
    for (m=0; m<SDDS_target->array[k].definition->dimensions; m++)
      SDDS_target->array[k].dimension[m] = SDDS_source->array[j].dimension[m];
    
    if (SDDS_target->array[k].definition->type!=SDDS_STRING)
      memcpy(SDDS_target->array[k].data, SDDS_source->array[j].data, 
             SDDS_type_size[SDDS_target->array[k].definition->type-1]*SDDS_target->array[k].elements);
    else if (!SDDS_CopyStringArray(SDDS_target->array[k].data, SDDS_source->array[j].data, 
                                   SDDS_target->array[k].elements)) {
      SDDS_SetError("Unable to copy arrays (SDDS_CopyArrays)");
      return(0);
    }
  }
  return 1;
}
                    
