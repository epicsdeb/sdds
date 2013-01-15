/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsmselect
 * purpose: creates a SDDS data set from another data set based
 *          on matching to data in a third data set.
 *
 * For example, one can select all of the rows from file1 that
 * have a match in file2, or that have no match in file2.
 * Similar to sddsxref, but doesn't transfer any data.
 *
 * Michael Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.9  2001/05/03 20:55:33  soliday
 Standardized usage messages.

 Revision 1.8  2001/01/23 19:47:28  soliday
 Fixed Solaris compiler warnings.

 Revision 1.7  1999/09/28 15:38:51  soliday
 Added SDDS_Terminate at the end.

 Revision 1.6  1999/05/25 19:12:05  soliday
 Removed compiler warning on linux.

 Revision 1.5  1999/01/06 19:54:48  borland
 Fixed the version number in the usage message.

 * Revision 1.4  1996/03/11  02:46:08  borland
 * Handles empty pages in input2 properly.  If invert is on, copies remainder
 * of input1.  Otherwise, deletes remainder of input1.
 *
 * Revision 1.3  1995/09/06  14:56:47  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
/*#include "SDDSaps.h"*/
#include "scan.h"

#define SET_MATCH_COLUMNS 0
#define SET_EQUATE_COLUMNS 1
#define SET_NOWARNINGS 2
#define SET_INVERT 3
#define SET_REUSE 4
#define SET_PIPE 5
#define N_OPTIONS 6

typedef char *STRING_PAIR[2];

long rows_equate(SDDS_DATASET *SDDS1, long row1, SDDS_DATASET *SDDS2, long row2, 
                 long equate_columns, STRING_PAIR *equate_column);

char *option[N_OPTIONS] = {
    "match", "equate", "nowarnings", "invert", "reuse", "pipe",
    } ;

char *USAGE = "sddsmselect [-pipe[=input][,output]] [<input1>] <input2> [<output>] \n\
[-match=<column-name>[=<column-name>][,...]]\n\
[-equate=<column-name>[=<column-name>][,...]] [-invert]\n\
[-reuse[=rows][,page]]\n\n\
sddsmselect selects data from <input1> for writing to <output>\n\
based on the presence or absence of matching data in <input2>.\n\
If <output> is not given, <input1> is replaced.\n\
match      specifies names of columns to match between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.\n\
equate     specifies names of columns to equate between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.\n\
reuse      specifies that rows of <input2> may be reused, i.e., matched\n\
           with more than one row of <input1>.  Also, -reuse=page specifies\n\n\
           that only the first page of <input2> is used.\n\
invert     specifies that only nomatched rows are to be kept.\n\
nowarnings specifies that no warnings should be printed.\n\n\
Program by Michael Borland. (This is version 3, March 1996.)\n";

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_1, SDDS_2, SDDS_output;
    long outputRow, i, j, k, i_arg, rows1, rows2, reuse, reusePage;
    SCANNED_ARG *s_arg;
    char s[200], *ptr;
    STRING_PAIR *match_column, *equate_column;
    long match_columns, equate_columns;
    char *input1, *input2, *output, *match_value;
    long tmpfile_used, retval1, retval2;
    long *row_used, n, warnings, invert;
    unsigned long pipeFlags;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<3) 
        bomb(NULL, USAGE);

    input1 = input2 = output = NULL;
    match_column  = equate_column  = NULL;
    match_columns = equate_columns = reuse = reusePage = invert = 0;
    tmpfile_used = 0;
    warnings = 1;
    pipeFlags = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            delete_chars(s_arg[i_arg].list[0], "_");
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_MATCH_COLUMNS:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -match syntax");
		match_column = trealloc(match_column, 
					 sizeof(*match_column)*(match_columns+s_arg[i_arg].n_items-1));
		for (i=1; i<s_arg[i_arg].n_items; i++) {
                    if ((ptr=strchr(s_arg[i_arg].list[i], '=')))
                        *ptr++ = 0;
                    else 
                        ptr = s_arg[i_arg].list[i];
                    match_column[i-1+match_columns][0] = s_arg[i_arg].list[i];
		    match_column[i-1+match_columns][1] = ptr;
                    }
		match_columns += s_arg[i_arg].n_items-1;
                break;
              case SET_EQUATE_COLUMNS:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -equate syntax");
		equate_column = trealloc(equate_column, 
					 sizeof(*equate_column)*(equate_columns+s_arg[i_arg].n_items-1));
		for (i=1; i<s_arg[i_arg].n_items; i++) {
                    if ((ptr=strchr(s_arg[i_arg].list[i], '=')))
                        *ptr++ = 0;
                    else 
                        ptr = s_arg[i_arg].list[i];
                    equate_column[i-1+equate_columns][0] = s_arg[i_arg].list[i];
		    equate_column[i-1+equate_columns][1] = ptr;
                    }
		equate_columns += s_arg[i_arg].n_items-1;
                break;
              case SET_REUSE:
                if (s_arg[i_arg].n_items==1)
                    reuse = 1;
                else {
                    char *reuseOptions[2] = {"rows", "page"};
                    for (i=1; i<s_arg[i_arg].n_items; i++) {
                        switch (match_string(s_arg[i_arg].list[i], reuseOptions, 2, 0)) {
                          case 0:
                            reuse = 1;
                            break;
                          case 1:
                            reusePage = 1;
                            break;
                          default:
                            SDDS_Bomb("unknown reuse keyword");
                            break;
                            }
                        }
                    }
                break;
              case SET_NOWARNINGS:
                warnings = 0;
                break;
              case SET_INVERT:
                invert = 1;
                break;
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              default:
                fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
                bomb(NULL, USAGE);
                break;
                }
            }
        else {
            if (input1==NULL)
                input1 = s_arg[i_arg].list[0];
            else if (input2==NULL)
                input2 = s_arg[i_arg].list[0];
            else if (output==NULL)
                output = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }

    if (pipeFlags&USE_STDIN && input1) {
        if (output)
            SDDS_Bomb("too many filenames (sddsxref)");
        output = input2;
        input2 = input1;
        input1 = NULL;
        }
    processFilenames("sddsmselect", &input1, &output, pipeFlags, !warnings, &tmpfile_used);
    if (!input2)
        SDDS_Bomb("second input file not specified (sddsxref)");

    if (!match_columns && !equate_columns)
        SDDS_Bomb("either -match or -equate must be given");

    if (!SDDS_InitializeInput(&SDDS_1, input1)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    if (!SDDS_InitializeInput(&SDDS_2, input2)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    for (i=0; i<match_columns; i++) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, match_column[i][0]))<0 || SDDS_GetColumnType(&SDDS_1, j)!=SDDS_STRING) {
            sprintf(s, "error: column %s not found or not string type in file %s", match_column[i][0], input1?input1:"stdin");
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((j=SDDS_GetColumnIndex(&SDDS_2, match_column[i][1]))<0 || SDDS_GetColumnType(&SDDS_2, j)!=SDDS_STRING) {
            sprintf(s, "error: column %s not found or not string type in file %s", match_column[i][1], input2);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }
    for (i=0; i<equate_columns; i++) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, equate_column[i][0]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_1, j))) {
            sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[i][0], input1?input1:"stdin");
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((j=SDDS_GetColumnIndex(&SDDS_2, equate_column[i][1]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_2, j))) {
            sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[i][1], input2);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }

    if (output && pipeFlags&USE_STDOUT)
        SDDS_Bomb("too many filenames with -pipe option (sddsxref)");
    if (!output && !(pipeFlags&USE_STDOUT)) {
        if (warnings)
            fprintf(stderr, "warning: existing file %s will be replaced (sddsxref)\n", input1?input1:"stdin");
        tmpfile_used = 1;
        cp_str(&output, tmpname(NULL));
        }
    if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_1, output, "w")) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!SDDS_WriteLayout(&SDDS_output)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    row_used = NULL;
    while ((retval1=SDDS_ReadPage(&SDDS_1))>0) {
        if (!reusePage) {
            if ((retval2=SDDS_ReadPage(&SDDS_2))<=0) {
                if (warnings)
                    fprintf(stderr, "warning: <input2> ends before <input1>\n");
                if (invert) {
                    /* nothing to match, so everything would normally be thrown out */
                    if (!SDDS_CopyPage(&SDDS_output, &SDDS_1) || !SDDS_WritePage(&SDDS_output))
                        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    continue;
                    }
                else
                    /* nothing to match, so everything thrown out */
                    break;
                }
            }
        else {
            if (retval1==1 && (retval2=SDDS_ReadPage(&SDDS_2))<=0)
                SDDS_Bomb("<input2> has no data");
            SDDS_SetRowFlags(&SDDS_2, 1);
            }
        SDDS_SetRowFlags(&SDDS_1, 1);
        rows1 = SDDS_CountRowsOfInterest(&SDDS_1);
        if ((rows2 = SDDS_CountRowsOfInterest(&SDDS_2))) {
            row_used = SDDS_Realloc(row_used, sizeof(*row_used)*rows2);
            SDDS_ZeroMemory(row_used, rows2*sizeof(*row_used));
            }
        if (!SDDS_StartPage(&SDDS_output, rows1)) {
            SDDS_SetError("Problem starting output page");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (!SDDS_CopyParameters(&SDDS_output, &SDDS_1) || !SDDS_CopyArrays(&SDDS_output, &SDDS_1)) {
            SDDS_SetError("Problem copying parameter or array data from first input file");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        outputRow = 0;
        for (j=0; j<rows1; j++) {
            /* Set up to match all rows of file 2 to row j of file 1 */
            SDDS_SetRowFlags(&SDDS_2, 1);
            for (i=0; i<match_columns; i++) {
                if (!SDDS_GetValue(&SDDS_1, match_column[i][0], j, &match_value)) {
                    sprintf(s, "Problem getting column %s from file %s", match_column[i][0], input1?input1:"stdin");
                    SDDS_SetError(s);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    }
                if (SDDS_MatchRowsOfInterest(&SDDS_2, match_column[i][1], match_value, SDDS_AND)<0) {
                    sprintf(s, "Problem setting rows of interest for column %s", match_column[i][1]);
                    SDDS_SetError(s);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    }
                }
            n = SDDS_CountRowsOfInterest(&SDDS_2);
            if ((!n && !invert) || (n && invert))
                /* no match in file2 for row j of file 1, or unwanted match found--so don't copy it */
                continue;
            for (k=0; k<rows2; k++) {
                if (SDDS_GetRowFlag(&SDDS_2, k)<0)
                    /* test if row k of file 2 passed string-matches.  If not, go to next row */
                    continue;
                /* If row k of file 2 is not already used, then test it for a match to row j of file 1.
                   If no -equate options were given, this test is always true.
                   */
                if (!row_used[k]) {
                    long equal;
                    equal = rows_equate(&SDDS_1, j, &SDDS_2, k, equate_columns, equate_column);
                    if ((equal && !invert) || (!equal && invert)) {
                        row_used[k] = reuse?0:1;
                        break;
                        }
                    }
                }
            if ((k==rows2 && !invert) || (k!=rows2 && invert))
                /* no match in file 2 for row j of file 1, or unwanted match found--so don't copy it */
                continue;
            if (!SDDS_CopyRowDirect(&SDDS_output, outputRow, &SDDS_1, j)) {
                sprintf(s, "Problem copying to row %ld of output from row %ld of data set 1",
                                      outputRow, j);
                SDDS_SetError(s);
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
            outputRow++;
            }
        if (!SDDS_WritePage(&SDDS_output)) {
            SDDS_SetError("Problem writing data to output file");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }

    if (!SDDS_Terminate(&SDDS_1) || !SDDS_Terminate(&SDDS_2) || !SDDS_Terminate(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    if (tmpfile_used && !replaceFileAndBackUp(input1, output))
        exit(1);
    return(0);
}

long rows_equate(SDDS_DATASET *SDDS1, long row1, SDDS_DATASET *SDDS2, long row2, 
                 long equate_columns, STRING_PAIR *equate_column)
{
    char *data1, *data2;
    long index1, index2, size, type, i;
    char s[SDDS_MAXLINE];
    index2 = 0;
    for (i=0; i<equate_columns; i++) {
        if ((index1=SDDS_GetColumnIndex(SDDS1, equate_column[i][0]))<0 ||
            (index2=SDDS_GetColumnIndex(SDDS2, equate_column[i][1]))<0 ) {
            SDDS_SetError("Problem equating rows");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((type = SDDS_GetColumnType(SDDS1, index1))!=SDDS_GetColumnType(SDDS2, index2)) {
            sprintf(s, "Problem equating rows--types don't match for column %s=%s", 
                                  equate_column[i][0], equate_column[i][1]);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        size = SDDS_GetTypeSize(type);
        data1 = (char*)SDDS1->data[index1]+size*row1;
        data2 = (char*)SDDS2->data[index2]+size*row2;
        if (memcmp(data1, data2, size)!=0)
            return(0);
        }
    return(1);
    }
