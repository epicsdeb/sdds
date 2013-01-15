/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssortcolumn
 * purpose: sorts the columns of an SDDS file 
 *
 * Hairong Shang, 2003
 $Log: not supported by cvs2svn $
 Revision 1.3  2009/04/06 18:27:14  shang
 fixed a bug sorting columns by provided order in a file.

 Revision 1.2  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.1  2003/07/09 18:20:59  shang
 first version

 sort the columns of an SDDS file in specified order.
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#if defined(_WIN32)
#include <process.h>
#define pid_t int
#else
#if defined(linux)
#include <sys/types.h>
#endif
#include <unistd.h>
#endif

#define SET_PIPE 0
#define SET_SORTLIST 1
#define SET_DECREASING 2
#define SET_BPMORDER 3
#define SET_SORTWITH 4
#define N_OPTIONS 5

char *option[N_OPTIONS] = {
  "pipe","sortList","decreasing","bpmOrder","sortWith",
    } ;

char *USAGE = "sddssortcolumn [<SDDSinput>] [<SDDSoutput>] [-pipe=[input][,output]]\n\
[-sortList=<list of columns in order>] [-decreasing] \n\
[-bpmOrder] [-sortWith=<filename>,column=<string>]\n\n\
-sortList             specifiy the order of column names in a list. \n\
-sortWith=<filename>,column=<string> \n\
                      sort the columns of input by the order in the column of <filename> \n\
                      provided by sortWith option. It overwrites other sorting order. \n\
-bpmOrder             sort the columns by the assumed bpm position in storage ring. \n\
-decreasing           sort the columns in decreasing order, default is increasing.\n\
rearrange the columns of an SDDS input file in specified order. \n\n\
Program by Hairong Shang\n";

static char **ColumnName;
static long increasing,bpmOrder;
static int32_t columns;

long getBPMSubOrder(char *bpmName)
{
  long suborder;
  /*suborder of bpm: A:P0,A:P1,A:P2,A:P3,A:P4,A:P5, B:P5,B:P4,B:P3,B:P2,B:P1,B:P0,BM:P1, BM:P2,ID:P1, ID:P2 */
  if (wild_match(bpmName,"*A:P0*"))
    suborder=1;
  else if (wild_match(bpmName,"*A:P1*"))
    suborder=2;
  else if (wild_match(bpmName,"*A:P2*"))
    suborder=3;
  else if (wild_match(bpmName,"*A:P3*"))
    suborder=4;
  else if (wild_match(bpmName,"*A:P4*"))
    suborder=5;
  else if (wild_match(bpmName,"*A:P5*"))
    suborder=6;
  else if (wild_match(bpmName,"*B:P5*"))
    suborder=7;
  else if (wild_match(bpmName,"*B:P4*"))
    suborder=8;
  else if (wild_match(bpmName,"*B:P3*"))
    suborder=9;
  else if (wild_match(bpmName,"*B:P2*"))
    suborder=10;
  else if (wild_match(bpmName,"*B:P1*"))
    suborder=11;
  else if (wild_match(bpmName,"*B:P0*"))
    suborder=12;
  else if (wild_match(bpmName,"*BM:P1*"))
    suborder=13;
  else if (wild_match(bpmName,"*BM:P2*"))
    suborder=14;
  else if (wild_match(bpmName,"*ID:P1*"))
    suborder=15;
  else if (wild_match(bpmName,"*ID:P2*"))
    suborder=16;
  else
    suborder=17;
  return suborder;
}


#if defined(SOLARIS) || defined(LINUX) || defined(_WIN32)
int CompareStrings(const void *vindex1, const void *vindex2)
#else
long CompareStrings(const void *vindex1, const void *vindex2)
#endif
{
  static long index1, index2;
  long comparison,sector1,sector2,subsector1,subsector2;
  /*type: 0: A:P, 1 B:P, 2 BM, 3 ID bpm */
  index1 = *(long*)vindex1;
  index2 = *(long*)vindex2;
  if (bpmOrder) {
    if (sscanf(ColumnName[index1],"S%ld",&sector1)!=1)
      sector1=0;
    if (sscanf(ColumnName[index2],"S%ld",&sector2)!=1)
      sector2=0;
    if (sector1==0 && sector2==0) 
      comparison=strcmp(ColumnName[index1], ColumnName[index2]);
    else {
      if (sector1>sector2)
        comparison=1;
      else if (sector1<sector2)
        comparison=-1;
      else {
        subsector1=getBPMSubOrder(ColumnName[index1]);
        subsector2=getBPMSubOrder(ColumnName[index2]);
        if (subsector1>subsector2)
          comparison=1;
        else if (subsector1<subsector2)
          comparison=-1;
        else
          comparison=0;
      }
    }
  } else
    comparison=strcmp(ColumnName[index1], ColumnName[index2]);
  if (!increasing)
    comparison=-comparison;
  return comparison;
}

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output,SDDS_sort;
  char *input, *output;
  char **sortList, **parameterName;
  char *sortFile, *sortColumn, **sortedColumn;
  long i_arg, sortLists=0,i,*sort_column_index, index,count,rows;
  SCANNED_ARG *s_arg;
  long tmpfile_used, tmpfileForInternalPipe;
  int32_t parameters=0;
  unsigned long pipeFlags,dummyflags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
    bomb(NULL, USAGE);
  sort_column_index=NULL;
  sortFile=sortColumn=NULL;
  ColumnName=NULL;
  input = output = NULL;
  sortList=NULL;
  tmpfile_used = tmpfileForInternalPipe=0;
  pipeFlags = 0;
  increasing=1;
  bpmOrder=0;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_DECREASING:
        increasing=0;
        break;
      case SET_BPMORDER:
        bpmOrder=1;
        break;
      case SET_SORTLIST:
        sortLists=s_arg[i_arg].n_items-1;
        sortList=(char**)malloc(sizeof(*sortList)*sortLists);
        for (i=0;i<sortLists;i++)
          SDDS_CopyString(&sortList[i],s_arg[i_arg].list[i+1]);
        break;
      case SET_SORTWITH:
        if (s_arg[i_arg].n_items!=3)
          SDDS_Bomb("Invalid -sortWith option given!");
        sortFile=s_arg[i_arg].list[1];
        s_arg[i_arg].n_items=1;
        if (!scanItemList(&dummyflags,s_arg[i_arg].list+2,&s_arg[i_arg].n_items,0,
                           "column",SDDS_STRING,&sortColumn,1,0,NULL) || !sortColumn)
          SDDS_Bomb("invalid -sortWith syntax/values");
        s_arg[i_arg].n_items=3;
        break;
      default:
        fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
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
  processFilenames("sddssort", &input, &output, pipeFlags, 0, &tmpfile_used);
  
  if (!SDDS_InitializeInput(&SDDS_input, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_InitializeOutput(&SDDS_output,SDDS_BINARY,1,NULL,NULL,output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  ColumnName=(char**)SDDS_GetColumnNames(&SDDS_input,&columns);
  parameterName=(char**)SDDS_GetParameterNames(&SDDS_input,&parameters);
  for (i=0;i<parameters;i++)
    if (!SDDS_TransferParameterDefinition(&SDDS_output, &SDDS_input,
                                          parameterName[i],parameterName[i]))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  sort_column_index=(long*)malloc(sizeof(*sort_column_index)*columns);
  for (i=0; i<columns; i++)
    sort_column_index[i] = i;
  count=0;
  if (sortFile && sortColumn) {
    if (sortList && sortLists) {
      for (i=0;i<sortLists;i++)
        free(sortList[i]);
      free(sortList);
    }
    sortLists=0;
    sortList=NULL;
    if (!SDDS_InitializeInput(&SDDS_sort, sortFile)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (SDDS_ReadPage(&SDDS_sort)<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!(sortLists=SDDS_CountRowsOfInterest(&SDDS_sort)))
      SDDS_Bomb("Zero rows found in sortWith file.\n");
    if (!(sortList=(char**)SDDS_GetColumn(&SDDS_sort,sortColumn)))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_Terminate(&SDDS_sort))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
   
  }
  count=0;
  sortedColumn=(char**)malloc(sizeof(*sortedColumn)*columns);
  if (sortList) {
    for (i=0;i<sortLists;i++) {
      if ((index=match_string(sortList[i],ColumnName,columns,EXACT_MATCH))>=0) {
        sortedColumn[count] = sortList[i];
        count++;
      }
    }
    for (i=0;i<columns;i++)
      if (match_string(ColumnName[i],sortList,sortLists,EXACT_MATCH)<0) {
        sortedColumn[count] = ColumnName[i];
        count++;
      }   
  } else {
    qsort((void*)sort_column_index, columns, sizeof(*sort_column_index), CompareStrings);
    for (i=0;i<columns;i++) {
      index=sort_column_index[i];
      sortedColumn[i] = ColumnName[index];
    }
  }
  for (i=0;i<columns;i++) {
    if (!SDDS_TransferColumnDefinition(&SDDS_output,&SDDS_input,
                                       sortedColumn[i],sortedColumn[i]))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_WriteLayout(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
  
  while (SDDS_ReadPage(&SDDS_input)>0) {
    rows = SDDS_CountRowsOfInterest(&SDDS_input);
    if (!SDDS_StartPage(&SDDS_output, rows)) {
      SDDS_SetError("Problem starting output page");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_CopyParameters(&SDDS_output, &SDDS_input))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_CopyColumns(&SDDS_output, &SDDS_input)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_WritePage(&SDDS_output)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (tmpfile_used && !replaceFileAndBackUp(input, output)) 
    exit(1);
  if (parameters) {
    for (i=0;i<parameters;i++)
      free(parameterName[i]);
    free(parameterName);
  }
  if (columns) {
    for (i=0;i<columns;i++) {
      free(ColumnName[i]);
    }
    free(ColumnName);
  }
  if (sortLists) {
    for (i=0;i<sortLists;i++)
      free(sortList[i]);
    free(sortList);
  }
  if (sortColumn)
    free(sortColumn);
  
  free(sortedColumn);
  free(sort_column_index);
  free_scanargs(&s_arg,argc);
  
  return(0);
}

