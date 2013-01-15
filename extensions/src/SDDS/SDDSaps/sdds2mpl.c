/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sdds2mpl.c
 * purpose: extract data from SDDS format and create mpl files
 * Michael Borland, 1993
 $Log: not supported by cvs2svn $
 Revision 1.9  2002/08/14 17:12:39  soliday
 Added Open License

 Revision 1.8  2001/01/23 19:47:26  soliday
 Fixed Solaris compiler warnings.

 Revision 1.7  2001/01/10 19:35:30  soliday
 Standardized usage message.

 Revision 1.6  1999/05/25 19:04:45  soliday
 Removed compiler warning on linux.

 Revision 1.5  1999/01/06 19:54:36  borland
 Fixed the version number in the usage message.

 * Revision 1.4  1996/07/05  16:33:11  borland
 * Added 0 for new mode argument of SDDS_PrintTypedValue and/or
 * SDDS_SprintTypedValue.
 *
 * Revision 1.3  1995/09/06  14:56:02  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "table.h"
#include "scan.h"
#include "SDDS.h"
#include "SDDSaps.h"

#define SET_ROOTNAME 0
#define SET_OUTPUT 1
#define SET_SEPARATE_PAGES 2
#define SET_LABEL_PARAMETERS 3
#define SET_ANNOUNCE_OPENINGS 4
#define SET_PIPE 5
#define N_OPTIONS 6

char *option[N_OPTIONS] = {
    "rootname", "output", "separatepages", "labelparameters", "announceopenings", "pipe",
    } ;

char *USAGE="sdds2mpl [-pipe[=input]] [<SDDSfilename>] [-rootname=<string>]\n\
 -output={column|parameter},<x-name>,<y-name>[,{<sy-name>|<sx-name>,<sy-name>}]... \n\
 [-labelParameters=<name>[=<format>]...]  [-separatePages] [-announceOpenings]\n\n\
Any number of -output specifications may be given.\n\n\
sdds2mpl extracts data from an SDDS file into mpl-format files.\n\
Program by Michael Borland.  (This is version 6, July 1996.)\n";


int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset;
    LABEL_PARAMETER *label_parameter;
    long label_parameters;
    COLUMN_DEFINITION **coldef;
    PARAMETER_DEFINITION **pardef;

    char *inputfile;
    long outputs;
    OUTPUT_REQUEST **output;
    char *rootname, filename[200];

    long n_rows_orig, n_rows, i, j, i_arg, announce_openings;
    long page_number, separate_pages;
    SCANNED_ARG *s_arg;
    char *ptr;
    void *data[4];
    double param[4];

    long data_present;
    unsigned long pipeFlags;

    argc = scanargs(&s_arg, argc, argv);
    if (argc<3) {
        fputs(USAGE, stderr);
        exit(1);
        }

    outputs = separate_pages = 0;
    inputfile = NULL;
    rootname = NULL;
    output = NULL;
    label_parameter = NULL;
    label_parameters = 0;
    announce_openings = 0;
    pipeFlags = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            delete_chars(s_arg[i_arg].list[0], "_");
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_ROOTNAME:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -rootname syntax");
                rootname = s_arg[i_arg].list[1];
                break;
              case SET_OUTPUT:
                if (s_arg[i_arg].n_items<4 || s_arg[i_arg].n_items>6)
                    SDDS_Bomb("invalid -output syntax");
                output = trealloc(output, sizeof(*output)*(outputs+1));
                if (!(output[outputs] =
                      process_output_request(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, outputs?output[outputs-1]:NULL)))
                    SDDS_Bomb("invalid -output syntax");
                outputs++;
                break;
              case SET_SEPARATE_PAGES:
                separate_pages = 1;
                break;
              case SET_LABEL_PARAMETERS:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -labelparameters syntax");
                label_parameter = trealloc(label_parameter, sizeof(*label_parameter)*(label_parameters+s_arg[i_arg].n_items));
                for (i=1; i<s_arg[i_arg].n_items; i++)
                    scan_label_parameter(label_parameter+i-1, s_arg[i_arg].list[i]);
                label_parameters += s_arg[i_arg].n_items-1;
                break;
              case SET_ANNOUNCE_OPENINGS:
                announce_openings = 1;
                break;
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              default:
                SDDS_Bomb("unknown switch");
                break;
                }
            }
        else {
            if (inputfile==NULL)
                inputfile = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }

    if (!inputfile && !(pipeFlags&USE_STDIN))
        SDDS_Bomb("no input source given");

    if (!rootname) {
        if (!inputfile)
            SDDS_Bomb("you must give a rootname if you don't give an input filename");
        SDDS_CopyString(&rootname, inputfile);
        if ((ptr=strrchr(rootname, '.')))
            *ptr = 0;
        }
    if (outputs<=0)
        SDDS_Bomb("no output specifications given");

    if (!SDDS_InitializeInput(&SDDS_dataset, inputfile)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    for (i=0; i<outputs; i++) {
        if (!output[i]->parameter_output)  {
            if (SDDS_GetColumnIndex(&SDDS_dataset, output[i]->item[0])<0 ||
                SDDS_GetColumnIndex(&SDDS_dataset, output[i]->item[1])<0 ||
                (output[i]->item[2] && SDDS_GetColumnIndex(&SDDS_dataset, output[i]->item[2])<0) ||
                (output[i]->item[3] && SDDS_GetColumnIndex(&SDDS_dataset, output[i]->item[3])<0) ) {
                fprintf(stderr, "error: unrecognized column name given\n");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                exit(1);
                }
            }
        else {
            if (SDDS_GetParameterIndex(&SDDS_dataset, output[i]->item[0])<0 ||
                SDDS_GetParameterIndex(&SDDS_dataset, output[i]->item[1])<0 ||
                (output[i]->item[2] && SDDS_GetParameterIndex(&SDDS_dataset, output[i]->item[2])<0) ||
                (output[i]->item[3] && SDDS_GetParameterIndex(&SDDS_dataset, output[i]->item[3])<0) ) {
                fprintf(stderr, "error: unrecognized parameter name given\n");
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                exit(1);
                }
            }
                
        }

    data_present = 0;
    while ((page_number=SDDS_ReadPage(&SDDS_dataset))>0) {
        data_present = 1;
        n_rows_orig=SDDS_CountRowsOfInterest(&SDDS_dataset);
        if (!SDDS_SetRowFlags(&SDDS_dataset, 1)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
        if ((n_rows=SDDS_CountRowsOfInterest(&SDDS_dataset))<=0) {
            fprintf(stderr, "warning: no rows selected for page %" PRId32 "\n", SDDS_dataset.page_number);
            if (SDDS_NumberOfErrors()) 
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            }
        for (i=0; i<outputs; i++) {
            if (!output[i]->fp) {
                if (separate_pages && !output[i]->parameter_output)
                    sprintf(filename, "%s_%03ld_%s_%s.out", rootname, output[i]->counter++, output[i]->item[0], 
                            output[i]->item[1]);
                else
                    sprintf(filename, "%s_%s_%s.out", rootname, output[i]->item[0], output[i]->item[1]);
                set_up_output(filename, output[i], label_parameter, label_parameters, separate_pages, announce_openings,
                              &SDDS_dataset);
                }
            if (!output[i]->parameter_output) {
                coldef = (COLUMN_DEFINITION**)output[i]->definitions;
                data[2] = data[3] = NULL;
                if (!(data[0]=SDDS_GetColumn(&SDDS_dataset, output[i]->item[0])) ||
                    !(data[1]=SDDS_GetColumn(&SDDS_dataset, output[i]->item[1])) ||
                    (output[i]->columns>2 && !(data[2]=SDDS_GetColumn(&SDDS_dataset, output[i]->item[2]))) ||
                    (output[i]->columns>3 && !(data[3]=SDDS_GetColumn(&SDDS_dataset, output[i]->item[3]))) ) {
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                    exit(1);
                    }
                if (output[i]->columns==2)
                    for (j=0; j<n_rows; j++) {
                        SDDS_PrintTypedValue((char*)data[0], j, coldef[0]->type, coldef[0]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[1], j, coldef[1]->type, coldef[1]->format_string, output[i]->fp, 0);
                        fputc('\n', output[i]->fp);
                        }
                else if (output[i]->columns==3) 
                    for (j=0; j<n_rows; j++) {
                        SDDS_PrintTypedValue((char*)data[0], j, coldef[0]->type, coldef[0]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[1], j, coldef[1]->type, coldef[1]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[2], j, coldef[2]->type, coldef[2]->format_string, output[i]->fp, 0);
                        fputc('\n', output[i]->fp);
                        }
                else 
                    for (j=0; j<n_rows; j++) {
                        SDDS_PrintTypedValue((char*)data[0], j, coldef[0]->type, coldef[0]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[1], j, coldef[1]->type, coldef[1]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[2], j, coldef[2]->type, coldef[2]->format_string, output[i]->fp, 0);
                        fputc(' ', output[i]->fp);
                        SDDS_PrintTypedValue((char*)data[3], j, coldef[3]->type, coldef[3]->format_string, output[i]->fp, 0);
                        fputc('\n', output[i]->fp);
                        }
                output[i]->points += n_rows;
                free(data[0]);
                free(data[1]);
                free(data[2]);
                free(data[3]);
                }
            else {
                pardef = (PARAMETER_DEFINITION**)output[i]->definitions;
                if (!SDDS_GetParameter(&SDDS_dataset, output[i]->item[0], &param[0]) ||
                    !SDDS_GetParameter(&SDDS_dataset, output[i]->item[1], &param[1]) ||
                    (output[i]->columns>2 && !SDDS_GetParameter(&SDDS_dataset, output[i]->item[2], &param[2])) ||
                    (output[i]->columns>3 && !SDDS_GetParameter(&SDDS_dataset, output[i]->item[3], &param[3])) ) {
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                    exit(1);
                    }
                output[i]->points += 1;
                SDDS_PrintTypedValue((char*)&param[0], 0, pardef[0]->type, pardef[0]->format_string, output[i]->fp, 0);
                fputc(' ', output[i]->fp);
                SDDS_PrintTypedValue((char*)&param[1], 0, pardef[1]->type, pardef[1]->format_string, output[i]->fp, 0);
                if (output[i]->columns==2) {
                    fputc('\n', output[i]->fp);
                    continue;
                    }
                fputc(' ', output[i]->fp);
                SDDS_PrintTypedValue((char*)&param[2], 0, pardef[2]->type, pardef[2]->format_string, output[i]->fp, 0);
                if (output[i]->columns==3) {
                    fputc('\n', output[i]->fp);
                    continue;
                    }
                fputc(' ', output[i]->fp);
                SDDS_PrintTypedValue((char*)&param[3], 0, pardef[3]->type, pardef[3]->format_string, output[i]->fp, 0);
                fputc('\n', output[i]->fp);
                }
            if (separate_pages && !output[i]->parameter_output) {
                fclose(output[i]->fp);
                output[i]->fp = NULL;
                output[i]->points = 0;
                }
            }
        }
        
    if (page_number==0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    if (page_number==-1 && !data_present) {
        if (SDDS_NumberOfErrors()) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
        fprintf(stderr, "error: input data file is empty\n");
        exit(1);
        }

    for (i=0; i<outputs; i++)
        if (output[i]->fp)
            fclose(output[i]->fp);

    for (i=0; i<outputs; i++) {
        if (!separate_pages || output[i]->parameter_output) {
            sprintf(filename, "%s_%s_%s.out", rootname, output[i]->item[0], output[i]->item[1]);
            fixcount(filename, output[i]->points);
            }
        }
    return(0);
}
