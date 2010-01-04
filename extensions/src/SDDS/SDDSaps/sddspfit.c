/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddspfit
 * purpose: do nth order polynomial least squares fitting for SDDS files.
 *
 * Michael Borland, 1995.
 * Based on mpl program 'lsf'.
 $Log: sddspfit.c,v $
 Revision 1.30  2009/10/29 19:12:53  borland
 Added parameters for each fit coefficient.

 Revision 1.29  2008/06/03 15:58:02  soliday
 Changed the way NaN was defined.

 Revision 1.28  2008/05/21 14:47:48  shang
 fixed a free memory bug.

 Revision 1.27  2008/05/20 20:12:03  shang
 fixed the bug that program crushes when the input file has no data or invalid pages (i.e. the row number is less than the terms). Now the output file has the same number of pages as input, the output page is empty if input page data is invalid.

 Revision 1.26  2007/04/17 17:31:11  soliday
 Fixed issue on WIN32

 Revision 1.25  2007/02/27 19:30:50  borland
 Added -copyParameters option.

 Revision 1.24  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.23  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.22  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.21  2005/11/04 22:46:16  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.20  2005/03/31 21:57:06  soliday
 Output from the verbose option now goes to stdout instead of stderr.

 Revision 1.19  2004/04/21 20:11:49  shang
 added "NULL" to the end of arguments of SDDS_FindColumn(...) to fix the bug
 of core dump error when column does not exist

 Revision 1.18  2003/10/10 18:34:37  borland
 Added -autoOffset option.  This sets the x offset to the mean x value.
 It helps prevent numerical problems when x values have large magnitude.

 Revision 1.17  2003/09/02 19:16:06  soliday
 Cleaned up code for Linux.

 Revision 1.16  2002/08/14 17:12:50  soliday
 Added Open License

 Revision 1.15  2002/01/16 18:25:33  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.14  2001/01/10 19:35:43  soliday
 Standardized usage message.

 Revision 1.13  1999/07/20 19:24:05  borland
 Further improved revise orders fitting.  Now uses two algorithms, the new
 one being a complete search using the orders given by the user, limited
 by a "good-enough" criterion.

 Revision 1.12  1999/07/20 18:02:40  borland
 Improved function of reviseOrders feature.

 Revision 1.11  1999/05/25 19:13:14  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/01/06 19:54:50  borland
 Fixed the version number in the usage message.

 Revision 1.9  1997/08/25 19:21:25  borland
 Fixed if statement that checks for proper sigma mode for revise-orders mode.

 Revision 1.8  1997/08/11 22:17:02  borland
 Removed unnecessary restriction that prevented using -sigma and -generate
 option together.

 Revision 1.7  1997/08/11 21:00:03  borland
 Fixed problems with -generateSigmas=keepX options.  Now they do what the
 manual says they should (i.e., will run with sigmas from file, replacement
 is now based on point-by-point comparision).

 * Revision 1.6  1996/02/14  01:05:29  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.5  1995/12/12  06:50:32  borland
 * Fixed bug in parsing of -sigmas option.
 *
 * Revision 1.4  1995/11/13  15:56:13  borland
 * Added long cast to strlen() in numerical comparison to satisfy Solaris compiler.
 *
 * Revision 1.3  1995/09/06  14:56:54  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

void print_coefs(FILE *fprec, double x_offset, double x_scale, long chebyshev,
                 double *coef, double *s_coef, int32_t *order, long n_terms, double chi, 
                 long norm_term, char *prepend);
char **makeCoefficientUnits(SDDS_DATASET *SDDSout, char *xName, char *yName, 
                            int32_t *order, long terms);
long setCoefficientData(SDDS_DATASET *SDDSout, double *coef, double *coefSigma, char **coefUnits,
                        int32_t *order, long terms);
char **initializeOutputFile(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin, char *input,
			    char *xName, char *yName, char *xSigmaName, char *ySigmaName,
			    long sigmasValid, int32_t *order, long terms, long isChebyshev,
                            long copyParameters);
void checkInputFile(SDDS_DATASET *SDDSin, char *xName, char *yName, char *xSigmaName, char *ySigmaName);
long coefficient_index(int32_t *order, long terms, long order_of_interest);
void makeFitLabel(char *buffer, long bufsize, char *fitLabelFormat, double *coef, int32_t *order, long terms);

void set_argument_offset(double offset);
void set_argument_scale(double scale);
double tcheby(double x, long n);
double dtcheby(double x, long n);
double ipower(double x, long n);
double dipower(double x, long n);
long reviseFitOrders(double *x, double *y, double *sy, long points,
                     long terms, int32_t *order, double *coef, double *coefSigma,
                     double *diff, double (*basis_fn)(double xa, long ordera),
                     unsigned long reviseOrders, double xOffset, double xScaleFactor,
                     long normTerm, long ySigmasValid,
                     long chebyshev, double revpowThreshold, double revpowCompleteThres,
                     double goodEnoughChi);
long reviseFitOrders1(double *x, double *y, double *sy, long points,
		      long terms, int32_t *order, double *coef, double *coefSigma,
		      double *diff, double (*basis_fn)(double xa, long ordera),
		      unsigned long reviseOrders, double xOffset, double xScaleFactor,
		      long normTerm, long ySigmasValid,
		      long chebyshev, double revpowThreshold, double goodEnoughChi);

#define CLO_COLUMNS 0
#define CLO_ORDERS  1
#define CLO_TERMS  2
#define CLO_SYMMETRY  3
#define CLO_REVISEORDERS  4
#define CLO_CHEBYSHEV 5
#define CLO_MODIFYSIGMAS  6
#define CLO_SIGMAS  7
#define CLO_GENERATESIGMAS 8
#define CLO_RANGE  9
#define CLO_SPARSE  10
#define CLO_NORMALIZE  11
#define CLO_XFACTOR  12
#define CLO_XOFFSET  13
#define CLO_VERBOSE 14
#define CLO_FITLABELFORMAT 15
#define CLO_PIPE 16
#define CLO_EVALUATE 17
#define CLO_AUTOOFFSET 18
#define CLO_COPY_PARAMETERS 19
#define N_OPTIONS 20

char *option[N_OPTIONS] = {
  "columns", "orders", "terms", "symmetry", "reviseorders", "chebyshev",
  "modifysigmas", "sigmas", "generatesigmas",
  "range", "sparse", "normalize", "xfactor", "xoffset", "verbose", 
  "fitlabelformat", "pipe", "evaluate", "autooffset", "copyparameters"
};

char *USAGE="sddspfit [<inputfile>] [<outputfile>] [-pipe=[input][,output]]\n\
  -columns=<xname>,<yname>[,xSigma=<name>][,ySigma=<name>]\n\
  [{-terms=<number> [-symmetry={none|odd|even}] | -orders=<number>[,...]}]\n\
  [-reviseOrders[=threshold=<chiValue>][,verbose][,complete=<chiThreshold>][,goodEnough=<chiValue>]]\n\
  [-chebyshev[=convert]]\n\
  [-xOffset=value] [-autoOffset] -xFactor=value]\n\
  [-sigmas=<value>,{absolute|fractional}] \n\
  [-modifySigmas] [-generateSigmas[={keepLargest,keepSmallest}]]\n\
  [-sparse=<interval>] [-range=<lower>,<upper>]\n\
  [-normalize[=<termNumber>]] [-verbose]\n\
  [-evaluate=<filename>[,begin=<value>][,end=<value>][,number=<integer>]]\n\
  [-fitLabelFormat=<sprintf-string>] [-copyParameters]\n\n\
Program by Michael Borland. ("__DATE__")\n";

static char *additional_help1 = "\n\
sddspfit does fits to the form y = SUM(i){ A[i] *P(x-x_offset, i)}, where P(x,i) is the ith basis\n\
function evaluated at x.  sddspfit returns the A[i] and estimates of the errors in the values.\n\
By default P(x,i) = x^i.  One can also select Chebyshev T polynomials as the basis functions.\n\n\
-columns               specify names of data columns to use.\n\
-terms                 number of terms desired in fit.\n\
-symmetry              symmetry of desired fit about x_offset.\n\
-orders                orders (P[i]) to use in fitting.\n\
-reviseOrders          the orders used in the fit are modified from the specified ones\n\
                       in order eliminate poorly-determined coefficients, based on fitting\n\
                       of the first data page.\n\
-chebyshev             use Chebyshev T polynomials (xOffset is set automatically).\n\
                       Giving the `convert' option causes the fit to be written out in\n\
                       terms of ordinary polynomials.\n\
-xOffset               desired value of x to fit about.\n";
static char *additional_help2 = "-autoOffset            automatically offset x values by the mean x value for fitting.\n\
                       Helpful if x values are very large in magnitude.\n\
-xFactor               desired factor to multiply x values by before fitting.\n\
-sigmas                specify absolute or fractional sigma for all points.\n\
-modifySigmas          modify the y sigmas using the x sigmas and an initial fit.\n\
-generateSigmas        generate y sigmas from the rms deviation from an initial fit.\n\
                       optionally keep the sigmas from the data if larger/smaller than rms\n\
                       deviation.\n\
-sparse                specify integer interval at which to sample data.\n\
-range                 specify range of independent variable to fit over.\n\
-normalize             normalize so that specified term is unity.\n\
-evaluate              specify evaluation of fit over a selected range of\n\
                       equispaced points.\n\
-copyParameters        if given, program copies all parameters from the input file\n\
                       into the main output file.  By default, no parameters are copied.\n\
-verbose               generates extra output that may be useful.\n\n";

#define NO_SYMMETRY 0
#define EVEN_SYMMETRY 1
#define ODD_SYMMETRY 2
#define N_SYMMETRY_OPTIONS 3
char *symmetry_options[N_SYMMETRY_OPTIONS] = {"none", "even", "odd"};

#define ABSOLUTE_SIGMAS 0
#define FRACTIONAL_SIGMAS 1
#define N_SIGMAS_OPTIONS 2
char *sigmas_options[N_SIGMAS_OPTIONS] = {"absolute", "fractional"};

#define FLGS_GENERATESIGMAS 1
#define FLGS_KEEPLARGEST    2
#define FLGS_KEEPSMALLEST   4

#define REVPOW_ACTIVE  0x0001
#define REVPOW_VERBOSE 0x0002
#define REVPOW_COMPLETE 0x0004

/* SDDS indices for output page */
static long iIntercept = -1, iInterceptSigma = -1;
static long iSlope = -1, iSlopeSigma = -1;
static long iCurvature = -1, iCurvatureSigma = -1;
static long *iTerm = NULL;
static long iOffset = -1, iFactor = -1;
static long iChiSq = -1, iRmsResidual = -1, iSigLevel = -1;
static long iFitIsValid = -1, iFitLabel = -1, iTerms = -1;

static long ix = -1, iy = -1, ixSigma = -1, iySigma = -1;
static long iFit = -1, iResidual = -1;

static char *xSymbol, *ySymbol;

#define EVAL_BEGIN_GIVEN  0x0001U
#define EVAL_END_GIVEN    0x0002U
#define EVAL_NUMBER_GIVEN 0x0004U

typedef struct {
  char *file;
  long initialized;
  int32_t number;
  unsigned long flags;
  double begin, end;
  SDDS_DATASET dataset;
} EVAL_PARAMETERS;
void makeEvaluationTable(EVAL_PARAMETERS *evalParameters, double *x, long n,
                         double *coef, int32_t *order, long terms, 
                         SDDS_DATASET *SDDSin, char *xName, char *yName);

static double (*basis_fn)(double xa, long ordera);
static double (*basis_dfn)(double xa, long ordera);
/*#define NaN 0.0/0.0*/

/*#define NaN (*(__const double *) __nan)*/
int main(int argc, char **argv)
{
  unsigned long nan[2] = {0xffffffff, 0x7fffffff};
  double nNum = *(double*)nan;
  double *x, *y, *sy, *sx, *diff, xOffset, xScaleFactor;
  double *xOrig, *yOrig, *sxOrig, *syOrig, *sy0;
  long i, j, points, terms, normTerm, pointsOrig, ySigmasValid;
  long symmetry, chebyshev, autoOffset, copyParameters=0;
  double sigmas;
  long sigmasMode, sparseInterval;
  unsigned long flags;
  double *coef, *coefSigma;
  double chi, xLow, xHigh, rmsResidual;
  char *xName, *yName, *xSigmaName, *ySigmaName;
  char *input, *output, **coefUnits;
  SDDS_DATASET SDDSin, SDDSout;
  long isFit, iArg, modifySigmas, termIndex;
  long generateSigmas, verbose, ignoreSigmas;
  long npages=0, invalid=0;
  int32_t *order;
  SCANNED_ARG *s_arg;
  double xMin, xMax, revpowThreshold, revpowCompleteThres, goodEnoughChi;
  double rms_average(double *d_x, int d_n);
  char *fitLabelFormat = "%g";
  static char fitLabelBuffer[SDDS_MAXLINE];
  unsigned long pipeFlags, reviseOrders;
  EVAL_PARAMETERS evalParameters;

  sxOrig = syOrig = NULL;
  rmsResidual = 0;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2 || argc>(3+N_OPTIONS)) {
    fprintf(stderr, "usage: %s%s%s\n", USAGE, additional_help1, additional_help2);
    exit(1);
  }

  input = output = NULL;
  xName = yName = xSigmaName = ySigmaName = NULL;
  modifySigmas = reviseOrders = chebyshev = 0;
  order = NULL;
  symmetry = NO_SYMMETRY;
  xMin = xMax = 0;
  autoOffset = 0;
  generateSigmas = 0;
  sigmasMode = -1;
  sigmas = 1;
  sparseInterval = 1;
  terms = 2;
  verbose = ignoreSigmas = 0;
  normTerm = -1;
  xOffset = 0;
  xScaleFactor = 1;
  coefUnits = NULL;
  basis_fn = ipower;
  basis_dfn = dipower;
  pipeFlags = 0;
  evalParameters.file = NULL;
  evalParameters.initialized = 0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (s_arg[iArg].arg_type==OPTION) {
      switch (match_string(s_arg[iArg].list[0], option, N_OPTIONS, 0)) {
      case CLO_MODIFYSIGMAS:
	modifySigmas = 1;
	break;
      case CLO_AUTOOFFSET:
        autoOffset = 1;
        break;
      case CLO_ORDERS:
	if (s_arg[iArg].n_items<2)
	  SDDS_Bomb("invalid -orders syntax");
	order = tmalloc(sizeof(*order)*(terms=s_arg[iArg].n_items-1));
	for (i=1; i<s_arg[iArg].n_items; i++) {
	  if (sscanf(s_arg[iArg].list[i], "%" SCNd32, order+i-1)!=1)
	    SDDS_Bomb("unable to scan order from -orders list");
	}
	break;
      case CLO_RANGE:
	if (s_arg[iArg].n_items!=3 ||
	    1!=sscanf(s_arg[iArg].list[1], "%lf", &xMin) ||
	    1!=sscanf(s_arg[iArg].list[2], "%lf", &xMax) ||
	    xMin>=xMax)
	  SDDS_Bomb("incorrect -range syntax");
	break;
      case CLO_GENERATESIGMAS:
	generateSigmas = FLGS_GENERATESIGMAS;
	if (s_arg[iArg].n_items>1) {
	  if (s_arg[iArg].n_items!=2)
	    SDDS_Bomb("incorrect -generateSigmas synax");
	  if (strncmp(s_arg[iArg].list[1], "keepsmallest", strlen(s_arg[iArg].list[1]))==0)
	    generateSigmas |= FLGS_KEEPSMALLEST;
	  if (strncmp(s_arg[iArg].list[1], "keeplargest", strlen(s_arg[iArg].list[1]))==0)
	    generateSigmas |= FLGS_KEEPLARGEST;
	  if ((generateSigmas&FLGS_KEEPSMALLEST) && (generateSigmas&FLGS_KEEPLARGEST))
	    SDDS_Bomb("ambiguous -generateSigmas synax");
	}
	break;
      case CLO_TERMS:
	if (s_arg[iArg].n_items!=2 || sscanf(s_arg[iArg].list[1], "%ld", &terms)!=1)
	  SDDS_Bomb("invalid -terms syntax");
	break;
      case CLO_XOFFSET:
	if (s_arg[iArg].n_items!=2 || sscanf(s_arg[iArg].list[1], "%lf", &xOffset)!=1)
	  SDDS_Bomb("invalid -xOffset syntax");
	break;
      case CLO_SYMMETRY:
	if (s_arg[iArg].n_items==2) {
	  if ((symmetry=match_string(s_arg[iArg].list[1], symmetry_options,
				     N_SYMMETRY_OPTIONS, 0))<0)
	    SDDS_Bomb("unknown option used with -symmetry");
	}
	else
	  SDDS_Bomb("incorrect -symmetry syntax");
	break;
      case CLO_SIGMAS:
	if (s_arg[iArg].n_items!=3)
	  SDDS_Bomb("incorrect -sigmas syntax");
	if (sscanf(s_arg[iArg].list[1], "%lf", &sigmas)!=1)
	  SDDS_Bomb("couldn't scan value for -sigmas");
	if ((sigmasMode=match_string(s_arg[iArg].list[2], sigmas_options,
				     N_SIGMAS_OPTIONS, 0))<0)
	  SDDS_Bomb("unrecognized -sigmas mode");
	break;
      case CLO_SPARSE:
	if (s_arg[iArg].n_items!=2)
	  SDDS_Bomb("incorrect -sparse syntax");
	if (sscanf(s_arg[iArg].list[1], "%ld", &sparseInterval)!=1)
	  SDDS_Bomb("couldn't scan value for -sparse");
	if (sparseInterval<1)
	  SDDS_Bomb("invalid -sparse value");
	break;
      case CLO_VERBOSE:
	verbose = 1;
	break;
      case CLO_NORMALIZE:
	normTerm = 0;
	if (s_arg[iArg].n_items>2 ||
	    (s_arg[iArg].n_items==2 && sscanf(s_arg[iArg].list[1], "%ld", &normTerm)!=1) ||
	    normTerm<0)
	  SDDS_Bomb("invalid -normalize syntax");
	break;
      case CLO_REVISEORDERS:
	revpowThreshold = 0.1;
	revpowCompleteThres = 10;
	goodEnoughChi = 0;
	s_arg[iArg].n_items -= 1;
	if (!scanItemList(&reviseOrders, s_arg[iArg].list+1, &s_arg[iArg].n_items, 0,
			  "threshold", SDDS_DOUBLE, &revpowThreshold, 1, 0,
			  "complete", SDDS_DOUBLE, &revpowCompleteThres, 1, REVPOW_COMPLETE,
			  "goodenough", SDDS_DOUBLE, &goodEnoughChi, 1, 0,
			  "verbose", -1, NULL, 1, REVPOW_VERBOSE,
			  NULL) || 
	    revpowThreshold<0 || revpowCompleteThres<0 || goodEnoughChi<0)
	  SDDS_Bomb("invalid -reviseOrders syntax");
	reviseOrders |= REVPOW_ACTIVE;
	break;
      case CLO_CHEBYSHEV:
	if (s_arg[iArg].n_items>2 ||
	    (s_arg[iArg].n_items==2 && 
	     strncmp(s_arg[iArg].list[1], "convert", strlen(s_arg[iArg].list[1]))!=0))
	  SDDS_Bomb("invalid -chebyshev syntax");
	chebyshev = s_arg[iArg].n_items;
	basis_fn = tcheby;
	basis_dfn = dtcheby;
	break;
      case CLO_XFACTOR:
	if (s_arg[iArg].n_items!=2 ||
	    sscanf(s_arg[iArg].list[1], "%lf", &xScaleFactor)!=1 || xScaleFactor==0)
	  SDDS_Bomb("invalid -xFactor syntax");
	break;
      case CLO_COLUMNS:
	if (s_arg[iArg].n_items<3 || s_arg[iArg].n_items>5)
	  SDDS_Bomb("invalid -columns syntax");
	xName = s_arg[iArg].list[1];
	yName = s_arg[iArg].list[2];
	s_arg[iArg].n_items -= 3;
	if (!scanItemList(&flags, s_arg[iArg].list+3, &s_arg[iArg].n_items, 0,
			  "xsigma", SDDS_STRING, &xSigmaName, 1, 0,
			  "ysigma", SDDS_STRING, &ySigmaName, 1, 0,
			  NULL))
	  SDDS_Bomb("invalid -columns syntax");
	break;
      case CLO_FITLABELFORMAT:
	if (s_arg[iArg].n_items!=2)
	  SDDS_Bomb("invalid -fitLabelFormat syntax");
	fitLabelFormat = s_arg[iArg].list[1];
	break;
      case CLO_PIPE:
	if (!processPipeOption(s_arg[iArg].list+1, s_arg[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case CLO_EVALUATE:
	if (s_arg[iArg].n_items<2)
	  SDDS_Bomb("invalid -evaluate syntax");
	evalParameters.file = s_arg[iArg].list[1];
	s_arg[iArg].n_items -= 2;
	s_arg[iArg].list += 2;
	if (!scanItemList(&evalParameters.flags, s_arg[iArg].list, &s_arg[iArg].n_items, 0,
			  "begin", SDDS_DOUBLE, &evalParameters.begin, 1, EVAL_BEGIN_GIVEN,
			  "end", SDDS_DOUBLE, &evalParameters.end, 1, EVAL_END_GIVEN,
			  "number", SDDS_LONG, &evalParameters.number, 1, EVAL_NUMBER_GIVEN,
			  NULL))
	  SDDS_Bomb("invalid -evaluate syntax");
	evalParameters.initialized = 0;
	break;
      case CLO_COPY_PARAMETERS:
        copyParameters = 1;
        break;
      default:
	bomb("unknown switch", USAGE);
	break;
      }
    }
    else {
      if (input==NULL)
	input = s_arg[iArg].list[0];
      else if (output==NULL)
	output = s_arg[iArg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }
  
  processFilenames("sddspfit", &input, &output, pipeFlags, 0, NULL);

  if (symmetry && order)
    SDDS_Bomb("can't specify both -symmetry and -orders");
  if (!xName || !yName)
    SDDS_Bomb("you must specify a column name for x and y");

  if (modifySigmas && !xSigmaName)
    SDDS_Bomb("you must specify x sigmas with -modifySigmas");
  if (generateSigmas) {
    if (modifySigmas)
      SDDS_Bomb("you can't specify both -generateSigmas and -modifySigmas");
  }
  if (ySigmaName) {
    if (sigmasMode!=-1)
      SDDS_Bomb("you can't specify both -sigmas and a y sigma name");
  }
  ySigmasValid = 0;
  if (sigmasMode!=-1 || generateSigmas || ySigmaName || modifySigmas)
    ySigmasValid = 1;
    
  if (normTerm>=0 && normTerm>=terms)
    SDDS_Bomb("can't normalize to that term--not that many terms");
  if (reviseOrders && !(sigmasMode!=-1 || generateSigmas || ySigmaName))
    SDDS_Bomb("can't use -reviseOrders unless a y sigma or -generateSigmas is given");

  if (symmetry==EVEN_SYMMETRY) {
    order = tmalloc(sizeof(*order)*terms);
    for (i=0; i<terms; i++)
      order[i] = 2*i;
  }
  else if (symmetry==ODD_SYMMETRY) {
    order = tmalloc(sizeof(*order)*terms);
    for (i=0; i<terms; i++)
      order[i] = 2*i+1;
  }
  else if (!order) {
    order = tmalloc(sizeof(*order)*terms);
    for (i=0; i<terms; i++)
      order[i] = i;
  }
  coef = tmalloc(sizeof(*coef)*terms);
  coefSigma = tmalloc(sizeof(*coefSigma)*terms);
  iTerm = tmalloc(sizeof(*iTerm)*terms);

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  checkInputFile(&SDDSin, xName, yName, xSigmaName, ySigmaName); 
  coefUnits = initializeOutputFile(&SDDSout, output, &SDDSin, input, 
                                   xName, yName, xSigmaName, ySigmaName,
                                   ySigmasValid, order, terms, chebyshev, copyParameters);
   
  while (SDDS_ReadPage(&SDDSin)>0) {
    npages++;
    invalid = 0;
    if ((points = SDDS_CountRowsOfInterest(&SDDSin))<terms) {
      pointsOrig = 0;
      invalid=1;
      isFit = 0;     
    } else {
      if (!(x=SDDS_GetColumnInDoubles(&SDDSin, xName))) {
        fprintf(stderr, "error: unable to read column %s\n", xName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!(y=SDDS_GetColumnInDoubles(&SDDSin, yName))) {
        fprintf(stderr, "error: unable to read column %s\n", yName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      sx = NULL;
      if (xSigmaName && !(sx=SDDS_GetColumnInDoubles(&SDDSin, xSigmaName))) {
        fprintf(stderr, "error: unable to read column %s\n", xSigmaName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      sy0 = NULL;
      if (ySigmaName && !(sy0=SDDS_GetColumnInDoubles(&SDDSin, ySigmaName))) {
        fprintf(stderr, "error: unable to read column %s\n", ySigmaName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!sy0)
        sy0 = tmalloc(sizeof(*sy0)*points);
      
      if (xMin!=xMax || sparseInterval!=1) {
        xOrig  = tmalloc(sizeof(*xOrig)*points);
        yOrig  = tmalloc(sizeof(*yOrig)*points);
        if (sx)
          sxOrig = tmalloc(sizeof(*sxOrig)*points);
        if (ySigmasValid)
          syOrig = tmalloc(sizeof(*syOrig)*points);
        pointsOrig = points;
        for (i=j=0; i<points; i++) {
          xOrig[i]  = x[i];
          yOrig[i]  = y[i];
          if (ySigmasValid)
            syOrig[i] = sy0[i];
          if (sx)
            sxOrig[i] = sx[i];
        }
        if (xMin!=xMax) {
          for (i=j=0; i<points; i++) {
            if (xOrig[i]<=xMax && xOrig[i]>=xMin) {
              x[j] = xOrig[i];
              y[j] = yOrig[i];
              if (ySigmasValid)
                sy0[j] = syOrig[i];
              if (sx)
                sx[j] = sxOrig[i];
              j++;
            }
          }
          points = j;
        }
        if (sparseInterval!=1) {
          for (i=j=0; i<points; i++) {
            if (i%sparseInterval==0) {
              x[j] = x[i];
              y[j] = y[i];
              if (ySigmasValid)
                sy0[j] = sy0[i];
              if (sx)
                sx[j] = sx[i];
              j++;
            }
          }
          points = j;
        }
      }
      else {
        xOrig = x;
        yOrig = y;
        sxOrig = sx;
        syOrig = sy0;
        pointsOrig = points;
      }
      
      find_min_max(&xLow, &xHigh, x, points);
      
      if (sigmasMode==ABSOLUTE_SIGMAS) {
        for (i=0; i<points; i++)
          sy0[i] = sigmas;
        if (sy0!=syOrig)
          for (i=0; i<pointsOrig; i++)
            syOrig[i] = sigmas;
      }
      else if (sigmasMode==FRACTIONAL_SIGMAS) {
        for (i=0; i<points; i++)
          sy0[i] = sigmas*fabs(y[i]);
        if (sy0!=syOrig)
          for (i=0; i<pointsOrig; i++)
            syOrig[i] = fabs(yOrig[i])*sigmas;
      }
      
      if (!ySigmasValid || generateSigmas)
        for (i=0; i<points; i++)
          sy0[i] = 1;
      else
        for (i=0; i<points; i++) 
          if (sy0[i]==0)
            SDDS_Bomb("y sigma = 0 for one or more points.");
      
      diff   = tmalloc(sizeof(*x)*points);
      sy     = tmalloc(sizeof(*sy)*points);
      for (i=0; i<points; i++)
        sy[i] = sy0[i];
      
      if (autoOffset && !compute_average(&xOffset, x, points))
        xOffset = 0;
      
      set_argument_offset(xOffset);
      set_argument_scale(xScaleFactor);
      if (chebyshev) {
        xOffset = (xHigh+xLow)/2;
        set_argument_offset(xOffset);
        set_argument_scale(xScaleFactor=(xHigh-xLow)/2);
      }
      
      if (generateSigmas || modifySigmas) {
        /* do an initial fit */
        isFit = lsfg(x, y, sy, points, terms, order, coef, coefSigma, &chi, diff, basis_fn);
        if (!isFit)
          SDDS_Bomb("initial fit failed.");
        if (verbose) {
          fputs("initial_fit:", stdout);
          print_coefs(stdout, xOffset, xScaleFactor, chebyshev, coef,
                      NULL, order, terms, chi, normTerm, "");
          fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
        }
        if (modifySigmas) {
          if (!ySigmasValid) {
            for (i=0; i<points; i++) 
              sy[i] = fabs(eval_sum(basis_dfn, coef, order, terms, x[i])*sx[i]);
          }
          else 
            for (i=0; i<points; i++) {
              sy[i] = sqrt(sqr(sy0[i]) +
                           sqr(eval_sum(basis_dfn, coef, order, terms, x[i])*sx[i]) );
            }
        }
        if (generateSigmas) {
          double sigma;
          for (i=sigma=0; i<points; i++) {
            sigma += sqr(diff[i]);
          }
          sigma = sqrt(sigma/(points-terms)); 
          for (i=0; i<points; i++) {
            if (generateSigmas&FLGS_KEEPSMALLEST) {
              if (sigma<sy[i]) 
                sy[i] = sigma;
            } else if (generateSigmas&FLGS_KEEPLARGEST) {
              if (sigma>sy[i])
                sy[i] = sigma;
            } else {
              sy[i] = sigma;
            }
          }
          for (i=0; i<pointsOrig; i++) {
            if (generateSigmas&FLGS_KEEPSMALLEST) {
              if (sigma<sy0[i])
                sy0[i] = sigma;
            }
            else if (generateSigmas&FLGS_KEEPLARGEST) {
              if (sigma>sy0[i])
                sy0[i] = sigma;
            }
            else {
              sy0[i] = sigma;
            }
          }
        }
      }
      
      
      if (reviseOrders&REVPOW_ACTIVE) {
        terms = reviseFitOrders(x, y, sy, points, terms, order, coef, coefSigma, diff, basis_fn,
                                reviseOrders, xOffset, xScaleFactor, normTerm, ySigmasValid, chebyshev, 
                                revpowThreshold, revpowCompleteThres, goodEnoughChi);
        reviseOrders = 0;
      }
      
      isFit = lsfg(x, y, sy, points, terms, order, coef, coefSigma, &chi, diff, basis_fn);
      if (isFit) {
        rmsResidual = rms_average(diff, points);
        if (verbose) {
          print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                      coef, (ySigmasValid?coefSigma:NULL), order, terms, chi, normTerm, "");
          fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rmsResidual);
        }
      }
      else if (verbose)
        fprintf(stdout, "fit failed.\n");
      
      if (evalParameters.file)
        makeEvaluationTable(&evalParameters, x, points, coef, order, terms, &SDDSin, xName, yName);
    }
      
    if (!SDDS_StartPage(&SDDSout, pointsOrig)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!invalid) {
      setCoefficientData(&SDDSout, coef, (ySigmasValid?coefSigma:NULL), coefUnits, order, terms);
      for (i=0; i<points; i++)
        diff[i] = -diff[i];  /* convert from (Fit-y) to (y-Fit) to get residual */
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, x, points, ix) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, y, points, iy) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, diff, points, iResidual))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (i=0; i<points; i++)
        diff[i] = y[i] - diff[i]; /* computes fit values from residual and y */
      if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, diff, points, iFit))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      
      if (ixSigma!=-1 &&
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, sx, points, ixSigma))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (ySigmasValid && iySigma!=-1 &&
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_INDEX, sy, points, iySigma))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      
      termIndex = coefficient_index(order, terms, 0);
    }
    
    if (iIntercept!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iIntercept, invalid ? nNum: coef[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (iInterceptSigma!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iInterceptSigma, invalid ? nNum : coefSigma[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!invalid)
      termIndex = coefficient_index(order, terms, 1);
    if (iSlope!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iSlope, invalid ? nNum : coef[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (iSlopeSigma!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iSlopeSigma, invalid ? nNum : coefSigma[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!invalid)
      termIndex = coefficient_index(order, terms, 2);
    if (iCurvature!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iCurvature, invalid ? nNum : coef[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (iCurvatureSigma!=-1 &&
        !SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iCurvatureSigma, invalid ? nNum: coefSigma[termIndex], -1))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (iFitLabel!=-1 && !invalid) {
      makeFitLabel(fitLabelBuffer, SDDS_MAXLINE, fitLabelFormat, coef, order, terms);
      if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                              iFitLabel, fitLabelBuffer, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    for (i=0; i<terms; i++) {
      termIndex = -1;
      if (!invalid)
        termIndex = coefficient_index(order, terms, i);
      if (termIndex==-1) {
        if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                                iTerm[i], nNum,  -1))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      else {
        if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                                iTerm[i], coef[termIndex], -1))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    if (copyParameters && !SDDS_CopyParameters(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,
                            iRmsResidual, invalid ? nNum: rmsResidual, iChiSq, invalid ? nNum: chi,
                            iTerms, terms, 
                            iSigLevel, invalid ? nNum : ChiSqrSigLevel(chi, points-terms),
                            iOffset, invalid ? nNum: xOffset, iFactor, invalid ? nNum: xScaleFactor, 
                            iFitIsValid, isFit?'y':'n', -1) ||
        !SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!invalid) {
      free(diff);
      free(sy);
      if (xOrig!=x)
        free(xOrig);
      if (yOrig!=y)
        free(yOrig);
      if (syOrig!=sy0)
        free(syOrig);
      if (sxOrig!=sx)
        free(sxOrig);
      free(x);
      free(y);
      free(sx);
      free(sy0);
    }
  }
  if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (evalParameters.initialized && !SDDS_Terminate(&(evalParameters.dataset)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
 /* free_scanargs(&s_arg, argc);*/
  free(coef);
  free(coefSigma);
  if (coefUnits) 
    free(coefUnits);
  if (order) free(order);
  
  return(0);
}

void print_coefs(FILE *fpo, double xOffset, double xScaleFactor, long chebyshev, double *coef,
                 double *coefSigma, int32_t *order, long terms, double chi, long normTerm, char *prepend)
{
  long i;

  if (chebyshev)
    fprintf(fpo, 
	    "%s%ld-term Chebyshev T polynomial least-squares fit about x=%21.15e, scaled by %21.15e:\n",
	    prepend, terms, xOffset, xScaleFactor);
  else
        fprintf(fpo, "%s%ld-term polynomial least-squares fit about x=%21.15e:\n",
                prepend, terms, xOffset);
    if (normTerm>=0 && terms>normTerm) {
        if (coef[normTerm]!=0)
            fprintf(fpo, "%s  coefficients are normalized with factor %21.15e to make a[%ld]==1\n",
                    prepend, coef[normTerm], (order?order[normTerm]:normTerm));
        else {
            fprintf(fpo, "%s can't normalize coefficients as requested: a[%ld]==0\n",
                    prepend, (order?order[normTerm]:normTerm));
            normTerm = -1;
            }
        }
    else
        normTerm = -1;

    for (i=0; i<terms; i++) {
        fprintf(fpo, "%sa[%ld] = %21.15e ", prepend, (order?order[i]:i),
                (normTerm<0?coef[i]:coef[i]/coef[normTerm]));
        if (coefSigma)
            fprintf(fpo, "+/- %21.15e\n", (normTerm<0?coefSigma[i]:coefSigma[i]/fabs(coef[normTerm])));
        else
            fputc('\n', fpo);
        }
    if (coefSigma)
        fprintf(fpo, "%sreduced chi-squared = %21.15e\n", prepend, chi);
    }

void makeFitLabel(char *buffer, long bufsize, char *fitLabelFormat, double *coef, int32_t *order, long terms)
{
    long i;
    static char buffer1[SDDS_MAXLINE], buffer2[SDDS_MAXLINE];

    sprintf(buffer, "%s = ", ySymbol);
    for (i=0; i<terms; i++) {
        if (order[i]==0)
            sprintf(buffer1, fitLabelFormat, coef[i]);
        else {
            if (coef[i]>=0) {
                strcpy(buffer1, " +");
                sprintf(buffer1+2, fitLabelFormat, coef[i]);
                }
            else 
                sprintf(buffer1, fitLabelFormat, coef[i]);
            strcat(buffer1, "*");
            strcat(buffer1, xSymbol);
            if (order[i]>1) {
                sprintf(buffer2, "$a%d$n", order[i]);
                strcat(buffer1, buffer2);
                }
            }
        if ((long)(strlen(buffer)+strlen(buffer1))>(long)(0.95*bufsize)) {
            fprintf(stderr, "buffer overflow making fit label!\n");
            return;
            }
        strcat(buffer, buffer1);
        }
    }


double rms_average(double *x, int n)
{
    double sum2;
    int i;

    for (i=sum2=0; i<n; i++)
        sum2 += sqr(x[i]);

    return(sqrt(sum2/n));
    }

long coefficient_index(int32_t *order, long terms, long order_of_interest)
{
    long i;
    for (i=0; i<terms; i++)
        if (order[i]==order_of_interest)
            return(i);
    return(-1);
    }

void checkInputFile(SDDS_DATASET *SDDSin, char *xName, char *yName, char *xSigmaName, char *ySigmaName)
{
  char *ptr=NULL;
  if (!(ptr=SDDS_FindColumn(SDDSin, FIND_NUMERIC_TYPE, xName, NULL)))
    SDDS_Bomb("x column doesn't exist or is nonnumeric");
  free(ptr);
  ptr = NULL;
  if (!(ptr=SDDS_FindColumn(SDDSin, FIND_NUMERIC_TYPE, yName, NULL)))
    SDDS_Bomb("y column doesn't exist or is nonnumeric");
  free(ptr);
  ptr = NULL;
  if (xSigmaName && !(ptr=SDDS_FindColumn(SDDSin, FIND_NUMERIC_TYPE, xSigmaName, NULL)))
    SDDS_Bomb("x sigma column doesn't exist or is nonnumeric");
  if (ptr)
    free(ptr);
  ptr = NULL;
  if (ySigmaName && !(ptr=SDDS_FindColumn(SDDSin, FIND_NUMERIC_TYPE, ySigmaName, NULL)))
    SDDS_Bomb("y sigma column doesn't exist or is nonnumeric");
  if (ptr)
    free(ptr);
  ptr = NULL;
}

char **initializeOutputFile(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin, char *input,
                            char *xName, char *yName, char *xSigmaName, char *ySigmaName,
                            long sigmasValid, int32_t *order, long terms, long isChebyshev,
                            long copyParameters)
{
  char buffer[SDDS_MAXLINE], buffer1[SDDS_MAXLINE], *xUnits, *yUnits, **coefUnits;
  long i;

  if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddspfit output", output) ||
      !SDDS_TransferColumnDefinition(SDDSout, SDDSin, xName, NULL) ||
      !SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName, NULL) ||
      SDDS_GetColumnInformation(SDDSout, "symbol", &xSymbol, SDDS_GET_BY_NAME, xName)
      !=SDDS_STRING ||
      SDDS_GetColumnInformation(SDDSout, "symbol", &ySymbol, SDDS_GET_BY_NAME, yName)
      !=SDDS_STRING ||
      (xSigmaName && !SDDS_TransferColumnDefinition(SDDSout, SDDSin, xSigmaName, NULL)) || 
      (ySigmaName && !SDDS_TransferColumnDefinition(SDDSout, SDDSin, ySigmaName, NULL)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!xSymbol || SDDS_StringIsBlank(xSymbol))
    xSymbol = xName;
  if (!ySymbol || SDDS_StringIsBlank(ySymbol))
    ySymbol = yName;
  ix = SDDS_GetColumnIndex(SDDSout, xName);
  iy = SDDS_GetColumnIndex(SDDSout, yName);
  if (ySigmaName)
    iySigma = SDDS_GetColumnIndex(SDDSout, ySigmaName);
  if (xSigmaName)
    ixSigma = SDDS_GetColumnIndex(SDDSout, xSigmaName);
  if (SDDS_NumberOfErrors()) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  sprintf(buffer, "%sFit", yName);
  sprintf(buffer1, "Fit[%s]", ySymbol);
  if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName, buffer) ||
      !SDDS_ChangeColumnInformation(SDDSout, "symbol", buffer1, SDDS_SET_BY_NAME, buffer))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if ((iFit = SDDS_GetColumnIndex(SDDSout, buffer))<0)
    SDDS_Bomb("unable to get index of just-defined fit output column");

  sprintf(buffer, "%sResidual", yName);
  sprintf(buffer1, "Residual[%s]", ySymbol);
  if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName, buffer) ||
      !SDDS_ChangeColumnInformation(SDDSout, "symbol", buffer1, SDDS_SET_BY_NAME, buffer))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!(iResidual = SDDS_GetColumnIndex(SDDSout, buffer)))
    SDDS_Bomb("unable to get index of just-defined residual output column");

  if (sigmasValid && !ySigmaName) {
    sprintf(buffer, "%sSigma", yName);
    if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, yName, buffer))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    iySigma = SDDS_GetColumnIndex(SDDSout, buffer);
    if (ySymbol && !SDDS_StringIsBlank(ySymbol)) {
      sprintf(buffer1, "Sigma[%s]", ySymbol);
      if (!SDDS_ChangeColumnInformation(SDDSout, "symbol", buffer1, SDDS_SET_BY_NAME, buffer))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }

  if (!(coefUnits=makeCoefficientUnits(SDDSout, xName, yName, order, terms)))
    SDDS_Bomb("unable to make coefficient units");

  if (SDDS_DefineArray(SDDSout, "Order", NULL, NULL, "Order of term in fit", NULL, 
                       SDDS_LONG, 0, 1, "FitResults")<0 ||
      SDDS_DefineArray(SDDSout, "Coefficient", "a", "[CoefficientUnits]", 
                       "Coefficient of term in fit", NULL,
                       SDDS_DOUBLE, 0, 1, "FitResults")<0 ||
      (sigmasValid && SDDS_DefineArray(SDDSout, "CoefficientSigma", "$gs$r$ba$n", "[CoefficientUnits]",
                                       "sigma of coefficient of term in fit", NULL, SDDS_DOUBLE, 
                                       0, 1, "FitResults")<0) ||
      SDDS_DefineArray(SDDSout, "CoefficientUnits", NULL, NULL, NULL,
                       NULL, SDDS_STRING, 0, 1, "FitResults")<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (SDDS_DefineParameter(SDDSout, "Basis", NULL, NULL, "Function basis for fit", 
                           NULL, SDDS_STRING, 
                           isChebyshev?"Chebyshev T polynomials":"ordinary polynomials")<0 ||
      (iChiSq=SDDS_DefineParameter(SDDSout, "ReducedChiSquared", "$gh$r$a2$n/(N-M)", NULL, 
                                   "Reduced chi-squared of fit",
                                   NULL, SDDS_DOUBLE, NULL))<0 ||
      SDDS_GetColumnInformation(SDDSout, "units", &yUnits, SDDS_GET_BY_NAME, yName)
      !=SDDS_STRING ||
      (iRmsResidual=
       SDDS_DefineParameter(SDDSout, "RmsResidual", "$gs$r$bres$n", yUnits, "RMS residual of fit",
                            NULL, SDDS_DOUBLE, NULL))<0 ||
      (iSigLevel=
       SDDS_DefineParameter(SDDSout, "SignificanceLevel", NULL, NULL, 
                            "Probability that data is from fit function", NULL, SDDS_DOUBLE, NULL))<0 )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (yUnits)
    free(yUnits);

  if (SDDS_GetColumnInformation(SDDSout, "units", &xUnits, SDDS_GET_BY_NAME, xName)
      !=SDDS_STRING)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(buffer, "%sOffset", xName);
  sprintf(buffer1, "Offset of %s for fit", xName);
  if ((iOffset=SDDS_DefineParameter(SDDSout, buffer, NULL, xUnits, 
                                    buffer1, NULL, SDDS_DOUBLE, NULL))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(buffer, "%sScale", xName);
  sprintf(buffer1, "Scale factor of %s for fit", xName);
  if ((iFactor=SDDS_DefineParameter(SDDSout, buffer, NULL, xUnits, 
                                    buffer1, NULL, SDDS_DOUBLE, NULL))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if ((iFitIsValid=SDDS_DefineParameter(SDDSout, "FitIsValid", NULL, NULL, NULL, NULL,
                                        SDDS_CHARACTER, NULL))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if ((iTerms=SDDS_DefineParameter(SDDSout, "Terms", NULL, NULL, "Number of terms in fit", NULL,
                                   SDDS_LONG, NULL))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!isChebyshev) {
    iFitLabel = SDDS_DefineParameter(SDDSout, "sddspfitLabel", NULL, NULL, NULL, NULL,
                                     SDDS_STRING, NULL);
    if ((i=coefficient_index(order, terms, 0))>=0) {
      iIntercept=
        SDDS_DefineParameter(SDDSout, "Intercept", "Intercept", coefUnits[i], "Intercept of fit", 
                             NULL, SDDS_DOUBLE, NULL);
      if (sigmasValid)
        iInterceptSigma = 
          SDDS_DefineParameter(SDDSout, "InterceptSigma", "InterceptSigma", coefUnits[i],
                               "Sigma of intercept of fit", NULL,
                               SDDS_DOUBLE, NULL);
    }
    if ((i=coefficient_index(order, terms, 1))>=0) {
      iSlope = SDDS_DefineParameter(SDDSout, "Slope", "Slope", coefUnits[i], "Slope of fit", NULL,
                                    SDDS_DOUBLE, NULL);
      if (sigmasValid)
        iSlopeSigma = 
          SDDS_DefineParameter(SDDSout, "SlopeSigma", "SlopeSigma", coefUnits[i],
                               "Sigma of slope of fit", NULL,
                               SDDS_DOUBLE, NULL);
    }
    if ((i=coefficient_index(order, terms, 2))>=0) {
      iCurvature = 
        SDDS_DefineParameter(SDDSout, "Curvature", "Curvature", coefUnits[i], "Curvature of fit", 
                             NULL, SDDS_DOUBLE, NULL);
      if (sigmasValid)
        iCurvatureSigma = 
          SDDS_DefineParameter(SDDSout, "CurvatureSigma", "CurvatureSigma", coefUnits[i],
                               "Sigma of curvature of fit", NULL,
                               SDDS_DOUBLE, NULL);
    }
    for (i=0; i<terms; i++) {
      char s[100];
      sprintf(s, "Coefficient%02ld", order[i]);
      iTerm[i] = SDDS_DefineParameter(SDDSout, s, s, coefUnits[i], NULL, NULL, SDDS_DOUBLE, NULL);
    }
    
    if (SDDS_NumberOfErrors()) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }

  if (copyParameters && !SDDS_TransferAllParameterDefinitions(SDDSout, SDDSin, 0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (!SDDS_WriteLayout(SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
  return coefUnits;
}


long setCoefficientData(SDDS_DATASET *SDDSout, double *coef, double *coefSigma, char **coefUnits,
                        int32_t *order, long terms)
{

    if (!SDDS_SetArrayVararg(SDDSout, "Order", SDDS_POINTER_ARRAY, order, terms) ||
        !SDDS_SetArrayVararg(SDDSout, "Coefficient", SDDS_POINTER_ARRAY, coef, terms) ||
        (coefSigma && 
         !SDDS_SetArrayVararg(SDDSout, "CoefficientSigma", SDDS_POINTER_ARRAY, coefSigma, terms)) ||
        !SDDS_SetArrayVararg(SDDSout, "CoefficientUnits", SDDS_POINTER_ARRAY, coefUnits, terms) )
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return 1;
    }

char **makeCoefficientUnits(SDDS_DATASET *SDDSout, char *xName, char *yName, 
                            int32_t *order, long terms)
{
    char *xUnits, *yUnits, buffer[SDDS_MAXLINE];
    char **coefUnits = NULL;
    long i;

    if (!SDDS_GetColumnInformation(SDDSout, "units", &xUnits, SDDS_GET_BY_NAME, xName) ||
        !SDDS_GetColumnInformation(SDDSout, "units", &yUnits, SDDS_GET_BY_NAME, yName) )
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    coefUnits = tmalloc(sizeof(*coefUnits)*terms);
    if (!xUnits || SDDS_StringIsBlank(xUnits)) {
        if (!yUnits || SDDS_StringIsBlank(yUnits))
            SDDS_CopyString(&yUnits, "");
        for (i=0; i<terms; i++) 
            coefUnits[i] = yUnits;
        }
    else {
        if (!yUnits || SDDS_StringIsBlank(yUnits))
            SDDS_CopyString(&yUnits, "1");
        for (i=0; i<terms; i++) {
            if (order[i]==0) {
                if (strcmp(yUnits, "1")!=0)
                    SDDS_CopyString(coefUnits+i, yUnits);
                else
                    SDDS_CopyString(coefUnits+i, "");
                }
            else if (strcmp(xUnits, yUnits)==0) {
                if (order[i]>1)
                    sprintf(buffer, "1/%s$a%d$n", xUnits, order[i]-1);
                else
                    sprintf(buffer, "");
                SDDS_CopyString(coefUnits+i, buffer);
                }
            else {
                if (order[i]>1)
                    sprintf(buffer, "%s/%s$a%d$n", yUnits, xUnits, order[i]);
                else
                    sprintf(buffer, "%s/%s", yUnits, xUnits);
                SDDS_CopyString(coefUnits+i, buffer);
                }
            }
        }
    return coefUnits;
    }

void makeEvaluationTable(EVAL_PARAMETERS *evalParameters, double *x, long points,
                         double *coef, int32_t *order, long terms, 
                         SDDS_DATASET *SDDSin, char *xName, char *yName)
{
  double *xEval, *yEval, delta;
  long i;
  yEval = NULL;
  if (!evalParameters->initialized) {
    if (!SDDS_InitializeOutput(&evalParameters->dataset, SDDS_BINARY, 0, NULL, "sddspfit evaluation table",
                               evalParameters->file) ||
        !SDDS_TransferColumnDefinition(&evalParameters->dataset, SDDSin, xName, NULL) ||
        !SDDS_TransferColumnDefinition(&evalParameters->dataset, SDDSin, yName, NULL) ||
        !SDDS_WriteLayout(&evalParameters->dataset))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    evalParameters->initialized = 1;
  }
    
  if (!(evalParameters->flags&EVAL_BEGIN_GIVEN) || !(evalParameters->flags&EVAL_END_GIVEN)) {
    double min, max;
    find_min_max(&min, &max, x, points);
    if (!(evalParameters->flags&EVAL_BEGIN_GIVEN))
      evalParameters->begin = min;
    if (!(evalParameters->flags&EVAL_END_GIVEN))
      evalParameters->end = max;
  }
  if (!(evalParameters->flags&EVAL_NUMBER_GIVEN))
    evalParameters->number = points;
  if (evalParameters->number>1)
    delta = (evalParameters->end - evalParameters->begin)/(evalParameters->number-1);
  else
    delta = 0;
    
  if (!(xEval = (double*)malloc(sizeof(*xEval)*evalParameters->number)) ||
      !(yEval = (double*)malloc(sizeof(*yEval)*evalParameters->number)))
    SDDS_Bomb("allocation failure");

  for (i=0; i<evalParameters->number; i++) {
    xEval[i] = evalParameters->begin + i*delta;
    yEval[i] = eval_sum(basis_fn, coef, order, terms, xEval[i]);
  }

  if (!SDDS_StartPage(&evalParameters->dataset, evalParameters->number) ||
      !SDDS_SetColumnFromDoubles(&evalParameters->dataset, SDDS_SET_BY_NAME,
                                 xEval, evalParameters->number, xName) ||
      !SDDS_SetColumnFromDoubles(&evalParameters->dataset, SDDS_SET_BY_NAME,
                                 yEval, evalParameters->number, yName) ||
      !SDDS_WritePage(&evalParameters->dataset))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free(xEval);
  free(yEval);
}


long reviseFitOrders(double *x, double *y, double *sy, long points,
                     long terms, int32_t *order, double *coef, double *coefSigma,
                     double *diff, double (*basis_fn)(double xa, long ordera),
                     unsigned long reviseOrders,
                     double xOffset, double xScaleFactor, long normTerm, long ySigmasValid,
                     long chebyshev, double revpowThreshold, double revpowCompleteThreshold,
                     double goodEnoughChi)
{
  double bestChi, chi;
  long bestTerms, newTerms, newBest, *termUsed;
  int32_t *newOrder, *bestOrder;
  long i, ip, j;
  long origTerms, *origOrder;
  
  bestOrder = tmalloc(sizeof(*bestOrder)*terms);
  newOrder = tmalloc(sizeof(*newOrder)*terms);
  termUsed = tmalloc(sizeof(*termUsed)*terms);
  origOrder = tmalloc(sizeof(*origOrder)*terms);
  origTerms = terms;
  for (i=0; i<terms; i++)
    origOrder[i] = order[i];
  qsort((void*)order, terms, sizeof(*order), long_cmpasc);
  bestOrder[0] = newOrder[0] = order[0];
  termUsed[0] = 1;
  newTerms = bestTerms = 1;
  /* do a fit */
  if (!lsfg(x, y, sy, points, newTerms, newOrder, coef, coefSigma, &bestChi, diff, basis_fn))
    SDDS_Bomb("revise-orders fit failed.");
  if (reviseOrders&REVPOW_VERBOSE) {
    fputs("fit to revise orders:", stdout);
    print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                coef, (ySigmasValid?coefSigma:NULL), bestOrder, bestTerms, bestChi, normTerm, "");
    fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
  }
  
  do {
    newBest = 0;
    newTerms = newTerms+1;
    for (ip=1; ip<terms; ip++) {
      if (termUsed[ip]) continue;
      newOrder[newTerms-1] = order[ip];
      if (!lsfg(x, y, sy, points, newTerms, newOrder, coef, coefSigma, &chi, diff, basis_fn))
        SDDS_Bomb("revise-orders fit failed.");
      if (chi<=bestChi-revpowThreshold) {
        bestChi = chi;
        bestTerms = newTerms;
        newBest = 1;
        termUsed[ip] = 1;
        for (i=0; i<newTerms; i++)
          bestOrder[i] = newOrder[i];
        if (reviseOrders&REVPOW_VERBOSE) {
          fputs("new best fit:", stdout);
          print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                      coef, (ySigmasValid?coefSigma:NULL), bestOrder, bestTerms, bestChi, normTerm, "");
          fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
        }
        break;
      }
    }
  } while (newBest && bestChi>goodEnoughChi);

  terms = bestTerms;
  for (ip=0; ip<terms; ip++)
    order[ip] = bestOrder[ip];

  if (newBest && bestChi>goodEnoughChi) {
    do {
      newBest = 0;
      for (ip=0; ip<terms; ip++) {
        for (i=j=0; i<terms; i++) {
          if (i!=ip)
            newOrder[j++] = order[i];
        }
        newTerms = terms-1;
        if (!lsfg(x, y, sy, points, newTerms, newOrder, coef, coefSigma, &chi, diff, basis_fn))
          SDDS_Bomb("revise-orders fit failed.");
        if (chi<bestChi+revpowThreshold) {
          bestChi = chi;
          terms = newTerms;
          newBest = 1;
          for (i=0; i<newTerms; i++)
            order[i] = newOrder[i];
          if (reviseOrders&REVPOW_VERBOSE) {
            fputs("new best fit:", stdout);
            print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                        coef, (ySigmasValid?coefSigma:NULL), order, terms, bestChi, normTerm, "");
            fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
          }
          break;
        }
      }
    } while (newBest && terms>1 && bestChi>goodEnoughChi);  
  }
  
  free(bestOrder);
  free(termUsed);
  free(newOrder);

  if (reviseOrders&REVPOW_COMPLETE && bestChi>revpowCompleteThreshold) {
    terms = origTerms;
    for (i=0; i<origTerms; i++)
      order[i] = origOrder[i];
    free(origOrder);
    if (reviseOrders&REVPOW_VERBOSE)
      fprintf(stdout, "Result unsatisfactory---attempting complete trials\n");
    return reviseFitOrders1(x, y, sy, points, terms, order, coef, coefSigma,
                            diff, basis_fn, reviseOrders, xOffset, xScaleFactor,
                            normTerm, ySigmasValid, chebyshev, revpowThreshold,
                            goodEnoughChi);
  }

  free(origOrder);
  return terms;
}

long reviseFitOrders1(double *x, double *y, double *sy, long points,
                     long terms, int32_t *order, double *coef, double *coefSigma,
                     double *diff, double (*basis_fn)(double xa, long ordera),
                     unsigned long reviseOrders,
                     double xOffset, double xScaleFactor, long normTerm, long ySigmasValid,
                     long chebyshev, double revpowThreshold, double goodEnoughChi)
{
  double bestChi, chi;
  long bestTerms, newTerms, newBest;
  int32_t *newOrder=NULL, *bestOrder;
  long i, ip, j, bestFound;
  long *counter=NULL, *counterLim=NULL;
  
  if (!(bestOrder = malloc(sizeof(*bestOrder)*terms)) ||
      !(newOrder = malloc(sizeof(*newOrder)*terms)) ||
      !(counter = calloc(sizeof(*counter), terms)) ||
      !(counterLim = calloc(sizeof(*counterLim), terms))) {
    fprintf(stderr, "Error: memory allocation failure (%ld terms)\n", terms);
    SDDS_Bomb(NULL);
  }
  for (i=0; i<terms; i++)
    counterLim[i] = 2;
  qsort((void*)order, terms, sizeof(*order), long_cmpasc);
  /* do a fit */
  if (!lsfg(x, y, sy, points, 1, order, coef, coefSigma, &bestChi, diff, basis_fn))
    SDDS_Bomb("revise-orders fit failed.");
  bestOrder[0] = order[0];
  bestTerms = 1;
  if (reviseOrders&REVPOW_VERBOSE) {
    fputs("starting fit to revise orders:", stdout);
    print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                coef, (ySigmasValid?coefSigma:NULL), order, 1, bestChi, normTerm, "");
    fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
  }
  
  bestFound = 0;
  newTerms = 1;
  while (advance_counter(counter, counterLim, terms)>=0 && bestChi>goodEnoughChi) {  
    for (i=j=0; i<terms; i++) {
      if (counter[i])
        newOrder[j++] = order[i];
    }
    newTerms = j;
    if (!lsfg(x, y, sy, points, newTerms, newOrder, coef, coefSigma, &chi, diff, basis_fn))
      SDDS_Bomb("revise-orders fit failed.");
    if (chi<=bestChi-revpowThreshold) {
      bestChi = chi;
      bestTerms = newTerms;
      newBest = bestFound = 1;
      for (i=0; i<newTerms; i++)
        bestOrder[i] = newOrder[i];
      if (reviseOrders&REVPOW_VERBOSE) {
        fputs("new best fit:", stdout);
        print_coefs(stdout, xOffset, xScaleFactor, chebyshev,
                    coef, (ySigmasValid?coefSigma:NULL), bestOrder, bestTerms, bestChi, normTerm, "");
        fprintf(stdout, "unweighted rms deviation from fit: %21.15e\n", rms_average(diff, points));
      }
    }
  }
  
  terms = bestTerms;
  for (ip=0; ip<terms; ip++)
    order[ip] = bestOrder[ip];
   
  free(bestOrder);
  free(newOrder);
  free(counter);
  free(counterLim);
  return terms;
}
