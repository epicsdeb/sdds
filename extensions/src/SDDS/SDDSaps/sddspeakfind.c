/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddspeakfind.c
 * purpose: find peaks in one SDDS column as a function of another.
 * M. Borland, 1995, based on mpl peakfind program.
 $Log: sddspeakfind.c,v $
 Revision 1.13  2005/11/04 22:46:16  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.12  2003/07/16 23:55:40  shang
 added feature to -threshold option, i,e., -threshold=<value>|@<parameterName>.
 The threshold can be obtained from the parameter if -threshold=@<parameterName>
 is provided, and it overrides -threshold=<value>

 Revision 1.11  2002/08/14 17:12:50  soliday
 Added Open License

 Revision 1.10  2002/01/16 18:20:45  soliday
 Added missing SDDS_Terminate function.

 Revision 1.9  2001/01/23 19:47:29  soliday
 Fixed Solaris compiler warnings.

 Revision 1.8  2001/01/10 19:35:42  soliday
 Standardized usage message.

 Revision 1.7  1999/05/25 19:12:59  soliday
 Removed compiler warning on linux.

 Revision 1.6  1999/01/06 19:54:49  borland
 Fixed the version number in the usage message.

 * Revision 1.5  1996/03/31  04:01:58  borland
 * Exclusion zone now applied to peaks that are <= first peak, not just <.
 *
 * Revision 1.4  1996/03/31  03:34:17  borland
 * Improved behavior for digitized data by allowing one side value to be
 * equal to the middle value.
 *
 * Revision 1.3  1995/09/06  14:56:52  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define THRESHOLD   0x0001UL
#define FIVEPOINT   0x0002UL
#define CHANGETHRES 0x0004UL
#define EZONEFRAC   0x0008UL
#define PAR_THRESHOLD 0x0010UL

#define CLO_THRESHOLD 0
#define CLO_FIVEPOINT 1
#define CLO_CHANGETHRESHOLD 2
#define CLO_PIPE 3
#define CLO_COLUMN 4
#define CLO_EXCLUSIONZONE 5
#define N_OPTIONS 6

char *option[N_OPTIONS] = {
    "threshold", "fivepoint", "changethreshold", "pipe", "column",
    "exclusionzone"
    };

static char *USAGE="sddspeakfind [<inputfile>] [<outputfile>] [-pipe=[input][,output]] \n\
-column=<columnName> [-threshold=<value>|@<parametername>] [-fivePoint]\n\
[-exclusionZone=<fraction>] [-changeThreshold=<fractionalChange>]\n\n\
Finds peaks in a column of data as a function of row index.\n\
Program by Michael Borland. (This is version 2, March 1996.)\n";

void markPeaks(double *data, int32_t *rowFlag, long rows, long fivePoint);
void unmarkFlatPeaks(double *data, int32_t *rowFlag, long rows, double changeThreshold, long fivePoint);
void unmarkExcludedPeaks(double *data, int32_t *rowFlag, long rows, double ezoneFraction);

int main(int argc, char **argv)
{
    SDDS_DATASET inSet, outSet;
    SCANNED_ARG *s_arg;
    long i_arg, pageReturned, rows, row;
    int32_t *rowFlag;
    char *input, *output, *columnName, *par_thresholdName;
    double *data;
    unsigned long pipeFlags, flags;
    double threshold, ezoneFraction, changeThreshold;
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2 || argc>(2+N_OPTIONS))
        bomb(NULL, USAGE);

    flags = pipeFlags = 0;
    input = output = NULL;
    columnName = NULL;
    ezoneFraction = changeThreshold = 0;
    rowFlag = NULL;
    par_thresholdName=NULL;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
            case CLO_THRESHOLD:
              if (s_arg[i_arg].n_items==2) {
                if (s_arg[i_arg].list[1][0]=='@' ) {
                  SDDS_CopyString(&par_thresholdName, s_arg[i_arg].list[1]+1);
                  flags |= PAR_THRESHOLD;
                } else  {
                  if (sscanf(s_arg[i_arg].list[1], "%lf", &threshold)!=1)
                    SDDS_Bomb("incorrect -threshold syntax");
                  flags |= THRESHOLD;
                }
              } else
                 SDDS_Bomb("incorrect -threshold syntax");
                break;
              case CLO_FIVEPOINT:
                flags |= FIVEPOINT;
                break;
              case CLO_CHANGETHRESHOLD:
                if (s_arg[i_arg].n_items!=2 ||
                    sscanf(s_arg[i_arg].list[1], "%lf", &changeThreshold)!=1 || changeThreshold<=0)
                    SDDS_Bomb("incorrect -changeThreshold syntax or values");
                flags |= CHANGETHRES;
                break;
              case CLO_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_COLUMN:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -column syntax");
                columnName = s_arg[i_arg].list[1];
                break;
              case CLO_EXCLUSIONZONE:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%lf", &ezoneFraction)!=1 ||
                    ezoneFraction<=0)
                    SDDS_Bomb("invalid -exclusionZone syntax or value");
                flags |= EZONEFRAC;
                break;
              default:
                fprintf(stderr, "error: unknown/ambiguous option: %s\n", s_arg[i_arg].list[0]);
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

    processFilenames("sddspeakfind", &input, &output, pipeFlags, 0, NULL);

    if (!columnName)
        SDDS_Bomb("-column option must be given");

    if (!SDDS_InitializeInput(&inSet, input))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_FindColumn(&inSet, FIND_NUMERIC_TYPE, columnName, NULL))
        SDDS_Bomb("the given column is nonexistent or nonnumeric");

    if (!SDDS_InitializeCopy(&outSet, &inSet, output, "w") || !SDDS_WriteLayout(&outSet))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    while ((pageReturned=SDDS_ReadPage(&inSet))>0) {
        if (!SDDS_CopyPage(&outSet, &inSet)) {
            SDDS_SetError("Problem copying data for output file");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        if ((rows=SDDS_CountRowsOfInterest(&outSet))>1) {
            if (!(data = SDDS_GetColumnInDoubles(&inSet, columnName)))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            rowFlag = SDDS_Realloc(rowFlag, sizeof(*rowFlag)*rows);
            for (row=0; row<rows; row++)
                rowFlag[row] = 0;
            markPeaks(data, rowFlag, rows, flags&FIVEPOINT);
            if (flags&PAR_THRESHOLD) {
              if (!SDDS_GetParameter(&inSet,par_thresholdName,&threshold))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
            if (flags&THRESHOLD || flags&PAR_THRESHOLD ) {
              for (row=0; row<rows; row++)
                if (rowFlag[row] && data[row]<threshold)
                  rowFlag[row] = 0;
            }
            if (flags&CHANGETHRES) 
                unmarkFlatPeaks(data, rowFlag, rows, changeThreshold, flags&FIVEPOINT);
            if (flags&EZONEFRAC) 
                unmarkExcludedPeaks(data, rowFlag, rows, ezoneFraction);
            if (!SDDS_AssertRowFlags(&outSet, SDDS_FLAG_ARRAY, rowFlag, rows))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            free(data);
            }
        if (!SDDS_WritePage(&outSet))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!SDDS_Terminate(&inSet) || !SDDS_Terminate(&outSet)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    free_scanargs(&s_arg,argc);
    if (par_thresholdName) free(par_thresholdName);
    if (rowFlag) free(rowFlag);
    
    return(0);
}

void markPeaks(double *data, int32_t *rowFlag, long rows, long fivePoint)
{
    double y0, y1, y2;
    long i, rowsMinus;

    if (!fivePoint) {
        if (rows<3)
	    return ;
        y0 = data[0];
        y1 = data[1];
        }
    else {
        y0 = data[1];
        y1 = data[2];
	if (rows<5)
	    return;
        }

    rowsMinus = rows - (fivePoint?2:1);
    for (i=(fivePoint?2:1); i<rowsMinus; i++) {
        y2 = data[i+1];
        if ((y1>y0 && y1>y2) || (y1==y0 && y1>y2) || (y1>y0 && y1==y2)) {
            if (!fivePoint || (data[i-2]<y0 && data[i+2]<y2))
                rowFlag[i] = 1;
            }
        y0 = y1;
        y1 = y2;
        }
    }

void unmarkFlatPeaks(double *data, int32_t *rowFlag, long rows, double changeThreshold, long fivePoint)
{
    long i, delta, rows1;

    delta = fivePoint?2:1;
    rows1 = rows-delta;
    for (i=delta-1; i<=rows1; i++)
        if (rowFlag[i]) {
            if (i!=rows1) {
                if (!data[i] || (data[i]-data[i+delta])/data[i]>changeThreshold)
                    continue;
                }
            if (i!=0) {
                if (!data[i] || (data[i]-data[i-delta])/data[i]>changeThreshold)
                    continue;
                }
            rowFlag[i] = 0;
            }
    }

void unmarkExcludedPeaks(double *data, int32_t *rowFlag, long rows, double ezoneFraction)
{
    long lower, upper, i, j, rows1;
    rows1 = rows-1;
    for (i=0; i<rows; i++)  {
        if (rowFlag[i]) {
            if ((lower = i - ((long)(ezoneFraction/2.0*rows+0.5)))<0)
                lower = 0;
            if ((upper = i + ((long)(ezoneFraction/2.0*rows+0.5)))>rows1)
                upper = rows1;
            for (j=lower; j<=upper; j++) {
	        if (j!=i && rowFlag[j] && data[j]<=data[i])
                    rowFlag[j] = 0;
                }
            }
        }
    }

