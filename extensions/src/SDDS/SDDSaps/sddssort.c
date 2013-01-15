/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssort
 * purpose: sorts an SDDS data set by the values in one or more
 *          columns
 *
 * Michael Borland, 1994
 $Log: not supported by cvs2svn $
 Revision 1.31  2007/04/10 16:50:56  shang
 added reading and writing constraints violation value for non-dominated sort

 Revision 1.30  2007/04/09 19:58:47  shang
 added considering constraints violation for non-dominated sorting.

 Revision 1.29  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.28  2006/10/31 16:16:35  soliday
 Fixed issue on 64 bit machines.

 Revision 1.27  2005/11/07 21:48:11  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.26  2005/11/04 22:46:18  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.25  2005/05/11 19:46:54  shang
 added maximize/minimize option to columm options for non dominated sorting

 Revision 1.24  2005/05/06 21:10:03  shang
 added non dominated sorting feature

 Revision 1.23  2003/12/19 19:53:43  soliday
 Added the -numericHigh option to change the method of string comparison.
 Numeric characters are ranked higher than other characters. It also
 ranks numeric character sets with fewer characters below those with more.

 Revision 1.22  2003/09/02 19:16:07  soliday
 Cleaned up code for Linux.

 Revision 1.21  2003/07/22 20:01:16  soliday
 Added support for Kylix.

 Revision 1.20  2002/08/14 17:12:55  soliday
 Added Open License

 Revision 1.19  2002/08/13 20:03:41  soliday
 Added include file for Linux

 Revision 1.18  2002/01/30 21:29:44  borland
 Updated version number.

 Revision 1.17  2002/01/03 21:43:43  soliday
 Fixed problem on WIN32 with getpid.

 Revision 1.16  2001/11/30 15:23:17  borland
 Fixed code for file that is opened on an internal pipe.

 Revision 1.15  2001/11/28 23:53:18  shang
 added -parameter feature to sort the pages by parameter

 Revision 1.14  2001/01/10 19:35:48  soliday
 Standardized usage message.

 Revision 1.13  1999/09/28 15:38:59  soliday
 Added SDDS_Terminate at the end.

 Revision 1.12  1999/07/09 15:10:44  soliday
 Changed defined(WIN32) to defined(_WIN32)

 Revision 1.11  1999/05/25 19:16:37  soliday
 Removed compiler warning on linux.

 Revision 1.10  1999/04/17 01:38:19  borland
 Much faster version using a second array of indices to eliminate an N^2 search.

 Revision 1.9  1999/01/06 19:54:57  borland
 Fixed the version number in the usage message.

 Revision 1.8  1996/08/27 18:27:05  borland
 Corrected initialization problem for array keeping counts of duplicate rows.

 * Revision 1.7  1996/06/25  04:10:35  borland
 * Added 'count' qualifier to -unique option, provide count of the number
 * of identical rows found.
 *
 * Revision 1.6  1995/12/14  23:45:52  borland
 * Fixed problem in reindexing loop that resulted in poor performance; basically,
 * added a break statement.
 *
 * Revision 1.5  1995/11/22  00:40:01  borland
 * Fixed memory leak in comparison function that occured for string data.
 *
 * Revision 1.4  1995/11/13  15:57:11  borland
 * Added conditional to give int type to sort comparison function under
 * Solaris.
 *
 * Revision 1.3  1995/09/06  14:57:15  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "non_dominated_sort.h"
#if defined(_WIN32)
#include <process.h>
#define pid_t int
#else
#if defined(linux)
#include <sys/types.h>
#endif
#include <unistd.h>
#endif

#define SET_COLUMN 0
#define SET_PARAMETER 1
#define SET_NOWARNINGS 2
#define SET_PIPE 3
#define SET_UNIQUE 4
#define SET_NUMERICHIGH 5
#define SET_NON_DOMINATE_SORT 6
#define N_OPTIONS 7

char *option[N_OPTIONS] = {
  "column","parameter", "nowarnings", "pipe", "unique", "numerichigh", "nonDominateSort",
};

char *USAGE = "sddssort [<SDDSinput>] [<SDDSoutput>] [-pipe=[input][,output]]\n\
[-column=<name>[,{increasing|decreasing}|{minimize|maximize}]...] [-unique[=count]]\n\
[-nowarnings] [-parameter=<name>[,{increasing|decreasing}]...]\n\
[-numericHigh] [-nonDominateSort] \n\
-column=<name>[,{increasing|decreasing}|{maximize|minimize}] \n\
        increasing|descreasing is the sorting direction for regular sorting.\n\
        minimize|maximize is for non-dominated sorting, default is minimize;\n\
              if maximize is specified the maximum value will have the smallest rank,\n\
              otherwise, the minimum value will have the smallest rank.\n\
-nonDominateSort   perform non-dominated-sort when multiply sorting columns supplied. \n\
                   non-dominated-sort only works for numeric columns.\n\
For regular sorting, \n\
        any number of sort columns may be given, resulting in subsorts by each\n\
        successive column in the list.\n\
If -unique is given, \"duplicate\" rows are eliminated, where a duplicate\n\
is any row that matches another row in the sort columns.  If the count\n\
qualifer is given, a count of the number of identical is supplied in\n\
a column called IdenticalCount.\n\
If the -numericHigh option is given it will rank the numeric characters\n\
higher than other characters in a string comparison. It also ranks numeric\n\
character sets with fewer characters below numeric character sets with more\n\
characters.\n\n\
Program by Michael Borland. ("__DATE__")\n";

typedef struct {
    char *name;
    long index, type, decreasing_order, maximize_order;
    double *data;
    } SORT_REQUEST ;

static char *order_mode[4] = {
    "increasing", "decreasing", "minimize", "maximize",
        } ;
long SDDS_SortRows(SDDS_DATASET *SDDS_dataset, SORT_REQUEST *xsort_request, long xsort_requests, long non_dominate_sort);
long SDDS_UnsetDuplicateRows(SDDS_DATASET *SDDS_dataset, SORT_REQUEST *xsort_request, 
  long xsort_requests, long provideIdenticalCount);

long SDDS_SortAll(SDDS_DATASET *SDDS_input,SDDS_DATASET *SDDS_output,
    SORT_REQUEST *xsort_request,long xsort_requests,SORT_REQUEST *xsort_parameter,
    long xsort_parameters,long uniqueRows,long provideIdenticalCount,long pipeFlags, long non_dominate_sort);

double *read_constr_violation(SDDS_DATASET *SDDS_dataset);

long numericHigh=0, constDefined=0; 

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output,SDDS_tmp;
  char *input, *output;
  long i_arg, non_dominate_sort=0;
  SCANNED_ARG *s_arg;
  
  long tmpfile_used, sort_requests, noWarnings, uniqueRows, provideIdenticalCount,tmpfileForInternalPipe;
  long sort_parameters;
  SORT_REQUEST *sort_request, *sort_parameter;
  unsigned long pipeFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
    bomb(NULL, USAGE);

  input = output = NULL;
  tmpfile_used = sort_requests = noWarnings =sort_parameters=tmpfileForInternalPipe=0;
  sort_request = sort_parameter=NULL;
  pipeFlags = 0;
  uniqueRows = provideIdenticalCount = 0;
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_NON_DOMINATE_SORT:
        non_dominate_sort=1;
        break;
      case SET_COLUMN:
        if (s_arg[i_arg].n_items<2 || s_arg[i_arg].n_items>3)
          SDDS_Bomb("invalid -column syntax");
        sort_request = trealloc(sort_request, sizeof(*sort_request)*(sort_requests+1));
        sort_request[sort_requests].name = s_arg[i_arg].list[1];
        sort_request[sort_requests].maximize_order = 0;
        sort_request[sort_requests].decreasing_order = 0;
        if (s_arg[i_arg].n_items==3) {
          switch (match_string(s_arg[i_arg].list[2], order_mode, 4, 0)) {
          case 0:
            break;
          case 1:
            sort_request[sort_requests].decreasing_order = 1;
            break;
          case 2:
            break;
          case 3:
            sort_request[sort_requests].maximize_order = 1;
            break;
          default:
            fprintf(stderr,"unknown sort order specified--give 'increasing' or 'decreasing' for dominated sorting\n or'maximize' or 'minimize' for non-dominated-sorting.\n");
            exit(1);
            break;
          }
        }
        sort_requests++;
        break;
      case SET_PARAMETER:
        if (s_arg[i_arg].n_items<2 || s_arg[i_arg].n_items>3)
          SDDS_Bomb("invalid -parameter syntax");
        sort_parameter = trealloc(sort_parameter, sizeof(*sort_parameter)*(sort_parameters+1));
        sort_parameter[sort_parameters].name = s_arg[i_arg].list[1];
        if (s_arg[i_arg].n_items==3) {
          if ((sort_parameter[sort_parameters].decreasing_order=
               match_string(s_arg[i_arg].list[2], order_mode, 2, 0))<0)
            SDDS_Bomb("unknown sort order specified--give 'increasing' or 'decreasing'");
        }
        else
          sort_parameter[sort_parameters].decreasing_order = 0;
        sort_parameters++;
        break;  
      case SET_NOWARNINGS:
        noWarnings = 1;
        break;
      case SET_NUMERICHIGH:
        numericHigh = 1;
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_UNIQUE:
        uniqueRows = 1;
        if (s_arg[i_arg].n_items>1) {
          str_tolower(s_arg[i_arg].list[1]);
          if (s_arg[i_arg].n_items>2 || 
              strncmp("count", s_arg[i_arg].list[1],
                      strlen(s_arg[i_arg].list[1]))!=0)
            SDDS_Bomb("invalid -unique syntax");
          provideIdenticalCount = 1;
        }
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
  
  if (!sort_requests && !sort_parameters)
    SDDS_Bomb("No sorting requests!");
  processFilenames("sddssort", &input, &output, pipeFlags, noWarnings, &tmpfile_used);
  if (!SDDS_InitializeInput(&SDDS_input, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (sort_requests<=1)
    non_dominate_sort=0;
  
  if (SDDS_input.layout.popenUsed) {
    /* SDDS library has opened the file using a command on a pipe, usually for
     * decompression in the absence of the zlib library.
     */
    pid_t pid;
    char tmpfileName[1024];
    pid = getpid();
    sprintf(tmpfileName, "/tmp/sddssort.%ld", (long)pid);
    tmpfileForInternalPipe=1;
    if (!SDDS_InitializeCopy(&SDDS_tmp, &SDDS_input, tmpfileName, "w")) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (non_dominate_sort) {
      if (!SDDS_DefineSimpleColumn(&SDDS_output, "Rank", NULL, SDDS_LONG) ||
          !SDDS_DefineSimpleColumn(&SDDS_output, "CrowdingDistance", NULL, SDDS_DOUBLE)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (SDDS_CheckColumn(&SDDS_input, "ConstraintsViolation", NULL, SDDS_ANY_NUMERIC_TYPE, NULL)!=SDDS_CHECK_OK) {
        if (!SDDS_DefineSimpleColumn(&SDDS_output, "ConstraintsViolation", NULL, SDDS_DOUBLE))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        constDefined = 1;
      }
    }
    
    if (!SDDS_WriteLayout(&SDDS_tmp)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    while (SDDS_ReadPage(&SDDS_input)>0) {
      if (!SDDS_CopyPage(&SDDS_tmp,&SDDS_input) || 
          !SDDS_WritePage(&SDDS_tmp)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }      
    }
    if (!SDDS_Terminate(&SDDS_tmp)) {
       SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
       exit(1);
    }
    if (!SDDS_InitializeInput(&SDDS_input, tmpfileName)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_input, output, "w")) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (provideIdenticalCount &&
      !SDDS_DefineSimpleColumn(&SDDS_output, "IdenticalCount", NULL, SDDS_LONG)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (non_dominate_sort) {
    if (!SDDS_DefineSimpleColumn(&SDDS_output, "Rank", NULL, SDDS_LONG) ||
        !SDDS_DefineSimpleColumn(&SDDS_output, "CrowdingDistance", NULL, SDDS_DOUBLE)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (SDDS_CheckColumn(&SDDS_input, "ConstraintsViolation", NULL, SDDS_ANY_NUMERIC_TYPE, NULL)!=SDDS_CHECK_OK) {
      if (!SDDS_DefineSimpleColumn(&SDDS_output, "ConstraintsViolation", NULL, SDDS_DOUBLE))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      constDefined = 1;
    }
  }
  if (!SDDS_WriteLayout(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_SortAll(&SDDS_input,&SDDS_output,sort_request,sort_requests,
                 sort_parameter,sort_parameters,uniqueRows,provideIdenticalCount,pipeFlags, non_dominate_sort)) {
    SDDS_SetError("Problem sorting data");
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (tmpfile_used && !replaceFileAndBackUp(input, output)) 
    exit(1);
  free_scanargs(&s_arg,argc);
/*  if (tmpfileForInternalPipe)
    system("rm /tmp/tmpfile"); */
  return(0);
}

static SDDS_DATASET *SDDS_sort;
static SORT_REQUEST *sort_request;
static long sort_requests;
static int32_t *sort_row_index;

int SDDS_CompareData(SDDS_DATASET *SDDS_dataset, long type, void *data1, void *data2)
{
  double double_diff;
  float float_diff;
  int32_t long_diff;
  short short_diff;
  int char_diff;
  
  switch (type) {
  case SDDS_DOUBLE:
    if ((double_diff = *(double*)data1 - *(double*)data2)>0)
      return(1);
    else if (double_diff<0)
      return(-1);
    return(0);
  case SDDS_FLOAT:
    if ((float_diff = *(float*)data1 - *(float*)data2)>0)
      return(1);
    else if (float_diff<0)
      return(-1);
    return(0);
  case SDDS_LONG:
    if ((long_diff = *(int32_t*)data1 - *(int32_t*)data2)>0)
      return(1);
    else if (long_diff<0)
      return(-1);
    return(0);
  case SDDS_SHORT:
    if ((short_diff = *(short*)data1 - *(short*)data2)>0)
      return(1);
    else if (short_diff<0)
      return(-1);
    return(0);
  case SDDS_CHARACTER:
    if ((char_diff = *(char*)data1 - *(char*)data2)>0)
      return(1);
    else if (char_diff<0)
      return(-1);
    return(0);
  case SDDS_STRING:
    if (numericHigh)
      return(strcmp_nh(*(char**)data1, *(char**)data2));
    else
      return(strcmp(*(char**)data1, *(char**)data2));
  default:
    SDDS_SetError("Problem doing data comparison--invalid data type (SDDS_CompareData)");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
}

#if defined(SOLARIS) || defined(LINUX) || defined(_WIN32) || (defined(__BORLANDC__) && defined(__linux__))
int SDDS_CompareRows(const void *vrow1, const void *vrow2)
#else
long SDDS_CompareRows(const void *vrow1, const void *vrow2)
#endif
{
  /* I'm assuming that a double is large enough to store any of the data types,
   * including a pointer
   */
  static double data1, data2;
  static int32_t row1, row2;
  long i, comparison;
  row1 = *(int32_t*)vrow1;
  row2 = *(int32_t*)vrow2;
  
  for (i=0; i<sort_requests; i++) {
    if (!SDDS_GetValueByAbsIndex(SDDS_sort, sort_request[i].index, row1, &data1) ||
        !SDDS_GetValueByAbsIndex(SDDS_sort, sort_request[i].index, row2, &data2)) { 
      SDDS_SetError("Problem getting value for sort (SDDS_CompareRows)");
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
    if ((comparison=SDDS_CompareData(SDDS_sort, sort_request[i].type, (void*)&data1, (void*)&data2))) {
      if (sort_request[i].type==SDDS_STRING) {
        free(*((char**)&data1));
        free(*((char**)&data2));
      }
      return(sort_request[i].decreasing_order?-comparison:comparison);
    }
    if (sort_request[i].type==SDDS_STRING) {
      free(*((char**)&data1));
      free(*((char**)&data2));
    }
  }
  return(0);
}

long SDDS_SwapRows(SDDS_DATASET *SDDS_dataset, long row1, long row2)
{
#define SWAP_BUFFER_SIZE 16
  static char buffer[SWAP_BUFFER_SIZE];
  void **data;
  long i, size;
  data = SDDS_dataset->data;
#if defined(DEBUG)
  fprintf(stderr, "swapping row %ld with row %ld\n", row1, row2);
#endif    
  for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
    if ((size = SDDS_GetTypeSize(SDDS_dataset->layout.column_definition[i].type))>SWAP_BUFFER_SIZE) {
      SDDS_SetError("Unable to swap rows--swap buffer is too small (SDDS_SwapRows)");
      return(0);
    }
#if defined(DEBUG)
    if (SDDS_dataset->layout.column_definition[i].type==SDDS_STRING)
      fprintf(stderr, "    %s  <-->  %s\n", 
              *(char**)((char*)(data[i])+row1*size),
              *(char**)((char*)(data[i])+row2*size));
#endif
    memcpy(             (char*)buffer, (char*)(data[i])+row1*size, size);
    memcpy((char*)(data[i])+row1*size, (char*)(data[i])+row2*size, size);
    memcpy((char*)(data[i])+row2*size,              (char*)buffer, size);
  }
  return(1);
}


long SDDS_SortRows(SDDS_DATASET *SDDS_dataset, SORT_REQUEST *xsort_request, long xsort_requests, long non_dominate_sort)
{
  long rows, i, j, k;
  int32_t *row_location;
  char s[1024];
  double **data=NULL, *dist=NULL, *const_violation=NULL;
  int32_t *rank=NULL;
  population pop;
  long *maximize=NULL;
  
  SDDS_sort = SDDS_dataset;
  sort_request = xsort_request;
  sort_requests = xsort_requests;
  if ((rows = SDDS_CountRowsOfInterest(SDDS_sort))<0) 
    return(0);
  
  for (i=0; i<sort_requests; i++) {
    if ((sort_request[i].index=SDDS_GetColumnIndex(SDDS_sort, sort_request[i].name))<0) {
      sprintf(s,"column name \"%s\" is not recognized(SDDS_GetColumnIndex)",sort_request[i].name);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      return(0);
    }
    sort_request[i].type = SDDS_GetColumnType(SDDS_sort, sort_request[i].index);
  }
  if (non_dominate_sort) {
    data=(double**)malloc(sizeof(*data)*sort_requests);
    maximize=(long*)malloc(sizeof(*maximize)*sort_requests);
    for (i=0; i<sort_requests; i++) {
      if (sort_request[i].type==SDDS_STRING) {
        fprintf(stderr, "Non-dominated sort is not available for string column.\n");
        exit(1);
      }
      if (!(data[i]=(double*)SDDS_GetColumnInDoubles(SDDS_sort, sort_request[i].name))) {
        SDDS_SetError("Problem performing sort");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      maximize[i]=sort_request[i].maximize_order;
    }
	const_violation = read_constr_violation(SDDS_sort);
    fill_population(&pop, rows, sort_requests, data, maximize, const_violation);
    sort_row_index = non_dominated_sort(&pop);
    rank = (int32_t*)malloc(sizeof(*rank)*rows);
    dist = (double*)malloc(sizeof(*dist)*rows);
    if (!const_violation)
      const_violation = calloc(rows, sizeof(*const_violation));
    for (i=0; i< rows; i++) {
      rank[i] = pop.ind[sort_row_index[i]].rank;
      dist[i] = pop.ind[sort_row_index[i]].crowd_dist;
      const_violation[i] = pop.ind[sort_row_index[i]].constr_violation;
    }
    free_pop_mem(&pop);
    for (i=0; i<sort_requests; i++)
      free(data[i]);
    free(data);
    free(maximize);
  } else {
    sort_row_index = tmalloc(sizeof(*sort_row_index)*rows);
    for (i=0; i<rows; i++)
      sort_row_index[i] = i;
    /* After this sort, sort_row_index will contain the indices of the rows
     * in sorted order 
     */
    qsort((void*)sort_row_index, rows, sizeof(*sort_row_index), SDDS_CompareRows);
    
    /* create an array to give the location in the sort_row_index array of
     * a particular row
     */
#if defined(DEBUG)
    fprintf(stderr, "new row order:\n");
    for (i=0; i<rows; i++)
      fprintf(stderr, "%ld %ld\n", i, sort_row_index[i]);
    fprintf(stderr, "\n");
#endif
  }
  row_location = tmalloc(sizeof(*sort_row_index)*rows);
  for (i=0; i<rows; i++)
    row_location[sort_row_index[i]] = i;
  for (i=0; i<rows; i++) {
    if ((j=sort_row_index[i])!=i) {
      /* move this row from position i to position j, where it belongs */
      if (!SDDS_SwapRows(SDDS_sort, i, j)) {
        SDDS_SetError("Problem swapping rows after index sort (SDDS_SortRows");
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }
      /* adjust the indices to reflect the swap */
      sort_row_index[i] = i;
      k = row_location[i];
      row_location[i] = i;
      sort_row_index[k] = j;
      row_location[j] = k;
    }
#if defined(DEBUG)
    fprintf(stderr, "new row order:\n");
    for (j=0; j<rows; j++)
      fprintf(stderr, "%ld %ld\n", j, sort_row_index[j]);
    fprintf(stderr, "\n");
#endif
  }
  if (non_dominate_sort) {
    if (SDDS_SetColumn(SDDS_sort, SDDS_SET_BY_NAME, rank, rows, "Rank", NULL) != 1 || 
        SDDS_SetColumn(SDDS_sort, SDDS_SET_BY_NAME, dist, rows, "CrowdingDistance", NULL) != 1 ||
        SDDS_SetColumn(SDDS_sort, SDDS_SET_BY_NAME, const_violation, rows, "ConstraintsViolation", NULL) != 1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    free(rank);
    free(dist);
    free(const_violation);
  }
  free(sort_row_index);
  free(row_location);
  return 1;
}

long SDDS_UnsetDuplicateRows(SDDS_DATASET *SDDS_dataset, SORT_REQUEST *xsort_request, long xsort_requests,
                             long provideIdenticalCount)
{
  int32_t rows, *identicalCount;
  int32_t i, j, *rowFlag;
  char s[1024];
  
  SDDS_sort = SDDS_dataset;
  sort_request = xsort_request;
  sort_requests = xsort_requests;

  for (i=0; i<sort_requests; i++) {
    if ((sort_request[i].index=SDDS_GetColumnIndex(SDDS_sort, sort_request[i].name))<0) {
      sprintf(s,"column name \"%s\" is not recognized(SDDS_GetColumnIndex)",sort_request[i].name);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      return 0;
    }
    sort_request[i].type = SDDS_GetColumnType(SDDS_sort, sort_request[i].index);
  }
  
  if ((rows = SDDS_CountRowsOfInterest(SDDS_sort))<0) 
    return(0);
  
  rowFlag = tmalloc(sizeof(*rowFlag)*rows);
  identicalCount = tmalloc(sizeof(*identicalCount)*rows);
  for (i=0; i<rows; i++)
    rowFlag[i] = identicalCount[i] = 1;
  
  for (i=0; i<rows-1; i++) {
    if (!rowFlag[i])
      continue;
    for (j=i+1; j<rows; j++) {
      if (rowFlag[j]) {
        if (SDDS_CompareRows(&i, &j)==0) {
          identicalCount[i] += 1;
          rowFlag[j] = 0;
        }
        else
          break;
      }
    }
  }
  if (!SDDS_AssertRowFlags(SDDS_sort, SDDS_FLAG_ARRAY, rowFlag, rows))
    return 0;
  if (provideIdenticalCount &&
      !SDDS_SetColumn(SDDS_sort, SDDS_SET_BY_NAME, identicalCount, rows, "IdenticalCount"))
    return 0;
  free(rowFlag);
  return 1;
}

static SORT_REQUEST *sort_parameter;
static long sort_parameters;
static SDDS_DATASET *SDDS_sortpage;

#if defined(SOLARIS) || defined(LINUX) || defined(_WIN32) || (defined(__BORLANDC__) && defined(__linux__))
int SDDS_ComparePages(const void *vpage1, const void *vpage2)
#else
long SDDS_ComparePages(const void *vpage1, const void *vpage2)
#endif
{
  /* I'm assuming that a double is large enough to store any of the data types,
   * including a pointer
   */
  static int32_t page1, page2;
  long i, comparison;

  page1 = *(int32_t*)vpage1;
  page2 = *(int32_t*)vpage2; 
  
  for (i=0; i<sort_parameters; i++) {
    if ((comparison=SDDS_CompareData(SDDS_sortpage, sort_parameter[i].type, 
                                     (void*)&(sort_parameter[i].data[page1]), 
                                     (void*)&(sort_parameter[i].data[page2])))){
     /* if (sort_parameter[i].type==SDDS_STRING) {
        free(*((char**)&(sort_parameter[i].data[page1])));
        free(*((char**)&(sort_parameter[i].data[page2])));
      } */
      return(sort_parameter[i].decreasing_order?-comparison:comparison);
    }
   /* if (sort_parameter[i].type==SDDS_STRING) {
      free(*((char**)&(sort_parameter[i].data[page1])));
      free(*((char**)&(sort_parameter[i].data[page2])));
    } */
  }
  return(0);
}

long SDDS_SortAll(SDDS_DATASET *SDDS_inputx,SDDS_DATASET *SDDS_outputx,
               SORT_REQUEST *xsort_request,long xsort_requests,SORT_REQUEST *xsort_parameter,
               long xsort_parameters,long uniqueRows,long provideIdenticalCount,long xpipeFlags, long non_dominate_sort)
{
  long i,j,k,pages;
  int32_t *xsort_page_index;
  char s[1024];
  SDDS_DATASET *tmp_datasets;
  
  tmp_datasets=NULL;
  SDDS_sortpage=SDDS_inputx;
  sort_parameter = xsort_parameter;
  sort_parameters = xsort_parameters;
  sort_request = xsort_request;
  sort_requests = xsort_requests;
  xsort_page_index=NULL;
  pages=i=j=k=0;
  for (i=0; i<sort_parameters; i++) {
    if ((sort_parameter[i].index=SDDS_GetParameterIndex(SDDS_sortpage, 
                                                        sort_parameter[i].name))<0) {
      sprintf(s,"Unable to get parameter value--parameter name \"%s\" is not recognized(SDDS_GetParameterIndex)",sort_parameter[i].name);
      SDDS_SetError(s);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      return(0);
    }
    sort_parameter[i].type = SDDS_GetParameterType(SDDS_sortpage, sort_parameter[i].index);
  }
  if (sort_parameters) {
    if (xpipeFlags==0 && !SDDS_sortpage->layout.gzipFile) 
      SDDS_SetDefaultIOBufferSize(0);
    for (i=0;i<sort_parameters;i++)  
      sort_parameter[i].data=NULL;
    while (SDDS_ReadPage(SDDS_sortpage)>0) {
      if (xpipeFlags || SDDS_sortpage->layout.gzipFile) {
        /*copy each page of sortpage into tmp_datasets indexed by the pages, i.e.
          tmp_datasets[pages] contains only one page */
        tmp_datasets=realloc(tmp_datasets,sizeof(*tmp_datasets)*(pages+1));
        if (!SDDS_InitializeCopy(&tmp_datasets[pages],SDDS_sortpage,NULL,"m")) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          return(0);
        }
        if (!SDDS_CopyPage(&tmp_datasets[pages],SDDS_sortpage)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          return(0);
        }
      }
      for (i=0;i<sort_parameters;i++)  {
        sort_parameter[i].data=realloc(sort_parameter[i].data,
                                       sizeof(*sort_parameter[i].data)*(pages+1));
        
        if (!SDDS_GetParameterByIndex(SDDS_sortpage, sort_parameter[i].index,
                                      &(sort_parameter[i].data[pages]))) {
          SDDS_SetError("Problem getting parameter value for sort (SDDS_SortAll");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        }
      }
      pages++;
    }
    /*sort pages by parameter*/
    xsort_page_index = tmalloc(sizeof(*xsort_page_index)*pages);
    for (k=0; k<pages; k++)
      xsort_page_index[k] = k;
    if (pages>1)
      qsort((void*)xsort_page_index, pages, sizeof(*xsort_page_index), SDDS_ComparePages);
    
    /*sort_page_index=xsort_page_index;*/
    for (i=0; i<pages; i++) {
      j=xsort_page_index[i];
      if (xpipeFlags || SDDS_sortpage->layout.gzipFile) {
        if (!SDDS_CopyPage(SDDS_outputx,&tmp_datasets[j])) {
          SDDS_SetError("Problem copying data from memory");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (!SDDS_Terminate(&tmp_datasets[j])) {
          SDDS_SetError("Problem terminate datasets");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      } else {
        if (!SDDS_GotoPage(SDDS_sortpage,j+1)) {
          SDDS_SetError("Problem goto page");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (SDDS_ReadPage(SDDS_sortpage)<1) {
          SDDS_SetError("Problem read page");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        } 
        if (!SDDS_CopyPage(SDDS_outputx,SDDS_sortpage)) {
          SDDS_SetError("Problem copying data");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (sort_requests) {
        if (SDDS_CountRowsOfInterest(SDDS_outputx)>0) {
          if (!SDDS_SortRows(SDDS_outputx, sort_request, sort_requests, non_dominate_sort)) {
            SDDS_SetError("Problem performing sort");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
          if (uniqueRows &&
              !SDDS_UnsetDuplicateRows(SDDS_outputx, sort_request, sort_requests,
                                       provideIdenticalCount)) {
            SDDS_SetError("Problem marking duplicate rows.");
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        }
      }
      if (!SDDS_WritePage(SDDS_outputx)) {
        SDDS_SetError("Problem writing data to output file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  else {
    while (SDDS_ReadPage(SDDS_inputx)>0) {
      if (!SDDS_CopyPage(SDDS_outputx, SDDS_inputx)) {
        SDDS_SetError("Problem copying data for output file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      if (SDDS_CountRowsOfInterest(SDDS_outputx)>0) {
        if (!SDDS_SortRows(SDDS_outputx, sort_request, sort_requests, non_dominate_sort)) {
          SDDS_SetError("Problem performing sort");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (uniqueRows &&
            !SDDS_UnsetDuplicateRows(SDDS_outputx, sort_request, sort_requests,
                                     provideIdenticalCount)) {
          SDDS_SetError("Problem marking duplicate rows.");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (!SDDS_WritePage(SDDS_outputx)) {
        SDDS_SetError("Problem writing data to output file");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  if (tmp_datasets)
    free(tmp_datasets);
  if (xsort_page_index)
    free(xsort_page_index);
  return 1;
}

double *read_constr_violation(SDDS_DATASET *SDDS_dataset) {
  double *const_violation=NULL, **data=NULL;
  int32_t i, columns, constraints, rows, j;
  char **columnName = NULL;
  
  columns = constraints = 0;
  rows = SDDS_CountRowsOfInterest(SDDS_dataset); 
  if (!constDefined && !(const_violation=(double*)SDDS_GetColumnInDoubles(SDDS_dataset, "ConstraintsViolation")))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  else {
	if (!(columnName=(char**)SDDS_GetColumnNames(SDDS_dataset, &columns)))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	for (i=0; i<columns; i++) {
	  if (wild_match(columnName[i], "*Constraints*") && strcmp(columnName[i], "ConstraintsViolation")!=0) {
		data = SDDS_Realloc(data, sizeof(*data)*(constraints+1));
		data[constraints] = NULL;
		if (!(data[constraints] = (double*)SDDS_GetColumnInDoubles(SDDS_dataset, columnName[i])))
		  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
		constraints ++;
	  }
	} 
	if (constraints) {
	  const_violation = calloc(rows, sizeof(*const_violation));
	  for (i=0; i<rows; i++) {
		for (j=0; j<constraints; j++) {
		  if (data[j][i]<0)
			const_violation[i] +=data[j][i];
		}
	  }
	  for (j=0; j<constraints; j++)
		free(data[j]);
	  free(data);
	}
  }
  return const_violation;
}

