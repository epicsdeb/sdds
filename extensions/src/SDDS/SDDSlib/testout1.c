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
 Revision 1.3  1995/12/12 04:26:46  borland
 Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 to files with no_row_counts=1.

 * Revision 1.2  1995/09/05  21:11:33  saunders
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

    SDDS_InitializeOutput(&SDDS_dataset, SDDS_ASCII, 1, "Test header from testout.c", NULL, "testout.sdds");
    SDDS_SetNoRowCounts(&SDDS_dataset, 1);
    puts("output initialized"); fflush(stdout);
    if (SDDS_DefineColumn(&SDDS_dataset, "number", NULL, NULL, "element number", "%4ld", SDDS_LONG, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "s", "s", "m", "arc length tranversed", "%15.8e", SDDS_DOUBLE, 15)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "element", NULL, NULL, "element name", "%15s", SDDS_STRING, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "charval", NULL, NULL, "character value", NULL, SDDS_CHARACTER, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "x", "x", "m", "horizontal position", "%15.8e", SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "xp", "x'", "", "horizontal slope", "%15.8e", SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "y", "y", "m", "vertical position", "%15.8e", SDDS_DOUBLE, 0)<0 ||
        SDDS_DefineColumn(&SDDS_dataset, "yp", "y'", "", "vertical slope", "%15.8e", SDDS_DOUBLE, 0)<0 
        ) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    puts("columns defined"); fflush(stdout);
    if (
        SDDS_DefineParameter(&SDDS_dataset, "adouble", "AD", "dus", "a double precision value",
                             NULL, SDDS_DOUBLE, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "afloat", "AF", "fus", "a float precision value",
                             NULL, SDDS_FLOAT, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "along", "AL", "lus", "a long integer value",
                             NULL, SDDS_LONG, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "ashort", "AS", "sus", "a short integer value",
                             NULL, SDDS_SHORT, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "achar", "AC", "cus", "a character value",
                             NULL, SDDS_CHARACTER, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "astring", "ACS", "sus", "a string value",
                             NULL, SDDS_STRING, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "string1", "ACS", "sus", "another string value",
                             NULL, SDDS_STRING, NULL)<0 ||
        SDDS_DefineParameter(&SDDS_dataset, "string2", "ACS", "sus", "a fixed string value",
                             NULL, SDDS_STRING, "fixed value of string2")<0 )   {
        SDDS_PrintErrors(stderr, 0);
        exit(1);
        }
    puts("parameters defined"); fflush(stdout);
    if (SDDS_DefineAssociate(&SDDS_dataset, "relatedData", "testout1.sdds", NULL, "some related data", "unknown", 1)<0 ||
        SDDS_DefineAssociate(&SDDS_dataset, "moreRelatedData", "testout2.sdds", NULL, "more related data", "unknown", 0)<0 ) {
        SDDS_PrintErrors(stderr, 0);
        exit(1);
        }
    puts("associates defined"); fflush(stdout);
    if (!SDDS_WriteLayout(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, 0);
        exit(1);
        }
    puts("layout writen"); fflush(stdout);

    if (!SDDS_StartPage(&SDDS_dataset, 50)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "adouble", 450.0, "afloat", 450.1, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "along", 1234567891, "ashort", 12345, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "achar", '\002', "astring", "\002\003this is a string", 
                            "string1", "\"A string with!\" quotation marks, backslashes \"\\\", and exclamation points!", NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    for (i=0; i<50; i++) {
        if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "number", i,
                       "x", (double)(1.0*i+0.1), "xp", (double)(1.0*i+0.2), 
                       "y", (double)(1.0*i+0.3), "yp", (double)(1.0*i+0.4), 
                       "s", (double)(1.0*i), NULL) ||
            !SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "element", 
                               sprintf(buffer, "element %ld", i), "charval", (char)(i&127), NULL) ) {
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

    if (!SDDS_StartPage(&SDDS_dataset, 50)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "adouble", 450.0, "afloat", 450.1, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "along", 1234567891, "ashort", 12345, NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, "achar", '\041', "astring", "this is a string", 
                            "string1", "\"A string with!\" quotation marks and exclamation points\\!", NULL)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    for (i=0; i<50; i++) {
        if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "number", (i+50),
                       "x", (double)(1.0*(i+50)+0.1), "xp", (double)(1.0*(i+50)+0.2), 
                       "y", (double)(1.0*(i+50)+0.3), "yp", (double)(1.0*(i+50)+0.4), 
                       "s", (double)(1.0*(i+50)), NULL) ||
            !SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i, "element", 
                               sprintf(buffer, "\"element\" %ld %c", (i+50), i&127), 
                               "charval", (char)(i&127), NULL) ) {
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
    }

