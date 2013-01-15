/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: mpl2sdds
 * purpose: add mpl data sets to a SDDS data set
 *
 * Michael Borland, 1993
 $Log: not supported by cvs2svn $
 Revision 1.6  2002/08/14 17:12:38  soliday
 Added Open License

 Revision 1.5  2001/01/23 19:14:56  soliday
 Standardized usage message.

 Revision 1.4  1999/05/25 19:04:02  soliday
 Removed compiler warning on linux.

 Revision 1.3  1995/09/06 14:55:55  saunders
 First test release of SDDS1.5

 */
#include "mdb.h"
#include "table.h"
#include "SDDS.h"
#include "scan.h"
#include "match_string.h"

#define SET_ERASE 0
#define SET_OUTPUT 1
#define SET_BINARY 2
#define N_OPTIONS 3

char *option[N_OPTIONS] = {
    "erase", "output", "binary"
    } ;

char *USAGE = "mpl2sdds <mpl-filename> [<mpl-filename>...] \n\
-output=<SDDS-filename> [-erase] [-binary]\n\n\
Program by Michael Borland. (This is version 1, November 1993.).";

void extract_name_and_unit(char **name, char **unit, char *label);
long add_definition(SDDS_DATASET *SDDS_dataset, char *label, char *filename);
void fix_mpl_name(char *name);

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset;
    TABLE *mpl_data;
    SCANNED_ARG *scanned;
    char **input;
    long i, j, i_arg, inputs;
    char *output;
    long erase, rows, new_columns, SDDS_rows;
    double **data;
    long *index;
    long binary;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&scanned, argc, argv);
    if (argc<3)
        bomb(NULL, USAGE);

    input = NULL;
    output = NULL;
    inputs = erase = binary = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (scanned[i_arg].arg_type==OPTION) {
            /* process options here */
            switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_ERASE:
                erase = 1;
                break;
              case SET_OUTPUT:
                if (scanned[i_arg].n_items!=2)
                    bomb("invalid -output syntax", USAGE);
                output = scanned[i_arg].list[1];
                break;
              case SET_BINARY:
                if (scanned[i_arg].n_items!=1)
                    bomb("invalid -binary syntax", USAGE);
                binary = 1;
                break;
              default:
                bomb("unknown option given", USAGE);
                break;
                }
            }
        else {
            input = trealloc(input, (inputs+1)*sizeof(*input));
            input[inputs++] = scanned[i_arg].list[0];
            }
        }

    if (!output)
        bomb("-output option must be given", USAGE);
    if (!input)
        bomb("no input files listed", USAGE);

    if (!erase && fexists(output)) {
        /* load data from existing file */
        if (!SDDS_InitializeInput(&SDDS_dataset, output)) {
            fprintf(stderr, "error: couldn't read SDDS layout\n");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
        if (!SDDS_ReadPage(&SDDS_dataset)) {
            fprintf(stderr, "error: couldn't read data table\n");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
        SDDS_rows = SDDS_CountRowsOfInterest(&SDDS_dataset);
        fclose(SDDS_dataset.layout.fp);
        SDDS_dataset.layout.fp = fopen_e(output, "w", 0);
        }
    else {
        /* start a new file */
        if (!SDDS_InitializeOutput(&SDDS_dataset, SDDS_BINARY, 1, NULL, NULL, output)) {
            fprintf(stderr, "error: unable to initialize output SDDS structure\n");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
        SDDS_rows = -1;
        }
    

    rows = 0;
    mpl_data = tmalloc(sizeof(*mpl_data)*inputs);
    data = tmalloc(sizeof(*data)*(2*inputs));
    index = tmalloc(sizeof(*index)*(2*inputs));
    new_columns = 0;
    for (i=0; i<inputs; i++) {
        if (!get_table(mpl_data+i, input[i], 1, 0)) {
            fprintf(stderr, "warning: unable to read data from %s--continuing\n", input[i]);
            continue;
            }
 
        if (!rows) {
            if (!(rows = mpl_data[i].n_data)) {
                fprintf(stderr, "warning: no data in file %s--continuing\n", input[i]);
                continue;
                }
            }
        else if (rows!=mpl_data[i].n_data)
            SDDS_Bomb("mpl files do not have the same number of data points");
        else if (SDDS_rows!=-1 && rows!=SDDS_rows)
            SDDS_Bomb("mpl files must have same number of data points as SDDS file has rows");

        if ((index[new_columns]=add_definition(&SDDS_dataset, mpl_data[i].xlab, input[i]))<0)
            free(mpl_data[i].c1);
        else
            data[new_columns++] = mpl_data[i].c1;

        if ((index[new_columns]=add_definition(&SDDS_dataset, mpl_data[i].ylab, input[i]))<0)
            free(mpl_data[i].c2);
        else
            data[new_columns++] = mpl_data[i].c2;
        }


    if (!rows || !new_columns)
        SDDS_Bomb("all files are empty or invalid");

    if (!SDDS_WriteLayout(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    if (!SDDS_StartPage(&SDDS_dataset, rows)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    for (i=0; i<new_columns; i++)
        for (j=0; j<rows; j++) 
            SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, j, index[i], data[i][j], -1);

    if (!SDDS_WritePage(&SDDS_dataset)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    return(0);
}


long add_definition(SDDS_DATASET *SDDS_dataset, char *label, char *filename)
{
    char *symbol, *name, *unit;
    long index;

    extract_name_and_unit(&symbol, &unit, label);
    SDDS_CopyString(&name, symbol);
    fix_mpl_name(name);
    if (SDDS_GetColumnIndex(SDDS_dataset, name)>=0) {
        fprintf(stderr, "warning: column name %s from file %s already exists--ignored\n", name, filename);
        return(-1);
        }
    if ((index=SDDS_DefineColumn(SDDS_dataset, name, symbol, unit, NULL, NULL, SDDS_DOUBLE, 0))<0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    return(index);
    }


void extract_name_and_unit(char **name, char **unit, char *label)
{
    char *ptr, *uptr;

    if ((uptr=strchr(label, '('))) {
        *uptr++ = 0;
        if ((ptr=strchr(uptr, ')')))
            *ptr = 0;
        SDDS_CopyString(unit, uptr);
        }
    else
        *unit = NULL;
    ptr = label + strlen(label)-1;
    while (ptr!=label && *ptr==' ')
        *ptr-- = 0;
    SDDS_CopyString(name, label);
    }

void fix_mpl_name(char *name)
{
    char *ptr, *ptr1;
    ptr = name;
    while ((ptr=strchr(ptr, '$'))) {
        switch (*(ptr+1)) {
          case 'a': case 'b': case 'n': case 'g': case 'r': case 's': case 'e':
            strcpy(ptr, ptr+2);
            break;
          default:
            ptr += 1;
            break;
            }
        }
    ptr = name;
    while ((ptr=strchr(ptr, ' '))) {
        ptr1 = ptr;
        while (*ptr1==' ')
            ptr1++;
        strcpy(ptr, ptr1);
        }
    }
