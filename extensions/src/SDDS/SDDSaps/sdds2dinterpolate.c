/******************************************************************************
 * File:           nnbathy.c
 *
 * Created:        04/08/2000
 *
 * Author:         Pavel Sakov
 *                 CSIRO Marine Research
 *
 * Purpose:        Interpolate scalar 2D data in specified points using
 *                 Natural Neighbours interpolation.
 *
 * Description:    See usage().
 *
 *                 The default version of nnbathy allocates the whole output
 *                 grid in memory before interpolating; the compilier flag
 *                 NN_SERIAL compiles a bit more sophisticated version of that
 *                 interpolates in output points on one-by-one basis.
 *
 * Revisions:      29/05/2006: introduced NN_SERIAL, see the description above
 *                 01/06/2006: moved flags and other command-line input into
 *                             structure "specs"
 *
 *****************************************************************************/
/* sdds2dinterpolate.c   H. Shang  3/20/2007 
   added sdds input and output so that the output can be plotted by sddscontour
   
   $Log: sdds2dinterpolate.c,v $
   Revision 1.6  2008/03/11 21:44:20  shang
   corrected the error message for -independent option

   Revision 1.5  2008/02/26 19:48:06  soliday
   Updated to get sdds2dinterpolate to compile on WIN32

   Revision 1.4  2008/01/17 17:20:37  shang
   corrected the usage message.

   Revision 1.3  2008/01/17 16:06:55  shang
   set weight=0 by default to turn off the extrapolation; To turn on the extrapolation, provide weight option by -weight=inf.

   Revision 1.2  2007/10/22 20:56:57  shang
   fixed bug in scanItemList of independentcolumn option

   Revision 1.1  2007/05/04 14:48:54  shang
   first version of sdds 2d interpolation.

   
   */

#include "SDDS.h"
#include "mdb.h"
#include "scan.h"
#include "nn_2d_interpolate.h"
#include "csa.h"

#if defined(_WIN32)
#define isnan(x) _isnan(x)
#endif

#define CLO_PIPE 0
#define CLO_INDEPENDENT_COLUMN 1
#define CLO_DEPENDENT_COLUMN 2
#define CLO_SCALE 3
#define CLO_OUT_DIMENSION 4
#define CLO_RANGE 5
#define CLO_ZOOM 6
#define CLO_DIMENSION_THIN 7
#define CLO_CLUSTER_THIN 8
#define CLO_PREPROCESS 9
#define CLO_ALGORITHM 10
#define CLO_WEIGHT 11
#define CLO_VERTEX 12
#define CLO_NPOINTS 13
#define CLO_VERBOSE 14
#define CLO_MERGE 15
#define CLO_FILE 16
#define N_OPTIONS 17

char *option[N_OPTIONS]={
  "pipe", "independentColumn", "dependentColumn", "scale", 
  "outDimension", "range", "zoom", "dimensionThin", "clusterThin",
  "preprocess", "algorithm", "weight", "vertex", "npoints", "verbose", "merge", "file"};

static char *USAGE1="sdds2dinterpolate <input> <output> \n\
[-pipe=[input][,output]]\n\
[-independentColumn=xcolumn=<string>,ycolumn=<string>[,errorColumn=<string>]] [-dependentColumn=<z column name>] \n\
[-scale=circle|square] [-outDimension=xdimension=<nx>,ydimension<ny>] \n\
[-range=xminimum=<value>,xmaximum=<value>,yminimum=<value>,ymaximum=<value>] \n\
[-zoom=<value>] [-dimensionThin=xdimension=<nx>,ydimension=<ny>] \n\
[-clusterThin=<value>] [-preprocess] [-algorithm=<string>,linear|sibson|nonsibson[,average=<value>][,sensitivity=<value>]] \n\
[-weight=<value>] [-vertex=<id>] [-npoints=<integer>] [-verbose] [-merge] \n";
static char *USAGE2="-indepentdentColumn provide the independent column names of <xcol> (default is x) \n\
                    <ycol> (default is y) and standard error column (default is stdError)\n\
                    standard error column is only used for csa algorithm).\n\
-dependendColumn    provided the column name to be interpolated, default is z.\n\
-scale          if scale=square, then scale internally so that Xmax - Xmin = Ymax - Ymin.\n\
                else if scale=circle, then scale internally so that the enclosing minimal ellipse \n\
                turns into a circle (this produces results invariant to affine transformations)\n\
-range          set the Xmin, Xmax, Ymin and Ymax for the output grid.\n\
-zoom           zoom in (if <zoom> < 1) or out (<zoom> > 1) (activated)\n\
                only when used in conjunction with -nDimension.\n\
-outDimension   generate <nx>x<ny> output rectangular grid.\n\
-file           provided the output grid, either outDimension or file has to be provided.\n\
-dimensionThin  thin input data by averaging X, Y and Z values within \n\
                every cell of the rectangular <nx>x<ny> grid size.\n\
                optional with -outDimension.\n\
-clusterThin    thin input data by averaging X, Y and Z values within \n\
                clusters of consequitive input points such that the sum of distances \n\
                between points within each cluster does not exceed the specified maximum value.\n";
static char *USAGE3="-algorithm      provide algorithm: \n\
                nn -- natural neighours interpolation, it has 3 three following options: \n\
                      linear    -- linear interpolation \n\
                      Sibson    -- Sibson interpolation (natural neighbour) (default) \n\
                      nonSibson -- non-Sibsonian interpolation.\n\
                csa -- 2D data approximation with bivariate C1 cubic spline.\n\
                      csa stands for cubic spline approximation. For uniformly distributed data\n\
                      this algorithm is clearly superior than natural neighbours interpolation: it is \n\
                      faster (O(n)), more robust (approximation, not interpolation) and better \n\
                      performing(C1).\n\
                      csa has other two options: \n\
                      average  -- the average number of points per cell (default=5) \n\
                      sensitivity -- the spline sensitivity (default = 140, reduce to get smoother results)\n\
-weight         restricts extrapolation by assigning minimal allowed weight for a vertex \n\
                (normally -1 or so; lower values correspond to lower reliability, 0 means \n\
                no extrapolation). If infinity is provided, then weight=-DBL_MAX; the default weight is 0.\n\
-vertex         verbose; in weigths output print weights associated with this vertex only.\n\
-verbose        print message during processing.\n";
static char *USAGE4="-merge          merge the data of all pages from the input file into one and then perform interpolation, \n\
                and the output file will have only one page if output points file is not provided.\n\
                However, if the output points file is provided, the number of output pages is the same as \n\
                the output points file (provided by -file option).\n\
                otherwise, each page is performed independently and the output file will have \n\
                the number of pages that have non-zero rows in the input files.\n\
-npoints        total number of points to done with -outDimension option for NN_SERIAL.\n\n\
converted from nnbathy.c by Hairong Shang (April 2007) for interpolating scalar 2D data in specified points \n\
using Natural Neighbours interpolation. The interpolated values are writtend to SDDS file.\n";

typedef struct {
double xmin, xmax, ymin, ymax, dx, dy;
int nx, ny, nout;
point *pout;
} OUT_POINT;


#define ALGORITHMS 2
#define LINEAR_NN 0x0001UL
#define SIBSON_NN 0x0002UL
#define NONSIBSON_NN 0x0004UL

char *algorithm_option[ALGORITHMS]={"nn", "csa"};

#define SCALE_OPTIONS 2
char *scale_option[SCALE_OPTIONS]={"circle", "square"};

void ReadInputFile(char *inputFile, char *xCol, char *yCol, char *zCol, char *stdCol,
                   long *pages, long **rows, double ***x, double ***y, double ***z, double ***std);
void SetupOutputFile(SDDS_DATASET *SDDS_out, char *outputFile, char *xCol, char *yCol, char *zCol);
void WriteOutputPage(SDDS_DATASET *SDDS_out, char *xCol, char *yCol, char *zCol, specs *spec, int nout, point *pout);
void ReadPointFile(char *pointsFile, char *xCol, char *yCol, long *point_pages, OUT_POINT **out_point);
int interpolate_output_points(int nin, point *pin, double *std, char *xCol, char *yCol, char *zCol,
                              specs *spec, long pages, OUT_POINT *out_point, SDDS_DATASET *SDDS_out);

static char *INFINITY_OPTION[1]={"infinity"};

int main(int argc, char* argv[])
{
  SDDS_DATASET SDDS_out, SDDS_points;
  char *inputFile, *outputFile, *xCol="x", *yCol="y", *zCol="z", *stdCol="StdError", *pointsFile=NULL;
  long i, j, i_arg, algorithm, scale, *rows, pages, merge=0, point_pages, page;
  unsigned long dummyFlags=0, pipeFlags=0;
  SCANNED_ARG *s_arg;
  double **x=NULL, **y=NULL, **z=NULL, xmin, xmax, ymin, ymax, **std=NULL, *std_all = NULL;
  specs* spec = specs_create();
  int nin = 0;
  point* pin = NULL;
  int nout = 0;
  point* pout = NULL;
  OUT_POINT *out_point = NULL;
  
  /*turn off the extrapolation by default*/
  spec->wmin = 0;
  inputFile = outputFile = NULL;
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) { 
    fprintf(stderr, "%s%s%s%s", USAGE1, USAGE2, USAGE3, USAGE4);
    return(1);
  }
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case CLO_FILE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -file syntax.");
        pointsFile = s_arg[i_arg].list[1];
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_INDEPENDENT_COLUMN:
        if (s_arg[i_arg].n_items<3)
          SDDS_Bomb("Invalid -independentColumn syntax.");
        s_arg[i_arg].n_items--;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "xcolumn", SDDS_STRING, &xCol, 1, 0,
                          "ycolumn", SDDS_STRING, &yCol, 1, 0,
                          "errorcolumn", SDDS_STRING, &stdCol, 1, 0, 
                          NULL))
          SDDS_Bomb("invalid -independent syntax");
        s_arg[i_arg].n_items ++; 
        break;
      case CLO_DEPENDENT_COLUMN:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -dependentColumn syntax.");
        zCol=s_arg[i_arg].list[1];
        break;
      case CLO_SCALE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -scale syntax.");
        if ((scale=match_string(s_arg[i_arg].list[1], scale_option, SCALE_OPTIONS, 0))==-1) {
          fprintf(stderr, "Invalide scale option - %s provided.\n", s_arg[i_arg].list[1]);
          exit(1);
        } 
        spec->square = !scale;
        spec->invariant = scale;
        break;
      case CLO_OUT_DIMENSION:
        if (s_arg[i_arg].n_items!=3)
          SDDS_Bomb("Invalid -outDimension syntax.");
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "xdimension", SDDS_LONG, &spec->nx, 1, 0,
                          "ydimension", SDDS_LONG, &spec->ny, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -outDimension syntax");
        s_arg[i_arg].n_items ++;
        if (spec->nx <= 0 || spec->nx > NMAX || spec->ny <= 0 || spec->ny > NMAX)
          SDDS_Bomb("Invalide size for output grid.");
        spec->generate_points = 1;
        break;
      case CLO_RANGE:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("Invalid -range syntax.");
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "xminimum", SDDS_DOUBLE, &spec->xmin, 1, 0,
                          "xmaximum", SDDS_DOUBLE, &spec->xmax, 1, 0,
                          "yminimum", SDDS_DOUBLE, &spec->ymin, 1, 0,
                          "ymaximum", SDDS_DOUBLE, &spec->ymax, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -range syntax");
        s_arg[i_arg].n_items ++;
        if (spec->xmin>spec->xmax || spec->ymin>spec->ymax || 
            isnan(spec->xmin) || isnan(spec->xmax) || isnan(spec->ymin) || isnan(spec->ymax))
          SDDS_Bomb("Invalide -range provided.");
        spec->range = 1;
        break;
      case CLO_ZOOM:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -zoom syntax.");
        if (!get_double(&spec->zoom, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalide -zoom value provided.");
        break;
      case CLO_DIMENSION_THIN:
        if (s_arg[i_arg].n_items!=3)
          SDDS_Bomb("Invalid -dimensionThin syntax.");
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "xdimension", SDDS_LONG, &spec->nxd, 1, 0,
                          "ydimension", SDDS_LONG, &spec->nyd, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -outDimension syntax");
        s_arg[i_arg].n_items ++;
        spec->thin = 1; 
        break;
      case CLO_CLUSTER_THIN:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -clusterThin syntax.");
        if (!get_double(&spec->rmax, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalide clusterThin value provided.");
        spec->thin = 2;
        break;
      case CLO_PREPROCESS:
        spec->nointerp = 1;
        break;
      case CLO_ALGORITHM:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("Invalide -algorithm syntax!");
        if ((spec->method=match_string(s_arg[i_arg].list[1],  algorithm_option, ALGORITHMS, 0))==-1) {
          fprintf(stderr, "Invalid algorithm - %s provided, has to be nn or csa.\n", s_arg[i_arg].list[1]);
          exit(1);
        }
        dummyFlags = 0;
        s_arg[i_arg].n_items -=2;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
                          "linear", -1, NULL, 0, LINEAR_NN,
                          "sibson", -1, NULL, 0, SIBSON_NN,
                          "nonSibson", -1, NULL, 0, NONSIBSON_NN,
                          "average", SDDS_LONG, &spec->nppc, 1, 0,
                          "sensitivity", SDDS_DOUBLE, &spec->k, 1, 0, NULL))
          SDDS_Bomb("Invalud -algorithm syntax!");
        spec->linear = 0;
        if (!dummyFlags || dummyFlags&SIBSON_NN) {
          nn_rule = SIBSON;
        } else if (dummyFlags&NONSIBSON_NN) {
          nn_rule = NON_SIBSONIAN;
        } else if (dummyFlags&LINEAR_NN) {
          spec->linear = 1;
        }
        s_arg[i_arg].n_items +=2;
        break;
      case CLO_WEIGHT:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -weight syntax.");
        if (match_string(s_arg[i_arg].list[1], INFINITY_OPTION, 1, 0)==0) 
          spec->wmin = -DBL_MAX; 
        else {
          if (!get_double(&spec->wmin, s_arg[i_arg].list[1]))
            SDDS_Bomb("Invalide weight value provided.");
        }
        break;
      case CLO_VERTEX:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -vertex syntax.");
        if (!get_long(&nn_test_vertice, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalide weight value provided.");
        nn_verbose = 1;
        break;
      case CLO_NPOINTS:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -npoints syntax.");
        if (!get_long(&spec->npoints, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalide npoints value provided.");
        break;
      case CLO_VERBOSE:
        nn_verbose = 2;
        break;
      case CLO_MERGE:
        merge = 1;
        break;
      default:
        fprintf(stderr, "Unknown option - %s provided.\n", s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    } else {
      if (!inputFile)
        inputFile = s_arg[i_arg].list[0];
      else if (!outputFile)
        outputFile = s_arg[i_arg].list[0];
      else
        SDDS_Bomb("Too many files given!");
    }
  }
  
  processFilenames("sddsnnbathy", &inputFile, &outputFile, pipeFlags, 0, NULL); 
  if (!spec->generate_points && !spec->nointerp && !pointsFile) {
    fprintf(stderr, "No output grid specified.\n");
    exit(1);
  }
  if (spec->thin) {
     if (spec->nxd == -1)
       spec->nxd = spec->nx;
     if (spec->nyd == -1)
       spec->nyd = spec->ny;
     if (spec->nxd <= 0 || spec->nyd <= 0) {
       fprintf(stderr, "invalid grid size for thinning\n");
       exit(1);
     }
  }
  if (spec->npoints == 1) {
    if (spec->nx <= 0)
      spec->npoints = 0;
    else
      spec->npoints = spec->nx * spec->ny;
  }
  ReadInputFile(inputFile, xCol, yCol, zCol, stdCol, &pages, &rows, &x, &y, &z, &std);
  if (!spec->nointerp)
    SetupOutputFile(&SDDS_out, outputFile, xCol, yCol, zCol); 
  if (pointsFile)
    ReadPointFile(pointsFile, xCol, yCol, &point_pages, &out_point);
  if (merge) {
    for (i=0; i<pages; i++) {
      if (!i) {
        xmin = xmax = x[0][0];
        ymin = ymax = y[0][0];
      }
      pin = SDDS_Realloc(pin, sizeof(*pin)*(nin+rows[i]));
      if (std)
        std_all = SDDS_Realloc(std_all, sizeof(*std_all)*(nin+rows[i]));
      
      for (j=0; j<rows[i]; j++) {
        if (x[i][j]<xmin)
          xmin = x[i][j];
        else if (x[i][j]>xmax)
          xmax = x[i][j];
        if (y[i][j]<ymin)
          ymin = y[i][j];
        else if (y[i][j]>ymax)
          ymax = y[i][j];
        pin[nin+j].x = x[i][j];
        pin[nin+j].y = y[i][j];
        pin[nin+j].z = z[i][j];
        if (std)
          std_all[nin+j] = std[i][j];
      }
      nin += rows[i];
      free(x[i]);
      free(y[i]);
      free(z[i]);
      if (std && std[i]) {
        free(std[i]);
        std[i] = NULL;
      }
      x[i]=y[i]=z[i] = NULL;
    }
    free(x);
    free(y);
    free(z);
    if (std) free(std);
    x=y=z=NULL;
    if (!spec->range) {
      spec->xmin = xmin;
      spec->ymin = ymin;
      spec->xmax = xmax;
      spec->ymax = ymax;
    }
    if (pointsFile && !interpolate_output_points(nin, pin, std_all, xCol, yCol, zCol, spec, pages, out_point, &SDDS_out)) {
      free(pin);
      exit(1);
    } else {
      if (spec->method==NN) {
        do_nn_2d_interpolate(spec, &nin, &pin, &nout, &pout);
      } else if (spec->method==CSA) {
        do_csa_2d_interpolate(spec, nin, pin, &nout, &pout, std_all);
        
      }
      if (!spec->nointerp)
        WriteOutputPage(&SDDS_out, xCol, yCol, zCol, spec, nout, pout);
      free(pout);
    }
    if (std_all)
      free(std_all);
    free(pin);
    pin = pout = NULL;
  } else {
    for (i=0; i<pages; i++) {
      xmin = xmax = x[i][0];
      ymin = ymax = y[i][0];
      nin = rows[i];
      pin = malloc(sizeof(*pin)*nin);
      for (j=0; j<rows[i]; j++) {
        pin[j].x = x[i][j];
        pin[j].y = y[i][j];
        pin[j].z = z[i][j];
        if (x[i][j]<xmin)
          xmin = x[i][j];
        else if (x[i][j]>xmax)
          xmax = x[i][j];
        if (y[i][j]<ymin)
          ymin = y[i][j];
        else if (y[i][j]>ymax)
          ymax = y[i][j];
      }
      if (!spec->range) {
        spec->xmin = xmin;
        spec->ymin = ymin;
        spec->xmax = xmax;
        spec->ymax = ymax;
      }
      free(x[i]);
      free(y[i]);
      free(z[i]);
      x[i] = y[i] = z[i] = NULL;
      if (std && std[i])
        std_all = std[i];
      else 
        std_all = NULL;
      if (pointsFile) {
        if (!interpolate_output_points(nin, pin, std_all, xCol, yCol, zCol,
                                       spec, point_pages, out_point, &SDDS_out)) {
          free(pin);
          exit(1);
        }
      } else {
        if (spec->method==NN) {
          do_nn_2d_interpolate(spec, &nin, &pin, &nout, &pout);
        } else if (spec->method==CSA) {
          do_csa_2d_interpolate(spec, nin, pin, &nout, &pout, std_all);
        }
        if (!spec->nointerp)
          WriteOutputPage(&SDDS_out, xCol, yCol, zCol, spec, nout, pout);
        free(pout);
        pout = NULL;
      }
      free(pin);
      pin = NULL;
      if (std && std[i]) {
        free(std[i]);
        std[i]=NULL;
      }
    }
    free(x);
    free(y);
    free(z);
    x = y = z = NULL;
    if (std) free(std);
  }
  if (!spec->nointerp && !SDDS_Terminate(&SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free(spec);
  free(rows);
  free_scanargs(&s_arg, argc);
  return 0;
}

void ReadInputFile(char *inputFile, char *xCol, char *yCol, char *zCol, char *stdCol,
                   long *pages, long **rows, double ***x, double ***y, double ***z, double ***std)
{
  SDDS_DATASET SDDS_in;
  int32_t rows1=0;
  long pages0=0, std_exist=0;
 
  *rows = NULL;
  if (!SDDS_InitializeInput(&SDDS_in, inputFile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (SDDS_CheckColumn(&SDDS_in, xCol, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
    fprintf(stderr,"x column - %s does not exist!\n", xCol);
    exit(1);
  }
  if (SDDS_CheckColumn(&SDDS_in, yCol, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
    fprintf(stderr,"y column - %s does not exist!\n", yCol);
    exit(1);
  }
  if (SDDS_CheckColumn(&SDDS_in, zCol, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
    fprintf(stderr,"z column - %s does not exist!\n", zCol);
    exit(1);
  }
  if (SDDS_CheckColumn(&SDDS_in, stdCol, NULL, SDDS_ANY_NUMERIC_TYPE, NULL)==SDDS_CHECK_OK)
    std_exist = 1;
  
  while (SDDS_ReadPage(&SDDS_in)>0) {
    rows1 = 0;
    rows1=SDDS_CountRowsOfInterest(&SDDS_in);
    if (!rows1)
      continue;
    *rows = SDDS_Realloc(*rows, sizeof(**rows)*(pages0+1));
    (*rows)[pages0] = rows1;
    if (!(*x = SDDS_Realloc(*x, sizeof(**x)*(pages0+1))) ||
        !(*y = SDDS_Realloc(*y, sizeof(**y)*(pages0+1))) ||
        !(*z = SDDS_Realloc(*z, sizeof(**z)*(pages0+1)))) {
      fprintf(stderr, "Memory allocation error.\n");
      exit(1);
    }
    if (std_exist) {
      if (!(*std = SDDS_Realloc(*std, sizeof(**std)*(pages0+1)))) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
      }
      (*std)[pages0] = NULL;
    }
    
    (*x)[pages0] = (*y)[pages0] = (*z)[pages0] = NULL;
    if (!((*x)[pages0]=(double*)SDDS_GetColumnInDoubles(&SDDS_in, xCol)) ||
        !((*y)[pages0]=(double*)SDDS_GetColumnInDoubles(&SDDS_in, yCol)) ||
        !((*z)[pages0]=(double*)SDDS_GetColumnInDoubles(&SDDS_in, zCol)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (std_exist && !((*std)[pages0]=(double*)SDDS_GetColumnInDoubles(&SDDS_in, stdCol)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    pages0 ++;
  }
  if (!SDDS_Terminate(&SDDS_in))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!pages0) {
    fprintf(stderr, "No data found in the input file.\n");
    exit(1);
  }
  *pages = pages0;
  return;
}

void SetupOutputFile(SDDS_DATASET *SDDS_out, char *outputFile, char *xCol, char *yCol, char *zCol) 
{
  char tmpstr[256];
  
  if (!SDDS_InitializeOutput(SDDS_out, SDDS_BINARY,1, NULL, NULL, outputFile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_DefineSimpleParameter(SDDS_out, "Variable1Name", NULL, SDDS_STRING) ||
      !SDDS_DefineSimpleParameter(SDDS_out, "Variable2Name", NULL, SDDS_STRING) ||
      !SDDS_DefineSimpleColumn(SDDS_out, xCol, NULL, SDDS_DOUBLE) ||
      !SDDS_DefineSimpleColumn(SDDS_out, yCol, NULL, SDDS_DOUBLE) ||
      !SDDS_DefineSimpleColumn(SDDS_out, zCol, NULL, SDDS_DOUBLE)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sDimension", xCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_LONG))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sDimension", yCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_LONG))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sInterval", xCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sInterval", yCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMinimum", xCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMinimum", yCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMaximum", xCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMaximum", yCol);
  if (!SDDS_DefineSimpleParameter(SDDS_out, tmpstr, NULL, SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_WriteLayout(SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}

void WriteOutputPage(SDDS_DATASET *SDDS_out, char *xCol, char *yCol, char *zCol, specs *spec, int nout, point *pout)
{
  char tmpstr[256];
  long i;
  
  if (!SDDS_StartPage(SDDS_out, nout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         "Variable1Name", yCol, "Variable2Name", xCol, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sDimension", xCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->nx, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors); 
  sprintf(tmpstr, "%sDimension", yCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->ny, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sInterval", xCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->dx, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sInterval", yCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->dy, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMinimum", xCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->xmin, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMinimum", yCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->ymin, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMaximum", xCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->xmax, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sprintf(tmpstr, "%sMaximum", yCol);
  if (!SDDS_SetParameters(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                         tmpstr, spec->ymax, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0; i<nout; i++) {
    if (!SDDS_SetRowValues(SDDS_out, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i,
                           xCol, pout[i].x, yCol, pout[i].y,
                           zCol, pout[i].z, NULL))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_WritePage(SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}

void ReadPointFile(char *inputFile, char *xCol, char *yCol,  
                   long *pages, OUT_POINT **out_point)
{
  SDDS_DATASET SDDS_in;
  int32_t rows1=0;
  long pages0=0, std_exist=0;
  double *x, *y, dx, dy, xmin, xmax, firstx, firsty, ymin, ymax;
  int nx, ny, i;
  
  if (!SDDS_InitializeInput(&SDDS_in, inputFile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (SDDS_CheckColumn(&SDDS_in, xCol, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
    fprintf(stderr,"x column - %s does not exist!\n", xCol);
    exit(1);
  }
  if (SDDS_CheckColumn(&SDDS_in, yCol, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) {
    fprintf(stderr,"y column - %s does not exist!\n", yCol);
    exit(1);
  }
  while (SDDS_ReadPage(&SDDS_in)>0) {
    rows1 = 0;
    rows1=SDDS_CountRowsOfInterest(&SDDS_in);
    if (!rows1)
      continue;
    *out_point = SDDS_Realloc(*out_point, sizeof(**out_point)*(pages0+1));
    x = y = NULL;
    (*out_point)[pages0].pout = malloc(sizeof(point)*rows1);
    (*out_point)[pages0].nout = rows1;
    if (!(x=(double*)SDDS_GetColumnInDoubles(&SDDS_in, xCol)) ||
        !(y=(double*)SDDS_GetColumnInDoubles(&SDDS_in, yCol)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    xmin = xmax = firstx = x[0];
    ymin = ymax = firsty = y[0];
    nx = ny = 0;
    
    for (i=0; i<rows1; i++) {
      (*out_point)[pages0].pout[i].x = x[i];
      (*out_point)[pages0].pout[i].y = y[i];
      if (x[i]<xmin)
        xmin = x[i];
      else if (x[i]>xmax)
        xmax = x[i];
      if (x[i]==firstx)
        ny ++;
      if (y[i]<ymin)
        ymin = y[i];
      else if (y[i]>ymax)
        ymax = y[i];
      if (y[i]==firsty)
        nx ++;
    }
    free(x);
    free(y);
    (*out_point)[pages0].xmin = xmin;
    (*out_point)[pages0].xmax = xmax;
    (*out_point)[pages0].ymin = ymin;
    (*out_point)[pages0].ymax = ymax;
    (*out_point)[pages0].nx = nx;
    (*out_point)[pages0].ny = ny;
    if (nx>1)
      (*out_point)[pages0].dx = (xmax - xmin)/(nx-1);
    else 
      (*out_point)[pages0].dx = 0;
    if (ny>1)
      (*out_point)[pages0].dx = (ymax - ymin)/(ny-1);
    else
      (*out_point)[pages0].dy = 0;
    pages0 ++;
  }
  if (!SDDS_Terminate(&SDDS_in))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!pages0) {
    fprintf(stderr, "No data found in the points file.\n");
    exit(1);
  }
  *pages = pages0;
  return;
}

int interpolate_output_points(int nin, point *pin, double *std, char *xCol, char *yCol, char *zCol,
                              specs *spec, long pages, OUT_POINT *out_point, SDDS_DATASET *SDDS_out)
{
  int nout, page;
  point *pout=NULL;
  
  for (page=0; page<pages; page++) {
    nout = out_point[page].nout;
    pout = out_point[page].pout;
    spec->nx = out_point[page].nx;
    spec->ny = out_point[page].ny;
    spec->xmin = out_point[page].xmin;
    spec->xmax = out_point[page].xmax;
    spec->dx = out_point[page].dx;
    spec->ymin = out_point[page].ymin;
    spec->ymax = out_point[page].ymax;
    spec->dy = out_point[page].dy;
    if (spec->method==NN) {
      do_nn_2d_interpolate(spec, &nin, &pin, &nout, &pout); 
    } else 
      do_csa_2d_interpolate(spec, nin, pin, &nout, &pout, std);
    if (!spec->nointerp)
      WriteOutputPage(SDDS_out, xCol, yCol, zCol, spec, nout, pout);
    free(pout);
  }
  free(out_point);
  out_point = NULL;
  return 1;
}
