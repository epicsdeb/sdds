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
 Revision 1.8  2011/06/14 16:51:25  lemery
 Minor edit of usage message.

 Revision 1.7  2008/09/23 20:10:38  soliday
 Fixed a bug that caused problems when a file has zero rows.

 Revision 1.6  2008/09/22 21:22:05  soliday
 Fixed an variable initialization issue.

 Revision 1.5  2008/09/22 21:18:08  soliday
 Fixed problem that occured when multiple rows in the same file had the same
 Time. I didn't expect this to happen but it did.

 Revision 1.4  2008/09/19 21:06:52  soliday
 Sped up the code by 35 percent by avoiding calls to FindMatchingKeyGroup
 if it is fairly certain that the time values matchup throughout the files.
 It does this by checking that they have the same number of rows and that the
 time values are equal for the first few and last few rows.

 Revision 1.3  2008/09/18 20:18:26  soliday
 Modified to use new SDDS_GetColumnInFloats and SDDS_SetColumnFromFloats
 procedures to reduce the memory required when dealing with doubles instead
 of floats.

 Revision 1.2  2008/09/16 20:07:45  soliday
 Added units to the Time column.

 Revision 1.1  2008/09/15 21:34:42  soliday
 This tool is used to combine multiple log files in the one PV per file format
 into one larger file with all the PVs. Only the timestamps that exist for
 all the PVs are kept.

 *
 */
#include "mdb.h"
#include "scan.h"
#include "SDDS.h"

#define SET_PIPE 0
#define SET_OVERWRITE 1
#define N_OPTIONS 2
static char *option[N_OPTIONS] = {
  "pipe", "overwrite"
};

char *USAGE="sddscombinelogfiles [<SDDSinputfilelist>] [<SDDSoutputfile>]\n\
[-pipe=[output]] [-overwrite]\n\n\
This program is used to combine data logger output files that are in\n\
the one-PV-per-file format.\n";

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_input;
  SDDS_DATASET SDDS_output;
  SCANNED_ARG *s_arg;
  KEYED_EQUIVALENT **keyGroup=NULL;
  long keyGroups=0;
  char **inputfile=NULL;
  int inputfiles=0;
  char *outputfile=NULL;
  int i_arg, i, j, n, m, r, row, s, z;
  unsigned long pipeFlags=0;
  int overwrite=0;
  char **columnname;
  int32_t columnnames;
  int caerrorsIndex;
  int timeIndex;
  int dataIndex;
  double **timeValues=NULL;
  float **dataValues=NULL;
  short **flag=NULL;
  int32_t *rows=NULL;
  char **dataNames=NULL;
  char **uniqueDataName=NULL;
  int uniqueDataNames=0;
  int page=0;
  int pages;
  int found;
  double *outputTimeValues=NULL;
  float **outputDataValues=NULL;
  int allocated_rows=0;
  int **array=NULL;
  int *arrayCount;


  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) {
    fprintf(stderr, "%s", USAGE);
    return(1);
  }
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_OVERWRITE:
        overwrite = 1;
        break;
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags)) {
          fprintf(stderr, "invalid -pipe syntax\n");
          return(1);
        }
        if (pipeFlags&USE_STDIN) {
          fprintf(stderr, "-pipe=in not supported\n");
          return(1);
        }
        break;
      default:
        fprintf(stderr, "unrecognized option\n%s", USAGE);
        return(1);
      }
    } else {
      inputfile = trealloc(inputfile, sizeof(*inputfile)*(inputfiles+1));
      inputfile[inputfiles++] = s_arg[i_arg].list[0];
    }
  }

  if (inputfiles>1) {
    if (!(pipeFlags&USE_STDOUT)) {
      outputfile = inputfile[--inputfiles];
      if (fexists(outputfile) && !overwrite) {
        fprintf(stderr, "output file exists already--give -overWrite option to force replacement\n");
        return(1);
      }
    }
  } else if (inputfiles==1) {
    if (pipeFlags&USE_STDOUT && outputfile) {
      fprintf(stderr, "too many filenames given with -pipe=output\n");
      return(1);
    }
  } else {
    fprintf(stderr, "too few filenames given\n");
    return(1);
  }


  for (i=0 ; i<inputfiles ; i++) {
    if (SDDS_InitializeInput(&SDDS_input, inputfile[i]) == 0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    columnname=SDDS_GetColumnNames(&SDDS_input, &columnnames);
    if (columnname==NULL) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if ((columnnames > 3) || (columnnames < 2)) {
      fprintf(stderr, "unexpected columns in data logger file\n");
      return(1);
    }
    if (columnnames == 2) {
      if (strcmp("Time", columnname[0]) == 0) {
        timeIndex = 0;
        dataIndex = 1;
      } else if (strcmp("Time", columnname[1]) == 0) {
        timeIndex = 1;
        dataIndex = 0;
      } else {
        fprintf(stderr, "Time column is missing in data logger file\n");
        return(1);
      }
    }
    if (columnnames == 3) {
      if (strcmp("CAerrors", columnname[0]) == 0) {
        caerrorsIndex = 0;
        if (strcmp("Time", columnname[1]) == 0) {
          timeIndex = 1;
          dataIndex = 2;
        } else if (strcmp("Time", columnname[2]) == 0) {
          timeIndex = 2;
          dataIndex = 1;
        } else {
          fprintf(stderr, "Time column is missing in data logger file\n");
          return(1);
        }
      } else if (strcmp("CAerrors", columnname[1]) == 0) {
        caerrorsIndex = 1;
        if (strcmp("Time", columnname[0]) == 0) {
          timeIndex = 0;
          dataIndex = 2;
        } else if (strcmp("Time", columnname[2]) == 0) {
          timeIndex = 2;
          dataIndex = 0;
        } else {
          fprintf(stderr, "Time column is missing in data logger file\n");
          return(1);
        }
      } else if (strcmp("CAerrors", columnname[2]) == 0) {
        caerrorsIndex = 2;
        if (strcmp("Time", columnname[0]) == 0) {
          timeIndex = 0;
          dataIndex = 1;
        } else if (strcmp("Time", columnname[1]) == 0) {
          timeIndex = 1;
          dataIndex = 0;
        } else {
          fprintf(stderr, "Time column is missing in data logger file\n");
          return(1);
        }
      } else {
        fprintf(stderr, "CAerrors column is missing in data logger file\n");
        return(1);
      }
    }

    while (SDDS_ReadTable(&SDDS_input)>0) {
      timeValues = realloc(timeValues, sizeof(*timeValues)*(page+1));
      dataValues = realloc(dataValues, sizeof(*dataValues)*(page+1));
      dataNames = realloc(dataNames, sizeof(*dataNames)*(page+1));
      rows = realloc(rows, sizeof(*rows)*(page+1));
      SDDS_CopyString(&dataNames[page], columnname[dataIndex]);
      rows[page] = SDDS_RowCount(&SDDS_input);
      if (rows[page] > 0) {
        timeValues[page] = SDDS_GetColumnInDoubles(&SDDS_input, "Time");
        if (timeValues[page] == NULL) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
        dataValues[page] = SDDS_GetColumnInFloats(&SDDS_input, columnname[dataIndex]);
        if (dataValues[page] == NULL) {
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
          return(1);
        }
      } else {
        timeValues[page]=NULL;
        dataValues[page]=NULL;
      }
      page++;
    }
    for (j=0; j<columnnames; j++) {
      free(columnname[j]);
    }
    free(columnname);
    if (SDDS_Terminate(&SDDS_input) == 0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
  }
  pages = page;

  for (page=0; page<pages; page++) {
    found = 0;
    for (i=0; i<uniqueDataNames; i++) {
      if (strcmp(dataNames[page], uniqueDataName[i]) == 0) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      uniqueDataName = realloc(uniqueDataName, sizeof(*uniqueDataName)*(uniqueDataNames+1));
      SDDS_CopyString(&uniqueDataName[uniqueDataNames], dataNames[page]);
      uniqueDataNames++;
    }
  }

  if (SDDS_InitializeOutput(&SDDS_output, SDDS_BINARY, 0, NULL, NULL, outputfile) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (SDDS_DefineSimpleColumn(&SDDS_output, "Time", "s", SDDS_DOUBLE) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  for (i=0; i<uniqueDataNames; i++) {
    if (SDDS_DefineSimpleColumn(&SDDS_output, uniqueDataName[i], NULL, SDDS_FLOAT) == 0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
  }
  outputDataValues = malloc(sizeof(*outputDataValues)*uniqueDataNames);
  if (uniqueDataNames == 1) {
    for (page=0; page<pages; page++) {
      allocated_rows += rows[page];
    }
    outputTimeValues = malloc(sizeof(*outputTimeValues)*allocated_rows);
    outputDataValues[0] = malloc(sizeof(*(outputDataValues[0]))*allocated_rows);
    i = 0;
    for (page=0; page<pages; page++) {
      for (j=0; j<rows[page]; j++) {
        outputTimeValues[i] = timeValues[page][j];
        outputDataValues[0][i] = dataValues[page][j];
        i++;
      }
    }
  } else {
    flag = malloc(sizeof(*flag)*pages);
    for (page=0; page<pages; page++) {
      flag[page] = calloc(rows[page], sizeof(*(flag[page])));
    }
    array = malloc(sizeof(*array)*uniqueDataNames);
    arrayCount = calloc(uniqueDataNames, sizeof(*arrayCount));
    for (i=0; i<uniqueDataNames; i++) {
      for (page=0; page<pages; page++) {
        if (strcmp(dataNames[page], uniqueDataName[i]) == 0) {
          arrayCount[i]++;
          if (arrayCount[i] == 1) {
            array[i] = malloc(sizeof(*(array[i])));
          } else {
            array[i] = realloc(array[i], sizeof(*(array[i]))*arrayCount[i]);
          }
          array[i][arrayCount[i] - 1] = page;
        }
      }
    }
    
    for (i=0; i<arrayCount[0]; i++) {
      keyGroup = MakeSortedKeyGroups(&(keyGroups), SDDS_DOUBLE, timeValues[array[0][i]], rows[array[0][i]]);
      for (n=1; n<uniqueDataNames; n++) {
        for (m=0; m<arrayCount[n]; m++) {
          if ((i==m) && (rows[array[0][i]] == rows[array[n][m]]) && (rows[array[0][i]] > 10)) {
            if ((timeValues[array[0][i]][0] == timeValues[array[n][m]][0]) && 
                (timeValues[array[0][i]][1] == timeValues[array[n][m]][1]) && 
                (timeValues[array[0][i]][rows[array[0][i]]-2] == timeValues[array[n][m]][rows[array[n][m]]-2]) && 
                (timeValues[array[0][i]][rows[array[0][i]]-1] == timeValues[array[n][m]][rows[array[n][m]]-1])) {
                /*Assume the entire page matches because it has the same number or rows and the first few and last few lines match*/
              for (r=0; r<rows[array[n][m]]; r++) {
                if (flag[array[n][m]][r]) {
                  continue;
                }
                flag[array[0][i]][r] += 1;
                flag[array[n][m]][r] = 1;
              }
            }
          }
          for (r=0; r<rows[array[n][m]]; r++) {
            if (flag[array[n][m]][r]) {
              continue;
            }
            row = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_DOUBLE, &(timeValues[array[n][m]][r]), 1);
            if (row>=0) {
              flag[array[0][i]][row] += 1;
              flag[array[n][m]][r] = 1;
            }
          }
        }
      }
      for (j=0; j<keyGroups; j++) {
        free(keyGroup[j]->equivalent);
        free(keyGroup[j]);
      }
      free(keyGroup);
    }
    
    z = uniqueDataNames-1;
    for (n=0; n<arrayCount[0]; n++) {
      for (m=0; m<rows[array[0][n]]; m++) {
        if (flag[array[0][n]][m] >= z) {
          allocated_rows++;
        }
      }
    }
    outputTimeValues = malloc(sizeof(*outputTimeValues)*allocated_rows);
    for (i=0; i<uniqueDataNames; i++) {
      outputDataValues[i] = malloc(sizeof(*(outputDataValues[i]))*allocated_rows);
    }
    s=0;
    for (i=0; i<arrayCount[0]; i++) {
      for (j=0; j<rows[array[0][i]]; j++) {
        if (flag[array[0][i]][j] >= z) {
          outputTimeValues[s] = timeValues[array[0][i]][j];
          outputDataValues[0][s] = dataValues[array[0][i]][j];
          s++;
        }
      }
    }
    if (s==0) {
      fprintf(stderr, "No matching Time rows in input files\n");
      return(1);
    }

    keyGroup = MakeSortedKeyGroups(&keyGroups, SDDS_DOUBLE, outputTimeValues, s);

    for (n=1; n<uniqueDataNames; n++) {
      for (m=0; m<arrayCount[n]; m++) {
        for (r=0; r<rows[array[n][m]]; r++) {
          if (flag[array[n][m]][r]) {
            row = FindMatchingKeyGroup(keyGroup, keyGroups, SDDS_DOUBLE, &(timeValues[array[n][m]][r]), 1);
            if (row>=0) {
              outputDataValues[n][row] = dataValues[array[n][m]][r];
            }
          }
        }
      }
    }
    for (i=0; i<uniqueDataNames; i++) {
      free(array[i]);
    }
    for (j=0; j<keyGroups; j++) {
      if (keyGroup[j]->equivalent) free(keyGroup[j]->equivalent);
      if (keyGroup[j]) free(keyGroup[j]);
    }
    for (page=0; page<pages; page++) {
      free(flag[page]);
    }
    free(array);
    free(keyGroup);
    free(arrayCount);
    free(flag);
  }
  for (page=0; page<pages; page++) {
    if (timeValues[page]) free(timeValues[page]);
    if (dataValues[page]) free(dataValues[page]);
    free(dataNames[page]);
  }
  free(timeValues);
  free(dataValues);
  free(dataNames);

  if (SDDS_WriteLayout(&SDDS_output) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (SDDS_StartPage(&SDDS_output, allocated_rows) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (SDDS_SetColumnFromDoubles(&SDDS_output, SDDS_SET_BY_NAME, outputTimeValues, allocated_rows, "Time") == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  for (i=0; i<uniqueDataNames; i++) {
    if (SDDS_SetColumnFromFloats(&SDDS_output, SDDS_SET_BY_NAME, outputDataValues[i], allocated_rows, uniqueDataName[i]) == 0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
  }
  if (SDDS_WriteTable(&SDDS_output) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (SDDS_Terminate(&SDDS_output) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
 
  for (i=0; i<uniqueDataNames; i++) {
    free(uniqueDataName[i]);
    free(outputDataValues[i]);
  }
  free(outputTimeValues);
  free(outputDataValues);
  free(uniqueDataName);
  free(rows);
  if (inputfiles > 0) {
    free(inputfile);
  }
  free_scanargs(&s_arg,argc);
  return(0);
}


