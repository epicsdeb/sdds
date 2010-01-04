/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsinteg
 * purpose: integrate a data set
 * 
 * Michael Borland, 1995
 * based on mpl-format program integ
 $Log: sddsinteg.c,v $
 Revision 1.12  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.11  2002/08/14 17:12:47  soliday
 Added Open License

 Revision 1.10  2002/01/16 17:14:20  soliday
 Added missing SDDS_Terminate and free functions.

 Revision 1.9  2001/01/10 19:35:39  soliday
 Standardized usage message.

 Revision 1.8  1999/05/25 19:10:52  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:46  borland
 Fixed the version number in the usage message.

 Revision 1.6  1998/12/16 21:26:04  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.5  1998/11/13 22:39:53  borland
 Now copies through parameters from the input to the output.

 * Revision 1.4  1996/02/14  01:05:24  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:56:39  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include "scan.h"
#include <ctype.h>

static char *USAGE = "sddsinteg [<input>] [<output>] [-pipe=[input][,output]]\n\
-integrate=<column-name>[,<sigma-name>]...\n\
[-exclude=<column-name>[,...]] -versus=<column-name>[,<sigma-name>]\n\
[-mainTemplates=<item>=<string>[,...]] \n\
[-errorTemplates=<item>=<string>[,...]] \n\
[-method=<method-name>] [-printFinal[=bare][,stdout][,format=<string>]]\n\n\
At present the only method supported is \"trapezoidRule\".\n\
The -templates <item> may be \"name\", \"symbol\" or \"description\".\n\
The default main name, description, and symbol templates are \"%yNameInteg\",\n\
\"Integral w.r.t %xSymbol of %ySymbol\", and \"$sI$e %ySymbol d%xSymbol\", respectively.\n\
The default error name, description, and symbol templates are \"%yNameIntegSigma\",\n\
\"Sigma of integral w.r.t %xSymbol of %ySymbol\", and \"Sigma[$sI$e %ySymbol d%xSymbol]\", respectively.\n\
Program by Michael Borland.  (This is version 3, December 1998.)" ;

#define CLO_INTEGRATE 0
#define CLO_VERSUS 1
#define CLO_METHOD 2
#define CLO_PRINTFINAL 3
#define CLO_MAINTEMPLATE 4
#define CLO_ERRORTEMPLATE 5
#define CLO_PIPE 6
#define CLO_EXCLUDE 7
#define CLO_OPTIONS 8
static char *option[CLO_OPTIONS] = {
    "integrate", "versus", "method", "printfinal", "maintemplate", "errortemplate",
    "pipe", "exclude",
    };

void trapizoid(double *x, double *y, double *sx, double *sy, long n, double *integ, double *error);
long checkErrorNames(char **yErrorName, long nIntegrals);
void makeSubstitutions(char *buffer1, char *buffer2, char *template, char *nameRoot, char *symbolRoot, 
                       char *xName, char *xSymbol);
char *changeInformation(SDDS_DATASET *SDDSout, char *name, char *nameRoot, char *symbolRoot, char *xName,
                       char *xSymbol, char **template, char *newUnits);
long setupOutputFile(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *output,
                     char ***yOutputName, char ***yOutputErrorName, char ***yOutputUnits,
                     char *xName, char *xErrorName,
                     char **yName, char **yErrorName, long yNames,
                     char **mainTemplate, char **errorTemplate);

#define NORMAL_PRINTOUT 1
#define BARE_PRINTOUT 2
#define STDOUT_PRINTOUT 4

int main(int argc, char **argv)
{
    double *xData, *yData, *xError, *yError, *integral, *integralError;
    char *input, *output, *xName, *xErrorName, **yName, **yErrorName, **yOutputName, **yOutputErrorName, *ptr;
    char **yOutputUnits, **yExcludeName;
    char *mainTemplate[3] = {"%yNameInteg", "Integral w.r.t. %xSymbol of %ySymbol", "$sI$e %ySymbol d%xSymbol"};
    char *errorTemplate[3] = {"%yNameIntegSigma", "Sigma of integral w.r.t. %xSymbol of %ySymbol", 
                                  "Sigma[$sI$e %ySymbol d%xSymbol]"};
    long i, iArg, yNames, yExcludeNames, rows;
    SDDS_DATASET SDDSin, SDDSout;
    SCANNED_ARG *scanned;
    unsigned long flags, pipeFlags, printFlags;
    FILE *fpPrint;
    char *printFormat;

    SDDS_RegisterProgramName(argv[0]);

    argc = scanargs(&scanned, argc, argv); 
    if (argc==1)
        bomb(NULL, USAGE);

    input = output = xName = xErrorName = NULL;
    yName = yErrorName = yExcludeName = NULL;
    integral = integralError = yError = yData = xData = xError = NULL;
    yNames = yExcludeNames = 0;
    pipeFlags = printFlags = 0;
    fpPrint = stderr;
    printFormat = "%21.15e";

    for (iArg=1; iArg<argc; iArg++) {
        if (scanned[iArg].arg_type==OPTION) {
            /* process options here */
            switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
              case CLO_INTEGRATE:
                if (scanned[iArg].n_items!=2 && scanned[iArg].n_items!=3)
                    SDDS_Bomb("invalid -integrate syntax");
                yName = SDDS_Realloc(yName, sizeof(*yName)*(yNames+1));
                yErrorName = SDDS_Realloc(yErrorName, sizeof(*yErrorName)*(yNames+1));
                yName[yNames] = scanned[iArg].list[1];
                if (scanned[iArg].n_items==3)
                    yErrorName[yNames] = scanned[iArg].list[2];
                else
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
                if (scanned[iArg].n_items!=2 && scanned[iArg].n_items!=3)
                    SDDS_Bomb("invalid -verbus syntax");
                xName = scanned[iArg].list[1];
                if (scanned[iArg].n_items==3)
                    xErrorName = scanned[iArg].list[2];
                else
                    xErrorName = NULL;
                break;
              case CLO_METHOD:
                break;
              case CLO_PRINTFINAL:
                if ((scanned[iArg].n_items-=1)>=1) {
                    if (!scanItemList(&printFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                                        "bare", -1, NULL, 0, BARE_PRINTOUT,
                                        "stdout", -1, NULL, 0, STDOUT_PRINTOUT,
                                        "format", SDDS_STRING, &printFormat, 1, 0,
                                        NULL)) 
                        SDDS_Bomb("invalid -printFinal syntax");
                    }
                if (!(printFlags&BARE_PRINTOUT))
                    printFlags |= NORMAL_PRINTOUT;
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

    processFilenames("sddsinteg", &input, &output, pipeFlags, 0, NULL);

    if (printFlags&STDOUT_PRINTOUT)
        fpPrint = stdout;

    if (!yNames)
        SDDS_Bomb("-integrate option must be given at least once");
    if (!checkErrorNames(yErrorName, yNames))
        SDDS_Bomb("either all -integrate quantities must have errors, or none");

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
        fprintf(stderr, "error: no quantities to integrate found in file\n");
        exit(1);
        }

    setupOutputFile(&SDDSout, &SDDSin, output, &yOutputName, &yOutputErrorName, &yOutputUnits,
                    xName, xErrorName, yName, yErrorName, yNames, mainTemplate, errorTemplate);

    while (SDDS_ReadPage(&SDDSin)>0) {
      rows = SDDS_CountRowsOfInterest(&SDDSin);
      integral = SDDS_Realloc(integral, sizeof(*integral)*rows);
      integralError = SDDS_Realloc(integralError, sizeof(*integralError)*rows);
      if (!SDDS_StartPage(&SDDSout, rows) || !SDDS_CopyParameters(&SDDSout, &SDDSin))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      xError = NULL;
      if (!(xData = SDDS_GetColumnInDoubles(&SDDSin, xName)) ||
	  (xErrorName && !(xError=SDDS_GetColumnInDoubles(&SDDSin, xErrorName))))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, xData, rows, xName) ||
	  (xErrorName && !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, xError, rows, xErrorName)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<yNames; i++) {
	yError = NULL;
	if (!(yData = SDDS_GetColumnInDoubles(&SDDSin, yName[i])) ||
	    (yErrorName && yErrorName[i] && !(yError=SDDS_GetColumnInDoubles(&SDDSin, yErrorName[i]))))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	trapizoid(xData, yData, xError, yError, rows, integral, integralError);
	if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, integral, rows, yOutputName[i]) ||
	    (yOutputErrorName && yOutputErrorName[i] && 
	     !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_BY_NAME, integralError, rows, yOutputErrorName[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (printFlags&BARE_PRINTOUT) {
	  fprintf(fpPrint, printFormat, integral[rows-1]);
	  if (xError || yError) {
	    fputc(' ', fpPrint);
	    fprintf(fpPrint, printFormat, integralError[rows-1]);
	  }
	  fputc('\n', fpPrint);
	}
	else if (printFlags&NORMAL_PRINTOUT) {
	  fprintf(fpPrint, "%s: ", yName[i]);
	  fprintf(fpPrint, printFormat, integral[rows-1]);
	  if (xError || yError) {
	    fputs(" +/- ", fpPrint);
	    fprintf(fpPrint, printFormat, integralError[rows-1]);
	    fputs(yOutputUnits[i], fpPrint);
	  }
	  fputc('\n', fpPrint);
	}
	if (yData) free(yData);
	if (yError) free(yError);
	yData = yError = NULL;
      }
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
    if (integral) free(integral);
    if (integralError) free(integralError);
    return(0);
    }

long setupOutputFile(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *output,
                     char ***yOutputName, char ***yOutputErrorName, char ***yOutputUnits,
                     char *xName, char *xErrorName,
                     char **yName, char **yErrorName, long yNames,
                     char **mainTemplate, char **errorTemplate)
{
    long i;
    char *xSymbol, *ySymbol;

    *yOutputName = tmalloc(sizeof(*yOutputName)*yNames);
    *yOutputErrorName = tmalloc(sizeof(*yOutputErrorName)*yNames);
    *yOutputUnits = tmalloc(sizeof(*yOutputUnits)*yNames);
    if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddsinteg output", output))
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
        (*yOutputUnits)[i] = multiplyColumnUnits(SDDSout, yName[i], xName);
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

void trapizoid(
               double *x, double *y, 	    /* indep, dependent variable */
               double *sx, double *sy,         /* sigmas 		     */
               long n,                         /* number of points	     */
               double *integ, double *error
               )
{
    double dx, dy;
    long i;
    
    integ[0] = error[0] = 0;
    for (i=1; i<n; i++) {
        dy = y[i]+y[i-1];
        dx = x[i]-x[i-1];
        integ[i] = integ[i-1]+dy*dx;
        if (sx && sy)
            error[i] = error[i-1]+sqr(dy)*(sqr(sx[i-1])+sqr(sx[i]))+sqr(dx)*(sqr(sy[i-1])+sqr(sy[i]));
        else if (sx)
            error[i] = error[i-1]+sqr(dy)*(sqr(sx[i-1])+sqr(sx[i]));
        else if (sy)
            error[i] = error[i-1]+sqr(dx)*(sqr(sy[i-1])+sqr(sy[i]));
        else
            error[i] = 0;
        }
    for (i=0; i<n; i++) {
        error[i] = sqrt(error[i])/2;
        integ[i] /= 2;
        }
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

