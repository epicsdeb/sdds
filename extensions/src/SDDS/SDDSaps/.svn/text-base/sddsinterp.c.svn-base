/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsinterp
 * purpose: SDDS-format interpolation program
 *
 * Michael Borland, 1995
 * based on mpl-format interpolation program
 $Log: not supported by cvs2svn $
 Revision 1.30  2011/02/21 16:15:48  shang
 added documentation for interpShort option, and changed the type of returnCode in interp_short to "unsigned long" to be consistent with interp()

 Revision 1.29  2011/02/20 19:41:30  borland
 Fixed uninitialized variable bug.

 Revision 1.28  2011/01/28 16:39:37  shang
 added interp_short option to interpolate short values.

 Revision 1.27  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.26  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.25  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.24  2003/10/07 16:25:32  shang
 moved the interpolate() to mdbmth area, and the definition of outrange structure to
 mdb.h

 Revision 1.23  2003/09/02 19:16:05  soliday
 Cleaned up code for Linux.

 Revision 1.22  2003/08/26 18:02:41  borland
 Added -equispaced option, which is very similar to the -sequence option,
 expect instead of giving the number of points we give the point spacing.

 Revision 1.21  2002/08/14 17:12:48  soliday
 Added Open License

 Revision 1.20  2002/02/13 15:00:03  shang
 updated the version message

 Revision 1.19  2002/02/13 14:56:31  shang
 modified interpolate() function to make it work correctly in out of range cases.

 Revision 1.18  2002/01/16 17:50:09  soliday
 Added missing SDDS_terminate and free functions.

 Revision 1.17  2001/05/03 20:55:33  soliday
 Standardized usage messages.

 Revision 1.16  2001/01/10 19:35:40  soliday
 Standardized usage message.

 Revision 1.15  2000/11/21 17:17:36  soliday
 Added missing SDDS_Terminate procedures.

 Revision 1.14  2000/04/13 18:02:48  soliday
 Removed invalid prototypes.

 Revision 1.13  2000/04/13 17:10:48  soliday
 Added missing include statment.

 Revision 1.12  1999/11/22 21:47:02  borland
 Changed new -equidistant option to -fillIn, which is more descriptive.

 Revision 1.11  1999/11/22 21:41:04  borland
 Added -equidistant option for interpolation at equidistant points with spacing
 equal to minimum spacing in page.

 Revision 1.10  1999/05/25 19:11:02  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/04/05 18:05:09  borland
 Added -forceMonotonicity option.

 Revision 1.8  1999/01/06 19:54:47  borland
 Fixed the version number in the usage message.

 Revision 1.7  1998/12/16 21:26:05  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.6  1996/10/25 18:35:40  borland
 Fixed bug that occured when main file had multiple pages and values file
 had one.  Wasn't setting number of rows or getting new list of independent
 quantities.

 * Revision 1.5  1996/02/14  01:05:26  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1995/12/15  06:36:13  borland
 * Added transfer of parameters from the input file to the output file.
 *
 * Revision 1.3  1995/09/06  14:56:41  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <ctype.h>

#define CLO_ORDER 0
#define CLO_ATVALUES 1
#define CLO_SEQUENCE 2
#define CLO_COLUMNS 3
#define CLO_PRINTOUT 4
#define CLO_FILEVALUES 5
#define CLO_COMBINEDUPLICATES 6
#define CLO_BRANCH 7
#define CLO_BELOWRANGE 8
#define CLO_ABOVERANGE 9
#define CLO_PIPE 10
#define CLO_EXCLUDE 11
#define CLO_FORCEMONOTONIC 12
#define CLO_FILLIN 13
#define CLO_EQUISPACED 14
#define CLO_INTERP_SHORT 15
#define CLO_OPTIONS 16

char *option[CLO_OPTIONS] = {
  "order", "atvalues", "sequence", "columns", 
  "printout", "filevalues", "combineduplicates", 
  "branch", "belowrange", "aboverange", "pipe",
  "exclude", "forcemonotonic", "fillin", "equispaced", "interpShort"
};

static char *USAGE = "sddsinterp [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
 [-columns=<independent-quantity>,<name>[,...]]\n\
 [-exclude=<name>[,...]]\n\
 [{-atValues=<values-list> | \n\
   -sequence=<points>[,<start>,<end>] | \n\
   -equispaced=<spacing>[,<start>,<end>] \n\
   -fillIn | \n\
   -fileValues=<valuesfile>[,column=<column-name>][,parallelPages]}] \n\
 [-interpShort=-1|-2] \n\
 [-order=<number>] [-printout[=bare][,stdout]]\n\
 [-forceMonotonic[={increasing|decreasing}]] \n\
 [-belowRange={value=<value>|skip|saturate|extrapolate|wrap}[,{abort|warn}]]\n\
 [-aboveRange={value=<value>|skip|saturate|extrapolate|wrap}[,{abort|warn}]]\n\
-interpshort    for interpolating short columns, with order of -1 or -2; \n\
                order=-1, means the interpolating point inheriting value from previous point; \n\
                order=-2, means the interpolating point inheriting value from next point.\n\n\
    Program by Michael Borland.  (This is version 1.20, August 2003.)\n";

double *makeSequence(long *points, double start, double end, double spacing, double *data, long dataRows);
double *makeFillInSequence(double *x, long dataRows, long *nPointsReturn);
long checkMonotonicity(double *indepValue, long rows);
long forceMonotonicity(double *indepValue, double **y, long columns, long rows,
     unsigned long direction);
long combineDuplicatePoints(double *x, double **y, long columns, long rows, double tolerance);

#define FILEVALUES_PARALLEL_PAGES 0x0001U

#define NORMAL_PRINTOUT 1
#define BARE_PRINTOUT 2
#define STDOUT_PRINTOUT 4

#define FORCE_MONOTONIC  0x0001U
#define FORCE_INCREASING 0x0002U
#define FORCE_DECREASING 0x0004U

int main(int argc, char **argv)
{
  int iArg;
  char *indepQuantity, **depenQuantity, *fileValuesQuantity, *fileValuesFile, **exclude;
  long depenQuantities, monotonicity, excludes;
  char *input, *output;
  long i, j, rows, readCode, order, valuesReadCode, fillIn, row;
  long sequencePoints, combineDuplicates, branch;
  int32_t *rowFlag;
  double sequenceStart, sequenceEnd;
  double sequenceSpacing;
  unsigned long flags, interpCode, printFlags, forceMonotonic;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout, SDDSvalues;
  OUTRANGE_CONTROL aboveRange, belowRange;
  double *atValue;
  long atValues, interpPoints, doNotRead, parallelPages;
  double *indepValue, **depenValue, *interpPoint, **outputData;
  unsigned long pipeFlags;
  FILE *fpPrint;
  short interpShort=0, interpShortOrder=-1, *shortValue=NULL;
  long nextPos;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3 || argc>(3+CLO_OPTIONS)) 
    bomb(NULL, USAGE);

  atValue = NULL;
  atValues = fillIn = 0;
  output = input = NULL;
  combineDuplicates = branch = sequencePoints = parallelPages = 0;
  indepQuantity = NULL;
  depenQuantity = exclude = NULL;
  depenQuantities = excludes = 0;
  aboveRange.flags = belowRange.flags = OUTRANGE_SATURATE;
  order = 1;
  readCode = interpPoints = 0;
  fileValuesFile = fileValuesQuantity = NULL;
  sequenceStart = sequenceEnd = sequenceSpacing = 0;
  printFlags = pipeFlags = 0;
  forceMonotonic = 0;
  indepValue = interpPoint = NULL;
  depenValue = outputData = NULL;

  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_ORDER:
        if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%ld", &order)!=1 ||
            order<1)
          SDDS_Bomb("invalid -order syntax/value");
        break;
      case CLO_ATVALUES:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -atValues syntax");
        if (atValue)
          SDDS_Bomb("give -atValues only once");
        atValue = tmalloc(sizeof(*atValue)*(atValues=scanned[iArg].n_items-1));
        for (i=0; i<atValues; i++)
          if (sscanf(scanned[iArg].list[i+1], "%lf", &atValue[i])!=1)
            SDDS_Bomb("invalid -atValues value");
        break;
      case CLO_INTERP_SHORT:
        if (scanned[iArg].n_items==2) {
          if (sscanf(scanned[iArg].list[1], "%hd", &interpShortOrder)!=1)
            SDDS_Bomb("invalid -interpShort value");
        }
        interpShort = 1;
        break;
      case CLO_SEQUENCE:
        if ((scanned[iArg].n_items!=2 && scanned[iArg].n_items!=4) ||
            sscanf(scanned[iArg].list[1], "%ld", &sequencePoints)!=1 || sequencePoints<2)
          SDDS_Bomb("invalid -sequence syntax/value");
        if (scanned[iArg].n_items==4 &&
            (sscanf(scanned[iArg].list[2], "%lf", &sequenceStart)!=1 ||
             sscanf(scanned[iArg].list[3], "%lf", &sequenceEnd)!=1))
          SDDS_Bomb("invalid -sequence syntax/value");
        if (sequenceSpacing)
          SDDS_Bomb("give only one of -sequence and -equispaced");
        break;
      case CLO_EQUISPACED:
        if ((scanned[iArg].n_items!=2 && scanned[iArg].n_items!=4) ||
            sscanf(scanned[iArg].list[1], "%lf", &sequenceSpacing)!=1 || sequenceSpacing<=0)
          SDDS_Bomb("invalid -equispaced syntax/value");
        if (scanned[iArg].n_items==4 &&
            (sscanf(scanned[iArg].list[2], "%lf", &sequenceStart)!=1 ||
             sscanf(scanned[iArg].list[3], "%lf", &sequenceEnd)!=1))
          SDDS_Bomb("invalid -equispaced syntax/values");
        if (sequencePoints)
          SDDS_Bomb("give only one of -sequence and -equispaced");
        break;
      case CLO_COLUMNS:
        if (indepQuantity)
          SDDS_Bomb("only one -columns option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        indepQuantity = scanned[iArg].list[1];
        if (scanned[iArg].n_items>=2) {
          depenQuantity = tmalloc(sizeof(*depenQuantity)*(depenQuantities=scanned[iArg].n_items-2));
          for (i=0; i<depenQuantities; i++)
            depenQuantity[i] = scanned[iArg].list[i+2];
        }
        break;
      case CLO_PRINTOUT:
        if ((scanned[iArg].n_items-=1)>=1) {
          if (!scanItemList(&printFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                            "bare", -1, NULL, 0, BARE_PRINTOUT,
                            "stdout", -1, NULL, 0, STDOUT_PRINTOUT,
                            NULL)) 
            SDDS_Bomb("invalid -printout syntax");
        }
        if (!(printFlags&BARE_PRINTOUT))
          printFlags |= NORMAL_PRINTOUT;
        break;
      case CLO_FILEVALUES:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -fileValues syntax");
        fileValuesFile = scanned[iArg].list[1];
        scanned[iArg].n_items -= 2;
        if (!scanItemList(&flags, scanned[iArg].list+2, &scanned[iArg].n_items, 0,
                          "column", SDDS_STRING, &fileValuesQuantity, 1, 0,
                          "parallelpages", -1, NULL, 0, FILEVALUES_PARALLEL_PAGES,
                          NULL))
          SDDS_Bomb("invalid -fileValues syntax");
        if (flags&FILEVALUES_PARALLEL_PAGES)
          parallelPages = 1;
        break;
      case CLO_COMBINEDUPLICATES:
        SDDS_Bomb("-combineDuplicates option not implemented yet--send email to borland@aps.anl.gov");
        combineDuplicates = 1;
        break;
      case CLO_BRANCH:
        SDDS_Bomb("-branch option not implemented yet--send email to borland@aps.anl.gov");
        if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%ld", &branch)!=1 ||
            branch<1)
          SDDS_Bomb("invalid -branch syntax/value");
        break;
      case CLO_BELOWRANGE:
        if ((scanned[iArg].n_items-=1)<1 ||
            !scanItemList(&belowRange.flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "value", SDDS_DOUBLE, &belowRange.value, 1, OUTRANGE_VALUE,
                          "skip", -1, NULL, 0, OUTRANGE_SKIP,
                          "saturate", -1, NULL, 0, OUTRANGE_SATURATE,
                          "extrapolate", -1, NULL, 0, OUTRANGE_EXTRAPOLATE,
                          "wrap", -1, NULL, 0, OUTRANGE_WRAP,
                          "abort", -1, NULL, 0, OUTRANGE_ABORT,
                          "warn", -1, NULL, 0, OUTRANGE_WARN,
                          NULL))
          SDDS_Bomb("invalid -belowRange syntax/value");
        if ((i=bitsSet(belowRange.flags&
                       (OUTRANGE_VALUE|OUTRANGE_SKIP|OUTRANGE_SATURATE|OUTRANGE_EXTRAPOLATE|OUTRANGE_WRAP|OUTRANGE_ABORT)))>1)
          SDDS_Bomb("incompatible keywords given for -belowRange");
        if (i!=1)
          belowRange.flags |= OUTRANGE_SATURATE;
        break;
      case CLO_ABOVERANGE:
        if ((scanned[iArg].n_items-=1)<1 ||
            !scanItemList(&aboveRange.flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "value", SDDS_DOUBLE, &aboveRange.value, 1, OUTRANGE_VALUE,
                          "skip", -1, NULL, 0, OUTRANGE_SKIP,
                          "saturate", -1, NULL, 0, OUTRANGE_SATURATE,
                          "extrapolate", -1, NULL, 0, OUTRANGE_EXTRAPOLATE,
                          "wrap", -1, NULL, 0, OUTRANGE_WRAP,
                          "abort", -1, NULL, 0, OUTRANGE_ABORT,
                          "warn", -1, NULL, 0, OUTRANGE_WARN,
                          NULL))
          SDDS_Bomb("invalid -aboveRange syntax/value");
        if ((i=bitsSet(aboveRange.flags&
                       (OUTRANGE_VALUE|OUTRANGE_SKIP|OUTRANGE_SATURATE|OUTRANGE_EXTRAPOLATE|OUTRANGE_WRAP|OUTRANGE_ABORT)))>1)
          SDDS_Bomb("incompatible keywords given for -aboveRange");
        if (i!=1)
          aboveRange.flags |= OUTRANGE_SATURATE;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_EXCLUDE:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -exclude syntax");
        moveToStringArray(&exclude, &excludes, scanned[iArg].list+1, scanned[iArg].n_items-1);
        break;
      case CLO_FORCEMONOTONIC:
        if ((scanned[iArg].n_items-=1)>0) {
          if (!scanItemList(&forceMonotonic, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                            "increasing", -1, NULL, 0, FORCE_INCREASING,
                            "decreasing", -1, NULL, 0, FORCE_DECREASING,
                            NULL) || bitsSet(forceMonotonic)!=1)
            SDDS_Bomb("invalid -forceMonotonic syntax/value");
        }
        else 
          forceMonotonic = FORCE_MONOTONIC;
        break;
      case CLO_FILLIN:
        fillIn = 1;
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

  processFilenames("sddsinterp", &input, &output, pipeFlags, 0, NULL);

  fpPrint = stderr;
  if (printFlags&STDOUT_PRINTOUT)
    fpPrint = stdout;

  if (!indepQuantity)
    SDDS_Bomb("supply the independent quantity name with the -columns option");

  if ((atValues?1:0)+(fileValuesFile?1:0)+(sequencePoints?1:0)+fillIn+(sequenceSpacing>0?1:0) != 1)
    SDDS_Bomb("you must give one and only one of -atValues, -fileValues, -sequence, -equispaced, and -fillIn");

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  excludes = appendToStringArray(&exclude, excludes, indepQuantity);
  if (!depenQuantities)
    depenQuantities = appendToStringArray(&depenQuantity, depenQuantities, "*"); 

  if ((depenQuantities=expandColumnPairNames(&SDDSin, &depenQuantity, NULL, depenQuantities, 
                                             exclude, excludes, FIND_NUMERIC_TYPE, 0))<=0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    SDDS_Bomb("no dependent quantities selected for interpolation");
  }

  if (fileValuesFile && !SDDS_InitializeInput(&SDDSvalues, fileValuesFile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, "sddsinterp output", output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, indepQuantity, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  /*
    if (fileValuesQuantity && strcmp(fileValuesQuantity, indepQuantity)!=0 &&
    !SDDS_TransferColumnDefinition(&SDDSout, &SDDSvalues, fileValuesQuantity, NULL)) {
    fprintf(stderr, "problem creating -fileValues column %s\n", fileValuesQuantity);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    */
  if (SDDS_DefineParameter(&SDDSout, "InterpDataPage", NULL, NULL, 
                           "Page of interpolation data file used to create this page",
                           NULL, SDDS_LONG, NULL)<0 ||
      SDDS_DefineParameter(&SDDSout, "InterpPointsPage", NULL, NULL, 
                           "Page of interpolation points file used to create this page",
                           NULL, SDDS_LONG, NULL)<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<depenQuantities; i++) 
    if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, depenQuantity[i], NULL)) {
      fprintf(stderr, "problem creating interpolated-output column %s\n", depenQuantity[i]);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  if (!SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  doNotRead = 0;
  interpPoint = NULL;
  outputData = tmalloc(sizeof(*outputData)*(depenQuantities));
  depenValue = tmalloc(sizeof(*depenValue)*(depenQuantities));
  rowFlag = NULL;
  valuesReadCode = 0;

  while (doNotRead || (readCode=SDDS_ReadPage(&SDDSin))>0) {
    rows = SDDS_CountRowsOfInterest(&SDDSin);
    if (!(indepValue = SDDS_GetColumnInDoubles(&SDDSin, indepQuantity)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (atValues) {
      interpPoint = atValue;
      interpPoints = atValues;
    }
    else if (fileValuesFile) {
      if (interpPoint)
        free(interpPoint);
      if ((valuesReadCode=SDDS_ReadPage(&SDDSvalues))==0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      else if (valuesReadCode==-1) {
        if (parallelPages) {
          fprintf(stderr, "warning: file %s ends before file %s\n", fileValuesFile, input);
          break;
        }
        else {
          /* "rewind" the values file */
          if (!SDDS_Terminate(&SDDSvalues) ||
              !SDDS_InitializeInput(&SDDSvalues, fileValuesFile))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if ((valuesReadCode=SDDS_ReadPage(&SDDSvalues))<1) {
            fprintf(stderr, "error: unable to (re)read file %s\n", fileValuesFile);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
          }
          /* read the next page of the interpolation data file */
          if ((readCode=SDDS_ReadPage(&SDDSin))<1) {
            if (readCode==-1)
              break;
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          rows = SDDS_CountRowsOfInterest(&SDDSin);
          if (indepValue)
            free(indepValue);
          if (!(indepValue = SDDS_GetColumnInDoubles(&SDDSin, indepQuantity)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      
      if (!parallelPages)
        doNotRead = 1;
      interpPoints = SDDS_CountRowsOfInterest(&SDDSvalues);
      interpPoint = SDDS_GetColumnInDoubles(&SDDSvalues, fileValuesQuantity);
      if (SDDS_NumberOfErrors())
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    else if (sequencePoints || sequenceSpacing) {
      if (interpPoint)
        free(interpPoint);
      interpPoints = sequencePoints;
      if (!(interpPoint = makeSequence(&interpPoints, sequenceStart, sequenceEnd, sequenceSpacing, 
                                       indepValue, rows)))
        exit(1);
    } else {
      /* fillIn interpolation */
      if (interpPoint)
        free(interpPoint);
      if (!(interpPoint = makeFillInSequence(indepValue, rows, &interpPoints)))
        exit(1);
    }
    
    for (i=0; i<depenQuantities; i++)
      outputData[i] = tmalloc(sizeof(*outputData[i])*interpPoints);
    rowFlag = trealloc(rowFlag, sizeof(*rowFlag)*interpPoints);
    for (j=0; j<interpPoints; j++)
      rowFlag[j] = 1;
    for (i=0; i<depenQuantities; i++) {
      if (!(depenValue[i] = SDDS_GetColumnInDoubles(&SDDSin, depenQuantity[i])))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (forceMonotonic)
      rows = forceMonotonicity(indepValue, depenValue, depenQuantities, rows, forceMonotonic);
    else if (combineDuplicates)
      rows = combineDuplicatePoints(indepValue, depenValue, depenQuantities, rows, 0.0);
    if ((monotonicity=checkMonotonicity(indepValue, rows))==0)
      SDDS_Bomb("independent data values do not change monotonically or repeated independent values exist");
    if (interpShort)
      shortValue = malloc(sizeof(*shortValue)*rows);
    
    for (i=0; i<depenQuantities; i++) {
      if (interpShort) {
        for (row=0; row<rows; row++) {
          shortValue[row] = (short)depenValue[i][row];
        }
      }
      for (j=0; j<interpPoints; j++) {
        if (!interpShort) {
          outputData[i][j] = interpolate(depenValue[i], indepValue, rows, interpPoint[j], &belowRange,
                                         &aboveRange, order, &interpCode, monotonicity);
        } else {
          outputData[i][j] = (double)interp_short(shortValue, indepValue, rows, interpPoint[j], 0, -1,
                                                  &interpCode, &nextPos);
        }
        if (interpCode) {
          if (interpCode&OUTRANGE_ABORT) {
            fprintf(stderr, "error: value %e is out of range for column %s\n",
                    interpPoint[j], depenQuantity[i]);
            exit(1);
          }
          if (interpCode&OUTRANGE_WARN)
            fprintf(stderr, "warning: value %e is out of range for column %s\n",
                    interpPoint[j], depenQuantity[i]);
          if (interpCode&OUTRANGE_SKIP) 
            rowFlag[j] = 0;
        }
      }
    }
    if (interpShort)
      free(shortValue);
    if (!SDDS_StartPage(&SDDSout, interpPoints) || 
        !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, interpPoint, interpPoints, indepQuantity))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_SetParameters(&SDDSout, SDDS_BY_NAME|SDDS_PASS_BY_VALUE,
                            "InterpDataPage", readCode,
                            "InterpPointsPage", valuesReadCode, NULL) ||
        !SDDS_CopyParameters(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<depenQuantities; i++)
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, outputData[i], interpPoints, depenQuantity[i]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_AssertRowFlags(&SDDSout, SDDS_FLAG_ARRAY, rowFlag, rows) || !SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (printFlags&BARE_PRINTOUT) {
      for (j=0; j<interpPoints; j++)
        if (rowFlag[j]) {
          fprintf(fpPrint, "%21.15e ", interpPoint[j]);
          for (i=0; i<depenQuantities; i++)
            fprintf(fpPrint, "%21.15e ", outputData[i][j]);
          fputc('\n', fpPrint);
        }
    }
    else if (printFlags&NORMAL_PRINTOUT) {
      for (j=0; j<interpPoints; j++)
        if (rowFlag[j]) {
          fprintf(fpPrint, "%s=%21.15e ", indepQuantity, interpPoint[j]);
          for (i=0; i<depenQuantities; i++)
            fprintf(fpPrint, "%s=%21.15e ", depenQuantity[i], outputData[i][j]);
          fputc('\n', fpPrint);
        }
    }
    if (indepValue) free(indepValue);
    indepValue = NULL;
    for (i=0; i<depenQuantities; i++) {
      if (outputData[i]) free(outputData[i]);
      outputData[i] = NULL;
      if (depenValue[i]) free(depenValue[i]);
      depenValue[i] = NULL;
    }
    if (fileValuesFile) {
      if (interpPoint) free(interpPoint);
      interpPoint = NULL;
    }
    if (rowFlag) free(rowFlag);
    rowFlag = NULL;
  }

  if (!SDDS_Terminate(&SDDSin)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (fileValuesFile) {
    if (!SDDS_Terminate(&SDDSvalues)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }

  return 0;
}

double *makeFillInSequence(double *x, long dataRows, long *nPointsReturn)
{
  long i;
  double dxMin, dx, xMin, xMax;
  
  dxMin = DBL_MAX;
  xMin = xMax = x[0];
  for (i=1; i<dataRows; i++) {
    if ((dx=fabs(x[i]-x[i-1]))<dxMin && dx>0)
      dxMin = dx;
    if (x[i]<xMin)
      xMin = x[i];
    if (x[i]>xMax)
      xMax = x[i];
  }
  *nPointsReturn = (xMax-xMin)/dxMin+1;
  return makeSequence(nPointsReturn, xMin, xMax, 0.0, x, dataRows);
}


double *makeSequence(long *points, double start, double end, double spacing, double *data, long dataRows)
{
  long i;
  double delta, *sequence;
  if (start==end) {
    if (!find_min_max(&start, &end, data, dataRows))
      return NULL;
  }
  if (*points>1) {
    delta = (end-start)/(*points-1);
  } else if (spacing>=0) {
    delta = spacing;
    *points = (end-start)/delta + 1.5;
  } else{
    *points = 1;
    delta = 0;
  }
  sequence = tmalloc(sizeof(*sequence)*(*points));
  for (i=0; i<*points; i++)
    sequence[i] = start+delta*i;
  return sequence;
}

long checkMonotonicity(double *indepValue, long rows)
{

  if (rows==1)
    return 1;
  if (indepValue[rows-1]>indepValue[0]) {
    while (--rows>0)
      if (indepValue[rows]<=indepValue[rows-1])
        return 0;
    return 1;
  }
  else {
    while (--rows>0)
      if (indepValue[rows]>=indepValue[rows-1])
        return 0;
    return -1;
  }
}


long combineDuplicatePoints(double *x, double **y, long columns, long rows, double tolerance)
{
  double xmin, xmax;
  long i, j, column;

  find_min_max(&xmin, &xmax, x, rows);
  if (xmin==xmax)
    SDDS_Bomb("interpolation data is invalid--no range in independent variable");
  tolerance *= xmax-xmin;
  for (i=1; i<rows; i++) {
    if (fabs(x[i]-x[i-1])<=tolerance) {
      x[i-1] = (x[i]+x[i-1])/2;
      for (column=0; column<columns; column++) 
        y[column][i-1] = (y[column][i]+y[column][i-1])/2;
      for (j=i+1; j<rows; j++) {
        x[j-1] = x[j];
        for (column=0; column<columns; column++) 
          y[column][j-1] = y[column][j];
      }
      rows--;
      i--;
    }
  }
  return rows;
}

long forceMonotonicity(double *x, double **y, long columns, long rows, unsigned long mode)
{
  long i, j, column, direction;
  char *keep;
  double reference;
  
  if (rows<2)
    return rows;

  if (mode&FORCE_INCREASING)
    direction = 1;
  else if (mode&FORCE_DECREASING)
    direction = -1;
  else
    direction = x[1]>x[0] ? 1 : -1;

  keep = tmalloc(sizeof(*keep)*rows);
  reference = x[0];
  for (i=1; i<rows; i++) {
    if (direction*(x[i]-reference)>0) {
      reference = x[i];
      keep[i] = 1;
    }
    else  {
      keep[i] = 0;
    }
  }
  for (i=j=1; i<rows; i++) {
    if (keep[i]) {
      if (i!=j) {
        for (column=0; column<columns; column++)
          y[column][j] = y[column][i];
        x[j] = x[i];
      }
      j++;
    } 
  }
  rows = j;
  free(keep);
  return rows;
}

