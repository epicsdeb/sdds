C     FORTRAN
      character*50 filename
      integer retval
      integer i
      integer j
      integer rows
      integer type
      integer numberOfColumns
      integer numberOfParameters
      dimension columnNames(50)
      character*50 columnNames
      dimension parameterNames(50)
      character*50 parameterNames
      integer index
      character*100 text
      character*100 contents

C     If the dimension is less then the number of rows
C     You will get invalid output
      dimension cdouble(1000)
      real*8 cdouble
      dimension cfloat(1000)
      real cfloat
      dimension clong(1000)
      integer clong
      dimension cshort(1000)
      integer*2 cshort
      real*8 pdouble
      real pfloat
      integer plong
      integer*2 pshort

      INCLUDE "SDDSFortran.h"

      filename = "fortranReadExample.sdds"

C     SDDS_InitializeInput_F is passed a file pointer (0 to 19)
C     and a filename
      i = SDDS_InitializeInput_F(1,filename)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_ColumnCount_F is passed a file pointer (0 to 19)
      numberOfColumns = SDDS_ColumnCount_F(1)

C     SDDS_ParameterCount_F is passed a file pointer (0 to 19)
      numberOfParameters = SDDS_ParameterCount_F(1)

      do i = 1,numberOfColumns
C     SDDS_GetColumnNameFromIndex_F is passed a file pointer (0 to 19)
C     and the index where the first possible index is 0
         columnNames(i) = SDDS_GetColumnNameFromIndex_F(1,i-1)
      end do

      do i = 1,numberOfParameters
C     SDDS_GetParNameFromIndex_F is passed a file pointer (0 to 19)
C     and the index where the first possible index is 0
         parameterNames(i) = SDDS_GetParNameFromIndex_F(1,i-1)
      end do

      text = SDDS_GetDescriptionText_F(1)
      contents = SDDS_GetDescriptionContents_F(1)
      write (6,901) "description text: ",text
      write (6,901) "description contents: ",contents

      index = 1

C     SDDS_ReadPage_F is passed a file pointer (0 to 19).
C     It returns the current page number or 0
      retval = SDDS_ReadPage_F(1)
      if (retval .NE. 1) then
         write (6,901) "first page of data is unreadable"
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
         stop 1
      end if

      do while (retval .GT. 0)
C     SDDS_CountRowsOfInterest_F is passed a file pointer (0 to 19)
         rows = SDDS_RowCount_F(1)
         if (rows .LT. 0) then
            write (6,901) "first page of empty"
            call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
            stop 1
         end if
         
         write (6,907) "page ",retval
         do j = 1,numberOfParameters
C     SDDS_GetParameterType_F is passed a file pointer (0 to 19) and a
C     parameter index. It returns a parameter type integer or 0 on error
            type = SDDS_GetParameterType_F(1, j-1) 
            if (type .LT. 1) then
               write (6,901) "unable to get parameter type",
     a              parameterNames(j)
               call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
               stop 1
            end if
            
            write (6,901) "parameter ", parameterNames(j)
            if (type .EQ. SDDS_SHORT) then
C     SDDS_GetParameterData_C_To_F is passed a file pointer (0 to 19), 
C     the parameter index, variable to store the data, and the desired type.
C     It returns a 0 on errors.
               i = SDDS_GetParameterData_F(1, j-1, pshort, SDDS_SHORT)
               if (i .NE. 1) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               write (6,904) pshort
            else if (type .EQ. SDDS_LONG) then
               i = SDDS_GetParameterData_F(1, j-1, plong, SDDS_LONG)
               if (i .NE. 1) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               write (6,904) plong
            else if (type .EQ. SDDS_FLOAT) then
               i = SDDS_GetParameterData_F(1, j-1, pfloat, SDDS_FLOAT)
               if (i .NE. 1) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               write (6,903) pfloat
            else if (type .EQ. SDDS_DOUBLE) then
               i = SDDS_GetParameterData_F(1, j-1, pdouble, SDDS_DOUBLE)
               if (i .NE. 1) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               write (6,903) pdouble
            end if
         end do

         do j = 1,numberOfColumns
C     SDDS_GetColumnType_F is passed a file pointer (0 to 19) and a
C     column index. It returns a column type integer or 0 on error
            type = SDDS_GetColumnType_F(1, j-1) 
            if (type .LT. 1) then
               write (6,901) "unable to read column ",
     a              columnNames(j)
               call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
               stop 1
            end if
            
            write (6,901) "column ", columnNames(j)
            if (type .EQ. SDDS_SHORT) then
C     SDDS_GetColumnData_F is passed a file pointer (0 to 19), 
C     the original type, the array variable to store the data, the length
C     of the array variable, and the desired data type.
C     It returns the number of rows on success, -1 on error.
               i = SDDS_GetColumnData_F(1,j-1,cshort,1000,SDDS_SHORT)
               if (i .LT. 0) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               do i = 0,rows-1
                  write (6,904) cshort(i+1)
               end do          
            else if (type .EQ. SDDS_LONG) then
               i = SDDS_GetColumnData_F(1,j-1,clong,1000,SDDS_LONG)
               if (i .LT. 0) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               do i = 0,rows-1
                  write (6,904) clong(i+1)
               end do            
            else if (type .EQ. SDDS_FLOAT) then
               i = SDDS_GetColumnData_F(1,j-1,cfloat,1000,SDDS_FLOAT)
               if (i .LT. 0) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               do i = 0,rows-1
                  write (6,903) cfloat(i+1)
               end do            
            else if (type .EQ. SDDS_DOUBLE) then
               i = SDDS_GetColumnData_F(1,j-1,cdouble,1000,SDDS_DOUBLE)
                if (i .LT. 0) then
                  write (6,901) "error getting C data"
                  call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors)
                  stop 1
               end if
               do i = 0,rows-1
                  write (6,903) cdouble(i+1)
               end do     
            end if
         end do
         retval = SDDS_ReadPage_F(1)
      end do

C     SDDS_Terminate_F is passed a file pointer (0 to 19)
      i = SDDS_Terminate_F(1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      write (6,904) SDDS_LONG
 901  FORMAT (99A)
 903  FORMAT (F6.2)
 904  FORMAT (I3)
 907  FORMAT (A,I3)
      end

