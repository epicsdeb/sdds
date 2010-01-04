/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

  /* program: sddscontour
   * purpose: do contour plotting of SDDS data
   *
   * Michael Borland, 1993
   $Log: sddscontour.c,v $
   Revision 1.63  2009/04/30 21:14:30  shang
   now it is able to get the units of varible1 and variable2 from the parameeters and plot them along with the variable1 and variable2 names as the x/y labels

   Revision 1.62  2009/04/30 15:08:37  shang
   modified to plot no title when "-title=" provided

   Revision 1.61  2008/11/05 16:16:10  shang
   fixed 3 bugs: 1)filename was plotted on the topline  when "-topline=" is given, nothing should be plotted instead since topline is emptystring, fixed by adding checking the string length of topline; 2)the swapxy did not work with -xrange and -yrange for even number plots, fixed by swaping the x/y range parameters after each plot; 3)the scale option did not work properly with -xrange, -yrange and -swapxy option together, fixed by restoring the original scale after each plot.

   Revision 1.60  2008/07/01 19:03:58  shang
   added -yaxis option to plot the y axis label by provided scale and offset for -columnMatch plot

   Revision 1.59  2008/03/19 21:58:10  shang
   changed to use "filename" for ploting the file name to topline and/or title.

   Revision 1.58  2008/03/19 21:04:22  shang
   made the print labels accept non-string parameter and added capability of printing file name or edited file name to the topline or title if request as -topline=file[,edit=<string>] or -title=file[,edit=<string>].

   Revision 1.57  2008/02/18 21:53:59  soliday
   Fixed issue with writing multiple PNG files without breaking PNG output when
   the layout option is set.

   Revision 1.56  2006/10/19 21:48:47  soliday
   Fixed issue with tsetFlags

   Revision 1.55  2006/10/19 17:55:40  soliday
   Updated to work with linux-x86_64.

   Revision 1.54  2006/10/18 15:37:43  shang
   add -nocolorbar option for not plotting colro bar.

   Revision 1.53  2006/10/02 20:28:05  jiaox
   Added strlen check for p2 p3 p4  to checkLabelParameters(). Those are missing in last revision.

   Revision 1.52  2006/10/02 20:00:07  jiaox
   Modified to pre-sort the parameters and columns for -waterfall options to make sure the plot is correct if the dataset are not sorted. However if the parameters/column-values are not evenly spaced the plot may not be appropriate since the axis labels are always evenly spaced. ( Should throw
   a warning if so ?). Also fixed a bug that cause a segmentation fault for png
   output. Hairong fixed another bug that caused "array bounds read" for checkparameters.

   Revision 1.51  2006/09/06 20:00:39  shang
   added -xrange and -yrang option for -columnmatch such that the xlabel and ylabel can be numerically labeled by the provided
   xrange and yrange.

   Revision 1.50  2006/08/21 21:30:10  soliday
   Updated to get it to build on WIN32.

   Revision 1.49  2006/05/22 22:49:44  jiaox
   Added gray keyword to -shade option.

   Revision 1.48  2005/11/07 22:20:55  soliday
   Updated to remove compiler warnings on Linux.

   Revision 1.47  2005/11/04 22:46:39  soliday
   Updated code to be compiled by a 64 bit processor.

   Revision 1.46  2005/09/19 20:43:09  shang
   fixed the problem of plotting multiple layout for columnMatch option when the
   values of y-axis are much less than those of x-axis.

   Revision 1.45  2005/04/26 22:24:36  shang
   made swapxy work for matchColumn option and rpntransform work; added scale feature
   to xlabel and ylabel option to be able to change the x/y label size; added plotting
   enumerated x tick-label and plotting the x/y label with enumerated tick-labels.

   Revision 1.44  2005/04/25 20:56:36  soliday
   Updated to work with the updated make_intensity_bar procedure when using
   less than 100 colors.

   Revision 1.43  2005/03/07 22:51:26  shang
   added the ability to plot the color bar name

   Revision 1.42  2005/01/31 15:21:57  shang
   added -waterfall option to plot multi-page data in one plot

   Revision 1.41  2005/01/13 21:43:33  soliday
   Made the rpn function calls consistent with the updated rpn library.

   Revision 1.40  2004/11/16 19:31:25  soliday
   Added the ability to pipe an SDDS file into sddscontour.

   Revision 1.39  2004/09/14 19:02:01  soliday
   Fixed problem when not using -shade. Updated scan args section so that
   free_scanargs can be used. Added the -ticksettings=xtime option.

   Revision 1.38  2004/08/02 15:24:49  borland
   Can now handle files in which the contents of Variable1Name and Variable2Name
   change from page to page.

   Revision 1.37  2003/08/16 19:38:55  borland
   More fixes for the frame_end() business. Frame ending is only done in the
   main routine now.  Removed special frame_end code for the columnMatch
   mode, since it shouldn't be needed now.

   Revision 1.36  2003/08/16 19:24:22  borland
   Renamed the variables ix and iy in main() so that there is no confusion with
   variables defined inside some if statements.
   Fixed bug in use of frame_end() which resulted in segmentation fault when
   plotting PNG with layout.

   Revision 1.35  2003/06/02 21:37:43  soliday
   Added a function to add parentheses around the string before passing it to if2pf.

   Revision 1.34  2002/12/11 21:23:44  borland
   Now plots shapes for -shade mode.

   Revision 1.33  2002/12/04 17:55:20  soliday
   Fixed the shape option when used with the contour option.

   Revision 1.32  2002/12/03 20:09:08  borland
   Added optional linetype for -shapes option.

   Revision 1.31  2002/12/03 19:05:43  borland
   Fixed problem with contour plotting due to uninitialized values for
   min_level and max_level.

   Revision 1.30  2002/08/14 17:35:28  soliday
   Added Open License

   Revision 1.29  2002/01/10 13:45:01  borland
   Added -thickness option.

   Revision 1.28  2002/01/09 03:37:09  borland
   Added support for device arguments for devices other than motif.

   Revision 1.27  2001/08/29 20:26:34  soliday
   Added the -swaparray option.

   Revision 1.26  2001/08/29 16:32:09  soliday
   Added the ability to plot SDDS arrays.

   Revision 1.25  2001/06/04 20:21:14  soliday
   Added the -layout option.

   Revision 1.24  2001/05/28 22:32:14  borland
   Replacing last changes as they were actually duplicating an existing feature.

   Revision 1.22  2001/04/10 20:42:57  soliday
   Added the ability to convert algebraic equations to rpn equations.

   Revision 1.21  2001/01/10 19:34:34  soliday
   Standardized usage message.

   Revision 1.20  2001/01/09 17:26:59  borland
   Added -mapShade option to sddscontour.

   Revision 1.19  2000/11/13 22:03:51  soliday
   Ticks are now used by default.

   Revision 1.18  2000/09/20 13:05:48  borland
   The fixedRange option now works properly.

   Revision 1.17  2000/06/15 16:53:24  borland
   No longer does ftell() on files unless it has to.  Prevents (in a kludgey
   way) core dumps on .gz input files.

   Revision 1.16  2000/05/11 20:19:29  soliday
   Added extra argument to plot_lines.

   Revision 1.15  2000/04/10 19:01:06  borland
   -shapes option now works again, and uses SDDS files.

   Revision 1.14  1999/09/28 15:49:59  borland
   Added a test to avoid going over the end of a string array.

   Revision 1.13  1999/09/14 18:35:04  soliday
   Added fftpackC.h include statement

   Revision 1.12  1999/07/23 15:53:29  soliday
   Added WIN32 support

   Revision 1.11  1999/06/03 20:42:59  soliday
   Removed compiler warnings under Linux.

   Revision 1.10  1998/10/02 17:31:27  borland
   Added -v1v2preferred option to sddscontour so that it can be told to
   use the Variable1Name and Variable2Name parameters to figure out the
   image rather than NumberOfRows and NumberOfColumns.

   Revision 1.9  1998/08/25 14:16:40  borland
   Added code to flip the x dimension for columnMatch mode if the independent
   data is in decreasing order.

   Revision 1.8  1998/08/14 14:19:34  borland
   Compatible with new version of graphics library (for sddsplot upgrade).
   Removed most references to SDDS structure internals and replaced with calls
   to SDDS library routines.

   Revision 1.7  1997/05/29 16:08:55  borland
   Fixed x11 args parsing problem (conditional compilation statements too
   restrictive).

   * Revision 1.6  1996/03/27  14:46:34  borland
   * Fixed option parsing problem by switching to scanItemList().
 *
 * Revision 1.5  1995/11/13  15:59:37  borland
 * Added long cast to strlen() in comparison to satisfy Solaris compiler.
 *
 * Revision 1.4  1995/10/31  02:40:05  borland
 * Added -ystrings option; it superseeds the -yeditstrings option (which is
 * still there, but not in the usage message).  Added capabilities to the
 * enumerated scale routine.
 *
 * Revision 1.3  1995/10/17  05:59:42  borland
 * Added -editystrings option to sddscontour.
 * Added some set_linetype() calls to shade_grid.c routines because last color
 * of color bar was "sticking" and getting applied to labels; I think this is
 * actually a bug in the X11 routines.
 *
 * Revision 1.2  1995/09/05  21:08:55  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "graph.h"
#include "matlib.h"
#include "contour.h"
#include "rpn.h"
#include "fftpackC.h"
#if defined(_WIN32)
#include <windows.h>
#define sleep(sec) Sleep(sec * 1000)
#else
#include <unistd.h>
#endif

#define SET_QUANTITY 0
#define SET_SWAP_XY 1
#define SET_SHADE 2
#define SET_CONTOURS 3
#define SET_EQUATION 4
#define SET_SCALES 5
#define SET_LABEL_CONTOURS 6
#define SET_DEVICE 7
#define SET_OUTPUT 8
#define SET_INTERPOLATE 9
#define SET_FILTER 10
#define SET_SHAPES 11
#define SET_EQUAL_ASPECT 12
#define SET_XLABEL 13
#define SET_YLABEL 14
#define SET_TITLE 15
#define SET_TOPLINE 16
#define SET_TOPTITLE 17
#define SET_NO_LABELS 18
#define SET_NO_BORDER 19
#define SET_NO_SCALES 20
#define SET_DATE_STAMP 21
#define SET_VERBOSITY 22
#define SET_RPN_DEFNS_FILES 23
#define SET_RPN_EXPRESSIONS 24
#define SET_RPN_TRANSFORM 25
#define SET_FIXED_RANGE 26
#define SET_COLUMNMATCH 27
#define SET_LOGSCALE 28
#define SET_DELTAS 29
#define SET_YSTRINGS 30
#define SET_EDITYSTRINGS 31
#define SET_PREFERV1V2PARAMS 32
#define SET_MAPSHADE 33
#define SET_LAYOUT 34
#define SET_ARRAY 35
#define SET_SWAPARRAY 36
#define SET_THICKNESS 37
#define SET_TICKSETTINGS 38
#define SET_PIPE 39
#define SET_WATERFALL 40
#define SET_YRANGE 41
#define SET_XRANGE 42
#define SET_NO_COLOR_BAR 43
#define SET_YAXIS 44
#define OPTIONS 45



static char *option[OPTIONS] = {
    "quantity", "swapxy", "shade", "contours", "equation", "scales", 
    "labelcontours", "device", "output", "interpolate", "filter", "shapes", 
    "equalaspect", "xlabel", "ylabel", "title", "topline", "toptitle", 
    "nolabels", "noborder", "noscales", "datestamp", "verbosity", 
    "rpndefinitionsfiles", "rpnexpressions", "rpntransform", "fixedrange", 
    "columnmatch", "logscale", "deltas", "ystrings", "yeditstrings",
    "v1v2preferred", "mapshade", "layout", "array", "swaparray",
    "thickness", "ticksettings", "pipe", "waterfall", "yrange", "xrange", "nocolorbar", "yaxis",
  } ;



char *USAGE = "sddscontour [-pipe] [<SDDSfilename>]\n\
 [{-quantity=<column-name> | -equation=<rpn-equation>[,algebraic] |\n\
  -waterfall=parameter=<parameter>,independentColumn=<xColumn>,colorColumn=<colorColumn>[,scroll=vertical|horizontal] | \n\
  -columnmatch=<indep-column-name>,<expression> [-deltas[={fractional|normalize}]]}]]\n\
 [-array=<z-2d-array>[,<x-1d-array>,<y-id-array>]] [-swaparray]\n\
 [-rpndefinitionsfiles=<filename>[,...]]\n\
 [-rpnexpressions=<setup-expression>[,...][,algebraic]]\n\
 [-rpntransform=<expression>[,algebraic]] [-fixedrange]\n\
 [[-shade=<number>[,<min>,<max>,gray]] | [-contours=<number>[,<min>,<max>]]] \n\
 [-mapShade=<hue0>,<hue1>] \n\
 [-scales=<xl>,<xh>,<yl>,<yh>] [-v1v2Preferred] \n\
 [-labelcontours=interval[,offset]] [-logscale[=<floor>]]\n\
 [-device=<name>[,<device-arguments>]] [-output=<filename>]\n\
 [-interpolate=<nx>,<ny>[,{floor|ceiling|antiripple}]] [-filter=<xcutoff>,<ycutoff>]\n\
 [-shapes=<filename>,<xColumn>,<yColumn>] [-swapxy] [-equalaspect[={-1,1}]]\n\
 [-xlabel=<string>|@<parameter-name>[,scale=<value>][,edit=<edit-command>]] [-ylabel=<string>|@<parameter-name>[,scale=<value>][,edit=<edit-command>]] \n\
 [-title=<string>|@<parameter-name>|filename[,edit=<string>]]\n\
 [-topline=<string>|@<parameter-name>|filename[,edit=<string>]] [-toptitle] [-nolabels]\n\
 [-yrange=minimum=<value>|@<parameter_name>,maximum=<value>|@<parameter_name>] \n\
 [-xrange=minimum=<value>|@<parameter_name>,maximum=<value>|@<parameter_name>] \n\
 [-ystrings=[edit=<editCommand>][,sparse=<integer>][,scale=<value>]]\n\
 [-noborder] [-noscales] [-datestamp] [-verbosity[=<level>]]\n\
 [-layout=<nx>,<ny>] [-thickness=<integer>]\n\
 [-ticksettings=[{xy}time]] [-nocolorbar] [-yaxis=scaleValue=<value>|scaleParameter=<name>[,offsetValue=<number>|offsetParameter=<name>] \n\n\
Program by Michael Borland. (This is version 3, January 2002.)\n";

double *fill_levels(double **level, double min, double max, long levels);
void getDimensionParameters(SDDS_TABLE *SDDS_table, char *name_of_name, char **variable, 
                            char **variableUnits,
                            double *minimum, double *interval, int32_t *number);

double **window_2d_array(double **data_value, double *xmin, double *xmax, double *ymin, double *ymax,
                         double dx, double dy, int32_t *nx, int32_t *ny, double *limit);
void PlotShapesData(SHAPE_DATA *shape, long shapes, double xmin, double xmax, double ymin, double ymax);

char *getParameterLabel(SDDS_TABLE *SDDS_table, char *parameter_name);
void checkParameter(SDDS_TABLE *SDDS_table, char *parameter_name);
void checkLabelParameters(SDDS_TABLE *SDDS_table, char *p1, char *p2, char *p3, char *p4);
void freeParameterLabel(char *users_label, char *label);
void make_enumerated_yscale(char **label, double *yposition, long labels, char *editCommand, long interval, double scale, long thickness, char *ylabel, double ylableScale);
void make_enumerated_xscale(char **label, double *xposition, long labels, char *editCommand, long interval, double scale, long thickness, char *xlabel, double xlabelScale);

long plot_contour(double **data_value, long nx, long ny, long verbosity,
                  double xmin, double xmax, double ymin, double ymax,
                  double dx, double dy, double *xintervals, double *yintervals,
                  char *device, long *frameEnded, char *title, char *xlabel, 
                  char *ylabel, char *topline, double min_level, double max_level, 
                  long levels, double hue0, double hue1, long *layout, 
                  long *ixl, long *iyl, long thickness, unsigned long long tsetFlags, 
                  SHAPE_DATA *shape, long shapes, int *pen, long *flags, 
                  long pause_interval, long columnmatches, char **columnname, long columns, 
                  char *yEditCommand, long ySparseInterval, double yScale, long contour_label_interval,
                  long contour_label_offset, long do_shade, long waterfall,
                  char *colorName, char *colorUnits, long swap_xy, double xlabelScale, double ylabelScale, long yRangeProvided, long xRangeProvided);
long get_plot_labels(SDDS_DATASET *SDDS_table, char *indeptcolumn, char **columnname, long columnnames, 
                     char *allmatches, char *waterfall_par,
                     char *users_xlabel, char *users_ylabel, char *users_title,
                     char **xlabel0, char **ylabel0, char **title0, long deltas, long xRangeProvide);
long process_data_values(double ***data_value, long nx, long ny, long deltas);
void make_waterfall_yscale(long interval, double scale, long ny, double min_y, double max_y, double dy, long thickness);
void process_data(double ***data_value, int32_t *nx, int32_t *ny, double *xmin, double *xmax,
                  double *ymin, double *ymax, double *dx, double *dy, 
                  double *limit, long logscale, double logfloor,
                  long nx_interp, long ny_interp, long x_lowpass, long y_lowpass, long interp_flags,
                  char **xyzArray, long verbosity);

int X11_args(int argc, char **argv);
char *addOuterParentheses(char *arg);
void jxyplot_string(double *x, double *y, char *s, char xmode, char ymode);

char * rearrange_by_index(char *data,long *index,long element_size,long num);

#ifndef COMPILE_AS_SUBROUTINE
FILE *outfile;
#endif

#ifdef VAX_VMS
#define DEFAULT_DEVICE "regis"
#include <unixlib.h>
#endif
#if defined(UNIX) || defined(_WIN32)
#define DEFAULT_DEVICE "motif"
#endif

#define SHADE_GRAY 0
#define SHADE_OPTIONS 1
static char *shadeOptions[SHADE_OPTIONS] = {"gray"};

#define WATERFALL_HORIZONTAL 0
#define WATERFALL_VERTICAL 1
#define WATERFALL_OPTIONS 2
static char *waterfallScroll[WATERFALL_OPTIONS] = {
  "horizontal", "vertical",
  };

#define DELTAS_PLAIN 0
#define DELTAS_NORMALIZED 1
#define DELTAS_FRACTIONAL 2
#define DELTAS_OPTIONS 3
static char *deltasOption[DELTAS_OPTIONS] = {
  "plain", "normalized", "fractional",
} ;

#define SPECTRUM_TRUE " -spectrum true"

void ReadShapeData(SHAPE_DATA *shape, long shapes, long swapXY);

#ifdef COMPILE_AS_SUBROUTINE


#include "draw.h"
void sddscontour_main(char *input_line)

#else

int main(int argc, char **argv)

#endif
{
#ifdef COMPILE_AS_SUBROUTINE
  int argc;
  char **argv;
#endif
  long i, j, i_arg, preferv1v2Parameters, thickness, yaxisScaleProvided=0, yaxisOffset;
  SCANNED_ARG *s_arg;
  SDDS_TABLE SDDS_table;
  char s[SDDS_MAXLINE];
  char *quantity, *variable1, *variable2, **columnmatch, *allmatches, *indepcolumn, *yaxisScalePar=NULL, *yaxisOffsetPar=NULL;
  char *waterfall_par, *waterfall_indeptcol, *waterfall_colorcol, *waterfall_scroll;
  char *variable1Units, *variable2Units, *variable1name, *variable2name;
  long columnmatches = 0, variable1Index, variable2Index;
  char *inputfile;
  char *users_title, *users_topline, *users_xlabel, *users_ylabel, *topline_editcommand=NULL, *title_editcommand=NULL;
  char *title, *topline, *xlabel, *ylabel=NULL, *xlabel_editcommand=NULL, *ylabel_editcommand=NULL;
  double **data_value, *waterfall_parValue, ylabelScale, xlabelScale;
  int32_t nx, ny;
  long swap_xy, swap_array, waterfall;
  double dx, dy, xmin, xmax, ymin, ymax, yaxisScale=0, xmin0, xmax0, ymin0, ymax0;
  char *ymaxPar, *yminPar, *xmaxPar, *xminPar, *maxPar, *minPar;
  double *xintervals, *yintervals;
  long levels, label_contour_interval, contour_label_offset;
  long do_shade, interp_flags = 0;
  double max_level=0, min_level=0, *level, hue0, hue1;
  char *device, *output;
  long x_lowpass, y_lowpass, nx_interp, ny_interp;
  long pause_interval = 1;
  double orig_limit[4] = {0,0,0,0};
  static char bufferstr[SDDS_MAXLINE];
  
  /* double limit[4];*/
  long flags, verbosity, contour_label_interval=0;
  SHAPE_DATA *shape;
  long shapes;
  int pen[4]={0,0,0,0};
  char **rpn_definitions_file, **rpn_expression, *rpn_equation, *rpn_transform;
  long mem1, mem2;
  long rpn_expressions, rpn_definitions_files, deltas, vertical_waterfall;
  static char *equdf_name = "SCEQ.UDF";
  static char *trudf_name = "SCTR.UDF";
  long fixed_range, readstatus, logscale;
  int32_t ySparseInterval;
  int32_t columns=0;
  char **columnname, *devargs, *buffer=NULL, *yEditCommand;
  double logfloor, yScale;
  unsigned long dummyFlags;
  unsigned long long tsetFlags=0;
  long rowNumberType, columnNumberType, yRangeProvided, xRangeProvided;
  char pfix[IFPF_BUF_SIZE];
  long layout[2];
  long ixl=0, iyl=0, frameEnded=0;
  int xTimeMode=0;
  char *colorName=NULL, *colorUnits=NULL;
  char *xyzArray[3], *ptr2;
  long pipe=0;
  char **shadelist;
  long shade_items;
  xlabelScale=ylabelScale=1.0;
  rpn_definitions_file = rpn_expression = NULL;
  rpn_equation = rpn_transform = NULL;
  rpn_expressions = rpn_definitions_files = 0;
  quantity = variable1 = variable2 = NULL;
  waterfall_parValue=NULL;
  waterfall=0;
  waterfall_par=waterfall_indeptcol=waterfall_colorcol=waterfall_scroll=NULL;
  variable1Units = variable2Units = variable1name = variable2name = NULL;
  inputfile = users_title = users_topline = users_xlabel = users_ylabel = NULL;
  data_value = NULL;
  level = NULL;
  shape = NULL;
  x_lowpass = y_lowpass = nx_interp = ny_interp = 0;
  levels = shapes = label_contour_interval = contour_label_offset = do_shade = 0;
  flags = verbosity = swap_xy = swap_array = fixed_range = 0;
  columnmatch = NULL;
  indepcolumn = NULL;
  logscale = logfloor = 0;
  deltas = -1;
  vertical_waterfall=0;
  columnname = NULL;
  yEditCommand = NULL;
  ySparseInterval = 1;
  yScale = 1;
  preferv1v2Parameters = 0;
  hue0 = 0;
  hue1 = 1;
  layout[0] = layout[1] = 0;
  xyzArray[0] = xyzArray[1] = xyzArray[2] = NULL;
  xintervals = yintervals = NULL;
  thickness = 1;
  dx=dy=xmin=ymin=xmax=ymax=0;
  yRangeProvided =  xRangeProvided = 0;
  ymaxPar = yminPar = xmaxPar = xminPar = maxPar = minPar = NULL;
#ifdef COMPILE_AS_SUBROUTINE 
  make_argv(&argc, &argv, input_line);
  flags = NO_GREND + DEVICE_DEFINED;
#else
  outfile = stdout;
  if ((device=getenv("MPL_DEVICE"))) {
    cp_str(&device, device);
#ifdef VAX_VMS
    str_tolower(device);
#endif
  }
#endif

  if (argc<2)
    bomb(NULL, USAGE);

#ifndef COMPILE_AS_SUBROUTINE
#if defined(UNIX) || defined(_WIN32)
  if (1) { 
    int n; 
    n = X11_args(argc, argv);
    argc -= n; argv += n; 
  }
#endif
#endif

  argc = scanargsg(&s_arg, argc, argv); 
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      /* process options here */
      switch (match_string(s_arg[i_arg].list[0], option, OPTIONS, 0)) {
      case SET_WATERFALL:
        if (s_arg[i_arg].n_items<4) {
          fprintf(stderr, "Error (sddscontour): invalid -waterfall syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "parameter", SDDS_STRING, &waterfall_par, 1, 0,
                          "independentColumn", SDDS_STRING, &waterfall_indeptcol, 1, 0,
                          "colorColumn", SDDS_STRING, &waterfall_colorcol, 1, 0,
                          "scroll", SDDS_STRING, &waterfall_scroll, 1, 0,
                          NULL) ||
            !waterfall_par || !waterfall_indeptcol || !waterfall_colorcol) {
          fprintf(stderr, "Error (sddscontour): invalid -waterfall syntax/values\n");
          return(1);
        }
        if (waterfall_scroll && (vertical_waterfall=match_string(waterfall_scroll,waterfallScroll,
                                                              WATERFALL_OPTIONS,0))<0) {
          fprintf(stderr, "Error (sddscontour): invalid scroll given in -waterfall syntax\n");
          return(1);
        }
        waterfall=1;
        s_arg[i_arg].n_items ++;
        break;
      case SET_QUANTITY:
        if (s_arg[i_arg].n_items!=2) {
          fprintf(stderr, "Error (sddscontour): invalid -quantity syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&quantity,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): invalid -quantity syntax\n");
          return(1);
        }
        break;
      case SET_YAXIS:
        /*this is only valid for columnMatch plotting */
        s_arg[i_arg].n_items -= 1;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "scaleValue", SDDS_DOUBLE,&yaxisScale, 1, 0,
                          "scaleParameter", SDDS_STRING,&yaxisScalePar, 1, 0,
                          "offsetValue", SDDS_LONG,&yaxisOffset, 1, 0,
                          "offsetParameter", SDDS_STRING,&yaxisOffsetPar, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -versus syntax/values");
        s_arg[i_arg].n_items += 1;
        if (!yaxisScale && !yaxisScalePar)
          SDDS_Bomb("Invaid -yaxis systax, the yaxis scalar is not provided!");
        yaxisScaleProvided = 1;
        break;
      case SET_COLUMNMATCH:
        if (s_arg[i_arg].n_items<3) {
          fprintf(stderr, "Error (sddscontour): invalid -column syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&indepcolumn,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): invalid -column syntax\n");
          return(1);
        }
        columnmatches = s_arg[i_arg].n_items-2;
        columnmatch = tmalloc(sizeof(*columnmatch)*columnmatches);
        for (i=0; i<columnmatches; i++) {
          if (!SDDS_CopyString(&(columnmatch[i]),s_arg[i_arg].list[2+i])) {
            fprintf(stderr, "Error (sddscontour): invalid -column syntax\n");
            return(1);
          }
        }
        break;
      case SET_ARRAY:
        if ((s_arg[i_arg].n_items!=4) && (s_arg[i_arg].n_items!=2)) {
          fprintf(stderr, "Error (sddscontour): invalid -array syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&(xyzArray[0]),s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): invalid -array syntax\n");
          return(1);
        }
	if (s_arg[i_arg].n_items==4) {
	  if (!SDDS_CopyString(&(xyzArray[1]),s_arg[i_arg].list[2])) {
            fprintf(stderr, "Error (sddscontour): invalid -array syntax\n");
            return(1);
          }
	  if (!SDDS_CopyString(&(xyzArray[2]),s_arg[i_arg].list[3])) {
            fprintf(stderr, "Error (sddscontour): invalid -array syntax\n");
            return(1);
          }
	}
        break;
      case SET_SWAPARRAY:
	swap_array = 1;
        break;
      case SET_SWAP_XY:
        swap_xy = 1;
        break;
      case SET_DEVICE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -device syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&device,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): invalid -device syntax\n");
          return(1);
        }
        if (s_arg[i_arg].n_items>2) {
          gs_device_arguments(s_arg[i_arg].list[2], 0); /* for backward compatibility */
	  setDeviceArgv(s_arg[i_arg].list+2, s_arg[i_arg].n_items-2);
	}
        break;
#ifndef COMPILE_AS_SUBROUTINE
      case SET_OUTPUT:
        if (s_arg[i_arg].n_items!=2) {
          fprintf(stderr, "Error (sddscontour): couldn't scan output filename\n");
          return(1);
        }
        if (!SDDS_CopyString(&output,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): couldn't scan output filename\n");
          return(1);
        }
        outfile = fopen_e(output, "w", FOPEN_INFORM_OF_OPEN);
        break;
#endif
      case SET_SCALES:
        if (s_arg[i_arg].n_items!=5 ||
            sscanf(s_arg[i_arg].list[1], "%lf", &orig_limit[0])!=1 ||
            sscanf(s_arg[i_arg].list[2], "%lf", &orig_limit[1])!=1 ||
            sscanf(s_arg[i_arg].list[3], "%lf", &orig_limit[2])!=1 ||
            sscanf(s_arg[i_arg].list[4], "%lf", &orig_limit[3])!=1 ) {
          fprintf(stderr, "Error (sddscontour): incorrect -scales syntax\n");
          return(1);
        }
        break;
      case SET_LABEL_CONTOURS:
        if ((s_arg[i_arg].n_items!=2 && s_arg[i_arg].n_items!=3) ||
            sscanf(s_arg[i_arg].list[1], "%ld", &contour_label_interval)!=1) {
          fprintf(stderr, "Error (sddscontour): incorrect -label_contour syntax\n");
          return(1);
        }
        contour_label_offset = 0;
        if (s_arg[i_arg].n_items==3 && 
            (sscanf(s_arg[i_arg].list[2], "%ld", &contour_label_offset)!=1 || contour_label_offset<0)) {
          fprintf(stderr, "Error (sddscontour): incorrect -label_contour syntax\n");
          return(1);
        }
        break;
      case SET_CONTOURS:
        if (s_arg[i_arg].n_items==2) {
          if (sscanf(s_arg[i_arg].list[1], "%ld", &levels)!=1) {
            fprintf(stderr, "Error (sddscontour): incorrect -contours syntax (invalid number of levels)\n");
            return(1);
          }
        }
        else if (s_arg[i_arg].n_items==4) {
          if (sscanf(s_arg[i_arg].list[1], "%ld", &levels)!=1 ||
              sscanf(s_arg[i_arg].list[2], "%lf", &min_level)!=1 ||
              sscanf(s_arg[i_arg].list[3], "%lf", &max_level)!=1 ||
              min_level>=max_level) {
            fprintf(stderr, "Error (sddscontour): incorrect -contours syntax\n");
            return(1);
          }
        }
        else {
          fprintf(stderr, "Error (sddscontour): incorrect -contours syntax (wrong number of items)\n");
          return(1);
        }
        break;
      case SET_SHAPES:
        if (s_arg[i_arg].n_items!=4 && s_arg[i_arg].n_items!=5) {
          fprintf(stderr, "Error (sddscontour): incorrect -shapes syntax--give filename(s)\n");
          return(1);
        }
        shape = trealloc(shape, sizeof(*shape)*(s_arg[i_arg].n_items-1+shapes));
        if (!SDDS_CopyString(&(shape[shapes].filename),s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): incorrect -shapes syntax--give filename(s)\n");
          return(1);
        }
        if (!SDDS_CopyString(&(shape[shapes].xColumn),s_arg[i_arg].list[2])) {
          fprintf(stderr, "Error (sddscontour): incorrect -shapes syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&(shape[shapes].yColumn),s_arg[i_arg].list[3])) {
          fprintf(stderr, "Error (sddscontour): incorrect -shapes syntax\n");
          return(1);
        }
        if (s_arg[i_arg].n_items==5) {
          if (sscanf(s_arg[i_arg].list[4], "%ld", &shape[shapes].lineType)!=1) {
            fprintf(stderr, "Error (sddscontour): invalid -shapes sytnax\n");
            return(1);
          }
        }
        else
          shape[shapes].lineType = 0;
        if (!fexists(shape[shapes].filename)) {
          fprintf(stderr, "Error (sddscontour): invalid -shapes syntax---file not found\n");
          return(1);
        }
        shapes++;
        break;
      case SET_EQUAL_ASPECT:
        switch (s_arg[i_arg].n_items) {
        case 1:
          flags |= EQUAL_ASPECT1;
          break;
        case 2:
          if (sscanf(s_arg[i_arg].list[1], "%ld", &i)!=1) {
            fprintf(stderr, "Error (sddscontour): incorrect -equal_aspect syntax\n");
            return(1);
          }
          if (i==1)
            flags |= EQUAL_ASPECT1;
          else if (i== -1)
            flags |= EQUAL_ASPECT_1;
          else {
            fprintf(stderr, "Error (sddscontour): incorrect -equal_aspect syntax\n");
            return(1);
          }
          break;
        default:
          fprintf(stderr, "Error (sddscontour): incorrect -equal_aspect syntax\n");
          return(1);
        }
        break; 
      case SET_XLABEL:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): incorrect -xlabel syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&users_xlabel,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): incorrect -xlabel syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items -=2;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
                          "scale", SDDS_DOUBLE, &xlabelScale, 1, 0,
                          "edit", SDDS_STRING, &xlabel_editcommand, 1, 0, 
                          NULL) || xlabelScale<=0) {
          fprintf(stderr, "Error (sddscontour): invalid -xlabel syntax/values\n");
          return(1);
        }
        s_arg[i_arg].n_items +=2;
        break;
      case SET_YLABEL:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): incorrect -ylabel syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&users_ylabel,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): incorrect -ylabel syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items -=2;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
                          "scale", SDDS_DOUBLE, &ylabelScale, 1, 0,
                          "edit", SDDS_STRING, &ylabel_editcommand, 1, 0,
                          NULL) || ylabelScale<=0) {
          fprintf(stderr, "Error (sddscontour): invalid -ylabel syntax/values\n");
          return(1);
        }
        s_arg[i_arg].n_items +=2;
        break;
      case SET_TITLE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): incorrect -title syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&users_title,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): incorrect -title syntax\n");
          return(1);
        }
        if (s_arg[i_arg].n_items>2) {
          s_arg[i_arg].n_items -=2;
          if (!scanItemList(&dummyFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
                            "edit", SDDS_STRING, &title_editcommand, 1, 0, NULL)) {
            fprintf(stderr, "Error (sddscontour): invalid -title syntax/values\n");
            return(1);
          }
          s_arg[i_arg].n_items +=2;
        }
        break;
      case SET_TOPLINE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): incorrect -topline syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&users_topline,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): incorrect -topline syntax\n");
          return(1);
        }
        if (s_arg[i_arg].n_items>2) {
          s_arg[i_arg].n_items -=2;
          if (!scanItemList(&dummyFlags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
                            "edit", SDDS_STRING, &topline_editcommand, 1, 0, NULL)) {
            fprintf(stderr, "Error (sddscontour): invalid -topline syntax/values\n");
            return(1);
          }
          s_arg[i_arg].n_items +=2;
        }
        break;
      case SET_NO_BORDER:
        flags |= NO_BORDER;
        break;
	  case SET_NO_COLOR_BAR:
		flags |= NO_COLOR_BAR;
		break;
      case SET_NO_SCALES:
        flags |= NO_SCALES;
        break;
      case SET_NO_LABELS:
        flags |= NO_LABELS;
        break;
      case SET_DATE_STAMP:
        flags |= DATE_STAMP;
        break;
      case SET_MAPSHADE:
        if (s_arg[i_arg].n_items!=3) {
          fprintf(stderr, "Error (sddscontour): incorrect -mapshade syntax (wrong number of items)\n");
          return(1);
        }
        if (sscanf(s_arg[i_arg].list[1], "%lf", &hue0)!=1 ||
            sscanf(s_arg[i_arg].list[2], "%lf", &hue1)!=1 ||
            hue0<0 || hue1<0 || hue0>1 || hue1>1 || hue0==hue1) {
          fprintf(stderr, "Error (sddscontour): -incorrect -mapshade syntax.  Hues must be [0, 1].\n");
          return(1);
        }
        break;
      case SET_LAYOUT:
        if (s_arg[i_arg].n_items!=3) {
          fprintf(stderr, "Error (sddscontour): invalid -layout syntax\n");
          return(1);
        }
	if (sscanf(s_arg[i_arg].list[1], "%ld", &layout[0])!=1 || 
	    sscanf(s_arg[i_arg].list[2], "%ld", &layout[1])!=1 ||
	    layout[0]<=0 || layout[1]<=0) {
	  fprintf(stderr, "Error (sddscontour): invalid -layout syntax\n");
          return(1);
        }
        break;
      case SET_SHADE:
        do_shade = 1;
        levels = 100;
        min_level = max_level = 0;
	shadelist = tmalloc(sizeof(*shadelist)*s_arg[i_arg].n_items);
	shade_items=0;
	for(i=0;i<s_arg[i_arg].n_items;i++) {
	   if( match_string(s_arg[i_arg].list[i],shadeOptions,SHADE_OPTIONS,0)>=0) {
	      do_shade=2;
	   } else {
	      shadelist[shade_items]=s_arg[i_arg].list[i];   
	      shade_items++;
	   }
	} 
	
        if (shade_items==3 || shade_items>4) {
          fprintf(stderr, "Error (sddscontour): incorrect -shade syntax (wrong number of items)\n");
          return(1);
        }
        if (shade_items>=2) {
          if (sscanf(shadelist[1], "%ld", &levels)!=1 ||
              levels==0) {
            fprintf(stderr, "Error (sddscontour): incorrect -shade syntax (invalid number of levels)\n");
            return(1);
          }
        }
        if (shade_items==4) {
          if (sscanf(shadelist[2], "%lf", &min_level)!=1 ||
              sscanf(shadelist[3], "%lf", &max_level)!=1 ||
              min_level>max_level) {
            fprintf(stderr, "Error (sddscontour): incorrect -shade syntax\n");
            return(1);
          }
        }
        if (levels > 100) {
          levels = 100;
        }
        free(shadelist);
        break;
      case SET_TOPTITLE:
        flags |= TITLE_AT_TOP;
        break;
      case SET_VERBOSITY:
        verbosity = 1;
        if (s_arg[i_arg].n_items>1) {
          if (sscanf(s_arg[i_arg].list[1], "%ld", &verbosity)!=1 ||
              verbosity<0) {
            fprintf(stderr, "Error (sddscontour): incorrect -verbosity syntax\n");
            return(1);
          }
        }
        break;
      case SET_EQUATION:
	if ((s_arg[i_arg].n_items<2) || (s_arg[i_arg].n_items>3)) {
	  fprintf(stderr, "Error (sddscontour): incorrect -equation syntax\n");
          return(1);
        }
	if (s_arg[i_arg].n_items==2) {
          if (!SDDS_CopyString(&rpn_equation,s_arg[i_arg].list[1])) {
            fprintf(stderr, "Error (sddscontour): incorrect -equation syntax\n");
            return(1);
          }
	  if (!strlen(rpn_equation)) {
	    fprintf(stderr, "Error (sddscontour): incorrect -equation syntax\n");
            return(1);
	  }
	} else if (s_arg[i_arg].n_items==3) {
	  if (strncmp(s_arg[i_arg].list[2], "algebraic", strlen(s_arg[i_arg].list[2]))==0) {
	    ptr2 = addOuterParentheses(s_arg[i_arg].list[1]);
	    if2pf(pfix, ptr2, sizeof pfix);
	    free(ptr2);
	    if (!SDDS_CopyString(&rpn_equation, pfix)) {
	      fprintf(stderr, "Error (sddscontour): problem copying argument string\n");
	      return(1);
	    }
	  } else {
	    fprintf(stderr, "Error (sddscontour): incorrect -equation syntax\n");
            return(1);
	  }
	}
        break;
      case SET_RPN_DEFNS_FILES:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -rpn_definitions_files syntax\n");
          return(1);
        }
        rpn_definitions_file = trealloc(rpn_definitions_file,
                                        sizeof(*rpn_definitions_file)*(rpn_definitions_files+s_arg[i_arg].n_items-1));
        for (i=1; i<s_arg[i_arg].n_items; i++) {
          if (!SDDS_CopyString(&(rpn_definitions_file[rpn_definitions_files+i-1]),s_arg[i_arg].list[i])) {
            fprintf(stderr, "Error (sddscontour): invalid -rpn_definitions_files syntax\n");
            return(1);
          }
          if (!fexists(rpn_definitions_file[rpn_definitions_files+i-1])) {
            fprintf(stderr, "Error (sddscontour): one or more rpn definitions files do not exist\n");
            return(1);
          }
        }
        rpn_definitions_files += s_arg[i_arg].n_items-1;
        break;
      case SET_RPN_EXPRESSIONS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -rpn_expressions syntax\n");
          return(1);
        }
	if (strncmp(s_arg[i_arg].list[s_arg[i_arg].n_items - 1], "algebraic", strlen(s_arg[i_arg].list[s_arg[i_arg].n_items - 1]))==0) {
	  rpn_expression = trealloc(rpn_expression,
				    sizeof(*rpn_expression)*(rpn_expressions+s_arg[i_arg].n_items-2));
	  for (i=1; i<s_arg[i_arg].n_items - 1; i++) {
	    ptr2 = addOuterParentheses(s_arg[i_arg].list[i]);
	    if2pf(pfix, ptr2, sizeof pfix);
	    free(ptr2);
	    if (!SDDS_CopyString(&rpn_expression[rpn_expressions+i-1], pfix)) {
	      fprintf(stderr, "Error (sddscontour): invalid -rpn_expressions syntax\n");
	      return(1);
	    }
	  }
	  rpn_expressions += s_arg[i_arg].n_items-2;
	} else {
	  rpn_expression = trealloc(rpn_expression,
				    sizeof(*rpn_expression)*(rpn_expressions+s_arg[i_arg].n_items-1));
	  for (i=1; i<s_arg[i_arg].n_items; i++) {
	    if (!SDDS_CopyString(&(rpn_expression[rpn_expressions+i-1]),s_arg[i_arg].list[i])) {
	      fprintf(stderr, "Error (sddscontour): invalid -rpn_expressions syntax\n");
	      return(1);
            }
          }
	  rpn_expressions += s_arg[i_arg].n_items-1;
	}
        break;
      case SET_RPN_TRANSFORM:
	if ((s_arg[i_arg].n_items<2) || (s_arg[i_arg].n_items>3)) {
	  fprintf(stderr, "Error (sddscontour): incorrect -rpn_transform syntax\n");
          return(1);
        }
	if (s_arg[i_arg].n_items==2) {
          if (!SDDS_CopyString(&rpn_transform,s_arg[i_arg].list[1])) {
	    fprintf(stderr, "Error (sddscontour): incorrect -rpn_transform syntax\n");
            return(1);
          }
	  if (!strlen(rpn_transform)) {
	    fprintf(stderr, "Error (sddscontour): incorrect -rpn_transform syntax\n");
            return(1);
	  }
	} else if (s_arg[i_arg].n_items==3) {
	  if (strncmp(s_arg[i_arg].list[2], "algebraic", strlen(s_arg[i_arg].list[2]))==0) {
	    ptr2 = addOuterParentheses(s_arg[i_arg].list[1]);
	    if2pf(pfix, ptr2, sizeof pfix);
	    free(ptr2);
	    if (!SDDS_CopyString(&rpn_transform, pfix)) {
	      fprintf(stderr, "Error (sddscontour): incorrect -rpn_transform syntax\n");
	      return(1);
	    }
	  } else {
	    fprintf(stderr, "Error (sddscontour): incorrect -rpn_transform syntax\n");
            return(1);
	  }
	}
        break;
      case SET_INTERPOLATE:
        if (s_arg[i_arg].n_items<3 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &nx_interp)!=1 || nx_interp<=0 ||
            !(nx_interp==1 || power_of_2(nx_interp)) ||
            sscanf(s_arg[i_arg].list[2], "%ld", &ny_interp)!=1 || ny_interp<=0 ||
            !(ny_interp==1 || power_of_2(ny_interp)) ) {
          fprintf(stderr, "Error (sddscontour): invalid -interpolate syntax--integers must be 2^n\n");
          return(1);
        }
        if (s_arg[i_arg].n_items>3) {
          char *flag_text[3] = {"floor", "ceiling", "antiripple"};
          long flag_bit[3] = {CONTOUR_FLOOR, CONTOUR_CEILING, CONTOUR_ANTIRIPPLE};
          interp_flags = 0;
          for (i=3; i<s_arg[i_arg].n_items; i++) {
            if ((j=match_string(s_arg[i_arg].list[i], flag_text, 3, 0))>=0)
              interp_flags |= flag_bit[j];
            else {
              fprintf(stderr, "Error (sddscontour): unknown modifer given with -interpolate\n");
              return(1);
            }
          }
        }
        break;
      case SET_FILTER:
        if (s_arg[i_arg].n_items!=3 ||
            sscanf(s_arg[i_arg].list[1], "%ld", &x_lowpass)!=1 || x_lowpass<=0 ||
            sscanf(s_arg[i_arg].list[2], "%ld", &y_lowpass)!=1 || y_lowpass<=0) {
          fprintf(stderr, "Error (sddscontour): invalid -filter syntax--integers must be > 0\n");
          return(1);
        }
        if (nx_interp==0)
          nx_interp = 1;
        if (ny_interp==0)
          ny_interp = 1;
        break;
      case SET_FIXED_RANGE:
        fixed_range = 1;
        break;
      case SET_LOGSCALE:
        logscale = 1;
        if ((s_arg[i_arg].n_items!=1 && s_arg[i_arg].n_items!=2) ||
            ((s_arg[i_arg].n_items==2 && sscanf(s_arg[i_arg].list[1], "%lf", &logfloor)!=1) ||
             logfloor<0)) {
          fprintf(stderr, "Error (sddscontour): invalid -logscale syntax\n");
          return(1);
        }
        break;
      case SET_DELTAS:
        deltas = DELTAS_PLAIN;
        if (s_arg[i_arg].n_items>=2) {
          if ((deltas=match_string(s_arg[i_arg].list[1], 
                                   deltasOption, DELTAS_OPTIONS, 0))<0) {
            fprintf(stderr, "Error (sddscontour): invalid -deltas syntax\n");
            return(1);
          }
        }
        break;
      case SET_YRANGE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -yRange syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "maximum", SDDS_STRING, &maxPar, 1, 0,
                          "minimum", SDDS_STRING, &minPar, 1, 0,
                          NULL)) {
          fprintf(stderr, "Error (sddscontour): invalid -yRange syntax/values\n");
          return(1);
        }
        if (maxPar) {
          if (wild_match(maxPar, "@*")) 
            SDDS_CopyString(&ymaxPar, maxPar+1);
          else if (!get_double(&ymax, maxPar)) {
            fprintf(stderr, "Error (sddscontour): invalid -yRange syntax/values\n");
            return(1);
          }
          free(maxPar);
        }
        if (minPar) {
          if (wild_match(minPar, "@*"))
            SDDS_CopyString(&yminPar, minPar+1);
          else if (!get_double(&ymin, minPar)) {
            fprintf(stderr, "Error (sddscontour): invalid -yRange syntax/values\n");
            return(1);
          }
          free(minPar);
        }
        s_arg[i_arg].n_items ++;
        yRangeProvided = 1;
        break;
      case SET_XRANGE:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -xRange syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "maximum", SDDS_STRING, &maxPar, 1, 0,
                          "minimum", SDDS_STRING, &minPar, 1, 0,
                          NULL)) {
          fprintf(stderr, "Error (sddscontour): invalid -xRange syntax/values\n");
          return(1);
        }
        if (maxPar) {
          if (wild_match(maxPar, "@*"))
            SDDS_CopyString(&xmaxPar, maxPar+1);
          else if (!get_double(&xmax, maxPar)) {
            fprintf(stderr, "Error (sddscontour): invalid -xRange syntax/values\n");
            return(1);
          }
          free(maxPar);
        }
        if (minPar) {
          if (wild_match(minPar, "@*"))
            SDDS_CopyString(&xminPar, minPar+1);
          else if (!get_double(&xmin, minPar)) {
            fprintf(stderr, "Error (sddscontour): invalid -xRange syntax/values\n");
            return(1);
          } 
          free(minPar);
        }
        s_arg[i_arg].n_items ++;
        xRangeProvided=1;
        break;
      case SET_YSTRINGS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -yStrings syntax\n");
          return(1);
        }
        yEditCommand = NULL;
        ySparseInterval = 1;
        yScale = 1;
        s_arg[i_arg].n_items --;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "edit", SDDS_STRING, &yEditCommand, 1, 0,
                          "sparse", SDDS_LONG, &ySparseInterval, 1, 0,
                          "scale", SDDS_DOUBLE, &yScale, 1, 0,
                          NULL) ||
            ySparseInterval<=0 || yScale<=0) {
          fprintf(stderr, "Error (sddscontour): invalid -yStrings syntax/values\n");
          return(1);
        }
        s_arg[i_arg].n_items ++;
        break;
      case SET_EDITYSTRINGS:
        if (s_arg[i_arg].n_items!=2) {
          fprintf(stderr, "Error (sddscontour): invalid -editYstrings syntax\n");
          return(1);
        }
        if (!SDDS_CopyString(&yEditCommand,s_arg[i_arg].list[1])) {
          fprintf(stderr, "Error (sddscontour): invalid -editYstrings syntax\n");
          return(1);
        }
        break;
      case SET_PREFERV1V2PARAMS:
        preferv1v2Parameters = 1;
        break;
      case SET_THICKNESS:
        if (s_arg[i_arg].n_items!=2 ||
	    sscanf(s_arg[i_arg].list[1], "%ld", &thickness)!=1 ||
	    thickness<=0 || thickness>9) {
	  fprintf(stderr, "Error (sddscontour): invalid -thickness syntax\n");
          return(1);
        }
	break;
      case SET_TICKSETTINGS:
        if (s_arg[i_arg].n_items<2) {
          fprintf(stderr, "Error (sddscontour): invalid -ticksettings syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items --;
        if (!scanItemListLong(&tsetFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "xtime", -1, NULL, 0, TICKSET_XTIME,
                          NULL)) {
          fprintf(stderr, "Error (sddscontour): invalid -ticksettings syntax\n");
          return(1);
        }
        s_arg[i_arg].n_items ++;
        if (tsetFlags&TICKSET_XTIME) {
          xTimeMode=1;
        }
        break;
      case SET_PIPE:
        pipe = 1;
        break;
      default:
        fprintf(stderr, "unknown option - %s given.\n", s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    }
    else {
      /* argument is input filename */
      if (inputfile)
        bomb("only one filename accepted", NULL);
      inputfile = s_arg[i_arg].list[0];
    }
  }

  if (inputfile==NULL) {
    if (!pipe)
      bomb("no input file listed", NULL);
  }

  if (!device)
    device = DEFAULT_DEVICE;

  if (strncmp("motif", device, strlen(device))==0) {
    devargs = gs_device_arguments(NULL, 1);
    buffer = tmalloc(sizeof(*buffer)*(strlen(devargs?devargs:"")+strlen(SPECTRUM_TRUE)+1));
    sprintf(buffer, "%s%s", devargs?devargs:"", SPECTRUM_TRUE);
    gs_device_arguments(buffer, 0);
  }

  if (rpn_definitions_files) {
    rpn(rpn_definitions_file[0]);
    if (rpn_check_error())
      return(1);
    for (i=1; i<rpn_definitions_files; i++) {
      sprintf(s, "\"%s,s\"  @", rpn_definitions_file[i]);
      rpn(s);
      if (rpn_check_error())
        return(1);
    }
  }
  else {
    rpn(getenv("RPN_DEFNS"));
    if (rpn_check_error())
      return(1);
  }
  for (i=0; i<rpn_expressions; i++) {
    rpn(rpn_expression[i]);
    if (rpn_check_error())
      return(1);
  }
  if (!SDDS_InitializeInput(&SDDS_table, inputfile)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return(1);
  }
  
  if (users_topline && strlen(users_topline) && strncmp_case_insensitive(users_topline,"filename", MIN(8, strlen(users_topline)))==0) {
    free(users_topline);
    SDDS_CopyString(&users_topline, inputfile);
    if (topline_editcommand) {
      strcpy(bufferstr, users_topline);
      edit_string(bufferstr, topline_editcommand);
      free(users_topline);
      SDDS_CopyString(&users_topline, bufferstr);
    }
  }
  if (topline_editcommand) free(topline_editcommand);
  
  if (users_title && strlen(users_title) &&  strncmp_case_insensitive(users_title,"filename", MIN(8, strlen(users_title)))==0) {
    free(users_title);
    SDDS_CopyString(&users_title, inputfile);
    if (title_editcommand) {
      strcpy(bufferstr, users_title);
      edit_string(bufferstr, title_editcommand);
      free(users_title);
      SDDS_CopyString(&users_title, bufferstr);
    }
  }
  if (title_editcommand) free(title_editcommand);
  
  checkLabelParameters(&SDDS_table, users_xlabel, users_ylabel, users_title,
                       users_topline);

  if (shapes)
    ReadShapeData(shape, shapes, swap_xy);
  if (swap_xy) 
    SWAP_DOUBLE(xlabelScale, ylabelScale);
  
  if (!quantity && !rpn_equation && !columnmatch && !xyzArray[0] && !waterfall) {
    char **ptr;
    int32_t number;
    if (!(ptr=SDDS_GetColumnNames(&SDDS_table, &number)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (number>1)
      SDDS_Bomb("no quantity specified and more than one column in file.\n");
    SDDS_CopyString(&quantity, *ptr);
    SDDS_FreeStringArray(ptr, number);
    if (verbosity>0)
      printf("will do contour plotting for the quantity %s\n", quantity);
  }
  if (waterfall) {
    if (rpn_equation || columnmatch || quantity || xyzArray[0]) {
      SDDS_Bomb("waterfall option is not compatible with equation, columnmatch or array option!");
    }
    
    if (0>SDDS_GetParameterIndex(&SDDS_table, waterfall_par) ||
        0>SDDS_GetColumnIndex(&SDDS_table, waterfall_indeptcol) ||
        0>SDDS_GetColumnIndex(&SDDS_table, waterfall_colorcol)) {
      SDDS_SetError("waterfall parameter or columns does not exist in the input file.");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      return(1);
    }
    if (columnmatch && yaxisScalePar) {
      if (SDDS_GetParameterIndex(&SDDS_table, yaxisScalePar)<0) {
        fprintf(stderr, "parameter %s does not exist in the input file.\n", yaxisScalePar);
        return(1);
      }
    }
    if (columnmatch && yaxisOffsetPar) {
      if (SDDS_GetParameterIndex(&SDDS_table, yaxisOffsetPar)<0) {
        fprintf(stderr, "parameter %s does not exist in the input file.\n", yaxisOffsetPar);
        return(1);
      }
    }
    if (SDDS_GetColumnInformation(&SDDS_table, "symbol", &colorName, SDDS_GET_BY_NAME, waterfall_colorcol)!=SDDS_STRING ||
        SDDS_GetColumnInformation(&SDDS_table, "units", &colorUnits, SDDS_GET_BY_NAME, waterfall_colorcol)!=SDDS_STRING ) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!colorName)
      colorName=waterfall_colorcol;
  } else if (!rpn_equation && !columnmatch && !xyzArray[0]) {
    if (0>SDDS_GetColumnIndex(&SDDS_table, quantity)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (SDDS_GetColumnInformation(&SDDS_table, "symbol", &colorName, SDDS_GET_BY_NAME, quantity)!=SDDS_STRING ||
        SDDS_GetColumnInformation(&SDDS_table, "units", &colorUnits, SDDS_GET_BY_NAME, quantity)!=SDDS_STRING ) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!colorName)
      colorName=quantity;
    if (rpn_transform)
      colorName=rpn_transform;
  }
  if (waterfall) {
    long pages, rows, rows1;
    double *indepdata=NULL;
    double *indepdata_page = NULL;
    double *tmpptr =NULL;
    double **tmpptr2 =NULL;
    long *sorted_index = NULL;
    
    nx=ny=0;
    pages=rows=0;
    while ((readstatus=SDDS_ReadPage(&SDDS_table))>0) {
      waterfall_parValue=(double*)SDDS_Realloc(waterfall_parValue,sizeof(*waterfall_parValue)*(pages+1));
      data_value=(double**)SDDS_Realloc(data_value,sizeof(*data_value)*(pages+1));
      data_value[pages]=NULL;
      if (!SDDS_GetParameterAsDouble(&SDDS_table, waterfall_par, &waterfall_parValue[pages])) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      if (pages==0) {
        if (!(indepdata=SDDS_GetColumnInDoubles(&SDDS_table, waterfall_indeptcol))) 
          bomb("unable to read independent variable data", NULL);
        rows = SDDS_CountRowsOfInterest(&SDDS_table);
        if (!users_topline) {
          SDDS_GetDescription(&SDDS_table, &topline, NULL);
          if (!topline || !strlen(topline)) {
            sprintf(s, "Data from SDDS file %s, table %ld", inputfile, readstatus);
            SDDS_CopyString(&topline, s);
          }
        }
        else
          topline = users_topline;
      } else {
        rows1 = SDDS_CountRowsOfInterest(&SDDS_table);
        if (rows1<rows) {
          fprintf(stderr,"The rows in page %d is less than that of the first page.\n", ny+1);
          return (1);
        }
      }
      if (!(indepdata_page=SDDS_GetColumnInDoubles(&SDDS_table, waterfall_indeptcol))) 
          bomb("unable to read independent variable data", NULL);
      sorted_index = sort_and_return_index(indepdata_page, SDDS_DOUBLE,rows, 1);
     
      
           	  
      if (!(tmpptr=(double*)SDDS_GetColumnInDoubles(&SDDS_table, waterfall_colorcol))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
     
      
      data_value[pages] =(double *) rearrange_by_index((char *) tmpptr,sorted_index,SDDS_type_size[SDDS_DOUBLE-1],rows);
     
      free(indepdata_page);
      free(sorted_index); 
      free(tmpptr);
      pages++;
    }
    nx=pages;
    ny=rows;
    sorted_index = sort_and_return_index(waterfall_parValue,SDDS_DOUBLE,pages,1);
    tmpptr2 = (double **) rearrange_by_index((char *) (data_value),sorted_index,sizeof(double *),pages); 
    free(sorted_index);
    free(data_value);
    data_value = tmpptr2;
    
    find_min_max(&xmin, &xmax, waterfall_parValue, nx);
    find_min_max(&ymin,&ymax, indepdata,ny);
    get_plot_labels(&SDDS_table, waterfall_par, &waterfall_colorcol, 1,
                    waterfall_colorcol, waterfall_indeptcol, users_xlabel, users_ylabel, users_title,
                    &xlabel, &ylabel, &title, deltas, xRangeProvided);
    free(waterfall_parValue);
    free(indepdata);
    dx = (xmax-xmin)/(nx-1);
    dy = (ymax-ymin)/(ny-1);
 /*   if (orig_limit[0]!=orig_limit[1] || orig_limit[2]!=orig_limit[3]) {
      memcpy((char*)limit, (char*)orig_limit, 4*sizeof(limit[0]));
      data_value = window_2d_array(data_value, &xmin, &xmax, &ymin, &ymax,
                                   dx, dy, &nx, &ny, limit);
    } */
    /*  flags |= NO_YSCALES;*/
    if (process_data_values(&data_value,nx,ny,deltas))
      return 1;
    /*above data_value is for horizontal waterfall*/
    if ((!vertical_waterfall && swap_xy) || (vertical_waterfall && !swap_xy)) {
      /*make vertical waterfall plot*/
      double **new_data;
      new_data=(double**)zarray_2d(sizeof(double), ny, nx);
      for (i=0;i<nx;i++) {
        for (j=0;j<ny;j++)
          new_data[j][i]=data_value[i][j];
      }
      SDDS_FreeMatrix((void**)data_value, nx);
      data_value=new_data;
      SWAP_PTR(xlabel, ylabel);
      SWAP_DOUBLE(xmin, ymin);
      SWAP_DOUBLE(dx, dy);
      SWAP_DOUBLE(xmax, ymax);
      SWAP_LONG(nx, ny);
    }
    if (xlabel[0]=='@') {
      xlabel = getParameterLabel(&SDDS_table, xlabel+1);
      if (xlabel_editcommand) {
        strcpy(bufferstr, xlabel);
        edit_string(bufferstr, xlabel_editcommand);
        free(xlabel);
        SDDS_CopyString(&xlabel, bufferstr);
      }
    }
    if (ylabel[0]=='@') {
      ylabel = getParameterLabel(&SDDS_table, ylabel+1);
      if (ylabel_editcommand) {
        strcpy(bufferstr, ylabel);
        edit_string(bufferstr, ylabel_editcommand);
        free(ylabel);
        SDDS_CopyString(&ylabel, bufferstr);
      }
    }
    if (title[0]=='@')
      title = getParameterLabel(&SDDS_table, title+1);
    if (topline[0]=='@')
      topline = getParameterLabel(&SDDS_table, topline+1);
    if (!SDDS_Terminate(&SDDS_table)) {
      SDDS_SetError("problem closing file");
      SDDS_PrintErrors(stderr,  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    xmax = xmin + (nx-1)*dx;
    ymax = ymin + (ny-1)*dy;
    process_data(&data_value, &nx, &ny, &xmin, &xmax, &ymin, &ymax, &dx, &dy, 
                 orig_limit, logscale, logfloor, nx_interp, ny_interp, x_lowpass, 
                 y_lowpass, interp_flags, xyzArray, verbosity); 
    
    plot_contour(data_value,  nx, ny, verbosity,
                 xmin,  xmax, ymin, ymax,
                 dx,  dy,  xintervals, yintervals,
                 device, &frameEnded, title, xlabel, 
                 ylabel, topline, min_level,  max_level, 
                 levels, hue0,  hue1, layout, 
                 &ixl, &iyl, thickness, tsetFlags, 
                 shape, shapes, pen, &flags, 
                 pause_interval, columnmatches, columnname,  columns, 
                 yEditCommand, ySparseInterval, yScale,  contour_label_interval,
                 contour_label_offset, do_shade, 1, colorName, colorUnits, swap_xy, xlabelScale, ylabelScale, yRangeProvided, xRangeProvided);
    if (data_value)
      SDDS_FreeMatrix((void**)data_value, nx);
    data_value = NULL;
    free(waterfall_par);
    free(waterfall_indeptcol);
    free(waterfall_colorcol);
    free(title);
    free(xlabel);
    free(ylabel);
    if (!users_topline && topline) free(topline);
    
  } else {
    rowNumberType = 0;
    columnNumberType = 0;
    if (!columnmatch && !xyzArray[0]) {
      if (!preferv1v2Parameters) {
        rowNumberType = SDDS_GetNamedParameterType(&SDDS_table, "NumberOfRows");
        columnNumberType = SDDS_GetNamedParameterType(&SDDS_table, "NumberOfColumns");
        SDDS_ClearErrors();
        if (rowNumberType && columnNumberType) {
          if (!SDDS_INTEGER_TYPE(rowNumberType) || !SDDS_INTEGER_TYPE(columnNumberType)) {
            fputs("NumberOfRows and NumberOfColumns parameters are present but at least one has a non-integer type--attempting alternative processing mode\n", stderr);
            rowNumberType = columnNumberType = 0;
          }
        }
        else
          rowNumberType = columnNumberType = 0;
        if (!rowNumberType || !columnNumberType) {
          if ((variable1Index = SDDS_GetParameterIndex(&SDDS_table, "Variable1Name"))<0 ||
              (variable2Index = SDDS_GetParameterIndex(&SDDS_table, "Variable2Name"))<0 ||
              SDDS_GetParameterType(&SDDS_table, variable1Index)!=SDDS_STRING ||
              SDDS_GetParameterType(&SDDS_table, variable2Index)!=SDDS_STRING)
            SDDS_Bomb("Can't figure out how to turn column into 2D grid!\nCheck existence and type of Variable1Name and Variable2Name");
        }
      } else {
        variable1Index = SDDS_GetParameterIndex(&SDDS_table, "Variable1Name");
        variable2Index = SDDS_GetParameterIndex(&SDDS_table, "Variable2Name");
        SDDS_ClearErrors();
        if (variable1Index>=0 && variable2Index>=0) {
          if (SDDS_GetParameterType(&SDDS_table, variable1Index)!=SDDS_STRING ||
              SDDS_GetParameterType(&SDDS_table, variable2Index)!=SDDS_STRING) {
            variable1Index = variable2Index = -1;
          } 
        }
        if (variable1Index<0 || variable2Index<0) {
          rowNumberType = SDDS_GetNamedParameterType(&SDDS_table, "NumberOfRows");
          columnNumberType = SDDS_GetNamedParameterType(&SDDS_table, "NumberOfColumns");
          SDDS_ClearErrors();
          if (rowNumberType && columnNumberType) {
            if (!SDDS_INTEGER_TYPE(rowNumberType) || !SDDS_INTEGER_TYPE(columnNumberType)) {
              fputs("NumberOfRows and NumberOfColumns parameters are present but at least one has a non-integer type--attempting alternative processing mode\n", stderr);
              SDDS_Bomb("Can't figure out how to turn column into 2D grid!\n");
            }
          }
        }
      }
    }
    else if (!xyzArray[0]) {
      if (!xRangeProvided) {
        if (SDDS_GetColumnIndex(&SDDS_table, indepcolumn)<0) {
          fprintf(stderr, "error: couldn't find column %s in file\n", indepcolumn);
          return(1);
        }
      }
      SDDS_SetColumnFlags(&SDDS_table, 0);
      s[0] = 0;
      for (i=0; i<columnmatches; i++) {
        if (strlen(s)<256) {
          strcat(s, columnmatch[i]);
          strcat(s, " ");
        }
        
        if (!SDDS_SetColumnsOfInterest(&SDDS_table, SDDS_MATCH_STRING, columnmatch[i], SDDS_OR)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      }
      SDDS_CopyString(&allmatches, s);
      if ((columns=SDDS_CountColumnsOfInterest(&SDDS_table))<=0) {
        fprintf(stderr, "error: no columns found that match\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      SDDS_SetColumnFlags(&SDDS_table, 1);
    } else {
      for (i=0; i<3; i++) {
        if (xyzArray[i]) {
          if (SDDS_GetArrayIndex(&SDDS_table, xyzArray[i])<0) {
            fprintf(stderr, "error: couldn't find array %s in file\n", xyzArray[i]);
            return(1);
          }
        }
      }
    }
    
    if (rpn_equation)
      create_udf(equdf_name, rpn_equation);
    if (rpn_transform)
      create_udf(trudf_name, rpn_transform);
    if (fixed_range && quantity) {
      double minMin, maxMax, thisMin, thisMax, *data;
      long rows;
      maxMax = -(minMin = DBL_MAX);
      while (SDDS_ReadPage(&SDDS_table)>0) {
        if ((rows = SDDS_RowCount(&SDDS_table))<=0)
          continue;
        if (!(data = SDDS_GetColumnInDoubles(&SDDS_table, quantity))) {
          SDDS_SetError("problem reading data for fixed range determination");
          SDDS_PrintErrors(stderr,  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        find_min_max(&thisMin, &thisMax, data, rows);
        free(data);
        if (thisMin<minMin)
          minMin = thisMin;
        if (thisMax>maxMax)
          maxMax = thisMax;
      }
      if (maxMax>minMin) {
        max_level = maxMax;
        min_level = minMin;
      }
      if (!SDDS_Terminate(&SDDS_table) ||
          !SDDS_InitializeInput(&SDDS_table, inputfile)) {
        SDDS_SetError("problem closing and reopening input file");
        SDDS_PrintErrors(stderr,  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    while ((readstatus=SDDS_ReadPage(&SDDS_table))>0) {
      if (rowNumberType && columnNumberType) {
        if (!SDDS_GetParameter(&SDDS_table, "NumberOfRows", &nx) 
            || !SDDS_GetParameter(&SDDS_table, "NumberOfColumns", &ny)) {
          fprintf(stderr, "error: unable to read NumberOfRows or NumberOfColumns parameter\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        if (!SDDS_CastValue(&nx, 0, rowNumberType, SDDS_LONG, &nx) || 
            !SDDS_CastValue(&ny, 0, columnNumberType, SDDS_LONG, &ny) ) {
          fprintf(stderr, "error: unable to cast row or column numbers to type SDDS_LONG\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
#ifndef COMPILE_AS_SUBROUTINE
        if (verbosity>0)
          printf("Data has %" PRId32 " rows by %" PRId32 " columns\n", nx, ny);
#endif
        xmin = ymin = 0;
        dx = dy = 1;
        if (!users_xlabel) {
          SDDS_CopyString(&xlabel, "row");
          users_xlabel = xlabel;
        }
        else 
          xlabel = users_xlabel;
        if (!users_ylabel) {
          SDDS_CopyString(&ylabel, "column");
          users_ylabel = ylabel;
        }
        else
          ylabel = users_ylabel;
        if (!users_title) {
          sprintf(s, "contours of constant %s", quantity?quantity:rpn_equation);
          SDDS_CopyString(&title, s);
          users_title = title;
        }
        else
          title = users_title;
      } /*end of if (rowNumberType && columnNumberType) */
      else if (!columnmatch && !xyzArray[0]) {
        getDimensionParameters(&SDDS_table, "Variable1Name", 
                               &variable1, &variable1Units, 
                               &xmin, &dx, &nx);
        getDimensionParameters(&SDDS_table, "Variable2Name", 
                               &variable2, &variable2Units, 
                               &ymin, &dy, &ny);
        if (!users_xlabel)  {
          if (variable1Units) {
            sprintf(s, "%s (%s)", variable1, variable1Units);
            SDDS_CopyString(&xlabel, s);
          }
          else
            SDDS_CopyString(&xlabel, variable1);
        }
        else
          xlabel = users_xlabel;
        if (!users_ylabel)  {
          if (variable2Units) {
            sprintf(s, "%s (%s)", variable2, variable2Units);
            SDDS_CopyString(&ylabel, s);
          }
          else
            SDDS_CopyString(&ylabel, variable2);
        }
        else
        ylabel = users_ylabel;
        if (!users_title) {
          if (rpn_transform) 
            sprintf(s, "%s as a function of %s and %s", rpn_transform,
                    swap_xy?variable2:variable1, swap_xy?variable1:variable2);
          else
            sprintf(s, "%s as a function of %s and %s", quantity?quantity:rpn_equation,
                    swap_xy?variable2:variable1, swap_xy?variable1:variable2);
          SDDS_CopyString(&title, s);
          users_title = title;
        }
        else
          title = users_title;
        
#ifndef COMPILE_AS_SUBROUTINE
        if (verbosity>1) {
          /* -- print out some information about the data */
          printf("dimension 1:  name = %s, minimum = %e, interval = %e, dimension = %" PRId32 "\n",
                 variable1, xmin, dx, nx);
          printf("dimension 2:  name = %s, minimum = %e, interval = %e, dimension = %" PRId32 "\n",
                 variable2, ymin, dy, ny);
        }
#endif
      } 
      
      if (!users_topline) {
        SDDS_GetDescription(&SDDS_table, &topline, NULL);
        if (!topline || !strlen(topline)) {
          sprintf(s, "Data from SDDS file %s, table %ld", inputfile, readstatus);
          SDDS_CopyString(&topline, s);
        }
      }
      else
        topline = users_topline;
      
      if (quantity) {
        if (data_value)
          SDDS_FreeMatrix((void**)data_value, nx);
        data_value=NULL;
        if (swap_xy) {
          SWAP_PTR(xlabel, ylabel);
          SWAP_DOUBLE(xmin, ymin);
          SWAP_DOUBLE(dx, dy);
          SWAP_LONG(nx, ny);
          if (!(data_value=SDDS_GetDoubleMatrixFromColumn(&SDDS_table, 
                                                          quantity, nx, ny, SDDS_COLUMN_MAJOR_DATA))) {
            fputs("unable to get array from SDDS table\n", stderr);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
        }
        else if (!(data_value=SDDS_GetDoubleMatrixFromColumn(&SDDS_table, 
                                                             quantity, nx, ny, SDDS_ROW_MAJOR_DATA))) {
          fputs("unable to get array from SDDS table\n", stderr);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      } /*end of if (quantity) */
      else if (columnmatch) {
        long ix, iy, flip, colID, minID, maxID;
        double *indepdata;
        
        nx = SDDS_CountRowsOfInterest(&SDDS_table);
        flip = 0;
        if (!xRangeProvided) {
          SDDS_SetColumnFlags(&SDDS_table, 1);
          if (!(indepdata=SDDS_GetColumnInDoubles(&SDDS_table, indepcolumn))) 
            bomb("unable to read independent variable data", NULL);
          if (indepdata[0]>indepdata[nx-1])
            flip = 1;
          find_min_max(&xmin, &xmax, indepdata, nx);
          free(indepdata);
        } else {
          if (xmaxPar && !SDDS_GetParameterAsDouble(&SDDS_table, xmaxPar, &xmax)) 
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (xminPar && !SDDS_GetParameterAsDouble(&SDDS_table, xminPar, &xmin))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (xmax<xmin) {
            fprintf(stderr,"Invalid xrange values provided, xmax is less than xmin.\n");
            return 1;
          }
        }
        if (yRangeProvided) {
          if (ymaxPar && !SDDS_GetParameterAsDouble(&SDDS_table, ymaxPar, &ymax))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (yminPar && !SDDS_GetParameterAsDouble(&SDDS_table, yminPar, &ymin))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (ymax<ymin) {
            fprintf(stderr,"Invalid yrange values provided, ymax is less than ymin.\n");
            return 1;
          }
        }
        if (yaxisScalePar) {
          char *label, *units, s[1024];
          label = units = NULL;
          if (!SDDS_GetParameterAsDouble(&SDDS_table, yaxisScalePar, &yaxisScale)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
          if (!users_ylabel) {
            SDDS_GetParameterInformation(&SDDS_table, "units", &units, SDDS_GET_BY_NAME, yaxisScalePar);
            SDDS_GetParameterInformation(&SDDS_table, "symbol", &label, SDDS_GET_BY_NAME, yaxisScalePar);
            if (label) {
              sprintf(s, "%s", label);
              if (units)
                sprintf(s,"%s (%s)", s, units);
              SDDS_CopyString(&users_ylabel, s);
              free(label);
              if (units) free(units);
            }
          }
        }
        if (yaxisOffsetPar) {
          if (!SDDS_GetParameterAsLong(&SDDS_table, yaxisOffsetPar, &yaxisOffset)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          } 
        }
        if (verbosity>1)
          printf("range of independent variable: %e to %e\n", xmin,xmax); 
        
        SDDS_SetColumnFlags(&SDDS_table, 0);
        for (i=0; i<columnmatches; i++) { 
          if (!SDDS_SetColumnsOfInterest(&SDDS_table, SDDS_MATCH_STRING, columnmatch[i], SDDS_OR)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
        }
        if (!columnname) 
          columnname = SDDS_GetColumnNames(&SDDS_table, &columns);
         
        if (data_value)
          SDDS_FreeMatrix((void**)data_value, nx);
        data_value = SDDS_AllocateMatrix(sizeof(**data_value), nx, columns);
        ny = 0;
        minID = INT32_MAX;
        maxID = 0;
        
        for (iy=0; iy<columns; iy++) {
          double *data;
          if (yaxisScaleProvided) {
            if (!get_long1(&colID, columnname[iy]))
              SDDS_Bomb("Unable to get the integer from column name.");
            
            if (yRangeProvided && (iy*yaxisScale<ymin || iy*yaxisScale>ymax))
              continue;
            if (colID<minID)
              minID = colID;
            if (maxID<colID)
              maxID = colID;
          }
          if (!(data = SDDS_GetColumnInDoubles(&SDDS_table, columnname[iy]))) {
            fprintf(stderr, "Unable to get column value %s\n", columnname[iy]);
            
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
          if (!flip) 
            for (ix=0; ix<nx; ix++)
              data_value[ix][ny] = data[ix];
          else
            for (ix=0; ix<nx; ix++)
              data_value[nx-ix-1][ny] = data[ix];
          ny++;
          free(data);
        }
        if (!ny)
          SDDS_Bomb("No data for plotting.");
        
        if (verbosity>1)
          printf("%" PRId32 " rows and %" PRId32 " columns\n", nx, ny);
        dx = (xmax-xmin)/(nx-1);
        if (yaxisScaleProvided) {
          ymin = (minID - yaxisOffset) * yaxisScale;
          ymax = (maxID - yaxisOffset) * yaxisScale;
          dy = (ymax - ymin)/(ny-1);
          yRangeProvided = 1;
        } else {
          if (yRangeProvided) {
            dy = (ymax-ymin)/(ny-1); 
          } else {
            ymin = 0;
            ymax = ny-1;
            dy = 1;
          }
        }
        
        get_plot_labels(&SDDS_table,indepcolumn,columnname,columns,
                        allmatches,NULL,users_xlabel,users_ylabel,users_title,
                        &xlabel,&ylabel,&title,deltas, xRangeProvided);
        if (swap_xy) {
          SWAP_LONG(xRangeProvided, yRangeProvided);
        }
        if (!users_title)
          users_title=title; 
        if (!swap_xy && !yRangeProvided)
          flags |= NO_YSCALES;
        else if (swap_xy && !xRangeProvided)
          flags |= NO_XSCALES; 
        if (swap_xy) {
          double **new_data;
          new_data=(double**)zarray_2d(sizeof(double), ny, nx);
          for (i=0;i<nx;i++) {
            for (j=0;j<ny;j++)
              new_data[j][i]=data_value[i][j];
          }
          SDDS_FreeMatrix((void**)data_value, nx);
          data_value=new_data;
          SWAP_PTR(xlabel, ylabel);
          SWAP_DOUBLE(xmin, ymin);
          SWAP_DOUBLE(dx, dy);
          SWAP_DOUBLE(xmax, ymax);
          SWAP_LONG(nx, ny);
        }
        if (process_data_values(&data_value,nx,ny,deltas))
          return 1;
      } else if (xyzArray[0]) {
        char *columnunits;
        long ix, iy, type[3];
        SDDS_ARRAY *xyzData[3];
        for (i=0; i<3; i++) {
          if (xyzArray[i]) {
            SDDS_GetArrayInformation(&SDDS_table, "type", &type[i], SDDS_GET_BY_NAME,
                                     xyzArray[i]);
            if (!(xyzData[i]=SDDS_GetArray(&SDDS_table, xyzArray[i], NULL))) {
              fprintf(stderr, "unable to read %s array\n", xyzArray[i]);
              return(1);
            }
          }
        }
        if (xyzData[0]->definition->dimensions != 2) {
          fprintf(stderr, "array %s must be 2 dimensions\n", xyzArray[0]);
          return(1);	
        }
        if (xyzArray[1]) {
          if (xyzData[1]->definition->dimensions != 1) {
            fprintf(stderr, "array %s must be 1 dimension\n", xyzArray[1]);
            return(1);	
          }
          if (xyzData[1]->dimension[0] != xyzData[0]->dimension[0]) {
            fprintf(stderr, "array dimension mismatch between %s and %s\n", xyzArray[0], xyzArray[1]);
            return(1);
          }
        }
        if (xyzArray[2]) {
          if (xyzData[2]->definition->dimensions != 1) {
            fprintf(stderr, "array %s must be 1 dimension\n", xyzArray[2]);
            return(1);	
          }
          if (xyzData[2]->dimension[0] != xyzData[0]->dimension[1]) {
            fprintf(stderr, "array dimension mismatch between %s and %s\n", xyzArray[0], xyzArray[2]);
            return(1);
          }   
        }
        if (swap_array) {
          nx = xyzData[0]->dimension[1];
          ny = xyzData[0]->dimension[0];
        } else {
          nx = xyzData[0]->dimension[0];
          ny = xyzData[0]->dimension[1];
        }
        if (data_value)
          SDDS_FreeMatrix((void**)data_value, nx);
        data_value = SDDS_AllocateMatrix(sizeof(**data_value), nx, ny);
        switch (type[0]) {
        case SDDS_SHORT:
          for (ix=0; ix<nx; ix++)
            for (iy=0; iy<ny; iy++) {
              data_value[ix][iy] = (double)(((short*)xyzData[0]->data)[ix * ny + iy]);
            }
          break;
        case SDDS_LONG:
          for (ix=0; ix<nx; ix++)
            for (iy=0; iy<ny; iy++) {
              data_value[ix][iy] = (double)(((int32_t*)xyzData[0]->data)[ix * ny + iy]);
            }
          break;
        case SDDS_FLOAT:
          for (ix=0; ix<nx; ix++)
            for (iy=0; iy<ny; iy++) {
              data_value[ix][iy] = (double)(((float*)xyzData[0]->data)[ix * ny + iy]);
            }
          break;
        case SDDS_DOUBLE:
          for (ix=0; ix<nx; ix++)
            for (iy=0; iy<ny; iy++) {
              data_value[ix][iy] = ((double*)xyzData[0]->data)[ix * ny + iy];
            }
          break;
        }
        
        xintervals = malloc(sizeof(double) * nx);
        yintervals = malloc(sizeof(double) * ny);
        if (verbosity>1)
          printf("%" PRId32 " rows and %" PRId32 " columns\n", nx, ny);
        
        if (swap_array) {
          j = 2;
        } else {
          j = 1;
        }
        if (xyzArray[1]) {
          switch (type[1]) {
          case SDDS_SHORT:
            for (i=0; i<nx; i++)
              xintervals[i] = (double)(((short*)xyzData[j]->data)[i]);
            break;
          case SDDS_LONG:
            for (i=0; i<nx; i++)
	    xintervals[i] = (double)(((int32_t*)xyzData[j]->data)[i]);
            break;
          case SDDS_FLOAT:
            for (i=0; i<nx; i++)
              xintervals[i] = (double)(((float*)xyzData[j]->data)[i]);
            break;
          case SDDS_DOUBLE:
            for (i=0; i<nx; i++)
              xintervals[i] = ((double*)xyzData[j]->data)[i];
            break;
          }
        } else {
          for (i=0; i<nx; i++)
            xintervals[i] = i;
        }
        
        if (swap_array) {
          j = 1;
        } else {
          j = 2;
        }
        if (xyzArray[2]) {
          switch (type[2]) {
          case SDDS_SHORT:
            for (i=0; i<ny; i++)
              yintervals[i] = (double)(((short*)xyzData[j]->data)[i]);
            break;
          case SDDS_LONG:
            for (i=0; i<ny; i++)
              yintervals[i] = (double)(((int32_t*)xyzData[j]->data)[i]);
            break;
          case SDDS_FLOAT:
            for (i=0; i<ny; i++)
	    yintervals[i] = (double)(((float*)xyzData[j]->data)[i]);
            break;
          case SDDS_DOUBLE:
            for (i=0; i<ny; i++)
              yintervals[i] = ((double*)xyzData[j]->data)[i];
            break;
          }
        } else {
          for (i=0; i<ny; i++)
            yintervals[i] = i;
        }
        
        find_min_max(&xmin, &xmax, xintervals, nx);
        find_min_max(&ymin, &ymax, yintervals, ny);
        
        dx = (xmax-xmin)/(nx-1);
        dy = (ymax-ymin)/(ny-1);
        
        if (!do_shade) {
          fprintf(stderr, "warning: dx and dy are static in a contour plot\n");
        }
        
        if (xyzArray[1]) {
          if (!users_xlabel) {
            SDDS_GetArrayInformation(&SDDS_table, "units", &xlabel, SDDS_GET_BY_NAME, xyzArray[1]);
            if (xlabel && xlabel[0])
              sprintf(s, "%s (%s)", xyzArray[1], xlabel);
            else
              sprintf(s, "%s", xyzArray[1]);
            free(xlabel);
            SDDS_CopyString(&xlabel, s);
          }
          else
            xlabel = users_xlabel;
        } else {
          if (!users_xlabel) {
            sprintf(s, "x");
            SDDS_CopyString(&xlabel, s);
          }
          else
            xlabel = users_xlabel;
        }
        
        if (xyzArray[2]) {
          if (!users_ylabel) {
            SDDS_GetArrayInformation(&SDDS_table, "units", &ylabel, SDDS_GET_BY_NAME, xyzArray[2]);
            if (ylabel && ylabel[0])
              sprintf(s, "%s (%s)", xyzArray[2], ylabel);
            else
              sprintf(s, "%s", xyzArray[2]);
            free(ylabel);
            SDDS_CopyString(&ylabel, s);
          } else
            ylabel = users_ylabel;
        } else {
          if (!users_ylabel) {
            sprintf(s, "y");
            SDDS_CopyString(&ylabel, s);
          }
          else
            ylabel = users_ylabel;
        }
        
        if (!users_title) {
          columnunits = NULL;
          SDDS_GetArrayInformation(&SDDS_table, "units", &columnunits, SDDS_GET_BY_NAME, xyzArray[0]);
          if (columnunits && columnunits[0])
            sprintf(s, "%s (%s)", xyzArray[0], columnunits);
          else
            sprintf(s, "%s", xyzArray[0]);
          if (columnunits) free(columnunits);
          SDDS_CopyString(&title, s);
          users_title = title;
        }
        else
          title = users_title;
        
        if (SDDS_NumberOfErrors())
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (i=0; i<3; i++) {
          if (xyzArray[i]) {
            SDDS_FreeArray(xyzData[i]);
          }
        }
      }
      else {
        if (swap_xy) {
          SWAP_PTR(xlabel, ylabel);
          SWAP_DOUBLE(xmin, ymin);
          SWAP_DOUBLE(dx, dy);
          SWAP_LONG(nx, ny);
        }
        data_value = SDDS_AllocateMatrix(sizeof(double), nx, ny);
      }
      
      if (xlabel[0]=='@') {
        xlabel = getParameterLabel(&SDDS_table, xlabel+1);
        if (xlabel_editcommand) {
          strcpy(bufferstr, xlabel);
          edit_string(bufferstr, xlabel_editcommand);
          free(xlabel);
          SDDS_CopyString(&xlabel, bufferstr);
        }
      }
      if (ylabel[0]=='@') {
        ylabel = getParameterLabel(&SDDS_table, ylabel+1);
        if (ylabel_editcommand) {
          strcpy(bufferstr, ylabel);
          edit_string(bufferstr, ylabel_editcommand);
          free(ylabel);
          SDDS_CopyString(&ylabel, bufferstr);
        }
      }
      if (title[0]=='@')
        title = getParameterLabel(&SDDS_table, title+1);
      if (topline[0]=='@')
        topline = getParameterLabel(&SDDS_table, topline+1);
      
      if (rpn_equation) {
        if (!swap_xy) {
          mem1 = rpn_create_mem(variable1?variable1:"row", 0);
          mem2 = rpn_create_mem(variable2?variable2:"column", 0);
        }
        else {
          mem2 = rpn_create_mem(variable1?variable1:"row", 0);
          mem1 = rpn_create_mem(variable2?variable2:"column", 0);
        }
        if (!SDDS_StoreParametersInRpnMemories(&SDDS_table)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        if (xyzArray[0]) {
          fprintf(stderr, "warning: rpn equations calculated using static dx and dy\n");
        }
        for (i=0; i<nx; i++) {
          rpn_store(i*dx+xmin, NULL, mem1);
          for (j=0; j<ny; j++) {
            rpn_store(j*dy+ymin, NULL, mem2);
            if (!SDDS_StoreRowInRpnMemories(&SDDS_table, swap_xy?i+j*nx:i*ny+j)) {
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
              return(1);
            }
            data_value[i][j] = rpn(equdf_name);
            if (rpn_check_error())
              return(1);
            rpn_clear();
          }
        }
      }
      
      if (rpn_transform) {
        if (!swap_xy) {
          mem1 = rpn_create_mem(variable1?variable1:"row", 0);
          mem2 = rpn_create_mem(variable2?variable2:"column", 0);
        }
        else {
          mem2 = rpn_create_mem(variable1?variable1:"row", 0);
          mem1 = rpn_create_mem(variable2?variable2:"column", 0);
        }
        if (!SDDS_StoreParametersInRpnMemories(&SDDS_table)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        if (xyzArray[0]) {
          fprintf(stderr, "warning: rpn transform calculated using static dx and dy\n");
        }
        for (i=0; i<nx; i++) {
          rpn_store(i*dx+xmin, NULL, mem1);
          for (j=0; j<ny; j++) {
            rpn_store(j*dy+ymin, NULL, mem2);
            if (!SDDS_StoreRowInRpnMemories(&SDDS_table, swap_xy?i+j*nx:i*ny+j)) {
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
              return(1);
            }
            /*push_num(data_value[i][j]);*/
            data_value[i][j] = rpn(trudf_name);
            if (rpn_check_error())
              return(1);
            rpn_clear();
          }
        }
      }
      xmin0 = xmin;
      xmax0 = xmax;
      ymax0 = ymax;
      ymin0 = ymin;
      /*the xmin, xmax, ymin, ymax will be changed after processing data, needs remind their values for next plot */
      process_data(&data_value, &nx, &ny, &xmin, &xmax, &ymin, &ymax, &dx, &dy, 
                   orig_limit, logscale, logfloor, nx_interp, ny_interp, x_lowpass, 
                   y_lowpass, interp_flags, xyzArray, verbosity); 
       
      if (!plot_contour(data_value,  nx, ny, verbosity,
                        xmin,  xmax, ymin, ymax,
                        dx,  dy,  xintervals, yintervals,
                        device, &frameEnded, title, xlabel, 
                        ylabel, topline, min_level,  max_level, 
                        levels, hue0,  hue1, layout, 
                        &ixl, &iyl, thickness, tsetFlags, 
                        shape, shapes, pen, &flags, 
                        pause_interval, columnmatches, columnname,  columns, 
                        yEditCommand, ySparseInterval, yScale,  contour_label_interval,
                        contour_label_offset, do_shade, 0, colorName, colorUnits, swap_xy, xlabelScale, ylabelScale, yRangeProvided, xRangeProvided))
        continue;
      /*restore the values after swap since they will be used for next plot */
     
      xmin = xmin0;
      xmax = xmax0;
      ymin = ymin0;
      ymax = ymax0; 
      if (swap_xy && columnmatch) {
        SWAP_PTR(xlabel, ylabel);
        SWAP_DOUBLE(xmin, ymin);
        SWAP_DOUBLE(dx, dy);
        SWAP_DOUBLE(xmax, ymax);
        SWAP_LONG(xRangeProvided, yRangeProvided);
        SWAP_DOUBLE(xlabelScale, ylabelScale);
      }
      if (!layout[0] || !layout[1]) {
        if (!frameEnded) {	 
          frame_end(1);
          frameEnded = 1;
        }
      }
      freeParameterLabel(users_xlabel, xlabel);
      freeParameterLabel(users_ylabel, ylabel);
      freeParameterLabel(users_title, title);
      freeParameterLabel(users_topline, topline); 
      if (data_value)
        SDDS_FreeMatrix((void**)data_value, nx);
      data_value = NULL;
    }
    if (buffer) free(buffer);
    if (!SDDS_Terminate(&SDDS_table)) {
      SDDS_SetError("problem closing file");
      SDDS_PrintErrors(stderr,  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    
    if (SDDS_NumberOfErrors()) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
  }
  if (!frameEnded)
    frame_end(1); 
  if (columnname) {
    for (i=0;i<columns;i++) {
      free(columnname[i]);
    }
    free(columnname);
  }
  free_scanargs(&s_arg,argc);
  return(0);
}

/* plotting of contours: */
long plot_contour(double **data_value, long nx, long ny, long verbosity,
                  double xmin, double xmax, double ymin, double ymax,
                  double dx, double dy, double *xintervals, double *yintervals,
                  char *device, long *frameEnded, char *title, char *xlabel, 
                  char *ylabel, char *topline, double min_level, double max_level, 
                  long levels, double hue0, double hue1, long *layout, 
                  long *ixl, long *iyl, long thickness, unsigned long long tsetFlags, 
                  SHAPE_DATA *shape, long shapes, int *pen, long *flags, 
                  long pause_interval, long columnmatches, char **columnname, long columns, 
                  char *yEditCommand, long ySparseInterval, double yScale, long contour_label_interval,
                  long contour_label_offset, long do_shade, long waterfall, char *colorName, 
                  char *colorUnits, long swap_xy, double xlabelScale, double ylabelScale, long yRangeProvided, long xRangeProvided) {
  long i,j, ix_min=0, ix_max=0, iy_min=0, iy_max=0,gray=0;
  double max_value, min_value, *level;
  register double value;

  level=NULL;
  max_value = -DBL_MAX;
  min_value = DBL_MAX;
  for (i=nx-1; i>=0; i--) {
    for (j=ny-1; j>=0; j--) {
      value = data_value[i][j];
      if (min_value>value) {
        ix_min = i;
        iy_min = j;
        min_value = value;
      }
      if (max_value<value) {
        ix_max = i;
        iy_max = j;
        max_value = value;
      }
    }
  }
#ifndef COMPILE_AS_SUBROUTINE
  if (verbosity>1) {
    printf("maximum value is %e, at x=%e, y=%e\n",
           max_value, ix_max*dx+xmin, iy_max*dy+ymin);
    printf("minimum value is %e, at x=%e, y=%e\n",
           min_value, ix_min*dx+xmin, iy_min*dy+ymin);
    if (strncmp(device, "regis", strlen(device))==0) {
      fputs("hit return to continue.", stdout);
      getchar();
    }
  }
#endif
  if (!xintervals) {
    xintervals = malloc(sizeof(double) * nx);
    for (i=0; i<nx; i++)
      xintervals[i] = xmin + dx * i;
  }
  if (!yintervals) {
    yintervals = malloc(sizeof(double) * ny);
    for (i=0; i<ny; i++)
      yintervals[i] = ymin + dy * i;
  }
  set_mapping(0.0, 0.0, 0.0, 0.0);
  *frameEnded = 0;
  if (min_level==max_level) {
    min_level=min_value;
    max_level=max_value;
  }
  if (do_shade) {
    if(do_shade ==2)
       gray=1;
    go_shade_grid(device, title, xlabel, ylabel, topline,
                  data_value, xmin, xmax, ymin, ymax, xintervals, yintervals, nx, ny, 
                  min_level, max_level, levels?levels:100,
                  hue0, hue1, layout, *ixl, *iyl,
                  NULL, pen, *flags, pause_interval,
                  thickness, tsetFlags, colorName, colorUnits, xlabelScale, ylabelScale,gray);
    *flags |= DEVICE_DEFINED;
    PlotShapesData(shape, shapes, xmin, xmax, ymin, ymax);
  } else {
    if (!(level = fill_levels(&level, 
                              min_level, 
                              max_level, 
                              levels?levels:10)))
      return 0;
    go_plot_contours(device, title, xlabel, ylabel, topline,
                     data_value, xmin, xmax, ymin, ymax, 
                     dx, dy, nx, ny, level, levels?levels:10,
                     contour_label_interval, contour_label_offset, 
                     layout, *ixl, *iyl, NULL, pen, 
                     *flags, pause_interval,
                     shape, shapes, tsetFlags, xlabelScale, ylabelScale);
    *flags |= DEVICE_DEFINED;
  }
  if (columnmatches) {
    if (!swap_xy && !yRangeProvided)
      make_enumerated_yscale(columnname, NULL, columns, yEditCommand, ySparseInterval, yScale, thickness, ylabel, ylabelScale);
    else if (swap_xy && !xRangeProvided)
      make_enumerated_xscale(columnname, NULL, columns, yEditCommand, ySparseInterval, yScale, thickness, xlabel, xlabelScale);
  }
  
  
  if (xintervals) free(xintervals);
  if (yintervals) free(yintervals);
  xintervals = NULL;
  yintervals = NULL;
  
  if (layout[0] && layout[1]) {
    if (++(*ixl) == layout[0]) {
      *ixl = 0;
      if (++(*iyl) == layout[1]) {
        *iyl = 0;
        frame_end(1);
        *frameEnded = 1;
      }
    }
  }
  return 1;
}
  

double *fill_levels(double **level, double min, double max, long levels)
{
  long i;
  double delta;
  if (levels<=0)
    return(NULL);
  *level = trealloc(*level, sizeof(**level)*levels);
  if (levels>1)
    delta = (max-min)/(levels-1);
  else
    delta = 0;
  for (i=0; i<levels; i++)
    (*level)[i] = min+i*delta;
  return(*level);
}

void getDimensionParameters(SDDS_TABLE *SDDS_table, char *name_of_name, char **variable, 
                            char **variableUnits,
                            double *minimum, double *interval, int32_t *number)
{
  static char s[SDDS_MAXLINE], message[SDDS_MAXLINE];
  long index;
  char **ptr, *units;
  double dValue;

  if (!name_of_name)
    bomb("improper calling of get_dimension_parameters", NULL);
  if (!(ptr=SDDS_GetParameter(SDDS_table, name_of_name, NULL))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb("Unable to get dimension parameters");
  }
  if (*variable)
    free(*variable);
  *variable = *ptr;
  sprintf(s, "%sInterval", *variable);
  if ((SDDS_GetParameterIndex(SDDS_table, s))<0 ||
      !SDDS_FLOATING_TYPE(SDDS_GetNamedParameterType(SDDS_table, s))) {
    sprintf(message, "Problem with parameter %s---check existence.  Should be floating type", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb(message);
  }
  
  sprintf(s, "%sDimension", *variable);
  if ((SDDS_GetParameterIndex(SDDS_table, s))<0 ||
      !SDDS_INTEGER_TYPE(SDDS_GetNamedParameterType(SDDS_table, s))) {
    sprintf(message, "Problem with parameter %s---check existence.  Should be integer type", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb(message);
  }
  
  sprintf(s, "%sMinimum", *variable);
  if (((index=SDDS_GetParameterIndex(SDDS_table, s)))<0 ||
      !SDDS_FLOATING_TYPE(SDDS_GetNamedParameterType(SDDS_table, s))) {
    sprintf(message, "Problem with parameter %s---check existence.  Should be floating type", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    SDDS_Bomb(message);
  }
  if (*variableUnits)
    free(*variableUnits);
  if (SDDS_GetParameterInformation(SDDS_table, "units", &units, SDDS_GET_BY_INDEX, index)!=
      SDDS_STRING)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!units || !strlen(units)) {
    sprintf(s, "%sUnits", *variable);
    if (SDDS_GetParameterIndex(SDDS_table, s)>=0 && !SDDS_GetParameter(SDDS_table, s, &units))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  *variableUnits = units;

  sprintf(s, "%sMinimum", *variable);
  if (!(SDDS_GetParameterAsDouble(SDDS_table, s, minimum))) {
    fprintf(stderr, "error: problem finding/casting %s\n", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  sprintf(s, "%sInterval", *variable);
  if (!(SDDS_GetParameterAsDouble(SDDS_table, s, interval))) {
    fprintf(stderr, "error: problem finding/casting %s\n", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  sprintf(s, "%sDimension", *variable);
  if (!(SDDS_GetParameterAsDouble(SDDS_table, s, &dValue))) {
    fprintf(stderr, "error: problem finding %s or with it's type\n", s);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  *number = dValue;
}



double **window_2d_array(double **data_value, double *xmin, double *xmax, double *ymin, double *ymax,
                         double dx, double dy, int32_t *nx, int32_t *ny, double *limit)
{
  double **new_data;
  long new_nx, new_ny, ix_min, iy_min, ix, iy;

  new_nx = *nx;
  new_ny = *ny;
  ix_min = 0;
  iy_min = 0;

  if (dx && limit[0]!=limit[1]) {
    if (limit[0]< *xmin)
      limit[0] = *xmin;
    else
      limit[0] = (ix_min=(long)((limit[0]- *xmin)/dx))*dx + *xmin;
    if (limit[1]> *xmax) {
      limit[1] = *xmax;
      new_nx -= ix_min;
    }
    else {
      new_nx = (limit[1]- limit[0])/dx + 1.5;
      limit[1] = (new_nx-1)*dx +  limit[0];
    }
    if (limit[1]<=limit[0])
      bomb("horizontal scales are invalid", NULL);
    *xmin = limit[0];
    *xmax = limit[1];
  }
  if (dy && limit[2]!=limit[3]) {
    if (limit[2]< *ymin)
      limit[2] =  *ymin;
    else
      limit[2] = (iy_min=(long)((limit[2]- *ymin)/dy))*dy +  *ymin;
    if (limit[3]> *ymax) {
      limit[3] =  *ymax;
      new_ny -= iy_min;
    }
    else {
      new_ny = (limit[3]- limit[2])/dy + 1.5;
      limit[3] = (new_ny-1)*dy +  limit[2];
    }
    if (limit[3]<=limit[2])
      bomb("vertical scales are invalid", NULL);
    *ymin = limit[2];
    *ymax = limit[3];
  }
  if (dx==0 && dy==0) {
    return data_value;
  } else {
    new_data = (double**)zarray_2d(sizeof(double), new_nx, new_ny);
    for (ix=ix_min; ix<new_nx+ix_min; ix++)
      for (iy=iy_min; iy<new_ny+iy_min; iy++)
        new_data[ix-ix_min][iy-iy_min] = data_value[ix][iy];
    free_zarray_2d((void**)data_value, *nx, *ny);
    *nx = new_nx;
    *ny = new_ny;
    return(new_data);
  }
}

void checkParameter(SDDS_TABLE *SDDS_table, char *parameter_name)
{
  long index;
  if (!parameter_name) {
    fprintf(stderr, "parameter name is null\n");
    exit(1);
  }
  if ((index=SDDS_GetParameterIndex(SDDS_table, parameter_name))<0) {
    fprintf(stderr, "error: unable to make labels from parameter %s\n",
            parameter_name);
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit (1);
  }
  if (SDDS_GetParameterType(SDDS_table, index)!=SDDS_STRING) {
    fprintf(stderr, "error: unable to make labels from parameter %s--must be string type\n",
            parameter_name);
    exit (1);
  }
}

char *getParameterLabel(SDDS_TABLE *SDDS_table, char *parameter_name)
{
  char *ptr;
  
  if (!SDDS_GetParameterAsString(SDDS_table, parameter_name, &ptr)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit (1);
  }
  return(ptr);
}

void checkLabelParameters(SDDS_TABLE *SDDS_table,
                          char *p1, char *p2, char *p3, char *p4)
{
  if (p1 && strlen(p1)>1 && p1[1]=='@')
    checkParameter(SDDS_table, p1+1);
  if (p2 && strlen(p2)>1 && p2[1]=='@')
    checkParameter(SDDS_table, p2+1);
  if (p3 && strlen(p3)>1 && p3[1]=='@')
    checkParameter(SDDS_table, p3+1);
  if (p4 && strlen(p4)>1 && p4[1]=='@')
    checkParameter(SDDS_table, p4+1);
}

void freeParameterLabel(char *users_label, char *label)
{
  if (users_label && users_label[0]=='@' && label)
    free(label);
}

void make_waterfall_yscale(long interval, double scale, long ny, double min_y, double max_y, double dy,  long thickness)
{
  double hsave, vsave, hsize, vsize;
  double xmin, xmax, ymin, ymax, deltay;
  double x, y, yrange, xrange;
  double pmin, pmax, qmin, qmax, value;
  double wpmin, wpmax, wqmin, wqmax;
  char label[256];
  double tickFraction=0.0125;
  long i, digits=0;
  char format[256], *temp=NULL;

  get_mapping(&xmin, &xmax, &ymin, &ymax);
  get_pspace(&pmin, &pmax, &qmin, &qmax);
  get_wspace(&wpmin, &wpmax, &wqmin, &wqmax);
  
  yrange = ymax-ymin;
  xrange = xmax-xmin;
  deltay = yrange/(ny*1.0);
  
  get_char_size(&hsave, &vsave, 1);
  vsize = vsave;
  hsize = hsave;
  if (yrange<ny*1.5*vsize/interval) {
    vsize = yrange/ny/1.5;
    hsize = hsave/vsave*vsize;
  }
  find_format(format, min_y, max_y, dy, 0);
  temp=strchr(format,'.');
  temp++;
  xrange = xrange/(pmax-pmin)*(pmin-wpmin)*0.8; 
  sscanf(temp,"%ld",&digits);
  if (log10(max_y)>0)
    digits=(long)(log10(max_y))+2+digits;
  if (xrange < digits*hsize) {
    hsize = xrange/digits;
    vsize = vsave/hsave*hsize;
  }
  hsize *= scale;
  vsize *= scale;
  
  set_char_size(hsize, vsize, 1);
  
  widen_window(1);
  i = 0;
  x = xmin - xrange*0.05;
  while (ny--) {
    value = min_y + ny * dy;
    y=value;
    if ((i++%interval)!=0)
      continue;
    set_linethickness(thickness);
    sprintf(label,format,value);
    jplot_string(&x, &y, label, 'r');
    make_ticks('y', y, 1, 1, xmin, tickFraction*(xmax-xmin), 0, thickness);
    make_ticks('y', y, 1, 1, xmax, -tickFraction*(xmax-xmin), 0, thickness);
  }
  set_char_size(hsave, vsave, 1);
}

void make_enumerated_yscale(char **label0, double *yposition, long labels, char *yEdit, long interval, double scale, long thickness, char *ylabel, double ylabelScale)
{
  double hsave, vsave, hsize, vsize;
  double xmin, xmax, ymin, ymax;
  double x, y, yrange, xrange;
  double pmin, pmax, qmin, qmax;
  double wpmin, wpmax, wqmin, wqmax;
  long i, maxlen;
  char buffer[1024], **label;
  double tickFraction=0.0125;

  
  get_mapping(&xmin, &xmax, &ymin, &ymax);
  get_pspace(&pmin, &pmax, &qmin, &qmax);
  get_wspace(&wpmin, &wpmax, &wqmin, &wqmax);
 
  yrange = ymax-ymin;
  xrange = xmax-xmin;
  label_character_size(1);
  get_char_size(&hsave, &vsave, 1);
  vsize = vsave;
  hsize = hsave;
  if (yrange<labels*1.5*vsize/interval) {
    vsize = yrange/labels/1.5;
    hsize = hsave/vsave*vsize;
  }
  label = tmalloc(sizeof(*label)*labels);
  for (i=maxlen=0; i<labels; i++) {
    if (yEdit) {
      strcpy(buffer, label0[i]);
      edit_string(buffer, yEdit);
      SDDS_CopyString(&label[i], buffer);
    }
    else
      SDDS_CopyString(&label[i], label0[i]);
    if ((long)strlen(label[i])>maxlen)
      maxlen = strlen(label[i]);
  }
  xrange = xrange/(pmax-pmin)*0.8*(pmin-wpmin);
  if (xrange < maxlen*hsize) {
    hsize = xrange/maxlen;
    vsize = vsave/hsave*hsize;
  }
  
  hsize *= scale;
  vsize *= scale;
  set_char_size(hsize, vsize, 1);
  
  widen_window(1);
  i = 0;
  while (labels--) {
    if (i++%interval!=0) {
      free(label[labels]);
      continue;
    }
    x = xmin - xrange*0.05;
    if (yposition)
      y = yposition[labels];
    else
      y = labels;
    set_linethickness(thickness);
    jplot_string(&x, &y, label[labels], 'r');
    make_ticks('y', y, 1, 1, xmin, tickFraction*(xmax-xmin), 0, thickness);
    make_ticks('y', y, 1, 1, xmax, -tickFraction*(xmax-xmin), 0, thickness);
    free(label[labels]);
  }
  free(label);
  set_char_size(hsave, vsave, 1);
  xrange=maxlen*hsize;
  
  if (ylabel) {
    get_char_size(&hsize, &vsize, 1);
    hsize *=ylabelScale;
    vsize *=ylabelScale;
    maxlen=strlen(ylabel);
    if (yrange<maxlen*vsize) {
      vsize=yrange/maxlen;
      hsize=hsave/vsave*vsize;
    }
    x = x -(xmax-xmin)*0.15;
    y = ymin + (ymin+ymax)/2.0;
    vertical_print(1);
    set_char_size(hsize, vsize, 1);
    jxyplot_string(&x, &y, ylabel, 'c', 'c');  
    set_char_size(hsave, vsave, 1);
    vertical_print(0);
  }
}

void make_enumerated_xscale(char **label0, double *xposition, long labels, char *yEdit, long interval, double scale, long thickness, char *xlabel, double xlabelScale)
{
  double hsave, vsave, hsize, vsize;
  double xmin, xmax, ymin, ymax;
  double x, y, yrange, xrange;
  double pmin, pmax, qmin, qmax;
  long i, maxlen;
  char buffer[1024], **label;
  double tickFraction=0.02;
  
  label_character_size(1);
  get_char_size(&hsave, &vsave, 1);
  label_character_size(0);
  hsize = hsave;
  vsize = vsave;
  
  get_mapping(&xmin, &xmax, &ymin, &ymax);
  get_pspace(&pmin, &pmax, &qmin, &qmax);
  yrange = ymax-ymin;
  xrange = xmax-xmin;
  if (xrange<labels*1.5*hsize/interval) {
    hsize = xrange/labels/1.5;
    vsize = vsave/hsave*hsize;
  }
  label = tmalloc(sizeof(*label)*labels);
  for (i=maxlen=0; i<labels; i++) {
    if (yEdit) {
      strcpy(buffer, label0[i]);
      edit_string(buffer, yEdit);
      SDDS_CopyString(&label[i], buffer);
    }
    else
      SDDS_CopyString(&label[i], label0[i]);
    if ((long)strlen(label[i])>maxlen)
      maxlen = strlen(label[i]);
  }
  hsize *= scale;
  vsize *= scale;
  yrange= yrange*0.1;
  if (yrange<vsize*maxlen) {
    vsize=yrange/maxlen;
    hsize=hsave/vsave*vsize;
  }  
  set_char_size(hsize, vsize, 1);
  widen_window(1);
  vertical_print(1);
  i = 0;
  while (labels--) {
    if (i++%interval!=0) {
      free(label[labels]);
      continue;
    }
    y = ymin-0.1*yrange;    
    if (xposition)
      x = xposition[labels];
    else
      x = labels;
    if (x<xmin || x>xmax)
      continue;
    set_linethickness(thickness);
    jxyplot_string(&x, &y, label[labels], 'r', 'c');
    make_ticks('x', x, 1, 1, ymin, tickFraction*(ymax-ymin), 0, thickness);
    make_ticks('x', x, 1, 1, ymax, -tickFraction*(ymax-ymin), 0, thickness);
    free(label[labels]);
  }
  free(label);
  vertical_print(0);
  set_char_size(hsave, vsave, 1);
  if (xlabel) {
    get_char_size(&hsize, &vsize, 1);
    hsize *=xlabelScale;
    vsize *=xlabelScale;
    y -= yrange;
    maxlen=strlen(xlabel);
    if (xrange<hsize*maxlen) {
      hsize=xrange/maxlen;
      vsize=vsave/hsave*hsize;
    }
    x = xmin + xrange/2.0;
    set_char_size(hsize, vsize, 1);
    jxyplot_string(&x, &y, xlabel, 'c', 't');  
    set_char_size(hsave, vsave, 1);
  }
}

void ReadShapeData(SHAPE_DATA *shape, long shapes, long swapXY)
{
  long iFile, page, code;
  SDDS_DATASET SDDSin;
  for (iFile=0; iFile<shapes; iFile++) {
    if (!SDDS_InitializeInput(&SDDSin, shape[iFile].filename))
      SDDS_Bomb("problem reading shape file");
    shape[iFile].xData = shape[iFile].yData = NULL;
    shape[iFile].nPages = 0;
    shape[iFile].nPoints = NULL;
    page = 0;
    if (swapXY)
      SWAP_PTR(shape[iFile].xColumn, shape[iFile].yColumn);
    while ((code=SDDS_ReadPage(&SDDSin))>0) {
      if (!(shape[iFile].xData = SDDS_Realloc(shape[iFile].xData,
                                              sizeof(*(shape[iFile].xData))*(page+1))) ||
          !(shape[iFile].yData = SDDS_Realloc(shape[iFile].yData,
                                              sizeof(*(shape[iFile].yData))*(page+1))) ||
          !(shape[iFile].nPoints = SDDS_Realloc(shape[iFile].nPoints,
						sizeof(*(shape[iFile].nPoints))*(page+1))))
        SDDS_Bomb("Memory allocation failure reading shape file.");
      if ((shape[iFile].nPoints[page] = SDDS_RowCount(&SDDSin))<=0)
        continue;
      if (!(shape[iFile].xData[page] = 
            SDDS_GetColumnInDoubles(&SDDSin, shape[iFile].xColumn)) ||
          !(shape[iFile].yData[page] =
            SDDS_GetColumnInDoubles(&SDDSin, shape[iFile].yColumn)))
        SDDS_Bomb("Problem getting column data from shape file.");
      page++;
    }
    shape[iFile].nPages = page;
    if (!SDDS_Terminate(&SDDSin))
      SDDS_Bomb("Problem terminating shape file.");
  }
}

char *addOuterParentheses(char *arg) 
{
  char *ptr;
  ptr = tmalloc(sizeof(*ptr)*(strlen(arg)+2));
  sprintf(ptr, "(%s)", arg);
  return ptr;
}

long get_plot_labels(SDDS_DATASET *SDDS_table, char *indepcolumn, char **columnname, long columns, 
                     char *allmatches, char *waterfall_par, 
                     char *users_xlabel, char *users_ylabel, char *users_title,
                     char **xlabel0, char **ylabel0, char **title0, long deltas, long xRangeProvided) {
  char *columnunits=NULL;
  char s[1024];
  long i, is_par=0;
  char *xlabel, *ylabel, *title, *units;
  
  xlabel=ylabel=title=units=NULL;
  SDDS_GetColumnInformation(SDDS_table, "units", &columnunits, SDDS_GET_BY_NAME,
                            columnname[0]);
  if (!xRangeProvided) {
    if (SDDS_GetColumnIndex(SDDS_table,indepcolumn)<0) {
      if (SDDS_GetParameterIndex(SDDS_table,indepcolumn)<0) {
	fprintf(stderr,"%s is neither a column or a parameter!\n", indepcolumn);
	exit(1);
      }
      is_par=1;
    }
  }
  if (!users_ylabel) {
    if (!waterfall_par)
      SDDS_CopyString(&ylabel, "");
    else
      SDDS_CopyString(&ylabel, waterfall_par);
  }
  else
    SDDS_CopyString(&ylabel, users_ylabel);
  if (!users_xlabel) {
    if (!xRangeProvided) {
      if (is_par) 
        SDDS_GetParameterInformation(SDDS_table, "units", &xlabel, SDDS_GET_BY_NAME, indepcolumn);
      else
        SDDS_GetColumnInformation(SDDS_table, "units", &xlabel, SDDS_GET_BY_NAME, indepcolumn);
    }
    if (xlabel && xlabel[0])
      sprintf(s, "%s (%s)", indepcolumn, xlabel);
    else
      sprintf(s, "%s", indepcolumn);
    free(xlabel);
    SDDS_CopyString(&xlabel, s);
  }
  else
    SDDS_CopyString(&xlabel, users_xlabel);
  if (!users_title) {
    units = NULL;
    if (columnunits) {
      for (i=1; i<columns; i++) {
        SDDS_GetColumnInformation(SDDS_table, "units", &units, SDDS_GET_BY_NAME, 
                                  columnname[i]);
        if (!units || strcmp(columnunits, units)!=0)
          break;
        if (units)
          free(units);
      }
      if (i!=columns) 
        SDDS_CopyString(&units, "(V.U.)");
      else {
        units = tmalloc(sizeof(*units)*(strlen(columnunits)+3));
        sprintf(units, "(%s)", columnunits);
      }
    }
    else
      SDDS_CopyString(&units, "");
    switch (deltas) {
    case DELTAS_FRACTIONAL:
      sprintf(s, "$gD$r%s(fractional) as a function of %s",
              allmatches, indepcolumn);
      break;
    case DELTAS_PLAIN:
      sprintf(s, "$gD$r%s%s as a function of %s",
                  allmatches, units, indepcolumn);
      break;
    case DELTAS_NORMALIZED:
      sprintf(s, "%s(normalized)  as a function of %s",
              allmatches, indepcolumn);
      break;
    default:
      sprintf(s, "%s%s as a function of %s",
              allmatches, units, indepcolumn);
      break;
    }
    if (units!=columnunits)
      free(units);
    SDDS_CopyString(&title, s);
  }
  else
    SDDS_CopyString(&title, users_title);
  if (SDDS_NumberOfErrors())
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (columnunits) free(columnunits);
  *xlabel0=xlabel;
  *ylabel0=ylabel;
  *title0=title;
  return 1;
}

long process_data_values(double ***data_value0, long nx, long ny, long deltas)
{
  long ix,iy;
  double average, min, max, range, factor, **data_value;
  if (deltas<0)
    return 0;
  data_value=*data_value0;
  for (iy=0; iy<ny; iy++) {
    average = 0;
    max = -DBL_MAX;
    min = DBL_MAX;
    for (ix=nx-1; ix>=0; ix--) {
      average += data_value[ix][iy];
      if (max<data_value[ix][iy])
        max = data_value[ix][iy];
      if (min>data_value[ix][iy])
        min = data_value[ix][iy];
    }
    average /= nx;
    switch (deltas) {
    case DELTAS_PLAIN:
      for (ix=nx-1; ix>=0; ix--)
        data_value[ix][iy] -= average;
      break;
    case DELTAS_FRACTIONAL:
      for (ix=nx-1; ix>=0; ix--)
        data_value[ix][iy] = (data_value[ix][iy]-average)/average;
      break;
    case DELTAS_NORMALIZED:
      if ((range = max-min))
        factor = 1/range;
      else
        factor = 0;
      for (ix=nx-1; ix>=0; ix--)
        data_value[ix][iy] = (data_value[ix][iy]-average)*factor;
      break;
    default:
      fprintf(stderr, "error: invalid -deltas code %ld detected\n",
              deltas);
      return(1);
    }
  }
  return 0;
}

void process_data(double ***data_value0, int32_t *nx, int32_t *ny, double *xmin, double *xmax,
                  double *ymin, double *ymax, double *dx, double *dy, 
                  double *orig_limit, long logscale, double logfloor,
                  long nx_interp, long ny_interp, long x_lowpass, long y_lowpass, long interp_flags,
                  char **xyzArray, long verbosity)
{
  long i, j;
  double limit[4], **data_value, xmim0, xmax0, ymax0, ymin0;
  
  data_value=*data_value0;
  if (logscale) {
    for (i=0; i<*nx; i++)
      for (j=0; j<*ny; j++)
        data_value[i][j] = (data_value[i][j]<=0?(logfloor==0?-300:log10(logfloor)):log10(data_value[i][j]));
  }
  *xmax = *xmin + ((*nx)-1)*(*dx);
  *ymax = *ymin + ((*ny)-1)*(*dy);
  if (orig_limit[0]!=orig_limit[1] || orig_limit[2]!=orig_limit[3]) {
    memcpy((char*)limit, (char*)orig_limit, 4*sizeof(limit[0]));
    data_value = window_2d_array(data_value, xmin, xmax, ymin, ymax,
                                 *dx, *dy, nx, ny, limit);
  }
  if (nx_interp!=0 || ny_interp!=0) {
    if (xyzArray[0]) {
      fprintf(stderr, "warning: interpolation done using static dx and dy\n");
    }
    if (nx_interp!=1 || x_lowpass>0) {
#ifndef COMPILE_AS_SUBROUTINE
      if (verbosity>1 && nx_interp>1)
        printf("interpolating on %ld-times finer grid in x\n", nx_interp);
      if (verbosity>1 && x_lowpass>0)
        printf("low-pass filtering with cutoff at %ld steps below maximum x frequency\n", x_lowpass);
#endif
      data_value = fft_interpolation_index1(data_value, *nx, *ny, nx_interp, x_lowpass, interp_flags);
      *dx /= nx_interp;
      *nx = nx_interp*((*nx)-1)+1;
    }
    if (ny_interp!=1 || y_lowpass>0) {
#ifndef COMPILE_AS_SUBROUTINE
      if (verbosity>1 && ny_interp>1)
        printf("interpolating on %ld-times finer grid in y\n", ny_interp);
      if (verbosity>1 && y_lowpass>0)
        printf("low-pass filtering with cutoff at %ld steps below maximum y frequency\n", y_lowpass);
#endif
      data_value = fft_interpolation_index2(data_value, *nx, *ny, ny_interp, y_lowpass, interp_flags);
      (*dy) /= ny_interp;
      (*ny) = ny_interp*((*ny)-1)+1;
    }
  }
  *data_value0=data_value;
}

char * rearrange_by_index(char *data,long *index,long element_size,long num) {

   char * tmpdata = NULL; 
   long i, j,offset;
      
   tmpdata = tmalloc(element_size*num);
     
   offset = element_size/sizeof(char);
   
   for(i=0;i<num;i++) {
      for(j=0;j<offset;j++) 
        tmpdata[i*offset+j] = data[index[i]*offset+j];
   }
   
   return (tmpdata);
}
