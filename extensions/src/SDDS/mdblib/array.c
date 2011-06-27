/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 $Log: array.c,v $
 Revision 1.5  2009/12/18 21:13:17  shang
 moved definition of czarray_2d(), free_czarray_2d(), and resize_czarray_2d() routines from elegant.c

 Revision 1.4  2002/08/14 16:06:04  soliday
 Added Open License

 Revision 1.3  1999/05/25 21:54:44  soliday
 Removed compiler warnings under Linux.

 Revision 1.2  1995/09/05 21:16:43  saunders
 First test release of the SDDS1.5 package.

*/

/* file: array.c
 * contents: zarray_2d(), resize_zarray_2d(), free_zarray_2d()
 *           tmalloc(), treallloc(), zero_memory()
 *
 * Michael Borland, 1989, 1990
 */
#include "mdb.h"

static FILE *fp_tmalloc = NULL;
static FILE *fp_trealloc = NULL;
static FILE *fp_tfree = NULL;

void keep_alloc_record(char *filename)
{
    char s[100];
    
    if (fp_tmalloc)
        free(fp_tmalloc);
    if (fp_trealloc)
        free(fp_trealloc);
    if (fp_tfree)
        free(fp_tfree);
    sprintf(s, "%s.tmalloc", filename);
    fp_tmalloc = fopen_e(s, "w", 0);
    sprintf(s, "%s.trealloc", filename);
    fp_trealloc = fopen_e(s, "w", 0);
    sprintf(s, "%s.tfree", filename);
    fp_tfree = fopen_e(s, "w", 0);
    }

void *tmalloc(
    unsigned long size_of_block
    )
{
    void *ptr;
    static unsigned long total_bytes = 0;

    if (size_of_block<=0)
        size_of_block = 4;

    /* even though the function is tMalloc, I use calloc to get memory filled
     * with zeros 
     */
    if (!(ptr=calloc(size_of_block, 1))) {
        printf("error: memory allocation failure--%lu bytes requested.\n",
                size_of_block);
        printf("tmalloc() has allocated %lu bytes previously\n", total_bytes);
        abort();
        }
    if (fp_tmalloc) {
        fprintf(fp_tmalloc, "%x  %lu\n", (unsigned int)ptr, size_of_block);
        fflush(fp_tmalloc);
        }
    total_bytes += size_of_block;
    return(ptr);
    }

void **zarray_2d(long size, long n1, long n2)
{
    void **ptr1, **ptr0;
    
    ptr0 = ptr1 = (void**)tmalloc((unsigned)(sizeof(*ptr0)*n1));
    while (n1--)
        *ptr1++ = (void*)tmalloc((unsigned)(size*n2));
    return(ptr0);
    }

void **resize_zarray_2d(
        long size, long old_n1, long old_n2, 
        void **array, long n1, long n2
        )
{
    void **ptr;
    
    if (n1>old_n1) {
        /* increase length of array of pointers */
        if (!(array = (void**)trealloc((void*)array, (unsigned)(sizeof(*array)*n1))))
            bomb("memory allocation failuire in resize_zarray_2d()", NULL);
        /* allocate memory for new pointed-to objects */
        ptr = array+n1;
        while (n1--!=old_n1)
            *--ptr = (void*)tmalloc(size*n2);
        }

    if (n2>old_n2) {
        /* increase size of old pointed-to objects */
        ptr = array;
        while (old_n1--) {
            if (!(*ptr = (void*)trealloc((void*)*ptr, (unsigned)(size*n2))))
                bomb("memory allocation failure in resize_zarray_2d()", NULL);
            ptr++;
            }
        }

    return(array);
    }

int free_zarray_2d(void **array, long n1, long n2)
{
    void *ptr0;

    if (!(ptr0 = array))
        return(0);
    while (n1--) {
        if (*array) {
            tfree(*array);
            *array = NULL;
            }
        else
            return(0);
        array++;
        }
    return(tfree(ptr0));
    }

void *trealloc(
    void *old_ptr, unsigned long size_of_block
    )
{
    void *ptr;
    static unsigned long total_bytes = 0;

    if (size_of_block<=0)
        size_of_block = 4;

    if (!old_ptr)
        return(tmalloc(size_of_block));
    if (!(ptr=realloc((void*)old_ptr, (unsigned)(size_of_block)))) {
        printf("error: memory reallocation failure--%lu bytes requested.\n",
                size_of_block);
        printf("trealloc() has reallocated %lu bytes previously\n", total_bytes);
        abort();
        }
    if (fp_trealloc) {
        fprintf(fp_trealloc, "d:%x\na:%x  %lu\n", (unsigned int)old_ptr, (unsigned int)ptr, size_of_block);
        fflush(fp_trealloc);
        }
    total_bytes += size_of_block;
    return(ptr);
    }

void zero_memory(void *mem, long n_bytes)
{
    char *cmem;

    if (!(cmem=mem))
        return;
    while (n_bytes--)
        *cmem++ = 0;
    }

int tfree(void *ptr)
{
    if (fp_tfree) {
        fprintf(fp_tfree, "%x\n", (unsigned int)ptr);
        fflush(fp_tfree);
        }
    if (ptr) {
        free(ptr);
        return(1);
        }
    return(0);
    }

/* file: array.c
 * contents: array_1d(), array_2d(), free_array_1d(), free_array_2d()
 *
 * Michael Borland, 1989, 1990
 */

/* routine: array_1d()
 * purpose: allocate and return a 1-d array 
 */

void *array_1d(long size, long lower_index, long upper_index)
{
    char *ptr;

    if (!(ptr=tmalloc((unsigned)size*(upper_index-lower_index+1))))
        bomb("unable to allocate array (array_1d)", NULL);
    ptr -= lower_index*size;
    return((void*)ptr);
    }

/* routine: array_2d()
 * purpose: return an array of pointers to 1-d arrays.
 */

void **array_2d(long size,
    long lower1, long upper1,
    long lower2, long upper2
    )
/* array is [upper1-lower1+1]x[upper2-lower2+1] */
{
    register long i, n1, n2;
    char **ptr;

    if (!(ptr=tmalloc((unsigned)sizeof(*ptr)*(n1=upper1-lower1+1))))
        bomb("unable to allocate array (array_2d)", NULL);

    n2 = upper2-lower2+1;
    for (i=0; i<n1; i++) {
        if (!(ptr[i]=tmalloc((unsigned)size*n2)))
            bomb("unable to allocate array (array_2d)", NULL);
        ptr[i] -= lower2*size;
        }

    return((void**)(ptr-lower1));
    }

/* routine: free_array_1d()
 * purpose: free a 1-d array 
 */

int free_array_1d(void *array, long size, long lower_index, long upper_index)
{
    if (!array) 
        return(0);
    free((char*)array+size*lower_index);
    return(1);
    }

/* routine: free_array_2d()
 * purpose: free an array of pointers to 1-d arrays.
 */

int free_array_2d(void **array, long size, long lower1, long upper1,
    long lower2, long upper2)
/* array is [upper1-lower1]x[upper2-lower2] */
{
    long i, n1;
    char *ptr;
    
    if (!array)
        return(0);
    
    n1 = upper1-lower1+1;
    array += lower1;
    for (i=0; i<n1; i++) {
      if ((ptr=(char*)array[i]+size*lower2))
        free(ptr);
    }
    
    free(array);
    return(1);
}

/* allocate Continguous Zero-offset 2D array */

void **czarray_2d(long size, long n1, long n2)
{
  char **ptr0;
  char *buffer;
  long i;

  ptr0 = (char**)tmalloc((unsigned)(sizeof(*ptr0)*n1));
  buffer =  (char*)tmalloc((unsigned)(sizeof(*buffer)*size*n1*n2));
  for (i=0; i<n1; i++)
    ptr0[i] = buffer+i*size*n2;
  return((void**)ptr0);
}

void **resize_czarray_2d(void **data, long size, long n1, long n2)
{
  char **ptr0;
  char *buffer;
  long i;
  
  if (!data)
    return czarray_2d(size, n1, n2);
  buffer =  (char*)trealloc(*data, (unsigned)(sizeof(char)*size*n1*n2));
  ptr0 = (char**)trealloc(data, (unsigned)(sizeof(char*)*n1));
  for (i=0; i<n1; i++)
    ptr0[i] = buffer+i*size*n2;
  return((void**)ptr0);
}

/* arguments are for compatibility with free_zarray_2d */
int free_czarray_2d(void **array, long n1, long n2)
{
  free(*array);
  free(array);
  return 0;
}
