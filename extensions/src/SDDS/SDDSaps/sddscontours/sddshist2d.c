/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: hist2d.c
 * purpose: 2-d histogram program with output to sddscontour program
 *
 * Michael Borland, 1988, 1995.
 $Log: not supported by cvs2svn $
 Revision 1.15  2009/06/23 22:31:05  borland
 Fixed bug in last revision.

 Revision 1.14  2009/06/23 21:38:54  borland
 Added -copyParameters option.

 Revision 1.13  2007/07/05 21:21:09  borland
 Updated version number.

 Revision 1.12  2007/07/05 16:15:22  borland
 Corrected the code that determines the plot range.  The previous version worked poorly when the range of data was small compared to the mean value.

 Revision 1.11  2006/12/14 22:22:41  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.

 Revision 1.10  2006/07/18 18:32:36  shang
 added -zcolumn option for 3-d histogram computation.

 Revision 1.9  2006/03/27 21:46:36  jiaox
 Fixed a bug that non-doubles type columns caused segmentation fault.

 Revision 1.8  2004/09/15 16:07:35  borland
 Added -minimumScale option.

 Revision 1.7  2002/08/14 17:35:28  soliday
 Added Open License

 Revision 1.6  2001/01/10 19:34:35  soliday
 Standardized usage message.

 Revision 1.5  2000/10/04 21:02:51  soliday
 Added definition for outfile that was causing problems with Linux when using
 shared libraries.

 Revision 1.4  1999/06/03 20:43:11  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/09/06 14:22:58  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"
#include "SDDS.h"
#include "table.h"
#include "scan.h"

#define SET_X_PARAMETERS 0
#define SET_Y_PARAMETERS 1
#define SET_NORMALIZE 2
#define SET_SMOOTH 3
#define SET_WEIGHTS 4
#define SET_VERBOSE 5
#define SET_COLUMNS 6
#define SET_OUTPUTNAME 7
#define SET_SAMESCALE 8
#define SET_COMBINE 9
#define SET_PIPE 10
#define SET_MINIMUM_SCALE 11
#define SET_Z_COLUMN 12
#define SET_COPY_PARAMETERS 13
#define N_OPTIONS 14

FILE* outfile;
char *option[N_OPTIONS] = {
    "xparameters", "yparameters", "normalize",
    "smooth", "weights", "verbose", "columns",
    "outputname", "samescale", "combine", "pipe",
    "minimumscale", "zColumn", "copyparameters",
    } ;

char *USAGE="sddshist2d [<inputfile>] [<outputfile>] [-pipe[=input][,output]] \n\
-columns={<xname>,<yname>|<yname>} \n\
[-outputname=<string>] [-samescale] [-combine] \n\
[-xparameters=<n_bins>[,<lower>,<upper>]] \n\
[-yparameters=<n_bins>[,<lower>,<upper>]] \n\
[-minimumScale=<value>]\n\
[-verbose] [-normalize[=sum]] [-smooth[=<passes>]] \n\
[-weights=<column-name>[,average]] [-zColumn=<column-name>[,bins=<n_bins>,lower=<lower>,upper=<upper>]]\n\
[-copyParameters]\n\n\
Produces output suitable for use with contour.\n\
  Defaults:  21x21 bins encompassing all of the data.\n\
             11 contours encompassing all of the data.\n\
Program by Michael Borland. (This is version 3, July 2007.)\n";

typedef struct param_store {
/* array for parameter data.  The address of the data for the ith parameter
 * is parameter[i].  So *(<type-name> *)parameter[i] gives the data itself.  For type
 * SDDS_STRING the "data itself" is actually the address of the string, and the type-name
 * is "char *".
 */
void **param;
short filled;
struct param_store *next;
} PARAM_STORE;

PARAM_STORE *saveParameters(PARAM_STORE *context, SDDS_DATASET *SDDSin);
PARAM_STORE *setParameters(PARAM_STORE *context, SDDS_DATASET *SDDSout);

long define_sddscontour_parameters(SDDS_TABLE *output, SDDS_TABLE *input, char *varname, char *rootname);
long set_sddscontour_parameters(SDDS_TABLE *output, char *rootname, long dimen, double min, double delta);
long find2d_min_max(double *min, double *max, double **data, long *rows, long sets);

int main(int argc, char **argv)
{
  long i_arg, copyParameters=0;
  SDDS_TABLE SDDS_in, SDDS_out;
  SCANNED_ARG *scanned;
  char *input, *output;
  double **histogram;
  long **count;
  long i, j, nx, ny, ix, iy, do_normalize, n_smooth_passes, *rows, nz, iz;
  long n_binned, n_pages, verbose, one_page_per_line, weights_average;
  double **xdata, **ydata, **weight, **zdata, z_center;
  double x, y, dx, dy, z, dz;
  double users_xmin, users_xmax, users_ymin, users_ymax, users_zmin, users_zmax;
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double cmin, cmax, minimumScale;
  double hrange, middle;
  char *weight_column, *output_name, *output_units;
  long tablecount, samescale, combine;
  char *xname, *yname, *zname;
  unsigned long pipeFlags, dummyFlags=0;
  PARAM_STORE *paramContext, paramStore;
  
  if (argc<3)
    bomb(NULL, USAGE);

  input = output = NULL;
  users_xmin = users_xmax = 0;
  users_ymin = users_ymax = 0;
  users_zmin = users_zmax = 0;
  cmin = cmax = 0;
  nx = ny = 21;
  nz = 1;
  do_normalize = n_smooth_passes = verbose = 0;
  weight_column = NULL;
  weights_average = 0;
  samescale = combine = 0;
  yname = xname = zname = NULL;
  output_name = output_units = NULL;
  one_page_per_line = 0;
  pipeFlags = 0;
  minimumScale = 0;
  
  scanargs(&scanned, argc, argv); 
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (scanned[i_arg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[i_arg].list[0],
                           option, N_OPTIONS, 0)) {
      case SET_Z_COLUMN:
	zname = scanned[i_arg].list[1];
	if (scanned[i_arg].n_items>2) {
	  scanned[i_arg].n_items -=2;
	  if (!scanItemList(&dummyFlags, scanned[i_arg].list+2, &scanned[i_arg].n_items, 0,
                            "bins", SDDS_LONG, &nz, 1, 0,
                            "lower", SDDS_DOUBLE, &users_zmin, 1, 0,
			    "upper", SDDS_DOUBLE, &users_zmax, 1, 0,
                            NULL))
            SDDS_Bomb("Invalid -zColumn syntax");
	  scanned[i_arg].n_items +=2;
	}
	if (nz<1)
	  nz = 21;
	break;
      case SET_X_PARAMETERS:
        if (scanned[i_arg].n_items==2) {
          if (1!=sscanf(scanned[i_arg].list[1], "%ld", &nx) ||
              nx<=2)
            SDDS_Bomb("invalid number of bins for x");
        }
        else if (scanned[i_arg].n_items==4) {
          if (1!=sscanf(scanned[i_arg].list[1], "%ld", &nx) ||
              1!=sscanf(scanned[i_arg].list[2], "%lf", &users_xmin) ||
              1!=sscanf(scanned[i_arg].list[3], "%lf", &users_xmax) ||
              nx<=2 || users_xmin>=users_xmax )
            SDDS_Bomb("invalid -x_parameters values");
        }
        else
          SDDS_Bomb("wrong number of items for -x_parameters");
        break;
      case SET_Y_PARAMETERS:
        if (scanned[i_arg].n_items==2) {
          if (1!=sscanf(scanned[i_arg].list[1], "%ld", &ny) ||
              ny<=2)
            SDDS_Bomb("invalid number of bins for y");
        }
        else if (scanned[i_arg].n_items==4) {
          if (1!=sscanf(scanned[i_arg].list[1], "%ld", &ny) ||
              1!=sscanf(scanned[i_arg].list[2], "%lf", &users_ymin) ||
              1!=sscanf(scanned[i_arg].list[3], "%lf", &users_ymax) ||
              ny<=2 || users_ymin>=users_ymax )
            SDDS_Bomb("invalid -y_parameters values");
        }
        else
          SDDS_Bomb("wrong number of items for -y_parameters");
        break;
      case SET_MINIMUM_SCALE:
        if (scanned[i_arg].n_items!=2 || sscanf(scanned[i_arg].list[1], "%lf", &minimumScale)!=1 ||
              minimumScale<=0)
          SDDS_Bomb("invalid -minimumScale syntax/value");
        break;
      case SET_NORMALIZE:
        if (scanned[i_arg].n_items==2) {
          if (strncmp(scanned[i_arg].list[1], "sum", strlen(scanned[i_arg].list[1]))==0)
            do_normalize = 2;
          else
            SDDS_Bomb("invalid -normalize syntax");
        }
        else
          do_normalize = 1;
        break;
      case SET_SMOOTH:
        if (scanned[i_arg].n_items==1)
          n_smooth_passes = 1;
        else if (scanned[i_arg].n_items==2) {
          if (sscanf(scanned[i_arg].list[1], "%ld", &n_smooth_passes)!=1 ||
              n_smooth_passes<1)
            SDDS_Bomb("invalid -smooth syntax");
        }
        else
          SDDS_Bomb("invalid -smooth syntax");
        break;
      case SET_WEIGHTS:
        if (scanned[i_arg].n_items!=2 && scanned[i_arg].n_items!=3)
          SDDS_Bomb("invalid -weights syntax");
        weight_column = scanned[i_arg].list[1];
        weights_average = 0;
        if (scanned[i_arg].n_items==3) {
          if (strncmp(scanned[i_arg].list[2], "average", strlen(scanned[i_arg].list[2]))!=0)
            SDDS_Bomb("invalid -weights syntax");
          weights_average = 1;
        }
        break;
      case SET_VERBOSE:
        verbose = 1;
        break;
      case SET_COLUMNS:
        if (scanned[i_arg].n_items!=3 && scanned[i_arg].n_items!=2)
          SDDS_Bomb("invalid -columns syntax");
        if (scanned[i_arg].n_items==2) {
          yname = scanned[i_arg].list[1];
          one_page_per_line = 1;
        }
        else {
          xname = scanned[i_arg].list[1];
          yname = scanned[i_arg].list[2];
        }
        break;
      case SET_OUTPUTNAME:
        if (scanned[i_arg].n_items!=2)
          SDDS_Bomb("invalid -outputname syntax");
        output_name = scanned[i_arg].list[1];
        break;
      case SET_SAMESCALE:
        samescale = 1;
        break;
      case SET_COMBINE:
        combine = 1;
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_COPY_PARAMETERS:
        copyParameters = 1;
        paramContext = &paramStore;
        paramStore.filled = -1;  /* indicates root node */
        break;
      default:
        bomb("unknown option given", USAGE);
        break;
      }
    }
    else {
      if (!input)
        input = scanned[i_arg].list[0];
      else if (!output)
        output = scanned[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }

  processFilenames("sddshist2d", &input, &output, pipeFlags, 0, NULL);

  if (!yname || SDDS_StringIsBlank(yname) || (xname && SDDS_StringIsBlank(xname)))
    SDDS_Bomb("invalid/missing -columns option");

  if (!SDDS_InitializeInput(&SDDS_in, input)) {
    fprintf(stderr, "problem initializing file %s\n", input);
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  }

  if (!output_name) {
    if (!weights_average) 
      output_name = "frequency";
    else
      SDDS_CopyString(&output_name, weight_column);
  }

  output_units = NULL;
  if (weights_average && !SDDS_GetColumnInformation(&SDDS_in, "units", &output_units, SDDS_GET_BY_NAME, weight_column))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

  if (!SDDS_InitializeOutput(&SDDS_out, SDDS_BINARY, 0, NULL, "sddshist2d output", output) ||
      !define_sddscontour_parameters(&SDDS_out, &SDDS_in, "Variable1Name", xname) ||
      !define_sddscontour_parameters(&SDDS_out, &SDDS_in, "Variable2Name", yname) ||
      SDDS_DefineColumn(&SDDS_out, output_name, NULL, output_units, NULL, NULL, SDDS_DOUBLE, 0)<0)
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if (zname && (!define_sddscontour_parameters(&SDDS_out, &SDDS_in, "ZColumnName", zname) || 
                SDDS_DefineParameter(&SDDS_out,"zCenter",NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if (copyParameters && !SDDS_TransferAllParameterDefinitions(&SDDS_out, &SDDS_in, 0))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if (!SDDS_WriteLayout(&SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  
  xdata = ydata = weight = zdata = NULL;
  rows = NULL;
  n_pages = 0;
  while ((tablecount=SDDS_ReadTable(&SDDS_in))>0) {
    xdata  = SDDS_Realloc(xdata, sizeof(*xdata)*(n_pages+1));
    ydata  = SDDS_Realloc(ydata, sizeof(*ydata)*(n_pages+1));
    if (zname)
      zdata = SDDS_Realloc(zdata, sizeof(*zdata)*(n_pages+1));
    if (weight_column)
      weight = SDDS_Realloc(weight, sizeof(*xdata)*(n_pages+1));
    rows   = SDDS_Realloc(rows, sizeof(*xdata)*(n_pages+1));
    if (xname && !(xdata[n_pages] = SDDS_GetColumnInDoubles(&SDDS_in, xname))) {
      fprintf(stderr, "problem getting data for x quantity (%s)\n", xname);
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if (!(ydata[n_pages] = SDDS_GetColumnInDoubles(&SDDS_in, yname))) {
      fprintf(stderr, "problem getting data for y quantity (%s)\n", yname);
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if (zname && !(zdata[n_pages] = SDDS_GetColumnInDoubles(&SDDS_in, zname))) {
      fprintf(stderr, "problem getting data for z column (%s)\n", zname);
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if (weight_column && !(weight[n_pages]=SDDS_GetColumnInDoubles(&SDDS_in, weight_column))) {
      fprintf(stderr, "problem getting data for weight quantity (%s)\n", weight_column);
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if (copyParameters)
      paramContext = saveParameters(paramContext, &SDDS_in);
    rows[n_pages] = SDDS_CountRowsOfInterest(&SDDS_in);
    n_pages++;
  }
  if (!SDDS_Terminate(&SDDS_in))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  if (copyParameters)
    paramContext = &paramStore;

  if (one_page_per_line) {
    users_xmin = 0;
    users_xmax = (n_pages-1)*(1+1e-7);
    dx = 1;
    nx = n_pages;
    combine = 1;
    for (j=0; j<n_pages; j++) {
      xdata[j] = tmalloc(sizeof(**xdata)*rows[j]);
      for (i=0; i<rows[j]; i++)
        xdata[j][i] = j;
    }
  }
  histogram = tmalloc(sizeof(*histogram)*nz);
  count = tmalloc(sizeof(*count)*nz);
  for (i=0; i<nz; i++) {
    histogram[i] = tmalloc(sizeof(**histogram)*nx*ny);
    count[i] = tmalloc(sizeof(**count)*nx*ny);
  }
  if (samescale || combine) {
    if (users_xmin==users_xmax) {
      /* auto-scale in x */
      find2d_min_max(&xmin, &xmax, xdata, rows, n_pages);
      if (xmin==xmax) {
        if (!minimumScale)
          SDDS_Bomb("can't auto-scale in x--no spread to data.  Try using the -minimumScale option.");
        xmin -= minimumScale/2;
        xmax += minimumScale/2;
      }
      hrange = 1.0001*(xmax-xmin)/2.;
      middle = (xmax+xmin)/2.;
      xmax = middle+hrange;
      xmin = middle-hrange;
      users_xmin = xmin;
      users_xmax = xmax;
    }

    if (users_ymin==users_ymax) {
      /* auto-scale in y */
      find2d_min_max(&ymin, &ymax, ydata, rows, n_pages);
      if (ymin==ymax) {
        if (!minimumScale)
          SDDS_Bomb("can't auto-scale in y--no spread to data.  Try using the -minimumScale option.");
        ymin -= minimumScale/2;
        ymax += minimumScale/2;
      }
      hrange = 1.0001*(ymax-ymin)/2.;
      middle = (ymax+ymin)/2.;
      ymax = middle+hrange;
      ymin = middle-hrange;
      users_ymin = ymin;
      users_ymax = ymax;
    }
    if (zname && users_zmin==users_zmax) {
      /* auto-scale in y */
      find2d_min_max(&zmin, &zmax, zdata, rows, n_pages);
      if (zmin==zmax) {
        if (!minimumScale)
          SDDS_Bomb("can't auto-scale in y--no spread to data.  Try using the -minimumScale option.");
        zmin -= minimumScale/2;
        zmax += minimumScale/2;
      }
      hrange = 1.0001*(zmax-zmin)/2.;
      middle = (zmax+zmin)/2.;
      zmax = middle+hrange;
      zmin = middle-hrange;
      users_zmin = zmin;
      users_zmax = zmax;
    }
  }
  
  for (j=0; j<n_pages; j++) {
    if (j==0 || !combine)
      for (iz=0; iz<nz; iz++)
        for (ix=0; ix<nx; ix++)
          for (iy=0; iy<ny; iy++) {
            histogram[iz][ix*ny+iy] = 0.;
            count[iz][ix*ny+iy] = 0;
          }
    if (users_xmin==users_xmax) {
      /* auto-scale in x */
      find_min_max(&xmin, &xmax, xdata[j], rows[j]);
      if (xmin==xmax) {
        if (!minimumScale)
          SDDS_Bomb("can't auto-scale in x--no spread to data.  Try using the -minimumScale option.");
        xmin -= minimumScale/2;
        xmax += minimumScale/2;
      }
      hrange = 1.0001*(xmax-xmin)/2.;
      middle = (xmax+xmin)/2.;
      xmax = middle+hrange;
      xmin = middle-hrange;
    }
    else {
      xmin = users_xmin;
      xmax = users_xmax;
    }
    dx = (xmax-xmin)/(nx-1);
    
    if (users_ymin==users_ymax) {
      /* auto-scale in y */
      find_min_max(&ymin, &ymax, ydata[j], rows[j]);
      if (ymin==ymax) {
        if (!minimumScale)
          SDDS_Bomb("can't auto-scale in y--no spread to data.  Try using the -minimumScale option.");
        ymin -= minimumScale/2;
        ymax += minimumScale/2;
      }
      hrange = 1.0001*(ymax-ymin)/2.;
      middle = (ymax+ymin)/2.;
      ymax = middle+hrange;
      ymin = middle-hrange;
    }
    else {
      ymin = users_ymin;
      ymax = users_ymax;
    }
    dy = (ymax-ymin)/(ny-1);
    
    if (zname) {
      if (users_zmin==users_zmax) {
        /* auto-scale in y */
        find_min_max(&zmin, &zmax, zdata[j], rows[j]);
        if (zmin==zmax) {
          if (!minimumScale)
            SDDS_Bomb("can't auto-scale in y--no spread to data.  Try using the -minimumScale option.");
          zmin -= minimumScale/2;
          zmax += minimumScale/2;
        }
        hrange = 1.0001*(zmax-zmin)/2.;
        middle = (zmax+zmin)/2.;
        zmax = middle+hrange;
        zmin = middle-hrange;
      }
      else {
        zmin = users_zmin;
        zmax = users_zmax;
      }
      dz = (zmax-zmin)/(nz-1);
    }
    for (i=n_binned=0; i<rows[j]; i++) {
      x = xdata[j][i];
      y = ydata[j][i];
      if (x<xmin || x>=xmax || y<ymin || y>=ymax) 
        continue;
      iz = 0;
      if (zname) {
        z = zdata[j][i];
        if (z<zmin || z>=zmax)
          continue;
        iz = (z-zmin)/dz + 0.5;
      }
      ix = (x-xmin)/dx + 0.5;
      iy = (y-ymin)/dy + 0.5;
      
      histogram[iz][ix*ny+iy] += (weight?weight[j][i]:1);
      count[iz][ix*ny+iy] ++;
      n_binned++;
    }
    if (verbose)
      fprintf(stderr, "page %ld: %ld of %ld points binned\n", j, n_binned, rows[j]);
    free(xdata[j]);
    free(ydata[j]);
    if (zname)
      free(zdata[j]);
    if (weight)
      free(weight[j]);
    
    if (combine && j!=n_pages-1)
      continue;
    
    if (weights_average) {
      for (iz=0; iz<nz; iz++)
        for (ix=0; ix<nx; ix++)
          for (iy=0; iy<ny; iy++)
            if (count[iz][ix*ny+iy])
              histogram[iz][ix*ny+iy] /= count[iz][ix*ny+iy];
    }
    
    if (n_smooth_passes) {
      double *new_hist, *tmp, sum;
      long jx, jy, nsum, ip, dix;
      
      new_hist = tmalloc(sizeof(*new_hist)*nx*ny);
      dix = one_page_per_line?0:1; 
      for (iz=0; iz<nz; iz++) {
        for (ip=0; ip<n_smooth_passes; ip++) { 
          for (ix=0; ix<nx; ix++) { 
            for (iy=0; iy<ny; iy++) {
              sum = nsum = 0;
              for (jx=ix-dix; jx<=ix+dix; jx++) {
                if (jx<0 || jx>=nx)
                  continue;
                for (jy=iy-1; jy<=iy+1; jy++) {
                  if (jy<0 || jy>=ny)
                    continue;
                  sum += histogram[iz][jx*ny+jy];
                  nsum++;
                }
              }
              if (nsum)
                new_hist[ix*ny+iy] = sum/nsum;
              else
                new_hist[ix*ny+iy] = histogram[iz][ix*ny+iy];
            }
          }
          tmp = histogram[iz];
          histogram[iz] = new_hist;
          new_hist = tmp;
        }
      }
      free(new_hist);
    }
    if (do_normalize) {
      double max_count = 0;
      for (iz=0; iz<nz; iz++) {
        max_count = 0;
        if (do_normalize==1) 
          /* peak normalization */
          for (ix=0; ix<nx; ix++) {
            for (iy=0; iy<ny; iy++) {
              if (histogram[iz][ix*ny+iy]>max_count)
                max_count = histogram[iz][ix*ny+iy];
            }
          }
        else
          /* sum normalization */
          for (ix=0; ix<nx; ix++) {
            for (iy=0; iy<ny; iy++) {
              max_count += histogram[iz][ix*ny+iy];
            }
          }
        if (!max_count)
          SDDS_Bomb("can't normalize histogram--no points histogrammed");
        for (ix=0; ix<nx; ix++) {
          for (iy=0; iy<ny; iy++) {
            histogram[iz][ix*ny+iy] /= max_count;
          }
        }
      }
    }
    for (iz=0; iz<nz; iz++) {
      if (!SDDS_StartPage(&SDDS_out, nx*ny) ||
          !set_sddscontour_parameters(&SDDS_out, xname, nx, xmin, dx) ||
          !set_sddscontour_parameters(&SDDS_out, yname, ny, ymin, dy) ||
          !SDDS_SetColumn(&SDDS_out, SDDS_SET_BY_NAME, histogram[iz], nx*ny, output_name))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      if (zname) {
        if (iz==0)
          z_center = zmin + dz/2.0;
        else
          z_center += dz;
        if (!set_sddscontour_parameters(&SDDS_out, zname, nz, zmin, dz) ||
            !SDDS_SetParameters(&SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "zCenter", z_center, NULL))
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
      if (copyParameters)
        paramContext = setParameters(paramContext, &SDDS_out);
      if (!SDDS_WritePage(&SDDS_out))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
  }
  if (!SDDS_Terminate(&SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  for (iz=0; iz<nz; iz++) {
    free(histogram[iz]);
    free(count[iz]);
  }
  if (xdata) free(xdata);
  if (ydata) free(ydata);
  if (zdata) free(zdata);
  free(rows);
  if (weight) free(weight);
  if (output_units) free(output_units);
  
  free(histogram);
  free(count);
  free_scanargs(&scanned,argc);
  return(0);
}

long define_sddscontour_parameters(SDDS_TABLE *output, SDDS_TABLE *input, char *varname, char *rootname)
{
  char name[SDDS_MAXLINE];
  int32_t double_type = SDDS_DOUBLE;
  if (!rootname) {
    rootname = "Page";
    if (SDDS_DefineParameter(output, varname, NULL, NULL, NULL, NULL, SDDS_STRING, rootname)<0) 
      return 0;
    sprintf(name, "%sDimension", rootname);
    if (SDDS_DefineParameter(output, name, NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0)
      return 0;
    sprintf(name, "%sInterval", rootname);
    if (SDDS_DefineParameter(output, name, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0)
      return 0;
    sprintf(name, "%sMinimum", rootname);
    if (SDDS_DefineParameter(output, name, NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0)<0)
      return 0;
    return 1;
  }
  if (SDDS_DefineParameter(output, varname, NULL, NULL, NULL, NULL, SDDS_STRING, rootname)<0) 
    return 0;
  sprintf(name, "%sDimension", rootname);
  if (SDDS_DefineParameter(output, name, NULL, NULL, NULL, NULL, SDDS_LONG, 0)<0)
    return 0;
  sprintf(name, "%sInterval", rootname);
  if (SDDS_DefineParameterLikeColumn(output, input, rootname, name)<0)
    return 0;
  if(!SDDS_ChangeParameterInformation(output,"type",&double_type,0,name) )
    return 0;
  sprintf(name, "%sMinimum", rootname);
  if (SDDS_DefineParameterLikeColumn(output, input, rootname, name)<0)
    return 0;
  if(!SDDS_ChangeParameterInformation(output,"type",&double_type,0,name) )
      return 0;
  return 1;
}

long set_sddscontour_parameters(SDDS_TABLE *output, char *rootname, long dimen, double min, double delta)
{
  char name[SDDS_MAXLINE];
  if (!rootname)
    rootname = "Page";
  sprintf(name, "%sDimension", rootname);
  if (!SDDS_SetParameters(output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, name, dimen, NULL))
    return 0;
  sprintf(name, "%sMinimum", rootname);
  if (!SDDS_SetParameters(output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, name, min, NULL))
    return 0;
  sprintf(name, "%sInterval", rootname);
  if (!SDDS_SetParameters(output, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, name, delta, NULL))
    return 0;
  return 1;
}

long find2d_min_max(double *min, double *max, double **data, long *rows, long sets)
{
  long i;
  double min1, max1;
  *min = DBL_MAX;
  *max = -DBL_MAX;
  for (i=0; i<sets; i++) {
    find_min_max(&min1, &max1, data[i], rows[i]);
    if (*min>min1)
      *min = min1;
    if (*max<max1)
      *max = max1;
  }
  return 0;
}

static int32_t nParameters = -1;
static char **parameterName = NULL;

PARAM_STORE *saveParameters(PARAM_STORE *context, SDDS_DATASET *SDDSin)
{
  long i;
  
  if (nParameters==-1 && !(parameterName = SDDS_GetParameterNames(SDDSin, &nParameters)))
    SDDS_Bomb("problem getting parameter names");
  if (nParameters==0)
    return NULL;

  if (context->filled==1) {
    context->next = tmalloc(sizeof(PARAM_STORE));
    context->next->filled = 0;
    context->next->next = NULL;
    context->next->param = NULL;
    context = context->next;
  }
  
  context->param = tmalloc(sizeof(*(context->param))*nParameters);

  for (i=0; i<nParameters; i++)
    if (!(context->param[i] = SDDS_GetParameterByIndex(SDDSin, i, NULL)))
      SDDS_Bomb("Error storing parameters");
  
  context->filled = 1;

  return context;
}


PARAM_STORE *setParameters(PARAM_STORE *context, SDDS_DATASET *SDDSout)
{
  long i;
  
  if (nParameters==0)
    return NULL;

  for (i=0; i<nParameters; i++)
    if (!SDDS_SetParameters(SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,
                           parameterName[i], context->param[i], NULL))
      SDDS_Bomb("Error setting parameters");
  
  context = context->next;
  return context;
}

