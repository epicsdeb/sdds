/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include <ctype.h>
#include "SDDS.h"

void substituteTagValue(char *input, long buflen, 
                        char **macroTag, char **macroValue, long macros)
{
  char *buffer;
  long i;
  char *version1=NULL, *version2=NULL;
  if (!(buffer=malloc(sizeof(*buffer)*buflen)))
    bomb("memory allocation failure doing macro substitution", NULL);
  for (i=0; i<macros; i++) {
    if (i==0) {
      if (!(version1 = malloc(sizeof(*version1)*(strlen(macroTag[i])+10))) ||
	  !(version2 = malloc(sizeof(*version2)*(strlen(macroTag[i])+10))))
	bomb("memory allocation failure doing macro substitution", NULL);
    } else {
      if (!(version1 = realloc(version1, sizeof(*version1)*(strlen(macroTag[i])+10))) ||
	  !(version2 = realloc(version2, sizeof(*version2)*(strlen(macroTag[i])+10))))
	bomb("memory allocation failure doing macro substitution", NULL);
    }
    sprintf(version1, "<%s>", macroTag[i]);
    sprintf(version2, "$%s", macroTag[i]);
    if (replace_string(buffer, input, version1, macroValue[i]))
      strcpy_ss(input, buffer);
    if (replace_string(buffer, input, version2, macroValue[i]))
      strcpy_ss(input, buffer);
  }
  free(buffer);
  if (version1) free(version1);
  if (version2) free(version2);
}
