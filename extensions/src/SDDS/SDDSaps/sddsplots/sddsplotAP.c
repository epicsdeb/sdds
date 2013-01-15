/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: sddsplot_AP.c
 * 
 * argument parsing for sddsplot
 *
 * Michael Borland, 1994.
 $Log: not supported by cvs2svn $
 Revision 1.93  2011/01/11 22:51:03  soliday
 Changed all the strcpy commands to strcpy_ss because of problems with
 RedHat Enterprise 6. If a strcpy copies the result to the same memory
 space you will get unexpected results.

 Revision 1.92  2010/09/14 16:20:11  soliday
 Added the rspectral order for the colors. This is the reverse of the spectral
 order.

 Revision 1.91  2010/07/08 21:26:47  borland
 Added "first" qualifier to -omnipresent option.  When given, omnipresent datasets
 are plotted first.  The default is to plot them last.

 Revision 1.90  2010/06/03 16:33:45  borland
 Added fixForRequest qualifier to -graphic option.

 Revision 1.89  2010/01/05 20:06:46  soliday
 Added the -intensityBar option which can be used to move the intensity bar
 and to resize the label text to the left and the unit text above.

 Revision 1.88  2009/07/28 14:01:16  borland
 Added scroll feature to -replicate option.  Greatly improved memory management for
 split and replicated datasets.  Added -nocolorbar option.

 Revision 1.87  2009/06/26 18:58:28  borland
 Added scrolled mode to the -replicate option.  Intended use:
 sddsplot -layout=2,2 -groupby=page,req -sep=names \
          -col=t,p -col=x,xp -col=y,yp -title=@Pass run.w1 -graph=dot -split=page \
          -column=Pass,Sdelta run.w2 -replicate=match=page,scroll -same -graph=sym,con
 where run.w1 is a phase space file and run.w2 is a turn-by-turn parameters file.

 Revision 1.86  2009/06/17 21:40:19  borland
 Added fixForFile qualifier to -graphic option.  Allows using syntax like
 -column=x,y <filenames> -graph=line,vary,fixforfile -split=page and getting
 fixed line type for data from each file.

 Revision 1.85  2009/04/29 14:15:59  borland
 Added nosubtype qualifier to -legend option, to allow suppressing the subtype in the legend.

 Revision 1.84  2009/04/28 15:08:33  shang
 fixed the segmentation error when -subtick=<number> option is provided.

 Revision 1.83  2009/03/09 14:43:15  borland
 Some corrections to autolog mode.

 Revision 1.82  2009/02/27 22:51:54  borland
 Added autolog mode.

 Revision 1.81  2008/08/22 18:50:36  soliday
 Updated so that no additional sub-options are needed for the
 -split=columnBin=<column-name> option. It will now automatically
 bin the column data in 100 equal size bins from the min to max values.

 Revision 1.80  2007/06/13 20:09:54  soliday
 Fixed a memory access error when the column command has an extra comma at the end.
 It now prints out an error message.

 Revision 1.79  2007/03/08 18:54:40  soliday
 Added the ability to vary both the type and subtype at the same time.

 Revision 1.78  2006/10/20 14:57:57  jiaox
 1) Added format key word for -topline and -legend options.If given with paramteter keyword (or @), the format will be applied to the parameter output. If not given, but there is format_string for the parameter in
 the SDDS file, it output will take that format_string for output.

 2) Fixed a problem that when multiple file with multiple pages are plotted with -split=page -order=Spectral and eachpage keyword is used in -graphic option, only part of the spectra is actually used in the plot.

 Revision 1.77  2005/11/04 22:46:46  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.76  2005/10/04 22:04:30  borland
 Fixed usage message for -timeFilter option.

 Revision 1.75  2005/08/29 21:31:25  shang
 added linetype to xlabel, ylabel, topline and title options

 Revision 1.74  2005/08/19 13:18:06  borland
 Fixed minor code style issue.

 Revision 1.73  2005/08/15 14:11:28  jiaox
 Added keyword "fill" for graphic option. Also added circel type(9) for symbols. Fixed the bug that caused incorrect plotting if verticalprint is
 used.

 Revision 1.72  2005/02/22 17:49:30  shang
 added feature of plotting labels of sub-tick for log-scale axis

 Revision 1.71  2005/01/22 22:35:25  borland
 Added thickness qualifier to pointLabel option.

 Revision 1.70  2004/08/02 18:43:28  borland
 Added -dither option.  It can be used to make scatter plots look more like
 density plots, particularly for digitized data.

 Revision 1.69  2004/04/20 17:18:06  shang
 modified plotnames_AP1 to be able to accept fewer y1names than yname and accept
 typ1 y1edit feature for obtainin y1name from editing yname

 Revision 1.68  2004/03/26 19:46:09  shang
 added plotExclude_AP(), xExclude_columnnames_AP, yExclude_columnnames_AP,
 fromPage_AP() and toPage()

 Revision 1.67  2003/11/13 18:37:42  soliday
 Fixed bug with -clip option when only one item is passed.

 Revision 1.66  2003/09/02 19:29:28  soliday
 Cleaned up code for Linux.

 Revision 1.65  2003/04/14 21:21:01  borland
 Commited from Shang's directory (time filter).

 Revision 1.64  2002/12/04 00:34:49  borland
 Added fractional-deviation mode to -mode option:
         -mode=[xy]FractionalDeviation
 Added value-from-file and invert features to -factor option:
         -factor=[xy]Parameter=<name>,[xy]Invert
 Fixed invert feature for -offset option (was ignored when the offset
 was given literally but used when given as parameter).

 Revision 1.63  2002/10/30 17:58:36  borland
 Added to -string option the ability to take position values from
 parameters in the file.

 Revision 1.62  2002/08/14 17:24:54  soliday
 Added Open License

 Revision 1.61  2002/07/18 22:36:32  borland
 Added "end offset" mode.

 Revision 1.60  2002/07/03 15:19:30  shang
 modified tagrequest_AP() to get tag value as string

 Revision 1.59  2002/07/01 18:38:23  borland
 Can now use wildcards for x quantity and templates for y.

 Revision 1.58  2002/06/05 20:25:05  shang
 fixed UMR errors by setting sspec->thickness=0 in string_AP()

 Revision 1.57  2002/04/22 20:33:45  soliday
 Added common offset mode.

 Revision 1.56  2002/01/11 03:28:28  borland
 Fixed problem with global thickness and -string commands.

 Revision 1.55  2002/01/11 03:15:46  borland
 Fixed problems with -thickness option when tick or label otpions where given in
 one of the plot requests.

 Revision 1.54  2002/01/10 02:29:46  borland
 Added -thickness option, which permits specifying the default thickness
 for all other options (expect -graphic).  Also, fixed bug in thickness
 control for the -xlabel (etc.) options (a flag was reused).

 Revision 1.53  2002/01/10 02:02:17  borland
 Added -rowlimit option to force program to ignore pages with more than a
 defined number of rows.

 Revision 1.52  2001/10/12 18:27:44  soliday
 Added the -nextPage option to split requests.

 Revision 1.51  2001/10/09 21:30:27  soliday
 Added the thickness option to the -string option.

 Revision 1.50  2001/08/13 21:25:29  borland
 Fixed bug in -device option.  Wasn't recognizing multiple device arguments.

 Revision 1.49  2001/08/13 20:11:53  soliday
 Added thickness options for several options.

 Revision 1.48  2000/11/01 13:50:06  borland
 Fixed bug in parsing of -mode option that resulted in resetting all prior
 settings if x=lin or y=lin was given.

 Revision 1.47  2000/09/14 17:19:50  soliday
 Changed default fraction size in ticksettings_AP

 Revision 1.46  2000/07/07 16:00:29  soliday
 Fixed -drawLine usage message. Also fixed a bug with the thickness of -drawLine
 option.

 Revision 1.45  2000/06/29 14:54:30  borland
 Restored function of -noscales option; however, it still won't work properly
 for multiple scales.  Fixed usage message for -[xy]scalesgroup options.

 Revision 1.44  2000/06/09 20:54:15  soliday
 -orderColors is no longer a global option.

 Revision 1.43  2000/06/08 17:39:01  soliday
 Added spectral colors.

 Revision 1.42  2000/06/07 20:14:39  soliday
 Added option for custom colors.

 Revision 1.41  2000/06/02 20:58:16  soliday
 Added thickness option to lineTypeDefault, drawLine, axes, tick and subtick
 options.

 Revision 1.40  2000/05/16 19:42:41  soliday
 Cleaned up some Solaris compiler warnings.

 Revision 1.39  2000/05/02 18:40:37  borland
 Fixed a egregious bug in alignZeros (argument parsing) and added some diagnostics.

 Revision 1.38  2000/04/27 19:30:11  soliday
 Added support for line thickness.

 Revision 1.37  2000/01/10 19:04:00  borland
 Improved function of -alignZero option.

 Revision 1.36  1999/11/30 22:48:07  borland
 Added fixForName qualifier to -graph option.  Allows fixing the line/symbol/etc
 type for a yname.  Fixed bug in legend creation (was duplicating the same
 legend in cases where the legend string had special sequences).

 Revision 1.35  1999/11/09 15:04:12  borland
 Added clip qualifier to -drawLines option so that lines needn't be shown
 if outside the pspace.

 Revision 1.34  1999/07/21 18:07:25  borland
 Eliminated many memory leaks.  Program is now suitable for running with
 -repeat option, although not all leaks are gone.

 Revision 1.33  1999/06/03 16:07:46  soliday
 Removed compiler warnings under linux.

 Revision 1.32  1999/05/18 20:33:56  borland
 Now allows width parameter for -split to be <0.

 Revision 1.31  1999/02/03 19:38:50  borland
 Improved operation of -range option when the center/range is undefined.
 Improved operation of time-scales when the time range is undefined.

 Revision 1.30  1998/10/01 22:48:08  borland
 Reimplemented scale and offset qualifiers for -xlabel and -ylabel.
 Implemented {xy}scaleChars qualifier for -ticksettings.
 These options allow users to control the size of characters on scale labels
 and ticks labels.

 Revision 1.29  1998/09/08 18:12:32  borland
 Added slant qualifier to -strings option.

 Revision 1.28  1998/09/02 14:09:06  borland
 Added -alignZero option.  Also added delay qualifier to -zoom option.
 Changed implementation of panel scale determination.  Also modified
 arguments to make_scale() to permit inverted scales.

 Revision 1.27  1998/08/25 15:05:33  borland
 New version with major changes to allow multiple scales on x and y axes.

 Revision 1.26  1998/04/11 19:31:07  borland
 Added editcommand to -filenamesOnTopline option.

 Revision 1.25  1998/02/20 20:11:59  borland
 Added qalign and palign qualifiers for overlays.  Results in lining up
 of 0 of overlay data with 0 of pspace.

 Revision 1.24  1998/02/20 15:19:30  borland
 Improved/fixed overlays.  Now has qoffset/poffset option.   xoffset/yoffset
 no longer changes behavior from unit to normal mode.  May break some applications,
 but they'll be easy to fix.

 Revision 1.23  1998/02/18 22:39:24  borland
 Fixed bug that prevented -overlay request from being executed when
 unit mode was requested in both planes and no other flags where given.

 Revision 1.22  1997/02/05 16:21:36  borland
 Fixed usage message for -drawLine

 Revision 1.21  1997/02/05 15:59:12  borland
 Added -drawline option.

 Revision 1.20  1996/11/11 21:31:59  borland
 Added control over the type of labels for logscales: -tickSettings option
 now takes [xy]nonExponentialLabels options.

 Revision 1.19  1996/11/08 16:54:55  borland
 Implemented -replicate option.

 Revision 1.18  1996/09/30 15:56:33  borland
 Added some casts in memcpy and before malloc calls to avoid waring messages
 on SUN4.

 Revision 1.17  1996/08/14 20:30:55  borland
 Added new -groupby and -separate qualifier: inamestring.  Allows
 sorting namestrings compared from the end of the string first.

 * Revision 1.16  1996/04/30  21:06:31  borland
 * Updated usage messages.
 *
 * Revision 1.15  1996/04/10  21:37:04  borland
 * Implemented -pointLabel option for labeling data points with string values
 * from data files.
 *
 * Revision 1.14  1996/03/21  17:19:44  borland
 * Removed dependence on SDDS library (gif.trm graphics.c).
 * Added presparse option and implementation.
 *
 * Revision 1.13  1996/03/20  18:52:11  borland
 * Added improved device argument support via setDeviceArgv and getDeviceArgv
 * routines.  Backward compatible for x11 and mif devices.  Added generation
 * file feature to gif driver for multipage output.
 *
 * Revision 1.12  1996/02/15  00:13:53  borland
 * Removed capitalized versions of qualifiers from scanItemList call in
 * axes_AP().
 *
 * Revision 1.11  1996/02/14  00:59:21  borland
 * Fixed bug in read routines (wasn't resetting row flags between requests).
 * Changed argument parsing over to scanItemList form scan_item_list.
 *
 * Revision 1.10  1996/02/09  00:07:30  borland
 * Added xtime and ytime qualifiers to -ticksettings option, along with code
 * to do time-labeled axes (assumes time values are times in seconds since
 * epoch).
 *
 * Revision 1.9  1996/02/08  00:59:13  borland
 * Enhanced -offset option to allow offseting data based on values in parameters.
 *
 * Revision 1.8  1996/01/21  17:57:41  borland
 * Added -namescan option to control scanning of files for matches to wildcard
 * dataname specifications; this allows the user to cut running time when there
 * are many files with identical data elements.
 *
 * Revision 1.7  1995/12/13  21:59:58  borland
 * Updated help text for -limit and -range options.
 *
 * Revision 1.6  1995/12/08  07:08:07  borland
 * Added autoscaling qualifier to -limit option. Modified mapping routines
 * to set scales to +/- Sqrt(DBL_MAX)/2 if they are undefined; produces
 * better results when plotting multiple panels.
 *
 * Revision 1.5  1995/12/01  01:12:41  borland
 * Added global qualifier to -sameScale option; changed range determination
 * code to implement.  Added sticky feature for individual -limit fields;
 * global values for individual fields can only be overridden by local values
 * for the same field.
 *
 * Revision 1.4  1995/10/11  00:18:01  borland
 * Added ylabelontopline routine to sddsplotAP.c and supporting code in
 * sddsplotLabels.c, sddsplot.h, and sddsplot.c.
 *
 * Revision 1.3  1995/09/14  22:04:27  borland
 * Added support for yimpulse and ybar graphic types.
 *
 * Revision 1.2  1995/09/05  21:09:46  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "scan.h"
#include "graph.h"
#include "table.h"
#include "SDDS.h"
#include "sddsplot.h"
#include <ctype.h>

static char *NO_REQUESTS_MESSAGE = "no plot requests";
long defaultLineThickness = 0;

long graphic_AP1(GRAPHIC_SPEC *graphic_spec, long element, char **item, long items);
long plotnames_AP1(PLOT_SPEC *plotspec, char **item, long items, char *plotnames_usage, long class);
void add_plot_request(PLOT_SPEC *plspec);
long plotlabel_AP(PLOT_SPEC *plotspec, long label_index, char **item, long items);
long plotExclude_AP(PLOT_SPEC *plotspec, long exclude_type, char **item, long items);
long keepnames_AP1(PLOT_SPEC *plotspec);
long keepfilenames_AP1(PLOT_SPEC *plotspec);
long add_filename(PLOT_SPEC *plotspec, char *filename);
long count_chars(char *string, char c);

long thickness_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1 ||
        sscanf(item[0], "%ld", 
	       &(plotspec->plot_request[plotspec->plot_requests-1].global_thickness_default))!=1 ||
        plotspec->plot_request[plotspec->plot_requests-1].global_thickness_default<=0)
        return bombre("invalid -thickness syntax", "-thickness=<integer>", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_THICKNESS;
    return 1;
}

long aspectratio_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1 ||
        sscanf(item[0], "%lf", &(plotspec->plot_request[plotspec->plot_requests-1].aspect_ratio))!=1 ||
        plotspec->plot_request[plotspec->plot_requests-1].aspect_ratio==0)
        return bombre("invalid -aspectratio syntax", "-aspectratio=<value>", 0);
    plotspec->plot_request[plotspec->plot_requests-1].aspect_ratio =
      fabs(plotspec->plot_request[plotspec->plot_requests-1].aspect_ratio);
    return 1;
    }

long rowlimit_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  long rowLimit;
  if (items!=1 || sscanf(item[0], "%ld", &rowLimit)!=1 || rowLimit<=0)
    return bombre("invalid -rowlimit value", "-rowlimit=<integer>", 0);
  SDDS_SetRowLimit(rowLimit);
  return 1;
}

long device_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items<1)
        return bombre("invalid -device syntax", "-device=<name>[,<device-arguments>]", 0);
    SDDS_CopyString(&plotspec->device, item[0]);
    plotspec->deviceArgc = items-1;
    if (items>1) {
      if (!(plotspec->deviceArgv=(char**)malloc(sizeof(*plotspec->deviceArgv)*(items-1))) ||
          !SDDS_CopyStringArray(plotspec->deviceArgv, item+1, items-1)) {
        SDDS_Bomb("error copying device arguments");
      }
    } else
        plotspec->deviceArgv = NULL;
    return 1;
    }

#define SET_LINE_GRAPHIC 0
#define SET_SYMBOL_GRAPHIC 1
#define SET_ERRORBAR_GRAPHIC 2
#define SET_DOT_GRAPHIC 3
#define SET_IMPULSE_GRAPHIC 4
#define SET_CONTINUE_GRAPHIC 5
#define SET_BAR_GRAPHIC 6
#define SET_YIMPULSE_GRAPHIC 7
#define SET_YBAR_GRAPHIC 8
#define GRAPHIC_ELEMENTS 9
static char *graphic_element[GRAPHIC_ELEMENTS] = {
    "line", "symbol", "errorbar", "dot", "impulse", "continue", "bar",
    "yimpulse", "ybar",
    } ;

unsigned long translate_to_plotcode(GRAPHIC_SPEC graphic)
{
    unsigned long plotcode = 0;
    if (graphic.element==-1)
        bomb("graphic.element==-1 in translate_to_plotcode.  This shouldn't happen.", NULL);
    plotcode = graphic.element;
    if (graphic.connect_linetype!=-1)
        plotcode = PLOT_CSYMBOL;
    plotcode += PLOT_SIZE_MASK&(((long)graphic.scale)<<4);
    plotcode += PLOT_CODE_MASK&graphic.type;
    return(plotcode);
    }

static char *graphic_usage = "-graphic=<element>[,type=<type>][,fill][,subtype={<type> | type}][,thickness=<integer>][,connect[={<linetype> | type | subtype}]][,vary=type][,vary=subtype][,scale=<factor>][,modulus=<integer>][,eachfile][,eachpage][,eachrequest][,fixForName][,fixForFile][,fixfForRequest]\n\
<element> is one of continue, line, symbol, errorbar, impulse, yimpulse, dot, bar, or ybar.\n";

long graphic_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    if (items<1) {
        fprintf(stderr, "error: invalid -graphic syntax\nusage: %s\n", graphic_usage);
        return 0;
        }
    
    switch (match_string(item[0], graphic_element, GRAPHIC_ELEMENTS, 0)) {
      case SET_CONTINUE_GRAPHIC:
        if (plotspec->plot_requests<2)
            return bombre("can't use -graphic=continue for first plot request", NULL, 0);
        if (items!=1)
            return bombre("invalid -graphic=continue syntax--no other keywords allowed", NULL, 0);
        plreq->graphic.element = -1;
        break;
      case SET_LINE_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_LINE, item+1, items-1);
      case SET_SYMBOL_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_SYMBOL, item+1, items-1);
      case SET_ERRORBAR_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_ERRORBAR, item+1, items-1);
      case SET_DOT_GRAPHIC:
        if (!graphic_AP1(&plreq->graphic, PLOT_DOTS, item+1, items-1))
            return 0;
        if (plreq->graphic.connect_linetype!=-1)
            return bombre("can't connect dots with a line--the dots won't be visible!", NULL, 0);
        break;
      case SET_IMPULSE_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_IMPULSE, item+1, items-1);
      case SET_YIMPULSE_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_YIMPULSE, item+1, items-1);
      case SET_BAR_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_BAR, item+1, items-1);
      case SET_YBAR_GRAPHIC:
        return graphic_AP1(&plreq->graphic, PLOT_YBAR, item+1, items-1);
      default:
        return bombre("invalid graphic element name", graphic_usage, 0);
        }
    return 1;
    }

#define GRAPHIC_KW_TYPE 0
#define GRAPHIC_KW_SCALE 1
#define GRAPHIC_KW_CONNECT 2
#define GRAPHIC_KW_VARY 3
#define GRAPHIC_KW_EACHPAGE 4
#define GRAPHIC_KW_EACHFILE 5
#define GRAPHIC_KW_EACHREQUEST 6
#define GRAPHIC_KW_SUBTYPE 7
#define GRAPHIC_KW_MODULUS 8
#define GRAPHIC_KW_FIXFORNAME 9
#define GRAPHIC_KW_THICKNESS 10
#define GRAPHIC_KW_FILL 11
#define GRAPHIC_KW_FIXFORFILE 12
#define GRAPHIC_KW_FIXFORREQUEST 13
#define GRAPHIC_KWS 14
static char *graphic_kw[GRAPHIC_KWS] = {
    "type", "scale", "connect", "vary", "eachpage", "eachfile", "eachrequest", "subtype", "modulus",
    "fixforname", "thickness","fill", "fixforfile", "fixforrequest",
    } ;

#define CONNECT_KW_SUBTYPE 0
#define CONNECT_KW_TYPE 1
#define CONNECT_KWS 2
static char *connect_kw[CONNECT_KWS] = {
    "subtype", "type" 
        };

long graphic_AP1(GRAPHIC_SPEC *graphic_spec, long element, char **item, long items)
/* second stage processing of -graphic options for everything by arrow elements */
{
    long i;
    char *eqptr;
    graphic_spec->element = element;
    graphic_spec->type = 0;
    graphic_spec->scale = 1;
    graphic_spec->connect_linetype = -1;
    graphic_spec->vary = 0;
    graphic_spec->flags = 0;
    graphic_spec->subtype = 0;
    graphic_spec->thickness = defaultLineThickness;
    graphic_spec->fill = 0;
    for (i=0; i<items; i++) {
        if ((eqptr=strchr(item[i], '=')))
            *eqptr = 0;
        switch (match_string(item[i], graphic_kw, GRAPHIC_KWS, 0)) {
          case GRAPHIC_KW_TYPE:
            if (!eqptr || SDDS_StringIsBlank(eqptr+1) || sscanf(eqptr+1, "%ld", &graphic_spec->type)!=1 ||
                graphic_spec->type<0)
                return bombre("invalid type specification for -graphic", graphic_usage, 0);
            break;
          case GRAPHIC_KW_SUBTYPE:
            if (!eqptr || SDDS_StringIsBlank(eqptr+1))
                return bombre("invalid subtype specification for -graphic", graphic_usage, 0);
            if (sscanf(eqptr+1, "%ld", &graphic_spec->subtype)!=1) {
                if (strncmp(eqptr+1, "type", strlen(eqptr+1))!=0)
                    return bombre("invalid subtype specification for -graphic", graphic_usage, 0);
                graphic_spec->flags |= GRAPHIC_SUBTYPE_EQ_TYPE;
                }
            else if (graphic_spec->subtype<0)
                return bombre("invalid subtype specification for -graphic", graphic_usage, 0);
            break;
          case GRAPHIC_KW_THICKNESS:
            if (!eqptr || SDDS_StringIsBlank(eqptr+1) || sscanf(eqptr+1, "%ld", &graphic_spec->thickness)!=1)
	      return bombre("invalid thickness specification for -graphic", graphic_usage, 0);
	    if (graphic_spec->thickness<=0)
	      graphic_spec->thickness = 1;
	    if (graphic_spec->thickness>9)
	      graphic_spec->thickness = 9;
            break;
          case GRAPHIC_KW_SCALE:
            if (!eqptr || SDDS_StringIsBlank(eqptr+1) || sscanf(eqptr+1, "%lf", &graphic_spec->scale)!=1 ||
                graphic_spec->scale<=0)
                return bombre("invalid scale specification for -graphic", graphic_usage, 0);
            break;
          case GRAPHIC_KW_CONNECT:
            if (eqptr) {
                if (SDDS_StringIsBlank(eqptr+1))
                    return bombre("invalid connect linetype for -graphic", graphic_usage, 0);
                if (sscanf(eqptr+1, "%ld", &graphic_spec->connect_linetype)!=1) {
                    switch (match_string(eqptr+1, connect_kw, CONNECT_KWS, 0)) {
                      case CONNECT_KW_SUBTYPE:
                        graphic_spec->flags |= GRAPHIC_CONNECT_EQ_SUBTYPE;
                        break;
                      case CONNECT_KW_TYPE:
                        graphic_spec->flags |= GRAPHIC_CONNECT_EQ_TYPE;
                        break;
                      default:
                        return bombre("invalid connect value for -graphic", graphic_usage, 0);
                        }
                    }
                else if (graphic_spec->connect_linetype<0)
                    return bombre("invalid connect linetype for -graphic", graphic_usage, 0);
                }
            else
                graphic_spec->connect_linetype = 0;
            graphic_spec->flags |= GRAPHIC_CONNECT;
            break;
          case GRAPHIC_KW_VARY:
            if (eqptr) {
                long code;
                char *varyChoice[2] = {"type", "subtype"};
                if (SDDS_StringIsBlank(eqptr+1) || 
                    (code=match_string(eqptr+1, varyChoice, 2, 0))<0)
                    return bombre("invalid -vary syntax", graphic_usage, 0);
                switch (code) {
                  case 0: graphic_spec->flags |= GRAPHIC_VARY_TYPE; break;
                  case 1: graphic_spec->flags |= GRAPHIC_VARY_SUBTYPE; break;
                    }
                }
            else
                graphic_spec->flags |= GRAPHIC_VARY_TYPE;
            graphic_spec->vary = 1;
            break;
          case GRAPHIC_KW_EACHPAGE:
            graphic_spec->flags |= GRAPHIC_VARY_EACHPAGE;
          case GRAPHIC_KW_EACHFILE:
            graphic_spec->flags |= GRAPHIC_VARY_EACHFILE;
            break;
          case GRAPHIC_KW_EACHREQUEST:
            graphic_spec->flags |= GRAPHIC_VARY_EACHREQUEST;
            break;
          case GRAPHIC_KW_FIXFORNAME:
            graphic_spec->flags |= GRAPHIC_VARY_FIXFORNAME;
            break;
          case GRAPHIC_KW_FIXFORFILE:
            graphic_spec->flags |= GRAPHIC_VARY_FIXFORFILE;
            break;
          case GRAPHIC_KW_FIXFORREQUEST:
            graphic_spec->flags |= GRAPHIC_VARY_FIXFORREQUEST;
            break;
          case GRAPHIC_KW_MODULUS:
            if (!eqptr || SDDS_StringIsBlank(eqptr+1) || sscanf(eqptr+1, "%ld", &graphic_spec->modulus)!=1 ||
                graphic_spec->modulus<=0)
                return bombre("invalid modulus specification for -graphic", graphic_usage, 0);
            break;
	  case GRAPHIC_KW_FILL:
	    graphic_spec->fill = 1;
	    break;  
	  default:
            return bombre("invalid keyword for -graphic", graphic_usage, 0);
	}
    }
    if (graphic_spec->flags&GRAPHIC_VARY_SUBTYPE &&
        graphic_spec->flags&GRAPHIC_SUBTYPE_EQ_TYPE)
      return bombre("can't vary subtype and equate it to type in -graphic", NULL, 0);
    return 1;
}

long lspace_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];

    SDDS_ZeroMemory((void*)plreq->lspace, sizeof(*plreq->lspace)*4);
    if (items<4 ||
        sscanf(item[0], "%lf", &plreq->lspace[0])!=1 ||
        sscanf(item[1], "%lf", &plreq->lspace[1])!=1 ||
        sscanf(item[2], "%lf", &plreq->lspace[2])!=1 ||
        sscanf(item[3], "%lf", &plreq->lspace[3])!=1 ||
        plreq->lspace[0]>plreq->lspace[1] || plreq->lspace[2]>plreq->lspace[3])
        return bombre("invalid -lspace syntax", "-lspace=<qmin>,<qmax>,<pmin>,<pmax>", 0);
    plreq->flags |= PLREQ_LSPACE_GIVEN;
    return 1;
    }

long mplfiles_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    add_plot_request(plotspec);
    if (!scanItemList(&plotspec->plot_request[plotspec->plot_requests-1].mplflags, item, &items, 0,
                        "notitle", -1, NULL, 0, MPLFILE_NOTITLE,
                        "notopline", -1, NULL, 0, MPLFILE_NOTOPLINE,
                        NULL))
        return bombre("invalid -mplfiles syntax", "-mplfiles[=[notitle][,notopline]] <filename> ...", 0);
    plotspec->plot_request[plotspec->plot_requests-1].mplflags |= MPLFILE;
    return 1;
    }

long outputfile_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1)
        return bombre("invalid -outputfile syntax", "-outputfile=<filename>", 0);
    SDDS_CopyString(&plotspec->outputfile, item[0]);
    return 1;
    }

long columnnames_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  static char *columnnames_usage = 
    "-columnnames=<xname>,<yname-list>[,{<y1name-list> | <x1name>,<y1name-list>}]";
  return plotnames_AP1(plotspec, item, items, columnnames_usage, COLUMN_DATA);
}

long toPage_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;
  plreq = plotspec->plot_request+plotspec->plot_requests-1;
  if (items!=1)
    return bombre("invalid -toPage syntax",NULL,0);
  if (sscanf(item[0], "%ld", &plreq->topage)!=1)
    return bombre("invalid -toPage syntax",NULL,0);
  return 1;
}

long fromPage_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;
  plreq = plotspec->plot_request+plotspec->plot_requests-1;
  if (items!=1)
    return bombre("invalid -toPage syntax",NULL,0);
  if (sscanf(item[0], "%ld", &plreq->frompage)!=1)
    return bombre("invalid -fromPage syntax",NULL,0);
  return 1;
}

long xexclude_columnnames_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  static char *exclude_columnnames_usage = 
    "-xExclude=<xname>,<xname-list> -yExclude=<yname>,<yname-list>";
  return plotExclude_AP(plotspec,X_EXCLUDE,item,items);
}

long yexclude_columnnames_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  static char *yexclude_columnnames_usage = "-yExclude=<yname>[,<yname-list>]";
  return plotExclude_AP(plotspec,Y_EXCLUDE,item,items);
}

long plotExclude_AP(PLOT_SPEC *plotspec, long exclude_type, char **item, long items)
{
  EXCLUDE_SPEC *exclude_spec;
  long i;
  
  switch (exclude_type) {
  case X_EXCLUDE:
    exclude_spec = &plotspec->plot_request[plotspec->plot_requests-1].x_exclude_spec;
    break;
  case Y_EXCLUDE:
    exclude_spec = &plotspec->plot_request[plotspec->plot_requests-1].y_exclude_spec;
    break;
  default:
    return bombre("invalid exclude type in plotExclude_AP",NULL,0);
  }
  exclude_spec->excludeNames=items;
  exclude_spec->excludeName=tmalloc(sizeof(*exclude_spec->excludeName)*items);
  exclude_spec->was_wildExclude=tmalloc(sizeof(*exclude_spec->was_wildExclude)*items);
  for (i=0;i<items;i++) {
    exclude_spec->was_wildExclude[i]=has_wildcards(item[i]);
    SDDS_CopyString(&exclude_spec->excludeName[i],item[i]);
  }
  return 1;
}


long parameternames_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    static char *parameternames_usage = 
        "-parameternames=<xname>,<yname-list>[,{<y1name-list> | <x1name>,<y1name-list>}]";
    return plotnames_AP1(plotspec, item, items, parameternames_usage, PARAMETER_DATA);
    }

long arraynames_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    static char *arraynames_usage = 
        "-arraynames=<xname>,<yname-list>[,{<y1name-list> | <x1name>,<y1name-list>}]";
    return plotnames_AP1(plotspec, item, items, arraynames_usage, ARRAY_DATA);
    }

long plotnames_AP1(PLOT_SPEC *plotspec, char **item, long items, char *plotnames_usage, long class)
{
  PLOT_REQUEST *plreq;
  long i, groups, y1_index;
  char *ptry;
  char **item0, **item0Orig=NULL;
  
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  if (plreq->filenames!=0 || plreq->data_class!=class || plotspec->plot_requests==1)
    add_plot_request(plotspec);
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  
  if (items<2)
    return bombre("invalid syntax", plotnames_usage, 0);
  
  if (!(item0=SDDS_Malloc(sizeof(*item0)*items)) ||
      !(item0Orig=SDDS_Malloc(sizeof(*item0Orig)*items)))
    SDDS_Bomb("memory allocation failure");
  for (i=0; i<items; i++) {
    SDDS_CopyString(item0+i, item[i]);
    item0Orig[i] = item0[i];
  }
  
  y1_index = -1;
  if (items>2) 
    y1_index = items==3?2:3;
  
  groups = count_chars(item0[1], ',')+1;
  /* if (y1_index!=-1 && count_chars(item0[y1_index], ',')!=(groups-1))
     return bombre("invalid syntax", plotnames_usage, 0); */
  
  plreq->xname = SDDS_Realloc(plreq->xname, sizeof(*plreq->xname)*(plreq->datanames+groups));
  plreq->yname = SDDS_Realloc(plreq->yname, sizeof(*plreq->yname)*(plreq->datanames+groups));
  plreq->x1name = SDDS_Realloc(plreq->x1name, sizeof(*plreq->x1name)*(plreq->datanames+groups));
  plreq->y1name = SDDS_Realloc(plreq->y1name, sizeof(*plreq->y1name)*(plreq->datanames+groups));
  plreq->was_wildname = SDDS_Realloc(plreq->was_wildname, sizeof(*plreq->was_wildname)*(plreq->datanames+groups));
  plreq->data_class = class;
  for (i=0; i<groups; i++) {
    plreq->was_wildname[i] = 0;
    SDDS_CopyString(&plreq->xname[plreq->datanames+i], item0[0]);
    plreq->x1name[plreq->datanames+i] = plreq->y1name[plreq->datanames+i] = NULL;
    if (items==4)
      SDDS_CopyString(&plreq->x1name[plreq->datanames+i], item0[2]);
    if (SDDS_StringIsBlank(item0[1]))
      return bombre("invalid syntax---too few <ynames> items", plotnames_usage, 0);
    if ((ptry=strchr(item0[1], ',')))
      *ptry = 0;
    if (SDDS_StringIsBlank(item0[1])) 
      return bombre("invalid syntax---too few <ynames> items", plotnames_usage, 0);
        SDDS_CopyString(&plreq->yname[plreq->datanames+i], item0[1]);
    if (ptry) 
      item0[1] = ptry+1;
    else 
      item0[1] = NULL;
    if (y1_index!=-1) {
      if (SDDS_StringIsBlank(item0[y1_index])) {
        if (i == 0) {
          return bombre("invalid syntax---too few <y1names> items", plotnames_usage, 0);
        }
        SDDS_CopyString(&plreq->y1name[plreq->datanames+i],plreq->y1name[plreq->datanames+i-1]);
      } else {
        if ((ptry=strchr(item0[y1_index], ',')))
          *ptry = 0;
        if (SDDS_StringIsBlank(item0[y1_index])) {
          if (i == 0) {
            return bombre("invalid syntax---too few <y1names> items", plotnames_usage, 0);
          }
          SDDS_CopyString(&plreq->y1name[plreq->datanames+i],plreq->y1name[plreq->datanames+i-1]);
        } else
          SDDS_CopyString(&plreq->y1name[plreq->datanames+i], item0[y1_index]);
        if (ptry)
          item0[y1_index] = ptry+1;
        else 
          item0[y1_index] = NULL;
      }
    }
  }
  for (i=0; i<items; i++)
      free(item0Orig[i]);
  free(item0);
  free(item0Orig);
  
  plreq->datanames += groups;
  return 1;
}


long pspace_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];

    SDDS_ZeroMemory((void*)plreq->pspace, sizeof(*plreq->pspace)*4);
    if (items<4 ||
        sscanf(item[0], "%lf", &plreq->pspace[0])!=1 ||
        sscanf(item[1], "%lf", &plreq->pspace[1])!=1 ||
        sscanf(item[2], "%lf", &plreq->pspace[2])!=1 ||
        sscanf(item[3], "%lf", &plreq->pspace[3])!=1 ||
        plreq->pspace[0]>plreq->pspace[1] || plreq->pspace[2]>plreq->pspace[3])
        return bombre("invalid -pspace syntax", "-scales=<xmin>,<xmax>,<ymin>,<ymax>", 0);
    return 1;
    }

long scales_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];

    SDDS_ZeroMemory((void*)plreq->mapping, sizeof(*plreq->mapping)*4);
    if (items<4 ||
        sscanf(item[0], "%lf", &plreq->mapping[0])!=1 ||
        sscanf(item[1], "%lf", &plreq->mapping[1])!=1 ||
        sscanf(item[2], "%lf", &plreq->mapping[2])!=1 ||
        sscanf(item[3], "%lf", &plreq->mapping[3])!=1 ||
        plreq->mapping[0]>plreq->mapping[1] || plreq->mapping[2]>plreq->mapping[3])
        return bombre("invalid -scales syntax", "-scales=<xmin>,<xmax>,<ymin>,<ymax>", 0);
    return 1;
    }

long unsuppresszero_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    unsigned long bits;
    if (items>2)
        return bombre("invalid -unsuppresszero syntax", "-unsuppresszero[={x | y}]", 0);
    bits = 0;
    if (items==0)
        bits = PLREQ_UNSUPPRESSX + PLREQ_UNSUPPRESSY;
    while (items--) {
        if (item[items][0]=='x')
            bits |= PLREQ_UNSUPPRESSX;
        else if (item[items][0]=='y')
            bits |= PLREQ_UNSUPPRESSY;
        else
            return bombre("invalid -unsuppresszero syntax", "-unsuppresszero[={x | y}]", 0);
        }
    plotspec->plot_request[plotspec->plot_requests-1].flags |= bits;
    return 1;
    }

long zoom_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];

    plreq->zoom.magnification[0] = plreq->zoom.magnification[1] = 0;
    plreq->zoom.center[0] = plreq->zoom.center[1] = 0;
    if (!scanItemList(&plreq->zoom.flags, item, &items, 0,
                        "xfactor", SDDS_DOUBLE, plreq->zoom.magnification+0, 1, ZOOM_XMAG,
                        "yfactor", SDDS_DOUBLE, plreq->zoom.magnification+1, 1, ZOOM_YMAG,
                        "xcenter", SDDS_DOUBLE, plreq->zoom.center+0, 1, ZOOM_XCEN,
                        "ycenter", SDDS_DOUBLE, plreq->zoom.center+1, 1, ZOOM_YCEN,
                        "pcenter", SDDS_DOUBLE, plreq->zoom.center+0, 1, ZOOM_PCEN,
                        "qcenter", SDDS_DOUBLE, plreq->zoom.center+1, 1, ZOOM_QCEN,
                        "delay", -1, NULL, 0, ZOOM_DELAY,
                        NULL))
        return bombre("invalid -zoom syntax", "-zoom=[xfactor=<value>][,yfactor=<value>][,{xcenter=<value> | qcenter=<value>}][,{ycenter=<value> | pcenter=<value>}],[,delay]", 0);
#if defined(DEBUG)
    fprintf(stderr, "zoom: factor=%e, %e   center=%e, %e\n",
            plreq->zoom.magnification[0], plreq->zoom.magnification[1],
            plreq->zoom.center[0], plreq->zoom.center[1]);
#endif
    return 1;
    }

long nolabels_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NOLABELS;
    return 1;
    }

long noborder_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NOBORDER;
    return 1;
    }

long noscales_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (!items)
        plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NOSCALESX|PLREQ_NOSCALESY;
    while (items--) {
        switch (item[items][0]) {
          case 'x': case 'X':
            plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NOSCALESX;
            break;
          case 'y': case 'Y':
            plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NOSCALESY;
            break;
          default:
            return bombre("invalid -noscales syntax", "-noscales[={x | y}]", 0);
            }
        }
    return 1;
    }

long xlabel_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    return plotlabel_AP(plotspec, 0, item, items);
    }
long ylabel_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    return plotlabel_AP(plotspec, 1, item, items);
    }
long title_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    return plotlabel_AP(plotspec, 2, item, items);
    }
long topline_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    return plotlabel_AP(plotspec, 3, item, items);
    }

long plotlabel_AP(PLOT_SPEC *plotspec, long label_index, char **item, long items)
{
    LABEL_SPEC *lspec;
    long offset;
    char *useItem;
    unsigned long oldFlags;
    
    static char *usage[4] = {
        "-xlabel=[@<parameter-name> | format=<format_string> | <string> | use={name | symbol |      description}[,units]][,offset=<value>][,scale=<value>][,edit=<edit-command>][,thickness=<integer>][,linetype=<integer>]",
        "-ylabel=[@<parameter-name> | format=<format_string> | <string> | use={name | symbol | description}[,units]][,offset=<value>][,scale=<value>][,edit=<edit-command>][,thickness=<integer>][,linetype=<integer>]|[,vary]",
        "-title=[@<parameter-name> | format=<format_string> | <string> | use={name | symbol | description}[,units]][,offset=<value>][,scale=<value>][,edit=<edit-command>][,thickness=<integer>][,linetype=<integer>]",
        "-topline=[@<parameter-name> | format=<format_string> | <string> | use={name | symbol | description[,units]}][,offset=<value>][,scale=<value>][,edit=<edit-command>][,thickness=<integer>][,linetype=<integer>]",
        } ;
    if (label_index<0 || label_index>3)
        return bombre("programming error--invalid label_index in plotlabel_AP", NULL, 0);
    lspec = &plotspec->plot_request[plotspec->plot_requests-1].label[label_index];
    if (items<1)
        return bombre("invalid labeling syntax", usage[label_index], 0);
    if (!contains_keyword_phrase(item[0]))
        offset = 1;
    else 
        offset = 0;
    items -= offset;
    useItem = NULL;
    oldFlags = lspec->flags;
    lspec->linetype=0;
    if (!scanItemList(&lspec->flags, item+offset, &items, 0,
                      "use", SDDS_STRING, &useItem, 1, LABEL_USE_NAME,
                      "offset", SDDS_DOUBLE, &lspec->offset, 1, LABEL_OFFSET_GIVEN, 
                      "scale", SDDS_DOUBLE, &lspec->scale, 1, LABEL_SCALE_GIVEN,
                      "edit", SDDS_STRING, &lspec->edit_command, 1, LABEL_EDITCOMMAND_GIVEN,
                      "units", -1, NULL, 0, LABEL_INCLUDE_UNITS,
                      "thickness", SDDS_LONG, &lspec->thickness, 1, LABEL_THICKNESS_GIVEN,
                      "linetype", SDDS_LONG, &lspec->linetype, 1, LABEL_LINETYPE_GIVEN,
		      "format",SDDS_STRING,&lspec->format,1,LABEL_FORMAT_GIVEN,
                        NULL))
        return bombre("invalid labeling syntax", usage[label_index], 0);
    lspec->flags |= oldFlags;
      
    if (lspec->flags&LABEL_USE_NAME) {
        char *useChoice[3] = {"name", "symbol", "description"};
        long index;
        lspec->flags -= LABEL_USE_NAME;
        if ((index=match_string(useItem, useChoice, 3, 0))<0)
            return bombre("invalid labeling syntax--unrecognized use field", usage[label_index], 0);
        else 
            lspec->flags += LABEL_USE_NAME<<index;
        free(useItem);
        }
    if (offset) {
        lspec->label = item[0];
        if (item[0][0]=='@') {
            lspec->flags |= LABEL_PARAMETER_GIVEN;
            SDDS_CopyString(&lspec->label, item[0]+1);
            }
        else {
            SDDS_CopyString(&lspec->label, item[0]);
            lspec->flags |= LABEL_STRING_GIVEN;
          }
        }
    else {
        lspec->flags |= LABEL_USE_DEFAULT;
        lspec->label = NULL;
        }
    if (lspec->linetype<0)
      lspec->linetype = 0;
    if (lspec->thickness < 0)
      lspec->thickness = 0;
    if (lspec->thickness >= 10)
      lspec->thickness = 9;
    return 1;
    }


long string_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    STRING_LABEL_SPEC *sspec;
    long sspecs, countx, county, i;
    
    static char *usage = 
        "-string={ @<parameterName> | <string>},{xCoordinate={<value>|@<parameterName>} | pCoordinate=<value>},{yCoordinate={<value>|@<parameterName>} | qCoordinate=<value>}[,scale=<value>][,angle=<degrees>][,linetype=<integer>][,edit=<editCommand>][,justifyMode=<modes>][,slant=<degrees>][,thickness=<integer>]";

    sspecs = plotspec->plot_request[plotspec->plot_requests-1].string_labels;
    plotspec->plot_request[plotspec->plot_requests-1].string_label =
        SDDS_Realloc(plotspec->plot_request[plotspec->plot_requests-1].string_label,
                     sizeof(*sspec)*(sspecs+1));
    sspec = plotspec->plot_request[plotspec->plot_requests-1].string_label+sspecs;
    sspec->thickness=0;
    plotspec->plot_request[plotspec->plot_requests-1].string_labels += 1;
    if (items<1)
        return bombre("invalid -string syntax", usage, 0);
    items -= 1;
    sspec->edit_command = sspec->justify_mode = NULL;
    sspec->positionParameter[0] = sspec->positionParameter[1] = NULL;
    if (!scanItemList(&sspec->flags, item+1, &items, 0,
                        "scale", SDDS_DOUBLE, &sspec->scale, 1, LABEL_SCALE_GIVEN,
                        "angle", SDDS_DOUBLE, &sspec->angle, 1, LABEL_ANGLE_GIVEN,
                        "xcoordinate", SDDS_STRING, sspec->positionParameter+0, 1, LABEL_X_GIVEN,
                        "ycoordinate", SDDS_STRING, sspec->positionParameter+1, 1, LABEL_Y_GIVEN,
                        "pcoordinate", SDDS_DOUBLE, sspec->position+0, 1, LABEL_P_GIVEN,
                        "qcoordinate", SDDS_DOUBLE, sspec->position+1, 1, LABEL_Q_GIVEN,
                        "linetype", SDDS_LONG, &sspec->linetype, 1, LABEL_LINETYPE_GIVEN,
                        "thickness", SDDS_LONG, &sspec->thickness, 1, LABEL_THICKNESS_GIVEN,
                        "justify", SDDS_STRING, &sspec->justify_mode, 1, LABEL_JUSTIFYMODE_GIVEN,
                        "edit", SDDS_STRING, &sspec->edit_command, 1, LABEL_EDITCOMMAND_GIVEN,
                        "slant", SDDS_DOUBLE, &sspec->slant, 1, LABEL_SLANT_GIVEN,
                        NULL))
        return bombre("invalid -string syntax", usage, 0);
    for (i=0; i<2; i++) {
      if (sspec->flags&(LABEL_X_GIVEN<<i)) {
        if (sspec->positionParameter[i][0]=='@') {
          strcpy_ss(sspec->positionParameter[i], sspec->positionParameter[i]+1);
          sspec->flags |= (LABEL_XPARAM_GIVEN<<i);
        }
        else {
          if (!sscanf(sspec->positionParameter[i], "%le", &sspec->position[i]))
            return bombre("invalid coordinate value for -string", usage, 0);
        }
      }
    }
    
    countx = ((sspec->flags&LABEL_X_GIVEN)?1:0) + ((sspec->flags&LABEL_P_GIVEN)?1:0) ;
    county = ((sspec->flags&LABEL_Y_GIVEN)?1:0) + ((sspec->flags&LABEL_Q_GIVEN)?1:0) ;
    if (countx!=1 || county!=1)
        return bombre("invalid -string syntax---specify one of (x, p) and one of (y, q)", usage, 0);
    if (item[0][0]=='@') {
        sspec->flags |= LABEL_PARAMETER_GIVEN;
        SDDS_CopyString(&sspec->string, item[0]+1);
        }
    else
        SDDS_CopyString(&sspec->string, item[0]);
    if (plotspec->plot_request[plotspec->plot_requests-1].global_thickness_default &&
        !(sspec->flags&LABEL_THICKNESS_GIVEN)) {
      sspec->thickness = plotspec->plot_request[plotspec->plot_requests-1].global_thickness_default;
      sspec->flags |= LABEL_THICKNESS_GIVEN;
    }
    if (sspec->thickness < 0)
      sspec->thickness = 0;
    if (sspec->thickness >= 10)
      sspec->thickness = 9;
    return 1;
    }

long filenamesontopline_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  unsigned long dummy;
  plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_FNONTOPLINE;
  plotspec->plot_request[plotspec->plot_requests-1].flags &= ~PLREQ_YLONTOPLINE;
  plotspec->plot_request[plotspec->plot_requests-1].filenamesOnToplineEditCmd = NULL;
  if (!scanItemList(&dummy, item, &items, 0,
                    "editcommand", SDDS_STRING, 
                    &plotspec->plot_request[plotspec->plot_requests-1].filenamesOnToplineEditCmd,
                    1, 0,
                    NULL)) {
    return bombre("invalid -filenamesOnTopline syntax", "-filenamesOnTopline[=editcommand=<string>]",
                  0);
  }
  return 1;
}

long ylabelontopline_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_YLONTOPLINE;
    plotspec->plot_request[plotspec->plot_requests-1].flags &= ~PLREQ_FNONTOPLINE;
    return 1;
    }

long verticalprint_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    static char *option[2] = {"up", "down"};
    long i;
    if (items!=1 || (i=match_string(item[0], option, 2, 0))<0)
        return bombre("invalid -verticalprint syntax", "-verticalprint={up | down}", 0);
    if (i)
      plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_VPRINTDOWN;
    return 1;
    }

long toptitle_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_TOPTITLE;
    return 1;
    }

long datestamp_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_DATESTAMP;
    return 1;
    }

long samescale_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    unsigned long flags;
    if (items==0) {
        plotspec->plot_request[plotspec->plot_requests-1].flags 
            |= PLREQ_SAMESCALEX+PLREQ_SAMESCALEY;
        return 1;
        }

    if (!scanItemList(&flags, item, &items, 0,
                        "x", -1, NULL, 0, PLREQ_SAMESCALEX,
                        "y", -1, NULL, 0, PLREQ_SAMESCALEY,
                        "global", -1, NULL, 0, PLREQ_SAMESCALEGLOBAL,
                        NULL))
        return bombre("invalid -samescales syntax", "-samescales[=x][,y][,global]", 0);
    if (!(flags&(PLREQ_SAMESCALEX+PLREQ_SAMESCALEY)))
        flags |= PLREQ_SAMESCALEX+PLREQ_SAMESCALEY;
    plotspec->plot_request[plotspec->plot_requests-1].flags |= flags;
    return 1;
    }


static char *legend_usage = "-legend={{xy}symbol | {xy}description | {xy}name | filename | specified=<string> | parameter=<name>} {,format=<format_string>}{,editCommand=<edit-string>}[,units][,firstFileOnly][,scale=<value>][,thickness=<integer>][,nosubtype]";

long legend_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    LEGEND_SPEC *legend;
    unsigned long oldFlags;
    
    plotspec->plot_request[plotspec->plot_requests-1].legend.scale = 1;
    if (items==0) {
        plotspec->plot_request[plotspec->plot_requests-1].legend.code = LEGEND_YSYMBOL;
#if defined(DEBUG)
        fprintf(stderr, "legend defaulting to ysymbol mode\n");
#endif
        return 1;
        }
    legend = &plotspec->plot_request[plotspec->plot_requests-1].legend;
    oldFlags = legend->code;
    if (!scanItemList(&legend->code,
                        item, &items, 0,
                        "ysymbol", -1, NULL, 0, LEGEND_YSYMBOL,
                        "xsymbol", -1, NULL, 0, LEGEND_XSYMBOL,
                        "yname", -1, NULL, 0, LEGEND_YNAME,
                        "xname", -1, NULL, 0, LEGEND_XNAME,
                        "filename", -1, NULL, 0, LEGEND_FILENAME,
                        "specified", SDDS_STRING, &legend->value, 1, LEGEND_SPECIFIED, 
                        "rootname", -1, NULL, 0, LEGEND_ROOTNAME,
                        "parameter", SDDS_STRING, &legend->value, 1, LEGEND_PARAMETER,
                        "ydescription", -1, NULL, 0, LEGEND_YDESCRIPTION,
                        "xdescription", -1, NULL, 0, LEGEND_XDESCRIPTION,
                        "units", -1, NULL, 0, LEGEND_UNITS,
                        "editcommand", SDDS_STRING, &legend->edit_command, 1, LEGEND_EDIT,
                        "firstfileonly", -1, NULL, 0, LEGEND_FIRSTFILEONLY,
                        "scale", SDDS_DOUBLE, &legend->scale, 1, LEGEND_SCALEGIVEN,
                        "thickness", SDDS_LONG, &legend->thickness, 1, LEGEND_THICKNESS,
			"format", SDDS_STRING, &legend->format, 1, LEGEND_FORMAT,
                        "nosubtype", -1, NULL, 0, LEGEND_NOSUBTYPE,
                        NULL) ||
        (legend->code&LEGEND_SCALEGIVEN && legend->scale<=0))
        return bombre("invalid -legend syntax", legend_usage, 0);
    legend->code |= oldFlags;
    if (!(legend->code&LEGEND_CHOICES))
        legend->code |= LEGEND_YSYMBOL;
    if (legend->thickness < 0)
      legend->thickness = 0;
    if (legend->thickness >= 10)
      legend->thickness = 9;
#if defined(DEBUG)
    fprintf(stderr, "legend.code = %x\n", legend->code);
    if (legend->code&LEGEND_EDIT)
        fprintf(stderr, "editing legend with edit command %s\n", 
                legend->edit_command);
#endif
    return 1;
    }

#define OPT_OVERLAY_XFACTOR 0
#define OPT_OVERLAY_YFACTOR 1
#define OPT_OVERLAY_XOFFSET 2
#define OPT_OVERLAY_YOFFSET 3
#define OPT_OVERLAY_XMODE 4
#define OPT_OVERLAY_YMODE 5
#define OPT_OVERLAY_XCENTER 6
#define OPT_OVERLAY_YCENTER 7
#define OPT_OVERLAY_POFFSET 8
#define OPT_OVERLAY_QOFFSET 9
#define OPT_OVERLAY_PALIGN 10
#define OPT_OVERLAY_QALIGN 11
#define OVERLAY_OPTIONS 12
static char *overlay_option[OVERLAY_OPTIONS] = {
    "xfactor", "yfactor", "xoffset", "yoffset", "xmode", "ymode",
    "xcenter", "ycenter", "poffset", "qoffset", "palign", "qalign",
    } ;

#define OPT_OVERLAYMODE_NORMAL 0
#define OPT_OVERLAYMODE_UNIT   1
#define OVERLAYMODE_OPTIONS 2
static char *overlaymode_option[OVERLAYMODE_OPTIONS] = {
    "normal", "unit",
    } ;

static char *overlay_usage = "-overlay=[{xy}mode=<mode>][,{xy}factor=<value>][,{xy}offset=<value>][,{xy}center][,{pq}offset=<value>][,{pq}align]";

long overlay_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    char *ptr;
    unsigned long flags;
    
    plotspec->plot_request[plotspec->plot_requests-1].overlay.flags = OVERLAY_DATA;
    if (items==0) {
        return 1;
        }
    while (items--) {
        ptr = NULL;
        if ((ptr=strchr(item[items], '=')))
            *ptr++ = 0;
        switch (match_string(item[items], overlay_option, OVERLAY_OPTIONS, 0)) {
          case OPT_OVERLAY_XMODE:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags &= ~OVERLAY_XNORMAL;
            switch (match_string(ptr, overlaymode_option, OVERLAYMODE_OPTIONS, 0)) {
              case OPT_OVERLAYMODE_NORMAL:
                plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_XNORMAL;
                break;
              case OPT_OVERLAYMODE_UNIT:
                break;
              default:
                return bombre("invalid -overlay xmode syntax", overlay_usage, 0);
                }
            break;
          case OPT_OVERLAY_XFACTOR:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_XFACTOR;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.factor+0)!=1)
                return bombre("invalid -overlay xfactor syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_XOFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_XOFFSET;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.offset+0)!=1)
                return bombre("invalid -overlay xoffset syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_POFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_POFFSET;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.unitOffset+0)!=1)
                return bombre("invalid -overlay poffset syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_XCENTER:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_XCENTER;
            break;
          case OPT_OVERLAY_PALIGN:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_PALIGN;
            break;
          case OPT_OVERLAY_YMODE:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags &= ~OVERLAY_YNORMAL;
            switch (match_string(ptr, overlaymode_option, OVERLAYMODE_OPTIONS, 0)) {
              case OPT_OVERLAYMODE_NORMAL:
                plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_YNORMAL;
                break;
              case OPT_OVERLAYMODE_UNIT:
                break;
              default:
                return bombre("invalid -overlay ymode syntax", overlay_usage, 0);
                }
            break;
          case OPT_OVERLAY_YFACTOR:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_YFACTOR;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.factor+1)!=1)
                return bombre("invalid -overlay yfactor syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_YOFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_YOFFSET;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.offset+1)!=1)
                return bombre("invalid -overlay yoffset syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_QOFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_QOFFSET;
            if (sscanf(ptr, "%lf",  plotspec->plot_request[plotspec->plot_requests-1].overlay.unitOffset+1)!=1)
                return bombre("invalid -overlay qoffset syntax", overlay_usage, 0);
            break;
          case OPT_OVERLAY_YCENTER:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_YCENTER;
            break;
          case OPT_OVERLAY_QALIGN:
            plotspec->plot_request[plotspec->plot_requests-1].overlay.flags |= OVERLAY_QALIGN;
            break;
          default:
            return bombre("unknown -overlay keyword", overlay_usage, 0);
            }
        }
    flags = plotspec->plot_request[plotspec->plot_requests-1].overlay.flags;
    if ((flags&OVERLAY_YCENTER) && (flags&OVERLAY_QALIGN))
      return bombre("give only one of ycenter or qalign for overlay", overlay_usage, 0);
    if ((flags&OVERLAY_XCENTER) && (flags&OVERLAY_PALIGN))
      return bombre("give only one of xcenter or palign for overlay", overlay_usage, 0);
    return 1;
    }

static char *separate_usage = "-separate[=[<number-to-group>][,groupsOf=<number>][,fileIndex][,fileString][,nameIndex][,nameString][,page][,request][,tag][,subpage][,inamestring]";

long separate_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    short numberSeen = 0;

    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_SEPARATE;
    plotspec->plot_request[plotspec->plot_requests-1].separate_group_size = 1;
    
    if (items<1)
        return 1;
    if (isdigit(item[0][0])) {
        if (sscanf(item[0], "%" SCNd32, &plotspec->plot_request[plotspec->plot_requests-1].separate_group_size)!=1  ||
            plotspec->plot_request[plotspec->plot_requests-1].separate_group_size<=0)
            return bombre("invalid -separate syntax", separate_usage, 0);
        item += 1;
        items -= 1;
        numberSeen = 1;
        }
    if (!scanItemList(&plotspec->plot_request[plotspec->plot_requests-1].separate_flags,
                        item, &items,  0,
                        "groupsof", SDDS_LONG, &plotspec->plot_request[plotspec->plot_requests-1].separate_group_size, 1, 
                        SEPARATE_GROUPSOF,
                        "fileindex", -1, NULL, 0, SEPARATE_FILEINDEX,
                        "nameindex", -1, NULL, 0, SEPARATE_NAMEINDEX,
                        "filestring", -1, NULL, 0, SEPARATE_FILESTRING,
                        "namestring", -1, NULL, 0, SEPARATE_NAMESTRING,
                        "inamestring", -1, NULL, 0, SEPARATE_INAMESTRING,
                        "page", -1, NULL, 0, SEPARATE_PAGE,
                        "request", -1, NULL, 0, SEPARATE_REQUEST,
                        "tag", -1, NULL, 0, SEPARATE_TAG,
                        "subpage", -1, NULL, 0, SEPARATE_SUBPAGE,
                        NULL) ||
        (plotspec->plot_request[plotspec->plot_requests-1].separate_flags&SEPARATE_GROUPSOF &&
         plotspec->plot_request[plotspec->plot_requests-1].separate_group_size<=0))
        return bombre("invalid -separate syntax", separate_usage, 0);
    if (numberSeen && !(plotspec->plot_request[plotspec->plot_requests-1].separate_flags&SEPARATE_GROUPSOF)) 
        plotspec->plot_request[plotspec->plot_requests-1].separate_flags |= SEPARATE_GROUPSOF;
    return 1;
    }

long tagrequest_AP(PLOT_SPEC *plotspec, char **item, long items)
{
   /* if (items!=1 ||
        (item[0][0]!='@' && sscanf(item[0], "%lf", &plotspec->plot_request[plotspec->plot_requests-1].user_tag)!=1))
        return bombre("invalid -tagrequest syntax", "-tagrequest={<number> | @<parameter-name>}", 0); */
  if (items!=1 ||
      (item[0][0]!='@' && !SDDS_CopyString(&plotspec->plot_request[plotspec->plot_requests-1].user_tag,item[0])))
    return bombre("invalid -tagrequest syntax", "-tagrequest={<number> | @<parameter-name>}", 0);
  if (item[0][0]=='@')
    SDDS_CopyString(&plotspec->plot_request[plotspec->plot_requests-1].tag_parameter, item[0]+1);
  return 1;
}


static char *xScalesGroup_usage = "-xScalesGroup={ID=<string> | fileIndex|fileString|nameIndex|nameString|page|request|units}";
long scalesGroup_AP(PLOT_SPEC *plotspec, char **item, long items, long plane,
                    char *errorMessage, char *usage);

long xScalesGroup_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  return scalesGroup_AP(plotspec, item, items, 0, "invalid -xScalesGroup syntax",
                        xScalesGroup_usage);
}

static char *yScalesGroup_usage = "-yScalesGroup={ID=<string> | fileIndex|fileString|nameIndex|nameString|page|request|units}";

long yScalesGroup_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  return scalesGroup_AP(plotspec, item, items, 1, "invalid -yScalesGroup syntax",
                        yScalesGroup_usage);
}

long scalesGroup_AP(PLOT_SPEC *plotspec, char **item, long items, long plane,
                    char *errorMessage, char *usage)
{
  PLOT_REQUEST *plreq;

  plreq = plotspec->plot_request+plotspec->plot_requests-1;
  if (!scanItemList(&plreq->scalesGroupSpec[plane].flags,
                    item, &items,  0,
                    "id", SDDS_STRING, &plreq->scalesGroupSpec[plane].ID, 1, SCALESGROUP_ID_GIVEN,
                    "fileindex", -1, NULL, 0, SCALESGROUP_USE_FILEINDEX,
                    "filestring", -1, NULL, 0, SCALESGROUP_USE_FILESTRING,
                    "nameindex", -1, NULL, 0, SCALESGROUP_USE_NAMEINDEX,
                    "namestring", -1, NULL, 0, SCALESGROUP_USE_NAMESTRING,
                    "page", -1, NULL, 0, SCALESGROUP_USE_PAGE,
                    "request", -1, NULL, 0, SCALESGROUP_USE_REQUEST,
                    "units", -1, NULL, 0, SCALESGROUP_USE_UNITS,
                    NULL) ||
      bitsSet(plreq->scalesGroupSpec[plane].flags)!=1)
    return bombre(errorMessage, usage, 0);

  if (plreq->scalesGroupSpec[plane].flags&SCALESGROUP_ID_GIVEN &&
      strcmp(plreq->scalesGroupSpec[plane].ID, RESERVED_SCALESGROUP_ID)==0)
    return bombre("invalid -[xy]scalesGroup syntax---reserved ID used.", usage, 0);
  return 1;
}

long newpanel_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<2)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -newpanel", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NEWPANEL;
    return 1;
    }

long endpanel_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<2)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -endpanel", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_ENDPANEL;
    return 1;
    }

long nextpage_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<2)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -nextPage", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_NEXTPAGE;
    return 1;
    }

static char *omnipresent_usage = "-omnipresent[=first]" ;

long omnipresent_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<2)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -omnipresent", 0);
    if (items==0)
      plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_OMNIPRESENT;
    else if (items==1 && strncmp(item[0], "first", strlen(item[0]))==0) 
      plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_OMNIPRESENT+PLREQ_OMNIFIRST;
    else
      return bombre("invalid -omnipresent sytnax", omnipresent_usage, 0);
    return 1;
    }

static char *layout_usage = "-layout=<nx>,<ny>[,limitPerPage=<integer>]";

long layout_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    unsigned long flags;

    if (items<2)
        return bombre("invalid -layout syntax", layout_usage, 0);
    if (sscanf(item[0], "%ld", plotspec->layout+0)!=1 || sscanf(item[1], "%ld", plotspec->layout+1)!=1 ||
        plotspec->layout[0]<=0 || plotspec->layout[1]<=0) 
        return bombre("invalid -layout syntax", layout_usage, 0);
    item  += 2;
    plotspec->maxPanelsPerPage = plotspec->layout[0]*plotspec->layout[1];
    if ((items-=2) &&
        !scanItemList(&flags, item, &items, 0,
                        "limitperpage", SDDS_LONG, &plotspec->maxPanelsPerPage, 1, 0,
                        NULL))
        return bombre("invalid -layout syntax", layout_usage, 0);
    return 1;
    }

#define OPT_SPLIT_PARAMETERCHANGE 0
#define OPT_SPLIT_PAGES 1
#define OPT_SPLIT_POINTS 2
#define OPT_SPLIT_COLUMNCHANGE 3
#define OPT_SPLIT_NOCOLORBAR 4
#define SPLIT_OPTIONS 5
/*
static char *split_option[SPLIT_OPTIONS] = {
    "parameterchange", "pages", "points",
    "columnchange", "nocolorbar",
        } ;
*/

static char *split_usage = "-split={parameterChange=<parameter-name>[,width=<value>][,start=<value>] |\n\
 columnBin=<column-name>,width=<value>[,start=<value>][,completely] |\n\
 pages[,interval=<interval>][,nocolorbar] }\n";

long split_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    SPLIT_SPEC *split;

    if (items<1)
        return bombre("invalid -split syntax", split_usage, 0);
    split = &plotspec->plot_request[plotspec->plot_requests-1].split;
    split->flags = 0;
    if (!scanItemList(&split->flags, 
                        item, &items,  0,
                        "parameterchange", SDDS_STRING, &split->name, 1, SPLIT_PARAMETERCHANGE,
                        "columnbin", SDDS_STRING, &split->name, 1, SPLIT_COLUMNBIN,
                        "pages", -1, NULL, 0, SPLIT_PAGES,
                        "interval", SDDS_LONG, &split->interval, 1, SPLIT_PAGES_INTERVAL,
                        "width", SDDS_DOUBLE, &split->width, 1, SPLIT_CHANGE_WIDTH,
                        "start", SDDS_DOUBLE, &split->start, 1, SPLIT_CHANGE_START,
                        "points", -1, NULL, 0, SPLIT_POINTS, 
                        "completely", -1, NULL, 0, SPLIT_COMPLETELY,
                        "nocolorbar", -1, NULL, 0, SPLIT_NOCOLORBAR,
                      NULL)) {
      return bombre("invalid -split syntax", split_usage, 0);
    }
    if (split->flags&(SPLIT_PARAMETERCHANGE) && (!(split->flags&SPLIT_CHANGE_WIDTH) || split->width==0)) {
      return bombre("invalid -split syntax", split_usage, 0);
    }
    if (split->flags&(SPLIT_COLUMNBIN) && (!(split->flags&SPLIT_CHANGE_WIDTH))) {
      split->width = 0;
    }
    return 1;
    }


long grid_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (!items)
        plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_GRIDX|PLREQ_GRIDY;
    while (items--) {
        switch (item[items][0]) {
          case 'x': case 'X':
            plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_GRIDX;
            break;
          case 'y': case 'Y':
            plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_GRIDY;
            break;
          default:
            return bombre("invalid -grid syntax", "-grid[={x | y}]", 0);
            }
        }
    return 1;
    }


long axes_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    unsigned long flags;
    if (!items) {
        plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_AXESX|PLREQ_AXESY;
        return 1;
        }
    if (!scanItemList(&flags, item, &items, 0,
                        "x", -1, NULL, 0, PLREQ_AXESX,
                        "y", -1, NULL, 0, PLREQ_AXESY,
                        "linetype", SDDS_LONG, 
                        &plotspec->plot_request[plotspec->plot_requests-1].axesLinetype,
                        1, 0,
                        "thickness", SDDS_LONG, 
                        &plotspec->plot_request[plotspec->plot_requests-1].axesLinethickness,
                        1, 0,
                        NULL))
        return bombre("invalid -axes syntax", "-axes[=x][,y][,linetype=<number>][,thickness=<number>]", 0);
    if (!(flags&(PLREQ_AXESX|PLREQ_AXESY)))
        flags |= PLREQ_AXESX|PLREQ_AXESY;
    plotspec->plot_request[plotspec->plot_requests-1].flags |= flags;
    if (plotspec->plot_request[plotspec->plot_requests-1].axesLinethickness < 0)
      plotspec->plot_request[plotspec->plot_requests-1].axesLinethickness = 0;
    if (plotspec->plot_request[plotspec->plot_requests-1].axesLinethickness >= 10)
      plotspec->plot_request[plotspec->plot_requests-1].axesLinethickness = 9;
    return 1;
    }

static char *subticks_usage = "-subticksettings=[{xy}divisions=<integer>][,[{xy}]grid][,[{xy}]linetype=<integer>][,[{xy}]thickness=<integer>][,{xy}size=<fraction>][,xNoLogLabel][,yNoLogLabel]";

long subticks_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    TICK_SETTINGS *tset;
    double fraction;
    int32_t linetype, thickness;

    if (items<1)
        return bombre("invalid -subticks syntax", subticks_usage, 0);
    tset = &plotspec->plot_request[plotspec->plot_requests-1].subtick_settings;
    if (isdigit(item[0][0])) {
        if (sscanf(item[0], "%" SCNd32, tset->divisions+0)!=1 ||
            (items>1 && sscanf(item[1], "%" SCNd32, tset->divisions+1)!=1) )
          return bombre("invalid -subticksettings syntax", subticks_usage, 0);
        tset->flags |= TICKSET_GIVEN|TICKSET_XDIVISIONS|TICKSET_YDIVISIONS;
        return 1;
    }
    tset->divisions[0] = tset->divisions[1] = 0;
    tset->linetype[0] = tset->linetype[1] = 0;
    tset->thickness[0] = tset->thickness[1] = 0;
    tset->fraction[0] = tset->fraction[1] = 0;
    if (!scanItemListLong(&tset->flags,
                      item, &items,  0,
                      "xdivisions", SDDS_LONG, tset->divisions+0, 1, TICKSET_XDIVISIONS,
                      "ydivisions", SDDS_LONG, tset->divisions+1, 1, TICKSET_YDIVISIONS,
                      "xgrid", -1, NULL, 0, TICKSET_XGRID,
                      "ygrid", -1, NULL, 0, TICKSET_YGRID,
                      "grid", -1, NULL, 0, TICKSET_XGRID|TICKSET_YGRID,
                      "xlinetype", SDDS_LONG, tset->linetype+0, 1, TICKSET_XLINETYPE,
                      "ylinetype", SDDS_LONG, tset->linetype+1, 1, TICKSET_YLINETYPE,
                      "linetype", SDDS_LONG, &linetype, 1, TICKSET_LINETYPE,
                      "xthickness", SDDS_LONG, tset->thickness+0, 1, TICKSET_XTHICKNESS,
                      "ythickness", SDDS_LONG, tset->thickness+1, 1, TICKSET_YTHICKNESS,
                      "thickness", SDDS_LONG, &thickness, 1, TICKSET_THICKNESS,
                      "xsize", SDDS_DOUBLE, tset->fraction+0, 1, TICKSET_XFRACTION,
                      "ysize", SDDS_DOUBLE, tset->fraction+1, 1, TICKSET_YFRACTION,
                      "size", SDDS_DOUBLE, &fraction, 1, TICKSET_FRACTION,
                      "xnologlabel", -1, NULL, 0, TICKSET_XNOLOGLABEL,
                      "ynologlabel", -1, NULL, 0, TICKSET_YNOLOGLABEL,
                        NULL))
        return bombre("invalid -subticksettings syntax", subticks_usage, 0);
    if (tset->flags&TICKSET_LINETYPE) {
        if (!(tset->flags&TICKSET_XLINETYPE))
            tset->linetype[0] = linetype;
        if (!(tset->flags&TICKSET_YLINETYPE))
            tset->linetype[1] = linetype;
        tset->flags |= TICKSET_XLINETYPE|TICKSET_YLINETYPE;
        }
    if (tset->flags&TICKSET_FRACTION) {
        if (!(tset->flags&TICKSET_XFRACTION))
            tset->fraction[0] = fraction;
        if (!(tset->flags&TICKSET_YFRACTION))
            tset->fraction[1] = fraction;
        tset->flags |= TICKSET_XFRACTION|TICKSET_YFRACTION;
        }
    if (tset->flags&TICKSET_XTHICKNESS) {
      if (tset->thickness[0] < 0)
	tset->thickness[0] = 0;
      if (tset->thickness[0] > 10)
	tset->thickness[0] = 9;
    }
    if (tset->flags&TICKSET_YTHICKNESS) {
      if (tset->thickness[1] < 0)
	tset->thickness[1] = 0;
      if (tset->thickness[1] > 10)
	tset->thickness[1] = 9;
    }
    if (tset->flags&TICKSET_THICKNESS) {
      if (thickness < 0)
	thickness = 0;
      if (thickness > 10)
	thickness = 9;
      if (!(tset->flags&TICKSET_XTHICKNESS))
	tset->thickness[0] = thickness;
      if (!(tset->flags&TICKSET_YTHICKNESS))
            tset->thickness[1] = thickness;
      tset->flags |= TICKSET_XTHICKNESS|TICKSET_YTHICKNESS;
    }
    tset->flags |= TICKSET_GIVEN;
    return 1;
  }

#define OPT_MODE_LINEAR 0
#define OPT_MODE_LOG    1
#define OPT_MODE_SPECIALSCALES 2
#define OPT_MODE_NORMALIZE 3
#define OPT_MODE_OFFSET 4
#define OPT_MODE_CENTER 5
#define OPT_MODE_MEANCENTER 6
#define OPT_MODE_COFFSET 7
#define OPT_MODE_EOFFSET 8
#define OPT_MODE_FRACDEV 9
#define OPT_MODE_AUTOLOG 10
#define MODE_OPTIONS    11
static char *mode_option[MODE_OPTIONS] = {
    "linear", "logarithmic", "specialscales", "normalize", "offset",
    "center", "meancenter", "coffset", "eoffset", "fractionaldeviation",
    "autolog",
        } ;

static char *mode_usage = "-mode=[{x | y} = {linear | logarithmic | specialscales | normalize | offset | coffset | eoffset | center | meanCenter | fractionalDeviation | autolog}][,...]";

long mode_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    char *ptr;
    long shift;
    if (items<1)
        return bombre("invalid -mode syntax", mode_usage, 0);
    while (items--) {
        if (!(ptr=strchr(item[items], '=')))
            return bombre("invalid -mode syntax", mode_usage, 0);
        *ptr++ = 0;
        shift = 0;
        switch (item[items][0]) {
          case 'x': case 'X':
            break;
          case 'y': case 'Y':
            shift = 16;
            break;
          default:
            return bombre("invalid -mode syntax", mode_usage, 0);
            }
        switch (match_string(ptr, mode_option, MODE_OPTIONS, 0)) {
          case OPT_MODE_LINEAR:
            plotspec->plot_request[plotspec->plot_requests-1].mode &= ~(MODE_X_LOG<<shift);
            break;
          case OPT_MODE_LOG:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_LOG<<shift;
            break;
          case OPT_MODE_SPECIALSCALES:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_SPECIALSCALES<<shift;
            break;
          case OPT_MODE_NORMALIZE:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_NORMALIZE<<shift;
            break;
          case OPT_MODE_OFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_OFFSET<<shift;
            break;
          case OPT_MODE_COFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_COFFSET<<shift;
            break;
          case OPT_MODE_EOFFSET:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_EOFFSET<<shift;
            break;
          case OPT_MODE_CENTER:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_CENTER<<shift;
            break;
          case OPT_MODE_MEANCENTER:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_MEANCENTER<<shift;
            break;
          case OPT_MODE_FRACDEV:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_FRACDEV<<shift;
            break;
          case OPT_MODE_AUTOLOG:
            plotspec->plot_request[plotspec->plot_requests-1].mode |= MODE_X_AUTOLOG<<shift;
            break;
          default:
            return bombre("invalid -mode syntax", mode_usage, 0);
            }
        }
    if ((plotspec->plot_request[plotspec->plot_requests-1].mode&MODE_X_LOG && 
         plotspec->plot_request[plotspec->plot_requests-1].mode&MODE_X_AUTOLOG) ||
        (plotspec->plot_request[plotspec->plot_requests-1].mode&MODE_Y_LOG && 
         plotspec->plot_request[plotspec->plot_requests-1].mode&MODE_Y_AUTOLOG))
      return bombre("give either log or autolog, not both, for mode for each plane", mode_usage, 0);

    return 1;
    }

static char *stagger_usage = "-stagger=[xIncrement=<value>][,yIncrement=<value>][,files][,datanames]";

long stagger_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    double dummy;
    plreq = plotspec->plot_request+plotspec->plot_requests-1;
    if (items && sscanf(item[0], "%lf", &dummy)==1) {
        if (items!=2)
            return bombre("invalid -stagger syntax", stagger_usage, 0);
        if (sscanf(item[0], "%lf", plreq->stagger_amount+0)!=1 ||
            sscanf(item[1], "%lf", plreq->stagger_amount+1)!=1 )
            return bombre("invalid -stagger syntax", stagger_usage, 0);
        plreq->stagger_flags = 
            (plreq->stagger_amount[0]?STAGGER_XINC_GIVEN:0) +
                (plreq->stagger_amount[1]?STAGGER_YINC_GIVEN:0) ;
        }
    else {
        if (!scanItemList(&plreq->stagger_flags,
                            item, &items,  0,
                            "xincrement", SDDS_DOUBLE, plreq->stagger_amount+0, 1, STAGGER_XINC_GIVEN,
                            "yincrement", SDDS_DOUBLE, plreq->stagger_amount+1, 1, STAGGER_YINC_GIVEN,
                            "files", -1, NULL, 0, STAGGER_FILES,
                            "columns", -1, NULL, 0, STAGGER_DATANAMES,  /* for backward compatibility */
                            "datanames", -1, NULL, 0, STAGGER_DATANAMES,
                            NULL)) 
            return bombre("invalid -stagger syntax", stagger_usage, 0);
        }
    return 1;
    }

static char *factor_usage = "-factor=[{xy}Multiplier=<value>][,{xy}Parameter=<value>][,{xy}Invert]";

long factor_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    long i;
    
    plreq = plotspec->plot_request+plotspec->plot_requests-1;
    if (!scanItemList(&plreq->factor_flags,
                        item, &items,  0,
                        "xmultiplier", SDDS_DOUBLE, plreq->factor+0, 1, FACTOR_XMULT_GIVEN,
                        "ymultiplier", SDDS_DOUBLE, plreq->factor+1, 1, FACTOR_YMULT_GIVEN,
                        "xparameter", SDDS_STRING, plreq->factor_parameter+0, 1, FACTOR_XPARAMETER_GIVEN,
                        "yparameter", SDDS_STRING, plreq->factor_parameter+1, 1, FACTOR_YPARAMETER_GIVEN,
                        "xinvert", -1, NULL, 0, FACTOR_XINVERT_GIVEN,
                        "yinvert", -1, NULL, 0, FACTOR_YINVERT_GIVEN,
                        NULL) ||
        ((plreq->factor_flags&FACTOR_XMULT_GIVEN) && (plreq->factor_flags&FACTOR_XPARAMETER_GIVEN)) ||
        ((plreq->factor_flags&FACTOR_YMULT_GIVEN) && (plreq->factor_flags&FACTOR_YPARAMETER_GIVEN)))
        return bombre("invalid -factor syntax", factor_usage, 0);
    for (i=0; i<2; i++) {
      if ((plreq->factor_flags&(FACTOR_XINVERT_GIVEN<<i)) &&
          (plreq->factor_flags&(FACTOR_XMULT_GIVEN<<i)))
        plreq->factor[i] = 1/plreq->factor[i];
    }
    return 1;
    }

static char *offset_usage = "-offset=[{x|y}change={value>][,{x|y}parameter=<name>][,{x|y}invert]";

long offset_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    long i;
    
    plreq = plotspec->plot_request+plotspec->plot_requests-1;
    if (!scanItemList(&plreq->offset_flags,
                        item, &items,  0,
                        "xchange", SDDS_DOUBLE, plreq->offset+0, 1, OFFSET_XCHANGE_GIVEN,
                        "ychange", SDDS_DOUBLE, plreq->offset+1, 1, OFFSET_YCHANGE_GIVEN,
                        "xparameter", SDDS_STRING, plreq->offset_parameter+0, 1, OFFSET_XPARAMETER_GIVEN,
                        "yparameter", SDDS_STRING, plreq->offset_parameter+1, 1, OFFSET_YPARAMETER_GIVEN,
                        "xinvert", -1, NULL, 0, OFFSET_XINVERT_GIVEN,
                        "yinvert", -1, NULL, 0, OFFSET_YINVERT_GIVEN,
                        NULL))
        return bombre("invalid -offset syntax", offset_usage, 0);
    for (i=0; i<2; i++) {
      if ((plreq->offset_flags&(OFFSET_XINVERT_GIVEN<<i)) &&
          (plreq->offset_flags&(OFFSET_XCHANGE_GIVEN<<i)))
        plreq->offset[i] *= -1;
    }
    return 1;
  }

static char *dither_usage = "-dither=[{x|y}range=<fraction>]";

long dither_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    long i;
    unsigned long flags;
    
    plreq = plotspec->plot_request+plotspec->plot_requests-1;
    if (!scanItemList(&flags,
                        item, &items,  0,
                        "xrange", SDDS_DOUBLE, plreq->dither+0, 1, 0,
                        "yrange", SDDS_DOUBLE, plreq->dither+1, 1, 0,
                        NULL))
        return bombre("invalid -dither syntax", dither_usage, 0);
    return 1;
  }

static char *sever_usage = "-sever[={xgap=<value> | ygap=<value>}]";

long sever_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    unsigned long flags;
    PLOT_REQUEST *plreq;

    plreq = plotspec->plot_request+plotspec->plot_requests-1;
    if (items==0) 
        flags = PLREQ_SEVER;
    else if (!scanItemList(&flags, item, &items, 0,
                             "xgap", SDDS_DOUBLE, &plreq->xgap, 1, PLREQ_XGAP,
                             "ygap", SDDS_DOUBLE, &plreq->ygap, 1, PLREQ_YGAP,
                             NULL) ||
             (flags&PLREQ_XGAP && flags&PLREQ_YGAP))
        return bombre("invalid -sever syntax", sever_usage, 0);
    plreq->flags |= flags;
    return 1;
    }

static char *sparse_usage = "-sparse=<interval>[,<offset>]";

long sparse_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1 && items!=2)
        return bombre("invalid -sparse syntax", sparse_usage, 0);
    if ((sscanf(item[0], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].sparse_interval)!=1) ||
        (plotspec->plot_request[plotspec->plot_requests-1].sparse_interval<=0) ||
        ((items==2) && ((sscanf(item[1], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].sparse_offset)!=1) ||
         (plotspec->plot_request[plotspec->plot_requests-1].sparse_offset<0))))
        return bombre("invalid -sparse syntax", sparse_usage, 0);
    return 1;
    }

static char *presparse_usage = "-presparse=<interval>[,<offset>]";

long presparse_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1 && items!=2)
        return bombre("invalid -presparse syntax", sparse_usage, 0);
    if ((sscanf(item[0], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].presparse_interval)!=1) ||
        (plotspec->plot_request[plotspec->plot_requests-1].presparse_interval<=0) ||
        ((items==2) && ((sscanf(item[1], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].presparse_offset)!=1) ||
         (plotspec->plot_request[plotspec->plot_requests-1].presparse_offset<0))))
        return bombre("invalid -presparse syntax", presparse_usage, 0);
    return 1;
    }

static char *sample_usage = "-sample=<fraction>";

long sample_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items!=1)
        return bombre("invalid -sample syntax", sample_usage, 0);
    if (sscanf(item[0], "%lf", &plotspec->plot_request[plotspec->plot_requests-1].sample_fraction)!=1 ||
        plotspec->plot_request[plotspec->plot_requests-1].sample_fraction<=0 )
        return bombre("invalid -sample syntax", sample_usage, 0);
    return 1;
    }

static char *clip_usage = "-clip=<head>,<tail>[,invert]";

long clip_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (items<2 || items>3)
        return bombre("invalid -clip syntax", clip_usage, 0);
    if (sscanf(item[0], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].clip_head)!=1 ||
        plotspec->plot_request[plotspec->plot_requests-1].clip_head<0 ||
        sscanf(item[1], "%ld", &plotspec->plot_request[plotspec->plot_requests-1].clip_tail)!=1 ||
        plotspec->plot_request[plotspec->plot_requests-1].clip_tail<0 )
        return bombre("invalid -clip syntax", clip_usage, 0);
    if (items==3) {
        if (strncmp(item[2], "invert", strlen(item[2]))==0)
            plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_INVERTCLIP;
        else
            return bombre("invalid -clip syntax", clip_usage, 0);
        }
    return 1;
    }

#define OPT_KEEP_NAMES 0
#define OPT_KEEP_FILES 1
#define KEEP_OPTIONS 2
static char *keep_option[KEEP_OPTIONS] = {
    "names", "files",
    } ;
static char *keep_usage = "-keep[={names | files}]";

long keep_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<2 ||
        plotspec->plot_request[plotspec->plot_requests-1].mplflags&MPLFILE)
        return bombre("-parameterNames or -columnNames must be given prior to -keep", NULL, 0);
    add_plot_request(plotspec);
    if (items==0) {
        keepnames_AP1(plotspec);
        keepfilenames_AP1(plotspec);
        }
    else {
        while (items--) {
            switch (match_string(item[items], keep_option, KEEP_OPTIONS, 0)) {
              case OPT_KEEP_NAMES:
                keepnames_AP1(plotspec);
                break;
              case OPT_KEEP_FILES:
                keepfilenames_AP1(plotspec);
                break;
              default:
                return bombre("invalid -keep keyword", keep_usage, 0);
                }
            }
        }
    return 1;
    }

long keepnames_AP1(PLOT_SPEC *plotspec)
{
    PLOT_REQUEST *plreq, *pllreq;
    long i;

    pllreq = &plotspec->plot_request[plotspec->plot_requests-2];
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->datanames = pllreq->datanames;
    plreq->xname = SDDS_Realloc(plreq->xname, sizeof(*plreq->xname)*plreq->datanames);
    plreq->yname = SDDS_Realloc(plreq->yname, sizeof(*plreq->yname)*plreq->datanames);
    plreq->x1name = SDDS_Realloc(plreq->x1name, sizeof(*plreq->x1name)*plreq->datanames);
    plreq->y1name = SDDS_Realloc(plreq->y1name, sizeof(*plreq->y1name)*plreq->datanames);
    plreq->was_wildname = SDDS_Realloc(plreq->was_wildname, sizeof(*plreq->was_wildname)*plreq->datanames);

    for (i=0; i<plreq->datanames; i++) {
        if (plreq->xname[i])
          free(plreq->xname[i]);
        SDDS_CopyString(plreq->xname+i, pllreq->xname[i]);
        if (plreq->yname[i])
          free(plreq->yname[i]);
        SDDS_CopyString(plreq->yname+i, pllreq->yname[i]);
        if (plreq->x1name[i])
          free(plreq->x1name[i]);
        if (plreq->x1name[i]) 
            SDDS_CopyString(plreq->x1name+i, pllreq->x1name[i]);
        if (plreq->y1name[i])
          free(plreq->y1name[i]);
        if (plreq->y1name[i])
            SDDS_CopyString(plreq->y1name+i, pllreq->y1name[i]);
        plreq->was_wildname[i] = 0;
        }
    return 1;
    }

long keepfilenames_AP1(PLOT_SPEC *plotspec)
{
    PLOT_REQUEST *plreq, *pllreq;
    long i;

    pllreq = &plotspec->plot_request[plotspec->plot_requests-2];
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->filename = tmalloc(sizeof(*plreq->filename)*pllreq->filenames);
    for (i=0; i<pllreq->filenames; i++) 
        SDDS_CopyString(plreq->filename+i, pllreq->filename[i]);
    plreq->filenames = pllreq->filenames;
    return 1;
    }

static char *filter_usage = " -filter={`column' | `parameter'},<range-spec>[,<range-spec>[,<logic-operation>...]\n\
A <logic-operation> is one of & (logical and) or | (logical or), optionally followed by a ! to \n\
logically negate the value of the expression.\n\
A <range-spec> is of the form <name>,<lower-value>,<upper-value>[,!].\n";

long filter_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->filter = (FILTER_DEFINITION**)SDDS_Realloc(plreq->filter, 
                                                      sizeof(*plreq->filter)*(plreq->filters+1));
    if (!(plreq->filter[plreq->filters]=process_new_filter_definition(item, items)))
        return bombre("invalid -filter syntax", filter_usage, 0);
    plreq->filters ++;
    return 1;
    }

static char *time_filter_usage = " -timeFilter={`column' | `parameter'},<name>[,before=YYYY/MM/DD@HH:MM:SS][,after=YYYY/MM/DD@HH:MM:SS][,invert]";

long time_filter_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  plreq->time_filter = (TIME_FILTER_DEFINITION**)SDDS_Realloc(plreq->time_filter, 
                                                    sizeof(*plreq->time_filter)*(plreq->time_filters+1));
  if (!(plreq->time_filter[plreq->time_filters]=process_new_time_filter_definition(item, items)))
    return bombre("invalid -timeFilter syntax", time_filter_usage, 0);
  plreq->time_filters ++;
  return 1;
}

static char *match_usage = " -match={`column' | `parameter'},<match-test>[,<match-test>[,<logic-operation>...]\n\
A <match-test> is of the form <name>=<matching-string>[,!], where ! signifies logical negation.\n\
A <logic-operation> is one of & (logical and) or | (logical or), optionally followed by a ! to \n\
logically negate the value of the expression.\n";

long match_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->match = (MATCH_DEFINITION**)SDDS_Realloc(plreq->match, 
                                                    sizeof(*plreq->match)*(plreq->matches+1));
    if (!(plreq->match[plreq->matches]=process_new_match_definition(item, items)))
        return bombre("invalid -match syntax", match_usage, 0);
    plreq->matches ++;
    return 1;
    }


long drawline_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  DRAW_LINE_SPEC *drawLineSpec;
  long drawLineSpecs;
  static char *drawlineUsage = "-drawLine=\
{x0value=<value> | p0value=<value> | x0parameter=<name> | p0parameter=<name>},\
{x1value=<value> | p1value=<value> | x1parameter=<name> | p1parameter=<name>},\
{y0value=<value> | q0value=<value> | y0parameter=<name> | q0parameter=<name>},\
{y1value=<value> | q1value=<value> | y1parameter=<name> | q1parameter=<name>}\
[,linetype=<integer>][,thickness=<integer>][,clip]";
  
  drawLineSpec = plotspec->plot_request[plotspec->plot_requests-1].drawLineSpec;
  drawLineSpecs = plotspec->plot_request[plotspec->plot_requests-1].drawLineSpecs;
  if (!(drawLineSpec=SDDS_Realloc(drawLineSpec, sizeof(*drawLineSpec)*(drawLineSpecs+1)))) {
    return bombre("allocation failure in drawline_AP", NULL, 0);
  }
  drawLineSpec[drawLineSpecs].linethickness = 0;
  if (!scanItemList(&drawLineSpec[drawLineSpecs].flags, item, &items, 0,
		    "x0value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].x0, 1, DRAW_LINE_X0GIVEN,
                    "y0value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].y0, 1, DRAW_LINE_Y0GIVEN,
                    "p0value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].p0, 1, DRAW_LINE_P0GIVEN,
		    "q0value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].q0, 1, DRAW_LINE_Q0GIVEN,
                    "x1value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].x1, 1, DRAW_LINE_X1GIVEN,
                    "y1value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].y1, 1, DRAW_LINE_Y1GIVEN,
                    "p1value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].p1, 1, DRAW_LINE_P1GIVEN,
                    "q1value", SDDS_DOUBLE, &drawLineSpec[drawLineSpecs].q1, 1, DRAW_LINE_Q1GIVEN,
                    "x0parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].x0Param, 1, DRAW_LINE_X0PARAM,
                    "y0parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].y0Param, 1, DRAW_LINE_Y0PARAM,
                    "p0parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].p0Param, 1, DRAW_LINE_P0PARAM,
                    "q0parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].q0Param, 1, DRAW_LINE_Q0PARAM,
                    "x1parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].x1Param, 1, DRAW_LINE_X1PARAM,
                    "y1parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].y1Param, 1, DRAW_LINE_Y1PARAM,
                    "p1parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].p1Param, 1, DRAW_LINE_P1PARAM,
                    "q1parameter", SDDS_STRING, &drawLineSpec[drawLineSpecs].q1Param, 1, DRAW_LINE_Q1PARAM,
                    "linetype", SDDS_LONG, &drawLineSpec[drawLineSpecs].linetype, 1, DRAW_LINE_LINETYPEGIVEN, 
                    "thickness", SDDS_LONG, &drawLineSpec[drawLineSpecs].linethickness, 1, 0, 
                    "clip", -1, NULL, 0, DRAW_LINE_CLIPGIVEN,
                    NULL)) 
    return bombre("invalid -drawline syntax", drawlineUsage, 0);
  if (bitsSet(drawLineSpec[drawLineSpecs].flags&
              (DRAW_LINE_X0GIVEN+DRAW_LINE_P0GIVEN+DRAW_LINE_X0PARAM+DRAW_LINE_P0PARAM))!=1 ||
      bitsSet(drawLineSpec[drawLineSpecs].flags&
              (DRAW_LINE_Y0GIVEN+DRAW_LINE_Q0GIVEN+DRAW_LINE_Y0PARAM+DRAW_LINE_Q0PARAM))!=1 ||
      bitsSet(drawLineSpec[drawLineSpecs].flags&
              (DRAW_LINE_X1GIVEN+DRAW_LINE_P1GIVEN+DRAW_LINE_X1PARAM+DRAW_LINE_P1PARAM))!=1 ||
      bitsSet(drawLineSpec[drawLineSpecs].flags&
              (DRAW_LINE_Y1GIVEN+DRAW_LINE_Q1GIVEN+DRAW_LINE_Y1PARAM+DRAW_LINE_Q1PARAM))!=1)
    return bombre("invalid -drawline syntax", drawlineUsage, 0);
  plotspec->plot_request[plotspec->plot_requests-1].drawLineSpec = drawLineSpec;
  plotspec->plot_request[plotspec->plot_requests-1].drawLineSpecs += 1;
  if (drawLineSpec[drawLineSpecs].linethickness < 0)
    drawLineSpec[drawLineSpecs].linethickness = 0;
  if (drawLineSpec[drawLineSpecs].linethickness >= 10)
    drawLineSpec[drawLineSpecs].linethickness = 9;
  return 1;
}


long swap_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<1)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -swap", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_SWAP;
    return 1;
    }

long showlinkdate_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    void put_link_date();
    put_link_date(stderr);
    return 0;
    }

long transpose_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    if (plotspec->plot_requests<1)
        return bombre(NO_REQUESTS_MESSAGE, "-parameterNames, -columnNames or -mplfiles must be given prior to -transpose", 0);
    plotspec->plot_request[plotspec->plot_requests-1].flags |= PLREQ_TRANSPOSE;
    return 1;
    }

#define LINETYPEDEFAULT_KW_THICKNESS 0
#define LINETYPEDEFAULT_KWS 1
static char *linetypedefault_kw[LINETYPEDEFAULT_KWS] = {
  "thickness"
};
long linetypedefault_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    int i;
    char *eqptr;
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    if ((items < 1) ||
	(sscanf(item[0], "%ld", &plreq->linetype_default)!=1) ||
	(plreq->linetype_default<0))
        return bombre("invalid -linetypedefault syntax", "-linetypedefault=<integer>[,thickness=<value>]", 0);
    for (i=1; i<items; i++) {
      if ((eqptr=strchr(item[i], '=')))
	*eqptr = 0;
      switch (match_string(item[i], linetypedefault_kw, LINETYPEDEFAULT_KWS, 0)) {
      case LINETYPEDEFAULT_KW_THICKNESS:
	if (!eqptr || SDDS_StringIsBlank(eqptr+1) || sscanf(eqptr+1, "%ld", &plreq->linethickness_default)!=1)
	  return bombre("invalid -linetypedefault syntax", "-linetypedefault=<integer>[,thickness=<value>]", 0);
	if (plreq->linethickness_default <= 0)
	  plreq->linethickness_default = 1;
	if (plreq->linethickness_default >= 10)
	  plreq->linethickness_default = 9;
	defaultLineThickness = plreq->linethickness_default;
	break;
      default:
	return bombre("invalid -linetypedefault syntax", "-linetypedefault=<integer>[,thickness=<value>]", 0);
      }
    }
    return 1;
    }


static char *ticksettings_usage = "-ticksettings=[{xy}spacing=<value>][,[{xy}]grid][,[{xy}]linetype=<integer>][,[{xy}]thickness=<integer>][,{xy}size=<fraction>][{xy}modulus=<value>][,[{xy}]logarithmic][,{xy}factor=<value>][,{xy}offset=<value>][,{xy}time][,{xy}nonExponentialLabels][,{xy}invert][,{xy}scaleChars=<value>][,[{xy}]labelThickness=<integer>]\n";

long ticksettings_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    TICK_SETTINGS *tset;
    double fraction;
    int32_t linetype;
    int32_t thickness;
    int32_t labelThickness=0;
    unsigned long long oldFlags;
    
    if (items<1)
        return bombre("invalid -ticksettings syntax", ticksettings_usage, 0);

    tset = &plotspec->plot_request[plotspec->plot_requests-1].tick_settings;
    if (isdigit(item[0][0])) {
        if (items!=2 ||
            sscanf(item[0], "%lf", tset->spacing+0)!=1  ||
            sscanf(item[1], "%lf", tset->spacing+1)!=1 )
            return bombre("invalid -ticksettings syntax", ticksettings_usage, 0);
        tset->flags = TICKSET_GIVEN|TICKSET_XSPACING|TICKSET_YSPACING;
        return 1;
        }
    tset->spacing[0] = tset->spacing[1] = 0;
    tset->modulus[0] = tset->modulus[1] = 0;
    tset->linetype[0] = tset->linetype[1] = 0;
    tset->factor[0] = tset->factor[1] = 1;
    tset->fraction[0] = 0.02;
    tset->fraction[1] = 0.01;
    oldFlags = tset->flags;
    if (!scanItemListLong(&tset->flags,
                        item, &items,  0,
                        "xspacing", SDDS_DOUBLE, tset->spacing+0, 1, TICKSET_XSPACING,
                        "yspacing", SDDS_DOUBLE, tset->spacing+1, 1, TICKSET_YSPACING,
                        "xmodulus", SDDS_DOUBLE, tset->modulus+0, 1, TICKSET_XMODULUS,
                        "ymodulus", SDDS_DOUBLE, tset->modulus+1, 1, TICKSET_YMODULUS,
                        "xgrid", -1, NULL, 0, TICKSET_XGRID,
                        "ygrid", -1, NULL, 0, TICKSET_YGRID,
                        "grid", -1, NULL, 0, TICKSET_XGRID|TICKSET_YGRID,
                        "xlogarithmic", -1, NULL, 0, TICKSET_XLOGARITHMIC,
                        "ylogarithmic", -1, NULL, 0, TICKSET_YLOGARITHMIC,
                        "logarithmic", -1, NULL, 0, TICKSET_XLOGARITHMIC|TICKSET_YLOGARITHMIC,
                        "xlinetype", SDDS_LONG, tset->linetype+0, 1, TICKSET_XLINETYPE,
                        "ylinetype", SDDS_LONG, tset->linetype+1, 1, TICKSET_YLINETYPE,
                        "linetype", SDDS_LONG, &linetype, 1, TICKSET_LINETYPE,
                        "xthickness", SDDS_LONG, tset->thickness+0, 1, TICKSET_XTHICKNESS,
                        "ythickness", SDDS_LONG, tset->thickness+1, 1, TICKSET_YTHICKNESS,
                        "thickness", SDDS_LONG, &thickness, 1, TICKSET_THICKNESS,
                        "xsize", SDDS_DOUBLE, tset->fraction+0, 1, TICKSET_XFRACTION,
                        "ysize", SDDS_DOUBLE, tset->fraction+1, 1, TICKSET_YFRACTION,
                        "size", SDDS_DOUBLE, &fraction, 1, TICKSET_FRACTION,
                        "xfactor", SDDS_DOUBLE, tset->factor+0, 1, TICKSET_XFACTOR,
                        "yfactor", SDDS_DOUBLE, tset->factor+1, 1, TICKSET_YFACTOR,
                        "xoffset", SDDS_DOUBLE, tset->offset+0, 1, TICKSET_XOFFSET,
                        "yoffset", SDDS_DOUBLE, tset->offset+1, 1, TICKSET_YOFFSET,
                        "xtime", -1, NULL, 0, TICKSET_XTIME,
                        "ytime", -1, NULL, 0, TICKSET_YTIME,
                        "xnonexponentiallabels", -1, NULL, 0, TICKSET_XNONEXPLABELS,
                        "ynonexponentiallabels", -1, NULL, 0, TICKSET_YNONEXPLABELS,
                        "xinvert", -1, NULL, 0, TICKSET_XINVERT,
                        "yinvert", -1, NULL, 0, TICKSET_YINVERT,
                        "xscalechars", SDDS_DOUBLE, tset->scaleChar+0, 1, TICKSET_XSCALECHAR,
                        "yscalechars", SDDS_DOUBLE, tset->scaleChar+1, 1, TICKSET_YSCALECHAR,
		        "ylabelthickness", SDDS_LONG, tset->labelThickness+1, 1, 0ULL,
		        "xlabelthickness", SDDS_LONG, tset->labelThickness+0, 1, 0ULL,
		        "labelthickness", SDDS_LONG, &labelThickness, 1, 0ULL,
                        NULL) ||
        (tset->flags&TICKSET_XMODULUS && tset->modulus[0]<=0) ||
        (tset->flags&TICKSET_YMODULUS && tset->modulus[1]<=0) ||
        (tset->flags&TICKSET_XFACTOR && tset->factor[0]<=0) ||
        (tset->flags&TICKSET_YFACTOR && tset->factor[1]<=0) ||
        (tset->flags&TICKSET_XSCALECHAR && tset->scaleChar[0]<0) ||
        (tset->flags&TICKSET_YSCALECHAR && tset->scaleChar[1]<0))
        return bombre("invalid -ticksettings syntax", ticksettings_usage, 0);
    tset->flags |= oldFlags;
    if (tset->flags&TICKSET_LINETYPE) {
        if (!(tset->flags&TICKSET_XLINETYPE))
            tset->linetype[0] = linetype;
        if (!(tset->flags&TICKSET_YLINETYPE))
            tset->linetype[1] = linetype;
        tset->flags |= TICKSET_XLINETYPE|TICKSET_YLINETYPE;
        }
    if (tset->flags&TICKSET_XTHICKNESS) {
      if (tset->thickness[0] < 0)
	tset->thickness[0] = 0;
      if (tset->thickness[0] > 10)
	tset->thickness[0] = 9;
    }
    if (tset->flags&TICKSET_YTHICKNESS) {
      if (tset->thickness[1] < 0)
	tset->thickness[1] = 0;
      if (tset->thickness[1] > 10)
	tset->thickness[1] = 9;
    }
    if (tset->flags&TICKSET_THICKNESS) {
      if (thickness < 0)
	thickness = 0;
      if (thickness > 10)
	thickness = 9;
      if (!(tset->flags&TICKSET_XTHICKNESS))
	tset->thickness[0] = thickness;
      if (!(tset->flags&TICKSET_YTHICKNESS))
            tset->thickness[1] = thickness;
      tset->flags |= TICKSET_XTHICKNESS|TICKSET_YTHICKNESS;
    }
    if (tset->labelThickness[0] < 0)
      tset->labelThickness[0] = 0;
    if (tset->labelThickness[0] > 10)
      tset->labelThickness[0] = 9;
    if (tset->labelThickness[1] < 0)
      tset->labelThickness[1] = 0;
    if (tset->labelThickness[1] > 10)
      tset->labelThickness[1] = 9;
    if (labelThickness < 0)
      labelThickness = 0;
    if (labelThickness > 10)
      labelThickness = 9;
    if ((labelThickness != 0) && (tset->labelThickness[0] == 0))
      tset->labelThickness[0] = labelThickness;
    if ((labelThickness != 0) && (tset->labelThickness[1] == 0))
      tset->labelThickness[1] = labelThickness;
    if (tset->flags&TICKSET_FRACTION) {
        if (!(tset->flags&TICKSET_XFRACTION))
            tset->fraction[0] = fraction;
        if (!(tset->flags&TICKSET_YFRACTION))
            tset->fraction[1] = fraction;
        tset->flags |= TICKSET_XFRACTION|TICKSET_YFRACTION;
        }
    tset->flags |= TICKSET_GIVEN;
    return 1;
    }

long labelsize_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    if (items!=1 || sscanf(item[0], "%lf", &plreq->labelsize_fraction)!=1 ||
        plreq->labelsize_fraction<=0)
        return bombre("invalid -labelsize syntax", "-labelsize=<fraction>", 0);
    return 1;
    }


long enumeratedscales_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;

    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    if (items<1 ||
        !scanItemList(&plreq->enumerate_settings.flags,
                        item, &items,  0,
                        "rotate", -1, NULL, 0, ENUM_ROTATE,
                        "scale", SDDS_DOUBLE, &plreq->enumerate_settings.scale, 1, ENUM_SCALEGIVEN,
                        "interval", SDDS_LONG, &plreq->enumerate_settings.interval, 1, ENUM_INTERVALGIVEN,
                        "limit", SDDS_LONG, &plreq->enumerate_settings.limit, 1, ENUM_LIMITGIVEN,
                        "allticks", -1, NULL, 0, ENUM_ALLTICKS,
                        "editcommand", SDDS_STRING, &plreq->enumerate_settings.editcommand, 1, ENUM_EDITCOMMANDGIVEN,
                        NULL) ||
        (plreq->enumerate_settings.flags&ENUM_INTERVALGIVEN && plreq->enumerate_settings.interval<=0) ||
        (plreq->enumerate_settings.flags&ENUM_LIMITGIVEN && plreq->enumerate_settings.limit<=0) ||
        (plreq->enumerate_settings.flags&ENUM_SCALEGIVEN && plreq->enumerate_settings.scale<=0) )
        return bombre("invalid -enumeratedscales syntax",
                      "-enumeratedscales=[interval=<integer>][,limit=<integer>][,scale=<factor>][,allTicks][,rotate][,editcommand=<string>]", 0);
    return 1;
    }


char *arrowsettings_usage = "-arrowsettings=[scale=<value>][,barblength=<value>][,barbangle=<deg>][,linetype=<number>][,centered][,cartesiandata[,endpoints]][,polardata][,scalardata][,singlebarb][,autoscale][,thickness=<number>]";

long arrowsettings_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    ARROW_SETTINGS *arrow;

    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    arrow = &plreq->graphic.arrow;
    plreq->graphic.element = PLOT_ARROW;
    arrow->linetype = plreq->graphic.type;

    /*
      if (plreq->datanames>1) 
      bomb("can't have multiple datanames for arrow plotting", NULL);
      */

    if (items<1 ||
        !scanItemList(&arrow->flags,
                        item, &items,  0,
                        "centered", -1, NULL, 0, ARROW_CENTERED,
                        "scale", SDDS_DOUBLE, &arrow->scale, 1, ARROW_SCALE_GIVEN,
                        "barblength", SDDS_DOUBLE, &arrow->barbLength, 1, ARROW_BARBLENGTH_GIVEN,
                        "barbangle", SDDS_DOUBLE, &arrow->barbAngle, 1, ARROW_BARBANGLE_GIVEN,
                        "linetype", SDDS_LONG, &arrow->linetype, 1, ARROW_LINETYPE_GIVEN,
                        "thickness", SDDS_LONG, &arrow->thickness, 1, ARROW_THICKNESS_GIVEN,
                        "cartesiandata", -1, NULL, 0, ARROW_CARTESIAN_DATA,
                        "polardata", -1, NULL, 0, ARROW_POLAR_DATA,
                        "scalardata", -1, NULL, 0, ARROW_SCALAR_DATA,
                        "singlebarb", -1, NULL, 0, ARROW_SINGLEBARB,
                        "autoscale", -1, NULL, 0, ARROW_AUTOSCALE,
                        "endpoints", -1, NULL, 0, ARROW_ENDPOINTS,
                        NULL))
        return bombre("invalid -arrowsettings syntax", arrowsettings_usage, 0);
    if (arrow->flags&ARROW_ENDPOINTS) {
        if (arrow->flags&ARROW_POLAR_DATA || arrow->flags&ARROW_SCALAR_DATA)
            return bombre("invalid -arrowsettings syntax---endpoints keyword not compatible with polar or scalar keywords",
                          NULL, 0);
        if (arrow->flags&ARROW_AUTOSCALE)
            return bombre("invalid -arrowsettings syntax---endpoints keyword not compatible with autoscale keyword", NULL, 0);
        arrow->flags |= ARROW_CARTESIAN_DATA;
        }
    return 1;
    }


long listDevices_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  list_terms(stderr);
  exit(1);
  return(1);
}


long limit_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    LIMIT_SETTINGS defaultLimits;
    
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    defaultLimits = plreq->limit;
    plreq->limit.flags = 0;
    if (items<1 ||
        !scanItemList(&plreq->limit.flags, item, &items, 0,
                        "xminimum", SDDS_DOUBLE, &plreq->limit.xmin, 1, LIMIT_XMIN_GIVEN,
                        "yminimum", SDDS_DOUBLE, &plreq->limit.ymin, 1, LIMIT_YMIN_GIVEN,
                        "xmaximum", SDDS_DOUBLE, &plreq->limit.xmax, 1, LIMIT_XMAX_GIVEN,
                        "ymaximum", SDDS_DOUBLE, &plreq->limit.ymax, 1, LIMIT_YMAX_GIVEN,
                        "autoscaling", -1, NULL, 0, LIMIT_AUTOSCALING,
                        NULL))
        return bombre("invalid -limit syntax", "-limit=[{x| y}{min | max}imum=<value>][,autoscaling]", 0);
    if (defaultLimits.flags&LIMIT_XMIN_GIVEN && !(plreq->limit.flags&LIMIT_XMIN_GIVEN)) {
        plreq->limit.flags |= LIMIT_XMIN_GIVEN;
        plreq->limit.xmin = defaultLimits.xmin;
        }
    if (defaultLimits.flags&LIMIT_XMAX_GIVEN && !(plreq->limit.flags&LIMIT_XMAX_GIVEN)) {
        plreq->limit.flags |= LIMIT_XMAX_GIVEN;
        plreq->limit.xmax = defaultLimits.xmax;
        }
    if (defaultLimits.flags&LIMIT_YMIN_GIVEN && !(plreq->limit.flags&LIMIT_YMIN_GIVEN)) {
        plreq->limit.flags |= LIMIT_YMIN_GIVEN;
        plreq->limit.ymin = defaultLimits.ymin;
        }    
    if (defaultLimits.flags&LIMIT_YMAX_GIVEN && !(plreq->limit.flags&LIMIT_YMAX_GIVEN)) {
        plreq->limit.flags |= LIMIT_YMAX_GIVEN;
        plreq->limit.ymax = defaultLimits.ymax;
        }
    return 1;
    }

long intensityBar_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;

  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  plreq->intensityBar_settings.flags = 0;
  if (items<1 ||
      !scanItemList(&plreq->intensityBar_settings.flags, item, &items, 0,
                    "labelsize", SDDS_DOUBLE, &plreq->intensityBar_settings.labelsize, 1, INTENSITYBAR_LABELSIZE_GIVEN,
                    "unitsize", SDDS_DOUBLE, &plreq->intensityBar_settings.unitsize, 1, INTENSITYBAR_UNITSIZE_GIVEN,
                    "xadjust", SDDS_DOUBLE, &plreq->intensityBar_settings.xadjust, 1, INTENSITYBAR_XADJUST_GIVEN,
                    NULL))
    return bombre("invalid -intensityBar syntax", "-intensityBar=[labelsize=<value>][,unitsize=<value>][,xadjust=<value>]\nThe defaults are -intensityBar=labelsize=1,unitsize=1,xadjust=0", 0);
  
  return 1;
}


long range_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->range.flags = 0;
    if (items<1 ||
        !scanItemList(&plreq->range.flags, item, &items, 0,
                        "xminimum", SDDS_DOUBLE, plreq->range.minimum+0, 1, XRANGE_MINIMUM,
                        "xmaximum", SDDS_DOUBLE, plreq->range.maximum+0, 1, XRANGE_MAXIMUM,
                        "xcenter" , SDDS_DOUBLE, plreq->range.center+0 , 1, XRANGE_CENTER,
                        "yminimum", SDDS_DOUBLE, plreq->range.minimum+1, 1, YRANGE_MINIMUM,
                        "ymaximum", SDDS_DOUBLE, plreq->range.maximum+1, 1, YRANGE_MAXIMUM,
                        "ycenter" , SDDS_DOUBLE, plreq->range.center+1 , 1, YRANGE_CENTER,
                        NULL))
        return bombre("invalid -range syntax", "-range=[{x| y}{minimum | maximum | center}=<value>]", 0);
    return 1;
    }


long namescan_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    plreq->nameScanFlags = 0;
    if (items<1 ||
        !scanItemList(&plreq->nameScanFlags, item, &items, 0,
			"all", -1, NULL, 0, NAMESCAN_ALL,
			"first", -1, NULL, 0, NAMESCAN_FIRST,
                        NULL) ||
	!plreq->nameScanFlags || 
	(plreq->nameScanFlags&NAMESCAN_ALL && plreq->nameScanFlags&NAMESCAN_FIRST))
        return bombre("invalid -namescan syntax", "-namescan={all | first}", 0);
    return 1;
    }

static char *pointLabel_usage = "-pointlabel=<name>[,edit=<editCommand>][,scale=<number>][,justifyMode={rcl}{bct}][,thickness=<integer>[,lineType=<integer>]]";

long pointlabel_AP(PLOT_SPEC *plotspec, char **item, long items)
{
    PLOT_REQUEST *plreq;
    
    if (items<1) 
      return bombre("invalid -pointLabel syntax", pointLabel_usage, 0);
    
    plreq = &plotspec->plot_request[plotspec->plot_requests-1];
    SDDS_CopyString(&plreq->pointLabelSettings.name, item[0]);
    item++;
    items--;
    plreq->pointLabelSettings.editCommand = plreq->pointLabelSettings.justifyMode = NULL;
    plreq->pointLabelSettings.scale = 1;
    plreq->pointLabelSettings.thickness = 1;

    if (!scanItemList(&plreq->pointLabelSettings.flags, item, &items, 0,
                      "edit", SDDS_STRING, &plreq->pointLabelSettings.editCommand, 1, 0,
                      "scale", SDDS_DOUBLE, &plreq->pointLabelSettings.scale, 1, 0,
                      "thickness", SDDS_LONG, &plreq->pointLabelSettings.thickness, 1, 0,
                      "justify", SDDS_STRING, &plreq->pointLabelSettings.justifyMode, 1, 0,
                      "linetype",SDDS_LONG, &plreq->pointLabelSettings.linetype, 1, POINTLABEL_LINETYPE_GIVEN,
                      NULL) ||
        (plreq->pointLabelSettings.justifyMode && strlen(plreq->pointLabelSettings.justifyMode)!=2))
      return bombre("invalid -pointLabel syntax", pointLabel_usage, 0);
    return 1;
    }

static char *replicate_usage = "-replicate={number=<integer> | match={names | pages | requests | files}}[,scroll]";

long replicate_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;
  char *matchMode;
  static char *matchName[4] = {"names", "pages", "requests", "files" };
  
  if (items<1)
    return bombre("invalid -replicate syntax", pointLabel_usage, 0);
  
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  if (!scanItemList(&plreq->replicateSettings.flags, item, &items, 0, 
                    "number", SDDS_LONG, &plreq->replicateSettings.number, 1, REPLICATE_NUMBER,
                    "match", SDDS_STRING, &matchMode, 1, REPLICATE_MATCH,
                    "scroll", -1, NULL, 0, REPLICATE_SCROLL_MODE,
                    NULL) || !plreq->replicateSettings.flags ||
      (plreq->replicateSettings.flags&REPLICATE_NUMBER &&
      plreq->replicateSettings.flags&REPLICATE_MATCH ))
    return bombre("invalid -replicate syntax", replicate_usage, 0);
  
  if (plreq->replicateSettings.flags&REPLICATE_MATCH) {
    switch (match_string(matchMode, matchName, 4, 0)) {
    case 0:
      plreq->replicateSettings.flags |= REPLICATE_MATCH_NAMES;
      break;
    case 1:
      plreq->replicateSettings.flags |= REPLICATE_MATCH_PAGES;
      break;
    case 2:
      plreq->replicateSettings.flags |= REPLICATE_MATCH_REQUESTS;
      break;
    case 3:
      plreq->replicateSettings.flags |= REPLICATE_MATCH_FILES;
      break;
    default:
      return bombre("invalid -replicate sytnax: unknown/ambiguous match mode", replicate_usage, 0);
    }
  }
  free(matchMode);
  return 1;
}

long alignzero_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  PLOT_REQUEST *plreq;
  unsigned long flags;
  long plane;
  
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];
  
  if (!scanItemList(&flags, item, &items, 0,
                    "xcenter", -1, NULL, 0, ALIGNZERO_XCENTER_GIVEN,
                    "ycenter", -1, NULL, 0, ALIGNZERO_YCENTER_GIVEN,
                    "xfactor", -1, NULL, 0, ALIGNZERO_XFACTOR_GIVEN,
                    "yfactor", -1, NULL, 0, ALIGNZERO_YFACTOR_GIVEN,
                    "ppin", SDDS_DOUBLE, &plreq->alignSettings.pinUnitSpace[0], 1, ALIGNZERO_PPIN_GIVEN, 
                    "qpin", SDDS_DOUBLE, &plreq->alignSettings.pinUnitSpace[1], 1, ALIGNZERO_QPIN_GIVEN, 
                    NULL) || !flags)
    return bombre("invalid -alignZero syntax", "-alignZero[={xcenter|xfactor|pPin=<value>}][,{ycenter|yfactor|qPin=<value>}]", 0);
  for (plane=0; plane<2; plane++) {
    if (flags&(ALIGNZERO_XCENTER_GIVEN<<plane)) {
      plreq->alignSettings.pinUnitSpace[0] = 0.5;
      flags &= ~((ALIGNZERO_XCENTER_GIVEN<<plane));
      flags |= (ALIGNZERO_PPIN_GIVEN<<plane);
    }
  }
  plreq->alignSettings.flags = flags;
  
  return 1;
}

static char *orderColors_usage = "-orderColors={spectral|rspectral|start=(<red>,<green>,<blue>){[,finish=(<red>,<green>,<blue>)]|[,increment=(<red>,<green>,<blue>)]}}\n\
All colors range from 0 to 65535\n";

#define ORDERCOLORS_KW_START 0
#define ORDERCOLORS_KW_FINISH 1
#define ORDERCOLORS_KW_INCREMENT 2
#define ORDERCOLORS_KW_SPECTRAL 3
#define ORDERCOLORS_KW_RSPECTRAL 4
#define ORDERCOLORS_KWS 5
static char *ordercolors_kw[ORDERCOLORS_KWS] = {
  "start", "finish", "increment", "spectral", "rspectral"
};
long orderColors_AP(PLOT_SPEC *plotspec, char **item, long items)
{
  int i;
  char *eqptr, *commaptr;
  PLOT_REQUEST *plreq;
  plreq = &plotspec->plot_request[plotspec->plot_requests-1];

  if (items < 1) {
    fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
    return 0;
  }
  for (i=0; i<items; i++) {
    if ((eqptr=strchr(item[i], '=')))
      *eqptr = 0;
    switch (match_string(item[i], ordercolors_kw, ORDERCOLORS_KWS, 0)) {
    case ORDERCOLORS_KW_START:
      if (eqptr == NULL) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.red[0] = (unsigned short)atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.green[0] = (unsigned short)atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.blue[0] = (unsigned short)atol(eqptr+1);
      plreq->color_settings.flags |= COLORSET_START;
      break;
    case ORDERCOLORS_KW_FINISH:
      if (eqptr == NULL) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.red[1] = (unsigned short)atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.green[1] = (unsigned short)atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.blue[1] = (unsigned short)atol(eqptr+1);
      plreq->color_settings.flags |= COLORSET_FINISH;
      break;
    case ORDERCOLORS_KW_INCREMENT:
      if (eqptr == NULL) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.increment[0] = atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	*commaptr = 0;
      } else {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.increment[1] = atol(eqptr+1);
      eqptr = commaptr;
      if ((commaptr=strchr(eqptr+1, ','))) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      if (!eqptr || SDDS_StringIsBlank(eqptr+1)) {
	fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
	return 0;
      }
      plreq->color_settings.increment[2] = atol(eqptr+1);
      plreq->color_settings.flags |= COLORSET_INCREMENT;
      break;
    case ORDERCOLORS_KW_SPECTRAL:
      plreq->color_settings.flags |= COLORSET_SPECTRAL;
      break;
    case ORDERCOLORS_KW_RSPECTRAL:
      plreq->color_settings.flags |= COLORSET_RSPECTRAL;
      break;
    default:
      fprintf (stderr, "error: invalid -orderColors syntax\nusage: %s\n", orderColors_usage);
      return 0;
    }
  }
  return 1;
}


