/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddstimeconvert
 * purpose: do time conversions on SDDS data.
 *
 * Michael Borland, 1996
 $Log: sddstimeconvert.c,v $
 Revision 1.14  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.13  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.12  2002/08/14 17:12:56  soliday
 Added Open License

 Revision 1.11  2002/06/05 20:42:54  soliday
 Changed slightly so Microsoft Visual Studio 7 doesn't bomb.

 Revision 1.10  2002/01/16 16:55:51  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.9  2001/01/10 19:35:49  soliday
 Standardized usage message.

 Revision 1.8  1999/05/25 19:17:03  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:58  borland
 Fixed the version number in the usage message.

 Revision 1.6  1997/03/20 20:10:03  borland
 Added text qualifier to -breakDown option, which results in printing of
 a text string with the time in it.

 Revision 1.5  1996/08/07 14:46:33  borland
 Fixed bug in parameter conversion from time-since-epoch.  Wasn't properly
 terminating the variable argument list for SetParametersFromDoubles calls.

 * Revision 1.4  1996/05/01  15:55:25  borland
 * Added exit(0) for normal exits.
 *
 * Revision 1.3  1996/03/28  05:45:33  borland
 * Added -breakdown option (inverse of -epoch).  First working version.
 *
 * Revision 1.2  1996/03/24  23:19:23  borland
 * Improved usage message.  Still not a working version.
 *
 * Revision 1.1  1996/03/21  17:20:59  borland
 * First version.  Probably doesn't work.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_EPOCH 0
#define SET_PIPE 1
#define SET_BREAKDOWN 2
#define N_OPTIONS 3

char *option[N_OPTIONS] = {
  "epoch", "pipe", "breakdown",
} ;

char *USAGE = "sddstimeconvert [<SDDSinput>] [<SDDSoutput>] [-pipe=[input][,output]]\n\
-breakdown={column|parameter},<timeName>[,year=<newName>][,julianDay=<newName>][,month=<newName>][,day=<newName>][,hour=<newName>][,text=<newName>]\n\
-epoch={column|parameter},<newName>,year=<name>,[julianDay=<name>|month=<name>,day=<name>],hour=<name>...\n\n\
-pipe      Standard SDDS Toolkit pipe option.\n\
-breakdown Creates new columns or parameters containing results of breaking down the\n\
           time-since-epoch value in <timeName> into the requested components.\n\
-epoch     Creates a new column or parameter <newName> containing the time-since-epoch\n\
           in seconds equivalent to the Julian day and year or day, month, and year given\n\
           in the named columns or parameters.\n\n\
Program by Michael Borland. (This is version 2, March 1997.)\n";

#define IS_COLUMN    0x0001U
#define IS_PARAMETER 0x0002U
#define EPOCH_GIVEN  0x0004U
#define YEAR_GIVEN   0x0008U
#define JDAY_GIVEN   0x0010U
#define MONTH_GIVEN  0x0020U
#define DAY_GIVEN    0x0040U
#define HOUR_GIVEN   0x0080U
#define DO_BREAKDOWN 0x0100U
#define DO_EPOCH     0x0200U
#define TEXT_GIVEN   0x0400U

typedef struct {
  char *epochName, *yearName, *jDayName, *monthName, *dayName, *hourName;
  char *textName;
  long epochIndex, yearIndex, jDayIndex, monthIndex, dayIndex, hourIndex;
  long textIndex;
  unsigned long flags;
} TIME_CONVERSION;

void DoColumnEpochConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                        TIME_CONVERSION *conversion);
void DoParameterEpochConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                           TIME_CONVERSION *conversion);
void DoColumnBreakdownConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                        TIME_CONVERSION *conversion);
void DoParameterBreakdownConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                           TIME_CONVERSION *conversion);
void InitializeOutput(SDDS_DATASET *SDDSout, char *outputfile, TIME_CONVERSION *conversion,
                      long conversions, SDDS_DATASET *SDDSin);
void CheckEpochConversionElements(SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion,
                             long conversions);
void CheckBreakdownConversionElements(SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion,
                             long conversions);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDSin, SDDSout;
  long i_arg, iconv;
  SCANNED_ARG *s_arg;
  char *input, *output;
  TIME_CONVERSION *conversion;
  long conversions;
  unsigned long pipeFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);

  input = output = NULL;
  conversions = 0;
  conversion = NULL;
  pipeFlags = 0;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_EPOCH:
        if (s_arg[i_arg].n_items<4)
          SDDS_Bomb("Invalid -epoch syntax");
        if (!(conversion 
              = SDDS_Realloc(conversion,
                             sizeof(*conversion)*(conversions+1))))
          SDDS_Bomb("Memory allocation failure");
        memset((char*)(conversion+conversions), 0, sizeof(*conversion));
        conversion[conversions].epochName = s_arg[i_arg].list[2];
        s_arg[i_arg].list[2] = s_arg[i_arg].list[1];
        s_arg[i_arg].n_items -= 2;
        if (!scanItemList
            (&conversion[conversions].flags, 
             s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
             "column", -1, NULL, 0, IS_COLUMN,
             "parameter", -1, NULL, 0, IS_PARAMETER,
             "year", SDDS_STRING, &conversion[conversions].yearName, 1, YEAR_GIVEN,
             "julianday", SDDS_STRING, &conversion[conversions].jDayName, 1, JDAY_GIVEN,
             "month", SDDS_STRING, &conversion[conversions].monthName, 1, MONTH_GIVEN,
             "day", SDDS_STRING, &conversion[conversions].dayName, 1, DAY_GIVEN,
             "hour", SDDS_STRING, &conversion[conversions].hourName, 1, HOUR_GIVEN,
             NULL))
          SDDS_Bomb("invalid -epoch syntax");
        conversion[conversions].flags |= EPOCH_GIVEN+DO_EPOCH;
        if (!(conversion[conversions].flags&(IS_COLUMN|IS_PARAMETER)))
          SDDS_Bomb("give column or parameter qualifier with -epoch");
        if (conversion[conversions].flags&IS_COLUMN &&
            conversion[conversions].flags&IS_PARAMETER)
          SDDS_Bomb("give only one of column or parameter qualifier with -epoch");
        if (!(conversion[conversions].flags&YEAR_GIVEN))
          SDDS_Bomb("give year name with -epoch");
        if (!(conversion[conversions].flags&JDAY_GIVEN) &&
            (conversion[conversions].flags&(MONTH_GIVEN|DAY_GIVEN))!=
            (MONTH_GIVEN|DAY_GIVEN)) 
          SDDS_Bomb("give either julianDay name, or month and day names with -epoch");
        if (conversion[conversions].flags&JDAY_GIVEN &&
            conversion[conversions].flags&(MONTH_GIVEN|DAY_GIVEN))
          SDDS_Bomb("improper combination of julianDay name with month or day name for -epoch");
        conversions++;
        break;
      case SET_BREAKDOWN:
        if (s_arg[i_arg].n_items<4)
          SDDS_Bomb("Invalid -breakdown syntax");
        if (!(conversion 
              = SDDS_Realloc(conversion,
                             sizeof(*conversion)*(conversions+1))))
          SDDS_Bomb("Memory allocation failure");
        memset((char*)(conversion+conversions), 0, sizeof(*conversion));
        conversion[conversions].epochName = s_arg[i_arg].list[2];
        s_arg[i_arg].list[2] = s_arg[i_arg].list[1];
        s_arg[i_arg].n_items -= 2;
        if (!scanItemList
            (&conversion[conversions].flags, 
             s_arg[i_arg].list+2, &s_arg[i_arg].n_items, 0,
             "column", -1, NULL, 0, IS_COLUMN,
             "parameter", -1, NULL, 0, IS_PARAMETER,
             "year", SDDS_STRING, &conversion[conversions].yearName, 1, YEAR_GIVEN,
             "julianday", SDDS_STRING, &conversion[conversions].jDayName, 1, JDAY_GIVEN,
             "month", SDDS_STRING, &conversion[conversions].monthName, 1, MONTH_GIVEN,
             "day", SDDS_STRING, &conversion[conversions].dayName, 1, DAY_GIVEN,
             "hour", SDDS_STRING, &conversion[conversions].hourName, 1, HOUR_GIVEN,
             "text", SDDS_STRING, &conversion[conversions].textName, 1, TEXT_GIVEN,
             NULL))
          SDDS_Bomb("invalid -breakdown syntax");
        conversion[conversions].flags |= EPOCH_GIVEN+DO_BREAKDOWN;
        if (!(conversion[conversions].flags&(IS_COLUMN|IS_PARAMETER)))
          SDDS_Bomb("give column or parameter qualifier with -breakdown");
        if (conversion[conversions].flags&IS_COLUMN &&
            conversion[conversions].flags&IS_PARAMETER)
          SDDS_Bomb("give only one of column or parameter qualifier with -breakdown");
        if (!(conversion[conversions].flags&(YEAR_GIVEN+JDAY_GIVEN+
                                           MONTH_GIVEN+DAY_GIVEN+HOUR_GIVEN+TEXT_GIVEN))) 
          SDDS_Bomb("give at least one of year, julianDay, month, day, hour, or text qualifiers with -conversion");
        conversions++;
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      default:
        fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
        SDDS_Bomb(NULL);
        break;
      }
    }else {
      if (!input)
        input = s_arg[i_arg].list[0];
      else if (!output)
        output = s_arg[i_arg].list[0];
      else {
        fprintf(stderr, "argument %s is invalid: too many filenames (sddstimeocnvert)\n",
                s_arg[i_arg].list[0]);
        exit(1);
      }
    }
  }

  processFilenames("sddstimeconvert", &input, &output, pipeFlags, 0, NULL);

  if (!SDDS_InitializeInput(&SDDSin, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  CheckEpochConversionElements(&SDDSin, conversion, conversions);
  CheckBreakdownConversionElements(&SDDSin, conversion, conversions);

  InitializeOutput(&SDDSout, output, conversion, conversions, &SDDSin);
  
  while (SDDS_ReadPage(&SDDSin)>0) {
    if (!SDDS_CopyPage(&SDDSout, &SDDSin)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    for (iconv=0; iconv<conversions; iconv++) {
      if (conversion[iconv].flags&DO_EPOCH) {
        if (conversion[iconv].flags&IS_PARAMETER)
          DoParameterEpochConversion(&SDDSout, &SDDSin, conversion+iconv);
        else 
          DoColumnEpochConversion(&SDDSout, &SDDSin, conversion+iconv);
      } else {
        if (conversion[iconv].flags&IS_PARAMETER)
          DoParameterBreakdownConversion(&SDDSout, &SDDSin, conversion+iconv);
        else 
          DoColumnBreakdownConversion(&SDDSout, &SDDSin, conversion+iconv);
      }
    }
    if (!SDDS_WritePage(&SDDSout)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  return(0);
}


void CheckEpochConversionElements(SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion,
                                  long conversions)
{
  while (conversions-- > 0) {
    if (!(conversion[conversions].flags&DO_EPOCH))
      continue;
    if (conversion->flags&IS_PARAMETER) {
      if (SDDS_CheckParameter(SDDSin, conversion->yearName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK ||
          (conversion->jDayName &&
           SDDS_CheckParameter(SDDSin, conversion->jDayName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          (conversion->dayName &&
           SDDS_CheckParameter(SDDSin, conversion->dayName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          (conversion->monthName &&
           SDDS_CheckParameter(SDDSin, conversion->monthName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          SDDS_CheckParameter(SDDSin, conversion->hourName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      conversion->yearIndex = SDDS_GetParameterIndex(SDDSin, conversion->yearName);
      conversion->hourIndex = SDDS_GetParameterIndex(SDDSin, conversion->hourName);
      conversion->dayIndex = conversion->jDayIndex = conversion->monthIndex = -1;
      if (conversion->dayName)
        conversion->dayIndex = SDDS_GetParameterIndex(SDDSin, conversion->dayName);
      if (conversion->jDayName)
        conversion->jDayIndex = SDDS_GetParameterIndex(SDDSin, conversion->jDayName);
      if (conversion->monthName)
        conversion->monthIndex = SDDS_GetParameterIndex(SDDSin, conversion->monthName);
    } else {
      if (SDDS_CheckColumn(SDDSin, conversion->yearName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK ||
          (conversion->jDayName &&
           SDDS_CheckColumn(SDDSin, conversion->jDayName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          (conversion->dayName &&
           SDDS_CheckColumn(SDDSin, conversion->dayName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          (conversion->monthName &&
           SDDS_CheckColumn(SDDSin, conversion->monthName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK) ||
          SDDS_CheckColumn(SDDSin, conversion->hourName, NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      conversion->yearIndex = SDDS_GetColumnIndex(SDDSin, conversion->yearName);
      conversion->hourIndex = SDDS_GetColumnIndex(SDDSin, conversion->hourName);
      conversion->dayIndex = conversion->jDayIndex = conversion->monthIndex = -1;
      if (conversion->dayName)
        conversion->dayIndex = SDDS_GetColumnIndex(SDDSin, conversion->dayName);
      if (conversion->jDayName)
        conversion->jDayIndex = SDDS_GetColumnIndex(SDDSin, conversion->jDayName);
      if (conversion->monthName)
        conversion->monthIndex = SDDS_GetColumnIndex(SDDSin, conversion->monthName);
    }
    conversion++;
  }
}

void CheckBreakdownConversionElements(SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion,
                                      long conversions)
{
  while (conversions-- > 0) {
    if (!(conversion[conversions].flags&DO_BREAKDOWN))
      continue;
    if (conversion->flags&IS_PARAMETER) {
      if (SDDS_CheckParameter(SDDSin, conversion->epochName, NULL, 
                              SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      conversion->epochIndex = SDDS_GetParameterIndex(SDDSin, conversion->epochName);
    } else {
      if (SDDS_CheckColumn(SDDSin, conversion->epochName, NULL, 
                              SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OK)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      conversion->epochIndex = SDDS_GetColumnIndex(SDDSin, conversion->epochName);
    }
    conversion++;
  }
}

void InitializeOutput(SDDS_DATASET *SDDSout, char *outputfile, TIME_CONVERSION *conversion,
                      long conversions, SDDS_DATASET *SDDSin)
{
  if (!SDDS_InitializeCopy(SDDSout, SDDSin, outputfile, "w"))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  while (conversions-- > 0) {
    if (conversion->flags&DO_EPOCH) {
      if (conversion->flags&IS_PARAMETER) {
        if ((conversion->epochIndex=
             SDDS_DefineParameter(SDDSout, conversion->epochName, NULL, "s", "Time since start of epoch",
                                  NULL, SDDS_DOUBLE, NULL))<0)
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      } else {
        if ((conversion->epochIndex=
             SDDS_DefineColumn(SDDSout, conversion->epochName, NULL, "s", "Time since start of epoch",
                               NULL, SDDS_DOUBLE, 0))<0)
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    } else {
      if (conversion->flags&IS_PARAMETER) {
        if ((conversion->yearName &&
             (conversion->yearIndex=SDDS_DefineParameter(SDDSout, conversion->yearName, NULL, NULL, NULL, NULL,
                                                         SDDS_SHORT, NULL))<0) ||
            (conversion->dayName &&
             (conversion->dayIndex=SDDS_DefineParameter(SDDSout, conversion->dayName, NULL, NULL, "Day of month", 
                                                        NULL, SDDS_SHORT, NULL))<0) ||
            (conversion->monthName &&
             (conversion->monthIndex=SDDS_DefineParameter(SDDSout, conversion->monthName, NULL, NULL, NULL, NULL,
                                                          SDDS_SHORT, NULL))<0) ||
            (conversion->jDayName &&
             (conversion->jDayIndex=SDDS_DefineParameter(SDDSout, conversion->jDayName, NULL, NULL, "Julian day", 
                                                         NULL, SDDS_SHORT, NULL))<0) ||
            (conversion->hourName &&
             (conversion->hourIndex=SDDS_DefineParameter(SDDSout, conversion->hourName, NULL, NULL, "Hour of day", 
                                                         NULL, SDDS_DOUBLE, NULL))<0) || 
            (conversion->textName &&
             (conversion->textIndex=SDDS_DefineParameter(SDDSout, conversion->textName, NULL, NULL, "Timestamp",
                                                        NULL, SDDS_STRING, NULL))<0) )
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      } else {
        if ((conversion->yearName &&
             (conversion->yearIndex=SDDS_DefineColumn(SDDSout, conversion->yearName, NULL, NULL, NULL, NULL,
                                                      SDDS_SHORT, 0))<0) ||
            (conversion->dayName &&
             (conversion->dayIndex=SDDS_DefineColumn(SDDSout, conversion->dayName, NULL, NULL, "Day of month", 
                                                     NULL, SDDS_SHORT, 0))<0) ||
            (conversion->monthName &&
             (conversion->monthIndex=SDDS_DefineColumn(SDDSout, conversion->monthName, NULL, NULL, NULL, NULL,
                                                       SDDS_SHORT, 0))<0) ||
            (conversion->jDayName &&
             (conversion->jDayIndex=SDDS_DefineColumn(SDDSout, conversion->jDayName, NULL, NULL, "Julian day", 
                                                      NULL, SDDS_SHORT, 0))<0) ||
            (conversion->hourName &&
             (conversion->hourIndex=SDDS_DefineColumn(SDDSout, conversion->hourName, NULL, NULL, "Hour of day", 
                                                      NULL, SDDS_DOUBLE, 0))<0) ||
            (conversion->textName &&
             (conversion->textIndex=SDDS_DefineColumn(SDDSout, conversion->textName, NULL, NULL, "Timestamp",
                                                      NULL, SDDS_STRING, 0))<0))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
    conversion++;
  }
  if (!SDDS_WriteLayout(SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}


void DoParameterEpochConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion)
{
  double hour;
  double month, day, jDay, year, epochTime;

  jDay = 0;
  if (!SDDS_GetParameterAsDouble(SDDSin, conversion->hourName, &hour) ||
      !SDDS_GetParameterAsDouble(SDDSin, conversion->yearName, &year) ||
      (conversion->jDayName &&
       !SDDS_GetParameterAsDouble(SDDSin, conversion->jDayName, &jDay)) ||
      (conversion->monthName &&
       !SDDS_GetParameterAsDouble(SDDSin, conversion->monthName, &month)) ||
      (conversion->dayName &&
       !SDDS_GetParameterAsDouble(SDDSin, conversion->dayName, &day)) )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  TimeBreakdownToEpoch((short)year, (short)jDay, (short)month, (short)day, hour, &epochTime);
  if (!SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_NAME|SDDS_PASS_BY_VALUE,
                                     conversion->epochName, epochTime, NULL))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}

void DoParameterBreakdownConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                                    TIME_CONVERSION *conversion)
{
  double hour, epochTime;
  short year, jDay, month, day;
  char text[30];
  
  if (!SDDS_GetParameterAsDouble(SDDSin, conversion->epochName, &epochTime))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  TimeEpochToBreakdown(&year, &jDay, &month, &day, &hour, epochTime);
  TimeEpochToText(text, epochTime);
  if ((conversion->yearName &&
       !SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                                      conversion->yearIndex, (double)year, -1)) ||
      (conversion->dayName &&
       !SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                                      conversion->dayIndex, (double)day, -1)) ||
      (conversion->jDayName &&
       !SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                                      conversion->jDayIndex, (double)jDay, -1)) ||
      (conversion->monthName &&
       !SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                                      conversion->monthIndex, (double)month, -1)) ||
      (conversion->hourName &&
       !SDDS_SetParametersFromDoubles(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                                      conversion->hourIndex, (double)hour, -1)) ||
      (conversion->textName &&
       !SDDS_SetParameters(SDDSout, SDDS_BY_INDEX|SDDS_PASS_BY_VALUE,
                           conversion->textIndex, text, -1)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}


void DoColumnEpochConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, TIME_CONVERSION *conversion)
{
  double *hour;
  double *month, *day, *jDay, *year, *epochTime;
  long row, rows;

  year = NULL;

  if (!(rows=SDDS_CountRowsOfInterest(SDDSin)))
    return;

  jDay = month = day = NULL;
  if (!(hour=SDDS_GetColumnInDoubles(SDDSin, conversion->hourName)) ||
      !(year=SDDS_GetColumnInDoubles(SDDSin, conversion->yearName)) ||
      (conversion->jDayName &&
       !(jDay=SDDS_GetColumnInDoubles(SDDSin, conversion->jDayName))) ||
      (conversion->monthName &&
       !(month=SDDS_GetColumnInDoubles(SDDSin, conversion->monthName))) ||
      (conversion->dayName &&
       !(day=SDDS_GetColumnInDoubles(SDDSin, conversion->dayName))))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!(epochTime=(double*)malloc(sizeof(*epochTime)*rows)))
    SDDS_Bomb("memory allocation failure");
    
  for (row=0; row<rows; row++)
      TimeBreakdownToEpoch((short)year[row], jDay?(short)jDay[row]:0, 
                           month?(short)month[row]:0, day?(short)day[row]:0, hour[row],
                           epochTime+row);
  if (!SDDS_SetColumnFromDoubles(SDDSout, SDDS_BY_NAME, epochTime, 
                                 rows, conversion->epochName))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  free(hour);
  free(year);
  free(epochTime);
  if (jDay)
    free(jDay);
  if (month)
    free(month);
  if (day)
    free(day);
}

void DoColumnBreakdownConversion(SDDS_DATASET *SDDSout, SDDS_DATASET *SDDSin, 
                             TIME_CONVERSION *conversion)
{
  double *hour, *epochTime;
  short *month, *day, *jDay, *year;
  char **text;
  long row, rows;
  
  if (!(rows=SDDS_CountRowsOfInterest(SDDSin)))
    return;

  if (!(epochTime=SDDS_GetColumnInDoubles(SDDSin, conversion->epochName)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  hour = NULL;
  month = day = jDay = year = NULL;
  text = NULL;
  if ((conversion->hourName && !(hour=(double*)malloc(sizeof(*hour)*rows))) ||
      (conversion->monthName && !(month=(short*)malloc(sizeof(*month)*rows))) ||
      (conversion->dayName && !(day=(short*)malloc(sizeof(*day)*rows))) ||
      (conversion->jDayName && !(jDay=(short*)malloc(sizeof(*jDay)*rows))) ||
      (conversion->textName && !(text=(char **)malloc(sizeof(*text)*rows))) ||
      (conversion->yearName && !(year=(short*)malloc(sizeof(*year)*rows)))
      )
    SDDS_Bomb("memory allocation failure");
    
  for (row=0; row<rows; row++) {
    if (text)
      text[row] = malloc(sizeof(**text)*30);
    if (!TimeEpochToBreakdown(year?year+row:NULL, 
                              jDay?jDay+row:NULL, 
                              month?month+row:NULL, 
                              day?day+row:NULL, 
                              hour?hour+row:NULL,
                              epochTime[row]) ||
        (text && !TimeEpochToText(text[row], epochTime[row])))
      SDDS_Bomb("problem doing time breakdown");
    }
  

  if ((year && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, year,
                               rows, conversion->yearName)) ||
      (day && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, day,
                               rows, conversion->dayName)) ||
      (month && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, month,
                               rows, conversion->monthName)) ||
      (jDay && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, jDay,
                               rows, conversion->jDayName)) ||
      (hour && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, hour,
                               rows, conversion->hourName)) || 
      (text && !SDDS_SetColumn(SDDSout, SDDS_BY_NAME, text, 
                               rows, conversion->textName)))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (hour)
    free(hour);
  if (year)
    free(year);
  if (epochTime)
    free(epochTime);
  if (jDay)
    free(jDay);
  if (month)
    free(month);
  if (day)
    free(day);
  if (text) {
    for (row=0; row<rows; row++)
      free(text[row]);
    free(text);
  }
}




