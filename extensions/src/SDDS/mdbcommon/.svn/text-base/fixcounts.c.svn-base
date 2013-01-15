/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: fixcount()
 * purpose: fix the data point count in a mpl-format file, by over-writing
 *          the existing point count.  If this is not possible, return 0 for
 *          failure.
 *          An adaptation of my fixcount program.
 * Michael Borland, 1990
 $Log: not supported by cvs2svn $
 Revision 1.2  2002/08/14 16:48:27  soliday
 Added Open License

 Revision 1.1  2000/04/11 16:21:26  soliday
 Moved here from mdbmth.

 Revision 1.5  1999/07/08 22:12:31  soliday
 Added ctype.h include statement

 Revision 1.4  1999/05/28 14:59:10  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/11/13 16:19:18  borland
 Added long cast to strlen() in comparisons to satisfy Solaris compiler.

 * Revision 1.2  1995/09/05  21:19:50  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "table.h"
#include <ctype.h>

#define LINE_LENGTH 1024

int fixcount(char *filename, long n_points)
{
    long count;
    FILE *fp;
    char s[LINE_LENGTH], t[LINE_LENGTH];
    int32_t l_count_line, posi_count_line;

    if (!(fp = fopen(filename, "r"))) 
        return(0);
    if (!fgets_skip(s, LINE_LENGTH, fp, '!', 0))
        return 0;
    if (strncmp(s, "SDDS", 4)==0 && isdigit(s[4]))
        return -1;
    if (!fgets_skip(s, LINE_LENGTH, fp, '!', 0) ||
        !fgets_skip(s, LINE_LENGTH, fp, '!', 0) ||
        !fgets_skip(s, LINE_LENGTH, fp, '!', 0) )
        return(0);
    posi_count_line = ftell(fp);
    if (!fgets_skip(s, LINE_LENGTH, fp, '!', 0))
        return(0);
    l_count_line = strlen(s)-1;
    count = 0;
    if (n_points == -1) {
        /* count the number of points */
        while (fgets(s, LINE_LENGTH, fp))
            if (s[0]!='!')
                count++;
        }
    else
        count = n_points;
    fclose(fp);
    sprintf(t, "%ld", count);
    if ((long)strlen(t)<=l_count_line && (fp = fopen(filename, "r+"))) {
        pad_with_spaces(t, l_count_line-strlen(t));
        if (!(fseek(fp, posi_count_line, 0)!=EOF && fputs(t, fp)!=EOF)){
            fclose(fp);
            return(0);
            }
        else {
            fclose(fp);
            return(count);
            }
        }
    return(0);
    }

