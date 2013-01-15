/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file : linked_list.c
 * contents: initialize_list(), add_to_list(), insert_in_list(),
 *           remove_from_list(), move_to_node(), free_list(), 
 *           swap_list_entries(), qsort_list()
 *
 * purpose: routines for making and using linked lists
 * 
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/05/25 18:51:49  soliday
 Modified qsort_list parameters

 Revision 1.2  1995/09/05 21:17:47  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"
#include "linked_list.h"

/* routine: initialize_list()
 * purpose: initialize the root node of a linked list.
 * usage  : the calling routine passes a pointer to a pointer to the root node.
 * returns: a pointer to the root node.
 */

struct node *initialize_list(struct node **lptr)
{
    (*lptr) = tmalloc(sizeof(**lptr));
    (*lptr)->has_data = 0;
    (*lptr)->last = (*lptr)->next = NULL;
    (*lptr)->data = NULL;
    return(*lptr);
    }

/* routine: add_to_list()
 * purpose: add an entry at the end of a linked list.
 * usage  : the calling routine passes a pointer to any node in the list,
 *          along with a pointer to the data.
 * returns: a pointer to the new node.
 */

struct node *add_to_list(struct node *lptr, void *data)
{
    while (lptr->next!=NULL && lptr->has_data) {
        lptr = lptr->next;
        }
    if (!lptr->has_data) {
        /* have reached an empty node, which by construction is followed
         * only by other empty nodes.
         */
        lptr->has_data = 1;
        lptr->data     = data;
        }
    else if (lptr->next==NULL) {
        /* need to add another node at the end */
        lptr->next       = tmalloc(sizeof(*lptr));
        lptr->next->last = lptr;
        lptr             = lptr->next;
        lptr->next       = NULL;

        lptr->has_data = 1;
        lptr->data     = data;

        return(lptr);
        }
    return(lptr);
    }

/* routine: insert_in_list()
 * purpose: insert a node into a given point in a linked-list.
 * usage  : the calling routine provides pointers to the data to be
 *          inserted and to the node after which it is to be inserted.
 * returns: a pointer to the new node.
 */

struct node *insert_in_list(struct node *lptr, void *data)
{
    struct node *next, *new;

    if (lptr==NULL)
        return(NULL);

    if (lptr->next==NULL) 
        return(add_to_list(lptr, data));

    /* allocate node and put data in */
    new           = tmalloc(sizeof(*lptr));
    new->data     = data;
    new->has_data = 1;

    /* insert in the list--use temporary variables for clarity */

    /* -- set links in new node point to its last and next nodes */
    new->last  = lptr;
    new->next  = lptr->next;

    /* -- change links in other nodes to point to new node */
    next       = lptr->next;
    lptr->next = new;
    next->last = new;

    return(new);
    }

/* routine: remove_from_list()
 * purpose: remove an element from a linked-list
 * usage  : calling routine passes a pointer to the pointer to first 
 *           node in the list and a pointer to the node to be deleted
 * returns: pointer to the node that was removed.
 * comments: The next and last pointers in the removed node are NOT changed.
 *           This allows access to the nodes at the point of removal.
 */

struct node *remove_from_list(struct node **root, struct node *lptr)
{
    if (root==NULL || *root==NULL || lptr==NULL)
        return(NULL);

    if (*root==lptr) {
        /* node to be removed is first node--change root pointer */
        *root = lptr->next;
        }

    if (lptr->next!=NULL) {
        /* set next node to have new last node that is the same as the 
         * last node of node being deleted.
         */
        lptr->next->last = lptr->last;
        }
    if (lptr->last!=NULL) {
        /* set last node to have new next node that is the same as the
         * next node of the node being deleted.
         */
        lptr->last->next = lptr->next;
        }
    return(lptr);
    }

/* routine: move_to_node()
 * purpose: move to the ith node in a linked-list.
 * usage  : the calling routine passes the address of any node in the
 *          list, and the number of the node desired (0 - n_nodes-1)
 * returns: a pointer to the node desired, or NULL if it doesn't exist.
 */

struct node *move_to_node(struct node *lptr, int n)
{
    register int i;

    if (lptr==NULL)
        return(lptr);

    while (lptr->last!=NULL)
        lptr = lptr->last;
    
    for (i=0; i<n; i++) {
        if (lptr->next!=NULL)
            lptr = lptr->next;
        else 
            break;
        }
    if (i==n)
        return(lptr);
    return(NULL);
    }

/* routine: free_list()
 * purpose: erase linked-list, optionally freeing the memory used for the data.
 * usage  : the calling routine passes a pointer to any node on the list,
 *          as wells as a flag variable.  Flag values are SAVE_DATA and 
 *          FREE_DATA.
 * returns: nothing.
 */

void free_list(struct node *lptr, int flag)
{
    struct node *next;

    if (lptr==NULL)
        return;

    while (lptr->last!=NULL)
        lptr = lptr->last;
    
    while (lptr!=NULL) {
        next = lptr->next;
        if (flag==FREE_DATA)
            tfree(lptr->data);
        tfree(lptr);
        lptr = next;
        }
    }

/* routine: swap_list_entries()
 * purpose: swap two linked list entries.
 * usage  : the caller provides pointers to the two entries.
 * returns: 0 if either entry is NULL, non-zero otherwise
 * comments: The two entries need not belong to the same linked-list.
 *           The swap is accomplished by exchanging data pointers, not
 *           by changing the links.
 */

int swap_list_entries(struct node *lptr1, struct node *lptr2)
{
    void *tmp_ptr;
    short tmp_flag;

    if (lptr1==NULL || lptr2==NULL)
        return(0);

    tmp_ptr  = lptr1->data;
    tmp_flag = lptr1->has_data;

    lptr1->data     = lptr2->data;    
    lptr1->has_data = lptr2->has_data;

    lptr2->data     = tmp_ptr;
    lptr2->has_data = tmp_flag;

    return(1);
    }

/* routine: qsort_list()
 * purpose: sort a linked list using quick-sort algorithm.
 * usage  : the calling routine provides a pointer to any node on the
 *          list and a pointer to a function that compares two data
 *          items, each passed by pointer, and returns >,=, or < 0
 *          as the first argument is greater, equal to, or less than
 *          the second.
 * returns: nothing.
 * comments: The data pointers are copied into an array, sorted, and then
 *           copied back into the nodes.  The list links are never changed.
 */

void qsort_list(struct node *lptr, int (*compare)(const void *, const void *))
{
    register void **ptr;
    register int i, n_items;
    struct node *root;

    if (lptr==NULL)
        return;
    
    while (lptr->last!=NULL)
        lptr = lptr->last;
    root = lptr;

    n_items = 0;
    while (lptr && lptr->has_data) {
        n_items++;
        lptr = lptr->next;
        }
    
    if (n_items==0)
        return;
    
    ptr  = tmalloc(sizeof(*ptr)*n_items);
    lptr = root;
    for (i=0; i<n_items; i++) {
        ptr[i] = lptr->data;
        lptr   = lptr->next;
        }

    qsort(ptr, (size_t)n_items, (size_t)(sizeof(*ptr)), compare);

    lptr = root;
    for (i=0; i<n_items; i++) {
        lptr->data = ptr[i];
        lptr   = lptr->next;
        }

    tfree(ptr);
    }

/* routine: show_list()
 * purpose: print the elements of a list, starting at the node passed,
 *          to a file.
 */

void show_list(FILE *fp, struct node *lptr, int (*show_elem)())
{
    
    if (lptr==NULL)
        return;
    
    while (lptr) {
        if (lptr->has_data) 
            (*show_elem)(fp, lptr->data);
        else 
            fprintf(fp, "- empty -\n");

        lptr = lptr->next;
        }
    }

