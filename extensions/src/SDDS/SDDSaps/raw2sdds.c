/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: raw2sdds
 * purpose: converts a raw data stream of binary data
 *          to SDDS format
 * M. Borland, 1994
 $Log: raw2sdds.c,v $
 Revision 1.5  2002/08/14 17:12:38  soliday
 Added Open License

 Revision 1.4  2001/01/23 19:14:57  soliday
 Standardized usage message.

 Revision 1.3  1999/05/25 19:04:18  soliday
 Removed compiler warning on linux.

 Revision 1.2  1995/09/06 14:55:56  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "match_string.h"

#define DEFAULT_HSIZE 484
#define DEFAULT_VSIZE 512

#define SET_DEFINITION 0
#define SET_SIZE 1
#define N_OPTIONS 2
 
char *option[N_OPTIONS] = {
    "definition", "size"
        };


char *USAGE = "raw2sdds <inputfile> <outputfile>\n\
 -definition=<name>,<definition-entries>\n\
 [-size=<horiz-pixels>,<vert-pixels>]\n\n\
raw2sdds converts a binary data stream to SDDS format.  The definition entries\
are of the form <keyword>=<value>, where the keyword is any valid field name for\
a SDDS column.\n\n\
Program by Michael Borland (This is Version 1, February 1994.)\n";

char *process_column_definition(char **argv, long argc);

int main(int argc, char **argv)
{
    SDDS_TABLE SDDS_table;
    SCANNED_ARG *scanned;
    long i_arg;
    char *input, *output, *definition;
    long hsize, vsize;
    char *data, *data_name;
    char ts1[100], ts2[100];
    FILE *fpi;

    argc = scanargs(&scanned, argc, argv);
    if (argc<4)
        bomb(NULL, USAGE);

    input = output = data_name = NULL;
    hsize = DEFAULT_HSIZE;
    vsize = DEFAULT_VSIZE;
    definition = NULL;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (scanned[i_arg].arg_type==OPTION) {
            /* process options here */
            switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_DEFINITION:
                data_name = scanned[i_arg].list[1];
                definition = process_column_definition(scanned[i_arg].list+1, 
                                                       scanned[i_arg].n_items-1);
                if (!strstr(definition, "type=character"))
                    SDDS_Bomb("data type must be character for now");
                break;
              case SET_SIZE:
                if (scanned[i_arg].n_items!=3 ||
                    sscanf(scanned[i_arg].list[1], "%ld", &hsize)!=1 || hsize<=0 ||
                    sscanf(scanned[i_arg].list[2], "%ld", &vsize)!=1 || vsize<=0)
                    bomb("invalid -size syntax", USAGE);
                break;
              default:
                bomb("invalid option seen", USAGE);
                break;
                }
            }
        else {
            if (!input)
                input = scanned[i_arg].list[0];
            else if (!output)
                output = scanned[i_arg].list[0];
            else
                bomb("too many filenames", USAGE);
            }
        }
    if (!input)
        SDDS_Bomb("input file not seen");
    if (!output)
        SDDS_Bomb("output file not seen");
    if (!definition)
        SDDS_Bomb("definition not seen");

    sprintf(ts1, "%ld", hsize);
    sprintf(ts2, "%ld", vsize);
    if (!SDDS_InitializeOutput(&SDDS_table, SDDS_BINARY, 0, 
                               "screen image from raw file", "screen image",
                               output) ||
        SDDS_ProcessColumnString(&SDDS_table, definition, 0)<0 || 
        SDDS_DefineParameter(&SDDS_table, "NumberOfRows", NULL, NULL, "number of rows",
                              NULL, SDDS_LONG, ts1)<0 ||
        !SDDS_DefineParameter(&SDDS_table, "NumberOfColumns", NULL, NULL, 
                              "number of columns", NULL, SDDS_LONG, ts2)<0 ||
        !SDDS_WriteLayout(&SDDS_table) || !SDDS_StartTable(&SDDS_table, hsize*vsize))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    data = tmalloc(sizeof(*data)*hsize*vsize);
    fpi = fopen_e(input, "r", 0);
    if (fread(data, sizeof(*data), hsize*vsize, fpi)!=hsize*vsize)
        SDDS_Bomb("unable to read (all) data from input file");
    fclose(fpi);
    if (!SDDS_SetColumn(&SDDS_table, SDDS_SET_BY_NAME, data, hsize*vsize, 
                        data_name) ||
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

