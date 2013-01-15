/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program sddsminterp.c (model interpolation)
 * interpolates data using another data set as a model function.
 * use abscissa from a values files or from the model function file.
 * version 1.0 Dec 1994 was called scaleref2data
 * version 2.0 June 1995 reads multiple table from the data file,
 *     uses option syntax that resembles that of sddsinterp,
 *     uses pipes.
 $Log: not supported by cvs2svn $
 Revision 1.9  2005/11/04 22:46:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.8  2002/08/14 17:12:48  soliday
 Added Open License

 Revision 1.7  2001/01/10 19:35:41  soliday
 Standardized usage message.

 Revision 1.6  1999/05/25 19:11:27  soliday
 Removed compiler warning on linux.

 Revision 1.5  1999/01/06 19:54:47  borland
 Fixed the version number in the usage message.

 * Revision 1.4  1996/02/14  01:05:28  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:56:45  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "SDDS.h"

#define CLO_COLUMNS 0
#define CLO_ORDER 1
#define CLO_MODEL 2
#define CLO_VALUES 3
#define CLO_VERBOSE 4
#define CLO_ASCII 5
#define CLO_PIPE 6
#define COMMANDLINE_OPTIONS 7
char *commandline_option[COMMANDLINE_OPTIONS] = {
    "columns",
    "order",
    "model",
    "fileValues",
    "verbose",
    "ascii",
    "pipe"
        };

static char *USAGE="sddsminterp <input-file> <output-file> [-pipe=[input],[output]] \n\
 -columns=<independent-quantity>,<name> [-interpOrder=<order>]\n\
 -model=<modelFile>,abscissa=<column>,ordinate=<column>[,interp=<order>] \n\
 -fileValues=<valuesFile>[,abscissa=<column>] \n\
 -verbose -ascii\n\n\
Multiplicative renormalized model interpolation of a data set using another data set as a model function.\n\
columns             specifies the data in the input file to be interpolated.\n\
order               interpolation order of the multiplicative factor.\n\
model               data representing the model function.\n\
fileValues          specifies abscissa at which interpolated values are calculated.\n\
                    If not present, then the abscissa values of the model file are used.\n\
pipe                writes the output to a pipe.\n\n\
Program by Louis Emery, ANL (This is version 3, February 1996.)\n";

#define MOD_ABSCISSA 0x0001U
#define MOD_ORDINATE 0x0002U
#define MOD_ORDER    0x0004U
static char *modUsage="-model=<file>,abscissa=<column>,ordinate=<column>[,interp=<order>]\n";

#define VAL_ABSCISSA 0x0001U
static char *valUsage="-valueFile=<file>,abscissa=<column>\n";

#define DEFAULT_ORDER 1
int main(int argc, char **argv)
{
    SCANNED_ARG *s_arg;
    SDDS_DATASET modDataSet, valDataSet, dataDataSet, outDataSet;
    unsigned long modFlags, valFlags;
    char *modAbscissaName, *modOrdinateName;
    char *valAbscissaName;
    char *dataAbscissaName, *dataOrdinateName;
    char *outAbscissaName, *outOrdinateName;
    char *modFile, *dataFile, *valFile, *outFile;
    long dataOrder;
    int32_t modOrder;
    double *modAbscissa, *modOrdinate;
    double *valAbscissa;
    double *dataAbscissa, *dataOrdinate, *dataScale, *dataOrdinateInterp;
    double *outScale, *outAbscissa, *outOrdinate;
    long modRows, valRows, dataRows, outRows;

    long i, i_arg;
    long verbose;
    long warning;
    long ascii;
    long returnCode;
    unsigned long pipeFlags;
    long tmpfile_used, noWarnings;
     
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc==1)
        bomb(NULL, USAGE);

    verbose=0;
    dataFile = dataAbscissaName = dataOrdinateName = modFile = valFile = NULL;
    modOrdinate = dataOrdinate = NULL;
    outFile = NULL;
    warning=0;
    modOrder=DEFAULT_ORDER;
    dataOrder=DEFAULT_ORDER;
    ascii=0;
    modFlags=0x0UL;
    valFlags=0x0UL;
    pipeFlags = 0;
    tmpfile_used = 0;
    noWarnings = 0;
    
    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch(match_string(s_arg[i_arg].list[0], commandline_option, COMMANDLINE_OPTIONS, 
                                UNIQUE_MATCH)) {
              case CLO_VERBOSE:
                verbose=1;
                break;
              case CLO_ASCII:
                ascii=1;
                break;
              case CLO_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_MODEL:
                if((s_arg[i_arg].n_items-=2)<0 ||
                   !scanItemList(&modFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
                                   "abscissa", SDDS_STRING, &modAbscissaName, 1, MOD_ABSCISSA, 
                                   "ordinate", SDDS_STRING, &modOrdinateName, 1, MOD_ORDINATE, 
                                   "interp", SDDS_LONG, &modOrder, 1, MOD_ORDER, 
                                   NULL) ||
                   !(modFlags&MOD_ABSCISSA) || !(modFlags&MOD_ORDINATE))
                    bomb("bad -model syntax", modUsage);
                if(!strlen(modFile=s_arg[i_arg].list[1]))
                    bomb("bad -model syntax", modUsage);
                break;
              case CLO_VALUES:
                if((s_arg[i_arg].n_items-=2)<0 ||
                   !scanItemList(&valFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
                                   "abscissa", SDDS_STRING, &valAbscissaName, 1, VAL_ABSCISSA, 
                                   NULL) ||
                   !(valFlags&VAL_ABSCISSA))
                    bomb("bad -val syntax", valUsage);
                if(!strlen(valFile=s_arg[i_arg].list[1]))
                    bomb("bad -val syntax", valUsage);
                break;
              case CLO_ORDER:
                if (!(get_long(&dataOrder, s_arg[i_arg].list[1])))
                    bomb("no string given for option -order", USAGE);
                break;
              case CLO_COLUMNS:
/* The input file and the -order and -columns options could be combined more
   compactly to resemble the syntax of the -model option.
   However we adopt the command line options of the 
   commadn sddsinterp since it allows an input pipe */
                if (s_arg[i_arg].n_items!=3 || 
                    !strlen(dataAbscissaName=s_arg[i_arg].list[1]) ||
                    !strlen(dataOrdinateName=s_arg[i_arg].list[2]))
                    SDDS_Bomb("bad -columns syntax");
                break;
              default:
                SDDS_Bomb("unrecognized option given");
                }
            }
        else {
            if (!dataFile)
                dataFile = s_arg[i_arg].list[0];
            else if (!outFile)
                outFile = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames given");
            }
        }
    
    processFilenames("sddsminterp", &dataFile, &outFile, pipeFlags, noWarnings, &tmpfile_used);
    
    if (valFlags) {
        fprintf(stderr,"Warning option -fileValue not operational yet. The model abscissa are used.\n");
        }
    
    if(!SDDS_InitializeInput(&modDataSet, modFile))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if(SDDS_ReadPage(&modDataSet)<0)
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    if (valFlags) {
        if(!SDDS_InitializeInput(&valDataSet, valFile))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        if(SDDS_ReadPage(&valDataSet)<0)
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        }
    
/* check that the selected columns exists */
    switch(SDDS_CheckColumn(&modDataSet, modAbscissaName, NULL, SDDS_DOUBLE, NULL)) {
      case SDDS_CHECK_NONEXISTENT:
        fprintf(stderr,"Error: Something wrong with column %s in file %s.\n", modAbscissaName, modFile);
        exit(1);
        }
    switch(SDDS_CheckColumn(&modDataSet, modOrdinateName, NULL, SDDS_DOUBLE, NULL)) {
      case SDDS_CHECK_NONEXISTENT:
        fprintf(stderr,"Error: Something wrong with column %s in file %s.\n", modOrdinateName, modFile);
        exit(1);
        }

    if(valFlags){
        switch(SDDS_CheckColumn(&valDataSet, valAbscissaName, NULL, SDDS_DOUBLE, NULL)) {
          case SDDS_CHECK_NONEXISTENT:
            fprintf(stderr,"Error: Something wrong with column %s in file %s.\n", valAbscissaName, valFile);
            exit(1);
            }
        }

    modRows = SDDS_CountRowsOfInterest(&modDataSet);
    if(!(modAbscissa=(double*)SDDS_GetColumnInDoubles(&modDataSet, modAbscissaName)) ||
       !(modOrdinate=(double*)SDDS_GetColumnInDoubles(&modDataSet, modOrdinateName)))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    if (valFlags) {
        valRows = SDDS_CountRowsOfInterest(&valDataSet);
        if(!(valAbscissa=(double*)SDDS_GetColumnInDoubles(&valDataSet, valAbscissaName)))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        outAbscissa = valAbscissa;
        outRows = valRows;
        }
    else {
        outAbscissa = modAbscissa;
        outRows = modRows;
        }
    
    if(!SDDS_InitializeInput(&dataDataSet, dataFile))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    switch(SDDS_CheckColumn(&dataDataSet, dataAbscissaName, NULL, SDDS_DOUBLE, NULL)) {
      case SDDS_CHECK_NONEXISTENT:
        fprintf(stderr,"Error: Something wrong with column %s in file %s.\n", dataAbscissaName, dataFile);
        exit(1);
        }
    switch(SDDS_CheckColumn(&dataDataSet, dataOrdinateName, NULL, SDDS_DOUBLE, NULL)) {
      case SDDS_CHECK_NONEXISTENT:
        fprintf(stderr,"Error: Something wrong with column %s in file %s.\n", dataOrdinateName, dataFile);
        exit(1);
        }

    if(!SDDS_InitializeOutput(&outDataSet, ascii?SDDS_ASCII:SDDS_BINARY, 1, "Interpolation data from model file", 
                              "Interpolated data", outFile) ||
       !SDDS_InitializeCopy(&outDataSet, &dataDataSet, outFile, "w") ||
       !SDDS_WriteLayout(&outDataSet))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (valFlags) {
        SDDS_CopyString(&outAbscissaName, valAbscissaName);
        }
    else {
        SDDS_CopyString(&outAbscissaName, modAbscissaName);
        }
    SDDS_CopyString(&outOrdinateName, dataOrdinateName);

    while (0<SDDS_ReadPage(&dataDataSet)) {
        if(!(dataAbscissa=(double*)SDDS_GetColumnInDoubles(&dataDataSet, dataAbscissaName)) ||
           !(dataOrdinate=(double*)SDDS_GetColumnInDoubles(&dataDataSet, dataOrdinateName)))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        dataRows=SDDS_CountRowsOfInterest(&dataDataSet);
        dataOrdinateInterp=(double*)malloc(sizeof(double)*dataRows);
        dataScale=(double*)malloc(sizeof(double)*dataRows);

        outScale=(double*)malloc(sizeof(double)*outRows);
        outOrdinate=(double*)malloc(sizeof(double)*outRows);

/* There are normally more rows in the model file or value file than in the data file. */
        for(i=0;i<dataRows;i++){
            dataOrdinateInterp[i]=interp(modOrdinate, modAbscissa, modRows, dataAbscissa[i], warning, modOrder, &returnCode);
            dataScale[i]=dataOrdinate[i]/dataOrdinateInterp[i]; /* dataScale is a numerator */
            }

        for(i=0;i<outRows;i++){
            outScale[i]=interp(dataScale, dataAbscissa, dataRows, outAbscissa[i], warning, dataOrder, &returnCode);
            outOrdinate[i]=modOrdinate[i]*outScale[i];
            }

        if (0>SDDS_StartPage(&outDataSet, outRows))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        if(!SDDS_CopyParameters(&outDataSet, &dataDataSet) ||
           !SDDS_CopyArrays(&outDataSet, &dataDataSet) ||
           !SDDS_SetColumn(&outDataSet, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, modAbscissa, outRows, outAbscissaName)||
           !SDDS_SetColumn(&outDataSet, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, outOrdinate, outRows, outOrdinateName)||
           0>SDDS_WritePage(&outDataSet))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        
        free(dataAbscissa);
        free(dataOrdinate);
        free(dataOrdinateInterp);
        free(dataScale);
        free(outScale);
        free(outOrdinate);
        }
    
    if(!SDDS_Terminate(&modDataSet) || !SDDS_Terminate(&outDataSet))
        SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if(!SDDS_Terminate(&dataDataSet))
            SDDS_PrintErrors(stdout, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    if (tmpfile_used && !replaceFileAndBackUp(dataFile, outFile)) 
        exit(1);

    return(0);
}
