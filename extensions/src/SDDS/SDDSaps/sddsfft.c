/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsfft
 * purpose: SDDS-format fft program
 *
 * Michael Borland, 1994
 * based on mpl-format fft program
 $Log: sddsfft.c,v $
 Revision 1.36  2010/06/11 17:10:06  borland
 Added more window types.

 Revision 1.35  2008/09/15 14:42:00  shang
 updated usage message, and added warning messages for inverse option

 Revision 1.34  2008/09/10 19:07:45  shang
 added inverse option, and added folded and unfolded suboptions to complexInput and fullOutput options.

 Revision 1.33  2006/12/14 22:21:58  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.32  2005/10/10 19:13:15  borland
 Added optional correction factors for windowed data.

 Revision 1.31  2003/10/20 16:17:25  borland
 Fixed error in phase computation.  Was using atan2(x, y) when I should have
 used atan2(y, x).

 Revision 1.30  2003/09/02 19:16:04  soliday
 Cleaned up code for Linux.

 Revision 1.29  2003/03/27 22:56:13  soliday
 Fixed output message that should not be set to stderr

 Revision 1.28  2002/08/14 17:12:46  soliday
 Added Open License

 Revision 1.27  2002/01/22 15:40:21  borland
 Previous changes resulted in changed behavior for -padwithzeroes option.
 It now behaves as before it the exponent value is not given.  Updated the
 usage message.

 Revision 1.26  2002/01/20 17:49:45  borland
 The pad option now accepts an integer specifying extra padding beyond
 the minimum necessary (as a power of 2).

 Revision 1.25  2001/10/25 21:59:09  borland
 makeFrequencyUnits() and greatestProductOfSmallPrimes() are not in
 SDDSutil.c

 Revision 1.24  2001/05/08 19:33:01  emery
 variable length was not defined for padding.

 Revision 1.23  2001/05/07 16:14:45  emery
 Fixed sign error for time shifting term.

 Revision 1.22  2001/05/03 21:29:19  soliday
 Split usage message because of limits on WIN32.

 Revision 1.21  2001/04/06 20:06:36  borland
 Added ability to perform FFTs of complex input.

 Revision 1.20  2001/04/04 18:16:46  borland
 Fixed problem with truncation.  Was still truncating to power of two in
 data processing but writing out truncation for power of primes.

 Revision 1.19  2001/01/10 19:35:38  soliday
 Standardized usage message.

 Revision 1.18  2000/11/27 14:58:33  borland
 Fixed memory leak.

 Revision 1.17  2000/11/21 17:17:36  soliday
 Added missing SDDS_Terminate procedures.

 Revision 1.16  2000/04/13 18:03:11  soliday
 Removed invalid prototypes.

 Revision 1.15  2000/04/13 17:10:34  soliday
 Added missing include statment.

 Revision 1.14  1999/11/23 16:46:11  borland
 Updated the usage message.

 Revision 1.13  1999/10/29 20:05:52  borland
 The -truncate option now truncates to a product of small primes between
 2 and 19.

 Revision 1.12  1999/09/14 18:31:27  soliday
 dp_pad_with_zeroes and realFFT2 are no longer defined locally

 Revision 1.11  1999/07/14 14:15:43  borland
 Changed the names for the integrated PSD output.  Was IntegSqrtPSD...,
 but should have been SqrtIntegPSD...

 Revision 1.10  1999/05/25 19:10:20  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/07 17:00:47  borland
 Added option for reverse integral to psdOutput.

 Revision 1.8  1999/01/06 19:54:44  borland
 Fixed the version number in the usage message.

 Revision 1.7  1998/12/16 21:26:02  borland
 Brought up to date with new version of SDDS_TransferAllParameters.
 Now correctly transfers through parameters, but overwrites them if it
 needs to do so.

 Revision 1.6  1998/11/13 22:39:51  borland
 Now copies through parameters from the input to the output.

 Revision 1.5  1998/10/08 18:21:03  borland
 Added qualifiers to -psdOutput option: plain, integrated.  plain is just
 the PSD as before.  integrated gives Integ{Sqrt{PSD}}.

 Revision 1.4  1998/06/03 01:35:32  borland
 Added -nowarning option.

 * Revision 1.3  1995/09/06  14:56:28  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "fftpackC.h"
#include "SDDSutils.h"
#include <ctype.h>

#define SET_WINDOW 0
#define SET_NORMALIZE 1
#define SET_PADWITHZEROES 2
#define SET_TRUNCATE 3
#define SET_SUPPRESSAVERAGE 4
#define SET_SAMPLEINTERVAL 5
#define SET_COLUMNS 6
#define SET_FULLOUTPUT 7
#define SET_PIPE 8
#define SET_PSDOUTPUT 9
#define SET_EXCLUDE 10
#define SET_NOWARNINGS 11
#define SET_COMPLEXINPUT 12
#define SET_INVERSE 13
#define N_OPTIONS 14

char *option[N_OPTIONS] = {
    "window", "normalize", "padwithzeroes",
    "truncate", "suppressaverage", "sampleinterval",
    "columns", "fulloutput", "pipe", "psdoutput", "exclude",
    "nowarnings", "complexinput", "inverse",
    };

#define FL_TRUNCATE        0x0008
#define FL_PADWITHZEROES   0x0010
#define FL_NORMALIZE       0x0020
#define FL_SUPPRESSAVERAGE 0x0040
#define FL_FULLOUTPUT      0x0080
#define FL_MAKEFREQDATA    0x0100
#define FL_PSDOUTPUT       0x0200
#define FL_PSDINTEGOUTPUT  0x0400
#define FL_PSDRINTEGOUTPUT 0x0800
#define FL_FULLOUTPUT_FOLDED 0x1000
#define FL_FULLOUTPUT_UNFOLDED 0x2000
#define FL_COMPLEXINPUT_FOLDED 0x4000
#define FL_COMPLEXINPUT_UNFOLDED 0x8000

#define WINDOW_HANNING 0
#define WINDOW_WELCH 1
#define WINDOW_PARZEN 2
#define WINDOW_HAMMING 3
#define WINDOW_FLATTOP 4
#define WINDOW_GAUSSIAN 5
#define N_WINDOW_TYPES 6
char *window_type[N_WINDOW_TYPES] = {
    "hanning", "welch", "parzen", "hamming", "flattop", "gaussian",
    } ;

static char *USAGE1="sddsfft [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-columns=<indep-variable>[,<depen-quantity>[,...]]] \n\
[-complexInput[=unfolded|folded]] [-exclude=<depen-quantity>[,...]]\n\
[-window[={hanning|welch|parzen|hamming|flattop|gaussian}[,correct]]] \n\
[-sampleInterval=<number>] [-normalize] [-fullOutput[=unfolded|folded]]\n\
[-psdOutput[=plain][,{integrated|rintegrated}]] [-inverse]\n\
[-padwithzeroes[=exponent] | -truncate] [-suppressaverage] [-noWarnings]\n\n\
-pipe            The standard SDDS Toolkit pipe option.\n\
-columns         Provides the name of the independent variable \n\
                 and the data to Fourier analyze.  <depen-quantity>\n\
                 entries may contain wildcards.\n\
-complexInput    Specifies that the names of the input columns are\n\
                 of the form Real<rootname> and Imag<rootname>, giving\n\
                 the real and imaginary part of a function to be analyzed.\n\
                 In this case, the <depen-quantity> entries in the -columns\n\
                 option give the rootname, not the full quantity name.\n\
                 It has options folded and unfolded, unfolded means the input frequency \n\
                 space input is unfolded and it must have negative frequency.\n\
                 default is \"folded\", If no option is given, and if the input file\n\
                 has \"SpectrumFolded\" parameter, then it will be defined by this parameter.\n\
-inverse         produce inverse fourier transform. when it is given, the output is\n\
                 always unfolded spectrum. so -fullOutput=folded will be changed to -fullOutput=unfoled.\n\
-exclude         Specifies a list of wild-card patterns to use in excluding\n\
                 quantities from analysis.\n";
static char *USAGE2="-window          Requests windowing of data prior to analysis.  The default\n\
                 window is Hanning.  If the \"correct\" qualifier is given, the signal\n\
                 is also multiplied by a correction factor to ensure that the integrated\n\
                 PSD is unchanged by the window.\n\
-sampleInterval  Requests sampling of the input data points with the given interval.\n\
-normalize       Specifies normalization of the output to a peak magnitude of 1.\n\
-fullOutput      Requests output of the real and imaginary parts of the FFT.\n\
                 it also has folded and unfolded options, while the unfolded option \n\
                 outputs the unfolded freqeuncy-space (full FFT), but the folded option \n\
                 outputs the folded frequency-space (half FFT). default is folded. \n\
-psdOutput       Requests output of the Power Spectral Density.  By default, the\n\
                 \"plain\" PSD output is included.  Giving the integrated or\n\
                 rintegrated qualifier substitutes the integrated or reverse-\n\
                 integrated PSD.  Giving the \"plain\" qualifier in addition\n\
                 gives both.\n\
-padWithZeroes   Requests padding of the data with zeroes if the number of data \n\
                 points is not a product of small prime numbers.  May substantially\n\
                 reducing run time.  If exponent is given, the padding takes place\n\
                 of the original number of points.  The number of data points used\n\
                 is the nearest power of 2 to the original number of points, times\n\
                 2^(exponent).\n\
-truncate        Requests truncation of the data if the number of data points is\n\
                 not a product of small prime numbers.  May substantially reduce run\n\
                 time.\n\
-suppressAverage Specifies that the average value of the data should be removed\n\
                 prior to taking the FFT.\n\
-noWarnings      Suppresses warning messages.\n\n\
Program by Michael Borland.  (This is version 10, January 2002.)\n";


long greatestProductOfSmallPrimes(long rows);
long process_data(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, double *tdata, long rows, long rowsToUse,
                  char *depenQuantity, char *depenQuantity2, 
                  unsigned long flags, long windowType, long sampleInterval,
                  long correctWindowEffects, long inverse);
long create_fft_frequency_column(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *timeName,
				 char *freqUnits, long inverse);

long create_fft_columns(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *origName,
			char *indepName, char *freqUnits, long full_output, unsigned long psd_output,
                        long complexInput, long inverse);
long create_fft_parameters(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *indepName,
			   char *freqUnits);
char *makeFrequencyUnits(SDDS_DATASET *SDDSin, char *indepName);
long expandComplexColumnPairNames(SDDS_DATASET *SDDSin, 
                                  char **name, char ***realName, char ***imagName, 
                                  long names, char **excludeName,
                                  long excludeNames, long typeMode, long typeValue);

int main(int argc, char **argv)
{
  int iArg;
  char *freqUnits;
  char *indepQuantity, **depenQuantity, **exclude, **realQuan, **imagQuan;
  long depenQuantities, excludes;
  char *input, *output;
  long sampleInterval, i, rows, readCode, rowsToUse, noWarnings, complexInput, inverse, spectrumFoldParExist=0, spectrumFolded=0, page=0;
  unsigned long flags, pipeFlags, complexInputFlags=0, fullOutputFlags=0;
  long windowType = -1;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double *tdata;
  long padFactor, correctWindowEffects=0;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3 || argc>(3+N_OPTIONS)) {
    fprintf(stderr, "%s%s", USAGE1, USAGE2);
    exit(1);
    /* bomb(NULL, USAGE);*/
  }
  output = input = NULL;
  flags = pipeFlags = excludes = complexInput = inverse = 0;
  sampleInterval = 1;
  indepQuantity = NULL;
  depenQuantity = exclude = NULL;
  depenQuantities = 0;
  noWarnings = 0;
  padFactor = 0;
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_NORMALIZE:
	flags |= FL_NORMALIZE;
	break;
      case SET_WINDOW:
	if (scanned[iArg].n_items!=1) {
	  if ((i=match_string(scanned[iArg].list[1], window_type, N_WINDOW_TYPES, 0))<0)
	    SDDS_Bomb("unknown window type");
	  windowType = i;
          if (scanned[iArg].n_items>2) {
            if (strncmp(scanned[iArg].list[2], "correct", strlen(scanned[iArg].list[2]))==0)
              correctWindowEffects = 1;
            else 
              SDDS_Bomb("invalid -window syntax");
          }
	}
	else
	  windowType = 0;
	break;
      case SET_PADWITHZEROES:
	flags |= FL_PADWITHZEROES; 
	if (scanned[iArg].n_items!=1) {
	  if (scanned[iArg].n_items!=2 ||
	      sscanf(scanned[iArg].list[1], "%ld", &padFactor)!=1 ||
	      padFactor<1)
	    SDDS_Bomb("invalid -padwithzeroes syntax");
	}
	break;
      case SET_TRUNCATE:
	flags |= FL_TRUNCATE;
	break;
      case SET_SUPPRESSAVERAGE:
	flags |= FL_SUPPRESSAVERAGE;
	break;
      case SET_SAMPLEINTERVAL:
	if (scanned[iArg].n_items!=2 || sscanf(scanned[iArg].list[1], "%ld", &sampleInterval)!=1 ||
	    sampleInterval<=0)
	  SDDS_Bomb("invalid -sampleinterval syntax");
	break;
      case SET_COLUMNS:
	if (indepQuantity)
	  SDDS_Bomb("only one -columns option may be given");
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -columns syntax");
	indepQuantity = scanned[iArg].list[1];
	if (scanned[iArg].n_items>=2) {
	  depenQuantity = tmalloc(sizeof(*depenQuantity)*(depenQuantities=scanned[iArg].n_items-2));
	  for (i=0; i<depenQuantities; i++)
	    depenQuantity[i] = scanned[iArg].list[i+2];
	}
	break;
      case SET_FULLOUTPUT:
        flags |= FL_FULLOUTPUT;
        if (scanned[iArg].n_items==2) {
          scanned[iArg].n_items--;
          if (!scanItemList(&fullOutputFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                            "folded", -1, NULL, 0, FL_FULLOUTPUT_FOLDED,
                            "unfolded", -1, NULL, 0, FL_FULLOUTPUT_UNFOLDED,
                            NULL))
            SDDS_Bomb("Invalid -fullOutput syntax");
          scanned[iArg].n_items++;
          if (fullOutputFlags&FL_FULLOUTPUT_UNFOLDED) 
            flags |= FL_FULLOUTPUT_UNFOLDED;
          else
            flags |= FL_FULLOUTPUT_FOLDED;
        } else
          flags != FL_FULLOUTPUT_FOLDED;
        
	break;
      case SET_PIPE:
	if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_PSDOUTPUT:
	if (scanned[iArg].n_items -= 1) {
	  unsigned long tmpFlags;
	  if (!scanItemList(&tmpFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
			    "integrated", -1, NULL, 0, FL_PSDINTEGOUTPUT,
			    "rintegrated", -1, NULL, 0, FL_PSDRINTEGOUTPUT,
			    "plain", -1, NULL, 0, FL_PSDOUTPUT,
			    NULL)) {
	    SDDS_Bomb("invalid -psdOutput syntax");
	  }
	  flags |= tmpFlags;
	} else 
	  flags |= FL_PSDOUTPUT;
	if (flags&FL_PSDINTEGOUTPUT && flags&FL_PSDRINTEGOUTPUT) 
	  SDDS_Bomb("invalid -psdOutput syntax: give only one of integrated or rintegrated");
	break;
      case SET_EXCLUDE:
	if (scanned[iArg].n_items<2)
	  SDDS_Bomb("invalid -exclude syntax");
	moveToStringArray(&exclude, &excludes, scanned[iArg].list+1, scanned[iArg].n_items-1);
	break;
      case SET_NOWARNINGS:
	noWarnings = 1;
	break;
      case SET_COMPLEXINPUT:
        complexInput = 1;
        if (scanned[iArg].n_items==2) {
          scanned[iArg].n_items--;
          if (!scanItemList(&complexInputFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                            "folded", -1, NULL, 0, FL_COMPLEXINPUT_FOLDED,
                            "unfolded", -1, NULL, 0, FL_COMPLEXINPUT_UNFOLDED,
                            NULL))
            SDDS_Bomb("Invalid -complexInput syntax");
          scanned[iArg].n_items++;
        } else
          complexInputFlags != FL_COMPLEXINPUT_FOLDED;
        break;
      case SET_INVERSE:
        inverse = 1;
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
  if (!complexInput) {
    if (!noWarnings && inverse)
      fprintf(stderr, "Warning, the inverse option is ignored since it only works with -complexInput.\n");
    inverse = 0;
  }
  if (!noWarnings && inverse && flags&FL_FULLOUTPUT_FOLDED) 
    fprintf(stderr, "Warning, the -inverse -fullOutput=folded will be changed to -inverse -fullOutput=unfoled.\n");
  
  processFilenames("sddsfft", &input, &output, pipeFlags, 0, NULL);
  
  if (!indepQuantity)
    SDDS_Bomb("supply the independent quantity name with the -columns option");
  
  if (flags&FL_TRUNCATE && flags&FL_PADWITHZEROES)
    SDDS_Bomb("specify only one of -padwithzeroes and -truncate");
  
  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (SDDS_CheckColumn(&SDDSin, indepQuantity, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OKAY) 
    exit(1);
  
  excludes = appendToStringArray(&exclude, excludes, indepQuantity);
  if (!depenQuantities)
    depenQuantities = appendToStringArray(&depenQuantity, depenQuantities, "*"); 

  if (!complexInput) {
    if ((depenQuantities=expandColumnPairNames(&SDDSin, &depenQuantity, NULL, depenQuantities, 
                                               exclude, excludes, FIND_NUMERIC_TYPE, 0))<=0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      SDDS_Bomb("No quantities selected to fft");
    }
  } else {
    if ((depenQuantities=expandComplexColumnPairNames(&SDDSin, 
                                                      depenQuantity, &realQuan, &imagQuan,
                                                      depenQuantities, exclude, excludes,
                                                      FIND_NUMERIC_TYPE, 0))<=0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      SDDS_Bomb("No quantities selected to fft");
    }
  }
  
#if 0
  fprintf(stderr, "%ld dependent quantities:\n", depenQuantities);
  for (i=0; i<depenQuantities; i++)
    fprintf(stderr, "  %s\n", depenQuantity[i]);
#endif
  
  if (!(freqUnits = makeFrequencyUnits(&SDDSin, indepQuantity)) ||
      !SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, "sddsfft output", output) ||
      !create_fft_frequency_column(&SDDSout, &SDDSin, indepQuantity, freqUnits, inverse) ||
      SDDS_DefineParameter(&SDDSout, "fftFrequencies", NULL, NULL, NULL, NULL, SDDS_LONG, NULL)<0 ||
      SDDS_DefineParameter(&SDDSout, "fftFrequencySpacing", "$gD$rf", freqUnits,
			   NULL, NULL, SDDS_DOUBLE, NULL)<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (flags&FL_FULLOUTPUT &&  SDDS_DefineParameter(&SDDSout, "SpectrumFolded", NULL, NULL,
                                                   NULL, NULL, SDDS_LONG, NULL)<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (complexInput) {
    if (!complexInputFlags) {
      if (SDDS_CheckParameter(&SDDSin, "SpectrumFolded", NULL, SDDS_LONG, NULL)==SDDS_CHECK_OK)
        spectrumFoldParExist = 1;
    } else if (complexInputFlags&FL_COMPLEXINPUT_UNFOLDED)
      flags |= FL_COMPLEXINPUT_UNFOLDED;
    else
      flags |= FL_COMPLEXINPUT_FOLDED; 
  }
  for (i=0; i<depenQuantities; i++) {
    if (!complexInput)
      create_fft_columns(&SDDSout, &SDDSin, depenQuantity[i], indepQuantity, freqUnits,
                         flags&FL_FULLOUTPUT, 
                         flags&(FL_PSDOUTPUT+FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT), 0, inverse);
    else
      create_fft_columns(&SDDSout, &SDDSin, realQuan[i], indepQuantity, freqUnits,
                         flags&FL_FULLOUTPUT, 
                         flags&(FL_PSDOUTPUT+FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT), 1, inverse);
  }
  
  if (!SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, SDDS_TRANSFER_KEEPOLD) ||
      !SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    page ++;
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (page==1 && spectrumFoldParExist) {
      if (!SDDS_GetParameterAsLong(&SDDSin, "SpectrumFolded", &spectrumFolded))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (spectrumFolded)
        flags |= FL_COMPLEXINPUT_FOLDED;
      else
        flags |= FL_COMPLEXINPUT_UNFOLDED;
    }
    if (rows) {
      long primeRows, pow2Rows;
      rowsToUse = rows;
      primeRows = greatestProductOfSmallPrimes(rows);
      if (rows!=primeRows || padFactor) {
	if (flags&FL_PADWITHZEROES) {
	  pow2Rows = ipow(2., ((long)(log((double)rows)/log(2.0F))) +
                          (padFactor?padFactor:1.0) );
          if ((primeRows = greatestProductOfSmallPrimes(pow2Rows))>rows)
            rowsToUse = primeRows;
          else
            rowsToUse = pow2Rows;
          fprintf(stdout, "Using %ld rows\n", rowsToUse);
        }
	else if (flags&FL_TRUNCATE)
	  rowsToUse = greatestProductOfSmallPrimes(rows);
	else if (largest_prime_factor(rows)>100 && !noWarnings)
	  fputs("Warning: number of points has large prime factors.\nThis could take a very long time.\nConsider using the -truncate option.\n", stderr);
      }
      if (!SDDS_StartPage(&SDDSout, 2*rowsToUse+2) ||
	  !SDDS_CopyParameters(&SDDSout, &SDDSin))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (!(tdata = SDDS_GetColumnInDoubles(&SDDSin, indepQuantity)))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<depenQuantities; i++) 
	if (!process_data(&SDDSout, &SDDSin, tdata, rows, rowsToUse, 
                          complexInput?realQuan[i]:depenQuantity[i], 
                          complexInput?imagQuan[i]:NULL,
			  flags|(i==0?FL_MAKEFREQDATA:0), windowType, sampleInterval,
                          correctWindowEffects, inverse)) {
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	  exit(1);
	}
      free(tdata);
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
  
  return 0;
}

static long psdOffset, argOffset, realOffset, imagOffset, fftOffset = -1, psdIntOffset;

long process_data(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, double *tdata, long rows,
                  long rowsToUse,  char *depenQuantity, 
                  char *imagQuantity, 
                  unsigned long flags, long windowType, long sampleInterval,
                  long correctWindowEffects, long inverse)
{
  long n_freq, i, offset, index, fftrows=0, unfold=0;
  double r, r1, r2, length, factor, df;
  double *real, *imag, *magData, *arg=NULL, *real_imag, *data, *psd=NULL, *psdInteg=NULL;
  double dtf_real, dtf_imag, t0;
  char s[256];
  double *fdata, *imagData = NULL;
  double *tDataStore = NULL;
  double windowCorrectionFactor = 0;
  
  if (!(data = SDDS_GetColumnInDoubles(SDDSin, depenQuantity)))
    return 0;
  if (imagQuantity && 
      !(imagData = SDDS_GetColumnInDoubles(SDDSin, imagQuantity)))
    return 0;
  
  if (rows<rowsToUse) {
    /* pad with zeroes */
    tDataStore = tmalloc(sizeof(*tDataStore)*rowsToUse);
    memcpy((char*)tDataStore, (char*)tdata, rows*sizeof(*tdata));
    if (!(data = SDDS_Realloc(data, sizeof(*data)*rowsToUse)))
      SDDS_Bomb("memory allocation failure");
    if (imagData &&
        !(imagData = SDDS_Realloc(imagData, sizeof(*imagData)*rowsToUse)))
      length = ((double)rows)*(tdata[rows-1]-tdata[0])/((double)rows-1.0);
    else
      length = tdata[rows-1]-tdata[0];
    for (i=rows; i<rowsToUse; i++) {
      tDataStore[i] = tDataStore[i-1] + length/((double)rows - 1);
      data[i] = 0;
    }
    if (imagData)
      for (i=rows; i<rowsToUse; i++)
        imagData[i] = 0;
    tdata = tDataStore;
  }
  
  rows = rowsToUse;  /* results in truncation if rows>rowsToUse */
  
  if (flags&FL_SUPPRESSAVERAGE) {
    compute_average(&r, data, rows);
    for (i=0; i<rows; i++)
      data[i] -= r;
    if (imagData) {
      compute_average(&r, imagData, rows);
      for (i=0; i<rows; i++)
        imagData[i] -= r;
    }
  }

  windowCorrectionFactor = 1;
  switch (windowType) {
  case WINDOW_HANNING:
    r = PIx2/(rows-1);
    for (i=0; i<rows; i++) {
      factor = (1 - cos(i*r))/2;
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= (1 - cos(i*r))/2;
    break;
  case WINDOW_HAMMING:
    r = PIx2/(rows-1);
    for (i=0; i<rows; i++) {
      factor = 0.54 - 0.46*cos(i*r);
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= (1 - cos(i*r))/2;
    break;
  case WINDOW_WELCH:
    r1 = (rows-1)/2.0;
    r2 = sqr((rows+1)/2.0);
    for (i=0; i<rows; i++) {
      factor = 1 - sqr(i-r1)/r2;
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= 1 - sqr(i-r1)/r2;
    break;
  case WINDOW_PARZEN:
    r = (rows-1)/2.0;
    for (i=0; i<rows; i++) {
      factor = 1-FABS((i-r)/r);
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= 1-FABS((i-r)/r);
    break;
  case WINDOW_FLATTOP:
    for (i=0; i<rows; i++) {
      r = i*PIx2/(rows-1);
      factor = 1 - 1.93*cos(r) + 1.29*cos(2*r) - 0.388*cos(3*r) + 0.032*cos(4*r);
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= 1-FABS((i-r)/r);
    break;
  case WINDOW_GAUSSIAN:
    for (i=0; i<rows; i++) {
      r = sqr((i-(rows-1)/2.)/(0.4*(rows-1)/2.))/2;
      factor = exp(-r);
      data[i] *= factor;
      windowCorrectionFactor += sqr(factor);
    }
    if (imagData)
      for (i=0; i<rows; i++)
        imagData[i] *= 1-FABS((i-r)/r);
    break;
  default:
    break;
  }
  


  if (correctWindowEffects) {
    /* Add correction factor to make the integrated PSD come out right. */
    windowCorrectionFactor = 1/sqrt(windowCorrectionFactor/rows);
    for (i=0; i<rows; i++)
      data[i] *= windowCorrectionFactor;
    if (imagData)
      imagData[i] *= windowCorrectionFactor;
  }
  if (imagData && flags&FL_COMPLEXINPUT_FOLDED) {
    double min, max, max1;
    data = SDDS_Realloc(data, sizeof(*data)*rows*2);
    imagData = SDDS_Realloc(imagData, sizeof(*data)*rows*2);
    find_min_max(&min, &max, data, rows);
    if (fabs(min)>fabs(max))
      max1 = fabs(min);
    else
      max1 = fabs(max);
    find_min_max(&min, &max, imagData, rows);
    if (fabs(min)>max1)
      max1 = fabs(min);
    if (fabs(max)>max1)
      max1 = fabs(max);
    if (fabs(imagData[rows-1])/max1<1.0e-15) {
      fftrows = 2 * (rows-1);
      for (i=1; i<rows-1; i++) {
        data[i] = data[i]/2.0;
        imagData[i] = imagData[i]/2.0;
      }
      for (i=1; i<rows-1; i++) {
        data[rows-1+i] = data[rows-1-i];
        imagData[rows-1+i] = -imagData[rows-1-i];
      }
      length = (tdata[rows-1]-tdata[0])*2.0;
    } else {
      fftrows = 2 * (rows-1) +1;
      for (i=1; i<rows; i++) {
        data[i] = data[i]/2.0;
        imagData[i] = imagData[i]/2.0;
      }
      for (i=0; i<rows-1; i++) {
        data[rows+i] = data[rows-1-i];
        imagData[rows+i] = -imagData[rows-1-i];
      }
      length = ((double)fftrows)*(tdata[rows-1]-tdata[0])/((double)fftrows-1.0)*2;
    }
  } else {
    fftrows = rows;
    length = ((double)rows)*(tdata[rows-1]-tdata[0])/((double)rows-1.0);
  }
  /* compute FFT */
  
  real_imag = tmalloc(sizeof(double)*(2*fftrows+2));
  for (i=0; i<fftrows; i++) {
    real_imag[2*i] = data[i];
    if (imagData)
      real_imag[2*i+1] = imagData[i];
    else
      real_imag[2*i+1] = 0;
  }
  if (!inverse) {
    complexFFT(real_imag, fftrows, 0);
    if (flags&FL_FULLOUTPUT_UNFOLDED) {
      n_freq = fftrows;
      unfold = 1;
    }
    else if (flags&FL_FULLOUTPUT_FOLDED)
      n_freq = fftrows/2 + 1;
    else if (!imagData)
      n_freq = fftrows/2 + 1;
    else
      n_freq = fftrows + 1;
  }
  else {
    complexFFT(real_imag, fftrows, INVERSE_FFT);
    n_freq = fftrows;
  }

   /* calculate factor for converting k to f or omega*/
  /* length is assumed the length of period, not the total length of the data file */
 
  /* length = ((double)rows)*(tdata[rows-1]-tdata[0])/((double)rows-1.0); */
  t0 = tdata[0];
  df = factor = 1.0/length;

  /* convert into amplitudes and frequencies, adding phase factor for t[0]!=0 */
  real = tmalloc(sizeof(double)*n_freq);
  imag = tmalloc(sizeof(double)*n_freq);
  fdata = tmalloc(sizeof(double)*n_freq);
  magData = tmalloc(sizeof(double)*n_freq);
  if (flags&FL_PSDOUTPUT || flags&(FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT)) {
    psd = tmalloc(sizeof(*psd)*n_freq);
    if (flags&(FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT))
      psdInteg = tmalloc(sizeof(*psdInteg)*n_freq);
  }

  for (i=0; i<n_freq; i++) {
    fdata[i] = i*df;
    dtf_real = cos(-2*PI*fdata[i]*t0);
    dtf_imag = sin(-2*PI*fdata[i]*t0);
    if (psd) 
      psd[i] = (sqr(real_imag[2*i])+sqr(real_imag[2*i+1]))/df;
    if (!imagData && i!=0 && !(i==(n_freq-1) && rows%2==0)) {
      /* This is not the DC or Nyquist term, so
         multiply by 2 to account for amplitude in
         negative frequencies 
         */
      if (!unfold) {
        real_imag[2*i  ] *= 2;
        real_imag[2*i+1] *= 2;
      }
      if (psd)
        psd[i] *= 2;  /* 2 really is what I want--not 4 */
    }
    real[i] = real_imag[2*i  ]*dtf_real - real_imag[2*i+1]*dtf_imag;
    imag[i] = real_imag[2*i+1]*dtf_real + real_imag[2*i  ]*dtf_imag;
    magData[i] = sqrt(sqr(real[i])+sqr(imag[i]));
  }
  if (psdInteg) {
    if (flags&FL_PSDINTEGOUTPUT) {
      psdInteg[0] = 0;
      for (i=1; i<n_freq; i++) 
        psdInteg[i] = psdInteg[i-1] + (psd[i-1]+psd[i])*df/2.;
      for (i=0; i<n_freq; i++)
        psdInteg[i] = sqrt(psdInteg[i]);
    }
    else {
      psdInteg[n_freq-1] = 0;
      for (i=n_freq-2; i>=0; i--) 
        psdInteg[i] = psdInteg[i+1] + (psd[i+1]+psd[i])*df/2.;
      for (i=0; i<n_freq; i++)
        psdInteg[i] = sqrt(psdInteg[i]);
    }
  }
  
  if (flags&FL_FULLOUTPUT) {
    arg = tmalloc(sizeof(*arg)*n_freq);
    for (i=0; i<n_freq; i++) {
      if (real[i] || imag[i]) 
        arg[i] = 180.0/PI*atan2(imag[i], real[i]);
      else
        arg[i] = 0;
    }
  }
  
  
  if (flags&FL_NORMALIZE) {
    factor = -DBL_MAX;
    for (i=0; i<n_freq; i++) 
      if (magData[i]>factor) 
        factor = magData[i];
    if (factor!= -DBL_MAX)
      for (i=0; i<n_freq; i++) {
        real[i]    /= factor;
        imag[i]    /= factor;
        magData[i] /= factor;
      }
  }
  if (!inverse)
    sprintf(s, "FFT%s", depenQuantity+(imagData?4:0));
  else {
    if (strncmp(depenQuantity, "FFT", 3)==0)
      sprintf(s, "%s", depenQuantity+3);
    else if (strncmp(depenQuantity, "RealFFT", 7)==0)
      sprintf(s, "%s", depenQuantity+7);
    else
      sprintf(s, "%s", depenQuantity);
  }
 
  if ((index=SDDS_GetColumnIndex(SDDSout, s))<0)
    return 0;
  
  if (flags&FL_SUPPRESSAVERAGE) {
    n_freq -= 1;
    offset = 1;
  }
  else
    offset = 0;
  
  if ((flags&FL_MAKEFREQDATA && 
       !SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, fdata+offset, n_freq, 0)) ||
      !SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, magData+offset, n_freq, index+fftOffset) ||
      (flags&FL_FULLOUTPUT && 
       (!SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, real+offset, n_freq, index+realOffset) ||
	!SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, imag+offset, n_freq, index+imagOffset) ||
	!SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, arg+offset, n_freq, index+argOffset))) ||
      (flags&FL_PSDOUTPUT &&
       !SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, psd+offset, n_freq, index+psdOffset)) ||
      (flags&(FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT) &&
       !SDDS_SetColumn(SDDSout, SDDS_SET_BY_INDEX, psdInteg+offset, n_freq, index+psdIntOffset)))
    return 0;
  
  if (!SDDS_SetParameters(SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			  "fftFrequencies", n_freq, "fftFrequencySpacing", df, NULL))
    return 0;
  if (flags&FL_FULLOUTPUT && !SDDS_SetParameters(SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                                                 "SpectrumFolded", flags&FL_FULLOUTPUT_UNFOLDED?0:1, NULL))
    return 0;
  free(data);
  free(magData);
  if (imagData)
    free(imagData);
  if (tDataStore)
    free(tDataStore);
  if (arg)
    free(arg);
  free(real_imag);
  free(real);
  free(imag);
  free(fdata);
  if (psd)
    free(psd);
  if (psdInteg)
    free(psdInteg);
  return 1;
}

long create_fft_frequency_column(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *timeName, char *freqUnits, long inverse)
{
  char s[SDDS_MAXLINE];
  char *timeSymbol;
  char *description;
  
  if (SDDS_GetColumnInformation(SDDSin, "symbol", &timeSymbol, SDDS_GET_BY_NAME, timeName)!=SDDS_STRING)
    return 0;
  if (!timeSymbol || SDDS_StringIsBlank(timeSymbol))
    SDDS_CopyString(&timeSymbol, timeName);
  
  sprintf(s, "Frequency for %s", timeSymbol);
  SDDS_CopyString(&description, s);
  if (!inverse) {
    if (SDDS_DefineColumn(SDDSout, "f", NULL, freqUnits, description, NULL, SDDS_DOUBLE, 0)<0) {
      free(timeSymbol);
      free(description);
      return 0;
    }
  } else {
    sprintf(s, "inverse for %s", timeSymbol);
    SDDS_CopyString(&description, s);
    if (SDDS_DefineColumn(SDDSout, "t", NULL, freqUnits, description, NULL, SDDS_DOUBLE, 0)<0) {
      free(timeSymbol);
      free(description);
      return 0;
    }
  }
  free(timeSymbol);
  free(description);
  return 1;
}

long create_fft_columns(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, char *origName,
                        char *indepName, char *freqUnits, long full_output, unsigned long psd_output,
                        long complexInput, long inverse)
{
  char s[SDDS_MAXLINE];
  char *origUnits, *origSymbol;
  char *description, *name, *symbol, *units;
  long index0, index1;
  long offset = 0;
  
  if (complexInput)
    offset = 4;
  if (SDDS_GetColumnInformation(SDDSin, "units", &origUnits, SDDS_GET_BY_NAME, origName)!=SDDS_STRING ||
      SDDS_GetColumnInformation(SDDSin, "symbol", &origSymbol, SDDS_GET_BY_NAME, origName)!=SDDS_STRING)
    return 0;
  if (!inverse) 
    sprintf(s, "FFT%s", origName+offset);
  else {
    if (strncmp(origName, "FFT", 3)==0)
      offset = 3;
    else if (strncmp(origName, "RealFFT", 7)==0)
      offset = 7;
    else 
      offset = 0;
    sprintf(s,"%s", origName+offset);
  }
  SDDS_CopyString(&name, s);
  if (!origSymbol)
    SDDS_CopyString(&origSymbol, origName+offset);
  sprintf(s, "FFT %s", origSymbol);
  SDDS_CopyString(&symbol, s);
  
  sprintf(s, "Amplitude of FFT of %s", origSymbol);
  SDDS_CopyString(&description, s);
  
  if (SDDS_NumberOfErrors() ||
      (index0=SDDS_DefineColumn(SDDSout, name, symbol, origUnits, description, NULL, SDDS_DOUBLE, 0))<0)
    return 0;
  free(name);
  free(symbol);
  free(description);
  
  if (fftOffset==-1)
    fftOffset = 0;
  
  if (psd_output&FL_PSDOUTPUT) {
    if (origUnits && !SDDS_StringIsBlank(origUnits)) {
      if (freqUnits && !SDDS_StringIsBlank(freqUnits))  {
	sprintf(s, "(%s)$a2$n/(%s)", origUnits, freqUnits);
      }
      else
	sprintf(s, "(%s)$a2$n", origUnits);
      SDDS_CopyString(&units, s);
    }
    else
      units = NULL;
    
    sprintf(s, "PSD%s", origName+offset);
    SDDS_CopyString(&name, s);
    
    if (!origSymbol)
      SDDS_CopyString(&origSymbol, origName+offset);
    sprintf(s, "PSD %s", origSymbol);
    SDDS_CopyString(&symbol, s);
    
    sprintf(s, "PSD of %s", origSymbol);
    SDDS_CopyString(&description, s);
    
    if (SDDS_NumberOfErrors() ||
	(index1=SDDS_DefineColumn(SDDSout, name, symbol, units, description, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    psdOffset = index1 - index0;
    free(name);
    if (units) free(units);
    free(symbol);
    free(description);
  }
  
  if (psd_output&(FL_PSDINTEGOUTPUT+FL_PSDRINTEGOUTPUT)) {
    if (origUnits && !SDDS_StringIsBlank(origUnits)) {
      SDDS_CopyString(&units, origUnits);
    }
    else
      units = NULL;
    
    sprintf(s, "SqrtIntegPSD%s", origName+offset);
    SDDS_CopyString(&name, s);
    
    if (!origSymbol)
      SDDS_CopyString(&origSymbol, origName+offset);
    sprintf(s, "Sqrt Integ PSD %s", origSymbol);
    SDDS_CopyString(&symbol, s);
    
    sprintf(s, "Sqrt Integ PSD of %s", origSymbol);
    SDDS_CopyString(&description, s);
    
    if (SDDS_NumberOfErrors() ||
	(index1=SDDS_DefineColumn(SDDSout, name, symbol, units, description, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    psdIntOffset = index1 - index0;
    free(name);
    if (units) free(units);
    free(symbol);
    free(description);
  }
  
  if (full_output) {
    sprintf(s, "RealFFT%s", origName+offset);
    SDDS_CopyString(&name, s);
    
    if (!origSymbol)
      SDDS_CopyString(&origSymbol, origName+offset);
    sprintf(s, "Re[FFT %s]", origSymbol);
    SDDS_CopyString(&symbol, s);
    
    sprintf(s, "Real part of FFT of %s", origSymbol);
    SDDS_CopyString(&description, s);
    
    if (SDDS_NumberOfErrors() ||
	(index1=SDDS_DefineColumn(SDDSout, name, symbol, origUnits, description, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    realOffset = index1 - index0;
    free(name);
    free(symbol);
    free(description);
    
    sprintf(s, "ImagFFT%s", origName+offset);
    SDDS_CopyString(&name, s);
    
    if (!origSymbol)
      SDDS_CopyString(&origSymbol, origName+offset);
    sprintf(s, "Im[FFT %s]", origSymbol);
    SDDS_CopyString(&symbol, s);
    
    sprintf(s, "Imaginary part of FFT of %s", origSymbol);
    SDDS_CopyString(&description, s);
    
    if (SDDS_NumberOfErrors() ||
	(index1=SDDS_DefineColumn(SDDSout, name, symbol, origUnits, description, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    imagOffset = index1 - index0;
    free(name);
    free(symbol);
    free(description);
    
    sprintf(s, "ArgFFT%s", origName+offset);
    SDDS_CopyString(&name, s);
    
    if (!origSymbol)
      SDDS_CopyString(&origSymbol, origName+offset);
    sprintf(s, "Arg[FFT %s]", origSymbol);
    SDDS_CopyString(&symbol, s);
    
    sprintf(s, "Phase of FFT of %s", origSymbol);
    SDDS_CopyString(&description, s);
    
    if (SDDS_NumberOfErrors() ||
	(index1=SDDS_DefineColumn(SDDSout, name, symbol, "degrees", description, NULL, SDDS_DOUBLE, 0))<0)
      return 0;
    argOffset = index1 - index0;
    free(name);
    free(symbol);
    free(description);
  }
  
  return 1;
}

void moveToStringArrayComplex(char ***targetReal, char ***targetImag, long *targets, 
                              char **sourceReal, char **sourceImag, long sources);

long expandComplexColumnPairNames(SDDS_DATASET *SDDSin, 
                                  char **name, char ***realName, char ***imagName, 
                                  long names, char **excludeName,
                                  long excludeNames, long typeMode, long typeValue)
{
  long i, j, k, realNames, imagNames, names2;
  char **realName1, **imagName1, **realName2, **imagName2;
  char *realPattern, *imagPattern=NULL;
  long longest;
  
  if (!names || !name)
    return 0;
  realName1 = imagName1 = realName2 = imagName2 = NULL;
  realNames = imagNames = names2 = 0;
  for (i=longest=0; i<names; i++) {
    if (strlen(name[i])>longest)
      longest = strlen(name[i]);
  }
  longest += 10;
  if (!(realPattern = SDDS_Malloc(sizeof(*realPattern)*longest)) ||
      !(imagPattern = SDDS_Malloc(sizeof(*imagPattern)*longest)))
    SDDS_Bomb("memory allocation failure");

  for (i=0; i<names; i++) {
    sprintf(realPattern, "Real%s", name[i]);
    sprintf(imagPattern, "Imag%s", name[i]);
    switch (typeMode) {
    case FIND_ANY_TYPE:
    case FIND_NUMERIC_TYPE:
    case FIND_INTEGER_TYPE:
    case FIND_FLOATING_TYPE:
      realNames = SDDS_MatchColumns(SDDSin, &realName1, 
                                     SDDS_MATCH_STRING, typeMode, 
                                     realPattern, SDDS_0_PREVIOUS|SDDS_OR);
      imagNames = SDDS_MatchColumns(SDDSin, &imagName1, 
                                     SDDS_MATCH_STRING, typeMode, 
                                     imagPattern, SDDS_0_PREVIOUS|SDDS_OR);
      break;
    case FIND_SPECIFIED_TYPE:
      if (!SDDS_VALID_TYPE(typeValue))
        SDDS_Bomb("invalid type value in expandColumnPairNames");
      realNames = SDDS_MatchColumns(SDDSin, &realName1, SDDS_MATCH_STRING, typeMode, typeValue, 
                                     realPattern, SDDS_0_PREVIOUS|SDDS_OR);
      imagNames = SDDS_MatchColumns(SDDSin, &imagName1, SDDS_MATCH_STRING, typeMode, typeValue, 
                                     imagPattern, SDDS_0_PREVIOUS|SDDS_OR);
      break;
    default:
      SDDS_Bomb("invalid typeMode in expandColumnPairNames");
      exit(1);
      break;
    }
    if (realNames==0)
      continue;
    if (realNames==-1 || imagNames==-1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      SDDS_Bomb("unable to perform column name match in expandColumnPairNames");
    }
    if (realNames!=imagNames)
      SDDS_Bomb("found different number of real and imaginary columns");
    if (excludeNames) {
      for (j=0; j<excludeNames; j++) 
        for (k=0; k<realNames; k++)
          if (wild_match(realName1[k], excludeName[j])) {
            free(realName1[k]);
            free(imagName1[k]);
            imagName1[k] = realName1[k] = NULL;
          } 
    }
    moveToStringArrayComplex(&realName2, &imagName2, &names2, realName1, imagName1, realNames);
    free(realName1);
    free(imagName1);
  }
  free(realPattern);
  free(imagPattern);
  if (names2==0)
    return 0;
  *realName = realName2;
  *imagName = imagName2;
  return names2;
}

void moveToStringArrayComplex(char ***targetReal, char ***targetImag, long *targets, 
                              char **sourceReal, char **sourceImag, long sources)
{
  long i, j;
  if (!sources)
    return;
  if (!(*targetReal = SDDS_Realloc(*targetReal, sizeof(**targetReal)*(*targets+sources))) ||
      !(*targetImag = SDDS_Realloc(*targetImag, sizeof(**targetImag)*(*targets+sources))))
    SDDS_Bomb("memory allocation failure");
  for (i=0; i<sources; i++) {
    if (sourceReal[i]==NULL || sourceImag[i]==NULL)
      continue;
    for (j=0; j<*targets; j++)
      if (strcmp(sourceReal[i], (*targetReal)[j])==0)
        break;
    if (j==*targets) {
      (*targetReal)[j] = sourceReal[i];
      (*targetImag)[j] = sourceImag[i];
      *targets += 1;
    }
  }
}

