/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddssplit
 * purpose: split a SDDS file up into many files, one page per file
 *
 * Michael Borland, 1994, 1995
 $Log: not supported by cvs2svn $
 Revision 1.8  2006/12/14 22:22:00  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.7  2002/08/14 17:12:56  soliday
 Added Open License

 Revision 1.6  2001/01/10 19:35:48  soliday
 Standardized usage message.

 Revision 1.5  1999/05/25 19:16:48  soliday
 Removed compiler warning on linux.

 Revision 1.4  1999/01/06 19:54:58  borland
 Fixed the version number in the usage message.

 * Revision 1.3  1995/09/06  14:57:18  saunders
 * First test release of SDDS1.5
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_BINARY 0
#define SET_ASCII 1
#define SET_DIGITS 2
#define SET_ROOTNAME 3 
#define SET_FIRST_PAGE 4
#define SET_LAST_PAGE 5
#define SET_INTERVAL 6
#define SET_EXTENSION 7
#define SET_PIPE 8
#define SET_NAMEPARAMETER 9
#define SET_OFFSET 10
#define N_OPTIONS 11

char *option[N_OPTIONS] = {
    "binary", "ascii", "digits", "rootname", "firstpage", "lastpage", "interval", "extension",
    "pipe", "nameparameter", "offset",
    } ;

char *USAGE = "sddssplit <inputFile> -pipe[=input]\n\
 [-binary | -ascii] [-digits=<number>] [-rootname=<string>]\n\
 [-firstPage=<number>] [-lastPage=<number>] [-interval=<number>] [-extension=<string>]\n\
 [-nameParameter=<filenameParameter>] [-offset=<integer>]\n\n\
sddssplit splits a SDDS file into many SDDS files, with each page going to a separate file. \
The files are named <rootname><integer>.sdds, where <rootname> is either the filename for \
the source file or the specified string, and where <integer> is by default <page-number>-<offset> \
and is printed to the number of digits given by -digits (3 is the default).\n\n\
Program by Michael Borland. (This is version 5, March 2009, M. Borland.)\n";

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset, SDDS_orig;
    long i_arg, offset;
    SCANNED_ARG *s_arg;
    char *input, *rootname, name[500], format[100], *extension;
    long ascii_output, binary_output, retval, digits;
    long first_page, last_page, interval;
    unsigned long pipeFlags;
    char *fileParameter, *nameFromParameter;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2) 
        bomb(NULL, USAGE);

    input = rootname = NULL;
    ascii_output = binary_output = 0;
    digits = 3;
    first_page = last_page = interval = 0;
    extension = "sdds";
    pipeFlags = 0;
    fileParameter = NULL;
    offset = 0;
    
    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            delete_chars(s_arg[i_arg].list[0], "_");
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_BINARY:
                binary_output = 1;
                ascii_output = 0;
                break;
              case SET_ASCII:
                ascii_output = 1;
                binary_output = 0;
                break;
              case SET_DIGITS:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &digits)!=1 || digits<=0)
                    SDDS_Bomb("invalid -digits syntax");
                break;
              case SET_ROOTNAME:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -rootname syntax");
                rootname = s_arg[i_arg].list[1];
                break;
              case SET_FIRST_PAGE:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &first_page)!=1 || first_page<=0)
                    SDDS_Bomb("invalid -firstpage syntax");
                break;
              case SET_LAST_PAGE:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &last_page)!=1 || last_page<=0)
                    SDDS_Bomb("invalid -lastpage syntax");
                break;
              case SET_INTERVAL:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &interval)!=1 || interval<=0)
                    SDDS_Bomb("invalid -interval syntax");
                break;
              case SET_EXTENSION:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -extension syntax");
                extension = s_arg[i_arg].list[1];
                break;
              case SET_OFFSET:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &offset)!=1)
                  SDDS_Bomb("invalid -offset syntax");
                break;
              case SET_PIPE:
                pipeFlags = USE_STDIN;
                break;
              case SET_NAMEPARAMETER:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -nameParameter syntax");
                fileParameter = s_arg[i_arg].list[1];
                break;
              default:
                fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
                bomb(NULL, USAGE);
                break;
                }
            }
        else {
            if (input==NULL)
                input = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }

    if (!input && !(pipeFlags&USE_STDIN))
        SDDS_Bomb("too few filenames");
    
    if (pipeFlags&USE_STDIN && !fileParameter && !rootname)
        SDDS_Bomb("you must give -rootname or -nameParameter with -pipe");

    if (!rootname && !fileParameter) {
        if ((rootname=strrchr(input, '.'))) {
            *rootname = 0;
            SDDS_CopyString(&rootname, input);
            input[strlen(input)] = '.';
            }
        else 
            SDDS_CopyString(&rootname, input);
        }
    if (first_page && last_page && first_page>last_page) 
        SDDS_Bomb("first_page>last_page");

    if (!SDDS_InitializeInput(&SDDS_orig, input)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }

    if (!extension || SDDS_StringIsBlank(extension)) {
        extension = NULL;
        sprintf(format, "%%s%%0%ldld", digits);
        }
    else
        sprintf(format, "%%s%%0%ldld.%s", digits, extension);

    if (fileParameter && 
        SDDS_CheckParameter(&SDDS_orig, fileParameter, NULL, SDDS_STRING, stderr)!=SDDS_CHECK_OKAY)
        SDDS_Bomb("filename parameter not present or wrong type");

    while ((retval=SDDS_ReadPage(&SDDS_orig))>0) {
        if (first_page && retval<first_page)
            continue;
        if (last_page && retval>last_page)
            break;
        if (interval) {
            if (first_page) {
                if ((retval-first_page)%interval!=0)
                    continue;
                }
            else if ((retval-1)%interval!=0)
                continue;
            }
        if (fileParameter) {
            if (!SDDS_GetParameter(&SDDS_orig, fileParameter, &nameFromParameter)) {
                SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
                exit(1);
                }
            strcpy(name, nameFromParameter);
            free(nameFromParameter);
            }
        else
            sprintf(name, format, rootname, retval-offset);
        if (!SDDS_InitializeCopy(&SDDS_dataset, &SDDS_orig, name, "w")) {
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
            exit(1);
            }
/*        SDDS_orig.page_number = 1; */
        if ((ascii_output && SDDS_dataset.layout.data_mode.mode!=SDDS_ASCII) ||
            (binary_output && SDDS_dataset.layout.data_mode.mode!=SDDS_BINARY)) { 
          SDDS_dataset.layout.data_mode.mode = ascii_output?SDDS_ASCII:SDDS_BINARY;
        }
        if (!SDDS_WriteLayout(&SDDS_dataset) ||
            !SDDS_CopyPage(&SDDS_dataset, &SDDS_orig) ||
            !SDDS_WritePage(&SDDS_dataset) ||
            !SDDS_Terminate(&SDDS_dataset)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        SDDS_orig.page_number = retval;
        }
    if (retval==0) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    if (!SDDS_Terminate(&SDDS_orig)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    return(0);
}

