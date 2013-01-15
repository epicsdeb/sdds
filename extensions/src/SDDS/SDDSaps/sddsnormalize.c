/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssnormalize
 * purpose: SDDS-format column normalization 
 *
 * Michael Borland, 1998
 */

/*
 *  $Log: not supported by cvs2svn $
 *  Revision 1.15  2006/10/19 17:55:40  soliday
 *  Updated to work with linux-x86_64.
 *
 *  Revision 1.14  2005/11/04 22:46:16  soliday
 *  Updated code to be compiled by a 64 bit processor.
 *
 *  Revision 1.13  2004/03/16 23:44:25  borland
 *  Fixed parsing bug that caused problems when column named looked like an abbreviated tag name
 *
 *  Revision 1.12  2003/09/02 19:16:05  soliday
 *  Cleaned up code for Linux.
 *
 *  Revision 1.11  2002/08/14 17:12:50  soliday
 *  Added Open License
 *
 *  Revision 1.10  2002/07/24 16:12:36  borland
 *  Fixed bug introduced in last revision.  Wasn't initializing the functionOf
 *  field.
 *
 *  Revision 1.9  2002/06/18 13:43:58  borland
 *  Added normalization by sum or area.
 *
 *  Revision 1.8  2001/11/21 22:45:24  borland
 *  Fixed usage message and code that checks for too many arguments.
 *
 *  Revision 1.7  2001/11/21 21:37:57  borland
 *  Added normalization by rms and standard deviation.
 *
 *  Revision 1.6  2001/05/03 20:55:34  soliday
 *  Standardized usage messages.
 *
 *  Revision 1.5  2001/01/10 19:35:42  soliday
 *  Standardized usage message.
 *
 *  Revision 1.4  1998/06/25 20:55:49  borland
 *  Now sets the units for the normalized columns.  If no suffix is give, the
 *  units are "Normalized", otherwise they are NULL.
 *
 *  Revision 1.3  1998/06/25 20:44:02  borland
 *  Added signedLargest and spread modes for normalization.
 *
 *  Revision 1.2  1998/06/25 19:39:56  borland
 *  Changed the definition of min/max normalization to correspond to what
 *  other sdds programs do.
 *
 *  Revision 1.1  1998/06/25 19:38:45  borland
 *  First version.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include <ctype.h>

#define CLO_COLUMNS 0
#define CLO_PIPE 1
#define CLO_OPTIONS 2

char *option[CLO_OPTIONS] = {
  "columns", "pipe",
    };

static char *USAGE = "sddsnormalize [<inputfile>] [<outputfile>] [-pipe=[input][,output]] \n\
-columns=[mode=<mode>,][suffix=<string>,][exclude=<wildcardString>,]<columnName>[,...]\n\n\
where <mode> is minimum, maximum, largest, signedlargest, \n\
spread, rms, or standarddeviation, referring to the factor used for\n\
normalization.  Largest (the default) is max(abs(min), abs(max)). \n\
signedLargest is largest with the sign of the value retained.\n\
spread is max-min. rms is the root-mean-square of the values, while\n\
standarddeviation is the n-1 weighted standard deviation of the values.\n\
The suffix is a string that is appended to the column name to\n\
make the new column name.  If left blank, the original column\n\
is replaced.  \n\
Program by Michael Borland.  (This is version 2, November 2001.)\n";

/* different modes for normalizing */
#define NORM_MINIMUM 0
#define NORM_MAXIMUM 1
#define NORM_LARGEST 2
#define NORM_SLARGEST 3
#define NORM_SPREAD 4
#define NORM_RMS 5
#define NORM_STDEV 6
#define NORM_SUM 7
#define NORM_AREA 8
#define NORM_OPTIONS 9
static char *normMode[NORM_OPTIONS] = {
  "minimum", "maximum", "largest", "signedlargest",
  "spread", "rms", "standarddeviation", "sum", "area",
} ;

/* structure for users requests to normalize */
#define FL_SUFFIX_GIVEN 0x0001U
#define FL_MODE_GIVEN   0x0002U
#define FL_FUNCOF_GIVEN 0x0004U
typedef struct {
  unsigned long flags;
  char *suffix, **source, *exclude, *functionOf;
  long sources, mode;
} NORM_REQUEST;

/* individual specifications for one column, made from
 * users request after expanding wildcards and lists
 */
typedef struct {
  unsigned long flags;
  char *source, *target, *functionOf;
  long mode;
} NORM_SPEC;

long resolveColumnNames(SDDS_DATASET *SDDSin, 
                        NORM_REQUEST *normRequest, long normRequests, 
                        NORM_SPEC **normSpecRet, long *normSpecsRet);


int main(int argc, char **argv)
{
  int iArg;
  NORM_REQUEST *normRequest;
  NORM_SPEC *normSpec;
  long normRequests, normSpecs, i, j, readCode, rows;
  char *input, *output, *modeString;
  unsigned long pipeFlags;
  SCANNED_ARG *scanned;
  SDDS_DATASET SDDSin, SDDSout;
  double *data, *funcOfData, factor, min, max;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<3)
    bomb(NULL, USAGE);

  output = input = NULL;
  pipeFlags = 0;
  normRequest = NULL;
  normSpec = NULL;
  normRequests = normSpecs = 0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_COLUMNS:
        if (!(normRequest=SDDS_Realloc(normRequest, sizeof(*normRequest)*(normRequests+1))))
          SDDS_Bomb("memory allocation failure");
        normRequest[normRequests].exclude = 
          normRequest[normRequests].suffix = NULL;
        if (!scanItemList(&normRequest[normRequests].flags,
                          scanned[iArg].list, &scanned[iArg].n_items,
                          SCANITEMLIST_UNKNOWN_VALUE_OK|SCANITEMLIST_REMOVE_USED_ITEMS|
                          SCANITEMLIST_IGNORE_VALUELESS,
                          "mode", SDDS_STRING, &modeString, 1, FL_MODE_GIVEN, 
                          "suffix", SDDS_STRING, &normRequest[normRequests].suffix,
                          1, FL_SUFFIX_GIVEN,
                          "functionof", SDDS_STRING, &normRequest[normRequests].functionOf,
                          1, FL_FUNCOF_GIVEN,
                          "exclude", SDDS_STRING, &normRequest[normRequests].exclude, 1, 0,
                          NULL))
          SDDS_Bomb("invalid -columns syntax");
        if (normRequest[normRequests].flags&FL_MODE_GIVEN) {
          if ((normRequest[normRequests].mode=match_string(modeString, normMode, NORM_OPTIONS, 0))<0) 
            SDDS_Bomb("invalid -columns syntax: unknown mode");
        } else 
          normRequest[normRequests].mode = NORM_LARGEST;
        if (scanned[iArg].n_items<1) 
          SDDS_Bomb("invalid -columns syntax: no columns listed");
        normRequest[normRequests].source = scanned[iArg].list+1;
        normRequest[normRequests].sources = scanned[iArg].n_items-1;
        normRequests++;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      default:
        fprintf(stderr, "error: unknown/ambiguous option: %s\n", 
                scanned[iArg].list[0]);
        exit(1);
        break;
      }
    }
    else {
      if (!input)
        input = scanned[iArg].list[0];
      else if (!output)
        output = scanned[iArg].list[0];
      else
        SDDS_Bomb("too many filenames seen");
    }
  }

  processFilenames("sddsnormalize", &input, &output, pipeFlags, 0, NULL);

  if (!normRequests)
    SDDS_Bomb("supply the names of columns to normalize with the -columns option");

  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!resolveColumnNames(&SDDSin, normRequest, normRequests, &normSpec, &normSpecs))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!normSpecs)
    SDDS_Bomb("no columns selected for normalization");

  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  for (i=0; i<normSpecs; i++) {
    if (normSpec[i].flags&FL_SUFFIX_GIVEN) {
      if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, normSpec[i].source,
                                         normSpec[i].target) || 
          !SDDS_ChangeColumnInformation(&SDDSout, "units", "", SDDS_BY_NAME,
                                        normSpec[i].target))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    else if (!SDDS_ChangeColumnInformation(&SDDSout, "units", "Normalized", SDDS_BY_NAME,
                                           normSpec[i].target))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }

  if (!SDDS_WriteLayout(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if (!SDDS_CopyPage(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if ((rows = SDDS_RowCount(&SDDSin))) {
      for (i=0; i<normSpecs; i++) {
        if (!(data = SDDS_GetColumnInDoubles(&SDDSin, normSpec[i].source)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        funcOfData = NULL;
        if (normSpec[i].functionOf &&
            !(funcOfData=SDDS_GetColumnInDoubles(&SDDSin, normSpec[i].functionOf)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (!find_min_max(&min, &max, data, rows))
          min = max = 1;
        switch (normSpec[i].mode) {
	case NORM_RMS:
	  factor = rmsValue(data, rows);
	  break;
	case NORM_STDEV:
	  factor = standardDeviation(data, rows);
	  break;
        case NORM_MINIMUM:
          factor = min;
          break;
        case NORM_MAXIMUM:
          factor = max;
          break;
        case NORM_LARGEST:
          min = fabs(min);
          max = fabs(max);
          factor = MAX(min, max);
          break;
        case NORM_SLARGEST:
          if (fabs(min)>fabs(max))
            factor = min;
          else
            factor = max;
          break;
        case NORM_SPREAD:
          factor = max-min;
          break;
        case NORM_SUM:
          for (j=factor=0; j<rows; j++)
            factor += data[j];
          break;
        case NORM_AREA:
          if (!funcOfData)
            SDDS_Bomb("functionOf qualifier must be given for area normalization");
          trapazoidIntegration(funcOfData, data, rows, &factor);
          break;
        default:
          SDDS_Bomb("Invalid normalization mode---programming error");
          break;
        }
        if (funcOfData)
          free(funcOfData);
        if (factor)
          for (j=0; j<rows; j++)
            data[j] /= factor;
        if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, data, rows, normSpec[i].target))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        free(data);
      }
    }
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  return(0);
}

long resolveColumnNames(SDDS_DATASET *SDDSin, 
                        NORM_REQUEST *normRequest, long normRequests, 
                        NORM_SPEC **normSpecRet, long *normSpecsRet)
                          
{
  long i, j;
  int32_t columns;
  char **column, buffer[1024];
  long normSpecs = 0;
  NORM_SPEC *normSpec = NULL;
  
  for (i=0; i<normRequests; i++) {
    SDDS_SetColumnFlags(SDDSin, 0);
    if (normRequest[i].flags&FL_SUFFIX_GIVEN) {
      if (!normRequest[i].suffix || !strlen(normRequest[i].suffix)) {
        SDDS_SetError("resolveColumnNames: missing or blank suffix");
        return 0;
      }
    }
    for (j=0; j<normRequest[i].sources; j++) {
      if (!SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, normRequest[i].source[j], SDDS_OR)) {
        SDDS_SetError("resolveColumnNames: SDDS_SetColumnsOfInterest error");
        return 0;
      }
    }
    if (normRequest[i].exclude && 
        !SDDS_SetColumnsOfInterest(SDDSin, SDDS_MATCH_STRING, normRequest[i].exclude, 
                                   SDDS_NEGATE_MATCH|SDDS_AND)) {
      SDDS_SetError("resolveColumnNames: SDDS_SetColumnsOfInterest error");
      return 0;
    }
    if (!(column = SDDS_GetColumnNames(SDDSin, &columns)) || columns==0) {
      sprintf(buffer, "No match for column list: ");
      for (j=0; j<normRequest[i].sources; j++) {
        strcat(buffer, normRequest[i].source[j]);
        if (j!=normRequest[i].sources-1)
          strcat(buffer, ", ");
      }
      SDDS_SetError(buffer);
      return 0;
    }
    if (!(normSpec = SDDS_Realloc(normSpec, sizeof(*normSpec)*(normSpecs+columns)))) {
      SDDS_SetError("resolveColumnNames: Memory allocation failure");
      return 0;
    }
    for (j=0; j<columns; j++) {
      normSpec[j+normSpecs].source = column[j];
      normSpec[j+normSpecs].mode = normRequest[i].mode;
      normSpec[j+normSpecs].flags = normRequest[i].flags;
      normSpec[j+normSpecs].functionOf = NULL;
      if (normRequest[i].flags&FL_FUNCOF_GIVEN) {
        if (!SDDS_CopyString(&normSpec[j+normSpecs].functionOf, 
                             normRequest[i].functionOf)) {
          SDDS_SetError("resolveColumnNames: Memory allocation failure");
          return 0;
        }
      }
      normSpec[j+normSpecs].target = NULL;
      if (normRequest[i].flags&FL_SUFFIX_GIVEN) {
        sprintf(buffer, "%s%s", normSpec[j+normSpecs].source, 
                normRequest[i].suffix);
        if (!SDDS_CopyString(&normSpec[j+normSpecs].target, buffer)) {
          SDDS_SetError("resolveColumnNames: Memory allocation failure");
          return 0;
        } 
      }
      else
        normSpec[j+normSpecs].target = normSpec[j+normSpecs].source;
    }
    normSpecs += columns;
  }
  *normSpecRet = normSpec;
  *normSpecsRet = normSpecs;
  return 1;
}



