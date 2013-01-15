/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sdds2stream
 * purpose: spew out values for a specified column or parameters
 *          of a SDDS data set
 *
 * Michael Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.22  2004/01/13 18:30:24  borland
 Fixed an error in the usage text for -rows option.

 Revision 1.21  2003/09/04 15:44:15  soliday
 Fixed bug with last version.

 Revision 1.20  2003/09/04 15:19:56  soliday
 Fixed bug when printing row count when there are no rows.

 Revision 1.19  2003/09/02 19:16:02  soliday
 Cleaned up code for Linux.

 Revision 1.18  2002/08/14 17:12:39  soliday
 Added Open License

 Revision 1.17  2002/07/25 00:12:40  borland
 Now check return values from SDDS_SprintTypedValue().

 Revision 1.16  2001/07/17 21:28:29  borland
 Reset the page counter for each file.

 Revision 1.15  2001/01/10 19:35:31  soliday
 Standardized usage message.

 Revision 1.14  2000/07/13 20:30:43  borland
 Removed an error message that didn't make sense (-pages vs. -npages).

 Revision 1.13  2000/07/13 20:26:53  borland
 Updated version numbers.

 Revision 1.12  2000/07/13 20:14:35  borland
 Per D. Blachowicz: -rows option now takes 'bare' and 'total' qualifiers.
 Added -npages option.

 Revision 1.11  1999/05/25 19:05:14  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/05/04 14:43:16  borland
 Added -array option to usage message.

 Revision 1.9  1999/04/02 17:56:45  soliday
 Added the -description option

 Revision 1.8  1999/01/06 19:54:37  borland
 Fixed the version number in the usage message.

 Revision 1.7  1996/07/17 19:47:36  borland
 Added -ignoreFormats option to cause printing without using header-defined
 formats, which are usually totally inappropriate.

 * Revision 1.6  1996/07/05  16:12:21  borland
 * Removed code that deletes quotes around strings in
 * -noQuotes mode.  Now using SDDS_PRINT_NOQUOTES mode to
 * SDDS_SprintTypedValue.
 *
 * Revision 1.5  1996/02/15  00:11:38  borland
 * Changed SDDS_GetColumn() call to SDDS_GetInternalColumn() for better performance.
 *
 * Revision 1.4  1995/11/05  18:17:52  borland
 * Added -array option for output of array data.
 *
 * Revision 1.3  1995/09/06  14:56:05  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
/*#include "SDDSaps.h"*/

#define SET_COLUMNS 0
#define SET_PARAMETERS 1
/* the -table option is retained for backward compatibility */
#define SET_TABLE 2
#define SET_DELIMITER 3
#define SET_FILENAMES 4
#define SET_ROWS 5
#define SET_NOQUOTES 6
#define SET_PIPE 7
#define SET_PAGE 8
#define SET_ARRAYS 9
#define SET_IGNOREFORMATS 10
#define SET_DESCRIPTION 11
#define SET_SHOW_PAGES 12
#define N_OPTIONS 13

char *option[N_OPTIONS] = {
    "columns", "parameters", "table", "delimiter", 
    "filenames", "rows", "noquotes", "pipe", "page", "arrays",
    "ignoreformats", "description", "npages",
    } ;

char *USAGE = "sdds2stream [-pipe] [<SDDSinput>...]\n\
 [-columns=<column-name>[,...]]\n\
 [-parameters=<parameter-name>[,...]]\n\
 [-arrays=<array-name>[,...]]]\n\
 [-page=<pageNumber>] [-delimiter=<delimiting-string>]\n\
 [-filenames] [-rows[=<bare>][,total]]\n\
 [-npages=<bare>] [-noquotes]\n\
 [-ignoreFormats] [-description]\n\n\
sdds2stream provides stream output to the standard output of data values from \
a group of columns or parameters.  Each line of the output contains a different \
row of the tabular data or a different parameter.  Values from different columns \
are separated by the delimiter string, which by default is a single space.\
If -page is not employed, all data pages are output sequentially. \
If multiple filenames are given, the files are processed sequentially in the \
order given.\n\n\
Program by Michael Borland. (This is version 5, July 2000, D. Blachowicz.)\n";

long SDDS_SprintTypedValue2(char *s, char *format, char *buffer, unsigned long mode);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_dataset;
  long i, j, k, i_arg, rows, retval, description;
  SCANNED_ARG *s_arg;
  char **input;
  char **column_name, **parameter_name, **array_name;
  char **parameterFormat, **columnFormat, **arrayFormat;
  long column_names, parameter_names, array_names, page_number, *type, inputs;
  char *delimiter;
  void **data;
  char *buffer;
  long filenames, print_rows, print_pages, noQuotes, pipe, ignoreFormats;
  static char printBuffer[SDDS_MAXLINE*16];
  long n_rows, n_rows_bare, n_rows_total, n_pages, n_pages_bare;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  type = NULL;
  data = NULL;
  buffer = tmalloc(sizeof(char)*16);  /* large enough for any data type */
  delimiter = NULL;
  input = parameter_name = column_name = array_name = NULL;
  parameterFormat = columnFormat = arrayFormat = NULL;
  inputs = parameter_names = column_names = array_names = 0;
  page_number = filenames = ignoreFormats = 0;
  n_rows = n_rows_bare = n_rows_total = n_pages = n_pages_bare = 0;
  print_rows = print_pages = noQuotes = pipe = description = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COLUMNS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        if (column_names)
          SDDS_Bomb("invalid syntax: specify -columns once only");
        column_name = tmalloc(sizeof(*column_name)*(s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          column_name[i-1] = s_arg[i_arg].list[i];
        column_names = s_arg[i_arg].n_items-1;
        break;
      case SET_PARAMETERS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -parameters syntax");
        if (parameter_names)
          SDDS_Bomb("invalid syntax: specify -parameters once only");
        parameter_name = tmalloc(sizeof(*parameter_name)*(s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          parameter_name[i-1] = s_arg[i_arg].list[i];
        parameter_names = s_arg[i_arg].n_items-1;
        break;
      case SET_ARRAYS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -arrays syntax");
        if (array_names)
          SDDS_Bomb("invalid syntax: specify -arrays once only");
        array_name = tmalloc(sizeof(*array_name)*(s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++)
          array_name[i-1] = s_arg[i_arg].list[i];
        array_names = s_arg[i_arg].n_items-1;
        break;
      case SET_TABLE:
      case SET_PAGE:
        if (s_arg[i_arg].n_items<2 || s_arg[i_arg].n_items>2)
          SDDS_Bomb("invalid -page syntax");
        if (page_number!=0)
          SDDS_Bomb("invalid syntax: specify -page once only");
        if (sscanf(s_arg[i_arg].list[1], "%ld", &page_number)!=1 || page_number<=0)
          SDDS_Bomb("invalid -page syntax or value");
        break;
      case SET_DELIMITER:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -delimiter syntax");
        delimiter = s_arg[i_arg].list[1];
        break;
      case SET_FILENAMES:
        filenames = 1;
        break;
      case SET_ROWS:
        if (s_arg[i_arg].n_items>3)
          SDDS_Bomb("invalid -rows syntax");
        else {
          char *rowsOutputMode[2] = {"bare", "total"};
          for (i=1; i<s_arg[i_arg].n_items; i++) {
            switch (match_string(s_arg[i_arg].list[i], rowsOutputMode, 2, 0)) {
            case 0:
              n_rows_bare = 1;
              break;
            case 1:
              n_rows_total = 1;
              break;
            default:
              SDDS_Bomb("unknown output mode for -rows option");
              break;
            }
          }
        }
        print_rows = 1;
        break;
      case SET_SHOW_PAGES:
        if (s_arg[i_arg].n_items>2)
          SDDS_Bomb("invalid -pages syntax");
        else {
          char *pagesOutputMode[1] = {"bare"};
          for (i=1; i<s_arg[i_arg].n_items; i++) {
            if (strcmp(s_arg[i_arg].list[i],"")!=0) {
              if (match_string(s_arg[i_arg].list[i], pagesOutputMode, 1, 0)==0) 
                n_pages_bare = 1;
              else 
                SDDS_Bomb("unknown output mode for -npages option");
            }
            else 
              SDDS_Bomb("unknown output mode for -npages option");
          }
        }
        print_pages = 1;
        break;
      case SET_NOQUOTES:
        noQuotes = 1;
        break;
      case SET_PIPE:
        pipe = 1;
        break;
      case SET_IGNOREFORMATS:
        ignoreFormats = 1;
        break;
      case SET_DESCRIPTION:
        description = 1;
        break;
      default:
        fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    }
    else {
      input = trealloc(input, sizeof(*input)*(inputs+1));
      input[inputs++] = s_arg[i_arg].list[0];
    }
  }

  if (!inputs) {
    if (!pipe)
      SDDS_Bomb("too few filenames");
    inputs = 1;
    input = trealloc(input, sizeof(*input)*(inputs+1));
    input[0] = NULL;
  }

  if (!column_names && !parameter_names && !array_names && !print_rows && !description && !print_pages)
    SDDS_Bomb("you must specify one of -columns, -parameters, -arrays, -rows or -description");
  if (!delimiter) {
    if (column_names || array_names)
      cp_str(&delimiter, " ");
    else
      cp_str(&delimiter, "\n");
  }

  SDDS_SetTerminateMode(TERMINATE_DONT_FREE_TABLE_STRINGS+
                        TERMINATE_DONT_FREE_ARRAY_STRINGS);
  for (k=0; k<inputs; k++) {
    if (!SDDS_InitializeInput(&SDDS_dataset, input[k])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    n_pages = 0;
    
    if (column_names) {
      if (k==0) {
        type = tmalloc(sizeof(*type)*column_names);
        data = tmalloc(sizeof(*data)*column_names);
        columnFormat = tmalloc(sizeof(*columnFormat)*column_names);
      }
      for (i=0; i<column_names; i++) {
        if ((j=SDDS_GetColumnIndex(&SDDS_dataset, column_name[i]))<0) {
          fprintf(stderr, "error: column %s does not exist\n", column_name[i]);
          exit(1);
        }
        if ((type[i]=SDDS_GetColumnType(&SDDS_dataset, j))<=0 || 
            !SDDS_GetColumnInformation(&SDDS_dataset, "format_string", columnFormat+i, SDDS_GET_BY_INDEX, j)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
    }
    else if (array_names) {
      if (k==0) {
        type = tmalloc(sizeof(*type)*array_names);
        data = tmalloc(sizeof(*data)*array_names);
        arrayFormat = tmalloc(sizeof(*arrayFormat)*array_names);
      }

      for (i=0; i<array_names; i++) {
        if ((j=SDDS_GetArrayIndex(&SDDS_dataset, array_name[i]))<0) {
          fprintf(stderr, "error: array %s does not exist\n", array_name[i]);
          exit(1);
        }
        if ((type[i]=SDDS_GetArrayType(&SDDS_dataset, j))<=0 ||
            !SDDS_GetArrayInformation(&SDDS_dataset, "format_string", arrayFormat+i, SDDS_BY_INDEX, j)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
    }
    else if (parameter_names) {
      if (k==0) {
        type = tmalloc(sizeof(*type)*parameter_names);
        parameterFormat = tmalloc(sizeof(*parameterFormat)*parameter_names);
      }
      for (i=0; i<parameter_names; i++) {
        if ((j=SDDS_GetParameterIndex(&SDDS_dataset, parameter_name[i]))<0) {
          fprintf(stderr, "error: parameter %s does not exist\n", parameter_name[i]);
          exit(1);
        }
        if ((type[i]=SDDS_GetParameterType(&SDDS_dataset, j))<=0 ||
            !SDDS_GetParameterInformation(&SDDS_dataset, "format_string", parameterFormat+i, SDDS_BY_INDEX, j)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
    }
    
    if (description) {
      if (!SDDS_SprintTypedValue2(SDDS_dataset.layout.description, NULL, 
                                  printBuffer,noQuotes?SDDS_PRINT_NOQUOTES:0)) {
        
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      
      fputs(printBuffer, stdout);
      fprintf(stdout, delimiter);

      if (!SDDS_SprintTypedValue2(SDDS_dataset.layout.contents, NULL, 
                                  printBuffer,noQuotes?SDDS_PRINT_NOQUOTES:0)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      fputs(printBuffer, stdout);
      fprintf(stdout, delimiter);

      if (!strchr(delimiter, '\n'))
        fputc('\n', stdout);
    }

    retval = -1;
    while (retval!=page_number && (retval=SDDS_ReadPage(&SDDS_dataset))>0) {
      if (page_number && retval!=page_number)
        continue;
      
      if (print_rows) {
        if (n_rows_total && !(page_number))
          n_rows += SDDS_CountRowsOfInterest(&SDDS_dataset);
        else {
          if (n_rows_bare)
            fprintf(stdout, "%" PRId32 "\n", SDDS_CountRowsOfInterest(&SDDS_dataset));
          else
            fprintf(stdout, "%" PRId32 " rows\n", SDDS_CountRowsOfInterest(&SDDS_dataset));
        }
      }

      if (column_names) {
        if ((rows=SDDS_CountRowsOfInterest(&SDDS_dataset))<0) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if (rows) {
          if (filenames) {
            fprintf(stdout, "%s%s", input[k], delimiter);
            if (!strchr(delimiter, '\n'))
              fputc('\n', stdout);
          }
          for (i=0; i<column_names; i++)
            if (!(data[i]=SDDS_GetInternalColumn(&SDDS_dataset, column_name[i]))) {
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
              exit(1);
            }
          for (j=0; j<rows; j++) {
            for (i=0; i<column_names; i++) {
              if (!SDDS_SprintTypedValue(data[i], j, type[i], 
                                         ignoreFormats?NULL:columnFormat[i], 
                                         printBuffer, 
                                         noQuotes?SDDS_PRINT_NOQUOTES:0)) {
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                exit(1);
              }
              fputs(printBuffer, stdout);
              if (i!=column_names-1)
                /* must use print here: */
                fprintf(stdout, delimiter);
            }
            fputc('\n', stdout);
          }
        }
      }
      else if (array_names) {
        SDDS_ARRAY *array;
        if (filenames) {
          fprintf(stdout, "%s%s", input[k], delimiter);
          if (!strchr(delimiter, '\n'))
            fputc('\n', stdout);
        }
        for (i=0; i<array_names; i++) {
          if (!(array=SDDS_GetArray(&SDDS_dataset, array_name[i], NULL))) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
          for (j=0; j<array->elements; j++) {
            if (!SDDS_SprintTypedValue(array->data, j, type[i], 
                                       ignoreFormats?NULL:arrayFormat[i], 
                                       printBuffer,
                                       noQuotes?SDDS_PRINT_NOQUOTES:0)) {
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
              exit(1);
            }
            fputs(printBuffer, stdout);
            fprintf(stdout, delimiter);
          }
        }
        if (!strchr(delimiter, '\n'))
          fputc('\n', stdout);
      }
      else {
        if (filenames)
          fprintf(stdout, "%s%s", input[k], delimiter);
        for (i=0; i<parameter_names; i++) {
          if (!SDDS_GetParameter(&SDDS_dataset, parameter_name[i], buffer)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
          if (!SDDS_SprintTypedValue(buffer, 0, type[i], 
                                     ignoreFormats?NULL:parameterFormat[i], 
                                     printBuffer,
                                     noQuotes?SDDS_PRINT_NOQUOTES:0)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
          fputs(printBuffer, stdout);
          /* must use print here: */
          fprintf(stdout, delimiter);
        }
        if (!strchr(delimiter, '\n'))
          fputc('\n', stdout);
      }
      n_pages++;
    }
    if (retval==0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    else {
      if (print_rows && (n_rows_total || (retval==-1 && n_pages==0)) && !(page_number)) {
        if (n_rows_bare)
          fprintf(stdout, "%ld\n", n_rows);
        else
          fprintf(stdout, "%ld rows\n", n_rows);
      }
      if (print_pages) {
        if (n_pages_bare)
          fprintf(stdout, "%ld\n", n_pages);
        else
          fprintf(stdout, "%ld pages\n", n_pages);
      }
    }
    if (!SDDS_Terminate(&SDDS_dataset)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  return(0);
}

long SDDS_SprintTypedValue2(char *s, char *format, char *buffer, unsigned long mode)
{
  char buffer2[SDDS_PRINT_BUFLEN];
  short printed;
  
  if (!s) {
    s = "";
  }
  if (!buffer) {
    SDDS_SetError("Unable to print value--buffer pointer is NULL (SDDS_SprintTypedValue2)");
    return(0);
  }
  if ((long)strlen(s)>SDDS_PRINT_BUFLEN-3) {
    SDDS_SetError("Buffer size overflow (SDDS_SprintTypedValue2)");
    return(0);
  }
  
  if (!(mode&SDDS_PRINT_NOQUOTES)) {
    printed = 0;
    if (!s || SDDS_StringIsBlank(s))
      sprintf(buffer, "\"\"");
    else if (strchr(s, '"')) {
      strcpy(buffer2, s);
      SDDS_EscapeQuotes(buffer2, '"');
      if (SDDS_HasWhitespace(buffer2))
	sprintf(buffer, "\"%s\"", buffer2);
      else
	strcpy(buffer, buffer2);
    }
    else if (SDDS_HasWhitespace(s))
      sprintf(buffer, "\"%s\"", s);
    else {
      sprintf(buffer, format?format:"%s", s);
      printed = 1;
    }
    if (!printed) {
      sprintf(buffer2, format?format:"%s", buffer);
      strcpy(buffer, buffer2);
    }
  } else {
    sprintf(buffer, format?format:"%s", s);
  }
  return(1);
}
