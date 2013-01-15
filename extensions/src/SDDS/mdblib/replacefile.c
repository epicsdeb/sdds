/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: replacefile.c
 * contents: replaceFile(), replaceFileAndBackUp()
 * M. Borland, 1995.
 $Log: not supported by cvs2svn $
 Revision 1.6  2000/04/05 21:33:44  soliday
 Fixed typo with if defined statement

 Revision 1.5  1999/07/30 19:33:16  borland
 Undid the last change and instead use "copy" command for WIN32 instead
 of UNIX cp.

 Revision 1.4  1999/07/30 18:57:13  borland
 For WIN32, use x<filename> for backup copy rather than <filename>~,
 as the latter isn't allowed.

 Revision 1.3  1997/02/03 21:22:07  borland
 Fixed problems with renameRobust() and file replacement routines.

 * Revision 1.2  1995/09/05  21:18:15  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"

long replaceFile(char *file, char *replacement)
{
    if (renameRobust(file, replacement, RENAME_OVERWRITE)) {
        fprintf(stderr, "unable to rename file %s to %s\n",
                replacement, file);
        perror(NULL);
        return 0;
        }
    return 1;
    }

long renameRobust(char *oldName, char *newName, unsigned long flags)
/* returns 0 on success, 1 on failure, just like ANSI rename() */
{
  char buffer[1024];
/*
  if (fexists(newName) && flags&RENAME_OVERWRITE) {
    remove(newName);
  }
  if (fexists(newName))
    return 1; 
*/
  if (fexists(newName) && !(flags&RENAME_OVERWRITE))
    return 1; 

  /* try using the system-provided version first */
  if (rename(oldName, newName)==0)
    return 0;
  /* do a copy-and-delete operation */
#if defined(_WIN32)
  sprintf(buffer, "copy %s %s", oldName, newName);
#else
  sprintf(buffer, "cp %s %s", oldName, newName);
#endif
  system(buffer);
  if (!fexists(newName)) {
    fprintf(stderr, "unable to copy %s to %s\n", oldName, newName);
    return 1;
  }
  remove(oldName); /* ignore return value */
  return 0;
}

long replaceFileAndBackUp(char *file, char *replacement)
{
    char *backup;
    backup = tmalloc(sizeof(*backup)*(strlen(file)+2));
    sprintf(backup, "%s~", file);
    if (renameRobust(file, backup, RENAME_OVERWRITE)==0) {
        if (renameRobust(replacement, file, RENAME_OVERWRITE)) {
            fprintf(stderr, "unable to rename temporary file %s to %s\n",
                    replacement, file);
            perror(NULL);
            if (renameRobust(backup, file, 0)) {
                fprintf(stderr, "unable to rename %s back to %s !\n", backup, file);
                perror(NULL);
                }
            else
                fprintf(stderr, "original version of %s restored\n", file);
            free(backup);
            return 0;
            }
        }
    else {
        fprintf(stderr, "unable to replace %s--result stored in %s\n",
                file, replacement);
        perror(NULL);
        free(backup);
        return 0;
        }
    free(backup);
    return 1;
    }

