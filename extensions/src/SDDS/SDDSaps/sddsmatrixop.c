/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/* program: sddsmatrixop
 * purpose: a general matrix operation sdds tool.
 *
 * Hairong Shang, 2003
 $Log: sddsmatrixop.c,v $
 Revision 1.13  2007/10/10 15:42:38  shang
 added -deteerminant option to compute the matrix's determinant and output it as parameter; removed the incorrect warning messages, and corrected
 definition of defintion-keeping.

 Revision 1.12  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.11  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.10  2005/11/04 22:46:15  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.9  2004/05/13 15:38:02  soliday
 Hairong added the -columnNames option. I am just adding it to cvs.

 Revision 1.8  2004/02/09 15:29:35  soliday
 Updated so that it would compile on Windows without any warning messages.

 Revision 1.7  2003/12/17 21:12:21  shang
 removed the printout statements for allocating matrices

 Revision 1.6  2003/12/17 20:57:58  shang
 removed the print statements in op_mat_mult function

 Revision 1.5  2003/12/17 20:56:10  shang
 fixed a logic error in op_mat_mult function

 Revision 1.4  2003/10/03 14:32:25  shang
 fixed the misspelling error (replaced substract by subtract)

 Revision 1.3  2003/07/29 19:27:57  shang
 added op_mat_mult() which is two times faster than m_mult() for matrix multiplication

 Revision 1.2  2003/06/19 22:25:15  shang
 added -swap option

 Revision 1.1  2003/06/19 21:47:58  shang
 generic matrix operation method, first version


*/

#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "rpn.h"
#include "matlib.h"


#define CLO_PUSH 0
#define CLO_ADD 1
#define CLO_SUBTRACT 2
#define CLO_SCALARMULTIPLY 3
#define CLO_SCALARDIVIDE 4
#define CLO_TRANSPOSE 5
#define CLO_INVERT 6
#define CLO_MULTIPLY 7
#define CLO_IDENTITY 8
#define CLO_PIPE 9
#define CLO_NEGATION 10
#define CLO_VERBOSE 11
#define CLO_DIVIDE 12
#define CLO_SWAP 13
#define CLO_COLUMNNAMES 14
#define CLO_DETERMINANT 15
#define N_OPTIONS 16

/*note: only -pipe=out is valid */
char *option[N_OPTIONS] = {"push","add","subtract","scalarmultiply","scalardivide",
"transpose","invert","multiply","identity","pipe","negation", "verbose","divide","swap", "columnNames", "determinant"} ;

#define BINARYOPTIONS 5
char *binaryOption[BINARYOPTIONS]={
  "add","subtract","multiply","divide","swap",
};

/*here, the unary option just mean that only one matrix is needed to be poped from the stack */
#define UNARYOPTIONS 7
char *unaryOption[UNARYOPTIONS]={
  "scalarmultiply","scalardivide","transpose","invert","identity", "negation", "determinant"
}
;

char *USAGE = "sddsmatrixop [input_matrix] [output_matrix] [-pipe=in|out] [-verbose] \n\
{[-push=<matrix>] [-multiply]|[-add]|[-subtract]|[-invert]...} \n\
[-columnNames=filename=<file>[,column=<column>]] [-determinant] \n\
<input_matrix>       SDDS file which contains the input matrix -- the first element \n\
                     in the stack.\n\
<output_matrix>      The result matrix is written into SDDS file named by <output_matrix>.\n\
-push=<matrix>       the matrix (file) that will pushed into stack. \n\
-add                 addition operator, other operator are also named by their meanings. \n\
                     such as multiply,invert,transpose,negation, ect. \n\
-identity            means push a unit matrix on the stack which has the same demension as \n\
                     top top matrix in the statck. \n\
-identity=<number>   push a square unit matrix with dimension of <number> into stack.\n\
-swap                swap the top two matrices in the stack. \n\
-multiply            normal matrix-matrix multiplication. \n\
-multiply=hadamard   element-by-element multiplication, similar to addition but \n\
                     with * operator. \n\
-divide=hadamard     element-by-element division. \n\
-columnNames=filename=<filename>[,column=<column>] \n\
                     This takes column <column> from <filename>. The default for <column> is \n\
                     ColumnName. This <column> supplies the output column names. \n\
-scalarmultiply=<value> multiply the matrix by a constant value. \n\
-scalardivide=<value>   divide the matrix by a constact value. \n\
-determinant         compute the determinant of final matrix and write to the output file \n\
                     \"Determinant\" parameter.\n\
The -push and operator can be repeated many times. \n\n\
sddsmatrixop provides a generic method for matrix operations.\n\
Program by Hairong Shang. (This is version 1.0, June 2003)\n";


#define MAX_PAGES 256 /*the number of pages in a file, each page is an element of an array stack */
long matrixStack[MAX_PAGES][STACKSIZE];
long matrixStackPtr[MAX_PAGES];
/*each page of files is a stack, each stack has a maximum number size of STACKSIZE */
/*the array size of stack is number of pages in the input file */
static long input_pages; /*number of pages in the input file */

void push_matrix(char *fileName, char ***doubleColumnName,long *doubleColumnNames,long isInput);
long h_mult(MATRIX *C, MATRIX *A, MATRIX *B);
long h_divide(MATRIX *C, MATRIX *A, MATRIX *B);
long push_address(long address, long index);
long pop_address(long index);
/*this matrix multiplication is faster than m_mult by 3 times*/
long op_mat_mult(MATRIX *A, MATRIX *Bt, MATRIX *C);

int main(int argc, char **argv)
{
  SCANNED_ARG *s_arg;
  SDDS_TABLE outputPage,columnPage;
  char *inputFile,*outputFile,*matrixA,*matrixB, columnName[256],**doubleColumnName,*columnNameFile,**givenColumnName,*columnNameFileColumn;
  unsigned long pipeFlags,dummyFlags;
  long currentOption, tmpfile_used,i,j,i_arg,n,identityN,keepDefinition,doubleColumns,givenColumnNames, do_determinant;
  double scalarValue, det;
  MATRIX **A,**B, **temp;
  short isBinary, isUnary,verbose=0;
  char format[100];
  long operationsSupplied=0;
  
  do_determinant=0;
  input_pages=0;
  columnNameFileColumn=NULL;
  doubleColumnName=NULL;
  doubleColumns=0;
  inputFile=outputFile=NULL;
  A=B=temp=NULL;
  pipeFlags=0;
  tmpfile_used=0;
  dstackptr = 0;
  sstackptr=0;
  keepDefinition=1;
  matrixA=matrixB=NULL;
  columnNameFile=NULL;
  givenColumnName=NULL;
  givenColumnNames=0;
  
  for (i=0;i<MAX_PAGES-1;i++)
    matrixStackPtr[i]=0;
  SDDS_RegisterProgramName(argv[0]);
  SDDS_CheckDatasetStructureSize(sizeof(SDDS_DATASET));

  argc = scanargs(&s_arg, argc, argv);
  if (argc<3) 
    bomb(NULL, USAGE);
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    isBinary=isUnary=0;
    if (s_arg[i_arg].arg_type==OPTION) {
      if (input_pages && !A && !B) {
        A=(MATRIX**)malloc(sizeof(*A)*input_pages);
        B=(MATRIX**)malloc(sizeof(*B)*input_pages);
        temp=(MATRIX**)malloc(sizeof(*temp)*input_pages);
      }
      delete_chars(s_arg[i_arg].list[0], "_");
      currentOption=match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0);
      if (match_string(s_arg[i_arg].list[0],binaryOption,BINARYOPTIONS,0)>=0) {
        isBinary=1;
        if (sstackptr<2) {
          fprintf(stderr,"Too few values on string stack.\n");
          exit(1);
        }
        matrixB=pop_string();
        matrixA=pop_string();
        for (i=0;i<input_pages;i++) {
          if (!(B[i]=(MATRIX *)pop_address(i)) || !(A[i]=(MATRIX *)pop_address(i)))
            exit(1);
        }
      } else if (match_string(s_arg[i_arg].list[0],unaryOption,UNARYOPTIONS,0)>=0) {
        isUnary=1;
        if (sstackptr<1) {
          fprintf(stderr,"Too few values on string stack.\n");
          exit(1);
        }
        matrixB=pop_string();
        for (i=0;i<input_pages;i++) 
          if (!(B[i]=(MATRIX *)pop_address(i)))
            exit(1);
      }
      switch (currentOption) {
      case CLO_COLUMNNAMES:
        if (s_arg[i_arg].n_items<2)
          SDDS_Bomb("invalid -columnNames option!");
        s_arg[i_arg].list +=1;
        s_arg[i_arg].n_items -=1;
        if (s_arg[i_arg].n_items>0 &&
            !scanItemList(&dummyFlags, s_arg[i_arg].list, &s_arg[i_arg].n_items, 0,
                          "filename", SDDS_STRING, &columnNameFile, 1, 0,
                          "column", SDDS_STRING, &columnNameFileColumn, 1, 0,NULL))
          SDDS_Bomb("invalid -parameter syntax");
        if (!columnNameFileColumn) 
          SDDS_CopyString(&columnNameFileColumn,"ColumnName");
        s_arg[i_arg].list -=1;
        s_arg[i_arg].n_items +=1;
        break;
      case CLO_PUSH:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -push option!");
        push_matrix(s_arg[i_arg].list[1],&doubleColumnName,&doubleColumns,0);
        push_string(s_arg[i_arg].list[1]);
        break;
      case CLO_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        switch(pipeFlags) {
        case USE_STDOUT:
          break;
        default:
          push_matrix(NULL,&doubleColumnName,&doubleColumns,1);
          push_string("stdin");
          break;
        }
        break;
      case CLO_ADD:
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],A[i]->n,A[i]->m);
          if (!m_add(temp[i],A[i],B[i])) {
            fprintf(stderr,"The dimension of file %s and %s at page %ld does not match for addtion!\n",
                    matrixA,matrixB,i+1);
            exit(1);
          }
        }
        break;
      case CLO_MULTIPLY:
        if (s_arg[i_arg].n_items==1) {
          for (i=0;i<input_pages;i++) {
            m_alloc(&temp[i],B[i]->n,A[i]->m); 
            if (!op_mat_mult(temp[i],B[i],A[i])) {
              fprintf(stderr,"The dimension of file %s and %s at page %ld does not match for multiplication!\n",
                        matrixA,matrixB,i+1);
              exit(1);
            } 
            /*replace the above function by libary function to test its correctness 
            if (!m_mult(temp[i],B[i],A[i])) {
              fprintf(stderr,"The dimension of file %s and %s at page %d does not match for multiplication!\n",
                      matrixA,matrixB,i+1);
              exit(1);
            } */
          }
          keepDefinition = 0;
        } else if (s_arg[i_arg].n_items==2) {
          n=strlen(s_arg[i_arg].list[1]);
          if (strncmp(s_arg[i_arg].list[1],"hadamard",n))
            SDDS_Bomb("Invalid -multiply=<value> option!");
          for (i=0;i<input_pages;i++) {
            m_alloc(&temp[i],A[i]->n,A[i]->m);
            if (!h_mult(temp[i],A[i],B[i])) {
              fprintf(stderr,"The dimension of file %s and %s at page %ld should have same dimensions for element-by-element multiplication!\n",
                      matrixA,matrixB,i+1);
              exit(1);
            }
          }
        } else 
          SDDS_Bomb("Invald -multiply option, should be -multiply or -multiply=hadamard!");
        break;
      case CLO_DIVIDE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -divide option!");
        n=strlen(s_arg[i_arg].list[1]);
        if (strncmp(s_arg[i_arg].list[1],"hadamard",n))
          SDDS_Bomb("Invalid -divide option!");
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],A[i]->n,A[i]->m);
          if (!h_divide(temp[i],A[i],B[i])) {
            fprintf(stderr,"The dimension of file %s and %s at page %ld should have same dimensions for element-by-element division!\n",
                    matrixA,matrixB, i+1);
            exit(1);
          }
        }
        break;
      case CLO_SUBTRACT:
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],B[i]->n,B[i]->m);
          m_scmul(B[i],B[i],-1.0);
          if (!m_add(temp[i],A[i],B[i])) {
            fprintf(stderr,"The dimension of file %s and %s at page %ld does not match for subtraction!\n",
                    matrixA,matrixB, i+1);
            exit(1);
          }
        }
        break;
      case CLO_SWAP:
        for (i=0;i<input_pages;i++) {
          push_address((long)B[i],i);
          push_address((long)A[i],i);
        }
        push_string(matrixB);
        push_string(matrixA);
        keepDefinition = 0;
        break;
      case CLO_SCALARMULTIPLY:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -scalarmultiply option!");
        if (!(get_double(&scalarValue,s_arg[i_arg].list[1])))
          SDDS_Bomb("no value given for option -scalarmultiply");
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],B[i]->n,B[i]->m);
          m_scmul(temp[i],B[i],scalarValue);
        }
        break;
      case CLO_SCALARDIVIDE:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("invalid -scalardivide option!");
        if (!(get_double(&scalarValue,s_arg[i_arg].list[1])))
          SDDS_Bomb("no value given for option -scalardivide");
        if (!scalarValue)
          SDDS_Bomb("Matrix is divided by zero!");
        scalarValue=1.0/scalarValue;
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],B[i]->n,B[i]->m);
          m_scmul(temp[i],B[i],scalarValue);
        }
        break;
      case CLO_TRANSPOSE:
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],B[i]->m,B[i]->n);
          m_trans(temp[i],B[i]);
        }
        keepDefinition = 0;
        break;
      case CLO_INVERT:
        for (i=0;i<input_pages;i++) {
          if (B[i]->m != B[i]->n) {
            fprintf(stderr,
                    "Error: Page %ld of %s is not square (i.e. rows!=columns), can not be inverted!\n",
                    i+1,matrixB);
            exit(1);
          }
          m_alloc(&temp[i],B[i]->m,B[i]->m);
          m_invert(temp[i],B[i]);
        }
        keepDefinition = 0;
        break;
      case CLO_NEGATION:
        for (i=0;i<input_pages;i++) {
          m_alloc(&temp[i],B[i]->n,B[i]->m);
          m_scmul(temp[i],B[i], -1);
        }
        break;
      case CLO_IDENTITY:
        if (s_arg[i_arg].n_items==1) {
          /*the indentity matrix has the same dimension as the top matrix */
          for (i=0;i<input_pages;i++) {
            m_alloc(&temp[i],B[i]->n,B[i]->m);
            m_identity(temp[i]);
          }
          push_string(matrixB);
        } else if (s_arg[i_arg].n_items==2) {
          if (!(get_long(&identityN,s_arg[i_arg].list[1])))
            SDDS_Bomb("invalid -identity option");
          for (i=0;i<input_pages;i++) {
            m_alloc(&temp[i],identityN,identityN);
            m_identity(temp[i]);
          }
        } else 
          SDDS_Bomb("invalid -identity option");
        /*push back the poped matrix */
        for (i=0;i<input_pages;i++) 
          push_address((long)B[i],i); 
        push_string(matrixB);
        break;
      case CLO_DETERMINANT:
        do_determinant=1;
        for (i=0;i<input_pages;i++) 
          temp[i] = B[i];
        operationsSupplied++;
        break;
      case CLO_VERBOSE:
        verbose=1;
        break;
      default:
        fprintf(stderr,"option %s is invalid!\n",s_arg[i_arg].list[0]);
        exit(1);
        break;
      }
      if (isBinary && currentOption!=CLO_SWAP) {
        for (i=0;i<input_pages;i++) {
          m_free(&B[i]);
          m_free(&A[i]);
          free(B[i]);
          free(A[i]);
        }
        free(matrixA);
        free(matrixB);
        operationsSupplied++;
      }
      if (isUnary && currentOption!=CLO_IDENTITY && !do_determinant) {
        for (i=0;i<input_pages;i++) {
          m_free(&B[i]);
          free(B[i]);
        }
        free(matrixB);
        operationsSupplied++;
      }
      if ((isBinary || isUnary) && currentOption!=CLO_SWAP) {
        for (i=0;i<input_pages;i++) 
          push_address((long)temp[i],i);
        push_string("Intermediate Matrix");
      }
    }
    else {
      if (inputFile==NULL && !pipeFlags) {
        inputFile=s_arg[i_arg].list[0];
        push_matrix(inputFile,&doubleColumnName,&doubleColumns,1);
        push_string(inputFile);
      }
      else if (outputFile==NULL) 
        outputFile=s_arg[i_arg].list[0];
      else {
        fprintf(stderr, "argument %s is invalid: too many filenames (sddsmatrixop)\n", 
                s_arg[i_arg].list[0]);
        exit(1);
      }
    }
  }
  processFilenames("sddsmatrixop",&inputFile,&outputFile,pipeFlags,1,&tmpfile_used);
  if (inputFile && tmpfile_used && verbose)
    fprintf(stderr,"Warning: input file %s will be overwritten.\n",inputFile);
  if (!operationsSupplied) {
    fprintf(stderr,"No operations performed on matrices!\n");
    exit(1);
  }
  if (columnNameFile) {
    if( !SDDS_InitializeInput(&columnPage,columnNameFile))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (0>SDDS_ReadPage(&columnPage) )
      SDDS_Bomb("Unable to read column file!");
    givenColumnNames=SDDS_CountRowsOfInterest(&columnPage);
    if (givenColumnNames) {
      if (!(givenColumnName=(char**)SDDS_GetColumn(&columnPage,columnNameFileColumn)))
        SDDS_Bomb("Unable to get the ColumnName from column name file!");
    }
    if (!SDDS_Terminate(&columnPage))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  if( !SDDS_InitializeOutput(&outputPage,SDDS_BINARY, 1,NULL, NULL, outputFile))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (do_determinant && !SDDS_DefineSimpleParameter(&outputPage, "Determinant", NULL, 
                                                  SDDS_DOUBLE))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (j=0;j<input_pages;j++) {
    if (!j) {
      if (givenColumnName) {
        for (i=0; i<temp[j]->n; i++) {
          if (i<givenColumnNames)
            sprintf(columnName,"%s",givenColumnName[i]);
          else 
            sprintf(columnName,"doubleColumn%ld",i);
          if (0>SDDS_DefineColumn(&outputPage,columnName,NULL,NULL,NULL,NULL,SDDS_DOUBLE,0))
            SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
      } else {
        if (!keepDefinition) {
          sprintf(format,"doubleColumn%%0%ldld",(long)log10(temp[j]->n)+1);
          for (i=0;i<temp[j]->n;i++) {
            sprintf(columnName,format,i);
            if (0>SDDS_DefineColumn(&outputPage,columnName,NULL,NULL,NULL,NULL,SDDS_DOUBLE,0))
              SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        } else {
          if (temp[j]->n !=doubleColumns) {
            fprintf(stderr,"The double columns is not the same as matrix columns.\n");
            exit(1);
          }
          for (i=0;i<doubleColumns;i++) {
            if (0>SDDS_DefineColumn(&outputPage,doubleColumnName[i],NULL,NULL,NULL,NULL,SDDS_DOUBLE,0))
              SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
          }
        }
      }
      if (!SDDS_WriteLayout(&outputPage))
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    if (!SDDS_StartPage(&outputPage,temp[j]->m))
      SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (do_determinant) {
      det = m_det(temp[j]);
      if (!SDDS_SetParameters(&outputPage, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                              "Determinant", det, NULL))
        SDDS_PrintErrors(stdout, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    for (i=0;i<temp[j]->n;i++) {
      if (givenColumnName && givenColumnNames) {
        if (i<givenColumnNames)
          sprintf(columnName,"%s",givenColumnName[i]);
        else 
          sprintf(columnName,"doubleColumn%ld",i);
        if (!SDDS_SetColumn(&outputPage,SDDS_SET_BY_NAME,temp[j]->a[i],temp[j]->m,columnName))
          SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
      } else {
        if (!keepDefinition) {
          sprintf(columnName,format,i);
          if (!SDDS_SetColumn(&outputPage,SDDS_SET_BY_NAME,temp[j]->a[i],temp[j]->m,columnName))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        } else {
          if (!SDDS_SetColumn(&outputPage,SDDS_SET_BY_NAME,temp[j]->a[i],temp[j]->m,doubleColumnName[i]))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        }
      }
    }
    if (!SDDS_WritePage(&outputPage))
      SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  
    m_free(&temp[j]);
    free(temp[j]);
  }
  if (!!SDDS_Terminate(&outputPage))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  if (tmpfile_used && !replaceFileAndBackUp(inputFile, outputFile)) 
    exit(1);
  /*free_scanargs has errors in purify mode */
  free_scanargs(&s_arg,argc); 
  free(A);
  free(B);
  free(temp);
  if (doubleColumns) {
    for (i=0;i<doubleColumns;i++)
      free(doubleColumnName[i]);
    free(doubleColumnName);
  }
  if (columnNameFileColumn)
    free(columnNameFileColumn);
  if (columnNameFile)
    free(columnNameFile);
  if (givenColumnNames) {
    for (i=0;i<givenColumnNames;i++)
      free(givenColumnName[i]);
    free(givenColumnName);
  }
  return 0;
}

void push_matrix(char *fileName,char ***doubleColumnName,long *doubleColumns,long isInput)
{
  SDDS_DATASET inputPage;
  char **InputColumn,**InputDoubleColumn;
  long i, inputRows, ipage,InputDoubleColumns=0,pages;
  int32_t InputColumns;
  MATRIX *matrix=NULL, *matrix1;

  /* note that the matrix loaded from input file is actually the transpose matrix of it should be 
     to save time. The operations are done between tranposed matrix;
     the benefit is that: no transpose need to be done when write to output file;
     while in previose version, readfrom input -->transpose -->operation --> transpose --> output
     now, readfrom input-->operation-->output, should be much more efficient */
  /* for subtraction, addition, hadamard multiplya and division, the operation order are the same 
     however, for C=A*B, Ct = Bt*At */
  if( !SDDS_InitializeInput(&inputPage, fileName))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  InputColumn=(char**)SDDS_GetColumnNames(&inputPage, &InputColumns);
  InputDoubleColumn=(char**)malloc(InputColumns*sizeof(char*));
  if (isInput)
    *doubleColumnName=(char**)malloc(InputColumns*sizeof(char*));
  /* count the numerical columns in input */
  pages=0;
  while ((ipage=SDDS_ReadTable(&inputPage))>0) {
    if (ipage==1) {
      InputDoubleColumns=0;
      for ( i=0; i < InputColumns; i++) {
        if ( SDDS_NUMERIC_TYPE( SDDS_GetColumnType( &inputPage, i))) {
          InputDoubleColumn[InputDoubleColumns]=InputColumn[i];
          if (isInput)
            SDDS_CopyString(&(*doubleColumnName)[InputDoubleColumns],InputColumn[i]);
          InputDoubleColumns++;
        }
      }
      if (isInput)
        *doubleColumns=InputDoubleColumns;
      if (!InputDoubleColumns) {
        fprintf(stderr, "No columns in page %ld of file %s.\n",
                ipage,fileName?fileName:"stdin");
      }
    }
    inputRows=SDDS_CountRowsOfInterest(&inputPage);
    if (!inputRows) {
      fprintf(stderr,"No date found in file %s\n", fileName?fileName:"stdin");
      exit(1);
    }
    matrix=(MATRIX*)tmalloc(sizeof(*matrix));
    matrix->a=(double**)tmalloc(sizeof(double*)*InputDoubleColumns);
    matrix->n=InputDoubleColumns;
    matrix->m=inputRows;
    for (i=0;i<InputDoubleColumns;i++) {
      if (!(matrix->a[i]=(double*)SDDS_GetColumnInDoubles(&inputPage,InputDoubleColumn[i])))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }
    push_address((long)matrix,pages);
    pages++;
    if (pages>MAX_PAGES) {
      fprintf(stderr,"The number of pages in %s file exceeds 256!\n",fileName?fileName:"stdin");
      exit(1);
    }
  }
  
  if (input_pages==0) 
    input_pages=pages;
  else if (pages>input_pages) {
    fprintf(stderr,"Warning: file %s has more pages than the input file, the extra pages will be ignored.\n",fileName);
  } else if (pages<input_pages) {
    fprintf(stderr,"Warning: file %s has less pages than the input, its last page will be reused!\n",fileName);
    for (ipage=pages;i<input_pages;i++) {
      m_alloc(&matrix1,matrix->n,matrix->m);
      m_copy(matrix1,matrix);
      push_address((long)matrix1,ipage);
    }
  }
  if (!SDDS_Terminate(&inputPage))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  for (i=0;i<InputColumns;i++)
    free(InputColumn[i]);
  free(InputColumn);
  free(InputDoubleColumn);  
}


long h_mult(MATRIX *C, MATRIX *A, MATRIX *B)
{
  long i, j;
  long n, m;
  double *a_i, *c_i;
  
  if ((m=A->m)!=B->m || (n=A->n)!=B->n || m!=C->m || n!=C->n) 
    return(0);
  for (i=0; i<n; i++) {
    a_i = A->a[i];
    c_i = C->a[i];
    for (j=0; j<m; j++) 
      c_i[j]= a_i[j]*B->a[i][j];
  }
  return(1);
}


long h_divide(MATRIX *C, MATRIX *A, MATRIX *B)
{
  long i, j;
  long n, m;
  double *a_i, *c_i;
  
  if ((m=A->m)!=B->m || (n=A->n)!=B->n || m!=C->m || n!=C->n) 
    return(0);
  for (i=0; i<n; i++) {
    a_i = A->a[i];
    c_i = C->a[i];
    for (j=0; j<m; j++) {
     /* if (!B->a[i][j])
        c_i[j]=DBL_MAX;
      else */
        c_i[j]= a_i[j]/B->a[i][j];
    }
  }
  return(1);
}

long push_address(long address, long index)
{
  if (index>=MAX_PAGES) {
    fputs("The index of stack exceeds the MAX_PAGES\n", stderr);
    return(0);
  }
  if (matrixStackPtr[index]>=STACKSIZE) {
    fputs("stack overflow--numeric stack size exceeded (push_address)\n", stderr);
    return(0);
  }
  matrixStack[index][matrixStackPtr[index]++] = address;
  return 1;
}

long pop_address(long index)
{
  if (index>=MAX_PAGES) {
    fputs("The index of stack exceeds the MAX_PAGES\n", stderr);
    return(0);
  }
  if (matrixStackPtr[index]<1) {
    fprintf(stderr,"too few items on matrix stack %ld\n", index);
    return(0);
  }
  return(matrixStack[index][--matrixStackPtr[index]]);
}


/*convert matrix B into an array by column major order,
  then multiply with matrix A 
  this method is tested to be 2 times faster than m_mult function*/
long op_mat_mult(MATRIX *C, MATRIX *A, MATRIX *B)
{
  register long i, j, k;
  register double *a_i, *c_i,c_i_j;
  long n,m,p;
  double *b_j;
  
  if ((m=A->m)!=B->n || (n=A->n)!=C->n || (p=B->m)!=C->m) 
    return(0);
  
  b_j=(double*)malloc(sizeof(*b_j)*m*p);
  for (j=0;j<p;j++)
    for (i=0;i<m;i++)
      b_j[i+j*m]=B->a[i][j];
  
  for (i=0; i<n; i++) {
    a_i = A->a[i];
    c_i = C->a[i];
    for (j=0; j<p; j++) {
      c_i_j = 0;
      for (k=0; k<m; k++) 
        c_i_j += a_i[k]*b_j[k+j*m];
      c_i[j]=c_i_j;
    } 
  }
  free(b_j);
  return 1;
}

