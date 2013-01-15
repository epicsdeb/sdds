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
 Revision 1.3  2002/08/14 16:06:05  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:16:58  saunders
 First test release of the SDDS1.5 package.

*/

#include "buffer.h"
#include "mdb.h"

main()
{
    char s[100], name[100];

    while (1) {
        do {
            queryn("buffer name to create/add to: ", name, 100);
            if (is_blank(name))
                break;
            chop_nl(name);
            if (!buffer_exists(name))
                create_buffer(name, 1);
            queryn("line to store: ", s, 100);
            add_to_buffer(name, s);
            } while (1);
    
        do {
            queryn("buffer name to print: ", name, 100);
            if (is_blank(name))
                break;
            chop_nl(name);
            if (!buffer_exists(name)) {
                puts("no such buffer");
                continue;
                }
            fprintf_buffer(stdout, "line: %s\n", name);
            } while (1);
        }
    }

