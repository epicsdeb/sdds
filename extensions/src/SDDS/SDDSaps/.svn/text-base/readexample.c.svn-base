/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: readexample.c
 * purpose: simple example of reading an SDDS file
 *
 * Michael Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.3  1995/09/06 14:55:58  saunders
 First test release of SDDS1.5

 */
#include "SDDS.h"

char *USAGE = "readexample <SDDSinput>\n\
This program is a simple example that illustrates the use of\n\
SDDS input routines.\n\
Program by Michael Borland. (This is version 1, August 1994.)\n";

#define COLUMNS 2

main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset;
    char *input;
    char *column_name[COLUMNS] = {"f", "FFT"};
    char *column_units[COLUMNS] = {"MHz", ""};
    double *f;
    float *FFT;
    long i, rows, *fl;

    if (argc!=2) {
        fprintf(stderr, "error: no input file given\nusage: %s\n", USAGE);
        exit(1);
        }

    input = argv[1];

    if (!SDDS_InitializeInput(&SDDS_dataset, input)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    for (i=0; i<COLUMNS; i++) {
        long code;
        if ((code=SDDS_CheckColumn(&SDDS_dataset, column_name[i], column_units[i], 0, NULL))==SDDS_CHECK_NONEXISTENT) {
            fprintf(stderr, "column %s not in file\n", column_name[i]);
            exit(1);
            }
        if (code==SDDS_CHECK_WRONGUNITS) {
            fprintf(stderr, "column %s doesn't have expected units (%s)\n", column_name[i], column_units[i]);
            exit(1);
            }
        }

    if (SDDS_ReadPage(&SDDS_dataset)!=1 || (rows=SDDS_CountRowsOfInterest(&SDDS_dataset))<0) {
        fprintf(stderr, "first page of data set is unreadable or empty\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!(f=SDDS_GetColumnInDoubles(&SDDS_dataset, column_name[0]))) {
        fprintf(stderr, "unable to read column %s\n", column_name[0]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!(fl=SDDS_GetNumericColumn(&SDDS_dataset, column_name[0], SDDS_LONG))) {
        fprintf(stderr, "unable to read column %s\n", column_name[0]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!(FFT=SDDS_GetNumericColumn(&SDDS_dataset, column_name[1], SDDS_FLOAT))) {
        fprintf(stderr, "unable to read column %s\n", column_name[1]);
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
        
    for (i=0; i<rows; i++)
        fprintf(stderr, "row %ld:  %ld, %e, %e\n", i, fl[i], f[i], FFT[i]);
        
    SDDS_Terminate(&SDDS_dataset);
    }
