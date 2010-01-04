/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsmxref
 * purpose: creates a SDDS data set from two other data sets
 *          by adding data from one data set to the other
 *          based on matching or filtering of column data columns
 *
 * For example:  suppose file.1 contains the columns (name,x,y) and
 * file.2 contains (name,z,betax,betay).  Then
 *   sddsmxref file.1 file.2 -take=z -match=name file.3 
 * would result in (name,x,y,z) being put in file.3.
 *
 * Michael Borland, 1994
 $Log: sddsmxref.c,v $
 Revision 1.21  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.20  2005/11/04 22:46:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.19  2004/02/17 17:16:19  shang
 added -rename and -editNames option

 Revision 1.18  2002/08/14 17:12:49  soliday
 Added Open License

 Revision 1.17  2001/05/03 20:55:34  soliday
 Standardized usage messages.

 Revision 1.16  2001/01/10 19:35:41  soliday
 Standardized usage message.

 Revision 1.15  2000/04/13 18:03:46  soliday
 Removed invalid prototype.

 Revision 1.14  1999/09/28 15:39:29  soliday
 Added SDDS_Terminate at the end.

 Revision 1.13  1999/07/09 15:08:12  soliday
 Seperated the USAGE string into two strings to fix problem on WIN32

 Revision 1.12  1999/05/25 19:12:32  soliday
 Removed compiler warning on linux.

 Revision 1.11  1999/01/06 19:54:49  borland
 Fixed the version number in the usage message.

 * Revision 1.10  1996/02/12  17:25:24  borland
 * Removed erroneous free'ing of take_column array.
 *
 * Revision 1.9  1996/01/29  22:30:26  borland
 * Added code to avoid attempts to transfer column and parameter definitions
 * when the element already exists.
 *
 * Revision 1.8  1996/01/17  16:05:32  borland
 * Added test for lack of columns in input2, so that parameters and arrays can
 * be transfered even if input2 has no column data.
 *
 * Revision 1.7  1996/01/15  22:35:53  borland
 * Now keeps tabular data in input1 if no columns are being taken from
 * input2, even if input2 has an empty tabular data section.
 *
 * Revision 1.4  1996/01/11  20:34:49  borland
 * Added -fillIn option and supporting code.
 *
 * Revision 1.3  1995/09/06  14:56:49  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "SDDSaps.h"
#include "scan.h"

#define SET_TAKE_COLUMNS 0
#define SET_LEAVE_COLUMNS 1
#define SET_MATCH_COLUMNS 2
#define SET_EQUATE_COLUMNS 3
#define SET_TRANSFER 4
#define SET_REUSE 5
#define SET_IFNOT 6
#define SET_NOWARNINGS 7
#define SET_IFIS 8
#define SET_PIPE 9
#define SET_FILLIN 10
#define SET_RENAME 11
#define SET_EDIT 12
#define N_OPTIONS 13

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
  long parameters;
  long arrays;
} REFDATA;

/*structure for getting editnames */
typedef struct {
  char *match_string, *edit_string;
} EDIT_NAME_REQUEST;

#define COLUMN_MODE 0
#define PARAMETER_MODE 1
#define ARRAY_MODE 2
#define MODES 3
static char *mode_name[MODES] = {
    "column", "parameter", "array",
  } ;


long expandTransferRequests(char ***match, long *matches, 
                            long type, TRANSFER_DEFINITION *transfer, long transfers,
                            SDDS_DATASET *inSet);
void process_newnames(SDDS_DATASET *SDDS_dataset, REFDATA *take_RefData, REFDATA rename_data,
                      EDIT_NAME_REQUEST *edit_column_request, long edit_column_requests,
                      EDIT_NAME_REQUEST *edit_parameter_request, long edit_parameter_requests,
                      EDIT_NAME_REQUEST *edit_array_request,long edit_array_requests);
char **process_editnames(char **orig_name, long **orig_flags, long orig_names,
                         EDIT_NAME_REQUEST *edit_request, long edit_requests);
long CopyRowToNewColumn(SDDS_DATASET *target, long target_row, SDDS_DATASET *source, long source_row,
                        REFDATA new_data, long columns, char *input2);
long CopyArraysFromSecondInput (SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source, 
                                REFDATA new_data);
long CopyParametersFromSecondInput(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,REFDATA new_data);
void free_refdata(REFDATA *refData, long rename);
void free_edit_request(EDIT_NAME_REQUEST *edit_request, long edit_requests);

typedef char *STRING_PAIR[2];

long rows_equate(SDDS_DATASET *SDDS1, long row1, SDDS_DATASET *SDDS2, long row2, 
                 long equate_columns, STRING_PAIR *equate_column);

char *option[N_OPTIONS] = {
  "take", "leave", "match", "equate", "transfer", "reuse", "ifnot",
  "nowarnings", "ifis", "pipe", "fillin","rename","editnames",
} ;
  
char *USAGE1 = "sddsmxref [<input1>] <input2> [<output>] [-pipe[=input][,output]] \n\
[-ifis={column|parameter},<name>[,...]]\n\
[-ifnot={parameter|column|array},<name>[,...]]\n\
[-transfer={parameter|array},<name>[,...]]\n\
[-take=<column-name>[,...]] [-leave=<column-name>[,...]]\n\
[-fillIn]\n\
[-match=<column-name>[=<column-name>][,...]]\n\
[-equate=<column-name>[=<column-name>]] [-reuse[=[rows][,page]]]\n\
[-rename={column|parameter|array},<oldname>=<newname>[,...]]\n\
[-editnames={column|parameter|array},<wildcard-string>,<edit-string>]\n\n\
sddsmxref takes columns, parameters, and arrays from succesive tables\n\
from file <input2> and adds them to successive tables from <input1>.\n\
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
           <input2>.\n\
equate     specifies names of columns to equate between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.\n\
reuse      specifies that rows of <input2> may be reused, i.e., matched\n\
           with more than one row of <input1>.  Also, -reuse=page specifies\n\n\
           that only the first page of <input2> is used.\n\
nowarnings specifies that warning messages should be suppressed.\n\
rename     Specifies new names for entities in the output data set.\n\
           The entities must still be referred to by their old names in \n\
           the other commandline options. \n\
editnames  Specifies creation of new names for entities of the specified \n\
           type with names matching the specified wildcard string. Editing \n\
           is performed using commands reminiscent of emacs keystrokes. \n\
           if -editnames=<entity>{column|parameter|array},wildcard,ei/%ld/ \n\
           is specified, the entity names will be changed to <name>N, N is the \n\
           position of input files in the command line. \n\
Program by Michael Borland. (This is version 4, February 1996.)\n";

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_1, SDDS_2, SDDS_output;
  long i, j, k, i_arg, rows1, rows2, reuse, reusePage;
  SCANNED_ARG *s_arg;
  char s[200], *ptr;
  char **take_column, **leave_column, **output_column;
  STRING_PAIR *match_column, *equate_column;
  long take_columns, leave_columns, match_columns, equate_columns, leave_all_columns;
  int32_t output_columns;
  char *input1, *input2, *output, *match_value;
  long tmpfile_used, retval1, retval2;
  long *row_used, n;
  TRANSFER_DEFINITION *transfer;
  long transfers;
  long warnings, fillIn;
  IFITEM_LIST ifnot_item, ifis_item;
  unsigned long pipeFlags;
  REFDATA rename_data, take_RefData;
  EDIT_NAME_REQUEST *edit_column_request,*edit_parameter_request,*edit_array_request;
  long edit_column_requests,edit_parameter_requests,edit_array_requests;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s\n%s", USAGE1, USAGE2);
    exit(1);
  }
  
  input1 = input2 = output = NULL;
  take_column  = leave_column  = NULL;
  match_column  = equate_column  = NULL;
  take_columns = leave_columns = match_columns = equate_columns = reuse = reusePage = 0;
  tmpfile_used = 0;
  transfer = NULL;
  transfers = 0;
  ifnot_item.items = ifis_item.items = 0;
  warnings = 1;
  pipeFlags = 0;
  fillIn = 0;
  leave_all_columns = 0;
  
  rename_data.columns=rename_data.parameters=rename_data.arrays=0;
  rename_data.new_column=rename_data.orig_column=rename_data.new_parameter
    =rename_data.orig_parameter=rename_data.new_array=rename_data.orig_array=NULL;
  edit_column_request=edit_parameter_request=edit_array_request=NULL;
  edit_column_requests=edit_parameter_requests=edit_array_requests=0; 
  take_RefData.columns=take_RefData.parameters=take_RefData.arrays=0;
  take_RefData.orig_column=take_RefData.new_column=take_RefData.orig_parameter=take_RefData.new_parameter
    =take_RefData.orig_array=take_RefData.new_array=NULL;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_LEAVE_COLUMNS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "%s\n%s", USAGE1, USAGE2);
          exit(1);
        }
        leave_column = trealloc(leave_column, 
                                sizeof(*leave_column)*(leave_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          leave_column[i-1+leave_columns] = s_arg[i_arg].list[i];
        leave_columns += s_arg[i_arg].n_items-1;
        break;
      case SET_TAKE_COLUMNS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "%s\n%s", USAGE1, USAGE2);
          exit(1);
        }
        take_column = trealloc(take_column, 
                               sizeof(*take_column)*(take_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          take_column[i-1+take_columns] = s_arg[i_arg].list[i];
        take_columns += s_arg[i_arg].n_items-1;
        break;
      case SET_MATCH_COLUMNS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "%s\n%s", USAGE1, USAGE2);
          exit(1);
        }
        match_column = trealloc(match_column, 
                                sizeof(*match_column)*(match_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++) {
          if ((ptr=strchr(s_arg[i_arg].list[i], '=')))
            *ptr++ = 0;
          else 
            ptr = s_arg[i_arg].list[i];
          match_column[i-1+match_columns][0] = s_arg[i_arg].list[i];
          match_column[i-1+match_columns][1] = ptr;
        }
        match_columns += s_arg[i_arg].n_items-1;
        break;
      case SET_EQUATE_COLUMNS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -equate syntax");
        equate_column = trealloc(equate_column, 
                                 sizeof(*equate_column)*(equate_columns+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++) {
          if ((ptr=strchr(s_arg[i_arg].list[i], '=')))
            *ptr++ = 0;
          else 
            ptr = s_arg[i_arg].list[i];
          equate_column[i-1+equate_columns][0] = s_arg[i_arg].list[i];
          equate_column[i-1+equate_columns][1] = ptr;
        }
        equate_columns += s_arg[i_arg].n_items-1;
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
      case SET_EDIT: 
        if (s_arg[i_arg].n_items!=4)
          SDDS_Bomb("invalid -editnames syntax");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          edit_column_request =
            trealloc(edit_column_request, sizeof(*edit_column_request)*(edit_column_requests+1));
          SDDS_CopyString(&edit_column_request[edit_column_requests].match_string, s_arg[i_arg].list[2]);
          SDDS_CopyString(&edit_column_request[edit_column_requests].edit_string, s_arg[i_arg].list[3]);
          edit_column_requests++;
          break;
        case PARAMETER_MODE:
          edit_parameter_request =
            trealloc(edit_parameter_request, sizeof(*edit_parameter_request)*(edit_parameter_requests+1));
          SDDS_CopyString(&edit_parameter_request[edit_parameter_requests].match_string, s_arg[i_arg].list[2]);
          SDDS_CopyString(&edit_parameter_request[edit_parameter_requests].edit_string,s_arg[i_arg].list[3]);
          edit_parameter_requests++;
          break;
        case ARRAY_MODE:
          edit_array_request =
            trealloc(edit_array_request, sizeof(*edit_array_request)*(edit_array_requests+1));
          SDDS_CopyString(&edit_array_request[edit_array_requests].match_string, s_arg[i_arg].list[2]);
          SDDS_CopyString(&edit_array_request[edit_array_requests].edit_string,s_arg[i_arg].list[3]);
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
      if (input1==NULL)
        input1 = s_arg[i_arg].list[0];
      else if (input2==NULL)
        input2 = s_arg[i_arg].list[0];
      else if (output==NULL)
        output = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames");
    }
  }
  
  if (pipeFlags&USE_STDIN && input1) {
    if (output)
      SDDS_Bomb("too many filenames (sddsxref)");
    output = input2;
    input2 = input1;
    input1 = NULL;
  }
  processFilenames("sddsmxref", &input1, &output, pipeFlags, !warnings, &tmpfile_used);
  if (!input2)
    SDDS_Bomb("second input file not specified");
  
  if (!SDDS_InitializeInput(&SDDS_1, input1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!check_ifitems(&SDDS_1, &ifnot_item, 0) || !check_ifitems(&SDDS_1, &ifis_item, 1))
    exit(0);
  if (!SDDS_InitializeInput(&SDDS_2, input2)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  /*get parameter and array names from transfer */
  if (transfers) {
    if (!expandTransferRequests(&take_RefData.orig_parameter, &take_RefData.parameters,
                                PARAMETER_TRANSFER, transfer, transfers, 
                                &SDDS_2) ||
        !expandTransferRequests(&take_RefData.orig_array, &take_RefData.arrays, 
                                ARRAY_TRANSFER, transfer, transfers, 
                                &SDDS_2))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (SDDS_ColumnCount(&SDDS_2)) {
    SDDS_SetColumnFlags(&SDDS_2, 1);
    if (take_columns) {
      SDDS_SetColumnFlags(&SDDS_2, 0);
      for (i=0; i<take_columns; i++) {
        if (!has_wildcards(take_column[i]) &&
            SDDS_GetColumnIndex(&SDDS_2, take_column[i])<0) {
          sprintf(s, "error: column %s not found in file %s", take_column[i], input2);
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (!SDDS_SetColumnsOfInterest(&SDDS_2, SDDS_MATCH_STRING, take_column[i], SDDS_OR))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    
    leave_all_columns = 0;
    if (leave_columns==1 && strcmp(leave_column[0], "*")==0)
      leave_all_columns = 1;
    else {
      if (!take_columns)
        SDDS_SetColumnFlags(&SDDS_2, 1);
      for (i=0; i<leave_columns; i++) {
        if (!has_wildcards(leave_column[i]) && SDDS_GetColumnIndex(&SDDS_2, leave_column[i])<0) 
          continue;
        if (!SDDS_SetColumnsOfInterest(&SDDS_2, SDDS_MATCH_STRING, leave_column[i], SDDS_AND|SDDS_NEGATE_MATCH))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (leave_columns) free(leave_column);
      if (take_columns) free(take_column);
   
      /*
      if (!(take_column = SDDS_GetColumnNames(&SDDS_2, &take_columns))) {
        SDDS_SetError("error: no columns selected to take from input file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        } */
      if (!(take_RefData.orig_column=SDDS_GetColumnNames(&SDDS_2, &take_RefData.columns))) {
        SDDS_SetError("error: no columns selected to take from input file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    
    for (i=0; i<match_columns; i++) {
      if ((j=SDDS_GetColumnIndex(&SDDS_1, match_column[i][0]))<0 || SDDS_GetColumnType(&SDDS_1, j)!=SDDS_STRING) {
        sprintf(s, "error: column %s not found or not string type in file %s", match_column[i][0], input1?input1:"stdin");
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if ((j=SDDS_GetColumnIndex(&SDDS_2, match_column[i][1]))<0 || SDDS_GetColumnType(&SDDS_2, j)!=SDDS_STRING) {
        sprintf(s, "error: column %s not found or not string type in file %s", match_column[i][1], input2);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    for (i=0; i<equate_columns; i++) {
      if ((j=SDDS_GetColumnIndex(&SDDS_1, equate_column[i][0]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_1, j))) {
        sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[i][0], input1?input1:"stdin");
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if ((j=SDDS_GetColumnIndex(&SDDS_2, equate_column[i][1]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_2, j))) {
        sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[i][1], input2);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    /*find the new names of taking columns */
    
    /*if (take_columns) {
      for (i=0; i<take_columns; i++)
        if (SDDS_GetColumnIndex(&SDDS_1, take_column[i])>=0) {
          for (j=i; j<take_columns-1; j++)
            take_column[j] = take_column[j+1];
          i--;
          take_columns--;
        }
      if (!take_columns && !leave_all_columns && warnings)
        fprintf(stderr, "warning: there are no columns being taken from %s that are not already in %s\n", 
                input1?input1:"stdin", input2);
      if (!take_columns)
        leave_all_columns = 1;
    } */
  }
  else {
    take_columns = 0;
    take_RefData.columns=0;
    leave_all_columns=1;
  }    
  if (!take_RefData.columns && !leave_all_columns && warnings)
    fprintf(stderr, "warning: there are no columns being taken from %s that are not already in %s\n", input1?input1:"stdin", input2);
  /*
  if (!leave_all_columns && !take_RefData.columns && !(take_RefData.orig_column = SDDS_GetColumnNames(&SDDS_2, &take_columns))) {
    sprintf(s, "Problem getting list of columns names for file %s", input2);
    SDDS_SetError(s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  } */
  if (leave_all_columns) {
    take_RefData.columns=0;
    take_columns = 0;
  }
  
  if (output && pipeFlags&USE_STDOUT)
    SDDS_Bomb("too many filenames with -pipe option");
  if (!output && !(pipeFlags&USE_STDOUT)) {
    if (warnings)
      fprintf(stderr, "warning: existing file %s will be replaced (sddsmxref)\n", input1?input1:"stdin");
    tmpfile_used = 1;
    cp_str(&output, tmpname(NULL));
  }
  if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_1, output, "w")) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  /*process new names for take ref data */
  process_newnames(&SDDS_2,&take_RefData,rename_data,edit_column_request,edit_column_requests,
                   edit_parameter_request,edit_parameter_requests,
                   edit_array_request,edit_array_requests);
  
  for (i=0; i<take_RefData.columns; i++) {
    if (SDDS_GetColumnIndex(&SDDS_output, take_RefData.new_column[i])>=0) {
      if (warnings)
        fprintf(stderr,"warning, column %s already exist in the first input file, no data will be taken from column %s of second input file.\n",take_RefData.new_column[i],take_RefData.orig_column[i]);
      free(take_RefData.new_column[i]);
      free(take_RefData.orig_column[i]); 
      for (j=i; j<take_RefData.columns-1; j++) {
        take_RefData.new_column[j] =take_RefData.new_column[j+1];
        take_RefData.orig_column[j] = take_RefData.orig_column[j+1];
      }
      
      take_RefData.columns -=1;
      i--;
      if (take_RefData.columns==0)
        break;
    } 
    else {
      if (!SDDS_TransferColumnDefinition(&SDDS_output, &SDDS_2, take_RefData.orig_column[i], 
                                         take_RefData.new_column[i]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!(output_column = (char**)SDDS_GetColumnNames(&SDDS_output, &output_columns)) || output_columns==0) {
    SDDS_SetError("Problem getting output column names");
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  for (i=0; i<take_RefData.parameters; i++) {
    if (SDDS_GetParameterIndex(&SDDS_output, take_RefData.new_parameter[i])>=0) {
      free(take_RefData.orig_parameter[i]);
      free(take_RefData.new_parameter[i]);
      for (j=i; j<take_RefData.parameters-1; j++)
        take_RefData.orig_parameter[j]=take_RefData.orig_parameter[j+1];
      take_RefData.parameters -= 1;
      i--;
      if (take_RefData.parameters==0 )
        break;
    } else {
      if (!SDDS_TransferParameterDefinition(&SDDS_output, &SDDS_2, take_RefData.orig_parameter[i], 
                                            take_RefData.new_parameter[i]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }  
  }
  
  for (i=0; i<take_RefData.arrays; i++) {
    if (SDDS_GetArrayIndex(&SDDS_output, take_RefData.new_array[i])<0) {
      free(take_RefData.orig_array[i]);
      free(take_RefData.new_array[i]);
      for (j=i; j<take_RefData.arrays-1; j++)
        take_RefData.orig_array[j]=take_RefData.orig_array[j+1];
      take_RefData.arrays -= 1;
      i--;
      if (take_RefData.arrays==0 )
        break;
    } else {
      if (!SDDS_TransferArrayDefinition(&SDDS_output, &SDDS_2, take_RefData.orig_array[i], 
                                        take_RefData.new_array[i]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!SDDS_WriteLayout(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!take_RefData.columns && !leave_all_columns && warnings)
    fprintf(stderr, "warning: there are no columns being taken from %s that are not already in %s\n", 
            input2, input1?input1:"stdin");
  if (output_columns) {
    for (i=0;i<output_columns;i++)
      free(output_column[i]);
    free(output_column);
  } 
  row_used = NULL;
  while ((retval1=SDDS_ReadPage(&SDDS_1))>0) {
    if (!reusePage) {
      if ((retval2=SDDS_ReadPage(&SDDS_2))<=0) {
        fprintf(stderr, "warning: <input2> ends before <input1>\n");
        break;
      }
    }
    else {
      if (retval1==1 && (retval2=SDDS_ReadPage(&SDDS_2))<=0)
        SDDS_Bomb("<input2> has no data");
      SDDS_SetRowFlags(&SDDS_2, 1);
    }
    if (take_RefData.columns && (!SDDS_SetColumnFlags(&SDDS_2, 0) ||
                                  !SDDS_SetColumnsOfInterest(&SDDS_2, SDDS_NAME_ARRAY, 
                                                             take_RefData.columns, take_RefData.orig_column)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    rows1 = SDDS_CountRowsOfInterest(&SDDS_1);
    if ((rows2 = SDDS_CountRowsOfInterest(&SDDS_2))) {
      row_used = SDDS_Realloc(row_used, sizeof(*row_used)*rows2);
      SDDS_ZeroMemory(row_used, rows2*sizeof(*row_used));
    }
    if (!SDDS_StartPage(&SDDS_output, rows1)) {
      SDDS_SetError("Problem starting output table");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (fillIn && !SDDS_ClearPage(&SDDS_output)) {
      SDDS_SetError("Problem clearing output table");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    /*copy parameters and arrays */
    if (!CopyParametersFromSecondInput(&SDDS_output, &SDDS_2,take_RefData)) {
      SDDS_SetError("Problem copying parameter from second input file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!CopyArraysFromSecondInput(&SDDS_output, &SDDS_2,take_RefData)) {
      SDDS_SetError("Problem copying parameter from second input file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_CopyParameters(&SDDS_output, &SDDS_1) || !SDDS_CopyArrays(&SDDS_output, &SDDS_1)) {
      SDDS_SetError("Problem copying parameter or array data from first input file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    for (j=0; j<rows1; j++) {
      if (!SDDS_CopyRowDirect(&SDDS_output, j, &SDDS_1, j)) {
        sprintf(s, "Problem copying row %ld of first data set", j);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      SDDS_output.row_flag[j] = 1;
      if (!match_columns && !equate_columns && !leave_all_columns) {
        if (j>=rows2) {
          if (warnings)
            fprintf(stderr, "Warning (1): No match for row %ld (value %s)\n", j, match_value);
          SDDS_output.row_flag[j] = 0;
          continue;
        }
        if (!CopyRowToNewColumn(&SDDS_output,j, &SDDS_2, j,take_RefData,
                                take_RefData.columns,input2)) {
          fprintf(stderr,"error in copying data to output!\n");
          exit(1);
        }
        continue;
      }
      if (!leave_all_columns) {
        SDDS_SetRowFlags(&SDDS_2, 1);
        for (i=0; i<match_columns; i++) {
          if (!SDDS_GetValue(&SDDS_1, match_column[i][0], j, &match_value)) {
            sprintf(s, "Problem getting column %s from file %s", match_column[i][0], input1?input1:"stdin");
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (SDDS_MatchRowsOfInterest(&SDDS_2, match_column[i][1], match_value, SDDS_AND)<0) {
            sprintf(s, "Problem setting rows of interest for column %s", match_column[i][1]);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          free(match_value);
          
        }
        if (!(n=SDDS_CountRowsOfInterest(&SDDS_2))) {
          if (warnings)
            fprintf(stderr, "Warning (2): No match for row %ld\n", j);
          SDDS_output.row_flag[j] = 0;
          continue;
        }
        i = -1;  /* counter for rows-of-interest in file 2 */
        for (k=0; k<rows2; k++) {
          if (!SDDS_2.row_flag[k])
            continue;
          i++;
          if (!row_used[k] && rows_equate(&SDDS_1, j, &SDDS_2, k, equate_columns, equate_column)) {
            row_used[k] = reuse?0:1;
            if (!CopyRowToNewColumn(&SDDS_output,j, &SDDS_2, k,take_RefData,
                                    take_RefData.columns,input2)) {
              fprintf(stderr,"error in copying data to output!\n");
              exit(1);
            }
            break;
          }
        }
        if (k==rows2) {
          if (warnings)
            fprintf(stderr, "Warning (3): No match for row %ld\n", j);
          if (!fillIn)
            SDDS_output.row_flag[j] = 0;
          continue;
        }
      }
    }
    if (!SDDS_WritePage(&SDDS_output)) {
      SDDS_SetError("Problem writing data to output file");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  
  if (!SDDS_Terminate(&SDDS_1) || !SDDS_Terminate(&SDDS_2) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (tmpfile_used && !replaceFileAndBackUp(input1, output))
    exit(1);
  free_scanargs(&s_arg,argc);
  if (row_used) free(row_used);
  
  free_refdata(&take_RefData,0);
  free_refdata(&rename_data,1); 
  
  free_edit_request(edit_column_request,edit_column_requests);
  free_edit_request(edit_parameter_request,edit_parameter_requests);
  free_edit_request(edit_array_request,edit_array_requests);
  if (match_columns) free(match_column);
  if (equate_columns) free(equate_column);
  
  return 0;
}

void free_refdata(REFDATA *refData, long rename)
{
  long i;
  if (!rename) {
    for (i=0;i<refData->columns;i++) {
      free(refData->orig_column[i]);
      free(refData->new_column[i]);   
    }
    for (i=0;i<refData->parameters;i++) {
      free(refData->orig_parameter[i]);
      free(refData->new_parameter[i]);   
    }
    for (i=0;i<refData->arrays;i++) {
      free(refData->orig_array[i]);
      free(refData->new_array[i]);   
    }
  }
  if (refData->columns) {
    free(refData->orig_column);
    free(refData->new_column);   
  }
  if (refData->parameters) {
    free(refData->orig_parameter);
    free(refData->new_parameter);   
  }
}

                   
long rows_equate(SDDS_DATASET *SDDS1, long row1, SDDS_DATASET *SDDS2, long row2, 
                 long equate_columns, STRING_PAIR *equate_column)
{
  char *data1, *data2;
  long index1, index2, size, type, i;
  char s[SDDS_MAXLINE];
  index2 = 0;
  for (i=0; i<equate_columns; i++) {
    if ((index1=SDDS_GetColumnIndex(SDDS1, equate_column[i][0]))<0 ||
        (index2=SDDS_GetColumnIndex(SDDS2, equate_column[i][1]))<0 ) {
      SDDS_SetError("Problem equating rows");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if ((type = SDDS_GetColumnType(SDDS1, index1))!=SDDS_GetColumnType(SDDS2, index2)) {
      sprintf(s, "Problem equating rows--types don't match for column %s=%s", 
              equate_column[i][0], equate_column[i][1]);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    size = SDDS_GetTypeSize(type);
    data1 = (char*)SDDS1->data[index1]+size*row1;
    data2 = (char*)SDDS2->data[index2]+size*row2;
    if (memcmp(data1, data2, size)!=0)
      return(0);
  }
  return(1);
}

long expandTransferRequests(char ***match, long *matches, 
                            long type, TRANSFER_DEFINITION *transfer, long transfers,
                            SDDS_DATASET *inSet)
{
  long i, first;
  int32_t (*matchRoutine)(SDDS_DATASET *SDDS_dataset, char ***nameReturn, int32_t matchMode, int32_t typeMode, ... );
  
  *matches = 0;
  *match = NULL;
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


void process_newnames(SDDS_DATASET *SDDS_dataset, REFDATA *take_RefData, REFDATA rename_data,
                      EDIT_NAME_REQUEST *edit_column_request, long edit_column_requests,
                      EDIT_NAME_REQUEST *edit_parameter_request, long edit_parameter_requests,
                      EDIT_NAME_REQUEST *edit_array_request,long edit_array_requests)
{
  long i, k=0, *orig_columnflags;
  long *orig_parameterflags,*orig_arrayflags;
  char **column_names,**parameter_names,**array_names,**new_names;
  int32_t columns,parameters,arrays;
  
  columns=parameters=arrays=0;
  column_names=parameter_names=array_names=new_names=NULL;
  orig_columnflags=orig_parameterflags=orig_arrayflags=NULL;
  

  if (take_RefData->columns)
    take_RefData->new_column=(char**)malloc(sizeof(*(take_RefData->new_column))*take_RefData->columns);
  if (take_RefData->parameters)
    take_RefData->new_parameter=(char**)malloc(sizeof(*(take_RefData->new_parameter))*take_RefData->parameters);
  if (take_RefData->arrays)
    take_RefData->new_array=(char**)malloc(sizeof(*(take_RefData->new_array))*take_RefData->arrays);

  /*transfer renames to take_RefData*/
  for (i=0;i<take_RefData->columns;i++) 
    if ((k=match_string(take_RefData->orig_column[i],rename_data.orig_column,rename_data.columns,EXACT_MATCH))>=0)
      SDDS_CopyString(&take_RefData->new_column[i],rename_data.new_column[k]);
    else
      SDDS_CopyString(&take_RefData->new_column[i],take_RefData->orig_column[i]);
  
  for (i=0;i<take_RefData->parameters;i++) 
    if ((k=match_string(take_RefData->orig_parameter[i],rename_data.orig_parameter,rename_data.parameters,EXACT_MATCH))>=0)
      SDDS_CopyString(&take_RefData->new_parameter[i],rename_data.new_parameter[k]);
    else
      SDDS_CopyString(&take_RefData->new_parameter[i],take_RefData->orig_parameter[i]);
  for (i=0;i<take_RefData->arrays;i++) 
    if ((k=match_string(take_RefData->orig_array[i],rename_data.orig_array,rename_data.arrays,EXACT_MATCH))>=0)
      SDDS_CopyString(&take_RefData->new_array[i],rename_data.new_array[k]);
    else
      SDDS_CopyString(&take_RefData->new_array[i],take_RefData->orig_array[i]);
  
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
                                     edit_column_request,edit_column_requests))) {
      for (i=0;i<columns;i++)
      {
        if (orig_columnflags[i]) {
          if ((k=match_string(column_names[i],take_RefData->orig_column,take_RefData->columns,EXACT_MATCH))>=0)
            SDDS_CopyString(&take_RefData->new_column[k],new_names[i]);
        }
        free(new_names[i]);
      }
      free(new_names);
    }
  }
  if (edit_parameter_requests) {
    if ((new_names=process_editnames(parameter_names,&orig_parameterflags,parameters,
                                     edit_parameter_request,edit_parameter_requests))) {
      for (i=0;i<parameters;i++)
      {
        if (orig_parameterflags[i]) {
          if ((k=match_string(parameter_names[i],take_RefData->orig_parameter,take_RefData->parameters,EXACT_MATCH))>=0)
            SDDS_CopyString(&take_RefData->new_parameter[k],new_names[i]);
        }
        free(new_names[i]);
      }
      free(new_names);
    }
  }
  
  if (edit_array_requests) {
    if ((new_names=process_editnames(array_names,&orig_arrayflags,arrays,
                                     edit_array_request,edit_array_requests))) {
      for (i=0;i<arrays;i++)
      {
        if (orig_arrayflags[i]) {
          if ((k=match_string(array_names[i],take_RefData->orig_array,take_RefData->arrays,EXACT_MATCH))>=0)
            SDDS_CopyString(&take_RefData->new_array[k],new_names[i]);
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
                         EDIT_NAME_REQUEST *edit_request, long edit_requests)
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
  sprintf(s,"%" PRId32,2);
  
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

long CopyParametersFromSecondInput(SDDS_DATASET *SDDS_target, SDDS_DATASET *SDDS_source,REFDATA new_data)
{
  long i,j,k;
  char s[1024];
  
  if (new_data.parameters==0)
    return 1;
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

void free_edit_request(EDIT_NAME_REQUEST *edit_request, long edit_requests)
{
  long i;
  if (edit_requests) {
    for (i=0;i<edit_requests;i++) {
      free(edit_request[i].match_string);
      free(edit_request[i].edit_string);
    }
    free(edit_request);
  }
}
