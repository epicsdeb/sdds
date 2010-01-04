/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsselect
 * purpose: creates a SDDS data set from another data set based
 *          on matching to data in a third data set.
 *
 * For example, one can select all of the rows from file1 that
 * have a match in file2, or that have no match in file2.
 * Similar to sddsxref, but doesn't transfer any data.
 *
 * Michael Borland, 1995
 $Log: sddsselect.c,v $
 Revision 1.17  2005/07/21 14:13:19  borland
 Additional changes to prevent possible free'ing of unallocated memory.

 Revision 1.16  2005/07/21 14:08:31  borland
 Fixed problem with freeing unallocated memory in key groups.

 Revision 1.15  2003/09/26 14:50:28  soliday
 Moved free_scanargs below replaceFileAndBackUp

 Revision 1.14  2003/09/02 19:16:06  soliday
 Cleaned up code for Linux.

 Revision 1.13  2002/10/23 03:26:26  shang
 fixed the bug which returned segmentation error when the second input file is
 empty

 Revision 1.12  2002/10/21 22:44:13  shang
 replaced the searching method by quick sort and comparison method implemented
 in sddsxref.c originally

 Revision 1.11  2002/08/14 17:12:53  soliday
 Added Open License

 Revision 1.10  2001/01/23 19:47:29  soliday
 Fixed Solaris compiler warnings.

 Revision 1.9  1999/09/28 15:39:49  soliday
 Added SDDS_Terminate at the end.

 Revision 1.8  1999/05/25 19:14:55  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:55  borland
 Fixed the version number in the usage message.

 * Revision 1.6  1996/03/11  02:46:06  borland
 * Handles empty pages in input2 properly.  If invert is on, copies remainder
 * of input1.  Otherwise, deletes remainder of input1.
 *
 * Revision 1.5  1996/03/10  01:09:18  borland
 * Fixed behavior for empty pages.  If input2 ends, copies remainder of input1
 * to output.
 *
 * Revision 1.4  1996/03/09  23:18:44  borland
 * Added if statement for conditional warning.
 *
 * Revision 1.3  1995/09/06  14:57:04  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
/*#include "SDDSaps.h"*/
#include "scan.h"

#define SET_MATCH_COLUMN 0
#define SET_EQUATE_COLUMN 1
#define SET_NOWARNINGS 2
#define SET_INVERT 3
#define SET_REUSE 4
#define SET_PIPE 5
#define N_OPTIONS 6

char *option[N_OPTIONS] = {
    "match", "equate", "nowarnings", "invert", "reuse", "pipe",
    } ;

char *USAGE = "sddsselect [-pipe[=input][,output]] [<input1>] <input2> [<output>] \n\
[-match=<column-name>[=<column-name>]]\n\
[-equate=<column-name>[=<column-name>]] [-invert]\n\
[-reuse[=rows][,page]]\n\n\
sddsselect selects data from <input1> for writing to <output>\n\
based on the presence or absence of matching data in <input2>.\n\
If <output> is not given, <input1> is replaced.\n\
match      specifies names of columns to match between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.  In general, the first unused match from <input2>\n\
           is taken.\n\
equate     specifies names of columns to equate between <input1> and\n\
           <input2> for selection and placement of data taken from\n\
           <input2>.  In general, the first unused match from <input2>\n\
           is taken.\n\
reuse      specifies that rows of <input2> may be reused, i.e., matched\n\
           with more than one row of <input1>.  Also, -reuse=page specifies\n\n\
           that only the first page of <input2> is used.\n\
invert     specifies that only nomatched rows are to be kept.\n\
nowarnings specifies that no warnings should be printed.\n\n\
Program by Michael Borland. (This is version 3, March 1996.)\n";

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_1, SDDS_2, SDDS_output;
    long i, j, i_arg, rows1, rows2, reuse, reusePage, i1, i2;
    SCANNED_ARG *s_arg;
    char s[200], *ptr;
    char **match_column, **equate_column;
    long match_columns, equate_columns;
    char *input1, *input2, *output;
    long tmpfile_used, retval1, retval2;
    long warnings, invert;
    unsigned long pipeFlags;
    KEYED_EQUIVALENT **keyGroup=NULL;
    long keyGroups=0;
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<3) 
        bomb(NULL, USAGE);

    input1 = input2 = output = NULL;
    match_column  = equate_column  = NULL;
    match_columns = equate_columns = reuse = reusePage = 0;
    tmpfile_used = invert = 0;
    warnings = 1;
    pipeFlags = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            delete_chars(s_arg[i_arg].list[0], "_");
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_MATCH_COLUMN:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -match syntax");
                if (match_columns!=0)
                    SDDS_Bomb("only one -match option may be given");
		match_column = tmalloc(sizeof(*match_column)*2);
                if ((ptr=strchr(s_arg[i_arg].list[1], '=')))
                    *ptr++ = 0;
                else 
                    ptr = s_arg[i_arg].list[1];
                match_column[0] = s_arg[i_arg].list[1];
                match_column[1] = ptr;
		match_columns = 1;
                break;
              case SET_EQUATE_COLUMN:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -equate syntax");
                if (equate_columns!=0)
                    SDDS_Bomb("only one -equate option may be given");
		equate_column = tmalloc(sizeof(*equate_column)*2);
                if ((ptr=strchr(s_arg[i_arg].list[1], '=')))
                    *ptr++ = 0;
                else 
                    ptr = s_arg[i_arg].list[1];
                equate_column[0] = s_arg[i_arg].list[1];
                equate_column[1] = ptr;
		equate_columns = 1;
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
              case SET_INVERT:
                invert = 1;
                break;
              case SET_NOWARNINGS:
                warnings = 0;
                break;
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              default:
                fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
                SDDS_Bomb(NULL);
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
    processFilenames("sddsselect", &input1, &output, pipeFlags, !warnings, &tmpfile_used);
    if (!input2)
        SDDS_Bomb("second input file not specified (sddsxref)");

    if (equate_columns && match_columns)
        SDDS_Bomb("only one of -equate or -match may be given");
    if (!equate_columns && !match_columns)
        SDDS_Bomb("one of -equate or -match must be given");

    if (!SDDS_InitializeInput(&SDDS_1, input1)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    if (!SDDS_InitializeInput(&SDDS_2, input2)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (match_columns) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, match_column[0]))<0 || SDDS_GetColumnType(&SDDS_1, j)!=SDDS_STRING) {
            sprintf(s, "error: column %s not found or not string type in file %s", match_column[0], input1?input1:"stdin");
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((j=SDDS_GetColumnIndex(&SDDS_2, match_column[1]))<0 || SDDS_GetColumnType(&SDDS_2, j)!=SDDS_STRING) {
            sprintf(s, "error: column %s not found or not string type in file %s", match_column[1], input2);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }
    if (equate_columns) {
        if ((j=SDDS_GetColumnIndex(&SDDS_1, equate_column[0]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_1, j))) {
            sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[0], input1?input1:"stdin");
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((j=SDDS_GetColumnIndex(&SDDS_2, equate_column[1]))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDS_2, j))) {
            sprintf(s, "error: column %s not found or not numeric type in file %s", equate_column[1], input2);
            SDDS_SetError(s);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }

    if (output && pipeFlags&USE_STDOUT)
        SDDS_Bomb("too many filenames with -pipe option");
    if (!output && !(pipeFlags&USE_STDOUT)) {
        if (warnings)
            fprintf(stderr, "warning: existing file %s will be replaced (sddsselect)\n", input1?input1:"stdin");
        tmpfile_used = 1;
        cp_str(&output, tmpname(NULL));
        }
    if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_1, output, "w")) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!SDDS_WriteLayout(&SDDS_output)) 
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
   
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
        rows1 = SDDS_CountRowsOfInterest(&SDDS_1);
        rows2 = SDDS_CountRowsOfInterest(&SDDS_2);
        
        if (!SDDS_StartPage(&SDDS_output, rows1)) {
            SDDS_SetError("Problem starting output page");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (!SDDS_CopyParameters(&SDDS_output, &SDDS_2) || !SDDS_CopyArrays(&SDDS_output, &SDDS_2)) {
            SDDS_SetError("Problem copying parameter or array data from second input file");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (!SDDS_CopyParameters(&SDDS_output, &SDDS_1) || !SDDS_CopyArrays(&SDDS_output, &SDDS_1)) {
            SDDS_SetError("Problem copying parameter or array data from first input file");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if (match_columns) {
          char **string1, **string2;
          long matched;
          string2 = NULL;
          if (!(string1 = SDDS_GetColumn(&SDDS_1, match_column[0]))) {
            fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                    match_column[0], input1?input1:"stdin");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (rows2 && !(string2 = SDDS_GetColumn(&SDDS_2, match_column[1]))) {
            fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                    match_column[1], input2);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (rows2)
            keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_STRING, string2, rows2);
          for (i1=0; i1<rows1; i1++) {
            if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
              sprintf(s, "Problem copying row %ld of first data set", i1);
              SDDS_SetError(s);
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
            matched = 0;
            if (rows2 &&(i2 = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_STRING, string1+i1, reuse))>=0) {
              matched = 1;
            }
            if ((!matched && !invert) || (matched && invert)) {
              if (!SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, 0)) 
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
          }
          if (string1) {
            for (i=0;i<rows1;i++) 
              free(string1[i]);
            free(string1);
            string1 = NULL;
          }
          if (string2) {
            for (i=0;i<rows2;i++) 
              free(string2[i]);
            free(string2);
            string2 = NULL;
          }
          for (i=0;i<keyGroups;i++) {
	    if (keyGroup[i]) {
	      if (keyGroup[i]->equivalent)
		free(keyGroup[i]->equivalent);
	      free(keyGroup[i]);
	      keyGroup[i] = NULL;
	    }
          }
          if (keyGroups) {
            free(keyGroup);
	    keyGroup = NULL;
	    keyGroups = 0;
	  }
        }
        else if (equate_columns) {
          double *value1, *value2;
          long equated;
          value2 = NULL;
          if (!(value1 = SDDS_GetColumnInDoubles(&SDDS_1, equate_column[0]))) {
            fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                    equate_column[0], input1?input1:"stdin");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (rows2 && !(value2 = SDDS_GetColumnInDoubles(&SDDS_2, equate_column[1]))) {
            fprintf(stderr, "Error: problem getting column %s from file %s\n", 
                    equate_column[1], input2);
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (rows2)
            keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_DOUBLE, value2, rows2);
          for (i1=0; i1<rows1; i1++) {
            if (!SDDS_CopyRowDirect(&SDDS_output, i1, &SDDS_1, i1)) {
              sprintf(s, "Problem copying row %ld of first data set", i1);
              SDDS_SetError(s);
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
            equated = 0;
            if (rows2 && (i2 = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_DOUBLE, value1+i1, reuse))>=0) {
              equated = 1;
            }
            if ((!equated && !invert) || (equated && invert)) {
              if (!SDDS_AssertRowFlags(&SDDS_output, SDDS_INDEX_LIMITS, i1, i1, 0)) 
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
          }
          if (value1)
            free(value1);
          value1 = NULL;
          if (rows2 && value2)
            free(value2);
          value2 = NULL;
          for (i=0;i<keyGroups;i++) {
	    if (keyGroup[i]) {
	      if (keyGroup[i]->equivalent)
		free(keyGroup[i]->equivalent);
	      free(keyGroup[i]);
	      keyGroup[i] = NULL;
	    }
          }
          if (keyGroups) {
            free(keyGroup);
	    keyGroup = NULL;
	    keyGroups = 0;
	  }
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
    free_scanargs(&s_arg,argc);
    if (match_columns) 
      free(match_column);
    return(0);
}

