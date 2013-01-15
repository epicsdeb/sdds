/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: lba2sdds.c
 * purpose: converts a LBA file from the spiricon
 * laser-beam analyser to SDDS format
 *
 * M. Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.5  2001/01/23 19:14:56  soliday
 Standardized usage message.

 Revision 1.4  1999/05/25 19:03:14  soliday
 Removed compiler warning on linux.

 Revision 1.3  1995/11/13 15:54:23  borland
 Removed non-ANSI use of char * return value from sprintf().

 * Revision 1.2  1995/09/06  14:55:51  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "match_string.h"

#define SET_DEFINITION 0
#define SET_PIPE 1
#define N_OPTIONS 2
 
char *option[N_OPTIONS] = {
    "definition", "pipe",
        };


char *USAGE = "lba2sdds [<inputfile>] [<outputfile>]\n\
 [-pipe[=input][,output]]\n\
 -definition=<name>,<definition-entries>\n\n\
lba2sdds converts a Spiricon LBA file to SDDS format.  The definition entries\
are of the form <keyword>=<value>, where the keyword is any valid field name for\
a SDDS column.\n\n\
Program by Michael Borland (This is Version 2, May 1995.)\n";

char *process_column_definition(char **argv, long argc);

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset;
    SCANNED_ARG *scanned;
    long i_arg;
    char *input, *output, *definition;
    long hsize, vsize;
    char *data, *data_name;
    char header[200];
    char ts1[100], ts2[100];
    FILE *fpi;
    unsigned long pipeFlags;

    SDDS_RegisterProgramName(argv[0]);

    argc = scanargs(&scanned, argc, argv);
    if (argc<4)
        bomb(NULL, USAGE);

    input = output = data_name = NULL;
    definition = NULL;
    pipeFlags = 0;
    hsize = vsize = 0;
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
              case SET_PIPE:
                if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
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

    processFilenames("lba2sdds", &input, &output, pipeFlags, 0, NULL);

    if (!definition)
        SDDS_Bomb("definition not seen");

    if (input)
        fpi = fopen_e(input, "r", 0);
    else
        fpi = stdin;

    if (fread(header, sizeof(*header), 200, fpi)!=200) 
        SDDS_Bomb("unable to read LBA file header");
    switch (header[0]) {
      case 'A':
        hsize = vsize = 120;
        break;
      case 'B':
        vsize = 256;
        hsize = 240;
        break;
      case 'C':
        vsize = 512;
        hsize = 480;
        break;
      default:
        SDDS_Bomb("data does not appear to be in LBA format--invalid frame type");
        break;
        }

    sprintf(ts1, "%ld", hsize);
    sprintf(ts2, "%ld", vsize);
    if (!SDDS_InitializeOutput(&SDDS_dataset, SDDS_BINARY, 0, 
                               "screen image from LBA file", "screen image",
                               output) ||
        SDDS_ProcessColumnString(&SDDS_dataset, definition, 0)<0 || 
        SDDS_DefineParameter(&SDDS_dataset, "NumberOfRows", NULL, NULL, "number of rows",
                             NULL, SDDS_LONG, ts1)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "NumberOfColumns", NULL, NULL, 
                              "number of columns", NULL, SDDS_LONG,
                              ts2)<0 ||
        !SDDS_WriteLayout(&SDDS_dataset))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    data = tmalloc(sizeof(*data)*hsize*vsize);
    do {
        if (fread(data, sizeof(*data), hsize*vsize, fpi)!=hsize*vsize)
            SDDS_Bomb("unable to read (all) data from input file");
        if (!SDDS_StartPage(&SDDS_dataset, hsize*vsize) ||
            !SDDS_SetColumn(&SDDS_dataset, SDDS_SET_BY_NAME, data, hsize*vsize, 
                            data_name) ||
            !SDDS_WritePage(&SDDS_dataset))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        } while (fread(header, sizeof(*header), 200, fpi)==200) ;
    fclose(fpi);
    if (!SDDS_Terminate(&SDDS_dataset))
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

