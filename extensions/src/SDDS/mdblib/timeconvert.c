/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: timeconvert.c
 * 
 * Michael Borland, 1996
 $Log: not supported by cvs2svn $
 Revision 1.8  2007/02/01 21:25:18  soliday
 Fixed an odd issue seen on Windows.

 Revision 1.7  2002/08/14 16:06:16  soliday
 Added Open License

 Revision 1.6  1999/05/25 21:55:48  soliday
 Removed compiler warnings under Linux.

 Revision 1.5  1997/03/20 20:09:03  borland
 Improved computation of hour of day from epoch time.  Now correctly
 keeps fractional seconds.

 * Revision 1.4  1996/03/28  04:57:56  borland
 * Changed from long integers to short integers throughout.
 *
 * Revision 1.3  1996/03/07  02:35:52  borland
 * Fixed bug in conversion of date breakdown to time-since-epoch (wasn't
 * setting isdst=-1).
 *
 * Revision 1.2  1996/03/07  02:13:56  borland
 * Added time conversion routines to Makefile; added routine to make breakdown
 * of time-since-epoch.
 *
 * Revision 1.1  1996/03/01  04:12:43  borland
 * First version.
 *
 *
 */
#include "mdb.h"
#include <time.h>

short IsLeapYear(short year)
{
  if (year<0)
    return -1;
  year = year<100 ? (year>95 ? year+1900 : year+2000) : year;
  if (year%4==0 && (year%100!=0 || year%400==0))
    return 1;
  return 0;
}

/* days in each month for leap years, nonleap years */
static short DaysInMonths[2][12] = {
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
 },
{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
 },
};

short JulianDayFromMonthDay(short month, short day, short year, short *julianDay)
{
  short leapYear, jday, i, daysInMonth;

  if (year<=0 || month<1 || month>12 || day<1 || !julianDay)
    return 0;

  leapYear = IsLeapYear(year);
  daysInMonth = DaysInMonths[leapYear][month-1];
  if (day>daysInMonth) 
    return 0;
  
  jday = day;
  for (i=1; i<month; i++)
    jday += DaysInMonths[leapYear][i-1];
  *julianDay = jday;
  return 1;
}

short MonthDayFromJulianDay(short julianDay, short year, short *month, short *day)
{
  short leapYear, sum, i, days;
  
  if (julianDay<0 || julianDay>366 || year<=0 || !month || !day)
    return 0;
  leapYear = IsLeapYear(year);
  if ((leapYear==0 && julianDay>=365) || julianDay>=366) {
    *month = 12;
    *day = 31;
    return 1;
    }

  sum = 0;
  for (i=1; i<=12; i++) {
    days = DaysInMonths[leapYear][i-1];
    if (sum+days<julianDay) {
      sum += days;
    } else
      break;
  }
  *month = i;
  *day = julianDay-sum;
  return 1;
}

#include <time.h>
short TimeEpochToBreakdown(short *year, short *jDay, short *month, short *day, double *hour, double epochTime)
{
  struct tm *timeBreakdown;
  double dayStartTime;
  short lyear, ljDay, lhour;
  time_t theTime;
  theTime = epochTime;
  if (!(timeBreakdown = localtime(&theTime)))
    return 0;
  lyear = timeBreakdown->tm_year+1900;
  ljDay = timeBreakdown->tm_yday+1;
  lhour = timeBreakdown->tm_hour;
  if (year)
    *year = lyear;
  if (jDay)
    *jDay = ljDay;
  if (month)
    *month = timeBreakdown->tm_mon+1;
  if (day)
    *day = timeBreakdown->tm_mday;
  if (hour) {
    /* go through some contortions to preserve fractional seconds */
    TimeBreakdownToEpoch(lyear, ljDay, (short)0, (short)0, (double)0.0, &dayStartTime);
    *hour = (epochTime-dayStartTime)/3600;
    if (((short)*hour)!=lhour) {
      /* daylight savings time problem? */
      *hour = *hour + lhour - ((short)*hour);
    }
  }
  return 1;
}

short TimeEpochToText(char *text, double epochTime)
{
  short year, jDay, month, day, hr, min;
  double dayTime, sec;
  if (!TimeEpochToBreakdown(&year, &jDay, &month, &day, &dayTime, epochTime))
    return 0;
  hr = dayTime;
  min = 60*(dayTime-hr);
  sec = 3600.0*dayTime-(3600.0*hr+60.0*min);
  sprintf(text, "%04hd/%02hd/%02hd %02hd:%02hd:%07.4f",
          year, month, day, hr, min, sec);
  return 1;
}

short TimeBreakdownToEpoch(short year, short jDay, short month, short day, double hour, double *epochTime)
{
  struct tm timeBreakdown;
  short imin, ihour;
  double fsec, fmin;

  if (!epochTime)
    return 0;
  memset((char*)&timeBreakdown, 0, sizeof(timeBreakdown));
  if (year>100)
    timeBreakdown.tm_year = year-1900;
  else
    timeBreakdown.tm_year = year;
  if (jDay) {
    short iday, imonth;
    if (!MonthDayFromJulianDay(jDay, year, &imonth, &iday)) {
      return 0;
    }
    timeBreakdown.tm_mday = iday;
    timeBreakdown.tm_mon  = imonth-1;
  } else {
    timeBreakdown.tm_mday = day;
    timeBreakdown.tm_mon  = month-1;
  }
  /* Break floating-point hours into integer H:M:S plus fractional seconds */
  ihour = timeBreakdown.tm_hour = hour;
  imin  = timeBreakdown.tm_min  = fmin = 60*(hour-ihour);
  timeBreakdown.tm_sec  = fsec = 60*(fmin-imin);
  fsec -= timeBreakdown.tm_sec;
  timeBreakdown.tm_isdst = -1;
#if defined(SUNOS4)
  *epochTime = timelocal(&timeBreakdown) + fsec;
#else
  *epochTime = mktime(&timeBreakdown) + fsec;
#endif
  return 1;
}

