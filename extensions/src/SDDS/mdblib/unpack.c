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

#if defined(_WIN32)
#define popen(x,y) _popen(x,y)
#endif

#define UNPACK_TYPES 3
static char *unpackSuffix[UNPACK_TYPES] = {
        "gz", "F", "Z", 
        };
static char *unpackCommand[UNPACK_TYPES] = {
        "gzip -dcn %s 2> /dev/null ", "freeze -dc %s ", "uncompress -c %s ",
        };

long PackSuffixType(char *filename, char **unpackedName, unsigned long mode)
{
  char *extension, buffer[10];
  FILE *fp;
  long i;
  
  if (!(extension = strrchr(filename, '.')))
    return -1;
  
  extension++;
  for (i=0; i<UNPACK_TYPES; i++)
    if (strcmp(extension, unpackSuffix[i])==0) {
      if (unpackedName) {
        cp_str(unpackedName, filename);
        extension = strrchr(*unpackedName, '.');
        *extension = 0;
      }
      break;
    }
  if (i==UNPACK_TYPES)
    return -1;

  if (mode&UNPACK_REQUIRE_SDDS) {
    if (!(fp = fopen(filename, FOPEN_READ_MODE)))
      return -1;
    if (fread(buffer, sizeof(*buffer), 4, fp)==4 && strncmp(buffer, "SDDS", 4)==0) {
      fclose(fp);
      return -1;
    }
    fclose(fp);
  }
  return i;
}

FILE *UnpackFopen(char *filename, unsigned long mode, short *popenUsed, char **tmpFileUsed) 
{
  static char *command = NULL;
  long type;
  char *tmpName;

  if (popenUsed)
    *popenUsed = 0;
  if (tmpFileUsed)
    *tmpFileUsed = NULL;
  if (!filename)
    return NULL;
  if ((type=PackSuffixType(filename, NULL, mode))<0)
    return fopen(filename, FOPEN_READ_MODE);

  if (!(command = trealloc(command, sizeof(*command)*(strlen(filename)+100))))
    return NULL;
  if (mode&UNPACK_USE_PIPE) {
    sprintf(command, unpackCommand[type], filename);
    if (popenUsed)
      *popenUsed = 1;
#if defined(vxWorks)
    fprintf(stderr, "popen is not supported in vxWorks\n");
    exit(1);
    return NULL;
#else
    return popen(command, FOPEN_READ_MODE);
#endif
  }
  else {
    sprintf(command, unpackCommand[type], filename);
    tmpName = tmpname(NULL);
    strcat(command, "> /tmp/");
    strcat(command, tmpName);
    system(command);

    sprintf(command, "/tmp/%s", tmpName);
    if (tmpFileUsed)
      cp_str(tmpFileUsed, command);

    return fopen(command, FOPEN_READ_MODE);
  }
}

#if defined(zLib)
gzFile *UnpackGZipOpen(char *filename) 
{
  if (!filename)
    return NULL;
  return gzopen(filename, "rb");
}
#endif



