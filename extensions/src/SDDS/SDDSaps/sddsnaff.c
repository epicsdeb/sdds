/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsnaff
 *
 * $Log: sddsnaff.c,v $
 * Revision 1.15  2006/12/14 22:21:59  soliday
 * Updated a bunch of programs because SDDS_SaveLayout is now called by
 * SDDS_WriteLayout and it is no longer required to be called directly.
 * Also the AutoCheckMode is turned off by default now so I removed calls to
 * SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 * the programmer to turn it on in new programs until debugging is completed
 * and then remove the call to SDDS_SetAutoCheckMode.
 *
 * Revision 1.14  2005/11/04 22:46:16  soliday
 * Updated code to be compiled by a 64 bit processor.
 *
 * Revision 1.13  2005/06/13 22:00:55  shang
 * added conjugate pair analysis
 *
 * Revision 1.12  2004/08/11 15:18:00  borland
 * Include Significance output.
 *
 * Revision 1.11  2004/04/07 16:50:21  borland
 * Added extra options to PerformNAFF() call.  They are nonfunctional in this instance.
 *
 * Revision 1.10  2003/09/02 19:16:05  soliday
 * Cleaned up code for Linux.
 *
 * Revision 1.9  2002/08/14 17:12:50  soliday
 * Added Open License
 *
 * Revision 1.8  2002/02/26 03:14:29  borland
 * Removed NAFF functions from this program and put them in fftpack and
 * mdbmth areas.
 *
 * Revision 1.7  2001/11/30 16:09:50  borland
 * Changed a varaible name to resolve collision on HPUX.
 *
 * Revision 1.6  2001/11/21 22:16:02  borland
 * Fixed problem with near-infinite loop in phase computation.
 *
 * Revision 1.4  2001/11/01 14:43:19  borland
 * Added computation and output of the phase.
 *
 * Revision 1.3  2001/10/28 00:59:32  borland
 * Fixed program name in processFilenames call.
 *
 * Revision 1.2  2001/10/19 21:45:42  borland
 * Frequency and amplitude are -1 if no component is found.
 *
 * Revision 1.1  2001/10/19 20:59:33  borland
 * First version.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "fftpackC.h"
#include "SDDSutils.h"
#include <ctype.h>

#define SET_TRUNCATE 0
#define SET_COLUMN 1
#define SET_EXCLUDE 2
#define SET_PIPE 3
#define SET_NOWARNINGS 4
#define SET_TERM_SEARCH 5
#define SET_ITERATE_FREQ 6
#define SET_PAIR 7
#define N_OPTIONS 8

char *option[N_OPTIONS] = {
    "truncate", "columns", "exclude", "pipe", "nowarnings",
    "terminatesearch", "iteratefrequency", "pair"
    };

static char *USAGE1="sddsnaff [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-columns=<indep-variable>[,<depen-quantity>[,...]]] \n\
[-pair=<column1>,<column2>] \n\
[-exclude=<depen-quantity>[,...]]\n\
[-terminateSearch={changeLimit=<fraction>[,maxFrequencies=<number>] | frequencies=<number>}]\n\
[-iterateFrequency=[cycleLimit=<number>][,accuracyLimit=<fraction>]]\n\
[-truncate] [-noWarnings]\n\n\
\n\
Determines frequency components of signals using Laskar's method\n\
of Numerical Analysis of Fundamental Frequencies (NAFF). FFTs are\n\
involved in this process, hence some of the parameters refer to FFTs.\n\n\
-pipe             The standard SDDS Toolkit pipe option.\n\
-column           Provides the name of the independent variable, \n\
                  and the data to analyze if no -pair options given.  \n\
                  <depen-quantity> entries may contain wildcards.\n\
-pair             provides the names of two columns, where the first column is used to \n\
                  obtain the basic frequency, the second columns is used to obtain the \n\
                  phase at the frequency of the first column. The relative phase between \n\
                  them will expand the resulting frequency from 0 - Fn to 0 - 2*Fn.\n\
                  as many as -pair options can be provided. The <depen-quantity> entries \n\
                  may be provided by either -column option or -pair option.\n\
-exclude          Specifies a list of wild-card patterns to use in excluding\n\
                  quantities from analysis.\n";
static char *USAGE2=
"-terminateSearch  Specifies terminating the search for frequency components\n\
                  when the RMS of the residual function fails to change by the\n\
                  specified fraction, or after determining the given number of\n\
                  frequencies.  By default, terminates when 1 frequency is found.\n\
                  If changeLimit is given, the default maximum number of frequencies\n\
                  that will be returned is 4.\n\
-iterateFrequency  Specifies how hard to try to do determine each frequency.\n\
                  You can request a given accuracy (as a fraction of Nyquist)\n\
                  and/or limit the number of cycles used.  By default, no more\n\
                  than 100 cycles are performed and an accuracy of 0.00001 is attempted.\n\
-truncate         Requests truncation of the data if the number of data points is\n\
                  not a product of small prime numbers.  May substantially reduce run\n\
                  time by making FFTs faster.\n\
-noWarnings       Suppresses warning messages.\n\n\
Program by Michael Borland.  (This is version 1, October 2001.)\n";

long SetupNAFFOutput(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin,
		     char *indepQuantity, 
		     long depenQuantities, char **depenQuantity,
		     long **frequencyIndex, long **amplitudeIndex,
                     long **phaseIndex, long **significanceIndex, 
                     char **depenQuantityPair, long **amplitudeIndex1,
                     long **phaseIndex1, long **significanceIndex1);

#ifdef DEBUG
double trialFn(double x, long *invalid)
{
  *invalid = 0;
  x -= 1;
  if (x==0)
    return 1;
  return (sin(x)/x + sqr(x-0.5)*x*0.5);
}
#endif


int main(int argc, char **argv)
{
  char *indepQuantity, **depenQuantity, **exclude, **depenQuantityPair;
  long depenQuantities, excludes;
  char *input, *output;
  long iArg, i, j, rows, readCode, noWarnings, items;
  long rowsToUse;
  unsigned long flags, pairFlags, tmpFlags, pipeFlags;
  SCANNED_ARG *scArg;
  SDDS_DATASET SDDSin, SDDSout;
  double *tdata, *data, t0, dt;
  double fracRMSChangeLimit, fracFreqAccuracyLimit;
  int32_t frequenciesDesired, maxFrequencies, freqCycleLimit;
  short truncate;
  double *frequency, *amplitude=NULL, *phase=NULL, *significance=NULL, *phase1=NULL, *amplitude1=NULL, *significance1=NULL;
  long frequenciesFound;
  long *frequencyIndex, *amplitudeIndex, *phaseIndex, *significanceIndex, pairs;
  long *amplitudeIndex1, *phaseIndex1, *significanceIndex1;

  SDDS_RegisterProgramName(argv[0]);

#ifdef DEBUG
  if (1) {
    long code;
    double x, y; 
    x = 1.1;
    code = OneDFunctionOptimize(&y, &x, 0.07, -4, 4, 
				trialFn, 50, 1e-6, 0, 1);
    fprintf(stderr, "code: %ld   x=%e,  y=%e\n", code, x, y);
    
    x = .9;
    code = OneDFunctionOptimize(&y, &x, 0.15, -4, 4, 
				trialFn, 50, 1e-6, 0, 1);
    fprintf(stderr, "code: %ld   x=%e,  y=%e\n", code, x, y);
    
    x = .999;
    code = OneDFunctionOptimize(&y, &x, 0.11, -4, 4, 
				trialFn, 50, 1e-6, 0, 1);
    fprintf(stderr, "code: %ld   x=%e,  y=%e\n", code, x, y);
    exit(0);
  }
#endif

  argc = scanargs(&scArg, argc, argv); 
  if (argc<3) {
    fprintf(stderr, "%s%s", USAGE1, USAGE2);
    exit(1);
  }
  output = input = NULL;
  flags = pipeFlags = excludes = truncate = pairFlags= 0;
  indepQuantity = NULL;
  depenQuantity = exclude = depenQuantityPair=NULL;
  depenQuantities = 0;
  noWarnings = 0;
  fracRMSChangeLimit = 0.0;
  fracFreqAccuracyLimit = 0.00001;
  frequenciesDesired = 1;
  maxFrequencies = 4;
  freqCycleLimit = 100;
  pairs=0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scArg[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scArg[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_TRUNCATE:
	truncate = 1;
	break;
      case SET_PAIR:
        if (depenQuantities)
          SDDS_Bomb("Invalid -pair option, the depent-quantity is provided by -column option already.");
        if (scArg[iArg].n_items!=3) 
          SDDS_Bomb("invalid -pair syntax");
        depenQuantity = SDDS_Realloc(depenQuantity, sizeof(*depenQuantity)*(pairs+1));
        depenQuantityPair = SDDS_Realloc(depenQuantityPair, sizeof(*depenQuantityPair)*(pairs+1));
        depenQuantity[pairs] = scArg[iArg].list[1];
        depenQuantityPair[pairs] = scArg[iArg].list[2];
        pairs++;
        break;
      case SET_COLUMN:
	if (indepQuantity)
	  SDDS_Bomb("only one -column option may be given");
	if (scArg[iArg].n_items<2)
	  SDDS_Bomb("invalid -column syntax");
	indepQuantity = scArg[iArg].list[1];
	if (scArg[iArg].n_items>=2) {
          if (pairs)
            SDDS_Bomb("Invalid -column syntax, the depent-quantity is provided by -pair option already.");
	  depenQuantity = tmalloc(sizeof(*depenQuantity)*(depenQuantities=scArg[iArg].n_items-2));
	  for (i=0; i<depenQuantities; i++)
	    depenQuantity[i] = scArg[iArg].list[i+2];
	}
	break;
      case SET_PIPE:
	if (!processPipeOption(scArg[iArg].list+1, scArg[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_EXCLUDE:
	if (scArg[iArg].n_items<2)
	  SDDS_Bomb("invalid -exclude syntax");
	moveToStringArray(&exclude, &excludes, scArg[iArg].list+1, scArg[iArg].n_items-1);
	break;
      case SET_NOWARNINGS:
	noWarnings = 1;
	break;
      case SET_TERM_SEARCH:
	items = scArg[iArg].n_items-1;
	flags &= ~(NAFF_RMS_CHANGE_LIMIT|NAFF_FREQS_DESIRED|NAFF_MAX_FREQUENCIES);
	fracRMSChangeLimit = 0;
	frequenciesDesired = 0;
	maxFrequencies = 10;
	if (!scanItemList(&tmpFlags, scArg[iArg].list+1, &items, 0,
			  "changelimit", SDDS_DOUBLE, &fracRMSChangeLimit, 1, 
			  NAFF_RMS_CHANGE_LIMIT,
			  "maxfrequencies", SDDS_LONG, &maxFrequencies, 1, 
			  NAFF_MAX_FREQUENCIES,
			  "frequencies", SDDS_LONG, &frequenciesDesired, 1, 
			  NAFF_FREQS_DESIRED,
			  NULL) ||
	    (tmpFlags&NAFF_RMS_CHANGE_LIMIT && tmpFlags&NAFF_FREQS_DESIRED) ||
	    maxFrequencies<1)
	  SDDS_Bomb("invalid -terminateSearch syntax");
	flags |= tmpFlags;
	if (frequenciesDesired)
	  maxFrequencies = frequenciesDesired;
        break;
      case SET_ITERATE_FREQ:
	items = scArg[iArg].n_items - 1;
	flags &= ~(NAFF_FREQ_CYCLE_LIMIT|NAFF_FREQ_ACCURACY_LIMIT);
	if (!scanItemList(&tmpFlags, scArg[iArg].list+1, &items, 0,
			  "cyclelimit", SDDS_LONG, &freqCycleLimit, 1, NAFF_FREQ_CYCLE_LIMIT,
			  "accuracylimit", SDDS_DOUBLE, &fracFreqAccuracyLimit, 
			  1, NAFF_FREQ_ACCURACY_LIMIT,
			  NULL) || !bitsSet(tmpFlags) || freqCycleLimit<2)
	  SDDS_Bomb("invalid -iterateFrequency syntax");
	flags |= tmpFlags;
        break;
      default:
	fprintf(stderr, "error: unknown/ambiguous option: %s\n", 
		scArg[iArg].list[0]);
	exit(1);
	break;
      }
    }
    else {
      if (!input)
	input = scArg[iArg].list[0];
      else if (!output)
	output = scArg[iArg].list[0];
      else
	SDDS_Bomb("too many filenames seen");
    }
  }
  
  processFilenames("sddsnaff", &input, &output, pipeFlags, 0, NULL);
  
  if (!indepQuantity)
    SDDS_Bomb("supply the independent quantity name with the -columns option");
  
  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (SDDS_CheckColumn(&SDDSin, indepQuantity, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)
      !=SDDS_CHECK_OKAY) 
    exit(1);
  
  excludes = appendToStringArray(&exclude, excludes, indepQuantity);
  if (pairs) {
    pairFlags = flags | NAFF_FREQ_FOUND;
    depenQuantities = pairs;
  }
  if (!depenQuantities)
    depenQuantities = appendToStringArray(&depenQuantity, depenQuantities, "*"); 
  if (!pairs) {
    if ((depenQuantities=expandColumnPairNames(&SDDSin, &depenQuantity, NULL, 
                                               depenQuantities, exclude, excludes, 
                                               FIND_NUMERIC_TYPE, 0))<=0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      SDDS_Bomb("No quantities selected to fft");
    }
  }
  
  if (!SetupNAFFOutput(&SDDSout, output, &SDDSin, indepQuantity,
		       depenQuantities, depenQuantity,
		       &frequencyIndex, &amplitudeIndex, &phaseIndex,
                       &significanceIndex, depenQuantityPair,&amplitudeIndex1,
                       &phaseIndex1,&significanceIndex1)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (!(frequency = SDDS_Malloc(sizeof(*frequency)*maxFrequencies)) ||
      !(amplitude = SDDS_Malloc(sizeof(*amplitude)*maxFrequencies)) ||
      !(phase     = SDDS_Malloc(sizeof(*phase    )*maxFrequencies)) ||
      !(significance= SDDS_Malloc(sizeof(*significance)*maxFrequencies))) 
    SDDS_Bomb("memory allocation failure");
  if (pairs) {
    if (!(amplitude1 = SDDS_Malloc(sizeof(*amplitude1)*maxFrequencies)) ||
        !(phase1     = SDDS_Malloc(sizeof(*phase1    )*maxFrequencies)) ||
        !(significance1 = SDDS_Malloc(sizeof(*significance1)*maxFrequencies)))
      SDDS_Bomb("memory allocation failure");
  }
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (rows) {
      long primeRows;
      rowsToUse = rows;
      primeRows = greatestProductOfSmallPrimes(rows);
      if (rows!=primeRows) {
	if (truncate)
	  rowsToUse = greatestProductOfSmallPrimes(rows);
	else if (largest_prime_factor(rows)>100 && !noWarnings)
	  fputs("Warning: number of points has large prime factors.\nThis could take a very long time.\nConsider using the -truncate option.\n", stderr);
      }
      if (!SDDS_StartPage(&SDDSout, maxFrequencies) ||
	  !SDDS_CopyParameters(&SDDSout, &SDDSin))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!(tdata = SDDS_GetColumnInDoubles(&SDDSin, indepQuantity)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=1; i<rowsToUse; i++)
	if (tdata[i]<=tdata[i-1])
	  SDDS_Bomb("independent data is not monotonically increasing");
      dt = (tdata[rowsToUse-1]-tdata[0])/(rowsToUse-1.0),
      t0 = tdata[0];
      free(tdata);
      for (i=0; i<depenQuantities; i++) {
	if (!(data = SDDS_GetColumnInDoubles(&SDDSin, depenQuantity[i])))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (j=0; j<maxFrequencies; j++)
          frequency[j] = amplitude[j] = phase[j] = significance[j] = -1;
	frequenciesFound = PerformNAFF(frequency, amplitude, phase, significance,
                                       t0, dt,
				       data, rowsToUse, flags,
				       fracRMSChangeLimit, maxFrequencies,
				       freqCycleLimit, fracFreqAccuracyLimit, 0, 0);
#ifdef DEBUG
        fprintf(stderr, "Column %s: ", depenQuantity[i]);
        fprintf(stderr, "f=%10.3e  a=%10.3e  p=%10.3e  s=%10.3e\n",
                frequency[0], amplitude[0], phase[0], significance[0]);
#endif
	free(data);
	data = NULL;
        if (pairs) {
          if (!(data = SDDS_GetColumnInDoubles(&SDDSin, depenQuantityPair[i])))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          frequenciesFound = PerformNAFF(frequency, amplitude1, phase1, significance1,
                                         t0, dt,
                                         data, rowsToUse, pairFlags,
                                         fracRMSChangeLimit, maxFrequencies,
                                         freqCycleLimit, fracFreqAccuracyLimit, 0, 0);
          
          for (j=0; j<maxFrequencies; j++)
            if (frequency[j]!=-1)
              frequency[j] = adjustFrequencyHalfPlane(frequency[j], phase[j], phase1[j], dt); 
          free(data);
          data = NULL;
        } 
	if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
			    frequency, maxFrequencies, frequencyIndex[i]) ||
	    !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
			    amplitude, maxFrequencies, amplitudeIndex[i]) ||
	    !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
			    phase, maxFrequencies, phaseIndex[i]) ||
	    !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
			    significance, maxFrequencies, significanceIndex[i]))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (pairs) {
          if (!SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
                              amplitude1, maxFrequencies, amplitudeIndex1[i]) ||
              !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
                              phase1, maxFrequencies, phaseIndex1[i]) ||
              !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_INDEX,
                              significance1, maxFrequencies, significanceIndex1[i]))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
    }
    else  {
      if (!SDDS_StartPage(&SDDSout, 0) || !SDDS_CopyParameters(&SDDSout, &SDDSin))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (!SDDS_Terminate(&SDDSin)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  free(frequency);
  free(amplitude);
  free(phase);
  free(significance);
  if (pairs) {
    free(amplitude1);
    free(phase1);
    free(significance1);
    free(amplitudeIndex1);
    free(phaseIndex1);
    free(significanceIndex1);
    free(depenQuantityPair);
  }
  free(depenQuantity);
  free(frequencyIndex);
  free(amplitudeIndex);
  free(phaseIndex);
  free(significanceIndex);
  return 0;
}

long SetupNAFFOutput(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin,
		     char *indepQuantity, 
		     long depenQuantities, char **depenQuantity,
		     long **frequencyIndex, long **amplitudeIndex, long **phaseIndex,
                     long **significanceIndex, char **depenQuantityPair,
                     long **amplitudeIndex1, long **phaseIndex1,
                     long **significanceIndex1)
{
  char *freqUnits, *buffer, *ampUnits;
  long i, maxBuffer, bufferNeeded;

  if (!(*frequencyIndex = SDDS_Malloc(sizeof(**frequencyIndex)*depenQuantities)) ||
      !(*amplitudeIndex = SDDS_Malloc(sizeof(**amplitudeIndex)*depenQuantities)) ||
      !(*phaseIndex = SDDS_Malloc(sizeof(**phaseIndex)*depenQuantities)) ||
      !(*significanceIndex = SDDS_Malloc(sizeof(**significanceIndex)*depenQuantities)) ||
      !(buffer = SDDS_Malloc(sizeof(*buffer)*(maxBuffer=1024)))) {
    SDDS_SetError("memory allocation failure");
    return 0;
  }
  if (!(freqUnits = makeFrequencyUnits(SDDSin, indepQuantity)) ||
      !SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddsnaff output", output))
    return 0;
  
  for (i=0; i<depenQuantities; i++) {
    if ((bufferNeeded=strlen(depenQuantity[i])+12)>maxBuffer &&
	!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*bufferNeeded)))
      SDDS_Bomb("memory allocation failure");
    sprintf(buffer, "%sFrequency", depenQuantity[i]);
    if (((*frequencyIndex)[i]=
	 SDDS_DefineColumn(SDDSout, buffer, NULL, freqUnits, NULL, NULL, SDDS_DOUBLE, 0))<0 ||
	SDDS_GetColumnInformation(SDDSin, "units", &ampUnits, SDDS_GET_BY_NAME,
				  depenQuantity[i])!=SDDS_STRING)
      return 0;
    sprintf(buffer, "%sAmplitude", depenQuantity[i]);
    if (((*amplitudeIndex)[i]=
	 SDDS_DefineColumn(SDDSout, buffer, NULL, ampUnits, NULL, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    sprintf(buffer, "%sPhase", depenQuantity[i]);
    if (((*phaseIndex)[i] = 
         SDDS_DefineColumn(SDDSout, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    sprintf(buffer, "%sSignificance", depenQuantity[i]);
    if (((*significanceIndex)[i] = 
         SDDS_DefineColumn(SDDSout, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
  }
  if (depenQuantityPair) {
    if (!(*amplitudeIndex1 = SDDS_Malloc(sizeof(**amplitudeIndex1)*depenQuantities)) ||
        !(*phaseIndex1 = SDDS_Malloc(sizeof(**phaseIndex1)*depenQuantities)) ||
        !(*significanceIndex1 = SDDS_Malloc(sizeof(**significanceIndex1)*depenQuantities))) {
      SDDS_SetError("memory allocation failure");
      return 0;
    }
    for (i=0; i<depenQuantities; i++) {
      if ((bufferNeeded=strlen(depenQuantityPair[i])+12)>maxBuffer &&
          !(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*bufferNeeded)))
        SDDS_Bomb("memory allocation failure");
      sprintf(buffer, "%sAmplitude", depenQuantityPair[i]);
      if (((*amplitudeIndex1)[i]=
           SDDS_DefineColumn(SDDSout, buffer, NULL, ampUnits, NULL, NULL, SDDS_DOUBLE, 0))<0)
        return 0;
      sprintf(buffer, "%sPhase", depenQuantityPair[i]);
      if (((*phaseIndex1)[i] = 
           SDDS_DefineColumn(SDDSout, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
        return 0;
      sprintf(buffer, "%sSignificance", depenQuantityPair[i]);
      if (((*significanceIndex1)[i] = 
           SDDS_DefineColumn(SDDSout, buffer, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0))<0)
        return 0;
    }
  }
  free(ampUnits);
  free(freqUnits);

  if (!SDDS_TransferAllParameterDefinitions(SDDSout, SDDSin, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(SDDSout))
    return 0;
  free(buffer);
  return 1;
}

