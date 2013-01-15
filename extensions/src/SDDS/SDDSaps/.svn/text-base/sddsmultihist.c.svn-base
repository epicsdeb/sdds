/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsmultihist
 * purpose: SDDS-format multi-column histogramming program
 *
 * Michael Borland, 1996
 $Log: not supported by cvs2svn $
 Revision 1.26  2009/03/03 19:18:56  borland
 Fixed problem with -autobins argument parsing.

 Revision 1.25  2009/03/03 17:39:28  borland
 Added -autobins option.

 Revision 1.24  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.23  2006/08/28 21:12:49  borland
 Fixed bug in setting abscissa range when -abscissa option was given with -sizeOfBins option.

 Revision 1.22  2005/11/04 22:46:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.21  2003/12/03 05:55:02  borland
 Previously, if the -sizeOfBins option was used with -separate, then the
 histogram range was not guaranteed to cover all the data for all the columns.
 Fixed this.

 Revision 1.20  2003/09/02 19:16:05  soliday
 Cleaned up code for Linux.

 Revision 1.19  2002/11/21 16:57:07  borland
 Fixed bug in -expand option.

 Revision 1.18  2002/10/05 19:25:25  borland
 Fixed problems that occurred when the first page was empty.

 Revision 1.17  2002/08/14 17:12:49  soliday
 Added Open License

 Revision 1.16  2002/02/10 17:25:16  borland
 No longer deletes abscissa column symbol and description information
 unnecessarily.

 Revision 1.15  2002/02/10 16:45:46  borland
 Now passes through parameter data from the input file.

 Revision 1.14  2001/07/18 18:54:02  borland
 Output of CDF added by H. Shang.

 Revision 1.13  2001/01/10 19:35:41  soliday
 Standardized usage message.

 Revision 1.12  2000/11/21 17:17:36  soliday
 Added missing SDDS_Terminate procedures.

 Revision 1.11  2000/06/16 19:31:48  borland
 Fixed code that sets histogram limits for data when the variation is very small.

 Revision 1.10  2000/04/13 18:03:27  soliday
 Removed invalid prototype.

 Revision 1.9  2000/04/13 17:09:47  soliday
 Added missing include statment.

 Revision 1.8  2000/01/25 14:31:53  borland
 Added qualifiers (close, against) to -sides and new -expand option.

 Revision 1.7  1999/11/20 15:51:47  borland
 Fixed bug in determination of limits for separate abscissa mode.

 Revision 1.6  1999/11/19 21:37:45  borland
 New version by Blachowicz that allows multiple abscissae.

 Revision 1.5  1999/05/25 19:12:18  soliday
 Removed compiler warning on linux.

 Revision 1.4  1999/01/06 19:54:48  borland
 Fixed the version number in the usage message.

 * Revision 1.3  1996/06/05  22:45:15  borland
 * Added ChangeColumnInformation calls to get rid of description and symbol
 * for abscissa; basically only take the units and format string from source
 * column now.
 *
 * Revision 1.2  1996/06/05  21:45:56  emery
 * Added a ChangeColumnInformation to force the abscissa column
 * to be of type double.
 *
 * Revision 1.1  1996/05/09  15:02:31  borland
 * First version.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <ctype.h>

#define SET_COLUMNS 0
#define SET_PIPE 1
#define SET_EXCLUDE 2
#define SET_ABSCISSA 3
#define SET_BINS 4
#define SET_SIZEOFBINS 5
#define SET_LOWERLIMIT 6
#define SET_UPPERLIMIT 7
#define SET_SIDES 8
#define SET_SEPARATE 9
#define SET_EXPAND 10
#define SET_CDF 11
#define SET_AUTOBINS 12
#define N_OPTIONS 13

char *option[N_OPTIONS] = {
  "columns", "pipe", "exclude", "abscissa", "bins", "sizeofbins",
  "lowerlimit", "upperlimit", "sides", "separate", "expand","cdf",
  "autobins",
  };

static char *USAGE="sddsmultihist [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
-columns=<name>[,...] -abscissa=<name>[,...] [-exclude=<name>[,...]]\n\
[{-bins=<integer> | -sizeOfBins=<value> | -autobins=target=<number>[,minimum=<integer>][,maximum=<integer>]}] [-sides[=close]] [-expand=<fraction>] [-separate]\n\
[-lowerLimit=<value>[,...]] [-upperLimit=<value>[,...]] [-cdf=[only]]\n\n\
pipe           The standard SDDS Toolkit pipe option.\n\
columns        Specifies the names of columns from the input to be histogrammed.\n\
               Names may contain wildcards.\n\
abscissa       Specifies the names of the abscissas in the output file.\n\
               When user wants to use column names as abscissa names,\n\
               the abscissa option is not required (the user has to use the option\n\
               -separate in this case). The user has to supply at least one abscissa\n\
               name when the option -separate is not used.\n\
bins           Number of bins for histogram.\n\
autobins       Requests that the number of bins is chosen automatically to give the target\n\
               number of samples per bin on average.  Optionally specify the minimum and\n\
               maximum number of bins to be used.\n\
sizeOfBins     Size of bins for histogram.\n\
lowerLimit     Lower limits of histograms.\n\
upperLimit     Upper limits of histograms.\n\
sides          Put zero-height bins at the ends of the histogram.\n\
expand         Expand range of histogram by a given fraction.\n\
cdf            If -cdf is given, CDF is included in the file in addition to the histogram.\n\
               If -cdf=only is given, the histogram is not included. \n\
separate       Creates separate abscissa for each histogram in the output file.\n\
Program by Michael Borland.  (This is version 6, March 2009, M. Borland.)\n";

#define DO_SIDES 1
#define DO_CLOSE_SIDES 2
#define DO_AGAINST_SIDES 3

void SetUpOutput(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, long **abscissaIndex,long **cdfIndex,
                 long **histogramIndex, char *output, 
                 char **columnName, long columnNames,
                 char **abscissaName, long abscissaNames);
static short cdfOnly,freOnly;
int main(int argc, char **argv)
{
  int iArg;
  char **abscissaName, **columnName, **excludeName;
  long columnNames, excludeNames, abscissaNames, column, i, writeBins, offset;
  long givenLowerLimits, givenUpperLimits;
  char *input, *output;
  long rows, readCode, bins, binsGiven;
  long lowerLimitGiven, upperLimitGiven, doSides, doSeparate;
  double autoBinsTarget;
  long autoBinsMinimum, autoBinsMaximum;
  unsigned long flags, pipeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double binSize, *lowerLimit=NULL, *upperLimit=NULL, *givenLowerLimit, *givenUpperLimit, *dx=NULL, range, middle;
  double **inputData, *abscissa, *histogram, *minValue, *maxValue, transferLimit,*cdf,sum;
  long *abscissaIndex, *histogramIndex, *cdfIndex;
  double expandRange, maxRange;
  short setCdf=0;
  char *CDFONLY;
  unsigned long dummyFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3 || argc>(3+N_OPTIONS)) 
    bomb(NULL, USAGE);
  
  minValue = maxValue = NULL;
  output = input = NULL;
  flags = pipeFlags = 0;
  abscissaName = NULL;
  columnName = excludeName = NULL;
  columnNames = excludeNames = abscissaNames = 0;
  givenLowerLimits = givenUpperLimits = 0;
  givenLowerLimit = givenUpperLimit = NULL;
  bins = binsGiven = binSize = doSides = doSeparate = 0;
  lowerLimitGiven = upperLimitGiven = 0;
  expandRange = 0;
  cdfOnly=0;
  freOnly=1;
  autoBinsTarget = 0;
  autoBinsMinimum = autoBinsMaximum = 0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_COLUMNS:
        if (columnName)
          SDDS_Bomb("only one -columns option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        if (!(columnName = 
              SDDS_Realloc(columnName, 
                           sizeof(*columnName)*(columnNames+scanned[iArg].n_items-1))))
          SDDS_Bomb("memory allocation failure");
        for (i=1; i<scanned[iArg].n_items; i++) 
          columnName[columnNames+i-1] = scanned[iArg].list[i];
        columnNames += scanned[iArg].n_items-1;
        break;
      case SET_ABSCISSA:
        if (abscissaName)
          SDDS_Bomb("only one -abscissa option may be given");
        if (scanned[iArg].n_items>=2) {
	  if (!(abscissaName = 
		SDDS_Realloc(abscissaName, 
                             sizeof(*abscissaName)*(abscissaNames+scanned[iArg].n_items-1))))
	    SDDS_Bomb("memory allocation failure");
	  for (i=1; i<scanned[iArg].n_items; i++) 
            abscissaName[abscissaNames+i-1] = scanned[iArg].list[i];
	  abscissaNames += scanned[iArg].n_items-1;
        }
        break;
      case SET_BINS:
        if (binsGiven) 
          SDDS_Bomb("-bins specified more than once");
        binsGiven = 1;
        if (sscanf(scanned[iArg].list[1], "%ld", &bins)!=1 || bins<=0)
          SDDS_Bomb("invalid value for bins---give a positive value");
        break;
      case SET_SIZEOFBINS:
        if (sscanf(scanned[iArg].list[1], "%le", &binSize)!=1 || binSize<=0)
          SDDS_Bomb("invalid value for bin size---give a positive value");
        break;
      case SET_AUTOBINS:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("incorrect -autoBins sytnax");
        scanned[iArg].n_items -= 1;
        autoBinsTarget = autoBinsMinimum = autoBinsMaximum = 0;
        if (!scanItemList(&dummyFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "target", SDDS_DOUBLE, &autoBinsTarget, 1, 0,
                          "minimum", SDDS_LONG, &autoBinsMinimum, 1, 0,
                          "maximum", SDDS_LONG, &autoBinsMaximum, 1, 0,
                          NULL)
            || autoBinsTarget<=0 || autoBinsMinimum<0 || autoBinsMaximum<0)
          SDDS_Bomb("incorrect -autoBins sytnax or values");
        break;
      case SET_EXCLUDE:
        if (excludeName)
          SDDS_Bomb("only one -excludes option may be given");
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -excludes syntax");
        if (!(excludeName = 
              SDDS_Realloc(excludeName, 
                           sizeof(*excludeName)*(excludeNames+scanned[iArg].n_items-1))))
          SDDS_Bomb("memory allocation failure");
        for (i=1; i<scanned[iArg].n_items; i++)
          excludeName[columnNames+i-1] = scanned[iArg].list[i];
        excludeNames += scanned[iArg].n_items-1;
        break;
      case SET_LOWERLIMIT:
        if (lowerLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        lowerLimitGiven = 1;
        if (!(givenLowerLimit = 
              SDDS_Realloc(givenLowerLimit, 
                           sizeof(*givenLowerLimit)*(givenLowerLimits+scanned[iArg].n_items-1))))
          SDDS_Bomb("SET_LOWERLIMIT: memory allocation failure");
        for (i=1; i<scanned[iArg].n_items; i++) {
	  if (sscanf(scanned[iArg].list[i], "%lf", &transferLimit)!=1)
            SDDS_Bomb("invalid value for -lowerLimit"); 
          givenLowerLimit[givenLowerLimits+i-1] = transferLimit;
        }
        givenLowerLimits += scanned[iArg].n_items-1;
        break;
      case SET_UPPERLIMIT:
        if (upperLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        upperLimitGiven = 1;
        if (!(givenUpperLimit = 
              SDDS_Realloc(givenUpperLimit, 
                           sizeof(*givenUpperLimit)*(givenUpperLimits+scanned[iArg].n_items-1))))
          SDDS_Bomb("SET_UPPERLIMIT: memory allocation failure");
        for (i=1; i<scanned[iArg].n_items; i++) {
          if (sscanf(scanned[iArg].list[i], "%lf", &transferLimit)!=1)
            SDDS_Bomb("invalid value for -upperLimit"); 
          givenUpperLimit[givenUpperLimits+i-1] = transferLimit;
        }
        givenUpperLimits += scanned[iArg].n_items-1; 
        break;
      case SET_SIDES:
        doSides = DO_SIDES;
        if (scanned[iArg].n_items==2) {
          static char *sideOpt[2] = {"close", "against"};
          switch (match_string(scanned[iArg].list[1], sideOpt, 2, 0)) {
          case 0:
            doSides = DO_CLOSE_SIDES;
            break;
          case 1:
            doSides = DO_AGAINST_SIDES;
            break;
          default:
            SDDS_Bomb("invalid value for -sides");
            break;
          }
        }
        break;
      case SET_SEPARATE:
        doSeparate = 1;
        break;
      case SET_EXPAND:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%lf", &expandRange)!=1 ||
            expandRange<=0)
          SDDS_Bomb("invalid -expand syntax");
        break;
      case SET_CDF:
        setCdf=1;
        if (scanned[iArg].n_items==1)
          cdfOnly=0;
        else {
          if (scanned[iArg].n_items >2)
            SDDS_Bomb("invalid -cdf syntax");
          CDFONLY = scanned[iArg].list[1];
          if (strcmp(CDFONLY,"only")!=0)
            SDDS_Bomb("invalid -cdf value, it should be -cdf or -cdf=only");
          cdfOnly=1;
        } 
        freOnly=0;
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
  
  if (columnNames<=0)
    SDDS_Bomb("supply the names of columns to histogram with -columns");
  
  processFilenames("sddsmultihist", &input, &output, pipeFlags, 0, NULL);
  
  if (!SDDS_InitializeInput(&SDDSin, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if ((columnNames=expandColumnPairNames(&SDDSin, &columnName, NULL, columnNames,
                                         excludeName, excludeNames, FIND_NUMERIC_TYPE, 0))<=0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    SDDS_Bomb("No quantities selected to histogram.");
  }
  
  if (doSeparate) {
    if (abscissaNames<=0) {
      if (!(abscissaName = 
              SDDS_Realloc(abscissaName, 
                           sizeof(*abscissaName)*(abscissaNames+columnNames))))
        SDDS_Bomb("memory allocation failure");
      abscissaNames=0;
      for (i=1; i<=columnNames; i++) {
        abscissaName[abscissaNames+i-1] = columnName[abscissaNames+i-1];
      }
      abscissaNames += columnNames; 
    }
    if (columnNames>1) {
      if (abscissaNames>0) {
        if (columnNames!=abscissaNames)
          SDDS_Bomb("the number of the abscissa names must match the number of columns");
	}
      if (givenLowerLimits)
        if (columnNames!=givenLowerLimits)
          SDDS_Bomb("the number of the lower limits must match the number of columns");
	if (givenUpperLimits)
          if (columnNames!=givenUpperLimits)
            SDDS_Bomb("the number of the upper limits must match the number of columns");
      } else {
        /* This is employed when there is just one column name specified
          and many names of other options. To avoid error display the application
          is going to assign first value from each option as the valid one. */
        if (abscissaNames>0) {
            if (columnNames!=abscissaNames)
              abscissaNames = 1;
        }
        if (givenLowerLimits)
            if (columnNames!=givenLowerLimits)
              givenLowerLimits = 1;
        if (givenUpperLimits)
          if (columnNames!=givenUpperLimits)
            givenUpperLimits =1;
      }
  } else {
    if (abscissaNames<=0)
          SDDS_Bomb("supply the name of the abscissa with -abscissaName");
    abscissaNames = 1;
  }
    
  SetUpOutput(&SDDSout, &SDDSin, &abscissaIndex,&cdfIndex, &histogramIndex,
              output, columnName, columnNames, abscissaName, abscissaNames);
  
  if (!(inputData = (double**)malloc(columnNames*sizeof(*inputData))) || 
      !(minValue = (double*)malloc(columnNames*sizeof(*minValue))) ||
      !(maxValue = (double*)malloc(columnNames*sizeof(*maxValue))) )
    SDDS_Bomb("memory allocation failure");
  
  if (((binSize?1:0)+(binsGiven?1:0)+(autoBinsTarget?1:0))>1) 
    SDDS_Bomb("give only one of -binSize, -bins, or -autoBins");
  if (!binSize && !binsGiven && !autoBinsTarget) {
    binsGiven = 1;
    bins = 20;
  } 
  
  abscissa = histogram =cdf= NULL;
  
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (rows) {
      for (column=0; column<columnNames; column++) {
        if (!(inputData[column]=SDDS_GetColumnInDoubles(&SDDSin, columnName[column])))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      
      if (!(lowerLimit = (double*)malloc(sizeof(*lowerLimit)*columnNames)))
        SDDS_Bomb("memory allocation failure");
      if (!(upperLimit = (double*)malloc(sizeof(*lowerLimit)*columnNames)))
        SDDS_Bomb("memory allocation failure");
      if (!(dx = (double*)malloc(sizeof(*dx)*columnNames)))
        SDDS_Bomb("memory allocation failure");
      
      if (doSeparate) {
        for (column=0; column<columnNames; column++)
        { 
           find_min_max(minValue+column, maxValue+column, inputData[column], rows);
           if (!givenLowerLimits)
                lowerLimit[column] = minValue[column];
           else
                lowerLimit[column] = givenLowerLimit[column];
           if (!givenUpperLimits)
             upperLimit[column] = maxValue[column];
           else
             upperLimit[column] = givenUpperLimit[column];
        }
      }
      else {
        for (column=0; column<columnNames; column++) 
          find_min_max(minValue+column, maxValue+column, inputData[column], rows);
        if (!givenLowerLimits)
          lowerLimit[0] = min_in_array(minValue, columnNames);
        else
          lowerLimit[0] = givenLowerLimit[0];
        if (!givenUpperLimits)
          upperLimit[0] = max_in_array(maxValue, columnNames);
        else
          upperLimit[0] = givenUpperLimit[0];
        for (column=1; column<columnNames; column++) {
          lowerLimit[column] = lowerLimit[0];
          upperLimit[column] = upperLimit[0];
        }
      }
      
#ifdef DEBUG
      fprintf(stderr, "lower, upper: %le, %le\n", lowerLimit, upperLimit);
#endif
      range = (1+expandRange)*(upperLimit[0] - lowerLimit[0]);
      if (autoBinsTarget) {
        bins = (long)(rows/autoBinsTarget);
        if (autoBinsMinimum) {
          if (bins<autoBinsMinimum)
            bins = autoBinsMinimum;
        } else if (bins<5) {
          bins = 5;
        }
        if (autoBinsMaximum) {
          if (bins>autoBinsMaximum)
            bins = autoBinsMaximum;
        } else if (bins>rows)
          bins = rows;
      }

      if (binSize) {
        maxRange = range;
        for (column=0; column<columnNames; column++)
        { 
          range = (1+expandRange)*(upperLimit[column] - lowerLimit[column]);
          range = ((range/binSize)+1)*binSize;
          if (range>maxRange)
            maxRange = range;
          middle = (lowerLimit[column]+upperLimit[column])/2;
          lowerLimit[column] = middle-range/2;
          upperLimit[column] = middle+range/2;
        }
        range = maxRange;
        if (!doSeparate) {
          /* lower and upper ranges are identical and equal to min/max of ranges */
          lowerLimit[0] = min_in_array(lowerLimit, columnNames);
          upperLimit[0] = max_in_array(upperLimit, columnNames);
          dx[0] = binSize;
        }    
        bins = maxRange/binSize+0.5;
        if (bins<1 && !doSides)
          bins = 2;
        if (doSeparate)
          for (column=0; column<columnNames; column++) {
            range = upperLimit[column]-lowerLimit[column];
            upperLimit[column] += (maxRange-range)/2;
            lowerLimit[column] -= (maxRange-range)/2;
            dx[column] = binSize;
          }
      } else {
        if (doSeparate) {
          for (column=0; column<columnNames; column++)
          { 
            range = (1+expandRange)*(upperLimit[column]-lowerLimit[column]);
	    middle = (upperLimit[column]+lowerLimit[column])/2;
	    upperLimit[column] = middle+range/2;
	    lowerLimit[column] = middle-range/2;
            if (upperLimit[column]==lowerLimit[column]) {
              if (fabs(upperLimit[column])<sqrt(DBL_MIN)) {
                upperLimit[column] = sqrt(DBL_MIN);
                lowerLimit[column] = -sqrt(DBL_MIN);
              }
              else {
                lowerLimit[column] = upperLimit[column]*(1-10000*DBL_EPSILON);
                upperLimit[column] = upperLimit[column]*(1+10000*DBL_EPSILON);
              }
            }
            dx[column] = (upperLimit[column]-lowerLimit[column])/bins;
          }
        } else {
	  range = (1+expandRange)*(upperLimit[0]-lowerLimit[0]);
	  middle = (upperLimit[0]+lowerLimit[0])/2;
	  upperLimit[0] = middle+range/2;
	  lowerLimit[0] = middle-range/2;
	  if (upperLimit[0]==lowerLimit[0]) {
	    if (fabs(upperLimit[0])<sqrt(DBL_MIN)) {
	      upperLimit[0] = sqrt(DBL_MIN);
	      lowerLimit[0] = -sqrt(DBL_MIN);
	    }
	    else {
	      lowerLimit[0] = upperLimit[0]*(1-10000*DBL_EPSILON);
	      upperLimit[0] = upperLimit[0]*(1+10000*DBL_EPSILON);
	    }
	  }
	  dx[0] = (upperLimit[0]-lowerLimit[0])/bins;
	}
      }
      if (!binsGiven || !abscissa) {
#ifdef DEBUG
        fprintf(stderr, "Allocating memory for %ld bins\n", bins+2);
#endif
        if (!(abscissa = SDDS_Realloc(abscissa, sizeof(*abscissa)*(bins+2))) ||
            !(cdf = SDDS_Realloc(cdf, sizeof(*cdf)*(bins+2))) ||
            !(histogram = SDDS_Realloc(histogram, sizeof(*histogram)*(bins+2))))
          SDDS_Bomb("memory allocation failure");
      }
      writeBins = bins + (doSides?2:0);
      offset = doSides?0:1;
      if (!SDDS_StartPage(&SDDSout, writeBins) ||
          !SDDS_CopyParameters(&SDDSout, &SDDSin))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      
      if (!doSeparate) {
        for (i=-1; i<bins+1; i++)
          abscissa[i+1] = (i+0.5)*dx[0] + lowerLimit[0];
        switch (doSides) {
        case DO_CLOSE_SIDES:
          abscissa[0] = abscissa[1]-dx[0]/2;
          abscissa[bins+1] = abscissa[bins]+dx[0]/2;
          break;
        case DO_AGAINST_SIDES:
          abscissa[0] = abscissa[1];
          abscissa[bins+1] = abscissa[bins];
          break;
        }
        if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, abscissa+offset,
                            writeBins, abscissaIndex[0]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (column=0; column<columnNames; column++) {
          histogram[0] = histogram[bins+1] = 0;
          /* cdf[i]=cdf[bins+1]=0; */
          make_histogram(histogram+1,  bins, lowerLimit[0], upperLimit[0], inputData[column],
                         rows, 1);
          sum=0;
          for (i=0;i<=bins+1;i++)
            sum +=histogram[i];
          for (i=0;i<=bins+1;i++) {
            if (!i)
              cdf[i]=histogram[i]/sum;
            else
              cdf[i]=cdf[i-1]+histogram[i]/sum;
          }
          if (!cdfOnly) {
            if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, histogram+offset,
                                writeBins, histogramIndex[column]))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (!freOnly) {
            if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, cdf+offset,
                                writeBins, cdfIndex[column]))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          free(inputData[column]);
        }
      } else {
        for (column=0; column<columnNames; column++) {
          for (i=-1; i<bins+1; i++)
            abscissa[i+1] = (i+0.5)*dx[column] + lowerLimit[column];
          switch (doSides) {
          case DO_CLOSE_SIDES:
            abscissa[0] = abscissa[1]-dx[column]/2;
            abscissa[bins+1] = abscissa[bins]+dx[column]/2;
            break;
          case DO_AGAINST_SIDES:
            abscissa[0] = abscissa[1];
            abscissa[bins+1] = abscissa[bins];
            break;
          }
          if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, abscissa+offset,
                              writeBins, abscissaIndex[column]))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
#ifdef DEBUG
          fprintf(stderr, "histogramming column %s\n", columnName[column]);
#endif
          histogram[0] = histogram[bins+1] = 0;
          /* cdf[0]=cdf[bins+1]=0; */
          make_histogram(histogram+1,  bins, lowerLimit[column], upperLimit[column], inputData[column],
                         rows, 1);
          sum=0;
          for (i=0;i<=bins+1;i++)
            sum +=histogram[i];
          for (i=0;i<=bins+1;i++) {
            if (!i)
              cdf[i]=histogram[i]/sum;
            else
              cdf[i]=cdf[i-1]+histogram[i]/sum;
          }
          if (!cdfOnly) {
            if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, histogram+offset,
                                writeBins, histogramIndex[column]))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (!freOnly) {
            if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, cdf+offset,
                                writeBins, cdfIndex[column]))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          free(inputData[column]);
        }
      }
    } else {
#ifdef DEBUG
      fprintf(stderr, "empty page seen\n");
#endif
      if (!SDDS_StartPage(&SDDSout, rows?bins:0))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
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

#ifdef DEBUG
  fprintf(stderr, "exiting\n");
#endif
  free(histogram);
  free(cdf);
  free(abscissa);
  if (minValue)
    free(minValue);
  if (maxValue)
    free(maxValue);
  if (lowerLimit)
    free(lowerLimit);
  if (upperLimit)
    free(upperLimit);
  if (dx)
    free(dx);
  
  return 0;
}


void SetUpOutput(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, long **abscissaIndex,long **cdfIndex,
                 long **histogramIndex, char *output, 
                 char **columnName, long columnNames,
                 char **abscissaName, long abscissaNames)
{
  long column;
  char s[SDDS_MAXLINE];
  int32_t outputType = SDDS_DOUBLE;
  char *blankString = "";

  if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddsmultihist output", output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(*cdfIndex = (long*)malloc(sizeof(**cdfIndex)*columnNames)))
    SDDS_Bomb("memory allocation failure");
  if (!cdfOnly) {
    if (!(*histogramIndex = (long*)malloc(sizeof(**histogramIndex)*columnNames)))
      SDDS_Bomb("memory allocation failure");
  }
  if (!(*abscissaIndex = (long*)malloc(sizeof(**abscissaIndex)*columnNames)))
    SDDS_Bomb("memory allocation failure");
  
  for (column=0; column<abscissaNames; column++) {
    if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, columnName[column], abscissaName[column]) ||
	!SDDS_ChangeColumnInformation(SDDSout, "type", &outputType, SDDS_BY_NAME, abscissaName[column]) ||
        ((*abscissaIndex)[column]=SDDS_GetColumnIndex(SDDSout, abscissaName[column]))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (strcmp(columnName[column], abscissaName[column])!=0 &&
	(!SDDS_ChangeColumnInformation(SDDSout, "description", blankString, SDDS_BY_NAME, abscissaName[column]) ||
         !SDDS_ChangeColumnInformation(SDDSout, "symbol", blankString, SDDS_BY_NAME, abscissaName[column]) ))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  for (column=0; column<columnNames; column++) {
    if (!freOnly) {
      sprintf(s, "%sCdf", columnName[column]);
      if (((*cdfIndex)[column]=
           SDDS_DefineColumn(SDDSout, s, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!cdfOnly) {
      sprintf(s, "%sFrequency", columnName[column]);
      if (((*histogramIndex)[column]=
           SDDS_DefineColumn(SDDSout, s, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!SDDS_TransferAllParameterDefinitions(SDDSout, SDDSin, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(SDDSout)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}
