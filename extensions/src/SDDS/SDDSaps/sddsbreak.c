/*************************************************************************\
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 * National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 * Operator of Los Alamos National Laboratory.
 * This file is distributed subject to a Software License Agreement found
 * in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssbreak
 * purpose: break up pages of a SDDS file into subpages
 *
 * Michael Borland, 1994
 $Log: sddsbreak.c,v $
 Revision 1.16  2009/06/23 22:58:43  borland
 Added -matchto option.

 Revision 1.15  2007/04/09 15:58:13  soliday
 Cleaned up code.

 Revision 1.14  2006/12/14 22:21:57  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.13  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.12  2005/11/04 22:46:11  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.11  2004/02/13 15:57:50  borland
 Fixed bug in -changeOf option.  The base qualifier was essentially ignored.

 Revision 1.10  2002/08/14 17:12:40  soliday
 Added Open License

 Revision 1.9  2001/01/10 19:35:32  soliday
 Standardized usage message.

 Revision 1.8  1999/05/25 19:05:28  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:37  borland
 Fixed the version number in the usage message.

 Revision 1.6  1997/10/29 16:30:40  borland
 Will now break on changes in string data, using -changeOf option.

 * Revision 1.5  1996/03/01  23:59:21  borland
 * Fixed bug that resulted in no breaking when the input file had no row
 * counts.
 *
 * Revision 1.4  1996/02/14  01:05:08  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.3  1995/09/06  14:56:07  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_GAPIN 0
#define SET_INCREASEOF 1
#define SET_DECREASEOF 2
#define SET_CHANGEOF 3
#define SET_ROWLIMIT 4
#define SET_PIPE 5
#define SET_MATCHTO 6
#define N_OPTIONS 7

char *option[N_OPTIONS] = {
  "gapin", "increaseof", "decreaseof", 
  "changeof", "rowlimit", "pipe", "matchto"
} ;

char *USAGE = "sddsbreak [-pipe=[input][,output]] [<inputfile>] [<outputfile>]\n\
 -gapin=<column-name>[,{amount=<value>|factor=<value>}] \n\
 -increaseof=<column-name> -decreaseof=<column-name> \n\
 -changeof=<column-name>[,amount=<value>,base=<value>] \n\
 -matchto=<column-name>,<pattern>[,after] \n\
 -rowlimit=<integer> \n\n\
Program by Michael Borland. (This is version 4, June 2009.)\n";

#define GAPIN_AMOUNT    0x0001U
#define GAPIN_FACTOR    0x0002U
char *gapinUsage = "-gapin=<column-name>[,{amount=<value> | factor=<value>}]";

#define CHANGEOF_AMOUNT 0x0001U
#define CHANGEOF_BASE   0x0002U
char *changeofUsage = "-changeof=<column-name>[,amount=<value>,base=<value>]";

int main(int argc, char **argv)
{
  SDDS_DATASET SDDSnew, SDDSold;
  long i, j, iArg;
  SCANNED_ARG *scArg;
  char *input, *output, *columnName;
  long mode, matchCode, rows, rowsMinus1, tmpfile_used;
  double gapAmount, *columnData, gapFactor;
  char *matchPattern;
  long matchPatternAfter = 0;
  
  double changeAmount, changeBase;
  long retval, newStart, rowLimit, breakNext;
  int32_t dataType;
  unsigned long flags, pipeFlags, changeFlags;
  char **stringData;
    
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scArg, argc, argv);
  if (argc<2) {
    fprintf(stderr, "%s", USAGE);
    return(1);
  }

  columnData = NULL;
  stringData = NULL;
  input = output = columnName = NULL;
  mode = -1;
  pipeFlags = flags = 0;
  gapAmount = changeAmount = rowLimit = gapFactor = 0;
  matchPattern = NULL;
  
  for (iArg=1; iArg<argc; iArg++) {
    if (scArg[iArg].arg_type==OPTION) {
      switch (matchCode=match_string(scArg[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_GAPIN:
        if ((scArg[iArg].n_items-=2)<0 || 
            !scanItemList(&flags, scArg[iArg].list+2, &scArg[iArg].n_items, 0,
                          "amount", SDDS_DOUBLE, &gapAmount, 1, GAPIN_AMOUNT, 
                          "factor", SDDS_DOUBLE, &gapFactor, 1, GAPIN_FACTOR, 
                          NULL) ||
            (flags&GAPIN_AMOUNT && gapAmount<=0) ||
            (flags&GAPIN_FACTOR && gapFactor<=0)) {
          fprintf(stderr, "Error: invalid -gapin syntax/values\n");
          return(1);
        }
        columnName = scArg[iArg].list[1];
        mode = matchCode;
        break;
      case SET_INCREASEOF: case SET_DECREASEOF:
        if (scArg[iArg].n_items!=2) {
          fprintf(stderr, "Error: invalid option syntax---specify column-name with -increaseof and -decreaseof\n");
          return(1);
        }
        columnName = scArg[iArg].list[1];
        mode = matchCode;
        break;
      case SET_CHANGEOF:
        if ((scArg[iArg].n_items-=2)<0 ||
            !scanItemList(&changeFlags, scArg[iArg].list+2, &scArg[iArg].n_items,  0,
                          "amount", SDDS_DOUBLE, &changeAmount, 1, CHANGEOF_AMOUNT, 
                          "base", SDDS_DOUBLE, &changeBase, 1, CHANGEOF_BASE, 
                          NULL) ||
            (changeFlags&CHANGEOF_AMOUNT && changeAmount<=0)) {
          fprintf(stderr, "Error: invalid -changeof syntax/values\n");
          return(1);
        }
        columnName = scArg[iArg].list[1];
        mode = matchCode;
        break;
      case SET_ROWLIMIT:
        if (scArg[iArg].n_items!=2 ||
            sscanf(scArg[iArg].list[1], "%ld", &rowLimit)!=1 ||
            rowLimit<=0) {
          fprintf(stderr, "Error: invalid -rowlimit syntax\n");
          return(1);
        }
        mode = matchCode;
        break;
      case SET_PIPE:
        if (!processPipeOption(scArg[iArg].list+1, scArg[iArg].n_items-1, &pipeFlags)) {
          fprintf(stderr, "Error: invalid -pipe syntax\n");
          return(1);
        }
        break;
      case SET_MATCHTO:
        if ((scArg[iArg].n_items!=3 && scArg[iArg].n_items!=4) ||
            strlen(columnName=scArg[iArg].list[1])==0 || 
            strlen(matchPattern=scArg[iArg].list[2])==0) {
          fprintf(stderr, "Error: invalid -matchTo syntax\n");
          return(1);
        }
        if (scArg[iArg].n_items==4) {
          if (strncmp(scArg[iArg].list[3], "after", strlen(scArg[iArg].list[3]))==0)
            matchPatternAfter = 1;
          else {
            fprintf(stderr, "Error: invalid -matchTo syntax\n");
            return(1);
          }
        }
        mode = matchCode;
        break;
      default:
        fprintf(stderr, "Error: unknown switch: %s\n", scArg[iArg].list[0]);
        fprintf(stderr, "%s", USAGE);
        return(1);
      }
    }
    else {
      if (input==NULL)
        input = scArg[iArg].list[0];
      else if (output==NULL)
        output = scArg[iArg].list[0];
      else {
        fprintf(stderr, "Error: too many filenames given\n");
        return(1);
      }
    }
  }

  processFilenames("sddsbreak", &input, &output, pipeFlags, 0, &tmpfile_used);

  if (mode==-1) {
    fprintf(stderr, "Error: no break mode specified\n");
    return(1);
  }

  if (!SDDS_InitializeInput(&SDDSold, input) || 
      !SDDS_InitializeCopy(&SDDSnew, &SDDSold, output, "w")) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }

  SDDSnew.layout.data_mode.no_row_counts = 0;
  if (!SDDS_WriteLayout(&SDDSnew)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
    
  if (mode!=SET_ROWLIMIT) {
    if (SDDS_GetColumnInformation(&SDDSold, "type", &dataType, SDDS_BY_NAME, columnName)!=SDDS_LONG) {
      SDDS_SetError("problem getting type information on given column");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (mode==SET_MATCHTO) {
      if (!(dataType==SDDS_STRING)) {
        fprintf(stderr, "Error: given column does not contain string data\n");
        return(1);
      }
    } else if (!SDDS_NUMERIC_TYPE(dataType)) {
      if (!(mode==SET_CHANGEOF && !(changeFlags&CHANGEOF_AMOUNT) && !(changeFlags&CHANGEOF_BASE))) {
        fprintf(stderr, "Error: given column does not contain numeric data\n");
        return(1);
      }
    }
  }

  while ((retval=SDDS_ReadPage(&SDDSold))>0) {
    if ((rows = SDDS_CountRowsOfInterest(&SDDSold))<0) {
      SDDS_SetError("Problem getting number of rows of tabular data");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    rowsMinus1 = rows-1;
    if (!SDDS_StartPage(&SDDSnew, rows) ||
        !SDDS_CopyParameters(&SDDSnew, &SDDSold) ||
        !SDDS_CopyArrays(&SDDSnew, &SDDSold)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (rows==0) {
      if (!SDDS_WritePage(&SDDSnew)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      continue;
    }
    switch (mode) {
    case SET_GAPIN:
      if (!(columnData=SDDS_GetColumnInDoubles(&SDDSold, columnName))) {
        SDDS_SetError("unable to read specified column");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      if (!gapAmount && rows>1) {
        double *gap;
        gap = tmalloc(sizeof(*gap)*rows);
        for (i=1; i<rows; i++)
          gap[i-1] = fabs(columnData[i]-columnData[i-1]);
        if (!compute_average(&gapAmount, gap, rows-1)) {
          fprintf(stderr, "Error: unable to determine default gap amount--couldn't find median gap\n");
          return(1);
        }
        gapAmount *= (gapFactor?gapFactor:2);
        free(gap);
      }
      newStart = 0;
      for (i=1; i<=rows; i++) {
        if (i!=rows && fabs(columnData[i]-columnData[i-1])<gapAmount)
          continue;
        if (!SDDS_SetRowFlags(&SDDSold, 0) || 
            !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
            !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        newStart = i;
      }
      free(columnData);
      break;
    case SET_INCREASEOF:
      if (!(columnData=SDDS_GetColumnInDoubles(&SDDSold, columnName))) {
        SDDS_SetError("unable to read specified column");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      newStart = 0;
      for (i=1; i<=rows; i++) {
        if (i!=rows && columnData[i]<=columnData[i-1]) 
          continue;
        if (!SDDS_SetRowFlags(&SDDSold, 0) || 
            !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
            !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        newStart = i;
      }
      free(columnData);
      break;
    case SET_DECREASEOF:
      if (!(columnData=SDDS_GetColumnInDoubles(&SDDSold, columnName))) {
        SDDS_SetError("unable to read specified column");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      newStart = 0;
      for (i=1; i<=rows; i++) {
        if (i!=rows && columnData[i]>=columnData[i-1]) 
          continue;
        if (!SDDS_SetRowFlags(&SDDSold, 0) || 
            !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
            !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        newStart = i;
      }
      free(columnData);
      break;
    case SET_CHANGEOF:
      if (dataType!=SDDS_STRING) {
        if (!(columnData=SDDS_GetColumnInDoubles(&SDDSold, columnName))) {
          SDDS_SetError("unable to read specified column");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      } else {
        if (!(stringData=SDDS_GetColumn(&SDDSold, columnName))) {
          SDDS_SetError("unable to read specified column");
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      }
      newStart = 0;
      if (dataType==SDDS_STRING || !changeAmount) {
        for (i=1; i<=rows; i++) {
          if (i!=rows && 
              ((dataType==SDDS_STRING && strcmp(stringData[i], stringData[i-1])==0) ||
               (dataType!=SDDS_STRING && columnData[i]==columnData[i-1])))
            continue;
          if (!SDDS_SetRowFlags(&SDDSold, 0) || 
              !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
              !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
          newStart = i;
        }
      }
      else {
        long region, lastRegion;
        region = lastRegion = 0;
        if (!(changeFlags&CHANGEOF_BASE) && rows>=1)
          changeBase = columnData[0];
        if (rows>1)
          lastRegion = (columnData[0]-changeBase)/changeAmount;
#ifdef DEBUG
        fprintf(stderr, "change base=%e, lastRegion=%ld\n", changeBase, lastRegion);
        fprintf(stderr, "start value = %e\n", columnData[0]);
#endif
        newStart = 0;
        for (i=1; i<=rows; i++) {
          if (i!=rows)
            region = (columnData[i]-changeBase)/changeAmount;
          if (i!=rows && region==lastRegion)
            continue;
#ifdef DEBUG
          fprintf(stderr, "split after %e, before %e, region = %d\n", 
                  columnData[i-1], columnData[i], region);
#endif
          if (!SDDS_SetRowFlags(&SDDSold, 0) || 
              !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
              !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            return(1);
          }
          newStart = i;
          lastRegion = region;
#ifdef DEBUG
          fprintf(stderr, "start value = %e\n", columnData[i]);
#endif
        }
      }
      if (dataType!=SDDS_STRING)
        free(columnData);
      else 
        SDDS_FreeStringArray(stringData, rows);
      break;
    case SET_MATCHTO:
      if (!(stringData=SDDS_GetColumn(&SDDSold, columnName))) {
        SDDS_SetError("unable to read specified column");
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
      newStart = 0;
      breakNext = 0;
      for (i=1; i<=rows; i++) {
        if (i!=rows && !breakNext) {
          if (wild_match(stringData[i], matchPattern)) {
            if (matchPatternAfter) {
              breakNext = 1;
              continue;
            }
          } else 
            continue;
        }
        if (!SDDS_SetRowFlags(&SDDSold, 0) || 
            !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, newStart, i-1, 1) ||
            !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        breakNext = 0;
        newStart = i;
      }
      SDDS_FreeStringArray(stringData, rows);
      break;
    case SET_ROWLIMIT:
      for (i=0; i<rows; i+=rowLimit) {
        if ((j=i+rowLimit-1)>=rows)
          j = rows-1;
        if (!SDDS_SetRowFlags(&SDDSold, 0) || 
            !SDDS_AssertRowFlags(&SDDSold, SDDS_INDEX_LIMITS, i, j, 1) ||
            !SDDS_CopyRowsOfInterest(&SDDSnew, &SDDSold) || !SDDS_WritePage(&SDDSnew)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      }
      break;
    default:
      fprintf(stderr, "Error: unknown break mode code seen---this can't happen\n");
      return(1);
    }
  }
  if (retval==0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_Terminate(&SDDSold)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (tmpfile_used && !replaceFileAndBackUp(input, output)) {
    return(1);
  }
  return(0);
}

