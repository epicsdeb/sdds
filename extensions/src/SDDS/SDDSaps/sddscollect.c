/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddscollect
 * purpose: collect data from many columns into a group of new columns
 * 
 * Michael Borland, 1996
 $Log: sddscollect.c,v $
 Revision 1.21  2006/12/14 22:21:57  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.20  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.19  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.18  2004/07/30 22:05:55  borland
 Added Units column to output to give the units of each row of values.

 Revision 1.17  2003/12/15 21:13:26  soliday
 Fixed issue when the exclude option is not used.

 Revision 1.16  2003/12/12 18:39:19  soliday
 Added the exlude option which can be used with the match option.

 Revision 1.15  2002/08/14 17:12:41  soliday
 Added Open License

 Revision 1.14  2001/01/10 19:35:33  soliday
 Standardized usage message.

 Revision 1.13  2000/06/20 13:09:59  borland
 Now correctly sets units when there is a mismatch of units in the input,
 even if -nowarning option is given.

 Revision 1.12  2000/03/31 23:13:45  emery
 Corrected the error printout for groups with different numbers.

 Revision 1.11  1999/07/26 19:44:57  borland
 Fixed bug introduced in last revision.  Affected prefix mode collection.

 Revision 1.10  1999/07/09 13:24:53  borland
 Fixed bug that occurs when the column names don't sort into the same
 order as the rootnames.  Fix works only for suffix-mode.  Need more
 work to fix for prefix and edit modes.

 Revision 1.9  1999/07/08 18:37:14  borland
 Added -nowarnings option.

 Revision 1.8  1999/05/25 19:06:09  soliday
 Removed compiler warning on linux.

 Revision 1.7  1998/12/16 21:24:12  borland
 Brought syntax up-to-date with latest version of SDDS_TransferAllParameters.

 Revision 1.6  1998/06/24 19:22:29  borland
 Now accepts -collect=match option in addition to prefix and suffix.
 This permits collecting columns with more general names.

 * Revision 1.5  1996/02/14  01:05:10  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.4  1996/01/31  15:26:52  borland
 * Fixed problem in sorting of column names; added diagnostic output for case
 * of a name mismatch.
 *
 * Revision 1.3  1996/01/17  23:29:16  borland
 * Replaced SDDS_GetColumn() call with SDDS_GetInternalColumn() call in a few
 * places in hopes of improving performance.
 *
 * Revision 1.2  1996/01/17  15:39:03  borland
 * Fixed problem that occured when input column name was shorter than part
 * name.  Added statement to set OriginalPage parameter value.
 *
 * Revision 1.1  1996/01/17  05:56:13  borland
 * First version of the program.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "SDDSutils.h"
#include "scan.h"
#include <ctype.h>

static char *USAGE = "sddscollect [-pipe=[input][,output]] \n\
[<input>] [<output>] [-nowarnings]\n\
-collect={suffix=<string>|prefix=<string>|match=<string>}[,column=<newName>][,editCommand=<string>][,exclude=<wildcard>]... \n\n\
-pipe         The standard SDDS toolkit pipe option.\n\
-collect      Specifies a suffix or prefix to use in selecting columns from\n\
              the input that will be collected into a new column in\n\
              the output.  Optionally specifies the name of the new\n\
              column, which defaults to the suffix or prefix.\n\n\
Program by Michael Borland.  (This is version 2, June 1998.)" ;

#define CLO_COLLECT 0
#define CLO_PIPE 1
#define CLO_NOWARNINGS 2
#define CLO_OPTIONS 3
static char *option[CLO_OPTIONS] = {
    "collect", "pipe", "nowarnings",
    };

typedef struct {
    char *part, *newColumn, *match, *editCommand, *exclude;
    char **oldColumn;
    void **data;
    long oldColumns, targetIndex, size;
    unsigned long flags;
    } COLLECTION;

typedef struct {
    char *name;
    void *data;
    long size, targetIndex;
    } NEW_PARAMETER;
 
#define COLLECTION_SUFFIX  0x0001U
#define COLLECTION_PREFIX  0x0002U
#define COLLECTION_COLUMN  0x0004U
#define COLLECTION_MATCH   0x0008U
#define COLLECTION_EDIT    0x0010U
#define COLLECTION_EXCLUDE 0x0020U

long InitializeOutput(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin, 
                      COLLECTION *collection, long collections,
                      NEW_PARAMETER **newParameter, long *newParameters,
                      char ***rootname, char ***units, long warnings);
void CollectAndWriteData(SDDS_DATASET *SDDSout, 
                         COLLECTION *collection, long collections,
                         NEW_PARAMETER *newParameter, long newParameters,
                         char **rootname, char **units,
                         long rootnames, long inputRow, long origPage);
void GetAndOrganizeData(SDDS_DATASET *SDDSin,
                        COLLECTION *collection, long collections,
                        NEW_PARAMETER *newParameter, long newParameters);
char **ConfirmMatchingColumns(COLLECTION *collection, long collections,
                              SDDS_DATASET *SDDSin, SDDS_DATASET *SDDSout, long *rootnames, 
                              char ***units, long warnings);


int main(int argc, char **argv)
{
    long iArg, ic;
    SDDS_DATASET SDDSin, SDDSout;
    SCANNED_ARG *scanned;
    unsigned long pipeFlags, flags;
    COLLECTION *collection;
    NEW_PARAMETER *newParameter;
    char **rootname, **units;
    long collections, newParameters, rootnames, rows, row, code;
    char *input, *output;
    long warnings;
    
    SDDS_RegisterProgramName(argv[0]);

    argc = scanargs(&scanned, argc, argv); 
    if (argc==1)
        bomb(NULL, USAGE);

    input = output = NULL;
    collection = NULL;
    collections = 0;
    pipeFlags = 0;
    warnings = 1;
    
    for (iArg=1; iArg<argc; iArg++) {
        if (scanned[iArg].arg_type==OPTION) {
            /* process options here */
            switch (match_string(scanned[iArg].list[0], option, CLO_OPTIONS, 0)) {
              case CLO_COLLECT:
                if (!(collection = SDDS_Realloc(collection, sizeof(*collection)*(collections+1))))
                    SDDS_Bomb("memory allocation failure");
                ic = collections;
                collection[ic].newColumn = collection[ic].part = 
                  collection[ic].match = collection[ic].editCommand = NULL;
                collection[ic].exclude = NULL;
                if (--scanned[iArg].n_items==0 || 
                    !scanItemList(&flags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                                  "suffix", SDDS_STRING, &collection[ic].part, 1, COLLECTION_SUFFIX,
                                  "prefix", SDDS_STRING, &collection[ic].part, 1, COLLECTION_PREFIX,
                                  "column", SDDS_STRING, &collection[ic].newColumn, 1, COLLECTION_COLUMN,
                                  "match", SDDS_STRING, &collection[ic].match, 1, COLLECTION_MATCH, 
                                  "editcommand", SDDS_STRING, &collection[ic].editCommand, 1, COLLECTION_EDIT, 
                                  "exclude", SDDS_STRING, &collection[ic].exclude, 1, COLLECTION_EXCLUDE, 
                                  NULL) ||
                    ((flags&COLLECTION_SUFFIX && flags&COLLECTION_PREFIX) ||
                     (flags&COLLECTION_SUFFIX && flags&COLLECTION_MATCH) ||
                     (flags&COLLECTION_PREFIX && flags&COLLECTION_MATCH)))
                    SDDS_Bomb("invalid -collect syntax");
                if (flags&COLLECTION_MATCH && 
                    (!(flags&COLLECTION_EDIT) || !(flags&COLLECTION_COLUMN))) 
                  SDDS_Bomb("invalid -collect syntax: must give editCommand and column with match");
                collection[ic].flags = flags;
                collections++;
                break;
              case CLO_PIPE:
                if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_NOWARNINGS:
                warnings = 0;
                break;
              default:
                fprintf(stderr, "invalid option seen: %s\n", scanned[iArg].list[0]);
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
                SDDS_Bomb("too many filenames");
            }
        }

    if (!collections)
        SDDS_Bomb("at least one -collect option must be given");
    
    processFilenames("sddscollect", &input, &output, pipeFlags, !warnings, NULL);

    if (!SDDS_InitializeInput(&SDDSin, input))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    rootnames = InitializeOutput(&SDDSout, output, &SDDSin, collection, collections,
                                 &newParameter, &newParameters, &rootname, &units, warnings);
    
    while ((code=SDDS_ReadPage(&SDDSin))>0) {
        if (!SDDS_StartPage(&SDDSout, rootnames) ||
            !SDDS_CopyParameters(&SDDSout, &SDDSin) || 
            !SDDS_CopyArrays(&SDDSout, &SDDSin))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if ((rows = SDDS_CountRowsOfInterest(&SDDSin))>0) {
            GetAndOrganizeData(&SDDSin, collection, collections, newParameter, newParameters);
            for (row=0; row<rows; row++) {
                CollectAndWriteData(&SDDSout, collection, collections, newParameter, newParameters, 
                                    rootname, units, rootnames, row, code);
                if (row!=rows-1 && !SDDS_StartPage(&SDDSout, rootnames))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
          }
        else if (!SDDS_WritePage(&SDDSout))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!code || !SDDS_Terminate(&SDDSout) || !SDDS_Terminate(&SDDSin))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return(0);
    }

void CollectAndWriteData(SDDS_DATASET *SDDSout, 
                         COLLECTION *collection, long collections,
                         NEW_PARAMETER *newParameter, long newParameters,
                         char **rootname, char **units, long rootnames, long inputRow, long origPage)
{
    long ic, ip, ir;
    if (rootnames) {
        if (!SDDS_SetColumn(SDDSout, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                            rootname, rootnames, "Rootname"))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (!SDDS_SetColumn(SDDSout, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                            units, rootnames, "Units"))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        for (ic=0; ic<collections; ic++) {
            for (ir=0; ir<rootnames; ir++) {
                if (!SDDS_SetRowValues(SDDSout, SDDS_PASS_BY_REFERENCE|SDDS_SET_BY_INDEX,
                                       ir, collection[ic].targetIndex,
                                       ((char*)(collection[ic].data[ir]))+inputRow*collection[ic].size,
                                       -1))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
            }
        }
    
    for (ip=0; ip<newParameters; ip++) {
        if (!SDDS_SetParameters(SDDSout, SDDS_PASS_BY_REFERENCE|SDDS_SET_BY_NAME,
                                newParameter[ip].name, 
                                ((char*)(newParameter[ip].data)+inputRow*newParameter[ip].size),
                                NULL))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    if (!SDDS_SetParameters(SDDSout, SDDS_PASS_BY_VALUE|SDDS_SET_BY_NAME,
                            "OriginalPage", origPage, NULL) ||
        !SDDS_WritePage(SDDSout))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }

void GetAndOrganizeData(SDDS_DATASET *SDDSin,
                        COLLECTION *collection, long collections,
                        NEW_PARAMETER *newParameter, long newParameters)
{
    long ic, ip, ii;
    for (ic=0; ic<collections; ic++) {
        for (ii=0; ii<collection[ic].oldColumns; ii++) {
            if (!(collection[ic].data[ii] = SDDS_GetInternalColumn(SDDSin, collection[ic].oldColumn[ii])))
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            }
        }
    for (ip=0; ip<newParameters; ip++) {
        if (!(newParameter[ip].data = SDDS_GetColumn(SDDSin, newParameter[ip].name)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
    }


long InitializeOutput(SDDS_DATASET *SDDSout, char *output, SDDS_DATASET *SDDSin, 
                      COLLECTION *collection, long collections,
                      NEW_PARAMETER **newParameter, long *newParameters,
                      char ***rootname, char ***units, long warnings)
{
  char **inputColumn, *partString;
  long partLength, *inputLength;
  int32_t inputColumns;
  short *inputUsed;
  long ic, ii, ip, inputsUsed, rootnames;
  char *matchString, *excludeString;
  
  partLength = 0;
  inputLength = NULL;

  if (!SDDS_InitializeOutput(SDDSout, SDDS_BINARY, 0, NULL, "sddscollect output", output) ||
      !SDDS_TransferAllParameterDefinitions(SDDSout, SDDSin, 0) ||
      !SDDS_TransferAllArrayDefinitions(SDDSout, SDDSin, 0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(inputColumn = SDDS_GetColumnNames(SDDSin, &inputColumns)) || inputColumns==0)
    SDDS_Bomb("No columns in input file");
  if (!(inputUsed = (short*)calloc(inputColumns, sizeof(*inputUsed))) ||
      !(inputLength = (long*)calloc(inputColumns, sizeof(*inputLength))))
    SDDS_Bomb("Memory allocation failure");
  for (ii=0; ii<inputColumns; ii++) 
    inputLength[ii] = strlen(inputColumn[ii]);

  inputsUsed = 0;
  matchString = NULL;
  excludeString = NULL;
  for (ic=0; ic<collections; ic++) {
    if (!collection[ic].newColumn)
      SDDS_CopyString(&collection[ic].newColumn, collection[ic].part);
    if ((partString = collection[ic].part))
      partLength = strlen(partString);
    if (collection[ic].match) {
      matchString = collection[ic].match;
      partLength = -1;
    }
    if (collection[ic].exclude) {
      excludeString = collection[ic].exclude;
    }
    collection[ic].oldColumn = NULL;
    collection[ic].oldColumns = 0;
    for (ii=0; ii<inputColumns; ii++) {
      if (inputUsed[ii])
        continue;
      if (partLength>=inputLength[ii])
        continue;
      if (matchString) {
        if (wild_match(inputColumn[ii], matchString)) {
          if ((excludeString == NULL) || (!wild_match(inputColumn[ii], excludeString))) {
            if (!(collection[ic].oldColumn = 
                  SDDS_Realloc(collection[ic].oldColumn, 
                               sizeof(*collection[ic].oldColumn)*(collection[ic].oldColumns+1)))) {
              SDDS_Bomb("Memory allocation failure");
            }
            collection[ic].oldColumn[collection[ic].oldColumns] = inputColumn[ii];
            inputUsed[ii] = 1;
            inputsUsed++;
            collection[ic].oldColumns++;
          }
        }
      }
      else if (collection[ic].flags&COLLECTION_PREFIX) {
        if (strncmp(partString, inputColumn[ii], partLength)==0) {
          if (!(collection[ic].oldColumn = 
                SDDS_Realloc(collection[ic].oldColumn, 
                             sizeof(*collection[ic].oldColumn)*(collection[ic].oldColumns+1)))) {
            SDDS_Bomb("Memory allocation failure");
          }
          collection[ic].oldColumn[collection[ic].oldColumns] = inputColumn[ii];
          inputUsed[ii] = 1;
          inputsUsed++;
          collection[ic].oldColumns++;
        }
      }
      else {
        if (strcmp(partString, inputColumn[ii]+inputLength[ii]-partLength)==0) {
          if (!(collection[ic].oldColumn = 
                SDDS_Realloc(collection[ic].oldColumn, 
                             sizeof(*collection[ic].oldColumn)*(collection[ic].oldColumns+1)))) {
            SDDS_Bomb("Memory allocation failure");
          }
          collection[ic].oldColumn[collection[ic].oldColumns] = inputColumn[ii];
          inputUsed[ii] = 1;
          inputsUsed++;
          collection[ic].oldColumns++;
        }
      }
    }
    if (!collection[ic].oldColumns && warnings)  {
      fprintf(stderr, "Warning (sddscollect): No columns in input for %s %s\n",
              collection[ic].flags&COLLECTION_PREFIX?"prefix":"suffix",
              collection[ic].part);
    }
    if (!(collection[ic].data = (void**)calloc(collection[ic].oldColumns,
                                               sizeof(*collection[ic].data))))
      SDDS_Bomb("Memory allocation failure");
  }

  if ((*newParameters=inputColumns-inputsUsed)) {
    *newParameter = (NEW_PARAMETER*)malloc(sizeof(**newParameter)*(*newParameters));
    for (ii=ip=0; ii<inputColumns; ii++) {
      if (inputUsed[ii])
        continue;
      if (!SDDS_CopyString(&(*newParameter)[ip].name, inputColumn[ii]) ||
          !SDDS_DefineParameterLikeColumn(SDDSout, SDDSin, inputColumn[ii], NULL) ||
          ((*newParameter)[ip].targetIndex=SDDS_GetParameterIndex(SDDSout, inputColumn[ii]))<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      (*newParameter)[ip].size 
        = SDDS_GetTypeSize(SDDS_GetParameterType(SDDSout, (*newParameter)[ip].targetIndex));
      ip++;
    }
  }

  *rootname = ConfirmMatchingColumns(collection, collections, SDDSin, SDDSout, &rootnames, 
                                     units, warnings);
  
  if (SDDS_DefineParameter(SDDSout, "OriginalPage", NULL, NULL, NULL, NULL, SDDS_LONG, NULL)<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (rootnames &&
      (SDDS_DefineColumn(SDDSout, "Rootname", NULL, NULL, NULL, NULL, SDDS_STRING, 0)<0 ||
       SDDS_DefineColumn(SDDSout, "Units", NULL, NULL, NULL, NULL, SDDS_STRING, 0)<0))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_WriteLayout(SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  return rootnames;
}


char **ConfirmMatchingColumns(COLLECTION *collection, long collections,
                              SDDS_DATASET *SDDSin, SDDS_DATASET *SDDSout,
                              long *rootnames, char ***units, long warnings)
{
  long ic, ip, ii, partLength;
  char **rootname, editBuffer[1024];
  char saveChar;
  
  partLength = 0;
  rootname = NULL;
  *units = NULL;
  *rootnames = 0;
  if (!collections)
    return NULL;
  for (ic=0; ic<collections; ic++) {
    if (!collection[ic].oldColumns)
      continue;
    if (collection[ic].part)
      partLength = strlen(collection[ic].part);
    if (collection[ic].part && collection[ic].flags&COLLECTION_SUFFIX) {
      /* sort collection according to truncated string */
      saveChar = collection[ic].part[0];
      for (ip=0; ip<collection[ic].oldColumns; ip++)
        collection[ic].oldColumn[ip][strlen(collection[ic].oldColumn[ip])-partLength] = 0;
      qsort(collection[ic].oldColumn, collection[ic].oldColumns, 
            sizeof(*collection[ic].oldColumn), string_cmpasc);
      /* now restore the strings (column names) to original form */
      for (ip=0; ip<collection[ic].oldColumns; ip++)
        collection[ic].oldColumn[ip][strlen(collection[ic].oldColumn[ip])] = saveChar;
    }
    else
      qsort(collection[ic].oldColumn, collection[ic].oldColumns, 
            sizeof(*collection[ic].oldColumn), string_cmpasc);
    if (rootname)
      continue;
    *rootnames = collection[ic].oldColumns;
    if (!(rootname = (char**)malloc(sizeof(*rootname)*(*rootnames))))
      SDDS_Bomb("Memory allocation failure");
    if (!(*units = (char**)malloc(sizeof(**units)*(*rootnames))))
      SDDS_Bomb("Memory allocation failure");
    for (ip=0; ip<(*rootnames); ip++) {
      if (SDDS_GetColumnInformation(SDDSin, "units",  *units+ip,
                                    SDDS_GET_BY_NAME, collection[ic].oldColumn[ip])!=SDDS_STRING)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      if (collection[ic].flags&COLLECTION_EDIT) {
        strcpy(editBuffer, collection[ic].oldColumn[ip]);
        if (!edit_string(editBuffer, collection[ic].editCommand)) {
          SDDS_Bomb("Problem editing column name.");
        }
        SDDS_CopyString(rootname+ip, editBuffer);
      }
      else if (collection[ic].flags&COLLECTION_PREFIX) {
        SDDS_CopyString(rootname+ip, collection[ic].oldColumn[ip]+partLength);
      }
      else {
        SDDS_CopyString(rootname+ip, collection[ic].oldColumn[ip]);
        rootname[ip][strlen(rootname[ip])-partLength] = 0;
      }
    }
  }
  if (!(*rootnames))
    return NULL;
  
  for (ic=0; ic<collections; ic++) {
    if (!collection[ic].oldColumns)
      continue;
    if (collection[ic].oldColumns!=(*rootnames)) {
      fprintf(stderr, 
              "Error (sddcollect): groups have different numbers of members\n");
      for (ip=0; ip<collections; ip++) {
        fprintf(stderr, "%ld in %s\n",
                collection[ip].oldColumns, 
                collection[ip].part?collection[ip].part:collection[ip].match);
      }
      exit(1);
    }
    if (collection[ic].flags&COLLECTION_MATCH)
      continue;
    for (ip=0; ip<collection[ic].oldColumns; ip++)
      if (strstr(collection[ic].oldColumn[ip], rootname[ip])==NULL) {
        fprintf(stderr, 
                "Error (sddscollect): mismatch with rootname %s for column %s in group %s\n",
                rootname[ip], collection[ic].oldColumn[ip], 
                collection[ic].part?collection[ic].part:collection[ic].match);
        /* about to bomb here, so it's okay to reuse these indices */
        for (ic=0; ic<collections; ic++) {
          fprintf(stderr, "Group %s (%ld):\n", 
                  collection[ic].part?collection[ic].part:collection[ic].match,
                  ic);
          for (ip=0; ip<collection[ic].oldColumns; ip++)
            fprintf(stderr, "  old column[%ld] = %s\n", ip, collection[ic].oldColumn[ip]);
        }
        exit(1);
      }
  }
  
  
  for (ic=0; ic<collections; ic++) {
    char *units;
    int32_t type;
    long unitsMismatch;
    if (!collection[ic].oldColumns)
      continue;
    if (!SDDS_TransferColumnDefinition(SDDSout, SDDSin, collection[ic].oldColumn[0],
                                       collection[ic].newColumn) ||
        (collection[ic].targetIndex =
         SDDS_GetColumnIndex(SDDSout, collection[ic].newColumn))<0 ||
        SDDS_GetColumnInformation(SDDSout, "units", &units, SDDS_GET_BY_NAME, 
                                  collection[ic].newColumn)!=SDDS_STRING ||
        SDDS_GetColumnInformation(SDDSout, "type", &type, SDDS_GET_BY_NAME,
                                  collection[ic].newColumn)!=SDDS_LONG )
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    collection[ic].size = SDDS_GetTypeSize(type);
    unitsMismatch = 0;
    for (ii=1; ii<collection[ic].oldColumns; ii++) {
      if (SDDS_CheckColumn(SDDSin, collection[ic].oldColumn[ii], NULL, type, stderr)==
          SDDS_CHECK_WRONGTYPE) {
        fprintf(stderr, "Error (sddscollect): inconsistent data types for suffix/prefix/match %s\n",
                collection[ic].part?collection[ic].part:collection[ic].match);
        exit(1);
      }
      if (SDDS_CheckColumn(SDDSin, collection[ic].oldColumn[ii], units, type, NULL)
          ==SDDS_CHECK_WRONGUNITS)
        unitsMismatch = 1;
    }
    if (unitsMismatch) {
      if (warnings)
        fprintf(stderr, "Warning (sddscollect): inconsistent units for suffix/prefix %s\n",
                collection[ic].part);
      if (!SDDS_ChangeColumnInformation(SDDSout, "units", "?", SDDS_BY_NAME, 
                                        collection[ic].newColumn))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }

  return rootname;
}

