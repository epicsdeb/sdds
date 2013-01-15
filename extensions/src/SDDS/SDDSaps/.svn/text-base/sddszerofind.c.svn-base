/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddszerofind.c
 * purpose: find zeros in one SDDS column as a function of another.
 * M. Borland, 1995, based on mpl zerofind program.
 $Log: not supported by cvs2svn $
 Revision 1.9  2005/11/04 22:46:19  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.8  2002/08/14 17:12:58  soliday
 Added Open License

 Revision 1.7  2002/01/16 16:50:50  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.6  2001/01/10 19:35:50  soliday
 Standardized usage message.

 Revision 1.5  1999/05/25 19:17:52  soliday
 Removed compiler warning on linux.

 Revision 1.4  1996/08/23 19:27:00  borland
 Added -offset option to allow finding offset zeroes, i.e., places where a
 function passes through a specific nonzero level.

 * Revision 1.3  1995/09/06  14:57:26  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define CLO_PIPE 0
#define CLO_COLUMNS 1
#define CLO_SLOPEOUTPUT 2
#define CLO_ZEROESOF 3
#define CLO_OFFSET 4
#define N_OPTIONS 5

char *option[N_OPTIONS] = {
    "pipe", "columns", "slopeoutput", "zeroesof", "offset",
    };

static char *USAGE="sddszerofind [<inputfile>] [<outputfile>] [-pipe=[input][,output]] \n\
-zeroesOf=<columnName> [-columns=<columnNames>] [-offset=<value>] [-slopeOutput]\n\n\
Finds values of columns of data at interpolated zero positions in another\n\
column.\n\n\
-zeroesOf    Specifies the column for which to find zeroes.\n\
-offset      Specifies a value to add to the values of the -zeroesOf column\n\
             prior to finding the zeroes.  -offset=1 will find places where the\n\
             original values are -1.\n\
-columns     Specifies the columns to interpolate at the zero positions.\n\
             Default is all numerical columns in the file.\n\
-slopeOutput Provide output of the slow of each -column column at the zero\n\
             position.\n\n\
Program by Michael Borland. (This is version 2, August 1996.)\n";

#define FL_SLOPEOUTPUT 0x00001UL

long resolveColumnNames(SDDS_DATASET *SDDSin, char *depenQuantity,
                        char ***indepQuantity, int32_t *indepQuantities);

int main(int argc, char **argv)
{
    SDDS_DATASET inSet, outSet;
    SCANNED_ARG *s_arg;
    char *input, *output, *zeroName, **columnName;
    long i_arg, i, pageReturned, rows, row, zrow;
    int32_t columnNames;
    double **indepData, *depenData, **slopeData, slope, offset;
    unsigned long pipeFlags, flags;
    char s[SDDS_MAXLINE];

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2 || argc>(2+N_OPTIONS))
        bomb(NULL, USAGE);

    flags = pipeFlags = 0;
    input = output = NULL;
    zeroName = NULL;
    columnName = NULL;
    columnNames = 0;
    offset = 0;
    
    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case CLO_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_ZEROESOF:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -zeroesOf syntax");
                zeroName = s_arg[i_arg].list[1];
                break;
              case CLO_COLUMNS:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -columns syntax");
                columnName = tmalloc(sizeof(*columnName)*(columnNames = s_arg[i_arg].n_items-1));
                for (i=0; i<columnNames; i++)
                    columnName[i] = s_arg[i_arg].list[i+1];
                break;
              case CLO_SLOPEOUTPUT:
                flags |= FL_SLOPEOUTPUT;
                break;
              case CLO_OFFSET:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%le", &offset)!=1)
                  SDDS_Bomb("invalid -offset syntax");
                break;
              default:
                fprintf(stderr, "Error (%s): unknown/ambiguous option: %s\n", argv[0], s_arg[i_arg].list[0]);
                exit(1);
                break;
                }
            }
        else {
            if (input==NULL) 
                input = s_arg[i_arg].list[0];
            else if (output==NULL)
                output = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }

    processFilenames("sddszerofind", &input, &output, pipeFlags, 0, NULL);

    if (!zeroName)
        SDDS_Bomb("-zeroesOf option must be given");
    
    if (!SDDS_InitializeInput(&inSet, input))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!resolveColumnNames(&inSet, zeroName, &columnName, &columnNames) ||
        !SDDS_InitializeOutput(&outSet, SDDS_BINARY, 0, NULL, "sddszerofind output", output) ||
        !SDDS_TransferColumnDefinition(&outSet, &inSet, zeroName, NULL))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0; i<columnNames; i++) {
        sprintf(s, "%sSlope", columnName[i]);
        if (!SDDS_TransferColumnDefinition(&outSet, &inSet, columnName[i], NULL) ||
            (flags&FL_SLOPEOUTPUT && 
             !SDDS_TransferColumnDefinition(&outSet, &inSet, columnName[i], s)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!SDDS_WriteLayout(&outSet))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
    indepData = tmalloc(sizeof(*indepData)*columnNames);
    slopeData = tmalloc(sizeof(*slopeData)*columnNames);

    while ((pageReturned=SDDS_ReadPage(&inSet))>0) {
        if (!SDDS_StartPage(&outSet, 0))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if ((rows=SDDS_CountRowsOfInterest(&inSet))>1) {
            if (!(depenData = SDDS_GetColumnInDoubles(&inSet, zeroName)))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            for (i=0; i<columnNames; i++) {
                if (!(indepData[i] = SDDS_GetColumnInDoubles(&inSet, columnName[i])))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                if (flags&FL_SLOPEOUTPUT)
                    slopeData[i] = tmalloc(sizeof(**slopeData)*rows);
                }
            if (offset)
              for (row=0; row<rows; row++)
                depenData[row] += offset;
            for (zrow=row=0; row<rows-1; row++) {
                if ((depenData[row]<=0 && depenData[row+1]>=0) ||
                    (depenData[row]>=0 && depenData[row+1]<=0)) {
                    for (i=0; i<columnNames; i++) {
                        if (indepData[i][row]==indepData[i][row+1]) {
                            if (flags&FL_SLOPEOUTPUT)
                                slopeData[i][zrow] = DBL_MAX;
                            indepData[i][zrow] = indepData[i][row];
                            }
                        else {
                            slope = (depenData[row+1]-depenData[row])/(indepData[i][row+1]-indepData[i][row]);
                            if (flags&FL_SLOPEOUTPUT)
                                slopeData[i][zrow] = slope;
                            if (slope)
                                indepData[i][zrow] = indepData[i][row] - depenData[row]/slope;
                            else
                                indepData[i][zrow] = (indepData[i][row] + indepData[i][row+1])/2;
                            }
                        }
                    depenData[zrow] = -offset;
                    zrow++;
                    }
                }
            if (zrow) {
                if (!SDDS_LengthenTable(&outSet, zrow) || 
                    !SDDS_SetColumnFromDoubles(&outSet, SDDS_SET_BY_NAME, depenData, zrow, zeroName))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                for (i=0; i<columnNames; i++) {
                    sprintf(s, "%sSlope", columnName[i]);
                    if (!SDDS_SetColumnFromDoubles(&outSet, SDDS_SET_BY_NAME, indepData[i], zrow, columnName[i]) ||
                        (flags&FL_SLOPEOUTPUT &&
                         !SDDS_SetColumnFromDoubles(&outSet, SDDS_SET_BY_NAME, slopeData[i], zrow, s)))
                        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                    }
                }
            free(depenData);
            for (i=0; i<columnNames; i++)
                free(indepData[i]);
            if (flags&FL_SLOPEOUTPUT)
                for (i=0; i<columnNames; i++)
                    free(slopeData[i]);
            }
        if (!SDDS_WritePage(&outSet))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!SDDS_Terminate(&inSet) || !SDDS_Terminate(&outSet)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    return(0);
}

long resolveColumnNames(SDDS_DATASET *SDDSin, char *depenQuantity,
                        char ***indepQuantity, int32_t *indepQuantities)
{
    long index, i;
    char s[SDDS_MAXLINE];

    if ((index=SDDS_GetColumnIndex(SDDSin, depenQuantity))<0 || !SDDS_NUMERIC_TYPE(SDDS_GetColumnType(SDDSin, index))) {
        sprintf(s, "column %s is non-existent or non-numeric", depenQuantity);
        SDDS_SetError(s);
        return 0;
        }
    
    if (*indepQuantities) {
        SDDS_SetColumnFlags(SDDSin, 0);
        for (i=0; i< *indepQuantities; i++) {
            if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, (*indepQuantity)[i], SDDS_OR))
                return 0;
            }
        }
    else {
        SDDS_SetColumnFlags(SDDSin, 1);
        if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, depenQuantity, SDDS_NEGATE_MATCH|SDDS_AND))
            return 0;
        if (!(*indepQuantity = SDDS_GetColumnNames(SDDSin, indepQuantities)) ||
            *indepQuantities==0) {
            SDDS_SetError("no indepdent quantities found");
            return 0;
            }
        for (i=0; i< *indepQuantities; i++) {
            index = SDDS_GetColumnIndex(SDDSin, (*indepQuantity)[i]);
            if (!SDDS_NUMERIC_TYPE(SDDS_GetColumnType(SDDSin, index)) && 
                !SDDS_AssertColumnFlags(SDDSin, SDDS_INDEX_LIMITS, index, index, 0)) 
                return 0;
            }
        }
    free(*indepQuantity);
    if (!(*indepQuantity = SDDS_GetColumnNames(SDDSin, indepQuantities)) ||
        *indepQuantities==0) {
        SDDS_SetError("no indepdent quantities found");
        return 0;
        }
    return 1; 
    }

