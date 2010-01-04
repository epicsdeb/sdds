/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: sddsplotFilter.c
 * 
 * part of sddsplot (plotting program for SDDS files)
 *
 * Michael Borland, 1994.
 $Log: sddsplotFilter.c,v $
 Revision 1.12  2005/11/04 22:46:46  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.11  2003/04/17 14:40:37  soliday
 Fixed last change for Windows.

 Revision 1.10  2003/04/14 14:46:24  shang
 added processing time filter function

 Revision 1.9  2002/08/14 17:24:54  soliday
 Added Open License

 Revision 1.8  1999/06/03 16:09:06  soliday
 Removed compiler warnings under linux.

 Revision 1.7  1997/07/08 14:39:50  borland
 Fixed bug that showed up when multipler -filter and/or -match options
 were given for one plot request.  Required knowing the number of rows,
 not number of rows of interest.

 Revision 1.6  1997/01/20 21:32:44  borland
 Fixed bug in allocation logic for rowFlag1 and rowFlag2.

 Revision 1.5  1996/09/30 17:51:38  borland
 Modified filtering routines to keep track of flags for multiple filters,
 which is necessary in order to implement the logic properly without
 requiring deletion of rows from the SDDS table structure.

 * Revision 1.4  1996/02/14  00:59:23  borland
 * Fixed bug in read routines (wasn't resetting row flags between requests).
 * Changed argument parsing over to scanItemList form scan_item_list.
 *
 * Revision 1.3  1995/10/18  21:56:33  borland
 * Modified data-reading routines to read multiple instances of the same file
 * only once (sddsplotRead.c).  Other routines have minor modifications.
 *
 * Revision 1.2  1995/09/05  21:09:51  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "graph.h"
#include "SDDS.h"
#include "sddsplot.h"
#include <ctype.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif

long perform_sddsplot_filtering(SDDS_TABLE *table, FILTER_DEFINITION **filter, long filters)
{
  long i, j, accept, skip_table;
  long n_left, row_deletion, n_rows;
  FILTER_TERM *filter_term;
  FILTER_DEFINITION *filter_ptr;
  PARAMETER_DEFINITION *pardefptr;
  double result;
  static int32_t *rowFlag1 = NULL, *rowFlag2 = NULL;
  static long rowFlags = 0;
  
  skip_table = 0;
#if DEBUG
  fprintf(stderr, "%ld filters being applied\n", filters);
#endif
  if (filters==0)
    return !skip_table;

  n_rows = SDDS_RowCount(table);
  row_deletion = 1;
  for (i=0; i<filters; i++) {
    filter_ptr = filter[i];
    if (filter_ptr->is_parameter) {
      accept = 1;
      filter_term = filter_ptr->filter_term;
      for (j=0; j<filter_ptr->filter_terms; j++) {
        if (!(pardefptr=SDDS_GetParameterDefinition(table, filter_term[j].name)) ||
            (pardefptr->type==SDDS_STRING || pardefptr->type==SDDS_CHARACTER)) {
          fprintf(stderr, "error: unknown or non-numeric parameter %s given for filter\n",
                  filter_term[j].name);
          exit(1);
        }
        accept = 
          SDDS_Logic(accept, 
                     SDDS_ItemInsideWindow(SDDS_GetParameter(table, filter_term[j].name, 
                                                             &result),
                                           0, pardefptr->type, filter_term[j].lower, 
                                           filter_term[j].upper),
                     filter_term[j].logic);
      }
      if (!accept) {
        skip_table = 1;
        break;
      }
    }
    else if (n_rows) {
      if (!rowFlag1 || !rowFlag2 || n_rows>rowFlags) {
        if (!(rowFlag1 = SDDS_Realloc(rowFlag1, sizeof(*rowFlag1)*n_rows)) || 
            !(rowFlag2 = SDDS_Realloc(rowFlag2, sizeof(*rowFlag2)*n_rows))) {
          fprintf(stderr, "Problem reallocating row flags (perform_sddsplot_filtering)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_GetRowFlags(table, rowFlag1, n_rows)) {
        fprintf(stderr, "Unable to get row flags (perform_sddsplot_filtering)\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      filter_term = filter_ptr->filter_term;
      for (j=0; j<filter_ptr->filter_terms; j++) {
#if defined(DEBUG)
        fprintf(stderr, "Filtering by %s on [%e, %e] with logic %x\n",
                filter_term[j].name, filter_term[j].lower,
                filter_term[j].upper, filter_term[j].logic);
#endif
        if ((n_left=SDDS_FilterRowsOfInterest(table, filter_term[j].name, filter_term[j].lower,
                                              filter_term[j].upper, filter_term[j].logic))<0) {
          fprintf(stderr, "error: unable to filter by column %s\n", 
                  filter_term[j].name);
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
       /* fprintf(stderr,"total_rows=%d, filtered left=%d\n",n_rows,n_left);*/
        
#if defined(DEBUG)
        fprintf(stderr, "%ld rows of %ld left after filter %ld of set %ld\n",
                n_left, n_rows, j, i);
#endif
      }
      if (i) {
        if (!SDDS_GetRowFlags(table, rowFlag2, n_rows)) {
          fprintf(stderr, "Unable to get row flags (perform_sddsplot_filtering)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        for (j=0; j<n_rows; j++) {
          rowFlag1[j] = rowFlag1[j]&rowFlag2[j];
        }
        if (!SDDS_AssertRowFlags(table, SDDS_FLAG_ARRAY, rowFlag1, n_rows)) {
          fprintf(stderr, "Unable to assert row flags (perform_sddsplot_filtering)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      } else {
        if (!SDDS_GetRowFlags(table, rowFlag1, n_rows)) {
          fprintf(stderr, "Unable to get row flags (perform_sddsplot_filtering)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }      
      row_deletion = 1;
    }
  }
  return !skip_table;
}

long perform_sddsplot_time_filtering(SDDS_TABLE *table, TIME_FILTER_DEFINITION **time_filter, long time_filters)
{
  long i, j, accept, skip_table;
  long n_left, row_deletion, n_rows;
  TIME_FILTER_DEFINITION *time_filter_ptr;
  PARAMETER_DEFINITION *pardefptr;
  double result;
  static int32_t *rowFlag1 = NULL, *rowFlag2 = NULL;
  static long rowFlags = 0;

skip_table = 0;
#if DEBUG
  fprintf(stderr, "%ld time filters being applied\n", time_filters);
#endif
  if (time_filters==0)
    return !skip_table;

  n_rows = SDDS_RowCount(table);
  row_deletion = 1;
  
  for (i=0;i<time_filters;i++) {
    time_filter_ptr=time_filter[i];
    if (time_filter_ptr->is_parameter) {
      if (!(pardefptr=SDDS_GetParameterDefinition(table, time_filter_ptr->name)) ||
          (pardefptr->type==SDDS_STRING || pardefptr->type==SDDS_CHARACTER)) {
        fprintf(stderr, "error: unknown or non-numeric parameter %s given for time filter\n",
                time_filter_ptr->name);
        exit(1);
      }
      accept=SDDS_ItemInsideWindow(SDDS_GetParameter(table, time_filter_ptr->name, &result),
                                   0, pardefptr->type, time_filter_ptr->after,time_filter_ptr->before);
      
      if (!accept) 
        skip_table=1;
      if (time_filter_ptr->flags&TIMEFILTER_INVERT_GIVEN)
        skip_table=!skip_table;
      if (skip_table)
        break;
    }
    else if (n_rows) {
      if (!rowFlag1 || !rowFlag2 || n_rows>rowFlags) {
        if (!(rowFlag1 = SDDS_Realloc(rowFlag1, sizeof(*rowFlag1)*n_rows)) || 
            !(rowFlag2 = SDDS_Realloc(rowFlag2, sizeof(*rowFlag2)*n_rows))) {
          fprintf(stderr, "Problem reallocating row flags (perform_sddsplot_filtering)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_GetRowFlags(table, rowFlag1, n_rows)) {
        fprintf(stderr, "Unable to get row flags (perform_sddsplot_filtering)\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
#if DEBUG     
      fprintf(stderr, "   * applying time filter (column %s)", time_filter_ptr->name);
#endif
      if ((n_left=SDDS_FilterRowsOfInterest(table, time_filter_ptr->name, time_filter_ptr->after,
                                            time_filter_ptr->before,
                                            time_filter_ptr->flags&TIMEFILTER_INVERT_GIVEN? SDDS_NEGATE_EXPRESSION:SDDS_AND ))<0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (!SDDS_GetRowFlags(table, rowFlag2, n_rows)) {
        fprintf(stderr, "Unable to get row flags (perform_sddsplot_filtering)\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      n_left=0;
      for (j=0; j<n_rows; j++) {
        rowFlag1[j] = rowFlag1[j]&rowFlag2[j];
        if (rowFlag1[j])
          n_left++;
      }
      if (!SDDS_AssertRowFlags(table, SDDS_FLAG_ARRAY, rowFlag1, n_rows)) {
        fprintf(stderr, "Unable to assert row flags (perform_sddsplot_filtering)\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
     /* fprintf(stderr,"total_rows=%d,left rows=%d\n",n_rows,n_left); */
     /* fprintf(stderr,"total_rows=%d, time filtered left=%d\n",n_rows,n_left); */
      if (!n_left) 
        skip_table=1;
      if (skip_table)
        break; 
      row_deletion = 1; 
    } 
  }
  return !skip_table;
}


long perform_sddsplot_matching(SDDS_TABLE *table, MATCH_DEFINITION **match, long matches)
{
  long i, j, accept, skip_table;
  long n_rows, row_deletion;
  MATCH_TERM *match_term;
  MATCH_DEFINITION *match_ptr;
  PARAMETER_DEFINITION *pardefptr;
  static char s[SDDS_MAXLINE];
  static int32_t *rowFlag1 = NULL, *rowFlag2 = NULL;
  static long rowFlags = 0;

  skip_table = 0;
  if (matches==0)
    return !skip_table;
  
  n_rows = SDDS_RowCount(table);
  for (i=0; i<matches; i++) {
    match_ptr = match[i];
    if (match_ptr->is_parameter) {
      accept = 1;
      match_term = match_ptr->match_term;
      for (j=0; j<match_ptr->match_terms; j++) {
        if (!(pardefptr=SDDS_GetParameterDefinition(table, match_term[j].name)) ||
            !(pardefptr->type==SDDS_STRING || pardefptr->type==SDDS_CHARACTER)) {
          fprintf(stderr, "error: unknown or numeric parameter %s given for match\n",
                  match_term[j].name);
          exit(1);
        }
        if (pardefptr->type==SDDS_STRING) {
          char **ppc;
          ppc = SDDS_GetParameter(table, match_term[j].name, NULL);
          strcpy(s, *ppc);
        }
        else {
          char *pc;
          pc = SDDS_GetParameter(table, match_term[j].name, NULL);
          sprintf(s, "%c", *pc);
        }
        accept = SDDS_Logic(accept, wild_match(s, match_term[j].string), match_term[j].logic);
      }
      if (!accept) {
        skip_table = 1;
        break;
      }
    }
    else if (n_rows) {
      if (!rowFlag1 || !rowFlag2 || n_rows>rowFlags) {
        if (!(rowFlag1 = SDDS_Realloc(rowFlag1, sizeof(*rowFlag1)*n_rows)) || 
            !(rowFlag2 = SDDS_Realloc(rowFlag2, sizeof(*rowFlag2)*n_rows))) {
          fprintf(stderr, "Unable to reallocate row flag arrays (perform_sddsplot_matching)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }
      if (!SDDS_GetRowFlags(table, rowFlag1, n_rows)) {
        fprintf(stderr, "Unable to get row flags (perform_sddsplot_matching-1)\n");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      match_term = match_ptr->match_term;
      for (j=0; j<match_ptr->match_terms; j++) {
#if defined(DEBUG)
        fprintf(stderr, "Matching %s to %s on with logic %x\n",
                match_term[j].name,
                match_term[j].string, match_term[j].logic);
#endif
        if (SDDS_MatchRowsOfInterest(table, match_term[j].name, 
                                     match_term[j].string, match_term[j].logic)<0) {
          fprintf(stderr, "Error matching rows (perform_sddsplot_matching)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
#if defined(DEBUG)
        fprintf(stderr, "%ld rows left after match %ld of set %ld\n",
                SDDS_CountRowsOfInterest(table), j, i);
#endif
      }
      if (i) {
        if (!SDDS_GetRowFlags(table, rowFlag2, n_rows)) {
          fprintf(stderr, "Unable to get row flags (perform_sddsplot_matching-2)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        for (j=0; j<n_rows; j++) {
          rowFlag1[j] = rowFlag1[j]&rowFlag2[j];
        }
        if (!SDDS_AssertRowFlags(table, SDDS_FLAG_ARRAY, rowFlag1, n_rows)) {
          fprintf(stderr, "Unable to assert row flags (perform_sddsplot_matching)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      } else {
        if (!SDDS_GetRowFlags(table, rowFlag1, n_rows)) {
          fprintf(stderr, "Unable to get row flags (perform_sddsplot_matching-3)\n");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
      }      
      row_deletion = 1;
    }
  }
  return !skip_table;
}

