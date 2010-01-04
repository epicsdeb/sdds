/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: rawread.c
 * purpose: routines to make raw terminal reads easier.
 * contents: tt_attach(), tt_detach(), rawread()
 * The names tt_attach() and tt_detach() are used for backward compatibility.
 *
 * Michael Borland, 1991.
 $Log: rawread.c,v $
 Revision 1.7  2005/07/29 21:14:31  soliday
 Updated to build with Blue Gene/L. linux-ppc-bgl-gnu

 Revision 1.6  2003/07/22 20:01:19  soliday
 Added support for Kylix.

 Revision 1.5  2002/08/14 16:06:11  soliday
 Added Open License

 Revision 1.4  1999/05/25 21:56:09  soliday
 Removed compiler warnings under Linux.

 Revision 1.3  1995/11/13 16:18:52  borland
 Added long cast to strlen() in comparisons to satisfy Solaris compiler.

 * Revision 1.2  1995/09/05  21:18:05  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include <signal.h>

#if defined(SOLARIS) || defined(LINUX) || (defined(__BORLANDC__) && defined(__linux__))
void tt_detach_and_exit(int);
#else
void tt_detach_and_exit(void);
#endif

#ifdef UNIX
#if defined(__BORLANDC__) || defined(__blrts__)
typedef struct _win_st WINDOW;
#else
#include <curses.h>
#endif

static WINDOW *main_window;

int tt_attach(void)
{
    signal(SIGINT, tt_detach_and_exit);
#ifdef SUNOS4
    signal(SIGHUP, tt_detach_and_exit);
    signal(SIGQUIT, tt_detach_and_exit);
#endif

    main_window = initscr();
    cbreak();
    noecho();
    return(1);
    }

char rawread()
{
    return(getch());
    }

void tt_detach()
{
    echo();
    nocbreak();
    endwin();
    }

#if defined(SOLARIS) || defined(LINUX)
void tt_detach_and_exit(int dummy)
#else
void tt_detach_and_exit()
#endif
{
    echo();
    nocbreak();
    endwin();
    exit(1);
    }
#endif   /* UNIX */

/* file: terminal.c
 * contents: tt_attach(), tt_detach(), rawread()
 * 
 * Routines from tablet.c, by E.Merritt/A.Cox.
 */
#ifdef VAX_VMS
#include <iodef.h>
#include <ssdef.h>

static long _tty_chan = 0;

int tt_attach()
{
    extern long sys$assign();
    register long i;
    struct {
	long	 str_len;
	char	*str_adr;
	} device_name;
    device_name.str_adr = "TT:";
    device_name.str_len = 4;

    signal(SIGINT, tt_detach_and_exit);
    signal(SIGHUP, tt_detach_and_exit);
    signal(SIGQUIT, tt_detach_and_exit);


    if (_tty_chan != 0)
        return(1);
    i = sys$assign(&device_name,&_tty_chan,NULL,NULL); 
    if (i != SS$_NORMAL)
        printf("\nError %ld in assigning channel",i);

    return(0);
    }

void tt_detach(void)
{
    extern long	sys$dassgn();

    if (_tty_chan != 0) 
    	sys$dassgn( _tty_chan );
    _tty_chan = 0;
    }

void tt_detach_and_exit()
{
    extern long	sys$dassgn();

    if (_tty_chan != 0) 
    	sys$dassgn( _tty_chan );
    _tty_chan = 0;
    exit(1);
    }

char rawread()
{
    extern long	sys$qiow();
    register long i, j;
    char input;

    j = IO$_READLBLK | IO$M_NOECHO | IO$M_NOFILTR;
    i = sys$qiow( NULL, _tty_chan, j, NULL,NULL,NULL, &input, 1,
		  NULL, NULL, NULL, NULL );

    return( input );
    }

#endif    /* VAX_VMS */

