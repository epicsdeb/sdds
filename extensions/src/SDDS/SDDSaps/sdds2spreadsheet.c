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
 $Log: sdds2spreadsheet.c,v $
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

#define SET_DELIMITER 0
#define SET_ALL 1
#define SET_VERBOSE 2
#define SET_PIPE 3
#define N_OPTIONS 4

#define DELIMITER "\t"

char *option[N_OPTIONS] = {
    "delimiter",
    "all",
    "verbose",
    "pipe",
} ;


char *USAGE=
"\n"
"sdds2spreadsheet [<SDDSfilename>] [<outputname>] [-pipe[=in][,out]]\n"
"[-delimiter=<delimiting-string>] [-all] [-verbose]\n"
"\n"
"sdds2spreadsheet converts an SDDS file to a form usable by a spreadsheet.\n"
"  -pipe      Standard SDDS toolkit pipe option.\n"
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
  long verbose=0, all=0, nvariableparms=0;
  void *data;
  unsigned long pipeFlags;
  
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
    outfile=fopen(output,"w");
    if(!outfile)
      SDDS_Bomb(strcat("Can't open output file ", output));
  }
  else {
    outfile = stdout;
  }
  
  if (input)
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
  fprintf(outfile,"%s%s\n",text?text:"No description",delimiter);
  
  if (contents)
    if (verbose) 
      fprintf(stderr, "contents: %s\n", contents);
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
    if (all) 
      fprintf(outfile,
	      "\nColumns%s\nName%sUnits%sSymbol%sFormat%sType%sField Length%sDescription%s\n",
	      delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, 
	      delimiter);
    for (i=0; i<layout->n_columns; i++) {
      coldef = layout->column_definition+i;
      if(verbose) 
	fprintf(stderr, "%-15s %-15s %-15s %-15s %-7s %-7" PRId32 " %s\n",
		coldef->name, coldef->units, coldef->symbol,
		coldef->format_string, SDDS_type_name[coldef->type-1], coldef->field_length,  
		coldef->description);
      if(all) 
	fprintf(outfile,"%s%s%s%s%s%s%s%s%s%s%-7" PRId32 "%s%s%s\n",
		coldef->name, delimiter,
		coldef->units, delimiter,
		coldef->symbol, delimiter,
		coldef->format_string, delimiter,
		SDDS_type_name[coldef->type-1], delimiter,
		coldef->field_length,  delimiter,
		coldef->description, delimiter);
    }
  }
  /* Parameters */    
  if (layout->n_parameters) {
    if (verbose) {
      fprintf(stderr, "\n%" PRId32 " parameters:\n", layout->n_parameters);
      fprintf(stderr, 
	      "NAME                UNITS               SYMBOL              TYPE                DESCRIPTION\n");
    }
    if(all) 
      fprintf(outfile,"\nParameters%s\nName%sFixedValue%sUnits%sSymbol%sType%sDescription%s\n",
	      delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
    for (i=0; i<layout->n_parameters; i++) {
      pardef = layout->parameter_definition+i;
      if(!pardef->fixed_value) {
	nvariableparms++;
	if(!all) continue;
      }
      if(verbose) fprintf(stderr, "%-19s %-19s %-19s %-19s %s\n",
			  pardef->name, pardef->units, pardef->symbol, 
			  SDDS_type_name[pardef->type-1], pardef->description);
      if(all) fprintf(outfile,"%s%s%s%s%s%s%s%s%s%s%s%s\n",
		      pardef->name, delimiter,
		      pardef->fixed_value, delimiter,
		      pardef->units, delimiter,
		      pardef->symbol, delimiter,
		      SDDS_type_name[pardef->type-1], delimiter,
		      pardef->description, delimiter);
      else fprintf(outfile,"%s%s%s%s%s\n",
		   pardef->name, delimiter, delimiter,
		   pardef->fixed_value, delimiter);
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
    fprintf(outfile,"\nArrays%s\nName%sUnits%sSymbol%sFormat%sType%sField Length%sGroup Name%sDescription%s\n",
	    delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
    for (i=0; i<layout->n_arrays; i++) {
      arraydef = layout->array_definition+i;
      if(verbose) fprintf(stderr, "%-15s %-15s %-15s %-7s %-8s*^%-5" PRId32 " %-7" PRId32 " %-15s %s\n",
			  arraydef->name, arraydef->units, arraydef->symbol,
			  arraydef->format_string, SDDS_type_name[arraydef->type-1], arraydef->dimensions,
			  arraydef->field_length,  arraydef->group_name, arraydef->description);
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
/* Process tables */    
  while ((ntable=SDDS_ReadTable(&SDDS_table))>0) {
    fprintf(outfile,"\nTable %ld\n",ntable);
    /* Variable parameters */
    if (nvariableparms) {
      for (i=0; i<layout->n_parameters; i++) {
	pardef = layout->parameter_definition+i;
	if(pardef->fixed_value) continue;
	if (!(data=SDDS_GetParameter(&SDDS_table, pardef->name, NULL))) {
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	  exit(1);
	}
	fprintf(outfile,"%s%s%s",pardef->name,delimiter,delimiter);
	SDDS_PrintTypedValue(data, 0, pardef->type, NULL, outfile, 0);
	fprintf(outfile,"%s\n",delimiter);
      }
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
	fprintf(outfile,"%s%s",coldef->name,delimiter);
      }
      fprintf(outfile,"\n");
      if (nrows) {
	for (j=0; j<nrows; j++) {
	  for (i=0; i<layout->n_columns; i++) {
	    coldef = layout->column_definition+i;
	    if (!(data=SDDS_GetValue(&SDDS_table, coldef->name,j,NULL))) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    switch(coldef->type) {
	    case SDDS_DOUBLE:
	    case SDDS_FLOAT:
	      SDDS_PrintTypedValue(data, 0, coldef->type, "%g", outfile, 0);
	      break;
	    default:
	      SDDS_PrintTypedValue(data, 0, coldef->type, NULL, outfile, 0);
	      break;
	    }
	    fprintf(outfile,delimiter);
	  }
	  fprintf(outfile,"\n");
	}
      }
    }
  }
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
