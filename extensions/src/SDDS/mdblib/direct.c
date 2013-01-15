/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:17:17  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"
#include "sort.h"
#include <dirent.h>

char **wild_list(
    int *n_names_ret,     /* number of names returned in array of pointers */
    int **origin_ret,     /* index of the item that gave rise to a name */
    char **item_list,     /* list of items to match with */
    int n_items           /* number of items to match with */
    );
int strcmpp(char **s1, char **s2);
int strcpyp(char **s1, char **s2);

main(int argc, char **argv)
{
    char **list; 
    int *origin;
    int i, n_ret;

    if (!(list=wild_list(&n_ret, &origin, argv+1, argc-1)))
        bomb("no match", NULL);
    for (i=0; i<n_ret; i++)
        printf("%d:  %s\n", origin[i], list[i]);
    }

typedef struct {
    char *name;
    int origin;
    } NAME_ORIGIN;

int NO_compare(NAME_ORIGIN *s1, NAME_ORIGIN *s2);
int NO_copy(NAME_ORIGIN *s1, NAME_ORIGIN *s2);

char **wild_list(
    int *n_names_ret,     /* number of names returned in array of pointers */
    int **origin_ret,     /* index of the item that gave rise to a name */
    char **item,          /* list of items to match with */
    int n_items           /* number of items to match with */
    )
{
    char template[1024];
    DIR *fdp;
    struct dirent *entry;
    NAME_ORIGIN *list;
    char **slist;
    long n_list;

    if (!(fdp=opendir(".")))
        bomb("unable to open current directory (wild_list)", NULL);

    n_list = 0;
    list = NULL;
    while (n_items--) {
        strcpy(template, item[n_items]);        
        while (entry=readdir(fdp)) {
            if (wild_match(entry->d_name, template)) {
                list = trealloc(list, sizeof(*list)*(n_list+1));
                cp_str(&list[n_list].name, entry->d_name);
                list[n_list].origin = n_items;
                n_list++;
                }
            }
        rewinddir(fdp);
        }
    qsort((char*)list, n_list, sizeof(*list), NO_compare);
    n_list = unique((char*)list, n_list, sizeof(*list), NO_compare, NO_copy);
    *n_names_ret = n_list;
    slist = tmalloc(sizeof(*slist)*n_list);
    *origin_ret = tmalloc(sizeof(**origin_ret)*n_list);
    while (n_list--) {
        slist[n_list]  = list[n_list].name;
        (*origin_ret)[n_list] = list[n_list].origin;
        }
    tfree(list);
    closedir(fdp);
    return(slist);
    }

int NO_compare(NAME_ORIGIN *s1, NAME_ORIGIN *s2)
{
    return(strcmp(s1->name, s2->name));
    }

int NO_copy(NAME_ORIGIN *s1, NAME_ORIGIN *s2)
{
    char *name;
    long origin;

    origin     = s1->origin; name     = s1->name;
    s1->origin = s2->origin; s1->name = s2->name;
    s2->origin = origin    ; s2->name = name;
    }
