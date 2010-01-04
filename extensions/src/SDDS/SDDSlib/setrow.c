/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "mdb.h"
#include "SDDS.h"

main()
{
    SDDS_DATASET SDDS_dataset;
    long rows, i, names;
    char **name;
    void *ptr;
    char *quad_name[16] = {
        "P1Q1", "P1Q2", "P1Q3", "P1Q4",
        "P2Q1", "P2Q2", "P2Q3", "P2Q4",
        "P3Q1", "P3Q2", "P3Q3", "P3Q4",
        "P4Q1", "P4Q2", "P4Q3", "P4Q4"
            } ;
    char *column_name[3] = {"s", "betax", "betay"};
    double **beta;

    if (!SDDS_InitializeInput(&SDDS_dataset, "par.sdds") || !SDDS_ReadPage(&SDDS_dataset))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);

    SDDS_SetRowFlags(&SDDS_dataset, 1);
    if (!SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_MATCH_STRING, "P?[qQ][12]", SDDS_AND))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if ((rows=SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_MATCH_STRING, "P?[qQ][34]", SDDS_OR))<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!(name=(char**)SDDS_GetColumn(&SDDS_dataset, "ElementName")))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for SDDS_MATCH_STRING mode:\n", rows);
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s%s", name[i], (i+1)%8?"  ":"\n");
    fflush(stdout);

    SDDS_SetRowFlags(&SDDS_dataset, 0);
    if (SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAMES_STRING, "P1Q1 P1Q2 P1Q3 P1Q4")<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAMES_STRING, "P2Q1 P2Q2 P2Q3 P2Q4")<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAMES_STRING, "P3Q1 P3Q2 P3Q3 P3Q4")<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAMES_STRING, "P4Q1 P4Q2 P4Q3 P4Q4")<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!(name=(char**)SDDS_GetColumn(&SDDS_dataset, "ElementName")))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for SDDS_NAMES_STRING mode:\n", rows=SDDS_CountRowsOfInterest(&SDDS_dataset));
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s%s", name[i], (i+1)%8?"  ":"\n");
    fflush(stdout);

    SDDS_SetRowFlags(&SDDS_dataset, 0);
    if (SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAME_STRINGS, "P1Q1", "P1Q2", "P1Q3", "P1Q4", NULL)<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAME_STRINGS, "P2Q1", "P2Q2", "P2Q3", "P2Q4", NULL)<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAME_STRINGS, "P3Q1", "P3Q2", "P3Q3", "P3Q4", NULL)<0 ||
        SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAME_STRINGS, "P4Q1", "P4Q2", "P4Q3", "P4Q4", NULL)<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!(name=(char**)SDDS_GetColumn(&SDDS_dataset, "ElementName")))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for SDDS_NAME_STRINGS mode:\n", rows=SDDS_CountRowsOfInterest(&SDDS_dataset));
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s%s", name[i], (i+1)%8?"  ":"\n");
    fflush(stdout);

    SDDS_SetRowFlags(&SDDS_dataset, 0);
    if (SDDS_SetRowsOfInterest(&SDDS_dataset, "ElementName", SDDS_NAME_ARRAY, 16, quad_name)<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!(name=(char**)SDDS_GetColumn(&SDDS_dataset, "ElementName")))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for SDDS_NAME_ARRAY mode:\n", rows=SDDS_CountRowsOfInterest(&SDDS_dataset));
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s%s", name[i], (i+1)%8?"  ":"\n");
    fflush(stdout);

    SDDS_SetColumnFlags(&SDDS_dataset, 0);
    if (!SDDS_SetColumnsOfInterest(&SDDS_dataset, SDDS_NAME_ARRAY, 3, column_name) ||
        !(beta=SDDS_GetMatrixOfRows(&SDDS_dataset, &rows)))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for matrix of rows, %ld columns\n", rows, SDDS_CountColumnsOfInterest(&SDDS_dataset));
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s    %f    %f    %f\n", name[i], beta[i][0], beta[i][1], beta[i][2]);

    if (SDDS_SetRowsOfInterest(&SDDS_dataset, NULL, SDDS_MATCH_STRING, NULL, SDDS_NEGATE_PREVIOUS)<0 ||
        !(name=(char**)SDDS_GetColumn(&SDDS_dataset, "ElementName")))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    fprintf(stderr, "%ld rows for complement using SDDS_MATCH_STRING mode:\n", rows=SDDS_CountRowsOfInterest(&SDDS_dataset));
    for (i=0; i<rows; i++)
        fprintf(stderr, "%10s%s", name[i], (i+1)%8?"  ":"\n");
    fprintf(stderr, "\n");
    fflush(stdout);

    if ((names=SDDS_MatchColumns(&SDDS_dataset, &name, SDDS_NAME_ARRAY, FIND_ANY_TYPE,
                                3, column_name))<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    for (i=0; i<names; i++)
        fprintf(stderr, "%s  ", name[i]);
    fprintf(stderr, "\n");
    fflush(stdout);
    free(name);
    if ((names=SDDS_MatchColumns(&SDDS_dataset, &name, SDDS_NAMES_STRING, FIND_ANY_TYPE,
                                 "s,betax,betay"))<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    for (i=0; i<names; i++)
        fprintf(stderr, "%s  ", name[i]);
    fprintf(stderr, "\n");
    fflush(stdout);
    free(name);
    if ((names=SDDS_MatchColumns(&SDDS_dataset, &name, SDDS_NAME_STRINGS, FIND_ANY_TYPE,
                                 "s", "betax", "betay", NULL))<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    for (i=0; i<names; i++)
        fprintf(stderr, "%s  ", name[i]);
    fprintf(stderr, "\n");
    fflush(stdout);

    free(name);
    if ((names=SDDS_MatchColumns(&SDDS_dataset, NULL, SDDS_MATCH_STRING, FIND_ANY_TYPE,
                                 "beta[xy]", SDDS_0_PREVIOUS|SDDS_OR))<0 ||
        (names=SDDS_MatchColumns(&SDDS_dataset, &name, SDDS_MATCH_STRING, FIND_ANY_TYPE,
                                 "s", SDDS_OR))<0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    for (i=0; i<names; i++)
        fprintf(stderr, "%s  ", name[i]);
    fprintf(stderr, "\n");
    fflush(stdout);
    free(name);
    }

        
