#include <Python.h>
#include "SDDS.h"

SDDS_DATASET dataset_f[20];

/* SDDS_InitializeInput
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_InitializeInput( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_InitializeInput(&dataset_f[fileIndex], filename));
}

/* SDDS_InitializeAppend
   Returns: 0 on error
            1 on success */ 
static PyObject* sddsdata_InitializeAppend( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_InitializeAppend(&dataset_f[fileIndex], filename));
}

/* SDDS_InitializeOutput
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_InitializeOutput( PyObject* self, PyObject* args )
{
  long fileIndex;
  long data_mode;
  long lines_per_row;
  char *description;
  char *contents;
  char *filename;
  if (!PyArg_ParseTuple(args, "lllsss", &fileIndex, &data_mode, &lines_per_row, &description, &contents, &filename)) {
    return NULL;
  }
  if (description) 
    if (strlen(description) == 0) 
      description = NULL;
  if (contents) 
    if (strlen(contents) == 0)
      contents = NULL;
  return PyLong_FromLong(SDDS_InitializeOutput(&dataset_f[fileIndex], data_mode, lines_per_row, description, contents, filename));
}

/* SDDS_Terminate
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_Terminate( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_Terminate(&dataset_f[fileIndex]));
}

/* SDDS_SetTerminateMode
   Sets TerminateMode to:
   TERMINATE_DONT_FREE_ARRAY_STRINGS+TERMINATE_DONT_FREE_TABLE_STRINGS */
static PyObject* sddsdata_SetTerminateMode( PyObject* self, PyObject* args )
{
  SDDS_SetTerminateMode(TERMINATE_DONT_FREE_TABLE_STRINGS+TERMINATE_DONT_FREE_ARRAY_STRINGS);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_DefineParameter
   Returns: -1 on error
            index on success */
static PyObject* sddsdata_DefineParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *symbol;
  char *units;
  char *description;
  char *format_string;
  long type;
  PyObject *fixed_value;
  char *fixedStringValue;
  long fixedLongValue;
  double fixedDoubleValue;
  char buffer[SDDS_MAXLINE];
  if (!PyArg_ParseTuple(args, "lssssslO", &fileIndex, &name , &symbol , &units , &description , &format_string , &type , &fixed_value)) {
    return NULL;
  }
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

  if (PyString_Check(fixed_value)) {
    fixedStringValue = PyString_AsString(fixed_value);
    if (fixedStringValue)
      if (strlen(fixedStringValue) == 0)
	fixedStringValue = NULL;
    return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						name, symbol, units, 
						description, format_string, 
						type, fixedStringValue));
  } else {
    if (PyNumber_Check(fixed_value)) {
      if (PyLong_Check(fixed_value)) {
	fixedLongValue = PyLong_AsLong(fixed_value);
	SDDS_SprintTypedValue(&fixedLongValue, 0, SDDS_LONG , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      } else if (PyInt_Check(fixed_value)) {
	fixedLongValue = PyInt_AsLong(fixed_value);
	SDDS_SprintTypedValue(&fixedLongValue, 0, SDDS_LONG , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      } else if (PyFloat_Check(fixed_value)) {
	fixedDoubleValue = PyFloat_AsDouble(fixed_value);
	SDDS_SprintTypedValue(&fixedDoubleValue, 0, SDDS_DOUBLE , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      }
    }
    return PyLong_FromLong(SDDS_DefineParameter1(&dataset_f[fileIndex], 
						   name, symbol, units, 
						   description, format_string, 
						   type, NULL));
  }
}

/* SDDS_DefineColumn
   Returns: -1 on error
            index on success */
static PyObject* sddsdata_DefineColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *symbol;
  char *units;
  char *description;
  char *format_string;
  long type;
  long field_length;
  if (!PyArg_ParseTuple(args, "lsssssll", &fileIndex, &name , &symbol, &units, &description, &format_string, &type, &field_length)) {
    return NULL;
  }
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
  return PyLong_FromLong(SDDS_DefineColumn(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, field_length));
}

/* SDDS_IsValidName
   Returns: 0 on invalid name
            1 on valid name */
static PyObject* sddsdata_IsValidName( PyObject* self, PyObject* args )
{
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_IsValidName(name, NULL));
}

/* SDDS_SetNameValidityFlags
   Sets nameValidityFlags to: SDDS_ALLOW_ANY_NAME
   Returns: previous value for nameValidityFlags */
static PyObject* sddsdata_SetNameValidityFlags( PyObject* self, PyObject* args )
{
  return PyLong_FromLong(SDDS_SetNameValidityFlags(SDDS_ALLOW_ANY_NAME));
}

/* SDDS_DefineSimpleColumn
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DefineSimpleColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return NULL;
  }
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return PyLong_FromLong(SDDS_DefineSimpleColumn(&dataset_f[fileIndex], name, units, type));
}

/* SDDS_DefineSimpleParameter
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DefineSimpleParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return NULL;
  }
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return PyLong_FromLong(SDDS_DefineSimpleParameter(&dataset_f[fileIndex], name, units, type));
}

/* SDDS_WriteLayout
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_WriteLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_WriteLayout(&dataset_f[fileIndex]));
}

/* SDDS_EraseData
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_EraseData( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_EraseData(&dataset_f[fileIndex]));
}

/* SDDS_ProcessColumnString
   Argument: mode
     Valid values: 0, 1 (SDDS_NORMAL_DEFINITION, SDDS_WRITEONLY_DEFINITION)
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_ProcessColumnString( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *string;
  long mode;
  if (!PyArg_ParseTuple(args, "lsl", &fileIndex, &string, &mode)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ProcessColumnString(&dataset_f[fileIndex], string, mode));
}

/* SDDS_ProcessParameterString
   Argument: mode
     Valid values: 0, 1 (SDDS_NORMAL_DEFINITION, SDDS_WRITEONLY_DEFINITION)
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_ProcessParameterString( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *string;
  long mode;
  if (!PyArg_ParseTuple(args, "lsl", &fileIndex, &string, &mode)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ProcessParameterString(&dataset_f[fileIndex], string, mode));
}

/* SDDS_InitializeCopy
   Argument: filemode
     Valid values: "r", "w", "rb", "wb", "m"
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_InitializeCopy( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *filename;
  char *filemode;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &filename, &filemode)) {
    return NULL;
  }
  if (filename)
    if (strlen(filename) == 0) 
      filename = NULL;
  return PyLong_FromLong(SDDS_InitializeCopy(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], filename, filemode));
}

/* SDDS_CopyLayout
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyLayout( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/* SDDS_AppendLayout
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_AppendLayout( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_AppendLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], 0));
}

/* SDDS_CopyPage
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyPage( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyPage(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/* SDDS_CopyParameters
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyParameters( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyParameters(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/* SDDS_CopyColumns
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyColumns( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyColumns(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/* SDDS_CopyRow
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyRow( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long target_row;
  long fileIndexSource;
  long source_row;
  if (!PyArg_ParseTuple(args, "llll", &fileIndexTarget, &target_row, &fileIndexSource, &source_row)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyRow(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row));
}

/* SDDS_CopyRowDirect
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyRowDirect( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long target_row;
  long fileIndexSource;
  long source_row;
  if (!PyArg_ParseTuple(args, "llll", &fileIndexTarget, &target_row, &fileIndexSource, &source_row)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyRowDirect(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row));
}

/* SDDS_CopyAdditionalRows
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CopyAdditionalRows( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CopyAdditionalRows(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/* SDDS_DeferSavingLayout
   Argument: mode
     Valid values: 0, 1 (False, True) */
static PyObject* sddsdata_DeferSavingLayout( PyObject* self, PyObject* args )
{
  long mode;
  if (!PyArg_ParseTuple(args, "l", &mode)) {
    return NULL;
  }
  SDDS_DeferSavingLayout(mode);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_SaveLayout
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_SaveLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_SaveLayout(&dataset_f[fileIndex]));
}

/* SDDS_RestoreLayout
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_RestoreLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_RestoreLayout(&dataset_f[fileIndex]));
}

/* SDDS_StartPage
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_StartPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  long expected_n_rows;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &expected_n_rows)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_StartPage(&dataset_f[fileIndex], expected_n_rows));
}

/* SDDS_ClearPage
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_ClearPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ClearPage(&dataset_f[fileIndex]));
}

/* SDDS_LengthenTable
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_LengthenTable( PyObject* self, PyObject* args )
{
  long fileIndex;
  long n_additional_rows;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &n_additional_rows)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_LengthenTable(&dataset_f[fileIndex], n_additional_rows));
}

/* SDDS_WritePage
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_WritePage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_WritePage(&dataset_f[fileIndex]));
}

/* SDDS_InitializeHeaderlessInput
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_InitHeaderlessInput( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_InitializeHeaderlessInput(&dataset_f[fileIndex], filename));
}

/* SDDS_ReadPage
   Returns: 0 on error
            -1 on EOF
            page number on success */
static PyObject* sddsdata_ReadPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ReadPage(&dataset_f[fileIndex]));
}

/* SDDS_ReadPageSparse
   Returns: 0 on error
            -1 on EOF
            page number on success */
static PyObject* sddsdata_ReadPageSparse( PyObject* self, PyObject* args )
{
  long fileIndex;
  long sparse_interval;
  long sparse_offset;
  if (!PyArg_ParseTuple(args, "lll", &fileIndex, &sparse_interval, &sparse_offset)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ReadPageSparse(&dataset_f[fileIndex], 0, sparse_interval, sparse_offset));
}

/* SDDS_RowCount
   Returns number of rows */
static PyObject* sddsdata_RowCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_RowCount(&dataset_f[fileIndex]));
}

/* SDDS_SetColumnFlags
   Argument: column_flag_value
     Valid values: 0, 1
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_SetColumnFlags( PyObject* self, PyObject* args )
{
  long fileIndex;
  long column_flag_value;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &column_flag_value)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_SetColumnFlags(&dataset_f[fileIndex], column_flag_value));
}

/* SDDS_SetRowFlags
   Argument: row_flag_value
     Valid values: 0, 1
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_SetRowFlags( PyObject* self, PyObject* args )
{
  long fileIndex;
  long row_flag_value;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &row_flag_value)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_SetRowFlags(&dataset_f[fileIndex], row_flag_value));
}

/* SDDS_GetRowFlag
   Returns: -1 on error
            row flag on success */
static PyObject* sddsdata_GetRowFlag( PyObject* self, PyObject* args )
{
  long fileIndex;
  long row;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &row)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetRowFlag(&dataset_f[fileIndex], row));
}

/* SDDS_DeleteColumn
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DeleteColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *column_name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &column_name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DeleteColumn(&dataset_f[fileIndex], column_name));
}

/* SDDS_DeleteParameter
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DeleteParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *parameter_name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &parameter_name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DeleteParameter(&dataset_f[fileIndex], parameter_name));
}

/* SDDS_DeleteUnsetColumns
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DeleteUnsetColumns( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DeleteUnsetColumns(&dataset_f[fileIndex]));
}

/* SDDS_DeleteUnsetRows
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DeleteUnsetRows( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DeleteUnsetRows(&dataset_f[fileIndex]));
}

/* SDDS_ColumnCount
   Returns: number of columns */
static PyObject* sddsdata_ColumnCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ColumnCount(&dataset_f[fileIndex]));
}

/* SDDS_ParameterCount
   Returns: number of parameters */
static PyObject* sddsdata_ParameterCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ParameterCount(&dataset_f[fileIndex]));
}

/* SDDS_GetDescription
   Modified from standard version
   Returns: NULL on error
            text and description on success */
static PyObject* sddsdata_GetDescription( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (!(v = PyList_New(2)))
    return NULL;
  if (text) {
    PyList_SetItem(v, 0, PyString_FromString(text));
    free(text);
  } else {
    PyList_SetItem(v, 0, PyString_FromString(""));
  }
  if (contents) {
    PyList_SetItem(v, 1, PyString_FromString(contents));
    free(contents);
  } else {
    PyList_SetItem(v, 1, PyString_FromString(""));
  }
  return v;
}

/* SDDS_GetDescription
   Modified from standard version
   Returns: NULL on error
            text on success */
static PyObject* sddsdata_GetDescriptionText( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (text)
    return PyString_FromString(text);
  return PyString_FromString("");
}

/* SDDS_GetDescription
   Modified from standard version
   Returns: NULL on error
            contents on success */
static PyObject* sddsdata_GetDescriptionContents( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (contents)
    return PyString_FromString(contents);
  return PyString_FromString("");
}

/* SDDS_NumberOfErrors
   Returns: number of errors */
static PyObject* sddsdata_NumberOfErrors( PyObject* self, PyObject* args ) 
{
  return PyLong_FromLong(SDDS_NumberOfErrors());
}

/* SDDS_ClearErrors 
   Sets number of errors to 0 */
static PyObject* sddsdata_ClearErrors( PyObject* self, PyObject* args ) 
{
  SDDS_ClearErrors();
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_SetError
   Sets error description */
static PyObject* sddsdata_SetError( PyObject* self, PyObject* args ) 
{
  char *error_text;
  if (!PyArg_ParseTuple(args, "s", &error_text)) {
    return NULL;
  }
  SDDS_SetError(error_text);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_Bomb
   Displays message on stderr and exits */
static PyObject* sddsdata_Bomb( PyObject* self, PyObject* args ) 
{
  char *message;
  if (!PyArg_ParseTuple(args, "s", &message)) {
    return NULL;
  }
  SDDS_Bomb(message);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_Warning
   Displays message on stderr */
static PyObject* sddsdata_Warning( PyObject* self, PyObject* args ) 
{
  char *message;
  if (!PyArg_ParseTuple(args, "s", &message)) {
    return NULL;
  }
  SDDS_Warning(message);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_RegisterProgramName
   Sets registeredProgramName to be printed with some error messages */
static PyObject* sddsdata_RegisterProgramName( PyObject* self, PyObject* args ) 
{
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  SDDS_RegisterProgramName(name);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_PrintErrors
   Argument: Mode
     Valid values: 0, 1, 2, 3 (0, SDDS_VERBOSE_PrintErrors, SDDS_EXIT_PrintErrors, 
                               SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors)
   Prints error messages that were set with SDDS_SetError */
static PyObject* sddsdata_PrintErrors( PyObject* self, PyObject* args ) 
{
  long mode;
  if (!PyArg_ParseTuple(args, "l", &mode)) {
    return NULL;
  }
  SDDS_PrintErrors(stderr, mode);
  Py_INCREF(Py_None);
  return Py_None;
}

/* SDDS_TransferColumnDefinition
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_TransferColumnDefinition( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_TransferColumnDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/* SDDS_DefineColumnLikeParameter
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DefineColumnLikeParameter( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DefineColumnLikeParameter(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/* SDDS_TransferParameterDefinition
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_TransferParameterDefinition( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_TransferParameterDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/* SDDS_DefineParameterLikeColumn
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DefineParameterLikeColumn( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DefineParameterLikeColumn(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/* SDDS_TransferAllParameterDefinitions
   Argument: mode
     Valid values: 0, 1, 2, 3 (0, SDDS_TRANSFER_KEEPOLD, SDDS_TRANSFER_OVERWRITE, 
                               SDDS_TRANSFER_KEEPOLD|SDDS_TRANSFER_OVERWRITE)
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_TransferAllParameterDefinitions( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  long mode;
  if (!PyArg_ParseTuple(args, "lll", &fileIndexTarget, &fileIndexSource, &mode)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_TransferAllParameterDefinitions(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], mode));
}

/* SDDS_GetColumnIndex
   Returns: -1 on error
            index on success */
static PyObject* sddsdata_GetColumnIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetColumnIndex(&dataset_f[fileIndex], name));
}

/* SDDS_GetParameterIndex
   Returns: -1 on error
            index on success */
static PyObject* sddsdata_GetParameterIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetParameterIndex(&dataset_f[fileIndex], name));
}

/* SDDS_GetColumnType
   Returns: 0 on error
            type on success */
static PyObject* sddsdata_GetColumnType( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetColumnType(&dataset_f[fileIndex], index));
}

/* SDDS_GetNamedColumnType
   Returns: 0 on error
            type on success */
static PyObject* sddsdata_GetNamedColumnType( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetNamedColumnType(&dataset_f[fileIndex], name));
}

/* SDDS_GetColumnDefinition
   Returns: NULL on error
            definition on success */
static PyObject* sddsdata_GetColumnDefinition( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  COLUMN_DEFINITION *pardef;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  if ((pardef = SDDS_GetColumnDefinition(&dataset_f[fileIndex], name)) == NULL)
    return NULL;
  if (!(v = PyList_New(6)))
    return NULL;

  if (pardef->symbol)
    PyList_SetItem(v, 0, PyString_FromString(pardef->symbol));
  else
    PyList_SetItem(v, 0, PyString_FromString(""));
  if (pardef->units)
    PyList_SetItem(v, 1, PyString_FromString(pardef->units));
  else
    PyList_SetItem(v, 1, PyString_FromString(""));
  if (pardef->description)
    PyList_SetItem(v, 2, PyString_FromString(pardef->description));
  else
    PyList_SetItem(v, 2, PyString_FromString(""));
  if (pardef->format_string)
    PyList_SetItem(v, 3, PyString_FromString(pardef->format_string));
  else
    PyList_SetItem(v, 3, PyString_FromString(""));
  PyList_SetItem(v, 4, PyLong_FromLong(pardef->type));
  PyList_SetItem(v, 5, PyLong_FromLong(pardef->field_length));

  SDDS_FreeColumnDefinition(pardef);

  return v;
}

/* SDDS_GetParameterType
   Returns: 0 on error
            type on success */
static PyObject* sddsdata_GetParameterType( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetParameterType(&dataset_f[fileIndex], index));
}

/* SDDS_GetNamedParameterType
   Returns: 0 on error
            type on success */
static PyObject* sddsdata_GetNamedParameterType( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetNamedParameterType(&dataset_f[fileIndex], name));  
}

/* SDDS_GetParameterDefinition
   Returns: NULL on error
            definition on success */
static PyObject* sddsdata_GetParameterDefinition( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  PARAMETER_DEFINITION *pardef;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  if ((pardef = SDDS_GetParameterDefinition(&dataset_f[fileIndex], name)) == NULL)
    return NULL;
  if (!(v = PyList_New(6)))
    return NULL;

  if (pardef->symbol)
    PyList_SetItem(v, 0, PyString_FromString(pardef->symbol));
  else
    PyList_SetItem(v, 0, PyString_FromString(""));
  if (pardef->units)
    PyList_SetItem(v, 1, PyString_FromString(pardef->units));
  else
    PyList_SetItem(v, 1, PyString_FromString(""));
  if (pardef->description)
    PyList_SetItem(v, 2, PyString_FromString(pardef->description));
  else
    PyList_SetItem(v, 2, PyString_FromString(""));
  if (pardef->format_string)
    PyList_SetItem(v, 3, PyString_FromString(pardef->format_string));
  else
    PyList_SetItem(v, 3, PyString_FromString(""));
  PyList_SetItem(v, 4, PyLong_FromLong(pardef->type));
  if (pardef->fixed_value)
    PyList_SetItem(v, 5, PyString_FromString(pardef->fixed_value));
  else
    PyList_SetItem(v, 5, PyString_FromString(""));

  SDDS_FreeParameterDefinition(pardef);

  return v;
}

/* SDDS_GetTypeSize
   Returns: -1 on error
            size on success */
static PyObject* sddsdata_GetTypeSize( PyObject* self, PyObject* args )
{
  long type;
  if (!PyArg_ParseTuple(args, "l", &type)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_GetTypeSize(type));
}

/* SDDS_GetTypeName
   Returns: NULL on error
            name on success */
static PyObject* sddsdata_GetTypeName( PyObject* self, PyObject* args )
{
  long type;
  char *name;
  char msgbuf[256];
  if (!PyArg_ParseTuple(args, "l", &type)) {
    return NULL;
  }
  name = SDDS_GetTypeName(type);
  if (name)
    return PyString_FromString(name);
  sprintf(msgbuf, "sdds.GetTypeName: %ld is an invalid SDDS data type", type);
  PyErr_SetString(PyExc_StandardError, msgbuf);
  return NULL;
}

/* SDDS_IdentifyType
   Argument: typeName
     Valid values: double, float, long, short, string, character
   Returns: 0 on error
            type on success */ 
static PyObject* sddsdata_IdentifyType( PyObject* self, PyObject* args )
{
  char *typeName;
  if (!PyArg_ParseTuple(args, "s", &typeName)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_IdentifyType(typeName));
}

/* SDDS_CheckColumn
   Returns: 0 (SDDS_CHECK_OKAY)
            1 (SDDS_CHECK_NONEXISTENT)
            2 (SDDS_CHECK_WRONGTYPE)
            3 (SDDS_CHECK_WRONGUNITS) */
static PyObject* sddsdata_CheckColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CheckColumn(&dataset_f[fileIndex], name, units, type, stderr));
}

/* SDDS_CheckParameter
   Returns: 0 (SDDS_CHECK_OKAY)
            1 (SDDS_CHECK_NONEXISTENT)
            2 (SDDS_CHECK_WRONGTYPE)
            3 (SDDS_CHECK_WRONGUNITS) */
static PyObject* sddsdata_CheckParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CheckParameter(&dataset_f[fileIndex], name, units, type, stderr));
}

/* SDDS_HasWhitespace
   Returns: 0 (false)
            1 (true) */
static PyObject* sddsdata_HasWhitespace( PyObject* self, PyObject* args )
{
  char *string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_HasWhitespace(string));
}

/* SDDS_StringIsBlank
   Returns: 0 (false)
            1 (true) */
static PyObject* sddsdata_StringIsBlank( PyObject* self, PyObject* args )
{
  char *s;
  if (!PyArg_ParseTuple(args, "s", &s)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_StringIsBlank(s));
}

/* SDDS_ApplyFactorToParameter
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_ApplyFactorToParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  double factor;
  if (!PyArg_ParseTuple(args, "lsd", &fileIndex, &name, &factor)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ApplyFactorToParameter(&dataset_f[fileIndex], name, factor));
}

/* SDDS_ApplyFactorToColumn
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_ApplyFactorToColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  double factor;
  if (!PyArg_ParseTuple(args, "lsd", &fileIndex, &name, &factor)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_ApplyFactorToColumn(&dataset_f[fileIndex], name, factor));
}

/* SDDS_DeleteParameterFixedValues
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_DeleteParameterFixedValues( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_DeleteParameterFixedValues(&dataset_f[fileIndex]));
}

/* SDDS_SetDataMode
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_SetDataMode( PyObject* self, PyObject* args )
{
  long fileIndex;
  long newmode;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &newmode)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_SetDataMode(&dataset_f[fileIndex], newmode));
}

/* SDDS_CheckDataset
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_CheckDataset( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *caller;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &caller)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_CheckDataset(&dataset_f[fileIndex], caller));
}

/* SDDS_SetAutoCheckMode
   Argument: newMode
     Valid values: 0, 1 (0, TABULAR_DATA_CHECKS)
   Returns: previous mode */
static PyObject* sddsdata_SetAutoCheckMode( PyObject* self, PyObject* args )
{
  long newMode;
  if (!PyArg_ParseTuple(args, "l", &newMode)) {
    return NULL;
  }
  return PyLong_FromLong(SDDS_SetAutoCheckMode(newMode));
}

/* SDDS_GetColumnNames
   Modified from standard version
   Returns: NULL on error
            name on success */
static PyObject* sddsdata_GetColumnNameFromIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    v = PyString_FromString(data[index]);
    for (i=0;i<number;i++)
      free(data[i]);
    free(data);
    return v;
  }
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return NULL;
}

/* SDDS_GetColumnNames
   Modified from standard version
   Returns: NULL on error
            names on success */
static PyObject* sddsdata_GetColumnNames( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if (!(v = PyList_New(number)))
    return NULL;
  for (index = 0; index < number; index++)
    PyList_SetItem(v, index, PyString_FromString(data[index]));
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return v;
}

/* SDDS_GetParameterNames
   Modified from standard version
   Returns: NULL on error
            name on success */
static PyObject* sddsdata_GetParameterNameFromIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    v = PyString_FromString(data[index]);
    for (i=0;i<number;i++)
      free(data[i]);
    free(data);
    return v;
  }
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return NULL;
}

/* SDDS_GetParameterNames
   Modified from standard version
   Returns: NULL on error
            names on success */
static PyObject* sddsdata_GetParameterNames( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if (!(v = PyList_New(number)))
    return NULL;
  for (index = 0; index < number; index++)
    PyList_SetItem(v, index, PyString_FromString(data[index]));
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return v;
}

/* SDDS_SetParameters
   Modified from standard version
   Returns: 0 on error
            1 success */
static PyObject* sddsdata_SetParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  PyObject *v;
  long index;
  long type;
  if (!PyArg_ParseTuple(args, "lOO", &fileIndex, &indexOrName, &v)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetParameterIndex(&dataset_f[fileIndex], PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  if ((type = SDDS_GetParameterType(&dataset_f[fileIndex], index)) == 0)
    return NULL;
  switch (type) {
  case SDDS_SHORT:
  case SDDS_USHORT:
  case SDDS_LONG:
  case SDDS_ULONG:
    if (PyLong_Check(v)) {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyLong_AsLong(v),-1));
    } else {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyInt_AsLong(v),-1));
    }
  case SDDS_FLOAT:
  case SDDS_DOUBLE:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyFloat_AsDouble(v),-1));
  case SDDS_CHARACTER:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,(char)(PyString_AsString(v)[0]),-1));
  case SDDS_STRING:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyString_AsString(v),-1));
  }
  return PyLong_FromLong(0);
}

/* SDDS_SetColumn
   Modified from standard version
   Returns: 0 on error
            1 success */
static PyObject* sddsdata_SetColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  long rows;
  PyObject *indexOrName;
  PyObject *v;
  long type;
  long i;
  void *data = NULL;
  long index;
  long result=0;
  PyObject *temp;
  if (!PyArg_ParseTuple(args, "lOO", &fileIndex, &indexOrName, &v)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetColumnIndex(&dataset_f[fileIndex], PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  if ((type = SDDS_GetColumnType(&dataset_f[fileIndex], index)) == 0)
    return NULL;
  rows = PyList_Size(v);
  switch (type) {
  case SDDS_SHORT:
    data = malloc(sizeof(short)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
	((short*)data)[i] = (short)PyLong_AsLong(temp);
      else
	((short*)data)[i] = (short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_USHORT:
    data = malloc(sizeof(unsigned short)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
	((unsigned short*)data)[i] = (unsigned short)PyLong_AsLong(temp);
      else
	((unsigned short*)data)[i] = (unsigned short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_LONG:
    data = malloc(sizeof(long)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
	((long*)data)[i] = (long)PyLong_AsLong(temp);
      else
	((long*)data)[i] = (long)PyInt_AsLong(temp);
    }
    break;
  case SDDS_ULONG:
    data = malloc(sizeof(unsigned long)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
	((unsigned long*)data)[i] = (unsigned long)PyLong_AsLong(temp);
      else
	((unsigned long*)data)[i] = (unsigned long)PyInt_AsLong(temp);
    }
    break;
  case SDDS_FLOAT:
    data = malloc(sizeof(float)*rows);
    for (i=0;i<rows;i++)
      ((float*)data)[i] = (float)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_DOUBLE:
    data = malloc(sizeof(double)*rows);
    for (i=0;i<rows;i++)
      ((double*)data)[i] = (double)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_CHARACTER:
    data = malloc(sizeof(char)*rows);
    for (i=0;i<rows;i++)
      ((char*)data)[i] = (char)(PyString_AsString(PyList_GetItem(v, i))[0]);
    break;
  case SDDS_STRING:
    data = malloc(sizeof(char*)*rows);
    for (i=0;i<rows;i++)
      SDDS_CopyString(((char**)data)+i, PyString_AsString(PyList_GetItem(v, i)));
    break;
  }
  result = SDDS_SetColumn(&dataset_f[fileIndex],SDDS_SET_BY_INDEX,data,rows,index,NULL);
  switch (type) {
  case SDDS_SHORT:
    free((short*)data);
    break;
  case SDDS_USHORT:
    free((unsigned short*)data);
    break;
  case SDDS_LONG:
    free((long*)data);
    break;
  case SDDS_ULONG:
    free((unsigned long*)data);
    break;
  case SDDS_FLOAT:
    free((float*)data);
    break;
  case SDDS_DOUBLE:
    free((double*)data);
    break;
  case SDDS_CHARACTER:
    free((char*)data);
    break;
  case SDDS_STRING:
    for (i=0;i<rows;i++)
      free(((char**)data)[i]);
    free((char**)data);
    break;
  }
  return PyLong_FromLong(result);
}

/* SDDS_GetColumn 
   Modified from standard version
   Returns: NULL on error
            data on success */
static PyObject* sddsdata_GetColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  long originalType, rows, i;
  void *columnValue;
  char *name;
  char **data;
  int32_t number;
  char buffer[40];
  long index;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetColumnIndex(&dataset_f[fileIndex], PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }

  rows = SDDS_RowCount(&(dataset_f[fileIndex]));
  if (rows < 0) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  originalType = SDDS_GetColumnType(&(dataset_f[fileIndex]), index);
    
  columnValue = SDDS_GetColumn(&(dataset_f[fileIndex]), name);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  if (!(columnValue))
    return NULL;

  if (!(v = PyList_New(rows)))
    return NULL;
  
  switch (originalType) {
  case SDDS_SHORT:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong(((short*)columnValue)[i]));
    }
    free((short*)columnValue);
    break;
  case SDDS_USHORT:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong(((unsigned short*)columnValue)[i]));
    }
    free((unsigned short*)columnValue);
    break;
  case SDDS_LONG:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong(((long*)columnValue)[i]));
    }
    free((long*)columnValue);
    break;
  case SDDS_ULONG:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong(((unsigned long*)columnValue)[i]));
    }
    free((unsigned long*)columnValue);
    break;
  case SDDS_FLOAT:
    for (i=0;i < rows;i++) {
      sprintf(buffer, "%.6E", ((float*)columnValue)[i]);
      PyList_SetItem(v, i, PyFloat_FromDouble(atof(buffer)));
    }
    free((float*)columnValue);
    break;
  case SDDS_DOUBLE:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyFloat_FromDouble(((double*)columnValue)[i]));
    }
    free((double*)columnValue);
    break;
  case SDDS_CHARACTER:
    for (i=0;i < rows;i++) {
      sprintf(buffer, "%c", ((char*)columnValue)[i]);
      PyList_SetItem(v, i, PyString_FromString(buffer));
    }
    free((char*)columnValue);
    break;
  case SDDS_STRING:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyString_FromString(((char**)columnValue)[i]));
    }
    for (i=0;i < rows;i++)
      free(((char**)columnValue)[i]);
    free((char**)columnValue);
    break;
  default:
    return NULL;
  }
  return v;
}

/* SDDS_GetParameter
   Modified from standard version
   Returns: NULL on error
            value on success */
static PyObject* sddsdata_GetParameter( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  PyObject *indexOrName;
  long originalType;
  void *parameterValue;
  char *name;
  char **data;
  int32_t number;
  char buffer[40];
  long index;
  long i;
  PyObject *v=NULL;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetParameterIndex(&dataset_f[fileIndex], PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;

  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }
  if (!name) {
    return NULL;
  }

  parameterValue = SDDS_GetParameter(&(dataset_f[fileIndex]), name, NULL);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  if (!(parameterValue)) {
    return NULL;
  }
  originalType = SDDS_GetParameterType(&dataset_f[fileIndex], index);

  switch (originalType) {
  case SDDS_SHORT:
    v = PyLong_FromLong(((short*)parameterValue)[0]);
    free((short*)parameterValue);
    break;
  case SDDS_USHORT:
    v = PyLong_FromLong(((unsigned short*)parameterValue)[0]);
    free((unsigned short*)parameterValue);
    break;
  case SDDS_LONG:
    v = PyLong_FromLong(((long*)parameterValue)[0]);
    free((long*)parameterValue);
    break;
  case SDDS_ULONG:
    v = PyLong_FromLong(((unsigned long*)parameterValue)[0]);
    free((unsigned long*)parameterValue);
    break;
  case SDDS_FLOAT:
    sprintf(buffer, "%.6E", ((float*)parameterValue)[0]);
    v = PyFloat_FromDouble(atof(buffer));
    free((float*)parameterValue);
    break;
  case SDDS_DOUBLE:
    v = PyFloat_FromDouble(((double*)parameterValue)[0]);
    free((double*)parameterValue);
    break;
  case SDDS_CHARACTER:
    sprintf(buffer, "%c", ((char*)parameterValue)[0]);
    v = PyString_FromString(buffer);
    free((char*)parameterValue);
    break;
  case SDDS_STRING:
    v = PyString_FromString(((char**)parameterValue)[0]);
    free(((char**)parameterValue)[0]);
    free((char**)parameterValue);
    break;
  }
  return v;
}

/* SDDS_InitializeInput
   Returns: 0 on error
            1 on success */
static PyObject* sddsdata_GetMode( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  return PyLong_FromLong(dataset_f[fileIndex].layout.data_mode.mode);
}

static PyMethodDef SddsdataMethods[] = {
  { "InitializeInput", sddsdata_InitializeInput, METH_VARARGS },
  { "InitializeAppend", sddsdata_InitializeAppend, METH_VARARGS },
  { "InitializeOutput", sddsdata_InitializeOutput, METH_VARARGS },
  { "Terminate", sddsdata_Terminate, METH_VARARGS },
  { "SetTerminateMode", sddsdata_SetTerminateMode, METH_VARARGS },
  { "DefineParameter", sddsdata_DefineParameter, METH_VARARGS },
  { "DefineColumn", sddsdata_DefineColumn, METH_VARARGS },
  { "IsValidName", sddsdata_IsValidName, METH_VARARGS },
  { "SetNameValidityFlags", sddsdata_SetNameValidityFlags, METH_VARARGS },
  { "DefineSimpleColumn", sddsdata_DefineSimpleColumn, METH_VARARGS },
  { "DefineSimpleParameter", sddsdata_DefineSimpleParameter, METH_VARARGS },
  { "WriteLayout", sddsdata_WriteLayout, METH_VARARGS },
  { "EraseData", sddsdata_EraseData, METH_VARARGS },
  { "ProcessColumnString", sddsdata_ProcessColumnString, METH_VARARGS },
  { "ProcessParameterString", sddsdata_ProcessParameterString, METH_VARARGS },
  { "InitializeCopy", sddsdata_InitializeCopy, METH_VARARGS },
  { "CopyLayout", sddsdata_CopyLayout, METH_VARARGS },
  { "AppendLayout", sddsdata_AppendLayout, METH_VARARGS },
  { "CopyPage", sddsdata_CopyPage, METH_VARARGS },
  { "CopyParameters", sddsdata_CopyParameters, METH_VARARGS },
  { "CopyColumns", sddsdata_CopyColumns, METH_VARARGS },
  { "CopyRow", sddsdata_CopyRow, METH_VARARGS },
  { "CopyRowDirect", sddsdata_CopyRowDirect, METH_VARARGS },
  { "CopyAdditionalRows", sddsdata_CopyAdditionalRows, METH_VARARGS },
  { "DeferSavingLayout", sddsdata_DeferSavingLayout, METH_VARARGS },
  { "SaveLayout", sddsdata_SaveLayout, METH_VARARGS },
  { "RestoreLayout", sddsdata_RestoreLayout, METH_VARARGS },
  { "StartPage", sddsdata_StartPage, METH_VARARGS },
  { "ClearPage", sddsdata_ClearPage, METH_VARARGS },
  { "LengthenTable", sddsdata_LengthenTable, METH_VARARGS },
  { "WritePage", sddsdata_WritePage, METH_VARARGS },
  { "InitHeaderlessInput", sddsdata_InitHeaderlessInput, METH_VARARGS },
  { "ReadPage", sddsdata_ReadPage, METH_VARARGS },
  { "ReadPageSparse", sddsdata_ReadPageSparse, METH_VARARGS },
  { "RowCount", sddsdata_RowCount, METH_VARARGS },
  { "SetColumnFlags", sddsdata_SetColumnFlags, METH_VARARGS },
  { "SetRowFlags", sddsdata_SetRowFlags, METH_VARARGS },
  { "GetRowFlag", sddsdata_GetRowFlag, METH_VARARGS },
  { "DeleteColumn", sddsdata_DeleteColumn, METH_VARARGS },
  { "DeleteParameter", sddsdata_DeleteParameter, METH_VARARGS },
  { "DeleteUnsetColumns", sddsdata_DeleteUnsetColumns, METH_VARARGS },
  { "DeleteUnsetRows", sddsdata_DeleteUnsetRows, METH_VARARGS },
  { "ColumnCount", sddsdata_ColumnCount, METH_VARARGS },
  { "ParameterCount", sddsdata_ParameterCount, METH_VARARGS },
  { "GetDescription", sddsdata_GetDescription, METH_VARARGS },
  { "GetDescriptionText", sddsdata_GetDescriptionText, METH_VARARGS },
  { "GetDescriptionContents", sddsdata_GetDescriptionContents, METH_VARARGS },
  { "NumberOfErrors", sddsdata_NumberOfErrors, METH_VARARGS },
  { "ClearErrors", sddsdata_ClearErrors, METH_VARARGS },
  { "SetError", sddsdata_SetError, METH_VARARGS },
  { "Bomb", sddsdata_Bomb, METH_VARARGS },
  { "Warning", sddsdata_Warning, METH_VARARGS },
  { "RegisterProgramName", sddsdata_RegisterProgramName, METH_VARARGS },
  { "PrintErrors", sddsdata_PrintErrors, METH_VARARGS },
  { "TransferColumnDefinition", sddsdata_TransferColumnDefinition, METH_VARARGS },
  { "DefineColumnLikeParameter", sddsdata_DefineColumnLikeParameter, METH_VARARGS },
  { "TransferParameterDefinition", sddsdata_TransferParameterDefinition, METH_VARARGS },
  { "DefineParameterLikeColumn", sddsdata_DefineParameterLikeColumn, METH_VARARGS },
  { "TransferAllParameterDefinitions", sddsdata_TransferAllParameterDefinitions, METH_VARARGS },
  { "GetColumnIndex", sddsdata_GetColumnIndex, METH_VARARGS },
  { "GetParameterIndex", sddsdata_GetParameterIndex, METH_VARARGS },
  { "GetColumnType", sddsdata_GetColumnType, METH_VARARGS },
  { "GetNamedColumnType", sddsdata_GetNamedColumnType, METH_VARARGS },
  { "GetColumnDefinition", sddsdata_GetColumnDefinition, METH_VARARGS },
  { "GetParameterType", sddsdata_GetParameterType, METH_VARARGS },
  { "GetNamedParameterType", sddsdata_GetNamedParameterType, METH_VARARGS },
  { "GetParameterDefinition", sddsdata_GetParameterDefinition, METH_VARARGS },
  { "GetTypeSize", sddsdata_GetTypeSize, METH_VARARGS },
  { "GetTypeName", sddsdata_GetTypeName, METH_VARARGS },
  { "IdentifyType", sddsdata_IdentifyType, METH_VARARGS },
  { "CheckColumn", sddsdata_CheckColumn, METH_VARARGS },
  { "CheckParameter", sddsdata_CheckParameter, METH_VARARGS },
  { "HasWhitespace", sddsdata_HasWhitespace, METH_VARARGS },
  { "StringIsBlank", sddsdata_StringIsBlank, METH_VARARGS },
  { "ApplyFactorToParameter", sddsdata_ApplyFactorToParameter, METH_VARARGS },
  { "ApplyFactorToColumn", sddsdata_ApplyFactorToColumn, METH_VARARGS },
  { "DeleteParameterFixedValues", sddsdata_DeleteParameterFixedValues, METH_VARARGS },
  { "SetDataMode", sddsdata_SetDataMode, METH_VARARGS },
  { "CheckDataset", sddsdata_CheckDataset, METH_VARARGS },
  { "SetAutoCheckMode", sddsdata_SetAutoCheckMode, METH_VARARGS },
  { "GetColumnNameFromIndex", sddsdata_GetColumnNameFromIndex, METH_VARARGS },
  { "GetColumnNames", sddsdata_GetColumnNames, METH_VARARGS },
  { "GetParameterNameFromIndex", sddsdata_GetParameterNameFromIndex, METH_VARARGS },
  { "GetParameterNames", sddsdata_GetParameterNames, METH_VARARGS },
  { "SetParameter", sddsdata_SetParameter, METH_VARARGS },
  { "SetColumn", sddsdata_SetColumn, METH_VARARGS },
  { "GetColumn", sddsdata_GetColumn, METH_VARARGS },
  { "GetParameter", sddsdata_GetParameter, METH_VARARGS },
  { "GetMode", sddsdata_GetMode, METH_VARARGS },
  { NULL, NULL }
};

#if defined(_WIN32)
__declspec(dllexport) void initsddsdata(void);
#endif

void initsddsdata(void) {
  (void) Py_InitModule( "sddsdata", SddsdataMethods );
}
