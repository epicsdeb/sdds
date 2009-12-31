/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsgenericfit.c
 * purpose: perform fitting using a generic form supplied by the user
 * M. Borland, 2001
 $Log: sddsgenericfit.c,v $
 Revision 1.22  2009/07/30 15:34:50  shang
 made the lower, upper limit, step size and starting value of the variables to also be able to read from input file parameters.

 Revision 1.21  2007/12/04 21:03:57  borland
 Added -expression argument.

 Revision 1.20  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.19  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.18  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.17  2005/01/13 17:08:07  shang
 made the rpn function calls consistent with the updated rpn libary.

 Revision 1.16  2003/09/02 19:16:04  soliday
 Cleaned up code for Linux.

 Revision 1.15  2003/06/02 21:39:13  soliday
 Added the function to add parentheses around a string before calling if2pf

 Revision 1.14  2003/01/21 18:57:47  borland
 Added yet another argument for simplexMin().

 Revision 1.13  2003/01/16 18:45:31  soliday
 Changed because simplexMin arguments changed.

 Revision 1.12  2002/08/14 17:12:46  soliday
 Added Open License

 Revision 1.11  2001/08/08 19:21:19  borland
 Made consistent with changes to simplexMin() (addition of n_divisions argument).

 Revision 1.10  2001/05/15 16:41:26  borland
 sddsgenericfit now supports the "no1dscans" option of simplexMin() from
 the commandline.  Others were modified to accomodate the new option,
 but don't allow commandline specification.

 Revision 1.9  2001/05/07 18:54:08  borland
 Added 'heat' parameter to -variable option to allow injecting noise into
 values between restarts.

 Revision 1.8  2001/05/03 21:29:19  soliday
 Split usage message because of limits on WIN32.

 Revision 1.7  2001/05/01 15:49:05  borland
 Uses original step sizes for restarts.
 Uses values in input parameters (in input file) for starting values,
 if input parameters are present.

 Revision 1.6  2001/04/10 20:42:29  soliday
 Added the ability to convert algebraic equations to rpn equations.

 Revision 1.5  2001/04/06 15:41:15  borland
 Fixed problems with parsing -variable arguments.
 Made input parameter values accessible in equations.

 Revision 1.4  2001/04/03 18:03:16  borland
 Added -startFromPrevious option.

 Revision 1.3  2001/03/23 23:29:22  emery
 Added newline to usage message.
 Louis

 Revision 1.2  2001/03/20 15:34:18  borland
 Fixed a bug (use of an uninitialized variable).

 Revision 1.1  2001/03/20 15:16:32  borland
 First version.

 *
 */
#if defined(LINUX)
#include <sys/stat.h>
#endif

#include "mdb.h"
#include "SDDS.h"
#include "SDDSaps.h"
#include "scan.h"
#include "rpn.h"
#include <time.h>
 
#define SET_VARIABLE 0
#define SET_PIPE 1
#define SET_EQUATION 2
#define SET_COLUMNS 3
#define SET_TARGET 4
#define SET_TOLERANCE 5
#define SET_SIMPLEX 6
#define SET_VERBOSITY 7
#define SET_STARTFROMPREVIOUS 8
#define SET_EXPRESSION 9
#define N_OPTIONS 10

char *option[N_OPTIONS] = {
  "variable", "pipe", "equation", "columns", "target", "tolerance", "simplex",
  "verbosity", "startfromprevious", "expression",
    };

static char *USAGE1="sddsgenericfit [-pipe=[input][,output]] [<inputfile>] [<outputfile>] \n\
-columns=<x-name>,<y-name>[,ySigma=<sy-name>] \n\
-equation=<rpnString>[,algebraic] [-expression=<string> [-expression=...]]\n\
[-target=<value>] [-tolerance=<value>]\n\
[-simplex=[restarts=<nRestarts>][,cycles=<nCycles>,][evaluations=<nEvals>][,no1DScans]\n\
-variable=name=<name>,lowerLimit=<value|parameter-name>,upperLimit=<value|parametr-name>,stepsize=<value|parameter-name>,startingValue=<value|parametername>[,units=<string>][,heat=<value|parameter-name>]\n\
[-variable=...] [-verbosity=<integer>] [-startFromPrevious]\n\
Uses Simplex method to finds a fit to <y-name> as a function of <x-name> by varying the given\n\
variables, which are assumed to appear in the <rpnString>.\n\n\
-columns    Give the names of the independent and dependent data.  If given, the data in <sy-name>\n\
            is used to weight the fit.\n\
-equation   Give an rpn expression for the equation to use in fitting.\n\
            This equation can use the names of any of the columns or parameters in the file,\n\
            just as in sddsprocess.  It is expected to return a value that will be compared to\n\
            the data in column <y-name>.\n\
-expression Give an rpn expression to evaluate before the equation is evaluated.  Can be used \n\
            to prepare quantities on the stack or in variables when the equation is complicated.\n\
-target     Give the value of the (weighted) rms residual that is acceptably small to consider the\n\
            fit \"good\".\n";
static char *USAGE2="-tolerance Give the minimum change in the (weighted) rms residual that is considered significant\n\
            enough to justify continuing optimization.\n\
-simplex    Give parameters of the simplex optimization. Each start or restart allows <nCycles> cycles\n\
            with up to <nEvals> evaluations of the function.  Defaults are 10 restarts, 10 cycles, and\n\
            5000 evaluations.\n\
-variable   Give the name, lower limit, upper limit, step size, and starting value for a parameter\n\
            of the fitting equation.  This name must not match the name of an existing column or\n\
            parameter in the input file.  The value will appear as a parameter in the output file.\n\
            If given, the 'heat' value specifies the gaussian parameter of random numbers to be\n\
            added to the best results of each restart.  This can help avoid getting stuck in\n\
            a local minimum.\n\
-verbosity  A higher value results in more output during optimization.\n\
-startFromPrevious\n\
            When starting fits for a new page, the fit parameters are normally set equal to the\n\
            starting values supplied with the -variable option.  However, if this option is given,\n\
            then the final values from the previous page's fit are used as the starting values for\n\
            fitting.\n\
Program by Michael Borland. (This is version 1, May 2001.)\n";

void report(double res, double *a, long pass, long n_eval, long n_dimen);
void setupOutputFile(SDDS_DATASET *OutputTable, long *xIndex, long *yIndex, long *syIndex,
                     long *fitIndex, long *residualIndex,
                     char *output, SDDS_DATASET *InputTable, char *xName, char *yName,
                     char *syName, char **variableName, char **variableUnits, long variables);
double fitFunction(double *a, long *invalid);

static SDDS_DATASET InputTable;
static double *xData=NULL, *yData=NULL, *syData=NULL, *yFit=NULL, *yResidual=NULL;
static long nData = 0;
static char *equation;
static long *variableMem, nVariables, verbosity;
static char **expression = NULL;
static long nExpressions = 0;

#define VARNAME_GIVEN  0x0001U
#define LOWER_GIVEN    0x0002U
#define UPPER_GIVEN    0x0004U
#define STEP_GIVEN     0x0008U
#define START_GIVEN    0x0010U
#define VARUNITS_GIVEN 0x0020U

int main(int argc, char **argv)
{
  long i;
  SDDS_DATASET OutputTable;
  SCANNED_ARG *s_arg;
  long i_arg;
  char *input, *output, *xName, *yName, *syName;
  long xIndex, yIndex, fitIndex, residualIndex, syIndex, retval;
  double rmsResidual, chiSqr, sigLevel;
  unsigned long pipeFlags, dummyFlags;
  int32_t nEvalMax=5000, nPassMax=10, nRestartMax=10;
  long nEval, iRestart;
  double tolerance, target;
  char **variableName, **variableUnits;
  double *lowerLimit, *upperLimit, *stepSize, *startingValue, *paramValue, *paramDelta=NULL, *paramDelta0=NULL;
  char **startingPar=NULL, **lowerLimitPar=NULL, **upperLimitPar=NULL, **heatPar=NULL, **stepPar=NULL;
  double *heat, *bestParamValue, bestResult=0;
  long iVariable, startFromPrevious=0;
  double result, lastResult=0;
  char pfix[IFPF_BUF_SIZE];
  unsigned long simplexFlags = 0;
  char *ptr;
  char * rpn_defns;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);

  if (argc<=1) {
    fprintf(stderr, "%s%s", USAGE1, USAGE2);
    exit(1);
    /* bomb(USAGE, NULL);*/
  }

  input = output = equation = NULL;
  variableName = variableUnits = NULL;
  lowerLimit = upperLimit = stepSize = startingValue = heat = bestParamValue = NULL; 
  nVariables = 0;
  pipeFlags = 0;
  verbosity = startFromPrevious = 0;
  xName = yName = syName = NULL;
  tolerance = target = 1e-14;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_TOLERANCE:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &tolerance)!=1 ||
            tolerance<=0)
          SDDS_Bomb("incorrect -tolerance syntax");
        break;
      case SET_TARGET:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &target)!=1 ||
            target<=0)
          SDDS_Bomb("incorrect -target syntax");
        break;
      case SET_VERBOSITY:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1)
          SDDS_Bomb("incorrect -verbosity syntax");
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
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_VARIABLE:
        if (!(variableName=SDDS_Realloc(variableName, sizeof(*variableName)*(nVariables+1))) ||
            !(lowerLimit=SDDS_Realloc(lowerLimit, sizeof(*lowerLimit)*(nVariables+1))) ||
            !(upperLimit=SDDS_Realloc(upperLimit, sizeof(*upperLimit)*(nVariables+1))) ||
            !(stepSize=SDDS_Realloc(stepSize, sizeof(*stepSize)*(nVariables+1))) ||
            !(heat=SDDS_Realloc(heat, sizeof(*heat)*(nVariables+1))) ||
            !(bestParamValue=SDDS_Realloc(bestParamValue, sizeof(*bestParamValue)*(nVariables+1))) ||
            !(startingValue=SDDS_Realloc(startingValue, sizeof(*startingValue)*(nVariables+1))) ||
            !(startingPar=SDDS_Realloc(startingPar, sizeof(*startingPar)*(nVariables+1))) ||
            !(lowerLimitPar=SDDS_Realloc(lowerLimitPar, sizeof(*lowerLimitPar)*(nVariables+1))) ||
            !(upperLimitPar=SDDS_Realloc(upperLimitPar, sizeof(*upperLimitPar)*(nVariables+1))) ||
            !(heatPar=SDDS_Realloc(heatPar, sizeof(*heatPar)*(nVariables+1))) ||
            !(stepPar=SDDS_Realloc(stepPar, sizeof(*stepPar)*(nVariables+1))) ||
            !(variableUnits=SDDS_Realloc(variableUnits, sizeof(*variableUnits)*(nVariables+1))))
          SDDS_Bomb("memory allocation failure");
        variableUnits[nVariables] = NULL;
        heat[nVariables] = 0;
        startingPar[nVariables] = heatPar[nVariables] = lowerLimitPar[nVariables] = upperLimitPar[nVariables] = stepPar[nVariables] = NULL;
        if ((s_arg[i_arg].n_items-=1)<5 ||
            !scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "name", SDDS_STRING, variableName+nVariables, 1, VARNAME_GIVEN,
                          "lowerlimit", SDDS_STRING, lowerLimitPar+nVariables, 1, LOWER_GIVEN,
                          "upperlimit", SDDS_STRING, upperLimitPar+nVariables, 1, UPPER_GIVEN,
                          "stepsize", SDDS_STRING, &(stepPar[nVariables]), 1, STEP_GIVEN,
                          "startingvalue", SDDS_STRING, startingPar+nVariables, 1, START_GIVEN,
                          "heat", SDDS_STRING, heatPar+nVariables, 1, 0,
                          "units", SDDS_STRING, variableUnits+nVariables, 1, VARUNITS_GIVEN,
                          NULL))
          SDDS_Bomb("invalid -variable syntax or values");
        if (startingPar[nVariables] && sscanf(startingPar[nVariables], "%lf", startingValue+nVariables)==1) {
          free(startingPar[nVariables]); startingPar[nVariables] = NULL;
        }
        if (lowerLimitPar[nVariables] &&  sscanf(lowerLimitPar[nVariables], "%lf", lowerLimit+nVariables)==1) {
          free(lowerLimitPar[nVariables]); lowerLimitPar[nVariables] = NULL;
        }
        if (upperLimitPar[nVariables] &&  sscanf(upperLimitPar[nVariables], "%lf", upperLimit+nVariables)==1) {
          free(upperLimitPar[nVariables]); upperLimitPar[nVariables] = NULL;
        }
        if (heatPar[nVariables] &&  sscanf(heatPar[nVariables], "%lf", heat+nVariables)==1) {
          free(heatPar[nVariables]); heatPar[nVariables] = NULL;
        }
        if (stepPar[nVariables] &&  sscanf(stepPar[nVariables], "%lf", stepSize+nVariables)==1) {
          free(stepPar[nVariables]); stepPar[nVariables] = NULL;
        }
        if ((dummyFlags&(VARNAME_GIVEN|LOWER_GIVEN|UPPER_GIVEN|STEP_GIVEN|START_GIVEN))!=
            (VARNAME_GIVEN|LOWER_GIVEN|UPPER_GIVEN|STEP_GIVEN|START_GIVEN))
          SDDS_Bomb("insufficient information given for -variable");
        if (!strlen(variableName[nVariables]))
          SDDS_Bomb("invalid blank variable name");
        if (!lowerLimitPar[nVariables] && !upperLimitPar[nVariables] &&lowerLimit[nVariables]>=upperLimit[nVariables])
          SDDS_Bomb("invalid limits value for variable");
        
        if (!lowerLimitPar[nVariables] && !upperLimitPar[nVariables] && !startingPar[nVariables] &&
            (startingValue[nVariables]<=lowerLimit[nVariables] || startingValue[nVariables]>=upperLimit[nVariables]))
          SDDS_Bomb("invalid limits or starting value for variable");
        if (!stepPar[nVariables] && stepSize[nVariables]<=0)
          SDDS_Bomb("invalid step size for variable");
        nVariables ++;
        break;
      case SET_SIMPLEX:
        s_arg[i_arg].n_items -= 1;
        if (!scanItemList(&simplexFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "restarts", SDDS_LONG, &nRestartMax, 1, 0,
                          "cycles", SDDS_LONG, &nPassMax, 1, 0,
                          "evaluations", SDDS_LONG, &nEvalMax, 1, 0,
                          "no1dscans", -1, NULL, 0, SIMPLEX_NO_1D_SCANS,
                          NULL) || nRestartMax<0 || nPassMax<=0 || nEvalMax<=0) 
          SDDS_Bomb("invalid -simplex syntax/values");
        break;
      case SET_EQUATION:
	/*
        if (s_arg[i_arg].n_items!=2 || !strlen(equation=s_arg[i_arg].list[1])) 
          SDDS_Bomb("invalid -equation syntax");
	*/
	if ((s_arg[i_arg].n_items<2) || (s_arg[i_arg].n_items>3))
	  SDDS_Bomb("invalid -equation syntax");
	if (s_arg[i_arg].n_items==2) {
	  if (!strlen(equation=s_arg[i_arg].list[1])) {
	    SDDS_Bomb("invalid -equation syntax");
	  }
	} else if (s_arg[i_arg].n_items==3) {
	  if (strncmp(s_arg[i_arg].list[2], "algebraic", strlen(s_arg[i_arg].list[2]))==0) {
	    ptr = addOuterParentheses(s_arg[i_arg].list[1]);
	    if2pf(pfix, ptr, sizeof pfix);
	    free(ptr);
	    if (!SDDS_CopyString(&equation, pfix)) {
	      fprintf(stderr, "error: problem copying argument string\n");
	      return(1);
	    }
	  } else {
	    SDDS_Bomb("invalid -equation syntax");
	  }
	}
        break;
      case SET_EXPRESSION:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -expression syntax");
        expression = trealloc(expression, sizeof(*expression)*(nExpressions+1));
        expression[nExpressions++] = s_arg[i_arg].list[1];
        break;
      case SET_STARTFROMPREVIOUS:
        startFromPrevious = 1;
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

  processFilenames("sddsgenericfit", &input, &output, pipeFlags, 0, NULL);

  if (!xName || !yName)
    SDDS_Bomb("-columns option must be given");
  if (nVariables==0)
    SDDS_Bomb("you must give at least one -variables option");
  if (equation==NULL)
    SDDS_Bomb("you must give an equation string");

  rpn_defns=getenv("RPN_DEFNS");
  /*if failed, check where default setting exists for a linux system, G. Shen, Dec 31, 2009 */
  if(!rpn_defns) {
#if defined(LINUX)
      if (!(stat(rpn_default, &sts) == -1 && errno == ENOENT)) { /* check whether default file exists */
	  rpn_defns = rpn_default;
      }
#endif
  }
  rpn(rpn_defns);
  /*rpn(getenv("RPN_DEFNS"));*/
  if (rpn_check_error()) exit(1);

  if (!SDDS_InitializeInput(&InputTable, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, xName, NULL) ||
      !SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, yName, NULL) ||
      (syName && !SDDS_FindColumn(&InputTable, FIND_NUMERIC_TYPE, syName, NULL)))
    SDDS_Bomb("one or more of the given data columns is nonexistent or nonnumeric");

  setupOutputFile(&OutputTable, &xIndex, &yIndex, &syIndex, &fitIndex, &residualIndex, output,
                  &InputTable, xName, yName, syName, variableName, variableUnits, nVariables);

  if (!(paramValue = SDDS_Malloc(sizeof(*paramValue)*nVariables)) ||
      !(paramDelta = SDDS_Malloc(sizeof(*paramDelta)*nVariables)) ||
      !(paramDelta0 = SDDS_Malloc(sizeof(*paramDelta0)*nVariables)) ||
      !(variableMem = SDDS_Malloc(sizeof(*variableMem)*nVariables)))
    SDDS_Bomb("memory allocation failure");
  for (iVariable=0; iVariable<nVariables; iVariable++)
    variableMem[iVariable] = rpn_create_mem(variableName[iVariable], 0);

  while ((retval=SDDS_ReadPage(&InputTable))>0) {
    if (!(xData = SDDS_GetColumnInDoubles(&InputTable, xName)) ||
        !(yData = SDDS_GetColumnInDoubles(&InputTable, yName)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (syName && !(syData = SDDS_GetColumnInDoubles(&InputTable, syName)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    if ((nData = SDDS_CountRowsOfInterest(&InputTable))<=nVariables)
      continue;

    for (iVariable=0; iVariable<nVariables; iVariable++) {
      if (startingPar[iVariable] && !SDDS_GetParameterAsDouble(&InputTable, startingPar[iVariable], &startingValue[iVariable]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (lowerLimitPar[iVariable] && !SDDS_GetParameterAsDouble(&InputTable, lowerLimitPar[iVariable], &lowerLimit[iVariable]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (upperLimitPar[iVariable] && !SDDS_GetParameterAsDouble(&InputTable, upperLimitPar[iVariable], &upperLimit[iVariable]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (heatPar[iVariable] && !SDDS_GetParameterAsDouble(&InputTable, heatPar[iVariable], &heat[iVariable]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (stepPar[iVariable] && !SDDS_GetParameterAsDouble(&InputTable, stepPar[iVariable], &stepSize[iVariable]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (SDDS_GetParameterIndex(&InputTable, variableName[iVariable])>=0) {
        if (!SDDS_GetParameterAsDouble(&InputTable, variableName[iVariable],
                                       &paramValue[iVariable])) 
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      else if (retval==1 || !startFromPrevious)
        paramValue[iVariable] = startingValue[iVariable];
      paramDelta[iVariable] = stepSize[iVariable];
      
    }
    
    if (verbosity>2) {
      /* show starting values */
      fprintf(stderr, "Starting values and step sizes:\n");
      for (iVariable=0; iVariable<nVariables; iVariable++)
        fprintf(stderr, "%s = %le  %le\n", variableName[iVariable], paramValue[iVariable],
                paramDelta[iVariable]);
    }

    if (!(yFit = SDDS_Realloc(yFit, sizeof(*yFit)*nData)) ||
        !(yResidual = SDDS_Realloc(yResidual, sizeof(*yResidual)*nData)))
      SDDS_Bomb("memory allocation failure");

    SDDS_StoreParametersInRpnMemories(&InputTable);
    memcpy(paramDelta0, paramDelta, sizeof(*paramDelta)*nVariables);
    for (iRestart=nEval=0; iRestart<=nRestartMax; iRestart++) {
      memcpy(paramDelta, paramDelta0, sizeof(*paramDelta)*nVariables);
      if (iRestart) {
        if (iRestart==1)
          random_1(-fabs(2*((long)time((time_t)0)/2)+1));
        for (iVariable=0; iVariable<nVariables; iVariable++) {
          paramValue[iVariable] += gauss_rn_lim(0.0, heat[iVariable], 2, random_1);
          if (paramValue[iVariable]<lowerLimit[iVariable])
            paramValue[iVariable] = lowerLimit[iVariable]+paramDelta[iVariable];
          if (paramValue[iVariable]>upperLimit[iVariable])
            paramValue[iVariable] = upperLimit[iVariable]-paramDelta[iVariable];
        }
      }
      nEval += simplexMin(&result, paramValue, paramDelta, lowerLimit, upperLimit,
                         NULL, nVariables, target, tolerance, fitFunction, 
                         (verbosity>0?report:NULL), nEvalMax, nPassMax, 12, 3, 1.0, simplexFlags);
      if (iRestart!=0 && result>bestResult) {
        result = bestResult;
        for (iVariable=0; iVariable<nVariables; iVariable++)
          paramValue[iVariable] = bestParamValue[iVariable];
      }
      else {
        bestResult = result;
        for (iVariable=0; iVariable<nVariables; iVariable++)
          bestParamValue[iVariable] = paramValue[iVariable];
      }
      if (verbosity>0)
        fprintf(stderr, "Result of simplex minimization: %le\n", result);
      if (result<target ||
          (iRestart!=0 && fabs(lastResult-result)<tolerance))
        break;
      lastResult = result;
      if (verbosity>0)
        fprintf(stderr, "Performing restart %ld\n", iRestart+1);
    }
    
    for (iVariable=0; iVariable<nVariables; iVariable++)
      paramValue[iVariable] = bestParamValue[iVariable];
    result = fitFunction(paramValue, &i);

    if (verbosity>3)
      fprintf(stderr, "%ld evaluations of fit function required, giving result %e\n", nEval, result);
    for (i=result=0; i<nData; i++) 
      result += sqr(yResidual[i]);
    rmsResidual = sqrt(result/nData);
    if (syData) {
      for (i=chiSqr=0; i<nData; i++)
        chiSqr += sqr(yResidual[i]/syData[i]);
    }
    else {
      double sy2;
      sy2 = result/(nData-nVariables);
      for (i=chiSqr=0; i<nData; i++)
        chiSqr += sqr(yResidual[i])/sy2;
    }
    sigLevel = ChiSqrSigLevel(chiSqr, nData-nVariables);
    if (verbosity>1) {
      if (syData)
        fprintf(stderr, "Significance level: %.5e\n", sigLevel);
      fprintf(stderr, "RMS deviation: %.15e\n", rmsResidual);
    }

    if (!SDDS_StartPage(&OutputTable, nData) ||
        !SDDS_CopyParameters(&OutputTable, &InputTable) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, xData, nData, xIndex) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yFit, nData, fitIndex) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yData, nData, yIndex) ||
        !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, yResidual, nData, residualIndex) ||
        (syName && !SDDS_SetColumn(&OutputTable, SDDS_SET_BY_INDEX, syData, nData, syIndex)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (iVariable=0; iVariable<nVariables; iVariable++)
      if (!SDDS_SetParameters(&OutputTable, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                              variableName[iVariable], paramValue[iVariable],
                              NULL))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_WritePage(&OutputTable))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    free(xData);
    free(yData);
    if (syData) 
      free(syData);
  }
  return 0;
}

void setupOutputFile(SDDS_DATASET *OutputTable, long *xIndex, long *yIndex, long *syIndex, long *fitIndex, 
                     long *residualIndex,
                     char *output, SDDS_DATASET *InputTable, 
                     char *xName, char *yName, char *syName, 
                     char **variableName, char **variableUnits, long variables
                     )
{
  char *name, *yUnits, *xUnits;
  int32_t typeValue = SDDS_DOUBLE;
  static char *residualNamePart = "Residual";
  long i;
  
  if (!SDDS_InitializeOutput(OutputTable, SDDS_BINARY, 0, NULL, "sddsgfit output", output) ||
      !SDDS_TransferColumnDefinition(OutputTable, InputTable, xName, NULL) ||
      !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, xName) ||
      (*xIndex=SDDS_GetColumnIndex(OutputTable, xName))<0 ||
      !SDDS_GetColumnInformation(InputTable, "units", &xUnits, SDDS_BY_NAME, xName) ||
      !SDDS_GetColumnInformation(InputTable, "units", &yUnits, SDDS_BY_NAME, yName))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_TransferColumnDefinition(OutputTable, InputTable, yName, NULL) ||
      !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, yName) ||
      (*yIndex=SDDS_GetColumnIndex(OutputTable, yName))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (syName && 
      (!SDDS_TransferColumnDefinition(OutputTable, InputTable, syName, NULL) ||
       !SDDS_ChangeColumnInformation(OutputTable, "type", &typeValue, SDDS_BY_NAME, syName) ||
       (*syIndex=SDDS_GetColumnIndex(OutputTable, syName))<0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  name = tmalloc(sizeof(*name)*(strlen(yName)+strlen(residualNamePart)+1));
  sprintf(name, "%s%s", yName, residualNamePart);
  if ((*residualIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, NULL,
                                        NULL, SDDS_DOUBLE, 0))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  sprintf(name, "%sFit", yName);
  if ((*fitIndex=SDDS_DefineColumn(OutputTable, name, NULL, yUnits, NULL,
                                   NULL, SDDS_DOUBLE, 0))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free(name);

  for (i=0; i<variables; i++) {
    if (SDDS_DefineParameter(OutputTable, variableName[i], NULL, variableUnits[i], NULL,
                             NULL, SDDS_DOUBLE, 0)<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }

  if (!SDDS_TransferAllParameterDefinitions(OutputTable, InputTable, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(OutputTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}


double fitFunction(double *a, long *invalid)
{
  double sum, tmp;
  long i, j;
  
  rpn_clear();
  *invalid = 0;

  for (i=0; i<nVariables; i++)
    rpn_store(a[i], NULL, variableMem[i]);

  if (verbosity>10)
    fprintf(stderr, "Running fit function:\n");
  if (!syData) {
    for (i=sum=0; i<nData; i++) {
      if (!SDDS_StoreRowInRpnMemories(&InputTable, i))
        SDDS_Bomb("problem storing data in RPN memories");
      rpn_clear();
      for (j=0; j<nExpressions; j++)
        rpn(expression[j]);
      yFit[i] = rpn(equation);
      if (rpn_check_error()) {
        *invalid = 1;
        return 0;
      }
      yResidual[i] = tmp = (yFit[i]-yData[i]);
      if (verbosity>10) {
        fprintf(stderr, "%le %le %le\n", xData[i], yData[i], yFit[i]);
      }
      sum += sqr(tmp);
    }
    return(sum/nData);
  }
  else {
    for (i=sum=0; i<nData; i++) {
      if (!SDDS_StoreRowInRpnMemories(&InputTable, i))
        SDDS_Bomb("problem storing data in RPN memories");
      rpn_clear();
      for (j=0; j<nExpressions; j++)
        rpn(expression[j]);
      yFit[i] = rpn(equation);
      if (rpn_check_error()) {
        *invalid = 1;
        return 0;
      }
      yResidual[i] = tmp = (yFit[i]-yData[i]);      
      sum += sqr(tmp/syData[i]);
    }
    return(sum/nData);
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

