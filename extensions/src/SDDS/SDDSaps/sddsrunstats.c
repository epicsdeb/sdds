/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsrunstats
 * 
 $Log: sddsrunstats.c,v $
 Revision 1.18  2008/01/24 19:12:33  shang
 added -median option

 Revision 1.17  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.16  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.15  2005/11/04 22:46:17  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.14  2004/03/16 23:44:24  borland
 Fixed parsing bug that caused problems when column named looked like an abbreviated tag name

 Revision 1.13  2004/03/16 23:25:38  borland
 Added option to scanItemList to prevent problems when a column is specified
 that happens to look like a qualifier name.

 Revision 1.12  2003/06/15 17:32:33  borland
 If number of points is 0, now does stats on the whole page.

 Revision 1.11  2002/08/14 17:12:53  soliday
 Added Open License

 Revision 1.10  2001/05/18 21:35:50  borland
 Fixed memory leak.

 Revision 1.9  2001/01/10 19:35:46  soliday
 Standardized usage message.

 Revision 1.8  1999/05/25 19:14:44  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:55  borland
 Fixed the version number in the usage message.

 Revision 1.6  1998/12/16 21:26:06  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.5  1998/08/14 17:13:24  borland
 Improved the way the number of output rows is computed.  Now allows
 statistics on partial segments, if -partialOk option is given.

 Revision 1.4  1998/01/10 18:26:13  borland
 Now works properly in overlap mode with -window option.

 Revision 1.3  1997/10/30 20:21:28  borland
 Added option to block the points using a window on a specified column.
 So, for example, one can now average points over specified time intervals.

 Revision 1.2  1996/11/13 18:17:13  borland
 Now actually does *running* statistics in addition to blocked statistics.

 Revision 1.1  1996/11/13 03:44:32  borland
 First version.

 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include <ctype.h>

/* if statistics are added, they must be added before the SET_POINTS
   item in this list and the following options array
   */
#define SET_MAXIMUM 0
#define SET_MINIMUM 1
#define SET_MEAN 2
#define SET_STANDARDDEVIATION 3
#define SET_RMS 4
#define SET_SUM 5
#define SET_SIGMA 6
#define SET_SAMPLE 7
#define SET_MEDIAN 8
#define N_STATS 9
#define SET_POINTS 9
#define SET_NOOVERLAP 10
#define SET_PIPE 11
#define SET_WINDOW 12
#define SET_PARTIALOK 13 
#define N_OPTIONS 14

char *option[N_OPTIONS] = {
  "maximum", "minimum", "mean", "standarddeviation", "rms", "sum", 
  "sigma", "sample", "median", "points", "nooverlap", "pipe", "window", "partialok", 
} ;
char *statSuffix[N_STATS] = {
  "Max", "Min", "Mean", "StDev", "RMS", "Sum", "Sigma", "", "Median",
  } ;

#define TOPLIMIT_GIVEN    0x0001U
#define BOTTOMLIMIT_GIVEN 0x0002U

/* this structure stores a command-line request for statistics computation */
/* individual elements of sourceColumn may contain wildcards */
typedef struct {
    char **sourceColumn;
    long sourceColumns, sumPower, optionCode;
    unsigned long flags;
    double topLimit, bottomLimit;
    } STAT_REQUEST;

/* this structure stores data necessary for accessing/creating SDDS columns and
 * for computing a statistic
 */
typedef struct {
    char **sourceColumn, **resultColumn;
    long sourceColumns, optionCode, *resultIndex, sumPower;
    unsigned long flags;
    double topLimit, bottomLimit;
    } STAT_DEFINITION;

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, 
                     long items, long code, unsigned long flag);
STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inTable, 
                                        STAT_REQUEST *request, long requests);
long setupOutputFile(SDDS_DATASET *outTable, char *output, SDDS_DATASET *inTable, 
                     STAT_DEFINITION *stat, long stats);

static char *USAGE="sddsrunstats [<input>] [<output>] [-pipe[=input][,output]]\n\
[{-points=<integer> | -window=column=<column>,width=<value>}] [-noOverlap]\n\
[-partialOk]\n\
[-mean=[<limitOps>],<columnNameList>]\n\
[-median=[<limitOps>],<columnNameList>]\n\
[-minimum=[<limitOps>],<columnNameList>]\n\
[-maximum=[<limitOps>],<columnNameList>]\n\
[-standardDeviation=[<limitOps>],<columnNameList>]\n\
[-sigma=[<limitOps>],<columnNameList>]\n\
[-sum=[<limitOps>][,power=<integer>],<columnNameList>] \n\
[-sample=[<limitOps>],<columnNameList>]]\n\n\
  <limitOps> is of the form [topLimit=<value>,][bottomLimit=<value>]\n\n\
Computes running statistics of columns of data.  The <columnNameList> may contain\n\
wildcards, in which case an additional output column is produced for every matching\n\
column.  By default, statistics are done with a sliding window, so the values are\n\
running statistics; for blocked statistics, use -noOverlap.  For statistics on\n\
the entire page, use -points=0.\n\
The -partialOk option tells sddsrunstats to do computations even\n\
if the number of available rows is less than the number of points\n\
specified; by default, such data is simply ignored.\n\
Program by Michael Borland. (This is version 4, December 1998.)\n";

int main(int argc, char **argv)
{
  STAT_DEFINITION *stat;
  long stats;
  STAT_REQUEST *request;
  long requests, count;
  SCANNED_ARG *scanned; 
  SDDS_DATASET inData, outData;

  int32_t power;
  long pointsToStat, pointsToStat0, overlap, rows, outputRowsMax, outputRows, outputRow;
  long iArg, code, iStat, iColumn;
  long startRow, rowOffset;
  char *input, *output, *windowColumn;
  double *inputData, *outputData, topLimit, bottomLimit, *inputDataOffset, result, sum1, sum2, *newData;
  double windowWidth, *windowData;
  long windowIndex;
  unsigned long pipeFlags, scanFlags;
  char s[100];
  long lastRegion, region, windowRef, partialOk;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2) {
    bomb("too few arguments", USAGE);
  }
  newData = NULL;
  result = 0;
  input = output = NULL;
  stat = NULL;
  request = NULL;
  stats = requests = pipeFlags = 0;
  pointsToStat = -1;
  partialOk = 0;
  overlap = 1;
  windowColumn = NULL;
  windowData = NULL;

  for (iArg=1; iArg<argc; iArg++) {
    scanFlags = 0;
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (code=match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_POINTS:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%ld", &pointsToStat)!=1 ||
            (pointsToStat<=2 && pointsToStat!=0))
          SDDS_Bomb("invalid -points syntax");
        break;
      case SET_NOOVERLAP:
        overlap = 0;
        break;
      case SET_MAXIMUM:
      case SET_MINIMUM:
      case SET_MEAN:
      case SET_STANDARDDEVIATION:
      case SET_RMS:
      case SET_SIGMA:
      case SET_SAMPLE:
      case SET_MEDIAN:
        if (scanned[iArg].n_items<2) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        if (!scanItemList(&scanFlags, scanned[iArg].list, &scanned[iArg].n_items,
                          SCANITEMLIST_UNKNOWN_VALUE_OK|SCANITEMLIST_REMOVE_USED_ITEMS|
                          SCANITEMLIST_IGNORE_VALUELESS,
                          "toplimit", SDDS_DOUBLE, &topLimit, 1, TOPLIMIT_GIVEN,
                          "bottomlimit", SDDS_DOUBLE, &bottomLimit, 1, BOTTOMLIMIT_GIVEN,
                          NULL)) {
          sprintf(s, "invalid -%s syntax", scanned[iArg].list[0]);
          SDDS_Bomb(s);
        }
        requests = addStatRequests(&request, requests, scanned[iArg].list+1, scanned[iArg].n_items-1,
                                   code, scanFlags);
        request[requests-1].topLimit = topLimit;
        request[requests-1].bottomLimit = bottomLimit;
        break;
      case SET_SUM:
        if (scanned[iArg].n_items<2) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        power = 1;
        if (!scanItemList(&scanFlags, scanned[iArg].list, &scanned[iArg].n_items, 
                          SCANITEMLIST_UNKNOWN_VALUE_OK|SCANITEMLIST_REMOVE_USED_ITEMS|
                          SCANITEMLIST_IGNORE_VALUELESS,
                          "power", SDDS_LONG, &power, 1, 0, 
                          "toplimit", SDDS_DOUBLE, &topLimit, 1, TOPLIMIT_GIVEN,
                          "bottomlimit", SDDS_DOUBLE, &bottomLimit, 1, BOTTOMLIMIT_GIVEN,
                          NULL))
          SDDS_Bomb("invalid -sum syntax");
        requests = addStatRequests(&request, requests, scanned[iArg].list+1, scanned[iArg].n_items-1,
                                   code, scanFlags);
        request[requests-1].sumPower = power;
        request[requests-1].topLimit = topLimit;
        request[requests-1].bottomLimit = bottomLimit;
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_WINDOW:
        windowWidth = -1;
        windowColumn = NULL;
        scanned[iArg].n_items -= 1;
        if (!scanItemList(&scanFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "column", SDDS_STRING, &windowColumn, 1, 0,
                          "width", SDDS_DOUBLE, &windowWidth, 1, 0,
                          NULL) 
            || !windowColumn || !strlen(windowColumn) || windowWidth<=0)
          SDDS_Bomb("invalid -window syntax/values");
        break;
      case SET_PARTIALOK:
        partialOk = 1;
        break;
      default:
        fprintf(stderr, "error: unknown option '%s' given\n", scanned[iArg].list[0]);
        exit(1);
        break;
      }
    }
    else {  
      /* argument is filename */
      if (!input)
        input = scanned[iArg].list[0];
      else if (!output)
        output = scanned[iArg].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }

  if (pointsToStat<0 && !windowColumn) {
    pointsToStat = 10;
  }
  processFilenames("sddsrunstats", &input, &output, pipeFlags, 0, NULL);

  if (!requests)
    SDDS_Bomb("no statistics requested");
  
  if (!SDDS_InitializeInput(&inData, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(stat=compileStatDefinitions(&inData, request, requests)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  stats = requests;
#ifdef DEBUG
  fprintf(stderr, "%ld stats\n", stats);
  for (iStat=0; iStat<stats; iStat++) {
    for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
      fprintf(stderr, "iStat=%ld  iColumn=%ld  source=%s  result=%s\n",
              iStat, iColumn, stat[iStat].sourceColumn[iColumn],
              stat[iStat].resultColumn[iColumn]);
      if (stat[iStat].flags&BOTTOMLIMIT_GIVEN)
        fprintf(stderr, "  bottom = %e\n", stat[iStat].bottomLimit);
      if (stat[iStat].flags&TOPLIMIT_GIVEN)
        fprintf(stderr, "  top = %e\n", stat[iStat].topLimit);
    }
  }
#endif
  
  if (!setupOutputFile(&outData, output, &inData, stat, stats))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (windowColumn) {
    if ((windowIndex=SDDS_GetColumnIndex(&inData, windowColumn))<0)
      SDDS_Bomb("Window column not present");
    if (!SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&inData, windowIndex)))
      SDDS_Bomb("Window column is not numeric");
  }
  
  outputData = NULL;
  outputRowsMax = 0;
  pointsToStat0 = pointsToStat;
  while ((code=SDDS_ReadPage(&inData))>0) {
    rows = SDDS_CountRowsOfInterest(&inData);
    pointsToStat = pointsToStat0;
    if (pointsToStat==0)
      pointsToStat = rows;
    if (windowColumn && 
        !(windowData = SDDS_GetColumnInDoubles(&inData, windowColumn))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!windowColumn) {
      if (rows<pointsToStat) {
        if (partialOk)
          pointsToStat = rows;
        else
          continue;
      }
      if (overlap) {
        outputRows = rows-pointsToStat+1;
      }
      else 
        outputRows = rows/pointsToStat;
    } else
      outputRows = rows;

    if (!SDDS_StartPage(&outData, outputRows) || !SDDS_CopyParameters(&outData, &inData) || 
        !SDDS_CopyArrays(&outData, &inData))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (outputRows>outputRowsMax && 
        !(outputData = SDDS_Realloc(outputData, sizeof(*outputData)*(outputRowsMax=outputRows))))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (iStat=0; iStat<stats; iStat++) {
      for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
        lastRegion = 0;
        windowRef = 0;
        if (!(inputData=SDDS_GetColumnInDoubles(&inData, stat[iStat].sourceColumn[iColumn])))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (outputRow=startRow=0; outputRow<outputRows; 
             outputRow++, startRow+=(overlap?1:pointsToStat)) {
          if (windowColumn) {
            short windowFound = 0;
            if (overlap) {
              windowRef += 1;
              lastRegion = 0;
            }
            for (pointsToStat=1; pointsToStat<outputRows-startRow; pointsToStat++) {
              region = (windowData[startRow+pointsToStat]-windowData[windowRef])/windowWidth;
              if (region!=lastRegion) {
                lastRegion = region;
                windowFound = 1;
                break;
              } 
            }
            if (!windowFound && pointsToStat<2)
              break;
            if (startRow+pointsToStat>rows) {
              pointsToStat = rows-startRow-1;
              if (pointsToStat<=0)
                break;
            }
#ifdef DEBUG
            fprintf(stderr, "row=%ld pointsToStat=%ld  delta=%.9lf (%.9lf -> %.9lf)\n", 
                    startRow, pointsToStat, windowData[startRow+pointsToStat-1] - windowData[startRow],
                    windowData[startRow], windowData[startRow+pointsToStat-1]);
#endif
          }
          inputDataOffset = inputData+startRow;
          switch (stat[iStat].optionCode) {
          case SET_MAXIMUM:
            result = -DBL_MAX;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              if (inputDataOffset[rowOffset]>result)
                result = inputDataOffset[rowOffset];
            }
            break;
          case SET_MINIMUM:
            result = DBL_MAX;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              if (inputDataOffset[rowOffset]<result)
                result = inputDataOffset[rowOffset];
            }
            break;
          case SET_MEAN:
            result = 0;
            count = 0;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              result += inputDataOffset[rowOffset];
              count++;
            }
            if (count)
              result /= count;
            else 
              result = DBL_MAX;
            break;
          case SET_MEDIAN:
            result = 0;
            count = 0;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              newData = SDDS_Realloc(newData, sizeof(*newData)*(count+1));
              newData[count] = inputDataOffset[rowOffset];
              count++;
            }
            if (count) {
              if (!compute_median(&result, newData, count))
                result = DBL_MAX;
              free(newData);
              newData = NULL;
            } else
              result = DBL_MAX;
            break;
          case SET_STANDARDDEVIATION:
          case SET_SIGMA:
            sum1 = sum2 = count = 0;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              sum1 += inputDataOffset[rowOffset];
              sum2 += inputDataOffset[rowOffset]*inputDataOffset[rowOffset];
              count++;
            }
            if (count>1) {
              if ((result = sum2/count - sqr(sum1/count))<=0)
                result = 0;
              else
                result = sqrt(result*count/(count-1.0));
              if (stat[iStat].optionCode==SET_SIGMA)
                result /= sqrt(count);
            } 
            break;
          case SET_RMS:
            sum2 = count = 0;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              sum2 += inputDataOffset[rowOffset]*inputDataOffset[rowOffset];
              count++;
            }
            if (count>0)
              result = sqrt(sum2/count);
            else 
              result = DBL_MAX;
            break;
          case SET_SUM:
            sum1 = count = 0;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN && 
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              sum1 += ipow(inputDataOffset[rowOffset], stat[iStat].sumPower);
              count++;
            }
            if (count>0)
              result = sum1;
            else
              result = DBL_MAX;
            break;
          case SET_SAMPLE:
            result = DBL_MAX;
            for (rowOffset=0; rowOffset<pointsToStat; rowOffset++) {
              if ((stat[iStat].flags&TOPLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]>stat[iStat].topLimit) ||
                  (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                   inputDataOffset[rowOffset]<stat[iStat].bottomLimit))
                continue;
              result = inputDataOffset[rowOffset];
              break;
            }
            break;
          default:
            fprintf(stderr, "Unknown statistics code %ld in sddsrunave\n",
                    stat[iStat].optionCode);
            exit(1);
            break;
          }
          outputData[outputRow] = result;
        }
        if (!SDDS_SetColumnFromDoubles(&outData, SDDS_SET_BY_INDEX, outputData, outputRow,
                                       stat[iStat].resultIndex[iColumn])) 
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        free(inputData);
        }
      }
    if (windowColumn)
      free(windowData); 
    if (!SDDS_WritePage(&outData))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  return(0);
}

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, 
                     long items, long code, unsigned long flags)
{
  long i;
  if (!(*statRequest = SDDS_Realloc(*statRequest, sizeof(**statRequest)*(requests+1))) ||
      !((*statRequest)[requests].sourceColumn = (char**)malloc(sizeof(*(*statRequest)[requests].sourceColumn)*
                                                               items)))
    SDDS_Bomb("memory allocation failure");
  for (i=0; i<items; i++) {
    (*statRequest)[requests].sourceColumn[i] = item[i];
  }
  (*statRequest)[requests].sourceColumns = items;
  (*statRequest)[requests].optionCode = code;
  (*statRequest)[requests].sumPower = 1;
  (*statRequest)[requests].flags = flags;
  return requests+1;
}

STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inData, STAT_REQUEST *request, long requests)
{
  STAT_DEFINITION *stat;
  long iReq, iName;
  char s[SDDS_MAXLINE];

  if (!(stat = (STAT_DEFINITION *)malloc(sizeof(*stat)*requests)))
    SDDS_Bomb("memory allocation failure");
  for (iReq=0; iReq<requests; iReq++) {
    if ((stat[iReq].sourceColumns 
         = expandColumnPairNames(inData, &request[iReq].sourceColumn, NULL,
                                 request[iReq].sourceColumns, NULL, 0, 
                                 FIND_NUMERIC_TYPE, 0))<=0) {
      fprintf(stderr, "Error: no match for column names (sddsrunstats):\n");
      for (iName=0; iName<request[iReq].sourceColumns; iName++)
        fprintf(stderr, "%s, ", request[iReq].sourceColumn[iReq]);
      fputc('\n', stderr);
      exit(1);
    }
    stat[iReq].sourceColumn = request[iReq].sourceColumn;
    if (!(stat[iReq].resultColumn = malloc(sizeof(*stat[iReq].resultColumn)*stat[iReq].sourceColumns)) ||
        !(stat[iReq].resultIndex = malloc(sizeof(*stat[iReq].resultIndex)*stat[iReq].sourceColumns))) {
      SDDS_Bomb("memory allocation failure");
    }
    for (iName=0; iName<stat[iReq].sourceColumns; iName++) {
      sprintf(s, "%s%s", stat[iReq].sourceColumn[iName], statSuffix[request[iReq].optionCode]);
      SDDS_CopyString(stat[iReq].resultColumn+iName, s);
    }
    stat[iReq].optionCode   = request[iReq].optionCode;
    stat[iReq].sumPower     = request[iReq].sumPower;
    stat[iReq].flags        = request[iReq].flags;
    stat[iReq].topLimit     = request[iReq].topLimit;
    stat[iReq].bottomLimit  = request[iReq].bottomLimit;
  }

  return stat;
}

long setupOutputFile(SDDS_DATASET *outData, char *output, SDDS_DATASET *inData, 
                     STAT_DEFINITION *stat, long stats)
{
  long column, iStat;
  char s[SDDS_MAXLINE];

  if (!SDDS_InitializeOutput(outData, SDDS_BINARY, 1, NULL, NULL, output))
      return 0;
  for (iStat=0; iStat<stats; iStat++) {
    for (column=0; column<stat[iStat].sourceColumns; column++) {
      if (!SDDS_TransferColumnDefinition(outData, inData, 
                                         stat[iStat].sourceColumn[column],
                                         stat[iStat].resultColumn[column])) {
        sprintf(s, "Problem transferring definition of column %s to %s\n",
                stat[iStat].sourceColumn[column], stat[iStat].resultColumn[column]);
        SDDS_SetError(s);
        return 0;
      }
      if ((stat[iStat].resultIndex[column]
           = SDDS_GetColumnIndex(outData, stat[iStat].resultColumn[column]))<0) {
        sprintf(s, "Problem creating column %s", stat[iStat].resultColumn[column]);
        SDDS_SetError(s);
        return 0;
      }
      if (!SDDS_ChangeColumnInformation
          (outData, "description", NULL, SDDS_SET_BY_NAME, stat[iStat].resultColumn[column]) ||
          !SDDS_ChangeColumnInformation
          (outData, "symbol", NULL, SDDS_SET_BY_NAME, stat[iStat].resultColumn[column]) ||
          !SDDS_ChangeColumnInformation
          (outData, "type", "double", SDDS_SET_BY_NAME|SDDS_PASS_BY_STRING, stat[iStat].resultColumn[column])) {
        sprintf(s, "Problem changing attributes of new column %s", stat[iStat].resultColumn[column]);
        SDDS_SetError(s);
        return 0;
      }
    }
  }
  if (!SDDS_TransferAllParameterDefinitions(outData, inData, SDDS_TRANSFER_KEEPOLD) || 
      !SDDS_TransferAllArrayDefinitions(outData, inData, 0) ||
      !SDDS_WriteLayout(outData))
    return 0;
  return 1;
}

