/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsseparate
 * purpose: takes an SDDS file with many columns in one page and
 *          separates the columns onto multiple pages, under common names.
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2006/12/14 22:22:00  soliday
 * Updated a bunch of programs because SDDS_SaveLayout is now called by
 * SDDS_WriteLayout and it is no longer required to be called directly.
 * Also the AutoCheckMode is turned off by default now so I removed calls to
 * SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 * the programmer to turn it on in new programs until debugging is completed
 * and then remove the call to SDDS_SetAutoCheckMode.
 *
 * Revision 1.7  2005/11/07 21:48:11  soliday
 * Updated to remove Linux compiler warnings.
 *
 * Revision 1.6  2005/11/04 22:46:18  soliday
 * Updated code to be compiled by a 64 bit processor.
 *
 * Revision 1.5  2004/01/26 02:36:52  borland
 * Removed initialization for variable defined in a for loop; the compiler
 * is incorrectly initializing the variable on each pass through the loop.
 *
 * Revision 1.4  2003/09/02 19:16:07  soliday
 * Cleaned up code for Linux.
 *
 * Revision 1.3  2003/02/19 18:10:46  soliday
 * Fixed nameing conflict with Borland C
 *
 * Revision 1.2  2002/08/14 17:12:54  soliday
 * Added Open License
 *
 * Revision 1.1  2001/10/28 01:06:50  borland
 * First version.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

typedef struct {
  char *newName, **usersOldName, **oldName, *parameterName;
  long usersOldNames;
  int32_t oldNames;
  } GROUPS;

#define SET_GROUP 0
#define SET_COPY 1
#define SET_PIPE 2
#define N_OPTIONS 3

char *option[N_OPTIONS] = {
  "group", "copy", "pipe",
};

static char *USAGE="sddsseparate [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-group=<newName>,<listOfOldNames>] [-copy=<listOfNames>] \
\n\
Reorganizes the column data in the input so that data from different\n\
columns ends up on different pages.  For each -group option, a column\n\
is created in the output that contains data from the columns <listOfOldNames>\n\
on sequential pages.  Columns named with the -copy option are duplicated on\n\
each page.\n\n\
Program by Michael Borland.  (This is version 1, October 2001.)\n";

int main(int argc, char **argv)
{
  char *input, *output;
  char **copyColumnName, **usersCopyColumnName;
  GROUPS *group;
  int32_t copyColumns;
  long usersCopyColumns, groups;
  long iArg, i, rows, readCode, items;
  unsigned long flags, pipeFlags;
  SCANNED_ARG *scArg;
  SDDS_DATASET SDDSin, SDDSout;

  SDDS_RegisterProgramName(argv[0]);

  argc = scanargs(&scArg, argc, argv); 
  if (argc<2) 
    bomb(USAGE, NULL);
  output = input = NULL;
  flags = pipeFlags = 0;
  group = NULL;
  copyColumnName = usersCopyColumnName = NULL;
  usersCopyColumns = copyColumns = groups = 0;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scArg[iArg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scArg[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
	if (!processPipeOption(scArg[iArg].list+1, scArg[iArg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      case SET_GROUP:
        if ((items = scArg[iArg].n_items-1)<2)
          SDDS_Bomb("invalid -group syntax");
        if (!(group = SDDS_Realloc(group, sizeof(*group)*(groups+1))) ||
            !SDDS_CopyString(&group[groups].newName, scArg[iArg].list[1]) ||
            !(group[groups].usersOldName = SDDS_Malloc(sizeof(*group[groups].usersOldName)*
                                                   (group[groups].usersOldNames=items-1))) ||
            !SDDS_CopyStringArray(group[groups].usersOldName, scArg[iArg].list+2,
                                  group[groups].usersOldNames))
          SDDS_Bomb("memory allocation failure");
        group[groups].oldName = NULL;
        group[groups].oldNames = 0;
        groups++;
        break;
      case SET_COPY:
        if (usersCopyColumns)
          SDDS_Bomb("give -copy only once");
        if ((usersCopyColumns=scArg[iArg].n_items-1)<1) 
          SDDS_Bomb("invalid -copy syntax");
        if (!(usersCopyColumnName = SDDS_Malloc(sizeof(*usersCopyColumnName)*usersCopyColumns)) ||
            !SDDS_CopyStringArray(usersCopyColumnName, scArg[iArg].list+1, usersCopyColumns))
          SDDS_Bomb("memory allocation failure");
        break;
      default:
	fprintf(stderr, "error: unknown/ambiguous option: %s\n", scArg[iArg].list[0]);
	exit(1);
	break;
      }
    }
    else {
      if (!input)
	input = scArg[iArg].list[0];
      else if (!output)
	output = scArg[iArg].list[0];
      else
	SDDS_Bomb("too many filenames seen");
    }
  }
  if (groups==0)
    SDDS_Bomb("no groups defined");
  
  processFilenames("sddsseparate", &input, &output, pipeFlags, 0, NULL);
  
  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (usersCopyColumns) {
    SDDS_SetColumnFlags(&SDDSin, 0);
    for (i=0; i<usersCopyColumns; i++)
      SDDS_SetColumnsOfInterest(&SDDSin, SDDS_MATCH_STRING, usersCopyColumnName[i], SDDS_OR);
    if (!(copyColumnName = SDDS_GetColumnNames(&SDDSin, &copyColumns)) || copyColumns==0)
      SDDS_Bomb("no match for copy columns");
  }
  
  for (i=0; i<groups; i++) {
    long j, type=0;
    SDDS_SetColumnFlags(&SDDSin, 0);
    
    for (j=0; j<group[i].usersOldNames; j++)
      SDDS_SetColumnsOfInterest(&SDDSin, SDDS_MATCH_STRING, group[i].usersOldName[j], SDDS_OR);
    if (!(group[i].oldName = SDDS_GetColumnNames(&SDDSin, &group[i].oldNames))) {
      fprintf(stderr, "No match for group %s (sddsseparate)\n",
              group[i].newName);
      exit(1);
    }
    if (i && group[i-1].oldNames!=group[i].oldNames) {
      fprintf(stderr, "Group %s comprises %" PRId32 " columns, whereas the last group comprises %" PRId32 " (sddsseparate)\n",
              group[i].newName, group[i].oldNames, group[i-1].oldNames);
      exit(1);
    }
    type = SDDS_GetColumnType(&SDDSin,  SDDS_GetColumnIndex(&SDDSin, group[i].oldName[0]));
    for (j=1; j<group[i].oldNames; j++) {
      if (type != SDDS_GetColumnType(&SDDSin, SDDS_GetColumnIndex(&SDDSin, group[i].oldName[j]))) {
	fprintf(stderr, "Inconsistent data types in group %s (sddsseparate)\n", group[i].newName);
	fprintf(stderr, "First inconsistent column is %s\n", group[i].oldName[j]);
	exit(1);
      }
    }
  }
  
  if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 0, NULL, NULL, output) ||
      !SDDS_TransferAllParameterDefinitions(&SDDSout, &SDDSin, 0))
    SDDS_Bomb("problem initializing output file");
  for (i=0; i<copyColumns; i++)
    if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, copyColumnName[i], NULL))
      SDDS_Bomb("problem transferring copy column definitions to output file");
  for (i=0; i<groups; i++)  {
    char *name;
    if (!SDDS_TransferColumnDefinition(&SDDSout, &SDDSin, group[i].oldName[0], group[i].newName)) {
      fprintf(stderr, "Problem transferring column %s as %s to output file (sddsseparate)\n",
              group[i].oldName[0],  group[i].newName);
      exit(1);
    }
    if (!(group[i].parameterName = SDDS_Malloc(sizeof(*name)*(strlen(group[i].newName)+100))))
      SDDS_Bomb("memory allocation failure");
    sprintf(group[i].parameterName, "%sSourceColumn", group[i].newName);
    if (!SDDS_DefineSimpleParameter(&SDDSout, group[i].parameterName, NULL, SDDS_STRING))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_WriteLayout(&SDDSout)) 
    SDDS_Bomb("problem writing layout to output file");
  
  while ((readCode=SDDS_ReadPage(&SDDSin))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDSin))<0) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!rows)
      continue;
    for (i=0; i<group[0].oldNames; i++) {
      long ic, ig;
      if (!SDDS_StartPage(&SDDSout, rows) || !SDDS_CopyParameters(&SDDSout, &SDDSin))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      for (ic=0; ic<copyColumns; ic++) {
        void *data;
        if (!(data = SDDS_GetInternalColumn(&SDDSin, copyColumnName[ic])) ||
            !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_NAME, data, rows, copyColumnName[ic]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      for (ig=0; ig<groups; ig++) {
        void *data;
        if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                                group[ig].parameterName, 
                                group[ig].oldName[i], NULL) ||
            !(data = SDDS_GetInternalColumn(&SDDSin, group[ig].oldName[i])) ||
            !SDDS_SetColumn(&SDDSout, SDDS_SET_BY_NAME, data, rows, group[ig].newName))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (!SDDS_WritePage(&SDDSout))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  
  if (!SDDS_Terminate(&SDDSin)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  return 0;
}
