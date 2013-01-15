/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: SDDSnl.nl
 * purpose: namelist preprocessor input file for SDDS library
 *
 * Michael Borland, November 1993
 $Log: not supported by cvs2svn $
 Revision 1.4  1999/05/25 18:59:36  soliday
 Removed compiler warning on linux.

 Revision 1.3  1998/07/07 22:29:44  borland
 Completely new method of scanning the namelists in the headers.
 Twice as fast and eliminates need for namelist library in the
 SDDS toolkit.

 * Revision 1.2  1995/09/05  21:11:22  saunders
 * First test release of the SDDS1.5 package.
 *
 */
#include "namelist.h"

typedef struct {
    STRING text;
    STRING contents;
    } DESCRIPTION_STRUCT;
static DESCRIPTION_STRUCT description_struct = {
    NULL,
    NULL,
    } ;
static DESCRIPTION_STRUCT description_struct_default = {
    NULL,
    NULL,
    } ;

static  ITEM description_item_list[2] = {
    { "text", TYPE_STRING, 0, NULL, (char*)&description_struct.text, (char*)&description_struct_default.text, sizeof(description_struct.text) },
    { "contents", TYPE_STRING, 0, NULL, (char*)&description_struct.contents, (char*)&description_struct_default.contents, sizeof(description_struct.contents) } 
    } ;

static  NAMELIST description = {
    description_item_list, 
    2,
    "description"
    };

typedef struct {
    STRING name;
    STRING symbol;
    STRING units;
    STRING description;
    STRING format_string;
    STRING type;
    long field_length;
    } COLUMN_STRUCT;
static COLUMN_STRUCT column_struct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    } ;
static COLUMN_STRUCT column_struct_default = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    } ;

static  ITEM column_item_list[7] = {
    { "name", TYPE_STRING, 0, NULL, (char*)&column_struct.name, (char*)&column_struct_default.name, sizeof(column_struct.name) },
    { "symbol", TYPE_STRING, 0, NULL, (char*)&column_struct.symbol, (char*)&column_struct_default.symbol, sizeof(column_struct.symbol) },
    { "units", TYPE_STRING, 0, NULL, (char*)&column_struct.units, (char*)&column_struct_default.units, sizeof(column_struct.units) },
    { "description", TYPE_STRING, 0, NULL, (char*)&column_struct.description, (char*)&column_struct_default.description, sizeof(column_struct.description) },
    { "format_string", TYPE_STRING, 0, NULL, (char*)&column_struct.format_string, (char*)&column_struct_default.format_string, sizeof(column_struct.format_string) },
    { "type", TYPE_STRING, 0, NULL, (char*)&column_struct.type, (char*)&column_struct_default.type, sizeof(column_struct.type) },
    { "field_length", TYPE_LONG, 0, NULL, (char*)&column_struct.field_length, (char*)&column_struct_default.field_length, sizeof(column_struct.field_length) } 
    } ;

static  NAMELIST column = {
    column_item_list, 
    7,
    "column"
    };

typedef struct {
    STRING name;
    STRING symbol;
    STRING units;
    STRING description;
    STRING format_string;
    STRING type;
    STRING fixed_value;
    } PARAMETER_STRUCT;
static PARAMETER_STRUCT parameter_struct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    } ;
static PARAMETER_STRUCT parameter_struct_default = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    } ;

static  ITEM parameter_item_list[7] = {
    { "name", TYPE_STRING, 0, NULL, (char*)&parameter_struct.name, (char*)&parameter_struct_default.name, sizeof(parameter_struct.name) },
    { "symbol", TYPE_STRING, 0, NULL, (char*)&parameter_struct.symbol, (char*)&parameter_struct_default.symbol, sizeof(parameter_struct.symbol) },
    { "units", TYPE_STRING, 0, NULL, (char*)&parameter_struct.units, (char*)&parameter_struct_default.units, sizeof(parameter_struct.units) },
    { "description", TYPE_STRING, 0, NULL, (char*)&parameter_struct.description, (char*)&parameter_struct_default.description, sizeof(parameter_struct.description) },
    { "format_string", TYPE_STRING, 0, NULL, (char*)&parameter_struct.format_string, (char*)&parameter_struct_default.format_string, sizeof(parameter_struct.format_string) },
    { "type", TYPE_STRING, 0, NULL, (char*)&parameter_struct.type, (char*)&parameter_struct_default.type, sizeof(parameter_struct.type) },
    { "fixed_value", TYPE_STRING, 0, NULL, (char*)&parameter_struct.fixed_value, (char*)&parameter_struct_default.fixed_value, sizeof(parameter_struct.fixed_value) } 
    } ;

static  NAMELIST parameter = {
    parameter_item_list, 
    7,
    "parameter"
    };

typedef struct {
    STRING name;
    STRING filename;
    STRING path;
    STRING description;
    STRING contents;
    long sdds;
    } ASSOCIATE_STRUCT;
static ASSOCIATE_STRUCT associate_struct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    } ;
static ASSOCIATE_STRUCT associate_struct_default = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    } ;

static  ITEM associate_item_list[6] = {
    { "name", TYPE_STRING, 0, NULL, (char*)&associate_struct.name, (char*)&associate_struct_default.name, sizeof(associate_struct.name) },
    { "filename", TYPE_STRING, 0, NULL, (char*)&associate_struct.filename, (char*)&associate_struct_default.filename, sizeof(associate_struct.filename) },
    { "path", TYPE_STRING, 0, NULL, (char*)&associate_struct.path, (char*)&associate_struct_default.path, sizeof(associate_struct.path) },
    { "description", TYPE_STRING, 0, NULL, (char*)&associate_struct.description, (char*)&associate_struct_default.description, sizeof(associate_struct.description) },
    { "contents", TYPE_STRING, 0, NULL, (char*)&associate_struct.contents, (char*)&associate_struct_default.contents, sizeof(associate_struct.contents) },
    { "sdds", TYPE_LONG, 0, NULL, (char*)&associate_struct.sdds, (char*)&associate_struct_default.sdds, sizeof(associate_struct.sdds) } 
    } ;

static  NAMELIST associate = {
    associate_item_list, 
    6,
    "associate"
    };


typedef struct {
    STRING mode;
    long lines_per_row;
    long no_row_counts;
    long additional_header_lines;
    long do_line_padding;
    } DATA_STRUCT;
static DATA_STRUCT data_struct = {
    NULL,
    1,
    0,
    0,
    1,
    } ;
static DATA_STRUCT data_struct_default = {
    NULL,
    1,
    0,
    0,
    1,
    } ;

static  ITEM data_item_list[5] = {
    { "mode", TYPE_STRING, 0, NULL, (char*)&data_struct.mode, (char*)&data_struct_default.mode, sizeof(data_struct.mode) },
    { "lines_per_row", TYPE_LONG, 0, NULL, (char*)&data_struct.lines_per_row, (char*)&data_struct_default.lines_per_row, sizeof(data_struct.lines_per_row) },
    { "no_row_counts", TYPE_LONG, 0, NULL, (char*)&data_struct.no_row_counts, (char*)&data_struct_default.no_row_counts, sizeof(data_struct.no_row_counts) },
    { "additional_header_lines", TYPE_LONG, 0, NULL, (char*)&data_struct.additional_header_lines, (char*)&data_struct_default.additional_header_lines, sizeof(data_struct.additional_header_lines) },
    { "do_line_padding", TYPE_LONG, 0, NULL, (char*)&data_struct.do_line_padding, (char*)&data_struct_default.do_line_padding, sizeof(data_struct.do_line_padding) } 
    } ;

static  NAMELIST data = {
    data_item_list, 
    5,
    "data"
    };


typedef struct {
    STRING filename;
    } INCLUDE_STRUCT;
static INCLUDE_STRUCT include_struct = {
    NULL,
    } ;
static INCLUDE_STRUCT include_struct_default = {
    NULL,
    } ;

static  ITEM include_item_list[1] = {
    { "filename", TYPE_STRING, 0, NULL, (char*)&include_struct.filename, (char*)&include_struct_default.filename, sizeof(include_struct.filename) } 
    } ;

static  NAMELIST include = {
    include_item_list, 
    1,
    "include"
    };


typedef struct {
    STRING name;
    STRING symbol;
    STRING units;
    STRING description;
    STRING format_string;
    STRING type;
    long field_length;
    long dimensions;
    STRING group_name;
    } ARRAY_STRUCT;
static ARRAY_STRUCT array_struct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    1,
    NULL,
    } ;
static ARRAY_STRUCT array_struct_default = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    1,
    NULL,
    } ;

static  ITEM array_item_list[9] = {
    { "name", TYPE_STRING, 0, NULL, (char*)&array_struct.name, (char*)&array_struct_default.name, sizeof(array_struct.name) },
    { "symbol", TYPE_STRING, 0, NULL, (char*)&array_struct.symbol, (char*)&array_struct_default.symbol, sizeof(array_struct.symbol) },
    { "units", TYPE_STRING, 0, NULL, (char*)&array_struct.units, (char*)&array_struct_default.units, sizeof(array_struct.units) },
    { "description", TYPE_STRING, 0, NULL, (char*)&array_struct.description, (char*)&array_struct_default.description, sizeof(array_struct.description) },
    { "format_string", TYPE_STRING, 0, NULL, (char*)&array_struct.format_string, (char*)&array_struct_default.format_string, sizeof(array_struct.format_string) },
    { "type", TYPE_STRING, 0, NULL, (char*)&array_struct.type, (char*)&array_struct_default.type, sizeof(array_struct.type) },
    { "field_length", TYPE_LONG, 0, NULL, (char*)&array_struct.field_length, (char*)&array_struct_default.field_length, sizeof(array_struct.field_length) },
    { "dimensions", TYPE_LONG, 0, NULL, (char*)&array_struct.dimensions, (char*)&array_struct_default.dimensions, sizeof(array_struct.dimensions) },
    { "group_name", TYPE_STRING, 0, NULL, (char*)&array_struct.group_name, (char*)&array_struct_default.group_name, sizeof(array_struct.group_name) } 
    } ;

static  NAMELIST array = {
    array_item_list, 
    9,
    "array"
    };

static  long n_namelists = 7;

static  NAMELIST *namelist_pointer[7] = {
    &description,
    &column,
    &parameter,
    &associate,
    &data,
    &include,
    &array
    } ;

static  char *namelist_name[7] = {
    "description",
    "column",
    "parameter",
    "associate",
    "data",
    "include",
    "array"
    } ;

static  NAMELIST_TEXT namelist_text;
#define DESCRIPTION_NAMELIST 0
#define COLUMN_NAMELIST 1
#define PARAMETER_NAMELIST 2
#define ASSOCIATE_NAMELIST 3
#define DATA_NAMELIST 4
#define INCLUDE_NAMELIST 5
#define ARRAY_NAMELIST 6
