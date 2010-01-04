/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: sddssampledist.c,v $
 Revision 1.15  2007/06/21 17:38:12  borland
 Restored ability to read from a file and write to a pipe.
 Fixed usage messages for -gaussian, -uniform, and -poisson.

 Revision 1.14  2007/02/12 23:03:59  soliday
 Updated to work with WIN32

 Revision 1.13  2007/01/03 20:15:36  shang
 added gaussian, uniform, and poisson distribution

 Revision 1.12  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.11  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.10  2005/11/07 21:48:11  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.9  2005/11/04 22:46:17  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.8  2005/03/02 01:40:27  borland
 Fixed bug in halton sequences (uninitialized values) that resulted in very
 long run times.
 Added verbose option.

 Revision 1.7  2003/05/08 18:27:40  borland
 Added the haltonOffset feature.

 Revision 1.6  2002/08/14 17:12:53  soliday
 Added Open License

 Revision 1.5  2002/02/10 07:38:01  borland
 Fixed bug that occurred when there was a single input file.

 Revision 1.4  2002/01/21 01:36:04  borland
 Fixed bug in randomization when no group was given.

 Revision 1.3  2002/01/21 00:39:09  borland
 Added units, factor, and offset qualifiers to -columns option.

 Revision 1.2  2002/01/20 23:15:11  borland
 Added option for individual input files for each -column option.
 Can use Halton sequences instead of random numbers, with grouping
 and randomization of order.

 Revision 1.1  2002/01/05 05:17:59  borland
 First version.

 */

#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include <time.h>

#define CLO_PIPE 0
#define CLO_COLUMNS 1
#define CLO_SAMPLES 2
#define CLO_SEED 3
#define CLO_VERBOSE 4
#define CLO_GAUSSIAN 5
#define CLO_UNIFORM 6
#define CLO_POISSON 7
#define CLO_OPTIONS 8

static char *option[CLO_OPTIONS] = {
  "pipe", "columns", "samples", "seed", "verbose", "gaussian", "uniform", "poisson",
    } ;

char *USAGE1 = "sddssampledist [<input>] [<output>] [-pipe=[in][,out]]\n\
-columns=independentVariable=<name>,{cdf=<CDFName> | df=<DFName>}[,output=<name>][,units=<string>][,factor=<value>][,offset=<value>][,datafile=<filename>][,haltonRadix=<primeNumber>[,haltonOffset=<integer>][,randomize[,group=<groupID>]]]\n\
[-columns=...] [-samples=<integer>] [-seed=<integer>] [-verbose] \n\
[-gaussian=columnName=<columnName>[,meanValue=<value>|@<parameter_name>][,sigmaValue=<value>|@<parameter_name>][,units=<string>]] \n\
[-uniform=columnName=<columnName>[,minimumValue=<value>|@<parameter_name>][,maximumValue=<value>|@<parameter_name>][,units=<string>]] \n\
[-poisson=columnName=<columnName>[,meanValue=<value>|@<parameter_name>][,units=<string>]  \n";
char *USAGE2 = "-columns        Gives the name of the independent variable for the distribution\n\
                and the name of the cumulative distribution function (CDF) or the\n\
                distribution function (DF).  The CDF is a nondecreasing function normalized\n\
                to 1.  The DF need not be normalized (a histogram is acceptable, for\n\
                example).  The output samples will have the same name as the independent\n\
                variable unless the output qualifier is given.  By default, the data is\n\
                taken from <input>.  If the datafile qualifier is given, the data is \n\
                taken from the named file.  If haltonRadix is given, then the samples\n\
                are generated using a Halton sequence, rather than random numbers.\n\
                If Halton sequences are used, one can randomize the values in groups\n\
                identified by an integer group ID.  This prevents strange banding from\n\
                arising in configuration space.  Also, the haltonOffset can be used to\n\
                skip the first <n> values in the halton sequence, which generates a\n\
                different but not independent sequence.\n\
                If factor or offset are given, the output values are transformed \n\
                according to x -> factor*x+offset.\n\
-gaussian       Sample gaussian distribution with specified mean and width, <columnName> \n\
                gives the output column. \n\
                If mean value or sigma is specified by meanValue=@<parameter_name> or \n\
                sigmaValue=@<parameter_name>, the mean or sigma value will be obtained from \n\
                input file parameter named by corresponding <parameter_name>.\n\
                Defaut mean is 0 and default sigma is 1. \n";
char *USAGE3 = "-uniform        Sample with uniform distribution with specified minimum and maximum value.\n\
                <columnName> gives the output column. \n\
                If mimimum or maximum value is specified by minimumValue=@<parameter_name> or \n\
                maximumValue=@<parameter_name>, the minimum or maximum value will be obtained from \n\
                input file parameter named by corresponding <parameter_name>.\n\
                The default minimum/maximum value is 0/1. \n\
-poisson        Sample with poisson distribution with specified mean. <columnName> gives the \n\
                output column. If the mean value is specified by meanValue=@<parameter_name>, \n\
                the mean value will be obtained from input file parameter named by <parameter_name>.\n\
                The default mean is 1.\n\
-samples        Specifies the number of samples to generate.\n\
-seed           Specifies the seed for the random number generator.  If not given,\n\
                the generator is seeded from the system clock.\n\
-verbose        Prints information to stderr while working.\n\n\
Program by Michael Borland. (This is version 3, March 2005.)\n";

#define SEQ_DATAFILE       0x0001UL
#define SEQ_INDEPNAME      0x0002UL
#define SEQ_CDFNAME        0x0004UL
#define SEQ_DFNAME         0x0008UL
#define SEQ_OUTPUTNAME     0x0010UL
#define SEQ_HALTONRADIX    0x0020UL
#define SEQ_RANDOMIZE      0x0040UL
#define SEQ_RANDOMGROUP    0x0080UL
#define SEQ_UNITSGIVEN     0x0100UL
#define SEQ_HALTONOFFSET   0x0200UL
#define SEQ_DIRECT_GAUSSIAN    0x0400UL
#define SEQ_DIRECT_UNIFORM     0x0800UL
#define SEQ_DIRECT_POISSON     0x1000UL
 
typedef struct {
  unsigned long flags;
  char *dataFileName, *indepName, *CDFName, *DFName, *outputName, *units, *meanPar, *sigmaPar, *minPar, *maxPar;
  SDDS_DATASET SDDSin;
  int32_t haltonRadix, randomizationGroup, haltonOffset;
  double factor, offset, mean, min, max, sigma;
} SEQ_REQUEST ;

typedef struct {
  long group;
  long *order;
} RANDOMIZED_ORDER;

long CreatePoissonDistributionTable(double **x, double **pos_CDF, double mean);

int main(int argc, char **argv)
{
  int iArg;
  char *input, *output,*meanPar, *sigmaPar, *maxPar, *minPar;
  long i, j, mainInputOpened, haltonID=0, requireInput=0;
  unsigned long pipeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout, *SDDSptr;
  long randomNumberSeed = 0;
  SEQ_REQUEST *seqRequest;
  long samples, values, seqRequests, randomizationGroups=0;
  double *sample, *IVValue, *CDFValue;
  char msgBuffer[1000];
  RANDOMIZED_ORDER *randomizationData = NULL;
  long verbose;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<2) {
    fprintf(stderr, "%s%s%s\n", USAGE1, USAGE2, USAGE3);
    return(1);
  }
  seqRequest = NULL;
  seqRequests = 0;
  output = input = NULL;
  pipeFlags = 0;
  samples = values = 0;
  sample = IVValue = CDFValue = NULL;
  verbose = 0;
  maxPar = minPar = meanPar = sigmaPar = NULL;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_COLUMNS:
        if (scanned[iArg].n_items<3)
          SDDS_Bomb("invalid -columns syntax");
        if (!(seqRequest = SDDS_Realloc(seqRequest, sizeof(*seqRequest)*(seqRequests+1))))
          SDDS_Bomb("memory allocation failure");
        scanned[iArg].n_items -= 1;
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest));
        /*remove following pointer initialization because memset already initialize them */
        seqRequest[seqRequests].randomizationGroup = -1;
        seqRequest[seqRequests].factor = 1;
        seqRequest[seqRequests].offset = 0;
        if (!scanItemList(&seqRequest[seqRequests].flags, 
                          scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "datafile", SDDS_STRING,
                          &seqRequest[seqRequests].dataFileName, 1, SEQ_DATAFILE,
                          "independentvariable", SDDS_STRING, 
                          &seqRequest[seqRequests].indepName, 1, SEQ_INDEPNAME,
                          "cdf", SDDS_STRING, 
                          &seqRequest[seqRequests].CDFName, 1, SEQ_CDFNAME,
                          "df", SDDS_STRING, 
                          &seqRequest[seqRequests].DFName, 1, SEQ_DFNAME,
                          "output", SDDS_STRING, 
                          &seqRequest[seqRequests].outputName, 1, SEQ_OUTPUTNAME,
                          "units", SDDS_STRING, 
                          &seqRequest[seqRequests].units, 1, SEQ_UNITSGIVEN,
                          "haltonradix", SDDS_LONG,
                          &seqRequest[seqRequests].haltonRadix, 1, SEQ_HALTONRADIX,
                          "haltonoffset", SDDS_LONG,
                          &seqRequest[seqRequests].haltonOffset, 1, SEQ_HALTONOFFSET,
                          "randomize", -1, NULL, 0, SEQ_RANDOMIZE,
                          "group", SDDS_LONG, 
                          &seqRequest[seqRequests].randomizationGroup, 1, SEQ_RANDOMGROUP,
                          "factor", SDDS_DOUBLE, 
                          &seqRequest[seqRequests].factor, 1, 0,
                          "offset", SDDS_DOUBLE, 
                          &seqRequest[seqRequests].offset, 1, 0,
                          NULL) ||
            bitsSet(seqRequest[seqRequests].flags&(SEQ_INDEPNAME+SEQ_CDFNAME+SEQ_DFNAME))!=2)
          SDDS_Bomb("invalid -columns syntax");
        if (seqRequest[seqRequests].flags&SEQ_RANDOMGROUP &&
            seqRequest[seqRequests].randomizationGroup<=0)
          SDDS_Bomb("use a positive integer for the randomization group ID");
        if (seqRequest[seqRequests].flags&SEQ_CDFNAME &&
            seqRequest[seqRequests].flags&SEQ_DFNAME) 
          SDDS_Bomb("give df or cdf for -columns, not both");
        if (seqRequest[seqRequests].flags&SEQ_HALTONRADIX &&
            !is_prime(seqRequest[seqRequests].haltonRadix))
          SDDS_Bomb("halton radix must be a prime number");
        seqRequests ++;
        scanned[iArg].n_items += 1;
        break;
      case CLO_GAUSSIAN:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -gaussian syntax");
        if (!(seqRequest = SDDS_Realloc(seqRequest, sizeof(*seqRequest)*(seqRequests+1))))
          SDDS_Bomb("memory allocation failure");
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest)); 
        scanned[iArg].n_items -= 1;
        seqRequest[seqRequests].randomizationGroup = -1;
        seqRequest[seqRequests].mean = 0;
        seqRequest[seqRequests].sigma = 1;
        if (!scanItemList(&seqRequest[seqRequests].flags, 
                          scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "columnName", SDDS_STRING, &seqRequest[seqRequests].outputName, 1, SEQ_OUTPUTNAME,
                          "meanValue", SDDS_STRING,  &meanPar, 1, 0,
                          "sigmaValue", SDDS_STRING, &sigmaPar, 1, 0, 
                          "units", SDDS_STRING, &seqRequest[seqRequests].units, 1, SEQ_UNITSGIVEN,
                          NULL))
          SDDS_Bomb("invalid -gaussian syntax");
        seqRequest[seqRequests].flags |= SEQ_DIRECT_GAUSSIAN; 
        if (!(seqRequest[seqRequests].flags&SEQ_OUTPUTNAME) || !(seqRequest[seqRequests].outputName))
          SDDS_Bomb("columnName is not provided for gaussian distribution/");
        if (meanPar) {
          if (wild_match(meanPar, "@*"))
            SDDS_CopyString(&seqRequest[seqRequests].meanPar, meanPar+1);
          else if (!get_double(&seqRequest[seqRequests].mean, meanPar))  
            SDDS_Bomb("Invalid value given for mean value of -gaussian distribution.");
          free(meanPar);
          meanPar = NULL;
        }
        if (sigmaPar) {
          if (wild_match(sigmaPar, "@*"))
            SDDS_CopyString(&seqRequest[seqRequests].sigmaPar, sigmaPar+1);
          else if (!get_double(&seqRequest[seqRequests].sigma, sigmaPar)) 
            SDDS_Bomb("Invalid value given for sigma value of -gaussian distribution.");
          free(sigmaPar);
          sigmaPar = NULL;
        }
        seqRequests ++;
        scanned[iArg].n_items += 1;
        break;
      case CLO_UNIFORM:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -uniform syntax");
        if (!(seqRequest = SDDS_Realloc(seqRequest, sizeof(*seqRequest)*(seqRequests+1))))
          SDDS_Bomb("memory allocation failure");
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest));
        scanned[iArg].n_items -= 1;
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest));
        seqRequest[seqRequests].randomizationGroup = -1;
        seqRequest[seqRequests].min = 0;
        seqRequest[seqRequests].max = 1;
        if (!scanItemList(&seqRequest[seqRequests].flags, 
                          scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "columnName", SDDS_STRING, &seqRequest[seqRequests].outputName, 1, SEQ_OUTPUTNAME,
                          "minimumValue", SDDS_STRING, &minPar, 1, 0,
                          "maximumValue", SDDS_STRING, &maxPar, 1, 0,
                          "units", SDDS_STRING, &seqRequest[seqRequests].units, 1, SEQ_UNITSGIVEN, 
                          NULL))
          SDDS_Bomb("invalid -uniform syntax");
        seqRequest[seqRequests].flags |= SEQ_DIRECT_UNIFORM; 
        if (!(seqRequest[seqRequests].flags&SEQ_OUTPUTNAME) || !(seqRequest[seqRequests].outputName))
          SDDS_Bomb("columnName is not provided for uniform distribution/");
        if (minPar) {
          if (wild_match(minPar, "@*"))
            SDDS_CopyString(&seqRequest[seqRequests].minPar, minPar+1);
          else if (!get_double(&seqRequest[seqRequests].min, minPar))  
            SDDS_Bomb("Invalid value given for minimum value of -uniform distribution.");
          free(minPar);
          minPar = NULL;
        }
        if (maxPar) {
          if (wild_match(maxPar, "@*"))
            SDDS_CopyString(&seqRequest[seqRequests].maxPar, maxPar+1);
          else if (!get_double(&seqRequest[seqRequests].max, maxPar))  
            SDDS_Bomb("Invalid value given for maximum value of -uniform distribution.");
          free(maxPar);
          maxPar = NULL;
        }
        seqRequests ++;
        scanned[iArg].n_items += 1;
        break;
      case CLO_POISSON:
        if (scanned[iArg].n_items<2)
          SDDS_Bomb("invalid -poisson syntax");
        if (!(seqRequest = SDDS_Realloc(seqRequest, sizeof(*seqRequest)*(seqRequests+1))))
          SDDS_Bomb("memory allocation failure");
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest));
        scanned[iArg].n_items -= 1;
        memset(seqRequest+seqRequests, 0, sizeof(*seqRequest));
        seqRequest[seqRequests].randomizationGroup = -1;
        seqRequest[seqRequests].mean = 1;
        if (!scanItemList(&seqRequest[seqRequests].flags, 
                          scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "columnName", SDDS_STRING, &seqRequest[seqRequests].outputName, 1, SEQ_OUTPUTNAME,
                          "meanValue", SDDS_STRING, &meanPar, 1, 0,
                          "units", SDDS_STRING, &seqRequest[seqRequests].units, 1, SEQ_UNITSGIVEN, 
                          NULL))
          SDDS_Bomb("invalid -poisson syntax");
        seqRequest[seqRequests].flags |= SEQ_DIRECT_POISSON; 
        if (!(seqRequest[seqRequests].flags&SEQ_OUTPUTNAME) || !(seqRequest[seqRequests].outputName))
          SDDS_Bomb("columnName is not provided for uniform distribution/");
        if (meanPar) {
          if (wild_match(meanPar, "@*"))
            SDDS_CopyString(&seqRequest[seqRequests].meanPar, meanPar+1);
          else if (!get_double(&seqRequest[seqRequests].mean, meanPar))  
            SDDS_Bomb("Invalid value given for mean value of -poisson distribution.");
          free(meanPar);
          meanPar = NULL;
        } 
        seqRequests ++;
        scanned[iArg].n_items += 1;
        break;
      case CLO_SAMPLES:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%ld", &samples)!=1 || 
            samples<=0)
          SDDS_Bomb("invalid -samples syntax");
        break;
      case CLO_SEED:
        if (scanned[iArg].n_items!=2 ||
            sscanf(scanned[iArg].list[1], "%ld", &randomNumberSeed)!=1 || 
            randomNumberSeed<=0)
          SDDS_Bomb("invalid -seed syntax");
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_VERBOSE:
        verbose = 1;
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
  
  if (!seqRequests)
    SDDS_Bomb("give one or more -columns options");
  if (samples<1)
    SDDS_Bomb("-samples option not given");

  for (i=0; i<seqRequests; i++) {
    if (!(seqRequest[i].flags&
          (SEQ_DATAFILE|SEQ_DIRECT_GAUSSIAN|SEQ_DIRECT_UNIFORM|SEQ_DIRECT_POISSON)))
      break;
  }
  if (i==seqRequests) {
    /* all columns options have either their own input files or else use
     * one of the "direct" distributions.  Hence, we don't expect an input
     * file.
     */
    if (!input)
      pipeFlags |= USE_STDIN;   /* not really, but fakes out processFilenames */
    if (input && !output) {
      output = input;
      input = NULL;
      pipeFlags |= USE_STDIN;
      if (fexists(output)) {
        sprintf(msgBuffer, "%s exists already (sddssampledist)", output);
        SDDS_Bomb(msgBuffer);
      }
    }
  }
  
  processFilenames("sddssampledist", &input, &output, pipeFlags, 0, NULL);
  
  if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, NULL, output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (verbose)
    fprintf(stderr, "Initialized output file %s\n", output);
  
  /* open and check input files */
  for (i=mainInputOpened=0; i<seqRequests; i++) {
    if (seqRequest[i].flags&SEQ_DIRECT_GAUSSIAN) {
      if (seqRequest[i].meanPar || seqRequest[i].sigmaPar) {
        if (!mainInputOpened) {
          if (!SDDS_InitializeInput(&SDDSin, input) ||
              !SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, 0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          mainInputOpened = 1;
        }
        requireInput = 1;
        SDDSptr = &SDDSin; 
        if ((seqRequest[i].meanPar &&
             SDDS_CheckParameter(SDDSptr, seqRequest[i].meanPar, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
            (seqRequest[i].sigmaPar &&
             SDDS_CheckParameter(SDDSptr, seqRequest[i].sigmaPar, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_DefineSimpleColumn(&SDDSout, seqRequest[i].outputName, NULL, SDDS_DOUBLE)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
    } else if (seqRequest[i].flags&SEQ_DIRECT_UNIFORM) {
      if (seqRequest[i].minPar || seqRequest[i].maxPar) {
        if (!mainInputOpened) {
          if (!SDDS_InitializeInput(&SDDSin, input) ||
              !SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, 0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          mainInputOpened = 1;
        }
        requireInput = 1;
        SDDSptr = &SDDSin;
        if ((seqRequest[i].minPar &&
             SDDS_CheckParameter(SDDSptr, seqRequest[i].minPar, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
            (seqRequest[i].maxPar &&
             SDDS_CheckParameter(SDDSptr, seqRequest[i].maxPar, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_DefineSimpleColumn(&SDDSout, seqRequest[i].outputName, NULL, SDDS_DOUBLE))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    } else if (seqRequest[i].flags&SEQ_DIRECT_POISSON) {
      if (seqRequest[i].meanPar) {
        if (!mainInputOpened) {
          if (!SDDS_InitializeInput(&SDDSin, input) ||
              !SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, 0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          mainInputOpened = 1;
        }
        requireInput = 1;
        SDDSptr = &SDDSin;
        if ( SDDS_CheckParameter(SDDSptr, seqRequest[i].meanPar, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_DefineSimpleColumn(&SDDSout, seqRequest[i].outputName, NULL, SDDS_LONG))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    } else {     
      if (seqRequest[i].flags&SEQ_RANDOMIZE) {
        long newGroupID=0;
        /* define randomization groups */
        if (seqRequest[i].flags&SEQ_RANDOMGROUP) {
          newGroupID = seqRequest[i].randomizationGroup;
          for (j=0; j<randomizationGroups; j++)
            if (randomizationData[j].group==newGroupID) {
              newGroupID = 0;
              break;
            }
        } else {
          seqRequest[i].randomizationGroup = newGroupID = -(i+1);
        }
        if (newGroupID!=0) {
          if (!(randomizationData = 
                SDDS_Realloc(randomizationData,
                             sizeof(*randomizationData)*(randomizationGroups+1))))
            SDDS_Bomb("memory allocation failure");
          randomizationData[randomizationGroups].group = newGroupID;
          randomizationData[randomizationGroups].order = NULL;
          randomizationGroups ++;
        }
      }
      if (seqRequest[i].flags&SEQ_DATAFILE) {
        if (!SDDS_InitializeInput(&seqRequest[i].SDDSin, 
                                  seqRequest[i].dataFileName))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        SDDSptr = &seqRequest[i].SDDSin;
      } else {
        if (!mainInputOpened) {
          if (!SDDS_InitializeInput(&SDDSin, input) ||
              !SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, 0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          mainInputOpened = 1;
        }
        requireInput = 1;
        SDDSptr = &SDDSin;
      }
      if (SDDS_CheckColumn(SDDSptr,
                           seqRequest[i].indepName, NULL, SDDS_ANY_NUMERIC_TYPE,
                           stderr)!=SDDS_CHECK_OK  ||
          ((seqRequest[i].flags&SEQ_CDFNAME) &&
           SDDS_CheckColumn(SDDSptr, seqRequest[i].CDFName, NULL, SDDS_ANY_NUMERIC_TYPE,
                            stderr)!=SDDS_CHECK_OK) ||
          ((seqRequest[i].flags&SEQ_DFNAME) &&
           SDDS_CheckColumn(SDDSptr, seqRequest[i].DFName, NULL, SDDS_ANY_NUMERIC_TYPE,
                            stderr)!=SDDS_CHECK_OK) ||
          !SDDS_TransferColumnDefinition(&SDDSout, SDDSptr,
                                         seqRequest[i].indepName,
                                         seqRequest[i].outputName)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      } 
    }
    
    if (seqRequest[i].flags&SEQ_UNITSGIVEN &&
        !SDDS_ChangeColumnInformation(&SDDSout, "units", seqRequest[i].units,
                                      SDDS_SET_BY_NAME, 
                                      seqRequest[i].outputName))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (verbose)
    fprintf(stderr, "Initialized input files\n");

  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (randomNumberSeed==0) {
    randomNumberSeed = (long)time((time_t *) NULL);
    randomNumberSeed = 2*(randomNumberSeed/2) + 1;
#if defined(_WIN32)
    random_1(-labs(randomNumberSeed));
#else
    random_1(-FABS(randomNumberSeed));
#endif
  } else
    random_1(-randomNumberSeed);
  
  if (!((sample = calloc(sizeof(*sample), samples))))
    SDDS_Bomb("memory allocation failure");
  while (1) {
    if (verbose)
      fprintf(stderr, "Beginning page loop\n");
    if (input && SDDS_ReadPage(&SDDSin)<=0)
      break;
    for (i=0; i<seqRequests; i++) {
      if (seqRequest[i].flags&SEQ_DATAFILE &&
          SDDS_ReadPage(&seqRequest[i].SDDSin)<=0)
        break;
    }
    if (i!=seqRequests)
      break;
    if (!SDDS_StartPage(&SDDSout, samples)  ||
        (input && !SDDS_CopyParameters(&SDDSout, &SDDSin)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (verbose)
      fprintf(stderr, "Defining randomization tables\n");
    /* define randomization tables */
    for (i=0; i<randomizationGroups; i++) {
      if (!(randomizationData[i].order=
            SDDS_Malloc(sizeof(*randomizationData[i].order)*samples)))
        SDDS_Bomb("memory allocation failure");
      for (j=0; j<samples; j++)
        randomizationData[i].order[j] = j;
      randomizeOrder((char*)randomizationData[i].order,
                     sizeof(*randomizationData[i].order), samples, 0,
                     random_1);
    }
    if (verbose)
      fprintf(stderr, "Beginning loop over sequence requests\n");
    for (i=0; i<seqRequests; i++) {
      if (verbose)
        fprintf(stderr, "Processing sequence request %ld\n", i);
      if (seqRequest[i].flags&SEQ_DIRECT_GAUSSIAN) {
        if ((seqRequest[i].meanPar && !SDDS_GetParameterAsDouble(&SDDSin, seqRequest[i].meanPar, &seqRequest[i].mean)) ||
            (seqRequest[i].sigmaPar && !SDDS_GetParameterAsDouble(&SDDSin, seqRequest[i].sigmaPar, &seqRequest[i].sigma)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (j=0; j<samples; j++)
          sample[j] = gauss_rn_lim(seqRequest[i].mean, seqRequest[i].sigma, -1, random_1);
      } else if (seqRequest[i].flags&SEQ_DIRECT_UNIFORM) {
        if ((seqRequest[i].minPar && !SDDS_GetParameterAsDouble(&SDDSin, seqRequest[i].minPar, &seqRequest[i].min)) ||
            (seqRequest[i].maxPar && !SDDS_GetParameterAsDouble(&SDDSin, seqRequest[i].maxPar, &seqRequest[i].max)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (j=0; j<samples; j++) 
          sample[j] = seqRequest[i].min + (seqRequest[i].max - seqRequest[i].min) * random_1(1);
      } else if (seqRequest[i].flags&SEQ_DIRECT_POISSON) {
        double *pos_x, *pos_cdf, CDF;
        long pos_points, code;
        pos_x = pos_cdf = NULL;
        if ((seqRequest[i].meanPar && !SDDS_GetParameterAsDouble(&SDDSin, seqRequest[i].meanPar, &seqRequest[i].mean)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        pos_points = CreatePoissonDistributionTable(&pos_x, &pos_cdf, seqRequest[i].mean);
        
        for (j=0; j<samples; j++) {
          CDF = random_1(1);
          sample[j]= (int)(interp(pos_x, pos_cdf, pos_points, CDF, 0, 1, &code));
        /*  fprintf(stderr, "%d, cdf=%f, sample=%f\n", j, CDF, sample[j]);*/
          
        }
        free(pos_x);
        free(pos_cdf);
      } else {
        if (input && !(seqRequest[i].flags&SEQ_DATAFILE))
          SDDSptr = &SDDSin;
        else
          SDDSptr = &seqRequest[i].SDDSin;
        if ((values = SDDS_CountRowsOfInterest(SDDSptr))) {
          if (!(IVValue 
                = SDDS_GetColumnInDoubles(SDDSptr, seqRequest[i].indepName)) ||
              (seqRequest[i].flags&SEQ_CDFNAME &&
               !(CDFValue 
                 = SDDS_GetColumnInDoubles(SDDSptr, seqRequest[i].CDFName))) ||
            (seqRequest[i].flags&SEQ_DFNAME &&
             !(CDFValue 
               = SDDS_GetColumnInDoubles(SDDSptr, seqRequest[i].DFName))))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        } else {
          sprintf(msgBuffer, "empty page for file %s\n",
                  seqRequest[i].flags&SEQ_DATAFILE?
                  seqRequest[i].dataFileName:input);
          SDDS_Bomb(msgBuffer);
        }
        if (verbose)
          fprintf(stderr, "Checking and converting CDF/DF values\n");
        /* check/convert CDF/DF values */
        for (j=1; j<values; j++) {
          if (IVValue[j-1]>IVValue[j]) {
            sprintf(msgBuffer, "random variate values not monotonically increasing for %s", 
                    seqRequest[i].flags&SEQ_DATAFILE?
                    seqRequest[i].dataFileName:input);
            SDDS_Bomb(msgBuffer);
          }
          if (seqRequest[i].flags&SEQ_DFNAME) 
            /* convert DF to CDF */
            CDFValue[j] += CDFValue[j-1];
          if (CDFValue[j] < CDFValue[j-1]) {
            sprintf(msgBuffer, "CDF values decreasing for %s", 
                    seqRequest[i].flags&SEQ_DATAFILE?
                    seqRequest[i].dataFileName:input);
            SDDS_Bomb(msgBuffer);
          }
        }
        if (verbose)
          fprintf(stderr, "Normalizing CDF\n");
        /* normalize the CDF */
        if (CDFValue[values-1]<=0) {
          sprintf(msgBuffer, "CDF not valid for %s\n", 
                  seqRequest[i].dataFileName);
          SDDS_Bomb(msgBuffer);
        }
        for (j=0; j<values; j++) 
          CDFValue[j] /= CDFValue[values-1];
        if (seqRequest[i].flags&SEQ_HALTONRADIX) {
          if (verbose)
            fprintf(stderr, "Starting halton sequence, offset=%" PRId32 "\n", seqRequest[i].haltonOffset);
        haltonID = startHaltonSequence(&seqRequest[i].haltonRadix, 0.5);
          while (seqRequest[i].haltonOffset-- >0)
            nextHaltonSequencePoint(haltonID);
        }
        if (verbose)
          fprintf(stderr, "Generating samples\n");
        for (j=0; j<samples; j++) {
          double CDF;
          long code;
          while (1) {
            if (seqRequest[i].flags&SEQ_HALTONRADIX)
              CDF = nextHaltonSequencePoint(haltonID);
            else 
              CDF = random_1(1);
            if (CDF<=CDFValue[values-1] && CDF>=CDFValue[0])
              break;
          }
          sample[j] 
            = seqRequest[i].factor*interp(IVValue, CDFValue, values, CDF, 0, 1, &code) 
              + seqRequest[i].offset;
        }
        if (seqRequest[i].flags&SEQ_RANDOMIZE) {
          long k, l;
          double *sample1;
          if (verbose)
            fprintf(stderr, "Randomizing order of values\n");
          if (!(sample1 = malloc(sizeof(*sample1)*samples)))
            SDDS_Bomb("memory allocation failure");
          for (l=0; l<randomizationGroups; l++) 
            if (randomizationData[l].group==seqRequest[i].randomizationGroup)
              break;
        if (l==randomizationGroups)
          SDDS_Bomb("problem with construction of randomization groups!");
          for (k=0; k<samples; k++)
            sample1[k] = sample[randomizationData[l].order[k]];
          free(sample);
          sample = sample1;
        } 
        free(IVValue);
        free(CDFValue);
      }
      if (verbose)
        fprintf(stderr, "Setting SDDS column values\n");
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, 
                                     sample, samples, 
                                     seqRequest[i].outputName?
                                     seqRequest[i].outputName:
                                     seqRequest[i].indepName))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
    }
    if (verbose)
      fprintf(stderr, "Writing data page\n");
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!requireInput)
      break;
  }
  if (verbose)
    fprintf(stderr, "Exited read loop\n");
  free(sample);
  if ((input && !SDDS_Terminate(&SDDSin)) || !SDDS_Terminate(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<seqRequests; i++) {
    if (seqRequest[i].dataFileName) free(seqRequest[i].dataFileName);
    if (seqRequest[i].indepName) free(seqRequest[i].indepName);
    if (seqRequest[i].outputName) free(seqRequest[i].outputName);
    if (seqRequest[i].DFName) free(seqRequest[i].DFName);
    if (seqRequest[i].CDFName) free(seqRequest[i].CDFName);
    if (seqRequest[i].units) free(seqRequest[i].units);
    if (seqRequest[i].meanPar) free(seqRequest[i].meanPar);
    if (seqRequest[i].sigmaPar) free(seqRequest[i].sigmaPar);
    if (seqRequest[i].minPar) free(seqRequest[i].minPar);
    if (seqRequest[i].maxPar) free(seqRequest[i].maxPar); 
    if (seqRequest[i].flags&SEQ_DATAFILE && !SDDS_Terminate(&(seqRequest[i].SDDSin)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  free(seqRequest);
  for (i=0; i<randomizationGroups; i++)
    free(randomizationData[i].order);
  if (randomizationData) free(randomizationData);
  
  free_scanargs(&scanned, argc);
  
  return(0);
}

long CreatePoissonDistributionTable(double **x, double **pos_CDF, double mean)
{
  long i, npoints=20, count=0;
  double *pos=NULL;
  SDDS_DATASET pos_out;
  
  *x = *pos_CDF = NULL;
  if (!(*x=malloc(sizeof(**x)*npoints)) || 
      !(pos=malloc(sizeof(*pos)*npoints)) || !(*pos_CDF=malloc(sizeof(**pos_CDF)*npoints)))
    SDDS_Bomb("memeroy allocation failure.");
  i = count = 0; 
  while (1) {
    if (count+2>=npoints) {
      npoints += 20;
      *x = SDDS_Realloc(*x, sizeof(**x)*npoints);
      *pos_CDF = SDDS_Realloc(*pos_CDF, sizeof(**pos_CDF)*npoints);
      pos = SDDS_Realloc(pos, sizeof(*pos)*npoints);
    }
    
    (*x)[count] = i;
    if (!i) {
      pos[i] = exp(-mean);
      (*pos_CDF)[count] = pos[i];
      count ++;
    }
    else {
      pos[i] = pos[i-1]*mean/i;
      (*pos_CDF)[count] = (*pos_CDF)[count-1];
      (*pos_CDF)[count+1] = (*pos_CDF)[count-1] + pos[i];
      (*x)[count+1] = i;
      if (1.0-(*pos_CDF)[count+1]<=1.0e-15)
        break;
      count +=2;
    }
    i++;
  }
 /* fprintf(stderr,"lamda=%f\n", mean);
  if (!SDDS_InitializeOutput(&pos_out, SDDS_BINARY, 0, NULL, NULL, "pos_dist.sdds"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_DefineSimpleColumn(&pos_out, "Count", NULL, SDDS_DOUBLE) ||
      !SDDS_DefineSimpleColumn(&pos_out, "P", NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_SaveLayout(&pos_out) || !SDDS_WriteLayout(&pos_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_StartPage(&pos_out, count) ||
      !SDDS_SetColumnFromDoubles(&pos_out, SDDS_SET_BY_NAME, *x, count, "Count") ||
      !SDDS_SetColumnFromDoubles(&pos_out, SDDS_SET_BY_NAME, *pos_CDF, count, "P"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_WritePage(&pos_out) || !SDDS_Terminate(&pos_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  */
  free(pos);
  return count;
}


