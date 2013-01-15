/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddspoly
 * purpose: evaluation of polynomials for N-dimensional input
 *
 * Michael Borland, 2002
 *
 $Log: not supported by cvs2svn $
 Revision 1.7  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.6  2005/11/04 22:46:17  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.5  2003/09/02 19:16:06  soliday
 Cleaned up code for Linux.

 Revision 1.4  2002/08/14 17:12:51  soliday
 Added Open License

 Revision 1.3  2002/08/07 01:07:56  borland
 Fixed silly bug that prevented doign more than two polynomials.

 Revision 1.2  2002/08/06 21:02:29  shang
 made -pipe option work and freed memory leaks

 Revision 1.1  2002/07/30 00:04:07  borland
 First version.

 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <ctype.h>

#define CLO_PIPE 0
#define CLO_EVALUATE 1
#define CLO_OPTIONS 2

char *option[CLO_OPTIONS] = {
  "pipe", "evaluate",
};

static char *USAGE = "sddspoly [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
 -evaluate=filename=<polyFilename>,output=<column>,coefficients=<column>,input0=<inputColumn>,power0=<powerColumn>[,input1=<inputColumn>,power1=<polyColumn>][,...]\n\
 [-evaluate=...]\n\n\
 -pipe        Standard SDDS Toolkit pipe option.\n\
 -evaluate    Specifies evaluation of polynomial, specified in file\n\
              <polyFilename>.  The results of the evaluation are in \n\
              the <outputfile> under the name given with output=<column>.\n\
              The polynomial coefficients are in the column named with\n\
              coefficients=<column>.  The input<n> qualifiers give the\n\
              names of columns in <inputfile> that are inputs to the \n\
              polynomial.  The power<n> qualifiers give the names of\n\
              columns in the <polyFilename> file that give the powers\n\
              of the input<n> columns to use for each coefficient.\n\n\
 Program by Michael Borland.  (This is version 1.0, July 2002.)\n";

#define POLY_FILE_SEEN    0x0001U
#define POLY_OUTPUT_SEEN  0x0002U
#define POLY_COEF_SEEN    0x0004U
#define POLY_IN0_SEEN     0x0008U
#define POLY_OUT0_SEEN    0x0010U
#define POLY_IN1_SEEN     0x0020U
#define POLY_OUT1_SEEN    0x0040U
#define POLY_IN2_SEEN     0x0080U
#define POLY_OUT2_SEEN    0x0100U
#define POLY_IN3_SEEN     0x0200U
#define POLY_OUT3_SEEN    0x0400U
#define POLY_IN4_SEEN     0x0800U
#define POLY_OUT4_SEEN    0x1000U

typedef struct {
  unsigned long flags;
  char *filename, *outputColumn, *coefColumn;
  char **inputColumn, **powerColumn;
  long nInputs, nTerms;
  int32_t **power;
  double *coef;
  double **inputData, *input;
} POLYNOMIAL;

void initializePolynomial(POLYNOMIAL *poly, SDDS_DATASET *SDDSin, SDDS_DATASET *SDDSout);
double evaluatePoly(double *coef, int32_t **power, long nCoef,
                    double *input, long nInputs);
void FreePolynormialMemory(POLYNOMIAL *poly, long npoly);
 
int main(int argc, char **argv)
{
  POLYNOMIAL *poly;
  long nPoly, iPoly, row, rows, iInput;
  int iArg;
  char *input, *output;
  unsigned long pipeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double *outputData ;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3)
    bomb(NULL, USAGE);

  outputData = NULL;
  input = output = NULL;
  pipeFlags = 0;
  poly = NULL;
  nPoly = 0;

  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_EVALUATE:
        if (!(poly = SDDS_Realloc(poly, sizeof(*poly)*(nPoly+1))) ||
            !(poly[nPoly].inputColumn 
              =SDDS_Malloc(sizeof(*(poly[nPoly].inputColumn))*5)) ||
            !(poly[nPoly].powerColumn 
              =SDDS_Malloc(sizeof(*(poly[nPoly].powerColumn))*5)))
          SDDS_Bomb("memory allocation failure");
        scanned[iArg].n_items -= 1;
        if (!scanItemList(&poly[nPoly].flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "filename", SDDS_STRING, &(poly[nPoly].filename), 1, POLY_FILE_SEEN,
                          "output", SDDS_STRING, &(poly[nPoly].outputColumn), 1, POLY_OUTPUT_SEEN,
                          "coefficients", SDDS_STRING, &(poly[nPoly].coefColumn), 1, POLY_COEF_SEEN,
                          "input0", SDDS_STRING, poly[nPoly].inputColumn+0, 1, POLY_IN0_SEEN,
                          "power0", SDDS_STRING, poly[nPoly].powerColumn+0, 1, POLY_OUT0_SEEN,
                          "input1", SDDS_STRING, poly[nPoly].inputColumn+1, 1, POLY_IN1_SEEN,
                          "power1", SDDS_STRING, poly[nPoly].powerColumn+1, 1, POLY_OUT1_SEEN,
                          "input2", SDDS_STRING, poly[nPoly].inputColumn+2, 1, POLY_IN2_SEEN,
                          "power2", SDDS_STRING, poly[nPoly].powerColumn+2, 1, POLY_OUT2_SEEN,
                          "input3", SDDS_STRING, poly[nPoly].inputColumn+3, 1, POLY_IN3_SEEN,
                          "power3", SDDS_STRING, poly[nPoly].powerColumn+3, 1, POLY_OUT3_SEEN,
                          "input4", SDDS_STRING, poly[nPoly].inputColumn+4, 1, POLY_IN4_SEEN,
                          "power4", SDDS_STRING, poly[nPoly].powerColumn+4, 1, POLY_OUT4_SEEN,
                          NULL) ||
            !(poly[nPoly].flags&POLY_FILE_SEEN) || !(poly[nPoly].flags&POLY_OUTPUT_SEEN) ||
            !(poly[nPoly].flags&POLY_COEF_SEEN) || !(poly[nPoly].flags&POLY_IN0_SEEN) ||
            !(poly[nPoly].flags&POLY_OUT0_SEEN))
          SDDS_Bomb("invalid -evaluate syntax");
        nPoly++;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      default:
        fprintf(stderr, "error: unknown/ambiguous option: %s\n", 
                scanned[iArg].list[0]);
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

  processFilenames("sddspoly", &input, &output, pipeFlags, 0, NULL);

  if (nPoly==0)
    SDDS_Bomb("give at least one -evaluate option");

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (iPoly=0; iPoly<nPoly; iPoly++) 
    initializePolynomial(&poly[iPoly], &SDDSin, &SDDSout);
  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  while (SDDS_ReadPage(&SDDSin)>0) {
    rows = SDDS_CountRowsOfInterest(&SDDSin);
    if (!SDDS_CopyPage(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!(outputData = SDDS_Realloc(outputData, sizeof(*outputData)*rows)))
      SDDS_Bomb("memory allocation failure");
    for (iPoly=0; iPoly<nPoly; iPoly++) {
      for (iInput=0; iInput<poly[iPoly].nInputs; iInput++) {
        if (!(poly[iPoly].inputData[iInput]=
              SDDS_GetColumnInDoubles(&SDDSin, poly[iPoly].inputColumn[iInput])))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      for (row=0; row<rows; row++) {
        for (iInput=0; iInput<poly[iPoly].nInputs; iInput++)
          poly[iPoly].input[iInput] = poly[iPoly].inputData[iInput][row];
        outputData[row] = evaluatePoly(poly[iPoly].coef, poly[iPoly].power, 
                                       poly[iPoly].nTerms,
                                       poly[iPoly].input, poly[iPoly].nInputs);
      }
      if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_NAME,
                          outputData, rows, poly[iPoly].outputColumn))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  free(outputData);

  if (!SDDS_Terminate(&SDDSin)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  free_scanargs(&scanned,argc);
  FreePolynormialMemory(poly,nPoly);
  free(poly);
  
  return 0;
}

double evaluatePoly(double *coef, int32_t **power, long nCoef,
                    double *input, long nInputs)
{
  double sum = 0, term;
  long iCoef, iInput;
  for (iCoef=0; iCoef<nCoef; iCoef++) {
    term = coef[iCoef];
    for (iInput=0; iInput<nInputs; iInput++)
      term *= ipow(input[iInput], power[iInput][iCoef]);
    sum += term;
  }
  return sum;
}


void initializePolynomial(POLYNOMIAL *poly, SDDS_DATASET *SDDSin, SDDS_DATASET *SDDSout)
{
  long i;
  SDDS_DATASET SDDSpoly;
  char buffer[1024];
  
  for (i=1; i<5; i++) {
   if ((poly->flags&(POLY_IN0_SEEN<<(2*i))) &&
        !(poly->flags&(POLY_OUT0_SEEN<<(2*i))))
      SDDS_Bomb("input qualifier seen without matching output qualifier");
    if (!(poly->flags&(POLY_IN0_SEEN<<(2*i))) &&
        (poly->flags&(POLY_OUT0_SEEN<<(2*i))))
      SDDS_Bomb("output qualifier seen without matching input qualifier");
    if (!(poly->flags&(POLY_IN0_SEEN<<(2*i))) &&
        !(poly->flags&(POLY_OUT0_SEEN<<(2*i))))
      break; 
  }
  poly->nInputs = i;
  for (i++; i<5; i++) {
    if ((poly->flags&(POLY_IN0_SEEN<<(2*i))) ||
        (poly->flags&(POLY_OUT0_SEEN<<(2*i))))
      SDDS_Bomb("input<n> or output<n> qualifiers skipped");
  }

  if (!SDDS_InitializeInput(&SDDSpoly, poly->filename))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (SDDS_GetColumnIndex(&SDDSpoly,poly->outputColumn)==-1) {
    if (!SDDS_DefineSimpleColumn(SDDSout, poly->outputColumn, NULL, SDDS_DOUBLE))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);         
  } else {
    if (SDDS_CheckColumn(&SDDSpoly, poly->outputColumn, NULL, SDDS_ANY_NUMERIC_TYPE, 
                         stderr)!=SDDS_CHECK_OKAY)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  } 
  
  if (SDDS_CheckColumn(&SDDSpoly, poly->coefColumn, NULL, SDDS_ANY_NUMERIC_TYPE, 
                       stderr)!=SDDS_CHECK_OKAY)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<poly->nInputs; i++) {
    if (SDDS_CheckColumn(SDDSin, poly->inputColumn[i], NULL, SDDS_ANY_NUMERIC_TYPE,
                       stderr)!=SDDS_CHECK_OKAY)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (SDDS_CheckColumn(&SDDSpoly, poly->powerColumn[i], NULL, SDDS_ANY_NUMERIC_TYPE,
                         stderr)!=SDDS_CHECK_OKAY)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (SDDS_ReadPage(&SDDSpoly)<=0) {
    sprintf(buffer, "problem with file %s\n", poly->filename);
    SDDS_SetError(buffer);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if ((poly->nTerms=SDDS_RowCount(&SDDSpoly))<=0) {
    sprintf(buffer, "problem with file %s: no rows\n", poly->filename);
    SDDS_SetError(buffer);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!(poly->coef=SDDS_GetColumnInDoubles(&SDDSpoly, poly->coefColumn)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!(poly->power=SDDS_Malloc(sizeof(*(poly->power))*poly->nInputs)))
    SDDS_Bomb("memory allocation failure");
  for (i=0; i<poly->nInputs; i++) {
    if (!(poly->power[i]=
          SDDS_GetColumnInLong(&SDDSpoly, poly->powerColumn[i])))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDSpoly))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!(poly->input = SDDS_Malloc(sizeof(*(poly->input))*poly->nInputs)))
    SDDS_Bomb("memory allocation failure");
  if (!(poly->inputData = SDDS_Malloc(sizeof(*(poly->inputData))*poly->nInputs)))
    SDDS_Bomb("memory allocation failure");
}

void FreePolynormialMemory(POLYNOMIAL *poly, long npoly)
{
  long i,j;
  for (i=0;i<npoly;i++) {
    for (j=0;j<poly[i].nInputs;j++) {
      free(poly[i].power[j]);
      free(poly[i].inputData[j]);
      free(poly[i].inputColumn[j]);
      free(poly[i].powerColumn[j]);
    }
    free(poly[i].powerColumn);
    free(poly[i].inputColumn);
    free(poly[i].power);
    free(poly[i].input);
    free(poly[i].inputData);
    free(poly[i].coef);
    free(poly[i].filename);
    free(poly[i].coefColumn);
    free(poly[i].outputColumn);
  }
}

