/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* routine: diffeq_result_description();
 * purpose: return string describing result returned by differential equation subroutines.
 * 
 * Michael Borland, 1991.
 $Log: not supported by cvs2svn $
 Revision 1.3  1999/07/29 21:24:45  borland
 Added a description

 * Revision 1.2  1995/09/05  21:19:31  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"

#define N_DIFFEQ_RETURNS 8

static struct {
    long code;
    char *description;
    } diffeq_return[N_DIFFEQ_RETURNS] = {
    { -4, "exit condition failure"},
    { -3, "zero stepsize"},
    { -2, "can't take initial step"},
    { -1, "solution stepped outside of integration interval"},
    {  0, "initial independent variable value greater than desired final value"},
    {  1, "differential equations solved already"},
    {  2, "zero of exit-function found"},
    {  3, "end of integration interval reached"}
    };
static char *unknown = "unknown error";
char *diffeq_result_description(long result_code)
{
    int i;
    for (i=0; i<N_DIFFEQ_RETURNS; i++) {
        if (result_code==diffeq_return[i].code)
            return(diffeq_return[i].description);
        }
    return unknown;
    }


