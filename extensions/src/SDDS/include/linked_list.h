/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: list.h
 * purpose: definitions for use with linked-list subroutines.
 * 
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/05/25 18:53:19  soliday
 Modified qsort_list parameters

 Revision 1.2  1995/09/05 21:15:08  saunders
 First test release of the SDDS1.5 package.

 */
#ifndef _LINKED_LIST_INCLUDED_
#define _LINKED_LIST_INCLUDED_ 1

struct node {
    void *data;
    short has_data;
    struct node *last, *next;
    } ;

#define SAVE_DATA 0
#define FREE_DATA 1

struct node *initialize_list(struct node **lptr);
struct node *add_to_list(struct node *lptr, void *data);
struct node *insert_in_list(struct node *lptr, void *data);
struct node *remove_from_list(struct node **root, struct node *lptr);
struct node *move_to_node(struct node *lptr, int n);
void free_list(struct node *lptr, int flag);
int swap_list_entries(struct node *lptr1, struct node *lptr2);
void qsort_list(struct node *lptr, int (*compare)(const void *, const void *));
void show_list(FILE *fp, struct node *lptr, int (*show_elem)());

#endif

