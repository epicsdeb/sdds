/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsoutlier
 * purpose: eliminate statistical outliers from data
 *
 * Michael Borland, 1995
 $Log: sddsoutlier.c,v $
 Revision 1.18  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.17  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.16  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.15  2005/11/04 22:46:16  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.14  2002/08/14 17:12:50  soliday
 Added Open License

 Revision 1.13  2001/01/10 19:35:42  soliday
 Standardized usage message.

 Revision 1.12  2000/04/13 18:04:09  soliday
 Removed invalid prototype.

 Revision 1.11  2000/04/13 17:11:09  soliday
 Added missing include statment.

 Revision 1.10  1999/05/25 19:12:47  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/13 17:59:01  borland
 Improved help message.  Fixed bug with -maximumLimit.

 Revision 1.8  1999/01/04 21:17:42  borland
 Officially committing -replaceOnly option.  Allows replacing outlier data
 on a column-by-column basis with data that is drawn from nearby values.

 Revision 1.7  1996/07/12 17:03:47  borland
 Added -passes option to do multiple outlier passes in
 one run.  Added -chanceLimit option to find outliers
 based on a lower limit to the probability of seeing
 the point (using gaussian statistics).

 * Revision 1.6  1996/05/31  20:53:26  borland
 * Improved function of -markOnly option by using IsOutlier data from
 * input file, if it is present.
 *
 * Revision 1.5  1996/05/31  20:32:57  borland
 * Added -markOnly option (marks outlier rows rather than deleting them).
 *
 * Revision 1.4  1996/01/25  19:03:04  borland
 * Added -invert option.
 *
 * Revision 1.3  1995/09/06  14:56:51  saunders
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
#define SET_STDDEV_LIMIT 2
#define SET_ABS_LIMIT 3
#define SET_ABSDEV_LIMIT 4
#define SET_VERBOSE 5
#define SET_PIPE 6
#define SET_NOWARNINGS 7
#define SET_INVERT 8
#define SET_MARKONLY 9
#define SET_CHANCELIMIT 10
#define SET_PASSES 11
#define SET_REPLACE 12
#define SET_MAXLIMIT 13
#define SET_MINLIMIT 14
#define N_OPTIONS 15

char *option[N_OPTIONS] = {
    "columns", "excludecolumns", "stdevlimit", "abslimit", "absdeviationlimit", "verbose",
    "pipe", "nowarnings", "invert", "markonly", "chancelimit", "passes",
    "replaceonly", "maximumlimit", "minimumlimit",
    };

#define USAGE "sddsoutlier [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
[-verbose] [-noWarnings] [-columns=<list-of-names>] \n\
[-excludeColumns=<list-of-names>]\n\
[-stDevLimit=<value>] [-absLimit=<value>] \n\
[-absDeviationLimit=<value>]\n\
[-maximumLimit=<value>] [-minimumLimit=<value>]\n\
[-chanceLimit=<minimumChance>] [-passes=<number>] [-invert]\n\
[{-markOnly | \n\
  -replaceOnly={lastValue|nextValue|interpolatedValue|value=<number>}}]\n\n\
-stDevLimit=<value>    Point is outlier if it is more than <value> standard deviations\n\
                       from mean.\n\
-absLimit=<value>      Point is an outlier if it has absolute value greater than <value>\n\
-absDeviationLimit=<value>\n\
                       Point is an outlier if its absolute deviation from the mean exceeds <value>\n\
-minimumLimit=<value>  Point is an outlier if it is less than <value>\n\
-maximumLimit=<value>  Point is an outlier if it is greater than <value>\n\
-chanceLimit=<minimumChance>\n\
                       Point is an outlier if it has probability less than\n\
                       <minimumChance> of occuring (gaussian statistics).\n\n\
Program by Michael Borland.  (This is version 5, December 1998.)\n"

#define OUTLIER_CONTROL_INVOKED     0x00001U
#define OUTLIER_STDEV_GIVEN         0x00002U
#define OUTLIER_FRACTION_GIVEN      0x00004U
#define OUTLIER_STDEVLIMIT_GIVEN    0x00008U
#define OUTLIER_PERCENTILE_GIVEN    0x00010U
#define OUTLIER_VERBOSE_GIVEN       0x00020U
#define OUTLIER_ABSLIMIT_GIVEN      0x00040U
#define OUTLIER_ABSDEVLIMIT_GIVEN   0x00080U
#define OUTLIER_INVERT_GIVEN        0x00100U
#define OUTLIER_MARKONLY            0x00200U
#define OUTLIER_CHANCELIMIT_GIVEN   0x00400U
#define OUTLIER_MAXLIMIT_GIVEN      0x00800U
#define OUTLIER_MINLIMIT_GIVEN      0x01000U
#define OUTLIER_REPLACELAST         0x02000U
#define OUTLIER_REPLACENEXT         0x04000U
#define OUTLIER_REPLACEINTERP       0x08000U
#define OUTLIER_REPLACEVALUE        0x10000U
#define OUTLIER_REPLACEFLAGS (OUTLIER_REPLACELAST|OUTLIER_REPLACENEXT|OUTLIER_REPLACEINTERP|OUTLIER_REPLACEVALUE)
typedef struct {
    double stDevLimit, fractionLimit, absoluteLimit, absDevLimit;
    double chanceLimit, replacementValue, maximumLimit, minimumLimit;
    long passes;
    unsigned long flags;
    } OUTLIER_CONTROL;

long removeOutliers(SDDS_DATASET *SDDSin, long rows, char **column, long columns, 
                    OUTLIER_CONTROL *outlierControl, int32_t *isOutlier);
long meanStDevForFlaggedData(double *mean, double *stDev,
                             double *data, int32_t *keep, long rows);

int main(int argc, char **argv)
{
  int iArg;
  char **column, **excludeColumn;
  long columns, excludeColumns;
  char *input, *output;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  long i, rows, readCode, dataLimitGiven, tmpfileUsed;
  long noWarnings, isOutlierIndex;
  int32_t *isOutlier;
  OUTLIER_CONTROL outlierControl;
  unsigned long pipeFlags, tmpFlags;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3)
    bomb(NULL, USAGE);

  output = input =  NULL;
  columns = excludeColumns = dataLimitGiven = 0;
  column = excludeColumn = NULL;
  
  outlierControl.flags = 0;
  outlierControl.passes = 1;
  pipeFlags = tmpfileUsed = noWarnings = isOutlierIndex = 0;
  
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
        if (excludeColumns)
          SDDS_Bomb("only one -excludecolumns option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -excludecolumns syntax");
        excludeColumn = tmalloc(sizeof(*excludeColumn)*(excludeColumns=scanned[iArg].n_items-1));
        for (i=0; i<excludeColumns; i++)
          excludeColumn[i] = scanned[iArg].list[i+1];
        break;
      case SET_STDDEV_LIMIT:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.stDevLimit)!=1 || 
            outlierControl.stDevLimit<=0)
          SDDS_Bomb("invalid -standardDeviationLimit syntax");
        outlierControl.flags |= OUTLIER_CONTROL_INVOKED|OUTLIER_STDEV_GIVEN|OUTLIER_STDEVLIMIT_GIVEN;
        break;
      case SET_ABS_LIMIT:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.absoluteLimit)!=1 || 
            outlierControl.absoluteLimit<=0)
          SDDS_Bomb("invalid -absoluteLimit syntax");
        outlierControl.flags |= OUTLIER_CONTROL_INVOKED|OUTLIER_ABSLIMIT_GIVEN;
        break;
      case SET_ABSDEV_LIMIT:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.absDevLimit)!=1 || 
            outlierControl.absDevLimit<=0)
          SDDS_Bomb("invalid -absDevLimit syntax");
        outlierControl.flags |= OUTLIER_CONTROL_INVOKED|OUTLIER_ABSDEVLIMIT_GIVEN;
        break;
      case SET_VERBOSE:
        outlierControl.flags |= OUTLIER_VERBOSE_GIVEN;
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_NOWARNINGS:
        noWarnings = 1;
        break;
      case SET_INVERT:
        outlierControl.flags |= OUTLIER_INVERT_GIVEN;
        break;
      case SET_MARKONLY:
        outlierControl.flags |= OUTLIER_MARKONLY;
        break;
      case SET_CHANCELIMIT:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.chanceLimit)!=1 || 
            outlierControl.chanceLimit<=0)
          SDDS_Bomb("invalid -chanceLimit syntax");
        outlierControl.flags |= OUTLIER_CONTROL_INVOKED|OUTLIER_CHANCELIMIT_GIVEN;
        break;
      case SET_PASSES:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%ld", &outlierControl.passes)!=1 ||
            outlierControl.passes<1)
          SDDS_Bomb("invalid -passes syntax");
        break;
      case SET_MAXLIMIT:
        outlierControl.flags |= OUTLIER_MAXLIMIT_GIVEN|OUTLIER_CONTROL_INVOKED;
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.maximumLimit)!=1)
          SDDS_Bomb("invalid -maximumLimit syntax");
        break;
      case SET_MINLIMIT:
        outlierControl.flags |= OUTLIER_MINLIMIT_GIVEN|OUTLIER_CONTROL_INVOKED;
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &outlierControl.minimumLimit)!=1)
          SDDS_Bomb("invalid -minimumLimit syntax");
        break;
      case SET_REPLACE:
        if (scanned[iArg].n_items!=2)
          SDDS_Bomb("invalid -replace syntax");
        scanned[iArg].n_items -= 1;
        if (!scanItemList(&tmpFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "lastvalue", -1, NULL, 0, OUTLIER_REPLACELAST,
                          "nextvalue", -1, NULL, 0, OUTLIER_REPLACENEXT,
                          "interpolatedvalue", -1, NULL, 0, OUTLIER_REPLACEINTERP,
                          "value", SDDS_DOUBLE, &outlierControl.replacementValue, 1, OUTLIER_REPLACEVALUE,
                          NULL)) 
          SDDS_Bomb("invalid -replace syntax/values");
        outlierControl.flags |= tmpFlags|OUTLIER_CONTROL_INVOKED;
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
  if (outlierControl.flags&OUTLIER_REPLACEFLAGS &&
      outlierControl.flags&OUTLIER_MARKONLY)
    SDDS_Bomb("Can't give -replaceOnly and -markOnly");
  
  processFilenames("sddsoutlier", &input, &output, pipeFlags, noWarnings, &tmpfileUsed);

  if (!(outlierControl.flags&OUTLIER_CONTROL_INVOKED)) {
    outlierControl.flags |= OUTLIER_CONTROL_INVOKED|OUTLIER_STDEV_GIVEN|OUTLIER_STDEVLIMIT_GIVEN;
    outlierControl.stDevLimit = 2;
  }

  if (!SDDS_InitializeInput(&SDDSin, input) || !SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (outlierControl.flags&OUTLIER_MARKONLY && (isOutlierIndex=SDDS_GetColumnIndex(&SDDSout, "IsOutlier"))<0) {
    if (!SDDS_DefineColumn(&SDDSout, "IsOutlier", NULL, NULL, NULL, NULL, SDDS_SHORT, 0))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if ((columns=expandColumnPairNames(&SDDSout, &column, NULL, columns, excludeColumn,
                                     excludeColumns, FIND_NUMERIC_TYPE, 0))<=0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    SDDS_Bomb("no columns selected for outlier control");
  }

  isOutlier = NULL;
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if (!SDDS_CopyPage(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if ((rows = SDDS_CountRowsOfInterest(&SDDSout))<3) {
      if (!SDDS_WritePage(&SDDSout))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      continue;
    }
    if (outlierControl.flags&OUTLIER_MARKONLY) {
      if (isOutlierIndex>0) {
        if (!(isOutlier = SDDS_GetNumericColumn(&SDDSout, "IsOutlier", SDDS_LONG)))
          SDDS_Bomb("Unable to get IsOutlier column from input file even though it appears to exist.\n");
      } else {
        long i;
        if (!(isOutlier = SDDS_Realloc(isOutlier, sizeof(*isOutlier)*rows)))
          SDDS_Bomb("memory allocation failure");
        for (i=0; i<rows; i++)
          isOutlier[i] = 0;
      }
    }
    if (outlierControl.flags&OUTLIER_VERBOSE_GIVEN)
      fprintf(stderr, "%ld rows in page %ld\n", rows, readCode);
    if ((rows = removeOutliers(&SDDSout, rows, column, columns, &outlierControl, isOutlier))==0) {
      fprintf(stderr, "  no rows left after outlier control--skipping page\n");
      continue;
    }
    if (outlierControl.flags&OUTLIER_VERBOSE_GIVEN)
      fprintf(stderr, "%ld rows left after outlier control\n", rows);
    if (rows!=SDDS_CountRowsOfInterest(&SDDSout)) {
      fprintf(stderr, "problem with row selection:\n  %ld expected, %" PRId32 " counted\n",
              rows, SDDS_CountRowsOfInterest(&SDDSout));
      exit(1);
    }
    if ((isOutlier && 
         !SDDS_SetColumnFromLongs(&SDDSout, SDDS_SET_BY_NAME, isOutlier, rows, "IsOutlier")) ||
        !SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (tmpfileUsed && !replaceFileAndBackUp(input, output)) 
    exit(1);
  return 0;
}

long removeOutliers(SDDS_DATASET *dataset, long rows, char **column, long columns, 
                    OUTLIER_CONTROL *outlierControl, int32_t *isOutlier)
{
  long icol, irow, kept, killed, summed, ipass;
  double *data, sum1, stDev, mean;
  static int32_t *keep = NULL;
  double lastGoodValue = 0;
  long irow0, irow1;
  
  if (!SDDS_SetRowFlags(dataset, 1)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  /* make a flag array to indicate which rows to keep, and initialize it */
  keep = SDDS_Realloc(keep, sizeof(*keep)*rows);
  if (!isOutlier) {
    for (irow=0; irow<rows; irow++)
      keep[irow] = 1;
    kept = rows;
  } else {
    for (irow=kept=0; irow<rows; irow++)
      if ((keep[irow] = !isOutlier[irow]))
        kept++;
  }
    
  for (icol=0; icol<columns; icol++) {
    /* loop over columns for which outlier control is to be done */
    if (!(data = SDDS_GetColumnInDoubles(dataset, column[icol])))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (ipass=0; ipass<outlierControl->passes; ipass++) {
      if (outlierControl->flags&OUTLIER_MINLIMIT_GIVEN) {
        /* limit values to exceed a minimum */
        for (irow=killed=0; irow<rows; irow++) {
          if (keep[irow] && data[irow]<outlierControl->minimumLimit) {
            kept--;
            keep[irow] = 0;
            killed++;
          }
        }
        if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
          fprintf(stderr, "%ld additional rows killed by column %s minimum value outlier control\n",
                  killed, column[icol]);
      }
      if (outlierControl->flags&OUTLIER_MAXLIMIT_GIVEN) {
        /* limit values to exceed a maximum */
        for (irow=killed=0; irow<rows; irow++) {
          if (keep[irow] && data[irow]>outlierControl->maximumLimit) {
            kept--;
            keep[irow] = 0;
            killed++;
          }
        }
        if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
          fprintf(stderr, "%ld additional rows killed by column %s maximum value outlier control\n",
                  killed, column[icol]);
      }
      if (outlierControl->flags&OUTLIER_ABSLIMIT_GIVEN) {
        /* limit absolute values */
        for (irow=killed=0; irow<rows; irow++) {
          if (keep[irow] && fabs(data[irow])>outlierControl->absoluteLimit) {
            kept--;
            keep[irow] = 0;
            killed++;
          }
        }
        if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
          fprintf(stderr, "%ld additional rows killed by column %s absolute value outlier control\n",
                  killed, column[icol]);
      }
      if (outlierControl->flags&OUTLIER_ABSDEVLIMIT_GIVEN) {
        /* limit absolute deviate from mean */
        for (irow=sum1=summed=0; irow<rows; irow++) {
          if (!keep[irow])
            continue;
          sum1 += data[irow];
          summed++;
        }
        if (summed<1)
          continue;
        mean = sum1/summed;
        for (irow=killed=0; irow<rows; irow++)
          if (keep[irow] && fabs(data[irow]-mean)>outlierControl->absDevLimit) {
            keep[irow] = 0;
            kept--;
            killed++;
          }
        if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
          fprintf(stderr, "%ld additional rows killed by column %s absolute deviation outlier control\n",
                  killed, column[icol]);
      }
      if (outlierControl->flags&OUTLIER_STDEV_GIVEN &&
          kept && 
          meanStDevForFlaggedData(&mean, &stDev, data, keep, rows) &&
          stDev ) {
        /* limit deviation from mean in units of standard deviations */
        for (irow=killed=0; irow<rows; irow++)
          if (keep[irow] && fabs(data[irow]-mean)>outlierControl->stDevLimit*stDev) {
            keep[irow] = 0;
            kept--;
            killed++;
          }
        if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
          fprintf(stderr, "%ld additional rows killed by column %s standard deviation outlier control\n",
                  killed, column[icol]);
      }
      if (outlierControl->flags&OUTLIER_PERCENTILE_GIVEN)
        SDDS_Bomb("outlier control via percentile is not implemented yet");
      if (outlierControl->flags&OUTLIER_CHANCELIMIT_GIVEN) {
        /* limit improbability of a point occuring for a given total number
           of points, assuming gaussian statistics
           */
        if (kept && 
            meanStDevForFlaggedData(&mean, &stDev, data, keep, rows) &&
            stDev) {
          long lastKept;
          double gProb, probOfSeeing;
          lastKept = kept;
          for (irow=killed=0; irow<rows; irow++) {
            if (!keep[irow])
              continue;
            /* gProb = (probability of value >= x)  */
            /* (1-gProb)^n = probability of not seeing x in n trials */
            gProb = normSigLevel((data[irow]-mean)/stDev, 2);
            probOfSeeing = 1-ipow(1-gProb, lastKept);
            if (probOfSeeing<outlierControl->chanceLimit) {
              keep[irow] = 0;
              kept--;
              killed++;
            }
          }
          if (killed && outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
            fprintf(stderr, "%ld additional rows killed by column %s chance limit outlier control\n",
                    killed, column[icol]);
        }
      }
    }
    if (outlierControl->flags&OUTLIER_REPLACEFLAGS && outlierControl->flags&OUTLIER_INVERT_GIVEN) {
      for (irow=0; irow<rows; irow++)
        keep[irow] = !keep[irow];
      kept = rows-kept;
    }
    if (outlierControl->flags&OUTLIER_REPLACELAST) {
      for (irow=0; irow<rows; irow++) {
        if (keep[irow]) {
          lastGoodValue = data[irow];
          break;
        }
      }
      for (irow=0; irow<rows; irow++) {
        if (!keep[irow]) {
          keep[irow] = 1;
          data[irow] = lastGoodValue;
        }
        else
          lastGoodValue = data[irow];
      }
      kept = rows;
      if (!SDDS_SetColumnFromDoubles(dataset, SDDS_SET_BY_NAME, data, rows, column[icol]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    else if (outlierControl->flags&OUTLIER_REPLACENEXT) {
      for (irow=rows-1; irow>=0; irow--) {
        if (keep[irow]) {
          lastGoodValue = data[irow];
          break;
        }
      }
      for (irow=rows-1; irow>=0; irow--) {
        if (!keep[irow]) {
          data[irow] = lastGoodValue;
          keep[irow] = 1;
        }
        else
          lastGoodValue = data[irow];
      }
      kept = rows;
      if (!SDDS_SetColumnFromDoubles(dataset, SDDS_SET_BY_NAME, data, rows, column[icol]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    else if (outlierControl->flags&OUTLIER_REPLACEINTERP) {
      irow0 = -1;
      irow1 = -1;
      for (irow=0; irow<rows; irow++) {
        if (!keep[irow]) {
          if ((irow0 = irow-1)>=0) {
            if ((irow1 = irow+1)<rows) {
              while (irow1<rows && !keep[irow1])
                irow1++;
            }
            if (irow1<rows && keep[irow1]) {
              /* irow is bracketed by irow0 and irow1, both of which
               * have good data 
               */
              if (!keep[irow0])
                SDDS_Bomb("bracketing problem");
              for ( ; irow<irow1; irow++)
                data[irow] = data[irow0]+(data[irow1]-data[irow0])/(1.0*irow1-irow0)*(irow-irow0);
              continue;
            }
            else {
              /* ran off the end with bad points---just duplicate the last good point */
              for ( ; irow<rows; irow++)
                data[irow] = data[irow0];
              continue;
            }  
          }
          else {
            /* no good point preceeds this point---look for a good point following it */
            for (irow1=irow+1; irow1<rows; irow1++) {
              if (keep[irow1])
                break;
            }
            if (irow1!=rows) {
              for (; irow<irow1; irow++)
                data[irow] = data[irow1];
              continue;
            }
          }
        }
      }
      for (irow=0; irow<rows; irow++)
        keep[irow] = 1;
      kept = rows;
      if (!SDDS_SetColumnFromDoubles(dataset, SDDS_SET_BY_NAME, data, rows, column[icol]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    else if (outlierControl->flags&OUTLIER_REPLACEVALUE) {
      for (irow=0; irow<rows; irow++) {
        if (!keep[irow]) {
          data[irow] = outlierControl->replacementValue;
          keep[irow] = 1;
          
        }
      }
      kept = rows;
      if (!SDDS_SetColumnFromDoubles(dataset, SDDS_SET_BY_NAME, data, rows, column[icol]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    free(data);
  }
  if (outlierControl->flags&OUTLIER_INVERT_GIVEN) {
    for (irow=0; irow<rows; irow++)
      keep[irow] = !keep[irow];
    kept = rows-kept;
    if (outlierControl->flags&OUTLIER_VERBOSE_GIVEN)
      fprintf(stderr, "%ld rows left after inversion\n", kept);
  }
  if (isOutlier && outlierControl->flags&OUTLIER_MARKONLY) {
    for (irow=0; irow<rows; irow++)
      isOutlier[irow] = !keep[irow];
    if (!SDDS_SetRowFlags(dataset, 1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return rows;
  }

  if (!SDDS_AssertRowFlags(dataset, SDDS_FLAG_ARRAY, keep, rows))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  return kept;
}

long meanStDevForFlaggedData(double *mean, double *stDev,
                             double *data, int32_t *keep, long rows)
{
  long irow, summed;
  double sum1, sum2, value;

  *mean = *stDev = 0;
  for (irow=sum1=summed=0; irow<rows; irow++) {
    if (!keep[irow])
      continue;
    sum1 += data[irow];
    summed++;
  }
  if (summed<2)
    return 0;
  *mean = sum1/summed;
  for (irow=sum2=0; irow<rows; irow++) {
    if (keep[irow]) {
      value = data[irow] - *mean;
      sum2 += value*value;
    }
  }
  *stDev = sqrt(sum2/(summed-1));
  return 1;
}

