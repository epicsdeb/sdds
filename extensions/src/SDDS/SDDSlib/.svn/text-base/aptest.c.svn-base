/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: not supported by cvs2svn $
 Revision 1.5  1995/12/12 21:05:18  borland
 Removed call to SDDS_SetNoRowCounts().

 * Revision 1.4  1995/12/12  04:26:43  borland
 * Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 * to files with no_row_counts=1.
 *
 * Revision 1.3  1995/11/13  16:05:39  borland
 * Revised prototype for  SDDS_UpdateBinaryPage to include mode argument.
 *
 * Revision 1.2  1995/09/05  21:11:26  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "SDDS.h"

main()
{
    SDDS_DATASET SDDS_dataset;
    long i;
    char buffer[100];

    if (!SDDS_InitializeAppend(&SDDS_dataset, "testout.sdds"))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    puts("output initialized"); fflush(stdout);
    if (!SDDS_StartPage(&SDDS_dataset, 50)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "adouble", 4500.0, "afloat", 4500.1, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "along", 123456781, "ashort", 1234, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "achar", 'd', "astring", "this is a string1", 
                            "string1", "\"A string with!\" quotation marks and exclamation points\\!1", NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    for (i=0; i<50; i++) {
        if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "number", i,
                       "x", (double)(2.0*i+0.1), "xp", (double)(2.0*i+0.2), 
                       "y", (double)(2.0*i+0.3), "yp", (double)(2.0*i+0.4), 
                       "s", (double)(2.0*i), NULL) ||
            !SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "element", sprintf(buffer, "element %ld", i), NULL) ) {
            SDDS_PrintErrors(stderr, 1);
            exit(1);
            }
        }
    puts("data filled in"); fflush(stdout);

    if (!SDDS_WritePage(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    puts("page written out"); fflush(stdout);
    printf("n_row_written = %ld\n", SDDS_dataset.n_rows_written);

    if (!SDDS_LengthenTable(&SDDS_dataset, 50)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    for (i=50; i<100; i++) {
        if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "number", i,
                       "x", (double)(2.0*i+0.1), "xp", (double)(2.0*i+0.2), 
                       "y", (double)(2.0*i+0.3), "yp", (double)(2.0*i+0.4), 
                       "s", (double)(2.0*i), NULL) ||
            !SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "element", sprintf(buffer, "\"element\" %ld", i), NULL) ) {
            SDDS_PrintErrors(stderr, 1);
            exit(1);
            }
        }
    puts("data filled in"); fflush(stdout);
    if (!SDDS_UpdatePage(&SDDS_dataset, 0)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    puts("page written out"); fflush(stdout);
    }

