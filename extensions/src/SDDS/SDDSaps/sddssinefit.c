/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssinefit.c
 * purpose: do  sinusoidal fit:  y(n) = a0 + a1*sin(2*PI*a2*x(n)+a3)
 * M. Borland, 1995
 $Log: sddssinefit.c,v $
 Revision 1.22  2005/11/04 22:46:18  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.21  2003/07/22 21:05:38  soliday
 IEEE functions are always used.

 Revision 1.20  2003/01/21 18:57:47  borland
 Added yet another argument for simplexMin().

 Revision 1.19  2003/01/16 18:50:17  soliday
 Changed simplexMin because the arguments changed.

 Revision 1.18  2002/08/14 17:12:55  soliday
 Added Open License

 Revision 1.17  2001/08/08 19:21:18  borland
 Made consistent with changes to simplexMin() (addition of n_divisions argument).

 Revision 1.16  2001/05/30 23:03:30  emery
 Added flag argument to call to simplexMin for compatibility with
 arguments of simplexMin in simplex.c

 Revision 1.15  2001/01/10 19:35:47  soliday
 Standardized usage message.

 Revision 1.14  2000/10/11 21:50:25  soliday
 Changed definition of isinf so that sunmath is no longer needed.

 Revision 1.13  2000/08/10 21:17:00  soliday
 Added support for gcc on Solaris.

 Revision 1.12  2000/04/13 18:05:08  soliday
 Added missing include statement.

 Revision 1.11  1999/07/09 15:10:14  soliday
 Added definition for isnan on WIN32

 Revision 1.10  1999/05/25 19:15:40  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/08 15:44:51  borland
 Made some further adjustments that *may* improve fitting.

 Revision 1.8  1999/01/07 22:25:16  borland
 Brought up-to-date with latest version of simplexMin().  Made some improvements
 to sddssinefit but it still doesn't do well.

 Revision 1.7  1999/01/06 19:54:56  borland
 Fixed the version number in the usage message.

 Revision 1.6  1998/08/26 14:47:53  borland
 Treatment of IEEE math function isinf is now uniform.  If on solaris
 and sunmath is missing, then modify mdb.h and Makefile.Unix.

 * Revision 1.5  1996/02/14  01:05:37  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1995/10/05  17:43:19  borland
 * Added conditional compilation around isnan() and isinf() calls.
 *
 * Revision 1.3  1995/09/06  14:57:10  saunders
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
#define SET_LIMITS 6
#define SET_PIPE 7
#define N_OPTIONS 8

char *option[N_OPTIONS] = {
    "tolerance", "verbosity", "clue", "guess", "columns", "fulloutput",
    "limits", "pipe",
    };

static char *USAGE="sddssinefit [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
[-fulloutput] [-columns=<x-name>,<y-name>] [-tolerance=<value>]\n\
[-limits=[evaluations=<number>][,passes=<number>]] [-verbosity=<integer>]\n\
[-guess=[constant=<constant>][,factor=<factor>][,frequency=<freq>][,phase=<phase>]] \n\n\
Does a fit to the form y = <constant> + <factor>*sin(2*PI*<freq>*x+<phase>)\n\n\
Program by Michael Borland. (This is version 3, August 1998.)\n";

static double *xData, *yData;
static long nData;
static double yMin, yMax, xMin, xMax;
static double fit[4];

double fitFunction(double *a, long *invalid);
void report(double res, double *a, long pass, long n_eval, long n_dimen);
void setupOutputFile(SDDS_DATASET *OutputTable, 
                     int32_t *xIndex, int32_t *yIndex, int32_t *fitIndex, int32_t *residualIndex,
                     char *output, long fullOutput, SDDS_DATASET *InputTable, char *xName, char *yName);
char *makeInverseUnits(char *units);

long verbosity;

#define GUESS_CONSTANT_GIVEN 0x0001
#define GUESS_FACTOR_GIVEN   0x0002
#define GUESS_FREQ_GIVEN     0x0004
#define GUESS_PHASE_GIVEN    0x0008

int main(int argc, char **argv)
{
    double tolerance, result;
    long nEval;
    int32_t nEvalMax = 5000, nPassMax = 25;
    double a[4], da[4];
    double alo[4], ahi[4];
    long n_dimen = 4, zeroes;
    SDDS_DATASET InputTable, OutputTable;
    SCANNED_ARG *s_arg;
    long i_arg, i, clue, fullOutput;
    char *input, *output, *xName, *yName;
    int32_t xIndex, yIndex, fitIndex, residualIndex;
    long retval;
    double *fitData, *residualData, rmsResidual;
    unsigned long guessFlags, dummyFlags, pipeFlags;
    double constantGuess, factorGuess, freqGuess, phaseGuess;
    double firstZero, lastZero;
    unsigned long simplexFlags = 0;
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2 || argc>(2+N_OPTIONS))
        bomb(NULL, USAGE);

    input = output = NULL;
    tolerance = 1e-6;
    verbosity = fullOutput = 0;
    clue = -1;
    xName = yName = NULL;
    guessFlags = 0;
    pipeFlags = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_TOLERANCE:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &tolerance)!=1)
                    SDDS_Bomb("incorrect -tolerance syntax");
                break;
              case SET_VERBOSITY:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1)
                    SDDS_Bomb("incorrect -verbosity syntax");
                break;
              case SET_GUESS:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("incorrect -guess syntax");
                s_arg[i_arg].n_items -= 1;
                if (!scanItemList(&guessFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                                    "constant", SDDS_DOUBLE, &constantGuess, 1, GUESS_CONSTANT_GIVEN,
                                    "factor", SDDS_DOUBLE, &factorGuess, 1, GUESS_FACTOR_GIVEN,
                                    "frequency", SDDS_DOUBLE, &freqGuess, 1, GUESS_FREQ_GIVEN,
                                    "phase", SDDS_DOUBLE, &phaseGuess, 1, GUESS_PHASE_GIVEN,
                                    NULL))
                    SDDS_Bomb("invalid -guess syntax");
                break;
              case SET_COLUMNS:
                if (s_arg[i_arg].n_items!=3)
                    SDDS_Bomb("invalid -columns syntax");
                xName = s_arg[i_arg].list[1];
                yName = s_arg[i_arg].list[2];
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

    processFilenames("sddssinefit", &input, &output, pipeFlags, 0, NULL);

    if (!xName || !yName)
        SDDS_Bomb("-columns option must be given");

    if (!SDDS_InitializeInput(&InputTable, input) ||
        SDDS_GetColumnIndex(&InputTable, xName)<0 || SDDS_GetColumnIndex(&InputTable, yName)<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    setupOutputFile(&OutputTable, &xIndex, &yIndex, &fitIndex, &residualIndex, output, fullOutput,
                    &InputTable, xName, yName);

    fitData = residualData = NULL;

    alo[0] = - (ahi[0] = DBL_MAX);
    alo[1] = alo[2] = 0;
    ahi[1] = ahi[2] = DBL_MAX;
    alo[3] = - (ahi[3] = PIx2);
    firstZero = lastZero = 0;
    while ((retval=SDDS_ReadPage(&InputTable))>0) {
        if (!(xData = SDDS_GetColumnInDoubles(&InputTable, xName)) ||
            !(yData = SDDS_GetColumnInDoubles(&InputTable, yName)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if ((nData = SDDS_CountRowsOfInterest(&InputTable))<4)
            continue;

        find_min_max(&yMin, &yMax, yData, nData);
        find_min_max(&xMin, &xMax, xData, nData);
        zeroes = 0;
        for (i=1; i<nData; i++)
          if (yData[i]*yData[i-1]<=0) {
            i++;
            if (!zeroes)
              firstZero = (xData[i]+xData[i-1])/2;
            else 
              lastZero = (xData[i]+xData[i-1])/2;
            zeroes ++;
          }
        a[0] = (yMin+yMax)/2;
        a[1] = (yMax-yMin)/2;
        if (!zeroes)
            a[2] = 2/fabs(xMax-xMin);
        else
            a[2] = zeroes/(2*fabs(lastZero-firstZero));
        a[3] = 0;

        if (guessFlags&GUESS_CONSTANT_GIVEN)
            a[0] = constantGuess;
        if (guessFlags&GUESS_FACTOR_GIVEN)
            a[1] = factorGuess;
        if (guessFlags&GUESS_FREQ_GIVEN)
            a[2] = freqGuess;
        if (guessFlags&GUESS_PHASE_GIVEN)
            a[3] = phaseGuess;

        alo[1] = a[1]/2;
        if (!(da[0] = a[0]*0.1))
          da[0] = 0.01;
        if (!(da[1] = a[1]*0.1))
          da[1] = 0.01;
        da[2] = a[2]*0.25;
        da[3] = 0.01;

        nEval = simplexMin(&result, a, da, alo, ahi, NULL, n_dimen, -DBL_MAX, tolerance, fitFunction, 
                            (verbosity>0?report:NULL), nEvalMax, nPassMax,
                           12, 3, 1.0, simplexFlags);
        
        fitData = trealloc(fitData, sizeof(*fitData)*nData);
        residualData = trealloc(residualData, sizeof(*residualData)*nData);
        for (i=result=0; i<nData; i++)
            result += sqr(residualData[i] = yData[i]-(fitData[i]=a[0]+a[1]*sin(PIx2*a[2]*xData[i]+a[3])));
        rmsResidual = sqrt(result/nData);
        if (verbosity>1) {
            fprintf(stderr, "RMS deviation: %.15e\n", rmsResidual);
            fprintf(stderr, "(RMS deviation)/(largest value): %.15e\n", rmsResidual/MAX(fabs(yMin), fabs(yMax)));
            }
        if (verbosity>0) {
            fprintf(stderr, "coefficients of fit to the form y = a0 + a1*sin(2*PI*a2*x+a3), a = \n");
            for (i=0; i<4; i++)
                fprintf(stderr, "%.8e ", a[i]);
            fprintf(stderr, "\n");
            }

        if (!SDDS_StartPage(&OutputTable, nData) ||
            !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, xData, nData, xIndex) ||
            !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, fitData, nData, fitIndex) ||
            !SDDS_SetParameters(&OutputTable, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                                "sinefitConstant", a[0], "sinefitFactor", a[1], "sinefitFrequency", a[2], 
                                "sinefitPhase", a[3], "sinefitRmsResidual", rmsResidual, NULL) ||
            (fullOutput && 
             (!SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yData, nData, yIndex) ||
              !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, residualData, nData, residualIndex) )) ||
            !SDDS_WritePage(&OutputTable))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    return 0;
    }

void setupOutputFile(SDDS_DATASET *OutputTable, 
                     int32_t *xIndex, int32_t *yIndex, int32_t *fitIndex, int32_t *residualIndex,
                     char *output, long fullOutput, SDDS_DATASET *InputTable, char *xName, char *yName)
{
    char *name, *yUnits, *description, *xUnits, *inverse_xUnits;
    int32_t typeValue = SDDS_DOUBLE;
    static char *residualNamePart = "Residual";
    static char *residualDescriptionPart = "Residual of sinusoidal fit to ";

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
    sprintf(description, "Sinusoidal fit to %s", yName);
    if ((*fitIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, description,
                                                   NULL, SDDS_DOUBLE, 0))<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    inverse_xUnits = makeInverseUnits(xUnits);

    if (SDDS_DefineParameter(OutputTable, "sinefitConstant", NULL, yUnits, "Constant term from sinusoidal fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "sinefitFactor", NULL, yUnits, "Factor from sinusoidal fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "sinefitFrequency", NULL, inverse_xUnits, "Frequency from sinusoidal fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "sinefitPhase", NULL, xUnits, "Phase from sinusoidal fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineParameter(OutputTable, "sinefitRmsResidual", NULL, yUnits, "rms residual from sinusoidal fit",
                              NULL, SDDS_DOUBLE, 0)<0 ||
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
    double chi;
    static double min_chi;

    min_chi = DBL_MAX;

    *invalid = 0;
    for (i=chi=0; i<nData; i++)
        chi += sqr(yData[i] - (a[0] + a[1]*sin(PIx2*a[2]*xData[i]+a[3])));
    if (isnan(chi) || isinf(chi))
        *invalid = 1;
    if (verbosity>3)
        fprintf(stderr, "trial: a = %e, %e, %e, %e  --> chi = %e, invalid = %ld\n", 
               a[0], a[1], a[2], a[3], chi, *invalid);
    if (min_chi>chi) {
        min_chi = chi;
        fit[0] = a[0];
        fit[1] = a[1];
        fit[2] = a[2];
        fit[3] = a[3];
        if (verbosity>2)
            fprintf(stderr, "new best chi = %e:  a = %e, %e, %e, %e\n", chi, fit[0], fit[1], fit[2], fit[3]);
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

