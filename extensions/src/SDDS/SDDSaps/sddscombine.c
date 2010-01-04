/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddscombine
 * purpose: combine a series of SDDS files into a single
 *          file with one page for each page in each file
 *
 * Michael Borland, 1994
 $Log: sddscombine.c,v $
 Revision 1.29  2008/01/15 22:30:08  borland
 Fixed bug with -merge=<parameterName> when multiple files are used.

 Revision 1.28  2006/12/14 22:21:57  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.27  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.26  2004/01/16 18:37:08  borland
 Fixed a memory leak due to non-freeing of parameter name list.

 Revision 1.25  2003/09/02 19:16:03  soliday
 Cleaned up code for Linux.

 Revision 1.24  2002/08/14 17:12:41  soliday
 Added Open License

 Revision 1.23  2001/10/25 21:59:25  borland
 Remove TABULAR_DATA_CHECKS.

 Revision 1.22  2001/02/06 16:07:45  soliday
 Added missing SDDS_Terminate function.

 Revision 1.21  2001/01/10 19:35:33  soliday
 Standardized usage message.

 Revision 1.20  2000/11/27 02:03:33  borland
 Fixed bug in previous changes.

 Revision 1.19  2000/11/27 00:48:57  borland
 Added -recover option.  Improved usage message.

 Revision 1.18  2000/10/21 23:48:17  borland
 The PageNumber column was being defined with a description field that was
 unitialized.  The description field is now NULL.

 Revision 1.17  1999/05/25 19:06:24  soliday
 Removed compiler warning on linux.

 Revision 1.16  1999/05/05 14:37:26  borland
 Fixed bug in SDDS_CompareParameterValues (wasn't compare string data
 correctly).

 Revision 1.15  1999/04/08 18:54:20  soliday
 The -collapse option now works when input files have different parameter names

 Revision 1.14  1999/01/06 19:54:39  borland
 Fixed the version number in the usage message.

 Revision 1.13  1998/12/01 19:48:49  soliday
 Added -collapse option.

 Revision 1.12  1998/04/17 21:24:42  borland
 Added -sparse option.

 Revision 1.11  1998/01/27 19:03:52  borland
 Commented out print statements that caused output to stderr in
 -merge=<paramName> mode.

 Revision 1.10  1997/12/19 17:01:46  borland
 Now keeps track of whether a page has been started in -merge mode,
 since otherwise if the first file is empty the program bombs.

 Revision 1.9  1997/10/29 21:26:10  borland
 When no columns are copied to the output file, uses SDDS_ReadPageSparse
 with a huge sparsing factor, so that only one row is copied per page of
 an input file.  Reduces memory requirements and increases speed.

 Revision 1.8  1997/10/24 14:55:39  borland
 Fixed bug in transferring of array definitions.

 Revision 1.7  1997/10/23 22:25:26  borland
 Added -delete and -retain options.

 Revision 1.6  1997/09/09 01:48:17  borland
 Added SDDS_ClearPage call when combining without merge.  Prevents propagation
 of data from one file to another when one file is missing columns.

 * Revision 1.5  1996/07/05  16:33:16  borland
 * Added 0 for new mode argument of SDDS_PrintTypedValue and/or
 * SDDS_SprintTypedValue.
 *
 * Revision 1.4  1996/01/20  05:35:44  borland
 * Now uses SDDS_AppendToLayout() to ensure that output file has all columns,
 * arrays, and parameters from input files, rather than just the first input
 * file.
 *
 * Revision 1.3  1995/09/06  14:56:14  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_MERGE 0
#define SET_OVERWRITE 1
#define SET_PIPE 2
#define SET_DELETE 3
#define SET_RETAIN 4
#define SET_SPARSE 5
#define SET_COLLAPSE 6
#define SET_RECOVER 7
#define N_OPTIONS 8
static char *option[N_OPTIONS] = {
  "merge", "overwrite", "pipe", "delete", "retain", "sparse", "collapse",
  "recover",
};

char *USAGE="sddscombine [<SDDSinputfilelist>] [<SDDSoutputfile>]\n\
[-pipe=[input][,output]]\n\
[-delete={column|parameter|array},<matching-string>[,...]]\n\
[-retain={column|parameter|array},<matching-string>[,...]]\n\
[-sparse=<integer>] [-merge[=<parameter-name>]] [-overWrite]\n\
[-collapse] [-recover[=clip]]\n\n\
sddscombine combines data from a series of SDDS files into a single \
SDDS file, usually with one page for each page in each file.  Data is added \
from files in the order that they are listed on the command line.  A \
new parameter ('Filename') is added to show the source of each page. \
-sparse    Samples tabular data, keeping every ith row.\n\
-merge     The data is merged into a single page in the output file.\n\
-overWrite Forces overwriting of the output file if it already exists.\n\
-collapse  As if output is put through sddscollapse.\n\
-recover   Incomplete or corrupted binary data is recovered.  If clip is\n\
           given, incomplete pages are deleted.\n\
If -pipe=input is given, then only one file should come down the pipe!\n\n\
Program by Michael Borland.  (This is version 7, December 2000.)\n";

long SDDS_CompareParameterValues(void *param1, void *param2, long type);
long keep_element(char *name, char **delete, long deletions,
                  char **retain, long retentions);
void *SDDS_GetParameterMod(SDDS_DATASET *SDDS_dataset, SDDS_DATASET *SDDS_output, char *parameter_name, void *memory);


#define COLUMN_MODE 0
#define PARAMETER_MODE 1
#define ARRAY_MODE 2
#define MODES 3
char *mode_name[MODES] = {
"column", "parameter", "array",
};
#define ROW_INCREMENT 100

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output;
  char **inputfile, *outputfile;
  long inputfiles, i, i_arg, retval=0, first_page;
  long iFile, first_data, sparse, setPageNumber;
  long allocated_rows, merge, overwrite, collapse, page;
  int32_t columns;
  SCANNED_ARG *s_arg;
  long buffer[16];
  char *param, *last_param, *this_param, *text, *contents, **column;
  long param_index, param_type, param_size, output_pending;
  unsigned long pipeFlags;

  char **retain_column, **delete_column;
  long retain_columns, delete_columns;
  char **retain_parameter, **delete_parameter;
  long retain_parameters, delete_parameters;
  char **retain_array, **delete_array;
  long retain_arrays, delete_arrays;
  long nColumns, recover, recovered;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  setPageNumber = allocated_rows = param_type = param_size = 0;
  last_param = this_param = NULL;
  column = NULL;
  inputfile = NULL;
  outputfile = param = NULL;
  inputfiles = merge = overwrite = collapse = 0;
  pipeFlags = 0;
  sparse = 1;
  recover = 0;
  
  retain_column = delete_column = NULL;
  retain_columns = delete_columns = 0;
  retain_parameter = delete_parameter = NULL;
  retain_parameters = delete_parameters = 0;
  retain_array = delete_array = NULL;
  retain_arrays = delete_arrays = 0;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_MERGE:
        if (s_arg[i_arg].n_items>2)
          bomb("invalid -merge syntax", USAGE);
        merge = 1;
        if (s_arg[i_arg].n_items==2)
          param = s_arg[i_arg].list[1];
        break;
      case SET_OVERWRITE:
        overwrite = 1;
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_RECOVER:
        recover = 1;
        if (s_arg[i_arg].n_items!=1) {
          recover = 2;
          if (s_arg[i_arg].n_items>2 ||
              strncmp(s_arg[i_arg].list[1], "clip", strlen(s_arg[i_arg].list[1]))!=0)
            SDDS_Bomb("invalid -recover syntax");
        }
        break;
      case SET_DELETE:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -delete syntax");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          delete_column = trealloc(delete_column, 
                                   sizeof(*delete_column)*(delete_columns+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_column[i-2+delete_columns] = expand_ranges(s_arg[i_arg].list[i]);
          delete_columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          delete_parameter = trealloc(delete_parameter, 
                                      sizeof(*delete_parameter)*(delete_parameters+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_parameter[i-2+delete_parameters] = expand_ranges(s_arg[i_arg].list[i]);
          delete_parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          delete_array = trealloc(delete_array, 
                                  sizeof(*delete_array)*(delete_arrays+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            delete_array[i-2+delete_arrays] = expand_ranges(s_arg[i_arg].list[i]);
          delete_arrays += s_arg[i_arg].n_items-2;
          break;
        default:
          SDDS_Bomb("invalid -delete syntax: specify column or parameter keyword");
          break;
        }
        break;
      case SET_RETAIN:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("invalid -retain syntax");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          retain_column = trealloc(retain_column, 
                                   sizeof(*retain_column)*(retain_columns+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_column[i-2+retain_columns] = expand_ranges(s_arg[i_arg].list[i]);
          retain_columns += s_arg[i_arg].n_items-2;
          break;
        case PARAMETER_MODE:
          retain_parameter = trealloc(retain_parameter, 
                                      sizeof(*retain_parameter)*(retain_parameters+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_parameter[i-2+retain_parameters] = expand_ranges(s_arg[i_arg].list[i]);
          retain_parameters += s_arg[i_arg].n_items-2;
          break;
        case ARRAY_MODE:
          retain_array = trealloc(retain_array, 
                                  sizeof(*retain_array)*(retain_arrays+s_arg[i_arg].n_items-2));
          for (i=2; i<s_arg[i_arg].n_items; i++)
            retain_array[i-2+retain_arrays] = expand_ranges(s_arg[i_arg].list[i]);
          retain_arrays += s_arg[i_arg].n_items-2;
          break;
        default:
          SDDS_Bomb("invalid -retain syntax: specify column or parameter keyword");
          break;
        }
        break;
      case SET_SPARSE:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &sparse)!=1 ||
            sparse<=0)
          bomb("invalid -sparse syntax", USAGE);
        break;
      case SET_COLLAPSE:
        collapse = 1;
        break;
      default:
        bomb("unrecognized option", USAGE);
        break;
      }
    }
    else {
      inputfile = trealloc(inputfile, sizeof(*inputfile)*(inputfiles+1));
      inputfile[inputfiles++] = s_arg[i_arg].list[0];
    }
  }

  outputfile = NULL;
  if (inputfiles>1) {
    if (pipeFlags&USE_STDIN)
      SDDS_Bomb("too many input files with -pipe option");
    if (!(pipeFlags&USE_STDOUT)) {
      outputfile = inputfile[--inputfiles];
      if (fexists(outputfile) && !overwrite)
        SDDS_Bomb("output file exists already--give -overWrite option to force replacement");
    }
  }
  else if (inputfiles==1) {
    if (pipeFlags&USE_STDIN) {
      outputfile = inputfile[0];
      inputfile[0] = NULL;
    }
    if (pipeFlags&USE_STDOUT && outputfile)
      SDDS_Bomb("too many filenames given with -pipe=output");
  }
  else {
    if (!(pipeFlags&USE_STDIN) || !(pipeFlags&USE_STDOUT))
      SDDS_Bomb("too few filenames given");
    inputfiles = 1;
    inputfile = tmalloc(sizeof(*inputfile)*1);
    inputfile[0] = outputfile = NULL;
  }
  
  for (i=0; i<inputfiles; i++)
    if (inputfile[i] && outputfile && strcmp(inputfile[i], outputfile)==0)
      SDDS_Bomb("output file is also an input file");

  output_pending = 0;

  if (!SDDS_InitializeInput(&SDDS_input, inputfile[0]) ||
      !SDDS_GetDescription(&SDDS_input, &text, &contents) ||
      !SDDS_InitializeOutput(&SDDS_output, SDDS_BINARY, 0, text, contents, outputfile)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  for (iFile=0; iFile<inputfiles; iFile++) {
    char **name;
    int32_t names;
    if ((iFile && !SDDS_InitializeInput(&SDDS_input, inputfile[iFile])))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!collapse) {
      if (!(name=SDDS_GetColumnNames(&SDDS_input, &names)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<names; i++) {
	if (keep_element(name[i], delete_column, delete_columns, retain_column,
			 retain_columns) && 
	    (SDDS_GetColumnIndex(&SDDS_output, name[i])<0 &&
	     !SDDS_TransferColumnDefinition(&SDDS_output, &SDDS_input, name[i], name[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	free(name[i]);
      }
      free(name);
    }

    if (!(name=SDDS_GetParameterNames(&SDDS_input, &names)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<names; i++) {
      if (collapse) {
	if (keep_element(name[i], delete_parameter, delete_parameters, retain_parameter,
			 retain_parameters) &&
	    (SDDS_GetColumnIndex(&SDDS_output, name[i])<0 &&
	     !SDDS_DefineColumnLikeParameter(&SDDS_output, &SDDS_input, name[i], name[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      } else {
	if (keep_element(name[i], delete_parameter, delete_parameters, retain_parameter,
			 retain_parameters) &&
	    (SDDS_GetParameterIndex(&SDDS_output, name[i])<0 &&
	     !SDDS_TransferParameterDefinition(&SDDS_output, &SDDS_input, name[i], name[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      free(name[i]);
    }
    free(name);
    
    if (!collapse) {
      if (!(name=SDDS_GetArrayNames(&SDDS_input, &names)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<names; i++) {
	if (keep_element(name[i], delete_array, delete_arrays, retain_array,
			 retain_arrays) && 
	    (SDDS_GetArrayIndex(&SDDS_output, name[i])<0 &&
	     !SDDS_TransferArrayDefinition(&SDDS_output, &SDDS_input, name[i], name[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	free(name[i]);
      }
      free(name);
    }

    if (inputfiles>1 && !SDDS_Terminate(&SDDS_input))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }

  if (collapse) {
    if (!(column = SDDS_GetColumnNames(&SDDS_output, &columns))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }

  if (collapse) {
    if (!merge) {
      if (SDDS_GetColumnIndex(&SDDS_output, "Filename")<0 && 
	  SDDS_DefineColumn(&SDDS_output, "Filename", NULL, NULL, "Name of file from which this page came",
			       NULL, SDDS_STRING, 0)<0)
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (SDDS_GetColumnIndex(&SDDS_output, "NumberCombined")<0 && 
	SDDS_DefineColumn(&SDDS_output, "NumberCombined", NULL, NULL, 
			  "Number of files combined to make this file", NULL, 
			  SDDS_LONG, 0)<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  } else {
    if (!SDDS_DeleteParameterFixedValues(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (!merge) {
      if (SDDS_GetParameterIndex(&SDDS_output, "Filename")<0 && 
	  SDDS_DefineParameter(&SDDS_output, "Filename", NULL, NULL, "Name of file from which this page came",
			       NULL, SDDS_STRING, NULL)<0)
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (SDDS_GetParameterIndex(&SDDS_output, "NumberCombined")<0 && 
	SDDS_DefineParameter(&SDDS_output, "NumberCombined", NULL, NULL, 
			     "Number of files combined to make this file", NULL, 
			     SDDS_LONG, NULL)<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }

  if (collapse) {
    if (SDDS_GetColumnIndex(&SDDS_output, "PageNumber")<0) {
      if (SDDS_DefineColumn(&SDDS_output, "PageNumber", NULL, NULL, NULL,
                            NULL, SDDS_LONG, 0)<0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      setPageNumber = 1;
    } else {
      setPageNumber = 0;
    }
  }
  if (!SDDS_WriteLayout(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (collapse) {
    if (!SDDS_StartPage(&SDDS_output, allocated_rows=ROW_INCREMENT)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  nColumns = SDDS_ColumnCount(&SDDS_output);
  
  page = 0;
  output_pending = 0;
  first_data = 1;  /* indicates no pages copied so far */
  for (iFile=0; iFile<inputfiles; iFile++) {
    if (inputfiles>1 && !SDDS_InitializeInput(&SDDS_input, inputfile[iFile])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    first_page = 1;
    recovered = 0;
    while (!recovered && 
           (retval=SDDS_ReadPageSparse(&SDDS_input, 0, nColumns?sparse:LONG_MAX-1, 0))>=0) {
      page++;
      if (retval==0) {
        if (!recover)
          break;
        recovered = 1;
        if (recover==2 || !SDDS_ReadRecoveryPossible())
          /* user doesn't want this page, or it can't be recovered */
          break;
      }
      if (param) {
        if (first_page) {
          if ((param_index=SDDS_GetParameterIndex(&SDDS_input, param))<0)
            SDDS_Bomb("-merge parameter not in input file(s)");
          if (param_type) {
            if (param_type!=SDDS_GetParameterType(&SDDS_input, param_index)) 
              SDDS_Bomb("-merge parameter changes type in subsequent files");
          } else {
            param_size = SDDS_GetTypeSize(param_type=SDDS_GetParameterType(&SDDS_input, param_index));
            this_param = tmalloc(param_size);
            last_param = tmalloc(param_size);
            if (!SDDS_GetParameter(&SDDS_input, param, last_param))
              SDDS_Bomb("error getting value for -merge parameter");
          }
        }
        else {
          memcpy(last_param, this_param, param_size);
	}
	if (!SDDS_GetParameter(&SDDS_input, param, this_param))
	  SDDS_Bomb("error getting value for -merge parameter");
      }
#ifdef DEBUG
      if (param) {
        fprintf(stderr, "parameter %s = ", param);
        SDDS_PrintTypedValue(this_param, 0, param_type, NULL, stderr, 0);
        fprintf(stderr, " now (was ");
        SDDS_PrintTypedValue(last_param, 0, param_type, NULL, stderr, 0);
        fprintf(stderr, ")\n");
      }
#endif
      if (collapse) {
	if (merge && param) {
	  if (SDDS_CompareParameterValues(this_param, last_param, param_type)!=0 && output_pending) {
	    output_pending = 0;
	  } 
	}
	if (!merge || (!param && first_data && first_page) || (param && !output_pending)) { 
	  if (page>allocated_rows) {
	    if (!SDDS_LengthenTable(&SDDS_output, ROW_INCREMENT)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    allocated_rows += ROW_INCREMENT;
	  }
	  for (i=0; i<columns; i++) {
	    if (!SDDS_GetParameterMod(&SDDS_input, &SDDS_output, column[i], buffer)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    if (!SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, page-1,
				   column[i], buffer, NULL)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  }
	  if (!merge) {
	    if (!SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, page-1, 
				   "Filename", 
				   inputfile[iFile]?inputfile[iFile]:"stdin", 
				   "NumberCombined", inputfiles, NULL)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  } else {
	    if (!SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, page-1, 
				   "NumberCombined", inputfiles, NULL)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  }
	  if (setPageNumber &&
	      !SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, page-1,
				 "PageNumber", page, NULL)) {
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	    exit(1);
	  }
	  first_data = 0;
	} else if (merge && param && output_pending) {
	  page--;
	}
      } else {
	if (!merge) {
	  if (!SDDS_ClearPage(&SDDS_output) ||
	      !SDDS_CopyPage(&SDDS_output, &SDDS_input) ||
	      !SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 
				  "Filename", 
				  inputfile[iFile]?inputfile[iFile]:"stdin", 
				  "NumberCombined", inputfiles, NULL) ||
	      !SDDS_WritePage(&SDDS_output)) {
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	    exit(1);
	  }
	} else if (merge && !param) {
	  if (first_data && first_page) {
	    if (!SDDS_CopyPage(&SDDS_output, &SDDS_input)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    first_data = 0;
	  } else {
	    if (!SDDS_CopyAdditionalRows(&SDDS_output, &SDDS_input)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  }
	} else {
#ifdef DEBUG
	  if (SDDS_CompareParameterValues(this_param, last_param, param_type)!=0) 
            fprintf(stderr, "Parameter value has changed\n");
#endif
	  if (SDDS_CompareParameterValues(this_param, last_param, param_type)!=0 && output_pending) {
	    if (!SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "NumberCombined",
				    inputfiles, NULL) ||
		!SDDS_WritePage(&SDDS_output)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    output_pending = 0;
	  }
	  if (!output_pending) {
	    if (!SDDS_CopyPage(&SDDS_output, &SDDS_input)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  } else {
	    if (!SDDS_CopyAdditionalRows(&SDDS_output, &SDDS_input)) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	  }
	}
      }
      if (merge) {
	output_pending = 1;
      }
      first_page = 0;
    }
    if (!recovered && (retval==0 || SDDS_NumberOfErrors() || !SDDS_Terminate(&SDDS_input))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!collapse && merge && output_pending) {
    if (!SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "NumberCombined", inputfiles, NULL) ||
        !SDDS_WritePage(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (collapse) {
    if (!SDDS_WritePage(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (page==0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  return(0);
}

void *SDDS_GetParameterMod(SDDS_DATASET *SDDS_dataset, SDDS_DATASET *SDDS_output, char *parameter_name, void *memory)
{
  long index, type, size;
  void *data;
  float floatdata;
  double doubledata;
  long longdata;
  int intdata;
  char chardata;
  char *stringdata;
  floatdata = 0.0;
  doubledata = 0.0;
  longdata = 0;
  intdata = 0;
  chardata = '\000';
  stringdata = "";

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_GetParameterMod"))
    return(NULL);
  if (!parameter_name) {
    SDDS_SetError("Unable to get parameter value--parameter name pointer is NULL (SDDS_GetParameterMod)");
    return(NULL);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_dataset, parameter_name))<0) {
    if ((index=SDDS_GetColumnIndex(SDDS_output, parameter_name))<0) {
      SDDS_SetError("Unable to get parameter value--parameter name is unrecognized (SDDS_GetParameterMod)");
      return(NULL);
    }
    if (!(type=SDDS_GetColumnType(SDDS_output, index))) {
      SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterMod)");
      return(NULL);
    }
    size = SDDS_type_size[type-1];
    if (memory)
      data = memory;
    else if (!(data = SDDS_Malloc(size))) {
      SDDS_SetError("Unable to get parameter value--parameter data size is invalid (SDDS_GetParameterMod)");
      return(NULL);
    }
    switch (type) {
    case SDDS_FLOAT:
      data = memcpy(data, &floatdata, size);
      break;
    case SDDS_DOUBLE:
      data = memcpy(data, &doubledata, size);;
      break;
    case SDDS_LONG:
      data = memcpy(data, &longdata, size);;
      break;
    case SDDS_SHORT:
      data = memcpy(data, &intdata, size);;
      break;
    case SDDS_CHARACTER:
      data = memcpy(data, &chardata, size);;
      break;
    case SDDS_STRING:
      if (!SDDS_CopyString((char**)data, stringdata))
      break;
    }
  } else {
    if (!(type=SDDS_GetParameterType(SDDS_dataset, index))) {
      SDDS_SetError("Unable to get parameter value--parameter data type is invalid (SDDS_GetParameterMod)");
      return(NULL);
    }
    if (!SDDS_dataset->parameter || !SDDS_dataset->parameter[index]) {
      SDDS_SetError("Unable to get parameter value--parameter data array is NULL (SDDS_GetParameterMod)");
      return(NULL);
    }
    size = SDDS_type_size[type-1];
    if (memory)
      data = memory;
    else if (!(data = SDDS_Malloc(size))) {
      SDDS_SetError("Unable to get parameter value--parameter data size is invalid (SDDS_GetParameterMod)");
      return(NULL);
    }
    if (type!=SDDS_STRING)
      memcpy(data, SDDS_dataset->parameter[index], size);
    else if (!SDDS_CopyString((char**)data, *(char**)SDDS_dataset->parameter[index]))
      return(NULL);
  }
  return(data);
}

long SDDS_CompareParameterValues(void *param1, void *param2, long type)
{
  double ddiff;
  long ldiff;
  char cdiff;

  switch (type) {
  case SDDS_FLOAT:
    ddiff = *((float*)param1) - *((float*)param2);
    return ddiff<0 ? -1 : ddiff>0?1:0 ;
  case SDDS_DOUBLE:
    ddiff = *((double*)param1) - *((double*)param2);
    return ddiff<0 ? -1 : ddiff>0?1:0 ;
  case SDDS_LONG:
    ldiff = *((long*)param1) - *((long*)param2);
    return ldiff<0 ? -1 : ldiff>0?1:0 ;
  case SDDS_SHORT:
    ldiff = *((short*)param1) - *((short*)param2);
    return ldiff<0 ? -1 : ldiff>0?1:0 ;
  case SDDS_CHARACTER:
    cdiff = (short)*((char*)param1) - (short)*((char*)param2);
    return cdiff<0 ? -1 : cdiff>0?1:0 ;
  case SDDS_STRING:
    return strcmp(*(char**)param1, *(char**)param2);
  default:
    SDDS_SetError("Problem doing data comparison--invalid data type (SDDS_CompareParameterValues)");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
}

long keep_element(char *name, char **delete, long deletions,
                  char **retain, long retentions) 
{
  long i, flag;

  flag = 1;

  if (deletions) {
    for (i=0; i<deletions; i++) {
      if (wild_match(name, delete[i])) {
        flag = 0;
        break;
      }
    }
  }

  if (retentions) {
    if (!deletions)
      flag = 0;
    for (i=0; i<retentions; i++) {
      if (wild_match(name, retain[i])) {
        flag = 1;
        break;
      }
    }
  }
  
  return flag;
}
