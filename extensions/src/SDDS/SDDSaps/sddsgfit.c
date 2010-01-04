/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsgfit.c
 * purpose: do an gaussian fit:  y(n) = baseline + N*exp(-0.5*(x(n)-mean)^2/sigma^2)
 * M. Borland, 1995
 $Log: sddsgfit.c,v $
 Revision 1.25  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.24  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.23  2003/10/15 17:55:45  borland
 Put limits on variables to prevent negative sigma (which can happen since
 only sigma^2 appears in the penalty function).

 Revision 1.22  2003/07/13 21:26:56  borland
 Removed debugging statement that caused information about disabled
 parameters to be printed.

 Revision 1.21  2003/01/21 18:57:47  borland
 Added yet another argument for simplexMin().

 Revision 1.20  2003/01/16 18:47:33  soliday
 Changed simplexMin because the arguments changed.

 Revision 1.19  2002/08/14 17:12:46  soliday
 Added Open License

 Revision 1.18  2002/01/16 18:17:10  soliday
 Added missing SDDS_terminate and free functions.

 Revision 1.17  2001/08/08 19:21:19  borland
 Made consistent with changes to simplexMin() (addition of n_divisions argument).

 Revision 1.16  2001/05/15 16:41:27  borland
 sddsgenericfit now supports the "no1dscans" option of simplexMin() from
 the commandline.  Others were modified to accomodate the new option,
 but don't allow commandline specification.

 Revision 1.15  2001/01/10 19:35:38  soliday
 Standardized usage message.

 Revision 1.14  2000/07/13 20:24:58  borland
 Per D. Blachowicz: fixed bug with -fixValues/-startValues consistency checking.

 Revision 1.13  2000/06/15 00:54:43  borland
 Added -fixValues option that permits giving fixed values for one or
 more fit parameters.

 Revision 1.12  1999/05/25 19:10:30  soliday
 Removed compiler warning on linux.

 Revision 1.11  1999/01/07 22:25:14  borland
 Brought up-to-date with latest version of simplexMin().  Made some improvements
 to sddssinefit but it still doesn't do well.

 Revision 1.10  1999/01/06 19:54:45  borland
 Fixed the version number in the usage message.

 Revision 1.9  1998/12/16 21:26:03  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.8  1998/11/13 22:39:52  borland
 Now copies through parameters from the input to the output.

 * Revision 1.7  1996/05/19  00:11:50  borland
 * Fixed bug in routine that computes initial guess.  (Wasn't doing sigma
 * guess right in all cases.)
 *
 * Revision 1.6  1996/04/23  20:31:01  borland
 * Updated usage message to include proper version.
 *
 * Revision 1.5  1996/04/23  19:56:31  borland
 * Implemented -fitRange option.
 *
 * Revision 1.4  1996/02/14  01:05:23  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:56:34  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
 
#define SET_FITRANGE 0
#define SET_GUESSES 1
#define SET_VERBOSITY 2
#define SET_COLUMNS 3
#define SET_TOLERANCE 4
#define SET_FULLOUTPUT 5
#define SET_STEPSIZE 6
#define SET_LIMITS 7
#define SET_PIPE 8
#define SET_FIXVALUE 9
#define N_OPTIONS 10

char *option[N_OPTIONS] = {
    "fitrange", "guesses", "verbosity", "columns", "tolerance", "fulloutput",
    "stepsize", "limits", "pipe", "fixvalue",
    };

static char *USAGE="sddsgfit [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
-columns=<x-name>,<y-name>[,ySigma=<sy-name>] \n\
[-fitRange=<lower>,<upper>] [-fullOutput] \n\
[-verbosity=<integer>] [-stepSize=<factor>] [-tolerance=<value>] \n\
[-guesses=[baseline=<value>][,mean=<value>][,height=<value>][,sigma=<value>]] \n\
[-fixValue=[baseline=<value>][,mean=<value>][,height=<value>][,sigma=<value>]] \n\
[-limits=[evaluations=<number>][,passes=<number>]]\n\n\
Does a fit to the form y = <baseline> + <height>*exp(-0.5*(x-<mean>)^2/<sigma>^2) \n\
Program by Michael Borland. (This is version 5, June 2000.)\n";

void report(double res, double *a, long pass, long n_eval, long n_dimen);
void setupOutputFile(SDDS_DATASET *OutputTable, long *xIndex, long *yIndex, long *syIndex, long *fitIndex, long *residualIndex,
                     long fullOutput, char *output, SDDS_DATASET *InputTable, char *xName, char *yName, char *syName);
long computeStartingPoint(double *a, double *da, double *x, double *y, long n,
                          unsigned long guessFlags, double sigmaGuess, double meanGuess, double baselineGuess, 
                          double heightGuess, double stepSize);
double fitFunction(double *a, long *invalid);
long makeFilteredCopy(double **xFit, double **yFit, double **syFit, 
                      double *x, double *y, double *sy, long n, 
                      double lower, double upper);

static double *xDataFit=NULL, *yDataFit=NULL, *syDataFit=NULL;
static long nDataFit = 0;

#define GUESS_BASELINE_GIVEN 0x0001
#define FIX_BASELINE_GIVEN (0x0001<<4)
#define GUESS_HEIGHT_GIVEN   0x0002
#define FIX_HEIGHT_GIVEN   (0x0002<<4)
#define GUESS_MEAN_GIVEN     0x0004
#define FIX_MEAN_GIVEN     (0x0004<<4)
#define GUESS_SIGMA_GIVEN    0x0008
#define FIX_SIGMA_GIVEN    (0x0008<<4)

#define BASELINE_INDEX 0
#define HEIGHT_INDEX 1
#define MEAN_INDEX 2
#define SIGMA_INDEX 3

int main(int argc, char **argv)
{
  double *xData=NULL, *yData=NULL, *syData=NULL;
  long nData = 0;
  SDDS_DATASET InputTable, OutputTable;
  SCANNED_ARG *s_arg;
  long i_arg;
  char *input, *output, *xName, *yName, *syName;
  long xIndex, yIndex, fitIndex, residualIndex, syIndex, retval;
  double *fitData, *residualData, rmsResidual, chiSqr, sigLevel;
  unsigned long guessFlags, dummyFlags, pipeFlags;
  double sigmaGuess, meanGuess, baselineGuess, heightGuess;
  double tolerance, stepSize;
  double a[4], da[4], lower, upper, result;
  double aLow[4], aHigh[4];
  short disable[4] = {0,0,0,0};
  int32_t nEvalMax=5000, nPassMax=100;
  long nEval, verbosity, fullOutput=0, i;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2 || argc>(2+N_OPTIONS))
    bomb(NULL, USAGE);

  for(i=0; i<4; i++) 
    aLow[i] = -(aHigh[i] = DBL_MAX);
  aLow[SIGMA_INDEX] = 0;
  
  input = output = NULL;
  stepSize = 1e-2;
  tolerance = 1e-8;
  verbosity = 0;
  guessFlags = sigmaGuess = heightGuess = baselineGuess = meanGuess = pipeFlags = 0;
  xName = yName = syName = NULL;
  lower = upper = 0;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_FITRANGE:
        if (s_arg[i_arg].n_items!=3 || 
            sscanf(s_arg[i_arg].list[1], "%lf", &lower)!=1 ||
            sscanf(s_arg[i_arg].list[2], "%lf", &upper)!=1 || lower>=upper)
          SDDS_Bomb("incorrect -fitRange syntax");
        break;
      case SET_TOLERANCE:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &tolerance)!=1 ||
            tolerance<=0)
          SDDS_Bomb("incorrect -tolerance syntax");
        break;
      case SET_STEPSIZE:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &stepSize)!=1 ||
            stepSize<=0)
          SDDS_Bomb("incorrect -stepSize syntax");
        break;
      case SET_VERBOSITY:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1)
          SDDS_Bomb("incorrect -verbosity syntax");
        break;
      case SET_GUESSES:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("incorrect -guess syntax");
        s_arg[i_arg].n_items -= 1;
        dummyFlags = guessFlags;
        if (!scanItemList(&guessFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "baseline", SDDS_DOUBLE, &baselineGuess, 1, GUESS_BASELINE_GIVEN,
                          "height", SDDS_DOUBLE, &heightGuess, 1, GUESS_HEIGHT_GIVEN,
                          "mean", SDDS_DOUBLE, &meanGuess, 1, GUESS_MEAN_GIVEN,
                          "sigma", SDDS_DOUBLE, &sigmaGuess, 1, GUESS_SIGMA_GIVEN,
                          NULL))
          SDDS_Bomb("invalid -guess syntax");
        if ((dummyFlags>>4)&guessFlags)
          SDDS_Bomb("can't have -fixedValue and -guess for the same item");
        guessFlags |= dummyFlags;
        break;
      case SET_FIXVALUE:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("incorrect -fixValue syntax");
        s_arg[i_arg].n_items -= 1;
        dummyFlags = guessFlags;
        if (!scanItemList(&guessFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "baseline", SDDS_DOUBLE, &baselineGuess, 1, FIX_BASELINE_GIVEN,
                          "height", SDDS_DOUBLE, &heightGuess, 1, FIX_HEIGHT_GIVEN,
                          "mean", SDDS_DOUBLE, &meanGuess, 1, FIX_MEAN_GIVEN,
                          "sigma", SDDS_DOUBLE, &sigmaGuess, 1, FIX_SIGMA_GIVEN,
                          NULL))
          SDDS_Bomb("invalid -fixValue syntax");
        if (dummyFlags&(guessFlags>>4))
          SDDS_Bomb("can't have -fixedValue and -guess for the same item");
        guessFlags |= dummyFlags;
        break;
      case SET_COLUMNS:
        if (s_arg[i_arg].n_items!=3 && s_arg[i_arg].n_items!=4)
          SDDS_Bomb("invalid -columns syntax");
        xName = s_arg[i_arg].list[1];
        yName = s_arg[i_arg].list[2];
        s_arg[i_arg].n_items -= 3;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+3, &s_arg[i_arg].n_items, 0,
                          "ysigma", SDDS_STRING, &syName, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -columns syntax");
        break;
      case SET_FULLOUTPUT:
        fullOutput = 1;
        break;
      case SET_LIMITS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("incorrect -limits syntax");
        s_arg[i_arg].n_items -= 1;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "evaluations", SDDS_LONG, &nEvalMax, 1, 0,
                          "passes", SDDS_LONG, &nPassMax, 1, 0,
                          NULL) ||
            nEvalMax<=0 || nPassMax<=0)
          SDDS_Bomb("invalid -limits syntax");
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      default:
        fprintf(stderr, "error: unknown/ambiguous option: %s\n", s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    }
    else {
      if (input==NULL) 
        input = s_arg[i_arg].list[0];
      else if (output==NULL)
        output = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames");
    }
  }

  processFilenames("sddsgfit", &input, &output, pipeFlags, 0, NULL);

  for (i=0; i<4; i++) {
    if ((guessFlags>>4)&(1<<i)) {
      disable[i] = 1;
    }
  }
    
  if (!xName || !yName)
    SDDS_Bomb("-columns option must be given");

  if (!SDDS_InitializeInput(&InputTable, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, xName, NULL) ||
      !SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, yName, NULL) ||
      (syName && !SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, syName, NULL)))
    SDDS_Bomb("one or more of the given data columns is nonexistent or nonnumeric");

  setupOutputFile(&OutputTable, &xIndex, &yIndex, &syIndex, &fitIndex, &residualIndex, fullOutput, output,
                  &InputTable, xName, yName, syName);


  while ((retval=SDDS_ReadPage(&InputTable))>0) {
    xData = yData = syData = NULL;
    fitData = residualData = NULL;
    if (!(xData = SDDS_GetColumnInDoubles(&InputTable, xName)) ||
        !(yData = SDDS_GetColumnInDoubles(&InputTable, yName)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (syName && !(syData = SDDS_GetColumnInDoubles(&InputTable, syName)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    if ((nData = SDDS_CountRowsOfInterest(&InputTable))<5)
      continue;

    if (lower<upper) {
      if ((nDataFit=makeFilteredCopy(&xDataFit, &yDataFit, &syDataFit, xData, yData, syData, nData, lower, upper))<5)
        continue;
    } else {
      xDataFit = xData;
      yDataFit = yData;
      syDataFit = syData;
      nDataFit = nData;
    }
    
    if (!computeStartingPoint(a, da, xDataFit, yDataFit, nDataFit, guessFlags, 
                              sigmaGuess, meanGuess, baselineGuess, heightGuess, stepSize)) {
      fprintf(stderr, "error: couldn't compute starting point for page %ld--skipping\n",
              retval);
      continue;
    }
    if (verbosity>2)
      fprintf(stderr, "starting values:  sigma=%.6e  mean=%.6e  base=%.6e  height=%.6e\n",
              a[SIGMA_INDEX], a[MEAN_INDEX], a[BASELINE_INDEX], a[HEIGHT_INDEX]);
    if (verbosity>3)
      fprintf(stderr, "starting steps:   sigma=%.6e  mean=%.6e  base=%.6e  height=%.6e\n",
              da[SIGMA_INDEX], da[MEAN_INDEX], da[BASELINE_INDEX], da[HEIGHT_INDEX]);

    nEval = simplexMin(&result, a, da, aLow, aHigh, disable, 4, -DBL_MAX, tolerance, fitFunction, 
                       (verbosity>0?report:NULL), nEvalMax, nPassMax, 12, 3, 1.0, 0);
    if (xData!=xDataFit) {
      free(xDataFit);
      free(yDataFit);
      if (syDataFit)
        free(syDataFit);
    }
    
    if (verbosity>3)
      fprintf(stderr, "%ld evaluations of fit function required, giving result %e\n", nEval, result);
    
    fitData = trealloc(fitData, sizeof(*fitData)*nData);
    residualData = trealloc(residualData, sizeof(*residualData)*nData);
    for (i=result=0; i<nData; i++) {
      fitData[i] = a[BASELINE_INDEX] + a[HEIGHT_INDEX]*exp(-ipow((xData[i]-a[MEAN_INDEX])/a[SIGMA_INDEX], 2)/2);
      residualData[i] = yData[i] - fitData[i];
      result += sqr(residualData[i]);
    }
    rmsResidual = sqrt(result/nData);
    if (syData) {
      for (i=chiSqr=0; i<nData; i++)
        chiSqr += sqr(residualData[i]/syData[i]);
    }
    else {
      double sy2;
      sy2 = result/(nData-4);
      for (i=chiSqr=0; i<nData; i++)
        chiSqr += sqr(residualData[i])/sy2;
    }
    sigLevel = ChiSqrSigLevel(chiSqr, nData-4);
    if (verbosity>0) {
      fprintf(stderr, "sigma: %.15e\nmean: %.15e\nbaseline: %.15e\nheight: %.15e\n",
              a[SIGMA_INDEX], a[MEAN_INDEX], a[BASELINE_INDEX], a[HEIGHT_INDEX]);
    }
    if (verbosity>1) {
      if (syData)
        fprintf(stderr, "Significance level: %.5e\n", sigLevel);
      fprintf(stderr, "RMS deviation: %.15e\n", rmsResidual);
    }

    if (!SDDS_StartPage(&OutputTable, nData) ||
        !SDDS_CopyParameters(&OutputTable, &InputTable) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, xData, nData, xIndex) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, fitData, nData, fitIndex) ||
        !SDDS_SetParameters(&OutputTable, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                            "gfitSigma", a[SIGMA_INDEX], "gfitMean", a[MEAN_INDEX],
                            "gfitBaseline", a[BASELINE_INDEX], "gfitHeight", a[HEIGHT_INDEX],
                            "gfitRmsResidual", rmsResidual, 
                            "gfitSigLevel", sigLevel, NULL) ||
        (fullOutput && 
         (!SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yData, nData, yIndex) ||
          !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, residualData, nData, residualIndex) ||
          (syName && !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, syData, nData, syIndex)))) ||
        !SDDS_WritePage(&OutputTable))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (xData) free(xData);
    if (yData) free(yData);
    if (syData) free(syData);
    if (fitData) free(fitData);
    if (residualData) free(residualData);
  }
  if (!SDDS_Terminate(&InputTable) || !SDDS_Terminate(&OutputTable)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}

void setupOutputFile(SDDS_DATASET *OutputTable, long *xIndex, long *yIndex, long *syIndex, long *fitIndex, long *residualIndex,
                     long fullOutput, char *output, SDDS_DATASET *InputTable, char *xName, char *yName, char *syName)
{
  char *name, *yUnits, *description, *xUnits;
  int32_t typeValue = SDDS_DOUBLE;
  static char *residualNamePart = "Residual";
  static char *residualDescriptionPart = "Residual of gaussian fit to ";

  if (!SDDS_InitializeOutput(OutputTable, SDDS_BINARY, 0, NULL, "sddsgfit output", output) ||
      !SDDS_TransferColumnDefinition(OutputTable, InputTable, xName, NULL) ||
      !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, xName) ||
      (*xIndex=SDDS_GetColumnIndex(OutputTable, xName))<0 ||
      !SDDS_GetColumnInformation(InputTable, "units", &xUnits, SDDS_BY_NAME, xName) ||
      !SDDS_GetColumnInformation(InputTable, "units", &yUnits, SDDS_BY_NAME, yName))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  name = tmalloc(sizeof(*name)*(strlen(yName)+strlen(residualNamePart)+1));
  description = tmalloc(sizeof(*name)*(strlen(yName)+strlen(residualDescriptionPart)+1));

  if (fullOutput) {
    if (!SDDS_TransferColumnDefinition(OutputTable, InputTable, yName, NULL) ||
        !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, yName) ||
        (*yIndex=SDDS_GetColumnIndex(OutputTable, yName))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (syName && 
        (!SDDS_TransferColumnDefinition(OutputTable, InputTable, syName, NULL) ||
         !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, syName) ||
         (*syIndex=SDDS_GetColumnIndex(OutputTable, syName))<0))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    sprintf(name, "%s%s", yName, residualNamePart);
    sprintf(description, "%s%s", yName, residualDescriptionPart);
    if ((*residualIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, description,
                                          NULL, SDDS_DOUBLE, 0))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  sprintf(name, "%sFit", yName);
  sprintf(description, "Gaussian fit to %s", yName);
  if ((*fitIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, description,
                                   NULL, SDDS_DOUBLE, 0))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (SDDS_DefineParameter(OutputTable, "gfitBaseline", NULL, yUnits, "Baseline from gaussian fit",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(OutputTable, "gfitHeight", NULL, yUnits, "Height from gaussian fit",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(OutputTable, "gfitMean", "$gm$r", xUnits, "Mean from gaussian fit",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(OutputTable, "gfitSigma", "$gs$r", xUnits, "Sigma from gaussian fit",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(OutputTable, "gfitRmsResidual", NULL, yUnits, "RMS residual from gaussian fit",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(OutputTable, "gfitSigLevel", NULL, NULL, "Significance level from chi-squared test",
                           NULL, SDDS_DOUBLE, 0)<0 ||
      !SDDS_TransferAllParameterDefinitions(OutputTable, InputTable, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(OutputTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
}


double fitFunction(double *a, long *invalid)
{
  double sum, tmp, mean, sigma, base, norm;
  long i;
  
  *invalid = 0;
  sigma = a[SIGMA_INDEX];
  mean  = a[MEAN_INDEX];
  base  = a[BASELINE_INDEX];
  norm  = a[HEIGHT_INDEX];

  if (!syDataFit) {
    for (i=sum=0; i<nDataFit; i++) {
      tmp = (xDataFit[i]-mean)/sigma;
      tmp = yDataFit[i] - base - norm*exp(-sqr(tmp)/2);
      sum += sqr(tmp);
    }
    return(sum/nDataFit);
  }
  else {
    for (i=sum=0; i<nDataFit; i++) {
      tmp = (xDataFit[i]-mean)/sigma;
      tmp = (yDataFit[i] - base - norm*exp(-sqr(tmp)/2))/syDataFit[i];
      sum += sqr(tmp);
    }
    return(sum/nDataFit);
  }
}

void report(double y, double *x, long pass, long nEval, long n_dimen)
{
  long i;

  fprintf(stderr, "pass %ld, after %ld evaluations: result = %.16e\na = ", pass, nEval, y);
  for (i=0; i<n_dimen; i++)
    fprintf(stderr, "%.8e ", x[i]);
  fputc('\n', stderr);
}

long computeStartingPoint(double *a, double *da, double *x, double *y, long n,
                          unsigned long guessFlags,
                          double sigmaGuess, double  meanGuess, double  baselineGuess, double  heightGuess,
                          double stepSize)
{    
  double xhalf, dhalf, ymin, ymax, xcenter, tmp, xmax, xmin;
  long i;
  
  if (n<5)
    return 0;

  /* first find maximum y value and corresponding x value, plus max x */
  xcenter = 0;
  ymax = xmax = -DBL_MAX;
  ymin = xmin =  DBL_MAX;
  for (i=0; i<n; i++) {
    if (xmax<(tmp=fabs(x[i])))
      xmax = tmp;
    if (xmin<tmp)
      xmin = tmp;
    if (ymax<y[i]) {
      ymax    = y[i];
      xcenter = x[i];
    }
    if (ymin>y[i])
      ymin = y[i];
  }

  /*  now find approximate half-max point */
  xhalf = 0;
  dhalf = DBL_MAX;
  for (i=0; i<n; i++) {
    tmp = fabs(fabs(y[i]-ymax)/(ymax-ymin)-.5);
    if (tmp<dhalf) {
      xhalf = x[i];
      dhalf = tmp;    
    }
  }
  if (dhalf!=DBL_MAX) 
    a[SIGMA_INDEX] = fabs(xhalf-xcenter)/1.177;    /* starting sigma */
  else
    a[SIGMA_INDEX] = xmax-xmin;
  a[MEAN_INDEX] = xcenter;                      /* starting mean */
  a[BASELINE_INDEX] = ymin;                         /* starting baseline */
  a[HEIGHT_INDEX] = ymax - ymin;                  /* starting height */

  if (guessFlags&(GUESS_SIGMA_GIVEN+FIX_SIGMA_GIVEN))
    a[SIGMA_INDEX] = sigmaGuess;
  if (guessFlags&(GUESS_MEAN_GIVEN+FIX_MEAN_GIVEN))
    a[MEAN_INDEX] = meanGuess;
  if (guessFlags&(GUESS_BASELINE_GIVEN+FIX_BASELINE_GIVEN))
    a[BASELINE_INDEX] = baselineGuess;
  if (guessFlags&(GUESS_HEIGHT_GIVEN+FIX_HEIGHT_GIVEN))
    a[HEIGHT_INDEX] = heightGuess;

  /* step sizes */
  for (i=0; i<4; i++)
    if (!(da[i] = a[i]*stepSize))
      da[i] = stepSize;

  return 1;
}

long makeFilteredCopy(double **xFit, double **yFit, double **syFit, 
                      double *x, double *y, double *sy, long n, 
                      double lower, double upper)
{
  long i, j;
  
  if (!(*xFit = (double*)malloc(sizeof(**xFit)*n)) ||
      !(*yFit = (double*)malloc(sizeof(**yFit)*n)) ||
      (sy && !(*syFit = (double*)malloc(sizeof(**syFit)*n))))
    return 0;
  
  for (i=j=0; i<n; i++) {
    if (x[i]<lower || x[i]>upper)
      continue;
    (*xFit)[j] = x[i];
    (*yFit)[j] = y[i];
    if (sy)
      (*syFit)[j] = sy[i];
    j++;
  }
  return j;
}

