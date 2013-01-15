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
 Revision 1.2  2001/08/29 18:08:40  borland
 Added a 3-D array plus nonequispaced y values.

 Revision 1.1  2001/08/29 17:56:02  borland
 First version.

 Revision 1.3  1995/12/12 04:26:45  borland
 Added SDDS_SetNoRowCounts() routine, plus support for writing and appending
 to files with no_row_counts=1.

 * Revision 1.2  1995/09/05  21:11:28  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "mdb.h"
#include "SDDS.h"

#define NX 40
#define NY 200
#define NZ 5

main()
{
  long d2[2] = {NX, NY}, dx[1] = {NX}, dy[1] = {NY}, dz[1] = {NZ};
  long d3[3] = {NX, NY, NZ }  ;
  double **p2, *x, *y, ***p3, *z;
  long i, j, k;
  SDDS_DATASET SDDS_dataset;

  /* make pointer arrays that index the data--note that the data
   * is NOT copied here.  All I'm doing is setting up multi-level
   * pointer arrays into the existing data.
   */
  if (!(p2=(double**)zarray_2d(sizeof(double), d2[0], d2[1])) ||
      !(p3=(double***)zarray_2d(sizeof(**p3), d3[0], d3[1])) ||
      !(x = malloc(sizeof(double)*d2[0])) || !(y=malloc(sizeof(double)*d2[1])) ||
      !(z = malloc(sizeof(double)*d3[2]))) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }
  
  for (i=0; i<NX; i++) {
    for (j=0; j<NY; j++) {
      if (!(p3[i][j] = malloc(sizeof(***p3)*NZ)))
        bomb("memory allocation failure", NULL);
    }
  }

  /* set up to put data into file "atest.out" */
  if (!SDDS_InitializeOutput(&SDDS_dataset, SDDS_ASCII, 1, "description 1", "description 2", "atest.out")) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }

  /* define double arrays */
  if (SDDS_DefineArray(&SDDS_dataset, "p2", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0, 2, NULL)<0 ||
      SDDS_DefineArray(&SDDS_dataset, "p3", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0, 3, NULL)<0 ||
      SDDS_DefineArray(&SDDS_dataset, "x", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0, 1, NULL)<0 ||
      SDDS_DefineArray(&SDDS_dataset, "y", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0, 1, NULL)<0 ||
      SDDS_DefineArray(&SDDS_dataset, "z", NULL, NULL, NULL, NULL, SDDS_DOUBLE, 0, 1, NULL)<0) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }
  /* write the layout and give SDDS the data for the first page */
  if (!SDDS_SetNoRowCounts(&SDDS_dataset, 1)) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }

  for (i=0; i<d2[0]; i++) {
    for (j=0; j<d2[1]; j++) {
      x[i] = (1.0*i)/d2[0];
      y[j] = sqr(sqr((1.0*j)/d2[1]));
      
      p2[i][j] = x[i] + y[j]*y[j];
    }
  }

  for (i=0; i<d3[0]; i++) {
    for (j=0; j<d3[1]; j++) {
      for (k=0; k<d3[2]; k++) {
        z[k] = k;
        p3[i][j][k] = (k+1)*(x[i] + y[j]*y[j]);
      }
    }
  }
  
  if (!SDDS_WriteLayout(&SDDS_dataset) || !SDDS_StartPage(&SDDS_dataset, 0) ||
      !SDDS_SetArray(&SDDS_dataset, "p2", SDDS_POINTER_ARRAY, p2, d2) ||
      !SDDS_SetArray(&SDDS_dataset, "p3", SDDS_POINTER_ARRAY, p3, d3) ||
      !SDDS_SetArray(&SDDS_dataset, "x", SDDS_POINTER_ARRAY, x, dx) ||
      !SDDS_SetArray(&SDDS_dataset, "y", SDDS_POINTER_ARRAY, y, dy) ||
      !SDDS_SetArray(&SDDS_dataset, "z", SDDS_POINTER_ARRAY, z, dz)) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }
  /* write out the data */ 
  if (!SDDS_WritePage(&SDDS_dataset) || !SDDS_Terminate(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }

  if (!SDDS_Terminate(&SDDS_dataset)) {
    SDDS_PrintErrors(stderr, 1);
    exit(1);
  }
  fprintf(stderr, "No errors detected\n");
}
