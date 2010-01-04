/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsrowstats
 * purpose: do statistics for a row of values across several columns,
 *          producing a new column of data.
 * 
 * Michael Borland, 1996
 $Log: sddsrowstats.c,v $
 Revision 1.24  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.23  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.22  2006/03/17 15:48:40  shang
 added positionColumn option to minimum, maximum, smallest and largest option to
  write the column name that has the option specified value in each row to the
  output file in the column name provided by positionColumn.

 Revision 1.21  2005/11/04 22:46:17  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.20  2004/03/16 23:44:25  borland
 Fixed parsing bug that caused problems when column named looked like an abbreviated tag name

 Revision 1.19  2002/08/14 17:12:53  soliday
 Added Open License

 Revision 1.18  2001/01/10 19:35:45  soliday
 Standardized usage message.

 Revision 1.17  2000/05/31 20:05:09  borland
 Updated version number in usage message.

 Revision 1.16  2000/05/22 17:56:23  borland
 Added -spread processing.

 Revision 1.15  1999/09/28 15:38:21  soliday
 Added SDDS_Terminate at the end.

 Revision 1.14  1999/05/25 19:14:34  soliday
 Removed compiler warning on linux.

 Revision 1.13  1999/01/06 19:54:54  borland
 Fixed the version number in the usage message.

 Revision 1.12  1998/06/02 21:23:45  borland
 Fixed problem with setting description and symbol to NULL for newly
 created columns (wasn't doing it but I wanted it to).

 Revision 1.11  1998/04/24 14:32:04  borland
 Added -smallest and -largest processing (min and max of absolute value).

 Revision 1.10  1998/04/23 19:00:16  borland
 Added decile and quartile range computation.

 Revision 1.9  1998/03/26 23:06:17  borland
 Added help for -rms option.

 Revision 1.8  1997/10/19 21:14:44  borland
 If a statistics request doesn't have any matching source columns,
 a warning is issued; previously, the program aborted.

 Revision 1.7  1997/08/28 14:16:35  borland
 Added median and mean-absolute-deviation processing.

 Revision 1.6  1996/11/13 03:47:37  borland
 Inserted statements to set top and bottom limits for sum processing.

 Revision 1.5  1996/11/07 15:11:37  borland
 Now does file replacement and has -nowarnings option.

 Revision 1.4  1996/10/23 17:57:24  borland
 Added text for -mean option to usage message.

 Revision 1.3  1996/09/20 19:59:02  borland
 Added -count statistic.

 Revision 1.2  1996/09/20 19:43:04  borland
 Added topLimit and bottomLimit qualifiers to all statistics options.
 Allows filtering data to be included in averages.

 * Revision 1.1  1996/05/29  21:42:16  borland
 * Added new program sddsrowstats.  Improved error messages in SDDSutils.c
 * routine (expandColumnPairNames).
 *
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include <ctype.h>

#define SET_MAXIMUM 0
#define SET_MINIMUM 1
#define SET_MEAN 2
#define SET_STANDARDDEVIATION 3
#define SET_RMS 4
#define SET_SUM 5
#define SET_SIGMA 6
#define SET_COUNT 7
#define SET_PIPE 8
#define SET_MEDIAN 9
#define SET_MAD 10
#define SET_NOWARNINGS 11
#define SET_DRANGE 12
#define SET_QRANGE 13
#define SET_LARGEST 14
#define SET_SMALLEST 15
#define SET_SPREAD 16
#define N_OPTIONS 17

char *option[N_OPTIONS] = {
  "maximum", "minimum", "mean", "standarddeviation", "rms", "sum",
  "sigma", "count", "pipe", "median", "mad", "nowarnings", "drange",
  "qrange", "largest", "smallest", "spread",
    } ;


#define TOPLIMIT_GIVEN    0x0001U
#define BOTTOMLIMIT_GIVEN 0x0002U
#define POSITIONCOLUMN_GIVEN 0x0004U

/* this structure stores a command-line request for statistics computation */
/* individual elements of sourceColumn may contain wildcards */
typedef struct {
    char **sourceColumn, *resultColumn, *positionColumn;
    long sourceColumns, sumPower, optionCode, positionColumnIndex;
    unsigned long flags;
    double topLimit, bottomLimit;
    } STAT_REQUEST;

/* this structure stores data necessary for accessing/creating SDDS columns and
 * for computing a statistic
 */
typedef struct {
    char **sourceColumn, *resultColumn, *positionColumn;
    long sourceColumns, optionCode, resultIndex, sumPower, positionColumnIndex;
    unsigned long flags;
    double topLimit, bottomLimit;
    } STAT_DEFINITION;

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, 
                     long items, long code, unsigned long flag);
STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inTable, 
                                        STAT_REQUEST *request, long requests, long *stats,
                                        long noWarnings);
long setupOutputFile(SDDS_DATASET *outTable, char *output, SDDS_DATASET *inTable, 
                     STAT_DEFINITION *stat, long stats);

static char *USAGE="sddsrowstats [<input>] [<output>] [-pipe[=input][,output]]\n\
[-nowarnings]\n\
[-mean=<newName>,[,<limitOps>],<columnNameList>]\n\
[-rms=<newName>,[,<limitOps>],<columnNameList>]\n\
[-median=<newName>[,<limitOps>],<columnNameList>]\n\
[-minimum=<newName>[,positionColumn=<name>][,<limitOps>],<columnNameList>]\n\
[-maximum=<newName>[,positionColumn=<name>][,<limitOps>],<columnNameList>]\n\
[-standardDeviation=<newName>[,<limitOps>],<columnNameList>]\n\
[-sigma=<newName>[,<limitOps>],<columnNameList>]\n\
[-mad=<newName>[,<limitOps>],<columnNameList>]\n\
[-sum=<newName>[,<limitOps>][,power=<integer>],<columnNameList>] \n\
[-spread=<newName>[,<limitOps>],<columnNameList>]\n\
[-drange=<newName>[,<limitOps>],<columnNameList>]\n\
[-qrange=<newName>[,<limitOps>],<columnNameList>]\n\
[-smallest=<newName>[,positionColumn=<name>][,<limitOps>],<columnNameList>]\n\
[-largest=<newName>[,positionColumn=<name>][,<limitOps>],<columnNameList>]\n\
[-count=<newName>[,<limitOps>],<columnNameList>]\n\n\
  <limitOps> is of the form [topLimit=<value>,][bottomLimit=<value>]\n\n\
Computes statistics for each row of each input table, adding new columns to the\n\
output table.  Each row statistic is done using data from the columns listed in\n\
<columnNameList>, which is a comma-separated list of optionally-wildcarded column\n\
names. positionColumn=<name> for minimum, maximum, smallest, largest option is to store \n\
the corresponding column name of the minimum, maximum, smallest, or largest in each row to \n\
the new output column - <name>.\n\n\
Program by Michael Borland. (This is version 5, May 2000.)\n";

int main(int argc, char **argv)
{
  STAT_DEFINITION *stat;
  long stats;
  STAT_REQUEST *request;
  long requests, count;
  SCANNED_ARG *scanned; 
  SDDS_DATASET inData, outData;

  int32_t power;
  long i_arg, code, iStat, rows, tmpFileUsed, iColumn, row, posColIndex;
  long noWarnings, maxSourceColumns;
  char *input, *output, *positionColumn, **posColumnName;
  double **inputData, *outputData, value1, value2, topLimit, bottomLimit;
  unsigned long pipeFlags, scanFlags;
  char s[100];
  double *statWorkArray;
  double quartilePoint[2] = {25.0, 75.0 }, quartileResult[2];
  double decilePoint[2]   = {10.0, 90.0 }, decileResult[2];

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2) {
    bomb("too few arguments", USAGE);
  }
  
  posColumnName = NULL;
  input = output = positionColumn = NULL;
  stat = NULL;
  request = NULL;
  stats = requests = pipeFlags = 0;
  noWarnings = 0;
  outputData = NULL;
  statWorkArray = NULL;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    scanFlags = 0;
    if (scanned[i_arg].arg_type==OPTION) {
      /* process options here */
      switch (code=match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_MAXIMUM:
      case SET_MINIMUM:
      case SET_MEAN:
      case SET_MEDIAN:
      case SET_STANDARDDEVIATION:
      case SET_RMS:
      case SET_SIGMA:
      case SET_MAD:
      case SET_COUNT:
      case SET_DRANGE:
      case SET_QRANGE:
      case SET_SMALLEST:
      case SET_LARGEST:
      case SET_SPREAD:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        if (!scanItemList(&scanFlags, scanned[i_arg].list, &scanned[i_arg].n_items,
                          SCANITEMLIST_UNKNOWN_VALUE_OK|SCANITEMLIST_REMOVE_USED_ITEMS|
                          SCANITEMLIST_IGNORE_VALUELESS,
                          "positionColumn", SDDS_STRING, &positionColumn, 1, POSITIONCOLUMN_GIVEN,
                          "toplimit", SDDS_DOUBLE, &topLimit, 1, TOPLIMIT_GIVEN,
                          "bottomlimit", SDDS_DOUBLE, &bottomLimit, 1, BOTTOMLIMIT_GIVEN,
                          NULL)) {
          sprintf(s, "invalid -%s syntax", scanned[i_arg].list[0]);
          SDDS_Bomb(s);
        }
        requests = addStatRequests(&request, requests, scanned[i_arg].list+1, scanned[i_arg].n_items-1,
                                   code, scanFlags);
        request[requests-1].topLimit = topLimit;
        request[requests-1].bottomLimit = bottomLimit;
        if (positionColumn) {
          if (code==SET_MAXIMUM || code==SET_MINIMUM || code==SET_LARGEST || code==SET_SMALLEST) 
            SDDS_CopyString(&request[requests-1].positionColumn, positionColumn);
          free(positionColumn);
          positionColumn = NULL;
        }
        break;
      case SET_SUM:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        power = 1;
        if (!scanItemList(&scanFlags, scanned[i_arg].list, &scanned[i_arg].n_items, 
                          SCANITEMLIST_UNKNOWN_VALUE_OK|SCANITEMLIST_REMOVE_USED_ITEMS|
                          SCANITEMLIST_IGNORE_VALUELESS,
                          "power", SDDS_LONG, &power, 1, 0, 
                          "toplimit", SDDS_DOUBLE, &topLimit, 1, TOPLIMIT_GIVEN,
                          "bottomlimit", SDDS_DOUBLE, &bottomLimit, 1, BOTTOMLIMIT_GIVEN,
                          NULL))
          SDDS_Bomb("invalid -sum syntax");
        requests = addStatRequests(&request, requests, scanned[i_arg].list+1, scanned[i_arg].n_items-1,
                                   code, scanFlags);
        request[requests-1].sumPower = power;
        request[requests-1].topLimit = topLimit;
        request[requests-1].bottomLimit = bottomLimit;
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_NOWARNINGS:
        noWarnings = 1;
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
        input = scanned[i_arg].list[0];
      else if (!output)
        output = scanned[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }
  
  processFilenames("sddsrowstats", &input, &output, pipeFlags, noWarnings, &tmpFileUsed);

  if (!requests)
    SDDS_Bomb("no statistics requested");
  
  if (!SDDS_InitializeInput(&inData, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(stat=compileStatDefinitions(&inData, request, requests, &stats, noWarnings))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (stats<0) 
    SDDS_Bomb("No valid statistics requests.");
  for (iStat=maxSourceColumns=0; iStat<stats; iStat++) {
    if (stat[iStat].sourceColumns>maxSourceColumns)
      maxSourceColumns = stat[iStat].sourceColumns;
  }
  if (!(statWorkArray=malloc(sizeof(*statWorkArray)*maxSourceColumns)))
    SDDS_Bomb("allocation failure (statWorkArray)");

  if (!setupOutputFile(&outData, output, &inData, stat, stats)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  inputData = NULL;

  while ((code=SDDS_ReadPage(&inData))>0) {
    if (!SDDS_CopyPage(&outData, &inData)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if ((rows = SDDS_CountRowsOfInterest(&inData))) {
      if (!(outputData = (double*)malloc(sizeof(*outputData)*rows))) 
        SDDS_Bomb("memory allocation failure");
      if (!(posColumnName = (char**)malloc(sizeof(*posColumnName)*rows))) 
        SDDS_Bomb("memory allocation failure");
      for (iStat=0; iStat<stats; iStat++) {
        if (!(inputData = (double**)malloc(sizeof(*inputData)*stat[iStat].sourceColumns)))
          SDDS_Bomb("memory allocation failure");
        for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
          if (!(inputData[iColumn] = SDDS_GetColumnInDoubles(&inData, stat[iStat].sourceColumn[iColumn])))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        for (row=0; row<rows; row++) 
          outputData[row] = DBL_MAX;
        switch (stat[iStat].optionCode) {
        case SET_MINIMUM:
          for (row=0; row<rows; row++) {
            value1 = DBL_MAX;
            posColIndex = 0;
            posColumnName[row] = NULL;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              if (inputData[iColumn][row]<value1) {
                value1 = inputData[iColumn][row];
                posColIndex = iColumn;
              }
            }
            outputData[row] = value1;
            if (stat[iStat].positionColumn) 
              posColumnName[row] = stat[iStat].sourceColumn[posColIndex];
          }
          break;
        case SET_MAXIMUM:
          for (row=0; row<rows; row++) {
            posColIndex = 0;
            value1 = -DBL_MAX;
            posColumnName[row] = NULL;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              if (inputData[iColumn][row]>value1) {
                posColIndex = iColumn; 
                value1 = inputData[iColumn][row];
              }
            }
            outputData[row] = value1;
            if (stat[iStat].positionColumn) 
              posColumnName[row] = stat[iStat].sourceColumn[posColIndex];
          }
          break;
        case SET_MEAN:
          for (row=0; row<rows; row++) {
            value1 = 0;
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              value1 += inputData[iColumn][row];
              count ++;
            }
            if (count)
              outputData[row] = value1/count;
          }
          break;
        case SET_MEDIAN:
          for (row=0; row<rows; row++) {
            for (iColumn=count=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              statWorkArray[count] = inputData[iColumn][row];
              count++;
            }
            if (count)
              compute_median(outputData+row, statWorkArray, count);
          }
          break;
        case SET_STANDARDDEVIATION:
          for (row=0; row<rows; row++) {
            value1 = 0;
            value2 = 0;
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              value1 += inputData[iColumn][row];
              value2 += inputData[iColumn][row]*inputData[iColumn][row];
              count ++;
            }
            if (count>1) {
              if ((value1 = value2/count - sqr(value1/count))<=0)
                outputData[row] = 0;
              else
                outputData[row] = sqrt(value1*count/(count-1.0));
            }
          }
          break;
        case SET_SIGMA:
          for (row=0; row<rows; row++) {
            value1 = 0;
            value2 = 0;
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              value1 += inputData[iColumn][row];
              value2 += inputData[iColumn][row]*inputData[iColumn][row];
              count ++;
            }
            if (count>1) {
              if ((value1 = value2/count - sqr(value1/count))<=0)
                outputData[row] = 0;
              else
                outputData[row] = sqrt(value1/(count-1.0));
            }
          }
          break;
        case SET_RMS:
          for (row=0; row<rows; row++) {
            value1 = 0;
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              value1 += sqr(inputData[iColumn][row]);
              count ++;
            }
            if (count)
              outputData[row] = sqrt(value1/count);
          }
          break;
        case SET_SUM:
          for (row=0; row<rows; row++) {
            value1 = 0;
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              value1 += ipow(inputData[iColumn][row], stat[iStat].sumPower);
              count ++;
            }
            if (count)
              outputData[row] = value1;
          }
          break;
        case SET_COUNT:
          for (row=0; row<rows; row++) {
            count = 0;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              count++;
            }
            outputData[row] = count;
          }
          break;
        case SET_MAD:
          for (row=0; row<rows; row++) {
            for (iColumn=count=value1=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              statWorkArray[count] = inputData[iColumn][row];
              count++;
            }
            if (count)
              computeMoments(NULL, NULL, NULL, &outputData[row],
                             statWorkArray, count);
          }
          break;
        case SET_DRANGE:
          for (row=0; row<rows; row++) {
            for (iColumn=count=value1=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN &&
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              statWorkArray[count] = inputData[iColumn][row];
              count++;
            }
            if (count && 
                compute_percentiles(decileResult, decilePoint, 2, 
                                    statWorkArray, count))
              outputData[row] = decileResult[1] - decileResult[0];
          }
          break;
        case SET_QRANGE:
          for (row=0; row<rows; row++) {
            for (iColumn=count=value1=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN &&
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN &&
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              statWorkArray[count] = inputData[iColumn][row];
              count++;
            }
            if (count && 
                compute_percentiles(quartileResult, quartilePoint, 2, 
                                    statWorkArray, count))
              outputData[row] = quartileResult[1] - quartileResult[0];
          }
          break;
        case SET_SMALLEST:
          for (row=0; row<rows; row++) {
            value1 = DBL_MAX;
            posColIndex = 0;
            posColumnName[row] = NULL;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              if ((value2=fabs(inputData[iColumn][row]))<value1) {
                posColIndex = iColumn;
                value1 = value2;
              }
            }
            outputData[row] = value1; 
            if (stat[iStat].positionColumn) 
              posColumnName[row] = stat[iStat].sourceColumn[posColIndex];
          }
          break;
        case SET_LARGEST:
          for (row=0; row<rows; row++) {
            value1 = 0;
            posColIndex = 0;
            posColumnName[row] = NULL;
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              if ((value2=fabs(inputData[iColumn][row]))>value1) {
                posColIndex = iColumn;
                value1 = value2;
              }
            }
            outputData[row] = value1;
            if (stat[iStat].positionColumn) 
              posColumnName[row] = stat[iStat].sourceColumn[posColIndex];
          }
          break;
        case SET_SPREAD:
          for (row=0; row<rows; row++) {
            value1 = DBL_MAX;  /* min */
            value2 = -DBL_MAX; /* max */
            for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++) {
              if (stat[iStat].flags&TOPLIMIT_GIVEN && 
                  inputData[iColumn][row]>stat[iStat].topLimit)
                continue;
              if (stat[iStat].flags&BOTTOMLIMIT_GIVEN && 
                  inputData[iColumn][row]<stat[iStat].bottomLimit)
                continue;
              if (inputData[iColumn][row]<value1)
                value1 = inputData[iColumn][row];
              if (inputData[iColumn][row]>value2)
                value2 = inputData[iColumn][row];
            }
            outputData[row] = value2-value1;
          }
          break;
        default:
          SDDS_Bomb("invalid statistic code (accumulation loop)");
          break;
        }
        if (!SDDS_SetColumn(&outData, SDDS_SET_BY_INDEX, outputData, rows, stat[iStat].resultIndex))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        
        if (stat[iStat].positionColumn) {
          if (!SDDS_SetColumn(&outData, SDDS_SET_BY_INDEX, posColumnName, rows, 
                              stat[iStat].positionColumnIndex))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++)
          free(inputData[iColumn]);
        free(inputData);
        inputData = NULL;
      }
      free(outputData);
      outputData = NULL;
      free(posColumnName);
      posColumnName = NULL;
    }
    if (!SDDS_WritePage(&outData))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  free_scanargs(&scanned, argc);
  for (iStat=0; iStat<stats; iStat++) { 
    if (stat[iStat].positionColumn) free(stat[iStat].positionColumn);
    for (iColumn=0; iColumn<stat[iStat].sourceColumns; iColumn++)
      free(stat[iStat].sourceColumn[iColumn]);
    free(stat[iStat].sourceColumn); 
  }
  free(request);
  free(stat);
  if (statWorkArray) free(statWorkArray);
  
  if (!SDDS_Terminate(&inData) || !SDDS_Terminate(&outData)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  } 
  if (tmpFileUsed && !replaceFileAndBackUp(input, output))
    exit(1);
  return 0;
}

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, 
                     long items, long code, unsigned long flags)
{
  long i;
  if (!(*statRequest = SDDS_Realloc(*statRequest, sizeof(**statRequest)*(requests+1))) ||
      !((*statRequest)[requests].sourceColumn = (char**)malloc(sizeof(*(*statRequest)[requests].sourceColumn)*
                                                               (items-1))))
    SDDS_Bomb("memory allocation failure");
  for (i=0; i<items-1; i++)
    SDDS_CopyString(&((*statRequest)[requests].sourceColumn[i]), item[i+1]);
  (*statRequest)[requests].resultColumn = item[0];
  (*statRequest)[requests].sourceColumns = items-1;
  (*statRequest)[requests].optionCode = code;
  (*statRequest)[requests].sumPower = 1;
  (*statRequest)[requests].flags = flags;
  (*statRequest)[requests].positionColumn = NULL; 
  
  return requests+1;
}

STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inData, STAT_REQUEST *request, long requests, long *stats,
                                        long noWarnings)
{
  STAT_DEFINITION *stat;
  long iReq, iName, iStat;

  if (!(stat = (STAT_DEFINITION *)malloc(sizeof(*stat)*requests)))
    SDDS_Bomb("memory allocation failure");
  for (iReq=iStat=0; iReq<requests; iReq++) {
    if ((stat[iStat].sourceColumns 
         = expandColumnPairNames(inData, &request[iReq].sourceColumn, NULL,
                                 request[iReq].sourceColumns, NULL, 0, 
                                 FIND_NUMERIC_TYPE, 0))<=0) {
      if (!noWarnings) {
        fprintf(stderr, "Warning: no match for column names (sddsrowstats):\n");
        for (iName=0; iName<request[iReq].sourceColumns; iName++)
          fprintf(stderr, "%s, ", request[iReq].sourceColumn[iName]);
        fputc('\n', stderr);
      }
    } else {
      stat[iStat].sourceColumn = request[iReq].sourceColumn;
      stat[iStat].resultColumn = request[iReq].resultColumn;
      stat[iStat].optionCode   = request[iReq].optionCode;
      stat[iStat].sumPower     = request[iReq].sumPower;
      stat[iStat].flags        = request[iReq].flags;
      stat[iStat].topLimit     = request[iReq].topLimit;
      stat[iStat].bottomLimit  = request[iReq].bottomLimit;
      stat[iStat].positionColumn = request[iReq].positionColumn;
      iStat++;
    }
    *stats = iStat;
  }
  
  return stat;
}

long setupOutputFile(SDDS_DATASET *outData, char *output, SDDS_DATASET *inData, 
                     STAT_DEFINITION *stat, long stats)
{
  long column;
  char s[SDDS_MAXLINE];
  
  if (!SDDS_InitializeCopy(outData, inData, output, "w"))
    return 0;
  for (column=0; column<stats; column++) {
    if (!SDDS_TransferColumnDefinition(outData, inData, stat[column].sourceColumn[0],
                                       stat[column].resultColumn)) {
      sprintf(s, "Problem transferring definition of column %s to %s\n",
              stat[column].sourceColumn[0], stat[column].resultColumn);
      SDDS_SetError(s);
      return 0;
    }
    if ((stat[column].resultIndex 
         = SDDS_GetColumnIndex(outData, stat[column].resultColumn))<0) {
      sprintf(s, "Problem creating column %s", stat[column].resultColumn);
      SDDS_SetError(s);
      return 0;
    }
    if (stat[column].positionColumn) {
      if (!SDDS_DefineSimpleColumn(outData, stat[column].positionColumn, NULL, SDDS_STRING)) {
        sprintf(s, "Problem define column %s\n", stat[column].positionColumn);
        SDDS_SetError(s);
        return 0;
      }
      if ((stat[column].positionColumnIndex 
           = SDDS_GetColumnIndex(outData, stat[column].positionColumn))<0) {
        sprintf(s, "Problem creating column %s", stat[column].positionColumn);
        SDDS_SetError(s);
        return 0;
      }
    }
    if (!SDDS_ChangeColumnInformation
        (outData, "description", "", SDDS_SET_BY_NAME, stat[column].resultColumn) ||
        !SDDS_ChangeColumnInformation
        (outData, "symbol", "", SDDS_SET_BY_NAME, stat[column].resultColumn) ||
        !SDDS_ChangeColumnInformation
        (outData, "type", "double", SDDS_SET_BY_NAME|SDDS_PASS_BY_STRING, stat[column].resultColumn)) {
      sprintf(s, "Problem changing attributes of new column %s", stat[column].resultColumn);
      SDDS_SetError(s);
      return 0;
    }
  }
  if (!SDDS_WriteLayout(outData))
    return 0;
  return 1;
}

