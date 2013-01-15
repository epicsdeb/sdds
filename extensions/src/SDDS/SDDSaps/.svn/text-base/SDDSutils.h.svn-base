/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

long appendToStringArray(char ***item, long items, char *newItem);
long expandColumnPairNames(SDDS_DATASET *SDDSin, char ***name, 
			   char ***errorName, long names, 
			   char **excludeName, long excludeNames, 
			   long typeMode, long typeValue);

char *multiplyColumnUnits(SDDS_DATASET *SDDSin, char *name1, char *name2);
char *divideColumnUnits(SDDS_DATASET *SDDSin, char *name1, char *name2);
/* basically just composes 1/(units) */
char *makeFrequencyUnits(SDDS_DATASET *SDDSin, char *indepName);

long compareParameterValues(void *param1, void *param2, long type);
void moveToStringArray(char ***target, long *targets, char **source, 
		       long sources);

long greatestProductOfSmallPrimes1(long rows, long *primeList, long nPrimes);
long greatestProductOfSmallPrimes(long rows);
