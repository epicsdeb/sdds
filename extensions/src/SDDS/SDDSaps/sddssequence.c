/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssequence.c
 * purpose: Create an SDDS file with equispaced indices in a column.
 *
 * M. Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.9  2002/08/14 17:12:54  soliday
 Added Open License

 Revision 1.8  2002/01/16 18:54:55  soliday
 Added missing SDDS_Terminate function.

 Revision 1.7  2001/01/10 19:35:46  soliday
 Standardized usage message.

 Revision 1.6  1999/05/25 19:15:12  soliday
 Removed compiler warning on linux.

 Revision 1.5  1999/01/06 19:54:56  borland
 Fixed the version number in the usage message.

 * Revision 1.4  1996/02/14  01:05:35  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:57:06  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
 
#define SET_PIPE 0
#define SET_DEFINE 1
#define SET_SEQUENCE 2
#define SET_REPEAT 3
#define N_OPTIONS 4

char *option[N_OPTIONS] = {
  "pipe", "define", "sequence", "repeat",
};

static char *USAGE="sddssequence [<outputfile>] [-pipe=[output]]\n\
-define=<columnName>[,<definitionEntries>] [-repeat=<number]\n\
-sequence=begin=<value>[,number=<integer>][,end=<value>][,delta=<value>][,interval=<integer>]\n\
[-sequence=[begin=<value>][,number=<integer>][,end=<value>][,delta=<value>][,interval=<integer>]...]\n\n\
Generates an SDDS file with a single page and several columns of data.\n\
The default data type is double.  To get a different type, use type=<typeName>.\n\
Each column is specified with a -define option followed by any number of\n\
-sequence options, ending with the end of the command or a new -define\n\
option.\n\
The default value of delta is 1.  The default for the beginning value is\n\
the ending value of the last sequence plus the delta of the last sequence.\n\
Program by Michael Borland. (This is version 2, February 1996.)\n";

#define SEQ_END_GIVEN      0x0001
#define SEQ_BEGIN_GIVEN    0x0002
#define SEQ_NUMBER_GIVEN   0x0004
#define SEQ_DELTA_GIVEN    0x0008
#define SEQ_INTERVAL_GIVEN 0x0010
/* Valid combinations of end, number, and delta: */
#define SEQ_ENDplusDELTA (SEQ_END_GIVEN+SEQ_DELTA_GIVEN)
#define SEQ_ENDplusNUMBER (SEQ_END_GIVEN+SEQ_NUMBER_GIVEN)
#define SEQ_DELTAplusNUMBER (SEQ_DELTA_GIVEN+SEQ_NUMBER_GIVEN)

typedef struct {
  unsigned long flags;
  double begin, end, delta;
  int32_t number, interval;
} SEQUENCE;
typedef struct {
  char *columnName, **item;
  SEQUENCE *sequence;
  long items, sequences, rows, repeats;
  double *data;
} DEFINITION;


void addSequence(char **item, long items, DEFINITION *definition);
void addDefinition(char **item, long items, DEFINITION **definition, long *definitions);
void generateOutput(SDDS_DATASET *outputTable, DEFINITION *definition, long definitions);
void setupOutputFile(SDDS_DATASET *outputTable, char *output,
                     DEFINITION *definition, long definitions);
void createColumn(SDDS_DATASET *outputTable, DEFINITION *definition);

int main(int argc, char **argv)
{
  SDDS_DATASET outputTable;
  SCANNED_ARG *s_arg;
  long i_arg;
  DEFINITION *definition;
  long definitions;

  char *output;
  unsigned long pipeFlags;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2)
    bomb(NULL, USAGE);

  output = NULL;
  pipeFlags = 0;
  definition = NULL;
  definitions = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_DEFINE:
	addDefinition(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &definition, 
		      &definitions);
	break;
      case SET_REPEAT:
	if (definitions==0)
	  SDDS_Bomb("can't give a repeat specifier prior to a definition");
	if (s_arg[i_arg].n_items!=2 ||
	    sscanf(s_arg[i_arg].list[1], "%ld", &definition[definitions-1].repeats)!=1 ||
	    definition[definitions-1].repeats<=0)
	  SDDS_Bomb("invalid -repeat syntax/value");
	break;
      case SET_SEQUENCE:
	if (definitions==0)
	  SDDS_Bomb("can't create a sequence prior to defining the variable");
	addSequence(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, definition+definitions-1);
	break;
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      default:
	fprintf(stderr, "error: unknown/ambiguous option: %s\n", s_arg[i_arg].list[0]);
	exit(1);
	break;
      }
    }
    else {
      if (output==NULL)
	output = s_arg[i_arg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }
  if (output==NULL && !(pipeFlags&USE_STDOUT))
    SDDS_Bomb("no output specified");
        
  if (!definitions)
    SDDS_Bomb("no sequences defined");

  setupOutputFile(&outputTable, output, definition, definitions);
  generateOutput(&outputTable, definition, definitions);
  if (!SDDS_Terminate(&outputTable)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}


void addDefinition(char **item, long items, DEFINITION **definition, long *definitions)
{
  if (items<1)
    SDDS_Bomb("unable to add definition--supply column name");
  if (!(*definition = SDDS_Realloc(*definition, sizeof(**definition)*(*definitions+1))))
    SDDS_Bomb("unable to add definition---allocation failure");
  (*definition)[*definitions].columnName = item[0];
  (*definition)[*definitions].item       = item+1;
  (*definition)[*definitions].items      = items-1;
  (*definition)[*definitions].sequence   = NULL;
  (*definition)[*definitions].sequences  = 0;
  (*definition)[*definitions].rows       = 0;
  (*definition)[*definitions].data       = NULL;
  (*definition)[*definitions].repeats    = 1;
  (*definitions)++;
}

void addSequence(char **item, long items, DEFINITION *definition)
{
  SEQUENCE *sequence;
  long i;

  if (!(definition->sequence = SDDS_Realloc(definition->sequence, 
					    sizeof(*definition->sequence)*
					    (definition->sequences+1))))
    SDDS_Bomb("unable to add sequence--memory allocation failure");
  sequence = definition->sequence + definition->sequences;
  sequence->interval = 1;
  if (!scanItemList(&sequence->flags, item, &items, 0,
		    "begin", SDDS_DOUBLE, &sequence->begin, 1, SEQ_BEGIN_GIVEN,
		    "end", SDDS_DOUBLE, &sequence->end, 1, SEQ_END_GIVEN,
		    "number", SDDS_LONG, &sequence->number, 1, SEQ_NUMBER_GIVEN,
		    "delta", SDDS_DOUBLE, &sequence->delta, 1, SEQ_DELTA_GIVEN,
		    "interval", SDDS_LONG, &sequence->interval, 1, SEQ_INTERVAL_GIVEN,
		    NULL))
    SDDS_Bomb("invalid -sequence syntax");
  if (sequence->flags&SEQ_NUMBER_GIVEN && sequence->number<=0)
    SDDS_Bomb("number<=0 is not valid for -sequence");
  if (sequence->flags&SEQ_DELTA_GIVEN && sequence->delta==0)
    SDDS_Bomb("delta==0 is not valid for -sequence");
  if (!(sequence->flags&SEQ_BEGIN_GIVEN)) {
    if (definition->sequences==0)
      SDDS_Bomb("you must give begin point for the first sequence of a definition");
    if (!(sequence->flags&SEQ_DELTA_GIVEN))
      SDDS_Bomb("you must give delta with implied begin point");
    sequence->begin = definition->sequence[definition->sequences-1].end + sequence->delta;
  }
  if (sequence->flags&SEQ_INTERVAL_GIVEN && sequence->interval<=0)
    SDDS_Bomb("interval for sequence must be >0");
  if ((sequence->flags&SEQ_ENDplusDELTA)==SEQ_ENDplusDELTA) {
    sequence->number = 
      ((long)((sequence->end-sequence->begin)/sequence->delta+1.5))*sequence->interval;
    if (sequence->number<=0)
      SDDS_Bomb("given (start, end, delta) implies number of points <=0");
  }
  else if ((sequence->flags&SEQ_ENDplusNUMBER)==SEQ_ENDplusNUMBER) {
    if (sequence->number==1)
      sequence->delta = 0;
    else
      sequence->delta = (sequence->end-sequence->begin)/(sequence->number-1)*sequence->interval;
  }
  else if ((sequence->flags&SEQ_DELTAplusNUMBER)==SEQ_DELTAplusNUMBER)
    sequence->end = (sequence->delta/sequence->interval)*(sequence->number-1) + sequence->begin;
  else
    SDDS_Bomb("you must supply (end, delta), (end, number), or (delta, number) ");
        
  if (!(definition->data 
	= SDDS_Realloc(definition->data, 
		       sizeof(*definition->data)*(definition->rows+sequence->number))))
    SDDS_Bomb("unable to generate sequence data--allocation failure");
  for (i=0; i<sequence->number; i++)
    definition->data[i+definition->rows] = sequence->begin + (i/sequence->interval)
      *sequence->delta;

  definition->rows += sequence->number;
  definition->sequences++;
}


void generateOutput(SDDS_DATASET *outputTable, DEFINITION *definition, long definitions)
{
  long idef, row, rows, totalRows;
  totalRows = 0;
  for (idef=rows=0; idef<definitions; idef++) {
    totalRows = definition[idef].rows*definition[idef].repeats;
    if (idef && totalRows!=rows)
      fputs("warning: sequences are of different length (sddssequence)\n", stderr);
    if (totalRows>rows)
      rows = totalRows;
  }
  if (rows==0)
    SDDS_Bomb("total number of points in sequence is zero");
  if (!SDDS_StartPage(outputTable, rows))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  for (idef=0; idef<definitions; idef++) {
    if (definition[idef].rows!=totalRows) {
      /* repeats are carried out here */
      if (!(definition[idef].data 
	    = SDDS_Realloc(definition[idef].data, sizeof(*definition[idef].data)*rows))) 
	SDDS_Bomb("unable to generate output--allocation failure");
      for (row=definition[idef].rows; row<totalRows; row++)
	definition[idef].data[row] = definition[idef].data[row%definition[idef].rows];
    }
    if (!SDDS_SetColumnFromDoubles(outputTable, SDDS_BY_NAME,
				   definition[idef].data, rows, definition[idef].columnName))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
    
  if (!SDDS_WritePage(outputTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}


void setupOutputFile(SDDS_DATASET *outputTable, char *output,
                     DEFINITION *definition, long definitions)
{
  long i;

  if (!SDDS_InitializeOutput(outputTable, SDDS_BINARY, 0, NULL, "sddssequence output", output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  for (i=0; i<definitions; i++)
    createColumn(outputTable, definition+i);

  if (!SDDS_WriteLayout(outputTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}

void createColumn(SDDS_DATASET *outputTable, DEFINITION *definition)
{
  char s[SDDS_MAXLINE], *ptr;
  long i;

  if (!definition->columnName) 
    SDDS_Bomb("column name is null");
  if (SDDS_GetColumnIndex(outputTable, definition->columnName)>=0) 
    SDDS_Bomb("column name already exists (ddssequence)");
  sprintf(s, "&column name=%s, ", definition->columnName);
  for (i=0; i<definition->items; i++) {
    if (!(ptr=strchr(definition->item[i], '='))) {
      fprintf(stderr, "error: invalid definition-entry: %s\n", definition->item[i]);
      exit(1);
    }
    *ptr = 0;
    strcat(s, definition->item[i]);
    strcat(s, "=\"");
    strcat(s, ptr+1);
    strcat(s, "\", ");
  }
  if (!strstr(s, ", type="))
    strcat(s, " type=\"double\", ");
  strcat(s, "&end");
  if (!SDDS_ProcessColumnString(outputTable, s, 0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}
