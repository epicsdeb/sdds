/*************************************************************************\
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 * National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 * Operator of Los Alamos National Laboratory.
 * This file is distributed subject to a Software License Agreement found
 * in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsconvert
 * purpose: copy a SDDS file and allow ascii/binary switching
 *
 * Michael Borland, 1993
 $Log: sddsconvert.c,v $
 Revision 1.20  2007/06/08 21:51:15  soliday
 Removed call to free_scanargs because string arguments currently are not
 copied with SDDS_CopyString which free_scanargs requires.

 Revision 1.19  2007/03/27 21:30:56  soliday
 Cleaned up the code.

 Revision 1.18  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.17  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2003/09/02 20:36:37  soliday
 Cleaned up for WIN32

 Revision 1.15  2003/04/29 17:23:41  borland
 Added -rowlimit option.

 Revision 1.14  2002/08/14 17:12:42  soliday
 Added Open License

 Revision 1.13  2002/05/07 20:14:40  shang
 freed memory allocated for sdds array

 Revision 1.12  2001/03/26 19:16:29  soliday
 Added options -removePages and -keepPages.

 Revision 1.11  2001/01/10 19:35:34  soliday
 Standardized usage message.

 Revision 1.10  1999/05/25 19:07:08  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/06 19:54:40  borland
 Fixed the version number in the usage message.

 * Revision 1.8  1996/06/11  22:12:10  borland
 * Added debugging statements to help with performance evaluation.
 *
 * Revision 1.7  1996/04/09  04:56:10  borland
 * Now use SDDS_GetInternalColumn for better performance.
 *
 * Revision 1.6  1995/10/31  02:38:10  borland
 * Fixed -toPage implementation to exit just after processing the indicated page,
 * rather than after reading the next page.
 *
 * Revision 1.5  1995/10/05  17:31:14  borland
 * Added initialization for noWarnings and ascii_output variables.
 *
 * Revision 1.4  1995/09/12  03:10:35  borland
 * Added -acceptAllNames option to give backward compatibility for "illegal" element names.
 *
 * Revision 1.3  1995/09/06  14:56:16  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define COLUMN_MODE 0
#define PARAMETER_MODE 1
#define ARRAY_MODE 2
#define MODES 3
static char *mode_name[MODES] = {
  "column", "parameter", "array",
} ;

#define SET_BINARY 0
#define SET_ASCII 1
#define SET_DELETE 2
#define SET_RETAIN 3
#define SET_RENAME 4
#define SET_DESCRIPTION 5
/* -table is retained for backward compatibility: */
#define SET_TABLE 6
#define SET_EDIT_NAMES 7
#define SET_LINES_PER_ROW 8
#define SET_NOWARNINGS 9
#define SET_RECOVER 10
#define SET_PIPE 11
#define SET_FROMPAGE 12
#define SET_TOPAGE 13
#define SET_ACCEPT_ALL_NAMES 14
#define SET_REMOVEPAGES 15
#define SET_KEEPPAGES 16
#define SET_ROWLIMIT 17
#define N_OPTIONS 18

char *option[N_OPTIONS] = {
  "binary", "ascii", "delete", "retain", "rename", "description",
  "table", "editnames", "linesperrow", "nowarnings", "recover", "pipe",
  "frompage", "topage", "acceptallnames", "removePages", "keepPages",
  "rowlimit",
} ;

char *USAGE = "sddsconvert [<source-file>] [<target-file>]\n\
[-pipe=[input][,output]]\n\
[-binary] [-ascii] [-description=<text>,<contents>]\n\
[-fromPage=<pageNumber>] [-toPage=<pageNumber>]\n\
[-removePages=<listOfNumbers>] [-keepPages=<listOfNumbers>]\n\
[-linesperrow=<integer>] [-nowarnings] [-recover[=clip]]\n\
[-delete={column|parameter|array},<matching-string>[,...]]\n\
[-retain={column|parameter|array},<matching-string>[,...]]\n\
[-rename={column|parameter|array},<oldname>=<newname>[,...]]\n\
[-editnames={column|parameter|array},<wildcard-string>,<edit-string>]\n\
[-acceptAllNames] [-rowlimit=<number>]\n\n\
sddsconvert converts SDDS files between ASCII and binary, and allows wildcard-based\
 filtering-out of unwanted columns and/or rows.\n\
Any column or parameter matched by a deletion string is deleted unless it is matched by a retention string.\n\
The -acceptAllNames option may be used to force the SDDS library to accept\n\
element names that have spaces and other normally unacceptable characters.\n\n\
Program by Michael Borland. (This is version 6, April 2003.)\n";

typedef char *STRING_PAIR[2];

typedef struct {
  char *match_string, *edit_string;
} EDIT_NAME_REQUEST;

char **process_name_options(char **orig_name, long **orig_flag, long orig_names, 
                            char **delete, long deletes,
                            char **retain, long retains,
                            STRING_PAIR *rename, long renames,
                            EDIT_NAME_REQUEST *edit_request, long edit_requests);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_dataset, SDDS_orig;
  long i, i_arg;
  SCANNED_ARG *s_arg;
  char *description_text, *description_contents;
  long first, tmpfile_used, noWarnings;
  char *input, *output, *ptr, *buffer;
  long ascii_output, binary_output, recover, recovered;
  unsigned long pipeFlags;

  char **retain_column, **delete_column;
  long retain_columns, delete_columns, rename_columns, edit_column_requests;
  EDIT_NAME_REQUEST *edit_column_request;
  STRING_PAIR *rename_column;
  char **orig_column_name, **new_column_name;
  long *orig_column_flag=NULL;
  int32_t orig_column_names;

  char **retain_parameter, **delete_parameter;
  long retain_parameters, delete_parameters, rename_parameters, edit_parameter_requests;
  EDIT_NAME_REQUEST *edit_parameter_request;
  STRING_PAIR *rename_parameter;
  char **orig_parameter_name, **new_parameter_name;
  long *orig_parameter_flag=NULL;
  int32_t orig_parameter_names;

  char **retain_array, **delete_array;
  long retain_arrays, delete_arrays, rename_arrays, edit_array_requests;
  EDIT_NAME_REQUEST *edit_array_request;
  STRING_PAIR *rename_array;
  char **orig_array_name, **new_array_name;
  long *orig_array_flag=NULL;
  int32_t orig_array_names;

  long lines_per_row, pageNumber=0, fromPage, toPage;
  long rows, max_size, output_columns;
  long *removePage, *keepPage;
  long removePages, keepPages, keep;
  long rowLimit;
    
  new_column_name = new_parameter_name = new_array_name = NULL;    

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s\n", USAGE);
    return(1);
  }
  input = output = NULL;
  ascii_output = binary_output = noWarnings = 0;

  retain_column = delete_column = NULL;
  retain_columns = delete_columns = rename_columns = edit_column_requests = 0;
  rename_column = NULL;
  edit_column_request = NULL;

  retain_parameter = delete_parameter = NULL;
  retain_parameters = delete_parameters = rename_parameters = edit_parameter_requests = 0;
  rename_parameter = NULL;
  edit_parameter_request = NULL;

  retain_array = delete_array = NULL;
  retain_arrays = delete_arrays = rename_arrays = edit_array_requests = 0;
  rename_array = NULL;
  edit_array_request = NULL;

  tmpfile_used = 0;
  description_text = description_contents = NULL;
  fromPage = toPage = recover = 0;
  removePage = keepPage = NULL;
  removePages = keepPages = 0;
  lines_per_row = 1;
  pipeFlags = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_BINARY:
        binary_output = 1;
        ascii_output = 0;
        break;
      case SET_ASCII:
        ascii_output = 1;
        binary_output = 0;
        break;
      case SET_DELETE:
        if (s_arg[i_arg].n_items<3) {
          fprintf(stderr, "Error (%s): invalid -delete syntax\n", argv[0]);
          return(1);
        }
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          delete_column = trealloc(delete_column, 
                                   sizeof(*delete_column)*(delete_columns+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_column[i-2+delete_columns] = s_arg[i_arg].list[i];
          delete_columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          delete_parameter = trealloc(delete_parameter, 
                                      sizeof(*delete_parameter)*(delete_parameters+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_parameter[i-2+delete_parameters] = s_arg[i_arg].list[i];
          delete_parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          delete_array = trealloc(delete_array, 
                                  sizeof(*delete_array)*(delete_arrays+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_array[i-2+delete_arrays] = s_arg[i_arg].list[i];
          delete_arrays += s_arg[i_arg].n_items-2;
          break;
        default:
          fprintf(stderr, "Error (%s): invalid -delete syntax: specify column or parameter keyword\n", argv[0]);
          return(1);
        }
        break;
      case SET_RETAIN:
        if (s_arg[i_arg].n_items<3) {
          fprintf(stderr, "Error (%s): invalid -retain syntax\n", argv[0]);
          return(1);
        }
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          retain_column = trealloc(retain_column, 
                                   sizeof(*retain_column)*(retain_columns+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_column[i-2+retain_columns] = s_arg[i_arg].list[i];
          retain_columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          retain_parameter = trealloc(retain_parameter, 
                                      sizeof(*retain_parameter)*(retain_parameters+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_parameter[i-2+retain_parameters] = s_arg[i_arg].list[i];
          retain_parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          retain_array = trealloc(retain_array, 
                                  sizeof(*retain_array)*(retain_arrays+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_array[i-2+retain_arrays] = s_arg[i_arg].list[i];
          retain_arrays += s_arg[i_arg].n_items-2;
          break;
        default:
          fprintf(stderr, "Error (%s): invalid -retain syntax: specify column or parameter keyword\n", argv[0]);
          return(1);
        }
        break;
      case SET_RENAME:
        if (s_arg[i_arg].n_items<3) {
          fprintf(stderr, "Error (%s): invalid -rename syntax\n", argv[0]);
          return(1);
        }
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          rename_column = trealloc(rename_column, 
                                   sizeof(*rename_column)*(rename_columns+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '='))) {
              fprintf(stderr, "Error (%s): invalid -rename syntax\n", argv[0]);
              return(1);
            }
            *ptr++ = 0;
            rename_column[i-2+rename_columns][0] = s_arg[i_arg].list[i];
            rename_column[i-2+rename_columns][1] = ptr;
          }
          rename_columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          rename_parameter = trealloc(rename_parameter, 
                                      sizeof(*rename_parameter)*(rename_parameters+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '='))) {
              fprintf(stderr, "Error (%s): invalid -rename syntax\n", argv[0]);
              return(1);
            }
            *ptr++ = 0;
            rename_parameter[i-2+rename_parameters][0] = s_arg[i_arg].list[i];
            rename_parameter[i-2+rename_parameters][1] = ptr;
          }
          rename_parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          rename_array = trealloc(rename_array, 
                                  sizeof(*rename_array)*(rename_arrays+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++) {
            if (!(ptr=strchr(s_arg[i_arg].list[i], '='))) {
              fprintf(stderr, "Error (%s): invalid -rename syntax\n", argv[0]);
              return(1);
            }
            *ptr++ = 0;
            rename_array[i-2+rename_arrays][0] = s_arg[i_arg].list[i];
            rename_array[i-2+rename_arrays][1] = ptr;
          }
          rename_arrays += s_arg[i_arg].n_items-2;
          break;
        default:
          fprintf(stderr, "Error (%s): invalid -rename syntax: specify column or parameter keyword\n", argv[0]);
          return(1);
        }
        break;
      case SET_DESCRIPTION:
        if (s_arg[i_arg].n_items!=3) {
          fprintf(stderr, "Error (%s): invalid -description syntax\n", argv[0]);
          return(1);
        }
        description_text     = s_arg[i_arg].list[1];
        description_contents = s_arg[i_arg].list[2];
        break;
      case SET_TABLE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (%s): invalid -table syntax\n", argv[0]);
          return(1);
        }
        if (pageNumber!=0) {
          fprintf(stderr, "Error (%s): invalid syntax: specify -table once only\n", argv[0]);
          return(1);
        }
        if (sscanf(s_arg[i_arg].list[1], "%ld", &pageNumber)!=1 || pageNumber<=0) {
          fprintf(stderr, "Error (%s): invalid -table syntax or value\n", argv[0]);
          return(1);
        }
        fromPage = toPage = pageNumber;
        break;
      case SET_FROMPAGE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (%s): invalid -fromPage syntax\n", argv[0]);
          return(1);
        }
        if (fromPage!=0) {
          fprintf(stderr, "Error (%s): invalid syntax: specify -fromPage once only\n", argv[0]);
          return(1);
        }
        if (sscanf(s_arg[i_arg].list[1], "%ld", &fromPage)!=1 || fromPage<=0) {
          fprintf(stderr, "Error (%s): invalid -fromPage syntax or value\n", argv[0]);
          return(1);
        }
        break;
      case SET_TOPAGE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (%s): invalid -toPage syntax\n", argv[0]);
          return(1);
        }
        if (toPage!=0) {
          fprintf(stderr, "Error (%s): invalid syntax: specify -toPage once only\n", argv[0]);
          return(1);
        }
        if (sscanf(s_arg[i_arg].list[1], "%ld", &toPage)!=1 || toPage<=0) {
          fprintf(stderr, "Error (%s): invalid -toPage syntax or value\n", argv[0]);
          return(1);
        }
        break;
      case SET_REMOVEPAGES:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (%s): invalid -removePages syntax\n", argv[0]);
          return(1);
        }
        if (removePage != NULL) {
          fprintf(stderr, "Error (%s): invalid syntax: specify -removePages once only\n", argv[0]);
          return(1);
        }
        removePages = s_arg[i_arg].n_items - 1;
        removePage = trealloc(removePage, sizeof(*removePage)*(removePages));
        for (i=0; i<removePages; i++) {
          if (sscanf(s_arg[i_arg].list[i+1], "%ld", &removePage[i])!=1 || removePage[i]<=0) {
            fprintf(stderr, "Error (%s): invalid -removePages syntax or value\n", argv[0]);
            return(1);
          }
        }
        break;
      case SET_KEEPPAGES:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (%s): invalid -keepPages syntax\n", argv[0]);
          return(1);
        }
        if (keepPage != NULL) {
          fprintf(stderr, "Error (%s): invalid syntax: specify -keepPages once only\n", argv[0]);
          return(1);
        }
        keepPages = s_arg[i_arg].n_items - 1;
        keepPage = trealloc(keepPage, sizeof(*keepPage)*(keepPages));
        for (i=0; i<keepPages; i++) {
          if (sscanf(s_arg[i_arg].list[i+1], "%ld", &keepPage[i])!=1 || keepPage[i]<=0) {
            fprintf(stderr, "Error (%s): invalid -keepPages syntax or value\n", argv[0]);
            return(1);
          }
        }
        break;
      case SET_EDIT_NAMES: 
        if (s_arg[i_arg].n_items<4) {
          fprintf(stderr, "Error (%s): invalid -editnames syntax\n", argv[0]);
          return(1);
        }
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
          fprintf(stderr, "Error (%s): invalid -editnames syntax: specify column or parameter keyword\n", argv[0]);
          return(1);
        }
        break;
      case SET_LINES_PER_ROW:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &lines_per_row)!=1 ||
            lines_per_row<=0) {
          fprintf(stderr, "Error (%s): invalid -linesperrow syntax\n", argv[0]);
          return(1);
        }
        break;
      case SET_NOWARNINGS:
        if (s_arg[i_arg].n_items!=1) {
          fprintf(stderr, "Error (%s): invalid -nowarnings syntax\n", argv[0]);
          return(1);
        }
        noWarnings = 1;
        break;
      case SET_RECOVER:
        recover = 1;
        if (s_arg[i_arg].n_items!=1) {
          recover = 2;
          if (s_arg[i_arg].n_items>2 ||
              strncmp(s_arg[i_arg].list[1], "clip", strlen(s_arg[i_arg].list[1]))!=0) {
            fprintf(stderr, "Error (%s): invalid -recover syntax\n", argv[0]);
            return(1);
          }
        }
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags)) {
          fprintf(stderr, "Error (%s): invalid -pipe syntax\n", argv[0]);
          return(1);
        }
        break;
      case SET_ACCEPT_ALL_NAMES:
        SDDS_SetNameValidityFlags(SDDS_ALLOW_ANY_NAME);
        break;
      case SET_ROWLIMIT:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &rowLimit)!=1 ||
            !rowLimit<0) {
          fprintf(stderr, "Error (%s): invalid -rowLimit syntax\n", argv[0]);
          return(1);
        }
        SDDS_SetRowLimit(rowLimit);
        break;
      default:
        fprintf(stderr, "Error (%s): unknown switch: %s\n", argv[0], s_arg[i_arg].list[0]);
        return(1);
      }
    }
    else {
      if (input==NULL)
        input = s_arg[i_arg].list[0];
      else if (output==NULL)
        output = s_arg[i_arg].list[0];
      else {
        fprintf(stderr, "Error (%s): too many filenames\n", argv[0]);
        return(1);
      }
    }
  }

  if (fromPage && toPage && fromPage>toPage) {
    fprintf(stderr, "Error (%s): invalid -fromPage and -toPage\n", argv[0]);
    return(1);
  }

  processFilenames("sddsconvert", &input, &output, pipeFlags, noWarnings, &tmpfile_used);

#ifdef DEBUG
  fprintf(stderr, "Initializing input and output files.\n");
#endif
  if (!SDDS_InitializeInput(&SDDS_orig, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!description_text) {
    SDDS_GetDescription(&SDDS_orig, &description_text, &description_contents);
  }
  if (!SDDS_InitializeOutput(&SDDS_dataset, 
                             ascii_output?SDDS_ASCII:(binary_output?SDDS_BINARY:SDDS_orig.layout.data_mode.mode), 
                             lines_per_row, description_text, description_contents, output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);

#ifdef DEBUG
  fprintf(stderr, "Getting column, parameter, and array names from input file.\n");
#endif
  if (!(orig_column_name = SDDS_GetColumnNames(&SDDS_orig, &orig_column_names))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (orig_column_names && 
      !(new_column_name=process_name_options(orig_column_name, &orig_column_flag, orig_column_names,
                                             delete_column, delete_columns,
                                             retain_column, retain_columns,
                                             rename_column, rename_columns,
                                             edit_column_request, edit_column_requests))) {
    return(1);
  }

  if (!(orig_parameter_name = SDDS_GetParameterNames(&SDDS_orig, &orig_parameter_names))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }

#ifdef DEBUG
  fprintf(stderr, "processing name options\n");
#endif
  if (orig_parameter_names && 
      !(new_parameter_name=process_name_options(orig_parameter_name, &orig_parameter_flag, orig_parameter_names,
                                                delete_parameter, delete_parameters,
                                                retain_parameter, retain_parameters,
                                                rename_parameter, rename_parameters,
                                                edit_parameter_request, edit_parameter_requests))) {
    return(1);
  }

  if (!(orig_array_name = SDDS_GetArrayNames(&SDDS_orig, &orig_array_names))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (orig_array_names && 
      !(new_array_name=process_name_options(orig_array_name, &orig_array_flag, orig_array_names,
                                            delete_array, delete_arrays,
                                            retain_array, retain_arrays,
                                            rename_array, rename_arrays,
                                            edit_array_request, edit_array_requests))) {
    return(1);
  }

#ifdef DEBUG
  for (i=0; i<orig_parameter_names; i++) {
    fprintf(stderr, "parameter %s goes to %s\n",
            orig_parameter_name[i],
            orig_parameter_flag[i]?new_parameter_name[i]:"<deleted>");
  }
  for (i=0; i<orig_array_names; i++) {
    fprintf(stderr, "array %s goes to %s\n",
            orig_array_name[i],
            orig_array_flag[i]?new_array_name[i]:"<deleted>");
  }
  for (i=0; i<orig_column_names; i++) {
    fprintf(stderr, "column %s goes to %s\n",
            orig_column_name[i],
            orig_column_flag[i]?new_column_name[i]:"<deleted>");
  }
#endif

#ifdef DEBUG
  fprintf(stderr, "Transferring definitions to new file\n");
#endif
  for (i=0; i<orig_parameter_names; i++) {
    if (orig_parameter_flag[i]) {
      if (!SDDS_TransferParameterDefinition(&SDDS_dataset, &SDDS_orig, orig_parameter_name[i],
                                            new_parameter_name[i])) {
        fprintf(stderr, "unable to transfer parameter %s to %s\n", orig_parameter_name[i], new_parameter_name[i]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
    }
  }
  for (i=0; i<orig_array_names; i++) {
    if (orig_array_flag[i]) {
      if (!SDDS_TransferArrayDefinition(&SDDS_dataset, &SDDS_orig, orig_array_name[i], new_array_name[i])) {
        fprintf(stderr, "unable to transfer array %s to %s\n", orig_array_name[i], new_array_name[i]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
    }
  }
  output_columns = 0;
  for (i=0; i<orig_column_names; i++) {
    if (orig_column_flag[i]) {
      output_columns++;
      if (!SDDS_TransferColumnDefinition(&SDDS_dataset, &SDDS_orig, orig_column_name[i], 
                                         new_column_name[i])) {
        fprintf(stderr, "unable to transfer column %s to %s\n", orig_column_name[i], new_column_name[i]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
    }
  }

#ifdef DEBUG
  fprintf(stderr, "Writing layout.\n");
#endif
  if (!SDDS_WriteLayout(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }

  max_size = 0;
  for (i=0; i<SDDS_NUM_TYPES; i++)
    if (max_size<SDDS_type_size[i])
      max_size = SDDS_type_size[i];
  buffer = tmalloc(max_size*sizeof(char));

  first = 1;
  recovered = 0;
#ifdef DEBUG
  fprintf(stderr, "Reading file\n");
#endif
  while (!recovered && (pageNumber=SDDS_ReadPage(&SDDS_orig))>=0) {
    if (pageNumber==0) {
      if (!recover) {
        /* user did not ask to recover garbled page, so clip it */
        fprintf(stderr, "error: SDDS data garbled--consider using -recover option\n");
        fprintf(stderr, "warning: one or more data pages may be missing\n");
        break;
      }
      else {
        if (recover!=2 && SDDS_ReadRecoveryPossible()) {
          /* user wants to recover bad page */
          recovered = 1;
        }
        else {
          if (recover!=2)
            fprintf(stderr, "warning: unable to recover garbled data--one or more data pages may be missing\n");
          break;
        }
      }
    }
    if (fromPage && pageNumber<fromPage)
      continue;
    if (removePages) {
      keep=1;
      for (i=0;i<removePages;i++) {
        if (removePage[i] == pageNumber) {
          keep = 0;
          break;
        }
      }
      if (keep == 0)
        continue;
    }
    if (keepPages) {
      keep=0;
      for (i=0;i<keepPages;i++) {
        if (keepPage[i] == pageNumber) {
          keep = 1;
          break;
        }
      }
      if (keep == 0)
        continue;
    }
    if ((rows = SDDS_RowCount(&SDDS_orig))<0) {
      fprintf(stderr, "error: problem counting rows in input page\n");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (!SDDS_StartPage(&SDDS_dataset, rows)) {
      fprintf(stderr, "error: problem starting output page\n");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    for (i=0; i<orig_parameter_names; i++) {
      if (orig_parameter_flag[i]) {
        if (!SDDS_GetParameter(&SDDS_orig, orig_parameter_name[i], buffer)) {
          fprintf(stderr, "error: problem getting parameter %s\n", orig_parameter_name[i]);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, new_parameter_name[i], 
                                buffer, NULL)) {
          fprintf(stderr, "error: problem setting parameter %s\n", new_parameter_name[i]);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        /* need to free string data here */
      }
    }
    for (i=0; i<orig_array_names; i++) {
      SDDS_ARRAY *array;
      if (orig_array_flag[i]) {
        if (!(array=SDDS_GetArray(&SDDS_orig, orig_array_name[i], NULL))) {
          fprintf(stderr, "error: problem getting array %s\n", orig_array_name[i]);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        if (!SDDS_SetArray(&SDDS_dataset, new_array_name[i], SDDS_CONTIGUOUS_DATA, array->data, array->dimension)) {
          fprintf(stderr, "error: problem setting array %s\n", new_array_name[i]);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        SDDS_FreeArray(array);
      }
    }
    if (SDDS_CountRowsOfInterest(&SDDS_orig)) {
      for (i=0; i<orig_column_names; i++) {
        if (orig_column_flag[i]) {
          if (!(ptr=SDDS_GetInternalColumn(&SDDS_orig, orig_column_name[i]))) {
            fprintf(stderr, "error: problem getting column %s\n", orig_column_name[i]);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
          if (!SDDS_SetColumn(&SDDS_dataset, SDDS_SET_BY_NAME, ptr, rows,  new_column_name[i])) {
            fprintf(stderr, "error: problem setting column %s\n", new_column_name[i]);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
        }
      }
    }
    if (!SDDS_WritePage(&SDDS_dataset)) {
      fprintf(stderr, "error: problem writing page to file %s\n", output);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (toPage && pageNumber>=toPage)
      break;
  }
    
  if (!SDDS_Terminate(&SDDS_orig) || !SDDS_Terminate(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }

  if (tmpfile_used && !replaceFileAndBackUp(input, output)) {
    return(1);
  }
  for (i=0; i<orig_parameter_names; i++) {
    if (orig_parameter_name[i])
      free(orig_parameter_name[i]);
    if (new_parameter_name[i])
      free(new_parameter_name[i]);
  }
  if (orig_parameter_name)
    free(orig_parameter_name);
  if (new_parameter_name)
    free(new_parameter_name);
  if (orig_parameter_flag)
    free(orig_parameter_flag);
  for (i=0; i<orig_column_names; i++) {
    if (orig_column_name[i])
      free(orig_column_name[i]);
    if (new_column_name[i])
      free(new_column_name[i]);
  }
  if (orig_column_name)
    free(orig_column_name);
  if (new_column_name)
    free(new_column_name);
  if (orig_column_flag)
    free(orig_column_flag);
  for (i=0; i<orig_array_names; i++) {
    if (orig_array_name[i])
      free(orig_array_name[i]);
    if (new_array_name[i])
      free(new_array_name[i]);
  }
  if (orig_array_name)
    free(orig_array_name);
  if (new_array_name)
    free(new_array_name);
  if (orig_array_flag)
    free(orig_array_flag);
  /*free_scanargs(&s_arg,argc);*/
  return(0);
}


char **process_name_options(char **orig_name, long **orig_flag, long orig_names, 
                            char **delete, long deletes,
                            char **retain, long retains,
                            STRING_PAIR *rename, long renames,
                            EDIT_NAME_REQUEST *edit_request, long edit_requests)
{
  long i, j;
  char **new_name;
  char *ptr;

  *orig_flag = tmalloc(sizeof(**orig_flag)*orig_names);
  for (i=0; i<orig_names; i++)
    (*orig_flag)[i] = 1;

  if (deletes) {
    for (i=0; i<deletes; i++) {
      ptr = expand_ranges(delete[i]);
      free(delete[i]);
      delete[i] = ptr;
    }
    for (j=0; j<orig_names; j++) {
      for (i=0; i<deletes; i++) {
        if (wild_match(orig_name[j], delete[i])) {
          (*orig_flag)[j] = 0;
          break;
        }
      }
    }
  }

  if (retains) {
    for (i=0; i<retains; i++) {
      ptr = expand_ranges(retain[i]);
      free(retain[i]);
      retain[i] = ptr;
    }
    if (!deletes)
      for (j=0; j<orig_names; j++)
        (*orig_flag)[j] = 0;
    for (j=0; j<orig_names; j++) {
      if ((*orig_flag)[j])
        continue;
      for (i=0; i<retains; i++) {
        if (wild_match(orig_name[j], retain[i])) {
          (*orig_flag)[j] = 1;
          break;
        }
      }
    }
  }

  new_name = tmalloc(sizeof(*new_name)*orig_names);
  for (j=0; j<orig_names; j++) {
    for (i=0; i<renames; i++) {
      if (strcmp(rename[i][0], orig_name[j])==0) {
        SDDS_CopyString(new_name+j, rename[i][1]);
        break;
      }
    }
    if (i==renames)
      SDDS_CopyString(new_name+j, orig_name[j]);
    for (i=0; i<edit_requests; i++) {
      char edit_buffer[256];
      ptr = expand_ranges(edit_request[i].match_string);
      free(edit_request[i].match_string);
      edit_request[i].match_string = ptr;
      if (wild_match(new_name[j], edit_request[i].match_string)) {
        strcpy(edit_buffer, new_name[j]);
        if (!edit_string(edit_buffer, edit_request[i].edit_string))
          SDDS_Bomb("error editing name");
        free(new_name[j]);
        SDDS_CopyString(&new_name[j], edit_buffer);
      }
    }
  }

  return(new_name);
}
