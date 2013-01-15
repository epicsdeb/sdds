/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*************************************************************************
 * FILE:      image2sdds.c
 * Author:    Josh Stein
 *
 * Purpose:   Program for converting a raw binary image file
 *            to a variety of SDDS formats.
 *
 * Mods:      login mm/dd/yy: description
 *
 *			stein	02/26/97: created
 *************************************************************************/
 
#include <stdio.h>
#include <string.h>
#include <SDDS.h>          /* SDDS library header */
 
#define  VERSION   "V1.2"
#define 	XDIMENSION 482			/* Default beam size in X axis*/
#define 	YDIMENSION 512			/* Default beam size in Y axis*/

/* Parameter defines */
#define	PARM_DEBUG 1
#define	PARM_XDIM 2
#define	PARM_YDIM 3
#define	PARM_TRANSPOSE 4
#define	PARM_ASCII 5
#define	PARM_HELP 6
#define	PARM_QMARK 7
#define PARM_CONTOUR 8
#define PARM_2D 9
#define	PARM_XMIN 10
#define	PARM_YMIN 11
#define	PARM_XMAX 12
#define	PARM_YMAX 13
#define PARM_MULTICOLUMNMODE 14

int asciiOutput=0; 	/* Set to 1 for ASCII sdds output, 0 for binary */
int array=0; 		/* Set to 1 for 2 dimensional array output */
int contour=0; 
int xDim=0;
int yDim=0;
double xMin=0;
double yMin=0;
double xMax=1;
double yMax=1;
int useXmax=0;
int useYmax=0;
int debug=0;
int transpose=0;
int multiColumnMode=0;

struct parm_info {
	char *parm;
	int len;
	int id;
	char *desc;
};

typedef struct parm_info PARM_INFO;

static PARM_INFO ptable[] = {
	{ "-2d",	3,	PARM_2D,	"2D_array"},
	{ "-debug",	6,	PARM_DEBUG,	"Debug_flag"},
	{ "-xdim",	5,	PARM_XDIM,	"X_Dimensions"},
	{ "-ydim",	5,	PARM_YDIM,	"Y_Dimensions"},
	{ "-transpose",10,	PARM_TRANSPOSE,"Transpose"},
	{ "-ascii",6,	PARM_ASCII,"Transpose"},
	{ "-contour",8,	PARM_CONTOUR,"Contour"},
	{ "-help",5,	PARM_HELP,"Help"},
	{ "-?",2,	PARM_QMARK,"Help"},
	{ "-xmin",      5,      PARM_XMIN,      "X_Minimum"},
	{ "-ymin",      5,      PARM_YMIN,      "Y_Minimum"},
	{ "-xmax",      5,      PARM_XMAX,      "X_Maximum"},
	{ "-ymax",      5,      PARM_YMAX,      "Y_Maximum"},
	{ "-multicolumnmode",16,PARM_MULTICOLUMNMODE,"MultiColumnMode"},
	{ NULL,	-1,	-1 }
};

/* Prototypes */
int process_cmdline_args(int argc, char *argv[], char *infile, char *outfile);
void usage(char *name);

int main(int argc,char *argv[]) {
  short *image=NULL;
  short *rotimage=NULL;
  char infile[255];
  char outfile[255];
  char xDimStr[10];
  char yDimStr[10];
  int32_t dim[2];
  FILE *infilefp;
  int x,y,i,n,j;
  SDDS_TABLE table;
  double xInterval=.02;
  double yInterval=.02;
  double *indexes=NULL;
  char **columnNames=NULL;
  int32_t *data=NULL;

  process_cmdline_args(argc,argv,infile,outfile);

  /* If user didn't specify extrema */
  if(!xDim) {
    xDim = XDIMENSION;
  }
  if(!yDim) {
    yDim = YDIMENSION;
  }

  image = (short*)malloc(sizeof(short)*xDim*yDim);
  rotimage = (short*)malloc(sizeof(short)*xDim*yDim);

  /* Open input file */
  if((infilefp = fopen(infile,"r"))==NULL) {
    fprintf(stderr,"%s: error: enable to open input file: %s\n",
            argv[0],infile);
    exit(1);
  }

  /* Read image into 1 dimensional array */
  for (x=0;x<xDim*yDim;x++) {
    image[x]=getc(infilefp);
    if (image[x] == EOF) {
      fprintf(stderr,"%s: Unexpected EOF at index %d\n",argv[0],x);
      exit(1);
    }
  }
  fclose(infilefp);
  /* Use transpose factor here to adjust image */
  i=0;
  for (x=0;x<xDim;x++) {
    for (y=0;y<yDim;y++) {
      rotimage[y*xDim+x]=image[i];
      i++;
    }
  }

  /* Begin SDDS specifics */

  if (transpose) { /* Reverse dimensions for transposed image */
    sprintf(yDimStr,"%d\0",xDim); /* Dimensions of image sent to sdds */
    sprintf(xDimStr,"%d\0",yDim);
  }
  else {
    sprintf(xDimStr,"%d\0",xDim); /* Dimensions of image sent to sdds */
    sprintf(yDimStr,"%d\0",yDim);
  }

  if(asciiOutput) {
    if (!SDDS_InitializeOutput(&table, SDDS_ASCII, 1, "ImageArray",
                               "Converted image data", outfile)) {
      fprintf(stderr,"%s: SDDS error: unable to initialize output file %s\n",
              argv[0], outfile);
      SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
    }
  }
  else {
    if (!SDDS_InitializeOutput(&table, SDDS_BINARY, 1, "ImageArray",
                               "Converted image data", outfile)) {
      fprintf(stderr,"%s: SDDS error: unable to initialize output file %s\n",
              argv[0], outfile);
      SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
    }
  }
  if (transpose) {
    if (useXmax)
      xInterval=(xMax-xMin)/(yDim-1);
    if (useYmax)
      yInterval=(yMax-yMin)/(xDim-1);
  } else {
    if (useXmax)
      xInterval=(xMax-xMin)/(xDim-1);
    if (useYmax)
      yInterval=(yMax-yMin)/(yDim-1);
  }
  if(contour) {
    if (!multiColumnMode) {
      /* Begin setting parameters for use in sddscontour tool */
      if (!SDDS_DefineParameter(&table,"Variable1Name", NULL, NULL, NULL, NULL,
                                SDDS_STRING,"x") == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define paramter 1: variable1Name\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter(&table,"Variable2Name", NULL, NULL, NULL, NULL,
                                SDDS_STRING,"y") == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define parameter 2: variable2Name\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter1(&table,"xInterval", NULL, NULL, NULL, NULL,
                                 SDDS_DOUBLE,&xInterval) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define parameter 3: xInterval\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter1(&table,"xMinimum", NULL, NULL, NULL, NULL,
                                 SDDS_DOUBLE,&xMin) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define paramter 4: xMinimum\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter(&table,"xDimension", NULL, NULL, NULL, NULL,
                                SDDS_LONG,xDimStr) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define paramter 5: xDimension\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter1(&table,"yInterval", NULL, NULL, NULL, NULL,
                                 SDDS_DOUBLE,&yInterval) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define parameter 6: yInterval\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter1(&table,"yMinimum", NULL, NULL, NULL, NULL,
                                 SDDS_DOUBLE,&yMin) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define paramter 7: yMinimum\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
      
      if (!SDDS_DefineParameter(&table,"yDimension", NULL, NULL, NULL, NULL,
                                SDDS_LONG,yDimStr) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define paramter 8: yDimension\n",
                argv[0]);
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
    }
  }
  /* Done setting parameters */
  
  if(array) {
    if (SDDS_DefineArray(&table, "ImageArray", NULL, NULL, "Intensity", NULL,
                         SDDS_CHARACTER,0,2,NULL) == -1) {
      fprintf(stderr,"%s: SDDS error: unable to define array %s\n",argv[0],
              "Image");
      SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
    }
  }
  else {
    if (!multiColumnMode) {
      if (SDDS_DefineColumn(&table, "Image", NULL, NULL, NULL, NULL,
                            SDDS_SHORT, 0) == -1) {
        fprintf(stderr,"%s: SDDS error: unable to define column %s\n",argv[0],
                "Image");
        SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
      }
    } else {
      if (SDDS_DefineSimpleColumn(&table, "Index", NULL, SDDS_DOUBLE)<0) {
        fprintf(stderr, "Problem defining column Index.\n");
        return(1);
      }
      if (transpose) {
        dim[0] = xDim; dim[1] = yDim;
      } else {
        dim[0] = yDim; dim[1] = xDim;
      }
      indexes = malloc(sizeof(double)*dim[1]);
      columnNames = malloc(sizeof(char*)*dim[0]);
      data = malloc(sizeof(int32_t)*dim[1]);
      for (i=0; i<dim[0]; i++) {
        columnNames[i] = malloc(sizeof(char)*10);
        sprintf(columnNames[i], "Line%" PRId32, i);
        if (SDDS_DefineSimpleColumn(&table, columnNames[i], NULL, SDDS_SHORT)<0) {
          fprintf(stderr, "Problem defining column %s.\n", columnNames[i]);
          return(1);
        }
      }
    }
  }

  if (!SDDS_WriteLayout(&table)) {
    fprintf(stderr,"%s: SDDS error: unable to write layout\n",argv[0]);
    SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
  }
  
  if (!multiColumnMode) {
    if (!SDDS_StartTable(&table,xDim*yDim)) {
      fprintf(stderr,"%s: SDDS error: unable to start table\n",argv[0]);
      SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
    }
  } else {
    if (!SDDS_StartTable(&table,dim[1])) {
      fprintf(stderr,"%s: SDDS error: unable to start table\n",argv[0]);
      SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
    }
  }

  if (!array) {
    if (!multiColumnMode) {
      if (transpose) {
        for (x=0;x<xDim*yDim;x++) {
          if (!SDDS_SetRowValues(&table, (SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE), x,
                                 "Image", rotimage[x], NULL)) {
            fprintf(stderr,"%s: SDDS error: unable to write row\n",argv[0]);
            SDDS_PrintErrors(stderr,SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          }			
        }
      }
      else {
        for (x=0;x<xDim*yDim;x++) {
          if (!SDDS_SetRowValues(&table, (SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE), x,
                                 "Image", image[x], NULL)) {
            fprintf(stderr,"%s: SDDS error: unable to write row\n",argv[0]);
            SDDS_PrintErrors(stderr,SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
          }			
        }
      }
    } else {
      for (i=0; i<dim[1]; i++)
        indexes[i] = xMin + xInterval*i;
      if (!SDDS_SetColumnFromDoubles(&table, SDDS_SET_BY_NAME,
                                   indexes, dim[1], "Index"))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      n=0;
      
      
      for (i=0; i<dim[0]; i++) {
        for (j=0; j<dim[1]; j++) {
          if (transpose) {
            data[j] = image[n];
          } else {
            data[j] = rotimage[n];
          }
          n++;
        }
      
        if (!SDDS_SetColumnFromLongs(&table, SDDS_SET_BY_NAME,
                                       data, dim[1], columnNames[i]))
          SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
      }
    }
  }
  else {  /* Requesting writing of 2D array */
    if (transpose) {
      dim[0] = xDim; dim[1] = yDim;
      if (SDDS_SetArray(&table, "ImageArray", SDDS_CONTIGUOUS_DATA, rotimage,
                        dim) == 0) {
        fprintf(stderr,"%s: SDDS Error - unable to stuff array\n",argv[0]);
        SDDS_PrintErrors(stderr,SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }			
    }
    else {
      dim[0] = yDim; dim[1] = xDim;
      if (SDDS_SetArray(&table, "ImageArray", SDDS_CONTIGUOUS_DATA, image,
                        dim) == 0) {
        fprintf(stderr,"%s: SDDS Error - unable to stuff array\n",argv[0]);
        SDDS_PrintErrors(stderr,SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      }			
    }
  }
  
  if (!SDDS_WriteTable(&table)) {
    fprintf(stderr,"%s: SDDS error: unable to write table\n",argv[0]);
    SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
  }
  
  if (!SDDS_Terminate(&table)) {
    fprintf(stderr,"%s: SDDS error: unable to terminate SDDS\n",argv[0]);
    SDDS_PrintErrors(stderr,(SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors));
  }
  if (image) free(image);
  if (rotimage) free(rotimage);
  if (indexes) free(indexes);
  if (data) free(data);
  if (columnNames) free(columnNames);
  return(0);
}

/*************************************************************************
 * FUNCTION : process_cmdline_args()
 * PURPOSE  : Get command line arguments and set flags appropriately.
 * ARGS in  : argc, argv - standard unix
 * ARGS out : fname - name of image file 
 * GLOBAL   : nothing
 * RETURNS  : 0
 ************************************************************************/
int
process_cmdline_args(int argc, char *argv[], char *infile, char *outfile)
  {
  int i,j;
  int cmderror = 0;
  int scan_done = 0;
	
  *infile = '\0';
  *outfile = '\0';
 
  /* Process command line options */
  if (argc < 3)
    cmderror++;
  else {
    strcpy(infile,argv[1]);
    strcpy(outfile,argv[2]);

    /* Begin parsing any optional args */
    for(i=3;i<argc && !cmderror;i++) {
      for(j=0;!scan_done && !cmderror && ptable[j].parm;j++) {
        if (strncmp(ptable[j].parm,argv[i],ptable[j].len)==0) {
          switch(ptable[j].id) {

          case PARM_DEBUG:
            if(++i>argc) cmderror++;
            else {
              if(argv[i][0]=='-') cmderror++;
              else {
                if(sscanf(argv[i],"%d",&debug)<1) cmderror++;
                else scan_done=1;
              }
            }
            break;

          case PARM_XDIM:
            if(++i>argc) cmderror++;
            else {
              if(argv[i][0]=='-') cmderror++;
              else {
                if(sscanf(argv[i],"%d",&xDim)<1) cmderror++;
                else scan_done=1;
              }
            }
            break;

          case PARM_YDIM:
            if(++i>argc) cmderror++;
            else {
              if(argv[i][0]=='-') cmderror++;
              else {
                if(sscanf(argv[i],"%d",&yDim)<1) cmderror++;
                else scan_done=1;
              }
            }
            break;

          case PARM_XMIN:
            if(++i>argc) 
              cmderror++;
            else {
              if(argv[i][0]=='-') 
                cmderror++;
              else {
                if(sscanf(argv[i],"%lf",&xMin)<1) 
                  cmderror++;
                else
                  scan_done=1;
              }
            }
            break;
          case PARM_XMAX:
            if(++i>argc) 
              cmderror++;
            else {
              if(argv[i][0]=='-') 
                cmderror++;
              else {
                if(sscanf(argv[i],"%lf",&xMax)<1) 
                  cmderror++;
                else {
                  scan_done=1;
                  useXmax=1;
                }
              }
            }
            break;
          case PARM_YMIN:
            if(++i>argc) 
              cmderror++;
            else {
              if(argv[i][0]=='-') 
                cmderror++;
              else {
                if(sscanf(argv[i],"%lf",&yMin)<1) 
                  cmderror++;
                else 
                  scan_done=1;
              }
            }
            break;
          case PARM_YMAX:
            if(++i>argc) 
              cmderror++;
            else {
              if(argv[i][0]=='-') 
                cmderror++;
              else {
                if(sscanf(argv[i],"%lf",&yMax)<1) 
                  cmderror++;
                else {
                  scan_done=1;
                  useYmax=1;
                }
              }
            }
            break;

          case PARM_TRANSPOSE:
            transpose++;
            scan_done=1;
            break;
          case PARM_MULTICOLUMNMODE:
            multiColumnMode=1;
            scan_done=1;
            break;

          case PARM_ASCII:
            asciiOutput++;
            scan_done=1;
            break;

          case PARM_HELP | PARM_QMARK:
            usage(argv[0]);
            scan_done=1;
            break;

          case PARM_CONTOUR:
            contour++;
            scan_done=1;
            break;

          case PARM_2D:
            array++;
            scan_done=1;
            break;

          default:
            cmderror++;
            break;
          }
        }
      }
      scan_done=0;
    }
  }
  if (cmderror) {
    usage(argv[0]);
    exit(1);
  }
  return(0);
}
 
/*************************************************************************
 * FUNCTION : usage()
 * PURPOSE  : Describes command line options available to client.
 * ARGS in  : char *name - name of executable program
 * ARGS out : none
 * GLOBAL   : nothing
 * RETURNS  : none
 ************************************************************************/

#define pr fprintf

void usage(char *name)
{
  pr(stderr,"Image conversion to SDDS utility %s\n",VERSION);
  pr(stderr,"-------------------------------------\n");
  pr(stderr,"usage: %s <IMAGE infile> <SDDS outfile> [optionlist...]\n",name);
  pr(stderr,"[-2d] output SDDS file as 2D array\n");
  pr(stderr,"[-ascii] write SDDS file as ascii (default is binary)\n");
  pr(stderr,"[-contour] generate SDDS headers for sddscontour tool \n");
  pr(stderr,"[-multicolumnmode]\n");
  pr(stderr,"[-?] or [-help] print this message\n");
  pr(stderr,"[-transpose] transpose image about diagonal\n");
  pr(stderr,"[-xdim <value>] X dimension of image (default = %d) \n",XDIMENSION);
  pr(stderr,"[-ydim <value>] Y dimension of image (default = %d) \n",YDIMENSION);
  pr(stderr,"[-xmin <value>] [-xmax <value>]\n");
  pr(stderr,"[-ymin <value>] [-ymax <value>]\n");
  pr(stderr,"\npurpose:\tReads the image data from file <infile>.\n");
  pr(stderr,"\t\tand write SDDS data to <outfile>.\n");
}
 

