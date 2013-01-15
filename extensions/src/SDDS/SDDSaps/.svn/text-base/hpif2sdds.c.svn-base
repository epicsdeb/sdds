/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: hpif2sdds.c
 * purpose: converts a waveform from an HP scope to 
 *          SDDS format
 *
 * M. Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.5  2001/01/23 19:14:55  soliday
 Standardized usage message.

 Revision 1.4  1999/05/25 19:02:36  soliday
 Removed compiler warning on linux.

 Revision 1.3  1999/01/06 19:54:35  borland
 Fixed the version number in the usage message.

 * Revision 1.2  1995/09/06  14:55:48  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "match_string.h"
#include <ctype.h>

#define SET_SIGNAL_NAME 0
#define SET_DESCRIPTION 1
#define SET_MPL_LABELS 2
#define SET_INITIAL_T 3
#define SET_SINGLE_PRECISION 4
#define SET_SEGMENT_INTERVAL 5
#define SET_SEPARATE_SEGMENTS 6
#define SET_PAIRS 7
#define N_OPTIONS 8

static char *option[N_OPTIONS] = {
    "signalname", "description", "mpllabels",
    "initialt", "singleprecision", "segmentinterval",
    "separatesegments", "pairs",
    } ;

char *USAGE = "hpif2sdds <inputfile> <outputfile>\n\
 -signalname={<name>|<name1>,<name2>}\n\
 [-description=<text>,<contents>]\n\
 [-mpllabels=<title>,<topline>]\n\
 [-initialt=<value>] [-singleprecision]\n\
 [-segmentinterval=<time-in-secs>]\n\
 [-separatesegments] [-pairs]\n\n\
This program converts HP internal format waveforms to SDDS format.\n\n\
Program by Michael Borland (This is Version 2, September 1995.)\n";

#define BUFSIZE 256
long get_next_field(char *buffer, FILE *fp);
long set_2d_parameters(SDDS_TABLE *table, char *variable, double initial, long dimension, double increment);
long define_2d_parameters(SDDS_TABLE *table, char *parameter, char *variable, char *units);
void swapdouble(double *data);

int main(int argc, char **argv)
{
    SDDS_TABLE SDDS_table;
    SCANNED_ARG *scanned;
    long i, i_arg, points, single_precision;
    char *input, *output, buffer[BUFSIZE];
    char *signal_name[2]; 
    char *mpl_title, *mpl_topline, *descrip_text, *descrip_contents;
    FILE *fpi;
    double xIncrement, xOrigin, yScale, yOffset;
    long xReference, yReference, segments, segment_length, separate_segments;
    double *time, *data, *data1;
    float *sp_time, *sp_data, *sp_data1;
    short *idata, *idata1;
    double initial_t, segment_interval;
    long code1, code2, pairs, size, length;

    argc = scanargs(&scanned, argc, argv);
    if (argc<3)
        bomb(NULL, USAGE);
    
    time = data = data1 = NULL;
    sp_time = sp_data = sp_data1 = NULL;
    input = output = NULL;
    mpl_title = mpl_topline = descrip_text = descrip_contents = NULL;
    initial_t = DBL_MAX;
    single_precision = 0;
    signal_name[0] = signal_name[1] = NULL;
    segment_interval = separate_segments = pairs = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (scanned[i_arg].arg_type==OPTION) {
            delete_chars(scanned[i_arg].list[0], "_");
            /* process options here */
            switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_SIGNAL_NAME:
                if (scanned[i_arg].n_items!=2 && scanned[i_arg].n_items!=3)
                    SDDS_Bomb("invalid -signalname syntax");
                signal_name[0] = scanned[i_arg].list[1];
                if (scanned[i_arg].n_items==3)
                    signal_name[1] = scanned[i_arg].list[2];
                break;
              case SET_DESCRIPTION:
                if (scanned[i_arg].n_items!=3)
                    SDDS_Bomb("invalid -description syntax");
                descrip_text = scanned[i_arg].list[1];
                descrip_contents = scanned[i_arg].list[2];
                break;
              case SET_MPL_LABELS:
                if (scanned[i_arg].n_items!=3)
                    SDDS_Bomb("invalid -mpllabels syntax");
                mpl_title = scanned[i_arg].list[1];
                mpl_topline = scanned[i_arg].list[2];
                break;
              case SET_INITIAL_T:
                if (scanned[i_arg].n_items!=2 ||
                    sscanf(scanned[i_arg].list[1], "%lf", &initial_t)!=1)
                    SDDS_Bomb("invalid -initialt syntax");
                break;
              case SET_SINGLE_PRECISION:
                single_precision = 1;
                break;
              case SET_SEGMENT_INTERVAL:
                if (scanned[i_arg].n_items!=2 ||
                    sscanf(scanned[i_arg].list[1], "%lf", &segment_interval)!=1)
                    SDDS_Bomb("invalid -segmentinterval syntax");
                break;
	      case SET_SEPARATE_SEGMENTS:
		separate_segments = 1;
		break;
              case SET_PAIRS:
                pairs = 1;
                break;
              default:
                SDDS_Bomb("invalid option seen");
                break;
                }
            }
        else {
            if (!input)
                input = scanned[i_arg].list[0];
            else if (!output)
                output = scanned[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }
    if (!input)
        SDDS_Bomb("input file not seen");
    if (!output)
        SDDS_Bomb("output file not seen");

    fpi = fopen_e(input, "r", 0);
    length = strlen(":WAV:PRE ");
    if (fread(buffer, 1, length, fpi)!=length || strncmp(buffer, ":WAV:PRE ", length)!=0) {
        printf("buffer = \"%s\"\n", buffer);
        SDDS_Bomb("file does not appear to be in HP internal format (1)");
        }
    if (!get_next_field(buffer, fpi) ||
	sscanf(buffer, "%ld", &code1)!=1)
        SDDS_Bomb("file does not appear to be in HP internal format (2)");
    if (!get_next_field(buffer, fpi) ||
	sscanf(buffer, "%ld", &code2)!=1)
        SDDS_Bomb("file does not appear to be in HP internal format (3)");
    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%ld", &segment_length)!=1)
        SDDS_Bomb("file does not appear to be in HP internal format (4)");
    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%ld", &segments)!=1)
        SDDS_Bomb("file does not appear to be in HP internal format (5)");
    points = segments*segment_length;
 
#if defined(DEBUG)
    fprintf(stderr, "%ld points, %ld segments\n", points, segments);
    if (pairs)
      fprintf(stderr, "data appears to come in point pairs\n");
#endif

    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%le", &xIncrement)!=1)
        SDDS_Bomb("unable to get x increment");
#if defined(DEBUG)
    fprintf(stderr, "x increment = %e\n", xIncrement);
#endif

    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%le", &xOrigin)!=1)
        SDDS_Bomb("unable to get x origin");
#if defined(DEBUG)
    fprintf(stderr, "x origin = %e\n", xOrigin);
#endif

    if (!get_next_field(buffer, fpi) || 
        sscanf(buffer, "%ld", &xReference)!=1)
        SDDS_Bomb("unable to get x reference");
#if defined(DEBUG)
    fprintf(stderr, "x reference = %ld\n", xReference);
#endif

    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%lf", &yScale)!=1)
        SDDS_Bomb("unable to get y scale factor");
#if defined(DEBUG)
    fprintf(stderr, "y scale = %e\n", yScale);
#endif

    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%lf", &yOffset)!=1)
        SDDS_Bomb("unable to get y offset");
#if defined(DEBUG)
    fprintf(stderr, "y offset = %e\n", yOffset);
#endif

    if (!get_next_field(buffer, fpi) ||
        sscanf(buffer, "%ld", &yReference)!=1)
        SDDS_Bomb("unable to get y reference");
#if defined(DEBUG)
    fprintf(stderr, "y reference = %ld\n", yReference);
#endif

    while (fread(buffer, 1, 1, fpi))
        if (buffer[0] == '#')
            break;
    fread(buffer, 1, 9, fpi);
    if (!sscanf(buffer+1, "%ld", &size))
        SDDS_Bomb("unable to read size of data block");
#if defined(DEBUG)
    fprintf(stderr, "data block is %ld bytes\n", size);
#endif    

    if (segments!=1) {
        double segment_offset;
        for (i=0; i<segments; i++) {
            if (!fread(&segment_offset, sizeof(segment_offset), 1, fpi))
                SDDS_Bomb("unable to read segment offsets");
            }
        }


    idata = tmalloc(sizeof(*idata)*points);
    idata1 = tmalloc(sizeof(*idata1)*points);
    if (!single_precision) {
        data = tmalloc(sizeof(*data)*points);
        data1 = tmalloc(sizeof(*data1)*points);
        }  
    else { 
        sp_data = tmalloc(sizeof(*sp_data)*points);
        sp_data1 = tmalloc(sizeof(*sp_data1)*points);
        }
#if defined(DEBUG)
    fprintf(stderr, "arrays allocated\n");
#endif

    if (!fread(idata, 2, points, fpi))
        SDDS_Bomb("unable to read data");
    if (pairs && !fread(idata1, 2, points, fpi))
        SDDS_Bomb("unable to read second data block");
#if defined(DEBUG)
    fprintf(stderr, "data read in\n");
#endif

    if (!single_precision) {
        for (i=0; i<points; i++) {
            data[i] = (idata[i]-yReference)*yScale +yOffset; 
	    data1[i] = (idata1[i] - yReference)*yScale + yOffset;
  	    }
        }
    else {
        for (i=0; i<points; i++) {
            sp_data[i] = (idata[i]-yReference)*yScale +yOffset; 
            sp_data1[i] = (idata1[i]-yReference)*yScale +yOffset; 
	    }
        }

#if defined(DEBUG)
    fprintf(stderr, "data copied\n");
#endif

    if (!SDDS_InitializeOutput(&SDDS_table, SDDS_BINARY, 0, descrip_text, descrip_contents,
                               output))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
#if defined(DEBUG)
    fprintf(stderr, "output initialized\n");
#endif

    if (mpl_title &&
        (SDDS_DefineParameter(&SDDS_table, "mplTitle", NULL, NULL, NULL, NULL, SDDS_STRING, mpl_title)<0 ||
         SDDS_DefineParameter(&SDDS_table, "mplTopline", NULL, NULL, NULL, NULL, SDDS_STRING, mpl_topline)<0))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
#if defined(DEBUG)
    fprintf(stderr, "title defined\n");
#endif

    if (segments!=1) {
        if (!define_2d_parameters(&SDDS_table, "Variable1Name", "Segment", NULL)<0 ||
            !define_2d_parameters(&SDDS_table, "Variable2Name", "t", "s")<0 )
            exit(1);
        }

    if (signal_name[0]==NULL) {
        if (!pairs)
            signal_name[0] = "V";
        else {
            signal_name[0] = "V1";
            signal_name[1] = "V2";
            }
        }

    if (pairs && !signal_name[1])
        SDDS_Bomb("data contains voltage pairs--supply two signal names");

    if (SDDS_DefineColumn(&SDDS_table, "t", NULL, "s", NULL, NULL, 
                          (single_precision?SDDS_FLOAT:SDDS_DOUBLE), 0)<0 ||
        SDDS_DefineColumn(&SDDS_table, signal_name[0], NULL, "V", NULL, NULL, 
                          (single_precision?SDDS_FLOAT:SDDS_DOUBLE), 0)<0 ||
        (pairs && SDDS_DefineColumn(&SDDS_table, signal_name[1], NULL, "V", NULL, NULL, 
                                    (single_precision?SDDS_FLOAT:SDDS_DOUBLE), 0)<0) ||
        !SDDS_WriteLayout(&SDDS_table))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fflush(SDDS_table.layout.fp);

    if (!single_precision) {
        time = tmalloc(sizeof(*time)*points);
        if (initial_t==DBL_MAX)
            initial_t = xOrigin-xReference*xIncrement;
        for (i=0; i<points; i++) {
            if (segments!=1 && i && i%segment_length==0)
                initial_t += segment_interval-segment_length*xIncrement;
            time[i] = i*xIncrement+initial_t;
            }
        }
    else { 
        sp_time = tmalloc(sizeof(*sp_time)*points);
        if (initial_t==DBL_MAX)
            initial_t = xOrigin-xReference*xIncrement;
        for (i=0; i<points; i++) {
            if (segments!=1 && i && i%segment_length==0)
                initial_t += segment_interval-segment_length*xIncrement;
            sp_time[i] = i*xIncrement+initial_t;
            }
        }
       

    i = 1;
    do {
        if (!SDDS_StartTable(&SDDS_table, separate_segments?segment_length:points))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        if (segments!=1 && i==1) {
            if (!set_2d_parameters(&SDDS_table, "Segment", 0.0, segments, 1.0) ||
                !set_2d_parameters(&SDDS_table, "t", initial_t, segment_length, xIncrement))
                exit(1);
            }
        
#if defined(DEBUG)
        fprintf(stderr, "row and column parameters set for table %ld\n", i);
#endif
        
        if (!SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, 
                            single_precision?(void*)(sp_time+(i-1)*segment_length)
                            :(void*)(time+(i-1)*segment_length),
                            separate_segments?segment_length:points, "t") ||
            !SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, 
                            single_precision?(void*)(sp_data+(i-1)*segment_length)
                            :(void*)(data+(i-1)*segment_length),
                            separate_segments?segment_length:points, signal_name[0]) ||
            (pairs && !SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, 
                            single_precision?(void*)(sp_data1+(i-1)*segment_length)
                            :(void*)(data1+(i-1)*segment_length),
                            separate_segments?segment_length:points, signal_name[1])) ||
            !SDDS_WriteTable(&SDDS_table))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        } while (separate_segments && ++i<=segments);
    if (!SDDS_Terminate(&SDDS_table))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    return(0);
}

char *process_column_definition(char **argv, long argc)
{
    char buffer[SDDS_MAXLINE], *ptr;
    long i;

    if (argc<1)
        return(NULL);
    sprintf(buffer, "&column name=%s, ", argv[0]);
    for (i=1; i<argc; i++) {
        if (!strchr(argv[i], '='))
            return(NULL);
        strcat(buffer, argv[i]);
        strcat(buffer, ", ");
        }
    if (!strstr(buffer, "type="))
        strcat(buffer, "type=character ");
    strcat(buffer, "&end");
    cp_str(&ptr, buffer);
    return(ptr);
    }

long get_next_field(char *buffer, FILE *fp)
{
    long i;
    buffer[i=0] = 0;
    while ((buffer[i] = fgetc(fp))) {
        buffer[i] = tolower(buffer[i]);
        if (buffer[i]==',' || buffer[i]==';')
            break;
        buffer[++i] = 0;
        }
    buffer[i] = 0;
    return(1);
    }

long define_2d_parameters(SDDS_TABLE *table, char *parameter, char *variable, char *units)
{
    char buffer[BUFSIZE];
    SDDS_DefineParameter(table, parameter, NULL, NULL, NULL, NULL, SDDS_STRING, variable);
    sprintf(buffer, "%sMinimum", variable);
    SDDS_DefineParameter(table, buffer, NULL, units, NULL, NULL, SDDS_DOUBLE, NULL);
    sprintf(buffer, "%sInterval", variable);
    SDDS_DefineParameter(table, buffer, NULL, units, NULL, NULL, SDDS_DOUBLE, NULL);
    sprintf(buffer, "%sDimension", variable);
    SDDS_DefineParameter(table, buffer, NULL, NULL, NULL, NULL, SDDS_LONG, NULL);
    if (SDDS_NumberOfErrors()) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(0);
        }
    return(1);
    }

long set_2d_parameters(SDDS_TABLE *table, char *variable, double initial, long dimension, double increment)
{
    char buffer[BUFSIZE];
    sprintf(buffer, "%sMinimum", variable);
    SDDS_SetParameters(table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, buffer, initial, NULL);
    sprintf(buffer, "%sInterval", variable);
    SDDS_SetParameters(table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, buffer, increment, NULL);
    sprintf(buffer, "%sDimension", variable);
    SDDS_SetParameters(table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, buffer, dimension, NULL);
    if (SDDS_NumberOfErrors()) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(0);
        }
    return(1);
    }

void swapdouble(double *data)
{
    double copy;
    short i;
    copy = *data;
    for (i=0; i<8; i++) 
        *(((char*)data)+i) = *(((char*)&copy)+(8-i));
    }
