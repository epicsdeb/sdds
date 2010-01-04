/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* Michael Borland, 1999
 $Log: sddseventhist.c,v $
 Revision 1.8  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.7  2005/11/15 00:06:06  jiaox
 Fixed a bug that normalize option does not work.

 Revision 1.6  2002/08/14 17:12:45  soliday
 Added Open License

 Revision 1.5  2001/01/10 19:35:37  soliday
 Standardized usage message.

 Revision 1.4  2000/12/07 23:26:10  borland
 Fixed a bug in counting events that occured when a single event sorted to
 the end of the list.

 Revision 1.3  1999/05/25 19:09:25  soliday
 Removed compiler warning on linux.

 Revision 1.2  1999/04/17 01:37:53  borland
 Much faster version that uses sorted event lists.

 Revision 1.1  1999/04/16 14:03:33  borland
 First version in repository.

 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_BINS 0
#define SET_LOWERLIMIT 1
#define SET_UPPERLIMIT 2
#define SET_DATACOLUMN 3
#define SET_BINSIZE 4
#define SET_NORMALIZE 5
#define SET_SIDES 6
#define SET_PIPE 7
#define SET_EVENTIDENTIFIER 8
#define SET_OVERLAPEVENT 9
#define N_OPTIONS 10

char *option[N_OPTIONS] = {
    "bins", "lowerlimit", "upperlimit", "datacolumn",
    "sizeofbins", "normalize", "sides", "pipe", "eventidentifier",
    "overlapevent",
    };

char *USAGE="sddseventhist [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
-dataColumn=<columnName> -eventIdentifier=<columnName> [-overlapEvent=<eventValue>]\n\
[{-bins=<number> | -sizeOfBins=<value>}] [-lowerLimit=<value>] [-upperLimit=<value>]\n\
[-sides] [-normalize[={sum|area|peak}]] \n";

static char *additional_help = "\n\
dataColumn       give the name of column to histogram.\n\
eventIdentifier  give the name of the column to use in identifying events.\n\
                 There will be one histogram for each unique value in this\n\
                 column.  The column must contain string or integer data;\n\
                 if string data, the values must be allowed SDDS column names.\n\
overlapEvent     If given, the histograms for all events are multiplied bin-by-bin\n\
                 with the histogram for the named event.\n\
bins             number of bins for histogram.\n\
sizeOfBins       size of bins for histogram.\n\
lowerLimit       lower limit of histogram.\n\
upperLimit       upper limit of histogram.\n\
normalize        normalize the histogram, as specified.\n\
sides            puts sides on the histogram, down to zero level.\n\
Program by Michael Borland.  (This is version 1, February 1999.)\n";

#define NORMALIZE_PEAK 0
#define NORMALIZE_AREA 1
#define NORMALIZE_SUM 2
#define NORMALIZE_NO 3
#define N_NORMALIZE_OPTIONS 4
char *normalize_option[N_NORMALIZE_OPTIONS] = {
    "peak", "area", "sum", "no"
    } ;

typedef struct {
  char *string;
  long events;
  double *data;
  /* Indices in output file of histogram and overlap */
  long histogramIndex, overlapIndex;
} EVENT_DATA;

typedef struct {
  char *string;
  double data;
} EVENT_PAIR;

static long setupOutputFile(SDDS_DATASET *outTable, char *outputfile, SDDS_DATASET *inTable,
                            char *inputfile, char *dataColumn, char *eventIDColumn, 
                            char *overlapEventID,
                            EVENT_DATA **eventDataRet, long *eventIDsRet, 
                            double **dataRet,
                            long bins, double binSize, long normalizeMode);
long makeEventHistogram(double *hist, long bins, double lowerLimit,
                        double dx, EVENT_DATA *eventRefData);
void makeEventOverlap(double *overlap, double *hist, double *overlapHist, long bins);
int event_cmpasc(const void *ep1, const void *ep2);

int main(int argc, char **argv)
{
  /* next line defines flags to keep track of what is set in command line: */
  long binsGiven, lowerLimitGiven, upperLimitGiven;
  SDDS_DATASET inTable, outTable;
  double *data;            /* pointer to the array to histogram */
  double *hist, *hist1;    /* to store the histogram */
  double *indep;           /* to store values of bin centers */
  double *overlap, *overlap1, *overlapHist;
  double lowerLimit, upperLimit; /* lower and upper limits in histogram */
  double givenLowerLimit, givenUpperLimit; /* lower and upper limits in histogram */
  double range, binSize;
  long bins;                /* number of bins in the histogram */
  char *dataColumn, *eventIDColumn, *overlapEventID;
  SCANNED_ARG *scanned;    /* scanned argument structure */
  char *inputfile, *outputfile; /* array of input files, output file */
  double dx;               /* spacing of bins in histogram */
  long pointsBinned;        /* number of points that are in histogram */
  long normalizeMode, doSides, verbose=0, readCode;
  unsigned long pipeFlags;
  long eventIDIndex, eventIDType, eventRefIDs;
  EVENT_DATA *eventRefData;
  char **eventID;
  long i, iEvent, firstData, points;
  
  SDDS_RegisterProgramName(argv[0]);
  
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3) {
    fprintf(stderr, "usage: %s\n", USAGE);
    fputs(additional_help, stderr);
    exit(1);
  }

  hist = overlap = overlapHist = NULL;
  binsGiven = lowerLimitGiven = upperLimitGiven = 0;
  binSize = doSides = 0;
  inputfile = outputfile = NULL;
  dataColumn = eventIDColumn = overlapEventID = NULL;
  normalizeMode = NORMALIZE_NO;
  pipeFlags = 0;

  for (i=1; i<argc; i++) {
    if (scanned[i].arg_type==OPTION) {
      switch (match_string(scanned[i].list[0], option, N_OPTIONS, 0)) {
      case SET_BINS:         /* set number of bins */
        if (binsGiven) 
          SDDS_Bomb("-bins specified more than once");
        binsGiven = 1;
        if (sscanf(scanned[i].list[1], "%ld", &bins)!=1 || bins<=0)
          SDDS_Bomb("invalid value for bins");
        break;
      case SET_LOWERLIMIT:
        if (lowerLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        lowerLimitGiven = 1;
        if (sscanf(scanned[i].list[1], "%lf", &givenLowerLimit)!=1)
          SDDS_Bomb("invalid value for lowerLimit");
        break;
      case SET_UPPERLIMIT:
        if (upperLimitGiven)
          SDDS_Bomb("-lowerLimit specified more than once");
        upperLimitGiven = 1;
        if (sscanf(scanned[i].list[1], "%lf", &givenUpperLimit)!=1)
          SDDS_Bomb("invalid value for upperLimit");
        break;
      case SET_DATACOLUMN:
        if (dataColumn)
          SDDS_Bomb("-dataColumn specified more than once");
        if (scanned[i].n_items!=2)
          SDDS_Bomb("invalid -dataColumn syntax---supply name");
        dataColumn = scanned[i].list[1];
        break;
      case SET_EVENTIDENTIFIER:
        if (eventIDColumn)
          SDDS_Bomb("-eventIdentifier specified more than once");
        if (scanned[i].n_items!=2)
          SDDS_Bomb("invalid -eventIdentifier syntax---supply name");
        eventIDColumn = scanned[i].list[1];
        break;
      case SET_OVERLAPEVENT:
        if (overlapEventID)
          SDDS_Bomb("-overlapEvent specified more than once");
        if (scanned[i].n_items!=2)
          SDDS_Bomb("invalid -overlapEvent syntax---supply value");
        overlapEventID = scanned[i].list[1];
        if (!strlen(overlapEventID)) 
          SDDS_Bomb("invalid -overlapEvent syntax---supply value");
        break;
      case SET_NORMALIZE:
        if (scanned[i].n_items==1)
          normalizeMode = NORMALIZE_SUM;
        else if (scanned[i].n_items!=2 || 
                 (normalizeMode=match_string(scanned[i].list[1], normalize_option, N_NORMALIZE_OPTIONS, 0))<0)
          SDDS_Bomb("invalid -normalize syntax");
        break;
      case SET_SIDES:
        doSides = 1;
        break;
      case SET_BINSIZE:
        if (sscanf(scanned[i].list[1], "%le", &binSize)!=1 || binSize<=0)
          SDDS_Bomb("invalid value for bin size");
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[i].list+1, scanned[i].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      default:
        fprintf(stderr, "error: option %s not recognized\n", scanned[i].list[0]);
        exit(0);
        break;
      }
    }
    else {
      /* argument is filename */
      if (!inputfile)
        inputfile = scanned[i].list[0];
      else if (!outputfile)
        outputfile = scanned[i].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }

  processFilenames("sddseventhist", &inputfile, &outputfile, pipeFlags, 0, NULL);

  if (binSize && binsGiven)
    SDDS_Bomb("give either -binSize or -bins, not both");
  if (!binsGiven)
    bins = 20;
  if (!dataColumn)
    SDDS_Bomb("-dataColumn must be given");
  if (!eventIDColumn)
    SDDS_Bomb("-eventIdentifier must be given");
  
  if (!(indep = SDDS_Malloc(sizeof(*indep)*(bins+2))) ||
      !(hist = SDDS_Malloc(sizeof(*hist)*(bins+2))) ||
      !(overlap = SDDS_Malloc(sizeof(*overlap)*(bins+2))) || 
      !(overlapHist = SDDS_Malloc(sizeof(*overlapHist)*(bins+2))))
    SDDS_Bomb("memory allocation failure");

  if (!SDDS_InitializeInput(&inTable, inputfile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (SDDS_GetColumnIndex(&inTable, dataColumn)<0)
    SDDS_Bomb("data column not found");
  if ((eventIDIndex=SDDS_GetColumnIndex(&inTable, eventIDColumn))<0)
    SDDS_Bomb("event ID column  not found");
  if ((eventIDType=SDDS_GetColumnType(&inTable, eventIDIndex))!=SDDS_STRING)
    SDDS_Bomb("event ID column must be string type");
  if (!SDDS_NUMERIC_TYPE(SDDS_GetNamedColumnType(&inTable, dataColumn)))
    SDDS_Bomb("data column must be numeric data type");
  

  data = NULL;
  firstData = 1;
  while ((readCode=SDDS_ReadPage(&inTable))>0) {
    if (readCode>1) 
      SDDS_Bomb("This program cannot process multiple page files");
    pointsBinned = 0;
    if ((points = SDDS_CountRowsOfInterest(&inTable))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!points)
      SDDS_Bomb("No data in file");
    if (!(data = SDDS_GetColumnInDoubles(&inTable, dataColumn)) ||
        !(eventID = SDDS_GetColumn(&inTable, eventIDColumn)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (!setupOutputFile(&outTable, outputfile, &inTable, inputfile, dataColumn,
                         eventIDColumn, overlapEventID, &eventRefData, &eventRefIDs,
                         &data, bins, binSize, normalizeMode))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (!lowerLimitGiven) {
      lowerLimit = 0;
      if (points)
        lowerLimit = data[0];
      for (i=0; i<points; i++) 
        if (lowerLimit>data[i])
          lowerLimit = data[i];
    }
    else
      lowerLimit = givenLowerLimit;
    if (!upperLimitGiven) {
      upperLimit = 0;
      if (points)
        upperLimit = data[0];
      for (i=0; i<points; i++) 
        if (upperLimit<data[i])
          upperLimit = data[i];
    }
    else
      upperLimit = givenUpperLimit;
      
    range = upperLimit-lowerLimit;
    if (!lowerLimitGiven)
      lowerLimit -= range*1e-7;
    if (!upperLimitGiven)
      upperLimit += range*1e-7;
    if (upperLimit==lowerLimit) {
      if (binSize) {
        upperLimit += binSize/2;
        lowerLimit -= binSize/2;
      } else  if (fabs(upperLimit)<sqrt(DBL_MIN)) {
        upperLimit = sqrt(DBL_MIN);
        lowerLimit = -sqrt(DBL_MIN);
      } else {
        upperLimit += upperLimit*(1+2*DBL_EPSILON);
        lowerLimit -= upperLimit*(1-2*DBL_EPSILON);
      }
    }
    dx = (upperLimit-lowerLimit)/bins;
      
    if (binSize) {
      double middle;
      range = ((range/binSize)+1)*binSize;
      middle = (lowerLimit+upperLimit)/2;
      lowerLimit = middle-range/2;
      upperLimit = middle+range/2;
      dx = binSize;
      bins = range/binSize+0.5;
      if (bins<1 && !doSides)
        bins = 2;
      if (!(indep = SDDS_Realloc(indep, sizeof(*indep)*(bins+2))) ||
          !(hist = SDDS_Realloc(hist, sizeof(*hist)*(bins+2))) ||
          !(overlap = SDDS_Realloc(overlap, sizeof(*overlap)*(bins+2))) ||
          !(overlapHist = SDDS_Realloc(overlapHist, sizeof(*overlapHist)*(bins+2))))
        SDDS_Bomb("memory allocation failure");
    }
    hist1 = hist+1;
    overlap1 = overlap+1;
    
    for (i=-1; i<bins+1; i++) {
      indep[i+1] = (i+0.5)*dx + lowerLimit;
    }
    if (!SDDS_StartPage(&outTable, points?(doSides?bins+2:bins):0) ||
        !SDDS_CopyParameters(&outTable, &inTable) ||
        (points && 
         (!SDDS_SetColumnFromDoubles(&outTable, SDDS_SET_BY_NAME, indep+(doSides?0:1),
                          doSides?bins+2:bins, dataColumn))))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (overlapEventID) {
      for (iEvent=0; iEvent<eventRefIDs; iEvent++) {
        if (strcmp(eventRefData[iEvent].string, overlapEventID)==0)
          break;
      }
      if (iEvent==eventRefIDs)
        SDDS_Bomb("Can't make overlap as overlap event is not present");
      makeEventHistogram(overlapHist, bins, lowerLimit, dx, 
                         eventRefData+iEvent);
    }
    for (iEvent=pointsBinned=0; iEvent<eventRefIDs; iEvent++) {
      pointsBinned += makeEventHistogram(hist, bins, lowerLimit, dx,
                                         eventRefData+iEvent);
      if(normalizeMode!=NORMALIZE_NO) {
        double norm = 0;
	switch (normalizeMode) {
	case NORMALIZE_PEAK:
	  norm = max_in_array(hist,bins);
	  break;
	case NORMALIZE_AREA:
	case NORMALIZE_SUM:
	  for (i=norm=0;i<bins;i++)
	    norm +=hist[i];
	  if (normalizeMode==NORMALIZE_AREA)
	    norm *= dx;
	  break;
	default:
	  SDDS_Bomb("invalid normalize mode--consult programmer.");
	  break;
	}
       	if (norm)
	  for (i=0;i<bins;i++)
	    hist[i] /= norm;
      }  	         
      
      if (!SDDS_SetColumnFromDoubles(&outTable, SDDS_SET_BY_INDEX, 
                          hist+(doSides?0:1),
                          doSides?bins+2:bins, eventRefData[iEvent].histogramIndex))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (overlapEventID) {
        makeEventOverlap(overlap, hist, overlapHist, bins+2);
        if (!SDDS_SetColumnFromDoubles(&outTable, SDDS_SET_BY_INDEX,
                                       overlap+(doSides?0:1),
                                       doSides?bins+2:bins, eventRefData[iEvent].overlapIndex))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    
    if (!SDDS_SetParameters(&outTable, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                            "sddseventhistBins", bins, 
                            "sddseventhistBinSize", dx, 
                            "sddseventhistPointsBinned", pointsBinned, 
                            NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    if (verbose)
      fprintf(stderr, "%ld points of %ld from page %ld histogrammed in %ld bins\n", 
              pointsBinned, points, readCode, bins);
    
    if (!SDDS_WritePage(&outTable))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  free(hist);
  free(overlapHist);
  free(overlap);
  return(0);
}


long setupOutputFile(SDDS_DATASET *outTable, char *outputfile, SDDS_DATASET *inTable,
                            char *inputfile, char *dataColumn, char *eventIDColumn, 
                            char *overlapEventID,
                            EVENT_DATA **eventDataRet, long *eventIDsRet, 
                            double **dataRet, long bins, double binSize, long normalizeMode)
{
  char **eventValue, buffer[SDDS_MAXLINE];
  long eventRows, uniqueRows, row;
  EVENT_DATA *eventData;
  EVENT_PAIR *eventPair;
  double *eventDataValue;
  long row0, iEvent, drow;
  
  if (!SDDS_InitializeOutput(outTable, SDDS_BINARY, 0, NULL, "sddseventhist output", outputfile) ||
      !SDDS_TransferColumnDefinition(outTable, inTable, dataColumn, NULL) ||
      (eventRows=SDDS_RowCount(inTable))==0 ||
      !(eventValue=SDDS_GetColumn(inTable, eventIDColumn)) ||
      !(*dataRet=eventDataValue=SDDS_GetColumnInDoubles(inTable, dataColumn)))
    return 0;
  if (!(eventPair = SDDS_Malloc(sizeof(*eventPair)*eventRows)))
    SDDS_Bomb("Memory allocation failure");

  /* Put (event-string, event-data) pairs into a structure and sort the structures 
   * by string/data 
   */
  for (row=0; row<eventRows; row++) {
    eventPair[row].string = eventValue[row];
    eventPair[row].data   = eventDataValue[row];
  }
  qsort(eventPair, eventRows, sizeof(*eventPair), event_cmpasc);

  /* Find out how many unique events there are */
  uniqueRows = 1;
  for (row=1; row<eventRows; row++) {
    if (strcmp(eventPair[row-1].string, eventPair[row].string)==0)
      continue;
    else 
      uniqueRows++;
  }
  *eventIDsRet = uniqueRows;

  /* Compose structures with (string, data-array) */
  if (!(eventData = *eventDataRet = SDDS_Malloc(sizeof(**eventDataRet)*uniqueRows)))
    SDDS_Bomb("Memory allocation failure");
  row0 = 0;
  iEvent = 0;
  for (row=1; row<eventRows; row++) {
    if (row==(eventRows-1) || strcmp(eventPair[row].string, eventPair[row0].string)!=0) {
      /* [row0, row-1] have the same event name */
      eventData[iEvent].events = row-row0;
      if (!SDDS_CopyString(&eventData[iEvent].string, eventPair[row0].string) ||
          !(eventData[iEvent].data=SDDS_Malloc(sizeof(*eventData[iEvent].data)*eventData[iEvent].events)))
        SDDS_Bomb("Memory allocation failure");
      for (drow=0; drow<eventData[iEvent].events; drow++) {
        eventData[iEvent].data[drow] = eventPair[row0+drow].data;
        /* free(eventPair[row0+drow].string); */
      } 
      if (row==(eventRows-1) && strcmp(eventPair[row].string, eventPair[row0].string)!=0) {
        /* this is necessary to pick up the last event if it is a loner */
        row0 = row;
        row--;
      } else {
        row0 = row;
      }
      iEvent++;
    }
  }
  free(eventPair);
  if (iEvent!=uniqueRows)
    SDDS_Bomb("Unique row count mismatch");
  
  
  if (overlapEventID && strlen(overlapEventID)) {
    for (iEvent=0; iEvent<uniqueRows; iEvent++) {
      if (strcmp(overlapEventID, eventData[iEvent].string)==0)
        break;
    }
    if (iEvent==uniqueRows)
      SDDS_Bomb("overlap event not found");
  }
  
  for (row=0; row<uniqueRows; row++) {
    sprintf(buffer, "%sFrequency", eventData[row].string);
    if ((eventData[row].histogramIndex
         =SDDS_DefineColumn(outTable, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    eventData[row].overlapIndex = -1;
    if (overlapEventID) {
      sprintf(buffer, "%s.%sOverlap", eventData[row].string, overlapEventID);
      if ((eventData[row].overlapIndex
         =SDDS_DefineColumn(outTable, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
        return 0;
    }
  }
  
  if (SDDS_DefineParameter(outTable, "sddseventhistInput", NULL, NULL, NULL, NULL, SDDS_STRING, 
                           inputfile)<0 ||
      SDDS_DefineParameter(outTable, "sddseventhistBins", NULL, NULL, NULL, NULL, SDDS_LONG,
                           NULL)<0 ||
      SDDS_DefineParameter(outTable, "sddseventhistBinSize", NULL, NULL, NULL, NULL,
                           SDDS_DOUBLE, NULL)<0 ||
      SDDS_DefineParameter(outTable, "sddseventhistPointsBinned", NULL, NULL, NULL, NULL, SDDS_LONG,
                           NULL)<0 ||
      SDDS_DefineParameter(outTable, "sddseventhistEventIDColumn", NULL, NULL, NULL, NULL,
                           SDDS_STRING, eventIDColumn)<0 ||
      SDDS_DefineParameter(outTable, "sddshistNormMode", NULL, NULL, NULL, NULL, SDDS_STRING, 
                           normalize_option[normalizeMode])<0 ||
      !SDDS_TransferAllParameterDefinitions(outTable, inTable, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(outTable))
    return 0;
  return 1;
}

long makeEventHistogram(double *hist, long bins, double lowerLimit,
                        double dx, EVENT_DATA *eventRefData)
{
  long iBin, iRow, pointsBinned;
  double *hist1;
  hist1 = hist+1;
  for (iBin=0; iBin<=bins+1; iBin++)
    hist[iBin] = 0;
  for (iRow=0; iRow<eventRefData->events; iRow++) {
    iBin = (eventRefData->data[iRow]-lowerLimit)/dx;
    if (iBin>=0 || iBin<bins)
      hist1[iBin] += 1;
  }
  for (iBin=pointsBinned=0; iBin<bins; iBin++)
    pointsBinned += hist1[iBin];
  return pointsBinned;
}

void makeEventOverlap(double *overlap, double *hist, double *overlapHist, long bins)
{
  long i;
  for (i=0; i<bins; i++) {
    overlap[i] = MIN(hist[i], overlapHist[i]);
  }
}

int event_cmpasc(const void *ep1, const void *ep2)
{
  EVENT_PAIR *ev1, *ev2;
  int comp;
  ev1 = (EVENT_PAIR*)ep1;
  ev2 = (EVENT_PAIR*)ep2;
  if ((comp=strcmp(ev1->string, ev2->string)))
    return comp;
  return ev1->data>ev2->data;
}

