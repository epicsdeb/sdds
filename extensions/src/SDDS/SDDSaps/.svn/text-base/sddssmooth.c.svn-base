/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssmooth
 * purpose: SDDS-format smoothing program
 *
 * Michael Borland, 1995
 * based on mpl-format smoot program
 $Log: not supported by cvs2svn $
 Revision 1.22  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.21  2005/11/07 21:48:11  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.20  2005/11/04 22:46:18  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.19  2004/07/29 18:25:18  soliday
 Updated to work with new option in despikeData

 Revision 1.18  2002/08/14 17:12:55  soliday
 Added Open License

 Revision 1.17  2002/08/09 13:56:56  borland
 Number of passes now applies to Savitzky-Golay filtering.

 Revision 1.16  2002/06/19 23:13:00  borland
 Fixed spelling of Savitzky-Golay.

 Revision 1.15  2002/01/16 18:29:51  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.14  2001/01/10 19:35:48  soliday
 Standardized usage message.

 Revision 1.13  1999/06/18 18:04:27  borland
 Added Savitzky-Golay smoothing and differentiation.

 Revision 1.12  1999/05/25 19:16:22  soliday
 Removed compiler warning on linux.

 Revision 1.11  1997/06/12 21:23:48  borland
 Fixed bug in previous revision.

 Revision 1.10  1997/06/12 21:17:41  borland
 Added -nowarnings option.

 Revision 1.9  1996/10/30 18:08:02  borland
 Now detects presence of some parameters before trying to define them.

 Revision 1.8  1996/10/11 22:56:12  borland
 Initialized despikeAverageOf before scanning despike args.  Added more info
 for bad despike parameters.

 Revision 1.7  1996/10/04 18:58:47  borland
 Removed smoothData() and despikeData() (now in mdbmth library).  Improved
 despiking algorithm and control.

 * Revision 1.6  1996/02/17  19:22:17  borland
 * Added better diagnostic messages in the event of an out-of-range index
 * in despiking routine.
 *
 * Revision 1.5  1996/02/14  01:05:38  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1995/10/10  23:55:54  borland
 * Added -despike option and despikeData routine to implement it.
 *
 * Revision 1.3  1995/09/06  14:57:13  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include <ctype.h>

#define CLO_POINTS 0
#define CLO_PASSES 1
#define CLO_COLUMNS 2
#define CLO_PIPE 3
#define CLO_NEWCOLUMNS 4
#define CLO_DIFFERENCECOLUMNS 5
#define CLO_DESPIKE 6
#define CLO_NOWARNINGS 7
#define CLO_SAVITZKYGOLAY 8
#define CLO_OPTIONS 9

char *option[CLO_OPTIONS] = {
  "points", "passes", "columns", "pipe", "newcolumns",
  "differencecolumns", "despike", "nowarnings",
  "savitzkygolay"
};

static char *USAGE = "sddssmooth [<inputfile>] [<outputfile>] [-pipe=[input][,output]] \n\
-columns=<name>[,...] [-points=<oddInteger>] [-passes=<integer>]\n\
[-despike[=neighbors=<integer>][,passes=<integer>][,averageOf=<integer>][,threshold=<value>]]\n\
[-SavitzkyGolay=<left>,<right>,<order>[,<derivativeOrder>]]\n\
[-newColumns] [-differenceColumns] [-nowarnings] \n\n\
Program by Michael Borland.  (This is version 5, June 1997.)\n";

long resolveColumnNames(SDDS_DATASET *SDDSin, char ***column, int32_t *columns);

#define FL_NEWCOLUMNS 0x0001UL
#define FL_DIFCOLUMNS 0x0002UL

#define DESPIKE_AVERAGEOF    0x0001U

int main(int argc, char **argv)
{
  int iArg;
  char **inputColumn, **outputColumn, **difColumn;
  int32_t columns;
  long despike;
  int32_t despikeNeighbors, despikePasses, despikeAverageOf;
  char *input, *output;
  long i, j, rows, readCode;
  int32_t smoothPoints, smoothPasses;
  long noWarnings;
  unsigned long pipeFlags, flags, despikeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double *data, despikeThreshold;
  int32_t SGLeft, SGRight, SGOrder, SGDerivOrder;
    
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3 || argc>(3+CLO_OPTIONS)) 
    bomb(NULL, USAGE);

  output = input = NULL;
  inputColumn = outputColumn = NULL;
  columns = 0;
  pipeFlags = 0;
  smoothPoints = 3;
  smoothPasses = 1;
  flags = 0;
  despike = 0;
  noWarnings = 0;
  SGOrder = -1;
  SGDerivOrder = 0;
    
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_PASSES:
	if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%" SCNd32, &smoothPasses)!=1 ||
	    smoothPasses<0)
	  SDDS_Bomb("invalid -passes syntax/value");
	break;
      case CLO_POINTS:
	if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%" SCNd32, &smoothPoints)!=1 ||
	    smoothPoints<1 || smoothPoints%2==0)
	  SDDS_Bomb("invalid -points syntax/value");
	break;
      case CLO_COLUMNS:
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -columns syntax");
	inputColumn = tmalloc(sizeof(*inputColumn)*(columns = scanned[iArg].n_items-1));
	for (i=0; i<columns; i++)
	  inputColumn[i] = scanned[iArg].list[i+1];
	break;
      case CLO_PIPE:
	if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case CLO_NEWCOLUMNS:
	flags |= FL_NEWCOLUMNS;
	break;
      case CLO_DIFFERENCECOLUMNS:
	flags |= FL_DIFCOLUMNS;
	break;
      case CLO_DESPIKE:
	scanned[iArg].n_items--;
	despikeNeighbors = 4;
	despikePasses = 1;
	despikeThreshold = 0;
	despikeAverageOf = 2;
	if (scanned[iArg].n_items>0 &&
	    (!scanItemList(&despikeFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
			   "neighbors", SDDS_LONG, &despikeNeighbors, 1, 0, 
			   "passes", SDDS_LONG, &despikePasses, 1, 0,
			   "averageof", SDDS_LONG, &despikeAverageOf, 1, DESPIKE_AVERAGEOF,
			   "threshold", SDDS_DOUBLE, &despikeThreshold, 1, 0,
			   NULL) ||
	     despikeNeighbors<2 || despikePasses<1 || despikeAverageOf<2 || 
	     despikeThreshold<0)) {
	  fprintf(stderr, 
		  "sddssmooth: Invalid -despike syntax/values: neighbors=%" PRId32 ", passes=%" PRId32 ", averageOf=%" PRId32 ", threshold=%e\n", 
		  despikeNeighbors, despikePasses, despikeAverageOf, despikeThreshold);
	  exit(1);
	}
	if (!(despikeFlags&DESPIKE_AVERAGEOF))
	  despikeAverageOf = despikeNeighbors;
	if (despikeAverageOf>despikeNeighbors)
	  SDDS_Bomb("invalid -despike syntax/values: averageOf>neighbors");
	despike = 1;
	break;
      case CLO_NOWARNINGS:
	noWarnings = 1;
	break;
      case CLO_SAVITZKYGOLAY:
	if ((scanned[iArg].n_items!=4 && scanned[iArg].n_items!=5) ||
	    sscanf(scanned[iArg].list[1], "%" SCNd32, &SGLeft)!=1 ||
	    sscanf(scanned[iArg].list[2], "%" SCNd32, &SGRight)!=1 ||
	    sscanf(scanned[iArg].list[3], "%" SCNd32, &SGOrder)!=1 ||
	    (scanned[iArg].n_items==5 &&
	     sscanf(scanned[iArg].list[4], "%" SCNd32, &SGDerivOrder)!=1) ||
	    SGLeft<0 || SGRight<0 || (SGLeft+SGRight)<SGOrder ||
	    SGOrder<0 || SGDerivOrder<0)
	  SDDS_Bomb("invalid -SavitzkyGolay syntax/values");
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

  processFilenames("sddssmooth", &input, &output, pipeFlags, 0, NULL);

  if (!despike && !smoothPasses && !noWarnings)
    fprintf(stderr, "warning: smoothing parameters won't result in any change in data (sddssmooth)\n");

  if (!columns)
    SDDS_Bomb("supply the names of columns to smooth with the -columns option");

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!resolveColumnNames(&SDDSin, &inputColumn, &columns))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!columns)
    SDDS_Bomb("no columns selected for smoothing");

  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  outputColumn = tmalloc(sizeof(*outputColumn)*columns);
    
  if (flags&FL_NEWCOLUMNS) {
    for (i=0; i<columns; i++) {
      if (SGDerivOrder<=0) {
	outputColumn[i] = tmalloc(sizeof(**outputColumn)*(strlen(inputColumn[i])+1+strlen("Smoothed")));
	sprintf(outputColumn[i], "%sSmoothed", inputColumn[i]);
      } else {
	outputColumn[i] = tmalloc(sizeof(**outputColumn)*(strlen(inputColumn[i])+1+strlen("SmoothedDeriv"))+5);
	sprintf(outputColumn[i], "%sSmoothedDeriv%" PRId32, inputColumn[i], SGDerivOrder);
      }
      if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, inputColumn[i], outputColumn[i]))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  else
    for (i=0; i<columns; i++)
      outputColumn[i] = inputColumn[i];

  difColumn = NULL;
  if (flags&FL_DIFCOLUMNS) {
    difColumn = tmalloc(sizeof(*difColumn)*columns);
    for (i=0; i<columns; i++) {
      difColumn[i] = tmalloc(sizeof(**difColumn)*(strlen(inputColumn[i])+1+strlen("Unsmooth")));
      sprintf(difColumn[i], "%sUnsmooth", inputColumn[i]);
      if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, inputColumn[i], difColumn[i]))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }

  if ((SDDS_GetParameterIndex(&SDDSout, "SmoothPoints")<0 &&
       SDDS_DefineParameter1(&SDDSout, "SmoothPoints", NULL, NULL, NULL,
			     NULL, SDDS_LONG, &smoothPoints)<0) || 
      (SDDS_GetParameterIndex(&SDDSout, "SmoothPasses")<0 && 
       SDDS_DefineParameter1(&SDDSout, "SmoothPasses", NULL, NULL, NULL,
			     NULL, SDDS_LONG, &smoothPasses)<0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if (!SDDS_CopyPage(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))) {
      for (i=0; i<columns; i++) {
	if (!(data = SDDS_GetColumnInDoubles(&SDDSin, inputColumn[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (despike)
	  despikeData(data, rows, despikeNeighbors, despikePasses, 
		      despikeAverageOf, despikeThreshold, 0);
	if (SGOrder>=0) {
          long pass = 0;
          for (pass=0; pass<smoothPasses; pass++)
            SavitzkyGolaySmooth(data, rows, SGOrder, SGLeft, SGRight, SGDerivOrder);
        } else if (smoothPasses)
	  smoothData(data, rows, smoothPoints, smoothPasses);
	if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, data, rows, outputColumn[i]))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (flags&FL_DIFCOLUMNS) {
	  double *data0;
	  if (!(data0 = SDDS_GetColumnInDoubles(&SDDSin, inputColumn[i])))
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	  for (j=0; j<rows; j++)
	    data0[j] -= data[j];
	  if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, data0, rows, difColumn[i]))
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	  free(data0);
	}
	free(data);
      }
    }
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}


long resolveColumnNames(SDDS_DATASET *SDDSin, char ***column, int32_t *columns)
{
  long i;

  SDDS_SetColumnFlags(SDDSin, 0);
  for (i=0; i< *columns; i++) {
    if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, (*column)[i], SDDS_OR))
      return 0;
  }
  free(*column);
  if (!(*column = SDDS_GetColumnNames(SDDSin, columns)) || *columns==0) {
    SDDS_SetError("no columns found");
    return 0;
  }
  return 1; 
}



