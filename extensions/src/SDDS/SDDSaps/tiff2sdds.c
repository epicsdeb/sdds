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
 Revision 1.4  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.3  2004/12/14 20:40:02  soliday
 Fixed a memory leak.

 Revision 1.2  2003/09/02 19:16:08  soliday
 Cleaned up code for Linux.

 Revision 1.1  2002/09/19 15:29:21  soliday
 Added tiff2sdds

 *
 */
#include "tiffio.h"
#include "SDDS.h"
#include "mdb.h"
#include "scan.h"

#define OPT_REDONLY 0
#define OPT_GREENONLY 1
#define OPT_BLUEONLY 2
#define OPT_SINGLECOLUMNMODE 3
#define N_OPTIONS 4

char *option[N_OPTIONS] = {
  "redOnly", "greenOnly", "blueOnly",
  "singleColumnMode",
};

char *USAGE="tiff2sdds <input> <output> \n\
  [-redOnly] [-greenOnly] [-blueOnly]\n\
  [-singleColumnMode]\n\
  Program by Robert Soliday. ("__DATE__")\n\n";

int main(int argc, char* argv[]) {
  SDDS_DATASET SDDS_dataset;
  SCANNED_ARG *s_arg;
  TIFF *tif;
  char *input=NULL, *output=NULL;
  char **columnNames=NULL;
  int32_t **data;
  int32_t *indexes=NULL;
  long rgb[3];
  long i, j, n=0, singleColumnMode=0;

  SDDS_RegisterProgramName(argv[0]);

  rgb[0] = rgb[1] = rgb[2] = 1;

  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s", USAGE);
    return(1);
  }

  for (i=1; i<argc; i++) {
    if (s_arg[i].arg_type==OPTION) {
      switch (match_string(s_arg[i].list[0], option, N_OPTIONS, 0)) {
      case OPT_REDONLY:
        rgb[0] = 1;
        rgb[1] = rgb[2] = 0;
        break;
      case OPT_GREENONLY:
        rgb[1] = 1;
        rgb[0] = rgb[2] = 0;
        break;
      case OPT_BLUEONLY:
        rgb[2] = 1;
        rgb[0] = rgb[1] = 0;
        break;
      case OPT_SINGLECOLUMNMODE:
        singleColumnMode = 1;
        break;
      default:
        fprintf(stderr, "invalid option seen\n%s", USAGE);
        return(1);
      }
    } else {
      if (!input)
        input = s_arg[i].list[0];
      else if (!output)
        output = s_arg[i].list[0];
      else {
        fprintf(stderr, "too many filenames\n%s", USAGE);
        return(1);
      }
    }
  }

  tif = TIFFOpen(input, "r");
  if (tif) {
    uint32 w, h;
    size_t npixels;
    uint32* raster;
    double tmp;
    int32_t tmp2;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
      if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
        
	if (!SDDS_InitializeOutput(&SDDS_dataset,SDDS_BINARY,
                                   1,NULL,NULL,output)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (singleColumnMode) {
          if (SDDS_DefineParameter(&SDDS_dataset, "Variable1Name",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_STRING, "x")<0) {
            fprintf(stderr, "Problem defining parameter Variable1Name.\n");
            return(1);
          }
          if (SDDS_DefineParameter(&SDDS_dataset, "Variable2Name",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_STRING, "y")<0) {
            fprintf(stderr, "Problem defining parameter Variable1Name.\n");
            return(1);
          }
          tmp = 1;
          if (SDDS_DefineParameter1(&SDDS_dataset, "xInterval",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_DOUBLE, &tmp)<0) {
            fprintf(stderr, "Problem defining parameter xInterval.\n");
            return(1);
          }
          if (SDDS_DefineParameter1(&SDDS_dataset, "yInterval",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_DOUBLE, &tmp)<0) {
            fprintf(stderr, "Problem defining parameter yInterval.\n");
            return(1);
          }
          tmp2 = w;
          if (SDDS_DefineParameter1(&SDDS_dataset, "xDimension",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_LONG, &tmp2)<0) {
            fprintf(stderr, "Problem defining parameter xDimension.\n");
            return(1);
          }
          tmp2 = h;
          if (SDDS_DefineParameter1(&SDDS_dataset, "yDimension",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_LONG, &tmp2)<0) {
            fprintf(stderr, "Problem defining parameter yDimension.\n");
            return(1);
          }
          tmp = 0;
          if (SDDS_DefineParameter1(&SDDS_dataset, "xMinimum",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_DOUBLE, &tmp)<0) {
            fprintf(stderr, "Problem defining parameter xMinimum.\n");
            return(1);
          }
          if (SDDS_DefineParameter1(&SDDS_dataset, "yMinimum",
                                   NULL, NULL, NULL, NULL,
                                   SDDS_DOUBLE, &tmp)<0) {
            fprintf(stderr, "Problem defining parameter yMinimum.\n");
            return(1);
          }
          if (SDDS_DefineSimpleColumn(&SDDS_dataset, "z", 
                                      NULL, SDDS_LONG)<0) {
            fprintf(stderr, "Problem defining column z.\n");
            return(1);
          }
          data = malloc(sizeof(*data)*1);
          data[0] = malloc(sizeof(*(data[0]))*npixels);
        } else {
          if (SDDS_DefineSimpleColumn(&SDDS_dataset, "Index", 
                                      NULL, SDDS_LONG)<0) {
            fprintf(stderr, "Problem defining column Index.\n");
            return(1);
          }
          indexes = malloc(sizeof(*indexes)*w);
          columnNames = malloc(sizeof(char**)*h);
          data = malloc(sizeof(*data)*h);
          for (i=0; i<h; i++) {
            columnNames[i] = malloc(sizeof(char*)*15);
            data[i] = malloc(sizeof(*(data[i]))*w);
            sprintf(columnNames[i], "Line%05ld", i);
            if (SDDS_DefineSimpleColumn(&SDDS_dataset, columnNames[i], 
                                        NULL, SDDS_LONG)<0) {
              fprintf(stderr, "Problem defining column %s.\n", columnNames[i]);
              return(1);
            }
          }
        }

        if (!SDDS_WriteLayout(&SDDS_dataset))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (singleColumnMode) {
          if (!SDDS_StartPage(&SDDS_dataset, npixels))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          for (i=0; i<h; i++) {
            for (j=0; j<w; j++) {
              data[0][j*h+i] = TIFFGetR(raster[n])*rgb[0]+TIFFGetG(raster[n])*rgb[1]+TIFFGetB(raster[n])*rgb[2];
              n++;
            }
          } 
          
          if (!SDDS_SetColumnFromLongs(&SDDS_dataset, SDDS_SET_BY_NAME,
                                       data[0], npixels, "z"))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        } else {
          if (!SDDS_StartPage(&SDDS_dataset, w))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          for (i=0; i<w; i++)
            indexes[i] = i;
          for (i=0; i<h; i++) {
            for (j=0; j<w; j++) {
              data[i][j] = TIFFGetR(raster[n])*rgb[0]+TIFFGetG(raster[n])*rgb[1]+TIFFGetB(raster[n])*rgb[2];
              n++;
            } 
          }
          if (!SDDS_SetColumnFromLongs(&SDDS_dataset, SDDS_SET_BY_NAME,
                                       indexes, w, "Index"))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          for (i=0; i<h; i++) {
            if (!SDDS_SetColumnFromLongs(&SDDS_dataset, SDDS_SET_BY_NAME,
                                         data[i], w, columnNames[i]))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        }
        
        if (!SDDS_WritePage(&SDDS_dataset) || !SDDS_Terminate(&SDDS_dataset))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

        if (singleColumnMode) {
          free(data[0]);
          free(data);
        } else {
          for (i=0; i<h; i++) {
            free(columnNames[i]);
            free(data[i]);
          }
          free(columnNames);
          free(data);
        }
      }
      _TIFFfree(raster);
    }
    TIFFClose(tif);
  }
  if (indexes) free(indexes);
  free_scanargs(&s_arg,argc);
  return(0);
}

