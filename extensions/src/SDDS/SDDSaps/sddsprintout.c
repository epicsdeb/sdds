/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsprintout
 * purpose: make a printout from SDDS data
 *
 * Michael Borland, 1995
 $Log: sddsprintout.c,v $
 Revision 1.47  2009/08/05 15:35:23  soliday
 Updated to replace ld and lu in the format strings with PRId32 and PRIu32

 Revision 1.46  2009/08/04 19:46:09  soliday
 Updated to convert %ld format strings to %d for 64 bit systems.

 Revision 1.45  2008/08/06 20:46:49  soliday
 Fixed the format strings.

 Revision 1.44  2008/06/06 18:10:32  soliday
 Fixed a problem with one of the elements has the maximum number of characters
 that SDDS will allow.

 Revision 1.43  2006/10/19 17:55:40  soliday
 Updated to work with linux-x86_64.

 Revision 1.42  2006/08/31 15:08:55  soliday
 Updated to work with SDDS2.

 Revision 1.41  2005/11/04 22:46:17  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.40  2004/04/21 20:07:52  shang
 the program will exit if checking datatype with format fails

 Revision 1.39  2004/01/08 03:33:46  borland
 Added escaping of latex output strings so that special characters don't
 cause latex processing problems.

 Revision 1.38  2004/01/06 20:11:25  borland
 Added cpation to latex table output.

 Revision 1.37  2003/12/03 00:11:17  borland
 Added -latexFormat option for output of a LaTeX table.

 Revision 1.36  2002/12/11 21:39:15  borland
 Provided a slight speedup in processing large files when no columns
 are printed by using SDDS_ReadPageSparse().

 Revision 1.35  2002/08/14 17:12:51  soliday
 Added Open License

 Revision 1.34  2002/07/25 00:12:40  borland
 Now check return values from SDDS_SprintTypedValue().

 Revision 1.33  2001/09/05 20:16:34  shang
 added cvs feature to -spreadsheet option, which has the same function as
 -spreadsheet=quote=\",delimiter=\\, -notitle. And modified the default format
 output to be the actual format when csv option is given.

 Revision 1.32  2001/01/10 19:35:43  soliday
 Standardized usage message.

 Revision 1.31  2000/04/13 19:51:20  soliday
 Added missing include statement.

 Revision 1.30  1999/08/11 17:50:29  borland
 Added -bufferlines option for putting blank lines between pages.

 Revision 1.29  1999/05/25 19:13:30  soliday
 Removed compiler warning on linux.

 Revision 1.28  1999/01/06 19:54:51  borland
 Fixed the version number in the usage message.

 Revision 1.27  1998/08/24 14:42:10  soliday
 Fixed bug when fromPage and toPage options are both used

 Revision 1.26  1998/08/21 20:58:52  soliday
 Added a noLabels option that works with columns and parameters

 Revision 1.25  1996/11/15 00:30:30  borland
 Added option to generate SCH file for spreadsheet output.

 Revision 1.24  1996/11/15 00:15:17  borland
 Added quote-mark and label control for spreadsheet mode.

 Revision 1.23  1996/11/12 20:45:16  borland
 Fixed bug with reuse of buffer used for edited column label.

 Revision 1.22  1996/11/11 20:11:02  borland
 Removed statements that added spaces after each field, as they aren't needed
 with new format string padding.  Added sensitivity to spreadsheet mode for
 format string modification routine.

 Revision 1.21  1996/11/11 18:43:26  borland
 Some debugging statements added.

 Revision 1.20  1996/11/07 22:18:22  borland
 Better handling of field width adjustments for long labels.  Now modifies
 user or default format string to fill available space, rather than trying
 to do symmetric space padding.

 * Revision 1.19  1996/07/05  16:09:49  borland
 * Removed routine cleanUpQuotes and started using SDDS_PRINT_NOQUOTES
 * mode to SDDS_SprintTypedValue.  Also, detects blank string data
 * and substitutes " " to get proper filling of field.
 *
 * Revision 1.18  1996/05/13  20:33:58  borland
 * Added -postPageLines option to allow putting blank lines after a page.
 *
 * Revision 1.17  1996/02/29  18:20:00  borland
 * Added -noWarnings option, and implemented in all relevant subroutines.
 *
 * Revision 1.16  1996/02/21  16:08:38  borland
 * For spreadsheet output, now set width=0 and use this to flag no line breaks
 * for a row.  User may reset width using -width option to get old behavior,
 * though I doubt anyone will.
 *
 * Revision 1.15  1996/02/14  01:05:33  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.14  1996/02/07  18:13:25  borland
 * checkPagination now initializes currentLine to 0 rather than 1, which gets
 * the right number of lines per page.
 *
 * Revision 1.13  1996/02/07  17:43:19  borland
 * Added initialization for currentLine field of PAGINATION structure.
 *
 * Revision 1.12  1996/02/05  23:57:06  borland
 * Added -pagination option.
 *
 * Revision 1.11  1996/01/29  22:28:42  borland
 * Added editLabel qualifier to -column and -parameter options.
 *
 * Revision 1.10  1996/01/12  19:03:13  borland
 * Added -spreadsheet option.
 *
 * Revision 1.9  1995/09/25  16:37:13  borland
 * Modified header code for column output to eliminate units line if all units
 * are blank.
 *
 * Revision 1.8  1995/09/18  17:26:01  borland
 * Added label qualifers to -parameter and -column options.
 *
 * Revision 1.7  1995/09/15  21:31:40  borland
 * Added -title option, and changed -noHeader option to -noTitle.
 *
 * Revision 1.6  1995/09/15  15:24:00  borland
 * Added useDefaultFormat qualifer to -column and -parameter options.  Added
 * code to ensure use of any format_string defined in the SDDS file before
 * any other string.
 *
 * Revision 1.5  1995/09/14  22:01:05  borland
 * Added blankLines qualifier to -parameter and -column options.
 *
 * Revision 1.4  1995/09/14  04:28:00  borland
 * Added -noHeader option to allow elimination of header on first page of
 * printout.
 *
 * Revision 1.3  1995/09/06  14:56:57  saunders
 * First test release of SDDS1.5
 *
 */
#include <ctype.h>
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
/*#include "SDDSaps.h"*/

#define SET_COLUMNS 0
#define SET_PARAMETERS 1
#define SET_ARRAYS 2
#define SET_FROMPAGE 3
#define SET_TOPAGE 4
#define SET_FORMATDEFAULTS 5
#define SET_WIDTH 6
#define SET_PIPE 7
#define SET_PAGEADVANCE 8
#define SET_NOTITLE 9
#define SET_TITLE 10
#define SET_SPREADSHEET 11
#define SET_PAGINATE 12
#define SET_NOWARNINGS 13
#define SET_POSTPAGELINES 14
#define SET_NOLABELS 15
#define SET_BUFFERLINES 16
#define SET_LATEXFORMAT 17
#define N_OPTIONS 18

char *option[N_OPTIONS] = {
    "columns", "parameters", "arrays", "frompage", "topage", 
    "formatdefaults", "width", "pipe", "pageadvance", "notitle", "title",
    "spreadsheet", "paginate", "nowarnings", "postpagelines", "nolabels",
    "bufferlines", "latexformat"
    } ;

char *USAGE = "sddsprintout [<SDDSinput>] [<outputfile>] [-pipe=[input][,output]] \n\
[-columns[=<name-list>[,format=<string>][,label=<string>][,editLabel=<command>][,useDefaultFormat][,endsline][,blankLines=<number>]]] \n\
[-parameters[=<name-list>[,format=<string>][,label=<string>][,editLabel=<command>][,useDefaultFormat][,endsline][,blankLines=<number]]] \n\
[-array[=<name-list>[,format=<string>]] \n\
[-fromPage=<number>] [-toPage=<number>] \n\
[-formatDefaults=<SDDStype>=<format-string>[,...]]\n\
[-width=<integer>] [-pageAdvance] [-paginate[=lines=<number>][,notitle][,nolabels]]\n\
[-noTitle] [-title=<string>] [-noLabels] [-postPageLines=<number>]\n\
[-spreadsheet[=delimiter=<string>][,quotemark=<string>][,nolabels][,csv][,schfile=<filename>]]\n\
[-latexFormat] [-noWarnings]\n\
-spreadsheet=csv is the simple way of -spreadsheet=nolabels,quote=\",delimiter=\\, -notitle \n\n\
Program by Michael Borland. (This is version 7, August 1998.)\n";

#define PAGINATION_ON       0x0001U
#define PAGINATION_NOTITLE  0x0002U
#define PAGINATION_NOLABELS 0x0004U

typedef struct {
    int32_t lines;
    long currentLine;
    unsigned long flags;
    } PAGINATION;
 
#define ENDSLINE         0x0001U
#define USEDEFAULTFORMAT 0x0002U
#define LABEL_GIVEN      0x0004U
#define EDITLABEL_GIVEN  0x0008U
 
typedef struct {
    char *name, *format, **header, *label, *editLabel;
    long fieldWidth, index, headers, type, endsLine, blankLines, useDefaultFormat;
    void *data;
    } PRINT_COLUMN;

typedef struct {
    char *name, *format, *label, *editLabel;
    long fieldWidth, index, type, endsLine, blankLines, useDefaultFormat;
    void *data;
    } PRINT_PARAMETER;

typedef struct {
    char *name, *format, *label;
    long index;
    SDDS_ARRAY *array;
    } PRINT_ARRAY;

char *makeTexSafeString(char *source);
void copyAndPad(char *target, char *source, long sourceWidth, long targetWidth);
long makeColumnHeaders(char ***header, long *fieldWidth, char *name, char *editLabel, char *units, char **format,
unsigned long spreadsheetFlags);
char *makeParameterLabel(long *fieldWidth, char *name, char *editLabel, char *units, char *format);
long changeDefaultFormats(char **argv, long argc, long noWarnings);
void setDefaultFormats(void);
long processPrintColumns(PRINT_COLUMN **printRequestPtr, long printRequests, SDDS_DATASET *inTable, long noWarnings,
unsigned long spreadsheetFlags,long csv);
void printColumnHeaders(FILE *fpOut, PRINT_COLUMN *printColumn, long printColumns,
                        long width, PAGINATION *pagination, long latexFormat, char *latexTitle);
long processPrintParameters(PRINT_PARAMETER **printRequestPtr, long printRequests, SDDS_DATASET *inTable, long noWarnings, long noLabels,long csv);
long processPrintArrays(PRINT_ARRAY **printRequestPtr, long printRequests, SDDS_DATASET *inTable);
long getFormatFieldLength(char *format, long *extraChars);
char **makeListOfNames(char *string, long *names);
void doPrintParameters(SDDS_DATASET *inTable, PRINT_PARAMETER *printParameter, long printParameters, long width, 
FILE *fpOut, unsigned long spreadsheetFlags, char *spreadsheetDelimiter, char *spreadsheetQuoteMark,
PAGINATION *pagination, char *title, long noLabels);
long characterCount(char *string, char c);
long printPageTitle(FILE *fpOut, char *title);
void doPrintColumns(SDDS_DATASET *inTable, PRINT_COLUMN *printColumn, long printColumns, 
long width, FILE *fpOut, unsigned long spreadsheetFlags, char *spreadsheetDelimiter,
char *spreadsheetQuoteMark, long latexFormat, char *latexTitle,
PAGINATION *pagination, char *title, long noLabels);
long checkPagination(FILE *fpOut, PAGINATION *pagination, char *title);
void replaceFormatWidth(char *buffer, char *format, long width);
void CreateSCHFile(char *output, char *input, unsigned long flags, char *delimiter,
                   char *quote, PRINT_COLUMN *printColumn, long printColumns);

static char **defaultFormat = NULL;
static char **csvFormat=NULL;

#define SPREADSHEET_ON        0x0001U
#define SPREADSHEET_DELIMITER 0x0002U
#define SPREADSHEET_QUOTEMARK 0x0004U
#define SPREADSHEET_NOLABELS  0x0008U
#define SPREADSHEET_CSV 0x0010UL

int main(int argc, char **argv)
{
    SDDS_DATASET SDDS_dataset;
    long i, i_arg, pageNumber, width;
    SCANNED_ARG *s_arg;
    char *input, *output, **name, *format, *title, *label, *editLabel, *schFile;
    PRINT_COLUMN *printColumn;
    PRINT_PARAMETER *printParameter;
    PRINT_ARRAY *printArray;
    long printColumns, printParameters, printArrays, firstPage, bufferLines;
    long fromPage, toPage, names, pageAdvance, noTitle, noLabels, noWarnings;
    int32_t blankLines;
    long latexFormat;
    FILE *fpOut;
    unsigned long flags, pipeFlags, spreadsheetFlags, dummyFlags, postPageLines,CSV=0;
    char *spreadsheetDelimiter, *spreadsheetQuoteMark;
    PAGINATION pagination;
    char **formatDefaultArg;
    long formatDefaultArgs;
    static char formatbuffer[100], formatbuffer2[100];
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<3) 
        bomb(NULL, USAGE);
    
    input = output = NULL;
    fromPage = toPage = 0;
    printColumn = NULL;
    printParameter = NULL;
    printArray = NULL;
    width = 130;
    setDefaultFormats();
    printColumns = printParameters = printArrays = pipeFlags=flags=0;
    postPageLines = pageAdvance = noTitle = noLabels = 0;
    title = NULL;
    spreadsheetFlags = 0;
    pagination.flags = pagination.currentLine = 0;
    noWarnings = 0;
    formatDefaultArgs = 0;
    schFile = NULL;
    formatDefaultArg = NULL;
    bufferLines = 0;
    latexFormat = 0;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
              case SET_COLUMNS:
                if (s_arg[i_arg].n_items<2) {
                    s_arg[i_arg].n_items = 2;
                    s_arg[i_arg].list = SDDS_Realloc(s_arg[i_arg].list, sizeof(*s_arg[i_arg].list)*2);
                    SDDS_CopyString(&s_arg[i_arg].list[1], "*");
                    }
                name = makeListOfNames(s_arg[i_arg].list[1], &names);
                printColumn = SDDS_Realloc(printColumn, sizeof(*printColumn)*(names+printColumns));
                s_arg[i_arg].n_items -= 2;
                format = NULL;
                blankLines = 0;
                if (!scanItemList(&flags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
                                    "format", SDDS_STRING, &format, 1, 0,
                                    "endsline", -1, NULL, 0, ENDSLINE, 
                                    "usedefaultformat", -1, NULL, 0, USEDEFAULTFORMAT,
                                    "blanklines", SDDS_LONG, &blankLines, 1, 0,
                                    "editlabel", SDDS_STRING, &editLabel, 1, EDITLABEL_GIVEN,
                                    "label", SDDS_STRING, &label, 1, LABEL_GIVEN,
                                    NULL))
                    SDDS_Bomb("invalid -columns syntax");
                for (i=0; i<names; i++) {
                    SDDS_ZeroMemory(printColumn+i+printColumns, sizeof(*printColumn));
                    printColumn[i+printColumns].name = name[i];
                    if (format) {
                      replaceString(formatbuffer, format, "ld", PRId32, -1, 0);
                      replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
                      SDDS_CopyString(&printColumn[i+printColumns].format, formatbuffer2);
                    }
                    if (flags&ENDSLINE && (i==names-1))
                        printColumn[i+printColumns].endsLine = 1;
                    if (flags&USEDEFAULTFORMAT)
                        printColumn[i+printColumns].useDefaultFormat = 1;
                    printColumn[i+printColumns].label = 
                        printColumn[i+printColumns].editLabel = NULL;
                    if (flags&LABEL_GIVEN)
                        printColumn[i+printColumns].label = label;
                    if (flags&EDITLABEL_GIVEN)
                        printColumn[i+printColumns].editLabel = editLabel;
                    printColumn[i+printColumns].blankLines = blankLines;
                    }
                free(name);
                printColumns += names;
                break;
              case SET_PARAMETERS:
                if (s_arg[i_arg].n_items<2) {
                    s_arg[i_arg].n_items = 2;
                    s_arg[i_arg].list = SDDS_Realloc(s_arg[i_arg].list, sizeof(*s_arg[i_arg].list)*2);
                    SDDS_CopyString(&s_arg[i_arg].list[1], "*");
                    }
                name = makeListOfNames(s_arg[i_arg].list[1], &names);
                printParameter = SDDS_Realloc(printParameter, sizeof(*printParameter)*(names+printParameters));
                s_arg[i_arg].n_items -= 2;
                format = NULL;
                blankLines = 0;
                if (!scanItemList(&flags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
                                    "format", SDDS_STRING, &format, 1, 0,
                                    "endsline", -1, NULL, 0, ENDSLINE, 
                                    "usedefaultformat", -1, NULL, 0, USEDEFAULTFORMAT,
                                    "blanklines", SDDS_LONG, &blankLines, 1, 0,
                                    "editlabel", SDDS_STRING, &editLabel, 1, EDITLABEL_GIVEN,
                                    "label", SDDS_STRING, &label, 1, LABEL_GIVEN,
                                    NULL))
                    SDDS_Bomb("invalid -parameters syntax");
                for (i=0; i<names; i++) {
                    SDDS_ZeroMemory(printParameter+i+printParameters, sizeof(*printParameter));
                    printParameter[i+printParameters].name = name[i];
                    if (format) {
                      replaceString(formatbuffer, format, "ld", PRId32, -1, 0);
                      replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
                      SDDS_CopyString(&printParameter[i+printParameters].format, formatbuffer2);
                    }
                    if (flags&ENDSLINE && (i==names-1))
                        printParameter[i+printParameters].endsLine = 1;
                    if (flags&USEDEFAULTFORMAT)
                        printParameter[i+printParameters].useDefaultFormat = 1;
                    printParameter[i+printParameters].label = 
                        printParameter[i+printParameters].editLabel = NULL;
                    if (flags&LABEL_GIVEN)
                        printParameter[i+printParameters].label = label;
                    if (flags&EDITLABEL_GIVEN)
                        printParameter[i+printParameters].editLabel = editLabel;
                    printParameter[i+printParameters].blankLines = blankLines;
                    }
                free(name);
                printParameters += names;
                break;
              case SET_ARRAYS:
                if (s_arg[i_arg].n_items<2) {
                    s_arg[i_arg].n_items = 2;
                    s_arg[i_arg].list = SDDS_Realloc(s_arg[i_arg].list, sizeof(*s_arg[i_arg].list)*2);
                    SDDS_CopyString(&s_arg[i_arg].list[1], "*");
                    }
                name = makeListOfNames(s_arg[i_arg].list[1], &names);
                printArray = SDDS_Realloc(printArray, sizeof(*printArray)*(names+printArrays));
                s_arg[i_arg].n_items -= 2;
                format = NULL;
                if (!scanItemList(&flags, s_arg[i_arg].list+2, &s_arg[i_arg].n_items,  0,
                                    "format", SDDS_STRING, &format, 1, 0,
                                    NULL))
                    SDDS_Bomb("invalid -arrays syntax");
                for (i=0; i<names; i++) {
                    printArray[i+printArrays].name = name[i];
                    if (format) {
                      replaceString(formatbuffer, format, "ld", PRId32, -1, 0);
                      replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
                      SDDS_CopyString(&printArray[i+printArrays].format, formatbuffer2);
                    }
                }
                free(name);
                printArrays += names;
                break;
              case SET_FROMPAGE:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -fromPage syntax");
                if (fromPage!=0)
                    SDDS_Bomb("invalid syntax: specify -fromPage once only");
                if (sscanf(s_arg[i_arg].list[1], "%ld", &fromPage)!=1 || fromPage<=0)
                    SDDS_Bomb("invalid -fromPage syntax or value");
                break;
              case SET_TOPAGE:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -toPage syntax");
                if (toPage!=0)
                    SDDS_Bomb("invalid syntax: specify -toPage once only");
                if (sscanf(s_arg[i_arg].list[1], "%ld", &toPage)!=1 || toPage<=0)
                    SDDS_Bomb("invalid -toPage syntax or value");
                break;
              case SET_FORMATDEFAULTS:
                formatDefaultArg = s_arg[i_arg].list+1;
                formatDefaultArgs = s_arg[i_arg].n_items-1;
                break;
              case SET_WIDTH:
                if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &width)!=1 || (width<40 && width))
                    SDDS_Bomb("invalid -width syntax or value");
                break;
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case SET_PAGEADVANCE:
                pageAdvance = 1;
                break;
              case SET_NOTITLE:
                noTitle = 1;
                break;
              case SET_TITLE:
                if (s_arg[i_arg].n_items!=2)
                    SDDS_Bomb("invalid -title syntax");
                title = s_arg[i_arg].list[1];
                break;
              case SET_SPREADSHEET:
                s_arg[i_arg].n_items--;
                spreadsheetDelimiter = NULL;
                spreadsheetQuoteMark = "";
                if (!scanItemList(&spreadsheetFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                                  "delimiter", SDDS_STRING, &spreadsheetDelimiter, 1, SPREADSHEET_DELIMITER,
                                  "quotemark", SDDS_STRING, &spreadsheetQuoteMark, 1, SPREADSHEET_QUOTEMARK,
                                  "nolabels", -1, NULL, 0, SPREADSHEET_NOLABELS,
                                  "csv",-1,NULL,0,SPREADSHEET_CSV,
                                  "schfile", SDDS_STRING, &schFile, 1, 0,
                                  NULL))
                    SDDS_Bomb("invalid -spreadsheet syntax");
                if (!spreadsheetDelimiter || !(spreadsheetFlags&SPREADSHEET_DELIMITER))
                  spreadsheetDelimiter = "\t";
                if (spreadsheetFlags&SPREADSHEET_CSV) {
                  spreadsheetDelimiter = ",";
                  spreadsheetFlags |=SPREADSHEET_DELIMITER;
                  spreadsheetFlags |=SPREADSHEET_QUOTEMARK;
                  spreadsheetQuoteMark="\"";
                  noTitle=1;
                  CSV=1;
                }
                spreadsheetFlags |= SPREADSHEET_ON;
                width = 0;
                break;
              case SET_PAGINATE:
                s_arg[i_arg].n_items--;
                pagination.lines = 66;
                if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                                    "lines", SDDS_LONG, &pagination.lines, 1, 0,
                                    "notitle", -1, NULL, 0, PAGINATION_NOTITLE,
                                    "nolabels", -1, NULL, 0, PAGINATION_NOLABELS,
                                    NULL) ||
                    pagination.lines<=3)
                    SDDS_Bomb("invalid -paginate syntax/values");
                pagination.flags |= PAGINATION_ON;
                break;
              case SET_NOWARNINGS:
                noWarnings = 1;
                break;
              case SET_POSTPAGELINES:
                if (s_arg[i_arg].n_items!=2 ||
                    sscanf(s_arg[i_arg].list[1], "%ld", &postPageLines)!=1)
                  SDDS_Bomb("invalid -postPageLines syntax/values");
                break;
              case SET_NOLABELS:
                noLabels = 1;
                break;
              case SET_BUFFERLINES:
                if (s_arg[i_arg].n_items<2)
                    SDDS_Bomb("invalid -bufferLines syntax");
                if (sscanf(s_arg[i_arg].list[1], "%ld", &bufferLines)!=1 || bufferLines<0)
                    SDDS_Bomb("invalid -bufferLines syntax or value");
                break;
              case SET_LATEXFORMAT:
                latexFormat = 1;
                break;
              default:
                fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
                SDDS_Bomb(NULL);
                break;
                }
            }
        else {
            if (!input)
                input = s_arg[i_arg].list[0];
            else if (!output)
                output = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }

    pipeFlags |= DEFAULT_STDOUT;
    processFilenames("sddsprintout", &input, &output, pipeFlags, noWarnings, NULL);
    
    if (formatDefaultArgs)
      changeDefaultFormats(formatDefaultArg, formatDefaultArgs, noWarnings);

    if (pageAdvance) {
      if (postPageLines) 
        SDDS_Bomb("-pageAdvance and -postPageLines are incompatible");
      if (pagination.flags&PAGINATION_ON)
        SDDS_Bomb("-pageAdvance and -paginate are incompatible");
    }
    if (pagination.flags&PAGINATION_ON && postPageLines)
      SDDS_Bomb("-postPageLines and -paginate are incompatible");
    
    if (!printColumns && !printParameters && !printArrays)
        SDDS_Bomb("you must specify at least one of -columns, -parameters, or -arrays");
    if (fromPage && toPage && fromPage>toPage)
        SDDS_Bomb("invalid -fromPage and -toPage");

    if (!SDDS_InitializeInput(&SDDS_dataset, input)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
        }
    
    fpOut = stdout;
    if (output && !(fpOut=fopen(output, "w")))
        SDDS_Bomb("unable to open output file");

    printColumns = processPrintColumns(&printColumn, printColumns, &SDDS_dataset, noWarnings, spreadsheetFlags,CSV);
    printParameters = processPrintParameters(&printParameter, printParameters, &SDDS_dataset, noWarnings, noLabels,CSV);
/*    printArrays = processPrintArrays(&printArray, printArrays, &SDDS_dataset); */
 
    SDDS_SetTerminateMode(TERMINATE_DONT_FREE_TABLE_STRINGS+
                          TERMINATE_DONT_FREE_ARRAY_STRINGS);

    firstPage = 1;
    pagination.flags |= noTitle?PAGINATION_NOTITLE:0;
    if (!title) {
      title = tmalloc(sizeof(*title)*( (input?strlen(input):10)+100));
      sprintf(title, "Printout for SDDS file %s\n", input?input:"stdin");
    }

    if (schFile) {
      if (printArrays || printParameters || !printColumns)
        SDDS_Bomb("Can't create schFile except for pure column printout.");
      CreateSCHFile(schFile, input, spreadsheetFlags, spreadsheetDelimiter, 
                    spreadsheetQuoteMark, printColumn, printColumns);
    }
    if (latexFormat)
      noTitle = 1;
    
    while ((pageNumber=SDDS_ReadPageSparse(&SDDS_dataset, 0, printColumns?1:1000000, 0))>0) {
        if ((fromPage && pageNumber<fromPage) || (toPage && pageNumber>toPage))
            continue;
        if (pagination.flags&PAGINATION_ON) {
            if (!firstPage) {
                fputc('\014', fpOut);
                pagination.currentLine = 1;
                }
            if (!noTitle)
                pagination.currentLine += printPageTitle(fpOut, title);
            }
        else {
            if (firstPage) {
                if (!noTitle) 
                    printPageTitle(fpOut, title);
                }
            else if (pageAdvance)
                fputc('\014', fpOut);
            else if (postPageLines>0) {
              long line=postPageLines;
              while (line--)
                fputc('\n', fpOut);
            }
          }
        doPrintParameters(&SDDS_dataset, printParameter, printParameters, width, fpOut,
                          spreadsheetFlags, spreadsheetDelimiter, spreadsheetQuoteMark, &pagination, title, noLabels);
/*        doPrintArrays(&SDDS_dataset, printArray, printArrays, width, fpOut, &pagination); */
        doPrintColumns(&SDDS_dataset, printColumn, printColumns, width, fpOut,
                       spreadsheetFlags, spreadsheetDelimiter, spreadsheetQuoteMark, 
                       latexFormat, title, &pagination, title, noLabels); 
        if (bufferLines) {
          for (i=0; i<bufferLines; i++)
            fputc('\n', fpOut);
        }
        firstPage = 0;
        }

    if (!SDDS_Terminate(&SDDS_dataset))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0;i<printColumns;i++) {
      if (printColumn[i].headers) 
        free(printColumn[i].header);
    }
    
    free(printColumn);
    
    return 0;
    }

long processPrintColumns(PRINT_COLUMN **printRequestPtr, long printRequests, SDDS_DATASET *inTable, long noWarnings,
                         unsigned long spreadsheetFlags,long csv)
{
    PRINT_COLUMN *printColumn, *printRequest;
    long i, irequest, iname, printColumns, names, *columnUsed, columnLimit, index;
    char **name, *units, *format;
    
    if (printRequests<1 || !(printRequest= *printRequestPtr))
        return 0;
    if ((columnLimit=SDDS_ColumnCount(inTable))<0) {
        if (!noWarnings)
            fprintf(stderr, "warning: no column data in input file\n");
        return 0;
        }
    columnUsed = tmalloc(sizeof(*columnUsed)*columnLimit);
    for (i=0; i<columnLimit; i++)
        columnUsed[i] = 0;
    printColumn = tmalloc(sizeof(*printColumn)*columnLimit);
    printColumns = 0;
    name = NULL;
    
    for (irequest=0; irequest<printRequests; irequest++) {
      if ((names=SDDS_MatchColumns(inTable, &name, SDDS_MATCH_STRING, FIND_ANY_TYPE, 
                                   printRequest[irequest].name, SDDS_0_PREVIOUS|SDDS_OR))>0) {
        for (iname=0; iname<names; iname++) {
          if ((index=SDDS_GetColumnIndex(inTable, name[iname]))<0)
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (!columnUsed[index]) {
            if (SDDS_GetColumnInformation(inTable, "units", &units, SDDS_GET_BY_INDEX, index)!=SDDS_STRING)
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            SDDS_CopyString(&printColumn[printColumns].name, name[iname]);
            printColumn[printColumns].index = index;
            if (iname==names-1)
              printColumn[printColumns].endsLine = printRequest[irequest].endsLine;
            else
              printColumn[printColumns].endsLine = 0;
            printColumn[printColumns].type = SDDS_GetColumnType(inTable, index);
            printColumn[printColumns].blankLines = printRequest[irequest].blankLines;
            if (SDDS_GetColumnInformation(inTable, "format_string", &format, SDDS_GET_BY_INDEX, index)!=SDDS_STRING)
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
            if (csv) {
              SDDS_CopyString(&printColumn[printColumns].format,
                              csvFormat[printColumn[printColumns].type-1]);
            } else {
              if (printRequest[irequest].format) {
                if (!SDDS_VerifyPrintfFormat(printRequest[irequest].format,  printColumn[printColumns].type)) {
                  fprintf(stderr, "error: given format (\"%s\") for column %s is invalid\n",
                          printRequest[irequest].format, name[iname]);
                  exit(1);
                }
                SDDS_CopyString(&printColumn[printColumns].format, printRequest[irequest].format);
              }
              else if (SDDS_StringIsBlank(format) || printRequest[irequest].useDefaultFormat)
                SDDS_CopyString(&printColumn[printColumns].format,
                                defaultFormat[printColumn[printColumns].type-1]);
              else
                SDDS_CopyString(&printColumn[printColumns].format, format);
            }
            
            printColumn[printColumns].headers = 
              makeColumnHeaders(&printColumn[printColumns].header, 
                                  &printColumn[printColumns].fieldWidth,
                                printRequest[irequest].label?printRequest[irequest].label:name[iname],
                                printRequest[irequest].editLabel, units, &printColumn[printColumns].format, 
                                spreadsheetFlags);
#if defined(DEBUG)
            fprintf(stderr, "%s has format >%s<\n",
                    printRequest[irequest].label?printRequest[irequest].label:name[iname],
                    printColumn[printColumns].format);
#endif
            printColumn[printColumns].data = NULL;
            printColumns++;
            columnUsed[index] = 1;
            if (units)
              free(units);
          }
          free(name[iname]);
        }
        free(name);
      }
      else if (names<0)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      else if (!noWarnings)
        fprintf(stderr, "warning: no column matches %s\n", printRequest[irequest].name);
      free(printRequest[irequest].name);
      if (printRequest[irequest].format)
        free(printRequest[irequest].format);
    }
    free(printRequest);
    *printRequestPtr = printColumn;
    return printColumns;
  }

long processPrintParameters(PRINT_PARAMETER **printRequestPtr, long printRequests, SDDS_DATASET *inTable, long noWarnings, long noLabels,long csv)
{
  PRINT_PARAMETER *printParameter, *printRequest;
  long i, irequest, iname, printParameters, names, *parameterUsed, parameterLimit, maxFieldWidth, index;
  char **name, *units, *format;
  
  if (printRequests<1 || !(printRequest= *printRequestPtr))
    return 0;
  if ((parameterLimit=SDDS_ParameterCount(inTable))<0) {
    if (!noWarnings)
      fprintf(stderr, "warning: no parameter data in input file\n");
    return 0;
  }
  parameterUsed = tmalloc(sizeof(*parameterUsed)*parameterLimit);
  for (i=0; i<parameterLimit; i++)
    parameterUsed[i] = 0;
  printParameter = tmalloc(sizeof(*printParameter)*parameterLimit);
  printParameters = 0;
  name = NULL;
  maxFieldWidth = 0;
  for (irequest=0; irequest<printRequests; irequest++) {
    if ((names=SDDS_MatchParameters(inTable, &name, SDDS_MATCH_STRING, FIND_ANY_TYPE, 
                                    printRequest[irequest].name, SDDS_0_PREVIOUS|SDDS_OR))>0) {
      for (iname=0; iname<names; iname++) {
        if ((index=SDDS_GetParameterIndex(inTable, name[iname]))<0)
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (!parameterUsed[index]) {
          if (SDDS_GetParameterInformation(inTable, "units", &units, SDDS_GET_BY_INDEX, index)!=SDDS_STRING)
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          SDDS_CopyString(&printParameter[printParameters].name, name[iname]);
          printParameter[printParameters].index = index;
          printParameter[printParameters].type = SDDS_GetParameterType(inTable, index);
          if (iname==names-1)
            printParameter[printParameters].endsLine = printRequest[irequest].endsLine;
          printParameter[printParameters].blankLines = printRequest[irequest].blankLines;
          if (SDDS_GetParameterInformation(inTable, "format_string", &format, SDDS_GET_BY_INDEX, index)!=SDDS_STRING)
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

          if (csv) {
            SDDS_CopyString(&printParameter[printParameters].format,
                            csvFormat[printParameter[printParameters].type-1]);
          } else {
            if (printRequest[irequest].format) {
              if (!SDDS_VerifyPrintfFormat(printRequest[irequest].format, printParameter[printParameters].type)) {
                fprintf(stderr, "error: given format (\"%s\") for parameter %s is invalid\n",
                        printRequest[irequest].format, name[iname]);
                exit(1);
              }
              SDDS_CopyString(&printParameter[printParameters].format, printRequest[irequest].format);
            }
            else if (SDDS_StringIsBlank(format) || printRequest[irequest].useDefaultFormat)
              SDDS_CopyString(&printParameter[printParameters].format,
                              defaultFormat[printParameter[printParameters].type-1]);
            else
              SDDS_CopyString(&printParameter[printParameters].format, format);
          }
        
	  if (!noLabels) {
	    printParameter[printParameters].label =
	      makeParameterLabel(&printParameter[printParameters].fieldWidth, 
				 printRequest[irequest].label?printRequest[irequest].label:name[iname],
				 printRequest[irequest].editLabel,
				 units, printParameter[printParameters].format);
	  } else {
	    printParameter[printParameters].label = "";
	      }	    
          if (printParameter[printParameters].fieldWidth>maxFieldWidth)
            maxFieldWidth = printParameter[printParameters].fieldWidth;
          printParameter[printParameters].data = NULL;
          printParameters++;
          parameterUsed[index] = 1;
          if (units)
            free(units);
        }
        free(name[iname]);
      }
      free(name);
    }
    else if (names<0)
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    else if (!noWarnings)
      fprintf(stderr, "warning: no parameter matches %s\n", printRequest[irequest].name);
    free(printRequest[irequest].name);
    if (printRequest[irequest].format)
      free(printRequest[irequest].format);
  }
  for (i=0; i<printParameters; i++) {
    char *ptr;
    if (printParameter[i].fieldWidth<maxFieldWidth) {
      ptr = tmalloc(sizeof(*ptr)*(maxFieldWidth+1));
      strcpy(ptr, printParameter[i].label);
      pad_with_spaces(ptr, maxFieldWidth-printParameter[i].fieldWidth);
      free(printParameter[i].label);
      printParameter[i].label = ptr;
      printParameter[i].fieldWidth = maxFieldWidth;
    }
  }
  free(printRequest);
  *printRequestPtr = printParameter;
  return printParameters;
}

#if SDDS_NUM_TYPES!=8
#error "number of SDDS types is not 8 as expected"
#endif

void setDefaultFormats(void)
{
  defaultFormat = tmalloc(sizeof(*defaultFormat)*SDDS_NUM_TYPES);
  SDDS_CopyString(defaultFormat+SDDS_DOUBLE-1, "%13.6e");
  SDDS_CopyString(defaultFormat+SDDS_FLOAT-1, "%13.6e");
  SDDS_CopyString(defaultFormat+SDDS_LONG-1, "%10" PRId32);
  SDDS_CopyString(defaultFormat+SDDS_ULONG-1, "%10" PRIu32);
  SDDS_CopyString(defaultFormat+SDDS_SHORT-1, "%5hd");
  SDDS_CopyString(defaultFormat+SDDS_USHORT-1, "%5hu");
  SDDS_CopyString(defaultFormat+SDDS_CHARACTER-1, "%c");
  SDDS_CopyString(defaultFormat+SDDS_STRING-1, "%16s");
  csvFormat=tmalloc(sizeof(*csvFormat)*SDDS_NUM_TYPES);
  SDDS_CopyString(csvFormat+SDDS_DOUBLE-1, "%13.6e");
  SDDS_CopyString(csvFormat+SDDS_FLOAT-1, "%13.3e");
  SDDS_CopyString(csvFormat+SDDS_LONG-1, "%" PRId32);
  SDDS_CopyString(csvFormat+SDDS_ULONG-1, "%" PRIu32);
  SDDS_CopyString(csvFormat+SDDS_SHORT-1, "%hd");
  SDDS_CopyString(csvFormat+SDDS_USHORT-1, "%hu");
  SDDS_CopyString(csvFormat+SDDS_CHARACTER-1, "%c");
  SDDS_CopyString(csvFormat+SDDS_STRING-1, "%s");
}


long changeDefaultFormats(char **argv, long argc, long noWarnings)
{
  long i, type;
  char *format;
  static char formatbuffer[100], formatbuffer2[100];

  for (i=0; i<argc; i++) {
    if (!(format=strchr(argv[i], '='))) {
      fprintf(stderr, "-formatDefault syntax error with keyword \"%s\"\n", argv[i]);
      return 0;
    }
    *format++ = 0;
    if (!(type=SDDS_IdentifyType(argv[i]))) {
      fprintf(stderr, "-formatDefault error: unknown type \"%s\"\n", argv[i]);
      return 0;
    }
    if (!SDDS_VerifyPrintfFormat(format, type)) {
      fprintf(stderr, "-formatDefault error: invalid format string \"%s\" for type \"%s\"\n", format, argv[i]);
      return 0;
    }
    free(defaultFormat[type-1]);

    if (SDDS_LONG == type) {
      replaceString(formatbuffer, format, "ld", PRId32, -1, 0);
      replaceString(formatbuffer2, formatbuffer, "lu", PRIu32, -1, 0);
      SDDS_CopyString(defaultFormat+type-1, formatbuffer2);
    } else {
      SDDS_CopyString(defaultFormat+type-1, format);
    }
    if (!getFormatFieldLength(format, NULL) && !noWarnings)
      fprintf(stderr, "warning: no field length for default format \"%s\"---this will produce poor results\n",
              defaultFormat[type-1]);
  }
  return 1;
}

long makeColumnHeaders(char ***header, long *fieldWidth, char *name, char *editLabel, char *units, char **format,
                       unsigned long spreadsheetHeaders)
{
  long formatWidth, unitsWidth, nameWidth, headers, i, formatPadding, formatExtraChars;
  static char buffer[1024], buffer2[1024];
  static char nameBuffer[1024];

  formatWidth = 0;
  
  if (editLabel) {
    strcpy(nameBuffer, name);
    if (!edit_string(nameBuffer, editLabel))
      SDDS_Bomb("Problem editing column label");
    name = nameBuffer;
  }

#if defined(DEBUG)
  fprintf(stderr, "name: >%s<  format: >%s<\n",
          name, *format?*format:"NULL");
#endif

  /* the field width is the large of the units width and the column name width */
  unitsWidth = units ? strlen(units) : 0;
  nameWidth = strlen(name);
  *fieldWidth = unitsWidth>nameWidth?unitsWidth:nameWidth;

  if (!(spreadsheetHeaders&SPREADSHEET_ON)) {
    /* compare the field width to the width in the format string, if any.
       field width needs to be at least equal to the format width + 2 to
       provide buffer spaces 
       */
    if (*format) {
      formatWidth = getFormatFieldLength(*format, &formatExtraChars);
      if ((formatWidth+formatExtraChars)>*fieldWidth)
        *fieldWidth = formatWidth+formatExtraChars;
    }
    *fieldWidth += 2;
    if (*fieldWidth<2)
      *fieldWidth = 2;
#if defined(DEBUG)
    fprintf(stderr, "fieldWidth = %ld, formatWidth = %ld, formatExtra = %ld\n",
            *fieldWidth, formatWidth, formatExtraChars);
#endif
    if (*format && (formatPadding = (*fieldWidth-2-formatExtraChars) - formatWidth)>0) {
      /* come here if the format width is smaller than the field width less
         the extra char and buffer space.  Change the format width to fill up the 
         available space 
         */
      formatWidth = *fieldWidth-2-formatExtraChars;
      replaceFormatWidth(buffer, *format, formatWidth);
#if defined(DEBUG)
      fprintf(stderr, "Adjusted format width to %ld: >%s<\n",
              formatWidth, buffer);
#endif  
      free(*format);
      sprintf(buffer2, " %s ", buffer);
      SDDS_CopyString(format, buffer2);
    } else {
      sprintf(buffer, " %s ", *format);
      free(*format);
      SDDS_CopyString(format, buffer);
    }
    
  }
  
  *header = tmalloc(sizeof(**header)*(headers=2));
  for (i=0; i<headers; i++)
    (*header)[i] = tmalloc(sizeof(***header)* (*fieldWidth+1));

  /* first header is column name */
  copyAndPad((*header)[0], name, nameWidth, *fieldWidth);
  /* second header is column units */
  copyAndPad((*header)[1], units, unitsWidth, *fieldWidth);
#if defined(DEBUG)
  fprintf(stderr, "header0 padded to %ld: >%s<\n", *fieldWidth, (*header)[0]);
  (*header)[0][0] = '+';
  (*header)[0][strlen((*header)[0])-1] = '-';
  fprintf(stderr, "header1 padded to %ld: >%s<\n", *fieldWidth, (*header)[1]);
  (*header)[1][0] = '+';
  (*header)[1][strlen((*header)[1])-1] = '-';
#endif

  return headers;
}

void copyAndPad(char *target, char *source, long sourceWidth, long targetWidth)
{
  long i, head, tail, excess;
  if ((excess=targetWidth-sourceWidth)<0)
    SDDS_Bomb("negative excess in copyAndPad()---programming error");
  head = tail = excess/2;
  if ((i=excess-head-tail))
    head += i;
  for (i=0; i<head; i++)
    target[i] = ' ';
  target[i] = 0;
  if (source)
    strcat(target, source);
  for (i=strlen(target); i<targetWidth; i++)
    target[i] = ' ';
  target[i] = 0;
}

char *makeParameterLabel(long *fieldWidth, char *name, char *editLabel, char *units, char *format)
{
  long formatWidth, labelWidth, extraFormatChars;
  char *label;
  static char buffer[1024];
  
  if (editLabel) {
    strcpy(buffer, name);
    if (!edit_string(buffer, editLabel))
      SDDS_Bomb("Problem editing parameter label");
    name = buffer;
  }
  
  *fieldWidth = (labelWidth = strlen(name)+4 + 
                 (units && !SDDS_StringIsBlank(units) ? strlen(units)+3 : 0)) + 
                   (formatWidth=getFormatFieldLength(format, &extraFormatChars));
  *fieldWidth += extraFormatChars;
  if (*fieldWidth<2)
    *fieldWidth = 2;
  label = tmalloc(sizeof(*label)*(labelWidth+1));
  if (units && !SDDS_StringIsBlank(units))
    sprintf(label, "%s (%s) = ", name, units);
  else
    sprintf(label, "%s = ", name);
  return label;
}

long getFormatFieldLength(char *format, long *extraChars)
{
  long width;
  char *format0;

  format0 = format;
  while (*format && *format!='%')
    format++;
  if (*format!='%') {
    fprintf(stderr, "Bad format string: %s\n", format0);
    exit(1);
  }
  if (extraChars)
    *extraChars = format-format0;
  while (*format && !isdigit(*format))
    if (*format=='.')
      break;
    else 
      format++;
  if (!*format || !isdigit(*format))
    width = 0;
  else {
    width = atoi(format);
  }
  while (isdigit(*format) || *format=='.')
    format++;
  if (*format=='l' || *format=='h')
    format++;
  if (*format)
    format++;
  if (extraChars)
    *extraChars += strlen(format);
  
  return width;
}

void doPrintParameters(SDDS_DATASET *inTable, PRINT_PARAMETER *printParameter, long printParameters, long width, 
                       FILE *fpOut, unsigned long spreadsheetFlags, char *spreadsheetDelimiter,
                       char *spreadsheetQuoteMark, 
                       PAGINATION *pagination, char *title, long noLabels)
{
  long parameter, outputRow, length;
  char printBuffer[2*SDDS_MAXLINE];
  static char *dataBuffer = NULL;

  if (!printParameters)
    return;
  if (!dataBuffer && !(dataBuffer = malloc(sizeof(double)*4)))
    SDDS_Bomb("Allocation failure in doPrintParameters");

  outputRow = 0;
  for (parameter=0; parameter<printParameters; parameter++) {
    if (!SDDS_GetParameter(inTable, printParameter[parameter].name, dataBuffer))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_SprintTypedValue((void*)dataBuffer, 0, printParameter[parameter].type, printParameter[parameter].format,
                               printBuffer, SDDS_PRINT_NOQUOTES))
      
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (spreadsheetFlags&SPREADSHEET_ON) {
      if (!(spreadsheetFlags&SPREADSHEET_NOLABELS))
        fprintf(fpOut, "%s%s", printParameter[parameter].name, spreadsheetDelimiter);
      fprintf(fpOut, "%s%s%s\n", 
              spreadsheetQuoteMark, printBuffer, spreadsheetQuoteMark);
      continue;
    }
    length = strlen(printParameter[parameter].label)+strlen(printBuffer) + (parameter?2:0);
    if ((parameter && printParameter[parameter-1].endsLine) || (outputRow && (outputRow+length)>width)) {
      fputc('\n', fpOut);
      checkPagination(fpOut, pagination, title);
      outputRow = length-2;
      if (parameter && printParameter[parameter-1].blankLines && printParameter[parameter-1].endsLine) { 
        long i;
        for (i=1; i<printParameter[parameter-1].blankLines; i++) {
          checkPagination(fpOut, pagination, title);
          fputc('\n', fpOut);
        }
      }
    }
    else {
      if (parameter)
        fputs("  ", fpOut);
      outputRow += length;
    }
    fputs(printParameter[parameter].label, fpOut);
    fputs(printBuffer, fpOut);
    if (printParameter[parameter].type==SDDS_STRING)
      free(*((char**)dataBuffer));
  }
  if (!(spreadsheetFlags&SPREADSHEET_ON)) {
    checkPagination(fpOut, pagination, title);
    fputc('\n', fpOut);
    checkPagination(fpOut, pagination, title);
    fputc('\n', fpOut);
  }
}

char **makeListOfNames(char *string, long *names)
{
  char **name, *ptr;
  long iName;

  *names = characterCount(string, ',')+1;
  name = tmalloc(sizeof(*name)* *names);
  for (iName=0; string && iName<*names; iName++) {
    if ((ptr=strchr(string, ',')))
      *ptr++ = 0;
    SDDS_CopyString(name+iName, string);
    string = ptr;
  }
  if (iName!=*names)
    SDDS_Bomb("problem occurred scanning list of names");
  return name;
}

long characterCount(char *string, char c)
{
  char *ptr;
  long count=0;
  while ((ptr=strchr(string, c))) {
    count ++;
    string = ptr+1;
  }
  return count;
}


long printPageTitle(FILE *fpOut, char *title)
{
  long lines;
  char *ptr;
  
  fprintf(fpOut, "%s\n", title);
  ptr = title;
  lines = 1;
  while ((ptr=strchr(ptr, '\n'))) {
    ptr++;
    lines++;
  }
  return lines;
}

long checkPagination(FILE *fpOut, PAGINATION *pagination, char *title)
{
  if (!(pagination->flags&PAGINATION_ON))
    return 0;
  if ((pagination->currentLine+=1)>=pagination->lines) {
    fputc('\014', fpOut);
    pagination->currentLine = 0;
    if (!(pagination->flags&PAGINATION_NOTITLE)) 
      pagination->currentLine += printPageTitle(fpOut, title);
    return 1;
  }
  return 0;
}


void printColumnHeaders(FILE *fpOut, PRINT_COLUMN *printColumn, long printColumns,
                        long width, PAGINATION *pagination, long latexFormat, char *latexTitle)
{
  
  long row, column, header, outputRow, maxOutputRow, length, noUnitsLine;

  if (latexFormat) {
    fprintf(fpOut, "\\begin{table}[htb]\\caption{%s}\n", 
            latexTitle?makeTexSafeString(latexTitle):"No caption");
    fprintf(fpOut, "\\begin{center}\n");
    fprintf(fpOut, "\\begin{tabular}{|");
    for (column=0; column<printColumns; column++) 
        fprintf(fpOut, "c|");
    fprintf(fpOut, "}\\hline\n");
  }
  
  noUnitsLine = 1;
  header = 1;
  for (column=0; column<printColumns; column++) {
    if (!SDDS_StringIsBlank(printColumn[column].header[header]))
      break;
  }
  if (column!=printColumns)
    noUnitsLine = 0;

  maxOutputRow = 0;
  for (header=0; header<printColumn[0].headers; header++) {
    outputRow = 0;
    if (noUnitsLine && header==1)
      continue;
    for (column=0; column<printColumns; column++) {
      length = strlen(printColumn[column].header[header]);
      if (!latexFormat &&
          ((column && printColumn[column-1].endsLine) || 
           (width && outputRow && (outputRow+length)>width))) {
        printColumn[column-1].endsLine = 1;
        fputc('\n', fpOut);
        pagination->currentLine ++;
        outputRow = 0;
      }
      if (latexFormat) {
        fputs(makeTexSafeString(printColumn[column].header[header]), fpOut);
        fputs(column==printColumns-1?" \\\\ \\hline ":" & ", fpOut);
      }
      else {
        fputs(printColumn[column].header[header], fpOut);
        outputRow += length;
        if (outputRow>maxOutputRow)
          maxOutputRow = outputRow;
      }
    }
    fputc('\n', fpOut);
    pagination->currentLine ++;
  }
  if (!latexFormat) {
    for (row=0; row<maxOutputRow; row++)
      fputc('-', fpOut);
    fputc('\n', fpOut);
  } 
  pagination->currentLine ++;
}

void doPrintColumns(SDDS_DATASET *inTable, PRINT_COLUMN *printColumn, long printColumns, 
                    long width, FILE *fpOut, unsigned long spreadsheetFlags, char *spreadsheetDelimiter,
                    char *spreadsheetQuoteMark, long latexFormat, char *latexTitle, PAGINATION *pagination, 
                    char *title, long noLabels)
{
  long row, column, rows;
  void **data;
  char printBuffer[16*SDDS_MAXLINE];
  if (!printColumns)
    return;

  if ((rows=SDDS_CountRowsOfInterest(inTable))<0)
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  else if (!rows) 
    return;

  if (!(spreadsheetFlags&SPREADSHEET_ON) && !noLabels)
    printColumnHeaders(fpOut, printColumn, printColumns, width, pagination, latexFormat, latexTitle);
  else if (!(spreadsheetFlags&SPREADSHEET_NOLABELS) && !noLabels) {
    for (column=0; column<printColumns; column++) 
      fprintf(fpOut, "%s%s", 
              printColumn[column].name,
              column!=printColumns-1?spreadsheetDelimiter:"\n");
  }
  
  data = tmalloc(sizeof(*data)*printColumns);
  for (column=0; column<printColumns; column++) 
    if (!(data[column]=SDDS_GetColumn(inTable, printColumn[column].name))) {
      fprintf(stderr, "Error: unable to get data for column %s\n", printColumn[column].name);
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  for (row=0; row<rows; row++) {
    for (column=0; column<printColumns; column++) {
      if (printColumn[column].type==SDDS_STRING &&
          SDDS_StringIsBlank(*((char**)(data[column])+row))) {
        sprintf(printBuffer, printColumn[column].format, " ");
      }
      else {
        if (!SDDS_SprintTypedValue(data[column], row, printColumn[column].type,
                              printColumn[column].format, printBuffer, 
                                   SDDS_PRINT_NOQUOTES)) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      }
      if (spreadsheetFlags&SPREADSHEET_ON) {
        fprintf(fpOut, "%s%s%s%s", spreadsheetQuoteMark, printBuffer, spreadsheetQuoteMark,
                column!=printColumns-1?spreadsheetDelimiter:"\n");
        continue;
      }
      if (latexFormat) {
        fputs(makeTexSafeString(printBuffer), fpOut);
        fputs(column!=printColumns-1?" & ":" \\\\ \\hline", fpOut);
        continue;
      }
      fputs(printBuffer, fpOut);
      if (printColumn[column].endsLine) {
        long i;
        fputc('\n', fpOut);
        if (checkPagination(fpOut, pagination, title))
          printColumnHeaders(fpOut, printColumn, printColumns, width, pagination, latexFormat, latexTitle);
        for (i=0; i<printColumn[column].blankLines; i++) {
          fputc('\n', fpOut);
          if (checkPagination(fpOut, pagination, title))
            printColumnHeaders(fpOut, printColumn, printColumns, width, pagination, latexFormat, latexTitle);
        }
      }
    }
    if (!(spreadsheetFlags&SPREADSHEET_ON) && !printColumn[column-1].endsLine) {
      fputc('\n', fpOut);
      if (!latexFormat && checkPagination(fpOut, pagination, title))
        printColumnHeaders(fpOut, printColumn, printColumns, width, pagination, latexFormat, latexTitle);
    }
  }
  if (latexFormat) {
    fprintf(fpOut, "\\end{tabular}\n");
    fprintf(fpOut, "\\end{center}\n");
    fprintf(fpOut, "%%\\label{tab:LabelHere}\n");
    fprintf(fpOut, "\\end{table}\n");
  }
  for (column=0; column<printColumns; column++) 
    free(data[column]);
  free(data);
}

void replaceFormatWidth(char *buffer, char *format, long width)
{
  char *ptr;
  ptr = format;
  while (*ptr) {
    if (*ptr=='%' && *(ptr+1)!='%')
      break;
    ptr++;
  }
  if (*ptr) {
    *ptr = 0;
    if (*(ptr+1)=='-') {
      sprintf(buffer, "%s%%-%ld", format, width);
      ptr++;
    } else 
      sprintf(buffer, "%s%%%ld", format, width);
    ptr++;
    while (*ptr && isdigit(*ptr))
      ptr++;
    strcat(buffer, ptr);
  } else {
    strcpy(buffer, format);
  }
}

void CreateSCHFile(char *output, char *input, unsigned long flags, char *delimiter,
                   char *quote, PRINT_COLUMN *printColumn, long printColumns)
{
  FILE *fp;
  long i;
  
  if (!(fp=fopen(output, "w"))) 
    SDDS_Bomb("Couldn't open SCHFile for writing.");
  fprintf(fp, "[%s]\nFiletype=Delimited\nDelimiter=%s\nSeparator=%s\nCharSet=ascii\n", 
          input?input:"NULL",
          quote, delimiter);
  for (i=0; i<printColumns; i++) {
    printColumn[i].useDefaultFormat = 1;
    fprintf(fp, "Field%ld=%s,%s,00,00,00\n\n",
            i+1,
            printColumn[i].name,
            SDDS_NUMERIC_TYPE(printColumn[i].type)?"Float":"Char");
  }
}

char *makeTexSafeString(char *source)
{
  static char *buffer = NULL;
  static long buflen = 0;
  long index = 0, length;
  if (!source)
    return source;
  length = strlen(source);
  if (length>(buflen-2)) {
    buflen = length*2+2;
    if (!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*buflen)))
      SDDS_Bomb("memory allocation failure");
  }    
  buffer[0] = 0;
  while (*source) {
    if (*source=='_' || *source=='^' || *source=='{' || *source=='}' || *source=='%') {
      buffer[index++] = '\\';
      buffer[index++] = *source++;
    }
    else if  (*source=='<' || *source=='>' || *source=='|') {
      buffer[index++] = '$';
      buffer[index++] = *source++;
      buffer[index++] = '$';
    }
    else
      buffer[index++] = *source++;
    if (index>=(buflen-1)) {
      buflen = index*2;
      if (!(buffer = SDDS_Realloc(buffer, sizeof(*buffer)*buflen)))
        SDDS_Bomb("memory allocation failure");
    }
  }
  buffer[index] = 0;
  return buffer;
}

