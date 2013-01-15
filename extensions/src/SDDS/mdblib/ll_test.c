/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: ltest
 * purpose: test linked-list subroutines
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:17:49  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"
#include "linked_list.h"

typedef struct {
    char *first_name;
    char *last_name;
    char *street_address;
    char *city;
    char *state;
    int zip_code;
    char *phone_number;
    } ADDRESS_BOOK_ENTRY;

#define USAGE "ltest inputfile1 inputfile2"

main(argc, argv)
int argc;
char **argv;
{
    struct node *root, *nptr;
    ADDRESS_BOOK_ENTRY *aptr;
    FILE *fp;
    char s[100];
    int show_entry();
    int last_name_compare();
    double rdrand();
    int i;

    initialize_list(&root);
    
    if (argc!=3) 
        bomb(NULL, USAGE);

    fp = fopen_e(*++argv, "r", 0);

    while (fgets(s, 100, fp)) {
        chop_nl(s);
        aptr = tmalloc(sizeof(*aptr));
        aptr->first_name = get_token(s);
        aptr->last_name  = get_token(s);

        if (!fgets(s, 100, fp)) 
            bomb("missing street address", NULL);
        chop_nl(s);
        cp_str(&(aptr->street_address), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing city", NULL);
        chop_nl(s);
        cp_str(&(aptr->city), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing state", NULL);
        chop_nl(s);
        cp_str(&(aptr->state), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing zip code", NULL);
        chop_nl(s);
        get_int(&(aptr->zip_code), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing phone number", NULL);
        chop_nl(s);
        cp_str(&(aptr->phone_number), s);

        add_to_list(root, (void *)aptr);
        }
    fclose(fp);

    puts("All entries read in and scanned.");
/*    show_list(stdout, root, show_entry); */

    puts("Sorting according to last name.");
    qsort_list(root, last_name_compare);

/*    show_list(stdout, root, show_entry); */

    puts("Opening second file and scanning.");
    fp = fopen_e(*++argv, "r", 0);
    while (fgets(s, 100, fp)) {
        chop_nl(s);
        aptr = tmalloc(sizeof(*aptr));
        aptr->first_name = get_token(s);
        aptr->last_name  = get_token(s);

        if (!fgets(s, 100, fp)) 
            bomb("missing street address", NULL);
        chop_nl(s);
        cp_str(&(aptr->street_address), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing city", NULL);
        chop_nl(s);
        cp_str(&(aptr->city), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing state", NULL);
        chop_nl(s);
        cp_str(&(aptr->state), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing zip code", NULL);
        chop_nl(s);
        get_int(&(aptr->zip_code), s);

        if (!fgets(s, 100, fp)) 
            bomb("missing phone number", NULL);
        chop_nl(s);
        cp_str(&(aptr->phone_number), s);

        nptr = move_to_node(root, 2);
        if (nptr!=NULL)
            insert_in_list(nptr, (void *)aptr);
        else 
            add_to_list(root, (void *)aptr);
        }
    fclose(fp);
    
    puts("All entries read in and scanned.");
/*    show_list(stdout, root, show_entry); */

    puts("Sorting according to last name.");
    qsort_list(root, last_name_compare);
/*    show_list(stdout, root, show_entry); */

    puts("Swapping.");
    for (i=0; i<100; i++) {
        nptr = move_to_node(root, (int)rdrand(1.0, 6.0));
        if (nptr)
            swap_list_entries(root, nptr);
        }
/*    show_list(stdout, root, show_entry); */

    puts("Sorting according to last name.");
    qsort_list(root, last_name_compare);
/*    show_list(stdout, root, show_entry); */

    puts("Removing entries.");
    remove_from_list(&root, move_to_node(root,0));
    remove_from_list(&root, move_to_node(root,3));
    remove_from_list(&root, move_to_node(root,3));
    show_list(stdout, root, show_entry);
    }

int show_entry(fp, aptr)
FILE *fp;
ADDRESS_BOOK_ENTRY *aptr;
{
    fprintf(fp, "name    : %s %s\n", aptr->first_name, aptr->last_name);
    fprintf(fp, "address : %s\n", aptr->street_address);
    fprintf(fp, "          %s, %s  %5ld\n", aptr->city, aptr->state, aptr->zip_code);
    fprintf(fp, "phone   : %s\n", aptr->phone_number);
    }

int last_name_compare(aptr1, aptr2)
ADDRESS_BOOK_ENTRY **aptr1, **aptr2;
{
    return(strcmp((*aptr1)->last_name, (*aptr2)->last_name));
    }

