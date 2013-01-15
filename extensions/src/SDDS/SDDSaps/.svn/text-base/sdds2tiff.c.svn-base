/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.4  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.3  2005/11/04 22:46:11  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.2  2004/12/17 21:47:20  soliday
 Added support for WIN32

 Revision 1.1  2004/12/14 20:39:35  soliday
 Used to create one or more TIFF images from an SDDS file.

 *
 */
#include "tiffio.h"
#include "SDDS.h"
#include "mdb.h"
#include "scan.h"

#define OPT_MAXCONTRAST 0
#define OPT_FROMPAGE 1
#define OPT_TOPAGE 2
#define OPT_COLUMNPREFIX 3
#define OPT_PIPE 4
#define N_OPTIONS 5

char *option[N_OPTIONS] = {
  "maxcontrast", "frompage", "topage", "columnPrefix", "pipe"
};

char *USAGE="sdds2tiff [-pipe[=input]] <input> <output> \n\
  [-fromPage=<pageNumber>] [-toPage=<pageNumber>]\n\
  [-columnPrefix=<Line>]\n\
  [-maxContrast]\n\n\
  Two styles input files are accepted. The first is a single column SDDS file\n\
  with Variable1Name and Variable2Name parameters as well as\n\
  <Variable1Name>Dimension and <Variable2Name>Dimension parameters.\n\
  The second style input file is expected to contain multiple columns called Line*.\n\
  Each page in the input file will be converted to a separage TIFF image.\n\
  The output files will be named <output>.%04ld\n\n\
  Program by Robert Soliday. ("__DATE__")\n\n";

int main(int argc, char* argv[]) {
  SDDS_DATASET SDDS_dataset;
  SCANNED_ARG *s_arg;
  TIFF *tif;
  char **columnNames=NULL;
  char *input=NULL, *output=NULL;
  char *columnPrefix=NULL;
  char *buffer=NULL, *outputName=NULL;
  int32_t **data=NULL;
  long i, j, k;
  long linesFound=0, rows=0;
  int32_t nColumns=0;
  long maxvalue=0, page=1, index=1;
  long fromPage=0, toPage=0;
  int  maxContrast=0;
  double div=1;
  char *xVar=NULL, *yVar=NULL, zColumnName[40];
  char xDimName[40], yDimName[40];
  int32_t xDim, yDim;
  long style=1;
  unsigned long pipeFlags=0;

  SDDS_RegisterProgramName(argv[0]);
  
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s", USAGE);
    return(1);
  }
  
  for (i=1; i<argc; i++) {
    if (s_arg[i].arg_type==OPTION) {
      switch (match_string(s_arg[i].list[0], option, N_OPTIONS, 0)) {
      case OPT_MAXCONTRAST:
        maxContrast=1;
        break;
      case OPT_FROMPAGE:
        if (s_arg[i].n_items<2)
          SDDS_Bomb("invalid -fromPage syntax");
        if (sscanf(s_arg[i].list[1], "%ld", &fromPage)!=1 || fromPage<=0)
          SDDS_Bomb("invalid -fromPage syntax or value");
        break;
      case OPT_TOPAGE:
        if (s_arg[i].n_items<2)
          SDDS_Bomb("invalid -toPage syntax");
        if (sscanf(s_arg[i].list[1], "%ld", &toPage)!=1 || toPage<=0)
          SDDS_Bomb("invalid -toPage syntax or value");
        break;
      case OPT_COLUMNPREFIX:
        if (s_arg[i].n_items<2)
          SDDS_Bomb("invalid -columnPrefix syntax");
        SDDS_CopyString(&columnPrefix, s_arg[i].list[1]);
        break;
      case OPT_PIPE:
        if (!processPipeOption(s_arg[i].list+1, s_arg[i].n_items-1, &pipeFlags)) {
          fprintf(stderr, "invalid -pipe syntax\n", argv[0]);
          return(1);
        }
        break;
      default:
        fprintf(stderr, "sdds2tiff: invalid option seen\n%s", USAGE);
        return(1);
      }
    } else {
      if (!input)
        SDDS_CopyString(&input,s_arg[i].list[0]);
      else if (!output)
        SDDS_CopyString(&output,s_arg[i].list[0]);
      else {
        fprintf(stderr, "sdds2tiff: too many filenames\n%s", USAGE);
        return(1);
      }
    }
  }
  
  if (fromPage && toPage && fromPage>toPage)
    SDDS_Bomb("invalid -fromPage and -toPage");

  if (!columnPrefix) {
    columnPrefix = malloc(sizeof(char) * 5);
    sprintf(columnPrefix, "Line");
  }

  if (pipeFlags&USE_STDIN) {
    processFilenames("sdds2tiff", &input, &output, USE_STDIN, 1, NULL);
  }
  if (!SDDS_InitializeInput(&SDDS_dataset, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  outputName = malloc((strlen(output)+10)*sizeof(char));

  /* see if parameters that are needed are present */ 
  if ((SDDS_CheckParameter(&SDDS_dataset, "Variable1Name", NULL, 
                           SDDS_STRING, NULL)) != SDDS_CHECK_OKAY) {
    style=2;
  } 
  if ((SDDS_CheckParameter(&SDDS_dataset, "Variable2Name", NULL, 
                           SDDS_STRING, NULL)) != SDDS_CHECK_OKAY) {
    style=2;
  }
  columnNames = SDDS_GetColumnNames(&SDDS_dataset, &nColumns);
  if (columnNames == NULL) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (style == 1) {
    if (nColumns != 1) {
      fprintf(stderr, "sdds2tiff: Expected one column but found more than one\n");
      return(1);      
    }
    sprintf(zColumnName, "%s", columnNames[0]);
  } else if (style == 2) {
    for (i=0; i<nColumns; i++) {
      if (strncmp(columnPrefix, columnNames[i], strlen(columnPrefix)) == 0) {
        linesFound++;
      }
    }
    if (linesFound == 0) {
      fprintf(stderr, "sdds2tiff: No columns found named %s*\n", columnPrefix);
      return(1);
    }
    data = malloc(sizeof(*data) * linesFound);
  }

  while (SDDS_ReadTable(&SDDS_dataset)>0) { 
    if ((fromPage > 0) && (fromPage > page))
      continue;
    if ((toPage > 0) && (toPage < page))
      break;

    rows = SDDS_RowCount(&SDDS_dataset);

    if (style==1) {
      if (!SDDS_GetParameter(&SDDS_dataset, "Variable1Name", &xVar)) {
        fprintf(stderr, "sdds2tiff: problem getting parameter Variable1Name\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!SDDS_GetParameter(&SDDS_dataset, "Variable2Name", &yVar)) {
        fprintf(stderr, "sdds2tiff: problem getting parameter Variable2Name\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      sprintf(xDimName, "%sDimension", xVar);
      sprintf(yDimName, "%sDimension", yVar);
      if (!SDDS_GetParameterAsLong(&SDDS_dataset, xDimName, &xDim)) {
        fprintf(stderr, "sdds2tiff: problem getting parameter %s\n", xDimName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!SDDS_GetParameterAsLong(&SDDS_dataset, yDimName, &yDim)) {
        fprintf(stderr, "sdds2tiff: problem getting parameter %s\n", yDimName);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      
      fprintf(stderr, "%s %s\n", xVar, yVar);
      fprintf(stderr, "%" PRId32 " %" PRId32 "\n", xDim, yDim);
      free(xVar);
      free(yVar);
      if ((xDim * yDim) != rows) {
        fprintf(stderr, "sdds2tiff: %s * %s does not equal the number of rows in the page\n", xDimName, yDimName);
        return(1);
      }

      data = malloc(sizeof(*data));
      data[0] = SDDS_GetColumnInLong(&SDDS_dataset, zColumnName);
      for (i=0; i<rows; i++) {
        if (data[0][i] > maxvalue)
          maxvalue = data[0][i];
      }
      buffer = malloc(sizeof(char) * rows);
    } else if (style==2) {
      j=0;
      for (i=0; i<nColumns; i++) {
        if (strncmp(columnPrefix, columnNames[i], strlen(columnPrefix)) == 0) {
          data[j] = SDDS_GetColumnInLong(&SDDS_dataset, columnNames[i]);
          for (k=0; k<rows; k++) {
            if (data[j][k] > maxvalue)
              maxvalue = data[j][k];
          }
          j++;
        }
      }
      buffer = malloc(sizeof(char) * rows * linesFound);
      xDim = rows;
      yDim = linesFound;
    }
    if (maxContrast) {
      div=maxvalue / 255.0;
    } else if (maxvalue <= 255) {
      div=1;
    } else if (maxvalue <= 765) {
      div=3;
    } else {
      div=maxvalue / 255.0;
    }
    
    sprintf(outputName, "%s.%04ld", output, index);
    tif = TIFFOpen(outputName, "w");
    if (tif) {
      TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, xDim);
      TIFFSetField(tif, TIFFTAG_IMAGELENGTH, yDim);
      TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
      TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
      TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, yDim);
      
      TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
      TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
      TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
      TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
      
      TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
      
      if (style==1) {
        k = 0;
        for (i=0; i<xDim; i++) {
          for (j=0; j<yDim; j++) {
            buffer[xDim * (yDim-(j+1)) + i] = (unsigned int)(round(data[0][k] / div));
            k++;
          }
        }
        TIFFWriteEncodedStrip(tif, 0, buffer, rows);
      } else if (style==2) {
        for (j=0; j<yDim; j++) {
          for (i=0; i<xDim; i++) {
            buffer[j*xDim+i] = (unsigned int)(round(data[(yDim-j)-1][i] / div));
          }
        }
        TIFFWriteEncodedStrip(tif, 0, buffer, xDim * yDim);
      }
      TIFFClose(tif);
    }
    free(buffer);
    if (style==1) {
      free(data[0]);
      free(data);
    } else if (style==2) {
      for (j=0; j<yDim; j++) {
        free(data[j]);
      }
    }
    index++;
  }


  if (!SDDS_Terminate(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (input) free(input);
  if (output) free(output);
  if (outputName) free(outputName);
  for (i=0; i<nColumns; i++) {
    if (columnNames[i]) free(columnNames[i]);
  }
  if (columnNames) free(columnNames);
  if (style==2) {
    if (data) free(data);
  }
  free_scanargs(&s_arg,argc);
  return(0);
}

