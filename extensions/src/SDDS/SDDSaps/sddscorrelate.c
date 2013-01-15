/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddscorrelate
 * purpose: compute and evaluate correlations among columns of data
 *
 * Michael Borland, 1994, 1995
 $Log: not supported by cvs2svn $
 Revision 1.12  2004/10/13 15:25:11  borland
 Use a more reliable algorithm for computing the standard deviation in doing
 outlier elimination.

 Revision 1.11  2002/08/14 17:12:42  soliday
 Added Open License

 Revision 1.10  2000/11/21 17:17:35  soliday
 Added missing SDDS_Terminate procedures.

 Revision 1.9  2000/04/13 18:01:44  soliday
 Removed invalid prototypes.

 Revision 1.8  2000/04/13 17:09:32  soliday
 Added missing include statment.

 Revision 1.7  1999/05/25 19:07:38  soliday
 Removed compiler warning on linux.

 Revision 1.6  1999/01/06 19:54:40  borland
 Fixed the version number in the usage message.

 * Revision 1.5  1996/02/14  01:05:14  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1995/12/10  02:33:20  borland
 * Added outlier elimination.
 *
 * Revision 1.3  1995/09/06  14:56:18  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <ctype.h>

#define SET_COLUMNS 0
#define SET_EXCLUDE 1
#define SET_WITHONLY 2
#define SET_PIPE 3
#define SET_RANKORDER 4
#define SET_STDEVOUTLIER 5
#define N_OPTIONS 6

char *option[N_OPTIONS] = {
    "columns", "excludecolumns", "withonly", "pipe", "rankorder",
    "stdevoutlier",
    };

#define USAGE "sddscorrelate [-pipe=[input][,output]] [<inputfile>] [<outputfile>] \n\
[-columns=<list-of-names>] [-excludeColumns=<list-of-names>] [-withOnly=<name>]\n\
[-rankOrder] [-stDevOutlier[=limit=<factor>][,passes=<integer>]]\n\n\
Program by Michael Borland.  (This is version 5, February 1996.)\n"

void replaceWithRank(double *data, long n);
double *findRank(double *data, long n);
void markStDevOutliers(double *data, double limit, long passes, short *keep, long n);

int main(int argc, char **argv)
{
  int iArg;
  char **column, **excludeColumn, *withOnly;
  long columns, excludeColumns;
  char *input, *output;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  long i, j, row, rows, count, readCode, rankOrder, iName1, iName2;
  int32_t outlierStDevPasses;
  double **data, correlation, significance, outlierStDevLimit;
  double **rank;
  short **accept;
  char s[SDDS_MAXLINE];
  unsigned long pipeFlags, dummyFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2)
    bomb(NULL, USAGE);
  
  output = input = withOnly = NULL;
  columns = excludeColumns = 0;
  column = excludeColumn = NULL;
  pipeFlags = 0;
  rankOrder = 0;
  outlierStDevPasses = 0;
  outlierStDevLimit  = 1.;
  rank = NULL;
  accept = NULL;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_COLUMNS:
	if (columns)
	  SDDS_Bomb("only one -columns option may be given");
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -columns syntax");
	column = tmalloc(sizeof(*column)*(columns=scanned[iArg].n_items-1));
	for (i=0; i<columns; i++)
	  column[i] = scanned[iArg].list[i+1];
	break;
      case SET_EXCLUDE:
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -excludeColumns syntax");
	moveToStringArray(&excludeColumn, &excludeColumns, scanned[iArg].list+1, scanned[iArg].n_items-1);
	break;
      case SET_WITHONLY:
	if (withOnly)
	  SDDS_Bomb("only one -withOnly option may be given");
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -withOnly syntax");
	withOnly = scanned[iArg].list[1];
	break;
      case SET_PIPE:
	if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_RANKORDER:
	rankOrder = 1;
	break;
      case SET_STDEVOUTLIER:
	scanned[iArg].n_items--;
	outlierStDevPasses = 1;
	outlierStDevLimit = 1;
	if (!scanItemList(&dummyFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
			  "limit", SDDS_DOUBLE, &outlierStDevLimit, 1, 0,
			  "passes", SDDS_LONG, &outlierStDevPasses, 1, 0,
			  NULL) || 
	    outlierStDevPasses<=0 || outlierStDevLimit<=0)
	  SDDS_Bomb("invalid -stdevOutlier syntax/values");
	break;
      default:
	fprintf(stderr, "error: unknown/ambiguous option: %s\n", 
		scanned[iArg].list[0]);
	exit(1);
	break;
      }
    }
    else {
      if (!input)
	input = scanned[iArg].list[0];
      else if (!output)
	output = scanned[iArg].list[0];
      else
	SDDS_Bomb("too many filenames seen");
    }
  }
  
  processFilenames("sddscorrelate", &input, &output, pipeFlags, 0, NULL);
  
  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (!columns)
    columns = appendToStringArray(&column, columns, "*"); 
  if (withOnly)
    columns = appendToStringArray(&column, columns, withOnly);
  
  if ((columns=expandColumnPairNames(&SDDSin, &column, NULL, columns, 
				     excludeColumn, excludeColumns, FIND_NUMERIC_TYPE, 0))<=0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    SDDS_Bomb("no columns selected for correlation analysis");
  }
  
  if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, "sddscorrelate output", output) ||
      SDDS_DefineColumn(&SDDSout, "Correlate1Name", NULL, NULL, "Name of correlated quantity 1", NULL, SDDS_STRING, 0)<0 ||
      SDDS_DefineColumn(&SDDSout, "Correlate2Name", NULL, NULL, "Name of correlated quantity 2", NULL, SDDS_STRING, 0)<0 ||
      SDDS_DefineColumn(&SDDSout, "CorrelatePair", NULL, NULL, "Names of correlated quantities", NULL, SDDS_STRING, 0)<0 ||
      SDDS_DefineColumn(&SDDSout, "CorrelationCoefficient", "r", NULL, "Linear correlation coefficient", 
			NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(&SDDSout, "CorrelationSignificance", "P$br$n", NULL, "Linear correlation coefficient significance", 
			NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(&SDDSout, "CorrelationPoints", NULL, NULL, "Number of points used for correlation",
			NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineParameter(&SDDSout, "CorrelatedRows", NULL, NULL, "Number of data rows in correlation analysis", 
			   NULL, SDDS_LONG, NULL)<0 ||
      SDDS_DefineParameter(&SDDSout, "sddscorrelateInputFile", NULL, NULL, "Data file processed by sddscorrelate", 
			   NULL, SDDS_STRING, input?input:"stdin")<0 ||
      SDDS_DefineParameter(&SDDSout, "sddscorrelateMode", NULL, NULL, NULL, NULL, SDDS_STRING,
			   rankOrder?"Rank-Order (Spearman)":"Linear (Pearson)")<0 ||
      SDDS_DefineParameter1(&SDDSout, "sddscorrelateStDevOutlierPasses", NULL, NULL, 
			    "Number of passes of standard-deviation outlier elimination applied",
			    NULL, SDDS_LONG, &outlierStDevPasses)<0 ||
      SDDS_DefineParameter1(&SDDSout, "sddscorrelateStDevOutlierLimit", NULL, NULL, 
			    "Standard-deviation outlier limit applied",
			    NULL, SDDS_DOUBLE, &outlierStDevLimit)<0 ||
      !SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (!(data = (double**)malloc(sizeof(*data)*columns)) ||
      (rankOrder && !(rank = (double**)malloc(sizeof(*rank)*columns))) ||
      !(accept = (short**)malloc(sizeof(*accept)*columns)))
    SDDS_Bomb("allocation failure");
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<3)
      continue;
    if (!SDDS_StartPage(&SDDSout, columns*(columns-1)/2) ||
	!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, 
			    "CorrelatedRows", rows, NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<columns; i++) {
      if (!(data[i] = SDDS_GetColumnInDoubles(&SDDSin, column[i])))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (rankOrder)
	rank[i] = findRank(data[i], rows);
      if (outlierStDevPasses) {
	if (!(accept[i] = (short*)malloc(sizeof(**accept)*rows)))
	  SDDS_Bomb("allocation failure");
	markStDevOutliers(data[i], outlierStDevLimit, outlierStDevPasses,
			  accept[i], rows);
      }
      else
	accept[i] = NULL;
    }
    for (i=row=0; i<columns; i++) {
      for (j=i+1; j<columns; j++) {
	iName1 = i;
	iName2 = j;
	if (withOnly) {
	  if (strcmp(withOnly, column[i])==0) {
	    iName1 = j;
	    iName2 = i;
	  }
	  else if (strcmp(withOnly, column[j])==0) {
	    iName1 = i;
	    iName2 = j;
	  }
	  else 
	    continue;
	}
	correlation 
	  = linearCorrelationCoefficient(rankOrder?rank[i]:data[i], 
					 rankOrder?rank[j]:data[j], 
					 accept[i], accept[j], rows, &count);
	significance = linearCorrelationSignificance(correlation, count);
	sprintf(s, "%s.%s", column[iName1], column[iName2]);
                if (!SDDS_SetRowValues(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, row++,
                                       0, column[iName1], 
                                       1, column[iName2], 
                                       2, s, 3, correlation, 4, significance, 
                                       5, count, -1))
		  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    for (i=0; i<columns; i++) {
      free(data[i]);
      if (rankOrder)
	free(rank[i]);
      if (accept[i])
	free(accept[i]);
    }
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (!SDDS_Terminate(&SDDSin)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  return 0;
}

void markStDevOutliers(double *data, double limit, long passes, short *keep, long n)
{
  double sum1, sum2, variance, mean, absLimit;
  long pass, i, summed, kept;
  
  for (i=0; i<n; i++)
    keep[i] = 1;
  kept = n;
  for (pass=0; pass<passes && kept; pass++) {
    for (i=summed=sum1=0; i<n; i++) {
      if (keep[i]) {
        sum1 += data[i];
        summed += 1;
      }
    }
    if (summed<2)
      return;
    mean = sum1/summed;
    for (i=sum2=0; i<n; i++) {
      if (keep[i])
        sum2 += sqr(data[i]-mean);
    }
    variance = sum2/summed;
    if (variance>0) {
      absLimit = limit*sqrt(variance);
      for (i=0; i<n; i++)
	if (keep[i] && fabs(data[i]-mean)>absLimit) {
	  keep[i] = 0;
	  kept--;
	}
    }
  }
}


typedef struct {
  double data;
  long originalIndex;
} DATAnINDEX;

int compareData(const void *d1, const void *d2)
{
  double diff;
  diff = ((DATAnINDEX*)d1)->data - ((DATAnINDEX*)d2)->data;
  return diff==0? 0 : (diff<0 ? -1 : 1 );
}

double *findRank(double *data, long n)
{
  long i;
  double *rank;
  
  if (!(rank = (double*)malloc(sizeof(*rank)*n)))
    return NULL;
  for (i=0; i<n; i++)
    rank[i] = data[i];
  replaceWithRank(rank, n);
  return rank;
}

void replaceWithRank(double *data, long n)
{
  static DATAnINDEX *indexedData = NULL;
  long i, j, iStart, iEnd;
  indexedData = SDDS_Realloc(indexedData, sizeof(*indexedData)*n);
  for (i=0; i<n; i++) {
    indexedData[i].data = data[i];
    indexedData[i].originalIndex = i;
  }
  qsort((void*)indexedData, n, sizeof(*indexedData), compareData);
  for (i=0; i<n; i++)
    data[indexedData[i].originalIndex] = i;
  for (i=0; i<n-1; i++) {
    if (data[i]==data[i+1]) {
      iStart = i;
      for (j=i+2; j<n-1; j++)
	if (data[i]!=data[j])
	  break;
      iEnd = j-1;
      for (j=iStart; j<=iEnd; j++)
	data[i] = (iStart+iEnd)/2.0;
    }
  }
}

