/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDS;

import java.io.*;
import java.lang.reflect.*;

import java.util.*;


/**
 * Title: SDDSArray
 * Description: Provide support for storing array data.
 * Copyright:    Copyright (c) 2001
 * Company: Collider Accelerator Department, Brookhaven National Laboratory
 * @author  Seth Nemesure <a href=mailto:seth@bnl.gov>seth@bnl.gov</a>
 * @version 1.0 (October, 2001)
 */

public class SDDSArray
{
    String arrayName;
    String arraySymbol;
    String arrayUnits;
    String arrayDescription;
    String arrayFormatString;
    String arrayGroupName;

    int arrayType;
    int arrayFieldLength;

    long[][] longValue;
    double[][] doubleValue;
    String[][] stringValue;
    char[][] characterValue;

    int pageCount;
    int arrayDimensions;
    int[][] arrayDim;
    int[] rowCount;

    final int SDDS_SHORT = 4;
    final int SDDS_LONG = 3;
    final int SDDS_FLOAT = 2;
    final int SDDS_DOUBLE = 1;
    final int SDDS_STRING = 5;
    final int SDDS_CHARACTER = 6;

    /**
     * Create a new SDDS array that can be used to store array information for
     * SDDS files
     */
    public SDDSArray()
    {
	pageCount = 0;
	arrayType = 0;
	arrayDimensions = 1;
	arrayDim = new int[1][arrayDimensions];
	arrayDim[0] = null;
    }

    /**
     * Set up the array information
     *
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param group_name       may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param field_length     has no effect currently
     * @param dimensions       integer
     */

    public void setupArray(String name, String symbol, String units, String description,
			   String format_string, String group_name, int type, int field_length, int dimensions)
    {
	arrayName = name;
	arraySymbol = symbol;
	arrayUnits = units;
	arrayDescription = description;
	arrayFormatString = format_string;
	arrayGroupName = group_name;
	arrayType = type;
	arrayFieldLength = field_length;
	arrayDimensions = dimensions;
	arrayDim = new int[1][arrayDimensions];
	arrayDim[0] = null;
    }

    /**
     * Returns the name of the array
     *
     * @return  The <code>String</code> value of the array name.
     */
    public String getName()
    {
	return arrayName;
    }

    /**
     * Set the name of the array
     *
     * @param name            name of the array
     */
    public void setName(String name)
    {
	arrayName = name;
    }

    /**
     * Returns the symbol of the array
     *
     * @return   The <code>String</code> value of the array symbol.
     */
    public String getSymbol()
    {
	return arraySymbol;
    }

    /**
     * Set the symbol of the array
     *
     * @param symbol           symbol of the array
     */
    public void setSymbol(String symbol)
    {
	arraySymbol = symbol;
    }

    /**
     * Returns the units of the array
     *
     * @return   The <code>String</code> value of the array units.
     */
    public String getUnits()
    {
	return arrayUnits;
    }

    /**
     * Set the units of the array
     *
     * @param units           units of the array
     */
    public void setUnits(String units)
    {
	arrayUnits = units;
    }

    /**
     * Returns the description of the array
     *
     * @return   The <code>String</code> value of the array description.
     */
    public String getDescription()
    {
	return arrayDescription;
    }

    /**
     * Set the description of the array
     *
     * @param description     description of the array
     */
    public void setDescription(String description) {
	arrayDescription = description;
    }

    /**
     * Returns the format string of the array
     *
     * @return   The <code>String</code> value of the array format string.
     */
    public String getFormat_String()
    {
	return arrayFormatString;
    }

    /**
     * Set the format string of the array
     *
     * @param format_string     format string of the array
     */
    public void setFormat_String(String format_string)
    {
	arrayFormatString = format_string;
    }

    /**
     * Returns the group name of the array
     *
     * @return   The <code>String</code> value of the array group name.
     */
    public String getGroup_Name()
    {
	return arrayGroupName;
    }

    /**
     * Set the group name of the array
     *
     * @param group_name     format string of the array
     */
    public void setGroup_Name(String group_name)
    {
	arrayGroupName = group_name;
    }

    /**
     * Returns the type of the array
     *
     * @return   The <code>int</code> value of the array type.
     */
    public int getType()
    {
	return arrayType;
    }

    /**
     * Returns the field length of the array
     *
     * @return   The <code>int</code> value of the array field length.
     */
    public int getField_Length() {
	return arrayFieldLength;
    }

    /**
     * Set the field length of the array
     *
     * @param field_length      field length of the array
     */
    public void setField_Length(int field_length) {
	arrayFieldLength = field_length;
    }

    /**
     * Returns the number of dimensions in the array
     *
     * @return   The <code>int</code> value of the dimensions.
     */
    public int getDimensions()
    {
	return arrayDimensions;
    }

    /**
     * Returns the number of elements in each dimension of the array
     *
     * @param page             index of page
     * @return   The <code>int</code> array values.
     */
    public int[] getDim(int page)
    {
	int i;
	if (arrayDim == null)
	    return null;
	i = Array.getLength(arrayDim);
	if ((page > i) || (arrayDim[page-1] == null))
	    return null;
	return arrayDim[page-1];
    }

    /**
     * Set the number of elements in each dimension of the array
     *
     * @param page    index of page
     * @param dim     format string of the array
     */
    public void setDim(int page, int[] dim)
    {
	if ((page > 0) && (page <= pageCount)) {
	    arrayDim[page-1] = dim;
	} else if (page == pageCount+1) {
	    arrayDim = (int[][])SDDSUtil.resize(arrayDim, page);
	    arrayDim[page-1] = dim;
	}
    }

    /**
     * Returns the number of pages that the array contains
     *
     * @return   The <code>int</code> value of the number of pages the array contains.
     */
    public int getPageCount()
    {
	return pageCount;
    }

    /**
     * Returns the number of rows a page contains
     *
     * @return   The <code>int</code> value of the number of pages the array contains.<br>
     *           <code>-1</code> if the page value is invalid.
     */
    public int getRowCount(int page)
    {
	int i;
	if (rowCount == null)
	    return -1;
	i = Array.getLength(rowCount);
	if (page > i)
	    return -1;
	return rowCount[page-1];
    }


    /**
     * Inserts a blank page at the specified index
     *
     * @param page             index of page to create insert
     * @return                 <code>true</code> on success<br>
     *                         <code>false</code> on error.
     */
    public boolean insertPage(int page) {
	if (page == pageCount+1) {
	    makeBlankPages(page);
	    return true;
	} else if ((page < 1) || (page > pageCount+1)) {
	    return false;
	} else {
	    int i;
	    switch (arrayType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		longValue = (long[][])SDDSUtil.resize(longValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (longValue[i] == null)
			longValue[i] = new long[rowCount[i-1]];
		    else
			longValue[i] = (long[])SDDSUtil.resize(longValue[i], rowCount[i-1]);
		    longValue[i] = longValue[i-1];
		}
		longValue[page-1] = null;
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		doubleValue = (double[][])SDDSUtil.resize(doubleValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (doubleValue[i] == null)
			doubleValue[i] = new double[rowCount[i-1]];
		    else
			doubleValue[i] = (double[])SDDSUtil.resize(doubleValue[i], rowCount[i-1]);
		    doubleValue[i] = doubleValue[i-1];
		}
		doubleValue[page-1] = null;
		break;
	    case SDDS_STRING:
		stringValue = (String[][])SDDSUtil.resize(stringValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (stringValue[i] == null)
			stringValue[i] = new String[rowCount[i-1]];
		    else
			stringValue[i] = (String[])SDDSUtil.resize(stringValue[i], rowCount[i-1]);
		    stringValue[i] = stringValue[i-1];
		}
		stringValue[page-1] = null;
		break;
	    case SDDS_CHARACTER:
		characterValue = (char[][])SDDSUtil.resize(characterValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (characterValue[i] == null)
			characterValue[i] = new char[rowCount[i-1]];
		    else
			characterValue[i] = (char[])SDDSUtil.resize(characterValue[i], rowCount[i-1]);
		    characterValue[i] = characterValue[i-1];
		}
		characterValue[page-1] = null;
		break;
	    default:
		return false;
	    }
	    arrayDim = (int[][])SDDSUtil.resize(arrayDim, pageCount+1);
	    for (i=pageCount;i>=page;i--) {
		if (arrayDim[i] == null)
		    arrayDim[i] = new int[arrayDimensions];
		arrayDim[i] = arrayDim[i-1];
	    }
	    arrayDim[page-1] = null;

	    rowCount = (int[])SDDSUtil.resize(rowCount, pageCount+1);
	    for (i=pageCount;i>=page;i--)
		rowCount[i] = rowCount[i-1];
	    rowCount[page-1] = 0;

	    pageCount++;
	    return true;
	}
    }

    /**
     * Delete a page at the specified index
     *
     * @param page             index of page to delete
     * @return                 <code>true</code> on success<br>
     *                         <code>false</code> on error.
     */
    public boolean deletePage(int page) {
	if ((page < 1) || (page > pageCount)) {
	    return false;
	} else {
	    int i;
	    switch (arrayType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		for (i=page;i<pageCount;i++) {
		    if (longValue[i-1] == null)
			longValue[i-1] = new long[rowCount[i]];
		    else
			longValue[i-1] = (long[])SDDSUtil.resize(longValue[i-1], rowCount[i]);
		    longValue[i-1] = longValue[i];
		}
		longValue = (long[][])SDDSUtil.resize(longValue, pageCount-1);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		for (i=page;i<pageCount;i++) {
		    if (doubleValue[i-1] == null)
			doubleValue[i-1] = new double[rowCount[i]];
		    else
			doubleValue[i-1] = (double[])SDDSUtil.resize(doubleValue[i-1], rowCount[i]);
		    doubleValue[i-1] = doubleValue[i];
		}
		doubleValue = (double[][])SDDSUtil.resize(doubleValue, pageCount-1);
		break;
	    case SDDS_STRING:
		for (i=page;i<pageCount;i++) {
		    if (stringValue[i-1] == null)
			stringValue[i-1] = new String[rowCount[i]];
		    else
			stringValue[i-1] = (String[])SDDSUtil.resize(stringValue[i-1], rowCount[i]);
		    stringValue[i-1] = stringValue[i];
		}
		stringValue = (String[][])SDDSUtil.resize(stringValue, pageCount-1);
		break;
	    case SDDS_CHARACTER:
		for (i=page;i<pageCount;i++) {
		    if (characterValue[i-1] == null)
			characterValue[i-1] = new char[rowCount[i]];
		    else
			characterValue[i-1] = (char[])SDDSUtil.resize(characterValue[i-1], rowCount[i]);
		    characterValue[i-1] = characterValue[i];
		}
		characterValue = (char[][])SDDSUtil.resize(characterValue, pageCount-1);
		break;
	    default:
		return false;
	    }
	    for (i=page;i<pageCount;i++) {
		if (arrayDim[i-1] == null)
		    arrayDim[i-1] = new int[arrayDimensions];
		arrayDim[i-1] = arrayDim[i];
	    }
	    arrayDim = (int[][])SDDSUtil.resize(arrayDim, pageCount-1);

	    for (i=page;i<pageCount;i++)
		rowCount[i-1] = rowCount[i];
	    rowCount = (int[])SDDSUtil.resize(rowCount, pageCount-1);
	    pageCount--;
	    return true;
	}
    }

    /**
     * Create blank pages up to the last page specified
     *
     * @param lastpage         last page to create blank page
     */
    public void makeBlankPages(int lastpage) {
	int n;
	n = lastpage - pageCount;
	if (n > 0) {
	    switch (arrayType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < lastpage)
		    longValue = (long[][])SDDSUtil.resize(longValue, lastpage);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < lastpage)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, lastpage);
	    case SDDS_STRING:
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < lastpage)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, lastpage);
	    case SDDS_CHARACTER:
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < lastpage)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, lastpage);
	    default:
		return;
	    }
	    if (arrayDim == null)
		arrayDim = new int[n][];
	    else if (Array.getLength(arrayDim) < lastpage)
		arrayDim = (int[][])SDDSUtil.resize(arrayDim, lastpage);	    
	    arrayDim[lastpage-1] = new int[arrayDimensions];
	    arrayDim[lastpage-1] = null;

	    if (rowCount == null)
		rowCount = new int[lastpage];
	    else if (Array.getLength(rowCount) < lastpage)
		rowCount = (int[])SDDSUtil.resize(rowCount, lastpage);
	    pageCount = lastpage;
	}
    }

    /**
     * Write the header to a file.
     *
     * @param dos         a valid DataOutputStream
     * @return            <code>true</code> if the header was successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeHeader(DataOutputStream dos) {
	String line;
	if (arrayName == null) return false;
	line = "&array name=" + arrayName + ", ";
	if (arraySymbol != null) {
	    line = line.concat("symbol=" + SDDSUtil.prepareString(arraySymbol) + ", ");
	}
	if (arrayUnits != null) {
	    line = line.concat("units=" + SDDSUtil.prepareString(arrayUnits) + ", ");
	}
	if (arrayDescription != null) {
	    line = line.concat("description=" + SDDSUtil.prepareString(arrayDescription) + ", ");
	}
	if (arrayFormatString != null) {
	    line = line.concat("format_string=" + SDDSUtil.prepareString(arrayFormatString) + ", ");
	}
	if (arrayGroupName != null) {
	    line = line.concat("group_name=" + SDDSUtil.prepareString(arrayGroupName) + ", ");
	}
        line = line.concat("type=" + SDDSUtil.getTypeName(arrayType) + ", ");
        line = line.concat("dimensions=" + String.valueOf(arrayDimensions) + ", ");
	line = line.concat("&end\n");
	try {
	    dos.writeBytes(line);
	} catch (Exception e) {
	    return false;
	}
	return true;
    }

    /**
     * Set the values of the array at a chosen page
     *
     * @param v               a long precision number array
     * @param page            the page of the array, must be greater then <code>0</code>
     */
    public void setValues(long v[], int page)
    {
	int n, arrayLength, i;
	arrayLength = Array.getLength(v);
	if (page < 1) return;
	n = page - pageCount;
	page--;

	switch (arrayType){
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[arrayLength];
	    else if (Array.getLength(longValue[page]) < arrayLength)
		longValue[page] = (long[])SDDSUtil.resize(longValue[page], arrayLength);
	    System.arraycopy(v,0,longValue[page],0,arrayLength);
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[arrayLength];
	    else if (Array.getLength(doubleValue[page]) < arrayLength)
		doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], arrayLength);
	    System.arraycopy(v,0,doubleValue[page],0,arrayLength);
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[arrayLength];
	    else if (Array.getLength(stringValue[page]) < arrayLength)
		stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], arrayLength);
	    for (i=0; i<arrayLength; i++)
		stringValue[page][i] = Long.toString(v[i]);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[arrayLength];
	    else if (Array.getLength(characterValue[page]) < arrayLength)
		characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], arrayLength);
	    for (i=0; i<arrayLength; i++)
		characterValue[page][i] = Long.toString(v[i]).charAt(0);
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page+1;
	return;
    }

    /**
     * Set the values of the array at a chosen page
     *
     * @param v               a long precision number array
     * @param page            the page of the array, must be greater then <code>0</code>
     */
    public void setValues(double v[], int page)
    {
	int n, arrayLength, i;
	arrayLength = Array.getLength(v);
	if (page < 1) return;
	n = page - pageCount;
	page--;

	switch (arrayType){
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[arrayLength];
	    else if (Array.getLength(longValue[page]) < arrayLength)
		longValue[page] = (long[])SDDSUtil.resize(longValue[page], arrayLength);
	    for (i=0;i<arrayLength;i++)
		longValue[page][i] = (long)v[i];
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[arrayLength];
	    else if (Array.getLength(doubleValue[page]) < arrayLength)
		doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], arrayLength);
	    System.arraycopy(v,0,doubleValue[page],0,arrayLength);
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[arrayLength];
	    else if (Array.getLength(stringValue[page]) < arrayLength)
		stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], arrayLength);
	    for (i=0; i<arrayLength; i++)
		stringValue[page][i] = Double.toString(v[i]);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[arrayLength];
	    else if (Array.getLength(characterValue[page]) < arrayLength)
		characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], arrayLength);
	    for (i=0; i<arrayLength; i++)
		characterValue[page][i] = Double.toString(v[i]).charAt(0);
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page+1;
	return;
    }

     /**
     * Set the values of the array at a chosen page
     *
     * @param v               a string precision number array
     * @param page            the page of the array, must be greater then <code>0</code>
     */
    public void setValues(String v[], int page)
    {
	int n, arrayLength, i;
	arrayLength = Array.getLength(v);
	if (page < 1) return;
	n = page - pageCount;
	page--;

	switch (arrayType){
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[arrayLength];
	    else if (Array.getLength(longValue[page]) < arrayLength)
		longValue[page] = (long[])SDDSUtil.resize(longValue[page], arrayLength);
	    for (i=0;i<arrayLength;i++){
		try {
		    longValue[page][i] = (Long.valueOf(v[i])).longValue();
		} catch (Exception e){
		    longValue[page][i] = (long)0;
		}
	    }
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[arrayLength];
	    else if (Array.getLength(doubleValue[page]) < arrayLength)
		doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], arrayLength);
	    for (i=0;i<arrayLength;i++){
		try{
		    doubleValue[page][i] = (Double.valueOf(v[i])).doubleValue();
		}
		catch (Exception e){
		    doubleValue[page][i] = (double)0;
		}
	    }
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[arrayLength];
	    else if (Array.getLength(stringValue[page]) < arrayLength)
		stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], arrayLength);
	    System.arraycopy(v,0,stringValue[page],0,arrayLength);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[arrayLength];
	    else if (Array.getLength(characterValue[page]) < arrayLength)
		characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], arrayLength);
	    for (i=0; i<arrayLength; i++){
		if (v[i].length() > 0){
		    characterValue[page][i] = v[i].charAt(0);
		}
		else {
		    characterValue[page][i] = '0';
		}
	    }
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page+1;
	return;
    }

    /**
     * Write the values of the array for a chosen page
     *
     * @param dos             a valid DataOutputStream
     * @param page            the page of the array, must be greater then <code>0</code>
     * @param ascii           set <code>true</code> to write ascii column data,
     *                        <code>false</code> to write big endian binary array data
     * @return                <code>true</code> if the values were successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the array type is undefined.
     */
    public boolean writeValues(DataOutputStream dos, int page, boolean ascii)
    {
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	int i, length;
	page--;
	try{
	    if (ascii){
		switch (arrayType){
		case SDDS_SHORT:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(longValue[page]);
		    for (i=0;i<length;i++)
			dos.writeBytes((short)longValue[page][i] + " ");
		    dos.writeBytes("\n");
		    break;
		case SDDS_LONG:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(longValue[page]);
		    for (i=0;i<length;i++)
			dos.writeBytes(longValue[page][i] + " ");
		    dos.writeBytes("\n");
		    break;
		case SDDS_FLOAT:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(doubleValue[page]);
		    for (i=0;i<length;i++)
			dos.writeBytes((float)doubleValue[page][i] + " ");
		    dos.writeBytes("\n");
		    break;
		case SDDS_DOUBLE:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(doubleValue[page]);
		    for (i=0;i<length;i++)
			dos.writeBytes(doubleValue[page][i] + " ");
		    dos.writeBytes("\n");
		    break;
		case SDDS_STRING:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(stringValue[page]);
		    for (i=0;i<length;i++){
			if (stringValue[page][i] == null)
			    stringValue[page][i] = "";
			dos.writeBytes(SDDSUtil.prepareString(stringValue[page][i]) + " ");
		    }
		    dos.writeBytes("\n");
		    break;
		case SDDS_CHARACTER:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeBytes(arrayDim[page][i] + " ");
		    dos.writeBytes("\n");
		    length = Array.getLength(characterValue[page]);
		    for (i=0;i<length;i++){
			if (Character.isLetterOrDigit(characterValue[page][i])){
			    dos.writeBytes(characterValue[page][i] + " ");
			}
			else {
			    String temp;
			    temp = Integer.toOctalString(characterValue[page][i]);
			    while (temp.length() < 3)
				temp = "0" + temp;
			    dos.writeBytes("\\" + temp + " ");
			}
		    }
		    dos.writeBytes("\n");
		    break;
		default:
		    return false;
		}
	    }
	    else {
		switch (arrayType){
		case SDDS_SHORT:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(longValue[page]);
		    for (i=0;i<length;i++){
			dos.writeShort((short)longValue[page][i]);
		    }
		    break;
		case SDDS_LONG:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(longValue[page]);
		    for (i=0;i<length;i++){
			dos.writeInt((int)longValue[page][i]);
		    }
		    break;
		case SDDS_FLOAT:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(doubleValue[page]);
		    for (i=0;i<length;i++){
			dos.writeFloat((float)doubleValue[page][i]);
		    }
		    break;
		case SDDS_DOUBLE:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(doubleValue[page]);
		    for (i=0;i<length;i++){
			dos.writeDouble(doubleValue[page][i]);
		    }
		    break;
		case SDDS_STRING:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(stringValue[page]);
		    for (i=0;i<length;i++){
			if (stringValue[page][i] == null){
			    dos.writeInt(0);
			}
			else{
			    dos.writeInt(stringValue[page][i].length());
			    dos.writeBytes(stringValue[page][i]);
			}
		    }
		    break;
		case SDDS_CHARACTER:
		    for (i=0; i<arrayDimensions; i++)
			dos.writeInt(arrayDim[page][i]);
		    length = Array.getLength(characterValue[page]);
		    for (i=0;i<length;i++){
			dos.writeByte((int)characterValue[page][i]);
		    }
		    break;
		default:
		    return false;
		}
	    }
	}
	catch (Exception e){
	    return false;
	}
	return true;
    }

    /**
     * Write the values of the array for a chosen page
     *
     * @param ledos             a valid DataOutputStream
     * @param page            the page of the array, must be greater then <code>0</code>
     * @return                <code>true</code> if the values were successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the array type is undefined.
     */
    public boolean writeValues(LEDataOutputStream ledos, int page)
    {
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	int i, length;
	page--;
	try{
	    switch (arrayType){
	    case SDDS_SHORT:
		length = Array.getLength(longValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++)
		    ledos.writeShort((short)longValue[page][i]);
		break;
	    case SDDS_LONG:
		length = Array.getLength(longValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++)
		    ledos.writeInt((int)longValue[page][i]);
		break;
	    case SDDS_FLOAT:
		length = Array.getLength(doubleValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++)
		    ledos.writeFloat((float)doubleValue[page][i]);
		break;
	    case SDDS_DOUBLE:
		length = Array.getLength(doubleValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++)
		    ledos.writeDouble(doubleValue[page][i]);
		break;
	    case SDDS_STRING:
		length = Array.getLength(stringValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++){
		    if (stringValue[page][i] == null)
			ledos.writeInt(0);
		    else{
			ledos.writeInt(stringValue[page][i].length());
			ledos.writeBytes(stringValue[page][i]);
		    }
		}
		break;
	    case SDDS_CHARACTER:
		length = Array.getLength(longValue[page]);
		ledos.writeInt(length);
		for (i=0;i<length;i++){
		    ledos.writeByte((int)characterValue[page][i]);
		}
		break;
	    default:
		return false;
	    }
	}
	catch (Exception e){
	    return false;
	}
	return true;
    }

    /**
     * Write the little endian binary value of the array for a chosen page
     *
     * @param ledos           a valid LEDataOutputStream
     * @param page            the page of the array, must be greater then <code>0</code>
     * @return                <code>true</code> if the value was successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the array type is undefined.
     */
    public boolean writeElement(LEDataOutputStream ledos, int page) {
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	page--;
	int i, length;
	try {
	    switch (arrayType){
	    case SDDS_SHORT:
		length = Array.getLength(longValue[page]);
		for (i=0;i<length;i++){
		    ledos.writeShort((short)longValue[page][i]);
		}
		break;
	    case SDDS_LONG:
		length = Array.getLength(longValue[page]);
		for (i=0;i<length;i++){
		    ledos.writeInt((int)longValue[page][i]);
		}
		break;
	    case SDDS_FLOAT:
		length = Array.getLength(doubleValue[page]);
		for (i=0;i<length;i++){
		    ledos.writeFloat((float)doubleValue[page][i]);
		}
		break;
	    case SDDS_DOUBLE:
		length = Array.getLength(doubleValue[page]);
		for (i=0;i<length;i++){
		    ledos.writeDouble(doubleValue[page][i]);
		}
		break;
	    case SDDS_STRING:
		length = Array.getLength(stringValue[page]);
		for (i=0;i<length;i++){
		    if (stringValue[page][i] == null){
			ledos.writeInt(0);
		    }
		    else {
			ledos.writeInt(stringValue[page][i].length());
			ledos.writeBytes(stringValue[page][i]);
		    }
		}
		break;
	    case SDDS_CHARACTER:
		length = Array.getLength(characterValue[page]);
		for (i=0;i<length;i++){
		    ledos.writeByte((int)characterValue[page][i]);
		}
		break;
	    default:
		return false;
	    }
	}
	catch (IOException e){
	    return false;
	}
	return true;
    }

    /**
     * Get the values of the array for a chosen page
     *
     * @param page            the page of the array, must be greater then <code>0</code>
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @return                Object array of data
     */
    public Object[] getValues(int page, boolean string_quotes) {
	Object values[];
	int i, j;
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return null;
	page--;
	switch (arrayType){
	case SDDS_SHORT:
	    if (longValue[page] == null)
		return null;
	    j = Array.getLength(longValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++)
		values[i] = new Short((short)longValue[page][i]);
	    break;
	case SDDS_LONG:
	    if (longValue[page] == null)
		return null;
	    j = Array.getLength(longValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++)
		values[i] = new Long(longValue[page][i]);
	    break;
	case SDDS_FLOAT:
	    if (doubleValue[page] == null)
		return null;
	    j = Array.getLength(doubleValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++)
		values[i] = new Float((float)doubleValue[page][i]);
	    break;
	case SDDS_DOUBLE:
	    if (doubleValue[page] == null)
		return null;
	    j = Array.getLength(doubleValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++)
		values[i] = new Double(doubleValue[page][i]);
	    break;
	case SDDS_STRING:
	    if (stringValue[page] == null)
		return null;
	    j = Array.getLength(stringValue[page]);
	    values = new Object[j];
	    if (string_quotes) {
		for (i=0;i<j;i++) {
		    if (stringValue[page][i] == null)
			stringValue[page][i] = "";
		    values[i] = SDDSUtil.prepareString(stringValue[page][i]);
		}
	    } else {
		for (i=0;i<j;i++) {
		    if (stringValue[page][i] == null)
			stringValue[page][i] = "";
		    values[i] = stringValue[page][i];
		}
	    }
	    break;
	case SDDS_CHARACTER:
	    if (characterValue[page] == null)
		return null;
	    String temp;
	    j = Array.getLength(characterValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) {
		if (Character.isLetterOrDigit(characterValue[page][i])) {
		    values[i] = (new Character(characterValue[page][i])).toString();
		} else {
		    temp = Integer.toOctalString(characterValue[page][i]);
		    while (temp.length() < 3)
			temp = "0" + temp;
		    values[i] = "\\" + temp;
		}
	    }
	    break;
	default:
	    return null;
	}
	return values;
    }

}
