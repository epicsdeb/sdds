/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sdds2math.c
 * purpose: read an SDDS file and convert it to be read by Mathematica
 * Kenneth Evans, 1994 (based on sddsquery and sdds2stream by Michael Borland)
 $Log: not supported by cvs2svn $
 Revision 1.14  2003/08/28 15:21:28  soliday
 Removed include statement for malloc.h

 Revision 1.13  2002/08/14 17:12:39  soliday
 Added Open License

 Revision 1.12  2002/07/31 14:32:53  soliday
 Added support for Mac

 Revision 1.11  2000/11/20 22:53:44  soliday
 Fixed a problem with the arguments passed to processFilenames.

 Revision 1.10  1999/05/25 19:04:32  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/06 19:54:35  borland
 Fixed the version number in the usage message.

 Revision 1.8  1998/09/08 19:12:04  soliday
 Added "No description" output

 Revision 1.7  1996/07/05 16:33:09  borland
 Added 0 for new mode argument of SDDS_PrintTypedValue and/or
 SDDS_SprintTypedValue.

 * Revision 1.6  1996/02/15  19:20:44  borland
 * Changed to SDDS_Bomb for error messages.  Changed allocation of format
 * strings in sdds2math to avoid array out-of-bounds-read.
 *
 * Revision 1.5  1996/01/12  22:07:58  borland
 * Added call to SDDS_RegisterProgramName().  Added printout of "No description"
 * for sdds2spreadsheet.
 *
 * Revision 1.4  1996/01/12  01:21:41  borland
 * Added pipe option; added exit(0) for normal exit.
 *
 * Revision 1.3  1995/09/06  14:56:00  saunders
 * First test release of SDDS1.5
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_COMMENTS 0
#define SET_FORMAT 1
#define SET_VERBOSE 2
#define SET_PIPE 3
#define N_OPTIONS 4

#define FORMAT "%g"

char *option[N_OPTIONS] = {
    "comments",
    "format",
    "verbose",
    "pipe",
} ;

char *USAGE=
"\n"
"sdds2math [<SDDSfilename>] [<outputname>] [-pipe[=in][,out]]\n"
"   [-comments] [-verbose]\n"
"   [-format=<format-string>]\n"
"\n"
"sdds2math converts an SDDS file to a form usable by Mathematica.\n"
"  -pipe      Standard SDDS Toolkit pipe option.\n"
"  -comments  Put helpful Mathematica comments in the file\n"
"  -format    Format for doubles (Default: "FORMAT")\n"
"  -verbose   Write header information to the terminal like sddsquery\n"
"\n"
"This routine writes a single Mathematica variable to the output file.\n"
"The variable has the form:\n"
"sdds={description,coldef,pardef,arraydef,associates,tables}\n"
"  description={text,contents}\n"
"  coldef={coldef-1, coldef-2, ...}\n"
"    coldef-n={name,units,symbol,format,type,fieldlength,description}\n"
"  pardef={pardef-1, pardef-2, ...}\n"
"    pardef-n={name,fixed_value,units,symbol,type,description}\n"
"  arraydef={arraydef-1, arraydef-2, ...}\n"
"    arraydef-n={name,units,symbol,format,type,fieldlength,group,description}\n"
"  associates={associate-1, associate-2,...}\n"
"    associate-n={sdds,filename,path,contents,description}\n"
"  tables={table-1, table-2, ...}\n"
"    table-n={parameters,data}\n"
"      parameters={parameter-1, parameter-2, ...}\n"
"      data={row-1, row-2, ...}\n"
"        row-n={val-1, val-2, ...}\n"
"\n"
"Program by Kenneth Evans.  (This is version 3, September 1998.)\n";

/**************************** main ****************************************/
int main(int argc, char **argv)
{
  FILE *outfile;
  SDDS_TABLE SDDS_table;
  SDDS_LAYOUT *layout;
  COLUMN_DEFINITION *coldef;
  PARAMETER_DEFINITION *pardef;
  ARRAY_DEFINITION *arraydef;
  long list_request;
  char s[256];
  char *input, *output;
  long i, j, i_arg, append_units, nrows, ntable, len;
  SCANNED_ARG *s_arg;
  char *text, *contents, *ss, *ptr, *iformat=FORMAT, *format, *rformat;
  long verbose=0, comments=0, addquotes=1;
  short nexp;
  double dd, ddred;
  float ff, ffred;
  void *data;
  unsigned long pipeFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  
  list_request = -1;
  input = output = NULL;
  append_units = 0;
  pipeFlags = 0;
  
  argc = scanargs(&s_arg, argc, argv);
  if (argc==1)
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COMMENTS:
	comments = 1;
	break;
      case SET_FORMAT:
	if (s_arg[i_arg].n_items<2)
	  SDDS_Bomb("Invalid -format syntax");
	iformat = s_arg[i_arg].list[1];     /* Input format */
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
  
  processFilenames("sdds2math", &input, &output, pipeFlags, 0, NULL);
  
  if (output) {
    outfile=fopen(output,"w");
    if(!outfile)
      SDDS_Bomb(strcat("Can't open output file ", output));
  }
  else {
    outfile = stdout;
  }
  
  
  /* Calculate formats for converting to Mathematica convention */    
  len=strlen(iformat);     /* Inputformat */
  format=(char *)calloc(256,sizeof(char));     /* Whole format */
  rformat=(char *)calloc(256,sizeof(char));    /* Part before e */
  strcpy(format,iformat);
  if((ptr=strchr(format,'E'))) *ptr='e';     /* Aren't going to use E or G anyway */
  if((ptr=strchr(format,'G'))) *ptr='g';
  strcpy(rformat,format);
  if((ptr=strpbrk(rformat,"eg"))) *ptr='f';
  
  /*     fprintf(outfile,"Created from SDDS file: %s\n",filename[file]); */
  if (!SDDS_InitializeInput(&SDDS_table, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  layout = &SDDS_table.layout;
  /* Start top level */
  fprintf(outfile,"{");
  /* Description */    
  fprintf(outfile,"{");
  if(verbose) printf("\nfile %s is in SDDS protocol version %" PRId32 "\n", input, layout->version);
  if (!SDDS_GetDescription(&SDDS_table, &text, &contents)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (text)
    if(verbose) printf("description: %s\n", text);
  if (text)
    fprintf(outfile,"\"%s\",",text?text:"No description\t");
  if (contents)
    if(verbose) printf("contents: %s\n", contents);
  fprintf(outfile,"\"%s\"",contents?contents:"No description\t");
  if (layout->data_mode.mode==SDDS_ASCII) {
    if(verbose) printf("\ndata is ASCII with %" PRId32 " lines per row and %" PRId32 " additional header lines expected.\n",
		       layout->data_mode.lines_per_row, layout->data_mode.additional_header_lines);
    if(verbose) printf("row counts: %s \n",
		       layout->data_mode.no_row_counts?"no":"yes");
  }
  else
    if(verbose) printf("\ndata is binary\n");
  fprintf(outfile,"},\n");
  /* Columns */    
  fprintf(outfile," {");
  if (layout->n_columns) {
    if(verbose) printf("\n%" PRId32 " columns of data:\n", layout->n_columns);
    if(verbose) printf("NAME            UNITS           SYMBOL          FORMAT          TYPE    FIELD  DESCRIPTION\n");
    if(verbose) printf("                                                                        LENGTH\n");
    for (i=0; i<layout->n_columns; i++) {
      if(i > 0) fprintf(outfile,",\n  ");
      coldef = layout->column_definition+i;
      if(verbose) printf("%-15s %-15s %-15s %-15s %-7s %-7" PRId32 " %s\n",
			 coldef->name, coldef->units, coldef->symbol,
			 coldef->format_string, SDDS_type_name[coldef->type-1], coldef->field_length,  coldef->description);
      fprintf(outfile,"{\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%" PRId32 ",\"%s\"}",
	      coldef->name?coldef->name:"No name",
	      coldef->units?coldef->units:"",
	      coldef->symbol?coldef->symbol:"",
	      coldef->format_string?coldef->format_string:"",
	      SDDS_type_name[coldef->type-1],
	      coldef->field_length,
	      coldef->description?coldef->description:"No description");
    }
  }
  fprintf(outfile,"},\n");
  /* Parameters */    
  fprintf(outfile," {");
  if (layout->n_parameters) {
    if(verbose) printf("\n%" PRId32 " parameters:\n", layout->n_parameters);
    if(verbose) printf("NAME                UNITS               SYMBOL              TYPE                DESCRIPTION\n");
    for (i=0; i < layout->n_parameters; i++) {
      if(i > 0) fprintf(outfile,",\n  ");
      pardef = layout->parameter_definition+i;
      if(verbose) printf("%-19s %-19s %-19s %-19s %s\n",
			 pardef->name, pardef->units, pardef->symbol, 
			 SDDS_type_name[pardef->type-1], pardef->description);
      fprintf(outfile,"{\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"}",
	      pardef->name?pardef->name:"No name",
	      pardef->fixed_value?pardef->units:"",
	      pardef->units?pardef->units:"",
	      pardef->symbol?pardef->symbol:"",
	      SDDS_type_name[pardef->type-1],
	      pardef->description?pardef->description:"No description");
    }
  }
  fprintf(outfile,"},\n");
  /* Arrays */    
  fprintf(outfile," {");
  if (layout->n_arrays) {
    if(verbose) printf("\n%" PRId32 " arrays of data:\n", layout->n_arrays);
    if(verbose) printf("NAME            UNITS           SYMBOL"
		       "          FORMAT  TYPE            FIELD   GROUP           DESCRIPTION\n");
    if(verbose) printf("                                                                        LENGTH  NAME\n");
    for (i=0; i<layout->n_arrays; i++) {
      if(i > 0) fprintf(outfile,",\n  ");
      arraydef = layout->array_definition+i;
      if(verbose) printf("%-15s %-15s %-15s %-7s %-8s*^%-5" PRId32 " %-7" PRId32 " %-15s %s\n",
			 arraydef->name, arraydef->units, arraydef->symbol,
			 arraydef->format_string, SDDS_type_name[arraydef->type-1], arraydef->dimensions,
			 arraydef->field_length,  arraydef->group_name, arraydef->description);
      fprintf(outfile,"{\"%s\",\"%s\",\"%s\",\"%s\",\"%s*^%" PRId32 "\",%-7" PRId32 ",\"%s\",\"%s\"}",
	      arraydef->name?arraydef->name:"No name",
	      arraydef->units?arraydef->units:"",
	      arraydef->symbol?arraydef->symbol:"",
	      arraydef->format_string?arraydef->format_string:"",
	      SDDS_type_name[arraydef->type-1], arraydef->dimensions,
	      arraydef->field_length,
	      arraydef->group_name,
	      arraydef->description);
    }
  }
  fprintf(outfile,"},\n");
  /* Associates */    
  fprintf(outfile," {");
  if (layout->n_associates) {
    if(verbose) printf("\n%" PRId32 " associates:\n", layout->n_associates);
    if(verbose) printf("SDDS  FILENAME            PATH                          CONTENTS            DESCRIPTION\n");
    for (i=0; i<layout->n_associates; i++)
      if(i > 0) fprintf(outfile,",\n  ");
    if(verbose) printf("%-5s %-19s %-29s %-19s %s\n",
		       layout->associate_definition[i].sdds?"SDDS":"Non-SDDS",
		       layout->associate_definition[i].filename, layout->associate_definition[i].path,
		       layout->associate_definition[i].contents, layout->associate_definition[i].description);
    fprintf(outfile,"{\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"}",
	    layout->associate_definition[i].sdds?"True":"False",
	    layout->associate_definition[i].filename,
	    layout->associate_definition[i].path,
	    layout->associate_definition[i].contents,
	    layout->associate_definition[i].description);
  }
  fprintf(outfile,"},\n");
  /* Process tables */    
  fprintf(outfile," {");     /* Start of array of tables */
  while ((ntable=SDDS_ReadTable(&SDDS_table))>0) {
    if(ntable > 1) fprintf(outfile,",\n  ");
    if(comments) fprintf(outfile,"(*Table %ld*)",ntable);
    fprintf(outfile,"{\n");     /* Start of this table */
    /* Variable parameters */
    fprintf(outfile,"   {");    /* Start of parameters */
    for (i=0; i<layout->n_parameters; i++) {
      if(i > 0) fprintf(outfile,",\n    ");
      pardef = layout->parameter_definition+i;
      if (!(data=SDDS_GetParameter(&SDDS_table, pardef->name, NULL))) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
      /* This parameter */
      if(comments) fprintf(outfile,"(*%s*)",pardef->name);
      addquotes=1;
      switch(pardef->type) {
      case SDDS_DOUBLE:
	dd=*(double *)data;
	sprintf(s,format,dd);
	if((ptr=strchr(s,'e'))) {
	  *ptr=' ';
	  sscanf(s,"%lf %hd",&ddred,&nexp);
	  fprintf(outfile,rformat,ddred);
	  fprintf(outfile,"*10^%d",nexp);
	}
	else {
	  fprintf(outfile,s);
	}
	break;
      case SDDS_FLOAT:
	ff=*(float *)data;
	sprintf(s,format,ff);
	if((ptr=strchr(s,'e'))) {
	  *ptr=' ';
	  sscanf(s,"%f %hd",&ffred,&nexp);
	  fprintf(outfile,rformat,ffred);
	  fprintf(outfile,"*10^%d",nexp);
	}
	else {
	  fprintf(outfile,s);
	}
	break;
      case SDDS_STRING:
	ss=*(char **)data;
	if(*ss == '"') addquotes=0;
	else if(SDDS_StringIsBlank(ss) || SDDS_HasWhitespace(ss)) addquotes=0;
      case SDDS_CHARACTER:
	if(addquotes) fprintf(outfile,"\"");
	SDDS_PrintTypedValue(data, 0, pardef->type, NULL, outfile, 0);
	if(addquotes) fprintf(outfile,"\"");
	break;
      default:
	SDDS_PrintTypedValue(data, 0, pardef->type, NULL, outfile, 0);
	break;
      }
    }
    fprintf(outfile,"},\n");     /* End of parameters */
    /* Columns */
    fprintf(outfile,"   {");     /* Start of data array */
    if (layout->n_columns) {
      SDDS_SetColumnFlags(&SDDS_table, 1);
      SDDS_SetRowFlags(&SDDS_table, 1);
      if ((nrows=SDDS_CountRowsOfInterest(&SDDS_table))<0) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
      }
      /* 	    for (i=0; i<layout->n_columns; i++) { */
      /* 		coldef = layout->column_definition+i; */
      /* 		fprintf(outfile,"%s%s",coldef->name,delimiter); */
      /* 	    } */
      /* 	    fprintf(outfile,"\n"); */
      if (nrows) {
	for (j=0; j<nrows; j++) {
	  if(j > 0) fprintf(outfile,",\n    ");
	  fprintf(outfile,"{");     /* Start of row */
	  for (i=0; i<layout->n_columns; i++) {
	    if( i > 0) fprintf(outfile,",");
	    coldef = layout->column_definition+i;
	    if (!(data=SDDS_GetValue(&SDDS_table, coldef->name,j,NULL))) {
	      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	      exit(1);
	    }
	    addquotes=1;
	    switch(coldef->type) {
	    case SDDS_DOUBLE:
	      dd=*(double *)data;
	      sprintf(s,format,dd);
	      if((ptr=strchr(s,'e'))) {
		*ptr=' ';
		sscanf(s,"%lf %hd",&ddred,&nexp);
		fprintf(outfile,rformat,ddred);
		fprintf(outfile,"*10^%d",nexp);
	      }
	      else {
		fprintf(outfile,s);
	      }
	      break;
	    case SDDS_FLOAT:
	      ff=*(float *)data;
	      sprintf(s,format,ff);
	      if((ptr=strchr(s,'e'))) {
		*ptr=' ';
		sscanf(s,"%f %hd",&ffred,&nexp);
		fprintf(outfile,rformat,ffred);
		fprintf(outfile,"*10^%d",nexp);
	      }
	      else {
		fprintf(outfile,s);
	      }
	      break;
	    case SDDS_STRING:
	      ss=*(char **)data;
	      if(*ss == '"') addquotes=0;
	      else if(SDDS_StringIsBlank(ss) || SDDS_HasWhitespace(ss)) addquotes=0;
	    case SDDS_CHARACTER:
	      if(addquotes) fprintf(outfile,"\"");
	      SDDS_PrintTypedValue(data, 0, coldef->type, NULL, outfile, 0);
	      if(addquotes) fprintf(outfile,"\"");
	      break;
	    default:
	      SDDS_PrintTypedValue(data, 0, coldef->type, NULL, outfile, 0);
	      break;
	    }
	  }
	  fprintf(outfile,"}");     /* End of row */
	}
      }
    }
    fprintf(outfile,"}");     /* End of data array */
    fprintf(outfile,"}");     /* End of this table */
  }
  fprintf(outfile,"\n }\n");     /* End of array of tables, last major component */
  /* End top level */    
  fprintf(outfile,"}\n");     /* End of top level */
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
