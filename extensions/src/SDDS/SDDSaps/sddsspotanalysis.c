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
 Revision 1.21  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.20  2005/11/15 17:42:47  soliday
 Updated to fix a problem when built with a 64bit compiler.

 Revision 1.19  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.18  2005/10/28 15:24:42  emery
 Removed unused variable k.

 Revision 1.17  2005/10/28 15:21:21  soliday
 No changes. Testing CVS.

 Revision 1.16  2005/01/10 22:23:10  shang
 added calculation of tilt angle and axis

 Revision 1.15  2004/07/29 18:25:18  soliday
 Updated to work with new option in despikeData

 Revision 1.14  2003/09/11 02:06:28  borland
 Added -centerOn option, which helps in analyzing spots that are not
 nice looking or that have hot spots.

 Revision 1.13  2003/09/02 19:16:08  soliday
 Cleaned up code for Linux.

 Revision 1.12  2002/08/14 17:12:56  soliday
 Added Open License

 Revision 1.11  2002/03/09 17:51:47  borland
 Added -singleSpot option.  Eliminates less intense blobs that are disconnected
 from the main blob.

 Revision 1.10  2000/11/21 00:56:21  borland
 Now computes x and y centroids, provides to output file.

 Revision 1.9  2000/10/31 21:21:12  soliday
 Split usage message into smaller parts so that WIN32 can handle it.

 Revision 1.8  2000/10/22 15:00:47  borland
 Saturation check is now done after background removal and cleanup.
 The saturation level is reduced by the background level before the
 comparison, so it is still valid.

 Revision 1.7  2000/10/11 03:40:05  borland
 Added -spotImage option to allow output of spots after processing.
 Hand-writes an SDDS binary file (!).

 Revision 1.6  2000/10/11 01:42:10  borland
 Improved background finder (now use the centroid over a user-defined range
 of the intensity histogram centered on the most populated bin).
 Added antiloner, antihalo, and "symmetric" background modes to remove
 positive noise that is left over after removal of negative pixel values
 that result from background subtraction.

 Revision 1.5  2000/10/09 15:25:32  borland
 Fixed bug in background determination (was off by 1 unit).

 Revision 1.4  2000/10/07 02:03:49  borland
 Added option to specify number of intensity levels and saturation level.
 Saturation level is used to produce count number of saturated pixels, which is
 output to the results file.

 Revision 1.3  2000/10/07 01:18:45  borland
 Fixed bugs in analysis of spot size.  The background subtraction now makes
 use of the "fact" that the data is discretized as integers from 0 to 255.

 Revision 1.2  2000/10/05 18:43:35  borland
 Fixed some memory leaks and UMRs.

 Revision 1.1  2000/10/05 17:59:28  borland
 First version.

 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include "fftpackC.h"

#define SET_ROI 0
#define SET_SPOTROISIZE 1
#define SET_IMAGECOLUMNS 2
#define SET_DESPIKE 3
#define SET_PIPE 4
#define SET_SIZELINES 5
#define SET_LEVELS 6
#define SET_BLANKOUT 7
#define SET_BACKGROUND 8
#define SET_SPOTIMAGE 9
#define SET_SINGLESPOT 10
#define SET_CENTERON 11
#define OPTIONS 12

static char *option[OPTIONS] = {
  "roi", "spotroi", "imagecolumns", "despike", "pipe", "sizeLines",
  "levels", "blankout", "background", "spotimage", "singlespot", "centeron",
    } ;

char *USAGE1 = "sddsspotanalysis [<input>] [<output>] [-pipe[=in][,out]] \n\
[-ROI=[{xy}{01}value=<value>][,{xy}{01}parameter=<name>]]\n\
[-spotROIsize=[{xy}value=<value>][,{xy}parameter=<name>]]\n\
[-centerOn={{xy}centroid | {xy}peak}]\n\
[-imageColumns=<listOfNames>] [-singleSpot]\n\
[-levels=[intensity=<integer>][,saturation=<integer>]]\n\
[-blankOut=[{xy}{01}value=<value>][,{xy}{01}parameter=<name>]]\n\
[-sizeLines=[{xy}value=<value>][,{xy}parameter=<name>]]\n\
[-background=[halfwidth=<value>][,symmetric][,antihalo][,antiloner]]\n\
[-despike=[=neighbors=<integer>][,passes=<integer>][,averageOf=<integer>][,threshold=<value>]]\n\
[-spotImage=<filename>]\n\n";
char *USAGE2 = "-pipe             The standard SDDS Toolkit pipe option.\n\
-ROI              Give the region of interest in pixel units.\n\
                  All data outside this region is ignored.\n\
-spotROISize      Give the full size in pixel units of the ROI\n\
                  around the spot.  This ROI is used for computing\n\
                  spot properties.\n\
-imageColumns     Give a list of names of columns containing image data.\n\
                  Wildcards may be used.\n\
-singleSpot       Used to eliminate multiple spots.  All pixels not connected to \n\
                  the most intense pixel by a path through nonzero pixels are set to\n\
                  zero.\n\
-centerOn         Choose whether to center the spot analysis region on the peak value\n\
                  or the centroid value for x and y.\n\
-levels           Use 'intensity' to give the number of intensity levels in the data;\n\
                  256 is the default, with values from 0 to 255.\n\
                  Use saturation to give the level at which saturation is considered\n\
                  to occur; 254 is the default.\n\
-sizeLines        Specify the number of lines to use for analysis of the beam\n\
                  size.  The default is 3.\n";
char *USAGE3 = "-background       Use halfwidth to specify the number of intensity levels on either side of the\n\
                  most populous intensity to include for computation of the background.\n\
                  The default is 3.  Setting this to zero means that the background\n\
                  level is equal to the intensity of the most populous level.\n\
                  If symmetric is given, then pixels within this width of 0 after\n\
                  background subtraction are set to zero if the intensity of all but one adjacent\n\
                  pixel is less than this level; this symmetrizes the background removal\n\
                  and avoids leaving positive noise.  If antihalo is given, then each line of\n\
                  the spot ROI is scanned from the outer edge toward the center.  Pixels within\n\
                  the halfwidth of 0 after background subtraction are set to zero, until\n\
                  the first pixel is reached which fails this criterion, after which the next\n\
                  line is processed. If antiloner is given, then after background subtraction, \n\
                  the program removes any pixel that is surrounded by all zero pixels.\n\
-despike          Enter despiking parameters for smoothing the data for\n\
                  purposes of finding the spot center.  If this isn't used\n\
                  then the program may pick a noise pixel as the spot center.\n\
                  Default equivalent to -despike=neighbors=4,passes=2,averageOf=4.\n\
-spotImage        Provide the name of a file to which images of the spots will be written.\n\
                  The file can be plotted with sddscontour.\n\n\
Program by Michael Borland. (This is version 2, October 2000.)\n";

#define DESPIKE_AVERAGEOF 0x0001U

#define X0VALUE 0x0001U
#define X1VALUE (X0VALUE<<1) 
#define Y0VALUE (X1VALUE<<1)
#define Y1VALUE (Y0VALUE<<1)
#define X0PARAM (Y1VALUE<<1)
#define X1PARAM (X0PARAM<<1) 
#define Y0PARAM (X1PARAM<<1)
#define Y1PARAM (Y0PARAM<<1)
#define OPTGIVEN (Y1PARAM<<1)

typedef struct {
  double backgroundLevel, integratedSpotIntensity, peakSpotIntensity;
  double saturationCount, backgroundKilledPositive, backgroundKilledNegative;
  int32_t ROI[4], spotROI[4], spotCenter[2];
  double spotSigma[2], spotRange50[2], spotRange65[2], spotRange80[2], spotCentroid[2];
  double rmsCrossProduct, phi, majorAxis, minorAxis;
} IMAGE_ANALYSIS;

typedef struct {
  unsigned long flags;
  int32_t neighbors, passes, averageOf;
  double threshold;
} DESPIKE_SETTINGS;
  
long SetUpOutputFile(SDDS_DATASET *SDDSout, char *outputFile, SDDS_DATASET *SDDSin,
                     char ***copyParamName, long *copyParamNames);
long DetermineQuadLongValues(int32_t *ROI, unsigned long flags, char **parameter, SDDS_DATASET *SDDSin,
		  long nx, long ny, long isROI);
long DetermineDualLongValues(int32_t *spotROISize, unsigned long flags, char **parameter, 
			  SDDS_DATASET *SDDSin, long defaultValue);
void BlankOutImageData(double **image, long nx, long ny, int32_t *region);
long GetImageData(long *nx, double ***image, char **imageColumn, 
		  long ny, SDDS_DATASET *SDDSin);

#define SYMMETRIC_BGREMOVAL 0x0001U
#define ANTIHALO_BGREMOVAL  0x0002U
#define REMOVE_LONERS       0x0004U
#define SINGLE_SPOT         0x0008U
#define XCENTER_PEAK        0x0010U
#define YCENTER_PEAK        0x0020U
#define XCENTER_CENTROID    0x0040U
#define YCENTER_CENTROID    0x0080U
long AnalyzeImageData(double **image, long nx, long ny, int32_t *ROI, int32_t *spotROISize,
		      int32_t *sizeLines, DESPIKE_SETTINGS *despikeSettings,
                      long intensityLevels, long saturationLevel, long backgroundHalfWidth,
                      unsigned long flags, IMAGE_ANALYSIS *analysisResults, FILE *fpSpotImage);

void DetermineBeamSizes(double *sigma, double *spotRange50, double *spotRange65,
                        double *spotRange80,  double *lineBuffer, long i0, long i1);
void DetermineBeamParameters(double **image, long *spotROI, long nx, long ny,
                             double *rmsCrossProduct, 
                             double *phi, double *majorAxis, double *minorAxis);

int main(int argc, char **argv)
{
  SCANNED_ARG *scArg;
  long iArg, imageColumns=0, i, ip;
  char **imageColumn;
  unsigned long pipeFlags=0, ROIFlags=0, spotROIFlags=0, blankOutFlags=0;
  int32_t intensityLevels=256, saturationLevel=254, backgroundHalfWidth=3;
  long despike=0;
  DESPIKE_SETTINGS despikeSettings;
  char *inputFile=NULL, *outputFile=NULL;
  int32_t ROI[4]={-1,-1,-1,-1}, spotROISize[2]={-1,-1};
  char *ROIParameter[4]={NULL,NULL,NULL,NULL};
  int32_t blankOut[4]={-1,-1,-1,-1};
  char *blankOutParameter[4]={NULL,NULL,NULL,NULL};
  char *spotROISizeParameter[2]={NULL,NULL};
  SDDS_DATASET SDDSin, SDDSout;
  long readStatus, nx, ny, outputRow, maxPages=0;
  double **image;
  IMAGE_ANALYSIS analysisResults;
  unsigned long sizeLinesFlags=0, dummyFlags=0, backgroundFlags=0, centerFlags=0;
  int32_t sizeLines[2]={-1,-1};
  char *sizeLinesParameter[2]={NULL, NULL};
  char **copyParamName;
  long copyParamNames, copyBuffer[32];
  char *spotImageFile = NULL;
  FILE *fpSpotImage = NULL;
  
  if (argc<2) {
    fprintf(stderr, "%s%s%s", USAGE1, USAGE2, USAGE3);
    exit(1);
  }
  
  argc = scanargsg(&scArg, argc, argv); 
  for (iArg=1; iArg<argc; iArg++) {
    if (scArg[iArg].arg_type==OPTION) {
      delete_chars(scArg[iArg].list[0], "_");
      /* process options here */
      switch (match_string(scArg[iArg].list[0], option, OPTIONS, 0)) {
      case SET_IMAGECOLUMNS:
        if (scArg[iArg].n_items<2)
          SDDS_Bomb("invalid -column syntax");
        if (imageColumns)
          SDDS_Bomb("give one and only one -column option");
        imageColumns = scArg[iArg].n_items-1;
        imageColumn = tmalloc(sizeof(*imageColumn)*imageColumns);
        for (i=0; i<imageColumns; i++)
          imageColumn[i] = scArg[iArg].list[i+1];
        break;
      case SET_ROI:
        if (ROIFlags&OPTGIVEN)
          SDDS_Bomb("give -ROI only once");
        ROIFlags = OPTGIVEN;
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            (!scanItemList(&ROIFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "x0value", SDDS_LONG, ROI+0, 1, X0VALUE,
                           "x1value", SDDS_LONG, ROI+1, 1, X1VALUE,
                           "y0value", SDDS_LONG, ROI+2, 1, Y0VALUE,
                           "y1value", SDDS_LONG, ROI+3, 1, Y1VALUE,
                           "x0parameter", SDDS_STRING, ROIParameter+0, 1, X0PARAM,
                           "x1parameter", SDDS_STRING, ROIParameter+1, 1, X1PARAM,
                           "y0parameter", SDDS_STRING, ROIParameter+2, 1, Y0PARAM,
                           "y1parameter", SDDS_STRING, ROIParameter+3, 1, Y1PARAM,
                           NULL) ||
             ((ROIFlags&X0VALUE) && ROI[0]<0) || ((ROIFlags&X1VALUE) && ROI[1]<0) ||
             ((ROIFlags&Y0VALUE) && ROI[2]<0) || ((ROIFlags&Y1VALUE) && ROI[3]<0) ||
             ((ROIFlags&X0PARAM) && (!ROIParameter[0] || !strlen(ROIParameter[0]))) ||
             ((ROIFlags&X1PARAM) && (!ROIParameter[1] || !strlen(ROIParameter[1]))) ||
             ((ROIFlags&Y0PARAM) && (!ROIParameter[2] || !strlen(ROIParameter[2]))) ||
             ((ROIFlags&Y1PARAM) && (!ROIParameter[3] || !strlen(ROIParameter[3])))))
          SDDS_Bomb("invalid -ROI syntax/values");
        break;
      case SET_BLANKOUT:
        if (blankOutFlags&OPTGIVEN)
          SDDS_Bomb("give -blankout only once");
        blankOutFlags = OPTGIVEN;
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            (!scanItemList(&blankOutFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "x0value", SDDS_LONG, blankOut+0, 1, X0VALUE,
                           "x1value", SDDS_LONG, blankOut+1, 1, X1VALUE,
                           "y0value", SDDS_LONG, blankOut+2, 1, Y0VALUE,
                           "y1value", SDDS_LONG, blankOut+3, 1, Y1VALUE,
                           "x0parameter", SDDS_STRING, blankOutParameter+0, 1, X0PARAM,
                           "x1parameter", SDDS_STRING, blankOutParameter+1, 1, X1PARAM,
                           "y0parameter", SDDS_STRING, blankOutParameter+2, 1, Y0PARAM,
                           "y1parameter", SDDS_STRING, blankOutParameter+3, 1, Y1PARAM,
                           NULL) ||
             ((blankOutFlags&X0VALUE) && blankOut[0]<0) || ((blankOutFlags&X1VALUE) && blankOut[1]<0) ||
             ((blankOutFlags&Y0VALUE) && blankOut[2]<0) || ((blankOutFlags&Y1VALUE) && blankOut[3]<0) ||
             ((blankOutFlags&X0PARAM) && (!blankOutParameter[0] || !strlen(blankOutParameter[0]))) ||
             ((blankOutFlags&X1PARAM) && (!blankOutParameter[1] || !strlen(blankOutParameter[1]))) ||
             ((blankOutFlags&Y0PARAM) && (!blankOutParameter[2] || !strlen(blankOutParameter[2]))) ||
             ((blankOutFlags&Y1PARAM) && (!blankOutParameter[3] || !strlen(blankOutParameter[3])))))
          SDDS_Bomb("invalid -blankOut syntax/values");
        break;
      case SET_SPOTROISIZE:
        if (spotROIFlags&OPTGIVEN)
          SDDS_Bomb("give -spotROISize only once");
        spotROIFlags = OPTGIVEN;
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            (!scanItemList(&spotROIFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "xvalue", SDDS_LONG, spotROISize+0, 1, X0VALUE,
                           "yvalue", SDDS_LONG, spotROISize+1, 1, Y0VALUE,
                           "xparameter", SDDS_STRING, spotROISizeParameter+0, 1, X0PARAM,
                           "yparameter", SDDS_STRING, spotROISizeParameter+1, 1, Y0PARAM,
                           NULL) ||
             ((spotROIFlags&X0VALUE) && spotROISize[0]<0) || 
             ((spotROIFlags&Y0VALUE) && spotROISize[1]<0) || 
             ((spotROIFlags&X0PARAM) && 
              (!spotROISizeParameter[0] || !strlen(spotROISizeParameter[0]))) ||
             ((spotROIFlags&Y0PARAM) && 
              (!spotROISizeParameter[1] || !strlen(spotROISizeParameter[1])))))
          SDDS_Bomb("invalid -spotROISize syntax/values");
        break;
      case SET_SIZELINES:
        if (sizeLinesFlags&OPTGIVEN)
          SDDS_Bomb("give -sizeLines only once");
	sizeLinesFlags = OPTGIVEN;
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            (!scanItemList(&sizeLinesFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "xvalue", SDDS_LONG, sizeLines+0, 1, X0VALUE,
                           "yvalue", SDDS_LONG, sizeLines+1, 1, Y0VALUE,
                           "xparameter", SDDS_STRING, sizeLinesParameter+0, 1, X0PARAM,
                           "yparameter", SDDS_STRING, sizeLinesParameter+1, 1, Y0PARAM,
                           NULL) ||
             ((sizeLinesFlags&X0VALUE) && sizeLines[0]<0) || 
             ((sizeLinesFlags&Y0VALUE) && sizeLines[1]<0) || 
             ((sizeLinesFlags&X0PARAM) && 
              (!sizeLinesParameter[0] || !strlen(sizeLinesParameter[0]))) ||
             ((sizeLinesFlags&Y0PARAM) && 
              (!sizeLinesParameter[1] || !strlen(sizeLinesParameter[1])))))
          SDDS_Bomb("invalid -sizeLines syntax/values");
        break;
      case SET_DESPIKE:
        scArg[iArg].n_items--;
        despikeSettings.neighbors = 4;
        despikeSettings.passes = 1;
        despikeSettings.threshold = 0;
        despikeSettings.averageOf = 2;
        despikeSettings.flags = 0;
        if (scArg[iArg].n_items>0 &&
            (!scanItemList(&despikeSettings.flags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "neighbors", SDDS_LONG, &despikeSettings.neighbors, 1, 0, 
                           "passes", SDDS_LONG, &despikeSettings.passes, 1, 0,
                           "averageof", SDDS_LONG, &despikeSettings.averageOf, 1, DESPIKE_AVERAGEOF,
                           "threshold", SDDS_DOUBLE, &despikeSettings.threshold, 1, 0,
                           NULL) ||
             despikeSettings.neighbors<2 || despikeSettings.passes<1 || despikeSettings.averageOf<2 || 
             despikeSettings.threshold<0))
          SDDS_Bomb("invalid -despike syntax/values");
        if (!(despikeSettings.flags&DESPIKE_AVERAGEOF))
          despikeSettings.averageOf = despikeSettings.neighbors;
        if (despikeSettings.averageOf>despikeSettings.neighbors)
          SDDS_Bomb("invalid -despike syntax/values: averageOf>neighbors");
        despike = 1;
        break;
      case SET_PIPE:
        if (!processPipeOption(scArg[iArg].list+1, scArg[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_LEVELS:
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            !scanItemList(&dummyFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                           "intensityLevels", SDDS_LONG, &intensityLevels, 1, 0, 
                           "saturationLevel", SDDS_LONG, &saturationLevel, 1, 0,
                           NULL) ||
            intensityLevels<=10 || saturationLevel<=0)
          SDDS_Bomb("invalid -levels syntax/values");
        break;
      case SET_BACKGROUND:
        scArg[iArg].n_items--;
        if (scArg[iArg].n_items<1 ||
            !scanItemList(&backgroundFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                          "halfwidth", SDDS_LONG, &backgroundHalfWidth, 1, 0,
                          "symmetric", -1, NULL, 0, SYMMETRIC_BGREMOVAL,
                          "antihalo", -1, NULL, 0, ANTIHALO_BGREMOVAL,
                          "antiloner", -1, NULL, 0, REMOVE_LONERS,
                           NULL) ||
            backgroundHalfWidth<0)
          SDDS_Bomb("invalid -background syntax/values");
        break;
      case SET_SINGLESPOT:
        if (scArg[iArg].n_items!=1)
          SDDS_Bomb("invalid -singleSpot syntax/values");
        backgroundFlags |= SINGLE_SPOT;
        break;
      case SET_SPOTIMAGE:
        if (scArg[iArg].n_items!=2 ||
            !(spotImageFile=scArg[iArg].list[1]))
          SDDS_Bomb("invalid -spotImage syntax/values");
        break;
      case SET_CENTERON:
        scArg[iArg].n_items--;
        
        if (scArg[iArg].n_items<1 ||
            !scanItemList(&centerFlags, scArg[iArg].list+1, &scArg[iArg].n_items, 0,
                          "xcentroid", -1, NULL, 0, XCENTER_CENTROID,
                          "ycentroid", -1, NULL, 0, YCENTER_CENTROID,
                          "xpeak", -1, NULL, 0, XCENTER_PEAK,
                          "ypeak", -1, NULL, 0, YCENTER_PEAK,
                          NULL))
          SDDS_Bomb("invalid -centerOn syntax");
        break;
      default:
        bomb("unknown option given", NULL);
        break;
      }
    }
    else {
      /* argument is input filename */
      if (!inputFile) {
        inputFile = scArg[iArg].list[0];
      }
      else if (!outputFile) {
        outputFile = scArg[iArg].list[0];
      }
      else
        SDDS_Bomb("too many filenames");
    }
  }

  processFilenames("sddsspotanalysis", &inputFile, &outputFile, pipeFlags, 0, NULL);

  if (!imageColumns)
    SDDS_Bomb("you must give the -imageColumn option");

  if (!SDDS_InitializeInput(&SDDSin, inputFile)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (spotImageFile) {
    fpSpotImage = fopen_e(spotImageFile, "wb", 0);
    fprintf(fpSpotImage, "SDDS1\n&parameter name=Variable1Name, type=string, fixed_value=x &end\n");
    fprintf(fpSpotImage, "&parameter name=Variable2Name, type=string, fixed_value=y &end\n");
    fprintf(fpSpotImage, "&parameter name=xInterval, type=double, fixed_value=1 &end\n");
    fprintf(fpSpotImage, "&parameter name=xMinimum, type=double,  &end\n");
    fprintf(fpSpotImage, "&parameter name=xDimension, type=long,  &end\n");
    fprintf(fpSpotImage, "&parameter name=yInterval, type=double, fixed_value=1 &end\n");
    fprintf(fpSpotImage, "&parameter name=yMinimum, type=double, &end\n");
    fprintf(fpSpotImage, "&parameter name=yDimension, type=long, &end\n");
    fprintf(fpSpotImage, "&column name=Image, type=float, &end\n");
    fprintf(fpSpotImage, "&data mode=binary &end\n");
    fflush(fpSpotImage);
  }
  
  if ((imageColumns=expandColumnPairNames(&SDDSin, &imageColumn, NULL, imageColumns,
                                          NULL, 0, FIND_NUMERIC_TYPE, 0))<=0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb("No quantities selected to analyze");
  }
  if (!SetUpOutputFile(&SDDSout, outputFile, &SDDSin, &copyParamName, &copyParamNames) 
      || !SDDS_StartPage(&SDDSout, maxPages=10)) 
    SDDS_Bomb("Problem setting up output file.");

  ny = imageColumns;
  outputRow = 0;
  if (!(image = calloc(ny, sizeof(*image))))
    SDDS_Bomb("Memory allocation failure");
  nx = 0;
  while ((readStatus=SDDS_ReadPage(&SDDSin))>0) {
    if (readStatus>maxPages) {
      if (!SDDS_LengthenTable(&SDDSout, 10))
	SDDS_Bomb("Problem lengthening output file.");
      maxPages += 10;
    }
    /* Get image into array.
     * N.B.: pixel (ix, iy) is accessed as image[iy][ix].
     */
    if (!GetImageData(&nx, &image, imageColumn, ny, &SDDSin))
      SDDS_Bomb("Problem getting image data.");
    if (!nx)
      continue;
    if (!DetermineQuadLongValues(ROI, ROIFlags, ROIParameter, &SDDSin, nx, ny, 1)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      SDDS_Bomb("Problem determining region of interest---verify that you gave sufficient information.");
    }
    if (blankOutFlags &&
        !DetermineQuadLongValues(blankOut, blankOutFlags, blankOutParameter, &SDDSin, nx, ny, 0))
      SDDS_Bomb("Problem determining blankout region---verify that you gave sufficient information.");
    if (!DetermineDualLongValues(spotROISize, spotROIFlags, spotROISizeParameter, &SDDSin, 150))
      SDDS_Bomb("Problem determine size of spot region of interest---verify that you gave sufficient information.");
    if (!DetermineDualLongValues(sizeLines, sizeLinesFlags, sizeLinesParameter, &SDDSin, 3))
      SDDS_Bomb("Problem determine size of number of lines to use for spot size computation---verify that you gave sufficient information.");
    if (blankOutFlags)
      BlankOutImageData(image, nx, ny, blankOut);
    if (!AnalyzeImageData(image, nx, ny, ROI, spotROISize, sizeLines,
			  despike?&despikeSettings:NULL, 
                          intensityLevels, saturationLevel, backgroundHalfWidth,
                          backgroundFlags|centerFlags, &analysisResults, fpSpotImage)) 
      continue;
    for (ip=0; ip<copyParamNames; ip++) {
      if (!SDDS_GetParameter(&SDDSin, copyParamName[ip], copyBuffer))
        SDDS_Bomb("Problem reading parameter data from input file.");
      if (!SDDS_SetRowValues(&SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, outputRow,
                             copyParamName[ip], copyBuffer, NULL)) {
        SDDS_Bomb("Problem copying parameter data from input file.");
      }
    }
    if (!SDDS_SetRowValues(&SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, outputRow++,
			   "ImageIndex", readStatus-1,
			   "xROILower", analysisResults.ROI[0], 
			   "xROIUpper", analysisResults.ROI[1],
			   "yROILower", analysisResults.ROI[2], 
			   "yROIUpper", analysisResults.ROI[3],
			   "xSpotROILower", analysisResults.spotROI[0], 
			   "xSpotROIUpper", analysisResults.spotROI[1], 
			   "ySpotROILower", analysisResults.spotROI[2], 
			   "ySpotROIUpper", analysisResults.spotROI[3], 
			   "xSpotCenter", analysisResults.spotCenter[0],
			   "ySpotCenter", analysisResults.spotCenter[1],
			   "xSpotCentroid", analysisResults.spotCentroid[0],
			   "ySpotCentroid", analysisResults.spotCentroid[1],
			   "xSpotSigma", analysisResults.spotSigma[0],
			   "xSpotRange50", analysisResults.spotRange50[0],
			   "xSpotRange65", analysisResults.spotRange65[0],
			   "xSpotRange80", analysisResults.spotRange80[0],
			   "ySpotSigma", analysisResults.spotSigma[1],
			   "ySpotRange50", analysisResults.spotRange50[1],
			   "ySpotRange65", analysisResults.spotRange65[1],
			   "ySpotRange80", analysisResults.spotRange80[1],
			   "BackgroundLevel", analysisResults.backgroundLevel,
			   "BackgroundKilledPositive", analysisResults.backgroundKilledPositive,
			   "BackgroundKilledNegative", analysisResults.backgroundKilledNegative,
			   "IntegratedSpotIntensity", analysisResults.integratedSpotIntensity,
			   "PeakSpotIntensity", analysisResults.peakSpotIntensity,
                           "SaturationCount", analysisResults.saturationCount,
                           "phi", analysisResults.phi,
                           "rmsCrossProduct", analysisResults.rmsCrossProduct,
                           "majorAxis", analysisResults.majorAxis,
                           "minorAxis", analysisResults.minorAxis,
			   NULL)) {
      SDDS_SetError("Problem setting values into output file.");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!SDDS_WritePage(&SDDSout) || !SDDS_Terminate(&SDDSout) || !SDDS_Terminate(&SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<ny; i++)
    free(image[i]);
  for (i=0; i<ny; i++)
    free(imageColumn[i]); 
  free(imageColumn);
  for (i=0; i<copyParamNames; i++)
    free(copyParamName[i]);
  free(copyParamName);
  free(image);
  return 0;
}

long SetUpOutputFile(SDDS_DATASET *SDDSout, char *outputFile, SDDS_DATASET *SDDSin,
                     char ***copyParamName, long *copyParamNames)
{
  char **paramName;
  int32_t paramNames;
  
  if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddsspotanalysis output", outputFile) ||
      SDDS_DefineColumn(SDDSout, "xROILower", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xROIUpper", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotROILower", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotROIUpper", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotCenter", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotCentroid", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotSigma", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotRange50", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotRange65", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "xSpotRange80", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "yROILower", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "yROIUpper", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotROILower", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotROIUpper", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotCenter", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotCentroid", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotSigma", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotRange50", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotRange65", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ySpotRange80", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "ImageIndex", NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "BackgroundLevel", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "BackgroundKilledNegative", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "BackgroundKilledPositive", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "IntegratedSpotIntensity", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "PeakSpotIntensity", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "SaturationCount", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 || 
      SDDS_DefineColumn(SDDSout, "phi",NULL, "degree", NULL, NULL, SDDS_DOUBLE, 0)<0 || 
      SDDS_DefineColumn(SDDSout, "rmsCrossProduct",NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
      SDDS_DefineColumn(SDDSout, "majorAxis",NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 || 
      SDDS_DefineColumn(SDDSout, "minorAxis",NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0 ) {
    SDDS_SetError("Problem setting up output file.");
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
  }
  *copyParamNames = 0;
  *copyParamName = NULL;
  if ((paramName=SDDS_GetParameterNames(SDDSin, &paramNames)) && paramNames>0) {
    long ip;
    if (!(*copyParamName = calloc(sizeof(**copyParamName), paramNames)))
      SDDS_Bomb("memory allocation failure");
    for (ip=0; ip<paramNames; ip++) {
      if (SDDS_GetColumnIndex(SDDSout, paramName[ip])>=0) {
        free(paramName[ip]);
        continue;
      }
      if (!SDDS_DefineColumnLikeParameter(SDDSout, SDDSin, paramName[ip], NULL)) {
        SDDS_SetError("Problem setting up output file.");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      }
      (*copyParamName)[ip] = paramName[ip];
      (*copyParamNames)++;
    }
  }
  free(paramName);
  if (!SDDS_WriteLayout(SDDSout)) {
    SDDS_SetError("Problem setting up output file.");
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
  }
  return 1;
}


long DetermineQuadLongValues(int32_t *ROI, unsigned long flags, char **parameter, SDDS_DATASET *SDDSin,
		  long nx, long ny, long isROI)
{
  long i;
  double value;
  double defaultROI[4];
  if (isROI) {
    defaultROI[0] = defaultROI[2] = 0;
    defaultROI[1] = nx-1;
    defaultROI[3] = ny-1;
  } else
    defaultROI[0] = defaultROI[2] = defaultROI[1] = 
      defaultROI[3] = -1;
  for (i=0; i<4; i++) {
    if (flags&(X0VALUE<<i)) continue;
    if (flags&(X0PARAM<<i)) {
      if (!SDDS_GetParameterAsDouble(SDDSin, parameter[i], &value)) {
	SDDS_SetError("parameter is nonexistent or nonnumeric");
	return 0;
      }
      ROI[i] = value+0.5;
    } else
      ROI[i] = defaultROI[i];
  }
  if (ROI[0]<0 || ROI[2]<0) {
    SDDS_SetError("lower bound of region less than zero");
    return 0;
  }
  if (ROI[1]>nx-1 || ROI[3]>ny-1) {
    SDDS_SetError("upper bound of region too large");
    return 0;
  }
  if (ROI[0]>=ROI[1]) {
    SDDS_SetError("x region has zero or negative width");
    return 0;
  }
  if (ROI[2]>=ROI[3]) {
    SDDS_SetError("y region has zero or negative width");
    return 0;
  }
  return 1;
}

long DetermineSpotROISize(int32_t *spotROISize, unsigned long flags, char **parameter, 
			  SDDS_DATASET *SDDSin)
{
  long i;
  double value;

  for (i=0; i<2; i++) {
    if (flags&(X0VALUE<<(2*i))) continue;
    if (flags&(X0PARAM<<(2*i))) {
      if (!SDDS_GetParameterAsDouble(SDDSin, parameter[i], &value)) {
	SDDS_SetError("spot ROI size parameter is nonexistent or nonnumeric");
	return 0;
      }
      spotROISize[i] = value+0.5;
    } else
      spotROISize[i] = 50;
  }
  if (spotROISize[0]<=0) {
    SDDS_SetError("spot ROI size in x is zero or negative");
    return 0;
  }
  if (spotROISize[1]<=0) {
    SDDS_SetError("spot ROI size in y is zero or negative");
    return 0;
  }
  return 1;
}


long DetermineDualLongValues(int32_t *returnValue, unsigned long flags, char **parameter, 
			  SDDS_DATASET *SDDSin, long defaultValue)
{
  long i;
  double value;

  for (i=0; i<2; i++) {
    if (flags&(X0VALUE<<(2*i))) continue;
    if (flags&(X0PARAM<<(2*i))) {
      if (!SDDS_GetParameterAsDouble(SDDSin, parameter[i], &value)) {
	SDDS_SetError("parameter is nonexistent or nonnumeric");
	return 0;
      }
      returnValue[i] = value;
    } else
      returnValue[i] = defaultValue;
  }
  if (returnValue[0]<=0 || returnValue[1]<=0) {
    SDDS_SetError("determined value is nonpositive");
    return 0;
  }
  return 1;
}

long GetImageData(long *nx, double ***image, char **imageColumn, 
		  long ny, SDDS_DATASET *SDDSin)
{
  /* Note that because each column is a horizontal line, the pixels are
   * accessed as image[iy][ix].  I could swap it, but that would mean
   * doubling the amount of memory used.
   */
  long i;
  *nx = 0;
  for (i=0; i<ny; i++) {
    if ((*image)[i]) {
      free((*image)[i]);
      (*image)[i] = NULL;
    }
    if (!((*image)[i] = 
	  SDDS_GetColumnInDoubles(SDDSin, imageColumn[i]))) {
      SDDS_Bomb("Unable to get data from columns");
    }
  }
  *nx = SDDS_RowCount(SDDSin);
  return 1;
}

long AnalyzeImageData(double **image, long nx, long ny, int32_t *ROI, int32_t *spotROISize,
		      int32_t *sizeLines,
		      DESPIKE_SETTINGS *despikeSettings,
                      long intensityLevels, long saturationLevel, long backgroundHalfWidth,
                      unsigned long flags,
		      IMAGE_ANALYSIS *analysisResults, FILE *fpSpotImage)
{
  long ix, iy, iy0, iy1, ix0, ix1, nxROI;
  double maxValue, minValue, background, value, sum1, sum2;
  long ixCenter, iyCenter, ixMax, ixMin;
  int32_t i;
  double *intensityHistogram, *lineBuffer=NULL, *x=NULL, *y=NULL;
  long spotROI[4];
  double mean, sum;

  /* Steps in image analysis:
   * . "Apply" the ROI
   * . Find the spot:
   *    optionally despike the image to remove noise
   *    scan each line for maximum until the maximum maximum is found
   * . Determine and subtract the background:
   *    accumulate histogram of pixel intensities.
   *    find the mode
   * . If requested, run "single-spot" filter.
   * . Check that ixCenter and iyCenter are consistent with having the
   *    spot ROI inside the pixel map.  If not, adjust.
   * . Sum over the spot ROI and subtract background*nx*ny.
   */

  /* set the ROI */
  if (ROI[0]>=0 && ROI[1]>=0 && ROI[0]<ROI[1]) {
    ix0 = ROI[0];
    ix1 = ROI[1];
    if (ix1>=nx)
      ix1 = nx-1;
  } else {
    ix0 = 0;
    ix1 = nx-1;
  }
  nxROI = ix1-ix0+1;

  if (ROI[2]>=0 && ROI[3]>=0 && ROI[2]<ROI[3]) {
    iy0 = ROI[2];
    iy1 = ROI[3];
    if (iy1>=ny)
      iy1 = ny-1;
  } else {
    iy0 = 0;
    iy1 = ny-1;
  }

  if ((iy1-iy0+1)<spotROISize[1] || (ix1-ix0+1)<spotROISize[0])
    SDDS_Bomb("spot ROI is larger than ROI");
  
  
  /* Find the spot  */
  if (despikeSettings)
    if (!(lineBuffer = SDDS_Malloc(sizeof(*lineBuffer)*nx)))
      SDDS_Bomb("memory allocation failure");
  minValue = DBL_MAX;
  maxValue = -DBL_MAX;
  ixCenter = iyCenter = -1;
  for (iy=iy0; iy<=iy1; iy++) {
    for (ix=ix0; ix<=ix1; ix++)
      if (image[iy][ix]<0 || image[iy][ix]>=intensityLevels)
        SDDS_Bomb("image intensity outside intensity level range");
    if (despikeSettings) {
      memcpy(lineBuffer, image[iy]+ix0, nxROI*sizeof(*lineBuffer));
      despikeData(lineBuffer, nxROI, despikeSettings->neighbors,
		  despikeSettings->passes, despikeSettings->averageOf,
		  despikeSettings->threshold, 0);
    } else
      lineBuffer = image[iy]+ix0;
    index_min_max(&ixMin, &ixMax, lineBuffer, nxROI);
    if (lineBuffer[ixMax]>maxValue) {
      maxValue = lineBuffer[ixMax];
      ixCenter = ixMax+ix0;   /* since lineBuffer copy starts at ix=ix0 */
      iyCenter = iy;
    }
    if (lineBuffer[ixMin]<minValue)
      minValue = lineBuffer[ixMin];
  }
  if (despikeSettings) {
    free(lineBuffer);
    lineBuffer = NULL;
  }
  if (ixCenter==-1 || iyCenter==-1) 
    return 0;

  /* Determine the background */
  if (!(intensityHistogram=calloc(sizeof(*intensityHistogram), intensityLevels))) 
    SDDS_Bomb("memory allocation failure");
  for (iy=iy0; iy<=iy1; iy++)
    make_histogram(intensityHistogram, intensityLevels, 
                   -0.5, intensityLevels+0.5, image[iy]+ix0, nxROI, iy==iy0);
 /* index_min_max(&ixMin, &ixMax, intensityHistogram, intensityLevels+1);*/
  index_min_max(&ixMin, &ixMax, intensityHistogram, intensityLevels);
  sum1 = sum2 = 0;
  for (i=ixMax-backgroundHalfWidth; i<=ixMax+backgroundHalfWidth; i++) {
    if (i<0 || i>=intensityLevels) continue;
    sum1 += intensityHistogram[i];
    sum2 += intensityHistogram[i]*i;
  }
  if (sum1)
    background = sum2/sum1;
  else
    background = ixMax;
  if (background<0)
    background = 0;
  free(intensityHistogram);
  /* Compute new ROI for spot only */
  if (flags&XCENTER_CENTROID) {
    mean = sum = 0;
    for (ix=ix0; ix<=ix1; ix++) 
      for (iy=iy0; iy<=iy1; iy++)  {
        sum += image[iy][ix];
        mean += image[iy][ix]*ix;
      }
    mean /= sum;
    spotROI[0] = mean - spotROISize[0]/2;
  } else {
    spotROI[0] = ixCenter - spotROISize[0]/2;
  }
  spotROI[1] = spotROI[0] + spotROISize[0] - 1;
  if (spotROI[0]<ix0) {
    spotROI[0] = ix0;
    spotROI[1] = ix0 + spotROISize[0] - 1;
  } else if (spotROI[1]>ix1) {
    spotROI[1] = ix1;
    spotROI[0] = ix1 - spotROISize[0] + 1;
  }
  if (spotROI[0]<ix0 || spotROI[1]>ix1)
    SDDS_Bomb("spot ROI is larger than ROI for x");  /* shouldn't ever see this */

  if (flags&XCENTER_CENTROID) {
    mean = sum = 0;
    for (ix=ix0; ix<=ix1; ix++) 
      for (iy=iy0; iy<=iy1; iy++) {
        sum += image[iy][ix];
        mean += image[iy][ix]*iy;
      }
    mean /= sum;
    spotROI[2] = mean - spotROISize[1]/2;
  } else {
    spotROI[2] = iyCenter - spotROISize[1]/2;
  }
  
  spotROI[3] = spotROI[2] + spotROISize[1] - 1;
  if (spotROI[2]<iy0) {
    spotROI[2] = iy0;
    spotROI[3] = iy0 + spotROISize[1] - 1;
  } else if (spotROI[3]>iy1) {
    spotROI[3] = iy1;
    spotROI[2] = iy1 - spotROISize[1] + 1;
  }
  if (spotROI[2]<iy0 || spotROI[3]>iy1)
    SDDS_Bomb("spot ROI is larger than ROI for y"); /* shouldn't ever see this */
  
  /* peform background removal */
  analysisResults->saturationCount = 0;
  analysisResults->integratedSpotIntensity = 0;
  analysisResults->backgroundKilledNegative = 0;
  analysisResults->backgroundKilledPositive = 0;
  saturationLevel -= background;
  for (ix=spotROI[0]; ix<=spotROI[1]; ix++) {
    for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
      if ((value = image[iy][ix]-background)<=0) {
        analysisResults->backgroundKilledNegative += 1;
	value = 0;
      }
      image[iy][ix] = value;
    }
  }
  if (flags&SYMMETRIC_BGREMOVAL) {
    for (ix=spotROI[0]; ix<=spotROI[1]; ix++) {
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
        long ox, oy, goodPixels;
        long ox0, ox1, oy0, oy1;
        if (image[iy][ix]>0 && image[iy][ix]<=backgroundHalfWidth) {
          /* if no more than one of the pixels around this pixel are >backgroundHalfWidth, then
           * set this pixel to zero
           */
          ox0 = (ix==spotROI[0]?0:-1);
          ox1 = (ix==spotROI[1]?0:1);
          oy0 = (iy==spotROI[2]?0:-1);
          oy1 = (iy==spotROI[3]?0:1);
          /* the pixel itself is counted but guaranteed to be subtracted off, too */
          /* this avoids an extra test in the loop */
          goodPixels = (ox1-ox0+1)*(oy1-oy0+1);
          for (ox=ox0; ox<=ox1; ox++) {
            for (oy=oy0; oy<=oy1; oy++) {
              if (image[iy+oy][ix+ox]<=backgroundHalfWidth)
                goodPixels--;
            }
          }
          if (goodPixels<=1) {
            analysisResults->backgroundKilledPositive += 1;
            image[iy][ix] = 0;
          }
        }
      }
    }
  }
  if (flags&REMOVE_LONERS) {
    for (ix=spotROI[0]; ix<=spotROI[1]; ix++) {
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
        long ox, oy, company;
        long ox0, ox1, oy0, oy1;
        if (image[iy][ix]>0) {
          /* if none of the pixels around this pixel are nonzero, then set it to zero.
           */
          ox0 = (ix==spotROI[0]?0:-1);
          ox1 = (ix==spotROI[1]?0:1);
          oy0 = (iy==spotROI[2]?0:-1);
          oy1 = (iy==spotROI[3]?0:1);
          /* the pixel itself is counted but guaranteed to be subtracted off, too */
          /* this avoids an extra test in the loop */
          company = (ox1-ox0+1)*(oy1-oy0+1);
          for (ox=ox0; ox<=ox1; ox++) {
            for (oy=oy0; oy<=oy1; oy++) {
              if (image[iy+oy][ix+ox]==0)
                company --;
            }
          }
          if (company<=1) {
            analysisResults->backgroundKilledPositive += 1;
            image[iy][ix] = 0;
          }
        }
      }
    }
  }
  if (flags&ANTIHALO_BGREMOVAL) {
    long try;
    for (try=0; try<2; try++) {
      for (ix=spotROI[0]; ix<=spotROI[1]; ix++) {
        for (iy=spotROI[2]; iy<spotROI[3]; iy++) {
          if (image[iy][ix]>backgroundHalfWidth ||
              image[iy+1][ix]>backgroundHalfWidth)
            break;
          if (image[iy][ix]) {
            image[iy][ix] = 0;
            analysisResults->backgroundKilledPositive += 1;
          }
        }
        if (iy!=spotROI[3]) 
          for (iy=spotROI[3]; iy>spotROI[2]; iy--) {
            if (image[iy][ix]>backgroundHalfWidth ||
                image[iy-1][ix]>backgroundHalfWidth)
              break;
            if (image[iy][ix]) {
              image[iy][ix] = 0;
              analysisResults->backgroundKilledPositive += 1;
            }
          }
      }
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
        for (ix=spotROI[0]; ix<spotROI[1]; ix++) {
          if (image[iy][ix]>backgroundHalfWidth ||
              image[iy][ix+1]>backgroundHalfWidth)
            break;
          if (image[iy][ix]) {
            image[iy][ix] = 0;
            analysisResults->backgroundKilledPositive += 1;
          }
        }
        if (ix!=spotROI[1]) 
          for (ix=spotROI[1]; ix>spotROI[0]; ix--) {
            if (image[iy][ix]>backgroundHalfWidth ||
                image[iy][ix-1]>backgroundHalfWidth)
              break;
            if (image[iy][ix]) {
              image[iy][ix] = 0;
              analysisResults->backgroundKilledPositive += 1;
            }
          }
      }
    }
  }
  if (flags&SINGLE_SPOT) {
    short **connected, changed;
    double maxVal;
    long ix_max=0, iy_max=0;
    
    connected = tmalloc(sizeof(*connected)*nx);
    for (ix=0; ix<nx; ix++)
      connected[ix] = tmalloc(sizeof(**connected)*ny);
    maxVal = -HUGE_VAL;
    for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
        connected[ix][iy] = 0;
        if (image[ix][iy]>maxVal) {
          ix_max = ix;
          iy_max = iy;
          maxVal = image[ix][iy];
        }
      }
    connected[ix_max][iy_max] = 1;
    
    do {
      changed = 0;
      for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
        for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
          if (!image[ix][iy] || connected[ix][iy])
            continue;
          if (ix>spotROI[0] && connected[ix-1][iy]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (ix<spotROI[1] && connected[ix+1][iy]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (iy>spotROI[2] && connected[ix][iy-1]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (iy<spotROI[3] && connected[ix][iy+1]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
        }
      for (ix=spotROI[1]; ix>=spotROI[0]; ix--)
        for (iy=spotROI[3]; iy>=spotROI[2]; iy--) {
          if (!image[ix][iy] || connected[ix][iy])
            continue;
          if (ix>spotROI[0] && connected[ix-1][iy]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (ix<spotROI[1] && connected[ix+1][iy]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (iy>spotROI[2] && connected[ix][iy-1]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
          if (iy<spotROI[3] && connected[ix][iy+1]) {
            changed += (connected[ix][iy] = 1);;
            continue;
          }
        }
    } while (changed);

    for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++)
        if (!connected[ix][iy])
          image[ix][iy] = 0;
  }

  /* check for saturation */
  for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
    for (iy=spotROI[2]; iy<=spotROI[3]; iy++)
      if (image[iy][ix]>saturationLevel) 
        analysisResults->saturationCount += 1;

  /* find the spot intensity and centroids */
  analysisResults->spotCentroid[0] = analysisResults->spotCentroid[1] = 0;
  for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
    for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
      analysisResults->integratedSpotIntensity += image[iy][ix];
      analysisResults->spotCentroid[0] += image[iy][ix]*ix;
      analysisResults->spotCentroid[1] += image[iy][ix]*iy;
    }
  if (analysisResults->integratedSpotIntensity)
    for (i=0; i<2; i++)
      analysisResults->spotCentroid[i] /= analysisResults->integratedSpotIntensity;

  /* find the spot size in y using central lines around the peak */
  if (!(lineBuffer=calloc(sizeof(*lineBuffer), ny)))
    SDDS_Bomb("memory allocation failure");
  if (!(x=calloc(sizeof(*x), ny)))
    SDDS_Bomb("memory allocation failure"); 
  if (!(y=calloc(sizeof(*y), ny)))
    SDDS_Bomb("memory allocation failure");
  for (ix=ixCenter-sizeLines[0]/2; ix<=ixCenter+sizeLines[0]/2; ix++) {
    if (ix<ix0 || ix>ix1)
      continue;
    for (iy=spotROI[2]; iy<=spotROI[3]; iy++)
      lineBuffer[iy] += image[iy][ix];
  }
  for (iy=spotROI[2]; iy<=spotROI[3]; iy++)
    y[iy]=lineBuffer[iy];
  
  DetermineBeamSizes(&analysisResults->spotSigma[1],
		     &analysisResults->spotRange50[1],
		     &analysisResults->spotRange65[1],
		     &analysisResults->spotRange80[1],
		     lineBuffer, spotROI[2], spotROI[3]);
  free(lineBuffer);
  /* find the spot size in x using central lines around the peak */
  if (!(lineBuffer=calloc(sizeof(*lineBuffer), nx)))
    SDDS_Bomb("memory allocation failure");
  for (iy=iyCenter-sizeLines[1]/2; iy<=iyCenter+sizeLines[1]/2; iy++) {
    if (iy<iy0 || iy>iy1) continue;
    for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
      lineBuffer[ix] += image[iy][ix];
  }
  DetermineBeamSizes(&analysisResults->spotSigma[0],
		     &analysisResults->spotRange50[0],
		     &analysisResults->spotRange65[0],
		     &analysisResults->spotRange80[0],
		     lineBuffer, spotROI[0], spotROI[1]);
  free(lineBuffer);
  DetermineBeamParameters(image, spotROI, nx, ny,
                          &analysisResults->rmsCrossProduct,
                          &analysisResults->phi,
                          &analysisResults->majorAxis,
                          &analysisResults->minorAxis);
  /* put results in the structure for return */
  analysisResults->peakSpotIntensity = maxValue - background;
  analysisResults->spotCenter[0] = ixCenter;
  analysisResults->spotCenter[1] = iyCenter;
  analysisResults->backgroundLevel = background;
  analysisResults->ROI[0] = ix0;
  analysisResults->ROI[1] = ix1;
  analysisResults->ROI[2] = iy0;
  analysisResults->ROI[3] = iy1;
  for (i=0; i<4; i++)
    analysisResults->spotROI[i] = spotROI[i];
  if (x) free(x);
  if (y) free(y);
  
  if (fpSpotImage) {
    double x;
    float xf;
    i = (spotROI[1]-spotROI[0]+1)*(spotROI[3]-spotROI[2]+1);
    fwrite(&i, 1, sizeof(i), fpSpotImage);
    x = spotROI[0];
    fwrite(&x, 1, sizeof(x), fpSpotImage);
    i = spotROI[1]-spotROI[0]+1;
    fwrite(&i, 1, sizeof(i), fpSpotImage);
    x = spotROI[2];
    fwrite(&x, 1, sizeof(x), fpSpotImage);
    i = spotROI[3]-spotROI[2]+1;
    fwrite(&i, 1, sizeof(i), fpSpotImage);
    for (ix=spotROI[0]; ix<=spotROI[1]; ix++)
      for (iy=spotROI[2]; iy<=spotROI[3]; iy++) {
        xf = image[iy][ix];
        fwrite(&xf, 1, sizeof(xf), fpSpotImage);
      }
    fflush(fpSpotImage);
  }
  return 1;
}

void DetermineBeamSizes(double *sigma, double *Range50, double *Range65, double *Range80,
                        double *lineBuffer, long i0, long i1)
{
  double centroid, sum;
  long i, j;
  double pLevel[6]={.10, .175, .25, .75, .825, .90};
  double pValue[6];

  centroid = sum = 0;
  *sigma = *Range80 = *Range65 = *Range50 = 0;
  for (i=i0; i<=i1; i++) {
    sum += lineBuffer[i];
    centroid += lineBuffer[i]*i;
  }
  if (sum) {
    centroid = centroid/sum;
    for (i=i0; i<=i1; i++) 
      *sigma += lineBuffer[i]*sqr(i-centroid);
    *sigma = sqrt(*sigma/sum);

    /* integrate the intensity */
    for (i=i0+1; i<=i1; i++)
      lineBuffer[i] += lineBuffer[i-1];
    if (lineBuffer[i1]) {
      for (i=i0; i<=i1; i++)
        lineBuffer[i] /= lineBuffer[i1];
      i = i0+1;
      for (j=0; j<6; j++) {
        pValue[j] = 0;
        while (i<=i1 && lineBuffer[i]<pLevel[j])
          i++;
        if (lineBuffer[i]==lineBuffer[i-1])
          pValue[j] = i - 0.5;
        else
          pValue[j] = i - (lineBuffer[i]-pLevel[j])/(lineBuffer[i]-lineBuffer[i-1]);
      }
      *Range80 = pValue[5] - pValue[0];
      *Range65 = pValue[4] - pValue[1];
      *Range50 = pValue[3] - pValue[2];
    }
  }
}

void BlankOutImageData(double **image, long nx, long ny, int32_t *region)
{
  long ix, iy, count=0;
  for (ix=region[0]; ix<=region[1]; ix++) 
    for (iy=region[2]; iy<=region[3]; iy++, count++)
      image[iy][ix] = 0;
}

void DetermineBeamParameters(double **image, long *spotROI, long nx, long ny,
                             double *rmsCrossProduct, 
                             double *phi, double *majorAxis, double *minorAxis)
{
  long i,j;
  long x1, x2, y1, y2;
  double imageArea=0, x2Rms=0, y2Rms=0, xyRms=0, dominator=0, xcentroid=0,ycentroid=0;
  
  x1=spotROI[0];
  x2=spotROI[1];
  y1=spotROI[2];
  y2=spotROI[3];
  
  /*use whole image */
 /* x1=y1=1;
  x2=nx-1;
  y2=ny-1; */
  
  /*calcuate the area of image(x,y) in a square defined by (x1,y1), (x1,y2), (x2,y2) and (x2,y1),
    i.e. integ of image(x,y)dxdy in the spotROI, here dxdy=1 */
  /*here, each i is for one column, image[i][j] for each j is the jth value of column i,
    j is the index of rows (x value) and i is the index of column (y value) */
  for (i=y1;i<=y2;i++) 
    for (j=x1;j<=x2;j++) {
      imageArea +=image[i][j];
      xcentroid += j * image[i][j];
      ycentroid += i * image[i][j];
    }
  if (imageArea==0) 
    *rmsCrossProduct=*majorAxis=*minorAxis=DBL_MAX;
  else {
    xcentroid = xcentroid/imageArea;
    ycentroid = ycentroid/imageArea;
    for (i=y1;i<=y2;i++) 
      for (j=x1;j<=x2;j++) {
        x2Rms += sqr(j-xcentroid)*image[i][j];
        y2Rms += sqr(i-ycentroid)*image[i][j];
        xyRms += (i-ycentroid)*(j-xcentroid)*image[i][j];
      }
    x2Rms = x2Rms / imageArea;
    y2Rms = y2Rms / imageArea;
    xyRms = xyRms /imageArea;
    dominator=x2Rms*y2Rms-xyRms*xyRms;
    *rmsCrossProduct = xyRms;
    *phi= 0.5 * atan2(2*xyRms, x2Rms - y2Rms) / PI * 180;
    *majorAxis = sqrt(2*dominator/(x2Rms+y2Rms-sqrt((x2Rms-y2Rms)*(x2Rms-y2Rms)+4*xyRms*xyRms)));
    *minorAxis = sqrt(2*dominator/(x2Rms+y2Rms+sqrt((x2Rms-y2Rms)*(x2Rms-y2Rms)+4*xyRms*xyRms)));
  }
}


