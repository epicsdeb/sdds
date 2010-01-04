/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program matrixmult.c
 * reads two sdds files and multplies them
 * as matrices.
 * Check the number of rows and columns.
 $Log: sddsmatrixmult.c,v $
 Revision 1.12  2006/12/14 22:21:59  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.11  2005/11/04 22:46:14  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.10  2002/08/14 17:12:48  soliday
 Added Open License

 Revision 1.9  2001/01/10 19:35:40  soliday
 Standardized usage message.

 Revision 1.8  2000/09/16 00:48:06  emery
 Added items in usage message.

 Revision 1.7  1999/05/25 19:11:15  soliday
 Removed compiler warning on linux.

 Revision 1.6  1997/05/29 21:25:00  emery
 Prints out matrix dimensions when matrix dimension mismatch
 is detected.

 Revision 1.5  1997/03/13 21:51:58  emery
 Changed a SetColumn call to a SetColumnFromDoubles call.
 This will make the output file columns follow the
 type of the second input file columns.

 Revision 1.4  1997/03/04 20:54:33  borland
 Added -commute option and fixed print statements that would try to print
 null strings when using -pipe and -verbose.

 * Revision 1.3  1995/09/06  14:56:43  saunders
 * First test release of SDDS1.5
 *
 */

#include "mdb.h"
#include "scan.h"
#include "match_string.h"
#include "matlib.h"
#include "SDDS.h"

#define CLO_PIPE 0
#define CLO_VERBOSE 1
#define CLO_ASCII 2
#define CLO_REUSE 3
#define CLO_COMMUTE 4
#define COMMANDLINE_OPTIONS 5
char *commandline_option[COMMANDLINE_OPTIONS] = {
    "pipe",
    "verbose",
    "ascii",
    "reuse",
    "commute",
         };

static char *USAGE="sddsmatrixmult [<file1>] <file2> [<output>]\n\
 [-pipe=[input][,output]] \n\
 [-commute] [-reuse] [-verbose] [-ascii]\n\n\
Multiplies matrices in SDDS files file1 and file2.\n\
pipe       reads input file1 from and/or write output to a pipe.\n\
file1      SDDS file for left-hand matrix of product.\n\
file2      SDDS file for right-hand matrix of product.\n\
outputfile SDDS file for product.\n\
commute    Use file1 for right-hand matrix and file2 for left-hand\n\
           matrix.  Useful with -pipe option.\n\
reuse      If one file runs out of data pages, then reuse the last one.\n\
verbose    Write diagnosis messages to stderr.\n\
ascii      Give output file in ascii mode.\n\
\n\
L.Emery ANL (Version 2, 3/3/1997, MB.)\n";

int main(int argc, char **argv)
{
    SCANNED_ARG *s_arg;
    SDDS_TABLE input1Page, input2Page, outputPage;
    
    char *inputfile1, *inputfile2, *outputfile;
    char **Input1Column, **Input1DoubleColumn;
    char **Input2Column, **Input2DoubleColumn;
    char **OutputDoubleColumn;
    long Input1Rows, Input1DoubleColumns;
    long Input2Rows, Input2DoubleColumns;
    long OutputRows;
    int32_t Input1Columns, Input2Columns, OutputDoubleColumns;

    long i,i_arg,col, commute;
#define BUFFER_SIZE_INCREMENT 20
    MATRIX *R1, *R1Trans, *R2, *R2Trans, *R3, *R3Trans;
    long verbose;
    long ipage, ipage1, ipage2, lastPage1, lastPage2, ascii;
    unsigned long pipeFlags;
    long tmpfile_used, noWarnings;
    long reuse;
    
    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if ( argc == 1 )
        bomb(NULL, USAGE);
    inputfile1 = inputfile2 = outputfile = NULL;
    Input1DoubleColumn = Input2DoubleColumn = OutputDoubleColumn = NULL;
    Input1Rows = Input1DoubleColumns = Input2Rows = Input2DoubleColumns = OutputRows = lastPage1 = lastPage2 = 0;
    tmpfile_used=0;
    verbose=0;
    pipeFlags = 0;
    tmpfile_used = 0;
    noWarnings = 0;
    reuse = commute = ascii = 0;
    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch(match_string(s_arg[i_arg].list[0], commandline_option, COMMANDLINE_OPTIONS, 
                                UNIQUE_MATCH)) {
              case CLO_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case CLO_VERBOSE:
                verbose=1;
                break;
              case CLO_ASCII:
                ascii=1;
                break;
              case CLO_REUSE:
                reuse=1;
                break;
              case CLO_COMMUTE:
                commute = 1;
                break;
              default:
                bomb("unrecognized option given", USAGE);
                }
            }
        else {
            if (!inputfile1)
                inputfile1 = s_arg[i_arg].list[0];
            else if (!inputfile2)
                inputfile2 = s_arg[i_arg].list[0];
            else if (!outputfile)
                outputfile = s_arg[i_arg].list[0];
            else
                bomb("too many filenames given", USAGE);
            }
        }

    if (pipeFlags&USE_STDIN && inputfile1) {
        if (outputfile)
            SDDS_Bomb("too many filenames (sddsxref)");
        outputfile = inputfile2;
        inputfile2 = inputfile1;
        inputfile1 = NULL;
        }
    processFilenames("sddsmatrixmult", &inputfile1, &outputfile, pipeFlags, noWarnings, &tmpfile_used);
    if (!inputfile2)
        SDDS_Bomb("second input file not specified");
    if (commute) {
      char *ptr;
      ptr = inputfile1;
      inputfile1 = inputfile2;
      inputfile2 = ptr;
    }
    
/* initialize files */
    if( !SDDS_InitializeInput(&input1Page, inputfile1))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    Input1Column=(char**)SDDS_GetColumnNames(&input1Page, &Input1Columns);
    if( !SDDS_InitializeInput(&input2Page, inputfile2))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    Input2Column=(char**)SDDS_GetColumnNames(&input2Page, &Input2Columns);
    if( !SDDS_InitializeOutput(&outputPage, ascii?SDDS_ASCII:SDDS_BINARY, 1, 
                               "Matrix product", "Matrix product", outputfile))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

/* while either files have data to read or are at the end of the file */
    while ( (ipage1=SDDS_ReadTable(&input1Page)) && (ipage2=SDDS_ReadTable(&input2Page)) ) {
        if ( (reuse && (ipage1 < 0 && ipage2 < 0)) || 
            /* if reuse, then quit when both files are at the end */
            (!reuse && (ipage1 < 0 || ipage2 < 0)) )
          /* if not reuse, then quit as sonn as one file is at the end */
            break;
        ipage = MAX( ipage1, ipage2 );
        
        /* page of first file */
        if ( ipage1 == 1 ) {
            Input1DoubleColumns=0;
            Input1DoubleColumn=(char**)malloc(Input1Columns*sizeof(char*));
            /* count the numerical columns in input1 */
            for ( i=0; i < Input1Columns; i++) {
                if ( SDDS_NUMERIC_TYPE( SDDS_GetColumnType( &input1Page, i))) {
                    Input1DoubleColumn[Input1DoubleColumns]=Input1Column[i];
                    Input1DoubleColumns++;
                    }
                }
            if (!Input1DoubleColumns) {
                if (verbose) {
                    fprintf(stderr, "No columns in page %ld of file %s.\n",
                            ipage,inputfile1?inputfile1:"stdin");
                    }
                }
            Input1Rows=SDDS_CountRowsOfInterest(&input1Page);
            if ( Input1DoubleColumns && Input1Rows )
                m_alloc(&R1, Input1DoubleColumns, Input1Rows);
            else if (!Input1Rows) {
                if(verbose)
                    fprintf(stderr, "No rows in page %ld of file %s.\n",
                            ipage,inputfile1?inputfile1:"stdin");
                }
            }
    
        if ( ipage1 > 0 ) {
            if( Input1Rows != SDDS_CountRowsOfInterest(&input1Page) )
                fprintf(stderr, "Number of rows in page %ld of file %s changed.\n",
                        ipage, inputfile1?inputfile1:"stdin");
            for(col=0;col<Input1DoubleColumns;col++){
                if(!(R1->a[col]=(double*)SDDS_GetColumnInDoubles(&input1Page,Input1DoubleColumn[col])))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
            lastPage1 = ipage1;
            if (verbose)
                fprintf(stderr, "Using page %ld of file %s.\n",
                        lastPage1,inputfile1?inputfile1:"stdin");
            }
        else if ( ipage1 < 0 ) {
            if (verbose)
                fprintf(stderr, "Reusing page %ld of file %s.\n",
                        lastPage1,inputfile1?inputfile1:"stdin");
            }
        
        if(verbose && Input1DoubleColumns && Input1Rows) {
            m_alloc(&R1Trans,Input1Rows,Input1DoubleColumns);
            m_trans(R1Trans,R1);
            m_show(R1Trans, "%9.6le ", "Input matrix 1:\n", stderr);
            m_free(&R1Trans);
            }
        
        /* page of second file */
        if ( ipage2 == 1 ) {
            Input2DoubleColumns=0;
            Input2DoubleColumn=(char**)malloc(Input2Columns*sizeof(char*));
            /* count the numerical columns in input2 */
            for ( i=0; i < Input2Columns; i++) {
                if ( SDDS_NUMERIC_TYPE( SDDS_GetColumnType( &input2Page, i))) {
                    Input2DoubleColumn[Input2DoubleColumns]=Input2Column[i];
                    Input2DoubleColumns++;
                    }
                }
            if (!Input2DoubleColumns) {
                if (verbose) {
                    fprintf(stderr, "No columns in page %ld of file %s.\n",ipage,inputfile2);
                    }
                }
            Input2Rows=SDDS_CountRowsOfInterest(&input2Page);
            if(Input2DoubleColumns && Input2Rows)
                m_alloc(&R2, Input2DoubleColumns, Input2Rows);
            else if (!Input2Rows) {
                if(verbose)
                    fprintf(stderr, "No rows in page %ld of file %s changed.\n",ipage,inputfile2);
                }
            }

        if ( ipage2 > 0 ) {
            if( Input2Rows != SDDS_CountRowsOfInterest(&input2Page) )
                fprintf(stderr, "Number of rows in page %ld of file %s changed.\n",ipage,inputfile2);
            for(col=0;col<Input2DoubleColumns;col++){
                if(!(R2->a[col]=(double*)SDDS_GetColumnInDoubles(&input2Page,Input2DoubleColumn[col])))
                    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
                }
            lastPage2 = ipage2;
            if (verbose)
              fprintf(stderr, "Using page %ld of file %s.\n",lastPage2,
                      inputfile2?inputfile2:"stdin");
            }
        else if ( ipage2 < 0 ) {
            if (verbose)
                fprintf(stderr, "Reusing page %ld of file %s.\n",
                        lastPage2,inputfile2?inputfile2:"stdin");
            }
        
        if(verbose && Input2DoubleColumns && Input2Rows) {
            m_alloc(&R2Trans,Input2Rows,Input2DoubleColumns);
            m_trans(R2Trans,R2);
            m_show(R2Trans, "%9.6le ", "Input matrix 2:\n", stderr);
            m_free(&R2Trans);
            }
    
        
        /* define output table */
        if ( ipage == 1 ) {
            OutputRows = Input1Rows;
            OutputDoubleColumns = Input2DoubleColumns;
            if(Input1DoubleColumns != Input2Rows) {
                fprintf(stderr, "Error: dimension mismatch in files.\n");
                fprintf(stderr, "Right-hand matrix (%s) is %ldx%ld.\n",
                        inputfile2?inputfile2:"stdin", Input2Rows, Input2DoubleColumns);
                fprintf(stderr, "Left-hand matrix (%s) is %ldx%ld.\n",
                        inputfile1?inputfile1:"stdin", Input1Rows, Input1DoubleColumns);
                exit(1);
                }
            }
    
    /* do matrix multiplication */
        if(OutputRows && OutputDoubleColumns) {
            if ( ipage == 1 )
                m_alloc(&R3, OutputDoubleColumns, OutputRows);
            if (verbose) 
              fprintf(stderr, "Multiplying %d x %d matrix by %d x %d matrix\n",
                      R2->m, R2->n, R1->m, R1->n);
            m_mult(R3, R2, R1);
            if(verbose) {
                m_alloc(&R3Trans, OutputRows, OutputDoubleColumns);
                m_trans(R3Trans,R3);
                m_show(R3Trans, "%9.6le ", "Output matrix:\n", stderr);
                m_free(&R3Trans);
                }
            }
        else {
            if(verbose)
                fprintf(stderr, "Output file will either have no columns or no rows in page %ld.\n",ipage);
            }
        
        if ( ipage == 1 ) {
          for ( i=0; i < Input2DoubleColumns; i++){
            if ( 0 > SDDS_TransferColumnDefinition(&outputPage,&input2Page,Input2DoubleColumn[i],NULL))
              SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
            OutputDoubleColumn = (char**) SDDS_GetColumnNames(&outputPage,&OutputDoubleColumns);
          }
          if ( !SDDS_WriteLayout(&outputPage))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        }
        if ( !SDDS_StartTable(&outputPage,OutputRows) )
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        /* turn selected string parameters from input to string columns in output */
        /* assign values to output table */
        if(OutputRows && OutputDoubleColumns) {
            for(i=0;i<OutputDoubleColumns;i++) /* i is the row index */
                if (!SDDS_SetColumnFromDoubles(&outputPage,SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,
                               R3->a[i],OutputRows,OutputDoubleColumn[i]))
                    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);   
            }
        if (!SDDS_WriteTable(&outputPage))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);  
        }
    
    if ( !SDDS_Terminate(&input1Page) || !SDDS_Terminate(&input2Page) || 
        !SDDS_Terminate(&outputPage))
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);  
    

    if (tmpfile_used && !replaceFileAndBackUp(inputfile1, outputfile))
        exit(1);

    return(0);
}

