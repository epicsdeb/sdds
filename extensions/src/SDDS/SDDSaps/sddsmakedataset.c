/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/* program: sddsmakedataset
 * purpose: make an SDDS file from inputing data in the commandline.
 *
 * Hairong Shang, 2003
 $Log: sddsmakedataset.c,v $
 Revision 1.8  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.7  2006/09/06 15:18:06  soliday
 Updated so that it uses the SDDS_IdentifyType command to determine the data
 type because it does not require a local list of data type names which
 may change in the future. I also added the ability to create ulong and ushort
 data types support.

 Revision 1.6  2003/09/26 17:46:37  soliday
 Fixed problem when using defaultType option.

 Revision 1.5  2003/09/26 14:50:28  soliday
 Moved free_scanargs below replaceFileAndBackUp

 Revision 1.4  2003/08/17 21:11:43  borland
 If the data type for a column is character, and if a single data value is
 given that has length greater than 1, then it is assumed that each character
 of the data string belongs in a new row.  This allows, for example,
 making a file with a character waveform to be fed directly to sddswput.

 Revision 1.3  2003/04/17 23:45:35  shang
 added -description, -contents and -ascii option

 Revision 1.2  2003/04/14 21:29:58  borland
 Fixed argument parsing problem: if no column or parameter name was given
 for -column or -parameter option, there was no error issued.

 Revision 1.1  2003/04/14 18:01:25  shang
 first version, to write the input data given in -column and -parameter format into
 an SDDS output file.

*/

#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define CLO_PARAMETER 0
#define CLO_COLUMN 1
#define CLO_DATA 2
#define CLO_PIPE 3
#define CLO_DEFAULTTYPE 4
#define CLO_NOWARNINGS 5
#define CLO_DESCRIPTION 6
#define CLO_CONTENTS 7
#define CLO_ASCII 8
#define N_OPTIONS 9

/*note: only -pipe=out is valid */
char *option[N_OPTIONS] = {"parameter", "column", "data", "pipe","defaultType","noWarnings", "description","contents","ascii",} ;
char *USAGE = "sddsmakedataset [<oututFile> | -pipe=out] \n\
[-defaultType={double|float|long|short|string|character}] \n\
[-parameter=<name>[,type=<string>][,units=<string>][,symbol=<string>][,description=<string>]]\n\
[-data=<value>] -parameter=.... -data=... \n\
[-column=<name>[,type=<string>][,units=<string>][,symbol=<string>][,description=<string>]]\n\
[-data=<listOfCommaSeparatedValue>] -column=... -data=... \n\
[-noWarnings] [-description=<string>] [-contents=<string>] [-mode=<string>] \n\
<outputFile>         SDDS output file for writing the data to. \n\
-pipe=out            output the data in SDDS format to the pipe instead of to a file.\n\
-defaultType         specify the default data type for paparemeters and columns if not \n\
                     specified in the parameter or column definition. \n\
-parameter           specify the parameter name, data type, units, symbol and description.\n\
-column              specify the column name, data type, units, symbol and/or description.\n\
-data                immediately follow a -parameter option or a -column option to provide \n\
                     the data value for corresponding parameter or column.\n\
-noWarnings          do not print out warning messages. \n\
-ascii               output file in ascii mode, the default is binary. \n\
-description         description of output file .\n\
-contents            contents of the description. \n\n\
sddsmakedataset writes the input data into a file or pipe in SDDS format.\n\
Program by Hairong Shang. (This is version 1.1, April 2003)\n";

typedef struct {
   char *name, *dataString;
   void *data;
   char *description,*symbol,*unit,*typename;
   long type;
} PARAMETER_INFO;

typedef struct {
  char *name, **dataList;
  void *data;
  char *description, *symbol,*unit,*typename;
  long type,rows;
} COLUMN_INFO;


COLUMN_INFO *InitializeColumnInfo();
PARAMETER_INFO *InitializeParameteterInfo();
void SetInfoData(PARAMETER_INFO **parameter, long parameters, 
                     COLUMN_INFO **column,long columns, char *defaultType, long noWarnings,long maxrows);
void FreeMemory(PARAMETER_INFO **parameter, long parameters,COLUMN_INFO **column, long columns,long maxrows);

int main(int argc, char **argv)
{
  SCANNED_ARG *s_arg;
  SDDS_DATASET outTable;
  char *defaultType,*outputFile;
  unsigned long pipeFlags,dummyFlags;
  PARAMETER_INFO **parameter;
  COLUMN_INFO **column;
  long parameters,columns,previousOption,i,j,i_arg,currentOption,tmpfile_used,noWarnings,maxrows=0,outputMode;
  char *input="obset";
  char *description, *contents;
  
  description=contents=NULL;
  parameter=NULL;
  column=NULL;
  parameters=columns=0;
  outputFile=defaultType=NULL;
  pipeFlags=0;
  tmpfile_used=noWarnings=0;
  outputMode=SDDS_BINARY;
  
  SDDS_RegisterProgramName(argv[0]);
  SDDS_CheckDatasetStructureSize(sizeof(SDDS_DATASET));

  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  previousOption=-1;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      currentOption=match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0);
      if (currentOption==CLO_DATA && previousOption!=CLO_PARAMETER && previousOption!=CLO_COLUMN) {
        SDDS_Bomb("-data option has to follow a -parameter or -column option");
      }
      switch (currentOption) {
      case CLO_NOWARNINGS:
        noWarnings=1;
        break;
      case CLO_PARAMETER:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("Invalid -parameter syntax");
        parameter=SDDS_Realloc(parameter,sizeof(*parameter)*(parameters+1));
        parameter[parameters]=InitializeParameteterInfo();
        SDDS_CopyString(&(parameter[parameters]->name),s_arg[i_arg].list[1]);
        s_arg[i_arg].list +=2;
        s_arg[i_arg].n_items -=2;
        if (!(parameter[parameters]->name) || !strlen(parameter[parameters]->name))
          SDDS_Bomb("invalid -parameter syntax (no name)");
        if (s_arg[i_arg].n_items>0 &&
            !scanItemList(&dummyFlags, s_arg[i_arg].list, &s_arg[i_arg].n_items, 0,
                          "type", SDDS_STRING, &(parameter[parameters]->typename), 1, 0,
                          "units", SDDS_STRING, &(parameter[parameters]->unit), 1, 0,
                          "symbol", SDDS_STRING, &(parameter[parameters]->symbol), 1, 0,
                          "description", SDDS_STRING, &(parameter[parameters]->description),
                          1, 0, NULL))
          SDDS_Bomb("invalid -parameter syntax");
        parameters++;
        s_arg[i_arg].list -=2;
        s_arg[i_arg].n_items +=2;
        break;
      case CLO_COLUMN:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("Invalid -column syntax");
        column=SDDS_Realloc(column,sizeof(*column)*(columns+1));
        column[columns]=InitializeColumnInfo();
        SDDS_CopyString(&(column[columns]->name),s_arg[i_arg].list[1]);
        s_arg[i_arg].list +=2;
        s_arg[i_arg].n_items -=2;
        if (!(column[columns]->name) || !strlen(column[columns]->name))
          SDDS_Bomb("invalid -column syntax (no name)");
        if (s_arg[i_arg].n_items>0 &&
            (!scanItemList(&dummyFlags, s_arg[i_arg].list, &s_arg[i_arg].n_items, 0,
                          "type", SDDS_STRING, &(column[columns]->typename), 1, 0,
                          "unit", SDDS_STRING, &(column[columns]->unit), 1, 0,
                          "symbol", SDDS_STRING, &(column[columns]->symbol), 1, 0,
                          "description", SDDS_STRING, &(column[columns]->description),
                          1, 0, NULL)))
          SDDS_Bomb("invalid -column syntax");
        columns++;
        s_arg[i_arg].list -=2;
        s_arg[i_arg].n_items +=2;
        break;
      case CLO_DATA:
        if (previousOption==CLO_PARAMETER) 
          parameter[parameters-1]->dataString=(void*)s_arg[i_arg].list[1];
        if (previousOption==CLO_COLUMN) {
          if (((s_arg[i_arg].n_items-1)==1) &&
              (strlen(s_arg[i_arg].list[1])>1) &&
              (((column[columns-1]->typename) && (strcmp(column[columns-1]->typename, "character")==0)) ||
               ((defaultType) && (strcmp(defaultType, "character")==0)))) {
            /* The user defined a character column with a *single* value longer than 1 character.
             * Assume this is a character string and take each character as a value for a row.
             */
            column[columns-1]->rows=strlen(s_arg[i_arg].list[1]);
            column[columns-1]->dataList= (char**)malloc(sizeof(*(column[columns-1]->dataList))*column[columns-1]->rows);
            for (j=0;j<column[columns-1]->rows;j++) {
              char buffer[2];
              buffer[0] = s_arg[i_arg].list[1][j];
              buffer[1] = 0;
              SDDS_CopyString(&column[columns-1]->dataList[j], buffer);
            }
          } else {
            column[columns-1]->rows=s_arg[i_arg].n_items-1;
            column[columns-1]->dataList=(char**)malloc(sizeof(*(column[columns-1]->dataList))*column[columns-1]->rows);
            for (j=0;j<column[columns-1]->rows;j++)
              SDDS_CopyString(&column[columns-1]->dataList[j],s_arg[i_arg].list[j+1]);
          }
        }
        break;
      case CLO_DEFAULTTYPE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -defaultType option!");
        SDDS_CopyString(&defaultType,s_arg[i_arg].list[1]);
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        if (pipeFlags!=USE_STDOUT)
          SDDS_Bomb("only -pipe=out syntax is valid!");
        break;
      case CLO_DESCRIPTION:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -description option!");
        SDDS_CopyString(&description,s_arg[i_arg].list[1]);
        break;
      case CLO_CONTENTS:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -contents option!");
        SDDS_CopyString(&contents,s_arg[i_arg].list[1]);
        break;
      case CLO_ASCII:
        outputMode=SDDS_ASCII;
        break;
      default:
        fprintf(stderr,"option %s is invalid!\n",s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
      previousOption=currentOption;
    }
    else {
      if (outputFile==NULL)
        SDDS_CopyString(&outputFile,s_arg[i_arg].list[0]);
      else {
        fprintf(stderr, "argument %s is invalid: too many filenames (sddsmakedataset)\n", 
                s_arg[i_arg].list[0]);
        exit(1);
      }
    }
  }
  if (!outputFile && !pipeFlags)
    SDDS_Bomb("Either output file or -pipe=out should be given!");
  if (outputFile && pipeFlags)
    SDDS_Bomb("Only one of output file and -pipe=out should be given!");
  processFilenames("sddsmakedataset",&input,&outputFile,pipeFlags,1,&tmpfile_used);
  if (!columns && !parameters)
    SDDS_Bomb("No data for writing!");
  if (contents && !description)
    if (!noWarnings) {
      fprintf(stderr,"Warning: the description text is not given for description contents.No description will be written\n");
      free(contents);
      contents=NULL;
    }
  for (i=0;i<columns;i++)
    if (maxrows<column[i]->rows)
      maxrows=column[i]->rows;
  SetInfoData(parameter, parameters,column,columns, defaultType, noWarnings,maxrows);

  if (!SDDS_InitializeOutput(&outTable, outputMode, 1, description, contents, outputFile))
    SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0;i<parameters;i++) {
    if (parameter[i]->dataString) {
      if (SDDS_DefineParameter(&outTable,parameter[i]->name,parameter[i]->symbol,parameter[i]->unit,
                               parameter[i]->description,NULL,parameter[i]->type,NULL)<0)
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  
  for (i=0;i<columns;i++) {
    if (column[i]->dataList) {
      if (SDDS_DefineColumn(&outTable,column[i]->name,column[i]->symbol,column[i]->unit,
                            column[i]->description,NULL,column[i]->type,0)<0)
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
  }
  if (!SDDS_WriteLayout(&outTable))
    SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (!SDDS_StartPage(&outTable,maxrows)) 
    SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0;i<columns;i++) {
    if (column[i]->data) {    
      if (!SDDS_SetColumn(&outTable,SDDS_SET_BY_NAME,column[i]->data,maxrows,column[i]->name))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
  }
  for (i=0;i<parameters;i++)
    if (parameter[i]->data) {
      if (!SDDS_SetParameters(&outTable, SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE, parameter[i]->name,
                              parameter[i]->data, NULL)) 
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    }
  if (!SDDS_WritePage(&outTable) || !SDDS_Terminate(&outTable))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  /*free_scanargs has errors in purify mode */
  if (tmpfile_used && !replaceFileAndBackUp(input, outputFile)) 
    exit(1);
  free_scanargs(&s_arg,argc); 
  FreeMemory(parameter,parameters,column,columns,maxrows);
  if (defaultType)
    free(defaultType);
  if (outputFile)
    free(outputFile);
  if (description)
    free(description);
  if (contents)
    free(contents);
  return 0;
}


COLUMN_INFO *InitializeColumnInfo()
{
  COLUMN_INFO *column;
  column=malloc(sizeof(*column));
  column->name=column->unit=column->description=column->symbol=NULL;
  column->data=NULL;
  column->typename=NULL;
  column->rows=0;
  column->type=-1;
  column->dataList=NULL;
  return column;
}

PARAMETER_INFO *InitializeParameteterInfo()
{
  PARAMETER_INFO *parameter;
  parameter=malloc(sizeof(*parameter));
  parameter->name=parameter->unit=parameter->description=parameter->symbol=NULL;
  parameter->typename=NULL;
  parameter->data=NULL;
  parameter->type=-1;
  parameter->dataString=NULL;
  return parameter;
}

void SetInfoData(PARAMETER_INFO **parameter, long parameters, 
                     COLUMN_INFO **column,long columns, char *defaultType, long noWarnings, long maxrows)
{
  long i,j;
  PARAMETER_INFO *par;
  COLUMN_INFO *col;
  char *type;

  type=NULL;
  for (i=0;i<parameters;i++) {
    par=parameter[i];
    if (!par->dataString) {
      if (!noWarnings)
        fprintf(stderr,"warning: no data given for parameter %s, it will not be written into output file.\n",par->name);
      continue;
    }
    if (par->typename)
      SDDS_CopyString(&type,par->typename);
    else {
      if (defaultType)
        SDDS_CopyString(&type,defaultType);
      else
        SDDS_CopyString(&type,"none");
    }
    if ((par->type=SDDS_IdentifyType(type))<=0) {
      fprintf(stderr,"Invalid data type given for parameter %s\n",par->name);
      exit(1);
    }
    par->data=(double*)malloc(sizeof(double));
    switch (par->type) {
    case SDDS_DOUBLE:
      *((double*)par->data) = atof(par->dataString);
      break;
    case SDDS_FLOAT:
      *((float*)par->data) = (float)atof(par->dataString);
      break;
    case SDDS_LONG:
      *((int32_t*)par->data) = atol(par->dataString);
      break;
    case SDDS_ULONG:
#if defined(_WIN32)
      *((uint32_t*)par->data) = (uint32_t)_atoi64(par->dataString);
#else
      *((uint32_t*)par->data) = (uint32_t)atoll(par->dataString);
#endif
      break;
    case SDDS_SHORT:
      *((short*)par->data) = (short)atol(par->dataString);
      break;
    case SDDS_USHORT:
      *((unsigned short*)par->data) = (unsigned short)atol(par->dataString);
      break;
    case SDDS_STRING:
      cp_str((char**)par->data, par->dataString);
      break;
    case SDDS_CHARACTER:
      *((char*)par->data) = par->dataString[0];
      break;
    default:
      SDDS_Bomb("Invalid data type given!");
      break;
    }
    free(type);
  }
  for (i=0;i<columns;i++) {
    col=column[i];
    if (!col->dataList) {
      if (!noWarnings)
        fprintf(stderr,"warning: no data given for column %s, it will not be writtern into output file.\n",col->name);
      continue;
    }
    if (col->typename)
      SDDS_CopyString(&type,col->typename);
    else {
      if (defaultType)
        SDDS_CopyString(&type,defaultType);
      else
        SDDS_CopyString(&type,"none");
    }
    if ((col->type=SDDS_IdentifyType(type))<=0) {
      fprintf(stderr,"Invalid data type given for column %s\n",col->name);
      exit(1);
    }
    if (col->rows<maxrows && !noWarnings)
      fprintf(stderr,"Warning: some data for column %s is missing.\n",col->name);
    switch (col->type) {
    case SDDS_DOUBLE:
      col->data=(double*)malloc(sizeof(double)*maxrows);
      for (j=0;j<col->rows;j++) 
        ((double*)col->data)[j]=atof(col->dataList[j]);
      for (j=col->rows;j<maxrows;j++)
        ((double*)col->data)[j]=0;
      break;
    case SDDS_FLOAT:
      col->data=(float*)malloc(sizeof(float)*maxrows);
      for (j=0;j<col->rows;j++) 
        ((float*)col->data)[j]=(float)atof(col->dataList[j]);
      for (j=col->rows;j<maxrows;j++)
        ((float*)col->data)[j]=0;
      break;
    case SDDS_LONG:
      col->data=(int32_t*)malloc(sizeof(int32_t)*maxrows);
      for (j=0;j<col->rows;j++) 
        ((int32_t*)col->data)[j]=(int32_t)atol(col->dataList[j]);
      for (j=col->rows;j<maxrows;j++)
        ((int32_t*)col->data)[j]=0;
      break;
    case SDDS_ULONG:
      col->data=(uint32_t*)malloc(sizeof(uint32_t)*maxrows);
#if defined(_WIN32)
      for (j=0;j<col->rows;j++) 
        ((uint32_t*)col->data)[j]=(uint32_t)_atoi64(col->dataList[j]);
#else
      for (j=0;j<col->rows;j++) 
        ((uint32_t*)col->data)[j]=(uint32_t)atoll(col->dataList[j]);
#endif
      for (j=col->rows;j<maxrows;j++)
        ((uint32_t*)col->data)[j]=0;
      break;
    case SDDS_SHORT:
      col->data=(short*)malloc(sizeof(short)*maxrows);
      for (j=0;j<col->rows;j++) 
        ((short*)col->data)[j]=(short)atol(col->dataList[j]); 
      for (j=col->rows;j<maxrows;j++)
        ((short*)col->data)[j]=0;
      break;
    case SDDS_USHORT:
      col->data=(unsigned short*)malloc(sizeof(unsigned short)*maxrows);
      for (j=0;j<col->rows;j++) 
        ((unsigned short*)col->data)[j]=(unsigned short)atol(col->dataList[j]); 
      for (j=col->rows;j<maxrows;j++)
        ((unsigned short*)col->data)[j]=0;
      break;
    case SDDS_CHARACTER:
      col->data=(char*)malloc(sizeof(char)*maxrows);  
      for (j=0;j<col->rows;j++) 
        ((char*)col->data)[j]=col->dataList[j][0];
      for (j=col->rows;j<maxrows;j++)
        ((double*)col->data)[j]=0;
      break;
    case SDDS_STRING:
      col->data=(char**)malloc(sizeof(char*)*maxrows);
      for (j=0;j<col->rows;j++) 
        SDDS_CopyString(&(((char**)col->data)[j]),col->dataList[j]);
      for (j=col->rows;j<maxrows;j++)
        SDDS_CopyString(&(((char**)col->data)[j]),"");
      break;
    default:
      SDDS_Bomb("Invalid data type given!");
      break;
    }
    free(type);
  }
}

void FreeMemory(PARAMETER_INFO **parameter, long parameters,COLUMN_INFO **column, long columns,long maxrows)
{
  long i,j;
  for (i=0;i<parameters;i++)
  {
    if (parameter[i]->name) free(parameter[i]->name);
    if (parameter[i]->data) free(parameter[i]->data);
    if (parameter[i]->unit) free(parameter[i]->unit);
    if (parameter[i]->description) free(parameter[i]->description);
    if (parameter[i]->symbol) free(parameter[i]->symbol);
    if (parameter[i]->typename) free(parameter[i]->typename);
    free(parameter[i]);
  }
  if (parameters)
    free(parameter);
  for (i=0;i<columns;i++) {
    if (column[i]->name) free(column[i]->name);
    if (column[i]->dataList) {
      for (j=0;j<column[i]->rows;j++) 
        free(column[i]->dataList[j]);
      free(column[i]->dataList);
    }
    if (column[i]->type!=SDDS_STRING) {
      if (column[i]->data) free(column[i]->data);
    } else {
      if (column[i]->data) {
        for (j=0;j<maxrows;j++)
          free(((char**)column[i]->data)[j]);
        free(column[i]->data);
      }
    }
    if (column[i]->unit) free(column[i]->unit);
    if (column[i]->symbol) free(column[i]->symbol);
    if (column[i]->description) free(column[i]->description);
    free(column[i]);
  }
  if (columns)
    free(column);
}
