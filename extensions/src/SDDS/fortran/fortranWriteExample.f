C FORTRAN
      character*50 filename
      integer i
      integer j
      dimension columnNames(50)
      character*50 columnNames
      dimension parameterNames(50)
      character*50 parameterNames
      dimension parameterTypes(50)
      integer parameterTypes
      dimension columnTypes(50)
      integer columnTypes

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

      filename = "fortranWriteExample.sdds"

C     SDDS_InitializeOutput_F is passed the SDDS file pointer, data mode,
C     lines per row, description, contents, and filename. "" is interpreted
C     as NULL by the SDDS library.
      i = SDDS_InitializeOutput_F(1,SDDS_ASCII,1,"","",filename)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

      parameterNames(1) = "ShortP"
      parameterTypes(1) = SDDS_SHORT
      parameterNames(2) = "LongP"
      parameterTypes(2) = SDDS_LONG
      parameterNames(3) = "FloatP"
      parameterTypes(3) = SDDS_FLOAT
      parameterNames(4) = "DoubleP"
      parameterTypes(4) = SDDS_DOUBLE
      do j = 1,4
C     SDDS_DefineParameter_F is passed the SDDS file pointer, parameter name,
C     symbol, units, description, format_string, type, and fixed_value.
C     "" is interpreted as NULL by the SDDS library.
         i = SDDS_DefineParameter_F(1,parameterNames(j),"","","","",
     a        parameterTypes(j),"")
         if (i .LT. 0) then
            call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
            stop 1
         end if
      end do

      columnNames(1) = "ShortC"
      columnTypes(1) = SDDS_SHORT
      columnNames(2) = "LongC"
      columnTypes(2) = SDDS_LONG
      columnNames(3) = "FloatC"
      columnTypes(3) = SDDS_FLOAT
      columnNames(4) = "DoubleC"
      columnTypes(4) = SDDS_DOUBLE
      do j = 1,4
C      SDDS_DefineColumn_F is passed the SDDS file pointer, column name,
C      symbol, units, description, format_string, type, and field_length.
C      "" is interpreted as NULL by the SDDS library.
         i = SDDS_DefineColumn_F(1,columnNames(j),"","","","",
     a        columnTypes(j),0)
         if (i .LT. 0) then
            call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
            stop 1
         end if
      end do

C     SDDS_WriteLayout_F is passed the SDDS file pointer.
      i = SDDS_WriteLayout_F(1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_StartPage_F is passed the SDDS file pointer and 
C     the expected number of rows.
      i = SDDS_StartPage_F(1, 100)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if


C     SDDS_SetParameterByName_F is passed the SDDS file pointer,
C     parameter name, and the value. (Note value must be the same
C     type as the parameter)
      pshort = 1
      i = SDDS_SetParameterByName_F(1, parameterNames(1), pshort)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      plong = 2
      i = SDDS_SetParameterByName_F(1, parameterNames(2), plong)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      pfloat = 3.3
      i = SDDS_SetParameterByName_F(1, parameterNames(3), pfloat)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      pdouble = 4.4
      i = SDDS_SetParameterByName_F(1, parameterNames(4), pdouble)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_SetColumnByName_F is passed the SDDS file pointer,
C     pointer to the data array, number or rows, column name.
C     (Note value must be the same type as the column)
      cshort(1) = 1
      cshort(2) = 2
      cshort(3) = 3
      i = SDDS_SetColumnByName_F(1, cshort, 3, columnNames(1))
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      clong(1) = 3
      clong(2) = 4
      clong(3) = 5
      i = SDDS_SetColumnByName_F(1, clong, 3, columnNames(2))
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      cfloat(1) = 6.6
      cfloat(2) = 7.7
      cfloat(3) = 8.8
      i = SDDS_SetColumnByName_F(1, cfloat, 3, columnNames(3))
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      cdouble(1) = 9.9
      cdouble(2) = 10.1
      cdouble(3) = 11.1
      i = SDDS_SetColumnByName_F(1, cdouble, 3, columnNames(4))
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

      i = SDDS_WritePage_F(1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

      i = SDDS_StartPage_F(1, 100)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_SetParameterByIndex_F is passed the SDDS file pointer,
C     parameter index, and the value. (Note value must be the same
C     type as the parameter)
      pshort = 5
      i = SDDS_SetParameterByIndex_F(1, 0, pshort)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      plong = 6
      i = SDDS_SetParameterByIndex_F(1, 1, plong)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      pfloat = 7.7
      i = SDDS_SetParameterByIndex_F(1, 2, pfloat)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      pdouble = 8.8
      i = SDDS_SetParameterByIndex_F(1, 3, pdouble)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_SetColumnByIndex_F is passed the SDDS file pointer,
C     pointer to the data array, number or rows, column index.
C     (Note value must be the same type as the column)
      cshort(1) = -1
      cshort(2) = -2
      i = SDDS_SetColumnByIndex_F(1, cshort, 2, 0)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      clong(1) = -3
      clong(2) = -4
      i = SDDS_SetColumnByIndex_F(1, clong, 2, 1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      cfloat(1) = -5.5
      cfloat(2) = -6.6
      i = SDDS_SetColumnByIndex_F(1, cfloat, 2, 2)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if
      cdouble(1) = -7.7
      cdouble(2) = -8.8
      i = SDDS_SetColumnByIndex_F(1, cdouble, 2, 3)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

      i = SDDS_WritePage_F(1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

C     SDDS_Terminate_F is passed a file pointer (0 to 19)
      i = SDDS_Terminate_F(1)
      if (i .NE. 1) then
         call SDDS_PrintErrors_F(SDDS_VERBOSE_PrintErrors) 
         stop 1
      end if

      write (6,901) "data written to ",filename

 901  FORMAT (99A)
      end

