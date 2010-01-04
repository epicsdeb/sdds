/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*move following functions (except those functions that related to the
  epics) originally located at SDDSepics.c and SDDSepics.h to mdblib 
  area so that other prorams could use them.
  
  Hairong Shang, May 2002
  $Log: logfile_gener.c,v $
  Revision 1.4  2009/08/28 20:46:21  soliday
  Updated to work with cygwin-x86

  Revision 1.3  2008/03/07 16:46:27  soliday
  Added the MakeMonthlyGenerationFilename function.

  Revision 1.2  2007/07/16 16:11:23  soliday
  Updated to work with Microsoft VC 2005

  Revision 1.1  2007/04/16 21:02:51  soliday
  Moved from mdbcommon after I removed SDDS calls.

  Revision 1.8  2007/04/16 18:45:53  soliday
  Removed SDDS calls.

  Revision 1.7  2004/05/05 20:56:39  norume
  Allow compilation on RTEMS.

  Revision 1.6  2003/08/28 15:06:59  soliday
  Added locking functions for OSX

  Revision 1.5  2003/06/15 20:53:55  soliday
  Added AIX support

  Revision 1.4  2002/08/14 16:48:28  soliday
  Added Open License

  Revision 1.3  2002/07/31 14:34:56  soliday
  Added support for Mac

  Revision 1.2  2002/07/30 08:55:48  soliday
  Made minor changes for vxworks.

  Revision 1.1  2002/07/24 20:50:25  shang
  moved non-epices functions from SDDSepics.c and made changes for more requests

  
*/

#include "mdb.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <sys/locking.h>
#include <sys/types.h>
#include <sys/timeb.h>
#if defined(__BORLANDC__)
#define lockf(handle, mode, nbytes) locking(handle, mode, nbytes)
#else
#define lockf(handle, mode, nbytes) _locking(handle, mode, nbytes)
#endif
#define usleep(usecs) Sleep(usecs / 1000)
#define popen(x,y) _popen(x,y)
#define F_TEST 3
#else
#include <unistd.h>
#if defined(__APPLE__) 
/*lockf is not defined by unistd.h like it should be*/
int lockf(int filedes, int function, off_t size);
#endif
#endif
#ifdef SOLARIS
#if defined(BELOW_SOLARIS_56)
#include <signal.h>
#include <sys/time.h>

typedef void (*OLD_SIG_FUNC)(int);
static void usleep_alarm(int x) {
  return;
}

void usleep(long usecs)
{
  struct itimerval v;
  OLD_SIG_FUNC old;
  int rc=0;

  if (usecs<1)
    return;
  
  v.it_value.tv_sec=usecs/1000000;
  v.it_value.tv_usec=usecs%1000000;
  
  v.it_interval.tv_sec=0;
  v.it_interval.tv_usec=0;
 
  old=signal(SIGALRM,usleep_alarm);
  if (setitimer(ITIMER_REAL,&v,(struct itimerval*)NULL)<0)
    rc=-1;
  else
    pause();
  
  signal(SIGALRM,old);
}
#endif
#endif

#include <time.h>
char *makeTimeStamp(double Time)
{
    time_t intTime;
    char *TimeStamp;
    intTime = Time;
    TimeStamp = ctime(&intTime);
    TimeStamp[strlen(TimeStamp)-1] = 0; /* kill newline */
    return TimeStamp;
    }

void getTimeBreakdown(double *ptrTime, double *ptrDay, double *ptrHour, 
                      double *ptrJulianDay, double *ptrMonth,
                      double *ptrYear, char **ptrTimeStamp)
{
    double theTime;
    theTime = getTimeInSecs();   /* may get more precision than time() offers */
    makeTimeBreakdown(theTime, ptrTime, ptrDay, ptrHour, ptrJulianDay,
                      ptrMonth, ptrYear, ptrTimeStamp);
    }

double getHourOfDay()
{
  double theTime, hour;
  theTime = getTimeInSecs();   /* may get more precision than time() offers */
  makeTimeBreakdown(theTime, NULL, NULL, &hour, NULL, NULL, NULL, NULL);
  return hour;
}

static long daysInMonth[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 
  };
    
void makeTimeBreakdown(double Time, double *ptrTime, double *ptrDay, double *ptrHour,
                       double *ptrJulianDay, double *ptrMonth, double *ptrYear, 
                       char **ptrTimeStamp)
{
    double SubSeconds;
    double Day, Hour, JulianDay, Year;
    struct tm *tmBreakdown;
    time_t integerTime;
    long isLeap;

    
    if (ptrTime)
        *ptrTime = Time;
    if (ptrTimeStamp)
        *ptrTimeStamp = makeTimeStamp(Time);

    /* get breakdown based on integer part of time */
    integerTime = (time_t)Time;
    SubSeconds = Time - integerTime;
    tmBreakdown = localtime(&integerTime);
    
    /* time since midnight in hours */
    Hour = (tmBreakdown->tm_min + (tmBreakdown->tm_sec+SubSeconds)/60.0)/60.0 + tmBreakdown->tm_hour;
    if (ptrHour)
        *ptrHour = Hour;

    /* time since beginning of month in days (includes partial days) */
    Day = tmBreakdown->tm_mday + Hour/24.0;
    if (ptrDay)
        *ptrDay = Day;

    /* time since start of year in days (includes partial days) plus 1 day to give Julian convention */
    JulianDay = tmBreakdown->tm_yday + Hour/24.0 + 1;
    if (ptrJulianDay)
        *ptrJulianDay = JulianDay;

    /* years since 0000 AD, including fractional years */
    isLeap = 0;
    tmBreakdown->tm_year += 1900;
    if ((tmBreakdown->tm_year%4==0 && tmBreakdown->tm_year%100!=0) || tmBreakdown->tm_year%400==0)
        isLeap = 1;
    Year = tmBreakdown->tm_year + (JulianDay-1)/(365.0+isLeap);
    if (ptrYear)
        *ptrYear = Year;

    /* time since start of year in 30-day months (includes partial days) */
    if (ptrMonth) {
        *ptrMonth = tmBreakdown->tm_mon + 1 +
            + (1.0*tmBreakdown->tm_mday-1)/(daysInMonth[tmBreakdown->tm_mon] + 
                                         (isLeap && tmBreakdown->tm_mon==1?1:0));
        }
    
    }

double computeYearStartTime(double StartTime)
{
    struct tm *YearStart;
    time_t intTime;

    intTime = StartTime;
    YearStart = localtime(&intTime);
    YearStart->tm_sec=0;
    YearStart->tm_min=0;
    YearStart->tm_hour=0;
    YearStart->tm_mday=1;
    YearStart->tm_mon=0;
#if defined(SUNOS4)
    return (double)timelocal(YearStart);
#else
    return (double)mktime(YearStart);
#endif
    }

double getTimeInSecs()
{
#if !defined(SOLARIS) && !defined(vxWorks) && !defined(__APPLE__) && !defined(_AIX)
#include <sys/types.h>
#include <sys/timeb.h>
    struct timeb theTime;
    ftime(&theTime);
    return ((double)theTime.time) + ((double)theTime.millitm)/1000.0;
#else
#if defined(__APPLE__)
#include <sys/time.h>
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return((double)tv.tv_sec) + ((double)tv.tv_usec)/1e6;
#else
#include <time.h>
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return ((double)tp.tv_sec) + ((double)tp.tv_nsec)/1e9;
#endif
#endif
  }

char *MakeGenerationFilename(char *rootname, long digits, char *delimiter, char *lastFile)
{
  char format[100], filename[1024], buffer[1024], *name, *ptr, *ptr1;
  long index = 1;
  FILE *fp;
    
  if (!rootname || strlen(rootname)==0)
    return NULL;
  if (digits<1)
    digits = DEFAULT_GENERATIONS_DIGITS;
   
  if (lastFile && strlen(lastFile)) {
    ptr1 = lastFile;
    ptr = NULL;
    while ((ptr1=strstr(ptr1, delimiter))) {
      ptr = ptr1;
      ptr1 += 1;
    }
    if (ptr) {
      ptr += strlen(delimiter);
      while (*ptr=='0')
	ptr++;
      if (sscanf(ptr, "%ld", &index)!=1) {
	sprintf(buffer, "Error scanning name of last file: %s", lastFile);
	fprintf(stderr, "Error: %s\n", buffer);
        exit(1);
      }
    } else {
      sprintf(buffer, "Error scanning name of last file: %s", lastFile);
      fprintf(stderr, "Error: %s\n", buffer);
      exit(1);
    }
    if (!fexists(lastFile))
      index += 1;   /* avoids reuse of the file even if it has been deleted */
  }
    
  sprintf(format, "%%s%s%%0%ldld", delimiter, digits);

  do {
    sprintf(filename, format, rootname, index);
    index++;
    if (!(fp = fopen(filename, "r")))
      break;
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
    if (lockf(fileno(fp), F_TEST, 0)==-1) {
      /* file exists and is locked */
      fclose(fp);
      sprintf(buffer, "aborting--previous generation of %s (%s) is still active", rootname,
	      filename);
      fprintf(stderr, "Warning: %s\n", buffer);
      exit(0);
    }
#endif
    fclose(fp);
  } while (1);
  if (!(name = malloc(sizeof(*name)*(strlen(filename)+1)))) {
    fprintf(stderr, "Error: memory allocation failure making generation filename\n");
    exit(1);
  }
  return strcpy(name, filename);
}

char *MakeSCRDailyTimeGenerationFilename(char *rootname) {
  char buffer[1024];
  char filename[1024];
  char *name;
  time_t now;
  struct tm *now1;
  FILE *fp;
  
  if (!rootname) {
    fprintf(stderr,"The rootname is not provided.\n");
    exit(1);
  }
  /*generate a new file name */
  do {
    now=time(NULL);
    now1=localtime(&now);
    strftime(buffer,1024,"%Y-%j-%m%d-%H%M%S",now1);
    sprintf(filename,"%s%s",rootname,buffer);
    fp=fopen(filename,"r");
  } while (fp!=NULL);
  
  if (!(name = malloc(sizeof(*name)*(strlen(filename)+1)))) {
    fprintf(stderr, "Error: memory allocation failure making generation filename\n");
    exit(1);
  }
  return strcpy(name, filename);
}

char *MakeDailyGenerationFilename(char *rootname, long digits, char *delimiter,long timetag)
{
#if !defined(vxWorks)
  char buffer[1024];
#endif
  char format[100], filename[1024], *name,*hourNow, match_date[1024];
  long index = 1;
  FILE *fp;
  double dayDbl, jDayDbl, monthDbl, yearDbl, theTime;
  long day, jDay, month, year;
  
  if (digits<0)
    digits = DEFAULT_GENERATIONS_DIGITS;
  theTime = getTimeInSecs();
  makeTimeBreakdown(theTime, NULL, &dayDbl, NULL, &jDayDbl, &monthDbl, &yearDbl, NULL);
  day = dayDbl;
  jDay = jDayDbl;
  month = monthDbl;
  year = yearDbl;
  
  if (timetag) {
    hourNow=getHourMinuteSecond();
    if (rootname && strlen(rootname)>0)
      sprintf(match_date,"%s-%4ld-%03ld-%02ld%02ld",rootname,year,jDay,month,day);
    else
      sprintf(match_date,"%4ld-%03ld-%02ld%02ld",year,jDay,month,day);
    checkGenerationFileLocks(match_date);
    if (rootname && strlen(rootname)>0)
      sprintf(filename,"%s-%4ld-%03ld-%02ld%02ld.%s",rootname,
              year,jDay, month, day,hourNow);
    else
      sprintf(filename,"%4ld-%03ld-%02ld%02ld.%s",year,jDay, month, day,hourNow);
  }
  else {
    if (!digits) {
      if (rootname && strlen(rootname)>0) 
        sprintf(filename, "%s-%4ld-%03ld-%02ld%02ld", rootname,year,jDay,month,day);
      else
        sprintf(filename, "%4ld-%03ld-%02ld%02ld",year,jDay,month,day);
      if ((fp = fopen(filename, "r"))) {
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
        if (lockf(fileno(fp), F_TEST, 0)==-1) {
          /* file exists and is locked */
          fclose(fp);
          sprintf(buffer, "aborting--previous generation of %s is still active",filename);
          fprintf(stderr, "Warning: %s\n", buffer);
          exit(0);
        }
#endif
      }       
    } else {
      if (!delimiter || strlen(delimiter)==0)
        return NULL;
      if (rootname && strlen(rootname)>0) 
        sprintf(format, "%s-%%4ld-%%03ld-%%02ld%%02ld%s%%0%ldld", rootname, delimiter, digits);
      else
        sprintf(format, "%%4ld-%%03ld-%%02ld%%02ld%s%%0%ldld", delimiter, digits);
      do {
        sprintf(filename, format, year, jDay, month, day, index);
        index++;
        if (!(fp = fopen(filename, "r")))
          break;
        theTime = getTimeInSecs();
        makeTimeBreakdown(theTime, NULL, &dayDbl, NULL, &jDayDbl, &monthDbl, &yearDbl, NULL);
        day = dayDbl;
        jDay = jDayDbl;
        month = monthDbl;
        year = yearDbl;
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
        if (lockf(fileno(fp), F_TEST, 0)==-1) {
          /* file exists and is locked */
          fclose(fp);
          sprintf(buffer, "aborting--previous generation of %s (%s) is still active", rootname,
                  filename);
          fprintf(stderr, "Warning: %s\n", buffer);
          exit(0);
        }
#endif
        fclose(fp);
      } while (1);
    }
  }  
  if (!(name = malloc(sizeof(*name)*(strlen(filename)+1)))) {
    fprintf(stderr, "Error: memory allocation failure making generation filename\n");
    exit(1);
  }
  return strcpy(name, filename);
}

char *MakeMonthlyGenerationFilename(char *rootname, long digits, char *delimiter,long timetag)
{
#if !defined(vxWorks)
  char buffer[1024];
#endif
  char format[100], filename[1024], *name,*hourNow, match_date[1024];
  long index = 1;
  FILE *fp;
  double dayDbl, jDayDbl, monthDbl, yearDbl, theTime;
  long day, jDay, month, year;
  
  if (digits<0)
    digits = DEFAULT_GENERATIONS_DIGITS;
  theTime = getTimeInSecs();
  makeTimeBreakdown(theTime, NULL, &dayDbl, NULL, &jDayDbl, &monthDbl, &yearDbl, NULL);
  day = dayDbl;
  jDay = jDayDbl;
  month = monthDbl;
  year = yearDbl;
  
  if (timetag) {
    hourNow=getHourMinuteSecond();
    if (rootname && strlen(rootname)>0)
      sprintf(match_date,"%s-%4ld-%02ld",rootname,year,month);
    else
      sprintf(match_date,"%4ld-%02ld",year,month);
    checkGenerationFileLocks(match_date);
    if (rootname && strlen(rootname)>0)
      sprintf(filename,"%s-%4ld-%02ld.%s",rootname,
              year,month,hourNow);
    else
      sprintf(filename,"%4ld-%02ld.%s",year,month,hourNow);
  }
  else {
    if (!digits) {
      if (rootname && strlen(rootname)>0) 
        sprintf(filename, "%s-%4ld-%02ld", rootname,year,month);
      else
        sprintf(filename, "%4ld-%02ld",year,month);
      if ((fp = fopen(filename, "r"))) {
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
        if (lockf(fileno(fp), F_TEST, 0)==-1) {
          /* file exists and is locked */
          fclose(fp);
          sprintf(buffer, "aborting--previous generation of %s is still active",filename);
          fprintf(stderr, "Warning: %s\n", buffer);
          exit(0);
        }
#endif
      }       
    } else {
      if (!delimiter || strlen(delimiter)==0)
        return NULL;
      if (rootname && strlen(rootname)>0) 
        sprintf(format, "%s-%%4ld-%%02ld%s%%0%ldld", rootname, delimiter, digits);
      else
        sprintf(format, "%%4ld-%%02ld%s%%0%ldld", delimiter, digits);
      do {
        sprintf(filename, format, year, month, index);
        index++;
        if (!(fp = fopen(filename, "r")))
          break;
        theTime = getTimeInSecs();
        makeTimeBreakdown(theTime, NULL, &dayDbl, NULL, &jDayDbl, &monthDbl, &yearDbl, NULL);
        day = dayDbl;
        jDay = jDayDbl;
        month = monthDbl;
        year = yearDbl;
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
        if (lockf(fileno(fp), F_TEST, 0)==-1) {
          /* file exists and is locked */
          fclose(fp);
          sprintf(buffer, "aborting--previous generation of %s (%s) is still active", rootname,
                  filename);
          fprintf(stderr, "Warning: %s\n", buffer);
          exit(0);
        }
#endif
        fclose(fp);
      } while (1);
    }
  }  
  if (!(name = malloc(sizeof(*name)*(strlen(filename)+1)))) {
    fprintf(stderr, "Error: memory allocation failure making generation filename\n");
    exit(1);
  }
  return strcpy(name, filename);
}

void usleepSystemIndependent(long usec) {
#if defined(vxWorks) && !defined(__rtems__)
  struct timespec rqtp;
  rqtp.tv_sec = (long)(usec / 1000000);
  rqtp.tv_nsec = usec%1000000 * 1000;
  nanosleep(&rqtp,NULL);
#else
  usleep(usec);
#endif

}
char *getHourMinuteSecond ()
{
  time_t now;
  char *Hour,*ptr;
  int i=0;
  
  now=time(NULL);
  Hour=ctime(&now);
  ptr=strtok(Hour," ");
  i++;
  while (ptr!=NULL) {
    if (i==4)
      break;
    ptr=strtok(NULL," ");
    i++;
  }
  return ptr;
}

void checkGenerationFileLocks(char *match_date) {
#if !defined(vxWorks) && !defined(__rtems__) && !defined(__CYGWIN__)
  char comm[1024],filename[2048],*ptr, buffer[1024];
  FILE *handle, *fp;
  
  sprintf(comm,"ls %s* 2> /dev/stdout",match_date);
  if (!(handle=popen(comm,"r"))) {
    fprintf(stderr, "Error: no data returned from popen call\n");
    exit(1);
  }
  while (!feof(handle)) {
    if (fgets(filename,sizeof(filename),handle)) {
      ptr=strtok(filename,"\n");
      if (!(fp = fopen(filename, "r")))
        continue;
      if (lockf(fileno(fp), F_TEST, 0)==-1) {
        /* file exists and is locked */
        fclose(fp);
        sprintf(buffer, "aborting--previous generation %s is still active",
                filename);
        fprintf(stderr, "Warning: %s\n", buffer);
        exit(0);
      }
      fclose(fp);
    }
  }
#endif
}


void TouchFile(char *filename) {
#if !defined(vxWorks) && !defined(__rtems__)
  FILE *fp;
  static FILE *fsh = NULL;
  
  if (!fsh) {
    if (!(fsh = popen("csh", "w"))) {
      fprintf(stderr, "Error: unable to launch csh for touchFile operations\n");
      exit(1);
    }
  }
  if (filename) {
    if ((fp=fopen(filename, "r"))) {
      fclose(fp);
      fprintf(fsh, "touch %s\n", filename);
      fflush(fsh);
    }
  }
#endif
}
