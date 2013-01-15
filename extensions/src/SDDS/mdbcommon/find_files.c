/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: find_files.c
   Hairong Shang 
   $Log: not supported by cvs2svn $
   Revision 1.2  2005/03/03 17:22:19  soliday
   Updated to work on WIN32

   Revision 1.1  2005/02/02 16:09:03  soliday
   Moved find_files.c from mdblib to mdbcommon

   Revision 1.4  2005/01/04 22:31:02  shang
   removed the printf statements

   Revision 1.3  2005/01/04 22:04:00  shang
   it seems that the conditional value can not be used in calculation equation.

   Revision 1.2  2004/12/22 19:16:55  shang
   initialized startHour and endHour

   Revision 1.1  2004/12/21 20:12:30  shang
   routines for finding files between dates

   
*/

#include "mdb.h"
#include "SDDS.h"

short make_four_digit_year (short year)
{
  if (year>100)
    return year;
  if (year<90)
    year +=2000;
  else
    year += 1900;
  return year;
}

/*Returns 0/1 for nonleap/leap year
  Returns -1 on error */
long is_leap_year (short year) 
{
  if (year<0)
    return -1;
  year=make_four_digit_year(year);
  return (year%4==0 && year%100!=0 || year%400==0);
}

#if !defined(_WIN32)
/*increaseOrder=1, increase; 0, descrease order */
char **ls_dir (char *path, char *matchstr, long tailsOnly, long *files)
{
  DIR *dirp;
  struct dirent *dp;
  char buf[2056];
  char **filename;
  char *tmpPath;
  long files0=0;
  
  filename=NULL;
  dirp=NULL;
  dp=NULL;
  if (!path) {
    /*work with current path */
    SDDS_CopyString(&tmpPath,".");
  } else {
    if (!fexists(path)) {
      fprintf(stderr, "path %s does not exist!\n",path);
      exit(1);
    }
    SDDS_CopyString(&tmpPath, path);
  }
  if (dirp=opendir(tmpPath)) {
    while (dp=readdir(dirp)) {
      if (strcmp(dp->d_name,".") && strcmp(dp->d_name,"..")) {
        if (!matchstr || (matchstr && wild_match(dp->d_name,matchstr))) {
          if (tailsOnly)
            sprintf(buf,"%s",dp->d_name);
          else
            sprintf(buf,"%s/%s",tmpPath,dp->d_name);
          filename=SDDS_Realloc(filename,sizeof(*filename)*(files0+1));
          SDDS_CopyString(&filename[files0],buf);
          files0++;
        }
      }
    }
    closedir(dirp);
  }
  if (files0) {
    /*sort file by time*/
    
  }
  
  free(tmpPath);
  *files=files0;
  return filename;
}

char **find_files_between_dates(char *directory, char *rootname, char *suffix, 
                            short startYear, short startMonth, short startDay, short startJDay,
                            short endYear, short endMonth, short endDay, short endJDay,
                            char *filter, char **extensionList, long extensions,
                            long tailsOnly, long *files, long increaseOrder)
{
  short iyear, iday, toYearEnd, jDayLimit, year0, month0, day0, iday0, istoday, listall;
  long yearLists, fileLists, newLists, i, index, tmpLists, j, *sortedIndex;
  char **fileList, **yearList, **newList, **tmpList;
  char buffer[2056], tmpstr[2056], format[2056];
  double startTime, endTime, startHour, endHour, time0, hour0;
  time_t longtime;
  
  sortedIndex=NULL;
  fileLists=yearLists=newLists=tmpLists=0;
  fileList=yearList=newList=tmpList=NULL;
  if (!directory || !rootname || !filter ) {
    fprintf(stderr,"directory or rootname or filter for searching files is not given!\n");
    exit(1);
  }
  startTime=endTime=0;
  startHour=endHour=0;
  time(&longtime);
  TimeEpochToBreakdown(&year0, &iday0, &month0, &day0, &hour0, longtime);
  if (endYear==year0) {
    if (endJDay==iday0) 
      istoday=1;
    else if (endMonth==month0 && endDay==day0)
      istoday=1;
    else
      istoday=0;
  } else
    istoday=0;
  
  if (strcmp(filter,"-??\??")==0) {
    if (tailsOnly)
      sprintf(format,"%s%s-%s",rootname,"%d","%02d%02d%s");
    else
      sprintf(format,"%s/%s%s-%s",directory, rootname,"%d","%02d%02d%s");
  } else if (strcmp(filter,"-??\?-??\??")==0) {
    if (tailsOnly)
      sprintf(format,"%s%s-%s-%s",rootname,"%d","%03d","%02d%02d%s");
    else 
      sprintf(format,"%s/%s%s-%s-%s",directory,rootname,"%d","%03d","%02d%02d%s");
  } else
    SDDS_Bomb("invalid filter given, should be -???? or -??\?-????");
  if (!startJDay) {
    TimeBreakdownToEpoch(startYear, 0, startMonth, startDay, startHour, &startTime);
    TimeEpochToBreakdown(&startYear, &startJDay, &startMonth, &startDay, &startHour,startTime);
  }
  if (!endJDay) {
    TimeBreakdownToEpoch(endYear, 0, endMonth, endDay, endHour, &endTime);
    TimeEpochToBreakdown(&endYear, &endJDay, &endMonth, &endDay, &endHour,endTime);
  }
  if (extensions) {
    tmpList=(char**)malloc(sizeof(tmpList)*extensions);
    for (i=0;i<extensions;i++) {
      if (suffix)
        sprintf(tmpstr,"%s%s",suffix,extensionList[i]);
      else
        sprintf(tmpstr,"%s",extensionList[i]);
      SDDS_CopyString(&tmpList[i],tmpstr);
    }
    tmpLists=extensions;
  } else {
    tmpLists=1;
    tmpList=(char**)malloc(sizeof(tmpList));
    if (suffix)
      SDDS_CopyString(&tmpList[0], suffix);
    else
      SDDS_CopyString(&tmpList[0], "");
  }
  startYear=make_four_digit_year(startYear);
  endYear=make_four_digit_year(endYear);
  for (iyear=startYear;iyear<=endYear;iyear++) {
    if (iyear!=endYear) {
      toYearEnd=1;
      if (is_leap_year(iyear))
        jDayLimit=366;
      else
        jDayLimit=365;
    } else {
      toYearEnd=0;
      jDayLimit=endJDay;
    }
    if (startJDay==1 && (toYearEnd==1 || istoday==1)) 
      listall=1;
    else
      listall=0;
    
    for (i=0;i<tmpLists;i++) {
      sprintf(tmpstr,"%s%d%s%s",rootname,iyear,filter,tmpList[i]);
      yearList=ls_dir(directory,tmpstr,tailsOnly,&yearLists);
      if (yearLists) {
        if (listall) {
          fileList=SDDS_Realloc(fileList, sizeof(*fileList)*(fileLists+yearLists));
          for (j=0;j<yearLists;j++) 
            SDDS_CopyString(&fileList[j + fileLists], yearList[j]);
          fileLists +=yearLists;
        } else {
          for (iday=startJDay;iday<=jDayLimit;iday++) {
            TimeBreakdownToEpoch(iyear, iday, 0, 0, 1, &time0);
            TimeEpochToBreakdown(&year0, &iday0, &month0, &day0, &hour0, time0);
            if (year0!=iyear || iday0!=iday || hour0!=1) {
              fprintf(stderr,"Error in getting the julian date!\n");
              exit(1);
            }
            if (strcmp(filter,"-??\??")==0)
              sprintf(buffer,format,iyear,month0,day0,tmpList[i]);
            else
              sprintf(buffer,format,iyear,iday0,month0,day0,tmpList[i]);
            if (has_wildcards(tmpList[i])) {
              for (j=0;j<yearLists;j++) {
                if (wild_match(yearList[j],buffer)) {
                  newList=SDDS_Realloc(newList, sizeof(*newList)*(newLists+1));
                  SDDS_CopyString(&newList[newLists], yearList[j]);
                  newLists++;
                }
              }
            } else {
              index=match_string(buffer,yearList,yearLists,EXACT_MATCH);
              if (index>=0) {
                newList=SDDS_Realloc(newList, sizeof(*newList)*(newLists+1));
                SDDS_CopyString(&newList[newLists], buffer);
                newLists++;
              }
            }
          } /*end of for iday loop */
          if (newLists) {
            fileList=SDDS_Realloc(fileList,sizeof(*fileList)*(fileLists+newLists));
            for (j=0;j<newLists;j++) {
              SDDS_CopyString(&fileList[j+fileLists],newList[j]);
              free(newList[j]);
            }
            free(newList);
            fileLists +=newLists;
            newLists=0;
            newList=NULL;
          }
        } /*end of if startJDay==1 */
        /*free yearList */
        SDDS_FreeStringArray(yearList, yearLists);
        free(yearList);
        yearLists=0;
        yearList=NULL;
      } /*end of if yearLists*/
    } /*end of tmpLists loop */
    startJDay=1;
  } /*end of iyear loop*/
  sortedIndex=sort_and_return_index(fileList,SDDS_STRING,fileLists,increaseOrder);
  free(sortedIndex);
  *files=fileLists;
  for (i=0;i<tmpLists;i++)
    free(tmpList[i]);
  if (tmpList)
    free(tmpList);
  return fileList;
}
#endif

/*sort the files by the value of StartTime parameter, assume that all files
contain StartTime parameter;
isTail -- 1 , the fileList is only tail of file, need add directory
otherwise -- need not add directory*/
void sort_files_by_start_time(char *directory, long isTail, char **fileList, long files, long increaseOrder)
{
  long i, *index=NULL;
  double *startTime;
  SDDS_DATASET dataset;
  char **tmpList;
  char buffer[2046];

  tmpList=fileList;
  startTime=NULL;
  if (!fileList || !files || files==1)
    return;
  startTime=(double*)calloc(sizeof(*startTime),files);
  for (i=0;i<files;i++) {
    if (isTail && directory) {
      sprintf(buffer,"%s/%s",directory,fileList[i]);
      if  (!SDDS_InitializeInput(&dataset, buffer)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    } else {
      if  (!SDDS_InitializeInput(&dataset, fileList[i])) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    }
    if (SDDS_ReadPage(&dataset)<0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (!SDDS_GetParameterAsDouble(&dataset,"StartTime",&startTime[i])) {
      fprintf(stderr,"Unable to get StartTime parameter from %s file.\n", fileList[i]);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (!SDDS_Terminate(&dataset)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  index=sort_and_return_index(startTime,SDDS_DOUBLE,files,increaseOrder);
  for (i=0;i<files;i++)
    fileList[i]=tmpList[index[i]];
  
}
