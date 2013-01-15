/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* FILE    : timer.c
 * CONTENTS: cpu_time(), page_faults(), elapsed_time(), bio_count(),  
 *           dio_count(), memory_count(), run_stats(), init_stats()
 *
 * VAX/VMS:
 *     These routines use the library routines lib$init_timer() and lib$stat_timer
 *     to allow collection of run-time statistics.
 * SUN UNIX:
 *     These routines use the getrusage() routine to get run-time statistics
 * 
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.20  2008/08/01 13:15:33  borland
 Modified to disable memory_count() on powerPC and also improve error
 messages when memory_count() fails.  Per Y. Wang.

 Revision 1.19  2008/02/26 21:12:30  borland
 Kludged something together to work on linux for getting memory usage.

 Revision 1.18  2007/09/05 20:36:24  soliday
 There are still problems running ioctl on Linux so I put it back the way it was.

 Revision 1.17  2007/09/05 19:46:35  soliday
 Made another change because the memory_count function cannot run on Darwin.

 Revision 1.16  2007/09/05 19:32:33  soliday
 Updated to fix problem on Linux.

 Revision 1.15  2002/08/14 16:06:17  soliday
 Added Open License

 Revision 1.14  2001/08/21 21:35:45  soliday
 Fixed problem with last change.

 Revision 1.13  2001/08/20 19:32:52  soliday
 Fixed for Redhat 7.1

 Revision 1.12  2000/08/10 21:14:28  soliday
 Added support for gcc on Solaris.

 Revision 1.11  2000/04/13 16:15:13  soliday
 Fixed prototype of delapsed_time

 Revision 1.10  2000/04/04 04:49:21  soliday
 Added some missing header include files for solaris.

 Revision 1.9  2000/01/14 17:58:47  borland
 Changed implementation of cpu_time() for Solaris to avoid the limitations
 of the clock() routine (rolls over at 2147s).

 Revision 1.8  2000/01/14 17:48:13  borland
 The memory_count() function now works on Solaris.

 Revision 1.7  1999/12/15 15:44:30  soliday
 Added vxWorks support.

 Revision 1.6  1999/08/24 13:41:14  borland
 Resolved apparent array bounds problem with addition of arguments from a file.
 Funny that it never happened til now.

 Revision 1.5  1999/07/08 20:38:20  soliday
 Changed defined(WIN32) to defined(_WIN32)

 Revision 1.4  1999/05/25 21:54:09  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1999/05/25 18:54:13  soliday
 Added WIN32 support

 Revision 1.2  1995/09/05 21:18:38  saunders
 First test release of the SDDS1.5 package.

 */
#include "mdb.h"

#ifdef VAX_VMS
#include <ssdef.h>

static long timer_initialized=0;

void init_stats()
{
    timer_initialized = 1;
    lib$init_timer();
    }

/* routine delapsed_time() returns the clock time elapsed since init_stats()
 * was last called, as a number
 */

double delapsed_time()
{
    char *ptr;
    long h, m;
    double s;

    if (ptr = elapsed_time()) {
        *(ptr+2) = *(ptr+5) = 0;
        sscanf(ptr, "%ld", &h);
        sscanf(ptr+3, "%ld", &m);
        sscanf(ptr+6, "%lf", &s);
        return(h*3600+m*60+s);
        }
    return(0.0L);
    }


/* routine elapsed_time() returns the clock time elapsed since init_stats()
 * was last called, as a character string
 */

char *elapsed_time()
{
    static long tbuf[2];

    if (lib$stat_timer(&1, tbuf)==SS$_NORMAL) {
        if (!timer_initialized)
            return(NULL);
        return(asc_time(tbuf, 1));
        }
    return(NULL);
    }

/* routine cpu_time() returns the cpu time used since init_stats() was
 * last called
 */

long cpu_time()
{
    long _cputime;
    if (lib$stat_timer(&2, &_cputime)==SS$_NORMAL)
        return(timer_initialized?_cputime:-1);
    return(-1);
    }

/* routine bio_count() returns the buffered io count since init_stats() was
 * last called
 */

long bio_count()
{
    long _bio_count;
    if (lib$stat_timer(&3, &_bio_count)==SS$_NORMAL)
        return(timer_initialized?_bio_count:-1);
    return(-1);
    }

/* routine dio_count() returns the direct io count since init_stats() was
 * last called
 */

long dio_count()
{
    long _dio_count;
    if (lib$stat_timer(&4, &_dio_count)==SS$_NORMAL)
        return(timer_initialized?_dio_count:-1);
    return(-1);
    }

/* routine page_faults() returns the page faults since init_stats() was
 * last called
 */

long page_faults()
{
    long _page_faults;
    if (lib$stat_timer(&5, &_page_faults)==SS$_NORMAL)
        return(timer_initialized?_page_faults:-1);
    return(-1);
    }

#include <jpidef.h>
#include <ssdef.h>

typedef struct {
    short buffer_length;
    short code;
    long *buffer_address;
    long *return_length_address;
    } ITEM_LIST;

long memory_count()
{
    static ITEM_LIST *item;
    static long initialized = 0;
    long mem;
   
    if (!initialized) {
        /* allocate an item list structure */
        item = tmalloc((unsigned)sizeof(*item)+sizeof(long));
        item->buffer_address = tmalloc((unsigned)4);   /* 1 longword */
        item->return_length_address = tmalloc((unsigned)4); /* 1 longword */
        *((char*)item+sizeof(*item)) = 0;
        initialized = 1;
        }

    /* get global page count */
    item->code = JPI$_GPGCNT;
    item->buffer_length = 4;
    if (sys$getjpiw(0, 0, 0, item, 0, 0, 0)!=SS$_NORMAL)
        return(0);
    mem = *(item->buffer_address);

    /* get page count in working set */
    item->code = JPI$_PPGCNT;
    item->buffer_length = 4;
    if (sys$getjpiw(0, 0, 0, item, 0, 0, 0)!=SS$_NORMAL)
        return(0);

    return(mem + *(item->buffer_address));
    }
#endif /* VAX_VMS */

#if defined(UNIX) || defined(_WIN32) || defined(vxWorks)
double delapsed_time(void);

/* routine elapsed_time() returns the clock time elapsed since init_stats()
 * was last called, as a character string
 */

char *elapsed_time()
{
    static char buffer[20];
    static double dtime;
    int h, m, s;

    dtime = delapsed_time();
    h = dtime/3600;
    dtime -= h*3600;
    m = dtime/60;
    dtime -= m*60;
    s = dtime;
 
    sprintf(buffer, "%02d:%02d:%02d", h, m, s);
    return(buffer);
    }


#if defined(SUNOS4)

#include <sys/time.h>
#include <sys/resource.h>

/* routine delapsed_time() returns the clock time elapsed since init_stats()
 * was last called, as a number
 */

static double delapsed_start = 0;
void init_stats()
{
    delapsed_start = delapsed_time();
    }

/* int getrusage(int who, struct rusage *ru); */

#include <sys/timeb.h>
int ftime(struct timeb *tp);

double delapsed_time()
{
    static struct timeb tp;

    ftime(&tp);
    return((long)tp.time + ((long)tp.millitm)/1000.0 - delapsed_start);
    }


/* routine cpu_time() returns the cpu time used since init_stats() was
 * last called.   time is in hundredths of seconds
 */

long cpu_time()
{
    static struct rusage ru;
    if (getrusage(RUSAGE_SELF, &ru)==0) {
        return( (ru.ru_utime.tv_sec+ru.ru_stime.tv_sec)*100 + 
                (ru.ru_utime.tv_usec+ru.ru_stime.tv_usec)/10000 );
        }
    return(-1);
    }

/* routine bio_count() returns the buffered io count since init_stats() was
 * last called
 */

long bio_count()
{
    static struct rusage ru;

    if (getrusage(RUSAGE_SELF, &ru)==0) {
        return( ru.ru_inblock+ru.ru_oublock );
        }
    return(-1);
    }

/* routine dio_count() returns the direct io count since init_stats() was
 * last called
 */

long dio_count()
{
    return(0);
    }

/* routine page_faults() returns the page faults since init_stats() was
 * last called
 */

long page_faults()
{
    static struct rusage ru;

    if (getrusage(RUSAGE_SELF, &ru)==0) {
        return(ru.ru_majflt + ru.ru_minflt);
        }
    return(-1);
    }

#include <sys/procfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

long memory_count()
{
  int fd;
  static short first = 1;
  static char proc[100];
  prpsinfo_t psinfo;
  
  if (first) {
    sprintf(proc, "/proc/%ld", getpid());
    first = 0;
  }
  if ((fd=open(proc, O_RDONLY))==-1) {
    perror("open failed in memory_count()");
    exit(1);
  }
  if (ioctl(fd, PIOCPSINFO, &psinfo)==-1) {
    perror("ioctl failed in memory_count()");
    exit(1);
  }
  close(fd);
  return psinfo.pr_bysize/1024;
}

#else /* SUNOS4  */

#if defined(_WIN32)
#include <time.h>
#else
#if defined(vxWorks)
#include <sysLib.h>
#include <time.h>
#else
#if defined(linux)
#include <time.h>
#endif
#include <sys/time.h>
#endif
#endif

static double delapsed_start = 0;
void init_stats()
{
    delapsed_start = delapsed_time();
    clock();
    }

double delapsed_time()
{
    static long delapsed_start = -1;
    if (delapsed_start==-1) {
        delapsed_start = time(0);
        return 0.0;
        }
    else
        return time(0) - delapsed_start;
    }

/* routine cpu_time() returns the cpu time used since init_stats() was
 * last called.   time is in hundredths of seconds
 */

#if defined(SOLARIS)
#include <sys/procfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>

long cpu_time()
/* returns memory usage in kBytes */
{
  int fd;
  static short first = 1;
  static char proc[100];
  prpsinfo_t psinfo;
  timestruc_t *prtime;  
  if (first) {
    sprintf(proc, "/proc/%ld", getpid());
    first = 0;
  }
  if ((fd=open(proc, O_RDONLY))==-1) {
    perror("open");
    exit(1);
  }
  if (ioctl(fd, PIOCPSINFO, &psinfo)==-1) {
    perror("ioctl");
    exit(1);
  }
  close(fd);
  prtime = &psinfo.pr_time;
  return (prtime->tv_sec)*100+(prtime->tv_nsec)/1e7;
}
#else
long cpu_time()
{
  return (long)((clock()*100.0)/CLOCKS_PER_SEC);
}
#endif

/* routine bio_count() returns the buffered io count since init_stats() was
 * last called
 */

long bio_count()
{
    return 0;
    }

/* routine dio_count() returns the direct io count since init_stats() was
 * last called
 */

long dio_count()
{
    return(0);
    }

/* routine page_faults() returns the page faults since init_stats() was
 * last called
 */

long page_faults()
{
    return 0;
    }

#if defined(SOLARIS)
#include <sys/procfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

long memory_count()
/* returns memory usage in kBytes */
{
  int fd;
  static short first = 1;
  static char proc[100];
  prpsinfo_t psinfo;
  
  if (first) {
    sprintf(proc, "/proc/%ld", getpid());
    first = 0;
  }
  if ((fd=open(proc, O_RDONLY))==-1) {
    perror("open failed in memory_count()");
    exit(1);
  }
  if (ioctl(fd, PIOCPSINFO, &psinfo)==-1) {
    perror("ioctl failed in memory_count()");
    exit(1);
  }
  close(fd);
  return psinfo.pr_bysize/1024;
}
#else
#if defined(linux) && !defined(__powerpc__)
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

long memory_count()
/* returns memory usage in kBytes */
{
  FILE *fp;
  static short first = 1;
  static char proc[100];
  long size1;
  
  if (first) {
    sprintf(proc, "/proc/%ld/statm", (long)getpid());
    first = 0;
  }
  if (!(fp=fopen(proc, "r"))) {
    perror("fopen failed in memory_count()");
    exit(1);
  }
  if (fscanf(fp, "%ld", &size1)!=1) {
    perror("fscanf failed in memory_count()");
    exit(1);
  }
  fclose(fp);
  /* Assume a page is 1 kB */
  return size1;
}
#else
long memory_count()
{
    return 0;
}
#endif
#endif

#endif

#endif /* UNIX || WIN32 */


