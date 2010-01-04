/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/*
 * Programmer:  Hairong Shang
 * Purpose:	This is the MPI-2 I/O driver.
 *
 */
#include "SDDS.h"
#include "SDDS_internal.h"
#include "scan.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif

typedef struct {
  int32_t type, field_length, dimensions, definition_mode, memory_number, pointer_number;
  int32_t name_len, symbol_len, units_len, description_len, format_string_len, fixed_value_len, group_name_len;
  char name[256], symbol[256], units[256], description[1024], format_string[256], fixed_value[1024], group_name[256];
} ELEMENT_DEF;

typedef struct {
  int32_t sdds, name_len, filename_len, path_len, description_len, contents_len;
  char name[256], filename[256], path[1024], description[1024], contents[1024];
} ASSOCIATE_DEF;

typedef struct {
  int32_t n_columns, n_parameters, n_associates, n_arrays, description_len, contents_len, version, layout_offset, filename_len;
  int32_t mode, lines_per_row, no_row_counts, fixed_row_count, fsync_data, additional_header_lines; /*data_mode definition*/
  short layout_written,  disconnected,  gzipFile,  popenUsed;
  uint32_t byteOrderDeclared;
  char description[1024], contents[1024], filename[1024];
} OTHER_DEF;

int32_t SDDS_MPI_ReadPage(SDDS_DATASET *SDDS_dataset)
{
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_ReadPageSparse"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't read page--file is disconnected (SDDS_ReadPageSparse)");
    return 0;
  }
  if (SDDS_dataset->original_layout.data_mode.mode==SDDS_ASCII) {
    SDDS_SetError("Unable to read ascii file with SDDS_MPI.");
    return 0;
  }
  else if (SDDS_dataset->original_layout.data_mode.mode==SDDS_BINARY) {
    return SDDS_MPI_ReadBinaryPage(SDDS_dataset);
  } else {
    SDDS_SetError("Unable to read page--unrecognized data mode (SDDS_ReadPageSparse)");
    return(0);
  }
}

int32_t SDDS_MPI_BroadcastLayout(SDDS_DATASET *SDDS_dataset)
{
  SDDS_LAYOUT *layout = NULL;
  MPI_DATASET *MPI_dataset = SDDS_dataset->MPI_dataset;
  char *symbol, *units, *description, *format_string, *fixed_value, *filename, *path, *contents;
  int i;
  ELEMENT_DEF *column=NULL, *parameter=NULL, *array=NULL;
  ASSOCIATE_DEF *associate=NULL;
  OTHER_DEF other;
  MPI_Datatype elementType, otherType, oldtypes[4], associateType;
  int blockcounts[4];
  /* MPI_Aint type used to be consistent with syntax of */
  /* MPI_Type_extent routine */
  MPI_Aint    offsets[4], int_ext, short_ext, uint_ext;
  
  MPI_Type_extent(MPI_INT, &int_ext);
  MPI_Type_extent(MPI_UNSIGNED, &uint_ext);
  MPI_Type_extent(MPI_SHORT, &short_ext);

  layout = &(SDDS_dataset->layout);
  /*commit element type */
  offsets[0] = 0;
  oldtypes[0] = MPI_INT;
  blockcounts[0] = 13;
  offsets[1] = 13 * int_ext;
  oldtypes[1] = MPI_CHAR;
  blockcounts[1] = 256 + 256 + 256 + 1024 + 256 + 1024 + 256;
  /* Now define structured type and commit it */
  MPI_Type_struct(2, blockcounts, offsets, oldtypes, &elementType);
  MPI_Type_commit(&elementType);
  
  /*commit other type */
  offsets[0] = 0;
  oldtypes[0] = MPI_INT;
  blockcounts[0] = 15;

  offsets[1] = 15 * int_ext;
  oldtypes[1] = MPI_SHORT;
  blockcounts[1] = 4;
  
  blockcounts[2] = 1;
  oldtypes[2] = MPI_UNSIGNED;
  offsets[2] = offsets[1] + 4 * short_ext;
  
  blockcounts[3] = 1024 + 1024 + 1024;
  oldtypes[3] = MPI_CHAR;
  offsets[3] = offsets[2] + 1 * uint_ext;
  
  /* Now define structured type and commit it */
  MPI_Type_struct(4, blockcounts, offsets, oldtypes, &otherType);
  MPI_Type_commit(&otherType);
  
  if (MPI_dataset->myid==0) {
    /*fill other layout structure */
    other.n_columns = layout->n_columns;
    other.n_parameters = layout->n_parameters;
    other.n_associates = layout->n_associates;
    other.n_arrays = layout->n_arrays;
    other.version = layout->version;
    other.layout_offset = SDDS_dataset->pagecount_offset[0];
    other.layout_written = layout->layout_written;
    other.disconnected = layout->disconnected;
    other.gzipFile = layout->gzipFile;
    other.popenUsed = layout->popenUsed;
    other.byteOrderDeclared = layout->byteOrderDeclared;
    other.mode = layout->data_mode.mode;
    other.lines_per_row = layout->data_mode.lines_per_row;
    other.no_row_counts = layout->data_mode.no_row_counts;
    other.fixed_row_count = layout->data_mode.fixed_row_count;
    other.fsync_data = layout->data_mode.fsync_data;
    other.additional_header_lines = layout->data_mode.additional_header_lines;
    other.description_len = other.contents_len = other.filename_len = 0;
    other.description[0] = other.contents[0] = other.filename[0] = '\0';
    if (layout->description) {
      other.description_len = strlen(layout->description);
      sprintf(other.description,"%s", layout->description);
    }
    if (layout->contents) {
      other.contents_len = strlen(layout->contents);
      sprintf(other.contents, "%s", layout->contents);
    }
    if (layout->filename) {
      other.filename_len = strlen(layout->filename);
      sprintf(other.filename, "%s", layout->filename);
    }
  } else {
    /* if (!SDDS_ZeroMemory((void *)SDDS_dataset, sizeof(SDDS_DATASET))) {
      SDDS_SetError("Unable to zero memory for sdds dataset.");
      return 0;
    }
    SDDS_dataset->MPI_dataset = MPI_dataset; */
  }
  /* broadcaset the layout other to other processors */
  MPI_Bcast(&other, 1, otherType, 0, MPI_dataset->comm);
  MPI_Type_free(&otherType);  
  if (other.n_columns)
    column = malloc(sizeof(*column)*other.n_columns);
  if (other.n_parameters)
    parameter = malloc(sizeof(*parameter)*other.n_parameters);
  if (other.n_arrays)
    array = malloc(sizeof(*array) * other.n_arrays);
  if (other.n_associates)
    associate = malloc(sizeof(*associate)*other.n_associates);
  if (MPI_dataset->myid==0) {
    /*fill elements*/
    for (i=0; i<other.n_columns; i++) {
      if (!SDDS_ZeroMemory(&column[i], sizeof(ELEMENT_DEF))) {
	SDDS_SetError("Unable to zero memory for columns(SDDS_MPI_InitializeInput)");
	return 0;
      }
      column[i].type = layout->column_definition[i].type;
      column[i].field_length = layout->column_definition[i].field_length;
      column[i].definition_mode = layout->column_definition[i].definition_mode;
      column[i].memory_number = layout->column_definition[i].memory_number;
      column[i].pointer_number = layout->column_definition[i].pointer_number;
      if (layout->column_definition[i].name) {
	column[i].name_len = strlen(layout->column_definition[i].name);
	sprintf(column[i].name, "%s", layout->column_definition[i].name);
      }
      if (layout->column_definition[i].symbol) {
	column[i].symbol_len = strlen(layout->column_definition[i].symbol);
	sprintf(column[i].symbol, "%s", layout->column_definition[i].symbol);
      }
      if (layout->column_definition[i].units) {
	column[i].units_len = strlen(layout->column_definition[i].units);
	sprintf(column[i].units, "%s", layout->column_definition[i].units);
      }
      if (layout->column_definition[i].description) {
	column[i].description_len = strlen(layout->column_definition[i].description);
	sprintf(column[i].description, "%s", layout->column_definition[i].description);
      }
      if (layout->column_definition[i].format_string) {
	column[i].format_string_len = strlen(layout->column_definition[i].format_string);
	sprintf(column[i].format_string, "%s", layout->column_definition[i].format_string);
      }
    }
    for (i=0; i<other.n_parameters; i++) {
      if (!SDDS_ZeroMemory(&parameter[i], sizeof(ELEMENT_DEF))) {
	SDDS_SetError("Unable to zero memory for parameters(SDDS_MPI_InitializeInput)");
	return 0;
      }
      parameter[i].type = layout->parameter_definition[i].type;
      parameter[i].definition_mode = layout->parameter_definition[i].definition_mode;
      parameter[i].memory_number = layout->parameter_definition[i].memory_number;
      if (layout->parameter_definition[i].name) {
	parameter[i].name_len = strlen(layout->parameter_definition[i].name);
	sprintf(parameter[i].name, "%s", layout->parameter_definition[i].name);
      }
      if (layout->parameter_definition[i].symbol) {
	parameter[i].symbol_len = strlen(layout->parameter_definition[i].symbol);
	sprintf(parameter[i].symbol, "%s", layout->parameter_definition[i].symbol);
      }
      if (layout->parameter_definition[i].units) {
	parameter[i].units_len = strlen(layout->parameter_definition[i].units);
	sprintf(parameter[i].units, "%s", layout->parameter_definition[i].units);
      }
      if (layout->parameter_definition[i].description) {
	parameter[i].description_len = strlen(layout->parameter_definition[i].description);
	sprintf(parameter[i].description, "%s", layout->parameter_definition[i].description);
      }
      if (layout->parameter_definition[i].format_string) {
	parameter[i].format_string_len = strlen(layout->parameter_definition[i].format_string);
	sprintf(parameter[i].format_string, "%s", layout->parameter_definition[i].format_string);
      }
      if (layout->parameter_definition[i].fixed_value) {
	parameter[i].fixed_value_len = strlen(layout->parameter_definition[i].fixed_value);
	sprintf(parameter[i].fixed_value, "%s", layout->parameter_definition[i].fixed_value);
      }
    }
    for (i=0; i<other.n_arrays; i++) {
      if (!SDDS_ZeroMemory(&array[i], sizeof(ELEMENT_DEF))) {
	SDDS_SetError("Unable to zero memory for arrays(SDDS_MPI_InitializeInput)");
	return 0;
      }
      array[i].type = layout->array_definition[i].type;
      array[i].field_length = layout->array_definition[i].field_length;
      array[i].dimensions = layout->array_definition[i].dimensions;
      if (layout->array_definition[i].name) {
	array[i].name_len = strlen(layout->array_definition[i].name);
	sprintf(array[i].name, "%s", layout->array_definition[i].name);
      }
      if (layout->array_definition[i].symbol) {
	array[i].symbol_len = strlen(layout->array_definition[i].symbol);
	sprintf(array[i].symbol, "%s", layout->array_definition[i].symbol);
      }
      if (layout->array_definition[i].units) {
	array[i].units_len = strlen(layout->array_definition[i].units);
	sprintf(array[i].units, "%s", layout->array_definition[i].units);
      }
      if (layout->array_definition[i].description) {
	array[i].description_len = strlen(layout->array_definition[i].description);
	sprintf(array[i].description, "%s", layout->array_definition[i].description);
      }
      if (layout->array_definition[i].format_string) {
	array[i].format_string_len = strlen(layout->array_definition[i].format_string);
	sprintf(array[i].format_string, "%s", layout->array_definition[i].format_string);
      }
      if (layout->array_definition[i].group_name) {
	array[i].group_name_len = strlen(layout->array_definition[i].group_name);
	sprintf(array[i].group_name, "%s", layout->array_definition[i].group_name);
      }
    }
    for (i=0; i<other.n_associates;i++) {
      if (!SDDS_ZeroMemory(&associate[i], sizeof(ASSOCIATE_DEF))) {
	SDDS_SetError("Unable to zero memory for associates(SDDS_MPI_InitializeInput)");
	return 0;
      }
      associate[i].sdds = layout->associate_definition[i].sdds;
     
      if (layout->associate_definition[i].name) {
	associate[i].name_len = strlen(layout->associate_definition[i].name);
	sprintf(associate[i].name, "%s", layout->associate_definition[i].name);
      }
      if (layout->associate_definition[i].filename) {
	associate[i].filename_len = strlen(layout->associate_definition[i].filename);
	sprintf(associate[i].filename, "%s", layout->associate_definition[i].filename);
      }
      if (layout->associate_definition[i].path) {
	associate[i].path_len = strlen(layout->associate_definition[i].path);
	sprintf(associate[i].path, "%s", layout->associate_definition[i].path);
      }
      if (layout->associate_definition[i].description) {
	associate[i].description_len = strlen(layout->associate_definition[i].description);
	sprintf(associate[i].description, "%s", layout->associate_definition[i].description);
      }
      if (layout->associate_definition[i].contents) {
	associate[i].contents_len = strlen(layout->associate_definition[i].contents);
	sprintf(associate[i].contents, "%s", layout->associate_definition[i].contents);
      }
    }
  } else {
    SDDS_dataset->page_number = SDDS_dataset->page_started = 0;
    layout->popenUsed = other.popenUsed;
    layout->gzipFile = other.gzipFile;
    layout->disconnected = other.disconnected;
    layout->layout_written = other.layout_written;
    if (other.filename_len)
      SDDS_CopyString(&layout->filename, other.filename);
    layout->version = other.version;
    layout->byteOrderDeclared = other.byteOrderDeclared;
    layout->data_mode.mode = other.mode;
    layout->data_mode.lines_per_row = other.lines_per_row;
    layout->data_mode.no_row_counts = other.no_row_counts;
    layout->data_mode.fixed_row_count = other.fixed_row_count;
    layout->data_mode.fsync_data = other.fsync_data;
    layout->data_mode.additional_header_lines = other.additional_header_lines;
    if (other.description_len)
      SDDS_CopyString(&layout->description, other.description);
    if (other.contents_len)
      SDDS_CopyString(&layout->contents, other.contents);
  }
  if (other.n_columns)
    MPI_Bcast(column, other.n_columns, elementType, 0, MPI_dataset->comm);
  if (other.n_parameters)
    MPI_Bcast(parameter, other.n_parameters, elementType, 0, MPI_dataset->comm);
  if (other.n_arrays)
    MPI_Bcast(array, other.n_arrays, elementType, 0, MPI_dataset->comm);
  MPI_Type_free(&elementType);
  if (other.n_associates) {
    /* create and commit associate type */
    offsets[0] = 0;
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 6;
    
    offsets[1] = 6 * int_ext;
    oldtypes[1] = MPI_CHAR;
    blockcounts[1] = 256 + 256 + 1024 + 1024 + 1024;
    
    MPI_Type_struct(2, blockcounts, offsets, oldtypes, &associateType);
    MPI_Type_commit(&associateType);
    MPI_Bcast(associate, other.n_associates, associateType, 0, MPI_dataset->comm);
    MPI_Type_free(&associateType);
  }
  if (MPI_dataset->myid) {
    for (i=0; i<other.n_columns; i++) {
      symbol = units = description = format_string = NULL;
      if (column[i].units_len)
	SDDS_CopyString(&units, column[i].units);
      if (column[i].description_len)
	SDDS_CopyString(&description, column[i].description);
      if (column[i].format_string_len)
	SDDS_CopyString(&format_string, column[i].format_string);
      if (column[i].symbol_len)
	SDDS_CopyString(&symbol, column[i].symbol);
      if (SDDS_DefineColumn(SDDS_dataset, column[i].name, symbol, units, description, format_string, 
			    column[i].type,  column[i].field_length)<0) {
	SDDS_SetError("Unable to define column (SDDS_MPI_BroadcastLayout)");
	return(0);
      }
      if (units) free(units);
      if (description) free(description);
      if (symbol) free(symbol);
      if (format_string) free(format_string);
    }
    for (i=0; i<other.n_parameters; i++) {
      symbol = units = description = format_string = fixed_value = NULL;
      if (parameter[i].units_len)
	SDDS_CopyString(&units, parameter[i].units);
      if (parameter[i].description_len)
	SDDS_CopyString(&description, parameter[i].description);
      if (parameter[i].format_string_len)
	SDDS_CopyString(&format_string, parameter[i].format_string);
      if (parameter[i].symbol_len)
	SDDS_CopyString(&symbol, parameter[i].symbol);
      if (parameter[i].fixed_value_len)
	SDDS_CopyString(&fixed_value, parameter[i].fixed_value);
      if (SDDS_DefineParameter(SDDS_dataset, parameter[i].name, symbol, units, description,
			       format_string, parameter[i].type, fixed_value)<0) {
	SDDS_SetError("Unable to define parameter (SDDS_MPI_BroadcastLayout)");
	return(0);
      }
      if (units) free(units);
      if (description) free(description);
      if (symbol) free(symbol);
      if (format_string) free(format_string);
      if (fixed_value) free(fixed_value);
    }
    for (i=0; i<other.n_arrays; i++) {
      if (SDDS_DefineArray(SDDS_dataset, array[i].name, array[i].symbol,
			   array[i].units, array[i].description,
			   array[i].format_string, array[i].type,
			   array[i].field_length, array[i].dimensions,
			   array[i].group_name)<0) {
	SDDS_SetError("Unable to define array (SDDS_BroadcastLayout)");
	return(0);
      }
    }
    for (i=0; i<other.n_associates; i++) {
      filename = path = description = contents = NULL;
      if (associate[i].filename_len)
	SDDS_CopyString(&filename, associate[i].filename);
      if (associate[i].path_len)
	SDDS_CopyString(&path, associate[i].path);
      if (associate[i].description_len)
	SDDS_CopyString(&description, associate[i].description);
      if (associate[i].contents_len)
	SDDS_CopyString(&contents, associate[i].contents);
      if (SDDS_DefineAssociate(SDDS_dataset, associate[i].name, filename, path, description, contents, associate[i].sdds)<0) {
	SDDS_SetError("Unable to define associate (SDDS_MPI_BroadcastLayout)");
	return(0);
      }
      if (description) free(description);
      if (contents) free(contents);
      if (path) free(path);
      if (filename) free(filename);
    }
    if (!SDDS_SaveLayout(SDDS_dataset))  {
      SDDS_SetError("Unable to save layout (SDDS_BroadcastLayout)");
      return(0);
    }
  }
  if (column) free(column);
  if (array) free(array);
  if (parameter) free(parameter);
  if (associate) free(associate);
  column = array = parameter = NULL;
  associate = NULL;
  MPI_dataset->file_offset = other.layout_offset;
  return 1;
}


int32_t SDDS_MPI_InitializeInput(SDDS_DATASET *SDDS_dataset, char *filename)
{
  /*  startTime = MPI_Wtime();*/
  MPI_DATASET *MPI_dataset = SDDS_dataset->MPI_dataset;
  
#if defined(MASTER_READTITLE_ONLY)
  if (MPI_dataset->myid==0) 
#endif 
  {
    /*  char *ptr, *datafile, *headerfile;*/
    static char s[SDDS_MAXLINE];
#if defined(zLib)
    char *extension;
#endif
    if (sizeof(gzFile) != sizeof(void*)) {
      SDDS_SetError("gzFile is not the same size as void *, possible corruption of the SDDS_LAYOUT structure");
      return(0);
    }
    if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_InitializeInput"))
      return(0);
    SDDS_dataset->layout.gzipFile = SDDS_dataset->layout.disconnected = SDDS_dataset->layout.popenUsed = 0;
    if (!filename)
      SDDS_dataset->layout.filename = NULL;
    else if (!SDDS_CopyString(&SDDS_dataset->layout.filename, filename)) {
      sprintf(s, "Memory allocation failure initializing file \"%s\" (SDDS_InitializeInput)", filename);
      SDDS_SetError(s);
      return(0);
    }
    if (!filename) {
#if defined(_WIN32)
      if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
        sprintf(s, "unable to set stdin to binary mode");
        SDDS_SetError(s);
        return 0;
      }      
#endif
      SDDS_dataset->layout.fp = stdin;
    } else {
#if defined(zLib)
      if (!(extension = strrchr(filename, '.')) || strcmp(extension, ".gz")!=0) {
#endif
        if (!(SDDS_dataset->layout.fp
              =UnpackFopen(filename, UNPACK_REQUIRE_SDDS|UNPACK_USE_PIPE, \
                           &SDDS_dataset->layout.popenUsed, NULL))) {
          sprintf(s, "Unable to open file \"%s\" for reading (SDDS_InitializeInput)", filename);
          SDDS_SetError(s);
          return(0);
        }
#if defined(zLib)
      } else {
        SDDS_dataset->layout.gzipFile = 1;
        if (!(SDDS_dataset->layout.gzfp
              =UnpackGZipOpen(filename))) {
          sprintf(s, "Unable to open file \"%s\" for reading (SDDS_InitializeInput)", filename);
          SDDS_SetError(s);
          return(0);
        }
      }
#endif
    }
    SDDS_dataset->page_number =SDDS_dataset->page_started= 0;
    SDDS_dataset->file_had_data = 0;
    SDDS_DeferSavingLayout(1);
#if defined(zLib)
    if (SDDS_dataset->layout.gzipFile) {
      if (!SDDS_GZipReadLayout(SDDS_dataset, SDDS_dataset->layout.gzfp))
        return(0);
    } else {
#endif
      if (!SDDS_ReadLayout(SDDS_dataset, SDDS_dataset->layout.fp))
        return(0);
#if defined(zLib)
    }
#endif
    SDDS_dataset->layout.layout_written = 0;
    SDDS_DeferSavingLayout(0);
    if (!SDDS_SaveLayout(SDDS_dataset))
      return 0;
    if (SDDS_dataset->layout.n_columns &&
        ((!(SDDS_dataset->column_flag=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns)) ||
          !(SDDS_dataset->column_order=(int32_t*)SDDS_Malloc(sizeof(int32_t)*SDDS_dataset->layout.n_columns))) ||
         (!SDDS_SetMemory(SDDS_dataset->column_flag, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)1, (int32_t)0) ||
          !SDDS_SetMemory(SDDS_dataset->column_order, SDDS_dataset->layout.n_columns, SDDS_LONG, (int32_t)0, (int32_t)1)))) {
      SDDS_SetError("Unable to initialize input--memory allocation failure (SDDS_InitializeInput)");
      return(0);
    }
    SDDS_dataset->mode=SDDS_READMODE; /*reading*/
    SDDS_dataset->pagecount_offset=NULL;
    if (!SDDS_dataset->layout.gzipFile && !SDDS_dataset->layout.popenUsed &&
        SDDS_dataset->layout.filename) {
      /* Data is not:
         1. from a gzip file 
         2. from a file that is being internally decompressed by a command executed with popen()
         3. from a pipe set up externally (e.g., -pipe=in on commandline)
         */
      SDDS_dataset->pages_read=0;
      SDDS_dataset->pagecount_offset=malloc(sizeof(*SDDS_dataset->pagecount_offset));
      SDDS_dataset->pagecount_offset[0]=ftell(SDDS_dataset->layout.fp);
      fseek(SDDS_dataset->layout.fp,0,2); /*point to the end of the file*/
      SDDS_dataset->endOfFile_offset=ftell(SDDS_dataset->layout.fp);
      fseek(SDDS_dataset->layout.fp,SDDS_dataset->pagecount_offset[0],0);
      /*point to the beginning of the first page*/
    }
    if (SDDS_dataset->layout.fp)
      fclose(SDDS_dataset->layout.fp);
    /*    SDDS_dataset->MPI_dataset = MPI_dataset;*/
  }
  
# if defined(MASTER_READTITLE_ONLY)
  if (!SDDS_MPI_BroadcastLayout(SDDS_dataset)) 
    return 0;
#else
  MPI_dataset->file_offset = SDDS_dataset->pagecount_offset[0];
#endif
  
  if (!SDDS_MPI_File_Open(MPI_dataset, filename, SDDS_MPI_READ_ONLY)) {
    SDDS_SetError("(SDDS_MPI_File_Open)Unablle to open file for reading.");
    return 0;
  }
   
  MPI_File_get_size(MPI_dataset->MPI_file, &(MPI_dataset->file_size));
  if (MPI_dataset->file_offset >= MPI_dataset->file_size) {
    SDDS_SetError("No data contained in the input file.");
    return 0;
  }
  MPI_dataset->column_offset = SDDS_MPI_Get_Column_Size(SDDS_dataset);
  SDDS_dataset->parallel_io = 1;
  return 1;
}

int32_t SDDS_MPI_InitializeInputFromSearchPath(SDDS_DATASET *SDDS_dataset, char *file)
{
  char *filename;
  int32_t value;
  if (!(filename=findFileInSearchPath(file))) {
    SDDS_SetError("file does not exist in searchpath (InitializeInputFromSearchPath)");
    return 0;
  }
  value = SDDS_MPI_InitializeInput(SDDS_dataset, filename);
  free(filename);
  return value;
}
