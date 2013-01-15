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
 Revision 1.3  2002/08/14 16:06:18  soliday
 Added Open License

 Revision 1.2  1995/09/05 21:18:49  saunders
 First test release of the SDDS1.5 package.

*/

#include "mdb.h"

char *expand_ranges(char *template);

main()
{
    char s[100], wt[100], *nt;
    long i;

    do {
        queryn("template: ", wt, 100);
        if (has_wildcards(wt))
            printf("template has wildcards\n");
        chop_nl(wt);
        if (is_blank(wt))
            break;
        nt = expand_ranges(wt);
        printf("expanded template: %s\n", nt);
        do {
            queryn("string: ", s, 100);
            chop_nl(s);
            if (s[0]=='q')
                break;
            if ((i=wild_match(s, nt)))
                printf("%s is matched by %s--return value %ld\n", s, wt, i);
            else
                printf("%s is not matched by %s\n", s, wt);
            } while (1);
        } while (1);
    }
