/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddschanges
 * purpose: analyse data from columns of a SDDS file to get changes
 *          from the first page.
 * 
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.18  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.17  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2004/06/16 22:08:33  borland
 Fixed bug that resulted in segmentation violation when the number of rows
 changed between pages in parallel pages mode, or when there were zero rows.

 Revision 1.15  2003/10/21 14:52:41  soliday
 Added the ability to exclude columns that may otherwise be included with
 a wildcard.

 Revision 1.14  2003/06/19 18:21:07  borland
 Added -pass option. This allows the user to ask for data to be passed
 from the input to the output file.  It is different from -copy,
 which transfers data from the reference page (or file) to each output
 page.

 Revision 1.13  2003/06/18 18:35:32  soliday
 Fixed a problem with multiple pages.

 Revision 1.12  2002/12/07 00:17:23  borland
 For backward compatibility, default behavior is not to emit empty pages.
 To force this, use the new option -keepEmpties.
 Also, updated and expanded the usage message.

 Revision 1.11  2002/12/07 00:05:57  borland
 Pages with zero rows are no longer deleted from the output.
 If the data and baseline pages both have zero rows, the program
 no longer exits with an error.

 Revision 1.10  2002/08/14 17:12:40  soliday
 Added Open License

 Revision 1.9  2002/01/16 18:35:46  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.8  2001/01/10 19:35:32  soliday
 Standardized usage message.

 Revision 1.7  1999/12/05 16:26:24  borland
 Updated version message.

 Revision 1.6  1999/12/03 14:28:33  borland
 Added -parallelPages option, which essentially allows subtracting two
 files page-by-page.

 Revision 1.5  1999/05/25 19:05:38  soliday
 Removed compiler warning on linux.

 Revision 1.4  1999/01/06 19:54:38  borland
 Fixed the version number in the usage message.

 * Revision 1.3  1995/09/06  14:56:09  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include "match_string.h"
#include <ctype.h>

#define SET_COPY 0  
#define SET_CHANGESIN 1
#define SET_PASS 2
#define SET_BASELINE 3
#define SET_PIPE 4
#define SET_PARALLELPAGES 5
#define SET_KEEPEMPTIES 6
#define N_OPTIONS 7

char *option[N_OPTIONS] = {
  "copy", "changesin", "pass", "baseline", "pipe", "parallelpages", "keepempties", 
} ;
char *optionSuffix[3] = {
  "", "ChangeIn", "", 
} ;

typedef struct {
  char *columnName;
  long optionCode;
  char *excludeName;
} CHANGE_REQUEST;

/* this structure stores data necessary for accessing/creating SDDS columns and
 * for computing a statistic
 */
typedef struct {
  char *sourceColumn, *resultColumn;
  long optionCode;
  /* these store intermediate values during processing */
  double *baseline, *change;
  void *copy;
  void *pass;
  long type;
} CHANGE_DEFINITION;

long addChangeRequests(CHANGE_REQUEST **request, long requests, char **item, long items, long code, char *excludeName);
CHANGE_DEFINITION *compileChangeDefinitions(SDDS_DATASET *inSet, long *changes,
                                            CHANGE_REQUEST *request, long requests);
long setupOutputFile(SDDS_DATASET *outSet, char *output, SDDS_DATASET *inSet, 
                     CHANGE_DEFINITION *change, long changes);
long addBaselineData(CHANGE_DEFINITION *change, long changes, char *baseline, long page,
                     long lastRows);
long copyBaselineData(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *dataset);
void computeChanges(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *inSet, long rows);
void outputChanges(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *outSet, long rows,
                   SDDS_DATASET *inSet);
long transferDefinitions(SDDS_DATASET *outSet, SDDS_DATASET *inSet, 
			 CHANGE_DEFINITION *change, long changes, long optionCode);

static char *USAGE="sddschanges [-pipe=[input][,output]] [<input>] [<output>]\n\
-changesIn=[exclude=<wildcard>,]<column-names> [-copy=<column-names>]\n\
[-baseline=<filename> [-parallelPages]] [-keepEmpties]\n\n\
Computes the changes in the named columns from the baseline data or\n\
from the first page.  \n\n\
changesIn  Specifies list of optionally wildcarded column names for \n\
           which to compute changes.\n\
copy       Specifies list of optionally wildcarded column names to copy\n\
           from the first page of <input> to all pages of <output>.  By \n\
           default only the requested changes appear in <output>.  \n\
pass       Specifies list of optionally wildcarded column names to copy\n\
           from the each page of <input> to each page of <output>.  By \n\
           default only the requested changes appear in <output>.  \n\
baseline   Specifies optional second file from which to take baseline \n\
           data for computation of changes.  If not given, the first\n\
           page of <input> is used as the reference.\n\
parallelPages\n\
           Used with -baseline, specifies that <input> and baseline\n\
           should be compared page-by-page.  Otherwise, <input> is compared\n\
           to the first page of the baseline data.\n\
keepEmpties\n\
           By default, empty pages in <input> do not appear in <output>.\n\
           Giving this option causes empty pages to be emitted to <output>.\n\n\
Program by Michael Borland. (This is version 5, December 6, 2002.)";

int main(int argc, char **argv)
{
  CHANGE_DEFINITION *change;
  long changes, requests;
  CHANGE_REQUEST *request;
  SCANNED_ARG *scanned;      /* structure for scanned arguments */
  SDDS_DATASET inSet, outSet;
  long i_arg, code, rows, lastRows, baselineRows, parallelPages, keepEmpties, i;
  char *input, *baseline, *output;
  unsigned long pipeFlags;
  char *excludeName=NULL;
  char *ptr=NULL;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3)
    bomb(NULL, USAGE);
        
  input = output = baseline = NULL;
  change = NULL;
  request = NULL;
  changes = requests = baselineRows = 0;
  pipeFlags = 0;
  parallelPages = keepEmpties = 0;
    
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (scanned[i_arg].arg_type==OPTION) {
      /* process options here */
      switch (code=match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COPY:
      case SET_CHANGESIN:
      case SET_PASS:
	if (scanned[i_arg].n_items<2) {
	  fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
	  exit(1);
	}
        if (code == SET_CHANGESIN) {
          if ((ptr=strchr((scanned[i_arg].list+1)[0], '='))) {
            *ptr='\0';
            if (strncmp_case_insensitive((scanned[i_arg].list+1)[0], "exclude", MIN(strlen((scanned[i_arg].list+1)[0]), strlen("exclude"))) == 0) {
              SDDS_CopyString(&excludeName, ++ptr);
            }
            requests = addChangeRequests(&request, requests, 
                                         scanned[i_arg].list+2, scanned[i_arg].n_items-2, code, excludeName);
          } else {
            requests = addChangeRequests(&request, requests, 
                                         scanned[i_arg].list+1, scanned[i_arg].n_items-1, code, NULL);
          }
        } else {
          requests = addChangeRequests(&request, requests, 
                                       scanned[i_arg].list+1, scanned[i_arg].n_items-1, code, NULL);
        }
	break;
      case SET_BASELINE:
	if (scanned[i_arg].n_items!=2) 
	  SDDS_Bomb("invalid -baseline syntax");
	SDDS_CopyString(&baseline, scanned[i_arg].list[1]);
	/*baseline = scanned[i_arg].list[1];*/
	break;
      case SET_PIPE:
	if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_PARALLELPAGES:
	parallelPages = 1;
	break;
      case SET_KEEPEMPTIES:
        keepEmpties = 1;
        break;
      default:
	fprintf(stderr, "error: unknown option '%s' given\n", scanned[i_arg].list[0]);
	exit(1);
	break;
      }
    }
    else {  
      /* argument is filename */
      if (!input)
	SDDS_CopyString(&input, scanned[i_arg].list[0]);
      else if (!output)
	SDDS_CopyString(&output, scanned[i_arg].list[0]);
      else
	SDDS_Bomb("too many filenames seen");
    }
  }
    
  if (parallelPages && !baseline)
    SDDS_Bomb("-parallelPages only makes sense with -baseline");
    
  processFilenames("sddschanges", &input, &output, pipeFlags, 0, NULL);

  if (!requests)
    SDDS_Bomb("no changes requested");
        
  if (!SDDS_InitializeInput(&inSet, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(change=compileChangeDefinitions(&inSet, &changes, request, requests)))
    SDDS_Bomb("unable to compile definitions");

  if (!setupOutputFile(&outSet, output, &inSet, change, changes)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb("unable to setup output file");
  }

  if (baseline && !parallelPages)
    baselineRows = addBaselineData(change, changes, baseline, 0, 0);

  lastRows = 0;
  while ((code=SDDS_ReadPage(&inSet))>0) {
    rows = SDDS_CountRowsOfInterest(&inSet);
    if (baseline && parallelPages)
      baselineRows = addBaselineData(change, changes, baseline, code, lastRows);
    if (!baseline && code==1) {
      baselineRows = copyBaselineData(change, changes, &inSet);
      continue;
    }
    if (rows!=baselineRows)
      SDDS_Bomb("number of rows in file changed");
    if (rows)
      computeChanges(change, changes, &inSet, rows);
    if (rows || keepEmpties)
      outputChanges(change, changes, &outSet, rows, &inSet);
    lastRows = rows;
  }
  if (!SDDS_Terminate(&inSet) || !SDDS_Terminate(&outSet)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (change) {
    for (i = 0; i < changes; i++) {
      if (change[i].baseline)
	free(change[i].baseline);
      if (change[i].change)
	free(change[i].change);
    }
    free(change);
  }
  if (input)
    free(input);
  if (output)
    free(output);
  if (baseline)
    free(baseline);
  if (request)
    free(request);
  free_scanargs(&scanned,argc);
  return(0);
}

long addBaselineData(CHANGE_DEFINITION *change, long changes, char *baseline, 
                     long page, long lastRows)
{
  static SDDS_DATASET dataset;
  long i, rows, code;
  rows = 0;

  if (page==0 || page==1) {
    if (!SDDS_InitializeInput(&dataset, baseline))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if ((code=SDDS_ReadPage(&dataset))<=0 || (rows = SDDS_CountRowsOfInterest(&dataset))<0) {
    SDDS_SetError("Problem reading (next) page of baseline data file");
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (page && code!=page)
    SDDS_Bomb("page mixup in baseline file");
  for (i=0; i<changes; i++) {
    if (change[i].optionCode==SET_CHANGESIN) {
      if (change[i].baseline) {
        if (page>1) 
          free(change[i].baseline);
        change[i].baseline = NULL;
      }
      if (rows &&
          !(change[i].baseline=SDDS_GetColumnInDoubles(&dataset, change[i].sourceColumn))) {
	fprintf(stderr, "problem reading baseline data\n");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    else if (change[i].optionCode==SET_COPY) {
      if (change[i].copy) {
        if (page>1) {
          if (change[i].type==SDDS_STRING && lastRows) 
            SDDS_FreeStringArray(change[i].copy, lastRows);
        }
        free(change[i].copy);
        change[i].copy = NULL;
      }
      if (rows &&
          !(change[i].copy=SDDS_GetColumn(&dataset, change[i].sourceColumn))) {
	fprintf(stderr, "problem reading baseline data\n");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  return rows;
}

long copyBaselineData(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *dataset)
{
  long i, rows;

  if (!(rows = SDDS_CountRowsOfInterest(dataset)))
    SDDS_Bomb("no data in first page of input file");
  for (i=0; i<changes; i++) {
    if (change[i].optionCode==SET_CHANGESIN) {
      if (!(change[i].baseline=SDDS_GetColumnInDoubles(dataset, change[i].sourceColumn))) {
	fprintf(stderr, "problem reading baseline data\n");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    else if (change[i].optionCode==SET_COPY) {
      if (!(change[i].copy=SDDS_GetColumn(dataset, change[i].sourceColumn))) {
	fprintf(stderr, "problem reading baseline data\n");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  return rows;
}

void computeChanges(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *inSet, long rows)
{
  double *data;
  long i, j;
    
  for (i=0; i<changes; i++) {
    switch (change[i].optionCode) {
    case SET_COPY:
      break;
    case SET_PASS:
      break;
    case SET_CHANGESIN:
      if (!(data=SDDS_GetColumnInDoubles(inSet, change[i].sourceColumn))) {
	fprintf(stderr, "problem reading input data\n");
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (change[i].change)
	free(change[i].change);
      change[i].change = (double*)tmalloc(sizeof(*(change[i].change))*rows);
      for (j=0; j<rows; j++) {
	change[i].change[j] = data[j] - change[i].baseline[j];
      }
      free(data);
      break;
    }
  }
}

void outputChanges(CHANGE_DEFINITION *change, long changes, SDDS_DATASET *outSet, long rows,
                   SDDS_DATASET *inSet)
{
  long i;
  void *data;
  
  if (!SDDS_StartPage(outSet, rows)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_CopyParameters(outSet, inSet))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (rows) {
    for (i=0; i<changes; i++) {
      switch (change[i].optionCode) {
      case SET_CHANGESIN:
        if (!SDDS_SetColumnFromDoubles(outSet, SDDS_SET_BY_NAME, change[i].change, rows,
                                       change[i].resultColumn))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      case SET_COPY:
        if (!SDDS_SetColumn(outSet, SDDS_SET_BY_NAME, change[i].copy, rows,
                            change[i].resultColumn))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      case SET_PASS:
        if (!(data = SDDS_GetInternalColumn(inSet, change[i].resultColumn)) ||
            !SDDS_SetColumn(outSet, SDDS_SET_BY_NAME, data, rows,
                            change[i].resultColumn))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      }
    }
  }
  if (!SDDS_WritePage(outSet))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}

long addChangeRequests(CHANGE_REQUEST **request, long requests, char **item, long items, long code, char *excludeName)
{
  long i;
  *request = SDDS_Realloc(*request, sizeof(**request)*(requests+items));
  for (i=0; i<items; i++) {
    (*request)[i+requests].columnName = item[i];
    (*request)[i+requests].optionCode = code;
    (*request)[i+requests].excludeName = excludeName;
  }
  return items+requests;
}

CHANGE_DEFINITION *compileChangeDefinitions(SDDS_DATASET *inSet, long *changes,
                                            CHANGE_REQUEST *request, long requests)
{
  CHANGE_DEFINITION *change;
  long iReq, iChange, iName, index;
  int32_t columnNames;
  char s[SDDS_MAXLINE];
  char **columnName;

  change = tmalloc(sizeof(*change)*requests);
  *changes = iChange = 0;
  for (iReq=0; iReq<requests; iReq++) {
    if (iChange>=*changes)
      change = SDDS_Realloc(change, sizeof(*change)*(*changes+=10));
    if (!has_wildcards(request[iReq].columnName)) {
      if ((index=SDDS_GetColumnIndex(inSet, request[iReq].columnName))<0) {
	sprintf(s, "error: column %s not found input file", 
		request[iReq].columnName);
	SDDS_SetError(s);
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      change[iChange].sourceColumn = request[iReq].columnName;
      change[iChange].optionCode = request[iReq].optionCode;
      change[iChange].change = change[iChange].baseline = NULL;
      change[iChange].type = SDDS_GetColumnType(inSet, index);
      if (change[iChange].optionCode==SET_CHANGESIN &&
	  !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(inSet, index))) {
	fprintf(stderr, "error: column %s is nonnumeric.  Can't compute changes.\n",
		change[iChange].sourceColumn);
	exit(1);
      }
      change[iChange].copy = change[iChange].pass = NULL;
      change[iChange].baseline = change[iChange].change = NULL;
      iChange++;
    } else {
      SDDS_SetColumnFlags(inSet, 0);
      if (!SDDS_SetColumnsOfInterest(inSet, SDDS_MATCH_STRING, request[iReq].columnName,
				     SDDS_OR))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (request[iReq].excludeName) {
        if (!SDDS_SetColumnsOfInterest(inSet, SDDS_MATCH_STRING, request[iReq].excludeName,
                                       SDDS_NEGATE_MATCH|SDDS_AND))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!(columnName = SDDS_GetColumnNames(inSet, &columnNames))) {
	sprintf(s, "no columns selected for wildcard sequence %s", 
		request[iReq].columnName);
	SDDS_SetError(s);
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (iChange+columnNames>*changes)
	change = SDDS_Realloc(change, sizeof(*change)*(*changes=iChange+columnNames+10));
      for (iName=0; iName<columnNames; iName++) {
	change[iChange+iName].sourceColumn = columnName[iName];
	change[iChange+iName].optionCode = request[iReq].optionCode;
	change[iChange+iName].change = change[iChange+iName].baseline = NULL;
	change[iChange+iName].type = SDDS_GetNamedColumnType(inSet, change[iChange].sourceColumn);
	if (change[iChange].optionCode==SET_CHANGESIN &&
	    !SDDS_NUMERIC_TYPE(SDDS_GetNamedColumnType(inSet, change[iChange].sourceColumn))) {
	  fprintf(stderr, "error: column %s is nonnumeric.  Can't compute changes.\n",
		  change[iChange].sourceColumn);
	  exit(1);
	}
	change[iChange].copy = change[iChange].pass = NULL;
	change[iChange].baseline = change[iChange].change = NULL;
      }
      iChange += columnNames;
      free(columnName);
    }
  }

  *changes = iChange;
  for (iChange=0; iChange<*changes; iChange++) {
    switch (change[iChange].optionCode) {
    case SET_COPY:
    case SET_PASS:
      strcpy(s, change[iChange].sourceColumn);
      break;
    case SET_CHANGESIN:
      sprintf(s, "ChangeIn%s", change[iChange].sourceColumn);
      break;
    }
    if (!SDDS_CopyString(&change[iChange].resultColumn, s))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  return change;
}

long setupOutputFile(SDDS_DATASET *outSet, char *output, SDDS_DATASET *inSet, 
                     CHANGE_DEFINITION *change, long changes)
{
  long i;
  int32_t parameters;
  char **parameter=NULL, s[SDDS_MAXLINE];

  if (!SDDS_InitializeOutput(outSet, SDDS_BINARY, 0, NULL, "sddschanges output", output))
    return 0;
    
  if (!transferDefinitions(outSet, inSet, change, changes, SET_CHANGESIN))
    return 0;
  if (!transferDefinitions(outSet, inSet, change, changes, SET_COPY))
    return 0;
  if (!transferDefinitions(outSet, inSet, change, changes, SET_PASS))
    return 0;
  if ((parameter=SDDS_GetParameterNames(inSet, &parameters)) && parameters) {
    for (i=0; i<parameters; i++) 
      if (!SDDS_TransferParameterDefinition(outSet, inSet, parameter[i], NULL))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<parameters; i++)
      free(parameter[i]);
    free(parameter);
  }
  if (!SDDS_WriteLayout(outSet)) {
    sprintf(s, "Unable complete setup of output file\n");
    SDDS_SetError(s);
    return 0;
  }
  return 1;
}

long transferDefinitions(SDDS_DATASET *outSet, SDDS_DATASET *inSet, 
			 CHANGE_DEFINITION *change, long changes, long optionCode)
{
  long column;
  char s[SDDS_MAXLINE], *symbol;

  for (column=0; column<changes; column++) {
    if (optionCode!=change[column].optionCode)
      continue;
    if (!SDDS_TransferColumnDefinition(outSet, inSet, change[column].sourceColumn, 
				       change[column].resultColumn)) {
      sprintf(s, "Problem transferring definition of column %s\n",
	      change[column].sourceColumn);
      SDDS_SetError(s);
      return 0;
    }
    /*
      if (strcmp(change[column].sourceColumn, change[column].resultColumn)!=0 &&
      !SDDS_ChangeColumnInformation(outSet, "name", change[column].resultColumn,
      SDDS_SET_BY_NAME, change[column].sourceColumn)) {
      sprintf(s, "Unable to rename column %s to %s\n",
      change[column].sourceColumn, change[column].resultColumn);
      SDDS_SetError(s);
      return 0;
      }
    */
    if (!SDDS_ChangeColumnInformation(outSet, "description", NULL,  
				      SDDS_SET_BY_NAME, change[column].resultColumn) ||
	SDDS_GetColumnInformation(outSet, "symbol", &symbol, SDDS_BY_NAME,
				  change[column].resultColumn)!=SDDS_STRING) {
      sprintf(s, "Unable get/modify column %s information\n",
	      change[column].sourceColumn);
      SDDS_SetError(s);
      return 0;
    }
    if (!symbol)
      SDDS_CopyString(&symbol, change[column].sourceColumn);
    switch (change[column].optionCode) {
    case SET_COPY:
    case SET_PASS:
      strcpy(s, symbol);
      break;
    case SET_CHANGESIN:
      sprintf(s, "ChangeIn[%s]", symbol);
      break;
    default:
      SDDS_Bomb("invalid option code (transferDefinitions)");
      break;
    }
    if (!SDDS_ChangeColumnInformation(outSet, "symbol", s, SDDS_BY_NAME,
				      change[column].resultColumn))
      return 0;
  }
  return 1;
}

