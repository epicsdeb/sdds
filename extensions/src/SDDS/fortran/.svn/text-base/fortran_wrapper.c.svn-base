/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.11  2010/10/31 19:42:46  soliday
 Added 64bit support.

 Revision 1.10  2009/02/24 16:51:29  soliday
 Updated to work with SDDS_USHORT and SDDS_ULONG

 Revision 1.9  2002/08/29 21:32:23  soliday
 Fixed some minor problems

 Revision 1.8  2002/07/01 19:51:06  soliday
 Added a few more procedure definitions.

 Revision 1.7  2000/07/06 16:51:45  soliday
 Procedure names have been shortened to 31 characters to work with f90

 Revision 1.6  2000/05/19 19:19:33  soliday
 Now defines _WINDOWS_ when running on Windows because this will stop
 mdb.h from including windows.h

 Revision 1.5  2000/01/10 16:34:14  soliday
 Added SDDS_SetParameterStringByName_F used for String parameters only

 Revision 1.4  1999/11/03 16:38:54  soliday
 Changed the new procedures so that they would work on most operating systems

 Revision 1.3  1999/11/03 16:24:45  soliday
 Added some new routines to read the data files

 Revision 1.2  1999/10/22 20:09:54  soliday
 This version now works with Linux with g77 installed.

 Revision 1.1  1999/10/21 18:53:20  soliday
 First version

 *
 *  This file enables fortran users access to most of the SDDS library
 *  functions. Instead of passing SDDS_DATASET variables back and forth,
 *  this library declares an array of 20 data sets. The fortran code only
 *  need to pass the index of the data set.
 *
 *  This version is currently missing many of the sdds libraries that 
 *  accept an arbitrary number of arguments. These can be added in the
 *  future if they are needed.
 */

#if defined(_WIN32)
#define _WINDOWS_
#endif
#include "cfortran.h"
#include "mdb.h"
#include "SDDS.h"

SDDS_DATASET dataset_f[20];
short *returnValueShort_f;
unsigned short *returnValueUShort_f;
int32_t *returnValueLong_f;
uint32_t *returnValueULong_f;
float *returnValueFloat_f;
double *returnValueDouble_f;
void *returnValueVoid_f;

/* SDDS_InitializeAppend
   Returns: 0 on error
            1 on success */ 
int SDDS_InitializeAppend_F(int fileIndex, char *filename)
{
  return SDDS_InitializeAppend(&dataset_f[fileIndex], filename);
}
FCALLSCFUN2(INT,SDDS_InitializeAppend_F,SDDS_INITIALIZEAPPEND_F,sdds_initializeappend_f,INT,STRING)

/* SDDS_InitializeOutput
   Returns: 0 on error
            1 on success */
int SDDS_InitializeOutput_F(int fileIndex, int data_mode, int lines_per_row, char *description, char *contents, char *filename)
{
  if (description) 
    if (strlen(description) == 0) 
      description = NULL;
  if (contents) 
    if (strlen(contents) == 0)
      contents = NULL;
  return SDDS_InitializeOutput(&dataset_f[fileIndex], data_mode, lines_per_row, description, contents, filename);
}
FCALLSCFUN6(INT,SDDS_InitializeOutput_F,SDDS_INITIALIZEOUTPUT_F,sdds_initializeoutput_f,INT,INT,INT,STRING,STRING,STRING)

/* SDDS_Terminate
   Returns: 0 on error
            1 on success */
int SDDS_Terminate_F(int fileIndex)
{
  return SDDS_Terminate(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_Terminate_F,SDDS_TERMINATE_F,sdds_terminate_f,INT)

/* SDDS_SetTerminateMode
   Sets TerminateMode to:
   TERMINATE_DONT_FREE_ARRAY_STRINGS+TERMINATE_DONT_FREE_TABLE_STRINGS */
void SDDS_SetTerminateMode_F()
{
  SDDS_SetTerminateMode(TERMINATE_DONT_FREE_TABLE_STRINGS+TERMINATE_DONT_FREE_ARRAY_STRINGS);
}
FCALLSCSUB0(SDDS_SetTerminateMode_F,SDDS_SETTERMINATEMODE_F,sdds_setterminatemode_f)

/* SDDS_DefineParameter
   Returns: -1 on error
            index on success */
int SDDS_DefineParameter_F(int fileIndex, char *name, char *symbol, char *units, char *description, char *format_string, int type, char *fixed_value)
{
  if (symbol) 
    if (strlen(symbol) == 0) 
      symbol = NULL;
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  if (description) 
    if (strlen(description) == 0)
      description = NULL;   
  if (format_string) 
    if (strlen(format_string) == 0)
      format_string = NULL;   
  if (fixed_value) 
    if (strlen(fixed_value) == 0)
      fixed_value = NULL;   
  return SDDS_DefineParameter(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, fixed_value);
}
FCALLSCFUN8(INT,SDDS_DefineParameter_F,SDDS_DEFINEPARAMETER_F,sdds_defineparameter_f,INT,STRING,STRING,STRING,STRING,STRING,INT,STRING)

/* SDDS_DefineParameter1
   Used in place of SDDS_DefineParameter when there is a numerical fixed value
   Returns: -1 on error
            index on success */
int SDDS_DefineParameter1_F(int fileIndex, char *name, char *symbol, char *units, char *description, char *format_string, int type, void *fixed_value)
{
  if (symbol) 
    if (strlen(symbol) == 0) 
      symbol = NULL;
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  if (description) 
    if (strlen(description) == 0)
      description = NULL;   
  if (format_string) 
    if (strlen(format_string) == 0)
      format_string = NULL;   
  return SDDS_DefineParameter1(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, fixed_value);
}
FCALLSCFUN8(INT,SDDS_DefineParameter1_F,SDDS_DEFINEPARAMETER1_F,sdds_defineparameter1_f,INT,STRING,STRING,STRING,STRING,STRING,INT,PVOID)

/* SDDS_DefineColumn
   Returns: -1 on error
            index on success */
int SDDS_DefineColumn_F(int fileIndex, char *name, char *symbol, char *units, char *description, char *format_string, int type, int field_length)
{
  if (symbol) 
    if (strlen(symbol) == 0)
      symbol = NULL;
  if (units) 
    if (strlen(units) == 0)
      units = NULL;
  if (description) 
    if (strlen(description) == 0)
      description = NULL;   
  if (format_string) 
    if (strlen(format_string) == 0)
      format_string = NULL;
  return SDDS_DefineColumn(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, field_length);
}
FCALLSCFUN8(INT,SDDS_DefineColumn_F,SDDS_DEFINECOLUMN_F,sdds_definecolumn_f,INT,STRING,STRING,STRING,STRING,STRING,INT,INT)

/* SDDS_IsValidName
   Returns: 0 on invalid name
            1 on valid name */
int SDDS_IsValidName_F(char *name)
{
  return SDDS_IsValidName(name, NULL);
}
FCALLSCFUN1(INT,SDDS_IsValidName_F,SDDS_ISVALIDNAME_F,sdds_isvalidname_f,STRING)

/* SDDS_SetNameValidityFlags
   Sets nameValidityFlags to: SDDS_ALLOW_ANY_NAME
   Returns: previous value for nameValidityFlags */
int SDDS_SetNameValidityFlags_F() /*it should be an unsigned long */
{
  return SDDS_SetNameValidityFlags(SDDS_ALLOW_ANY_NAME);
}
FCALLSCFUN0(INT,SDDS_SetNameValidityFlags_F,SDDS_SETNAMEVALIDITYFLAGS_F,sdds_setnamevalidityflags_f)

/* SDDS_DefineSimpleColumn
   Returns: 0 on error
            1 on success */
int SDDS_DefineSimpleColumn_F(int fileIndex, char *name, char *units, int type)
{
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return SDDS_DefineSimpleColumn(&dataset_f[fileIndex], name, units, type);
}
FCALLSCFUN4(INT,SDDS_DefineSimpleColumn_F,SDDS_DEFINESIMPLECOLUMN_F,sdds_definesimplecolumn_f,INT,STRING,STRING,INT)

/* SDDS_DefineSimpleParameter
   Returns: 0 on error
            1 on success */
int SDDS_DefineSimpleParameter_F(int fileIndex, char *name, char *units, int type)
{
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return SDDS_DefineSimpleParameter(&dataset_f[fileIndex], name, units, type);
}
FCALLSCFUN4(INT,SDDS_DefineSimpleParameter_F,SDDS_DEFINESIMPLEPARAMETER_F,sdds_definesimpleparameter_f,INT,STRING,STRING,INT)

/* SDDS_WriteLayout
   Returns: 0 on error
            1 on success */
int SDDS_WriteLayout_F(int fileIndex)
{
  return SDDS_WriteLayout(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_WriteLayout_F,SDDS_WRITELAYOUT_F,sdds_writelayout_f,INT)

/* SDDS_EraseData
   Returns: 0 on error
            1 on success */
int SDDS_EraseData_F(int fileIndex)
{
  return SDDS_EraseData(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_EraseData_F,SDDS_ERASEDATA_F,sdds_erasedata_f,INT)

/* SDDS_ProcessColumnString
   Argument: mode
     Valid values: 0, 1 (SDDS_NORMAL_DEFINITION, SDDS_WRITEONLY_DEFINITION)
   Returns: 0 on error
            1 on success */
int SDDS_ProcessColumnString_F(int fileIndex, char *string, int mode)
{
  return SDDS_ProcessColumnString(&dataset_f[fileIndex], string, mode);
}
FCALLSCFUN3(INT,SDDS_ProcessColumnString_F,SDDS_PROCESSCOLUMNSTRING_F,sdds_processcolumnstring_f,INT,STRING,INT)

/* SDDS_ProcessParameterString
   Argument: mode
     Valid values: 0, 1 (SDDS_NORMAL_DEFINITION, SDDS_WRITEONLY_DEFINITION)
   Returns: 0 on error
            1 on success */
int SDDS_ProcessParameterString_F(int fileIndex, char *string, int mode)
{
  return SDDS_ProcessParameterString(&dataset_f[fileIndex], string, mode);
}
FCALLSCFUN3(INT,SDDS_ProcessParameterString_F,SDDS_PROCESSPARAMETERSTRING_F,sdds_processparameterstring_f,INT,STRING,INT)

/* SDDS_InitializeCopy
   Argument: filemode
     Valid values: "r", "w", "rb", "wb", "m"
   Returns: 0 on error
            1 on success */
int SDDS_InitializeCopy_F(int fileIndexTarget, int fileIndexSource, char *filename, char *filemode)
{
  if (filename)
    if (strlen(filename) == 0) 
      filename = NULL;
  return SDDS_InitializeCopy(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], filename, filemode);
}
FCALLSCFUN4(INT,SDDS_InitializeCopy_F,SDDS_INITIALIZECOPY_F,sdds_initializecopy_f,INT,INT,STRING,STRING)

/* SDDS_CopyLayout
   Returns: 0 on error
            1 on success */
int SDDS_CopyLayout_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_CopyLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]);
}
FCALLSCFUN2(INT,SDDS_CopyLayout_F,SDDS_COPYLAYOUT_F,sdds_copylayout_f,INT,INT)

/* SDDS_AppendLayout
   Returns: 0 on error
            1 on success */
int SDDS_AppendLayout_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_AppendLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], 0);
}
FCALLSCFUN2(INT,SDDS_AppendLayout_F,SDDS_APPENDLAYOUT_F,sdds_appendlayout_f,INT,INT)

/* SDDS_CopyPage
   Returns: 0 on error
            1 on success */
int SDDS_CopyPage_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_CopyPage(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]);
}
FCALLSCFUN2(INT,SDDS_CopyPage_F,SDDS_COPYPAGE_F,sdds_copypage_f,INT,INT)

/* SDDS_CopyParameters
   Returns: 0 on error
            1 on success */
int SDDS_CopyParameters_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_CopyParameters(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]);
}
FCALLSCFUN2(INT,SDDS_CopyParameters_F,SDDS_COPYPARAMETERS_F,sdds_copyparameters_f,INT,INT)

/* SDDS_CopyColumns
   Returns: 0 on error
            1 on success */
int SDDS_CopyColumns_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_CopyColumns(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]);
}
FCALLSCFUN2(INT,SDDS_CopyColumns_F,SDDS_COPYCOLUMNS_F,sdds_copycolumns_f,INT,INT)

/* SDDS_CopyRow
   Returns: 0 on error
            1 on success */
int SDDS_CopyRow_F(int fileIndexTarget, int target_row, int fileIndexSource, int source_row)
{
  return SDDS_CopyRow(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row);
}
FCALLSCFUN4(INT,SDDS_CopyRow_F,SDDS_COPYROW_F,sdds_copyrow_f,INT,INT,INT,INT)

/* SDDS_CopyRowDirect
   Returns: 0 on error
            1 on success */
int SDDS_CopyRowDirect_F(int fileIndexTarget, int target_row, int fileIndexSource, int source_row)
{
  return SDDS_CopyRowDirect(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row);
}
FCALLSCFUN4(INT,SDDS_CopyRowDirect_F,SDDS_COPYROWDIRECT_F,sdds_copyrowdirect_f,INT,INT,INT,INT)

/* SDDS_CopyAdditionalRows
   Returns: 0 on error
            1 on success */
int SDDS_CopyAdditionalRows_F(int fileIndexTarget, int fileIndexSource)
{
  return SDDS_CopyAdditionalRows(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]);
}
FCALLSCFUN2(INT,SDDS_CopyAdditionalRows_F,SDDS_COPYADDITIONALROWS_F,sdds_copyadditionalrows_f,INT,INT)

/* SDDS_DeferSavingLayout
   Argument: mode
     Valid values: 0, 1 (False, True) */
void SDDS_DeferSavingLayout_F(int mode)
{
  SDDS_DeferSavingLayout(mode);
}
FCALLSCSUB1(SDDS_DeferSavingLayout_F,SDDS_DEFERSAVINGLAYOUT_F,sdds_defersavinglayout_f,INT)

/* SDDS_SaveLayout
   Returns: 0 on error
            1 on success */
int SDDS_SaveLayout_F(int fileIndex)
{
  return SDDS_SaveLayout(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_SaveLayout_F,SDDS_SAVELAYOUT_F,sdds_savelayout_f,INT)

/* SDDS_RestoreLayout
   Returns: 0 on error
            1 on success */
int SDDS_RestoreLayout_F(int fileIndex)
{
  return SDDS_RestoreLayout(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_RestoreLayout_F,SDDS_RESTORELAYOUT_F,sdds_restorelayout_f,INT)

/* SDDS_StartPage
   Returns: 0 on error
            1 on success */
int SDDS_StartPage_F(int fileIndex, int expected_n_rows)
{
  return SDDS_StartPage(&dataset_f[fileIndex], expected_n_rows);
}
FCALLSCFUN2(INT,SDDS_StartPage_F,SDDS_STARTPAGE_F,sdds_startpage_f,INT,INT)

/* SDDS_ClearPage
   Returns: 0 on error
            1 on success */
int SDDS_ClearPage_F(int fileIndex)
{
  return SDDS_ClearPage(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_ClearPage_F,SDDS_CLEARPAGE_F,sdds_clearpage_f,INT)

/* SDDS_LengthenTable
   Returns: 0 on error
            1 on success */
int SDDS_LengthenTable_F(int fileIndex, int n_additional_rows)
{
  return SDDS_LengthenTable(&dataset_f[fileIndex], n_additional_rows);
}
FCALLSCFUN2(INT,SDDS_LengthenTable_F,SDDS_LENGTHENTABLE_F,sdds_lengthentable_f,INT,INT)

/* SDDS_WritePage
   Returns: 0 on error
            1 on success */
int SDDS_WritePage_F(int fileIndex)
{
  return SDDS_WritePage(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_WritePage_F,SDDS_WRITEPAGE_F,sdds_writepage_f,INT)

/* SDDS_InitializeInput
   Returns: 0 on error
            1 on success */
int SDDS_InitializeInput_F(int fileIndex, char *filename)
{
  return SDDS_InitializeInput(&dataset_f[fileIndex], filename);
}
FCALLSCFUN2(INT,SDDS_InitializeInput_F,SDDS_INITIALIZEINPUT_F,sdds_initializeinput_f,INT,STRING)

/* SDDS_InitializeHeaderlessInput
   Returns: 0 on error
            1 on success */
int SDDS_InitHeaderlessInput_F(int fileIndex, char *filename)
{
  return SDDS_InitializeHeaderlessInput(&dataset_f[fileIndex], filename);
}
FCALLSCFUN2(INT,SDDS_InitHeaderlessInput_F,SDDS_INITHEADERLESSINPUT_F,sdds_initheaderlessinput_f,INT,STRING)

/* SDDS_ReadPage
   Returns: 0 on error
            -1 on EOF
            page number on success */
int SDDS_ReadPage_F(int fileIndex)
{
  return SDDS_ReadPage(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_ReadPage_F,SDDS_READPAGE_F,sdds_readpage_f,INT)

/* SDDS_ReadPageSparse
   Returns: 0 on error
            -1 on EOF
            page number on success */
int SDDS_ReadPageSparse_F(int fileIndex, int sparse_interval, int sparse_offset)
{
  return SDDS_ReadPageSparse(&dataset_f[fileIndex], 0, sparse_interval, sparse_offset);
}
FCALLSCFUN3(INT,SDDS_ReadPageSparse_F,SDDS_READPAGESPARSE_F,sdds_readpagesparse_f,INT,INT,INT)

/* SDDS_RowCount
   Returns number of rows */
int SDDS_RowCount_F(int fileIndex)
{
  return SDDS_RowCount(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_RowCount_F,SDDS_ROWCOUNT_F,sdds_rowcount_f,INT)

/* SDDS_SetColumnFlags
   Argument: column_flag_value
     Valid values: 0, 1
   Returns: 0 on error
            1 on success */
int SDDS_SetColumnFlags_F(int fileIndex, int column_flag_value)
{
  return SDDS_SetColumnFlags(&dataset_f[fileIndex], column_flag_value);
}
FCALLSCFUN2(INT,SDDS_SetColumnFlags_F,SDDS_SETCOLUMNFLAGS_F,sdds_setcolumnflags_f,INT,INT)

/* SDDS_SetRowFlags
   Argument: row_flag_value
     Valid values: 0, 1
   Returns: 0 on error
            1 on success */
int SDDS_SetRowFlags_F(int fileIndex, int row_flag_value)
{
  return SDDS_SetRowFlags(&dataset_f[fileIndex], row_flag_value);
}
FCALLSCFUN2(INT,SDDS_SetRowFlags_F,SDDS_SETROWFLAGS_F,sdds_setrowflags_f,INT,INT)

/* SDDS_GetRowFlag
   Returns: -1 on error
            row flag on success */
int SDDS_GetRowFlag_F(int fileIndex, int row)
{
  return SDDS_GetRowFlag(&dataset_f[fileIndex], row);
}
FCALLSCFUN2(INT,SDDS_GetRowFlag_F,SDDS_SETROWFLAG_F,sdds_setrowflag_f,INT,INT)

/* SDDS_DeleteColumn
   Returns: 0 on error
            1 on success */
int SDDS_DeleteColumn_F(int fileIndex, char *column_name)
{
  return SDDS_DeleteColumn(&dataset_f[fileIndex], column_name);
}
FCALLSCFUN2(INT,SDDS_DeleteColumn_F,SDDS_DELETECOLUMN_F,sdds_deletecolumn_f,INT,STRING)

/* SDDS_DeleteParameter
   Returns: 0 on error
            1 on success */
int SDDS_DeleteParameter_F(int fileIndex, char *parameter_name)
{
  return SDDS_DeleteParameter(&dataset_f[fileIndex], parameter_name);
}
FCALLSCFUN2(INT,SDDS_DeleteParameter_F,SDDS_DELETEPARAMETER_F,sdds_deleteparameter_f,INT,STRING)

/* SDDS_DeleteUnsetColumns
   Returns: 0 on error
            1 on success */
int SDDS_DeleteUnsetColumns_F(int fileIndex)
{
  return SDDS_DeleteUnsetColumns(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_DeleteUnsetColumns_F,SDDS_DELETEUNSETCOLUMNS_F,sdds_deleteunsetcolumns_f,INT)

/* SDDS_DeleteUnsetRows
   Returns: 0 on error
            1 on success */
int SDDS_DeleteUnsetRows_F(int fileIndex)
{
  return SDDS_DeleteUnsetRows(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_DeleteUnsetRows_F,SDDS_DELETEUNSETROWS_F,sdds_deleteunsetrows_f,INT)

/* SDDS_ColumnCount
   Returns: number of columns */
int SDDS_ColumnCount_F(int fileIndex)
{
  return SDDS_ColumnCount(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_ColumnCount_F,SDDS_COLUMNCOUNT_F,sdds_columncount_f,INT)

/* SDDS_ParameterCount
   Returns: number of parameters */
int SDDS_ParameterCount_F(int fileIndex)
{
  return SDDS_ParameterCount(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_ParameterCount_F,SDDS_PARAMETERCOUNT_F,sdds_parametercount_f,INT)

/* SDDS_GetColumn 
   Modified from standard version
   Comments: Column data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            column type on success */
int SDDS_GetColumn_F(int fileIndex, char *column_name) {
  int32_t index;
  returnValueVoid_f = SDDS_GetColumn(&dataset_f[fileIndex], column_name);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  index = SDDS_GetColumnIndex(&dataset_f[fileIndex], column_name);
  return SDDS_GetColumnType(&dataset_f[fileIndex], index);
}
FCALLSCFUN2(INT,SDDS_GetColumn_F,SDDS_GETCOLUMN_F,sdds_getcolumn_f,INT,STRING)

/* SDDS_GetInternalColumn
   Modified from standard version
   Comments: Column data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetInternalColumn_F(int fileIndex, char *column_name) 
{
  returnValueVoid_f = SDDS_GetInternalColumn(&dataset_f[fileIndex], column_name);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  return 1;  
}
FCALLSCFUN2(INT,SDDS_GetInternalColumn_F,SDDS_GETINTERNALCOLUMN_F,sdds_getinternalcolumn_f,INT,STRING)

/* SDDS_GetColumnInDoubles
   Modified from standard version
   Comments: Column data is stored in global variable 
             which can be retrieved by using sdds_getvaluesfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetColumnInDoubles_F(int fileIndex, char *column_name) 
{
  returnValueDouble_f = SDDS_GetColumnInDoubles(&dataset_f[fileIndex], column_name);
  if (!(returnValueDouble_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN2(INT,SDDS_GetColumnInDoubles_F,SDDS_GETCOLUMNINDOUBLES_F,sdds_getcolumnindoubles_f,INT,STRING)

/* SDDS_GetColumnInLong
   Modified from standard version
   Comments: Column data is stored in global variable 
             which can be retrieved by using sdds_getvaluesfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetColumnInLong_F(int fileIndex, char *column_name) 
{
  returnValueLong_f = SDDS_GetColumnInLong(&dataset_f[fileIndex], column_name);
  if (!(returnValueLong_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN2(INT,SDDS_GetColumnInLong_F,SDDS_GETCOLUMNINLONG_F,sdds_getcolumninlong_f,INT,STRING)

/* SDDS_GetNumericColumn
   Modified from standard version
   Comments: Column data is stored in global variable 
             which can be retrieved by using sdds_getvaluesfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetNumericColumn_F(int fileIndex, char *column_name, int desiredType) 
{
  switch (desiredType) {
  case SDDS_SHORT:
    returnValueShort_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueShort_f)) {
      return 0;
    }
    break;
  case SDDS_USHORT:
    returnValueUShort_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueUShort_f)) {
      return 0;
    }
    break;
  case SDDS_LONG:
    returnValueLong_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueLong_f)) {
      return 0;
    }
    break;
  case SDDS_ULONG:
    returnValueULong_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueULong_f)) {
      return 0;
    }
    break;
  case SDDS_FLOAT:
    returnValueFloat_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueFloat_f)) {
      return 0;
    }
    break;
  case SDDS_DOUBLE:
    returnValueDouble_f = SDDS_GetNumericColumn(&dataset_f[fileIndex], column_name, desiredType);
    if (!(returnValueDouble_f)) {
      return 0;
    }
    break;
  default:
    return 0;
  }
  return 1;
}
FCALLSCFUN3(INT,SDDS_GetNumericColumn_F,SDDS_GETNUMERICCOLUMN_F,sdds_getnumericcolumn_f,INT,STRING,INT)

/* SDDS_GetValue
   Modified from standard version
   Comments: data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetValue_F(int fileIndex, char *column_name, int srow_index) 
{
  returnValueVoid_f = SDDS_GetValue(&dataset_f[fileIndex], column_name, srow_index, NULL);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN3(INT,SDDS_GetValue_F,SDDS_GETVALUE_F,sdds_getvalue_f,INT,STRING,INT)

/* SDDS_GetValueByIndex
   Modified from standard version
   Comments: data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetValueByIndex_F(int fileIndex, int column_index, int srow_index) 
{
  returnValueVoid_f = SDDS_GetValueByIndex(&dataset_f[fileIndex], column_index, srow_index, NULL);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN3(INT,SDDS_GetValueByIndex_F,SDDS_GETVALUEBYINDEX_F,sdds_getvaluebyindex_f,INT,INT,INT)

/* SDDS_GetValueByAbsIndex
   Modified from standard version
   Comments: data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetValueByAbsIndex_F(int fileIndex, int column_index, int srow_index) 
{
  returnValueVoid_f = SDDS_GetValueByAbsIndex(&dataset_f[fileIndex], column_index, srow_index, NULL);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN3(INT,SDDS_GetValueByAbsIndex_F,SDDS_GETVALUEBYABSINDEX_F,sdds_getvaluebyabsindex_f,INT,INT,INT)

/* SDDS_GetParameter
   Modified from standard version
   Comments: parameter data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            parameter type on success */
int SDDS_GetParameter_F(int fileIndex, char *parameter_name) 
{
  int index;
  returnValueVoid_f = SDDS_GetParameter(&dataset_f[fileIndex], parameter_name, NULL);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  index = SDDS_GetParameterIndex(&dataset_f[fileIndex], parameter_name);
  return SDDS_GetParameterType(&dataset_f[fileIndex], index);
}
FCALLSCFUN2(INT,SDDS_GetParameter_F,SDDS_GETPARAMETER_F,sdds_getparameter_f,INT,STRING)

/* SDDS_GetParameterAsDouble
   Modified from standard version
   Comments: parameter data is stored in global variable 
             which can be retrieved by using sdds_getvaluesfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetParameterAsDouble_F(int fileIndex, char *parameter_name) 
{
  returnValueDouble_f = SDDS_GetParameterAsDouble(&dataset_f[fileIndex], parameter_name, NULL);
  if (!(returnValueDouble_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN2(INT,SDDS_GetParameterAsDouble_F,SDDS_GETPARAMETERASDOUBLE_F,sdds_getparameterasdouble_f,INT,STRING)

/* SDDS_GetFixedValueParameter
   Modified from standard version
   Comments: parameter data is stored in global variable 
             which can be retrieved by using sdds_getvoidfrom_c_to_f_
   Returns: 0 on error
            1 on success */
int SDDS_GetFixedValueParameter_F(int fileIndex, char *parameter_name) 
{
  returnValueVoid_f = SDDS_GetFixedValueParameter(&dataset_f[fileIndex], parameter_name, NULL);
  if (!(returnValueVoid_f)) {
    return 0;
  }
  return 1;
}
FCALLSCFUN2(INT,SDDS_GetFixedValueParameter_F,SDDS_GETFIXEDVALUEPARAMETER_F,sdds_getfixedvalueparameter_f,INT,STRING)

/* SDDS_GetDescription
   Modified from standard version
   Returns: NULL on error
            text on success */
char *SDDS_GetDescriptionText_F(int fileIndex)
{
  char *text;
  char *contents;
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0)
    return(NULL);
  return text;
}
FCALLSCFUN1(STRING,SDDS_GetDescriptionText_F,SDDS_GETDESCRIPTIONTEXT_F,sdds_getdescriptiontext_f,INT)

/* SDDS_GetDescription
   Modified from standard version
   Returns: NULL on error
            contents on success */
char *SDDS_GetDescriptionContents_F(int fileIndex)
{
  char *text;
  char *contents;
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0)
    return(NULL);
  return contents;
}
FCALLSCFUN1(STRING,SDDS_GetDescriptionContents_F,SDDS_GETDESCRIPTIONCONTENTS_F,sdds_getdescriptioncontents_f,INT)

/* SDDS_NumberOfErrors
   Returns: number of errors */
int SDDS_NumberOfErrors_F() 
{
  return SDDS_NumberOfErrors();
}
FCALLSCFUN0(INT,SDDS_NumberOfErrors_F,SDDS_NUMBEROFERRORS_F,sdds_numberoferrors_f)

/* SDDS_ClearErrors 
   Sets number of errors to 0 */
void SDDS_ClearErrors_F() 
{
  SDDS_ClearErrors();
}
FCALLSCSUB0(SDDS_ClearErrors_F,SDDS_CLEARERRORS_F,sdds_clearerrors_f)

/* SDDS_SetError
   Sets error description */
void SDDS_SetError_F(char *error_text) 
{
  SDDS_SetError(error_text);
}
FCALLSCSUB1(SDDS_SetError_F,SDDS_SETERROR_F,sdds_seterror_f,STRING)

/* SDDS_Bomb
   Displays message on stderr and exits */
void SDDS_Bomb_F(char *message) 
{
  SDDS_Bomb(message);
}
FCALLSCSUB1(SDDS_Bomb_F,SDDS_BOMB_F,sdds_bomb_f,STRING)

/* SDDS_Warning
   Displays message on stderr */
void SDDS_Warning_F(char *message) 
{
  SDDS_Warning(message);
}
FCALLSCSUB1(SDDS_Warning_F,SDDS_WARNING_F,sdds_warning_f,STRING)

/* SDDS_RegisterProgramName
   Sets registeredProgramName to be printed with some error messages */
void SDDS_RegisterProgramName_F(char *name) 
{
  SDDS_RegisterProgramName(name);
}
FCALLSCSUB1(SDDS_RegisterProgramName_F,SDDS_REGISTERPROGRAMNAME_F,sdds_registerprogramname_f,STRING)

/* SDDS_PrintErrors
   Argument: Mode
     Valid values: 0, 1, 2, 3 (0, SDDS_VERBOSE_PrintErrors, SDDS_EXIT_PrintErrors, 
                               SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors)
   Prints error messages that were set with SDDS_SetError */
void SDDS_PrintErrors_F(int mode) 
{
  SDDS_PrintErrors(stderr, mode);
}
FCALLSCSUB1(SDDS_PrintErrors_F,SDDS_PRINTERRORS_F,sdds_printerrors_f,INT) /* using stderr for file pointer */

/* SDDS_TransferColumnDefinition
   Returns: 0 on error
            1 on success */
int SDDS_TransferColumnDefinition_F(int fileIndexTarget, int fileIndexSource, char *name, char *newName)
{
  return SDDS_TransferColumnDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName);
}
FCALLSCFUN4(INT,SDDS_TransferColumnDefinition_F,SDDS_TRANSFERCOLUMNDEFINITION_F,sdds_transfercolumndefinition_f,INT,INT,STRING,STRING)

/* SDDS_DefineColumnLikeParameter
   Returns: 0 on error
            1 on success */
int SDDS_DefineColumnLikePar_F(int fileIndexTarget, int fileIndexSource, char *name, char *newName)
{
  return SDDS_DefineColumnLikeParameter(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName);
}
FCALLSCFUN4(INT,SDDS_DefineColumnLikePar_F,SDDS_DEFINECOLUMNLIKEPAR_F,sdds_definecolumnlikepar_f,INT,INT,STRING,STRING)

/* SDDS_TransferParameterDefinition
   Returns: 0 on error
            1 on success */
int SDDS_TransferParDefinition_F(int fileIndexTarget, int fileIndexSource,  char *name, char *newName)
{
  return SDDS_TransferParameterDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName);
}
FCALLSCFUN4(INT,SDDS_TransferParDefinition_F,SDDS_TRANSFERPARDEFINITION_F,sdds_transferpardefinition_f,INT,INT,STRING,STRING)

/* SDDS_DefineParameterLikeColumn
   Returns: 0 on error
            1 on success */
int SDDS_DefineParLikeColumn_F(int fileIndexTarget, int fileIndexSource,  char *name, char *newName)
{
  return SDDS_DefineParameterLikeColumn(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName);
}
FCALLSCFUN4(INT,SDDS_DefineParLikeColumn_F,SDDS_DEFINEPARLIKECOLUMN_F,sdds_defineparlikecolumn_f,INT,INT,STRING,STRING)

/* SDDS_TransferAllParameterDefinitions
   Argument: mode
     Valid values: 0, 1, 2, 3 (0, SDDS_TRANSFER_KEEPOLD, SDDS_TRANSFER_OVERWRITE, 
                               SDDS_TRANSFER_KEEPOLD|SDDS_TRANSFER_OVERWRITE)
   Returns: 0 on error
            1 on success */
int SDDS_TransferAllParDefs_F(int fileIndexTarget, int fileIndexSource, int mode)
{
  return SDDS_TransferAllParameterDefinitions(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], mode);
}
FCALLSCFUN3(INT,SDDS_TransferAllParDefs_F,SDDS_TRANSFERALLPARDEFS_F,sdds_transferallpardefs_f,INT,INT,INT)

/* SDDS_GetColumnIndex
   Returns: -1 on error
            index on success */
int SDDS_GetColumnIndex_F(int fileIndex, char *name)
{
  return SDDS_GetColumnIndex(&dataset_f[fileIndex], name);
}
FCALLSCFUN2(INT,SDDS_GetColumnIndex_F,SDDS_GETCOLUMNINDEX_F,sdds_getcolumnindex_f,INT,STRING)

/* SDDS_GetParameterIndex
   Returns: -1 on error
            index on success */
int SDDS_GetParameterIndex_F(int fileIndex, char *name)
{
  return SDDS_GetParameterIndex(&dataset_f[fileIndex], name);
}
FCALLSCFUN2(INT,SDDS_GetParameterIndex_F,SDDS_GETPARAMETERINDEX_F,sdds_getparameterindex_f,INT,STRING)

/* SDDS_GetColumnType
   Returns: 0 on error
            type on success */
int SDDS_GetColumnType_F(int fileIndex, int index)
{
  return SDDS_GetColumnType(&dataset_f[fileIndex], index);
}
FCALLSCFUN2(INT,SDDS_GetColumnType_F,SDDS_GETCOLUMNTYPE_F,sdds_getcolumntype_f,INT,INT)

/* SDDS_GetNamedColumnType
   Returns: 0 on error
            type on success */
int SDDS_GetNamedColumnType_F(int fileIndex, char *name)
{
  return SDDS_GetNamedColumnType(&dataset_f[fileIndex], name);
}
FCALLSCFUN2(INT,SDDS_GetNamedColumnType_F,SDDS_GETNAMEDCOLUMNTYPE_F,sdds_getnamedcolumntype_f,INT,STRING)

/* SDDS_GetParameterType
   Returns: 0 on error
            type on success */
int SDDS_GetParameterType_F(int fileIndex, int index)
{
  return SDDS_GetParameterType(&dataset_f[fileIndex], index);
}
FCALLSCFUN2(INT,SDDS_GetParameterType_F,SDDS_GETPARAMETERTYPE_F,sdds_getparametertype_f,INT,INT)

/* SDDS_GetNamedParameterType
   Returns: 0 on error
            type on success */
int SDDS_GetNamedParameterType_F(int fileIndex, char *name)
{
  return SDDS_GetNamedParameterType(&dataset_f[fileIndex], name);
}
FCALLSCFUN2(INT,SDDS_GetNamedParameterType_F,SDDS_GETNAMEDPARAMETERTYPE_F,sdds_getnamedparametertype_f,INT,STRING)

/* SDDS_GetTypeSize
   Returns: -1 on error
            size on success */
int SDDS_GetTypeSize_F(int type)
{
  return SDDS_GetTypeSize(type);
}
FCALLSCFUN1(INT,SDDS_GetTypeSize_F,SDDS_GETTYPESIZE_F,sdds_gettypesize_f,INT)

/* SDDS_GetTypeName
   Returns: NULL on error
            name on success */
char *SDDS_GetTypeName_F(int type)
{
  return SDDS_GetTypeName(type);
}
FCALLSCFUN1(STRING,SDDS_GetTypeName_F,SDDS_GETTYPENAME_F,sdds_gettypename_f,INT)

/* SDDS_IdentifyType
   Argument: typeName
     Valid values: double, float, long, short, string, character
   Returns: 0 on error
            type on success */ 
int SDDS_IdentifyType_F(char *typeName)
{
  return SDDS_IdentifyType(typeName);
}
FCALLSCFUN1(INT,SDDS_IdentifyType_F,SDDS_IDENTIFYTYPE_F,sdds_identifytype_f,STRING)

/* SDDS_CheckColumn
   Returns: 0 (SDDS_CHECK_OKAY)
            1 (SDDS_CHECK_NONEXISTENT)
            2 (SDDS_CHECK_WRONGTYPE)
            3 (SDDS_CHECK_WRONGUNITS) */
int SDDS_CheckColumn_F(int fileIndex, char *name, char *units, int type) /* stderr is used in place of file pointer */
{
  return SDDS_CheckColumn(&dataset_f[fileIndex], name, units, type, stderr);
}
FCALLSCFUN4(INT,SDDS_CheckColumn_F,SDDS_CHECKCOLUMN_F,sdds_checkcolumn_f,INT,STRING,STRING,INT)

/* SDDS_CheckParameter
   Returns: 0 (SDDS_CHECK_OKAY)
            1 (SDDS_CHECK_NONEXISTENT)
            2 (SDDS_CHECK_WRONGTYPE)
            3 (SDDS_CHECK_WRONGUNITS) */
int SDDS_CheckParameter_F(int fileIndex, char *name, char *units, int type) /* stderr is used in place of file pointer */
{
  return SDDS_CheckParameter(&dataset_f[fileIndex], name, units, type, stderr);
}
FCALLSCFUN4(INT,SDDS_CheckParameter_F,SDDS_CHECKPARAMETER_F,sdds_checkparameter_f,INT,STRING,STRING,INT)

/* SDDS_HasWhitespace
   Returns: 0 (false)
            1 (true) */
int SDDS_HasWhitespace_F(char *string)
{
  return SDDS_HasWhitespace(string);
}
FCALLSCFUN1(INT,SDDS_HasWhitespace_F,SDDS_HASWHITESPACE_F,sdds_haswhitespace_f,STRING)

/* SDDS_SprintTypedValue
   Argument: mode
     Valid value: 0, 1 (0, SDDS_PRINT_NOQUOTES)
   Returns: 0 on error
            1 on success */
int SDDS_SprintTypedValue_F(void *data, int index, int type, char *format, char *buffer, int mode)
{
  if (format) 
    if (strlen(format) == 0)
      format = NULL;   
  return SDDS_SprintTypedValue(data, index, type, format, buffer, mode);
}
FCALLSCFUN6(INT,SDDS_SprintTypedValue_F,SDDS_SPRINTTYPEDVALUE_F,sdds_sprinttypedvalue_f,PVOID,INT,INT,STRING,PSTRING,INT)

/* SDDS_StringIsBlank
   Returns: 0 (false)
            1 (true) */
int SDDS_StringIsBlank_F(char *s)
{
  return SDDS_StringIsBlank(s);
}
FCALLSCFUN1(INT,SDDS_StringIsBlank_F,SDDS_STRINGISBLANK_F,sdds_stringisblank_f,STRING)

/* SDDS_ApplyFactorToParameter
   Returns: 0 on error
            1 on success */
int SDDS_ApplyFactorToParameter_F(int fileIndex, char *name, double factor)
{
  return SDDS_ApplyFactorToParameter(&dataset_f[fileIndex], name, factor);
}
FCALLSCFUN3(INT,SDDS_ApplyFactorToParameter_F,SDDS_APPLYFACTORTOPARAMETER_F,sdds_applyfactortoparameter_f,INT,STRING,DOUBLE)

/* SDDS_ApplyFactorToColumn
   Returns: 0 on error
            1 on success */
int SDDS_ApplyFactorToColumn_F(int fileIndex, char *name, double factor)
{
  return SDDS_ApplyFactorToColumn(&dataset_f[fileIndex], name, factor);
}
FCALLSCFUN3(INT,SDDS_ApplyFactorToColumn_F,SDDS_APPLYFACTORTOCOLUMN_F,sdds_applyfactortocolumn_f,INT,STRING,DOUBLE)

/* SDDS_DeleteParameterFixedValues
   Returns: 0 on error
            1 on success */
int SDDS_DeleteParFixedValues_F(int fileIndex)
{
  return SDDS_DeleteParameterFixedValues(&dataset_f[fileIndex]);
}
FCALLSCFUN1(INT,SDDS_DeleteParFixedValues_F,SDDS_DELETEPARFIXEDVALUES_F,sdds_deleteparfixedvalues_f,INT)

/* SDDS_SetDataMode
   Returns: 0 on error
            1 on success */
int SDDS_SetDataMode_F(int fileIndex, int newmode)
{
  return SDDS_SetDataMode(&dataset_f[fileIndex], newmode);
}
FCALLSCFUN2(INT,SDDS_SetDataMode_F,SDDS_SETDATAMODE_F,sdds_setdatamode_f,INT,INT)

/* SDDS_CheckDataset
   Returns: 0 on error
            1 on success */
int SDDS_CheckDataset_F(int fileIndex, char *caller)
{
  return SDDS_CheckDataset(&dataset_f[fileIndex], caller);
}
FCALLSCFUN2(INT,SDDS_CheckDataset_F,SDDS_CHECKDATASET_F,sdds_checkdataset_f,INT,STRING)

/* SDDS_SetAutoCheckMode
   Argument: newMode
     Valid values: 0, 1 (0, TABULAR_DATA_CHECKS)
   Returns: previous mode */
int SDDS_SetAutoCheckMode_F(int newMode)
{
  return SDDS_SetAutoCheckMode(newMode);
}
FCALLSCFUN1(INT,SDDS_SetAutoCheckMode_F,SDDS_SETAUTOCHECKMODE_F,sdds_setautocheckmode_f,INT)

/* SDDS_GetColumnNames
   Modified from standard version
   Returns: NULL on error
            name on success */
char *SDDS_GetColumnNameFromIndex_F(int fileIndex, int index)
{
  char **data;
  int32_t number;
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return(NULL);
  if ((index >= 0) && (index < number)) {
    return data[index];
  }
  return(NULL);
}
FCALLSCFUN2(STRING,SDDS_GetColumnNameFromIndex_F,SDDS_GETCOLUMNNAMEFROMINDEX_F,sdds_getcolumnnamefromindex_f,INT,INT)

/* SDDS_GetParameterNames
   Modified from standard version
   Returns: NULL on error
            name on success */
char *SDDS_GetParNameFromIndex_F(int fileIndex, int index)
{
  char **data;
  int32_t number;
  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return(NULL);
  if ((index >= 0) && (index < number)) {
    return data[index];
  }
  return(NULL);
}
FCALLSCFUN2(STRING,SDDS_GetParNameFromIndex_F,SDDS_GETPARNAMEFROMINDEX_F,sdds_getparnamefromindex_f,INT,INT)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_SetParameterByName_F(int fileIndex, char *name, void *value)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name,value,NULL);
}
FCALLSCFUN3(INT,SDDS_SetParameterByName_F,SDDS_SETPARAMETERBYNAME_F,sdds_setparameterbyname_f,INT,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_Set2ParametersByName_F(int fileIndex, char *name1, void *value1, char *name2, void *value2)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name1,value1,name2,value2,NULL);
}
FCALLSCFUN5(INT,SDDS_Set2ParametersByName_F,SDDS_SET2PARAMETERSBYNAME_F,sdds_set2parametersbyname_f,INT,STRING,PVOID,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_Set3ParametersByName_F(int fileIndex, char *name1, void *value1, char *name2, void *value2, char *name3, void *value3)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name1,value1,name2,value2,name3,value3,NULL);
}
FCALLSCFUN7(INT,SDDS_Set3ParametersByName_F,SDDS_SET3PARAMETERSBYNAME_F,sdds_set3parametersbyname_f,INT,STRING,PVOID,STRING,PVOID,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_Set4ParametersByName_F(int fileIndex, char *name1, void *value1, char *name2, void *value2, char *name3, void *value3, char *name4, void *value4)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name1,value1,name2,value2,name3,value3,name4,value4,NULL);
}
FCALLSCFUN9(INT,SDDS_Set4ParametersByName_F,SDDS_SET4PARAMETERSBYNAME_F,sdds_set4parametersbyname_f,INT,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_Set5ParametersByName_F(int fileIndex, char *name1, void *value1, char *name2, void *value2, char *name3, void *value3, char *name4, void *value4, char *name5, void *value5)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name1,value1,name2,value2,name3,value3,name4,value4,name5,value5,NULL);
}
FCALLSCFUN11(INT,SDDS_Set5ParametersByName_F,SDDS_SET5PARAMETERSBYNAME_F,sdds_set5parametersbyname_f,INT,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_Set6ParametersByName_F(int fileIndex, char *name1, void *value1, char *name2, void *value2, char *name3, void *value3, char *name4, void *value4, char *name5, void *value5, char *name6, void *value6)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_REFERENCE,name1,value1,name2,value2,name3,value3,name4,value4,name5,value5,name6,value6,NULL);
}
FCALLSCFUN13(INT,SDDS_Set6ParametersByName_F,SDDS_SET6PARAMETERSBYNAME_F,sdds_set6parametersbyname_f,INT,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID,STRING,PVOID)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_SetParameterStringByName_F(int fileIndex, char *name, char *value)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,name,value,NULL);
}
FCALLSCFUN3(INT,SDDS_SetParameterStringByName_F,SDDS_SETPARAMETERSTRINGBYNAME_F,sdds_setparameterstringbyname_f,INT,STRING,STRING)

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_SetParameterByIndex_F(int fileIndex, int index, void *value)
{
  return SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,index,value,-1);
}
FCALLSCFUN3(INT,SDDS_SetParameterByIndex_F,SDDS_SETPARAMETERBYINDEX_F,sdds_setparameterbyindex_f,INT,INT,PVOID)

/* SDDS_SetColumn
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_SetColumnByName_F(int fileIndex, void *data, int rows, char *name)
{
  return SDDS_SetColumn(&dataset_f[fileIndex],SDDS_SET_BY_NAME,data,rows,name,NULL);
}
FCALLSCFUN4(INT,SDDS_SetColumnByName_F,SDDS_SETCOLUMNBYNAME_F,sdds_setcolumnbyname_f,INT,PVOID,INT,STRING)

/* SDDS_SetColumn
   Modified from standard version
   Returns: 0 on error
            1 success */
int SDDS_SetColumnByIndex_F(int fileIndex, void *data, int rows, int index)
{
  return SDDS_SetColumn(&dataset_f[fileIndex],SDDS_SET_BY_INDEX,data,rows,index,NULL);
}
FCALLSCFUN4(INT,SDDS_SetColumnByIndex_F,SDDS_SETCOLUMNBYINDEX_F,sdds_setcolumnbyindex_f,INT,PVOID,INT,INT)

/* sdds_getvaluesfrom_c_to_f_ 
   Allows fortran code to get values from C library
   Returns: 0 on error
            1 on success */
int sdds_getvaluesfrom_c_to_f_(void *returnValue, int *rows, int *type)
{
  int i;
  i = 0;
  switch (*type) {
  case SDDS_SHORT:
    for (i=0;i < *rows;i++) {
      *(((short*)returnValue)+i) = *(returnValueShort_f+i);
    }
    break;
  case SDDS_USHORT:
    for (i=0;i < *rows;i++) {
      *(((unsigned short*)returnValue)+i) = *(returnValueUShort_f+i);
    }
    break;
  case SDDS_LONG:
    for (i=0;i < *rows;i++) {
      *(((int32_t*)returnValue)+i) = *(returnValueLong_f+i);
    }
    break;
  case SDDS_ULONG:
    for (i=0;i < *rows;i++) {
      *(((uint32_t*)returnValue)+i) = *(returnValueULong_f+i);
    }
    break;
  case SDDS_FLOAT:
    for (i=0;i < *rows;i++) {
      *(((float*)returnValue)+i) = *(returnValueFloat_f+i);
    }
    break;
  case SDDS_DOUBLE:
    for (i=0;i < *rows;i++) {
      *(((double*)returnValue)+i) = *(returnValueDouble_f+i);
    }
    break;
  default:
    return 0;
  }
  return 1;
}

/* SDDS_GetColumn 
   Modified from standard version
   Returns: -1 on error
            rows on success */
int SDDS_GetColumnData_F(int *fileIndex, int *index, void *returnValue, int *arrayLength, int *newType) {
  int originalType, rows, i;
  void *columnValue;
  char *name;
  
  name = SDDS_GetColumnNameFromIndex_F(*fileIndex, *index);
  if (!name)
    return -1;

  rows = SDDS_RowCount(&(dataset_f[*fileIndex]));
  if (rows < 0)
    return -1;
  if (*arrayLength < rows) {
    SDDS_SetError("fortran array for column data is not big enough");
    return -1;
  }
  originalType = SDDS_GetColumnType(&(dataset_f[*fileIndex]), *index);
  if ((originalType == SDDS_STRING) || (originalType == SDDS_CHARACTER)) {
    SDDS_SetError("column is not a numerical column");
    return -1;
  }
    
  columnValue = SDDS_GetColumn(&(dataset_f[*fileIndex]), name);
  if (!(columnValue))
    return -1;

  switch (*newType) {
  case SDDS_SHORT:
    for (i=0;i < rows;i++) {
      *(((short*)returnValue)+i) = *(((short*)columnValue)+i);
    }
    break;
  case SDDS_USHORT:
    for (i=0;i < rows;i++) {
      *(((unsigned short*)returnValue)+i) = *(((unsigned short*)columnValue)+i);
    }
    break;
  case SDDS_LONG:
    for (i=0;i < rows;i++) {
      *(((int32_t*)returnValue)+i) = *(((int32_t*)columnValue)+i);
    }
    break;
  case SDDS_ULONG:
    for (i=0;i < rows;i++) {
      *(((uint32_t*)returnValue)+i) = *(((uint32_t*)columnValue)+i);
    }
    break;
  case SDDS_FLOAT:
    for (i=0;i < rows;i++) {
      *(((float*)returnValue)+i) = *(((float*)columnValue)+i);
    }
    break;
  case SDDS_DOUBLE:
    for (i=0;i < rows;i++) {
      *(((double*)returnValue)+i) = *(((double*)columnValue)+i);
    }
    break;
  default:
    return -1;
  }

  return rows;
}
FCALLSCFUN5(INT,SDDS_GetColumnData_F,SDDS_GETCOLUMNDATA_F,sdds_getcolumndata_f,PINT,PINT,PVOID,PINT,PINT)

/* SDDS_GetParameter
   Modified from standard version
   Returns: 0 on error
            1 on success */
int SDDS_GetParameterData_F(int *fileIndex, int *index, void *returnValue, int *newType) 
{
  int originalType;
  void *parameterValue;
  char *name;

  name = SDDS_GetParNameFromIndex_F(*fileIndex, *index);
  if (!name) {
    return 0;
  }

  parameterValue = SDDS_GetParameter(&(dataset_f[*fileIndex]), name, NULL);
  if (!(parameterValue)) {
    return 0;
  }
  originalType = SDDS_GetParameterType(&dataset_f[*fileIndex], *index);

  switch (originalType) {
  case SDDS_SHORT:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((short*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((short*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((short*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((short*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((short*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((short*)parameterValue));
      break;
    default:
      return 0;
    }
    break;
  case SDDS_USHORT:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((unsigned short*)parameterValue));
      break;
    default:
      return 0;
    }
    break;
  case SDDS_LONG:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((int32_t*)parameterValue));
      break;
    default:
     return 0;
    }
    break;
  case SDDS_ULONG:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((uint32_t*)parameterValue));
      break;
    default:
     return 0;
    }
    break;
  case SDDS_FLOAT:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((float*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((float*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((float*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((float*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((float*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((float*)parameterValue));
      break;
    default:
     return 0;
    }
    break;
  case SDDS_DOUBLE:
    switch (*newType) {
    case SDDS_SHORT:
	*(((short*)returnValue)) = *(((double*)parameterValue));
      break;
    case SDDS_USHORT:
	*(((unsigned short*)returnValue)) = *(((double*)parameterValue));
      break;
    case SDDS_LONG:
	*(((int32_t*)returnValue)) = *(((double*)parameterValue));
      break;
    case SDDS_ULONG:
	*(((uint32_t*)returnValue)) = *(((double*)parameterValue));
      break;
    case SDDS_FLOAT:
	*(((float*)returnValue)) = *(((double*)parameterValue));
      break;
    case SDDS_DOUBLE:
	*(((double*)returnValue)) = *(((double*)parameterValue));
      break;
    default:
      return 0;
    }
    break;
  default:
    return 0;
  }
  return 1;
}
FCALLSCFUN4(INT,SDDS_GetParameterData_F,SDDS_GETPARAMETERDATA_F,sdds_getparameterdata_f,PINT,PINT,PVOID,PINT)


/* sdds_getvoidfrom_c_to_f_ 
   Allows fortran code to get values from C library
   Returns: 0 on error
            1 on success */
int sdds_getvoidfrom_c_to_f_(void *returnValue, int *originalType, int *newType, int *rows)
{
  int i;
  i = 0;
  switch (*originalType) {
  case SDDS_SHORT:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((short*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  case SDDS_USHORT:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((unsigned short*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  case SDDS_LONG:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((int32_t*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  case SDDS_ULONG:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((uint32_t*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  case SDDS_FLOAT:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((float*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  case SDDS_DOUBLE:
    switch (*newType) {
    case SDDS_SHORT:
      for (i=0;i < *rows;i++) {
	*(((short*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_USHORT:
      for (i=0;i < *rows;i++) {
	*(((unsigned short*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_LONG:
      for (i=0;i < *rows;i++) {
	*(((int32_t*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_ULONG:
      for (i=0;i < *rows;i++) {
	*(((uint32_t*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_FLOAT:
      for (i=0;i < *rows;i++) {
	*(((float*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    case SDDS_DOUBLE:
      for (i=0;i < *rows;i++) {
	*(((double*)returnValue)+i) = *(((double*)returnValueVoid_f)+i);
      }
      break;
    default:
      return 0;
    }
    break;
  default:
    return 0;
  }
  return 1;
}

/* SDDS_SetDoubleRowValues
   Returns: 0 on error
            1 success */
int SDDS_SetDoubleRowValues_F(int fileIndex, int row, char *name1, double value1)
{
  return SDDS_SetRowValues(&dataset_f[fileIndex], 
			   SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			   row,
			   name1,value1,
			   NULL);
    }
FCALLSCFUN4(INT,SDDS_SetDoubleRowValues_F,SDDS_SETDOUBLEROWVALUES_F,sdds_setdoublerowvalues_f,INT,INT,STRING,DOUBLE)

/* SDDS_Set3DoubleRowValues
   Returns: 0 on error
            1 success */
int SDDS_Set3DoubleRowValues_F(int fileIndex, int row, char *name1, double value1, char *name2, double value2, char *name3, double value3)
{
  return SDDS_SetRowValues(&dataset_f[fileIndex], 
			   SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			   row,
			   name1,value1,
			   name2,value2,
			   name3,value3,
			   NULL);
    }
FCALLSCFUN8(INT,SDDS_Set3DoubleRowValues_F,SDDS_SET3DOUBLEROWVALUES_F,sdds_set3doublerowvalues_f,INT,INT,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE)

/* SDDS_Set4DoubleRowValues
   Returns: 0 on error
            1 success */
int SDDS_Set4DoubleRowValues_F(int fileIndex, int row, char *name1, double value1, char *name2, double value2, char *name3, double value3, char *name4, double value4)
{
  return SDDS_SetRowValues(&dataset_f[fileIndex], 
			   SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			   row,
			   name1,value1,
			   name2,value2,
			   name3,value3,
			   name4,value4,
			   NULL);
    }
FCALLSCFUN10(INT,SDDS_Set4DoubleRowValues_F,SDDS_SET4DOUBLEROWVALUES_F,sdds_set4doublerowvalues_f,INT,INT,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE)

/* SDDS_Set5DoubleRowValues
   Returns: 0 on error
            1 success */
int SDDS_Set5DoubleRowValues_F(int fileIndex, int row, char *name1, double value1, char *name2, double value2, char *name3, double value3, char *name4, double value4, char *name5, double value5)
{
  return SDDS_SetRowValues(&dataset_f[fileIndex], 
			   SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
			   row,
			   name1,value1,
			   name2,value2,
			   name3,value3,
			   name4,value4,
			   name5,value5,
			   NULL);
    }
FCALLSCFUN12(INT,SDDS_Set5DoubleRowValues_F,SDDS_SET5DOUBLEROWVALUES_F,sdds_set5doublerowvalues_f,INT,INT,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE,STRING,DOUBLE)
