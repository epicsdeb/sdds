/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: TFS2sdds
 * purpose: convert MAD TFS data to SDDS
 *
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.5  2001/01/23 19:14:53  soliday
 Standardized usage message.

 Revision 1.4  1999/05/25 19:01:52  soliday
 Removed compiler warning on linux.

 Revision 1.3  1995/09/06 14:55:45  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"
#include "match_string.h"

#define SET_PIPE 0
#define N_OPTIONS 1

static char *option[N_OPTIONS] = {
    "pipe",
    };

char *USAGE = "TFS2sdds <inputfile> <outputfile>\n\
 [-pipe[=input][,output]]\n\n\
Converts LEP TFS format files (used by MAD) to SDDS.\n\n\
Program by Michael Borland. (This is version 1, July 1995).";

#define SHORT_TYPE 0
#define LONG_TYPE 1
#define DOUBLE_TYPE 2
#define FLOAT_TYPE 3
#define STRING_TYPE 4
#define TYPENAMES 5
static char *typeName[TYPENAMES] = {
    "short", "long", "double", "float", "string", 
    } ;

#define SDDS_MAXLINE 1024

long identifyType(char *format);

int main(int argc, char **argv)
{
    SCANNED_ARG *scanned;
    long i_arg, type, inHeader;
    char *input, *output;
    unsigned long pipeFlags;
    FILE *fpi, *fpo;
    char s1[SDDS_MAXLINE], s2[SDDS_MAXLINE];
    char *name, *format, *value;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&scanned, argc, argv);
    if (argc<2)
        bomb(NULL, USAGE);

    input = output = format = value = NULL;
    pipeFlags = 0;
    for (i_arg=1; i_arg<argc; i_arg++) {
        if (scanned[i_arg].arg_type==OPTION) {
            switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_PIPE:
                if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags)) 
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              default:
                SDDS_Bomb("unknown switch");
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

    processFilenames("TFS2sdds", &input, &output, pipeFlags, 0, NULL);

    if (!input)
        fpi = stdin;
    else 
        if (!(fpi = fopen(input, "r")))
            SDDS_Bomb("Unable to open input file");
    if (!output)
        fpo = stdout;
    else 
        if (!(fpo = fopen(output, "w")))
            SDDS_Bomb("Unable to open output file");

    fprintf(fpo, "SDDS1\n");
    if (!fgets(s1, SDDS_MAXLINE, fpi) || !fgets(s2, SDDS_MAXLINE, fpi)) 
        SDDS_Bomb("Input file ends prematurely");
    if (s1[0]!='*')
        SDDS_Bomb("Column name line not seen");
    if (s2[0]!='$')
        SDDS_Bomb("Column format line not seen");
    strcpy(s1, s1+1);
    strcpy(s2, s2+1);
    while ((name = get_token(s1))) {
        if (!(format = get_token(s2))) 
            SDDS_Bomb("Missing format for column");
        fprintf(fpo, "&column name=%s, type=", name);
        if ((type=identifyType(format))<0) {
            fprintf(stderr, "Error (TFS2sdds): unknown format string: %s\n", format);
            exit(1);
            }
        fprintf(fpo, "%s &end\n", typeName[type]);
        }

    inHeader = 1;
    while (fgets(s1, SDDS_MAXLINE, fpi)) {
        if (inHeader) {
            if (s1[0]=='@') {
                strcpy(s1, s1+1);
                if (!(name = get_token(s1)) || !(format = get_token(s1)) ||
                    !(value = get_token(s1)))
                    SDDS_Bomb("Missing data for parameter");
                if ((type=identifyType(format))<0) {
                    fprintf(stderr, "Error (TFS2sdds): unknown format string: %s\n", format);
                    exit(1);
                    }
                fprintf(fpo, "&parameter name = %s, type = %s, fixed_value = \"%s\" &end\n",
                        name, typeName[type], value);
                continue;
                }
            inHeader = 0;
            fputs("&data mode=ascii, no_row_counts=1 &end\n", fpo);
            }
        fputs(s1, fpo);
        }
    if (inHeader) 
        fputs("&data mode=ascii, no_row_counts=1 &end\n\n", fpo);
        
    return(0);
}

long identifyType(char *format)
{
    long length;
    length = strlen(format);
    if (!format || format[0]!='%')
        SDDS_Bomb("Bad format string seen");
    if (strcmp(format+length-2, "le")==0 || strcmp(format+length-2, "lf")==0)
        return DOUBLE_TYPE;
    if (strcmp(format+length-1, "e")==0 || strcmp(format+length-1, "f")==0)
        return FLOAT_TYPE;
    if (strcmp(format+length-2, "ld")==0)
        return LONG_TYPE;
    if (strcmp(format+length-2, "hd")==0)
        return SHORT_TYPE;
    if (format[length-1]=='d')
        return LONG_TYPE;
    if (format[length-1]=='s')
        return STRING_TYPE;
    return -1;
    }
