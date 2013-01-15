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
 Revision 1.21  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.20  2003/09/02 19:16:02  soliday
 Cleaned up code for Linux.

 Revision 1.19  2002/08/14 17:12:36  soliday
 Added Open License

 Revision 1.18  2002/07/01 18:35:08  borland
 Added option to reverse order of rows in file.  Added 't' column.

 Revision 1.17  2001/11/21 16:30:29  borland
 No longer emits NaN values when the number of particles in a slice is zero.

 Revision 1.16  2001/08/08 19:20:22  borland
 Added ability to output the filtered beam data using the -removePTails option.

 Revision 1.15  2001/08/08 00:58:48  borland
 Added -removePTails option, which permits removing momentum tails from
 the beam.

 Revision 1.14  2001/07/11 15:37:51  borland
 Added -pipe option.

 Revision 1.13  2001/05/03 20:53:53  soliday
 Fixed a problem related to the last version that was merged with a previous
 version.

 Revision 1.12  2001/04/03 18:23:16  borland
 Added -chargeParameter option, which permits taking the beam charge from
 a parameter in the SDDS input file.

 Revision 1.11  2001/01/23 19:14:55  soliday
 Standardized usage message.

 Revision 1.10  2000/06/02 20:01:01  soliday
 To be consistant with GENESIS, where dgamma is the rms value of gamma,
 dgamma is calculated the same way it was before revision 1.5.

 Revision 1.9  2000/06/01 21:42:55  borland
 SDDS output is now the default.  Added -steer option.  Improved usage message.

 Revision 1.8  2000/05/31 20:31:17  soliday
 Fixed small bug from last change.

 Revision 1.7  2000/05/31 20:20:14  soliday
 Added alphax and alphay in ascii output.

 Revision 1.6  2000/05/31 19:28:19  borland
 Added gammaStDev column.

 Revision 1.5  2000/05/31 17:48:48  borland
 Fixed bug in computation of delta gamma (was computing delta s).
 Simplified computation of alphax and alphay, and used geometric emittance
 in place of normalized emittance.

 Revision 1.4  2000/05/31 15:20:55  soliday
 Added correct computation for alphax and alphay.

 Revision 1.3  2000/01/06 22:18:15  soliday
 Multiplied the previous value of Emittance by gamma to get the correct emittance.
 Divided the previous value of Current by the wavelength to get the correct value.

 Revision 1.2  2000/01/06 21:29:19  soliday
 Fixed a bug when calculating the emittance

 Revision 1.1  2000/01/06 18:02:25  soliday
 This program is used to convert the output file from elegant into the beamline
 file used by genesis.

 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_WAVELENGTH 0
#define SET_SLICES 1
#define SET_TOTALCHARGE 2
#define SET_TEXTOUTPUT 3
#define SET_STEER 4
#define SET_CHARGEPARAMETER 5
#define SET_PIPE 6
#define SET_REMPTAILS 7
#define SET_REVERSEORDER 8
#define SET_LOCAL_FIT 9
#define N_OPTIONS 10

char *option[N_OPTIONS] = {"wavelength", "slices", "totalcharge", "textoutput", "steer",
                             "chargeparameter", "pipe", "removePTails", 
                             "reverseorder", "localfit"};

char *USAGE = "elegant2genesis [<input>] [<output>] [-pipe=[in][,out]] [-textOutput]\n\
[-totalCharge=<charge-in-Coulombs> | -chargeParameter=<name>]\n\
[-wavelength=<meters> | -slices=<integer>]\n\
[-steer] [-removePTails=deltaLimit=<value>[,fit][,beamOutput=<filename>]]\n\
[-reverseOrder] [-localFit]\n\n\
textOutput       Make the output file a text file instead of an SDDS file.\n\
totalCharge      Specify the total charge in Coulombs.\n\
chargeParameter  Specify the name of a parameter in the input file that gives\n\
                 the total charge in Coulombs.\n\
wavelength       Specify the wavelength of light in meters.\n\
slices           Specify the number of slices to cut the beam into.\n\
steer            Force the x, x', y, and y' centroids for the whole beam to zero.\n\
                 Slices may still have nonzero centroids.\n\
removePTail      Removes the momentum tails from the beam.  deltaLimit is the maximum\n\
                 |p-<p>|/<p> that will be kept.  If 'fit' is given, then a linear fit to\n\
                 (t, p) is done and removal is based on residuals from this fit.\n\
                 If 'beamOutput' is given, the filtered beam is written to the named\n\
                 file for review.\n\
reverseOrder     By default, the data for the head of the beam comes first.  This\n\
                 option causes elegant to put the data for the tail of the beam \n\
                 first.\n\
localFit         If given, then for each slice the program performs a local linear\n\
                 fit of momentum vs time, and subtracts it from the momentum data.\n\
                 This removes a contribution to the energy spread, but may not be\n\
                 appropriate if slices are longer than the cooperation length.\n\n\
Program by Robert Soliday. This is version 2, August 2008, M. Borland. ("__DATE__")\n";

#define PTAILS_DELTALIMIT 0x0001U
#define PTAILS_FITMODE    0x0002U
#define PTAILS_OUTPUT     0X0004U
long removeMomentumTails(double *x, double *xp, double *y,
                         double *yp, double *s, double *p, double *t,
                         long rows, double limit, unsigned long flags);
void removeLocalFit(double *p, double *s, short *selected, double pAverage, long rows);

/* ********** */

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output, SDDS_pTails;
  FILE *fileID=NULL;
  SCANNED_ARG *s_arg;
  long i, j, i_arg;
  char *input, *output;
  unsigned long pipeFlags=0;
  long noWarnings=0, tmpfile_used=0;
  long retval, origRows, rows, sddsOutput=1, steer=0, firstOne, sliceOffset=0;
  long reverseOrder=0;
  char *chargeParameter;
  unsigned long removePTailsFlags = 0;
  double pTailsDeltaLimit, term;
  char *pTailsOutputFile = NULL;
  long localFit = 0;
  short *selected;
  
  char column[7][15] = {"x", "xp", "y", "yp", "t", "p", "particleID"};
  long columnIndex[7];
  
  double *tValues, *sValues, *xValues, *xpValues, *yValues, *ypValues, *pValues;
  double sAverage, pAverage, pStDev;
  double xAverage, xpAverage, xRMS, x2Average, xp2Average, xxpAverage, xEmittance;
  double yAverage, ypAverage, yRMS, y2Average, yp2Average, yypAverage, yEmittance;
  double sMin=1, sMax=-1, s1, s2;
  /*
    double pMin, pMax;
  */
  double alphax, alphay;

  double wavelength=0.0001;
  double totalCharge=0;
  double current;
  long slices=4;
  long tmp=0;
  long N=0;  

  input = output = NULL;
  sValues = NULL;
  chargeParameter = NULL;
  selected = NULL;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  /* parse the command line */
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_TOTALCHARGE:
	if (s_arg[i_arg].n_items!=2) {
	  fprintf(stderr, "error: invalid -totalCharge syntax\n");
	  exit(1);
	}
	if (sscanf(s_arg[i_arg].list[1], "%lf", &totalCharge) != 1) {
	  fprintf(stderr, "error: invalid -totalCharge syntax or value\n");
	  exit(1);
	}
	break;
      case SET_WAVELENGTH:
	if (tmp == 2) {
	  fprintf(stderr, "error: -wavelength and -slices can not be used together\n");
	  exit(1);
	}
	if (s_arg[i_arg].n_items!=2) {
	  fprintf(stderr, "error: invalid -wavelength syntax\n");
	  exit(1);
	}
	if (sscanf(s_arg[i_arg].list[1], "%lf", &wavelength) != 1 || wavelength <= 0) {
	  fprintf(stderr, "error: invalid -wavelength syntax or value\n");
	  exit(1);
	}
	tmp = 1;
	break;
      case SET_SLICES:
	if (tmp == 1) {
	  fprintf(stderr, "error: -wavelength and -slices can not be used together\n");
	  exit(1);
	}
	if (s_arg[i_arg].n_items!=2) {
	  fprintf(stderr, "error: invalid -slices syntax\n");
	  exit(1);
	}
	if (sscanf(s_arg[i_arg].list[1], "%ld", &slices) != 1 || slices <= 0) {
	  fprintf(stderr, "error: invalid -slices syntax or value\n");
	  exit(1);
	}
	tmp = 2;
	break;
      case SET_TEXTOUTPUT:
	sddsOutput = 0;
	break;
      case SET_STEER:
        steer = 1;
        break;
      case SET_CHARGEPARAMETER:
        if (s_arg[i_arg].n_items!=2) {
          fprintf(stderr, "error: invalid -chargeParameter syntax\n");
          exit(1);
        }
        chargeParameter = s_arg[i_arg].list[1];
        break;
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_REMPTAILS:
        if (s_arg[i_arg].n_items -= 1)  {
          if (!scanItemList(&removePTailsFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                            "deltalimit", SDDS_DOUBLE, &pTailsDeltaLimit, 1, PTAILS_DELTALIMIT,
                            "fit", -1, NULL, 0, PTAILS_FITMODE,
                            "beamoutput", SDDS_STRING, &pTailsOutputFile, 1, PTAILS_OUTPUT,
                            NULL) ||
              !(removePTailsFlags&PTAILS_DELTALIMIT) ||
              pTailsDeltaLimit<=0)
            SDDS_Bomb("invalid -removePTails syntax or values");
        }
        break;
      case SET_REVERSEORDER:
	reverseOrder = 1;
	break;
      case SET_LOCAL_FIT:
        localFit = 1;
        break;
      default:
	fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
	exit(1);
      }
    } else {
      if (input == NULL) {
	input = s_arg[i_arg].list[0];
      } else if (output == NULL) {
	output = s_arg[i_arg].list[0];
      } else {
	fprintf(stderr, "too many filenames\n");
	exit(1);
      }
    }
  }

  processFilenames("elegant2genesis", &input, &output, pipeFlags, noWarnings, &tmpfile_used);
    
  /* open the input file */
  if (!SDDS_InitializeInput(&SDDS_input, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  /* check for needed columns */
  for (i=0;i<7;i++) {
    if ((columnIndex[i] = SDDS_GetColumnIndex(&SDDS_input, column[i])) < 0) {
      fprintf(stderr, "error: column %s does not exist\n", column[i]);
      exit(1);
    }
  }

  if (removePTailsFlags&PTAILS_OUTPUT) {
    if (!SDDS_InitializeOutput(&SDDS_pTails, SDDS_BINARY, 0, NULL, NULL, pTailsOutputFile) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "t", "s", SDDS_DOUBLE) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "p", NULL, SDDS_DOUBLE) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "x", "m", SDDS_DOUBLE) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "xp", NULL, SDDS_DOUBLE) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "y", "m", SDDS_DOUBLE) ||
        !SDDS_DefineSimpleColumn(&SDDS_pTails, "yp", NULL, SDDS_DOUBLE) ||
        !SDDS_WriteLayout(&SDDS_pTails)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  
  retval = -1;
  firstOne = 1;
  while ((retval=SDDS_ReadPage(&SDDS_input))>=1) {
    rows = origRows = SDDS_RowCount(&SDDS_input);
    if (rows < 1) {
      fprintf(stderr, "error: no rows found for page %ld\n", retval);
      exit(1);
    }
    /* get all the needed column data */
    if (!(tValues = SDDS_GetNumericColumn(&SDDS_input, "t", SDDS_DOUBLE)) ||
        !(xValues = SDDS_GetNumericColumn(&SDDS_input, "x", SDDS_DOUBLE)) ||
        !(xpValues = SDDS_GetNumericColumn(&SDDS_input, "xp", SDDS_DOUBLE)) ||
        !(yValues = SDDS_GetNumericColumn(&SDDS_input, "y", SDDS_DOUBLE)) ||
        !(ypValues = SDDS_GetNumericColumn(&SDDS_input, "yp", SDDS_DOUBLE)) ||
        !(pValues = SDDS_GetNumericColumn(&SDDS_input, "p", SDDS_DOUBLE))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    if (chargeParameter &&
        !SDDS_GetParameterAsDouble(&SDDS_input, chargeParameter, &totalCharge))
      SDDS_Bomb("unable to read charge from file");
    if ((!(tValues)) || (!(xValues)) || (!(xpValues)) || (!(yValues)) || (!(ypValues)) || (!(pValues))) {
      fprintf(stderr, "error: invalid data for page %ld\n", retval);
      exit(1);
    }
    selected = realloc(selected, sizeof(*selected)*rows);
    sValues = realloc(sValues, sizeof(*sValues)*rows);
    sAverage = xAverage = xpAverage = yAverage = ypAverage = 0;
    for (i=0;i<rows;i++) { /* calculate s values and sAverage */
      sValues[i] = tValues[i]*2.99792458e8;
      sAverage += tValues[i];
      xAverage += xValues[i];
      xpAverage += xpValues[i];
      yAverage += yValues[i];
      ypAverage += ypValues[i];
    }
    sAverage = sAverage * 2.99792458e8 / rows;
    xAverage /= rows;
    xpAverage /= rows;
    yAverage /= rows;
    ypAverage /= rows;

    for (i=0;i<rows;i++)
      /* move origin of s values to center at sAverage and flip the sign */
      sValues[i] = sAverage - sValues[i];

    /* remove the momentum tails, if asked */
    if (removePTailsFlags) {
      if (!(rows = removeMomentumTails(xValues, xpValues, yValues, ypValues, sValues, pValues,
                                       tValues, rows, pTailsDeltaLimit, removePTailsFlags)))
        SDDS_Bomb("all data removed by P-tails filter");
      if (removePTailsFlags&PTAILS_OUTPUT) {
        if (!SDDS_StartPage(&SDDS_pTails, rows) ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, xValues, rows, "x") ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, xpValues, rows, "xp") ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, yValues, rows, "y") ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, ypValues, rows, "yp") ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, tValues, rows, "t") ||
            !SDDS_SetColumn(&SDDS_pTails, SDDS_SET_BY_NAME, pValues, rows, "p") ||
            !SDDS_WritePage(&SDDS_pTails)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
    }

    if (steer) {
      /* set the centroids for the whole beam to zero */
      for (i=0; i<rows; i++) {
        xValues[i] -= xAverage;
        xpValues[i] -= xpAverage;
        yValues[i] -= yAverage;
        ypValues[i] -= ypAverage;
      }
    }
    
    /* find limits of distribution in s */
    find_min_max(&sMin, &sMax, sValues, rows);
    if ((tmp == 1) || (tmp == 0)) { /* calculate the # of slices from the wavelength */
      slices = (sMax - sMin) / wavelength + 1;
    }
    if (tmp == 2) { /* calculate the wavelength from the # of slices */
      wavelength = (sMax - sMin) / slices;
    }

    if (firstOne) {
      /* open the output file */
      if (sddsOutput) {
        if (SDDS_InitializeOutput(&SDDS_output, SDDS_ASCII, 1, NULL, NULL, output) != 1) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if ((SDDS_DefineColumn(&SDDS_output, "s", "Location", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
	    (SDDS_DefineColumn(&SDDS_output, "t", "Time position", "s", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "gamma", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "dgamma", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "xemit", "NormalizedEmittance-x", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "yemit", "NormalizedEmittance-y", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "xrms", "Beam Size-x", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "yrms", "Beam Size-y", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "xavg", "Position-x", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "yavg", "Position-y", "m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "pxavg", "Average x'", "rad", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "pyavg", "Average y'", "rad", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "alphax", "Alpha-x", NULL, NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "alphay", "Alpha-y", NULL, NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "current", "Current", "Amp", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "wakez", "Wake Loss", "eV/m", NULL, NULL, SDDS_DOUBLE, 0) == -1) ||
            (SDDS_DefineColumn(&SDDS_output, "N", "Number of particles", NULL, NULL, NULL, SDDS_LONG, 0)==-1)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if ((SDDS_WriteLayout(&SDDS_output) != 1) ||
            (SDDS_StartPage(&SDDS_output, slices) != 1)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      } else {
        fileID = fopen(output, "w");
        if (fileID == NULL) {
          fprintf(stderr, "unable to open output file for writing\n");
          exit(1);
        }
        fprintf(fileID, "%ld", slices);
      }
      firstOne = 0;
    } else {
      if (sddsOutput &&
          !SDDS_LengthenTable(&SDDS_output, slices)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      sliceOffset += slices;
    }
    
    
    /* for each slice calculate all the needed values */
    for (i=0; i<slices; i++) {
      N=0;
      xAverage=xpAverage=xRMS=x2Average=xp2Average=xxpAverage=0;
      yAverage=ypAverage=yRMS=y2Average=yp2Average=yypAverage=0;
      pAverage=alphax=alphay=0;
      if (!reverseOrder) {
	s1 = sMin + (wavelength * i);
	s2 = sMin + (wavelength * (i + 1));
      } else {
	s1 = sMin + (wavelength * (slices-i-1));
	s2 = sMin + (wavelength * (slices-i));
      }
      for (j=0;j<rows;j++) {
        selected[j] = 0;
	if ((s1 <= sValues[j]) && (s2 > sValues[j])) {
          selected[j] = 1;
	  N++;
	  xAverage += xValues[j];
	  xpAverage += xpValues[j];
	  yAverage += yValues[j];
	  ypAverage += ypValues[j];
	  pAverage += pValues[j];
	}
      }
      if (N>2) {
        current = N * 2.99792458e8 * (totalCharge/(origRows * wavelength));
        xAverage /= N;
        yAverage /= N;
        xpAverage /= N;
        ypAverage /= N;
        pAverage /= N;
        /*
          pMin = pMax = pAverage;
          */
        pStDev = 0;
        if (localFit)
          removeLocalFit(pValues, sValues, selected, pAverage, rows);
        for (j=0;j<rows;j++) {
          if (selected[j]) {
            pStDev += sqr(pValues[j]-pAverage);
            x2Average += sqr(xValues[j] - xAverage);
            y2Average += sqr(yValues[j] - yAverage);
            xp2Average += sqr(xpValues[j] - xpAverage);
            yp2Average += sqr(ypValues[j] - ypAverage);
            xxpAverage += (xValues[j] - xAverage) * (xpValues[j] - xpAverage);
            yypAverage += (yValues[j] - yAverage) * (ypValues[j] - ypAverage);
            /*
              if (pValues[j] < pMin)
              pMin = pValues[j];
              if (pValues[j] > pMax)
              pMax = pValues[j];
              */
          }
        }
        pStDev = sqrt(pStDev/(N-1.0));
        x2Average /= N;
        y2Average /= N;
        xp2Average /= N;
        yp2Average /= N;
        xxpAverage /= N;
        yypAverage /= N;
        xRMS = sqrt(x2Average);
        yRMS = sqrt(y2Average);
        if ((term = (x2Average * xp2Average) - sqr(xxpAverage))>0)
          xEmittance = sqrt(term)*pAverage;
        else
          xEmittance = 0;
        if ((term = (y2Average * yp2Average) - sqr(yypAverage))>0)
          yEmittance = sqrt(term)*pAverage;
        else
          yEmittance = 0;
        
        alphax = -xxpAverage/(xEmittance/pAverage);
        alphay = -yypAverage/(yEmittance/pAverage);
      } else {
        pAverage = pStDev = xEmittance = yEmittance = xRMS = yRMS = xAverage = 
          yAverage = xpAverage = ypAverage = alphax = alphay = current =  0;
      }
      
      if (sddsOutput) {
	if (SDDS_SetRowValues(&SDDS_output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i+sliceOffset,
			      "s", s1,
			      "t", -s1/c_mks,
			      "gamma", pAverage, 
			      "dgamma", pStDev,
			      /* This was commented out to be consistant with GENESIS
				 where dgamma is the rms value of gamma
                              "dgamma", pMax - pMin,
                              "gammaStDev", pStDev,
			      */
			      "xemit", xEmittance, 
			      "yemit", yEmittance,
			      "xrms", xRMS, 
			      "yrms", yRMS,
			      "xavg", xAverage, 
			      "yavg", yAverage,
			      "pxavg", xpAverage, 
			      "pyavg", ypAverage,
			      "alphax", alphax, 
			      "alphay", alphay,
			      "current", current, 
			      "wakez", 0.0, 
                              "N", N,
                              NULL) != 1) {
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	  exit(1);
	}
      } else {
	fprintf(fileID, "\n % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E % .6E  0.000000E+00", \
		s1, pAverage, pStDev, xEmittance, yEmittance, \
		xRMS, yRMS, xAverage, yAverage, xpAverage, ypAverage, alphax, alphay, current);
      }
    }
    free(tValues);
    free(xValues);
    free(xpValues);
    free(yValues);
    free(ypValues);
    free(pValues);
  }
  free(sValues);
  
  /* close the output file */
  if (sddsOutput) {
    if (!SDDS_WritePage(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (!SDDS_Terminate(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  } else {
    fclose(fileID);
  }

  /* close the input file */
  if (!SDDS_Terminate(&SDDS_input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (removePTailsFlags&PTAILS_OUTPUT &&
      !SDDS_Terminate(&SDDS_pTails)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
    
  return(0);
}

long removeMomentumTails(double *x, double *xp, double *y,
                         double *yp, double *s, double *p, double *t,
                         long rows, double limit, unsigned long flags)
/* t is equivalent to s.  It is filtered here for convenience in output
 * to an elegant-type file
 */
{
  double *delta, pAve;
  long ip, jp;
#if defined(DEBUG)
  static FILE *fp = NULL;
#endif

  if (!rows)
    return rows;
  if (!(delta = malloc(sizeof(*delta)*rows)))
    SDDS_Bomb("memory allocation failure");

  for (ip=pAve=0; ip<rows; ip++)
    pAve += p[ip];
  pAve /= rows;
  for (ip=0; ip<rows; ip++)
    delta[ip] = (p[ip]-pAve)/pAve;
  
  if (flags&PTAILS_FITMODE) {
    double slope, intercept, variance;
    if (!unweightedLinearFit(s, delta, rows, &slope, &intercept, &variance))
      SDDS_Bomb("fit failed in momentum tail removal");
    for (ip=0; ip<rows; ip++)
      delta[ip] -= s[ip]*slope + intercept;
  }

  jp = rows-1;
  for (ip=0; ip<rows; ip++) {
    if (fabs(delta[ip])>limit) {
      if (jp>ip) {
        x[ip]  = x[jp];
        xp[ip] = xp[jp];
        y[ip]  = y[jp];
        yp[ip] = yp[jp];
        s[ip]  = s[jp];
        p[ip]  = p[jp];
        t[ip]  = t[jp];
        delta[ip] = delta[jp];
        jp --;
        ip --;
      }
      rows --;
    }
  }
  free(delta);

#if defined(DEBUG)  
  if (!fp) {
    fp = fopen("e2g.sdds", "w");
    fprintf(fp, "SDDS1\n&column name=x type=double &end\n");
    fprintf(fp, "&column name=xp type=double &end\n");
    fprintf(fp, "&column name=y type=double &end\n");
    fprintf(fp, "&column name=yp type=double &end\n");
    fprintf(fp, "&column name=s type=double &end\n");
    fprintf(fp, "&column name=p type=double &end\n");
    fprintf(fp, "&data mode=ascii &end\n");
    fprintf(fp, "%ld\n", rows);
  }
  for (ip=0; ip<rows; ip++)
    fprintf(fp, "%le %le %le %le %le %le\n", 
            x[ip], xp[ip], y[ip], yp[ip], s[ip], p[ip]);
#endif

  return rows;
}

void removeLocalFit(double *p, double *s, short *selected, double pAverage, long rows)
{
  double slope, intercept, variance;
  long i;

  if (unweightedLinearFitSelect(s, p, selected, rows, &slope, &intercept, &variance)) {
    for (i=0; i<rows; i++) 
      if (selected[i])
        p[i] -= intercept + slope*s[i] - pAverage;
  }
}

