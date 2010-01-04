/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: utils.c
 * contents: utility routines for SDDS applications
 * M. Borland, 1995
 $Log: SDDSutils.c,v $
 Revision 1.15  2005/11/29 18:28:54  shang
 added getSDDSMatchingNames()

 Revision 1.14  2003/09/02 19:16:01  soliday
 Cleaned up code for Linux.

 Revision 1.13  2002/11/25 23:20:50  borland
 Fixed bug resulting in freeing of same memory twice in multiplyColumnUnits().

 Revision 1.12  2002/10/17 15:50:52  soliday
 Fixed a memory allocation error.

 Revision 1.11  2002/08/14 17:12:35  soliday
 Added Open License

 Revision 1.10  2001/10/25 21:58:11  borland
 Moved greatestProductOfSmallPrimes() and makeFreqUnits() from sddsfft.c
 to this location for use by sddsfft.c and sddsnaff.c .

 Revision 1.6  2000/04/13 18:00:17  soliday
 Removed break statements after return statements.

 Revision 1.5  1999/09/14 18:28:50  soliday
 replace_stringn is no longer defined locally

 Revision 1.4  1999/05/25 19:01:33  soliday
 Removed compiler warning on linux.

 Revision 1.3  1999/05/05 14:37:27  borland
 Fixed bug in SDDS_CompareParameterValues (wasn't compare string data
 correctly).

 * Revision 1.2  1996/05/29  21:42:15  borland
 * Added new program sddsrowstats.  Improved error messages in SDDSutils.c
 * routine (expandColumnPairNames).
 *
 * Revision 1.1  1995/09/06  14:55:43  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "SDDS.h"
#include "SDDSutils.h"

long appendToStringArray(char ***item, long items, char *newItem)
{
    if (!(*item = SDDS_Realloc(*item, sizeof(**item)*(items+1))))
        SDDS_Bomb("allocation failure in appendToStringArray");
    if (!SDDS_CopyString((*item)+items, newItem)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    return items+1;
    }

long expandColumnPairNames(SDDS_DATASET *SDDSin, char ***name, char ***errorName, long names, char **excludeName,
                           long excludeNames, long typeMode, long typeValue)
{
    long i, j, names1, errorNames1, names2;
    char **name1, **errorName1, **name2, **errorName2;

    if (!names || !*name)
        return 0;
    name2 = errorName1 = errorName2 = NULL;
    names2 = errorNames1 = 0;
    for (i=0; i<names; i++) {
        switch (typeMode) {
          case FIND_ANY_TYPE:
          case FIND_NUMERIC_TYPE:
          case FIND_INTEGER_TYPE:
          case FIND_FLOATING_TYPE:
            names1 = SDDS_MatchColumns(SDDSin, excludeNames?NULL:&name1, SDDS_MATCH_STRING, typeMode, 
                                       (*name)[i], SDDS_0_PREVIOUS|SDDS_OR);
            break;
          case FIND_SPECIFIED_TYPE:
            if (!SDDS_VALID_TYPE(typeValue))
                SDDS_Bomb("invalid type value in expandColumnPairNames");
            names1 = SDDS_MatchColumns(SDDSin, excludeNames?NULL:&name1, SDDS_MATCH_STRING, typeMode, typeValue, 
                                      (*name)[i], SDDS_0_PREVIOUS|SDDS_OR);
            break;
          default:
            SDDS_Bomb("invalid typeMode in expandColumnPairNames");
	    exit(1);
            break;
            }
        if (names1==0)
            continue;
        if (names1==-1) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          SDDS_Bomb("unable to perform column name match in expandColumnPairNames");
          }
        
        if (excludeNames) {
            for (j=0; j<excludeNames; j++) 
                if (!(names1=SDDS_MatchColumns(SDDSin, (j==excludeNames-1?&name1:0),
                                               SDDS_MATCH_STRING, FIND_ANY_TYPE, 
                                               excludeName[j], SDDS_NEGATE_MATCH|SDDS_AND)))
                    break;
            }
        if (!names1)
            continue;
        moveToStringArray(&name2, &names2, name1, names1);
        if (errorName && *errorName && (*errorName)[i]) {
            if (strstr((*errorName)[i], "%s")) {
                if (!(errorName1 = (char**)malloc(sizeof(*errorName1)*names1))) 
                    SDDS_Bomb("allocation failure in expandColumnPairNames");
                errorNames1 = names1;
                for (j=0; j<names1; j++) {
                    if (!(errorName1[j] = (char*)malloc(sizeof(**errorName1)*
                                                        strlen(name1[j])+strlen((*errorName)[i])+1)))
                        SDDS_Bomb("allocation failure in expandColumnPairNames");
                    replace_stringn(errorName1[j], (*errorName)[i], "%s", name1[j], 1);
                    if (SDDS_CheckColumn(SDDSin, errorName1[j], NULL, SDDS_ANY_NUMERIC_TYPE, stderr)!=SDDS_CHECK_OKAY)
                        exit(1);
                    }
                }
            else if (names1==1) {
                errorNames1 = 1;
                if (!(errorName1 = (char**)malloc(sizeof(*errorName1))))
                    SDDS_Bomb("allocation failure in expandColumnPairNames");
                if (!SDDS_CopyString(errorName1, (*errorName)[i]))
                    return 0;
                }
            else
                SDDS_Bomb("%s template must be employed with error names when primary name has wildcards");
            names2 -= names1;
            moveToStringArray(&errorName2, &names2, errorName1, errorNames1);
            free(errorName1);
            free(name1);
           }
        }
    if (names2==0)
        return 0;
    SDDS_FreeStringArray(*name, names);
    *name = name2;
    if (errorName) {
        if (*errorName) 
            SDDS_FreeStringArray(*errorName, names);
        *errorName = errorName2;
        }
    return names2;
    }

void moveToStringArray(char ***target, long *targets, char **source, long sources)
{
    long i, j;
    if (!sources)
        return;
    *target = SDDS_Realloc(*target, sizeof(**target)*(*targets+sources));
    for (i=0; i<sources; i++) {
        for (j=0; j<*targets; j++)
            if (strcmp(source[i], (*target)[j])==0)
                break;
        if (j==*targets) {
            (*target)[j] = source[i];
            *targets += 1;
            }
        }
    }

char *multiplyColumnUnits(SDDS_DATASET *SDDSin, char *name1, char *name2)
{
    char buffer[SDDS_MAXLINE];
    char *units1, *units2;

    if (SDDS_GetColumnInformation(SDDSin, "units", &units1, SDDS_GET_BY_NAME, name1)!=SDDS_STRING ||
        SDDS_GetColumnInformation(SDDSin, "units", &units2, SDDS_GET_BY_NAME, name2)!=SDDS_STRING)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (units1 && !SDDS_StringIsBlank(units1)) {
        strcpy(buffer, units1);
        free(units1);
        units1 = NULL;
        if (units2 && !SDDS_StringIsBlank(units2)) {
            strcat(buffer, " ");
            strcat(buffer, units2);
            free(units2);
            units2 = NULL;
            }
        }
    else if (units2 && !SDDS_StringIsBlank(units2)) {
        strcpy(buffer, units2);
        free(units2);
        units2 = NULL;
        }
    else
        buffer[0] = 0;
    if (units1)
        free(units1);
    if (units2)
        free(units2);
    SDDS_CopyString(&units1, buffer);
    return units1;
    }

char *divideColumnUnits(SDDS_DATASET *SDDSin, char *name1, char *name2)
{
    char buffer[SDDS_MAXLINE];
    char *units1, *units2;

    if (SDDS_GetColumnInformation(SDDSin, "units", &units1, SDDS_GET_BY_NAME, name1)!=SDDS_STRING ||
        SDDS_GetColumnInformation(SDDSin, "units", &units2, SDDS_GET_BY_NAME, name2)!=SDDS_STRING)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (units1 && !SDDS_StringIsBlank(units1)) {
        strcpy(buffer, units1);
        free(units1);
        units1 = NULL;
        if (units2 && !SDDS_StringIsBlank(units2)) {
            strcat(buffer, "/(");
            strcat(buffer, units2);
            strcat(buffer, ")");
            free(units2);
            units2=NULL;
            }
        }
    else if (units2 && !SDDS_StringIsBlank(units2)) {
        strcpy(buffer, "1/(");
        strcat(buffer, units2);
        strcat(buffer, ")");
        free(units2);
        units2 = NULL;
        }
    else
        buffer[0] = 0;
    if (units1)
        free(units1);
    if (units2)
        free(units2);
    SDDS_CopyString(&units1, buffer);
    return units1;
    }

long SDDS_CompareParameterValues(void *param1, void *param2, long type)
{
  double ddiff;
  long ldiff;
  char cdiff;
  
  switch (type) {
  case SDDS_FLOAT:
    ddiff = *((float*)param1) - *((float*)param2);
    return ddiff<0 ? -1 : ddiff>0?1:0 ;
  case SDDS_DOUBLE:
    ddiff = *((double*)param1) - *((double*)param2);
    return ddiff<0 ? -1 : ddiff>0?1:0 ;
  case SDDS_LONG:
    ldiff = *((long*)param1) - *((long*)param2);
    return ldiff<0 ? -1 : ldiff>0?1:0 ;
  case SDDS_SHORT:
    ldiff = *((short*)param1) - *((short*)param2);
    return ldiff<0 ? -1 : ldiff>0?1:0 ;
  case SDDS_CHARACTER:
    cdiff = (short)*((char*)param1) - (short)*((char*)param2);
    return cdiff<0 ? -1 : cdiff>0?1:0 ;
  case SDDS_STRING:
    return strcmp(*(char**)param1, *(char**)param2);
  default:
    SDDS_SetError("Problem doing data comparison--invalid data type (SDDS_CompareParameterValues)");
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
}



char *makeFrequencyUnits(SDDS_DATASET *SDDSin, char *indepName)
{
  char *timeUnits;
  char *units;
  long reciprocal = 0, end;
  
  if (SDDS_GetColumnInformation(SDDSin, "units", &timeUnits, SDDS_GET_BY_NAME, indepName)!=SDDS_STRING)
    return 0;
  if (timeUnits) {
    while (1) {
      end = strlen(timeUnits) - 1;
      if (timeUnits[0]=='(' && timeUnits[end]==')') {
	timeUnits[end] = 0;
	strslide(timeUnits, 1);
      }
      else if (timeUnits[0]=='1' && timeUnits[1]=='/' && timeUnits[2]=='(' && timeUnits[end]==')') {
	timeUnits[end] = 0;
	strslide(timeUnits, 3);
	reciprocal = !reciprocal;
      }
      else 
	break;
    }
  }
  if (!timeUnits || SDDS_StringIsBlank(timeUnits)) {
    units = tmalloc(sizeof(*units)*1);
    units[0] = 0;
    return units;
  }
  
  if (reciprocal) {
    if (!SDDS_CopyString(&units, timeUnits))
      return NULL;
    return units;
  }
  
  units = tmalloc(sizeof(*units)*(strlen(timeUnits)+5));
  if (strchr(timeUnits, ' ')) 
    sprintf(units, "1/(%s)", timeUnits);
  else
    sprintf(units, "1/%s", timeUnits);
  return units;
}

#define MAXPRIMES 8
long greatestProductOfSmallPrimes(long rows)
/* doesn't really find the greatest product, but simply a large one */
{
  long bestResult = 0, result;
  static long prime[MAXPRIMES] = {2, 3, 5, 7, 11, 13, 17, 19};
  long nPrimes;
  
  for (nPrimes=1; nPrimes<=MAXPRIMES; nPrimes++) {
    if ((result = greatestProductOfSmallPrimes1(rows, prime, nPrimes))>bestResult &&
        result<=rows)
      bestResult = result;
  }
  if (bestResult==0)
    SDDS_Bomb("couldn't find acceptable number of rows for truncation/padding");
  return bestResult;
}

long greatestProductOfSmallPrimes1(long rows, long *primeList, long nPrimes)
{
  long iprime, product, remains, bestFactor=0;
  double remainder, smallestRemainder;
  
  remains = rows;
  product = 1;
  while (remains>2) {
    smallestRemainder = LONG_MAX;
    for (iprime=0; iprime<nPrimes; iprime++) {
      remainder = remains - primeList[iprime]*((long)(remains/primeList[iprime]));
      if (remainder<smallestRemainder) {
        smallestRemainder = remainder;
        bestFactor = primeList[iprime];
      }
      if (remainder==0)
        break;
    }
    remains /= bestFactor;
    product *= bestFactor;
  }
  return product*remains;
}

char **getMatchingSDDSNames(SDDS_DATASET *dataset, char **matchName, long matches, long *names, 
                            short type)
{
  char **name, **selectedName, *ptr=NULL;
  int32_t names0=0, selected=0, i, j;

  name=selectedName=NULL;
  switch(type) {
  case SDDS_MATCH_COLUMN:
    if (!(name=SDDS_GetColumnNames(dataset, &names0)))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    break;
  case SDDS_MATCH_PARAMETER:
    if (!(name=SDDS_GetParameterNames(dataset, &names0)))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    break;
  case SDDS_MATCH_ARRAY:
    if (!(name=SDDS_GetArrayNames(dataset, &names0)))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    break;
  default:
    SDDS_Bomb("Invalid match type provided.");
    break;
  }
  for (i=0; i<matches; i++) {
    if (has_wildcards(matchName[i])) {
      ptr = expand_ranges(matchName[i]);
      for (j=0; j<names0; j++) {
        if (wild_match(name[j], ptr)) {
          selectedName = SDDS_Realloc(selectedName, sizeof(*selectedName)*(selected+1));
          SDDS_CopyString(&selectedName[selected], name[j]);
          selected++;
        }
      }
      free(ptr);
    } else {
      if (match_string(matchName[i], name, names0, EXACT_MATCH)<0) {
        fprintf(stderr,"%s not found in input file.\n", matchName[i]);
        exit(1);
      } else {
        selectedName = SDDS_Realloc(selectedName, sizeof(*selectedName)*(selected+1));
        SDDS_CopyString(&selectedName[selected], matchName[i]);
        selected++;
      }
    }
  }
  SDDS_FreeStringArray(name, names0);
  free(name);
  *names = selected;
  return selectedName;
}
