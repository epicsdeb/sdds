/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.26  2010/07/29 21:12:00  soliday
 If a numerical column is empty it will now use the value from the previous
 row. If the first rows has empty values it will use the previous behaviour
 of substituting the largest possible value for that data type.

 Revision 1.25  2010/07/29 20:17:05  soliday
 Added the -uselabel option for files that have column names and units in the
 file before the data.

 Revision 1.24  2009/11/18 00:56:04  lemery
 Allow program to proceed when the lines that are skipped have no
 printable characters.  Previously the input file would be closed on
 first instance of no character in a line.

 Revision 1.23  2006/02/02 17:45:09  soliday
 Fixed bug in the call to SDDS_LengthenTable. It was passing the desired number
 of rows instead of the desired increase in rows.

 Revision 1.22  2003/09/02 19:16:01  soliday
 Cleaned up code for Linux.

 Revision 1.21  2002/08/14 17:12:36  soliday
 Added Open License

 Revision 1.20  2002/03/22 22:59:53  soliday
 Modifed free_scanargs argument.

 Revision 1.19  2002/01/28 16:51:05  soliday
 Added free_scanargs

 Revision 1.18  2001/09/05 20:20:00  shang
 add -skiplines option to the syntax which was forgot in previous version.

 Revision 1.17  2001/09/05 20:06:53  shang
 add -skiplines option and fix the read schFile problem, which did not recognize
 "double, string" data type and mistook "float" as "double"

 Revision 1.16  2001/01/23 19:14:54  soliday
 Standardized usage message.

 Revision 1.15  2000/10/05 17:20:33  soliday
 Increased the 1024 character limit per line to 10240 so that it can handle
 really big files.

 Revision 1.14  2000/09/19 20:10:49  borland
 Fixed bug introduced when units capability was introduced by D. Blachowicz.

 Revision 1.13  2000/05/22 16:18:17  soliday
 Fixed a bug so that it calls SDDS_LengthenTable one row earlier.

 Revision 1.12  2000/04/06 13:48:49  borland
 Per D. Blachowicz: added units qualifier to -column option.

 Revision 1.11  1999/09/14 18:29:29  soliday
 interpret_escaped_quotes is no longer defined locally

 Revision 1.10  1999/05/25 19:02:20  soliday
 Removed compiler warning on linux.

 Revision 1.9  1999/01/06 19:54:34  borland
 Fixed the version number in the usage message.

 Revision 1.8  1997/08/25 19:20:29  borland
 Added option to allow setting the maximum number of rows per page.
 Should find a way to eliminate the need for this.

 Revision 1.7  1997/07/16 22:06:36  borland
 Added -spanLines option, which permits parsing data were the rows occupy
 multiple lines.

 Revision 1.6  1997/03/19 19:08:31  borland
 Added exit(0) for normal termination.

 Revision 1.5  1997/03/06 22:20:16  borland
 Now interprets escapes in separator arguments.

 Revision 1.4  1996/11/11 00:55:54  borland
 Fixed bug that caused truncation of tokens from lines with separators but
 no delimiters.

 Revision 1.3  1996/11/07 20:25:33  borland
 Now ignores all lines in SCH file that don't have the form <tag>=<valueList>.

 Revision 1.2  1996/11/07 19:56:23  borland
 Added pipe option.

 Revision 1.1  1996/11/01 19:59:28  borland
 First version.

 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include <ctype.h>

#define SET_ASCIIOUTPUT 0
#define SET_DELIMITERS 1
#define SET_SEPARATOR 2
#define SET_COLUMNDATA 3
#define SET_SCHFILE 4
#define SET_PIPE 5
#define SET_SPANLINES 6
#define SET_MAXROWS 7
#define SET_SKIPLINES 8
#define SET_USELABELS 9
#define N_OPTIONS 10
 
char *option[N_OPTIONS] = {
  "asciioutput", "delimiters", "separator", "columndata", "schfile",
  "pipe", "spanlines", "maxrows","skiplines","uselabels"
};

char *USAGE = "csv2sdds [<inputFile>] [<outputFile>] [-pipe[=in][,out]]\n\
[-asciiOutput] [-spanLines] [-maxRows=<number>]\n\
[-schfile=<filename>] [-skiplines=<number>]\n\
[-delimiters=start=<start>,end=<char>] [-separator=<char>]\n\
[-columnData=name=<name>,type=<type>,units=<units>...]\n\
[-uselabels[=units]]\n\n\
pipe            SDDS toolkit pipe option.\n\
asciiOutput     Requests SDDS ASCII output.  Default is binary.\n\
spanLines       Ignore line breaks in parsing the input data.\n\
schFile         Gives the name of the SCH file that describes the\n\
                columns in the <inputFile>.\n\
delimiter       Gives the delimiter characters that bracket fields.\n\
                The default is \" for both the start and end delimiter.\n\
separator       Gives the separator character between fields.  The default\n\
                is ,\n\
skiplines       skip the first <number> lines of input file. \n\
columnData      Gives the name of a column in <inputFile> along with the\n\
                SDDS data type for the column (one of short, long, float, double,\n\
                character, or string), and optional units input.\n\
                -column options must be given in the\n\
                order corresponding to the order of the data in <inputFile>.\n\
uselabels       The column names and optionally the units are defined in the\n\
                file prior to the data.\n\
maxRows         Maximum number of rows to expect in input.\n\n\
Converts Comma Separated Values data to SDDS.\n\
Program by Michael Borland. (This is version 6, July 2010, R. Soliday.)\n" ;

typedef struct {
    char *name, *units;
    long type, index;
} COLUMN_DATA;

long ParseSchFile(char *file, COLUMN_DATA **columnData, char *separator, char *startDelim,
                  char *endDelim);
void SetUpOutputFile(SDDS_DATASET *SDDSout, char *input, char *output, 
                     COLUMN_DATA *columnData, long columns, long asciiOutput);
char *getToken(char *s,	char separator, char startDelim,
               char endDelim, char *buffer);
void writeOneRowToOutputFile (SDDS_DATASET *SDDSout, char *ptr, char separator,
                             char startDelim,char endDelim,long spanLines,
                             COLUMN_DATA *columnData,long columns,int rows,long maxRows);
void lowerstring(char *ptr);

int main(int argc,char **argv)
{
  FILE *fpi;
  char *input, *output, *schFile;
  SDDS_DATASET SDDSout;
  SCANNED_ARG *scanned;
  long i, iArg, rows, maxRows, column;
  long asciiOutput, columns, spanLines,skipLines=0,lines;
  short columnlabels=0,unitlabels=0,uselabels=0;
  COLUMN_DATA *columnData;
  char separator, startDelim, endDelim;
  char s[10240], *ptr, *typeName, *unitsName;
  unsigned long dummyFlags, pipeFlags;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  input = output = schFile = NULL;
  asciiOutput = spanLines = columns = 0;
  pipeFlags = 0;
  columnData = NULL;
  separator = ',';
  startDelim = '\"';
  endDelim = '\"';
  maxRows = 10000;
    
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      switch (match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_ASCIIOUTPUT:
        asciiOutput = 1;
        break;
      case SET_DELIMITERS:
        if (!(scanned[iArg].n_items-=1) || 
            !scanItemList(&dummyFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "start", SDDS_CHARACTER, &startDelim, 1, 0,
                          "end", SDDS_CHARACTER, &endDelim, 1, 0,
                          NULL)) {
          SDDS_Bomb("invalid -delimiters syntax");
        }
	scanned[iArg].n_items++;
        break;
      case SET_SEPARATOR:
        if (scanned[iArg].n_items!=2 || strlen(scanned[iArg].list[1])<1)
          SDDS_Bomb("invalid -separator syntax");
        interpret_escapes(scanned[iArg].list[1]);
        separator = scanned[iArg].list[1][0];
        break;
      case SET_COLUMNDATA:
        columnData = SDDS_Realloc(columnData, sizeof(*columnData)*(columns+1));
        columnData[columns].name = NULL;
        columnData[columns].units = NULL;
        unitsName = NULL;
        typeName = "string";
        if (!(scanned[iArg].n_items-=1) ||
            !scanItemList(&dummyFlags, scanned[iArg].list+1, &scanned[iArg].n_items, 0,
                          "name", SDDS_STRING, &(columnData[columns].name), 1, 0,
                          "units", SDDS_STRING, &unitsName, 1, 0,
                          "type", SDDS_STRING, &typeName, 1, 0,
                          NULL) ||
            !columnData[columns].name || !strlen(columnData[columns].name) ||
            !typeName || !(columnData[columns].type=SDDS_IdentifyType(typeName)))
          SDDS_Bomb("invalid -columnData syntax");
	scanned[iArg].n_items++;
        columnData[columns].units = unitsName;
        columns++;
        break;
      case SET_SCHFILE:
        if (scanned[iArg].n_items!=2)
          SDDS_Bomb("invalid -schFile syntax");
        schFile = scanned[iArg].list[1];
        if (!fexists(schFile)) {
          fprintf(stderr, "file not found: %s (csv2sdds)\n", schFile);
          exit(1);
        }
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_SPANLINES:
        spanLines = 1;
        break;
      case SET_MAXROWS:
        if (scanned[iArg].n_items!=2 || strlen(scanned[iArg].list[1])<1 ||
            sscanf(scanned[iArg].list[1], "%ld", &maxRows)!=1 ||
            maxRows<1) 
          SDDS_Bomb("invalid -maxRows syntax");
        break;
      case SET_SKIPLINES:
        if (scanned[iArg].n_items!=2 || strlen(scanned[iArg].list[1])<1 ||
            sscanf(scanned[iArg].list[1], "%ld", &skipLines)!=1 ||
            skipLines<1) 
          SDDS_Bomb("invalid -skipline syntax");
        break;  
      case SET_USELABELS:
        if (scanned[iArg].n_items>2)
          SDDS_Bomb("invalid -uselabels syntax");
        uselabels=1;
        columnlabels=1;
        if (scanned[iArg].n_items==2)
          unitlabels=1;
        break;
      default:
        bomb("invalid option seen", USAGE);
        break;
      }
    }
    else {
      if (!input)
        input = scanned[iArg].list[0];
      else if (!output)
        output = scanned[iArg].list[0];
      else
        bomb("too many filenames", USAGE);
    }
  }
  
  if (!columns && !schFile && !columnlabels)
    SDDS_Bomb("either give one or more -columnData options or -schFile option or -uselabels option");
  if (columns && schFile)
    SDDS_Bomb("either give one or more -columnData options or -uselabels option");
  if (columns && columnlabels)
    SDDS_Bomb("either give one or more -columnData options or -uselabels option");
  if (schFile && columnlabels)
    SDDS_Bomb("either give -schFile options or -uselabels option");
  
  processFilenames("csv2sdds", &input, &output, pipeFlags, 0, NULL);
  if (input) {
    if (!fexists(input))
      SDDS_Bomb("input file not found");
    if (!(fpi = fopen(input, "r")))
      SDDS_Bomb("problem opening input file");
  } else {
    fpi = stdin;
  }

  if (!columnlabels) {
    if (!columns && !(columns=ParseSchFile(schFile, &columnData, &separator, &startDelim, &endDelim)))
      SDDS_Bomb("problem reading or parsing sch file");
  
    SetUpOutputFile(&SDDSout, input, output, columnData, columns, asciiOutput);
    
    if (!SDDS_StartPage(&SDDSout, maxRows))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  rows = 0;      /* the row index we are storing in */
  column = 0;    /* the column index we are storing to */
  lines=0;
  
  while (fgets(s, 10240, fpi)) {
    lines++;
    /* convert unprintable character to a null */
    while ((i=strlen(s)) && s[i-1]<27)
      s[i-1] = 0;
    /* I added the second condition because sometimes we get files that
       has headers of fixed number of lines that are garbage. 
       The loop will break only when real data has first been read */
    if (strlen(s)==0 && (skipLines && (lines > skipLines)))
      break;
    ptr = s;
#if defined(DEBUG)
    fprintf(stderr, "line: >%s<\n", ptr);
#endif
    if (columnlabels && (!skipLines || (lines > skipLines))) {
      char t[10240];
      t[0]=0;
      while (1) {
        ptr = getToken(ptr, separator, startDelim, endDelim, t);
        if (strlen(t)==0)
          break;
        columnData = SDDS_Realloc(columnData, sizeof(*columnData)*(columns+1));
        columnData[columns].name = malloc(sizeof(char*) * strlen(t));
        replace_chars(t, (char*)" ", (char*)"_");
        sprintf(columnData[columns].name, "%s", t);
        columnData[columns].units = NULL;
        columnData[columns].type = SDDS_STRING;
        columns++;
      }
      columnlabels = 0;
      continue;
    } else if (unitlabels && (!skipLines || (lines > skipLines))) {
      char t[10240];
      t[0]=0;
      for (i=0; i<columns; i++) {
        ptr = getToken(ptr, separator, startDelim, endDelim, t);
        if (strlen(t)==0)
          break;
        columnData[i].units = malloc(sizeof(char*) * strlen(t));
        sprintf(columnData[i].units, "%s", t);
      }
      unitlabels = 0;
      continue;
    }
    if (uselabels) {
      char *tmpPtr;
      char t[10240];
      double tD;
      t[0]=0;
      tmpPtr = ptr;
      for (i=0; i<columns; i++) {
        tmpPtr = getToken(tmpPtr, separator, startDelim, endDelim, t);
        if (strlen(t)==0)
          break;
        if (sscanf(t, "%lf", &tD)==1)
          columnData[i].type = SDDS_DOUBLE;
      }
      SetUpOutputFile(&SDDSout, input, output, columnData, columns, asciiOutput);    
      if (!SDDS_StartPage(&SDDSout, maxRows))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      uselabels=0;
    }
    if (!skipLines || (lines > skipLines)) {
      writeOneRowToOutputFile (&SDDSout,ptr,separator,startDelim,endDelim,
                               spanLines,columnData,columns,rows,maxRows);
      rows++;
    }
    if (rows>=maxRows-1) {
      if (!SDDS_LengthenTable(&SDDSout, 1000)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
      maxRows += 1000;
    }
  }
  
  fclose(fpi);
  if (!SDDS_WritePage(&SDDSout) || !SDDS_Terminate(&SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  free_scanargs(&scanned, argc);

  return(0);
}
  
long ParseSchFile(char *file, COLUMN_DATA **columnData, char *separator, char *startDelim,
                  char *endDelim)
{
  FILE *fp;
  char s[10240], *ptr, *ptr0;
  long l, fieldIndex, lastFieldIndex, columns;
  
  if (!(fp=fopen(file, "r")))
    SDDS_Bomb("Unable to open SCH file");

  lastFieldIndex = 0;
  columns = 0;
  while (fgets(s, 10240, fp)) {
    while ((l=strlen(s)) && s[l-1]<27)
        s[l-1] = 0;
    if (strlen(s)==0)
      continue;
    if (!(ptr=strchr(s, '=')))
      continue;
    *ptr++ = 0;
    if (strcmp(s, "Filetype")==0) {
      if (strcmp(ptr, "Delimited"))
        SDDS_Bomb("Require Filetype = Delimited in SCH file.");
    }
    else if (strcmp(s, "Separator")==0) {
      if (!(*separator = *ptr))
        SDDS_Bomb("Null separator in SCH file.");
    }
    else if (strcmp(s, "Delimiter")==0) {
      if (!(*endDelim = *startDelim = *ptr))
        SDDS_Bomb("Null delimiter in SCH file.");
    }
    else if (strcmp(s, "CharSet")==0) {
      if (strcmp(ptr, "ascii"))
        SDDS_Bomb("Require CharSet = ascii in SCH file.");
    }
    else if (strncmp(s, "Field", strlen("Field"))==0) {
      if (!sscanf(s, "Field%ld", &fieldIndex))
        SDDS_Bomb("Error scanning field index in SCH file.");
      if (fieldIndex-lastFieldIndex!=1)
        SDDS_Bomb("Gap or nonmonotonicity in field index values.");
      lastFieldIndex = fieldIndex;
      *columnData = SDDS_Realloc(*columnData, sizeof(**columnData)*(columns+1));
      delete_chars(ptr, " ");
      ptr0 = ptr;
      if (!(ptr=strchr(ptr0, ','))) 
        SDDS_Bomb("Field name not found.");
      *ptr = 0;
      SDDS_CopyString(&((*columnData)[columns].name), ptr0);
      (*columnData)[columns].units = NULL;
      ptr0 = ptr+1;
      if (!(ptr=strchr(ptr0, ',')))
        SDDS_Bomb("Field type not found.");
      *ptr = 0;
     
      lowerstring(ptr0);
      if (strcmp(ptr0,"string")==0)
        (*columnData)[columns].type = SDDS_STRING;
      else if (strcmp(ptr0,"char")==0)
        (*columnData)[columns].type = SDDS_STRING;
      else if (strcmp(ptr0,"float")==0) 
        (*columnData)[columns].type = SDDS_FLOAT;
      else if (strcmp(ptr0,"double")==0 ) 
        (*columnData)[columns].type = SDDS_DOUBLE;
      else { 
        fprintf(stderr,"Unknow type '%s' given to '%s'\n",ptr0,(*columnData)[columns].name);
        exit(1);
      }
      columns++;
    }
    else {
      fprintf(stderr, "Warning: unknown tag value in SCH file: %s\n", s);
    }
  }
  return columns;
}

void SetUpOutputFile(SDDS_DATASET *SDDSout, char *input, char *output, COLUMN_DATA *columnData,
                     long columns, long asciiOutput)
{
  char s[10240];
  long i;
  
  sprintf(s, "csv2sdds conversion of %s", input?input:"stdin");
  
  if (!SDDS_InitializeOutput(SDDSout, asciiOutput?SDDS_ASCII:SDDS_BINARY,
                             1, NULL, s, output))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    
  for (i=0; i<columns; i++) {
      if ((columnData[i].index = 
	   SDDS_DefineColumn(SDDSout, columnData[i].name, NULL, columnData[i].units, NULL, NULL, 
                           columnData[i].type, 0))<0) {
	sprintf(s, "Problem defining column %s.", columnData[i].name);
	SDDS_SetError(s);
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
  }
  if (!SDDS_WriteLayout(SDDSout))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}


char *getToken(
               char *s,	            /* the string to be scanned */
               char separator,     /* typically , */
               char startDelim,    /* typically " */
               char endDelim,      /* typically " */
               char *buffer         /* place to put the result */
               )
{
    char *ptr;
    char *seek_level();
    if (*s==0) {
      buffer[0] = 0;
      return s;
    }
      
    if (*s==separator) {
      /* zero-length token */
      buffer[0] = 0;
      /* advance to next position */
      return s+1;
    }

    /* Check for quotes.  If found, return quote-bounded data. */
    if (*s==startDelim) {
      s++;
      ptr = s;
      while (*ptr) {
        if (*ptr==endDelim && *(ptr-1)!='\\') {
          ptr++;
          break;
        }
        ptr++;
      }
      strncpy(buffer, s, ptr-s-1);
      buffer[ptr-s-1] = 0;
      interpret_escaped_quotes(buffer);
      if (*ptr && *ptr==separator)
        return ptr+1;
      return ptr;
    }
    
    /* advance until the next separator is found */
    ptr = s;
    while (*ptr && *ptr!=separator)
      ptr++;
    if (*ptr==separator) {
      strncpy(buffer, s, ptr-s);
      buffer[ptr-s] = 0;
      return ptr+1;
    }
    strcpy(buffer, s);
    buffer[ptr-s] = 0;
    return ptr;
  }

void writeOneRowToOutputFile (SDDS_DATASET *SDDSout, char *ptr, char separator,
                             char startDelim,char endDelim,long spanLines,
                             COLUMN_DATA *columnData,long columns,int rows,long maxRows)
{
  int column=0;
  char t[10240];
  double doubleValue;
  float floatValue;
  short shortValue;
  long longValue, nullData=0;
  char charValue;
  t[0]=0;

  for (column=0;column<columns; column++) {
    ptr = getToken(ptr, separator, startDelim, endDelim, t);
#if defined(DEBUG)
    fprintf(stderr, "token: >%s<\n", t);
#endif
    nullData = 0;
    if (strlen(t)==0)
      nullData = 1;
    if (nullData && spanLines) {
      break;
    }
    switch (columnData[column].type) {
    case SDDS_SHORT:
      if (nullData || !sscanf(t, "%hd", &shortValue))
        if (rows == 0)
          shortValue = SHRT_MAX;
        else
          shortValue = ((short*)SDDSout->data[columnData[column].index])[rows-1];
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows,
                             columnData[column].index, shortValue, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    case SDDS_LONG:
      if (nullData || !sscanf(t, "%ld", &longValue))
        if (rows == 0)
          longValue = LONG_MAX;
        else
          longValue = ((long*)SDDSout->data[columnData[column].index])[rows-1];
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows,
                             columnData[column].index, longValue, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    case SDDS_FLOAT:
      if (nullData || !sscanf(t, "%f", &floatValue))
        if (rows == 0)
          floatValue = FLT_MAX;
        else
          floatValue = ((float*)SDDSout->data[columnData[column].index])[rows-1];
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows,
                             columnData[column].index, floatValue, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    case SDDS_DOUBLE:
      if (nullData || !sscanf(t, "%lf", &doubleValue)) {
        if (rows == 0)
          doubleValue = DBL_MAX;
        else 
          doubleValue = ((double*)SDDSout->data[columnData[column].index])[rows-1];
      }
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows,
                             columnData[column].index, doubleValue, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    case SDDS_CHARACTER:
      if (nullData || !sscanf(t, "%c", &charValue))
        if (rows == 0)
          charValue = 0;
        else 
          charValue = ((char*)SDDSout->data[columnData[column].index])[rows-1];
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows, 
                             columnData[column].index, charValue, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    case SDDS_STRING:
      SDDS_InterpretEscapes(t);
      if (!SDDS_SetRowValues(SDDSout, SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE, rows,
                             columnData[column].index, t, -1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      break;
    default:
      SDDS_Bomb("Unknown/unsupported data type encountered.");
      exit(1);
    }
  }
}

void lowerstring(char *ptr) {
  int size,i;
  size=strlen(ptr);
  for (i=0;i<size;i++)
    ptr[i]=tolower(ptr[i]);
}


