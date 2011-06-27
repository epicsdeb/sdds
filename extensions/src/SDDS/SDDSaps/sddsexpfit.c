/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsexpfit.c
 * purpose: do an exponential fit:  y(n) = a0 + a1*exp(a2*x(n))
 * M. Borland, 1995
 $Log: sddsexpfit.c,v $
 Revision 1.33  2010/04/12 22:54:06  lemery
 Untabified.

 Revision 1.32  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.31  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.30  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.29  2003/10/10 19:03:48  borland
 Added -autoOffset option, which results in suppression of the minimum value
 of the independent variable.  This gives fit results that are often more
 useful than non-offset results.

 Revision 1.28  2003/07/22 21:05:38  soliday
 IEEE functions are always used.

 Revision 1.27  2003/01/21 18:57:46  borland
 Added yet another argument for simplexMin().

 Revision 1.26  2003/01/16 18:42:52  soliday
 Changed because the arguments to simplexMin changed.

 Revision 1.25  2002/08/14 17:12:45  soliday
 Added Open License

 Revision 1.24  2002/01/16 18:12:05  soliday
 Added missing SDDS_Terminate and free functions.

 Revision 1.23  2001/08/08 19:21:19  borland
 Made consistent with changes to simplexMin() (addition of n_divisions argument).

 Revision 1.22  2001/05/15 16:41:26  borland
 sddsgenericfit now supports the "no1dscans" option of simplexMin() from
 the commandline.  Others were modified to accomodate the new option,
 but don't allow commandline specification.

 Revision 1.21  2001/01/10 19:35:37  soliday
 Standardized usage message.

 Revision 1.20  2000/10/11 21:50:24  soliday
 Changed definition of isinf so that sunmath is no longer needed.

 Revision 1.19  2000/08/10 21:17:00  soliday
 Added support for gcc on Solaris.

 Revision 1.18  2000/07/13 20:26:54  borland
 Updated version numbers.

 Revision 1.17  2000/07/13 20:24:37  borland
 Per D. Blachowicz: added -fixValues and -startValues options.

 Revision 1.16  2000/04/13 18:02:28  soliday
 Added missing include statement.

 Revision 1.15  2000/03/10 08:44:35  emery
 Changed the expression for the increment of the exponential factor
 to a smaller value. The previous expression made the simplex minimization
 instable because the delta value could be very large compared to the initial
 guess of the exponential factor.

 Revision 1.14  2000/02/24 17:42:16  borland
 Initial guess for exponential rate is now a smaller value.

 Revision 1.13  1999/10/23 15:45:28  borland
 Replaced default variable range for minimization with [-DBL_MAX/2, DBL_MAX/2],
 to prevent floating point exception on Linux.

 Revision 1.12  1999/07/09 15:06:38  soliday
 Added definition for isnan for WIN32

 Revision 1.11  1999/05/25 19:09:57  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/01/07 22:25:13  borland
 Brought up-to-date with latest version of simplexMin().  Made some improvements
 to sddssinefit but it still doesn't do well.

 Revision 1.9  1999/01/06 19:54:43  borland
 Fixed the version number in the usage message.

 Revision 1.8  1998/12/16 21:26:01  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.7  1998/11/13 22:39:51  borland
 Now copies through parameters from the input to the output.

 Revision 1.6  1998/08/26 14:47:53  borland
 Treatment of IEEE math function isinf is now uniform.  If on solaris
 and sunmath is missing, then modify mdb.h and Makefile.Unix.

 * Revision 1.5  1996/02/14  01:05:19  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1995/10/05  17:43:16  borland
 * Added conditional compilation around isnan() and isinf() calls.
 *
 * Revision 1.3  1995/09/06  14:56:26  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#if defined(_WIN32)
#define isnan(x) _isnan(x)
#endif

#define SET_TOLERANCE 0
#define SET_VERBOSITY 1
#define SET_CLUE 2
#define SET_GUESS 3
#define SET_COLUMNS 4
#define SET_FULLOUTPUT 5
#define SET_PIPE 6
#define SET_LIMITS 7
#define SET_STARTVALUES 8
#define SET_FIXVALUE 9
#define SET_AUTOOFFSET 10
#define N_OPTIONS 11

char *option[N_OPTIONS] = {
    "tolerance", "verbosity", "clue", "guess", "columns", "fulloutput", "pipe",
    "limits", "startvalues", "fixvalue", "autooffset",
    };

static char *USAGE="sddsexpfit [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-fulloutput] [-columns=<x-name>,<y-name>[,ySigma=<name>]] [-tolerance=<value>] \n\
[-verbosity=<integer>] [-clue={grows|decays}] [-guess=<constant>,<factor>,<rate>] \n\
[-startValues=[constant=<value>][,factor=<value>][,rate=<value>]] \n\
[-fixValue=[constant=<value>][,factor=<value>][,rate=<value>]] \n\
[-autoOffset] [-limits=[evaluations=<number>][,passes=<number>]\n\n\
Does a fit to the form y = <constant> + <factor>*exp(<rate>*x)\n\n\
Program by Michael Borland. (This is version 5, October 2003, M. Borland)\n";

static double *xData, *yData, *syData;
static long nData;
static double yMin, yMax, xMin, xMax;
static double fit[3];

double fitFunction(double *a, long *invalid);
void report(double res, double *a, long pass, long n_eval, long n_dimen);
void setupOutputFile(SDDS_DATASET *OutputTable, 
                     long *xIndex, long *yIndex, long *fitIndex, long *residualIndex,
                     char *output, long fullOutput, SDDS_DATASET *InputTable, char *xName, char *yName);
char *makeInverseUnits(char *units);

#define START_CONSTANT_GIVEN 0x0001
#define FIX_CONSTANT_GIVEN (0x0001<<3)
#define START_FACTOR_GIVEN 0x0002
#define FIX_FACTOR_GIVEN (0x0002<<3)
#define START_RATE_GIVEN 0x0004
#define FIX_RATE_GIVEN (0x0004<<3)

#define CLUE_GROWS 0
#define CLUE_DECAYS 1
#define N_CLUE_TYPES 2
char *clue_name[N_CLUE_TYPES] = {
    "grows", "decays"
     } ;

long verbosity;

int main(int argc, char **argv)
{
    double tolerance, result, chiSqr, sigLevel;
    long nEval;
    int32_t nEvalMax = 5000, nPassMax = 100;
    double guess[3];
    double a[3], da[3];
    double alo[3], ahi[3];
    long n_dimen = 3, guessGiven, startGiven;
    SDDS_DATASET InputTable, OutputTable;
    SCANNED_ARG *s_arg;
    long i_arg, i, clue, fullOutput;
    char *input, *output, *xName, *yName, *syName;
    long xIndex, yIndex, fitIndex, residualIndex, retval;
    double *fitData, *residualData, rmsResidual;
    unsigned long guessFlags, pipeFlags, dummyFlags;
    double constantStart, factorStart, rateStart;
    short disable[3] = {0,0,0};
    short autoOffset = 0;
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2 || argc>(2+N_OPTIONS))
        bomb(NULL, USAGE);

    input = output = NULL;
    tolerance = 1e-6;
    verbosity = fullOutput = guessGiven = startGiven = 0;
    clue = -1;
    xName = yName = syName = NULL;
    pipeFlags = guessFlags = 0;
    constantStart = factorStart = rateStart = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
            case SET_AUTOOFFSET:
              autoOffset = 1;
              break;
              case SET_TOLERANCE:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &tolerance)!=1)
                    SDDS_Bomb("incorrect -tolerance syntax");
                break;
              case SET_VERBOSITY:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1)
                    SDDS_Bomb("incorrect -verbosity syntax");
                break;
              case SET_CLUE:
                if (s_arg[i_arg].n_items!=2 || 
                    (clue=match_string(s_arg[i_arg].list[1], clue_name, N_CLUE_TYPES, 0))<0)
                    SDDS_Bomb("incorrect -clue syntax");
                break;
              case SET_GUESS:
	        if (startGiven)
		    SDDS_Bomb("can't have -startValues and -guess at once");
                if (s_arg[i_arg].n_items!=4 ||
                    sscanf(s_arg[i_arg].list[1], "%lf", guess+0)!=1 ||
                    sscanf(s_arg[i_arg].list[2], "%lf", guess+1)!=1 ||
                    sscanf(s_arg[i_arg].list[3], "%lf", guess+2)!=1 )
                    SDDS_Bomb("invalid -guess syntax");
                guessGiven = 1;
                break;
	      case SET_STARTVALUES:
	        if (s_arg[i_arg].n_items<2)
		    SDDS_Bomb("incorrect -startValues syntax");
	        if (guessGiven)
		    SDDS_Bomb("can't have -startValues and -guess at once");
	        s_arg[i_arg].n_items -= 1;
		dummyFlags = guessFlags;
	        if (!scanItemList(&guessFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "constant", SDDS_DOUBLE, &constantStart, 1, START_CONSTANT_GIVEN,
                          "factor", SDDS_DOUBLE, &factorStart, 1, START_FACTOR_GIVEN,
                          "rate", SDDS_DOUBLE, &rateStart, 1, START_RATE_GIVEN,
                          NULL))
		     SDDS_Bomb("invalid -fixValue syntax");
		if ((dummyFlags>>3)&(guessFlags))
		  SDDS_Bomb("can't have -fixValue and -startValue for the same item");
		guessFlags |= dummyFlags;
		startGiven = 1;
		break;
	      case SET_FIXVALUE:
		if (s_arg[i_arg].n_items<2)
		  SDDS_Bomb("incorrect -fixValue syntax");
		s_arg[i_arg].n_items -= 1;
		dummyFlags = guessFlags;
		if (!scanItemList(&guessFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "constant", SDDS_DOUBLE, &constantStart, 1, FIX_CONSTANT_GIVEN,
                          "factor", SDDS_DOUBLE, &factorStart, 1, FIX_FACTOR_GIVEN,
                          "rate", SDDS_DOUBLE, &rateStart, 1, FIX_RATE_GIVEN,
                          NULL))
		  SDDS_Bomb("invalid -fixValue syntax");
		if ((dummyFlags)&(guessFlags>>3))
		  SDDS_Bomb("can't have -fixValue and -startValue for the same item");
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
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
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

    processFilenames("sddsexpfit", &input, &output, pipeFlags, 0, NULL);

    for (i=0; i<3; i++) {
      if ((guessFlags>>3)&(1<<i)) {
        disable[i] = 1;
      }
    }

    if (!xName || !yName)
        SDDS_Bomb("-columns option must be given");

    if (!SDDS_InitializeInput(&InputTable, input) ||
        SDDS_GetColumnIndex(&InputTable, xName)<0 || SDDS_GetColumnIndex(&InputTable, yName)<0 ||
        (syName && SDDS_GetColumnIndex(&InputTable, syName)<0))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    setupOutputFile(&OutputTable, &xIndex, &yIndex, &fitIndex, &residualIndex, output, fullOutput,
                    &InputTable, xName, yName);

    while ((retval=SDDS_ReadPage(&InputTable))>0) {
      fitData = residualData = NULL;
      xData = yData = syData = NULL;
      if (!(xData = SDDS_GetColumnInDoubles(&InputTable, xName)) ||
          !(yData = SDDS_GetColumnInDoubles(&InputTable, yName)) ||
          (syName && !(syData = SDDS_GetColumnInDoubles(&InputTable, syName))))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if ((nData = SDDS_CountRowsOfInterest(&InputTable))<4)
        continue;

      if (xData[0]>xData[nData-1])
        fputs("warning: data reverse-ordered\7\7", stderr);
      
      find_min_max(&yMin, &yMax, yData, nData);
      find_min_max(&xMin, &xMax, xData, nData);
      for (i=0; i<nData; i++)
        xData[i] -= xMin;

      fill_double_array(alo, 3, -DBL_MAX/2);
      fill_double_array(ahi, 3,  DBL_MAX/2);

      if (!guessGiven) {
        if (clue==CLUE_GROWS) {
          a[0] = 0.9*yData[0];
          a[1] = yData[nData-1] - yData[0];
          a[2] = 1/(xData[nData-1]-xData[0]);
          alo[2] = 0;
          if (a[1]>0)
            alo[1] = 0;
          else 
            ahi[1] = 0;
        }
        else if (clue==CLUE_DECAYS) {
          a[0] = 0.9*yData[nData-1];
          a[1] = yData[0] - yData[nData-1];
          a[2] = 0;
          ahi[2] = 0;
          if (a[1]>0)
            alo[1] = 0;
          else 
            ahi[1] = 0;
        }
        else {
          a[0] = yMin*0.9;
          a[1] = yMax-yMin;
          a[2] = 0;
        }
      }
      else {
        a[0] = guess[0];
        a[1] = guess[1];
        a[2] = guess[2];
      }

      if (guessFlags&(START_CONSTANT_GIVEN+FIX_CONSTANT_GIVEN))
        a[0] = constantStart;
      if (guessFlags&(START_FACTOR_GIVEN+FIX_FACTOR_GIVEN))
        a[1] = factorStart;
      if (guessFlags&(START_RATE_GIVEN+FIX_RATE_GIVEN))
        a[2] = rateStart;

      da[0] = a[0]*0.05;
      da[1] = a[1]*0.05;
      da[2] = 0.1/(xData[nData-1]-xData[0]);
      if (verbosity>3) 
        fprintf(stderr, "starting guess: %e, %e, %e\n", 
                a[0], a[1], a[2]);
      
      nEval = simplexMin(&result, a, da, alo, ahi, disable, n_dimen, -DBL_MAX, tolerance, fitFunction, 
                         (verbosity>0?report:NULL), nEvalMax, nPassMax, 12, 3, 1.0, 0);
      
      da[0] = a[0]/10;
      da[1] = a[1]/10;
      da[2] = a[2]/10;
      nEval = simplexMin(&result, a, da, alo, ahi, disable, n_dimen, -DBL_MAX, tolerance, fitFunction, 
                         (verbosity>0?report:NULL), nEvalMax, nPassMax, 12, 3, 1.0, 0);
      
      if (!autoOffset) {
        /* user wants the coefficients with the offset removed */
        a[1] *= exp(-a[2]*xMin);
        for (i=0; i<nData; i++)
          xData[i] += xMin;
      }
      
      fitData = trealloc(fitData, sizeof(*fitData)*nData);
      residualData = trealloc(residualData, sizeof(*residualData)*nData);
      for (i=result=0; i<nData; i++)
        result += sqr(residualData[i] = yData[i]-(fitData[i]=a[0]+a[1]*exp(a[2]*xData[i])));
      rmsResidual = sqrt(result/nData);
      if (syData) {
        for (i=chiSqr=0; i<nData; i++)
          chiSqr += sqr(residualData[i]/syData[i]);
      }
      else {
        double sy2;
        sy2 = result/(nData-3);
        for (i=chiSqr=0; i<nData; i++)
          chiSqr += sqr(residualData[i])/sy2;
      }
      sigLevel = ChiSqrSigLevel(chiSqr, nData-3);
      if (verbosity>1) {
        fprintf(stderr, "RMS deviation: %.15e\n", rmsResidual);
        fprintf(stderr, "(RMS deviation)/(largest value): %.15e\n", rmsResidual/MAX(fabs(yMin), fabs(yMax)));
        if (syData)
          fprintf(stderr, "Significance level: %.5e\n", sigLevel);
      }
      if (verbosity>0) {
        fprintf(stderr, "coefficients of fit to the form y = a0 + a1*exp(a2*x), a = \n");
        for (i=0; i<3; i++)
          fprintf(stderr, "%.8e ", a[i]);
        fprintf(stderr, "\n");
      }

      if (!SDDS_StartPage(&OutputTable, nData) ||
          !SDDS_CopyParameters(&OutputTable, &InputTable) ||
          !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, xData, nData, xIndex) ||
          !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, fitData, nData, fitIndex) ||
          !SDDS_SetParameters(&OutputTable, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                              "expfitConstant", a[0], "expfitFactor", a[1], "expfitRate", a[2], 
                              "expfitRmsResidual", rmsResidual, 
                              "expfitSigLevel", sigLevel, NULL) ||
          (fullOutput && 
           (!SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yData, nData, yIndex) ||
            !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, residualData, nData, residualIndex) )) ||
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

void setupOutputFile(SDDS_DATASET *OutputTable, 
                     long *xIndex, long *yIndex, long *fitIndex, long *residualIndex,
                     char *output, long fullOutput, SDDS_DATASET *InputTable, char *xName, char *yName)
{
    char *name, *yUnits, *description, *xUnits, *inverse_xUnits;
    int32_t typeValue = SDDS_DOUBLE;
    static char *residualNamePart = "Residual";
    static char *residualDescriptionPart = "Residual of exponential fit to ";

    if (!SDDS_InitializeOutput(OutputTable, SDDS_BINARY, 0, NULL, "sddsexpfit output", output) ||
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
        sprintf(name, "%s%s", yName, residualNamePart);
        sprintf(description, "%s%s", yName, residualDescriptionPart);
        if ((*residualIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, description,
                                                       NULL, SDDS_DOUBLE, 0))<0)
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
                          
    sprintf(name, "%sFit", yName);
    sprintf(description, "Exponential fit to %s", yName);
    if ((*fitIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, description,
                                                   NULL, SDDS_DOUBLE, 0))<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    inverse_xUnits = makeInverseUnits(xUnits);

    if (SDDS_DefineParameter(OutputTable, "expfitConstant", NULL, yUnits, "Constant term from exponential fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "expfitFactor", NULL, yUnits, "Factor from exponential fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "expfitRate", NULL, inverse_xUnits, "Rate from exponential fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "expfitRmsResidual", NULL, yUnits, "rms residual from exponential fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "expfitSigLevel", NULL, NULL, "Significance level from chi-squared test",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        !SDDS_TransferAllParameterDefinitions(OutputTable, InputTable, SDDS_TRANSFER_KEEPOLD) ||
        !SDDS_WriteLayout(OutputTable))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    }

char *makeInverseUnits(char *units)
{
    char *inverseUnits;

    if (!units || SDDS_StringIsBlank(units))
        return NULL;
    inverseUnits = tmalloc(sizeof(*inverseUnits)*(strlen(units)+5));
    
    if (strncmp(units, "1/(", 3)==0 && units[strlen(units)-1]==')') {
        /* special case of "1/(<unit>)" */
        strcpy(inverseUnits, units+3);
        inverseUnits[strlen(inverseUnits)-1] = 0;
        }
    else if (!strchr(units, ' '))
        /* special case of units string without spaces */
        sprintf(inverseUnits, "1/%s", units);
    else
        /* general case */
        sprintf(inverseUnits, "1/(%s)", units);

    return inverseUnits;
    }


double fitFunction(double *a, long *invalid)
{
    long i;
    double chi, diff;
    static double min_chi;

    min_chi = DBL_MAX;

    *invalid = 0;
    for (i=chi=0; i<nData; i++) {
        diff = yData[i] - (a[0] + a[1]*exp(a[2]*xData[i]));
        if (syData)
            diff /= syData[i];
        chi += sqr(diff);
        }
    if (isnan(chi) || isinf(chi))
        *invalid = 1;
    if (verbosity>3)
        fprintf(stderr, "trial: a = %e, %e, %e  --> chi = %e, invalid = %ld\n", 
               a[0], a[1], a[2], chi, *invalid);
    if (min_chi>chi) {
        min_chi = chi;
        fit[0] = a[0];
        fit[1] = a[1];
        fit[2] = a[2];
        if (verbosity>2)
            fprintf(stderr, "new best chi = %e:  a = %e, %e, %e\n", chi, fit[0], fit[1], fit[2]);
        }
    return(chi);
    }

void report(double y, double *x, long pass, long nEval, long n_dimen)
{
    long i;

    fprintf(stderr, "pass %ld, after %ld evaluations: result = %.16e\na = ", pass, nEval, y);
    for (i=0; i<n_dimen; i++)
        fprintf(stderr, "%.8e ", x[i]);
    fputc('\n', stderr);
    }

