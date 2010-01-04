/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsderiv
 * purpose: differentiate a data set
 * 
 * Michael Borland, 1995
 * based on mpl-format program deriv and sddsinteg
 $Log: sddsderiv.c,v $
 Revision 1.19  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.18  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.17  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.16  2002/10/26 20:22:30  borland
 Fixed bug in preparation of symbols and column names when higher-order
 derivatives were requested.

 Revision 1.15  2002/09/24 14:11:03  soliday
 Changed the %y to %%y

 Revision 1.14  2002/08/14 17:12:43  soliday
 Added Open License

 Revision 1.13  2002/06/19 23:13:01  borland
 Fixed spelling of Savitzky-Golay.

 Revision 1.12  2002/01/16 17:35:23  soliday
 Added SDDS_Terminate and free functions.

 Revision 1.11  2001/01/10 19:35:35  soliday
 Standardized usage message.

 Revision 1.10  1999/06/18 18:47:54  borland
 Added optional use of Savitzky-Golay filters for doing derivatives.

 Revision 1.9  1999/05/25 19:08:02  soliday
 Removed compiler warning on linux.

 Revision 1.8  1999/04/22 13:30:25  borland
 Now does a better job of dealing with endpoints.  Specifically, the x
 values for the endpoint derivatives are correct.

 Revision 1.7  1999/01/06 19:54:42  borland
 Fixed the version number in the usage message.

 Revision 1.6  1998/12/16 21:26:00  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.5  1998/11/13 22:39:50  borland
 Now copies through parameters from the input to the output.

 * Revision 1.4  1996/02/14  01:05:18  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:56:20  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include "scan.h"
#include <ctype.h>

static char *USAGE = "sddsderiv [<input>] [<output>]\n\
[-pipe=[input][,output]]\n\
-differentiate=<column-name>[,<sigma-name>]...\n\
[-exclude=<column-name>[,...]] -versus=<column-name>\n\
[-interval=<integer>] [-SavitzkyGolay=<left>,<right>,<fitOrder>[,<derivOrder>]] \n\
[-mainTemplates=<item>=<string>[,...]] \n\
[-errorTemplates=<item>=<string>[,...]] \n\n\
The -templates <item> may be \"name\", \"symbol\" or \"description\".\n\
The default main name, description, and symbol templates are \"%yNameDeriv\",\n\
\"Derivative w.r.t %xSymbol of %ySymbol\", and \"d[%ySymbol]/d[%xSymbol]\", respectively.\n\
The default error name, description, and symbol templates are \"%yNameDerivSigma\",\n\
\"Sigma of derivative w.r.t %xSymbol of %ySymbol\", and \"Sigma[d[%ySymbol]/d[%xSymbol]]\", respectively.\n\
Program by Michael Borland.  (This is version 3, December 1998.)" ;

#define CLO_DIFFERENTIATE 0
#define CLO_VERSUS 1
#define CLO_INTERVAL 2
#define CLO_MAINTEMPLATE 3
#define CLO_ERRORTEMPLATE 4
#define CLO_PIPE 5
#define CLO_EXCLUDE 6
#define CLO_SAVITZKYGOLAY 7
#define CLO_OPTIONS 8
static char *option[CLO_OPTIONS] = {
    "differentiate", "versus", "interval", "maintemplate", "errortemplate",
    "pipe", "exclude", "savitzkygolay",
    };

long checkErrorNames(char **yErrorName, long nDerivatives);
void makeSubstitutions(char *buffer1, char *buffer2, char *template, char *nameRoot, char *symbolRoot, 
                       char *xName, char *xSymbol);
char *changeInformation(SDDS_DATASET *SDDSout, char *name, char *nameRoot, char *symbolRoot, char *xName,
                       char *xSymbol, char **template, char *newUnits);
long setupOutputFile(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *output,
                     char ***yOutputName, char ***yOutputErrorName, char ***yOutputUnits,
                     char *xName, char *xErrorName,
                     char **yName, char **yErrorName, long yNames,
                     char **mainTemplate, char **errorTemplate, int32_t interval, long order);
long findDerivIndices(long *i1, long *i2, long interval, long i, long rows);
void takeDerivative(double *x, double *y, double *sy, long rows, double *deriv, double *derivSigma, 
double *derivPosition, long interval);
void takeSGDerivative(double *x, double *y, long rows, 
                    double *deriv, double *derivPosition, 
                    long left, long right, long sgOrder, long derivOrder);

int main(int argc, char **argv)
{
    double *xData, *yData, *xError, *yError, *derivative, *derivativeError, *derivativePosition;
    char *input, *output, *xName, *xErrorName, **yName, **yErrorName, **yOutputName, **yOutputErrorName, *ptr;
    char **yOutputUnits, **yExcludeName;
    char *mainTemplate[3] = {NULL, NULL, NULL};
    char *errorTemplate[3] = {NULL, NULL, NULL};
    long i, iArg, yNames, yExcludeNames, rows;
    int32_t interval;
    SDDS_DATASET SDDSin, SDDSout;
    SCANNED_ARG *scanned;
    unsigned long flags, pipeFlags;
    long SGLeft, SGRight, SGOrder, SGDerivOrder, intervalGiven, yErrorsSeen;

    SDDS_RegisterProgramName(argv[0]);

    argc = scanargs(&scanned, argc, argv); 
    if (argc==1)
        bomb(NULL, USAGE);

    input = output = xName = xErrorName = NULL;
    yName = yErrorName = yExcludeName = NULL;
    derivative = derivativeError = derivativePosition = yError = yData = xData = xError = NULL;
    yNames = yExcludeNames = 0;
    pipeFlags = 0;
    interval = 2;
    SGOrder = -1;
    SGDerivOrder = 1;
    intervalGiven = 0;
    yErrorsSeen = 0;
    
    for (iArg=1; iArg<argc; iArg++) {
        if (scanned[iArg].arg_type==OPTION) {
            /* process options here */
            switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
              case CLO_DIFFERENTIATE:
                if (scanned[iArg].n_items!=2 && scanned[iArg].n_items!=3)
                    SDDS_Bomb("invalid -differentiate syntax");
                yName = SDDS_Realloc(yName, sizeof(*yName)*(yNames+1));
                yErrorName = SDDS_Realloc(yErrorName, sizeof(*yErrorName)*(yNames+1));
                yName[yNames] = scanned[iArg].list[1];
                if (scanned[iArg].n_items==3) {
                    yErrorsSeen = 1;
                    yErrorName[yNames] = scanned[iArg].list[2];
                } else
                    yErrorName[yNames] = NULL;
                yNames++;
                break;
              case CLO_EXCLUDE:
                if (scanned[iArg].n_items<2)
                    SDDS_Bomb("invalid -exclude syntax");
                moveToStringArray(&yExcludeName, &yExcludeNames, scanned[iArg].list+1, scanned[iArg].n_items-1);
                break;
              case CLO_VERSUS:
                if (xName)
                    SDDS_Bomb("give -versus only once");
                if (scanned[iArg].n_items!=2)
                    SDDS_Bomb("invalid -versus syntax");
                xName = scanned[iArg].list[1];
                xErrorName = NULL;
                break;
              case CLO_MAINTEMPLATE:
                if (scanned[iArg].n_items<2)
                    SDDS_Bomb("invalid -mainTemplate syntax");
                scanned[iArg].n_items--;
                if (!scanItemList(&flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                                    "name", SDDS_STRING, mainTemplate+0, 1, 0,
                                    "description", SDDS_STRING, mainTemplate+1, 1, 0,
                                    "symbol", SDDS_STRING, mainTemplate+2, 1, 0,
                                    NULL))
                    SDDS_Bomb("invalid -mainTemplate syntax");
                break;
              case CLO_ERRORTEMPLATE:
                if (scanned[iArg].n_items<2)
                    SDDS_Bomb("invalid -errorTemplate syntax");
                scanned[iArg].n_items--;
                if (!scanItemList(&flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                                    "name", SDDS_STRING, errorTemplate+0, 1, 0,
                                    "description", SDDS_STRING, errorTemplate+1, 1, 0,
                                    "symbol", SDDS_STRING, errorTemplate+2, 1, 0,
                                    NULL))
                    SDDS_Bomb("invalid -errorTemplate syntax");
                break;
              case CLO_PIPE:
                if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_INTERVAL:
                if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%" SCNd32, &interval)!=1 ||
                    interval<=0)
                    SDDS_Bomb("invalid -interval syntax/value");
                intervalGiven = 1;
                break;
              case CLO_SAVITZKYGOLAY:
                if ((scanned[iArg].n_items!=4 && scanned[iArg].n_items!=5) ||
                    sscanf(scanned[iArg].list[1], "%ld", &SGLeft)!=1 ||
                    sscanf(scanned[iArg].list[2], "%ld", &SGRight)!=1 ||
                    sscanf(scanned[iArg].list[3], "%ld", &SGOrder)!=1 ||
                    (scanned[iArg].n_items==5 &&
                     sscanf(scanned[iArg].list[4], "%ld", &SGDerivOrder)!=1) ||
                    SGLeft<0 || SGRight<0 || (SGLeft+SGRight)<SGOrder ||
                    SGOrder<0 || SGDerivOrder<0)
                  SDDS_Bomb("invalid -SavitzkyGolay syntax/values");
                break;
              default:
                fprintf(stderr, "invalid option seen: %s\n", scanned[iArg].list[0]);
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
                SDDS_Bomb("too many filenames");
            }
        }

    if (intervalGiven && SGOrder>=0)
      SDDS_Bomb("-interval and -SavitzkyGolay options are incompatible");
    if (SGOrder>=0 && (xErrorName || yErrorsSeen))
      SDDS_Bomb("Savitzky-Golay method does not support errors in data");
    
    processFilenames("sddsderiv", &input, &output, pipeFlags, 0, NULL);

    if (!yNames)
        SDDS_Bomb("-differentiate option must be given at least once");
    if (!checkErrorNames(yErrorName, yNames))
        SDDS_Bomb("either all -differentiate quantities must have errors, or none");

    if (!SDDS_InitializeInput(&SDDSin, input))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!(ptr=SDDS_FindColumn(&SDDSin, FIND_NUMERIC_TYPE, xName, NULL))) {
        fprintf(stderr, "error: column %s doesn't exist\n", xName);
        exit(1);
        }
    free(xName);
    xName = ptr;
    if (xErrorName) {
        if (!(ptr=SDDS_FindColumn(&SDDSin, FIND_NUMERIC_TYPE, xErrorName, NULL))) {
            fprintf(stderr, "error: column %s doesn't exist\n", xErrorName);
            exit(1);
            }
        else {
            free(xErrorName);
            xErrorName = ptr;
            }
        }

    if (!(yNames=expandColumnPairNames(&SDDSin, &yName, &yErrorName, yNames,
                                       yExcludeName, yExcludeNames, FIND_NUMERIC_TYPE, 0))) {
        fprintf(stderr, "error: no quantities to differentiate found in file\n");
        exit(1);
        }

    setupOutputFile(&SDDSout, &SDDSin, output, &yOutputName, &yOutputErrorName, &yOutputUnits,
                    xName, xErrorName, yName, yErrorName, yNames, mainTemplate, errorTemplate,
                    interval, SGOrder>=0?SGDerivOrder:1);

    while (SDDS_ReadPage(&SDDSin)>0) {
      if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<2)
	SDDS_Bomb("Can't compute derivatives: too little data.");
      derivative = SDDS_Realloc(derivative, sizeof(*derivative)*rows);
      derivativeError = SDDS_Realloc(derivativeError, sizeof(*derivativeError)*rows);
      derivativePosition = SDDS_Realloc(derivativePosition, sizeof(*derivativePosition)*rows);
      if (!SDDS_StartPage(&SDDSout, rows) || !SDDS_CopyParameters(&SDDSout, &SDDSin))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      xError = NULL;
      if (!(xData = SDDS_GetColumnInDoubles(&SDDSin, xName)) ||
	  (xErrorName && !(xError=SDDS_GetColumnInDoubles(&SDDSin, xErrorName))))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<yNames; i++) {
	yError = NULL;
	if (!(yData = SDDS_GetColumnInDoubles(&SDDSin, yName[i])) ||
	    (yErrorName && yErrorName[i] && !(yError=SDDS_GetColumnInDoubles(&SDDSin, yErrorName[i]))))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (SGOrder>=0)
	  takeSGDerivative(xData, yData, rows, derivative, derivativePosition, 
			   SGLeft, SGRight, SGOrder, SGDerivOrder);
	else
	  takeDerivative(xData, yData, yError, rows, derivative, derivativeError, 
			 derivativePosition, interval);
	if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, derivative, rows, yOutputName[i]) ||
	    (yOutputErrorName && yOutputErrorName[i] && 
	     !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, derivativeError, rows, yOutputErrorName[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (yData) free(yData);
	if (yError) free(yError);
	yData = yError = NULL;
      }
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, derivativePosition, rows, xName) ||
	  (xErrorName && !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, xError, rows, xErrorName)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!SDDS_WritePage(&SDDSout))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (xData) free(xData);
      if (xError) free(xError);
      xData = xError = NULL;
    }
    if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (derivative) free(derivative);
    if (derivativeError) free(derivativeError);
    if (derivativePosition) free(derivativePosition);
    return(0);
}

void takeSGDerivative(double *x, double *y, long rows, 
                    double *deriv, double *derivPosition, 
                    long left, long right, long sgOrder, long derivOrder)
{
  long i;
  double spacing;
  
  spacing = (x[rows-1]-x[0])/(rows-1);
  for (i=0; i<rows; i++) {
    derivPosition[i] = x[i];
    deriv[i] = y[i]/spacing;
  }
  SavitzkyGolaySmooth(deriv, rows, sgOrder, left, right, derivOrder);
}


void takeDerivative(double *x, double *y, double *sy, long rows, 
                    double *deriv, double *derivSigma, double *derivPosition, long interval)
{
  long i, i1, i2;
  double dx;

  if (sy) {
    for (i=0; i<rows; i++) {
      if (findDerivIndices(&i1, &i2, interval, i, rows) && (dx = x[i2]-x[i1])) {
        deriv[i] = (y[i2]-y[i1])/dx;
        derivSigma[i] = sqrt(sqr(sy[i1])+sqr(sy[i2]))/fabs(dx);
        derivPosition[i] = (x[i2]+x[i1])/2;
      }
      else
        deriv[i] = derivSigma[i] = derivPosition[i] = DBL_MAX;
    }
  }
  else {
    for (i=0; i<rows; i++) {
      if (findDerivIndices(&i1, &i2, interval, i, rows) && (dx = x[i2]-x[i1])) {
        deriv[i] = (y[i2]-y[i1])/dx;
        derivPosition[i] = (x[i2]+x[i1])/2;
      }
      else
        deriv[i] = derivPosition[i] = DBL_MAX;
    }
  }
}

long findDerivIndices(long *i1, long *i2, long interval, long i, long rows)
{
    long index, rows1;

    rows1 = rows-1;
    *i1 = index = i - interval/2;
    if (index<0) {
        *i2 = 2*i;
        *i1 = 0;
        if (*i1==*i2)
            *i2 += 1;
        if (*i2<rows)
            return 1;
        return 0;
        }
    index = rows1 - (*i2=i+interval/2);
    if (index<0) {
        *i1 =  2*i - rows1;
        *i2 = rows1;
        if (*i1==*i2)
            *i1 -= 1;
        if (*i1>=0)
            return 1;
        return 0;
        }
    if (*i1==*i2) {
        if (*i2<rows1)
            *i2 = *i1 + 1;
        else
            *i1 = *i1 - 1;
        }
    if (*i2<rows && *i1>=0)
        return 1;
    return 0;
    }

long setupOutputFile(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *output,
                     char ***yOutputName, char ***yOutputErrorName, char ***yOutputUnits,
                     char *xName, char *xErrorName,
                     char **yName, char **yErrorName, long yNames,
                     char **mainTemplate0, char **errorTemplate0, int32_t interval,
                     long order)
{
    long i;
    char *xSymbol, *ySymbol;
    char *mainTemplate[3] = {"%yNameDeriv", "Derivative w.r.t. %xSymbol of %ySymbol", "d[%ySymbol]/d[%xSymbol]"};
    char *errorTemplate[3] = {"%yNameDerivSigma", "Sigma of derivative w.r.t. %xSymbol of %ySymbol", 
                                  "Sigma[d[%ySymbol]/d[%xSymbol]]"};
    char buffer[1024];

    for (i=0; i<3; i++) {
      if (!mainTemplate0[i]) {
        if (order!=1) {
          switch (i) {
          case 0:
            /* name */
            sprintf(buffer, "%%yNameDeriv%ld", order);
            break;
          case 1:
            /* description */
            sprintf(buffer, "Derivative %ld w.r.t. %%xSymbol of %%ySymbol", order);
            break;
          case 2:
            /* symbol */
            sprintf(buffer, "d$a%ld$n[%%ySymbol]/d[%%xSymbol]$a%ld$n", order, order);
            break;
          }
          cp_str(&mainTemplate[i], buffer);
        }
      } else
        mainTemplate[i] = mainTemplate0[i];
      if (errorTemplate0[i])
        errorTemplate[i] = errorTemplate0[i];
    }
    
    *yOutputName = tmalloc(sizeof(*yOutputName)*yNames);
    *yOutputErrorName = tmalloc(sizeof(*yOutputErrorName)*yNames);
    *yOutputUnits = tmalloc(sizeof(*yOutputUnits)*yNames);
    if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddsderiv output", output) ||
        SDDS_DefineParameter1(SDDSout, "derivInterval", NULL, NULL, NULL, NULL, SDDS_LONG, &interval)<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, xName, NULL) ||
        (xErrorName && !SDDS_TransferColumnDefinition(SDDSout, SDDSin, xErrorName, NULL)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (SDDS_GetColumnInformation(SDDSout, "symbol", &xSymbol, SDDS_GET_BY_NAME, xName)!=SDDS_STRING) {
        fprintf(stderr, "error: problem getting symbol for column %s\n", xName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!xSymbol)
        SDDS_CopyString(&xSymbol, xName);
    for (i=0; i<yNames; i++) {
        if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName[i], NULL)) {
            fprintf(stderr, "error: problem transferring definition for column %s\n", yName[i]);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (SDDS_GetColumnInformation(SDDSout, "symbol", &ySymbol, SDDS_GET_BY_NAME, yName[i])!=SDDS_STRING) {
            fprintf(stderr, "error: problem getting symbol for column %s\n", yName[i]);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (!ySymbol || SDDS_StringIsBlank(ySymbol))
            SDDS_CopyString(&ySymbol, yName[i]);
        (*yOutputUnits)[i] = divideColumnUnits(SDDSout, yName[i], xName);
        (*yOutputName)[i] = changeInformation(SDDSout, yName[i], yName[i], ySymbol, xName, xSymbol, mainTemplate,
                                              (*yOutputUnits)[i]);
        if (yErrorName || xErrorName) {
            if (yErrorName && yErrorName[i]) {
                if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yErrorName[i], NULL)) { 
                    fprintf(stderr, "error: problem transferring definition for column %s\n", yErrorName[i]);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    }
                (*yOutputErrorName)[i] = changeInformation(SDDSout, yErrorName[i], yName[i], ySymbol, xName, xSymbol,
                                                           errorTemplate, (*yOutputUnits)[i]);
                }
            else {
                if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName[i], NULL)) { 
                    fprintf(stderr, "error: problem transferring error definition for column %s\n", yName[i]);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    }
                (*yOutputErrorName)[i] = changeInformation(SDDSout, yName[i], yName[i], ySymbol, xName, xSymbol,
                                                           errorTemplate, (*yOutputUnits)[i]);
                }
            }
        else
            (*yOutputErrorName)[i] = NULL;
        }
    if (!SDDS_TransferAllParameterDefinitions(SDDSout, SDDSin, SDDS_TRANSFER_KEEPOLD) ||
        !SDDS_WriteLayout(SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return(1);
  }


char *changeInformation(SDDS_DATASET *SDDSout, char *name, char *nameRoot, char *symbolRoot, char *xName,
                        char *xSymbol, char **template, char *newUnits)
{
    char buffer1[SDDS_MAXLINE], buffer2[SDDS_MAXLINE], *ptr;

    if (!SDDS_ChangeColumnInformation(SDDSout, "units", newUnits, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                                      name))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    makeSubstitutions(buffer1, buffer2, template[2], nameRoot, symbolRoot, xName, xSymbol);
    if (!SDDS_ChangeColumnInformation(SDDSout, "symbol", buffer2, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                                      name))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    makeSubstitutions(buffer1, buffer2, template[1], nameRoot, symbolRoot, xName, xSymbol);
    if (!SDDS_ChangeColumnInformation(SDDSout, "description", buffer2, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                                      name))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    makeSubstitutions(buffer1, buffer2, template[0], nameRoot, symbolRoot, xName, xSymbol);
    if (!SDDS_ChangeColumnInformation(SDDSout, "name", buffer2, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                                      name))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    SDDS_CopyString(&ptr, buffer2);
    return ptr;
    }

void makeSubstitutions(char *buffer1, char *buffer2, char *template, char *nameRoot, char *symbolRoot, 
                       char *xName, char *xSymbol)
{
    strcpy(buffer2, template);
    replace_string(buffer1, buffer2, "%ySymbol", symbolRoot);
    replace_string(buffer2, buffer1, "%xSymbol", xSymbol);
    replace_string(buffer1, buffer2, "%yName", nameRoot);
    replace_string(buffer2, buffer1, "%xName", xName);
    strcpy(buffer1, buffer2);
    }

long checkErrorNames(char **yErrorName, long yNames)
{
    long i;
    if (yErrorName[0]) {
        for (i=1; i<yNames; i++)
            if (!yErrorName[i])
                return 0;
        }
    else {
        for (i=1; i<yNames; i++)
            if (yErrorName[i])
                return 0;
        }
    return 1;
    }

