/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: not supported by cvs2svn $
 Revision 1.5  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.4  2003/09/02 19:16:03  soliday
 Cleaned up code for Linux.

 Revision 1.3  2002/08/14 17:12:41  soliday
 Added Open License

 Revision 1.2  2002/02/08 23:30:53  borland
 Now allows the absolute limit to be 0.

 Revision 1.1  2001/04/30 22:55:23  borland
 Added sddscliptails and sddsbaseline to toolkit.

 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define CLO_FRACTIONAL 0
#define CLO_ABSOLUTE 1
#define CLO_FWHM 2
#define CLO_PIPE 3
#define CLO_COLUMNS 4
#define CLO_AFTERZERO 5
#define CLO_OPTIONS 6

static char *option[CLO_OPTIONS] = {
  "fractional", "absolute", "fwhm", "pipe", "columns", "afterzero",
    } ;

char *USAGE = "sddscliptails [<input>] [<output>] [-pipe=[in][,out]]\n\
[-columns=<listOfNames>] [-fractional=<value>] [-absolute=<value>] [-fwhm=<multiplier>]\n\
[-afterzero[=<bufferWidth>]]\n\n\
-columns      List of columns to process.\n\
-fractional   Clip a tail if it falls below this fraction of the peak.\n\
-absolute     Clip a tail if it falls below this absolute value.\n\
-fwhm         Clip a tail if it is beyond this many FWHM from the peak.\n\
-afterzero    Clip a tail if it is separated from the peak by values equal to zero.\n\
              If <bufferWidth> is specified, then a region <bufferWidth> wide is kept\n\
              on either side of the peak, if possible.\n\n\
Program by Michael Borland. (This is version 1, April 2001.)\n";

long resolveColumnNames(SDDS_DATASET *SDDSin, char ***column, int32_t *columns);
void clipTail(double *data, long rows, double absLimit, double fracLimit, short *inTail);
void clipFWHM(double *data, long n_data, double fwhmLimit, double *indepData, short *inTail);
void clipAfterZero(double *data, long rows, long bufferWidth, short *inTail);

int main(int argc, char **argv)
{
  int iArg;
  char **inputColumn;
  char *input, *output;
  long i, rows, readCode, afterZero, afterZeroBufferWidth;
  int32_t columns;
  unsigned long pipeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double *data, *indepData;
  double fractionalLimit, absoluteLimit, fwhmLimit;
  short *inTail;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2)
    bomb(NULL, USAGE);

  output = input = NULL;
  inputColumn = NULL;
  columns = 0;
  pipeFlags = 0;
  fractionalLimit = fwhmLimit = 0;
  absoluteLimit = -1;
  afterZero = 0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_COLUMNS:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        inputColumn = tmalloc(sizeof(*inputColumn)*(columns = scanned[iArg].n_items-1));
        for (i=0; i<columns; i++)
          inputColumn[i] = scanned[iArg].list[i+1];
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_FRACTIONAL:
        if (scanned[iArg].n_items<2 ||
            sscanf(scanned[iArg].list[1], "%lf", &fractionalLimit)!=1 ||
            fractionalLimit<0)
          SDDS_Bomb("invalid -fractional syntax");
        break;
      case CLO_ABSOLUTE:
        if (scanned[iArg].n_items<2 ||
            sscanf(scanned[iArg].list[1], "%lf", &absoluteLimit)!=1 ||
            absoluteLimit<0)
          SDDS_Bomb("invalid -absolute syntax");
        break;
      case CLO_FWHM:
        if (scanned[iArg].n_items<2 ||
            sscanf(scanned[iArg].list[1], "%lf", &fwhmLimit)!=1 ||
            fwhmLimit<0)
          SDDS_Bomb("invalid -fwhm syntax");
        break;
      case CLO_AFTERZERO:
        afterZero = 1;
        if (scanned[iArg].n_items>2 ||
            (scanned[iArg].n_items==2 &&
             (sscanf(scanned[iArg].list[1], "%ld", &afterZeroBufferWidth)!=1 ||
              afterZeroBufferWidth<=0)))
          SDDS_Bomb("invalid -afterZero syntax");
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

  processFilenames("sddscliptails", &input, &output, pipeFlags, 0, NULL);

  if (!columns)
    SDDS_Bomb("supply the names of columns to process with the -columns option");

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!resolveColumnNames(&SDDSin, &inputColumn, &columns))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!columns)
    SDDS_Bomb("no columns selected for processing");

  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_DefineColumn(&SDDSout, "InTail", NULL, NULL, NULL, NULL, SDDS_SHORT, 0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  indepData = NULL;
  inTail = NULL;
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if (!SDDS_CopyPage(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))) {
      if (fwhmLimit>0) {
        if (!(indepData = SDDS_Realloc(indepData, sizeof(*indepData)*rows)))
          SDDS_Bomb("memory allocation failure");
        for (i=0; i<rows; i++)
          indepData[i] = i;
      }
      if (!(inTail = SDDS_Realloc(inTail, sizeof(*inTail)*rows)))
        SDDS_Bomb("memory allocation failure");
      for (i=0; i<rows; i++)
        inTail[i] = 0;
      for (i=0; i<columns; i++) {
        if (!(data = SDDS_GetColumnInDoubles(&SDDSin, inputColumn[i])))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        clipTail(data, rows, absoluteLimit, fractionalLimit, inTail);
        if (fwhmLimit>0)
          clipFWHM(data, rows, fwhmLimit, indepData, inTail);
        if (afterZero)
          clipAfterZero(data, rows, afterZeroBufferWidth, inTail);
        if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, data, rows, inputColumn[i]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        free(data);
      }
    }
    if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_NAME, inTail, rows, "InTail"))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (indepData)
    free(indepData);
  if (inTail)
    free(inTail);
  return(0);
}


long resolveColumnNames(SDDS_DATASET *SDDSin, char ***column, int32_t *columns)
{
  long i;

  SDDS_SetColumnFlags(SDDSin, 0);
  for (i=0; i< *columns; i++) {
    if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, (*column)[i], SDDS_OR))
      return 0;
  }
  free(*column);
  if (!(*column = SDDS_GetColumnNames(SDDSin, columns)) || *columns==0) {
    SDDS_SetError("no columns found");
    return 0;
  }
  return 1; 
}

void clipTail(double *data, long rows, double absLimit, double fracLimit, short *inTail)
{
  long i, clip, imin, imax;
  double absLimit2;
  
  if (absLimit<0 && fracLimit<=0)
    return;
  if (rows<3)
    return;
  
  index_min_max(&imin, &imax, data, rows);
  if (!data[imax])
    return;
  
  absLimit2 = fracLimit*data[imax];
  if (absLimit<0 || (fracLimit && absLimit2<absLimit))
    absLimit = absLimit2;
  
  if (absLimit<0)
    return;
  
  for (i=imax-1, clip=0; i>=0; i--) {
    if (!clip && data[i]<absLimit)
      clip = 1;
    if (clip) {
      inTail[i] += 1;
      data[i] = 0;
    }
  }

  for (i=imax+1, clip=0; i<rows; i++) {
    if (!clip && data[i]<absLimit)
      clip = 1;
    if (clip) {
      inTail[i] += 1;
      data[i] = 0;
    }
  }
}

void clipFWHM(double *data, long n_data, double fwhmLimit, double *indepData, short *inTail)
{
  double top, base, fwhm;
  long i1, i2, i2save, imin, imax, i;
  double point1, point2;
  
  if (n_data<3 || fwhmLimit<=0)
    return;

  index_min_max(&imin, &imax, data, n_data);
  if (!data[imax])
    return;
  
  if (!findTopBaseLevels(&top, &base, data, n_data, 50, 2.0)) 
    return;

  if ((i1=findCrossingPoint( 0, data, n_data, top*0.5,  1, indepData, &point1))<0 ||
      (i2=i2save=findCrossingPoint(i1, data, n_data, top*0.9, -1, NULL, NULL))<0 ||
      (i2=findCrossingPoint(i2, data, n_data, top*0.5, -1, indepData, &point2))<0 ) {
    return;
  }
  fwhm = point2 - point1;
  
  for (i=imax+fwhm*fwhmLimit; i<n_data; i++) {
    inTail[i] += 1;
    data[i] = 0;
  }
  for (i=imax-fwhm*fwhmLimit; i>=0; i--) {
    inTail[i] += 1;
    data[i] = 0;
  }
}

void clipAfterZero(double *data, long rows, long bufferWidth, short *inTail)
{
  long imin, imax, i, j;
  index_min_max(&imin, &imax, data, rows);
  if (!data[imax])
    return;
  for (i=imax+1; i<rows; i++) {
    if (data[i]==0) {
      for (j=i+bufferWidth; j<rows; j++) {
        inTail[j] += 1;
        data[j] = 0;
      }
      break;
    }
  }
  
  for (i=imax-1; i>=0; i--) {
    if (data[i]==0) {
      for (j=i-bufferWidth; j>=0; j--) {
        inTail[j] += 1;
        data[j] = 0;
      }
      break;
    }
  }
  
}

