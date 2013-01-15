/*************************************************************************\
* Copyright (c) 2010 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2010 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsduplicate
 * purpose: duplicate rows in a file according to a weight column
 *
 * Michael Borland, 2010
 $Log: not supported by cvs2svn $
 Revision 1.1  2010/09/08 19:40:41  borland
 First version.

 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_WEIGHT 0
#define SET_PIPE 1
#define SET_MAXFACTOR 2
#define SET_MINFACTOR 3
#define SET_FACTOR 4
#define SET_VERBOSITY 5
#define SET_SEED 6
#define SET_PROBABILISTIC 7
#define N_OPTIONS 8
static char *option[N_OPTIONS] = {
  "weight", "pipe", "maxfactor", "minfactor", "factor", "verbosity", "seed", "probabilistic",
};

static char *USAGE="sddsduplicate [<input>] [<output>] [-pipe=[input][,output]]\n\
{-weight=<columnName> {-minFactor=<integer> | -maxFactor=<integer>} | -factor=<integer>}\n\
[-probabilistic] [-seed=<integer>] [-verbosity[=<level>]]\n\
This program duplicates rows in the input file and creates a new file.\n\
The number of duplicates is determined either by a weight column or by a fixed value.\n\
-weight     Name of a column to use for weighting the number of duplicates.\n\
-minFactor  Minimum number of rows to emit.  Results in scaling of weights.\n\
-maxFactor  Maximum number of rows to emit.  Results in scaling of weights.\n\
            In some cases, input rows will not be reflected in the output file\n\
            because the weight is less than 1.\n\
-factor     Number of duplicates to create.  Incompatible with -weight.\n\
-probabilistic\n\
            If given, then fractional duplication counts are taken as probabilities.\n\
-seed       Set seed for random number generator.  By default, seeded by the \n\
            system clock.\n\
-verbosity  Set verbosity level.\n\n\
Program by Michael Borland.  (This is version 1, September 2010.)\n";

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output;
  char *inputfile, *outputfile;
  long i, i_arg, verbosity;
  SCANNED_ARG *s_arg;
  unsigned long pipeFlags;
  char *weightColumnName;
  double *weightData, minWeight, maxWeight;
  double *dupValue;
  long maxFactor, minFactor, dupRows;
  long j, inputRows, storedRows, randomNumberSeed;
  short probabilistic = 0;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  weightColumnName = NULL;
  maxFactor = minFactor = dupRows = 0;
  inputfile = NULL;
  outputfile = NULL;
  pipeFlags = 0;
  verbosity = 0;

  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_WEIGHT:
	if (s_arg[i_arg].n_items!=2 ||
	    !(weightColumnName=s_arg[i_arg].list[1]))
          bomb("invalid -weight syntax", USAGE);
	break;
      case SET_FACTOR:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &dupRows)!=1 ||
            dupRows<=0)
          bomb("invalid -rows syntax", USAGE);
        break;
      case SET_MINFACTOR:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &minFactor)!=1 ||
            minFactor<=0)
          bomb("invalid -minFactor syntax", USAGE);
        break;
      case SET_MAXFACTOR:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &maxFactor)!=1 ||
            maxFactor<=0)
          bomb("invalid -maxFactor syntax", USAGE);
        break;
      case SET_VERBOSITY:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1 ||
            verbosity<0)
          bomb("invalid -verbosity syntax", USAGE);
        break;
      case SET_PROBABILISTIC:
	probabilistic = 1;
        if (s_arg[i_arg].n_items!=1) 
          bomb("invalid -probabilistic syntax", USAGE);
	break;
      case SET_SEED:
        if (s_arg[i_arg].n_items!=2 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &randomNumberSeed)!=1 ||
            randomNumberSeed<0)
          bomb("invalid -seed syntax", USAGE);
        break;
      default:
        bomb("unrecognized option", USAGE);
	break;
      }
    }
    else {
      if (!inputfile)
	inputfile = s_arg[i_arg].list[0];
      else if (!outputfile)
	outputfile = s_arg[i_arg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }
  if (minFactor && maxFactor) {
    SDDS_Bomb("give only one of -minFactor and -maxFactor");
  }

  processFilenames("sddsduplicate", &inputfile, &outputfile, pipeFlags, 0, NULL);
  
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

  if (!SDDS_InitializeInput(&SDDS_input, inputfile) ||
      !SDDS_InitializeCopy(&SDDS_output, &SDDS_input, outputfile, "w") ||
      !SDDS_WriteLayout(&SDDS_output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  dupValue = NULL;
  while (SDDS_ReadPage(&SDDS_input)>0) {
    inputRows = SDDS_RowCount(&SDDS_input);
    if (inputRows) {
      dupValue = tmalloc(sizeof(*dupValue)*inputRows);
      if (weightColumnName) {
	if (!(weightData = SDDS_GetColumnInDoubles(&SDDS_input, weightColumnName)))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	if (minFactor) {
	  find_min_max(&minWeight, &maxWeight, weightData, inputRows);
	  if (minWeight<=0)
	    SDDS_Bomb("Minimum weight value is nonpositive.  Can't use -minFactor.");
	  for (i=0; i<inputRows; i++) 
	    dupValue[i] = weightData[i]*minFactor/minWeight;
	} else if (maxFactor) {
	  find_min_max(&minWeight, &maxWeight, weightData, inputRows);
	  if (maxWeight<=0)
	    SDDS_Bomb("Maximum weight value is nonpositive.  Can't use -maxFactor.");
	  for (i=0; i<inputRows; i++)
	    dupValue[i] = weightData[i]*maxFactor/maxWeight;
	} else {
	  for (i=0; i<inputRows; i++) 
	    dupValue[i] = weightData[i];
	}
	if (probabilistic) {
	  double fraction;
	  for (i=0; i<inputRows; i++) {
	    fraction = dupValue[i] - ((long)dupValue[i]);
	    dupValue[i] = (long)dupValue[i];
	    if (fraction>random_1(0))
	      dupValue[i] += 1;
	  }
	} else 
	  for (i=0; i<inputRows; i++) 
	    dupValue[i] = (long)dupValue[i];
      } else {
	for (i=0; i<inputRows; i++)
	  dupValue[i] = dupRows;
      }
      storedRows = 0;
      for (i=0; i<inputRows; i++) 
	storedRows += (long)dupValue[i];
      if (verbosity) {
	long maxDup = 0, minDup = LONG_MAX;
	for (i=0; i<inputRows; i++) {
	  if (maxDup<dupValue[i])
	    maxDup = dupValue[i];
	  if (minDup>dupValue[i])
	    minDup = dupValue[i];
	}
	fprintf(stderr, "%ld output rows, mininum and maximum duplication factor: %ld, %ld\n", storedRows, minDup, maxDup);
      }
      if (!SDDS_StartPage(&SDDS_output, storedRows) ||
	  !SDDS_CopyParameters(&SDDS_output, &SDDS_input) ||
	  !SDDS_CopyArrays(&SDDS_output, &SDDS_input)) 
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      storedRows = 0;
      for (i=0; i<inputRows; i++) {
	for (j=0; j<dupValue[i]; j++) {
	  if (SDDS_CopyRowDirect(&SDDS_output, storedRows++, &SDDS_input, i)) 
	    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	}
      }
      if (!SDDS_WritePage(&SDDS_output)) 
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      free(dupValue);
      dupValue = NULL;
    }
  }
  
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  return(0);
}

