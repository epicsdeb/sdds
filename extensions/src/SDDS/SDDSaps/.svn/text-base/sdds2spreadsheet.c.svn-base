/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sdds2spreadsheet.c
 * purpose: read an SDDS file and convert it to be read by a spreadsheet program
 * Kenneth Evans, 1994 (based on sddsquery and sdds2stream by Michael Borland)
 $Log: not supported by cvs2svn $
 Revision 1.18  2012/03/22 19:43:51  soliday
 Updated to compile if the xlslib library is not available.

 Revision 1.17  2012/01/27 21:31:18  soliday
 Added Excel XLS support

 Revision 1.16  2010/11/23 20:21:44  soliday
 Updated to fix a truncation issue with double and float values when they are
 printed. The full percision was being lost before.

 Revision 1.15  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.14  2002/08/14 17:12:39  soliday
 Added Open License

 Revision 1.13  2000/11/20 22:43:23  soliday
 Fixed a problem with the arguments passed to processFilenames.

 Revision 1.12  1999/11/23 16:45:57  borland
 Removed any mention of associates from the usage message.

 Revision 1.11  1999/05/25 19:05:00  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/01/06 19:54:36  borland
 Fixed the version number in the usage message.

 * Revision 1.9  1996/07/05  16:33:13  borland
 * Added 0 for new mode argument of SDDS_PrintTypedValue and/or
 * SDDS_SprintTypedValue.
 *
 * Revision 1.8  1996/02/15  19:20:47  borland
 * Changed to SDDS_Bomb for error messages.  Changed allocation of format
 * strings in sdds2math to avoid array out-of-bounds-read.
 *
 * Revision 1.7  1996/01/12  22:07:59  borland
 * Added call to SDDS_RegisterProgramName().  Added printout of "No description"
 * for sdds2spreadsheet.
 *
 * Revision 1.6  1996/01/12  01:21:18  borland
 * Added exit(0) for normal exit; modified usage statement; added usage
 * printout if no args are supplied.
 *
 * Revision 1.5  1996/01/11  20:38:38  borland
 * Added type cast in front of malloc() to avoid warning.
 *
 * Revision 1.4  1996/01/11  04:52:53  borland
 * Added -pipe option.  Rerouted -verbose output to stderr.
 *
 * Revision 1.3  1995/09/06  14:56:04  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#ifdef USE_XLS
#include "common/xlconfig.h"
#endif

#ifdef __GNUC__
#include <stdbool.h>
#else
typedef enum
{
	false = 0,
	true = 1
} bool;
#endif
#ifdef USE_XLS
#include "xlslib.h"
#include "common/systype.h"
#endif

#define SET_DELIMITER 0
#define SET_ALL 1
#define SET_VERBOSE 2
#define SET_PIPE 3
#define SET_EXCEL 4
#define N_OPTIONS 5

#define DELIMITER "\t"

char *option[N_OPTIONS] = {
    "delimiter",
    "all",
    "verbose",
    "pipe",
    "excel",
} ;


char *USAGE=
"\n"
"sdds2spreadsheet [<SDDSfilename>] [<outputname>] [-pipe[=in][,out]]\n"
"[-delimiter=<delimiting-string>] [-all] [-verbose] [-excel]\n"
"\n"
"sdds2spreadsheet converts an SDDS file to a form usable by a spreadsheet.\n"
"  -pipe      Standard SDDS toolkit pipe option.\n"
"  -excel     Write XLS Excel output file.\n"
"  -delimiter Delimiter string (Default is \"\\t\")\n"
"  -all       Write parameter, column, and array information.\n"
"               (Default is data and parameters only)\n"
"  -verbose   Write header information to the terminal like sddsquery\n"
"\n"
"Note: Excel 4.0 lines must be shorter than 255 characters\n"
"      Wingz delimiter can only be \"\\t\"\n"
"\n"
"Program by Kenneth Evans.  (This is version 3, July 1996.)\n";



int main(int argc, char **argv)
{
  FILE *outfile;
  SDDS_TABLE SDDS_table;
  SDDS_LAYOUT *layout;
  COLUMN_DEFINITION *coldef;
  PARAMETER_DEFINITION *pardef;
  ARRAY_DEFINITION *arraydef;
  long list_request;
  char *input, *output;
  long i, j, i_arg, append_units, nrows, ntable;
  SCANNED_ARG *s_arg;
  char *text, *contents, *delimiter;
  long verbose=0, all=0, nvariableparms=0, excel=0, line=0;
  void *data;
  unsigned long pipeFlags;
#ifdef USE_XLS
  workbook *w;
  worksheet *ws;
#endif
  int ret;
  char sheet[256];
  char buffer[5];

  SDDS_RegisterProgramName(argv[0]);
  
  list_request = -1;
  input = output = NULL;
  append_units = 0;
  delimiter = DELIMITER;
  pipeFlags = 0;
  
  argc = scanargs(&s_arg, argc, argv);
  if (argc==1)
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_DELIMITER:
	if (s_arg[i_arg].n_items<2)
	  SDDS_Bomb("Invalid -delimiter syntax");
	delimiter = s_arg[i_arg].list[1];
	break;
      case SET_ALL:
	all = 1;
	break;
      case SET_EXCEL:
#ifdef USE_XLS
	excel = 1;
#else
        SDDS_Bomb("-excel option is not available becuase sdds2spreadsheet was not compiled with xlslib support");
#endif
	break;
      case SET_VERBOSE:
	verbose = 1;
	break;
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, 
			       s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      default:
	SDDS_Bomb(strcat("Unknown option: ", s_arg[i_arg].list[0]));
	break;
      }
    }
    else {
      if (input==NULL)
	input = s_arg[i_arg].list[0];
      else if (output==NULL)
	output = s_arg[i_arg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }
  
  
  processFilenames("sdds2spreadsheet", &input, &output, pipeFlags, 0, NULL);
  
  if (output) {
    if (!excel) {
      outfile=fopen(output,"w");
      if(!outfile)
        SDDS_Bomb(strcat("Can't open output file ", output));
    }
  }
  else {
    if (excel) {
      SDDS_Bomb("-pipe=out and -excel options cannot be used together");
    }
    outfile = stdout;
  }
  
  if (input && !excel)
    fprintf(outfile,"Created from SDDS file: %s\n", input);
  if (!SDDS_InitializeInput(&SDDS_table, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  layout = &SDDS_table.layout;
  /* Description */    
  if(verbose && input) 
    fprintf(stderr, "\nfile %s is in SDDS protocol version %" PRId32 "\n", 
	    input, layout->version);

  if (!SDDS_GetDescription(&SDDS_table, &text, &contents)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (text) 
    if (verbose) 
      fprintf(stderr, "description: %s\n", text);

  if (!excel)
    fprintf(outfile,"%s%s\n",text?text:"No description",delimiter);
  
  if (contents)
    if (verbose) 
      fprintf(stderr, "contents: %s\n", contents);

  if (!excel)
    fprintf(outfile,"%s%s\n",contents?contents:"No description",delimiter);
  
  if (layout->data_mode.mode==SDDS_ASCII) {
    if (verbose) {
      fprintf(stderr, "\ndata is ASCII with %" PRId32 " lines per row and %" PRId32 " additional header lines expected.\n",
	      layout->data_mode.lines_per_row, layout->data_mode.additional_header_lines);
      fprintf(stderr, "row counts: %s    \n",
	      layout->data_mode.no_row_counts?"no":"yes");
    }
  } 
  else if (verbose) 
    fprintf(stderr, "\ndata is binary\n");

  /* Columns */    
  if (layout->n_columns) {
    if(verbose) {
      fprintf(stderr, "\n%" PRId32 " columns of data:\n", layout->n_columns);
      fprintf(stderr, "NAME            UNITS           SYMBOL          FORMAT          TYPE    FIELD  DESCRIPTION\n");
      fprintf(stderr, "                                                                        LENGTH\n");
    }
    if (all && !excel) 
      fprintf(outfile,
	      "\nColumns%s\nName%sUnits%sSymbol%sFormat%sType%sField Length%sDescription%s\n",
	      delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, 
	      delimiter);
    for (i=0; i<layout->n_columns; i++) {
      coldef = layout->column_definition+i;
      if(verbose) 
	fprintf(stderr, "%-15s %-15s %-15s %-15s %-7s %-7" PRId32 " %s\n",
		coldef->name, coldef->units?coldef->units:"", 
                coldef->symbol?coldef->symbol:"",
		coldef->format_string?coldef->format_string:"", 
                SDDS_type_name[coldef->type-1], 
                coldef->field_length,  
		coldef->description?coldef->description:"");
      if(all && !excel) 
	fprintf(outfile,"%s%s%s%s%s%s%s%s%s%s%-7" PRId32 "%s%s%s\n",
		coldef->name, delimiter,
		coldef->units?coldef->units:"", delimiter,
		coldef->symbol?coldef->symbol:"", delimiter,
		coldef->format_string?coldef->format_string:"", delimiter,
		SDDS_type_name[coldef->type-1], delimiter,
		coldef->field_length,  delimiter,
		coldef->description?coldef->description:"", delimiter);
    }
  }
  /* Parameters */    
  if (layout->n_parameters) {
    if (verbose) {
      fprintf(stderr, "\n%" PRId32 " parameters:\n", layout->n_parameters);
      fprintf(stderr, 
	      "NAME                UNITS               SYMBOL              TYPE                DESCRIPTION\n");
    }
    if(all && !excel) 
      fprintf(outfile,"\nParameters%s\nName%sFixedValue%sUnits%sSymbol%sType%sDescription%s\n",
	      delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
    for (i=0; i<layout->n_parameters; i++) {
      pardef = layout->parameter_definition+i;
      if(!pardef->fixed_value) {
	nvariableparms++;
	if(!all) continue;
      }
      if(verbose) fprintf(stderr, "%-19s %-19s %-19s %-19s %s\n",
			  pardef->name, pardef->units?pardef->units:"", pardef->symbol?pardef->symbol:"", 
			  SDDS_type_name[pardef->type-1], pardef->description?pardef->description:"");
      if (!excel) {
        if(all) fprintf(outfile,"%s%s%s%s%s%s%s%s%s%s%s%s\n",
                        pardef->name, delimiter,
                        pardef->fixed_value?pardef->fixed_value:"", delimiter,
                        pardef->units?pardef->units:"", delimiter,
                        pardef->symbol?pardef->symbol:"", delimiter,
                        SDDS_type_name[pardef->type-1], delimiter,
                        pardef->description?pardef->description:"", delimiter);
        else fprintf(outfile,"%s%s%s%s%s\n",
                     pardef->name, delimiter, delimiter,
                     pardef->fixed_value?pardef->fixed_value:"", delimiter);
      }
    }
  }
  /* Arrays */    
  if (layout->n_arrays && all) {
    if(verbose) {
      fprintf(stderr, "\n%" PRId32 " arrays of data:\n", layout->n_arrays);
      fprintf(stderr, "NAME            UNITS           SYMBOL"
	      "          FORMAT  TYPE            FIELD   GROUP           DESCRIPTION\n");
      fprintf(stderr, "                                                                        LENGTH  NAME\n");
    }
    if (!excel) {
      fprintf(outfile,"\nArrays%s\nName%sUnits%sSymbol%sFormat%sType%sField Length%sGroup Name%sDescription%s\n", delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
    }
    for (i=0; i<layout->n_arrays; i++) {
      arraydef = layout->array_definition+i;
      if(verbose) fprintf(stderr, "%-15s %-15s %-15s %-7s %-8s*^%-5" PRId32 " %-7" PRId32 " %-15s %s\n",
			  arraydef->name, arraydef->units, arraydef->symbol,
			  arraydef->format_string, SDDS_type_name[arraydef->type-1], arraydef->dimensions,
			  arraydef->field_length,  arraydef->group_name, arraydef->description);
      if (!excel) {
        fprintf(outfile,"%s%s%s%s%s%s%s%s%s*^%-5" PRId32 "%s%-7" PRId32 "%s%s%s%s%s\n",
                arraydef->name, delimiter,
                arraydef->units, delimiter,
                arraydef->symbol, delimiter,
                arraydef->format_string, delimiter,
                SDDS_type_name[arraydef->type-1], arraydef->dimensions, delimiter,
                arraydef->field_length,  delimiter,
                arraydef->group_name, delimiter,
                arraydef->description, delimiter);
      }
    }
  }
  /* Associates */    
  /*
    if (layout->n_associates && all) {
    if(verbose) {
            fprintf(stderr, "\n%ld associates:\n", layout->n_associates);
            fprintf(stderr, "SDDS  FILENAME            PATH                          CONTENTS            DESCRIPTION\n");
            }
        fprintf(outfile,"\nAssociates%s\nType%sFilename%sPath%sContents%sDescription%s\n",
	  delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
	for (i=0; i<layout->n_associates; i++)
          if(verbose) fprintf(stderr, "%-5s %-19s %-29s %-19s %s\n",
	    layout->associate_definition[i].sdds?"SDDS":"Non-SDDS",
	    layout->associate_definition[i].filename, layout->associate_definition[i].path,
	    layout->associate_definition[i].contents, layout->associate_definition[i].description);
	  fprintf(outfile,"%s%s%s%s%s%s%s%s%s%s\n",
	    layout->associate_definition[i].sdds?"yes":"no", delimiter,
	    layout->associate_definition[i].filename, delimiter,
	    layout->associate_definition[i].path, delimiter,
	    layout->associate_definition[i].contents, delimiter,
	    layout->associate_definition[i].description, delimiter);
    }
*/

#ifdef USE_XLS
  w = xlsNewWorkbook();
#ifdef __APPLE__
  xlsWorkbookIconvInType(w, "UCS-4-INTERNAL");
#endif
#endif
/* Process tables */    
  while ((ntable=SDDS_ReadTable(&SDDS_table))>0) {
    line=0;
#ifdef USE_XLS
    if (excel) {
      sprintf(sheet, "Sheet%d", ntable);
      ws = xlsWorkbookSheet(w, sheet);
    } else
      fprintf(outfile,"\nTable %ld\n",ntable);
#else
    fprintf(outfile,"\nTable %ld\n",ntable);  
#endif
    /* Variable parameters */
    if (nvariableparms) {
      for (i=0; i<layout->n_parameters; i++) {
	pardef = layout->parameter_definition+i;
	if(pardef->fixed_value) continue;
	if (!(data=SDDS_GetParameter(&SDDS_table, pardef->name, NULL))) {
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	  exit(1);
	}
#ifdef USE_XLS
        if (excel) {
          xlsWorksheetLabel(ws, line, 0, pardef->name, NULL);
          switch(pardef->type) {
          case SDDS_DOUBLE:
            xlsWorksheetNumberDbl(ws, line, 1, *((double*)data), NULL);
            break;
          case SDDS_FLOAT:
            xlsWorksheetNumberDbl(ws, line, 1, *((float*)data), NULL);  
            break;
          case SDDS_ULONG:
            xlsWorksheetNumberInt(ws, line, 1, *((uint32_t*)data), NULL);  
            break;
          case SDDS_LONG:
            xlsWorksheetNumberInt(ws, line, 1, *((int32_t*)data), NULL);  
            break;
          case SDDS_USHORT:
            xlsWorksheetNumberInt(ws, line, 1, *((unsigned short*)data), NULL);  
            break;
          case SDDS_SHORT:
            xlsWorksheetNumberInt(ws, line, 1, *((short*)data), NULL);  
            break;
          case SDDS_STRING:
            xlsWorksheetLabel(ws, line, 1, *((char**)data), NULL);
            break;
          case SDDS_CHARACTER:
            sprintf(buffer, "%c", *((char*)data));
            xlsWorksheetLabel(ws, line, 1, buffer, NULL);
            break;
          default:
            break;
          }

          line++;
        } else {
#endif
          fprintf(outfile,"%s%s%s",pardef->name,delimiter,delimiter);
          SDDS_PrintTypedValue(data, 0, pardef->type, NULL, outfile, 0);
          fprintf(outfile,"%s\n",delimiter);
#ifdef USE_XLS
        }
#endif
      }
      line++;
    }
    /* Columns */
    if (layout->n_columns) {
      SDDS_SetColumnFlags(&SDDS_table, 1);
      SDDS_SetRowFlags(&SDDS_table, 1);
      if ((nrows=SDDS_CountRowsOfInterest(&SDDS_table))<0) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
      for (i=0; i<layout->n_columns; i++) {
	coldef = layout->column_definition+i;
#ifdef USE_XLS
        if (excel) {
          xlsWorksheetLabel(ws, line, i, coldef->name, NULL);
        }
        else
          fprintf(outfile,"%s%s",coldef->name,delimiter);
#else
        fprintf(outfile,"%s%s",coldef->name,delimiter);
#endif
      }
      line++;
      if (!excel)
        fprintf(outfile,"\n");
      if (nrows) {
	for (j=0; j<nrows; j++) {
	  for (i=0; i<layout->n_columns; i++) {
	    coldef = layout->column_definition+i;
	    if (!(data=SDDS_GetValue(&SDDS_table, coldef->name,j,NULL))) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
#ifdef USE_XLS
            if (excel) {
              switch(coldef->type) {
              case SDDS_DOUBLE:
                xlsWorksheetNumberDbl(ws, line, i, *((double*)data), NULL);  
                break;
              case SDDS_FLOAT:
                xlsWorksheetNumberDbl(ws, line, i, *((float*)data), NULL);  
                break;
              case SDDS_ULONG:
                xlsWorksheetNumberInt(ws, line, i, *((uint32_t*)data), NULL);  
                break;
              case SDDS_LONG:
                xlsWorksheetNumberInt(ws, line, i, *((int32_t*)data), NULL);  
                break;
              case SDDS_USHORT:
                xlsWorksheetNumberInt(ws, line, i, *((unsigned short*)data), NULL);  
                break;
              case SDDS_SHORT:
                xlsWorksheetNumberInt(ws, line, i, *((short*)data), NULL);  
                break;
              case SDDS_STRING:
                xlsWorksheetLabel(ws, line, i, *((char**)data), NULL);
                break;
              case SDDS_CHARACTER:
                sprintf(buffer, "%c", *((char*)data));
                xlsWorksheetLabel(ws, line, i, buffer, NULL);
                break;
              default:
                break;
              }
            } else {
#endif
              switch(coldef->type) {
              case SDDS_DOUBLE:
                fprintf(outfile, "%.*g", DBL_DIG, *((double*)data));
                break;
              case SDDS_FLOAT:
                fprintf(outfile, "%.*g", FLT_DIG, *((float*)data));
                break;
              default:
                SDDS_PrintTypedValue(data, 0, coldef->type, NULL, outfile, 0);
                break;
              }
              fprintf(outfile,delimiter);
#ifdef USE_XLS
            }
#endif
	  }
          if (!excel)
            fprintf(outfile,"\n");
          line++;
	}
      }
    }
  }
#ifdef USE_XLS
  ret = xlsWorkbookDump(w, output);
  xlsDeleteWorkbook(w);
#endif

  /*	
	QUIT:
  */
  fflush(stdout);
  if (!SDDS_Terminate(&SDDS_table)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}

