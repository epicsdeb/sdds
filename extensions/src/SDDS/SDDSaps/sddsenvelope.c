/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsenvelope
 * purpose: combine data from SDDS pages to make a new file
 *          minimum, maximum, etc. values for specified columns
 * 
 * Michael Borland, 1995
 $Log: sddsenvelope.c,v $
 Revision 1.21  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.20  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.19  2004/02/14 06:58:06  borland
 Added -signedLargest option.

 Revision 1.18  2003/07/03 20:48:50  borland
 Fixed bug in last revision: didn't work for percentiles with wildcards.

 Revision 1.17  2003/06/13 22:58:44  borland
 Percentages can now be floating-point numbers for -percentile option.

 Revision 1.16  2002/11/12 02:23:37  borland
 Added -percentile processing.

 Revision 1.15  2002/11/09 00:18:13  borland
 Added -largest processing mode.

 Revision 1.14  2002/08/14 17:12:44  soliday
 Added Open License

 Revision 1.13  2001/11/14 22:19:32  shang
 remove printing warning messages when pages<2

 Revision 1.12  2001/11/14 21:28:56  shang
 added weighted statistical analysis features

 Revision 1.11  2001/01/10 19:35:36  soliday
 Standardized usage message.

 Revision 1.10  2000/04/06 14:27:11  borland
 Fixed unitialized memory read that showed up with median/decile range
 addition.

 Revision 1.9  2000/04/06 09:32:31  emery
 Added median and decile range processing.

 Revision 1.8  1999/05/25 19:08:55  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:43  borland
 Fixed the version number in the usage message.

 Revision 1.6  1996/09/06 15:47:11  borland
 Fixed problem with naming of columns produced by -sum with power greater than
 1.  Names were not allowable after SDDS1.5.  Also improved error message
 handling.

 * Revision 1.5  1996/01/21  00:13:37  borland
 * Put in initializations for some variables, including value2, value3, and
 * value4 arrays in structures.
 *
 * Revision 1.4  1995/11/10  21:59:22  borland
 * Fixed bug for slope/intercept computation; was passing SDDS_DATASET structure when I
 * should have passed a pointer to same.
 *
 * Revision 1.3  1995/09/06  14:56:24  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include <ctype.h>

#define SET_COPY 0  
#define SET_MAXIMA 1
#define SET_MINIMA 2
#define SET_MEANS 3
#define SET_SDS 4
#define SET_RMSS 5
#define SET_SUMS 6
#define SET_SLOPE 7
#define SET_INTERCEPT 8
#define SET_PIPE 9
#define SET_SIGMAS 10
#define SET_MEDIAN 11
#define SET_DRANGE 12
#define SET_WMEANS 13
#define SET_WSDS 14
#define SET_WRMSS 15
#define SET_WSIGMAS 16
#define SET_NOWARNINGS 17
#define SET_LARGEST 18
#define SET_PERCENTILE 19
#define SET_SIGNEDLARGEST 20
#define N_OPTIONS 21

char *option[N_OPTIONS] = {
  "copy", "maximum", "minimum", "mean", "standarddeviations", "rms", "sum",
  "slope", "intercept", "pipe", "sigmas", "median", "decilerange","wmean",
  "wstandarddeviations",
  "wrms","wsigma","nowarnings", "largest", "percentile", 
  "signedlargest",
  } ;
char *optionSuffix[N_OPTIONS] = {
  "", "Max", "Min", "Mean", "StDev", "Rms", "Sum", "Slope", "Intercept", "",
  "Sigma", "Median", "DRange","WMean","WStDev","WRms","WSigma","", "Largest",
  "Percentile", "SignedLargest",
  } ;

/* this structure stores a command-line request for statistics computation */
/* columnName may contain wildcards */
typedef struct {
  char *columnName;
  char *weightColumnName;
  long optionCode, sumPower;
  double percentile;
  char *percentileString;
  char *functionOf;
} STAT_REQUEST;

/* this structure stores data necessary for accessing/creating SDDS columns and
 * for computing a statistic
 */
typedef struct {
  char *sourceColumn, *weightColumn,*resultColumn, *functionOf;
  long optionCode, resultIndex, sumPower;
  double percentile;
  char *percentileString;
  /* these store intermediate values during processing */
  void *copy;
  double *value1, *value2, *value3, *value4;
  double **array;
  double *sumWeight;
} STAT_DEFINITION;

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, long items, long code, 
                     double percentile, long power, char *functionOf, long weighted, char *percentileString);
/*weighted=0, no weighted column; else, weighted statistic, the weight factor is given by
weightedColumn*/
STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inTable, long *stats, 
                                        STAT_REQUEST *request, long requests);
long setupOutputFile(SDDS_DATASET *outTable, char *output, SDDS_DATASET *inTable, 
                     STAT_DEFINITION *stat, long stats, long rows);

static char *USAGE="sddsenvelope [<input>] [<output>] [-copy=<column-names>] \n\
  [-pipe=[input][,output]] [-nowarnings]\n\
  [-maximum=<column-names>] [-minimum=<column-names>] \n\
  [-largest=<column-names>] [-signedLargest=<column-names>] \n\
  [-mean=<column-names>] [-sum=<power>,<column-names>] \n\
  [-median=<column-names>] [-decilerange=<column-names>] \n\
  [-percentile=<percentage>,<column-names>] \n\
  [-standarddeviation=<column-names>] [-rms=<column-names>] \n\
  [-sigma=<column-names>] \n\
  [-slope=<indep-variable>,<column-names>]\n\
  [-intercept=<indep-variable>,<column-names>] \n\
  [-wmean=<weightColumn>,<columnNames>] \n\
  [-wstandarddeviation=<weightColumn>,<columnNames>] \n\
  [-wrms=<weightColumn>,<columnNames>] \n\
  [-wsigma=<weightColumn>,<columnNames>] \n\n\
  Processes pages from <input> to produce <output> with\n\
  one page containing the specified quantities across pages\n\
  for each row of the specified columns.\n\
  Program by Michael Borland. (This is version 8, November 2002, M. Borland.)";


int main(int argc, char **argv)
{
  STAT_DEFINITION *stat;
  long stats;
  STAT_REQUEST *request;
  long requests;
  SCANNED_ARG *scanned;      /* structure for scanned arguments */
  SDDS_DATASET inTable, outTable;
  long i_arg, code, power, iStat, rows, firstRows, i, pages, nowarnings=0;
  char *input, *output;
  double *inputData, indepData, *weight;
  unsigned long pipeFlags;
  double decilePoint[2]   = {10.0, 90.0 }, decileResult[2];
  double percentilePoint, percentileResult;
  double percentile;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2) {
    bomb("too few arguments", USAGE);
  }
  weight=NULL;
  input = output = NULL;
  stat = NULL;
  request = NULL;
  stats = requests = pipeFlags = 0;
  rows = firstRows = i = 0;

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (scanned[i_arg].arg_type==OPTION) {
      /* process options here */
      switch (code=match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COPY:
      case SET_MINIMA:
      case SET_MAXIMA:
      case SET_LARGEST:
      case SET_SIGNEDLARGEST:
      case SET_MEANS:
      case SET_SDS:
      case SET_SIGMAS:
      case SET_RMSS:
      case SET_MEDIAN:
      case SET_DRANGE:
        if (scanned[i_arg].n_items<2) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        requests = addStatRequests(&request, requests, 
                                   scanned[i_arg].list+1, scanned[i_arg].n_items-1,
                                   code, 0, 0, NULL, 0, NULL);
        break;
      case SET_WMEANS:
      case SET_WSDS:
      case SET_WRMSS:
      case SET_WSIGMAS:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        /*note here, items=scanned[i_arg].n_items-2, because the weigthedColumn shoule be exclued*/
        requests = addStatRequests(&request, requests, 
                                   scanned[i_arg].list+1, scanned[i_arg].n_items-1,
                                   code, 0, 0, NULL, 1, NULL);
        break;
      case SET_SUMS:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        if (sscanf(scanned[i_arg].list[1], "%ld", &power)!=1 || power<1) {
          fprintf(stderr, "error: invalid -%s syntax--bad power in field %s\n", 
                  option[code], scanned[i_arg].list[2]);
          exit(1);
        }
        requests = addStatRequests(&request, requests,
                                   scanned[i_arg].list+2, scanned[i_arg].n_items-2,
                                   code, 0, power, NULL,0, NULL);
        break;
      case SET_PERCENTILE:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        if (sscanf(scanned[i_arg].list[1], "%lf", &percentile)!=1 || percentile<0 || percentile>100) {
          fprintf(stderr, "error: invalid -%s syntax--bad percentage in field %s\n", 
                  option[code], scanned[i_arg].list[1]);
          exit(1);
        }
        requests = addStatRequests(&request, requests,
                                   scanned[i_arg].list+2, scanned[i_arg].n_items-2,
                                   code, percentile, 0, NULL, 0, scanned[i_arg].list[1]);
        break;
      case SET_SLOPE:
      case SET_INTERCEPT:
        if (scanned[i_arg].n_items<3) {
          fprintf(stderr, "error: invalid -%s syntax\n", option[code]);
          exit(1);
        }
        requests = addStatRequests(&request, requests,
                                   scanned[i_arg].list+2, scanned[i_arg].n_items-2,
                                   code, 0, 0, scanned[i_arg].list[1], 0, NULL);
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_NOWARNINGS:
        nowarnings=1;
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
  
  processFilenames("sddsenvelope", &input, &output, pipeFlags, 0, NULL);

  if (!requests)
    SDDS_Bomb("no statistics requested");
  
  if (!SDDS_InitializeInput(&inTable, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  pages = 0;
  while ((code=SDDS_ReadPage(&inTable))>0) {
    pages++;
    if (!(rows = SDDS_CountRowsOfInterest(&inTable)))
      SDDS_Bomb("empty data page in input file");
    if (code==1) {
      firstRows = rows;
      if (!(stat=compileStatDefinitions(&inTable, &stats, request, requests))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (!setupOutputFile(&outTable, output, &inTable, stat, stats, rows)) {
        if (SDDS_NumberOfErrors())
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        else
          fprintf(stderr, "Error setting up output file.\n");
        exit(1);
      }
    }
    else if (firstRows!=rows)
      SDDS_Bomb("inconsistent number of rows in input file");
    for (iStat=0; iStat<stats; iStat++) {
      if (stat[iStat].optionCode==SET_COPY) {
        if (code==1 && !(stat[iStat].copy = SDDS_GetColumn(&inTable, stat[iStat].sourceColumn)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        continue;
      }
      stat[iStat].copy = NULL;
      if (!(inputData = SDDS_GetColumnInDoubles(&inTable, stat[iStat].sourceColumn)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      switch (stat[iStat].optionCode) {
      case SET_MINIMA:
        if (code==1)
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] = inputData[i];
        else 
          for (i=0; i<rows; i++)
            if (stat[iStat].value1[i]>inputData[i])
              stat[iStat].value1[i] = inputData[i];
        break;
      case SET_MAXIMA:
        if (code==1)
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] = inputData[i];
        else 
          for (i=0; i<rows; i++)
            if (stat[iStat].value1[i]<inputData[i])
              stat[iStat].value1[i] = inputData[i];
        break;
      case SET_LARGEST:
        if (code==1)
          for (i=0; i<rows; i++) 
            stat[iStat].value1[i] = fabs(inputData[i]);
        else 
          for (i=0; i<rows; i++)
            if (stat[iStat].value1[i]<fabs(inputData[i]))
              stat[iStat].value1[i] = fabs(inputData[i]);
        break;
      case SET_SIGNEDLARGEST:
        if (code==1)
          for (i=0; i<rows; i++) 
            stat[iStat].value1[i] = inputData[i];
        else 
          for (i=0; i<rows; i++)
            if (fabs(stat[iStat].value1[i])<fabs(inputData[i]))
              stat[iStat].value1[i] = inputData[i];
        break;
      case SET_MEANS:
        if (code==1)
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] = inputData[i];
        else 
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] += inputData[i];
        break;
      case SET_WMEANS:
        if (!(weight = SDDS_GetColumnInDoubles(&inTable, stat[iStat].weightColumn)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (i=0; i<rows; i++) {
          stat[iStat].sumWeight[i] += weight[i];
          stat[iStat].value1[i] += inputData[i]*weight[i];
        }
        free(weight);
        break;
      case SET_SDS:
      case SET_SIGMAS:
        if (code==1)
          for (i=0; i<rows; i++) {
            stat[iStat].value1[i] = inputData[i];
            stat[iStat].value2[i] = inputData[i]*inputData[i];
          }
        else 
          for (i=0; i<rows; i++) {
            stat[iStat].value1[i] += inputData[i];
            stat[iStat].value2[i] += inputData[i]*inputData[i];
          }
        break;
      case SET_WSDS:
      case SET_WSIGMAS:
        if (!(weight = SDDS_GetColumnInDoubles(&inTable, stat[iStat].weightColumn)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (i=0; i<rows; i++) {
          stat[iStat].sumWeight[i] +=weight[i];
          stat[iStat].value1[i] += inputData[i]*weight[i];
          stat[iStat].value2[i] += inputData[i]*inputData[i]*weight[i];
        }
        free(weight);
        break;
      case SET_RMSS:
        if (code==1)
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] = inputData[i]*inputData[i];
        else 
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] += inputData[i]*inputData[i];
        break;
      case SET_WRMSS:
        if (!(weight = SDDS_GetColumnInDoubles(&inTable, stat[iStat].weightColumn)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (i=0; i<rows; i++) {
          stat[iStat].sumWeight[i] +=weight[i];
          stat[iStat].value1[i] += inputData[i]*inputData[i]*weight[i];
        }
        free(weight);
        break;
      case SET_SUMS:
        if (code==1)
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] = ipow(inputData[i], stat[iStat].sumPower);
        else 
          for (i=0; i<rows; i++)
            stat[iStat].value1[i] += ipow(inputData[i], stat[iStat].sumPower);
        break;
      case SET_SLOPE:
      case SET_INTERCEPT:
        if (!SDDS_GetParameterAsDouble(&inTable, stat[iStat].functionOf, &indepData)) {
          fprintf(stderr, "error:  unable to get value of parameter %s\n", stat[iStat].functionOf);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        /* linear fit:
           y = a + bx
           a = (S x^2 Sy - S x S xy)/D
           b = (N S xy  - Sx Sy)/D
           D = N S x^2 - (S x)^2
           */
        if (code==1) 
          for (i=0; i<rows; i++) {
            stat[iStat].value1[i] = indepData;                 /* Sum x   */
            stat[iStat].value2[i] = indepData*indepData;       /* Sum x^2 */
            stat[iStat].value3[i] = inputData[i];              /* Sum y   */
            stat[iStat].value4[i] = indepData*inputData[i];    /* Sum xy  */
          }
        else 
          for (i=0; i<rows; i++) {
            stat[iStat].value1[i] += indepData;
            stat[iStat].value2[i] += indepData*indepData;
            stat[iStat].value3[i] += inputData[i];
            stat[iStat].value4[i] += indepData*inputData[i];
          }
        break;
      case SET_MEDIAN:
      case SET_DRANGE:
      case SET_PERCENTILE:
        if (code==1)
          for (i=0; i<rows; i++) {
            stat[iStat].array[i] = tmalloc(sizeof(*stat[iStat].array[i]));
            stat[iStat].array[i][pages-1] = inputData[i];
          }
        else {
          for (i=0; i<rows; i++) {
            stat[iStat].array[i] = SDDS_Realloc(stat[iStat].array[i],sizeof(*stat[iStat].array[i])*pages);
            stat[iStat].array[i][pages-1] = inputData[i];
          }
        }
        break;
      default:
        SDDS_Bomb("invalid statistic code (accumulation loop)");
        break;
      }
      free(inputData);
    }
  }
  if (pages==0)
    SDDS_Bomb("no pages in input");
  for (iStat=0; iStat<stats; iStat++) {
    switch (stat[iStat].optionCode) {
    case SET_COPY:
    case SET_MINIMA:
    case SET_MAXIMA:
    case SET_LARGEST:
    case SET_SIGNEDLARGEST:
    case SET_SUMS:
      break;
    case SET_MEANS:
      for (i=0; i<rows; i++)
        stat[iStat].value1[i] /= pages;
      break;
    case SET_WMEANS:
      for (i=0; i<rows; i++)
        if (stat[iStat].sumWeight[i])
          stat[iStat].value1[i] /= stat[iStat].sumWeight[i];
        else {
          if (!nowarnings)
            fprintf(stderr,"warning: the total weight for the %ldth row of %s is zero.\n",
                    i+1,stat[iStat].sourceColumn);
          stat[iStat].value1[i] =DBL_MAX;
        }
      break;
    case SET_SDS:
      if (pages<2)
        stat[iStat].value1[i] = DBL_MAX;
      else 
        for (i=0; i<rows; i++) {
          double tmp1;
          if ((tmp1 = stat[iStat].value2[i]/pages-sqr(stat[iStat].value1[i]/pages))<=0)
            stat[iStat].value1[i] = 0;
          else
            stat[iStat].value1[i] = sqrt(tmp1*pages/(pages-1.0));
        }
      break;
    case SET_WSDS:
      if (pages<2)
        stat[iStat].value1[i] = DBL_MAX;
      else 
        for (i=0; i<rows; i++) {
          double tmp1;
          if (stat[iStat].sumWeight[i]) {
            if ((tmp1 = stat[iStat].value2[i]/stat[iStat].sumWeight[i]
                 -sqr(stat[iStat].value1[i]/stat[iStat].sumWeight[i]))<=0)
              stat[iStat].value1[i] = 0;
            else
              stat[iStat].value1[i] = sqrt(tmp1*pages/(pages-1.0));
          } else {
            if (!nowarnings)
              fprintf(stderr,"Warning, the total weight for the %ldth row of %s is zero.\n",
                      i+1,stat[iStat].sourceColumn);
            stat[iStat].value1[i] = DBL_MAX;
          }
        }
      break;
    case SET_SIGMAS:
      if (pages<2)
        stat[iStat].value1[i] = DBL_MAX;
      else 
        for (i=0; i<rows; i++) {
          double tmp1;
          if ((tmp1 = stat[iStat].value2[i]/pages-sqr(stat[iStat].value1[i]/pages))<=0)
            stat[iStat].value1[i] = 0;
          else
            stat[iStat].value1[i] = sqrt(tmp1/(pages-1.0));
        }
      break;
    case SET_WSIGMAS:
      if (pages<2)
        stat[iStat].value1[i] = DBL_MAX;
      else 
        for (i=0; i<rows; i++) {
          double tmp1;
          if (stat[iStat].sumWeight[i]) {
            if ((tmp1 = stat[iStat].value2[i]/stat[iStat].sumWeight[i]-
                 sqr(stat[iStat].value1[i]/stat[iStat].sumWeight[i]))<=0)
              stat[iStat].value1[i] = 0;
            else
              stat[iStat].value1[i] = sqrt(tmp1/(pages-1.0));
          } else {
            if (!nowarnings)
              fprintf(stderr,"Warning, the total weight for the %ldth row of %s is zero.\n",
                      i+1,stat[iStat].sourceColumn);
            stat[iStat].value1[i] =DBL_MAX;
          }
        }
      break;
    case SET_RMSS:
      for (i=0; i<rows; i++)
        stat[iStat].value1[i] = sqrt(stat[iStat].value1[i]/pages);
      break;
    case SET_WRMSS:
      for (i=0; i<rows; i++) {
        if (stat[iStat].sumWeight[i])
          stat[iStat].value1[i] = sqrt(stat[iStat].value1[i]/stat[iStat].sumWeight[i]);
        else {
          if (!nowarnings)
            fprintf(stderr,"Warning, the total weight for the %ldth row of %s is zero.\n",
                    i+1,stat[iStat].sourceColumn);
          stat[iStat].value1[i] =DBL_MAX;
        }
      }
      break;  
    case SET_SLOPE:
      for (i=0; i<rows; i++) {
        double D;
        D = rows*stat[iStat].value2[i] - stat[iStat].value1[i]*stat[iStat].value1[i];
        stat[iStat].value1[i] = (rows*stat[iStat].value4[i] - stat[iStat].value1[i]*stat[iStat].value3[i])/D;
      }
      break;
    case SET_INTERCEPT:
      for (i=0; i<rows; i++) {
        double D;
        D = rows*stat[iStat].value2[i] - stat[iStat].value1[i]*stat[iStat].value1[i];
        stat[iStat].value1[i] = (stat[iStat].value2[i]*stat[iStat].value3[i] -
                                 stat[iStat].value1[i]*stat[iStat].value4[i])/D;
      }
      break;
    case SET_MEDIAN:
      for (i=0; i<rows; i++) {
        compute_median(&stat[iStat].value1[i], stat[iStat].array[i], pages);
      }
      break;
    case SET_DRANGE:
      for (i=0; i<rows; i++) {
        if (!compute_percentiles(decileResult, decilePoint, 2, stat[iStat].array[i], pages))
          stat[iStat].value1[i] = 0;
        else
          stat[iStat].value1[i] = decileResult[1] - decileResult[0];
      }
      break;
    case SET_PERCENTILE:
      percentilePoint = stat[iStat].percentile;
      for (i=0; i<rows; i++) {
        if (!compute_percentiles(&percentileResult, &percentilePoint, 1, stat[iStat].array[i], pages))
          stat[iStat].value1[i] = 0;
        else
          stat[iStat].value1[i] = percentileResult;
      }
      break;
    default:
      SDDS_Bomb("invalid statistic code (final loop)");
      break;
    }
    if (stat[iStat].optionCode==SET_COPY) {
      if (!SDDS_SetColumn(&outTable, SDDS_SET_BY_NAME, stat[iStat].copy, rows, stat[iStat].resultColumn)) {
        fprintf(stderr, "error setting column values for column %s\n",
                stat[iStat].resultColumn);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    else if (!SDDS_SetColumnFromDoubles(&outTable, SDDS_SET_BY_NAME, stat[iStat].value1, rows, 
                                        stat[iStat].resultColumn)) {
      fprintf(stderr, "error setting column values for column %s\n",
              stat[iStat].resultColumn);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (stat[iStat].value1)
      free(stat[iStat].value1);
    if (stat[iStat].value2)
      free(stat[iStat].value2);
    if (stat[iStat].value3)
      free(stat[iStat].value3);
    if (stat[iStat].value4)
      free(stat[iStat].value4);
    if (stat[iStat].copy)
      free(stat[iStat].copy);
    if (stat[iStat].array) {
      for (i=0; i<rows; i++) {
        free(stat[iStat].array[i]);
      }
      free(stat[iStat].array);
    }
    if (stat[iStat].sumWeight)
      free(stat[iStat].sumWeight);
    free(stat[iStat].sourceColumn);
    free(stat[iStat].resultColumn);
    stat[iStat].value1 = stat[iStat].value2 = stat[iStat].value3 = stat[iStat].value4 = NULL;
    stat[iStat].copy = NULL;
    stat[iStat].array = NULL;
  }
  free(stat);
  if (!SDDS_WritePage(&outTable) || !SDDS_Terminate(&inTable) || !SDDS_Terminate(&outTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  return(0);
}

long addStatRequests(STAT_REQUEST **statRequest, long requests, char **item, long items, long code,
                     double percentile, long power, char *functionOf, long weighted, char *percentileString)
{
  long i;
  /*weighted factor should be either 0 or 1*/
  if (weighted!=0 && weighted!=1) 
    SDDS_Bomb("addStatRequests: weighted parameter should be either 0 or 1");
  if (code==SET_PERCENTILE && (!percentileString || !strlen(percentileString))) {
    fprintf(stderr, "Percentile specification is incorrect: percentile=%e, percentileString=%s\n",
            percentile, percentileString?percentileString:"NULL");
    exit(1);
  }
  *statRequest = SDDS_Realloc(*statRequest, sizeof(**statRequest)*(requests+items-weighted));
  for (i=0; i<items-weighted; i++) {
    if (weighted)
      (*statRequest)[requests+i].weightColumnName=item[0];
    else
      (*statRequest)[requests+i].weightColumnName=NULL;
    (*statRequest)[i+requests].columnName = item[i+weighted];
    (*statRequest)[i+requests].optionCode = code;
    (*statRequest)[i+requests].sumPower = power;
    (*statRequest)[i+requests].percentile = percentile;
    (*statRequest)[i+requests].percentileString = percentileString;
    (*statRequest)[i+requests].functionOf = functionOf;
  }
  
  return items+requests-weighted;
}

STAT_DEFINITION *compileStatDefinitions(SDDS_DATASET *inTable, long *stats, 
                                        STAT_REQUEST *request, long requests)
{
  STAT_DEFINITION *stat;
  long iReq, iStat, iName;
  int32_t columnNames;
  char s[SDDS_MAXLINE];
  char **columnName;

  *stats = iStat = 0;
  stat = tmalloc(sizeof(*stat)*requests);
  for (iReq=0; iReq<requests; iReq++) {
    if (iStat>=*stats)
      stat = SDDS_Realloc(stat, sizeof(*stat)*(*stats+=10));
    if (!has_wildcards(request[iReq].columnName)) {
      if (SDDS_GetColumnIndex(inTable, request[iReq].columnName)<0) {
        sprintf(s, "error: column %s not found input file", 
                request[iReq].columnName);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      stat[iStat].weightColumn=request[iReq].weightColumnName;
      stat[iStat].sourceColumn = request[iReq].columnName;
      stat[iStat].optionCode = request[iReq].optionCode;
      stat[iStat].percentile   = request[iReq].percentile;
      stat[iStat].percentileString   = request[iReq].percentileString;
      stat[iStat].sumPower   = request[iReq].sumPower;
      stat[iStat].value1 = stat[iStat].value2 = 
        stat[iStat].value3 = stat[iStat].value4 = NULL;
      stat[iStat].array = NULL;
      stat[iStat].copy = NULL;
      stat[iStat].sumWeight=NULL;
      if ((stat[iStat].functionOf = request[iReq].functionOf) &&
          SDDS_GetParameterIndex(inTable, request[iReq].functionOf)<0) {
        sprintf(s, "error: parameter %s not found input file",
                request[iReq].functionOf);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      iStat++;
    }
    else {
      SDDS_SetColumnFlags(inTable, 0);
      if (!SDDS_SetColumnsOfInterest(inTable, SDDS_MATCH_STRING, request[iReq].columnName,
                                     SDDS_OR))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!(columnName = SDDS_GetColumnNames(inTable, &columnNames))) {
        sprintf(s, "no columns selected for wildcard sequence %s", 
                request[iReq].columnName);
        SDDS_SetError(s);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (iStat+columnNames>*stats)
        stat = SDDS_Realloc(stat, sizeof(*stat)*(*stats=iStat+columnNames+10));
      for (iName=0; iName<columnNames; iName++) {
        stat[iStat+iName].weightColumn=request[iReq].weightColumnName;
        stat[iStat+iName].sourceColumn = columnName[iName];
        stat[iStat+iName].optionCode = request[iReq].optionCode;
        stat[iStat+iName].sumPower   = request[iReq].sumPower;
        stat[iStat+iName].percentile = request[iReq].percentile;
        stat[iStat+iName].percentileString = request[iReq].percentileString;
        stat[iStat+iName].value1 = stat[iStat+iName].value2 = 
          stat[iStat+iName].value3 = stat[iStat+iName].value4 = NULL;
        stat[iStat+iName].array = NULL;
        stat[iStat+iName].copy = NULL;
        stat[iStat+iName].sumWeight=NULL;
        if ((stat[iStat+iName].functionOf = request[iReq].functionOf) && iName==0 &&
            SDDS_GetParameterIndex(inTable, request[iReq].functionOf)<0) {
          sprintf(s, "error: parameter %s not found input file",
                  request[iReq].functionOf);
          SDDS_SetError(s);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        
      }
      iStat += columnNames;
      free(columnName);
    }
  }

  *stats = iStat;
  for (iStat=0; iStat<*stats; iStat++) {
    switch (stat[iStat].optionCode) {
    case SET_COPY:
      strcpy(s, stat[iStat].sourceColumn);
      break;
    case SET_SUMS:
      if (stat[iStat].sumPower==1)
        sprintf(s, "%s%s", stat[iStat].sourceColumn, optionSuffix[stat[iStat].optionCode]);
      else
        sprintf(s, "%s%ld%s", stat[iStat].sourceColumn, stat[iStat].sumPower,
                optionSuffix[stat[iStat].optionCode]);
      break;
    case SET_PERCENTILE:
      sprintf(s, "%s%s%s", stat[iStat].sourceColumn, stat[iStat].percentileString,
              optionSuffix[stat[iStat].optionCode]);
      break;
    default:
      sprintf(s, "%s%s", stat[iStat].sourceColumn, optionSuffix[stat[iStat].optionCode]);
      break;
    }
    if (!SDDS_CopyString(&stat[iStat].resultColumn, s))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  return stat;
}

long setupOutputFile(SDDS_DATASET *outTable, char *output, SDDS_DATASET *inTable, 
                     STAT_DEFINITION *stat, long stats, long rows)
{
  long column;
  char s[SDDS_MAXLINE], *symbol, *symbol1;

  if (!SDDS_InitializeOutput(outTable, SDDS_BINARY, 0, NULL, "sddsenvelope output", output))
    return 0;
  for (column=0; column<stats; column++) {
    stat[column].value1 = calloc(sizeof(*stat[column].value1),rows);
    stat[column].value2 = stat[column].value3 = stat[column].value4 = NULL;
    if (stat[column].optionCode==SET_SDS || stat[column].optionCode==SET_SIGMAS ||
        stat[column].optionCode==SET_WSDS || stat[column].optionCode==SET_WSIGMAS)
      stat[column].value2 = calloc(sizeof(*stat[column].value2),rows);
    if (stat[column].optionCode==SET_INTERCEPT || stat[column].optionCode==SET_SLOPE) {
      stat[column].value2 = malloc(sizeof(*stat[column].value2)*rows);
      stat[column].value3 = malloc(sizeof(*stat[column].value3)*rows);
      stat[column].value4 = malloc(sizeof(*stat[column].value4)*rows);
    }
    if (stat[column].optionCode==SET_WSDS || stat[column].optionCode==SET_WSIGMAS ||
        stat[column].optionCode==SET_WRMSS || stat[column].optionCode==SET_WMEANS)
      stat[column].sumWeight=calloc(sizeof(*stat[column].sumWeight),rows);
    if (stat[column].optionCode==SET_MEDIAN || stat[column].optionCode==SET_DRANGE || 
        stat[column].optionCode==SET_PERCENTILE) {
      stat[column].array = tmalloc(sizeof(*stat[column].array)*rows);
    }
    if (!SDDS_TransferColumnDefinition(outTable, inTable, stat[column].sourceColumn,
                                       stat[column].resultColumn)) {
      sprintf(s, "Problem transferring definition of column %s to %s\n",
              stat[column].sourceColumn, stat[column].resultColumn);
      SDDS_SetError(s);
      return 0;
    }
    if (!SDDS_ChangeColumnInformation(outTable, "description", NULL,  
                                      SDDS_SET_BY_NAME, stat[column].resultColumn) ||
        SDDS_GetColumnInformation(outTable, "symbol", &symbol, SDDS_BY_NAME,
                                  stat[column].resultColumn)!=SDDS_STRING)
      return 0;
    if (!symbol)
      SDDS_CopyString(&symbol, stat[column].sourceColumn);
    switch (stat[column].optionCode) {
    case SET_COPY:
      strcpy(s, symbol);
      break;
    case SET_SUMS:
      if (stat[column].sumPower==1)
        sprintf(s, "%s[%s]", optionSuffix[stat[column].optionCode], symbol);
      else
        sprintf(s, "%s[%s$a%ld$n]", optionSuffix[stat[column].optionCode], symbol,
                stat[column].sumPower);
      break;
    case SET_PERCENTILE:
      sprintf(s, "%s[%s,%g]", optionSuffix[stat[column].optionCode], symbol,
              stat[column].percentile);
      break;
    case SET_INTERCEPT:
    case SET_SLOPE:
      if (SDDS_GetParameterInformation(outTable, "symbol", &symbol1, SDDS_BY_NAME,
                                       stat[column].functionOf)!=SDDS_STRING)
        symbol1 = stat[column].functionOf;
      sprintf(s, "%s[%s:%s]", optionSuffix[stat[column].optionCode], symbol, symbol1);
      break;
    default:
      sprintf(s, "%s[%s]", optionSuffix[stat[column].optionCode], symbol);
      break;
    }
    free(symbol);
    if (!SDDS_ChangeColumnInformation(outTable, "symbol", s, SDDS_BY_NAME,
                                      stat[column].resultColumn))
      return 0;
  }
  if (!SDDS_WriteLayout(outTable) || !SDDS_StartPage(outTable, rows))
    return 0;
  return 1;
}

