/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddshist
 * purpose: SDDS-format histogram command
 *  Based on the mpl program hist.
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.23  2005/11/14 17:58:49  soliday
 Fixed a bug where the lower and upper filter could be used when their
 values were uninitialized.

 Revision 1.22  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.21  2003/09/02 19:16:04  soliday
 Cleaned up code for Linux.

 Revision 1.20  2002/10/05 19:25:25  borland
 Fixed problems that occurred when the first page was empty.

 Revision 1.19  2002/08/14 21:01:49  soliday
 Fixed bug

 Revision 1.18  2002/08/14 17:12:47  soliday
 Added Open License

 Revision 1.17  2002/08/09 13:57:18  borland
 Added -expand option to allow increasing the range of the histogram by
 a factor.

 Revision 1.16  2001/11/30 16:12:06  borland
 Fixed UMR.

 Revision 1.15  2001/07/18 18:54:02  borland
 Output of CDF added by H. Shang.

 Revision 1.14  2001/01/10 19:35:39  soliday
 Standardized usage message.

 Revision 1.13  2000/08/08 20:31:21  borland
 The -sides option now accepts a number giving how many bins to add at each
 end.

 Revision 1.12  1999/05/25 19:10:41  soliday
 Removed compiler warning on linux.

 Revision 1.11  1999/01/06 19:54:45  borland
 Fixed the version number in the usage message.

 Revision 1.10  1998/12/16 21:26:03  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.9  1998/11/13 22:39:53  borland
 Now copies through parameters from the input to the output.

 Revision 1.8  1997/12/19 17:00:51  borland
 Now uses scanargsg instead of scanargs.

 Revision 1.7  1997/08/25 19:23:38  borland
 Apparently just reformated the code.

 Revision 1.6  1997/02/05 15:58:32  borland
 Changed some function prototypes to static to keep the names to myself.

 * Revision 1.5  1995/11/22  00:39:32  borland
 * Fixed bug having to do with transfer of a column and changing of its
 * type without changing the format_string field to match.
 *
 * Revision 1.4  1995/11/20  16:24:00  borland
 * Added code to prevent aborts when there are no rows in the input page.
 *
 * Revision 1.3  1995/09/06  14:56:36  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_BINS 0
#define SET_LOWERLIMIT 1
#define SET_UPPERLIMIT 2
#define SET_DATACOLUMN 3
#define SET_FILTER 4
#define SET_BINSIZE 5
#define SET_WEIGHTCOLUMN 6
#define SET_NORMALIZE 7
#define SET_STATISTICS 8
#define SET_SIDES 9
#define SET_VERBOSE 10
#define SET_PIPE 11
#define SET_CDF 12
#define SET_EXPAND 13
#define N_OPTIONS 14

char *option[N_OPTIONS] = {
    "bins", "lowerlimit", "upperlimit", "datacolumn",
    "filter", "sizeofbins", "weightcolumn",
    "normalize", "statistics", "sides", "verbose", "pipe","cdf",
    "expand",
    };

char *USAGE="sddshist [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
-dataColumn=<column-name> [{-bins=<number> | -sizeOfBins=<value>}] \n\
[-lowerLimit=<value>] [-upperLimit=<value>] [-expand=<factor>]\n\
[-filter=<column-name>,<lower-limit>,<upper-limit>] \n\
[-weightColumn=<column-name>] [-sides[=<points>]] \n\
[-normalize[={sum|area|peak}]] \n\
[-cdf[=only]] \n\
[-statistics] [-verbose]\n";

static char *additional_help = "\n\
bins           number of bins for histogram.\n\
sizeOfBins     size of bins for histogram.\n\
lowerLimit     lower limit of histogram.\n\
upperLimit     upper limit of histogram.\n\
expand         expand the range of the histogram by the given factor.\n\
dataColumn     give name of column to histogram.\n\
filter         histogram only those points for which the named data lies between the given values.\n\
weightColumn   weight the histogram with the named data.\n\
normalize      normalize the histogram, as specified.\n\
statistics     put statistical information in output file.\n\
sides          puts sides on the histogram, down to zero level.\n\
cdf            If -cdf is given, CDF is included in the file in addition to the histogram.\n\
               If -cdf=only is given, the histogram is not included. \n\
verbose        activates informational printouts during processing.\n\n\
Program by Michael Borland.  (This is version 5, August 2002, M. Borland)\n";

#define NORMALIZE_PEAK 0
#define NORMALIZE_AREA 1
#define NORMALIZE_SUM 2
#define NORMALIZE_NO 3
#define N_NORMALIZE_OPTIONS 4
char *normalize_option[N_NORMALIZE_OPTIONS] = {
    "peak", "area", "sum", "no"
    } ;

static long filter(double *x, double *y, double *filterData, long npts, double lower_filter, double upper_filter);
static long setupOutputFile(SDDS_DATASET *outTable, char *outputfile, SDDS_DATASET *inTable, char *inputfile,
                     char *dataColumn,char *weightColumn, char *filterColumn,
                     double lowerFilter, double upperFilter, long doStats, long bins, 
                     double binSize, long normalizeMode);

/* column and parameter indices for output file */
static long iIndep, iFreq, iBins, iBinSize, iLoFilter, iUpFilter, iMean, iRMS, iStDev, iPoints,iCdf;
static short cdfOnly,freOnly;
int main(int argc, char **argv)
{
  /* next line defines flags to keep track of what is set in command line: */
  long binsGiven, lowerLimitGiven, upperLimitGiven;
  SDDS_DATASET inTable, outTable;
  double *data;            /* pointer to the array to histogram */
  double *filterData;      /* pointer to the filter data */
  double *weightData;      /* pointer to the weight data */
  double *hist, *hist1;    /* to store the histogram */
  double *CDF, *CDF1;   /* to store the CDF which is defined as
                        CDF(i)=Sum(hist(j), all j such that j<=i)/Sum(hist(j),all j) */
  double sum; /* store the total of hist */
  double *indep;           /* to store values of bin centers */
  double lowerLimit, upperLimit; /* lower and upper limits in histogram */
  double givenLowerLimit, givenUpperLimit; /* lower and upper limits in histogram */
  double range, binSize;
  long bins;                /* number of bins in the histogram */
  long doStats;             /* if true, include statistics in output file */
  double mean, rms, standDev, mad;
  char *filterColumn, *dataColumn, *weightColumn;
  double lowerFilter=0, upperFilter=0;  /* filter range */
  long points;            /* number of data points after filtering */
  SCANNED_ARG *scanned;    /* scanned argument structure */
  char *inputfile, *outputfile; /* array of input files, output file */
  double dx;               /* spacing of bins in histogram */
  long i;                   /* guess what */
  long pointsBinned;        /* number of points that are in histogram */
  long normalizeMode, doSides, verbose, rows, readCode;
  unsigned long pipeFlags;
  char *cdf;
  double expansionFactor = 0;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3) {
    fprintf(stderr, "usage: %s\n", USAGE);
    fputs(additional_help, stderr);
    exit(1);
  }
  
  binsGiven = lowerLimitGiven = upperLimitGiven = 0;
  binSize = doSides = 0;
  inputfile = outputfile = NULL;
  dataColumn = filterColumn = weightColumn = NULL;
  doStats = verbose = 0;
  normalizeMode = NORMALIZE_NO;
  pipeFlags = 0;
  dx = 0;
  cdfOnly=0;
  freOnly=1;
  
  for (i=1; i<argc; i++) {
    if (scanned[i].arg_type==OPTION) {
      switch (match_string(scanned[i].list[0], option, N_OPTIONS, 0)) {
      case SET_BINS:         /* set number of bins */
        if (binsGiven) 
          SDDS_Bomb("-bins specified more than once");
        binsGiven = 1;
        if (sscanf(scanned[i].list[1], "%ld", &bins)!=1 || bins<=0)
          SDDS_Bomb("invalid value for bins");
        break;
      case SET_LOWERLIMIT:
        if (lowerLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        lowerLimitGiven = 1;
        if (sscanf(scanned[i].list[1], "%lf", &givenLowerLimit)!=1)
          SDDS_Bomb("invalid value for lowerLimit");
        break;
      case SET_UPPERLIMIT:
        if (upperLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        upperLimitGiven = 1;
        if (sscanf(scanned[i].list[1], "%lf", &givenUpperLimit)!=1)
          SDDS_Bomb("invalid value for upperLimit");
        break;
      case SET_EXPAND:
        expansionFactor = 0;
        if (sscanf(scanned[i].list[1], "%lf", &expansionFactor)!=1 && expansionFactor<=0)
          SDDS_Bomb("invalid value for expand");
        break;
      case SET_DATACOLUMN:
        if (dataColumn)
          SDDS_Bomb("-dataColumn specified more than once");
        if (scanned[i].n_items!=2)
          SDDS_Bomb("invalid -dataColumn syntax---supply name");
        dataColumn = scanned[i].list[1];
        break;
      case SET_FILTER:
        if (filterColumn)
          SDDS_Bomb("multiple filter specifications not allowed");
        if (scanned[i].n_items!=4 ||
            sscanf(scanned[i].list[2], "%lf", &lowerFilter)!=1 ||
            sscanf(scanned[i].list[3], "%lf", &upperFilter)!=1 ||
            lowerFilter>upperFilter)
          SDDS_Bomb("invalid -filter syntax/values");
        filterColumn = scanned[i].list[1];
        break;
      case SET_WEIGHTCOLUMN:
        if (weightColumn)
          SDDS_Bomb("multiple weighting columns not allowed");
        if (scanned[i].n_items!=2)
          SDDS_Bomb("-weightColumn requires a column name");
        weightColumn = scanned[i].list[1];
        break;
      case SET_NORMALIZE:
        if (scanned[i].n_items==1)
          normalizeMode = NORMALIZE_SUM;
        else if (scanned[i].n_items!=2 || 
                 (normalizeMode=match_string(scanned[i].list[1], normalize_option, N_NORMALIZE_OPTIONS, 0))<0)
          SDDS_Bomb("invalid -normalize syntax");
        break;
      case SET_STATISTICS:
        doStats = 1;
        break;
      case SET_SIDES:
        if (scanned[i].n_items==1) 
          doSides = 1;
        else if (scanned[i].n_items>2 ||
                 (sscanf(scanned[i].list[1], "%ld", &doSides)!=1 ||
                  doSides<=0))
          SDDS_Bomb("invalid -sides syntax");
        break;
      case SET_VERBOSE:
        verbose = 1;
        break;
      case SET_BINSIZE:
        if (sscanf(scanned[i].list[1], "%le", &binSize)!=1 || binSize<=0)
          SDDS_Bomb("invalid value for bin size");
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[i].list+1, scanned[i].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_CDF:
        if (scanned[i].n_items ==1)
          cdfOnly=0;
        else {
          if (scanned[i].n_items!=2)
            SDDS_Bomb("invalid -cdf syntax");
          cdf = scanned[i].list[1];
          if (strcmp(cdf,"only")!=0)
            SDDS_Bomb("invalid -cdf value, it should be -cdf or -cdf=only");
          cdfOnly=1;
        }
        freOnly=0;
        break;
      default:
        fprintf(stderr, "error: option %s not recognized\n", scanned[i].list[0]);
        exit(0);
        break;
      }
    }
    else {
      /* argument is filename */
      if (!inputfile)
        inputfile = scanned[i].list[0];
      else if (!outputfile)
        outputfile = scanned[i].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }
  
  processFilenames("sddshist", &inputfile, &outputfile, pipeFlags, 0, NULL);

  if (binSize && binsGiven)
    SDDS_Bomb("give either -binSize or -bins, not both");
  if (!binsGiven)
    bins = 20;
  if (!dataColumn)
    SDDS_Bomb("-dataColumn must be given");
  
  hist = tmalloc(sizeof(*hist)*(bins+2*doSides));
  CDF=tmalloc(sizeof(*hist)*(bins+2*doSides));
  indep = tmalloc(sizeof(*indep)*(bins+2*doSides));
  pointsBinned = 0;

  if (!SDDS_InitializeInput(&inTable, inputfile) ||
      SDDS_GetColumnIndex(&inTable, dataColumn)<0 ||
      (weightColumn && SDDS_GetColumnIndex(&inTable, weightColumn)<0) ||
      (filterColumn && SDDS_GetColumnIndex(&inTable, filterColumn)<0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!setupOutputFile(&outTable, outputfile, &inTable, inputfile, dataColumn,
                       weightColumn, 
                       filterColumn, lowerFilter, upperFilter, doStats, bins, binSize, normalizeMode))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  data = weightData = filterData = NULL;
  while ((readCode=SDDS_ReadPage(&inTable))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&inTable))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (rows && 
        (!(data = SDDS_GetColumnInDoubles(&inTable, dataColumn)) ||
         (weightColumn && !(weightData= SDDS_GetColumnInDoubles(&inTable, weightColumn))) ||
         (filterColumn && !(filterData= SDDS_GetColumnInDoubles(&inTable, filterColumn)))))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    if (rows && filterColumn)
      points = filter(data, weightData, filterData, rows, lowerFilter, upperFilter);
    else
      points = rows;

    pointsBinned = 0;
    if (points) {
      if (doStats) {
        if (!weightColumn)
          computeMoments(&mean, &rms, &standDev, &mad, data, points);
        else
          computeWeightedMoments(&mean, &rms, &standDev, &mad, data, weightData, points);
      }
      
      if (!lowerLimitGiven) {
        lowerLimit = 0;
        if (points)
          lowerLimit = data[0];
        for (i=0; i<points; i++) 
          if (lowerLimit>data[i])
            lowerLimit = data[i];
      }
      else
        lowerLimit = givenLowerLimit;
      if (!upperLimitGiven) {
        upperLimit = 0;
        if (points)
          upperLimit = data[0];
        for (i=0; i<points; i++) 
          if (upperLimit<data[i])
            upperLimit = data[i];
      }
      else
        upperLimit = givenUpperLimit;
      
      range = upperLimit-lowerLimit;
      if (!lowerLimitGiven)
        lowerLimit -= range*1e-7;
      if (!upperLimitGiven)
        upperLimit += range*1e-7;
      if (upperLimit==lowerLimit) {
        if (binSize) {
          upperLimit += binSize/2;
          lowerLimit -= binSize/2;
        } else  if (fabs(upperLimit)<sqrt(DBL_MIN)) {
          upperLimit = sqrt(DBL_MIN);
          lowerLimit = -sqrt(DBL_MIN);
        } else {
          upperLimit += upperLimit*(1+2*DBL_EPSILON);
          lowerLimit -= upperLimit*(1-2*DBL_EPSILON);
        }
      }
      if (expansionFactor>0) {
        double center;
        center = (upperLimit+lowerLimit)/2;
        range = expansionFactor*(upperLimit-lowerLimit);
        lowerLimit = center - range/2;
        upperLimit = center + range/2;
      }
      dx = (upperLimit-lowerLimit)/bins;

      if (binSize) {
        double middle;
        range = ((range/binSize)+1)*binSize;
        middle = (lowerLimit+upperLimit)/2;
        lowerLimit = middle-range/2;
        upperLimit = middle+range/2;
        dx = binSize;
        bins = range/binSize+0.5;
        if (bins<1 && !doSides)
          bins = 2*doSides;
        indep = trealloc(indep, sizeof(*indep)*(bins+2*doSides));
        hist = trealloc(hist, sizeof(*hist)*(bins+2*doSides));
        CDF = trealloc(CDF, sizeof(*hist)*(bins+2*doSides));
      }
      
      for (i=-doSides; i<bins+doSides; i++)
        indep[i+doSides] = (i+0.5)*dx + lowerLimit;
      hist1 = hist+doSides;
      CDF1=CDF+doSides;
      if (doSides)
        hist[0] = hist[bins+doSides] = 0;

      if (!weightColumn)
        pointsBinned = make_histogram(hist1, bins, lowerLimit, upperLimit, data, 
                                      points, 1);
      else
        pointsBinned = make_histogram_weighted(hist1, bins, lowerLimit, 
                                               upperLimit, data, points, 1, weightData);
      sum=0;
      for (i=0;i<bins+doSides;i++) {
        sum +=hist1[i]; 
      }
      CDF1[0]=hist1[0]/sum;
      for (i=1;i<bins+doSides;i++) {
        CDF1[i]=CDF1[i-1]+hist1[i]/sum;
      }
      
      if (verbose)
        fprintf(stderr, "%ld points of %ld from page %ld histogrammed in %ld bins\n", 
                pointsBinned, rows, readCode, bins);
      if (!cdfOnly) {
        if (normalizeMode!=NORMALIZE_NO) {
          double norm = 0;
          switch (normalizeMode) {
          case NORMALIZE_PEAK:
            norm = max_in_array(hist1, bins);
            break;
          case NORMALIZE_AREA:
          case NORMALIZE_SUM:
            for (i=norm=0; i<bins; i++)
              norm += hist1[i];
            if (normalizeMode==NORMALIZE_AREA)
              norm *= dx;
            break;
          default:
            SDDS_Bomb("invalid normalize mode--consult programmer.");
            break;
          }
          if (norm)
            for (i=0; i<bins; i++)
              hist1[i] /= norm;
        }
      }
    }
    if (!SDDS_StartPage(&outTable, bins+2*doSides) ||
        !SDDS_CopyParameters(&outTable, &inTable) ||
        (points && (!SDDS_SetColumn(&outTable, SDDS_SET_BY_INDEX, indep,
                          bins+2*doSides, iIndep))) ||
        !SDDS_SetParameters(&outTable, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iBins, bins, iBinSize, dx, iPoints, pointsBinned, -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!freOnly) {
      if (!SDDS_SetColumn(&outTable, SDDS_SET_BY_INDEX, CDF,
                          bins+2*doSides, iCdf))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!cdfOnly) {
      if (!SDDS_SetColumn(&outTable, SDDS_SET_BY_INDEX, hist,
                          bins+2*doSides, iFreq))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
    if (filterColumn && 
        !SDDS_SetParameters(&outTable, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iLoFilter, lowerFilter, iUpFilter, upperFilter, -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (doStats &&
        !SDDS_SetParameters(&outTable, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iMean, mean, iRMS, rms, iStDev, standDev, -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (!SDDS_WritePage(&outTable))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&inTable)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&outTable)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (data)
    free(data);
  if (weightData)
    free(weightData);
  if (filterData)
    free(filterData);
  return(0);
}

/* routine: filter()
 * purpose: filter set of points {(x, y)} with a specified
 *          window in one of the variables. Works in place.
 */

static long filter(double *x, double *y, double *filterData, long npts,
    double lowerFilter, double upperFilter
    )
{
    long i, j;
    static char *keep = NULL;
    static long maxPoints = 0;

    if (maxPoints<npts) 
        keep = trealloc(keep, sizeof(*keep)*(maxPoints=npts));
    
    for (i=0; i<npts; i++) {
        if (filterData[i]<lowerFilter || filterData[i]>upperFilter)
            keep[i] = 0;
        else
            keep[i] = 1;
        }

    for (i=j=0; i<npts; i++) 
        if (keep[i]) {
            if (i!=j) {
                if (x)
                    x[j] = x[i];
                if (y)
                    y[j] = y[j];
                filterData[j] = filterData[i];
                }
            j++;
            }

    return(j);
    }

static long setupOutputFile(SDDS_DATASET *outTable, char *outputfile, SDDS_DATASET *inTable, char *inputfile,
                     char *dataColumn, char *weightColumn, char *filterColumn,
                     double lowerFilter, double upperFilter, long doStats, long bins, 
                     double binSize, long normalizeMode)
{
  char *symbol, *units, *dataUnits, *outputFormat;
  int32_t outputType;
  char s[1024];

  if (!SDDS_InitializeOutput(outTable, SDDS_BINARY, 0, NULL, "sddshist output", outputfile))
    return 0;
  
  if (!SDDS_GetColumnInformation(inTable, "units", &dataUnits, SDDS_GET_BY_NAME, dataColumn))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  /* make output columns */
  outputType = SDDS_DOUBLE;
  outputFormat = NULL;
 
  if (!SDDS_TransferColumnDefinition(outTable, inTable, dataColumn, NULL) ||
      !SDDS_ChangeColumnInformation(outTable, "type", &outputType, SDDS_BY_NAME, dataColumn) ||
      !SDDS_ChangeColumnInformation(outTable, "format_string", &outputFormat, SDDS_BY_NAME,
                                    dataColumn) ||
      (iIndep=SDDS_GetColumnIndex(outTable, dataColumn))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!cdfOnly) {
    switch (normalizeMode) {
    case NORMALIZE_PEAK:
      symbol = "RelativeFrequency";
      units = NULL;
      break;
    case NORMALIZE_AREA:
      symbol = "NormalizedFrequency";
      if (dataUnits && !SDDS_StringIsBlank(dataUnits)) {
        units = tmalloc(sizeof(*units)*(strlen(dataUnits)+5));
        if (strchr(dataUnits, ' '))
          sprintf(units, "1/(%s)", dataUnits);
        else
          sprintf(units, "1/%s", dataUnits);
      }
      else
        units = NULL;
      break;
    case NORMALIZE_SUM:
      symbol = "FractionalFrequency";
      units = NULL;
      break;
    default:
      if (weightColumn) {
        char *weightUnits;
        if (weightColumn && 
            !SDDS_GetColumnInformation(inTable, "units", &weightUnits, SDDS_GET_BY_NAME, weightColumn))
          return 0;
        symbol = "WeightedNumberOfOccurences";
        units = weightUnits;
      }
      else {
        symbol = "NumberOfOccurences";
        units = NULL;
      }
      break;
    }
    
    if ((iFreq=SDDS_DefineColumn(outTable, "frequency", symbol, units, NULL, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    free(units);
    units = NULL;
  }
  if (!freOnly) {
    sprintf(s, "%sCdf", dataColumn);
    if ((iCdf=
         SDDS_DefineColumn(outTable, s, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  /* make output parameters:
     input filename, bins, bin size, filter name, filter limits, stats values, normalization mode 
     */
  if (SDDS_DefineParameter(outTable, "sddshistInput", NULL, NULL, NULL, NULL, SDDS_STRING, 
                           inputfile)<0 ||
      (weightColumn && SDDS_DefineParameter(outTable, "sddshistWeight", NULL, NULL, NULL, NULL,
                                              SDDS_STRING, weightColumn)<0) ||
      (iBins=SDDS_DefineParameter(outTable, "sddshistBins", NULL, NULL, NULL, NULL, SDDS_LONG,
                                  NULL))<0 ||
      (iBinSize=SDDS_DefineParameter(outTable, "sddshistBinSize", NULL, NULL, NULL, NULL,
                                     SDDS_DOUBLE, NULL))<0 ||
      (iPoints=SDDS_DefineParameter(outTable, "sddshistBinned", NULL, NULL, NULL, NULL, SDDS_LONG,
                                    NULL))<0)
    return 0;
  if (filterColumn) {
    char *filterUnits;
    if (!SDDS_GetColumnInformation(inTable, "units", &filterUnits, SDDS_GET_BY_NAME, filterColumn) ||
        SDDS_DefineParameter(outTable, "sddshistFilter", NULL, NULL, NULL, NULL, SDDS_STRING, 
                             filterColumn)<0 ||
        (iLoFilter=SDDS_DefineParameter(outTable, "sddshistLowerFilter", NULL, filterUnits, NULL, 
                                        NULL, SDDS_DOUBLE, NULL))<0 ||
        (iUpFilter=SDDS_DefineParameter(outTable, "sddshistUpperFilter", NULL, filterUnits, NULL, 
                                        NULL, SDDS_DOUBLE, NULL))<0)
      return 0;
    if (filterUnits)
      free(filterUnits);
  }
  if (doStats) {
    char *buffer;
    buffer = tmalloc(sizeof(*buffer)*(strlen(dataColumn)+20));
    sprintf(buffer, "%sMean", dataColumn);
    if ((iMean=SDDS_DefineParameter(outTable, buffer, NULL, dataUnits, NULL, NULL, 
                                    SDDS_DOUBLE, NULL))<0)
      return 0;
    sprintf(buffer, "%sRms", dataColumn);
    if ((iRMS=SDDS_DefineParameter(outTable, buffer, NULL, dataUnits, NULL, NULL,
                                   SDDS_DOUBLE, NULL))<0)
      return 0;
    sprintf(buffer, "%sStDev", dataColumn);
    if ((iStDev=SDDS_DefineParameter(outTable, buffer, NULL, dataUnits, NULL, NULL,
                                     SDDS_DOUBLE, NULL))<0)
      return 0;
  }
    if (SDDS_DefineParameter(outTable, "sddshistNormMode", NULL, NULL, NULL, NULL, SDDS_STRING, 
                             normalize_option[normalizeMode])<0 ||
        !SDDS_TransferAllParameterDefinitions(outTable, inTable, SDDS_TRANSFER_KEEPOLD) ||
        !SDDS_WriteLayout(outTable))
      return 0;
  return 1;
}

