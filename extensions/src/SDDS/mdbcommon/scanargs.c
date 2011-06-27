/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: scanargs.c
 * contents: scanargs(), scanargsg(), parseList(), match_string()
 * purpose: argument parsing, I/O redirection, filename "globbing"
 *
 * keyword syntax:
 *     -key[=item1[,item2...]]
 *     -key[:item1[,item2...]]
 *     -key[,item1[,item2...]]
 *     
 *
 * I/O redirection:
 *     >output_file    data writen to stdout goes to file
 *     <input_file     data read from stdin comes from file
 *    >>output_file    data writen to stdout goes to end of file
 *
 * Michael Borland, 1986, 1990, 1994, 1995
 $Log: scanargs.c,v $
 Revision 1.10  2011/01/10 17:35:29  borland
 Fixed problem on RH6 by switching from strcpy to strcpy_ss.

 Revision 1.9  2003/09/02 18:06:10  soliday
 Cleaned up code for Linux.

 Revision 1.8  2003/08/28 22:12:31  soliday
 Cleaned up the code.

 Revision 1.7  2002/08/14 16:48:29  soliday
 Added Open License

 Revision 1.6  2002/04/17 22:16:14  soliday
 Fixed a memory leak in parse_string.

 Revision 1.5  2002/03/22 22:52:01  soliday
 Replaced free_scanargs with free_scanargs2 because it is written better.

 Revision 1.4  2002/03/21 23:10:18  soliday
 Fixed parse_string and added free_scanargs2.

 Revision 1.3  2002/03/07 01:18:17  soliday
 Added parse_string to simulate command line parsing.

 Revision 1.2  2002/01/28 16:50:17  soliday
 Added free_scanargs

 Revision 1.1  2000/04/11 16:24:04  soliday
 Moved here from mdblib.

 Revision 1.18  1999/12/15 15:45:06  soliday
 Added vxWorks support.

 Revision 1.17  1999/08/24 13:41:14  borland
 Resolved apparent array bounds problem with addition of arguments from a file.
 Funny that it never happened til now.

 Revision 1.16  1999/07/08 20:36:42  soliday
 Changed defined(WIN32) to defined(_WIN32)

 Revision 1.15  1999/05/25 21:54:56  soliday
 Removed compiler warnings under Linux.

 Revision 1.14  1999/04/30 13:24:48  borland
 Now longer tries to set up IO redirection for WIN32.

 Revision 1.13  1999/04/09 18:45:18  soliday
 scanargs now frees unused memory before returning.

 Revision 1.12  1998/04/04 00:09:29  borland
 Fixed problem scanning arguments with (.  Was improperly treating ( like
 a quotation mark when it was not preceeded by =.

 Revision 1.11  1997/12/19 17:05:22  borland
 Turned off debugging.

 Revision 1.10  1997/10/21 16:45:24  borland
 Importing of arguments from file extended to include SDDS files.
 Syntax is @@<filename>,{column | parameter}=<name>.  The comma is
 used as the cue that it is an SDDS file.  If it isn't really such,
 it is treated like a normal text file.

 Revision 1.9  1997/10/21 15:44:58  borland
 Added method for reading additional "commandline" arguments from a file.
 Any argument starting with the sequence @@ is interpreted as a filename;
 each line from the file is interpreted as an argument that is equivalent
 to one placed on the commandline.

 Revision 1.8  1997/08/25 19:18:24  borland
 No longer does its own IO redirection for UNIX machines.

 * Revision 1.7  1996/02/15  02:39:35  borland
 * Made warning about overwriting of existing file contingent on state of
 * noWarnings variable (processFilenames).
 *
 * Revision 1.6  1996/01/21  00:16:30  borland
 * Revised unpacking routines and calls to same.  Now support temporary
 * files as well as pipes.
 *
 * Revision 1.5  1996/01/19  00:20:42  borland
 * Added code to processFilenames() to deal with possibility that input might
 * be a packed file.
 *
 * Revision 1.4  1995/11/13  16:18:53  borland
 * Added long cast to strlen() in comparisons to satisfy Solaris compiler.
 *
 * Revision 1.3  1995/10/07  22:44:07  borland
 * Modified processFilenames to give more helpful error messages when too many
 * filenames are present.
 *
 * Revision 1.2  1995/09/05  21:18:21  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "scan.h"
#include "mdb.h"
#include "SDDS.h"

#define DEBUG 0
#define COMMAS_SEPARATE_FILENAMES 0

#ifdef VAX_VMS
/* check for VAX/VMS wildcards */
#define HAS_WILDCARDS(s) (strchr(s, '*')!=NULL || strchr(s, '%')!=NULL)
#endif
#ifdef UNIX
#define HAS_WILDCARDS(s) (strchr(s, '*')!=NULL || strchr(s, '?')!=NULL)
#endif

int parseList(char ***list, char *string);
long add_file_arguments(int argc, char **argv, char ***argvNew);
int parse_string(char ***list, char *string);

int scanargs(SCANNED_ARG **scanned, int argc, char **argv)
{
    SCANNED_ARG *sc_arg_ptr;
    int i, i_store, argLimit;
    char *ptr, *arg;
    char **argvNew;
    void prompt_for_arguments(int *argc, char ***argv);
#if !defined(UNIX) && !defined(_WIN32) && !defined(vxWorks)
    FILE *fp;
#endif
    argvNew = NULL;
    
    if (argc>=2) {
        if (argv[argc-1][0]=='&') {
            /* prompt for arguments from command line--continuation character is & */
            argc--;
            prompt_for_arguments(&argc, &argv);
            }
        }

    if (argc>=1) {
      argc = add_file_arguments(argc, argv, &argvNew);
      argv = argvNew;
    }
    
#if DEBUG
    printf("%ld argv strings:\n", argc);
    for (i=0; i<argc; i++)
        printf("%ld: %s\n", i, argv[i]);
#endif

    sc_arg_ptr = tmalloc((unsigned)argc*sizeof(*sc_arg_ptr));
    *scanned = sc_arg_ptr;
    arg = tmalloc(sizeof(*arg)*(argLimit=1024));

    for (i=i_store=0; i<argc; i++) {
        if ((long)strlen(argv[i])>argLimit-1)
            arg = trealloc(arg, sizeof(*arg)*(argLimit=2*strlen(argv[i])));
        strcpy(arg, argv[i]);
        interpret_escapes(arg);
        if (arg[0]=='-') {
            /* it's an option or switch: "-key[=item1[,item2]...]".  / may be subsituted
             * for -, and : or , may be substituted for = */
            sc_arg_ptr[i_store].arg_type = OPTION;
            ptr = arg;
            while (*ptr && (*ptr!='=' && *ptr!=':' && *ptr!=','))
                ptr++;
            if (*ptr=='=' || *ptr==':' || *ptr==',') {
                /* there's a list of items. separate list into an array
                 * with the key in the first position */
                *ptr=',';
                sc_arg_ptr[i_store].n_items = parseList(&(sc_arg_ptr[i_store].list), arg+1);
                }
            else if (ptr-arg>1) {
                /* no list, just scan key */
                sc_arg_ptr[i_store].n_items = parseList(&(sc_arg_ptr[i_store].list), arg+1);
                }
            else {
                sc_arg_ptr[i_store].n_items = 0;
                sc_arg_ptr[i_store].list = tmalloc(sizeof(*sc_arg_ptr[i_store].list)*1);
                sc_arg_ptr[i_store].list[0] = tmalloc(sizeof(**sc_arg_ptr[i_store].list));
                sc_arg_ptr[i_store].list[0][0] = 0;
                }
            i_store++;
            }
#if !defined(UNIX) && !defined(_WIN32) && !defined(vxWorks)
        /* Set up IO redirection for non-UNIX platforms.
         * Used to work on VMS, still may...
         */
        else if (arg[0]=='>') {
            /* output redirection */
            if (arg[1]=='>') {
                if ((fp=fopen(arg+2, "a+"))==NULL) {
                    printf("unable to open %s for appending\n", arg+2);
                    exit(1);
                    }
                }
            else {
                if ((fp=fopen(arg+1, "w"))==NULL) {
                    printf("unable to open %s for writing\n", arg+1);
                    exit(1);
                    }
                }
            /* for other systems, may need to make a different assignment */
            stdout = fp;
            }
        else if (arg[0]=='<') {
            /* input redirection */
            if ((fp=fopen(arg+1, "r"))==NULL) {
                printf("unable to open %s for reading\n", arg+1);
                exit(1);
                }
            /* for other systems, may need to make a different assignment */
            stdin = fp;
            }
#endif
        else {
            /* not an option or switch */
            sc_arg_ptr[i_store].arg_type = A_LIST;
#if COMMAS_SEPARATE_FILENAMES
            sc_arg_ptr[i_store].n_items = parseList(&(sc_arg_ptr[i_store].list), arg);
#else
            sc_arg_ptr[i_store].n_items = 1;
            sc_arg_ptr[i_store].list = tmalloc(sizeof(*sc_arg_ptr[i_store].list)*1);
            cp_str(&sc_arg_ptr[i_store].list[0], arg);
#endif
            i_store++;
            }
        }

    if (argvNew) {
      for (i=0; i<argc; i++) {
	if (argvNew[i]) free(argvNew[i]);
      }
      free(argvNew);
    }
    if (arg) free(arg);

    return i_store;
    }


int scanargsg(SCANNED_ARG **scanned, int argc, char **argv)
{
    int i, j;
    int n_items;
#ifdef VAX_VMS
    int *origin;
#endif
    char **list;

    /* first get the command-line arguments and parse them into the
       SCANNED_ARG format */
    argc = scanargs(scanned, argc, argv);

    /* go through and find any non-option arguments with lists or wildcards */
    for (i=0; i<argc; i++) {
        if ((*scanned)[i].arg_type!=OPTION) {
            if ((*scanned)[i].n_items!=1) {
                /* turn each element of the list into a separate argument */
                list = (*scanned)[i].list;
                n_items = (*scanned)[i].n_items;
                *scanned = trealloc(*scanned, sizeof(**scanned)*(argc+n_items-1));
                for (j=argc-1; j>i; j--) {
                    (*scanned)[j+n_items-1].list = (*scanned)[j].list;
                    (*scanned)[j+n_items-1].n_items = (*scanned)[j].n_items;
                    (*scanned)[j+n_items-1].arg_type = (*scanned)[j].arg_type;
                    }
                for (j=0; j<n_items; j++) {
                    (*scanned)[i+j].arg_type = A_LIST;
                    (*scanned)[i+j].n_items  = 1;
                    (*scanned)[i+j].list     = tmalloc(sizeof(char**));
                    (*scanned)[i+j].list[0]  = list[j];
                    }
                argc += n_items-1;
                }
#ifdef VAX_VMS
            if (HAS_WILDCARDS((*scanned)[i].list[0])) {
                list = wild_list(&n_items, &origin, (*scanned)[i].list, 1);
                *scanned = trealloc(*scanned, sizeof(**scanned)*(argc+n_items-1));
                for (j=argc-1; j>i; j--) {
                    (*scanned)[j+n_items-1].list = (*scanned)[j].list;
                    (*scanned)[j+n_items-1].n_items = (*scanned)[j].n_items;
                    (*scanned)[j+n_items-1].arg_type = (*scanned)[j].arg_type;
                    }
                for (j=0; j<n_items; j++) {
                    (*scanned)[i+j].arg_type = A_LIST;
                    (*scanned)[i+j].n_items  = 1;
                    (*scanned)[i+j].list     = tmalloc(sizeof(char**));
                    (*scanned)[i+j].list[0]  = list[j];
                    }
                argc += n_items-1;
                i    += n_items-1;
                }
#endif
            }
        }
    return(argc);
    }

#define ITEMS_BUFSIZE 10

int parseList(char ***list, char *string)
{
    static char **items = NULL;
    char *ptr, *ptr1, last_char;
    int i, n_items;
    static int items_max = 0;

    n_items = 0;

    if (*(ptr = string)==0) {
        *list = 0;
        return(0);
        }
    do {
        ptr1 = ptr;
#if DEBUG
        printf("ptr1 = \"%s\"\n", ptr1);
#endif
        if (*ptr1=='(' && (ptr1==string || *(ptr1-1)!='\\')) {
          /* this item starts with a (  so search for the matching ")," or ")\0"
             to find the end of the item 
             */
          ptr = ++ptr1;
          while (*ptr1 && !(*ptr1==')' && *(ptr1-1)!='\\' && (*(ptr1+1)==',' || *(ptr1+1)==0)))
            ptr1++;
          if (*(ptr1+1)==',')
            *ptr1++ = 0;
        }
        else {
          while (*ptr1 && !(*ptr1==',' && (ptr1==ptr || *(ptr1-1)!='\\'))) {
            if (*ptr1=='=' && *(ptr1+1)=='(') {
              /* if this option contains =(, then search for matching ), and remove both */
              ptr1++;
              strslide(ptr1, -1);
              while (*ptr1) {
                if (*ptr1==')' && *(ptr1-1)!='\\' && (*(ptr1+1)==',' || *(ptr1+1)==0))
                  break;
                ptr1++;
              }
              if (*ptr1==')' && *(ptr1-1)!='\\' && (*(ptr1+1)==',' || *(ptr1+1)==0))
                strslide(ptr1, -1);
            } else 
              ptr1++;
          }
        }
        
        last_char = *ptr1;
        *ptr1 = 0;
        if (n_items>=items_max)
            items = trealloc(items, sizeof(*items)*(items_max+=ITEMS_BUFSIZE));
#if DEBUG
        printf("item = \"%s\"\n", ptr);
#endif
        items[n_items++] = ptr;
        if (last_char)
            ptr = ptr1+1;
        } while (*ptr && last_char);
    if (last_char==',') 
        items[n_items++] = ptr;

    *list = tmalloc((unsigned)sizeof(ptr)*n_items);
    for (i=0; i<n_items; i++) {
        ptr = items[i];
        while (*ptr) {
            if (*ptr=='\\' && (*(ptr+1)==',' || *(ptr+1)=='"' || *(ptr+1)=='(' || *(ptr+1)==')'))
                strcpy_ss(ptr, ptr+1);
            ptr++;
            }
        *(*list+i) = tmalloc((unsigned)strlen(items[i])+1);
        strcpy(*(*list+i), items[i]);
        }
    return(n_items);
    }

void prompt_for_arguments(int *argc, char ***argv)
{
    char *ptr, **cmd_line_arg;
    char *ptr1, *ptr2;
    char buffer[1024];
    int maxargs, i;

    /* copy command-line arguments into new argv array */
    cmd_line_arg = *argv;
    *argv = tmalloc(sizeof(**argv)*(maxargs=(*argc>10?*argc:10)));
    for (i=0; i<*argc; i++)
        (*argv)[i] = cmd_line_arg[i];
    tfree(cmd_line_arg);

    do {
        fgets(buffer, 1024, stdin);
        buffer[strlen(buffer)-1] = 0;
        while ((ptr=get_token_tq(buffer, " ", " ", "\"", "\""))) {
            if (*ptr=='&')
                break;
            ptr1 = ptr2 = ptr;
            while (*ptr1) {
                if (*ptr1=='"') {
                    if (*(ptr1+1)=='"') {
                        ptr1++;
                        while (*ptr1=='"')
                            ptr1++;
                        }
                    else {
                        while (*ptr1!='"')
                            *ptr2++ = *ptr1++;
                        ptr1++;
                        }
                    }
                else
                    *ptr2++ = *ptr1++;
                }
            *ptr2 = 0;
            if (*argc==maxargs)
                *argv = trealloc(*argv, sizeof(**argv)*(maxargs+=10));
            (*argv)[(*argc)++] = ptr;
            }
        } while (ptr && *ptr=='&');
    }

long processPipeOption(char **item, long items, unsigned long *flags)
{
    char *keyword[2] = {"input", "output"};
    long i;

    *flags = 0;
    if (items<1) {
        *flags = USE_STDIN+USE_STDOUT;
        return 1;
        }
    for (i=0; i<items; i++) {
        switch (match_string(item[i], keyword, 2, 0)) {
          case 0:
            *flags |= USE_STDIN;
            break;
          case 1:
            *flags |= USE_STDOUT;
            break;
          default:
            return 0;
            }
        }
    return 1;
    }


void processFilenames(char *programName, char **input, char **output, unsigned long pipeFlags,
                      long noWarnings, long *tmpOutputUsed)
{
    char *unpackedName;
    
    if (tmpOutputUsed)
        *tmpOutputUsed = 0;
    if (!(*input)) {
        if (pipeFlags&DEFAULT_STDIN)
            pipeFlags |= USE_STDIN;
        if (!(pipeFlags&USE_STDIN)) {
            fprintf(stderr, "error: too few filenames (%s)\n", programName);
            exit(1);
            }
        }
    if (*input && pipeFlags&USE_STDIN) {
        if (!*output) {
            *output = *input;
            *input = NULL;
            }
        else {
            fprintf(stderr, "error: too many filenames (%s)\n", programName);
            fprintf(stderr, "       offending argument is %s\n", *output);
            exit(1);
            }
        }

    if (*output && pipeFlags&USE_STDOUT) {
        fprintf(stderr, "error: too many filenames (%s)\n", programName);
        fprintf(stderr, "       offending argument is %s\n", *output);
        exit(1);
        }
    if (!*output && pipeFlags&DEFAULT_STDOUT)
        pipeFlags |= USE_STDOUT;
    
    if ((*input && *output && strcmp(*input, *output)==0) || (!*output && !(pipeFlags&USE_STDOUT))) {
        if (!*input) {
            fprintf(stderr, "error: no output filename---give output filename or -pipe=output (%s)\n", programName);
            exit(1);
            }
        if (!tmpOutputUsed) {
            fprintf(stderr, "error: input and output are identical (%s)\n", programName);
            exit(1);
            }
        if (PackSuffixType(*input, &unpackedName, UNPACK_REQUIRE_SDDS)>=0) {
            if (!unpackedName || !strlen(unpackedName)) {
                fprintf(stderr, "error: can't unpack %s---name too short for automatic name generation (%s)\n",
                        *input, programName);
                exit(1);
                }
            else if (fexists(unpackedName)) {
                fprintf(stderr, "error: can't unpack %s and create %s---%s exists (%s)\n",
                        *input, unpackedName, unpackedName, programName);
                exit(1);
                }
            else if (!noWarnings)
                fprintf(stderr, "warning: creating new file %s while leaving compressed file %s intact (%s)\n",
                        unpackedName, *input, programName);
            *tmpOutputUsed = 0;
            cp_str(output, unpackedName);
            }
        else {
            *tmpOutputUsed = 1;
            cp_str(output, tmpname(NULL));
            if (!noWarnings)
                fprintf(stderr, "warning: existing file %s will be replaced (%s)\n", *input, programName);
            }
        }
    }

long add_file_arguments(int argc, char **argv, char ***argvNew)
{
  long iarg, iNew, isSDDS, dataIndex=0, argcMax;
  FILE *fp;
  char buffer[16384], *ptr, *filename, *class, *dataName=NULL;
  int  isParameter=0;
  SDDS_DATASET SDDSinput;
  char *classOption[2] = {
    "column", "parameter" 
    };
  
  *argvNew = tmalloc(sizeof(**argvNew)*argc);
  argcMax = argc;
  iNew = 0;
  for (iarg=0; iarg<argc; iarg++) {
    if (argv[iarg][0]=='@' && argv[iarg][1]=='@') {
      isSDDS = 0;
      if ((ptr=strchr(filename=argv[iarg]+2, ',')) && *(ptr-1)!='\\') {
        *ptr = 0;
        if (!SDDS_InitializeInput(&SDDSinput, filename)) {
          *ptr = ',';
        } else {
          isSDDS = 1;
          class = ptr+1;
          if (!(ptr=strchr(class, '='))) {
            *ptr = ',';
            fprintf(stderr, "Bad argument file option: %s (%s)\n",
                    argv[iarg]+2, argv[0]);
            exit(1);
          }
          dataName = ptr+1;
          switch (match_string(class, classOption, 2, 0)) {
          case 0:
            /* column */
            isParameter = 0;
            break;
          case 1:
            /* parameter */
            isParameter = 1;
            break;
          default:
            fprintf(stderr, "Bad argument file option: %s (%s)\n",
                    argv[iarg]+2, argv[0]);
            exit(1);
            break;
          }
          if ((isParameter && (dataIndex=SDDS_GetParameterIndex(&SDDSinput, dataName))<0) ||
              (!isParameter && (dataIndex=SDDS_GetColumnIndex(&SDDSinput, dataName))<0)) {
            fprintf(stderr, "Error: %s %s not found in file %s (%s)\n",
                    isParameter?"parameter":"column", dataName, filename, argv[0]);
            exit(1);
          }
          if ((isParameter && SDDS_GetParameterType(&SDDSinput, dataIndex)!=SDDS_STRING) || 
              (!isParameter && SDDS_GetColumnType(&SDDSinput, dataIndex)!=SDDS_STRING)) {
            fprintf(stderr, "Error: %s %s in file %s is not string type (%s)\n",
                    isParameter?"parameter":"column", dataName, filename, argv[0]);
            exit(1);
          }
        }
      }
      if (!fexists(filename)) {
            fprintf(stderr, "error: argument file not found: %s (%s)\n",
                    filename, argv[0]);
            exit(1);
          }
      if (!isSDDS) {
        if (!(fp=fopen(filename, "r"))) {
          fprintf(stderr, "couldn't read argument file: %s\n", filename);
          exit(1);
        }
        while (fgets(buffer, 16384, fp)) {
          buffer[strlen(buffer)-1] = 0;
          if (!strlen(buffer))
            continue;
          if (iNew>=argcMax) {
            *argvNew = trealloc(*argvNew, sizeof(**argvNew)*(argcMax=iNew+10));
          }
          delete_chars(buffer, "\"");
          SDDS_CopyString((*argvNew)+iNew, buffer);
          iNew++;
        }
        fclose(fp);
      } else {
        if (!isParameter) {
          while (SDDS_ReadPage(&SDDSinput)>0) {
            char **column;
            long iRow, rows;
            if ((rows=SDDS_CountRowsOfInterest(&SDDSinput))<=0)
              continue;
            if (!(column=SDDS_GetColumn(&SDDSinput, dataName)))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            if ((iNew+rows)>=argcMax)
              *argvNew = trealloc(*argvNew, sizeof(**argvNew)*(argcMax=iNew+rows+1));
            for (iRow=0; iRow<rows; iRow++) {
              delete_chars(column[iRow], "\"");
              (*argvNew)[iNew] = column[iRow];
              iNew++;
            }
            free(column);
          }
        } else {
          while (SDDS_ReadPage(&SDDSinput)>0) {
            char *parameter;
            if (!SDDS_GetParameter(&SDDSinput, dataName, &parameter))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            if (iNew>=argcMax)
              *argvNew = trealloc(*argvNew, sizeof(**argvNew)*(argcMax=argcMax+10));
            (*argvNew)[iNew] = parameter;
            iNew++;
          }
        }
        SDDS_Terminate(&SDDSinput);
      }
    } else {
      if (iNew>=argcMax) 
        *argvNew = trealloc(*argvNew, sizeof(**argvNew)*(argcMax=argcMax+10));
      SDDS_CopyString((*argvNew)+iNew, argv[iarg]);
      iNew++;
    }
  }
  return iNew;
}
/*
void free_scanargs(SCANNED_ARG *scanned, int argc)
{
  int i, i_store;
  if (scanned) {
    for (i_store=0; i_store<argc; i_store++) {
      if (scanned[i_store].list) {
	for (i=0; i<scanned[i_store].n_items; i++) {
	  if (scanned[i_store].list[i]) {
	    free(scanned[i_store].list[i]);
	    scanned[i_store].list[i] = NULL;
	  }
	}
	free(scanned[i_store].list);
	scanned[i_store].list = NULL;
      }
    }
    free(scanned);
    scanned = NULL;
  }
}
*/
void free_scanargs(SCANNED_ARG **scanned, int argc)
{
  int i, i_store;
  if (*scanned) {
    for (i_store=0; i_store<argc; i_store++) {
      if ((*scanned)[i_store].list) {
	for (i=0; i<(*scanned)[i_store].n_items; i++) {
	  if ((*scanned)[i_store].list[i]) {
	    free((*scanned)[i_store].list[i]);
	    (*scanned)[i_store].list[i] = NULL;
	  }
	}
	free((*scanned)[i_store].list);
	(*scanned)[i_store].list = NULL;
      }
    }
    free(*scanned);
    *scanned = NULL;
  }
}

/* Simulates command line argument parseing */
int parse_string(char ***list, char *string)
{
  char *ptr=NULL, *ptr1, *ptr2, *buffer;
  int n_items=0;
  
  cp_str(&buffer, string);
  *list = NULL;
  do {
    if (ptr) free(ptr);
    ptr = NULL;
    while ((ptr=get_token_tq(buffer, " ", " ", "\"", "\""))) {
      if (*ptr=='&')
	break;
      ptr1 = ptr2 = ptr;
      while (*ptr1) {
	if (*ptr1=='"') {
	  if (*(ptr1+1)=='"') {
	    ptr1++;
	    while (*ptr1=='"')
	      ptr1++;
	  }
	  else {
	    while (*ptr1!='"')
	      *ptr2++ = *ptr1++;
	    ptr1++;
	  }
	}
	else
	  *ptr2++ = *ptr1++;
      }
      *ptr2 = 0;
      *list = trealloc(*list, sizeof(**list)*(n_items + 1));
      cp_str(&(*list)[n_items], ptr);
      /*   (*list)[n_items] = ptr;*/
      n_items++;
      if (ptr) free(ptr);
      ptr = NULL;
    }
  } while (ptr && *ptr=='&');
  if (ptr) free(ptr);
  ptr = NULL;
  if (buffer) free(buffer);
  return(n_items);
}
