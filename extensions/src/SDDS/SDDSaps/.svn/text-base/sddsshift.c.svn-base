/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*********************************************************************/
/* sddsshift - shift data column  */
/*********************************************************************/

/* program by J Carwardine 
 $Log: not supported by cvs2svn $
 Revision 1.14  2003/09/02 19:16:07  soliday
 Cleaned up code for Linux.

 Revision 1.13  2003/01/21 18:57:47  borland
 Added yet another argument for simplexMin().

 Revision 1.12  2003/01/16 18:49:10  soliday
 Changed simplexMin because the arguments changed.

 Revision 1.11  2002/08/14 17:12:54  soliday
 Added Open License

 Revision 1.10  2001/08/08 19:21:18  borland
 Made consistent with changes to simplexMin() (addition of n_divisions argument).

 Revision 1.9  2001/05/15 16:41:27  borland
 sddsgenericfit now supports the "no1dscans" option of simplexMin() from
 the commandline.  Others were modified to accomodate the new option,
 but don't allow commandline specification.

 Revision 1.8  2001/05/03 20:55:35  soliday
 Standardized usage messages.

 Revision 1.7  2001/01/10 19:35:46  soliday
 Standardized usage message.

 Revision 1.6  2000/04/13 17:11:50  soliday
 Added missing include statment.

 Revision 1.5  1999/05/25 19:15:28  soliday
 Removed compiler warning on linux.

 Revision 1.4  1999/01/07 22:25:15  borland
 Brought up-to-date with latest version of simplexMin().  Made some improvements
 to sddssinefit but it still doesn't do well.

 * Revision 1.3  1995/09/06  14:57:08  saunders
 * First test release of SDDS1.5
 *
*/

#include "SDDS.h"
#include "mdb.h"
#include "scan.h"

#define CLO_PIPE 0
#define CLO_COLUMN 1
#define CLO_DELAY 2
#define CLO_MATCH 3
#define CLO_ZERO 4
#define N_OPTIONS 5

static char *option[N_OPTIONS] = {"pipe",
                                    "column",
                                    "shift",
                                    "match", "zero" };
/**************************/

char *usage = { "usage: sddsshift [<inputfile>] [<outputfile>] \n\
    [-pipe=[input][,output]] -column=<inputcol>\n\
    [-zero] [-shift=<points> | -match=<matchcol>]\n\
-column      provide <inputcol>, i.e., the data column to be shifted.\n\
-shift       provide number of points to shift in rows. \n\
             (positive=later, negative=earlier).\n\
-match       provide <matchcol>. <inputcol> is shifted to \n\
             minimize the least squares error relative to <matchcol>.\n\
-zero        set exposed end-points to zero\n\n\
sddsshift shifts given data column by rows. A copy of <inputfile> is made with the\n\
addition of a new column \"Shifted<inputcol>\". Exposed end-points\n\
are set to zero if zero option is provided or \n\
the value of the first/last row in <inputcol> as appropriate.\n\
A parameter \"<inputcol>Shift\" contains the number of rows shifted.\n" };

/**** function prototypes ****/
void shift( double *inputcol, double *outputcol, long npoints, long delay, long zero );
double mse( double *y1, double *y2, long npoints );
double simplex_driver( double *data, long *invalid );

/**************************/
double *inputCol, *working, *matchCol;
long npoints, zero;


int main( int argc, char *argv[] )
{
  SCANNED_ARG *s_arg;
  double *outputCol;
  char *inputColName, *matchColName, *inputfile, *outputfile;
  long i, i_arg, delay, tmpFileUsed;
  unsigned long pipeFlags;
  char actualName[256], actualDesc[256], delayName[256];
  double simDelay, lower, upper, finalmse;
  SDDS_DATASET SDDS_input, SDDS_output;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if ( argc < 2 ) 
    bomb(NULL, usage);
  
  /* initialize flags & defaults */
  tmpFileUsed = 0;
  pipeFlags = 0;
  inputfile = outputfile = inputColName = matchColName = NULL;
  delay = 0;
  simDelay = 0.0;
  zero = 0;
  inputCol = matchCol = outputCol = working = NULL;
  
  /**** for each argument, compare with possible options ****/
  for (i_arg=1; i_arg < argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_COLUMN:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -column option.");
        inputColName = s_arg[i_arg].list[1];
        break;
      case CLO_DELAY:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -delay option.");
        if (!get_long(&delay, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalid delay value provided.");
        break;
      case CLO_MATCH:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -column option.");
        matchColName = s_arg[i_arg].list[1];
        break;
      case CLO_ZERO:
        zero = 1;
        break;
      default:
        fprintf(stderr, "Error (%s): unknown switch: %s\n", argv[0], s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    } else {
      if (!inputfile)
        inputfile = s_arg[i_arg].list[0];
      else if (!outputfile)
        outputfile = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("Too many files provided.");
    }
  }
  processFilenames("sddsdigfilter", &inputfile, &outputfile, pipeFlags, 1, &tmpFileUsed);
  /* check options */
  if ( !inputColName)
    SDDS_Bomb("The shift column is not given!");
  if (!matchColName && !delay)
    SDDS_Bomb("Either match column or shift should be provided.");
  if (matchColName && delay)
    SDDS_Bomb("-match column option and -shift option are incompatible.");
  sprintf( actualName, "Shifted%s", inputColName );
  sprintf( actualDesc, "Shifted %s", inputColName );
  sprintf( delayName, "%sShift", inputColName );
  /**** get input data using sdds routines ****/
  if (!SDDS_InitializeInput(&SDDS_input,inputfile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_input, outputfile, "w" ) ||
      SDDS_DefineColumn(&SDDS_output, actualName, NULL, NULL, actualDesc, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineParameter(&SDDS_output, delayName, NULL, NULL, NULL, NULL, SDDS_LONG, NULL)<0 ||
      !SDDS_WriteLayout(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  while ((SDDS_ReadPage(&SDDS_input))>0) {
    if (!SDDS_CopyPage(&SDDS_output, &SDDS_input))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
    if ((npoints = SDDS_CountRowsOfInterest(&SDDS_input))<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!(inputCol=SDDS_GetColumnInDoubles(&SDDS_input, inputColName)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if ( matchColName) {
      if (!(matchCol = SDDS_GetColumnInDoubles(&SDDS_input, matchColName)))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if ( (working = (double *) calloc(npoints, sizeof(*working))) == NULL ||
        (outputCol = (double *) calloc(npoints, sizeof(*outputCol))) == NULL ) 
      SDDS_Bomb("memory allocation failure");
    /**** PROCESS DATA ****/
    if ( matchColName ) {
      /* produce least squares error fit in time */
      lower = (double) -npoints + 1;
      upper = (double) npoints - 1;
      simplexMin( &finalmse, &simDelay, NULL, &lower, &upper, NULL, 1,
                 1e-6, 1e-12, simplex_driver, NULL, 2*npoints, 6, 12, 3, 1.0, 0);
      delay = (long) simDelay;
    }
    shift( inputCol, outputCol, npoints, delay, zero );
    if (!SDDS_SetParameters(&SDDS_output, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE,
                            delayName, delay, NULL) ||
        !SDDS_SetColumn(&SDDS_output, SDDS_SET_BY_NAME, outputCol, npoints, actualName) ||
        !SDDS_WritePage(&SDDS_output))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    free(outputCol);
    free(working);
    free(inputCol);
    if (matchCol) free(matchCol);
    outputCol = inputCol = matchCol = working = NULL;
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free_scanargs(&s_arg, argc);
  return(0);
  /**** end of main ****/
}

/*****************************************/

void shift( double *inputcol, double *outputcol, long npoints, long delay, long zero )
{
  long i;

  if ( delay < 0 ) {
    delay *= -1;
    for ( i = 0; i < npoints-delay; i++ ) {
      outputcol[i] = inputcol[i+delay];
    }
    for ( i = (npoints - delay); i < npoints; i++ ) {
      if (zero)
        outputcol[i] = 0;
      else
        outputcol[i] = inputcol[npoints-1];
    }
  }
  else {
    for ( i = (npoints - 1); i > delay; i-- ) {
      outputcol[i] = inputcol[i-delay];
    }
    for ( i = delay; i > 0; i-- ) {
      if (zero)
        outputcol[i] = 0;
      else
        outputcol[i] = inputcol[0];
    }
  }
  /* end of shift() */  
}

/*********************************/

double mse( double *y1, double *y2, long npoints )
{
  
  long i;
  double error;
  
  error = 0;
  for ( i = 0; i < npoints; i++ ) {
    error += (y1[i] - y2[i]) * (y1[i] - y2[i]);
  }
  error /= npoints;
  return error;
/* end of mse() */
}

/**************************************/

double simplex_driver( double *data, long *invalid )
{
  long delay;
  
  *invalid = 0;
  delay = (long) *data;
  shift( inputCol, working, npoints, delay, zero);
  return mse( matchCol, working, npoints );
/* end of simplex_driver() */
}    
