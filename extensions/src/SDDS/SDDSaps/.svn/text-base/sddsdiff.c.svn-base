/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/* program: sddsdiff
 * purpose: compare two sdds file.

 $Log: not supported by cvs2svn $
 Revision 1.26  2008/02/22 22:06:18  shang
 now it ignores the pre and post spaces when compare two strings.

 Revision 1.25  2007/10/05 19:11:53  shang
 modified to accept SDDS_ULONG and SDDS_USHORT type

 Revision 1.24  2007/04/02 19:00:07  soliday
 Removed call to isnan and isinf for integer numbers.

 Revision 1.23  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.22  2006/09/06 16:00:43  soliday
 Modified so that it uses the list of valid types from SDDS_type_name which
 comes from SDDSlib instead of a local copy which no longer matches SDDS_type_name

 Revision 1.21  2006/08/08 17:04:04  emery
 Removed unused variables to remove compilation warnings.

 Revision 1.20  2006/06/20 21:09:16  soliday
 Fixed a small sprintf bug.

 Revision 1.19  2006/05/02 18:16:12  soliday
 Fixed problems on WIN32.

 Revision 1.18  2006/04/05 18:22:27  shang
 added noComparison to rowlabel option to skip the comparison of row label column if it is
 specified. And the rowlabel column will not be compared if it only exists in one file no matter
 whether nocomparison is specified.

 Revision 1.17  2006/04/05 15:51:17  shang
 remvoed secondFile from rowlabel option, instead, program will check which file has rowlabel
 column automatically. If none of the input files has rowlabel column, a warning message will be
 printed out and use number of rows for labeling.

 Revision 1.16  2006/04/04 21:58:20  shang
 added -rowlabel=elementName[,secondFile] option to printout the row by label.
 The rowlabel column can any type of data.

 Revision 1.15  2006/03/24 21:20:15  shang
 fixed bug in printing out message for parameter difference that only the first different parameter name was printed out.

 Revision 1.14  2006/03/17 15:50:52  shang
 recommit because the previous statement was wrong. This version adds -absolute option
 to compare the different to absolute values.

 Revision 1.13  2006/03/17 15:47:34  shang
 This comment is actually for sddsrowstats.
 added positionColumn option to minimum, maximum, smallest and largest option to
 write the column name that has the option specified value in each row to the
 output file in the column name provided by positionColumn.

 Revision 1.12  2006/03/15 20:22:28  shang
 added -exact for exact comparison and -format for providing the print format for
  double, float and string data; fixed the bug that parameters or arrays are not compared if
 the page is empty (no rows).

 Revision 1.11  2005/11/29 18:34:33  shang
 replaced get_names by getMatchingSDDSNames

 Revision 1.10  2005/11/29 18:28:27  shang
 added precision option to compare given number of digits for floating point values.

 Revision 1.9  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.8  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.7  2005/11/01 15:15:27  shang
 added -tolerance option and modified to accept NaN and Inf as numerical values.

 Revision 1.6  2005/09/30 18:23:41  shang
 removed linux warnings.

 Revision 1.5  2005/09/30 18:04:34  shang
 modified to compare data with different data types and added displaying of
 the difference, added -columns, -parameters and -arrays option to compare
 provided column[s], parameter[s] and/or array[s].

 Revision 1.4  2005/04/28 18:34:50  shang
 fixed a bug in compare definitions

 Revision 1.3  2005/04/25 21:30:03  shang
 fixed bug in getting parameter infomation

 Revision 1.2  2005/04/25 21:13:23  shang
 fixed bug in compare definitons function and added compareCommon option to compare
 the common columns, parameters and arrays in two files.

 Revision 1.1  2005/03/16 16:30:42  shang
 first version for comparing two sdds files.
 
 */

#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "SDDSutils.h"
#include <float.h>

#define CLO_COMPARECOMMON 0
#define CLO_COLUMNS 1
#define CLO_PARAMETERS 2
#define CLO_ARRAYS 3
#define CLO_TOLERANCE 4
#define CLO_PRECISION 5
#define CLO_FORMAT 6
#define CLO_EXACT 7
#define CLO_ABSOLUTE 8
#define CLO_ROWLABEL 9
#define N_OPTIONS 10

char *option[N_OPTIONS] = {"compareCommon", "columns", "parameters", 
                             "arrays", "tolerance", "precision", "format", "exact", "absolute", "rowlabel"} ;

char *USAGE1 = "sddsdiff <file1> <file2> [-compareCommon[=column|parameter|array]] [-columns=<col1>[,<col2>...]]\n\
   [-parameters=<par1][,<par2>...]] [-arrays=<array1>[,<array2>...]] [-tolerance=<value>] \n\
   [-precision=<integer> [-format=float=<string>|double=<string>|string=<string>] [-exact] \n\
   [-absolute] [-rowlabel=<column-name>[,nocomparison]] \n\
sddsdiff compare two SDDS file.\n\
tolerance            if given and greater than zero, \n\
                     two numerical values will be considered to be the same \n\
                     if their absolute difference is within the tolerance. \n\
                     if given and less than zero, \n\
                     two numerical values will be considered to be the same \n\
                     only if they equal to each other. \n\
precision            if given, the number of digits compared for floating points values. \n\
exact                if given, two values will be considered to be the same only if they are.\n\
                     only one of tolerance, precision, and exact should be given.\n";
char *USAGE2 = "absolute             if given, the absolute of the values are compared, i.e., the signs are ignored.\n\
format               provides the print format for float, double and/or string data. \n\
                     the default print format for float, double and string data are \n\
                     %25.8e %25.16e and %25s respectively. \n\
rowlabel             specifies a column that will be printed out to label the rows. \n\
                     The rowlabel column has to exist in one of the input files. Otherwise, \n\
                     the rows are labeled by the row number (by default). \n\
                     if nocomparison is provided, then this rowlabel column  will be used as \n\
                     labeling only and will not be compared. \n\
compareCommon        if given, only compares the common columns, parameters and arrays.\n\
                     if compareCommon=column, only compares the common columns, and ect. \n\
columns              if given, compares the given columns. \n\
parameters           if given, compares the given parameters. \n\
arrays               if given, compares the given arrays. \n\
columns, parameters and arrays option overwrites compareCommon option, i.e. \n\
only given columns, parameters and arrays in two file will be compared. \n\
Program by Hairong Shang. (This is version 1.0, March 2005)\n";

#define SDDS_COLUMN_TYPE 0
#define SDDS_PARAMETER_TYPE 1
#define SDDS_ARRAY_TYPE 2
/*type=0,1,2, for Column, Parameter, Array*/
long CompareDefinitions(SDDS_DATASET *dataset1, SDDS_DATASET *dataset2, char *file1, char *file2, long *names, char ***name, int32_t **dataType, long type, long compareCommon, char *rowLabelColumn, long notCompareRowLabelCol);
long CompareData(SDDS_DATASET *dataset1, SDDS_DATASET *dataset2, char *file1, char *file2, long names, char **name, int32_t *dataType, long type, long page, double tolerance, double precisionTolerance, char *floatFormat, char *doubleFormat, char *stringFormat, long absolute, void *rowLabel, long rowLabelType, char *labelName);
long compare_two_data(void *data1, void *data2, long index, long datatype, 
                      long first, long flags, char *name, 
                      long page, double tolerance, double precisionTolerance,
                      char *floatFormat, char *doubleFormat, char *stringFormat, 
                      char *longFormat, char *ulongFormat, char *shortFormat, char *ushortFormat, char *charFormat, 
                      long absolute, long parameter, char *labelName);
void printTitle(long flags, char *name, long page, long absolute, char *labelName);

#define COMPARE_COMMON_COLUMN 0x0001U
#define COMPARE_COMMON_PARAMETER 0x0002U
#define COMPARE_COMMON_ARRAY 0x0004U

int main(int argc, char **argv)
{
  SCANNED_ARG *s_arg;
  SDDS_DATASET table1, table2;
  char *file1, *file2;
  long different=0, columns, parameters, arrays, i, pages1,pages2, rows1, rows2, pagediff=0, i_arg, columnMatches, parameterMatches, arrayMatches, absolute=0;
  int32_t *columnDataType, *parDataType, *arrayDataType;
  char **columnName, **parameterName, **arrayName, **columnMatch, **parameterMatch, **arrayMatch;
  long column_provided, parameter_provided, array_provided, precision, labelFromSecondFile=0, rowLabelType=0, rowLabelIndex=-1, notCompareRowLabel=0;
  double tolerance=0.0, precisionTolerance=0.0;
  unsigned long compareCommonFlags=0, dummyFlags=0;
  char *floatFormat, *doubleFormat, *stringFormat, *rowLabelColumn;
  void *rowLabel;
  
  rowLabelColumn = NULL;
  rowLabel = NULL;
  floatFormat = doubleFormat = stringFormat = NULL;  
  precision = 0;
  columnDataType=parDataType=arrayDataType=NULL;
  columnName=parameterName=arrayName=NULL;
  columnMatch=parameterMatch=arrayMatch=NULL;
  columnMatches=parameterMatches=arrayMatches=0;
  columns=parameters=arrays=0;
  file1=file2=NULL;
  pages1=pages2=0;
  column_provided=parameter_provided=array_provided=0;
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s%s", USAGE1, USAGE2);
    exit(1);
  }
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case CLO_ROWLABEL:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -rowLabel syntax");
        rowLabelColumn = s_arg[i_arg].list[1];
        if (s_arg[i_arg].n_items>2 && strncmp_case_insensitive("nocomparison", s_arg[i_arg].list[2], strlen(s_arg[i_arg].list[2]))==0)
          notCompareRowLabel = 1;
        break;
      case CLO_EXACT:
        tolerance = -1.0;
        break;
      case CLO_ABSOLUTE:
        absolute = 1;
        break;
      case CLO_TOLERANCE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -tolerance syntax");
        if (!get_double(&tolerance, s_arg[i_arg].list[1]))
          SDDS_Bomb("invalid -tolerance syntax (not a number given)");
        break;
      case CLO_PRECISION:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -precision syntax");
        if (!get_long(&precision, s_arg[i_arg].list[1]))
          SDDS_Bomb("invalid -precision syntax (not a number given)");
        if (precision<0)
          precision=0;
        break;
      case CLO_FORMAT:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("Invalid -format syntax.");
        s_arg[i_arg].n_items--;
        if (!scanItemList(&dummyFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                          "float", SDDS_STRING, &floatFormat, 1, 0,
                          "double", SDDS_STRING, &doubleFormat, 1, 0,  
                          "array", SDDS_STRING, &stringFormat, 1, 0,
                          NULL))
          SDDS_Bomb("Invalid -format syntax");
        s_arg[i_arg].n_items++;
        if (floatFormat && !SDDS_VerifyPrintfFormat(floatFormat, SDDS_FLOAT)) {
          fprintf(stderr, "Error: given print format (\"%s\") for float data is invalid.\n", floatFormat);
          exit(1);
        }
        if (doubleFormat && !SDDS_VerifyPrintfFormat(doubleFormat, SDDS_DOUBLE)) {
          fprintf(stderr, "Error: given print format (\"%s\") for double data is invalid.\n", doubleFormat);
          exit(1);
        }
        if (stringFormat && !SDDS_VerifyPrintfFormat(stringFormat, SDDS_STRING)) {
          fprintf(stderr, "Error: given print format (\"%s\") for string data is invalid.\n", stringFormat);
          exit(1);
        }
        break;
      case CLO_COMPARECOMMON:
        if (s_arg[i_arg].n_items==1)
          compareCommonFlags |= COMPARE_COMMON_COLUMN | COMPARE_COMMON_PARAMETER | COMPARE_COMMON_ARRAY;
        
        else {
          s_arg[i_arg].n_items--;
          if (!scanItemList(&compareCommonFlags, s_arg[i_arg].list+1, &s_arg[i_arg].n_items, 0,
                            "column", -1, NULL, 0,  COMPARE_COMMON_COLUMN,
                            "parameter", -1, NULL, 0, COMPARE_COMMON_PARAMETER,
                            "array", -1, NULL, 0,  COMPARE_COMMON_ARRAY,
                            NULL))
            SDDS_Bomb("Invalid -compareCommon syntax");
          s_arg[i_arg].n_items++;
        }
        break;
      case CLO_COLUMNS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -columns syntax");
        columnMatch = tmalloc(sizeof(*columnMatch)*(columnMatches = s_arg[i_arg].n_items-1));
        for (i=0; i<columnMatches; i++)
          columnMatch[i] = s_arg[i_arg].list[i+1];
        column_provided=1;
        break;
      case CLO_PARAMETERS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -parameters syntax");
        parameterMatch = tmalloc(sizeof(*parameterMatch)*(parameterMatches = s_arg[i_arg].n_items-1));
        for (i=0; i<parameterMatches; i++)
          parameterMatch[i] = s_arg[i_arg].list[i+1];
        parameter_provided=1;
        break;
      case CLO_ARRAYS:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -arrays syntax");
        arrayMatch = tmalloc(sizeof(*arrayMatch)*(arrayMatches = s_arg[i_arg].n_items-1));
        for (i=0; i<arrayMatches; i++)
          arrayMatch[i] = s_arg[i_arg].list[i+1];
        array_provided=1;
        break;
      default:
        fprintf(stderr,"unknow option given (sddsdiff): %s\n",s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
    } else {
      if (!file1) 
        file1=s_arg[i_arg].list[0];
      else if (!file2)
        file2=s_arg[i_arg].list[0];
      else 
        SDDS_Bomb("Too many files given.");
    }
  }
  if (!floatFormat)
    SDDS_CopyString(&floatFormat, "%25.8e");
  if (!doubleFormat)
    SDDS_CopyString(&doubleFormat, "%25.16e");
  if (!stringFormat) 
    SDDS_CopyString(&stringFormat, "%25s");
  
  if (tolerance && precision>0) {
    SDDS_Bomb("Tolerance and precision option are not compatible, only one of tolerance, precision and exact may be given.");
  }
  if (!file1 || !file2) {
    fprintf(stderr,"Two files must be provided for comparision.\n");
    exit(1);
  }
  if (strcmp(file1,file2)==0) {
    printf("%s and %s are identical.\n", file1, file2);
    return 0;
  }
  if (!SDDS_InitializeInput(&table1, file1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!SDDS_InitializeInput(&table2, file2)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (rowLabelColumn) { 
    if ((rowLabelIndex = SDDS_GetColumnIndex(&table1, rowLabelColumn))<0) {
      if ((rowLabelIndex =SDDS_GetColumnIndex(&table2, rowLabelColumn))<0) {
        fprintf(stdout,"Warning: row label column %s does not exist in the input files. The number of rows will be labeled instead.\n", rowLabelColumn);
        rowLabelColumn = NULL;
      } else {
        labelFromSecondFile = 1;
        notCompareRowLabel = 1;
      }
    } else {
      if (SDDS_GetColumnIndex(&table2, rowLabelColumn)<0)
        notCompareRowLabel = 1;
    }
    
    if (rowLabelColumn) {
      if (labelFromSecondFile)
        rowLabelType = SDDS_GetColumnType(&table2, rowLabelIndex);
      else
        rowLabelType = SDDS_GetColumnType(&table1, rowLabelIndex);
    }
  }
  if (!precision)
    precision = fabs(log10(DBL_EPSILON));
  precisionTolerance = pow(10, -precision);
  if (column_provided) {
    columnName = getMatchingSDDSNames(&table1, columnMatch, columnMatches, &columns,
                                      SDDS_MATCH_COLUMN);
    if (CompareDefinitions(&table1,&table2,file1,file2,&columns,&columnName,
                           &columnDataType,SDDS_COLUMN_TYPE, 
                           compareCommonFlags&COMPARE_COMMON_COLUMN, rowLabelColumn, notCompareRowLabel))
      different=1;
  }
  if (parameter_provided) {
    parameterName = getMatchingSDDSNames(&table1, parameterMatch, parameterMatches, &parameters,
                                         SDDS_MATCH_PARAMETER);
    
    if (CompareDefinitions(&table1,&table2,file1,file2,&parameters,&parameterName,
                           &parDataType,SDDS_PARAMETER_TYPE, 
                           compareCommonFlags&COMPARE_COMMON_PARAMETER, NULL, 1))
      different=1;
  }                      
  if (array_provided) {
    arrayName = getMatchingSDDSNames(&table1, arrayMatch, arrayMatches, &arrays,
                          SDDS_MATCH_ARRAY);
    if (CompareDefinitions(&table1,&table2,file1,file2,&arrays,
                           &arrayName,&arrayDataType,SDDS_ARRAY_TYPE, 
                           compareCommonFlags&COMPARE_COMMON_ARRAY, NULL, 1))
      different=1;
  }
  if (!columns && !parameters && !arrays) {
    if (!compareCommonFlags || compareCommonFlags&COMPARE_COMMON_COLUMN)
      different += CompareDefinitions(&table1,&table2,file1,file2,&columns,&columnName,
                                      &columnDataType,SDDS_COLUMN_TYPE,
                                      compareCommonFlags&COMPARE_COMMON_COLUMN, rowLabelColumn, notCompareRowLabel);
    if (!compareCommonFlags || compareCommonFlags&COMPARE_COMMON_PARAMETER)
      different += CompareDefinitions(&table1,&table2,file1,file2,&parameters,&parameterName,
                                      &parDataType,SDDS_PARAMETER_TYPE,
                                      compareCommonFlags&COMPARE_COMMON_PARAMETER, NULL, 1);
    if (!compareCommonFlags || compareCommonFlags&COMPARE_COMMON_ARRAY)
      different += CompareDefinitions(&table1,&table2,file1,file2,&arrays,
                                      &arrayName,&arrayDataType,SDDS_ARRAY_TYPE, 
                                      compareCommonFlags&COMPARE_COMMON_ARRAY, NULL, 1);
  }  
  if (!different) {
    if (!columns && !parameters && !arrays) {
      fprintf(stderr,"There are no common columns, parameters or arrays in two files.\n");
      different=1;
    } else {
      /*definitions are the same, now compare the data */
      while (1) {
        pagediff=0;
        pages1=SDDS_ReadPage(&table1);
        pages2=SDDS_ReadPage(&table2); 
        if (pages1>0 && pages2>0) {
          /*compare data */
          rows1=SDDS_CountRowsOfInterest(&table1);
          rows2=SDDS_CountRowsOfInterest(&table2);
          if (rows1!=rows2) {
            pagediff=1;
            different=1;
            fprintf(stderr,"Two files have different rows in page %ld, %s has %ld rows, while %s has %ld rows.\n",
                    pages1, file1, rows1, file2, rows2);
            break;
          } else {
            if (parameters) 
              pagediff +=CompareData(&table1,&table2,file1,file2,parameters,parameterName,parDataType,SDDS_PARAMETER_TYPE, pages1, tolerance, precisionTolerance, floatFormat, doubleFormat, stringFormat, absolute, NULL, rowLabelType, NULL);
            if (columns && rows1) {
              if (rowLabelColumn) {
                if (labelFromSecondFile) {
                  if (!(rowLabel=SDDS_GetColumn(&table2, rowLabelColumn))) 
                    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors); 
                } else { 
                  if (!(rowLabel=SDDS_GetColumn(&table1, rowLabelColumn))) 
                    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors); 
                }
              }
              pagediff +=CompareData(&table1,&table2,file1,file2,columns,columnName,columnDataType,SDDS_COLUMN_TYPE,pages1, tolerance, precisionTolerance, floatFormat, doubleFormat, stringFormat, absolute, rowLabel, rowLabelType, rowLabelColumn);
              if (rowLabelColumn) {
                if (rowLabelType==SDDS_STRING)
                  SDDS_FreeStringArray((char**)rowLabel, rows1);
                else 
                  free(rowLabel);
                rowLabel = NULL;
              }
            }
            if (arrays)
              pagediff +=CompareData(&table1,&table2,file1,file2,arrays,arrayName,arrayDataType,SDDS_ARRAY_TYPE,pages1, tolerance, precisionTolerance, floatFormat, doubleFormat, stringFormat, absolute, NULL, rowLabelType, NULL);
            different +=pagediff;
          }
          /*if (!pagediff)
            printf("%s and %s are same at page %ld\n",file1, file2, pages1);*/
        } else if (pages1>0 && pages2<=0) {
          fprintf(stderr,"%s has less pages than %s.\n",file2,file1);
          different=1;
          break;
        } else if (pages1<0 && pages2>0) {
          different=1;
          fprintf(stderr,"%s has less pages than %s.\n",file1,file2);
          break;
        } else {
          break;
        }
      }
    }
  } else {
    different=1;
  }
  if (!different) 
    printf("%s and %s are identical.\n",file1,file2);
  else 
    fprintf(stderr, "%s and %s are different.\n", file1, file2);
  if (columns) {
    for (i=0;i<columns;i++)
      free(columnName[i]);
    free(columnName);
    free(columnDataType);
    free(columnMatch);
  }
  if (parameters) {
    for (i=0;i<parameters;i++)
      free(parameterName[i]);
    free(parameterName);
    free(parDataType);
    free(parameterMatch);
  }
  if (arrays) {
    for (i=0;i<arrays;i++)
      free(arrayName[i]);
    free(arrayName);
    free(arrayDataType);
    free(arrayMatch);
  }
  free_scanargs(&s_arg,argc);
  free(stringFormat);
  free(floatFormat);
  free(doubleFormat);
  
  if (!SDDS_Terminate(&table1) || !SDDS_Terminate(&table2)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return 0;
}

long CompareDefinitions(SDDS_DATASET *dataset1, SDDS_DATASET *dataset2, char *file1, char *file2, long *names, char ***name, int32_t **dataType, long type, long compareCommon, char *rowLabelColumn, long notCompareRowLabel)
{
  long same_items, sames, free_same_name=0;
  int32_t *same, *datatype;
  int32_t names1, names2;
  char **name1, **name2, *def, **same_name;
  long type1, type2, i, returnValue=0, first=1, match, index1, index2;
  char *units1, *units2;
  units1=units2=NULL;
  name1=name2=same_name=NULL;
  names1=names2=0;
  def=NULL;
  match=1;
  same_items=0;
  same=datatype=NULL;
  
  type1=type2=-1;
  
  switch (type) {
  case SDDS_COLUMN_TYPE:
    name1=SDDS_GetColumnNames(dataset1,&names1);
    name2=SDDS_GetColumnNames(dataset2,&names2);
    SDDS_CopyString(&def,"column");
    break;
  case SDDS_PARAMETER_TYPE:
    name1=SDDS_GetParameterNames(dataset1,&names1);
    name2=SDDS_GetParameterNames(dataset2,&names2);
    SDDS_CopyString(&def,"parameter");
    break;
  case SDDS_ARRAY_TYPE:
    name1=SDDS_GetArrayNames(dataset1,&names1);
    name2=SDDS_GetArrayNames(dataset2,&names2);
    SDDS_CopyString(&def,"array");
    break;
  default:
    fprintf(stderr,"Unknown type given for CompareDefinitions().\n");
    return 1;
  }
  if (names1==0 && names2==0)
    return 0;
  if (*names) {
    if (!names1 || !names2) {
      fprintf(stderr, "One of the files does not have any %s\n", def);
      returnValue=1;
    }
    if (!returnValue) {
      for (i=0;i<*names;i++) {
        if (rowLabelColumn && notCompareRowLabel && strcmp((*name)[i], rowLabelColumn)==0)
          continue;
        if (-1==match_string((*name)[i],name1,names1,EXACT_MATCH)) {
          fprintf(stderr,"file %s does not have %s %s\n", file1, (*name)[i], def);
          returnValue=1;
          break;
        }
        if (-1==match_string((*name)[i],name2,names2,EXACT_MATCH)) {
          fprintf(stderr,"file %s does not have %s %s\n", file2, (*name)[i], def);
          returnValue=1;
          break;
        }
      }
    }
    if (returnValue) {
      for (i=0;i<names1;i++)
        free(name1[i]);
      free(name1);
      for (i=0;i<names2;i++)
        free(name2[i]);
      free(name2);
      free(def);
      return returnValue;
    }
    same_items=*names;
    same_name = *name;
    compareCommon=0;
  } else {
    if (!names1 && !names2) {
      free(def);
      return 0;
    }
    if (compareCommon && (!names1 || !names2)) {
      if (names1) {
        for (i=0;i<names1;i++)
          free(name1[i]);
        free(name1);
      }
      if (names2) {
        for (i=0;i<names2;i++)
          free(name2[i]);
        free(name2);
      }
      *names=0;
      return 0;
    }
    if (names1 != names2 && !compareCommon && !notCompareRowLabel)
      returnValue=1;
    if (returnValue) {
      fprintf(stderr,"Two files have different %ss:\n    %s has %" PRId32 " %ss while %s has %" PRId32 " %ss.\n", 
              def, file1, names1, def, file2, names2, def);
      for (i=0;i<names2;i++)
        free(name2[i]);
      free(name2);
      for (i=0;i<names1;i++)
        free(name1[i]);
      free(name1);
      free(def);
      return returnValue;
    }
    same_items=0;
    for (i=0;i<names1;i++) {
      if (rowLabelColumn && notCompareRowLabel && strcmp(rowLabelColumn, name1[i])==0)
        continue;
      if (-1==match_string(name1[i],name2,names2,EXACT_MATCH)) {
        if (!compareCommon) {
          if (first) {
            fprintf(stderr,"    Following %ss of %s are not in %s.\n", def, file1, file2);
            first=0;
          }
          fprintf(stderr,"    %s",name1[i]);
          returnValue++;
          match=0;
        }
      } else {
        same_name = (char**)SDDS_Realloc(same_name, sizeof(*same_name)*(same_items+1));
        same_name[same_items]=name1[i];
        same_items++;
      }
    }
    if (!compareCommon) {
      if (!first) 
        fprintf(stderr,"\n");
      first=1;
      for (i=0;i<names2;i++) {
        if (rowLabelColumn && notCompareRowLabel && strcmp(rowLabelColumn, name2[i])==0)
          continue;
        if (-1==match_string(name2[i],name1,names1,EXACT_MATCH)) {
          if (first) {
            fprintf(stderr,"\n    Following %ss of %s are not in %s.\n", def, file2, file1);
            first=0;
          }
          fprintf(stderr,"    %s",name2[i]);
          match=0;
          returnValue++;
        }
      }
      if (!first) 
        fprintf(stderr,"\n");
    }
    if (same_items)
      free_same_name=1;
  }
  sames = same_items;
  if (same_items) {
    datatype = (int32_t*)malloc(sizeof(datatype)*same_items);
    same = (int32_t*)malloc(sizeof(*same)*same_items);
    /*check the units and type*/
    for (i=0;i<same_items;i++) {
      same[i]=1;
      switch (type) {
      case SDDS_COLUMN_TYPE:
        index1=SDDS_GetColumnIndex(dataset1,same_name[i]);
        index2=SDDS_GetColumnIndex(dataset2,same_name[i]);
        type1=SDDS_GetColumnType(dataset1, index1);
        type2=SDDS_GetColumnType(dataset2, index2);
        if (SDDS_GetColumnInformation(dataset1,"units",&units1,SDDS_GET_BY_INDEX, 
                                      index1)!=SDDS_STRING) {
          SDDS_SetError("units field of column has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if (SDDS_GetColumnInformation(dataset2,"units",&units2,SDDS_GET_BY_INDEX, 
                                      index2)!=SDDS_STRING) {
          SDDS_SetError("units field of column has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        break;
      case SDDS_PARAMETER_TYPE:
        index1=SDDS_GetParameterIndex(dataset1,same_name[i]);
        index2=SDDS_GetParameterIndex(dataset2,same_name[i]);
        type1=SDDS_GetParameterType(dataset1, index1);
        type2=SDDS_GetParameterType(dataset2, index2);
        if (SDDS_GetParameterInformation(dataset1,"units",&units1,SDDS_GET_BY_INDEX, 
                                         index1)!=SDDS_STRING) {
          SDDS_SetError("units field of parameter has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if (SDDS_GetParameterInformation(dataset2,"units",&units2,SDDS_GET_BY_INDEX, 
                                         index2)!=SDDS_STRING) {
          SDDS_SetError("units field of parameter has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        break;
      case SDDS_ARRAY_TYPE:
        index1=SDDS_GetArrayIndex(dataset1, same_name[i]);
        index2=SDDS_GetArrayIndex(dataset2, same_name[i]);
        type1=SDDS_GetArrayType(dataset1, index1);
        type2=SDDS_GetArrayType(dataset2, index2);
        if (SDDS_GetArrayInformation(dataset1,"units",&units1,SDDS_GET_BY_INDEX, 
                                     index1)!=SDDS_STRING) {
          SDDS_SetError("units field of array has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        if (SDDS_GetArrayInformation(dataset2,"units",&units2,SDDS_GET_BY_INDEX, 
                                     index2)!=SDDS_STRING) {
          SDDS_SetError("units field of array has wrong data type!");
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          exit(1);
        }
        break;
      }
      datatype[i]=type1;
      if (type1!=type2) {
        if (first && !compareCommon) {
          fprintf(stderr,"The type of following %ss do not match in two files.\n", def);
          fprintf(stderr,"%20s\t%20s\t%20s\n","name",file1,file2);
          first=0;
        }
        if (!compareCommon) {
          fprintf(stderr,"%20s\t%20s\t%20s\n",same_name[i],SDDS_type_name[type1-1],SDDS_type_name[type2-1]);
          returnValue ++;
        }
        sames --;
        same[i]=0;
      } else if ((units1 && units2 && strcasecmp(units1,units2)!=0) ||
                 (units1 && !units2) || (!units1 && units2) ) {
        if (first && !compareCommon) {
          fprintf(stderr,"The units of following %ss do not match in two files.\n", def);
          fprintf(stderr,"%20s\t%20s\t%20s\n","name",file1,file2);
          first=0;
        }
        if (!compareCommon) {
          if (units1 && units2)
            fprintf(stderr,"%20s\t%20s\t%20s\n", same_name[i],units1,units2);
          else if (units1)
            fprintf(stderr,"%20s\t%20s\t%20s\n", same_name[i],units1,"   ");
          else if (units2)
            fprintf(stderr,"%20s\t%20s\t%20s\n", same_name[i],"   ",units2);
          returnValue ++;
        }
        sames--;
        same[i]=0;
      }
      free(units1);
      free(units2);
      units1=units2=NULL;
      if (returnValue && !compareCommon)
        break;
    }
    if (!compareCommon && returnValue) {
      sames=0;
    } 
  }
  if (sames) {
    if (!(*names)) {
      for (i=0;i<same_items;i++) {
        if (same[i]) {
          *name=(char**)SDDS_Realloc(*name, sizeof(**name)*(*names+1));
          *dataType=(int32_t*)SDDS_Realloc(*dataType, sizeof(**dataType)*(*names+1));
          SDDS_CopyString(*name+*names, same_name[i]);
          (*dataType)[*names]=datatype[i];
          (*names)++;
        }
      }
      free(datatype);
    } else
      *dataType=datatype;
  }
  
  for (i=0;i<names2;i++) free(name2[i]);
  free(name2);
  for (i=0;i<names1;i++) free(name1[i]);
  free(name1);
  if (same) free(same);
  if (free_same_name) free(same_name);
  free(def);
  return compareCommon?0:returnValue;
}

long CompareData(SDDS_DATASET *dataset1, SDDS_DATASET *dataset2, char *file1, char *file2,
                 long names, char **name, int32_t *dataType, long type, 
                 long page, double tolerance, double precisionTolerance,
                 char *floatFormat, char *doubleFormat, char *stringFormat, long absolute,
                 void *rowLabel, long rowLabelType, char *rowLabelColumn)
{
  long diff=0, rows, i, j, first=1, printLabel=0;
  char *str1, *str2, fFormat[256], dFormat[256], strFormat[256], lFormat[256], ulFormat[256], ushortFormat[256], shortFormat[256], cFormat[256], labelFormat[1024];
  SDDS_ARRAY *array1, *array2;
  void *data1, *data2, *rowLabel0;

  array1=array2=NULL;
  data1=data2=NULL;
  str1=str2=NULL;
  rowLabel0 = NULL;
  
  if (!rowLabel) {
    sprintf(fFormat, "%%20ld%s%s%s\n", floatFormat, floatFormat, floatFormat);
    sprintf(dFormat, "%%20ld%s%s%s\n", doubleFormat, doubleFormat, doubleFormat);
    sprintf(strFormat, "%%20ld%s%s%%25ld\n", stringFormat, stringFormat);
    sprintf(lFormat, "%s", "%20ld%25ld%25ld%25ld\n");
    sprintf(ulFormat, "%s", "%20ld%25lu%25lu%25lu\n");
    sprintf(shortFormat, "%s", "%20ld%25hd%25hd%25hd\n");
    sprintf(ushortFormat, "%s", "%20ld%25hu%25hu%25hu\n");
    sprintf(cFormat, "%s", "%20ld%25c%25c%25c\n");
  } else 
    printLabel = 1;
  switch (type) {
  case SDDS_COLUMN_TYPE:
    rows=SDDS_CountRowsOfInterest(dataset1);
    if (!rows)
      break;
    for (i=0;i<names;i++) {
      first=1;
      if (!(data1=SDDS_GetColumn(dataset1,name[i])) ||
          !(data2=SDDS_GetColumn(dataset2,name[i]))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      for (j=0;j<rows;j++) { 
        if (rowLabel) {
          switch (rowLabelType) {
          case SDDS_STRING:
            sprintf(labelFormat, "%20s", ((char**)rowLabel)[j]); 
            break;
          case SDDS_DOUBLE:
            sprintf(labelFormat, "%20.10e", ((double*)rowLabel)[j]);
            break;
          case SDDS_FLOAT:
            sprintf(labelFormat, "%20.5e", ((float*)rowLabel)[j]);
            break;
          case SDDS_ULONG:
            sprintf(labelFormat, "%20lu", ((long*)rowLabel)[j]);
            break;
          case SDDS_LONG:
            sprintf(labelFormat, "%20ld", ((long*)rowLabel)[j]);
            break;
          case SDDS_USHORT:
            sprintf(labelFormat, "%20hu", ((short*)rowLabel)[j]);
            break;
          case SDDS_SHORT:
            sprintf(labelFormat, "%20hd", ((short*)rowLabel)[j]);
            break;
          case SDDS_CHARACTER:
            sprintf(labelFormat, "%20c", ((char*)rowLabel)[j]);
            break;
          default:
            fprintf(stderr, "Unknown data type for rowlabel.\n");
            exit(1);
          }
          sprintf(fFormat, "%s%s%s%s\n", labelFormat, floatFormat, floatFormat, floatFormat);
          sprintf(dFormat, "%s%s%s%s\n", labelFormat, doubleFormat, doubleFormat, doubleFormat);
          sprintf(strFormat, "%s%s%s%%25ld\n", labelFormat, stringFormat, stringFormat);
          sprintf(lFormat, "%s%%25ld%%25ld%%25ld\n", labelFormat);
          sprintf(shortFormat, "%s%%25d%%25d%%25d\n", labelFormat);
          sprintf(cFormat, "%s%%25c%%25c%%25d\n", labelFormat);
        }
        if (compare_two_data(data1, data2, j, dataType[i], first, SDDS_COLUMN_TYPE, name[i], page, 
                             tolerance, precisionTolerance, 
                             fFormat, dFormat, strFormat, lFormat, ulFormat, shortFormat, ushortFormat, cFormat,
                             absolute, 0, rowLabelColumn)!=0) {
          diff++;
          if (first) 
            first=0;
        }
        if (dataType[i]==SDDS_STRING) {
          free((char*)((char**)data1)[j]);
          free((char*)((char**)data2)[j]);
        }
      }
      free((char**)data1);
      free((char**)data2);
      data1=data2=NULL;
    }
    break;
  case SDDS_PARAMETER_TYPE:
    for (i=0;i<names;i++) {
      first = 1;
      if (!(data1=SDDS_GetParameter(dataset1, name[i], NULL)) ||
          !(data2=SDDS_GetParameter(dataset2, name[i], NULL))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (compare_two_data(data1, data2, 0, dataType[i], first, SDDS_PARAMETER_TYPE, name[i],
                           page, tolerance, precisionTolerance,
                           fFormat, dFormat, strFormat, lFormat, ulFormat, shortFormat, ushortFormat, cFormat,
                           absolute, 1, NULL)!=0) {
        diff++;
        if (first) 
          first=0;
      }
      if (dataType[i]==SDDS_STRING) {
        free(*((char**)data1));
        free(*((char**)data2));
      }
      free(data1);
      free(data2);
      data1=data2=NULL;
    }
    break;
  case SDDS_ARRAY_TYPE:
    for (i=0;i<names;i++) {
      first=1;
      if (!(array1=SDDS_GetArray(dataset1,name[i], NULL)) ||
          !(array2=SDDS_GetArray(dataset2,name[i], NULL))) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
      if (array1->elements != array2->elements) {
        fprintf(stderr,"Array %s has %" PRId32 " elements in file1, but %" PRId32 " elements in file2.\n",
                name[i],array1->elements,array2->elements);
        diff++;
      } else {
        for (j=0;j<array1->elements;j++) {
          if (compare_two_data(array1->data, array2->data, j, dataType[i],
                               first, SDDS_ARRAY_TYPE, name[i], 
                               page, tolerance, precisionTolerance, 
                               fFormat, dFormat, strFormat, lFormat, ulFormat, shortFormat, ushortFormat, cFormat,
                               absolute, 0, NULL)!=0) {
            diff++;
            if (first) 
              first=0;
          }
        }
      }
      SDDS_FreeArray(array1);
      SDDS_FreeArray(array2);
      array1=array2=NULL;
    }
    break;
  }
  return diff;
}

void printTitle(long flags, char *name, long page, long absolute, char *labelName)
{
  char *type=NULL;
  char *element=NULL;
  
  switch (flags) {
  case SDDS_COLUMN_TYPE:
    SDDS_CopyString(&type, "column");
    if (labelName)
      SDDS_CopyString(&element, labelName);
    else
      SDDS_CopyString(&element, "row");
    break;
  case SDDS_PARAMETER_TYPE:
    SDDS_CopyString(&type, "parameter");
    SDDS_CopyString(&element, "page number");
    break;
  case SDDS_ARRAY_TYPE:
    SDDS_CopyString(&type, "array");
    SDDS_CopyString(&element, "element number");
    break;
  }
  if (type) {
    fprintf(stdout,"\nThe value of %s %s at page %ld are different in two files:\n",type, name,page);
    if (absolute)
      fprintf(stdout,"%20s%25s%25s%25s\n", element, "value in file1", "value in file2", "abs(file1)-abs(file2)");
    else
      fprintf(stdout,"%20s%25s%25s%25s\n", element, "value in file1", "value in file2", "file1-file2");
    free(type);
    free(element);
  }
}

long compare_two_data(void *data1, void *data2, long index, long datatype, 
                      long first, long flags, char *name, long page,
                      double tolerance, double precisionTolerance,
                      char *floatFormat, char *doubleFormat, char *stringFormat,
                      char *longFormat, char *ulongFormat, char *shortFormat, char *ushortFormat, char *charFormat,
                      long absolute,  long parameter, char *labelName)
{
  char *str1, *str2;
  double dval1, dval2, denominator, dabs1, dabs2, ddiff;
  float fval1, fval2, fabs1, fabs2, fdenominator, fdiff;
  int32_t lval1, lval2, labs1, labs2, ldiff;
  uint32_t ulval1, ulval2, uldiff;
  short sval1, sval2, sabs1, sabs2, sdiff;
  unsigned short usval1, usval2, usdiff;
  char cval1, cval2;
  long returnValue=0, printIndex;
  double tol;
  
  printIndex = index + 1;
  if (parameter)
    printIndex = page;
  
  if (tolerance<0)
    tol = 0;
  else
    tol = tolerance;
  
  switch (datatype) {
  case SDDS_STRING:
    str1=*((char**)data1+index);
    str2=*((char**)data2+index);
    returnValue=strcmp(trim_spaces(str1), trim_spaces(str2));
    if (returnValue!=0) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName) {
        fprintf(stdout, stringFormat, str1, str2,returnValue);
      } else  
        fprintf(stdout, stringFormat, printIndex, str1, str2,returnValue);
    }
    break;
  case SDDS_DOUBLE:
    dval1=*((double*)data1+index);
    dval2=*((double*)data2+index);
    if (absolute) {
      dabs1 = fabs(dval1);
      dabs2 = fabs(dval2);
    } else {
      dabs1 = dval1;
      dabs2 = dval2;
    }
    ddiff = dabs1-dabs2;
    if ((isnan(dval1)&& !isnan(dval2)) || (isinf(dval1) && !isinf(dval2))) 
      returnValue=1;
    else if (dabs1!=dabs2) {
      if (tolerance) { 
        if (fabs(ddiff)>tol)
          returnValue = 1;
      } else {
        if (dabs1==0 || dabs2==0) {
          if (fabs(dabs1-dabs2)>precisionTolerance)
            returnValue = 1;
        } else {
          dabs1 = fabs(dval1);
          dabs2 = fabs(dval2);
          denominator = (dabs1<dabs2)?dabs1:dabs2;
          if (fabs(dval1-dval2)/denominator>precisionTolerance)
            returnValue = 1;
        }
      }
    }
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, doubleFormat, dval1, dval2, ddiff);
      else  
        fprintf(stdout, doubleFormat, printIndex, dval1, dval2, ddiff);
    }
    break;
  case SDDS_FLOAT:
    fval1=*((float*)data1+index);
    fval2=*((float*)data2+index);
    if (absolute) {
      fabs1 = fabs(fval1);
      fabs2 = fabs(fval2);
    } else {
      fabs1 = fval1;
      fabs2 = fval2;
    }
    fdiff = fabs1 - fabs2;
    if ((isnan(fval1) && !isnan(fval2)) || (isinf(fval1) && !isinf(fval2))) 
      returnValue=1;
    else if (fabs1 != fabs2) {
      if (tolerance) {
        if (fabs(fdiff)>tol)
          returnValue = 1;
      } else {
        if (fabs1==0 || fabs2==0) {
          if (fabs(fabs1-fabs2)>precisionTolerance)
            returnValue = 1;
        } else {
          fabs1 = fabs(fval1);
          fabs2 = fabs(fval2);
          fdenominator = (fabs1<fabs2)?fabs1:fabs2;
          if (fabs(fval1-fval2)/fdenominator>precisionTolerance)
            returnValue = 1;
        }
      }
    }
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, floatFormat, fval1, fval2, fdiff);
      else  
        fprintf(stdout, floatFormat , printIndex, fval1, fval2, fdiff);
    }
    break;
  case SDDS_ULONG:
    ulval1=*((uint32_t*)data1+index);
    ulval2=*((uint32_t*)data2+index);
    uldiff = ulval1 - ulval2;
    if (labs(uldiff)>tol) 
      returnValue=1;
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, ulongFormat, ulval1, ulval2, uldiff);
      else
        fprintf(stdout, ulongFormat, printIndex, ulval1, ulval2, uldiff);
    }
    break;
  case SDDS_LONG:
    lval1=*((int32_t*)data1+index);
    lval2=*((int32_t*)data2+index);
    if (absolute) {
      labs1 = abs(lval1);
      labs2 = abs(lval2);
    } else {
      labs1 = lval1;
      labs2 = lval2;
    }
    ldiff = labs1 - labs2;
    if (labs(ldiff)>tol) 
      returnValue=1;
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, longFormat, lval1, lval2, ldiff);
      else
        fprintf(stdout, longFormat, printIndex, lval1, lval2, ldiff);
    }
    break;
  case SDDS_SHORT:
    sval1=*((short*)data1+index);
    sval2=*((short*)data2+index);
    if (absolute) {
      sabs1 = abs(sval1);
      sabs2 = abs(sval2);
    } else {
      sabs1 = sval1;
      sabs2 = sval2;
    }
    sdiff = sabs1 - sabs2;
    if (abs(sdiff)>tol) 
      returnValue=1;
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, shortFormat, sval1, sval2, sdiff);
      else
        fprintf(stdout, shortFormat, printIndex, sval1, sval2, sdiff);
    }
    break;
  case SDDS_USHORT:
    usval1=*((unsigned short*)data1+index);
    usval2=*((unsigned short*)data2+index);
    usdiff = usval1 - usval2;
    if (abs(usdiff)>tol) 
      returnValue=1;
    if (returnValue) {
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, ushortFormat, usval1, usval2, usdiff);
      else
        fprintf(stdout, ushortFormat, printIndex, usval1, usval2, usdiff);
    }
    break;
  case SDDS_CHARACTER:
    cval1=*((char*)data1+index);
    cval2=*((char*)data2+index);
    if (cval1!=cval2) {
      returnValue=1;
      if (first)
        printTitle(flags, name, page, absolute, labelName);
      if (labelName)
        fprintf(stdout, charFormat, cval1, cval2, cval1-cval2);
      else
        fprintf(stdout, charFormat, printIndex, cval1, cval2, cval1-cval2);
    }
    break;
  default:
    fprintf(stderr,"unknown data type %ld.\n",datatype);
    exit(1);
  }
  return returnValue;
}

 
