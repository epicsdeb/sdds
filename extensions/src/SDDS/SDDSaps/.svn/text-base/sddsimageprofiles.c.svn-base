/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsimageprofiles.c
 * purpose: analyze images stored as horizontal lines (one per column),
 *          based on ideas of B-X Yang.
 * M. Borland, 2000
 $Log: not supported by cvs2svn $
 Revision 1.9  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.8  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.7  2005/10/18 14:09:44  jiaox
 Fixed a background option bug that caused the program crashed.

 Revision 1.6  2003/09/18 21:44:24  soliday
 Added the peak method.

 Revision 1.5  2003/09/02 19:16:04  soliday
 Cleaned up code for Linux.

 Revision 1.4  2003/06/19 22:53:34  soliday
 Fixed a bug if the numbers in the column names started with 0 instead of 1.

 Revision 1.3  2002/08/14 17:12:47  soliday
 Added Open License

 Revision 1.2  2001/01/10 19:35:39  soliday
 Standardized usage message.

 Revision 1.1  2000/10/23 16:03:21  soliday
 First version.

 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_PIPE 0
#define SET_PROFILETYPE 1
#define SET_COLPREFIX 2
#define SET_METHOD 3
#define SET_AREAOFINTEREST 4
#define SET_BACKGROUND 5
#define N_OPTIONS 6

char *option[N_OPTIONS] = {
   "pipe", "profileType", "columnPrefix",
   "method", "areaOfInterest", "background"
};

char *USAGE = "sddsimageprofiles [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-columnPrefix=<prefix>]\n\
[-profileType={x|y}]\n\
[-method={centerLine|integrated|averaged|peak}]\n\
[-background=<filename>]\n\
[-areaOfInterest=<rowStart>,<rowEnd>,<columnStart>,<columnEnd>]\n\n\
Program by Robert Soliday. ("__DATE__")\n\n\
\
-method      If this option is not specified it is a real profile.\n\
             If centerLine is specified it will find the row with the\n\
             greatest integrated profile and display that line only.\n\
             If integrated is specified it will sum all the profiles\n\
             together. If averaged is specified it will divide the sum\n\
             of all the profiles by the number of profiles. If peak is\n\
             specified it will find the peak point and display the profile\n\
             for that row.\n";

typedef struct {
  short *shortData;
  int32_t *longData;
  float *floatData;
  double *doubleData;
} IMAGE_DATA;

int xImageProfile(IMAGE_DATA *data, int32_t *type, long rows, 
		  SDDS_DATASET *SDDS_dataset, long method,
		  long x1, long x2,
		  long y1, long y2,
		  long *colIndex, double *colIndex2);

int yImageProfile(IMAGE_DATA *data, int32_t *type, long rows, 
		  SDDS_DATASET *SDDS_dataset, long method,
		  long x1, long x2,
		  long y1, long y2,
		  long *colIndex, double *colIndex2);

long xPeakLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2);

long yPeakLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2);
long xCenterLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2);

long yCenterLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2);

long GetData(SDDS_DATASET *SDDS_orig, char *input,
	     IMAGE_DATA **data, int32_t **type,
	     long **colIndex, double **colIndex2,
	     char *colPrefix, long *validColumns);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_dataset, SDDS_orig, SDDS_bg;
  long i_arg;
  SCANNED_ARG *s_arg;
  IMAGE_DATA *data, *bg_data;
  char *input=NULL, *output=NULL, *colPrefix=NULL, *background=NULL;
  long profileType=1, noWarnings=0, tmpfile_used=0, method=0;
  unsigned long pipeFlags=0;
  long rows, bg_rows, i, j, validColumns=0, bg_validColumns=0;
  int32_t *type, *bg_type;

  long rowStart=1, rowEnd=0, columnStart=1, columnEnd=0;

  long *colIndex, *bg_colIndex;
  double *colIndex2, *bg_colIndex2;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
 
  if (argc<3) 
    bomb(NULL, USAGE);


  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_PROFILETYPE:
	if (s_arg[i_arg].n_items != 2)
	  SDDS_Bomb("invalid -profileType syntax");
	if (strcmp("x", s_arg[i_arg].list[1]) == 0)
	  profileType = 1;
	if (strcmp("y", s_arg[i_arg].list[1]) == 0)
	  profileType = 2;
	break;
      case SET_COLPREFIX:
	if (s_arg[i_arg].n_items != 2)
	  SDDS_Bomb("invalid -columnPrefix syntax");
	colPrefix = s_arg[i_arg].list[1];
	break;
      case SET_METHOD:
	if (s_arg[i_arg].n_items != 2)
	  SDDS_Bomb("invalid -method syntax");
	if ((strncasecmp("centralLine", s_arg[i_arg].list[1], strlen(s_arg[i_arg].list[1])) == 0) || (strncasecmp("centerLine", s_arg[i_arg].list[1], strlen(s_arg[i_arg].list[1])) == 0))
	  method = 1;
	if (strncasecmp("integrated", s_arg[i_arg].list[1], strlen(s_arg[i_arg].list[1])) == 0)
	  method = 2;
	if (strncasecmp("averaged", s_arg[i_arg].list[1], strlen(s_arg[i_arg].list[1])) == 0)
	  method = 3;
	if (strncasecmp("peak", s_arg[i_arg].list[1], strlen(s_arg[i_arg].list[1])) == 0)
	  method = 4;
	break;
      case SET_AREAOFINTEREST:
	if (s_arg[i_arg].n_items!=5)
	  SDDS_Bomb("invalid -areaOfInterest syntax");
	if (sscanf(s_arg[i_arg].list[1], "%ld", &rowStart)!=1 || rowStart<=0)
	  SDDS_Bomb("invalid -areaOfInterest syntax or value");
	if (sscanf(s_arg[i_arg].list[2], "%ld", &rowEnd)!=1 || rowEnd<=0)
	  SDDS_Bomb("invalid -areaOfInterest syntax or value");
	if (sscanf(s_arg[i_arg].list[3], "%ld", &columnStart)!=1 || columnStart<=0)
	  SDDS_Bomb("invalid -areaOfInterest syntax or value");
	if (sscanf(s_arg[i_arg].list[4], "%ld", &columnEnd)!=1 || columnEnd<=0)
	  SDDS_Bomb("invalid -areaOfInterest syntax or value");
	break;
      case SET_BACKGROUND:
	if (s_arg[i_arg].n_items!=2)
	  SDDS_Bomb("invalid -background syntax");
	background = s_arg[i_arg].list[1];
	break;
      case SET_PIPE:
	if (!processPipeOption(s_arg[i_arg].list+1, 
			       s_arg[i_arg].n_items-1, &pipeFlags))
	  SDDS_Bomb("invalid -pipe syntax");
	break;
      default:
	fprintf(stderr, "error: unknown switch: %s\n", s_arg[i_arg].list[0]);
	exit(1);
	break;
      }
    } else {
      if (input == NULL)
	input = s_arg[i_arg].list[0];
      else if (output == NULL)
	output = s_arg[i_arg].list[0];
      else
	SDDS_Bomb("too many filenames");
    }
  }

  if (colPrefix == NULL) {
    fprintf(stderr, "error: missing columnPrefix\n");
    exit(1);
  }

  processFilenames("sddsimageprofiles", &input, &output, 
		   pipeFlags, noWarnings, &tmpfile_used);

  /* Read in the image file */
  rows = GetData(&SDDS_orig, input, &data, &type,
		 &colIndex, &colIndex2, colPrefix, &validColumns);

  if (rows<0) {
    fprintf(stderr, "error: no rows in image file\n");
    exit(1);
  }

  if (background != NULL) {
    /* Read in the background image file */
    bg_rows = GetData(&SDDS_bg, background, &bg_data,
			      &bg_type, &bg_colIndex, &bg_colIndex2,
			      colPrefix, &bg_validColumns);
    if (rows != bg_rows) {
      fprintf(stderr, "error: background has a different number of rows\n");
      exit(1);
    }
    if (validColumns != bg_validColumns) {
      fprintf(stderr, "error: background has a different number of columns\n");
      exit(1);
    }
    /* Subtract the background from the image file */
    for (i=0;i<validColumns;i++) {
      if (type[colIndex[i]] != bg_type[bg_colIndex[i]]) {
	fprintf(stderr, "error: column types don't match with background image\n");
	exit(1);
      }
      if (colIndex2[i] != bg_colIndex2[i]) {
	fprintf(stderr, "error: image rows don't match with background image\n");
	exit(1);
      }
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
	for (j=0;j<rows;j++)
	  data[colIndex[i]].shortData[j] -= bg_data[bg_colIndex[i]].shortData[j];
	break;
      case SDDS_LONG:
	for (j=0;j<rows;j++)
	  data[colIndex[i]].longData[j] -= bg_data[bg_colIndex[i]].longData[j];
	break;
      case SDDS_FLOAT:
	for (j=0;j<rows;j++)
	  data[colIndex[i]].floatData[j] -= bg_data[bg_colIndex[i]].floatData[j];
	break;
      case SDDS_DOUBLE:
	for (j=0;j<rows;j++)
	  data[colIndex[i]].doubleData[j] -= bg_data[bg_colIndex[i]].doubleData[j];
	break;
      default:
	continue;
      }
    }
  }

    /* Initialize the output file and define the columns */
  if (!SDDS_InitializeOutput(&SDDS_dataset, SDDS_ASCII, 
			     1, NULL, NULL, output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (profileType == 1) {
    if (SDDS_DefineParameter(&SDDS_dataset, "Zone", NULL, NULL, NULL, 
			  NULL, SDDS_STRING, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (SDDS_DefineColumn(&SDDS_dataset, "x", NULL, NULL, NULL, 
			  NULL, SDDS_LONG, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (SDDS_DefineColumn(&SDDS_dataset, "y", NULL, NULL, NULL, 
			  NULL, SDDS_DOUBLE, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  } else {
    if (SDDS_DefineParameter(&SDDS_dataset, "Zone", NULL, NULL, NULL, 
			  NULL, SDDS_STRING, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (SDDS_DefineColumn(&SDDS_dataset, "x", NULL, NULL, NULL, 
			  NULL, SDDS_DOUBLE, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    if (SDDS_DefineColumn(&SDDS_dataset, "y", NULL, NULL, NULL, 
			  NULL, SDDS_DOUBLE, 0) == -1) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
  }
  if (SDDS_WriteLayout(&SDDS_dataset) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if ((rowEnd > rows) || (rowEnd < rowStart))
    rowEnd = rows;
  
  if ((columnEnd > validColumns) || (columnEnd < columnStart))
    columnEnd = validColumns;

  if (profileType == 1) {
    xImageProfile(data, type, rows, &SDDS_dataset, method,
                  rowStart-1, rowEnd, columnStart-1, columnEnd, colIndex, colIndex2);
  } else {
    yImageProfile(data, type, rows, &SDDS_dataset, method,
                  rowStart-1, rowEnd, columnStart-1, columnEnd, colIndex, colIndex2);
  }

  /* Close the output file */
  if (SDDS_Terminate(&SDDS_dataset) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  return(0);
}

int xImageProfile(IMAGE_DATA *data, int32_t *type, long rows,
		  SDDS_DATASET *SDDS_dataset, long method,
		  long x1, long x2,
		  long y1, long y2,
		  long *colIndex, double *colIndex2) {
  int i, j, k=0;
  double val=0;
  long center;
  int32_t *index;
  double *values;
  char value[30];
  index = malloc(sizeof(int32_t)*rows);
  values = malloc(sizeof(double)*rows);

  if (method == 0) { /* Highest point */
    for (i=x1; i<x2; i++) {
      switch (type[colIndex[y1]]) {
      case SDDS_SHORT:
	val = data[colIndex[y1]].shortData[i];
	break;
      case SDDS_LONG:
	val = data[colIndex[y1]].longData[i];
	break;
      case SDDS_FLOAT:
	val = data[colIndex[y1]].floatData[i];
	break;
      case SDDS_DOUBLE:
	val = data[colIndex[y1]].doubleData[i];
	break;
      }
      for (j=y1+1; j<y2; j++) {
	switch (type[colIndex[j]]) {
	case SDDS_SHORT:
	  if (val < data[colIndex[j]].shortData[i])
	    val = data[colIndex[j]].shortData[i];
	  break;
	case SDDS_LONG:
	  if (val < data[colIndex[j]].longData[i])
	    val = data[colIndex[j]].longData[i];
	  break;
	case SDDS_FLOAT:
	  if (val < data[colIndex[j]].floatData[i])
	    val = data[colIndex[j]].floatData[i];
	  break;
	case SDDS_DOUBLE:
	  if (val < data[colIndex[j]].doubleData[i])
	    val = data[colIndex[j]].doubleData[i];
	  break;
	  
	}
      }
      index[k] = i+1;
      values[k] = val;
      k++;
    }
  } else if ((method == 1) || (method == 4)) { /* Center line or peak*/
    if (method==4) {
      center = yPeakLine(data, type, colIndex, x1, x2, y1, y2);
    } else {
      center = yCenterLine(data, type, colIndex, x1, x2, y1, y2);
    }
    for (i=x1; i<x2; i++) {
      switch (type[colIndex[center]]) {
      case SDDS_SHORT:
	val = data[colIndex[center]].shortData[i];
	break;
      case SDDS_LONG:
	val = data[colIndex[center]].longData[i];
	break;
      case SDDS_FLOAT:
	val = data[colIndex[center]].floatData[i];
	break;
      case SDDS_DOUBLE:
	val = data[colIndex[center]].doubleData[i];
	break;
	
      }
      index[k] = i+1;
      values[k] = val;
      k++;
    }
  } else if ((method == 2) || (method == 3)) { /* Integrated or Averaged profile */
    for (i=x1; i<x2; i++) {
      val = 0;
      for (j=y1; j<y2; j++) {
	switch (type[colIndex[j]]) {
	case SDDS_SHORT:
	  val += data[colIndex[j]].shortData[i];
	  break;
	case SDDS_LONG:
	  val += data[colIndex[j]].longData[i];
	  break;
	case SDDS_FLOAT:
	  val += data[colIndex[j]].floatData[i];
	  break;
	case SDDS_DOUBLE:
	  val += data[colIndex[j]].doubleData[i];
	  break; 
	}
      }
      index[k] = i+1;
      if (method == 2)
	values[k] = val;
      else
	values[k] = val/(y2-y1);
      k++;
    }
  }

  if (SDDS_StartPage(SDDS_dataset, x2-x1) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  sprintf(value, "(%ld,%ld) x (%ld,%ld)", x1+1, y1+1, x2, y2);
  if (SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			 "Zone", value, NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_SetColumn(SDDS_dataset, SDDS_SET_BY_NAME, 
		     index, k, "x", NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_SetColumn(SDDS_dataset, SDDS_SET_BY_NAME, 
		     values, k, "y", NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_WritePage(SDDS_dataset) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}

int yImageProfile(IMAGE_DATA *data, int32_t *type, long rows,
		  SDDS_DATASET *SDDS_dataset, long method,
		  long x1, long x2,
		  long y1, long y2,
		  long *colIndex, double *colIndex2) {
  int i, j, k=0;
  double val;
  long center;
  
  double *index;
  double *values;
  char value[30];

  index = malloc(sizeof(double)*(y2-y1));
  values = malloc(sizeof(double)*(y2-y1));

  if (method == 0) { /* Highest point */
    for (i=y1; i<y2; i++) {
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
	val = data[colIndex[i]].shortData[0];
	for (j=x1+1;j<x2;j++) {
	  if (val < data[colIndex[i]].shortData[j])
	    val = data[colIndex[i]].shortData[j];
	}
	break;
      case SDDS_LONG:
	val = data[colIndex[i]].longData[0];
	for (j=x1+1;j<x2;j++) {
	  if (val < data[colIndex[i]].longData[j])
	    val = data[colIndex[i]].longData[j];
	}
	break;
      case SDDS_FLOAT:
	val = data[colIndex[i]].floatData[0];
	for (j=x1+1;j<x2;j++) {
	  if (val < data[colIndex[i]].floatData[j])
	    val = data[colIndex[i]].floatData[j];
	}
	break;
      case SDDS_DOUBLE:
	val = data[colIndex[i]].doubleData[0];
	for (j=x1+1;j<x2;j++) {
	  if (val < data[colIndex[i]].doubleData[j])
	    val = data[colIndex[i]].doubleData[j];
	}
	break;
      default:
	continue;
      }
      index[k] = colIndex2[i];
      values[k] = val;
      k++;
    }
  } else if ((method == 1) || (method==4)) { /* Center line */
    if (method==4) {
      center = xPeakLine(data, type, colIndex, x1, x2, y1, y2);
    } else {
      center = xCenterLine(data, type, colIndex, x1, x2, y1, y2);
    }
    for (i=y1; i<y2; i++) {
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
	val = data[colIndex[i]].shortData[center];
	break;
      case SDDS_LONG:
	val = data[colIndex[i]].longData[center];
	break;
      case SDDS_FLOAT:
	val = data[colIndex[i]].floatData[center];
	break;
      case SDDS_DOUBLE:
	val = data[colIndex[i]].doubleData[center];
	break;
      default:
	continue;
      }
      index[k] = colIndex2[i];
      values[k] = val;
      k++;
    }
  } else if ((method == 2) || (method == 3)) { /* Integrated or Averaged profile */
    for (i=y1; i<y2; i++) {
      val = 0;
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
	for (j=x1;j<x2;j++)
	  val += data[colIndex[i]].shortData[j];
	break;
      case SDDS_LONG:
	for (j=x1;j<x2;j++)
	  val += data[colIndex[i]].longData[j];
	break;
      case SDDS_FLOAT:
	for (j=x1;j<x2;j++)
	  val += data[colIndex[i]].floatData[j];
	break;
      case SDDS_DOUBLE:
	for (j=x1;j<x2;j++)
	  val += data[colIndex[i]].doubleData[j];
	break;
      default:
	continue;
      }
      index[k] = colIndex2[i];
      if (method == 2)
	values[k] = val;
      else
	values[k] = val/(x2-x1);
      k++;
    }
  }

  if (SDDS_StartPage(SDDS_dataset, k) == 0) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  sprintf(value, "(%ld,%ld) x (%ld,%ld)", x1+1, y1+1, x2, y2);
  if (SDDS_SetParameters(SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			 "Zone", value, NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_SetColumn(SDDS_dataset, SDDS_SET_BY_NAME, 
		     index, k, "y", NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_SetColumn(SDDS_dataset, SDDS_SET_BY_NAME, 
		     values, k, "x", NULL) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (SDDS_WritePage(SDDS_dataset) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  return(0);
}

long xPeakLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2) {
  int i, j;
  double maxValue=0;
  long index=0;
  
  index = x1;
  switch (type[colIndex[y1]]) {
  case SDDS_SHORT:
    maxValue = data[colIndex[y1]].shortData[x1];
    break;
  case SDDS_LONG:
    maxValue = data[colIndex[y1]].longData[x1];
    break;
  case SDDS_FLOAT:
    maxValue = data[colIndex[y1]].floatData[x1];
    break;
  case SDDS_DOUBLE:
    maxValue = data[colIndex[y1]].doubleData[x1];
    break;
  }
  for (i=y1; i<y2; i++) {
    for (j=x1;j<x2;j++) {
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
        if (maxValue < data[colIndex[i]].shortData[j]) {
          maxValue = data[colIndex[i]].shortData[j];
          index = j;
        }
        break;
      case SDDS_LONG:
        if (maxValue < data[colIndex[i]].longData[j]) {
          maxValue = data[colIndex[i]].longData[j];
          index = j;
        }
        break;
      case SDDS_FLOAT:
        if (maxValue < data[colIndex[i]].floatData[j]) {
          maxValue = data[colIndex[i]].floatData[j];
          index = j;
        }
        break;
      case SDDS_DOUBLE:
        if (maxValue < data[colIndex[i]].doubleData[j]) {
          maxValue = data[colIndex[i]].doubleData[j];
          index = j;
        }
        break;
      }
    }
  }
  return(index);
}

long yPeakLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2) {
  int i, j;
  double maxValue=0;
  long index=0;
  
  index = y1;
  switch (type[colIndex[y1]]) {
  case SDDS_SHORT:
    maxValue = data[colIndex[y1]].shortData[x1];
    break;
  case SDDS_LONG:
    maxValue = data[colIndex[y1]].longData[x1];
    break;
  case SDDS_FLOAT:
    maxValue = data[colIndex[y1]].floatData[x1];
    break;
  case SDDS_DOUBLE:
    maxValue = data[colIndex[y1]].doubleData[x1];
    break;
  }
  for (i=y1; i<y2; i++) {
    for (j=x1;j<x2;j++) {
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
        if (maxValue < data[colIndex[i]].shortData[j]) {
          maxValue = data[colIndex[i]].shortData[j];
          index = i;
        }
        break;
      case SDDS_LONG:
        if (maxValue < data[colIndex[i]].longData[j]) {
          maxValue = data[colIndex[i]].longData[j];
          index = i;
        }
        break;
      case SDDS_FLOAT:
        if (maxValue < data[colIndex[i]].floatData[j]) {
          maxValue = data[colIndex[i]].floatData[j];
          index = i;
        }
        break;
      case SDDS_DOUBLE:
        if (maxValue < data[colIndex[i]].doubleData[j]) {
          maxValue = data[colIndex[i]].doubleData[j];
          index = i;
        }
        break;
      }
    }
  }
  return(index);
}

long xCenterLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2) {
  int i, j, start=1;
  double val=0, maxValue=0;
  long index=0;

  for (i=x1; i<x2; i++) {
    val = 0;
    for (j=y1; j<y2; j++) {
      switch (type[colIndex[j]]) {
      case SDDS_SHORT:
	val += data[colIndex[j]].shortData[i];
	break;
      case SDDS_LONG:
	val += data[colIndex[j]].longData[i];
	break;
      case SDDS_FLOAT:
	val += data[colIndex[j]].floatData[i];
	break;
      case SDDS_DOUBLE:
	val += data[colIndex[j]].doubleData[i];
	break; 
      }
    }
    if (start == 1) {
      index = i;
      maxValue = val;
      start = 0;
    } else {
      if (val > maxValue) {
	index = i;
	maxValue = val;
      }
    }
  }
  return(index);
}

long yCenterLine(IMAGE_DATA *data, int32_t *type, 
		 long *colIndex,
		 long x1, long x2,
		 long y1, long y2) {
  int i, j, start=1;
  double val, maxValue=0;
  long index=0;
  
  for (i=y1; i<y2; i++) {
    val = 0;
    for (j=x1;j<x2;j++)
      switch (type[colIndex[i]]) {
      case SDDS_SHORT:
	val += data[colIndex[i]].shortData[j];
        break;
      case SDDS_LONG:
	val += data[colIndex[i]].longData[j];
        break;
      case SDDS_FLOAT:
	val += data[colIndex[i]].floatData[j];
        break;
      case SDDS_DOUBLE:
	val += data[colIndex[i]].doubleData[j];
        break;
      default:
        continue;
      }
    if (start == 1) {
      index = i;
      maxValue = val;
      start = 0;
    } else {
      if (val > maxValue) {
	index = i;
	maxValue = val;
      }
    }
  }
  return(index);
}

long GetData(SDDS_DATASET *SDDS_orig, char *input,
	     IMAGE_DATA **data, int32_t **type,
	     long **colIndex, double **colIndex2,
	     char *colPrefix, long *validColumns) {
  long rows, i, j, temp;
  double temp2;
  int32_t orig_column_names;
  char **orig_column_name;

  /* Open image file */
  if (!SDDS_InitializeInput(SDDS_orig, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  /* Read column names */
  if (!(orig_column_name = SDDS_GetColumnNames(SDDS_orig, 
					       &orig_column_names))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  /* Allocate memory for image file data */
  *data = malloc(sizeof(IMAGE_DATA)*orig_column_names);
  *type = malloc(sizeof(int32_t)*orig_column_names);

  /* Read page */
  if (SDDS_ReadPage(SDDS_orig) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  /* Get number of rows */
  rows = SDDS_RowCount(SDDS_orig);
  *colIndex = malloc(sizeof(long)*orig_column_names);
  *colIndex2 = malloc(sizeof(double)*orig_column_names);

  /* Read all numerical data from file */
  for (i=0; i<orig_column_names; i++) {
    (*type)[i] = SDDS_GetNamedColumnType(SDDS_orig, orig_column_name[i]);
    if ((*type)[i] == 0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    switch ((*type)[i]) {
    case SDDS_SHORT:
      (*data)[i].shortData = SDDS_GetColumn(SDDS_orig, orig_column_name[i]);
      break;
    case SDDS_LONG:
      (*data)[i].longData = SDDS_GetColumn(SDDS_orig, orig_column_name[i]);
      break;
    case SDDS_FLOAT:
      (*data)[i].floatData = SDDS_GetColumn(SDDS_orig, orig_column_name[i]);
      break;
    case SDDS_DOUBLE:
      (*data)[i].doubleData = SDDS_GetColumn(SDDS_orig, orig_column_name[i]);
      break;
    default:
      continue;
    }
    if (strncmp(colPrefix, orig_column_name[i], strlen(colPrefix)) != 0) { 
      continue;
    }
    (*colIndex)[*validColumns] = i;
    *validColumns += 1;
  }

  if (SDDS_Terminate(SDDS_orig) != 1) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (*validColumns == 0) {
    fprintf(stderr, "error: no valid columns in image file\n");
    exit(1);
  }
  /* For each valid column read the row from the column name */
  for (i=0;i<*validColumns;i++) {
    (*colIndex2)[i] = atof(orig_column_name[(*colIndex)[i]]+strlen(colPrefix));
  }
  /* Sort columns by their row value ('row' is not related to the SDDS rows)*/
  for (i=0;i<*validColumns;i++) {
    for (j=i+1;j<*validColumns;j++) {
      if ((*colIndex2)[j] < (*colIndex2)[i]) {
	temp = (*colIndex)[i];
	temp2 = (*colIndex2)[i];
	(*colIndex)[i] = (*colIndex)[j];
	(*colIndex2)[i] = (*colIndex2)[j];
	(*colIndex)[j] = temp;
	(*colIndex2)[j] = temp2;
      }
    }
  }
  return rows;
}
