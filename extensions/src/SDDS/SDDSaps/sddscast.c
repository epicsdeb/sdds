/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* 
 $Log: sddscast.c,v $
 Revision 1.7  2006/12/14 22:21:57  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.6  2005/11/04 22:46:12  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.5  2003/09/02 19:16:03  soliday
 Cleaned up code for Linux.

 Revision 1.4  2002/08/14 17:12:40  soliday
 Added Open License

 Revision 1.3  2002/03/22 22:59:53  soliday
 Modifed free_scanargs argument.

 Revision 1.2  2002/01/31 00:10:27  shang
 added -pipe option

 */

#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define COLUMN_MODE 0
#define PARAMETER_MODE 1
#define ARRAY_MODE 2
#define MODES 3
static char *mode_name[MODES] = {
    "column", "parameter", "array",
    } ;

#define SET_CAST 0
#define SET_NOWARNINGS 1
#define SET_PIPE 2
#define N_OPTIONS 3

char *option[N_OPTIONS] = {"cast", "nowarnings", "pipe",} ;

#define CLO_SHORT 0
#define CLO_LONG 1
#define CLO_FLOAT 2
#define CLO_DOUBLE 3
#define TYPES 4
char *types[TYPES]={
  "short","long","float","double",
};

typedef struct {
  char **name;
  char **new_type;
  long *index;
  long n;
} CAST_NAME;

typedef struct {
  char *match_string;
  char *type_string;
  char *new_type;
} CAST_REQUEST;


char *USAGE = "sddscast [<source-file>] [<target-file>] [-pipe=[input][,output]] -noWarnings \n\
-cast={column|parameter|array},<columnNames>,<typeNames>,<newType>\n\
<columnNames> is of the form <name> (with optional wildcards) or \n\
     '(<name>,<name>,....)' \n\
<typeNames> is of the form '(long,short,double,float,*)' \n\
<newType> is long, short,double, or float\n\n\
sddscast converts the numeric columns, parameters or arrays from one datatype to another.\n\n\
Program by Hairong Shang. (This is version 1.1, Jan. 2002)\n";

void process_cast_columns(SDDS_DATASET *SDDS_input,
                          CAST_NAME *column_cast,
                          CAST_REQUEST *col_request,
                          long col_requests,
                          long noWarnings);
void process_cast_parameters(SDDS_DATASET *SDDS_input,
                          CAST_NAME *parameter_cast,
                          CAST_REQUEST *par_request,
                          long par_requests,
                          long noWarnings);
void process_cast_arrays(SDDS_DATASET *SDDS_input,
                             CAST_NAME *array_cast,
                             CAST_REQUEST *array_request,
                             long array_requests,
                             long noWarnings);
long add_casts(char *old_type, long sdds_type, CAST_NAME *cast_name,
               char *add_name, char *new_type, long index,long noWarnings);
void cleanUpCast(CAST_NAME *cast_name);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input, SDDS_output;
  SCANNED_ARG *s_arg;
  char *inputfile, *outputfile, *ptr, *buffer;
  long tmpfile_used,noWarnings,i,i_arg;
  CAST_NAME column_cast, parameter_cast, array_cast;
  CAST_REQUEST *col_request, *par_request, *array_request;
  long col_requests,par_requests,array_requests,rows;
  unsigned long pipeFlags;

  column_cast.n=parameter_cast.n=array_cast.n=0;
  column_cast.name=column_cast.new_type=NULL;
  column_cast.index=parameter_cast.index=array_cast.index=0;
  parameter_cast.name=parameter_cast.new_type=NULL;
  array_cast.name=array_cast.new_type=NULL;
  col_request=par_request=array_request=NULL;
  col_requests=par_requests=array_requests=rows=0; 
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  
  inputfile = outputfile = NULL;
  ptr=buffer=NULL;
  noWarnings=tmpfile_used = pipeFlags = 0;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_NOWARNINGS:
        noWarnings=1;
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case SET_CAST:
        if (s_arg[i_arg].n_items<4)
          SDDS_Bomb("invalid -cast syntax");
        if (match_string(s_arg[i_arg].list[4],types,TYPES,0)<0) 
          SDDS_Bomb("The new type to cast has to be numeric type!");
        switch (match_string(s_arg[i_arg].list[1], mode_name, MODES, 0)) {
        case COLUMN_MODE:
          col_request=SDDS_Realloc(col_request,sizeof(*col_request)*(col_requests+1));
          col_request[col_requests].match_string=s_arg[i_arg].list[2];
          col_request[col_requests].type_string=s_arg[i_arg].list[3];
          col_request[col_requests].new_type=s_arg[i_arg].list[4];
          col_requests++;
          break;
        case PARAMETER_MODE:
          par_request=SDDS_Realloc(par_request,sizeof(*par_request)*(par_requests+1));
          par_request[par_requests].match_string=s_arg[i_arg].list[2];
          par_request[par_requests].type_string=s_arg[i_arg].list[3];
          par_request[par_requests].new_type=s_arg[i_arg].list[4];
          par_requests++;
          break;  
        case ARRAY_MODE:
          array_request=SDDS_Realloc(array_request,sizeof(*array_request)*(array_requests+1));
          array_request[array_requests].match_string=s_arg[i_arg].list[2];
          array_request[array_requests].type_string=s_arg[i_arg].list[3];
          array_request[array_requests].new_type=s_arg[i_arg].list[4];
          array_requests++;
          break;   
        }
        break;
      default:
        SDDS_Bomb("unknown syntax.");
        break;
      }
    }
    else {
      if (inputfile==NULL)
        inputfile = s_arg[i_arg].list[0];
      else if (outputfile==NULL)
        outputfile = s_arg[i_arg].list[0];
      else {
        fprintf(stderr, "argument %s is invalid: too many filenames (sddscast)\n", 
                s_arg[i_arg].list[0]);
        exit(1);
      }
    }
  }
  processFilenames("sddscast", &inputfile, &outputfile, pipeFlags, noWarnings, &tmpfile_used);

  if (!SDDS_InitializeInput(&SDDS_input, inputfile)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_InitializeCopy(&SDDS_output, &SDDS_input, outputfile, "w")) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  process_cast_columns(&SDDS_input,&column_cast,col_request,col_requests,noWarnings);
  process_cast_parameters(&SDDS_input,&parameter_cast,par_request,par_requests,noWarnings);
  process_cast_arrays(&SDDS_input,&array_cast,array_request,array_requests,noWarnings);
  
  for (i=0;i<column_cast.n;i++) {
    if (!SDDS_ChangeColumnInformation(&SDDS_output,"type",column_cast.new_type[i],
                                      SDDS_PASS_BY_STRING|SDDS_SET_BY_NAME,
                                      column_cast.name[i])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    } 
  } 
  for (i=0;i<parameter_cast.n;i++) {
    if (!SDDS_ChangeParameterInformation(&SDDS_output,"type",parameter_cast.new_type[i],
                                         SDDS_PASS_BY_STRING|SDDS_SET_BY_NAME,
                                         parameter_cast.name[i])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    } 
  } 

  for (i=0;i<array_cast.n;i++) {
    if (!SDDS_ChangeArrayInformation(&SDDS_output,"type",array_cast.new_type[i],
                                         SDDS_PASS_BY_STRING|SDDS_SET_BY_NAME,
                                         array_cast.name[i])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    } 
  }
  if (!SDDS_WriteLayout(&SDDS_output)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  while (SDDS_ReadPage(&SDDS_input)>0) {
    if (!SDDS_CopyPage(&SDDS_output,&SDDS_input)||!SDDS_WritePage(&SDDS_output)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (!SDDS_Terminate(&SDDS_input) || !SDDS_Terminate(&SDDS_output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (tmpfile_used && !replaceFileAndBackUp(inputfile, outputfile)) 
    exit(1);
  /*free memory */
  cleanUpCast(&column_cast);
  cleanUpCast(&parameter_cast);
  cleanUpCast(&array_cast); 
  if (array_request) free(array_request);
  if (par_request) free(par_request);
  if (col_request) free(col_request);
  
  free_scanargs(&s_arg,argc);
  return 0;
  
}

void cleanUpCast(CAST_NAME *cast_name)
{
  long i;
  for (i=0;i<cast_name->n;i++) {
    free(cast_name->name[i]);
    free(cast_name->new_type[i]);
    cast_name->name[i]=NULL;
    cast_name->new_type[i]=NULL;
  }
  free(cast_name->name);
  free(cast_name->new_type);
  free(cast_name->index);
  cast_name->name=NULL;
  cast_name->new_type=NULL;
  cast_name->index=NULL;
}


void process_cast_columns(SDDS_DATASET *SDDS_input,
                          CAST_NAME *column_cast,
                          CAST_REQUEST *col_request,
                          long col_requests,
                          long noWarnings)
{
  char *ptr,**Names, **Match_string, **type_string;
  long i,index,j,k,type, s1,s2;
  int32_t names;

  if (!col_requests)
    return;
  ptr=NULL;
  names=s1=s2=0;
  Names=NULL;
  Match_string=type_string=NULL;

  for (i=0;i<col_requests;i++) {
    s1=s2=0;
    ptr=strtok(col_request[i].match_string,",");
    while (ptr!=NULL) {
      Match_string=SDDS_Realloc(Match_string,sizeof(char*)*(s1+1));
      SDDS_CopyString(&Match_string[s1],ptr);
      s1++;
      ptr=strtok(NULL,",");
    }
    ptr=strtok(col_request[i].type_string,",");
    while (ptr!=NULL) {
      type_string=SDDS_Realloc(type_string,sizeof(char*)*(s2+1));
      SDDS_CopyString(&type_string[s2],ptr);
      s2++;
      ptr=strtok(NULL,",");
    }
    if (s2<s1) {
      for (j=s2;j<s1;j++) {
        type_string=SDDS_Realloc(type_string,sizeof(char*)*(j+1));
        SDDS_CopyString(&type_string[j],type_string[s2-1]);
        s2++;
      }
    }
    for (k=0;k<s1;k++) {
      if (!has_wildcards(Match_string[k])) {
        index=SDDS_GetColumnIndex(SDDS_input,Match_string[k]);
        if (index<0) {
          if (!noWarnings) 
            fprintf(stderr,"warning, %s column does not exist.\n",Match_string[k]);
          continue;
        }
        type=SDDS_GetColumnType(SDDS_input, index);
        add_casts(type_string[k],type,column_cast,
                  Match_string[k], col_request[i].new_type,index,noWarnings);
      } else {
        SDDS_SetColumnFlags(SDDS_input, 0);
        if (!SDDS_SetColumnsOfInterest(SDDS_input, SDDS_MATCH_STRING,Match_string[k],SDDS_OR))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (!(Names=(char**)SDDS_GetColumnNames(SDDS_input,&names))) {
          if (!noWarnings) 
            fprintf(stderr,"warning: no columns in the input file match %s.\n",Match_string[k]);
        } else {
          for (j=0;j<names;j++) {
            index=SDDS_GetColumnIndex(SDDS_input,Names[j]);
            type=SDDS_GetColumnType(SDDS_input,index);
            /*fprintf(stderr,"%ld %s\n",j,Names[j]); */
            add_casts(type_string[k],type,column_cast,
                      Names[j], col_request[i].new_type,index,noWarnings);
            free(Names[j]);
          }
          free(Names);
          Names=NULL;
        }
      }
    }
    if (Match_string) 
      SDDS_FreeStringArray(Match_string,s1);
    if (type_string)
      SDDS_FreeStringArray(type_string,s2);
    Match_string=type_string=NULL;
  }
}

void process_cast_parameters(SDDS_DATASET *SDDS_input,
                          CAST_NAME *parameter_cast,
                          CAST_REQUEST *par_request,
                          long par_requests,
                          long noWarnings)
{
  char *ptr,**Names, **Match_string, **type_string;
  long i,index,j,k,type, s1,s2;
  int32_t names;

  if (!par_requests)
    return;
  ptr=NULL;
  names=s1=s2=0;
  Names=NULL;
  Match_string=type_string=NULL;

  if (!(Names=(char**)SDDS_GetParameterNames(SDDS_input,&names))) {
    if (!noWarnings) 
      fprintf(stderr,"warning: no parameters found in the input file match.\n");
    return;
  }
  for (i=0;i<par_requests;i++) {
    s1=s2=0;
    ptr=strtok(par_request[i].match_string,",");
    while (ptr!=NULL) {
      Match_string=SDDS_Realloc(Match_string,sizeof(char*)*(s1+1));
      SDDS_CopyString(&Match_string[s1],ptr);
      s1++;
      ptr=strtok(NULL,",");
    }
    ptr=strtok(par_request[i].type_string,",");
    while (ptr!=NULL) {
      type_string=SDDS_Realloc(type_string,sizeof(char*)*(s2+1));
      SDDS_CopyString(&type_string[s2],ptr);
      s2++;
      ptr=strtok(NULL,",");
    }
    if (s2<s1) {
      for (j=s2;j<s1;j++) {
        type_string=SDDS_Realloc(type_string,sizeof(char*)*(j+1));
        SDDS_CopyString(&type_string[j],type_string[s2-1]);
        s2++;
      }
    }
    for (k=0;k<s1;k++) {
      if (!has_wildcards(Match_string[k])) {
        index=SDDS_GetParameterIndex(SDDS_input,Match_string[k]);
        if (index<0) {
          if (!noWarnings) 
            fprintf(stderr,"warning, %s parameter does not exist in the input file.\n",Match_string[k]);
          continue;
        }
        type=SDDS_GetParameterType(SDDS_input, index);
        add_casts(type_string[k],type,parameter_cast,
                  Match_string[k], par_request[i].new_type,index,noWarnings);
      } else {
        for (j=0;j<names;j++) {
          if(wild_match(Names[j],Match_string[k])) {
            index=SDDS_GetParameterIndex(SDDS_input,Names[j]);
            type=SDDS_GetParameterType(SDDS_input,index);
            add_casts(type_string[k],type,parameter_cast,
                      Names[j], par_request[i].new_type,index,noWarnings);
          }
        }
      }
    }
    if (Match_string) {
      for (j=0;j<s1;j++) 
        free(Match_string[j]);
      free(Match_string);
    }
    if (type_string) {
      for (j=0;j<s2;j++)
        free(type_string[j]);
      free(type_string);
    }
    Match_string=type_string=NULL;
  }
  for (i=0;i<names;i++)
    free(Names[i]);
  free(Names);
}

void process_cast_arrays(SDDS_DATASET *SDDS_input,
                          CAST_NAME *array_cast,
                          CAST_REQUEST *array_request,
                          long array_requests,
                          long noWarnings)
{
  char *ptr,**Names, **Match_string, **type_string;
  long i,index,j,k,type, s1,s2;
  int32_t names;

  if (!array_requests)
    return;
  ptr=NULL;
  names=s1=s2=0;
  Names=NULL;
  Match_string=type_string=NULL;

  if (!(Names=(char**)SDDS_GetArrayNames(SDDS_input,&names))) {
    if (!noWarnings) 
      fprintf(stderr,"warning: unable to get array names in the input file.\n");
    return;
  }
  for (i=0;i<array_requests;i++) {
    s1=s2=0;
    ptr=strtok(array_request[i].match_string,",");
    while (ptr!=NULL) {
      Match_string=SDDS_Realloc(Match_string,sizeof(char*)*(s1+1));
      SDDS_CopyString(&Match_string[s1],ptr);
      s1++;
      ptr=strtok(NULL,",");
    }
    ptr=strtok(array_request[i].type_string,",");
    while (ptr!=NULL) {
      type_string=SDDS_Realloc(type_string,sizeof(char*)*(s2+1));
      SDDS_CopyString(&type_string[s2],ptr);
      s2++;
      ptr=strtok(NULL,",");
    }
    if (s2<s1) {
      for (j=s2;j<s1;j++) {
        type_string=SDDS_Realloc(type_string,sizeof(char*)*(j+1));
        SDDS_CopyString(&type_string[j],type_string[s2-1]);
        s2++;
      }
    }
    for (k=0;k<s1;k++) {
      if (!has_wildcards(Match_string[k])) {
        index=SDDS_GetArrayIndex(SDDS_input,Match_string[k]);
        if (index<0) {
          if (!noWarnings) 
            fprintf(stderr,"warning, %s array does not exist in the input file.\n",Match_string[k]);
          continue;
        }
        type=SDDS_GetArrayType(SDDS_input, index);
        add_casts(type_string[k],type,array_cast,
                  Match_string[k], array_request[i].new_type,index,noWarnings);
      } else {
        for (j=0;j<names;j++) {
          if(wild_match(Names[j],Match_string[k])) {
            index=SDDS_GetArrayIndex(SDDS_input,Names[j]);
            type=SDDS_GetArrayType(SDDS_input,index);
            add_casts(type_string[k],type,array_cast,
                      Names[j], array_request[i].new_type,index,noWarnings);
          }
        }
      }
    }
    if (Match_string) 
      SDDS_FreeStringArray(Match_string,s1);
    if (type_string)
      SDDS_FreeStringArray(type_string,s2);
    Match_string=type_string=NULL;
  }
  for (i=0;i<names;i++)
    free(Names[i]);
  free(Names);
}

long add_casts(char *old_type, long sdds_type, CAST_NAME *cast_name,
               char *add_name, char *new_type, long index,long noWarnings)
{
  long add=0;

  if (!(SDDS_NUMERIC_TYPE(sdds_type))) {
    if (!noWarnings) 
      fprintf(stderr,"warning: the type of %s is not a numeric, cannot cast it to numeric.\n",
              add_name);
    return 0;
  } 
  if (strcmp(old_type,"*")==0)
    add=1;
  else {
    switch (sdds_type) {
    case SDDS_DOUBLE:
      if (strcmp(old_type,"double")==0)
        add=1;
      break;
    case SDDS_FLOAT:
      if (strcmp(old_type,"float")==0)
        add=1;
      break;
    case SDDS_LONG:
      if (strcmp(old_type,"long")==0)
        add=1;
      break; 
    case SDDS_SHORT:
      if (strcmp(old_type,"short")==0)
        add=1;
      break;
    default:
      break;
    }
  }
  if (!add) {
    if (!noWarnings) 
      fprintf(stderr,"warning: The type of %s does not match %s.\n",add_name,old_type);
    return 0;
  }
  cast_name->name=SDDS_Realloc(cast_name->name,sizeof(char*)*(cast_name->n+1));
  cast_name->new_type=SDDS_Realloc(cast_name->new_type,
                                   sizeof(char*)*(cast_name->n+1));
  SDDS_CopyString(&cast_name->name[cast_name->n],add_name);
  SDDS_CopyString(&cast_name->new_type[cast_name->n],new_type);
  cast_name->index=SDDS_Realloc(cast_name->index,sizeof(long)*(cast_name->n+1));
  cast_name->index[cast_name->n]=index;
  cast_name->n++;
  return 1;
}

