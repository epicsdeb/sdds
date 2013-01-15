/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: filestat.c
   Hairong Shang 
   $Log: not supported by cvs2svn $
   Revision 1.4  2007/02/02 18:45:28  borland
   Added str_copy.c, with strcpy_s() routine for safe string copies.
   Converted routines from strcpy() to strcpy_s().

   Revision 1.3  2004/12/03 17:43:05  soliday
   Moved some include statements to mdb.h

   Revision 1.2  2004/12/02 23:02:58  soliday
   Modified so that it will compile under Windows.

   Revision 1.1  2004/11/04 16:15:07  shang
   includes function of reading file links, getting file state and checking if
   file is modified.
 

*/

#if !defined(_WIN32)
#include <unistd.h>
#endif
#include "mdb.h"

/* Return the leading directories part of PATH, allocated with malloc.  If out
   of memory, return 0.  Assumes that trailing slashes have already been
   removed.

   In general, we can't use the builtin `dirname' function if available, since
   it has different meanings in different environments.  In some environments
   the builtin `dirname' modifies its argument.  */

char *dir_name (const char *path)
{
  char *newpath=NULL;
  const char *slash=NULL;
  int length;			/* Length of result, not including NUL.  */
  
  slash = strrchr (path, '/');
  if (slash == 0)
  {
    /* File is in the current directory.  */
    path = ".";
    length = 1;
  }
  else
  {
    /* Remove any trailing slashes from the result.  */
    while (slash > path && *slash == '/')
      --slash;
    length = slash - path + 1;
  }
  newpath = (char *) malloc (length + 1);
  if (newpath == 0)
    return 0;
  strncpy (newpath, path, length);
  newpath[length] = 0;
  return newpath;
}



/*return the first link or NULL if not a link file */
char *read_file_link (const char *filename)
{
  int size = 100, nchars=-1;
  char *tmpbuf=NULL, *dir=NULL, *tempname=NULL;
#if defined(_WIN32)
  return(NULL);
#else
  tmpbuf=(char*)calloc(size,sizeof(char));
  while (1)
  {
    nchars = readlink (filename, tmpbuf, size);
    if (nchars < 0)
    {
      free (tmpbuf);
      return NULL;
    }
    if (nchars < size) {
      if (tmpbuf[0]=='/')
        return tmpbuf;
      else {
        dir=dir_name(filename);
        tempname=(char*)malloc(sizeof(char)*(strlen(filename)+strlen(tmpbuf)+2));
        tempname[0]=0;
        strcat(tempname,dir);
        strcat(tempname,"/");
        strcat(tempname,tmpbuf);
        free(tmpbuf);
        free(dir);
        return tempname;
      }
    }
    size *= 2;
    tmpbuf = (char *) realloc (tmpbuf, sizeof(char)*size);
  }
#endif
}

/* return the last link file name that links to the final file */
char *read_file_lastlink(const char *filename)
{
  char *linkname=NULL;
  char *lastlink=NULL;
  lastlink=filename;
  while ((linkname=read_file_link(filename))!=NULL) {
    lastlink=filename;
    filename=linkname;
  }
  return lastlink;
}


/* return the final file name that filename links to*/
char *read_last_link_to_file(const char *filename)
{
  char *linkname=NULL;
  char *tmpname=NULL;

  if ((linkname=read_file_link(filename))==NULL)
    return NULL;
  tmpname=(char*)calloc(1024,sizeof(char));
  do {
    strcpy_ss(tmpname,linkname);
    free(linkname);
    linkname=read_file_link(tmpname);
  } while (linkname!=NULL);
  return tmpname;
}

/* get the file state of filename if not a link, or the final link-to file of filename if it is
   a link for example:
   filename --> filename1--> filename3-->filename4,
   the returned filestat is the state of filename4*/
long get_file_stat(const char *filename, const char *final_file, struct stat *filestat)
{
  char *input=NULL;
  if (!fexists(filename)) {
    fprintf(stderr,"%s file does not exist, unable to get the state of it!\n",filename);
    exit(1);
  }
  input=filename;
  if (final_file)
    input=final_file;
  if (stat(input,filestat)!=0) {
    fprintf(stderr,"Problem getting state of file %s\n",input);
    exit(1);
  }
  return 0;
}


/* check if input file is modified, including making modifications if no links
   or check if the final link to file name is changed or the final link to file state is changed 
   update the final link to file if changed.*/

long file_is_modified(const char *inputfile, char **final_file, struct stat *input_stat) {
  struct stat filestat;
  char *tempfile=NULL;
  char *tmpinput=NULL;
  
  if (!fexists(inputfile)) {
    fprintf(stderr,"%s file does not exist!\n",inputfile);
    exit(1);
  }
  if (*final_file && !fexists(*final_file)) {
    fprintf(stderr,"linked file %s of inputfile %s does not exist!\n",*final_file,inputfile);
    exit(1);
  }
  if (!input_stat) {
    fprintf(stderr,"The previous state of file %s is not known.\n",inputfile);
    exit(1);
  }
  
  tempfile=read_last_link_to_file(inputfile);
  
  /*the final link name changed */
  if (tempfile && *final_file && strcmp(tempfile,*final_file)) {
    if (*final_file) free(*final_file);
    *final_file=tempfile;
    return 1;
  }
  if ((tempfile && !(*final_file)) || (!tempfile && *final_file)) {
    if (*final_file) free(*final_file);
    *final_file=tempfile;
    return 1;
  }
  /* the final link name did not change, check if the file state changed */
  if (tempfile) free(tempfile);
  if (*final_file)
    tmpinput=*final_file;
  else 
    tmpinput=inputfile;
  filestat=*input_stat;
  if (stat(tmpinput,input_stat)!=0) {
    fprintf(stderr, "Problem getting modification time for %s\n", tmpinput);
    exit(1);
  }
  if (input_stat->st_ctime != filestat.st_ctime) {
    /* file state is changed */
    return 1;
  }
  return 0;
}
