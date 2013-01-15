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
 Revision 1.3  1995/12/12 04:26:45  borland
 Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 to files with no_row_counts=1.

 * Revision 1.2  1995/09/05  21:11:28  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "SDDS.h"

main()
{
    SDDS_ARRAY *da1, *da2, *da3, *da4, *sa1, *sa2, *sa3, *sa4;
    static double data[32] = {
        111, 112, 113, 114,
        121, 122, 123, 124,
        211, 212, 213, 214,
        221, 222, 223, 224,
        311, 312, 313, 314,
        321, 322, 323, 324,
        411, 412, 413, 414,
        421, 422, 423, 424,
        };
    static char *sdata[32] = {
        "1 1 1", "1 1 2", "1 1 3", "1 1 4",
        "1 2 1", "1 2 2", "1 2 3", "1 2 4",
        "2 1 1", "2 1 2", "2 1 3", "2 1 4",
        "2 2 1", "2 2 2", "2 2 3", "2 2 4",
        "3 1 1", "3 1 2", "3 1 3", "3 1 4",
        "3 2 1", "3 2 2", "3 2 3", "3 2 4",
        "4 1 1", "4 1 2", "4 1 3", "4 1 4",
        "\"4 !\002 2 1\"", "\"4 !\003 2 2\"", "\"4 !\004 2 3\"", "\"4 !\005 2 4\"",
        };
    long d4[4] = {2, 2, 4, 2};
    long d3[3] = {2, 4, 4};
    long d2[2] = {4, 8};
    long d1[1] = {32};
    double ****p4, ***p3, **p2, *p1;
    long i, j, k, l, index;
    char *****sp4, ****sp3, ***sp2, **sp1;
    SDDS_DATASET SDDS_dataset;

    /* make pointer arrays that index the data--note that the data
     * is NOT copied here.  All I'm doing is setting up multi-level
     * pointer arrays into the existing data.
     */
    if (!(p4=SDDS_MakePointerArray(data, SDDS_DOUBLE, 4, d4)) ||
        !(p3=SDDS_MakePointerArray(data, SDDS_DOUBLE, 3, d3)) ||
        !(p2=SDDS_MakePointerArray(data, SDDS_DOUBLE, 2, d2)) ||
        !(p1=SDDS_MakePointerArray(data, SDDS_DOUBLE, 1, d1))) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    if (!(sp4=SDDS_MakePointerArray(sdata, SDDS_STRING, 4, d4)) ||
        !(sp3=SDDS_MakePointerArray(sdata, SDDS_STRING, 3, d3)) ||
        !(sp2=SDDS_MakePointerArray(sdata, SDDS_STRING, 2, d2)) ||
        !(sp1=SDDS_MakePointerArray(sdata, SDDS_STRING, 1, d1))) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }

    /* set up to put data into file "atest.out" */
    if (!SDDS_InitializeOutput(&SDDS_dataset, SDDS_ASCII, 1, NULL, NULL, "atest.out")) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }

    /* define double arrays */
    if (SDDS_DefineArray(&SDDS_dataset, "p1", NULL, NULL, NULL, "%.1f", SDDS_DOUBLE, 0, 1, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "p2", NULL, NULL, NULL, "%.1f", SDDS_DOUBLE, 0, 2, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "p3", NULL, NULL, NULL, "%.1f", SDDS_DOUBLE, 0, 3, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "p4", NULL, NULL, NULL, "%.1f", SDDS_DOUBLE, 0, 4, NULL)<0) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    /* define string arrays */
    if (SDDS_DefineArray(&SDDS_dataset, "sp1", NULL, NULL, NULL, NULL, SDDS_STRING, 0, 1, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "sp2", NULL, NULL, NULL, NULL, SDDS_STRING, 0, 2, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "sp3", NULL, NULL, NULL, NULL, SDDS_STRING, 0, 3, NULL)<0 ||
        SDDS_DefineArray(&SDDS_dataset, "sp4", NULL, NULL, NULL, NULL, SDDS_STRING, 0, 4, NULL)<0) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    /* write the layout and give SDDS the data for the first page */
    if (!SDDS_SetNoRowCounts(&SDDS_dataset, 1)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }

    if (!SDDS_WriteLayout(&SDDS_dataset) || !SDDS_StartPage(&SDDS_dataset, 0) ||
        !SDDS_SetArrayVararg(&SDDS_dataset, "p1", SDDS_CONTIGUOUS_DATA, p1, 32) ||
        !SDDS_SetArray(&SDDS_dataset, "p2", SDDS_POINTER_ARRAY, p2, d2) ||
        !SDDS_SetArray(&SDDS_dataset, "p3", SDDS_CONTIGUOUS_DATA, p1, d3) ||
        !SDDS_SetArray(&SDDS_dataset, "p4", SDDS_POINTER_ARRAY, p4, d4) ||
        !SDDS_SetArray(&SDDS_dataset, "sp1", SDDS_POINTER_ARRAY, sp1, d1) ||
        !SDDS_SetArray(&SDDS_dataset, "sp2", SDDS_POINTER_ARRAY, sp2, d2) ||
        !SDDS_SetArrayVararg(&SDDS_dataset, "sp3", SDDS_POINTER_ARRAY, sp3, d3[0], d3[1], d3[2]) ||
        !SDDS_SetArray(&SDDS_dataset, "sp4", SDDS_CONTIGUOUS_DATA, sp1, d4) ) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    /* write out the data */ 
    if (!SDDS_WritePage(&SDDS_dataset) || !SDDS_Terminate(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }

    if (!SDDS_InitializeInput(&SDDS_dataset, "atest.out") || SDDS_ReadPage(&SDDS_dataset)<1) {
        SDDS_SetError("File atest.out nonexistent or empty");
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }

   /* get the data back from SDDS */
    if (!(da1=SDDS_GetArray(&SDDS_dataset, "p1", NULL)) ||
        !(da2=SDDS_GetArray(&SDDS_dataset, "p2", NULL)) ||
        !(da3=SDDS_GetArray(&SDDS_dataset, "p3", NULL)) ||
        !(da4=SDDS_GetArray(&SDDS_dataset, "p4", NULL)) ||
        !(sa1=SDDS_GetArray(&SDDS_dataset, "sp1", NULL)) ||
        !(sa2=SDDS_GetArray(&SDDS_dataset, "sp2", NULL)) ||
        !(sa3=SDDS_GetArray(&SDDS_dataset, "sp3", NULL)) ||
        !(sa4=SDDS_GetArray(&SDDS_dataset, "sp4", NULL))) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    for (i=index=0; i<32; i++, index++) {
        if (p1[i]!=((double*)da1->pointer)[i] || p1[i]!=((double*)da1->data)[index]) {
            fprintf(stderr, "mismatch for p1 and da1, i=%ld\n", i);
            exit(1);
            }
        if (strcmp(sp1[i], ((char**)sa1->pointer)[i])!=0 || strcmp(sp1[i], ((char**)sa1->data)[index])!=0) {
            fprintf(stderr, "mismatch for sp1 and sa1, i=%ld\n", i);
            exit(1);
            }
        }
    for (i=index=0; i<4; i++)
        for (j=0; j<8; j++, index++) {
            if (p2[i][j]!=((double**)da2->pointer)[i][j] || p2[i][j]!=((double*)da2->data)[index]) {
                fprintf(stderr, "mismatch for p2 and da2, i=%ld, j=%ld\n", i, j);
                exit(1);
                }
            if (strcmp(sp2[i][j], ((char***)sa2->pointer)[i][j])!=0 ||
                strcmp(sp2[i][j], ((char**)sa2->data)[index])!=0) {
                fprintf(stderr, "mismatch for sp2 and sa2, i=%ld, j=%ld\n", i, j);
                exit(1);
                }
            }
    for (i=index=0; i<2; i++)
        for (j=0; j<4; j++)
            for (k=0; k<4; k++, index++) {
                if (p3[i][j][k]!=((double***)da3->pointer)[i][j][k] ||
                    p3[i][j][k]!=((double*)da3->data)[index]) {
                    fprintf(stderr, "mismatch for p3 and da3, i=%ld, j=%ld, k=%ld\n", i, j, k);
                    exit(1);
                    }
                if (strcmp(sp3[i][j][k], ((char****)sa3->pointer)[i][j][k])!=0  ||
                    strcmp(sp3[i][j][k], ((char**)sa3->data)[index])!=0) {
                    fprintf(stderr, "mismatch for sp3 and sa3, i=%ld, j=%ld, k=%ld\n", i, j, k);
                    exit(1);
                    }
                }

    for (i=index=0; i<2; i++)
        for (j=0; j<2; j++)
            for (k=0; k<4; k++)
                for (l=0; l<2; l++, index++) {
                    if (p4[i][j][k][l]!=((double****)da4->pointer)[i][j][k][l] ||
                        p4[i][j][k][l]!=((double*)da4->data)[index]) {
                        fprintf(stderr, "mismatch for p4 and da4, i=%ld, j=%ld, k=%ld, l=%ld\n", i, j, k, l);
                        exit(1);
                        }
                    if (strcmp(sp4[i][j][k][l], ((char*****)sa4->pointer)[i][j][k][l])!=0 ||
                        strcmp(sp4[i][j][k][l], ((char**)sa4->data)[index])!=0) {
                        fprintf(stderr, "mismatch for p4 and da4, i=%ld, j=%ld, k=%ld, l=%ld\n", i, j, k, l);
                        exit(1);
                        }
                    }

     if (!SDDS_Terminate(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, 1);
        exit(1);
        }
    fprintf(stderr, "No errors detected\n");
    }
