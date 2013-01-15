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
 Revision 1.3  1999/05/25 21:51:50  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:16:55  saunders
 First test release of the SDDS1.5 package.

*/

/* file: buffer.c 
 * purpose: provide buffering of text strings 
 * 
 * Michael Borland, 1990
 */
#include "mdb.h"
#include "buffer.h"

#define DEBUG 0

struct buffer_struct {
    char *name;
    char **line;
    int n_lines_stored;
    int n_slots;
    int n_slots_increment;
    struct buffer_struct *next;
    } *buffers = NULL; 

#if DEBUG
void list_buffers(void);
#endif

void create_buffer(char *name, int increment)
{
    struct buffer_struct *bptr;

    if (buffers==NULL) {
        buffers = tmalloc(sizeof(*buffers));
        buffers->next = NULL;
        buffers->n_slots = buffers->n_lines_stored = 0;
        cp_str(&(buffers->name), name);
        buffers->n_slots_increment = increment?increment:1;
#if DEBUG
        printf("buffer %s created--increment is %d\n", 
                buffers->name, buffers->n_slots_increment);
#endif
        }
    else {
        if (find_buffer(name))
            bomb("duplicate buffer creation (create_buffer)", NULL);
        bptr = buffers;
        while (bptr->next)
            bptr = bptr->next;
        bptr = bptr->next = tmalloc(sizeof(*buffers));
        bptr->n_lines_stored = bptr->n_slots = 0;
        cp_str(&(bptr->name), name);
        bptr->n_slots_increment = increment?increment:1;
#if DEBUG
        printf("buffer %s created--increment is %d\n", 
                bptr->name, bptr->n_slots_increment);
#endif
        }
#if DEBUG
    list_buffers();
#endif
    }

void add_to_buffer(char *name, char *string)
{
    struct buffer_struct *bptr;

    if (!(bptr = find_buffer(name)))
        bomb("unknown buffer referenced (add_to_buffer)", NULL);
    
    if (bptr->n_slots==0)
        bptr->line = (char**)tmalloc(sizeof(*bptr->line)*
                        (bptr->n_slots=bptr->n_slots_increment));
    else if (bptr->n_lines_stored==bptr->n_slots) {
        if (!(bptr->line = (char**)trealloc(bptr->line, sizeof(*bptr->line)*
                        (bptr->n_slots+=bptr->n_slots_increment))))
            bomb("trealloc failure in add_to_buffer()", NULL);
        }
    cp_str(bptr->line+bptr->n_lines_stored++, string);
#if DEBUG
        printf("\"%s\" added to buffer %s--total of %d lines\n",
                string, buffers->name, buffers->n_lines_stored);
#endif
    }

void clear_buffer(char *name)
{
    int i;
    struct buffer_struct *bptr;
  
    if (!(bptr=find_buffer(name)))
        bomb("unknown buffer referenced (clear_buffer)", NULL);
    for (i=0; i<bptr->n_lines_stored; i++) 
        tfree(bptr->line[i]);
    bptr->n_lines_stored = 0;
  }

void fprintf_buffer(FILE *fp, char *format, char *name)
{
    struct buffer_struct *bptr;
    int i;

    if (!(bptr = find_buffer(name)))
        bomb("unknown buffer referenced (fprintf_buffer)", NULL);
    for (i=0; i<bptr->n_lines_stored; i++) {
        fprintf(fp, format, bptr->line[i]);
        }
    }


void fputs_buffer(char *name, FILE *fp)
{
    struct buffer_struct *bptr;
    int i;

    if (!(bptr = find_buffer(name)))
        bomb("unknown buffer referenced (fputs_buffer)", NULL);
    for (i=0; i<bptr->n_lines_stored; i++) {
        fputs(bptr->line[i], fp);
        }
    }

struct buffer_struct *find_buffer(char *name)
{
    struct buffer_struct *bptr;

    if ((bptr = buffers)) {
        do {
            if (strcmp(name, bptr->name)==0)
                return(bptr);
            } while ((bptr = bptr->next));
        }
    return(NULL);
    }

int buffer_exists(char *name)
{
    if (find_buffer(name))
        return(1);
    return(0);
    }

#if DEBUG
void list_buffers(void)
{
    struct buffer_struct *bptr;
    bptr = buffers;

    while (bptr) {
        printf("buffer %s has %d lines\n", bptr->name, bptr->n_lines_stored);
        bptr = bptr->next;
        }
    }
#endif
