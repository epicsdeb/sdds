/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: hpwf2sdds.c
 * purpose: converts a waveform from an HP scope to 
 *          SDDS format
 *
 * M. Borland, 1994
 $Log: hpwf2sdds.c,v $
 Revision 1.5  2002/08/14 17:12:37  soliday
 Added Open License

 Revision 1.4  2001/01/23 19:14:55  soliday
 Standardized usage message.

 Revision 1.3  1999/05/25 19:02:49  soliday
 Removed compiler warning on linux.

 Revision 1.2  1995/09/06 14:55:50  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "match_string.h"

#define SET_SIGNAL_NAME 0
#define SET_DESCRIPTION 1
#define SET_MPL_LABELS 2
#define N_OPTIONS 3

static char *option[N_OPTIONS] = {
    "signalname", "description", "mpllabels"
        } ;

char *USAGE = "hpwf2sdds <inputfile> <outputfile>\n\
 -signalname=<name> [-description=<text>,<contents>]\n\
 [-mpllabels=<title>,<topline>]\n\n\
This program converts HP verbose format waveforms to SDDS format.\n\n\
Program by Michael Borland (This is Version 1, March 1994.)\n";

typedef struct {
    char *HP_name, *SDDS_name, *value_string;
    long type;
    } HP_PARAMETER;

#define HP_PARAMETERS 23
HP_PARAMETER HP_parameter[HP_PARAMETERS] = {
    {"Type", "Type", NULL, SDDS_STRING} ,
    {"Points", "Points", NULL, SDDS_LONG},
    {"Count", "Count", NULL, SDDS_LONG},
    {"XInc", "XInc", NULL, SDDS_DOUBLE},
    {"XOrg", "XOrg", NULL, SDDS_DOUBLE},
    {"XRef", "XRef", NULL, SDDS_DOUBLE},
    {"YData range", "YDataRange", NULL, SDDS_DOUBLE},
    {"YData center", "YDataCenter", NULL, SDDS_DOUBLE},
    {"Coupling", "Coupling", NULL, SDDS_STRING},
    {"XRange", "XRange", NULL, SDDS_DOUBLE},
    {"XOffset", "XOffset", NULL, SDDS_DOUBLE},
    {"YRange", "YRange", NULL, SDDS_DOUBLE},
    {"YOffset", "YOffset", NULL, SDDS_DOUBLE},
    {"Date", "Date", NULL, SDDS_STRING},
    {"Time", "Time", NULL, SDDS_STRING},
    {"Frame", "Frame", NULL, SDDS_STRING},
    {"Acq mode", "AcqMode", NULL, SDDS_STRING},
    {"Completion", "Completion", NULL, SDDS_STRING},
    {"X Units", "XUnits", NULL, SDDS_STRING},
    {"Y Units", "YUnits", NULL, SDDS_STRING},
    {"Max Bandwidth", "MaxBandwidth", NULL, SDDS_DOUBLE},
    {"Min Bandwidth", "MinBandwidth", NULL, SDDS_DOUBLE},
    {NULL, NULL, NULL, 0},
    } ;

char *HP_DataMarker = "Data";
char *HP_XIncrementName = "XInc";
char *HP_XOriginName = "XOrg";
char *HP_XReferenceName = "XRef";
char *HP_XUnitsName = "X Units";
char *HP_YUnitsName = "Y Units";
char *HP_PointsName = "Points";

#define BUFSIZE 256

int main(int argc, char **argv)
{
    SDDS_TABLE SDDS_table;
    SCANNED_ARG *scanned;
    long i, i_arg, index, points;
    char *input, *output, buffer[BUFSIZE];
    char *signal_name, *ptr, *parameter_name;
    char *mpl_title, *mpl_topline, *descrip_text, *descrip_contents;
    FILE *fpi;
    double xIncrement, xOrigin, xReference;
    char *xUnits, *yUnits;
    double *time, *data;

    argc = scanargs(&scanned, argc, argv);
    if (argc<3)
        bomb(NULL, USAGE);

    input = output = signal_name = xUnits = yUnits = NULL;
    mpl_title = mpl_topline = descrip_text = descrip_contents = NULL;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (scanned[i_arg].arg_type==OPTION) {
            delete_chars(scanned[i_arg].list[0], "_");
            /* process options here */
            switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_SIGNAL_NAME:
                if (scanned[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -signalname syntax");
                signal_name = scanned[i_arg].list[1];
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
    if (!signal_name)
        SDDS_Bomb("-signal_name not seen");

    fpi = fopen_e(input, "r", 0);
    parameter_name = buffer;
    while (fgets(buffer, BUFSIZE, fpi)) {
        if (!(ptr=strchr(buffer, ':')))
            SDDS_Bomb("error parsing input file--missing colon on parameter tag");
        *ptr++ = 0;
        if (strcmp(HP_DataMarker, parameter_name)==0)
            break;
        index = 0;
        while (HP_parameter[index].HP_name) {
            if (strcmp(HP_parameter[index].HP_name, parameter_name)==0)
                break;
            index++;
            }
        if (!HP_parameter[index].HP_name) {
            fprintf(stderr, "error: parameter name %s is not recognized\n",
                    parameter_name);
            exit(1);
            }
        if (HP_parameter[index].value_string) {
            fprintf(stderr, "error: duplicate entries for parameter %s\n",
                    parameter_name);
            exit(1);
            }
        SDDS_RemovePadding(ptr);
        cp_str(&HP_parameter[index].value_string, ptr);
        }
    
    if (!SDDS_InitializeOutput(&SDDS_table, SDDS_BINARY, 0, descrip_text, descrip_contents,
                               output))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    index = 0;
    while (HP_parameter[index].HP_name) {
        if (!HP_parameter[index].value_string) {
            index++;
            continue;
            }
        if (strcmp(HP_parameter[index].HP_name, HP_XIncrementName)==0) {
            if (sscanf(HP_parameter[index].value_string, "%lf", &xIncrement)!=1)
                SDDS_Bomb("unable to scan value for x increment");
            }
        else if (strcmp(HP_parameter[index].HP_name, HP_XOriginName)==0) {
            if (sscanf(HP_parameter[index].value_string, "%lf", &xOrigin)!=1)
                SDDS_Bomb("unable to scan value for x origin");
            }
        else if (strcmp(HP_parameter[index].HP_name, HP_XReferenceName)==0) {
            if (sscanf(HP_parameter[index].value_string, "%lf", &xReference)!=1)
                SDDS_Bomb("unable to scan value for x reference");
            }
        else if (strcmp(HP_parameter[index].HP_name, HP_XUnitsName)==0) {
            xUnits = HP_parameter[index].value_string;
            }
        else if (strcmp(HP_parameter[index].HP_name, HP_YUnitsName)==0) {
            yUnits = HP_parameter[index].value_string;
            }
        else if (strcmp(HP_parameter[index].HP_name, HP_PointsName)==0) {
            if (sscanf(HP_parameter[index].value_string, "%ld", &points)!=1)
                SDDS_Bomb("unable to scan value for number of points");
            }
        if (SDDS_DefineParameter(&SDDS_table, HP_parameter[index].SDDS_name,
                                  NULL, NULL, HP_parameter[index].HP_name,
                                  NULL, HP_parameter[index].type, 
                                  HP_parameter[index].value_string)<0)
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        index++;
        }

    if (mpl_title &&
        (SDDS_DefineParameter(&SDDS_table, "mplTitle", NULL, NULL, NULL, NULL, SDDS_STRING, mpl_title)<0 ||
         SDDS_DefineParameter(&SDDS_table, "mplTopline", NULL, NULL, NULL, NULL, SDDS_STRING, mpl_topline)<0))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    if (SDDS_DefineColumn(&SDDS_table, "t", NULL, xUnits, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineColumn(&SDDS_table, signal_name, NULL, yUnits, NULL, NULL, SDDS_DOUBLE, 0)<0 ||
        !SDDS_WriteLayout(&SDDS_table) || !SDDS_StartTable(&SDDS_table, points))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fflush(SDDS_table.layout.fp);

    data = tmalloc(sizeof(*data)*points);
    time = tmalloc(sizeof(*time)*points);
    for (i=0; i<points; i++) {
        if (!fgets(buffer, BUFSIZE, fpi))
            SDDS_Bomb("insufficient data in input file");
        time[i] = xOrigin + (i-xReference)*xIncrement;
        if (sscanf(buffer, "%lf", data+i)!=1)
            SDDS_Bomb("invalid data in input file");
        }

    if (!SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, time, points, "t") ||
        !SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, data, points, signal_name) ||
        !SDDS_WriteTable(&SDDS_table) || !SDDS_Terminate(&SDDS_table))
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

