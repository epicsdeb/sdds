/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sdds3dconvert 
 *
 *  This program is for converting 3d vorpal output, which has
 *  Ex_1, Ex_2, ... Ex_ny columns for Ex (similar for other quantity, such as Ey, Ez, Rho, Jx, Jx, Jz ect)
 *  , nz pages, and nx rows, where
 *  each page has the same z coordinates, and each column has the same y coordinates,
 *  each row has the same x coordniates. The input data can be plot
 *  with sddscontour for Ex versus xy plane with columnMatch option.
 *  after the conversion, the output file can be ploted with sddscontour with quantity option for yz, and xz plane.
 *  the input file also has following parameters:
 *  origin1, origin2, origin3, max_ext1, max_ext2, max_ext3, delta1, delta2, delta3, numPhysCells1,
 *  numPhysCells12, numPhysCells3, i.e, the minimum, maximum, interval and dimension size of x, y, z
 *
 *  $Log: not supported by cvs2svn $
 */

#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

char *USAGE = "sdds3dconvert <inputFile> [<outputRoot>]\n";
void SetupOutputFile(SDDS_DATASET *SDDS_out, char *output, long yz, long outputColumns, char **outputColumn);
void free_data_memory(double ****data, long outputColumns, long pages, long ydim);

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS_orig, SDDS_out1, SDDS_out2;
  int32_t i, j, k, row, str_len;
  SCANNED_ARG *s_arg;
  char *inputFile, *outputRoot, output1[1024], output2[1024], tmpcol[256];
  double xmin, xmax, ymin, ymax, zmin, zmax, xinterval, yinterval, zinterval;
  int32_t xdim, ydim, zdim, columnNames, outputColumns, page=0;
  char **columnName, **outputColumn;
  double ****data;
  
  inputFile = outputRoot = NULL;
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
	bomb(NULL, USAGE);
  
  columnNames = outputColumns = 0;
  columnName = outputColumn = NULL;
  inputFile = s_arg[1].list[0];
  if (argc==3)
	outputRoot = s_arg[2].list[0];
  else 
	outputRoot = inputFile;
  
  sprintf(output1, "%s.yz", outputRoot);
  sprintf(output2, "%s.xz", outputRoot);
  data = NULL;
  if (!SDDS_InitializeInput(&SDDS_orig, inputFile)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
  }
  columnName = (char**)SDDS_GetColumnNames(&SDDS_orig, &columnNames);
  for (i=0; i<columnNames; i++) {
	if (wild_match(columnName[i], "*_1")) {
	  outputColumn = SDDS_Realloc(outputColumn, (outputColumns+1)*sizeof(*outputColumn));
	  outputColumn[outputColumns] = malloc(sizeof(**outputColumn)*strlen(columnName[i]));
	  str_len = strlen(columnName[i]);
	  strncpy(outputColumn[outputColumns], columnName[i], str_len-2);
	  outputColumn[outputColumns][str_len-2]=0;
	  outputColumns ++;
	}
  }
  data = malloc(sizeof(*data)*outputColumns);
  
  while (SDDS_ReadPage(&SDDS_orig)>0) {
	if (page==0) {
	  if (!SDDS_GetParameterAsDouble(&SDDS_orig, "origin1", &xmin) || !SDDS_GetParameterAsDouble(&SDDS_orig, "max_ext1", &xmax) ||
		  !SDDS_GetParameterAsDouble(&SDDS_orig, "delta1", &xinterval) || !SDDS_GetParameterAsLong(&SDDS_orig, "numPhysCells1", &xdim) ||
		  !SDDS_GetParameterAsDouble(&SDDS_orig, "origin2", &ymin) || !SDDS_GetParameterAsDouble(&SDDS_orig, "max_ext2", &ymax) ||
		  !SDDS_GetParameterAsDouble(&SDDS_orig, "delta2", &yinterval) || !SDDS_GetParameterAsLong(&SDDS_orig, "numPhysCells2", &ydim) ||
		  !SDDS_GetParameterAsDouble(&SDDS_orig, "origin3", &zmin) || !SDDS_GetParameterAsDouble(&SDDS_orig, "max_ext3", &zmax) ||
		  !SDDS_GetParameterAsDouble(&SDDS_orig, "delta3", &zinterval) || !SDDS_GetParameterAsLong(&SDDS_orig, "numPhysCells3", &zdim)) {
		SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
		exit(1);
	  }
	  SetupOutputFile(&SDDS_out1, output1, 1, outputColumns, outputColumn);
	  SetupOutputFile(&SDDS_out2, output2, 0, outputColumns, outputColumn);
	  for (i=0; i<outputColumns; i++) {
		data[i] = malloc(sizeof(**data)*zdim);
		for (j=0; j<zdim; j++)
		  data[i][j] = malloc(sizeof(***data)*ydim);
	  }
	}
	
	for (i=0; i<outputColumns; i++) {
	  for (j=1; j<=ydim; j++) {
		sprintf(tmpcol, "%s_%d", outputColumn[i], j);
		data[i][page][j-1] = NULL;
		if (!(data[i][page][j-1]=SDDS_GetColumnInDoubles(&SDDS_orig, tmpcol)))
		  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	  }
	}
	page ++;
  }
  if (!SDDS_Terminate(&SDDS_orig)) {
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
	exit(1);
  }
  if (page!=zdim) {
	free_data_memory(data, outputColumns, zdim, ydim);
	fprintf(stderr, "Error, the page number does not equal to zdim size.\n");
	exit(1);
  }
  /* write to yz output, each page has the same x */
  for (page=0; page<xdim; page++) {
	if (!SDDS_StartPage(&SDDS_out1, ydim*zdim) ||
		!SDDS_SetParameters(&SDDS_out1,SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
							"origin1", xmin, "origin2", ymin, "origin3", zmin, "max_ext1", xmax, "max_ext2", ymax, "max_ext3", zmax,
							"delta1", xinterval, "delta2", yinterval, "delta3", zinterval, "numPhysCells1", xdim,
							"numPhysCells2", ydim, "numPhysCells3", zdim, "Variable1Name", "Y", "Variable2Name", "Z",
							"ZMinimum", zmin, "ZMaximum", zmax, "ZInterval", zinterval, "ZDimension", zdim,
							"YMinimum", ymin, "YMaximum", ymax, "YInterval", yinterval, "YDimension", ydim, NULL))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	for (i=0; i<outputColumns; i++) {
	  row = 0;
	  for (j=0; j<ydim; j++) {
		for (k=0; k<zdim; k++) {
		  if (!SDDS_SetRowValues(&SDDS_out1, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, row, 
								 outputColumn[i], data[i][k][j][page], 
								 "z", k*zinterval + zmin, 
								 "y", j*yinterval + ymin, NULL))
			SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
		  row ++;
		}
	  }
	}
	if (!SDDS_WritePage(&SDDS_out1))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_Terminate(&SDDS_out1))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  /* write to xz output, each page has the same y */
  for (page=0; page<ydim; page++) {
	if (!SDDS_StartPage(&SDDS_out2, xdim*zdim) ||
		!SDDS_SetParameters(&SDDS_out2, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
							"origin1", xmin, "origin2", ymin, "origin3", zmin, "max_ext1", xmax, "max_ext2", ymax, "max_ext3", zmax,
							"delta1", xinterval, "delta2", yinterval, "delta3", zinterval, "numPhysCells1", xdim,
							"numPhysCells2", ydim, "numPhysCells3", zdim, "Variable1Name", "X", "Variable2Name", "Z",
							"ZMinimum", zmin, "ZMaximum", zmax, "ZInterval", zinterval, "ZDimension", zdim,
							"XMinimum", xmin, "XMaximum", xmax, "XInterval", xinterval, "XDimension", xdim, NULL))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
	for (i=0; i<outputColumns; i++) {
	  row = 0;
	  for (j=0; j<xdim; j++) {
		for (k=0; k<zdim; k++) {
		  if (!SDDS_SetRowValues(&SDDS_out2, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, row, 
								 outputColumn[i], data[i][k][page][j],
								 "z", k*zinterval + zmin, 
								 "x", j*xinterval+xmin, NULL))
			SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
		  row ++;
		}
	  }
	}
	if (!SDDS_WritePage(&SDDS_out2))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  free_data_memory(data, outputColumns, zdim, ydim);
  
  if (!SDDS_Terminate(&SDDS_out2))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  return 0;
}

void free_data_memory(double ****data, long outputColumns, long zdim, long ydim)
{
  long i, j, k;
  for (i=0; i<outputColumns; i++) {
	for (j=0; j<zdim; j++) {
	  for (k=0; k<ydim; k++)
		free(data[i][j][k]);
	  free(data[i][j]);
	}
	free(data[i]);
  }
  free(data);
  data = NULL;
}

void SetupOutputFile(SDDS_DATASET *SDDS_out, char *output, long yz, long outputColumns, char **outputColumn)
{
  long i;
  if (!SDDS_InitializeOutput(SDDS_out, SDDS_BINARY, 0, NULL, NULL, output))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (!SDDS_DefineSimpleParameter(SDDS_out, "origin1", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "origin2", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "origin3", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "max_ext1", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "max_ext2", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "max_ext3", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "delta1", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "delta2", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "delta3", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "numPhysCells1", NULL, SDDS_LONG) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "numPhysCells2", NULL, SDDS_LONG) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "numPhysCells3", NULL, SDDS_LONG) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "Variable1Name", NULL, SDDS_STRING) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "Variable2Name", NULL, SDDS_STRING) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "ZMinimum", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "ZMaximum", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "ZInterval", NULL, SDDS_DOUBLE) ||
	  !SDDS_DefineSimpleParameter(SDDS_out, "ZDimension", NULL, SDDS_LONG) ||
	  !SDDS_DefineSimpleColumn(SDDS_out, "z", NULL, SDDS_DOUBLE))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (yz) {
	if (!SDDS_DefineSimpleParameter(SDDS_out, "YMinimum", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "YMaximum", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "YInterval", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "YDimension", NULL, SDDS_LONG) ||
		!SDDS_DefineSimpleColumn(SDDS_out, "y", NULL, SDDS_DOUBLE) )
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  } else {
	/* xz */
	if (!SDDS_DefineSimpleParameter(SDDS_out, "XMinimum", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "XMaximum", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "XInterval", NULL, SDDS_DOUBLE) ||
		!SDDS_DefineSimpleParameter(SDDS_out, "XDimension", NULL, SDDS_LONG) ||
		!SDDS_DefineSimpleColumn(SDDS_out, "x", NULL, SDDS_DOUBLE) )
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  for (i=0; i<outputColumns; i++) {
	if (!SDDS_DefineSimpleColumn(SDDS_out, outputColumn[i], NULL, SDDS_DOUBLE))
	  SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if (!SDDS_WriteLayout(SDDS_out))
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
}
