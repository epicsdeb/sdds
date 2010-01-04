/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsdistest.c
 * purpose: test data against various distributions
 * M. Borland, August 1995.
   $Log: sddsdistest.c,v $
   Revision 1.8  2005/11/04 22:46:13  soliday
   Updated code to be compiled by a 64 bit processor.

   Revision 1.7  2005/10/18 14:14:01  jiaox
   Corrected PoissonCDF function. Fixed a bug of @parameterName option that did not work.

   Revision 1.6  2002/08/14 17:12:44  soliday
   Added Open License

   Revision 1.5  2001/01/10 19:35:35  soliday
   Standardized usage message.

   Revision 1.4  1999/07/12 21:11:56  soliday
   Added link to fdlibm library for lgamma function on WIN32

   Revision 1.3  1999/05/25 19:08:31  soliday
   Removed compiler warning on linux.

   Revision 1.2  1999/01/06 19:54:42  borland
   Fixed the version number in the usage message.

   Revision 1.1  1997/01/15 18:39:22  borland
   Found this old program in an old SDDS directory.  Think it is ok, but
   was accidentally left out of repository.

 */

#include "mdb.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include "scan.h"

#if defined(_WIN32)
#include "fdlibm.h"
#endif

void compareToFileDistribution(char *output, long testCode, SDDS_DATASET *SDDSin, char **columnName, long columnNames,
                               char *distFile, char *distFileIndep, char *distFileDepen);
void compareToDistribution(char *output, long testCode, SDDS_DATASET *SDDSin, char **columnName,  
                           char **sigmaName, long columnNames, long distCode, long degreesFree, char *dofParameter);
void ksTestWithFunction(double *data, long rows, double (*CDF)(double x),
                        double *statReturn, double *sigLevelReturn);
void chiSquaredTestWithFunction(double *data, long rows, double (*PDF)(double x), 
                        double *statReturn, double *sigLevelReturn);

static char *USAGE = "sddsdistest [<input>] [<output>] \n\
[-pipe=[in][,out]] \n\
-column=<name>[,sigma=<name>]... -exclude=<name>[,...]\n\
[-degreesOfFreedom={<value>|@<parameterName>}]\n\
[-test={ks|chisquared}] \n\
[{-fileDistribution=<filename>,<indepName>,<depenName> | \n\
-gaussian | -poisson | -student | -chisquared}]\n\n\
Program by M. Borland. (This is version 2, January 1997.)\n";

#define CLO_PIPE 0
#define CLO_COLUMN 1
#define CLO_TEST 2
#define CLO_FILEDIST 3
#define CLO_GAUSSIAN 4
#define CLO_POISSON 5
#define CLO_STUDENT 6
#define CLO_CHISQUARED 7
#define CLO_DOF 8
#define CLO_EXCLUDE 9
#define N_CL_OPTIONS 10

static char *option[N_CL_OPTIONS] = {
    "pipe", "column", "test", "filedistribution", "gaussian",
    "poisson", "student", "chisquared", "degreesoffreedom", "exclude",
    };

#define KS_TEST 0
#define CHI_TEST 1
#define N_TESTS 2
static char *testChoice[N_TESTS] = {
    "ks", "chisquared",
    } ;

int main(int argc, char **argv)
{
    int iArg;
    unsigned long dummyFlags, pipeFlags;
    SCANNED_ARG *scanned;
    SDDS_DATASET SDDSin;
    char *input, *output, *distFile, **columnName, **sigmaName, **excludeName, *distFileIndep, *distFileDepen;
    long testCode, distCode, code, degreesFree, columnNames, excludeNames;
    char *dofParameter;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&scanned, argc, argv); 
    if (argc<3)
        bomb(NULL, USAGE);

    output = input = distFile = distFileIndep = distFileDepen = NULL;
    columnName = sigmaName = excludeName = NULL;
    excludeNames = columnNames = 0;
    pipeFlags = 0;
    testCode = 0;
    distCode = degreesFree = -1;
    dofParameter = NULL;

    for (iArg=1; iArg<argc; iArg++) {
        if (scanned[iArg].arg_type==OPTION) {
            /* process options here */
            switch (code=match_string(scanned[iArg].list[0], option, N_CL_OPTIONS, 0)) {
              case CLO_PIPE:
                if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax/values");
                break;
              case CLO_COLUMN:
                if ((scanned[iArg].n_items!=2 && scanned[iArg].n_items!=3)
                    || SDDS_StringIsBlank(scanned[iArg].list[1]))
                    SDDS_Bomb("invalid -column syntax/values");
                moveToStringArray(&columnName, &columnNames, scanned[iArg].list+1, 1);
                sigmaName = SDDS_Realloc(sigmaName, sizeof(*sigmaName)*columnNames);
                if (scanned[iArg].n_items==3) {
                    scanned[iArg].n_items -= 2;
                    if (!scan_item_list(&dummyFlags, scanned[iArg].list+2, &scanned[iArg].n_items,
                                        "sigma", SDDS_STRING, sigmaName+columnNames-1, 1, 1,
                                        NULL) ||
                        dummyFlags!=1 || SDDS_StringIsBlank(sigmaName[columnNames-1]))
                        SDDS_Bomb("invalid -column syntax/values");
                    }
                else
                    sigmaName[columnNames-1] = NULL;
                break;
              case CLO_TEST:
                if (scanned[iArg].n_items!=2 ||
                    (testCode=match_string(scanned[iArg].list[1], testChoice, N_TESTS, 0))<0)
                    SDDS_Bomb("invalid -test syntax/values");
                break;
              case CLO_FILEDIST:
                if (scanned[iArg].n_items!=4)
                    SDDS_Bomb("too few qualifiers for -fileDistribution");
                if (SDDS_StringIsBlank(distFile=scanned[iArg].list[1]) ||
                    SDDS_StringIsBlank(distFileIndep=scanned[iArg].list[2]) ||
                    SDDS_StringIsBlank(distFileDepen=scanned[iArg].list[3]))
                    SDDS_Bomb("invalid -fileDistribution values");
                break;
              case CLO_GAUSSIAN:
              case CLO_POISSON:
              case CLO_STUDENT:
              case CLO_CHISQUARED:
                distCode = code;
                break;
              case CLO_DOF:
                if (scanned[iArg].n_items!=2)
                    SDDS_Bomb("too few qualifiers for -degreesOfFreedom");
                if (scanned[iArg].list[1][0]=='@') {
                    if (!SDDS_CopyString(&dofParameter, scanned[iArg].list[1]+1))
                        SDDS_Bomb("memory allocation failure");
                    }
                else if (sscanf(scanned[iArg].list[1], "%ld", &degreesFree)!=1 || degreesFree<=1)
                    SDDS_Bomb("invalid degrees-of-freedom given for -student/-chiSquared");
                break;
              case CLO_EXCLUDE:
                if (scanned[iArg].n_items<2 || SDDS_StringIsBlank(scanned[iArg].list[1]))
                    SDDS_Bomb("invalid -exclude syntax/values");
                moveToStringArray(&excludeName, &excludeNames, scanned[iArg].list+1, scanned[iArg].n_items-1);
                break;
              default:
                fprintf(stderr, "error: unknown/ambiguous option: %s (%s)\n", 
                        scanned[iArg].list[0], argv[0]);
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

    processFilenames("sddsdistest", &input, &output, pipeFlags, 0, NULL);

    if (!SDDS_InitializeInput(&SDDSin, input))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!columnNames)
        SDDS_Bomb("-column option must be supplied");
    if (!(columnNames=expandColumnPairNames(&SDDSin, &columnName, &sigmaName, columnNames, excludeName, excludeNames,
                                            FIND_NUMERIC_TYPE, 0))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        SDDS_Bomb("named columns nonexistent or nonnumerical");
        }
    if (dofParameter && SDDS_CheckParameter(&SDDSin, dofParameter, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OKAY)
        SDDS_Bomb("degrees-of-freedom parameter not found");

    if (distFile) 
        compareToFileDistribution(output, testCode, &SDDSin, columnName, columnNames,
                                  distFile, distFileIndep, distFileDepen);
    else 
        compareToDistribution(output, testCode, &SDDSin, columnName, sigmaName, columnNames, distCode, degreesFree,
                              dofParameter);

    return(0);
}

void compareToFileDistribution(char *output, long testCode, SDDS_DATASET *SDDSin, 
                               char **columnName, long columnNames,
                               char *distFile, char *distFileIndep, char *distFileDepen)
{
    SDDS_DATASET SDDSdist;
    if (!SDDS_InitializeInput(&SDDSdist, distFile))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (SDDS_CheckColumn(&SDDSdist, distFileIndep, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OKAY ||
        SDDS_CheckColumn(&SDDSdist, distFileDepen, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OKAY )
        exit(1);
    SDDS_Bomb("-fileDistribution option not implemented yet");
    }

static double sampleMean, sampleStDev;
static int32_t DOF;

double gaussianPDF(double x)
{
    double z;
    z = (x-sampleMean)/sampleStDev;
    return exp(-z*z/2)/sqrt(PIx2);
    }

double gaussianCDF(double x)
{
    long zSign;
    double z;
    z = (x-sampleMean)/sampleStDev;
    zSign = 1;
    if (z<0) {
        zSign = -1;
        z = -z;
        }
    return (1 + zSign*erf(z/SQRT2))/2.0;
    }

#define POISSON_ACCURACY (1e-8)

double poissonPDF(double xd)
{
    long x;

    if ((x = xd)<0)
        return 0;
    return exp(-sampleMean+x*log(sampleMean)-lgamma(x+1.0));
    }
    
double poissonCDF(double xd)
{
    double CDF, term, accuracy;
    long x, n;

    if (xd<0) 
        xd = 0;
    x = xd;
    xd = x;  
    term = 1;
    CDF = 1;
    accuracy = POISSON_ACCURACY/exp(-sampleMean);
    for (n=1; n<=x; n++) {
        term *= sampleMean/n;
        CDF += term;
        if (term<accuracy)
            break;
        }
    return CDF*exp(-sampleMean);
    }

double studentPDF(double t)
{
    return exp(-0.5*(DOF+1)*log(1+t*t/DOF)
               + lgamma((DOF+1.0)/2.0)
               - lgamma(DOF/2.0))/sqrt(PI*DOF);
    }

double studentCDF(double t)
{
    if (t>0)
        return 1- betaInc(DOF/2.0, 0.5, DOF/(DOF+t*t))/2;
    else
        return betaInc(DOF/2.0, 0.5, DOF/(DOF+t*t))/2;
    }

#define LOG2 0.693147180559945

double chiSquaredPDF(double x)
{
    double chiSqr, DOFover2;

    if (x<0)
        return 0;
    chiSqr = x*DOF/sampleMean;
    DOFover2 = DOF/2.0;
    return exp( (DOFover2-1.0)*log(chiSqr) - chiSqr/2 - DOFover2*LOG2 - lgamma(DOFover2) );
    }

double chiSquaredCDF(double x)
{
    double chiSqr;

    if (x<0)
        x = 0;
    chiSqr = x*DOF/sampleMean;
    return 1 - gammaQ(DOF/2.0, chiSqr/2.0);
    }

void compareToDistribution(char *output, long testCode, SDDS_DATASET *SDDSin, char **columnName, 
                           char **sigmaName, long columnNames, long distCode, long degreesFree, char *dofParameter)
{
    SDDS_DATASET SDDSout;
    double *data, *data1, stat, sigLevel;
    long iStat, iSigLevel, iCount, iColumnName;
    long rows, icol, row;
    iStat = iSigLevel = iCount = iColumnName = 0;
    if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, "sddsdistest output", output) ||
        0>SDDS_DefineParameter(&SDDSout, "distestDistribution", NULL, NULL, "sddsdistest distribution name", NULL,
                               SDDS_STRING, option[distCode]) ||
        0>SDDS_DefineParameter(&SDDSout, "distestTest", NULL, NULL, "sddsdistest test name", NULL,
                                      SDDS_STRING, testChoice[testCode]) ||
        0>(iCount=SDDS_DefineParameter(&SDDSout, "Count", NULL, NULL, "Number of data points", NULL,
                                          SDDS_LONG, 0)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    switch (testCode) {
      case KS_TEST:
        if (0>(iColumnName=SDDS_DefineColumn(&SDDSout, "ColumnName", NULL, NULL, "Column analysed by sddsdistest",
                                             NULL, SDDS_STRING, 0)) ||
            0>(iStat=SDDS_DefineColumn(&SDDSout, "D", NULL, NULL, "Kolmogorov-Smirnov D statistic", NULL,
                                          SDDS_DOUBLE, 0)) ||
            0>(iSigLevel=SDDS_DefineColumn(&SDDSout, "distestSigLevel", "P(D$ba$n>D)", NULL, "Probability of exceeding D", 
                                              NULL, SDDS_DOUBLE, 0)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      case CHI_TEST:
        if (0>(iColumnName=SDDS_DefineColumn(&SDDSout, "ColumnName", NULL, NULL, "Column analysed by sddsdistest",
                                             NULL, SDDS_STRING, 0)) ||
            0>(iStat=SDDS_DefineParameter(&SDDSout, "ChiSquared", "$gh$r$a2$n", NULL, "Chi-squared statistic", NULL,
                                          SDDS_DOUBLE, 0)) ||
            0>(iSigLevel=SDDS_DefineParameter(&SDDSout, "distestSigLevel", "P($gh$r$a2$n$ba$n>$gh$r$a2$n)", NULL, 
                                              "Probability of exceeding $gh$r$a2$n", NULL,
                                              SDDS_DOUBLE, 0))
            )
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        break;
      default:
        SDDS_Bomb("Invalid testCode seen in compareToDistribution--this shouldn't happen.");
        break;
        }
    if (!SDDS_WriteLayout(&SDDSout))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    while (SDDS_ReadPage(SDDSin)>0) {
        if (!SDDS_StartPage(&SDDSout, columnNames))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        rows = SDDS_CountRowsOfInterest(SDDSin);
        for (icol=0; icol<columnNames; icol++) {
            stat = 0;
            sigLevel = 1;
            if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX, columnName, columnNames, iColumnName) ||
                !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                                    iCount, rows, -1))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            if (rows>=2) {
                if (!(data=SDDS_GetColumnInDoubles(SDDSin, columnName[icol])))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                if (dofParameter) 
		    SDDS_GetParameterAsLong(SDDSin,dofParameter,&DOF);
		else
		    DOF = degreesFree;
		switch (distCode) {
                  case CLO_GAUSSIAN:
                    computeMoments(&sampleMean, NULL, &sampleStDev, NULL, data, rows);
                    if (testCode==KS_TEST)
                        ksTestWithFunction(data, rows, gaussianCDF, &stat, &sigLevel);
                    else
                        chiSquaredTestWithFunction(data, rows, gaussianPDF, &stat, &sigLevel);
                    break;
                  case CLO_POISSON:
                    computeMoments(&sampleMean, NULL, NULL, NULL, data, rows);
                    if (testCode==KS_TEST)
                        ksTestWithFunction(data, rows, poissonCDF, &stat, &sigLevel);
                    else
                        chiSquaredTestWithFunction(data, rows, poissonPDF, &stat, &sigLevel);
                    break;
                  case CLO_STUDENT:
                    if (DOF<1)
                        SDDS_Bomb("must have at least one degree of freedom for Student distribution tests");
                    computeMoments(&sampleMean, NULL, NULL, NULL, data, rows);
                    if (sigmaName && sigmaName[icol]) {
                        if (!(data1=SDDS_GetColumnInDoubles(SDDSin, sigmaName[icol])))
                            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                        for (row=0; row<rows; row++)
                            /* compute t = (x-mu)/sigma */
                            data[row] = (data[row]-sampleMean)/data1[row];
                        free(data1);
                        }
                    else
                        for (row=0; row<rows; row++)
                            data[row] -= sampleMean;
                    if (testCode==KS_TEST)
                        ksTestWithFunction(data, rows, studentCDF, &stat, &sigLevel);
                    else
                        chiSquaredTestWithFunction(data, rows, studentPDF, &stat, &sigLevel);
                    break;
                  case CLO_CHISQUARED:
                    computeMoments(&sampleMean, NULL, NULL, NULL, data, rows);
                    if (DOF<1)
                        SDDS_Bomb("must have at least one degree of freedom for chi-squared distribution tests");
                    if (testCode==KS_TEST)
                        ksTestWithFunction(data, rows, chiSquaredCDF, &stat, &sigLevel);
                    else
                        chiSquaredTestWithFunction(data, rows, chiSquaredPDF, &stat, &sigLevel);
                    break;
                  default:
                    SDDS_Bomb("Invalid distCode in compareToDistribution--this shouldn't happen");
                    break;
                    }
                }
            if (!SDDS_SetRowValues(&SDDSout, SDDS_PASS_BY_VALUE|SDDS_SET_BY_INDEX,
                                   icol, iStat, stat, iSigLevel, sigLevel, -1))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (!SDDS_WritePage(&SDDSout))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    }

void chiSquaredTestWithFunction(double *data, long rows, double (*PDF)(double x), 
                        double *statReturn, double *sigLevelReturn)
{
    SDDS_Bomb("Chi-squared distribution test not implemented yet---wouldn't you really like a nice K-S test instead?");
    }

void ksTestWithFunction(double *data, long rows, double (*CDF)(double x),
                        double *statReturn, double *sigLevelReturn)
{
    double CDF1, CDF2, dCDF1, dCDF2, CDF0, dCDFmaximum;
    long row;

    qsort((void*)data, rows, sizeof(*data), double_cmpasc);
    dCDFmaximum = CDF1 = 0;
    for (row=0; row<rows; row++) {
        CDF0 = (*CDF)(data[row]);
        CDF2 = (row+1.0)/rows;
        if ((dCDF1 =  CDF0 - CDF1)<0)
            dCDF1 = -dCDF1;
        if ((dCDF2 =  CDF0 - CDF2)<0)
            dCDF2 = -dCDF2;
        CDF1 = CDF2;
        if (dCDF1>dCDFmaximum)
            dCDFmaximum = dCDF1;
        if (dCDF2>dCDFmaximum)
            dCDFmaximum = dCDF2;
        }
    *statReturn = dCDFmaximum;
    *sigLevelReturn = KS_Qfunction(sqrt(1.0*rows)*dCDFmaximum);
    }
