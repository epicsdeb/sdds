/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDSnl.nl
 * purpose: namelist preprocessor input file for SDDS library
 *
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.3  1998/07/07 22:29:45  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

# Revision 1.2  1995/09/05  21:11:24  saunders
# First test release of the SDDS1.5 package.
#
 */
#include "namelist.h"

#namelist description static struct
    STRING text = NULL;
    STRING contents = NULL;
#end

#namelist column static struct
    STRING name = NULL;
    STRING symbol = NULL;
    STRING units = NULL;
    STRING description = NULL;
    STRING format_string = NULL;
    STRING type = NULL;
    long field_length = 0;
#endif

#namelist parameter static struct
    STRING name = NULL;
    STRING symbol = NULL;
    STRING units = NULL;
    STRING description = NULL;
    STRING format_string = NULL;
    STRING type = NULL;
    STRING fixed_value = NULL;
#endif

#namelist associate static struct
    STRING name = NULL;
    STRING filename = NULL;
    STRING path = NULL;
    STRING description = NULL;
    STRING contents = NULL;
    long sdds = 0;
#endif

#namelist data static struct
    STRING mode = NULL;
    long lines_per_row = 1;
    long no_row_counts = 0;
    long additional_header_lines = 0;
    long do_line_padding = 1;
#endif

#namelist include static struct
    STRING filename = NULL;
#endif

#namelist array static struct
    STRING name = NULL;
    STRING symbol = NULL;
    STRING units = NULL;
    STRING description = NULL;
    STRING format_string = NULL;
    STRING type = NULL;
    long field_length = 0;
    long dimensions = 1;
    STRING group_name = NULL;
#endif

