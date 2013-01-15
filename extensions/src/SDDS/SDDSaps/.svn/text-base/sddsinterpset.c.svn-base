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
 Revision 1.13  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.12  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.11  2005/08/23 19:25:21  jiaox
 fixed the bug that causes the column definition failure.

 Revision 1.9  2003/11/14 19:00:13  shang
 added "setting the functionOf column with given atValue" back, which was omitted
 in previous version.

 Revision 1.8  2003/11/11 15:39:16  shang
 it now processes multiply-page input data and accepts multiply data options.

 Revision 1.7  2003/10/07 16:32:11  shang
 removed interpolate() and the definition of OUTRANGE_CONTROL structure, since
 they have been moved to libary

 Revision 1.6  2003/09/02 19:16:05  soliday
 Cleaned up code for Linux.

 Revision 1.5  2002/10/23 02:49:33  shang
 added SDDS_CopyParameter so that the output file has the parameters from the input

 Revision 1.4  2002/08/14 17:12:48  soliday
 Added Open License

 Revision 1.3  2002/04/25 15:53:21  soliday
 Added support for WIN32

 Revision 1.2  2002/03/22 22:59:54  soliday
 Modifed free_scanargs argument.

 Revision 1.1  2002/02/13 22:59:45  shang
 first version


 */


#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <ctype.h>

#define CLO_ORDER 0
#define CLO_PIPE 1
#define CLO_BELOWRANGE 2
#define CLO_ABOVERANGE 3
#define CLO_DATA 4
#define CLO_VERBOSE 5
#define CLO_OPTIONS 6

char *option[CLO_OPTIONS] = {
  "order","pipe","belowrange","aboverange","data","verbose",
};

static char *USAGE1 = "sddsinterpset [<input>] [<output>] [-pipe=[input][,output]]\n\
 [-order=<number>] [-verbose] \n\
 [-data=fileColumn=<colName>,interpolate=<colName>,functionof=<colName>, \n\
     column=<colName> | atValue=<value>] \n\
 [-belowRange={value=<value>|skip|saturate|extrapolate|wrap}[,{abort|warn}]]\n\
 [-aboveRange={value=<value>|skip|saturate|extrapolate|wrap}[,{abort|warn}]]\n\
-verbose      -- print out messages. \n\
-pipe         -- The standard SDDS Toolkit pipe option. \n\
-data option: \n\
  -fileColumn -- Gives the name of a column in <input> that contains the \n\
                   names of files with tables of data. \n\
  -interpolate-- Gives the name of a column that must exist in all the \n\
                   files named in fileColumn.  This column will exist in \n\
                   the output file. \n\
  -functionOf -- Gives the name of a column that must exist in all the \n\
                   files named in fileColumn.  The 'interpolate' column \n\
                   is viewed as a function of this column:  I(F). \n\
  -column     -- Gives the name of a column in <input>.  The primary output \n\
                   is I(C). \n";
static char *USAGE2 = "  -atValue    -- Gives a number at which to perform the interpolations. \n\
-order        -- the order of the polynomials to use for interpolation. \n\
                 The default is 1, indicating linear interpolation. \n\
-belowRange  \n\
-aboveRange   -- They have the same options, which specify the behavior \n\
                 in the event that an interpolation point is, respectively, \n\
                 below or above the range of the independent data. If such an \n\
                 out-of-range point occurs, the default behavior is to assign \n\
                 the value at the nearest endpoint of the data; this is identical \n\
                 to specifying saturate. One may specify use of a specific value \n\
                 with value=value. skip specifies that offending points should be \n\
                 discarded. extrapolate specifies extrapolation beyond the limits \n\
                 of the data. wrap specifies that the data should be treated as \n\
                 periodic. abort specifies that the program should terminate. warn \n\
                 requests warnings for out-of-bounds points. \n\
sddsinterpset is used to perform multiple interpolations. The file provided in each row of \n\
the input file is the source of an interpolation table. \n\n\
Program by Hairong Shang.  (This is version 1, Feb. 2002)\n";


#define AT_COLUMN 0x00000001
#define AT_VALUE  0x00000002

typedef struct {
   char *fileColumn,*interpCol, *funcOfCol,*atCol;
   char **file; /*an name array of files which contain the data */
   long files;
   double atValue; /*it should not appear if atCol is given */
   double *colValue; /*the value of atCol in the input*/
   unsigned long hasdata, flags;
} DATA_CONTROL;

long processDataControls(char *input, DATA_CONTROL **data_control, long dataControls,long verbose,
                         SDDS_DATASET *SDDSout, char *output,long *rows);
long checkMonotonicity(double *indepValue, long rows);

void freedatacontrol(DATA_CONTROL *data_control, long dataControls);

int main(int argc, char **argv)
{
  int i_arg;
  char *input,*output, **interpCol,**funcOf;
  long i,j,order,dataControls,verbose, outputs,valid_option,monotonicity,datarows;
  SCANNED_ARG *s_arg;
  OUTRANGE_CONTROL aboveRange, belowRange;
  DATA_CONTROL *data_control;
  unsigned long pipeFlags, dummyFlags, interpCode;
  SDDS_DATASET SDDSdata, SDDSout, SDDSin;
  double *indepValue, *depenValue,**out_depenValue, atValue=0;
  int32_t **rowFlag;
  long valid_data=0,index,row,pages;
  int32_t *rows;
  
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv); 
  if (argc<3) {
    fprintf(stderr, "%s%s", USAGE1, USAGE2);
    exit(1);
  }
  data_control=NULL;
  input=output=NULL;
  interpCol=funcOf=NULL;
  dataControls=verbose=outputs=0;
  order=1;
  pipeFlags=dummyFlags=0;
  indepValue=depenValue=NULL;
  out_depenValue=NULL;
  rows=NULL;
  aboveRange.flags = belowRange.flags = OUTRANGE_SATURATE;
  valid_option=1;
  rowFlag=NULL;

  for (i_arg=1;i_arg<argc;i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, CLO_OPTIONS, 0)) {
      case CLO_ORDER:
        if (s_arg[i_arg].n_items!=2 || sscanf(s_arg[i_arg].list[1], "%ld", &order)!=1 ||
            order<1)
          SDDS_Bomb("invalid -order syntax/value");
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_ABOVERANGE:
        if ((s_arg[i_arg].n_items-=1)<1 ||
            !scanItemList(&aboveRange.flags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "value", SDDS_DOUBLE, &aboveRange.value, 1, OUTRANGE_VALUE,
                          "skip", -1, NULL, 0, OUTRANGE_SKIP,
                          "saturate", -1, NULL, 0, OUTRANGE_SATURATE,
                          "extrapolate", -1, NULL, 0, OUTRANGE_EXTRAPOLATE,
                          "wrap", -1, NULL, 0, OUTRANGE_WRAP,
                          "abort", -1, NULL, 0, OUTRANGE_ABORT,
                          "warn", -1, NULL, 0, OUTRANGE_WARN,
                          NULL))
          SDDS_Bomb("invalid -aboveRange syntax/value");
        if ((i=bitsSet(aboveRange.flags&
                       (OUTRANGE_VALUE|OUTRANGE_SKIP|OUTRANGE_SATURATE|OUTRANGE_EXTRAPOLATE|OUTRANGE_WRAP|OUTRANGE_ABORT)))>1)
          SDDS_Bomb("incompatible keywords given for -aboveRange");
        if (i!=1)
          aboveRange.flags |= OUTRANGE_SATURATE;
        break;
      case CLO_VERBOSE:
        verbose=1;
        break;
      case CLO_BELOWRANGE:
        if ((s_arg[i_arg].n_items-=1)<1 ||
            !scanItemList(&belowRange.flags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "value", SDDS_DOUBLE, &belowRange.value, 1, OUTRANGE_VALUE,
                          "skip", -1, NULL, 0, OUTRANGE_SKIP,
                          "saturate", -1, NULL, 0, OUTRANGE_SATURATE,
                          "extrapolate", -1, NULL, 0, OUTRANGE_EXTRAPOLATE,
                          "wrap", -1, NULL, 0, OUTRANGE_WRAP,
                          "abort", -1, NULL, 0, OUTRANGE_ABORT,
                          "warn", -1, NULL, 0, OUTRANGE_WARN,
                          NULL))
          SDDS_Bomb("invalid -belowRange syntax/value");
        if ((i=bitsSet(belowRange.flags&
                       (OUTRANGE_VALUE|OUTRANGE_SKIP|OUTRANGE_SATURATE|OUTRANGE_EXTRAPOLATE|OUTRANGE_WRAP|OUTRANGE_ABORT)))>1)
          SDDS_Bomb("incompatible keywords given for -belowRange");
        if (i!=1)
          belowRange.flags |= OUTRANGE_SATURATE;
        break;
      case CLO_DATA:
        if ((s_arg[i_arg].n_items -=1)<4)
          SDDS_Bomb("invalid -data syntax");
        data_control=SDDS_Realloc(data_control,sizeof(*data_control)*(dataControls+1));
        data_control[dataControls].fileColumn=data_control[dataControls].interpCol=
          data_control[dataControls].funcOfCol=data_control[dataControls].atCol=NULL;
        data_control[dataControls].file=NULL;
        data_control[dataControls].files=0;
        data_control[dataControls].hasdata=0;
        data_control[dataControls].colValue=NULL;
        data_control[dataControls].flags=0;
        if (!scanItemList(&data_control[dataControls].flags,s_arg[i_arg].list+1,&s_arg[i_arg].n_items,0,
                          "fileColumn",SDDS_STRING,&(data_control[dataControls].fileColumn),1,0,
                          "interpolate",SDDS_STRING,&(data_control[dataControls].interpCol),1,0,
                          "functionof",SDDS_STRING,&(data_control[dataControls].funcOfCol),1,0,
                          "column",SDDS_STRING,&(data_control[dataControls].atCol),1,AT_COLUMN,
                          "atValue",SDDS_DOUBLE,&(data_control[dataControls].atValue),1,AT_VALUE,
                          NULL) || !data_control[dataControls].fileColumn
            || !data_control[dataControls].interpCol || !data_control[dataControls].funcOfCol)
          SDDS_Bomb("Invalid -data syntax");
        if (!(data_control[dataControls].flags&AT_COLUMN) && !(data_control[dataControls].flags&AT_VALUE))
          SDDS_Bomb("Invali -data syntax: either column or atValue option should be given.");
        if ( (dummyFlags&AT_COLUMN) && (dummyFlags&AT_VALUE))
          SDDS_Bomb("Invali -data syntax: column and atValue options are not compatible.");
        valid_option=1;
        if (dataControls) {
          if (match_string(data_control[dataControls].funcOfCol,funcOf,dataControls,EXACT_MATCH)>0) {
            fprintf(stderr,"Multiple independent columns provided!");
            exit(1);
          }
          if (match_string(data_control[dataControls].interpCol,interpCol,dataControls,EXACT_MATCH)>0) {
            fprintf(stderr,"Warning:Interpolate column %s has been used.\n",data_control[dataControls].interpCol);
            valid_option=0;
          }
        }
        if (valid_option) {
          interpCol=SDDS_Realloc(interpCol,sizeof(*interpCol)*(dataControls+1));
          funcOf=SDDS_Realloc(funcOf,sizeof(*funcOf)*(dataControls+1));
          interpCol[dataControls]=data_control[dataControls].interpCol;
          funcOf[dataControls]=data_control[dataControls].funcOfCol;
          dataControls++;
        }
        break;
      default:
        fprintf(stderr, "error: unknown/ambiguous option: %s\n", 
                s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    }
    else {
      if (!input)
        input=s_arg[i_arg].list[0];
      else if (!output)
        output=s_arg[i_arg].list[0];
      else
        SDDS_Bomb("too many filenams seen");
    }
  }

  processFilenames("sddsinterpset", &input, &output, pipeFlags, 0, NULL);
 
  /*if (!processDataControls(input,&data_control,dataControls,verbose,&SDDSout,output,&rows)) {
    fprintf(stderr,"No valid -data options given for processing data.\n");
    exit(1);
  } */
  if (!SDDS_InitializeInput(&SDDSin, input))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0;i<dataControls;i++) {
    if ((index=SDDS_GetColumnIndex(&SDDSin,(data_control)[i].fileColumn))<0) {
      fprintf(stderr,"Warning, %s column does not exist in input file %s.\n",
              data_control[i].fileColumn,input);
      continue;
    }
    if (SDDS_GetColumnType(&SDDSin,index)!=SDDS_STRING) {
      fprintf(stderr,"Error, %s in input file %s is not a string column.\n",
              data_control[i].fileColumn,input);
      continue;
    }
    if (data_control[i].atCol) {
      if ((index=SDDS_GetColumnIndex(&SDDSin,data_control[i].atCol))<0) {
        fprintf(stderr,"Warning, %s column does not exist in input file %s.\n",
                data_control[i].fileColumn,input);
        continue;
      }
      if (!SDDS_NUMERIC_TYPE(SDDS_GetColumnType(&SDDSin,index))) {
        fprintf(stderr,"Error, %s in input file %s is not a numeric column.\n",
                data_control[i].atCol,input);
        continue;
      }
    }
    data_control[i].hasdata=1;
    valid_data++;
  }
  if (!valid_data) {
    fprintf(stderr,"No valid -data options given for processing data.\n");
    exit(1);
  }
  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w")) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  pages=0;
  while (SDDS_ReadPage(&SDDSin)>0) {
    rows=SDDS_Realloc(rows,sizeof(*rows)*(pages+1));
    out_depenValue=SDDS_Realloc(out_depenValue,sizeof(*out_depenValue)*(pages+1));
    rowFlag=SDDS_Realloc(rowFlag,sizeof(*rowFlag)*(pages+1));
    if (!(rows[pages]=SDDS_CountRowsOfInterest(&SDDSin))) {
      fprintf(stderr,"No data found in input file %s.\n",input);
      exit(1);
    }
    rowFlag[pages]=(int32_t*)malloc(sizeof(**rowFlag)*rows[pages]);
    out_depenValue[pages]=(double*)malloc(sizeof(**out_depenValue)*rows[pages]);
    for (i=0;i<rows[pages];i++)
      rowFlag[pages][i]=1;
    for (i=0;i<dataControls;i++) {
      if (data_control[i].hasdata) {
        data_control[i].files=rows[pages];
        if (!(data_control[i].file=(char**)SDDS_GetColumn(&SDDSin,data_control[i].fileColumn)))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (data_control[i].atCol) {
          if (!(data_control[i].colValue=SDDS_GetColumnInDoubles(&SDDSin,data_control[i].atCol)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
        if (!data_control[i].atCol)
          atValue=data_control[i].atValue;
        for (j=0;j<rows[pages];j++) {
          if (!SDDS_InitializeInput(&SDDSdata,data_control[i].file[j]))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          switch(SDDS_CheckColumn(&SDDSdata,data_control[i].interpCol,NULL,SDDS_ANY_NUMERIC_TYPE,NULL)) {
          case SDDS_CHECK_OKAY:
	    if (j == (rows[pages]-1)) {
	      if ( !pages ) {
                 if(!SDDS_TransferColumnDefinition(&SDDSout,&SDDSdata,data_control[i].interpCol,data_control[i].interpCol))                
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
              }
	    }  
            break;
          default:
            fprintf(stderr, "Something wrong with column %s or it does not exist in file %s.\n",
                    data_control[i].interpCol,data_control[i].file[j]);
            exit(1);
            break;
          }
          switch(SDDS_CheckColumn(&SDDSdata,data_control[i].funcOfCol,NULL,SDDS_ANY_NUMERIC_TYPE,NULL)) {
          case SDDS_CHECK_OKAY:
	    if ( j == (rows[pages]-1) ) {
	      if ((!pages) && !(data_control[i].atCol) ) {
                 if (!SDDS_TransferColumnDefinition(&SDDSout,&SDDSdata,data_control[i].funcOfCol,data_control[i].funcOfCol))
                   SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
               }
            }
	    break;
          default:
            fprintf( stderr, "Something wrong with column %s or it does not exist in file %s.\n",
                    data_control[i].funcOfCol,data_control[i].file[j]);
            exit(1);
            break;
          }
          if (SDDS_ReadPage(&SDDSdata)<=0)
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          datarows=SDDS_CountRowsOfInterest(&SDDSdata);
          if (!(indepValue=SDDS_GetColumnInDoubles(&SDDSdata,data_control[i].funcOfCol)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (!(depenValue=SDDS_GetColumnInDoubles(&SDDSdata,data_control[i].interpCol)))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (!SDDS_Terminate(&SDDSdata))
            SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          if (!(monotonicity=checkMonotonicity(indepValue, datarows))) {
            fprintf(stderr,"independent (%s) data values in file %s do not change monotonically\n",
                    data_control[i].funcOfCol, data_control[i].file[j]);
            exit(1);
          }
          if (data_control[i].atCol)
            atValue=data_control[i].colValue[j];
          out_depenValue[pages][j]=interpolate(depenValue,indepValue,datarows,atValue,&belowRange,
                                               &aboveRange,order,&interpCode, monotonicity);
          if (interpCode) {
            if (interpCode&OUTRANGE_ABORT) {
              fprintf(stderr, "error: value %e is out of range for column %s\n",
                      atValue,data_control[i].interpCol);
              exit(1);
            }
            if (interpCode&OUTRANGE_WARN)
              fprintf(stderr, "warning: value %e is out of range for column %s\n",
                      atValue,data_control[i].interpCol);
            if (interpCode&OUTRANGE_SKIP) 
              rowFlag[pages][j] = 0;
          }
          free(depenValue);
          free(indepValue);
        }
      }
      for (j=0;j<data_control[i].files;j++) 
        free(data_control[i].file[j]);
      free(data_control[i].file);
      data_control[i].file=NULL;
      if (data_control[i].colValue) {
        free(data_control[i].colValue);
        data_control[i].colValue=NULL;
      }
    }
    if (!pages) {
      if (!SDDS_WriteLayout(&SDDSout))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_StartTable(&SDDSout,rows[pages])) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if(!SDDS_CopyColumns(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if(!SDDS_CopyParameters(&SDDSout, &SDDSin))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    for (i=0;i<dataControls;i++) {
      if (data_control[i].hasdata) {
        if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME,out_depenValue[pages],
                                       rows[pages],data_control[i].interpCol))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        if (!(data_control[i].atCol)) {
          for (row=0;row<rows[pages];row++) {
            if (!SDDS_SetRowValues(&SDDSout, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, row,
                                   data_control[i].funcOfCol,data_control[i].atValue,NULL))
              SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        }
      }
    }
    if (!SDDS_AssertRowFlags(&SDDSout, SDDS_FLAG_ARRAY, rowFlag[pages], rows[pages]))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_WritePage(&SDDSout))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    pages++;
  }
  if (!SDDS_Terminate(&SDDSin))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  freedatacontrol(data_control,dataControls);
  if (out_depenValue) {
    for (i=0;i<pages;i++) {
      free(out_depenValue[i]);
      free(rowFlag[i]);
    }
    free(out_depenValue);
    free(rowFlag);
  }
  if (interpCol) free(interpCol);
  if (funcOf) free(funcOf);
  if (rows) free(rows);
  free_scanargs(&s_arg,argc);
  return 0;
}


long checkMonotonicity(double *indepValue, long rows)
{

  if (rows==1)
    return 1;
  if (indepValue[rows-1]>indepValue[0]) {
    while (--rows>0)
      if (indepValue[rows]<indepValue[rows-1])
        return 0;
    return 1;
  }
  else {
    while (--rows>0)
      if (indepValue[rows]>indepValue[rows-1])
        return 0;
    return -1;
  }
}


void freedatacontrol(DATA_CONTROL *data_control, long dataControls)
{
  long i;
  for (i=0;i<dataControls;i++) {
    free(data_control[i].interpCol);
    free(data_control[i].funcOfCol);
    free(data_control[i].fileColumn);
    if (data_control[i].atCol) 
      free(data_control[i].atCol);
  }
  free(data_control);
}
