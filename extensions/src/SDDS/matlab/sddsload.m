function [sdds] = sddsload(filename)
% ************************************************************************
% Copyright (c) 2002 The University of Chicago, as Operator of Argonne
% National Laboratory.
% Copyright (c) 2002 The Regents of the University of California, as
% Operator of Los Alamos National Laboratory.
% This file is distributed subject to a Software License Agreement found
% in the file LICENSE that is included with this distribution. 
% ************************************************************************
%   SDDSLOAD loads the SDDS data file into memory.
%   SDDSLOAD(FILENAME), where filename is the input SDDS data file

import SDDS.java.SDDS.*

if nargin < 1
    error('Not enough input arguments.')
end

sddsData = SDDSFile(filename);
i = sddsData.readFile;
if i == 0
    error(char(sddsData.getErrors))
end
sdds.filename = sddsData.fileName;
sdds.description.contents = sddsData.descriptionContents;
if length(sdds.description.contents) == 0
    sdds.description.contents = [];
end
sdds.description.text = sddsData.descriptionText;
if length(sdds.description.text) == 0
    sdds.description.text = [];
end
sdds.ascii = sddsData.asciiFile;
sdds.parameter_names = char(sddsData.getParameterNames);
sdds.array_names = char(sddsData.getArrayNames);
sdds.column_names = char(sddsData.getColumnNames);
sdds.pages = sddsData.pageCount;

if sdds.pages == 0
    return
end

k = size(sdds.parameter_names,1);
for j = 1:k
    name = convertSDDSname(sdds.parameter_names(j,1:end));
    type = sddsData.getParameterType(j-1);
    eval(['sdds.parameter.',name,'.type = SDDSUtil.getTypeName(type);'])

    units = sddsData.getParameterUnits(j-1);
    if length(units) == 0
        units = [];
    end
    eval(['sdds.parameter.',name,'.units = units;'])
    
    symbol = sddsData.getParameterSymbol(j-1);
    if length(symbol) == 0
        symbol = [];
    end
    eval(['sdds.parameter.',name,'.symbol = symbol;'])
  
    format_string = sddsData.getParameterFormatString(j-1);
    if length(format_string) == 0
        format_string = [];
    end
    eval(['sdds.parameter.',name,'.format_string = format_string;'])

    description = sddsData.getParameterDescription(j-1);
    if length(description) == 0
        description = [];
    end
    eval(['sdds.parameter.',name,'.description = description;'])

    if (type == SDDSUtil.SDDS_STRING) | (type == SDDSUtil.SDDS_CHARACTER)
        values = char(sddsData.getParameterValue(j-1,1,0));
        for i = 2:sdds.pages
            values = char(values,sddsData.getParameterValue(j-1,i,0));
        end
        eval(['sdds.parameter.',name,'.data = values;'])
    else
        for i = 1:sdds.pages
            eval(['sdds.parameter.',name,'.data(i) = sddsData.getParameterValue(j-1,i,0);'])
        end
    end
end

k = size(sdds.column_names,1);
for j = 1:k
    name = convertSDDSname(sdds.column_names(j,1:end));
    type = sddsData.getColumnType(j-1);
    eval(['sdds.column.',name,'.type = SDDSUtil.getTypeName(type);'])
    
    units = sddsData.getColumnUnits(j-1);
    if length(units) == 0
        units = [];
    end
    eval(['sdds.column.',name,'.units = units;'])

    symbol = sddsData.getColumnSymbol(j-1);
    if length(symbol) == 0
        symbol = [];
    end
    eval(['sdds.column.',name,'.symbol = symbol;'])

    format_string = sddsData.getColumnFormatString(j-1);
    if length(format_string) == 0
        format_string = [];
    end
    eval(['sdds.column.',name,'.format_string = format_string;'])

    description = sddsData.getColumnDescription(j-1);
    if length(description) == 0
        description = [];
    end
    eval(['sdds.column.',name,'.description = description;'])

    for i = 1:sdds.pages
        rows = sddsData.getRowCount(i);
        if rows == 0
            continue
        end
        values = sddsData.getColumnValues(j-1,i,0);
        if (type == SDDSUtil.SDDS_STRING) | (type == SDDSUtil.SDDS_CHARACTER)
            values = SDDSUtil.castArrayAsString(values, type);
        else
            values = SDDSUtil.castArrayAsDouble(values, type);
        end
        eval(['sdds.column.',name,'.page',int2str(i),' = values;'])
    end
end

k = size(sdds.array_names,1);
for j = 1:k
    name = convertSDDSname(sdds.array_names(j,1:end));
    dimensions = sddsData.getArrayDimensions(j-1);
    eval(['sdds.array.',name,'.dimensions = dimensions;'])
    type = sddsData.getArrayType(j-1);
    eval(['sdds.array.',name,'.type = SDDSUtil.getTypeName(type);'])
    
    units = sddsData.getArrayUnits(j-1);
    if length(units) == 0
        units = [];
    end
    eval(['sdds.array.',name,'.units = units;'])
    
    symbol = sddsData.getArraySymbol(j-1);
    if length(symbol) == 0
        symbol = [];
    end
    eval(['sdds.array.',name,'.symbol = symbol;'])

    format_string = sddsData.getArrayFormatString(j-1);
    if length(format_string) == 0
        format_string = [];
    end
    eval(['sdds.array.',name,'.format_string = format_string;'])

    group_name = sddsData.getArrayGroupName(j-1);
    if length(group_name) == 0
        group_name = [];
    end
    eval(['sdds.array.',name,'.group_name = group_name;'])

    description = sddsData.getArrayDescription(j-1);
    if length(description) == 0
        description = [];
    end
    eval(['sdds.array.',name,'.description = description;'])
    
    if dimensions == 0
        continue
    end
    for i = 1:sdds.pages
        values = sddsData.getArrayDim(i,j-1);
        clear datasize
        for n = 1:dimensions
            datasize(n) = double(values(n));
        end
        eval(['sdds.array.',name,'.size_page',int2str(i),' = datasize;'])
        arraySize = datasize(1);
        for n = 2:dimensions
            arraySize = datasize(n) * arraySize;
        end
        if arraySize == 0
            continue
        end
        values = sddsData.getArrayValues(j-1,i,0);
        eval(['sdds.array.',name,'.page',int2str(i),' = values;'])
    end
end
